
/*****************************************************************
 * rxMoc  v0.5 ( Motion Object Controller )
 *				1999/6/21  onitama ( for use with PS2-rxLib. )
 *				2000/11/27 onitama ( for use with win32 )
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include "geometry.h"
#include "moc.h"

#define setFVector( a,b,c,d,e ) a[0]=b;a[1]=c;a[2]=d;a[3]=e;

/*---------------------------------------------------------------------*
 *	Structures
 *---------------------------------------------------------------------*/

typedef void (* ADV_FUNC) ( void * );

typedef struct MOTINFO {
	short	flag;				//	flags
								//	  bit0-7 : current attr
								//	  bit8   : in action
								//	  bit9   : result need calc (0=NO)
	short	fields;				//	fields in use ( 1 to 4 )
	short	attr;				//	attribute flag
								//	  bit0-7 : motion type
								//	           1  = no motion
								//	           2  = linear
								//	           16 = spline
								//	  bit10  : Auto Key Frame Enable
								//
	short	parent;				//	parent motion ID

	float	*kfreal;			//	Real Data Array top
	KFRAME2	*kfdata;			//	Auto Key Frame Data ptr.

	ADV_FUNC adv_func;			//	Autokey CALLBACK function

	short	kframe[4];			//	Frame Info (0=none)
								//	  bit10  : Auto Key Frame Enable
								//	  bit13  : Parent Motion Ref
								//	  bit14  : Reverse Loop
								//	  bit15  : Eternal Loop

	RXM_FV	result;				//	current key result
	RXM_FV	key0;				//	key[0] value
	RXM_FV	key1;				//	key[1] value
	RXM_FV	key2;				//	key[2] value

	RXM_FV	frame;				//	frame(0.0～1.0 per key)
	RXM_FV	speed;				//	frame advance(0.0～1.0)
	RXM_FV	speed2;				//	frame advance (next key)
} MOTINFO;


static void MotNewAutoKey( MOTINFO *mot );
static void MotNewAutoKey2( MOTINFO *mot );


/*---------------------------------------------------------------------*
 *	Essential Values
 *---------------------------------------------------------------------*/

static	int MAXMOT = 4096;
static	MOTINFO	*mem_mot;
#define MOTPTR(n) (&mem_mot[n])

static float *motdef_kfreal;
static KFRAME2 *motdef_kfdata;

/*---------------------------------------------------------------------*
 *	Essential Routines
 *---------------------------------------------------------------------*/

static void MotReset( MOTINFO *mot )
{
	int a;
	float clr, zclr;

	clr=1.0f;
	zclr=0.0f;

	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
	mot->fields=4;
	mot->attr=2;
	//mot->total=0;
	mot->parent=0;
	mot->kfdata=NULL;
	mot->kfreal=NULL;
	mot->adv_func = (ADV_FUNC)MotNewAutoKey;

	for(a=0;a<4;a++) {
		mot->kframe[a]=0;
		mot->result[a]=clr;
		mot->key0[a]=clr;
		mot->key1[a]=clr;
		mot->key2[a]=clr;
		mot->frame[a]=zclr;
		mot->speed[a]=zclr;
		mot->speed2[a]=zclr;
	}
}


static void MotNewKey( MOTINFO *mot, int subid, float speed, float key )
{
	//		新しいキー設定
	//
	int flag;
	if ( mot->speed[subid]==0.0f ) {
		mot->key1[subid]=key;					// 新しいkey1
		mot->speed[subid]=speed;
		mot->key2[subid]=key;					// 新しいkey2
		mot->speed2[subid]=1.0f;
	}
	else {
		mot->key2[subid]=key;					// 新しいkey2
		mot->speed2[subid]=speed;
	}

	mot->kframe[subid] &= ~MOT_KEYOPT_AUTOKEY;

	flag = MOT_FLAG_ACTION | mot->attr;
	mot->flag = flag;							// action flag ON
}


static void MotNewKeyR( MOTINFO *mot, int subid, float speed, float key )
{
	//		新しいキー設定(PI2角度用)
	//
	int flag;
	float oldkey;
	float diff;
	float diff2;

	//		逆回転の方が近ければ補正
	//
	oldkey=mot->key0[subid];
	//if ((oldkey>1.0f)||(oldkey<0.0f)) {
	//	printf( "Invalid %f->%f\n",oldkey,key );
	//}

	//printf( "%f->%f\n",oldkey,key );

	if ( oldkey>key ) {
		diff=oldkey-key;
		diff2=key+PI2-oldkey;
		if ( diff > diff2 ) {
			mot->key0[subid]-=PI2;
			//printf( "%f(%f)->%f\n",oldkey,mot->key0[subid],key );
		}
	}
	else {
		diff=key-oldkey;
		diff2=oldkey+PI2-key;
		if ( diff > diff2 ) {
			mot->key0[subid]+=PI2;
			//printf( "%f(%f)->%f\n",oldkey,mot->key0[subid],key );
		}
	}

	mot->key1[subid]=key;					// 新しいkey1
	mot->speed[subid]=speed;
	mot->key2[subid]=key;					// 新しいkey2
	mot->speed2[subid]=1.0f;

	mot->kframe[subid] &= ~MOT_KEYOPT_AUTOKEY;
	flag = MOT_FLAG_ACTION | mot->attr;
	mot->flag = flag;							// action flag ON
}


static void MotNewAutoKey( MOTINFO *mot )
{
	//		次のキーフレームデータを自動設定する
	//
	int a,i,flag;
	char *p;
	float speed;
	float *kfreal;
	KFRAME2 *kfdata;

	kfreal = mot->kfreal;

	//		Key設定
	//
reget:
	kfdata = mot->kfdata;
	flag = kfdata->flag;
	if (flag) {
		if (flag==1) {
			a = ((KFRAME *)kfdata)->length;
			p = ((char *)kfdata) + 4;
			mot->kfdata = (KFRAME2 *)(p+a);
			goto reget;
		}
		mot->flag &= !MOT_FLAG_AUTOKEY;
		return;
	}
	else {
		mot->kfdata++;
	}
	i = kfdata->index;
	speed = kfdata->advance;

	for(a=0;a<4;a++) {
		if ( mot->kframe[a] & MOT_KEYOPT_AUTOKEY ) {
			mot->key2[a] = kfreal[i];
			mot->speed2[a] = speed;
		}
		i++;
	}

}


static void MotNewAutoKey2( MOTINFO *mot )
{
	//		次のキーフレームデータを自動設定する
	//		(拡張キーデータ形式)
	//
	int a;
	unsigned int i;
	char *p;
	float speed;
	float *kfreal;
	KFRAME3 *kfdata;

	kfreal = mot->kfreal;

	//		Key設定
	//
reget:
	kfdata = (KFRAME3 *)mot->kfdata;
	i = kfdata->index;
	if (i>=0x10000000) {
		if (i&0x10000000) {
			a = ((KFRAME *)kfdata)->length;
			p = ((char *)kfdata) + 4;
			mot->kfdata = (KFRAME2 *)(p+a);
			goto reget;
		}
		mot->flag &= !MOT_FLAG_AUTOKEY;
		return;
	}

	mot->kfdata++;
	speed = kfdata->advance;

	for(a=0;a<4;a++) {
		if ( mot->kframe[a] & MOT_KEYOPT_AUTOKEY ) {
			mot->key2[a] = kfreal[i];
			mot->speed2[a] = speed;
		}
		i++;
	}

}


static void MotUpdate( MOTINFO *mot )
{
	//		Frame更新
	//
	int a, alloff, kfinc;
	int kfid;
	float speed;

	if ((mot->flag & MOT_FLAG_ACTION )==0) return;

	//		Frame Advance
	//
	alloff=kfinc=mot->fields;
	AddVector( VECREF mot->frame , VECREF mot->frame , VECREF mot->speed );

	for(a=0;a<(mot->fields);a++) {
		if ( mot->frame[a] >= 1.0f ) {
			kfid=mot->kframe[a];
			mot->frame[a] -= 1.0f;
			if ( kfid & MOT_KEYOPT_ETERNAL ) {
				if ( kfid & MOT_KEYOPT_REVERSE ) {
					mot->key2[a] = mot->key0[a];
					mot->key0[a] = mot->key1[a];
					mot->key1[a] = mot->key2[a];
					alloff++;
				}
			}
			else {
				mot->key0[a] = mot->key1[a];
				mot->key1[a] = mot->key2[a];
				speed = mot->speed2[a];
				mot->speed[a] = speed;
				if ( speed==0.0f ) mot->frame[a] = 0.0f;
				mot->speed2[a] = 0.0f;
			}
		}
		if ( mot->speed[a]==0.0f ) alloff--;
		if ( mot->speed2[a]==0.0f ) kfinc--;
	}

	if ( kfinc==0 ) {
		if ( mot->flag & MOT_FLAG_AUTOKEY ) {
			mot->adv_func( mot );
			//MotNewAutoKey( mot );		// 次のキーフレームデータに
		}
	}
	if ( alloff==0 ) {
			mot->flag = 1;				// すべてのKey変化の必要なし
	}
	mot->flag |= MOT_FLAG_NEEDCALC;	// 再計算FLAG ON
	//mot->total++;
}


static void MotCalc( MOTINFO *mot )
{
	//		現frameのKeyを取得する
	//
	RXM_FV	key0;
	RXM_FV	key1;
	RXM_FV	key2;
	RXM_FV	key3;
	RXM_FV	t1;
	RXM_FV	t2;
	RXM_FV	t3;
	RXM_FV	fa;
	RXM_FV	fb;
//	RXM_FV	fc;
//	RXM_FV	fd;
	RXM_FV	tmp;

	CopyVector( VECREF t1 , VECREF mot->frame );

	switch( mot->flag & 0x7f ) {
	case 1:
		//		補間なし
		//
		CopyVector( VECREF mot->result , VECREF mot->key0 );
		break;
	case 2:
		//		linear補間
		//
		SubVector( VECREF key1 , VECREF mot->key1, VECREF mot->key0 );
		MulVector( VECREF key0 , VECREF key1 , VECREF t1 );
		AddVector( VECREF mot->result , VECREF key0 , VECREF mot->key0 );
		break;
	case 16:
		//		spline補間
		//
		MulVector( VECREF t2, VECREF t1, VECREF t1 );
		MulVector( VECREF t3, VECREF t2, VECREF t1 );

		//	r0 = kframe[i];
		//	r1 = kframe[i+1];
		//	r2 = kframe[i+1]-kframe[i];
		//	r3 = kframe[i+2]-kframe[i+1];
		CopyVector( VECREF key0 , VECREF mot->key0 );
		CopyVector( VECREF key1 , VECREF mot->key1 );
		SubVector(  VECREF key2 , VECREF key1, VECREF key0 );
		SubVector(  VECREF key3 , VECREF mot->key2, VECREF key1 );

		//	fa = 2.0f * r0 - 2.0f * r1 + r2 + r3;
		ScaleVector( VECREF fa , VECREF key0, 2.0f );
		ScaleVector( VECREF tmp , VECREF key1, 2.0f );
		SubVector( VECREF fa , VECREF fa, VECREF tmp );
		AddVector( VECREF fa , VECREF fa, VECREF key2 );
		AddVector( VECREF fa , VECREF fa, VECREF key3 );

		//	fb = -3.0f * r0 + 3.0f * r1 - 2.0f * r2 - r3;
		ScaleVector( VECREF fb , VECREF key0, -3.0f );
		ScaleVector( VECREF tmp , VECREF key1, 3.0f );
		AddVector( VECREF fb , VECREF fb, VECREF tmp );
		ScaleVector( VECREF tmp , VECREF key2, 2.0f );
		SubVector( VECREF fb , VECREF fb, VECREF tmp );
		SubVector( VECREF fb , VECREF fb, VECREF key3 );

		//	fc = r2;
		//	fd = r0;
		//	res = fa * t3 + fb * t2 + fc * t + fd;
		MulVector( VECREF fa, VECREF fa, VECREF t3 );
		MulVector( VECREF fb, VECREF fb, VECREF t2 );
		MulVector( VECREF key2, VECREF key2, VECREF t1 );
		AddVector( VECREF tmp, VECREF fa, VECREF fb );
		AddVector( VECREF tmp, VECREF tmp, VECREF key2 );
		AddVector( VECREF mot->result, VECREF tmp, VECREF key0 );

		break;
	}

	mot->flag &= 0xffff-MOT_FLAG_NEEDCALC;	// 再計算フラグOFF
}


/*---------------------------------------------------------------------*
 *	Interface Routines
 *---------------------------------------------------------------------*/

void MocSetMax( int max )
{
	MAXMOT = max;
}

int MocInit( void )
{
	int a;
	mem_mot=(MOTINFO *)malloc( MAXMOT * sizeof(MOTINFO) );
	if (mem_mot==NULL) return -1;
	for(a=0;a<MAXMOT;a++) {
		mem_mot[a].flag=0;
	}
	return 0;
}


void MocDeleteAll( void )
{
	int a;
	for(a=0;a<MAXMOT;a++) {
		mem_mot[a].flag=0;
	}
}


void MocTerm( void )
{
	free( mem_mot );
}


void MocNewKey( int id, int subid, float fval, int times )
{
	MotNewKey( MOTPTR(id), subid, 1.0f / (float)times, fval );
}


void MocNewKeyR( int id, int subid, float fval, int times )
{
	MotNewKeyR( MOTPTR(id), subid, 1.0f / (float)times, fval );
}


void MocNewKeyI( int id, int subid, int ival, int times )
{
	MotNewKey( MOTPTR(id), subid, 1.0f / (float)times, (float)ival );
}


void MocNewKeyFV( int id, VECTOR *fv, int times )
{
	float t;
	MOTINFO *mot;
	FVRef fvector;

	fvector = (FVRef)fv;
	mot=MOTPTR(id);
	t=1.0f / (float)times;
	MotNewKey( mot, 0, t, fvector[0] );
	MotNewKey( mot, 1, t, fvector[1] );
	MotNewKey( mot, 2, t, fvector[2] );
	MotNewKey( mot, 3, t, fvector[3] );
}


void MocNewKeyIV( int id, IVECTOR *ivector, int times )
{
	float t;
	MOTINFO *mot;
	mot=MOTPTR(id);
	t=1.0f / (float)times;
	MotNewKey( mot, 0, t, (float)ivector->x );
	MotNewKey( mot, 1, t, (float)ivector->y );
	MotNewKey( mot, 2, t, (float)ivector->z );
	MotNewKey( mot, 3, t, (float)ivector->w );
}


void MocSetKey( int id, int subid, float fval )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	mot->key0[subid] = fval;
	mot->speed[subid] = 0.0f;
	mot->frame[subid] = 0.0f;
	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
}


void MocSetKeyD( int id, int subid, float fval )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	mot->result[subid] = fval;
	mot->key0[subid] = fval;
	mot->key1[subid] = fval;
	mot->key2[subid] = fval;
//	mot->speed[subid] = 1.0f;
//	mot->speed2[subid] = 1.0f;
}


void MocSetKeyI( int id, int subid, int ival )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	mot->key0[subid] = (float)ival;
	mot->speed[subid] = 0.0f;
	mot->frame[subid] = 0.0f;
	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
}


void MocSetKeyFV( int id, VECTOR *fv )
{
	//		fvectorを設定(f3相当)
	//
	FVRef fvector;
	MOTINFO *mot;

	fvector = (FVRef)fv;
	mot=MOTPTR(id);
	CopyVector ( VECREF mot->key0, VECREF fvector );
	setFVector ( mot->speed, 0.0f, 0.0f, 0.0f, 0.0f );
	setFVector ( mot->frame, 0.0f, 0.0f, 0.0f, 0.0f );
	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
}


void MocSetKeyFVD( int id, VECTOR *fv )
{
	//		fvectorを設定(f3相当)(direct)
	//
	FVRef fvector;
	MOTINFO *mot;

	fvector = (FVRef)fv;
	mot=MOTPTR(id);
	CopyVector ( VECREF mot->result, VECREF fvector );
}


void MocSetKeyF3( int id, VECTOR *fv )
{
	//		fvectorを設定(x,y,zのみ)
	//
	FVRef fvector;
	MOTINFO *mot;

	fvector = (FVRef)fv;
	mot=MOTPTR(id);
	fvector[3] = mot->key0[3];
	CopyVector ( VECREF mot->key0, VECREF fvector );
	setFVector ( mot->speed, 0.0f, 0.0f, 0.0f, 0.0f );
	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
}


void MocSetKeyIV( int id, IVECTOR *ivector )
{
	MOTINFO *mot;
	RXM_FV fvector;
	mot=MOTPTR(id);
	fvector[0] = (float)ivector->x;
	fvector[1] = (float)ivector->y;
	fvector[2] = (float)ivector->z;
	fvector[3] = (float)ivector->w;
	CopyVector ( VECREF mot->key0, VECREF fvector );
	mot->flag=MOT_ATTR_STATIC | MOT_FLAG_NEEDCALC;
}


float MocGetKey( int id, int subid )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
	return ( mot->result[subid] );
}


int MocGetKeyI( int id, int subid )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
	return (int)( mot->result[subid] );
}


void MocGetKeyFV( int id, VECTOR *fv )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
	CopyVector( fv , VECREF mot->result );
}


void MocGetKeyFP( int id, float *fptr )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
	CopyVector( (VECTOR *)fptr , VECREF mot->result );
}


void MocGetKeyFP2( int id, int num, float *fptr )
{
	int a;
	int i;
	float *target;
	MOTINFO *mot;

	i=id;
	target = fptr;
	target+=4;
	for(a=0;a<num;a++) {

		mot=MOTPTR(i++);
		if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
		CopyVector( (VECTOR *)target , VECREF mot->result );
		target+=4;

		mot=MOTPTR(i++);
		if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
		CopyVector( (VECTOR *)target , VECREF mot->result );
		target+=8;

	}

}


void MocGetKeyFP3( int id, int num, float *fptr )
{
	int a;
	int i,lp;
	float *target;
	MOTINFO *mot;

	i=id;
	target = fptr;
	lp=num*3;
	for(a=0;a<lp;a++) {
		mot=MOTPTR(i++);
		if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
		CopyVector( (VECTOR *)target , VECREF mot->result );
		target+=4;
	}

}


void *MocGetPtr( int id )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	mot->flag=MOT_ATTR_STATIC;
	return (void *)&mot->result;
}


void MocGetKeyIV( int id, IVECTOR *ivector )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if ( mot->flag & MOT_FLAG_NEEDCALC ) MotCalc( mot );
	ivector->x = (int)mot->result[0];
	ivector->y = (int)mot->result[1];
	ivector->z = (int)mot->result[2];
	ivector->w = (int)mot->result[3];
}


int MocGetFinishAll( int id )
{
	if ( ( MOTPTR(id)->flag & 0xff ) < 2 ) return -1;
	return 0;
}


int MocGetFinish( int id, int subid )
{
	if ( MOTPTR(id)->speed[subid]==0.0f ) return -1;
	return 0;
}


void MocSetAttr( int id, int opt )
{
	MOTPTR(id)->attr = opt;
}


void MocSetOption( int id, int subid, int opt )
{
	MOTPTR(id)->kframe[subid] = opt;
}


void MocMove( int id1, int id2 )
{
	int a,ed;
	MOTINFO *mot;
	a=id1;
	ed=id1+id2;
	mot=MOTPTR(a);
	while(1) {
		if (a>=ed) break;
		if (mot->flag) {
			MotUpdate( mot );
		}
		a++;
		mot++;
	}
}


void MocMoveAll( void )
{
	MocMove( 0,MAXMOT-1 );
}


void MocDelete( int id )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	mot->flag=0;
}


int MocRegist( int opt )
{
	int a,ent,emp;
	int motent;
	MOTINFO *mot;

	if (opt<=0) return 0;

	a=0;
	emp=0;
	ent=0;
	while(1) {
		if (a>=MAXMOT) { ent=-1;break; }
		if ( mem_mot[a].flag==0 ) {
			if (emp==0) ent=a;
			emp++;
			if (emp==opt) break;
		}
		else {
			emp=0;
		}
		a++;
	}

	if (ent<0) {
		char st[128];
		sprintf( st,"[moc] Regist Size Overflow." );
		MessageBox( NULL, st, "error",MB_ICONINFORMATION | MB_OK );
		//printf( "[moc] Regist Size Overflow.\n" );
		//exit(1);
		return 0;
	}

	motent=ent;
	for(a=0;a<opt;a++) {
		mot=MOTPTR(motent);
		MotReset( mot );
		motent++;
	}
	return ent;
}


void MocSetType( int id, int type )
{
	MOTINFO *mot;
	mot=MOTPTR(id);
	if (type) {
		mot->adv_func = (ADV_FUNC)MotNewAutoKey2;
	}
	else {
		mot->adv_func = (ADV_FUNC)MotNewAutoKey;
	}
}


int MocCnvKeyArray( int kfnum, void *kfdata, void *kfreal )
{
	//		キーフレームデータ変換
	//			kfnum  = kfdataの数
	//			kfdata = Key Frame Data (KFRAME *)
	//			kfreal = Key Frame 実データ (float *)
	//
	int a;
	int len;
	KFRAME *kf;
	KFRAME2 *kf2;
	motdef_kfdata = (KFRAME2 *) kfdata;
	motdef_kfreal = (float *) kfreal;

	kf=(KFRAME *)kfdata;
	kf2=(KFRAME2 *)kfdata;

	for(a=0;a<kfnum;a++) {
		len=kf->length;
		if (len==0) {
			kf2->advance = 0.0f;
		}
		else {
			kf2->advance = 1.0f / (float)len;
		}
		kf++;
		kf2++;
	}
	return 0;
}


int MocAssignKeyArray( int id, int opt, void *fdata, void *kfdata )
{
	int a,mask,i;
	MOTINFO *mot;

	mot=MOTPTR(id);

	motdef_kfdata = (KFRAME2 *) kfdata;
	motdef_kfreal = (float *) fdata;

	mot->kfdata = motdef_kfdata + 3;
	mot->kfreal = motdef_kfreal;

	mot->attr |= MOT_FLAG_AUTOKEY;
	mot->flag = MOT_FLAG_ACTION + mot->attr;

	mask=opt;
	for(a=0;a<4;a++) {
		if (mask&1) {
			mot->kframe[a] |= MOT_KEYOPT_AUTOKEY;
			i=motdef_kfdata[0].index;
			mot->key0[a] = motdef_kfreal[i+a];
			i=motdef_kfdata[1].index;
			mot->key1[a] = motdef_kfreal[i+a];
			mot->speed[a] = motdef_kfdata[1].advance;
			i=motdef_kfdata[2].index;
			mot->key2[a] = motdef_kfreal[i+a];
			mot->speed2[a] = motdef_kfdata[2].advance;
			mot->frame[a] = 0.0f;
		}
		mask>>=1;
	}
	return 0;
}


