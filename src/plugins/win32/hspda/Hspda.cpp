
//
//		Easy Data Access Plugin for HSP3
//				onion software/onitama 1999
//				               onitama 2005/5
//

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "../hpi3sample/hsp3plugin.h"
#include "membuf.h"
#include "ccsv.h"
#include "MTRand.h"

static ccsv *csv;


void Alertf( char *format, ... )
{
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	MessageBox( NULL, textbf, "error",MB_ICONINFORMATION | MB_OK );
}



int WINAPI hspda_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		csv = new ccsv();
		MTRandInit( -1 );
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		delete csv;
		csv = NULL;
	}
	return TRUE ;
}

/*------------------------------------------------------------*/
/*
		Sort Routines
*/
/*------------------------------------------------------------*/

typedef struct {
	union {
		int ikey;
		double dkey;
		char *skey;
	} as;
	int info;
} DATA;

int NoteToData( char *adr, DATA *data )
{
	char *p = adr;
	int line = 0;
	while (*p != '\0') {
		data[line].as.skey=p;
		data[line].info=line;
		while (*p != '\0') {
			char c = *p;
			if (c == '\n' || c == '\r') {
				*p = '\0';
			}
			p ++;
			if (c == '\n') break;
			if (c == '\r') {
				if (*p == '\n') p++;
				break;
			}
		}
		line ++;
	}
	return line;
}


int GetNoteLines( char *adr )
{
	int line = 0;
	char *p = adr;
	while (*p != '\0') {
		while (*p != '\0') {
			char c = *p++;
			if (c == '\n') break;
			if (c == '\r') {
				if (*p == '\n') p ++;
				break;
			}
		}
		line ++;
	}
	return line;
}


size_t DataToNoteLen( DATA *data, int num )
{
	size_t len = 0;
	int i;
	for (i = 0; i < num; i++) {
		char *s = data[i].as.skey;
		len += lstrlen(s) + 2;	// lstrlen("\r\n")
	}
	return len;
}


void DataToNote( DATA *data, char *adr, int num )
{
	int a;
	char *p;
	char *s;
	p=adr;
	for(a=0;a<num;a++) {
		s=data[a].as.skey;
		lstrcpy( p, s );
		p+=lstrlen( s );
		*p++=13; *p++=10;			// Add CR/LF
	}
	*p=0;
}


/*------------------------------------------------------------*/
/*
		HSP interface
*/
/*------------------------------------------------------------*/

static	DATA *dtmp = NULL;
static	int dtmp_size;

static void DataBye( void )
{
	if (dtmp!=NULL) {
		free(dtmp);
	}
}

static void DataIni( int size )
{
	DataBye();
	dtmp=(DATA *)malloc( sizeof(DATA)*size );
	dtmp_size = size;
}

static void DataExpand( int size )
{
	if (size <= dtmp_size) return;
	int new_size = dtmp_size;
	if (new_size < 16) new_size = 16;
	while (size > new_size) {
		new_size *= 2;
	}
	dtmp = (DATA *)realloc( dtmp, sizeof(DATA)*new_size );
	memset( dtmp + dtmp_size, 0, sizeof(DATA)*(new_size - dtmp_size) );
	dtmp_size = new_size;
}


static void DataInc( int n )
{
	DataExpand( n + 1 );
	dtmp[n].info ++;
}


static void *Hsp3GetBlockSize( HSPEXINFO *hei, PVal *pv, APTR ap, int *size )
{
	//		(HSP3用)
	//		pv,apからメモリブロックを取得する
	//
	PDAT *pd;
	HspVarProc *proc;
	proc = hei->HspFunc_getproc( pv->flag );
	pv->offset = ap;
	pd =  proc->GetPtr( pv );
	return proc->GetBlockSize( pv,pd,size );
}


EXPORT BOOL WINAPI csvnote( PVal *p1, char *p2, int p3, int p4 )
{
	//
	//		csvnote val,csvdata,spchr  (type$87)
	//
	int len;
	unsigned char *stmp;
	unsigned char *p;
	unsigned char *s;
	unsigned char a1;
	unsigned char spchr;

	if ( p3==0 ) spchr=','; else spchr=p3;
	len=p1->len[1];
	p=(unsigned char *)p1->pt;
	//p1->flag = 2;
	stmp=NULL;
	s=(unsigned char *)p2;

	if (s==p) {
		stmp=(unsigned char *)malloc( len<<2 );
		memcpy( stmp, s, len<<2 );
		s=stmp;
	}

	while(1) {
		a1=*s++;if (a1==0) break;
		if (a1==spchr) {
			*p++=13; *p++=10;			// Add CR/LF
		}
		else {
			*p++=a1;
			if (a1>=129) {					// 全角文字チェック
				if ((a1<=159)||(a1>=224)) {
					a1=*s++;if (a1==0) break;
					*p++=a1;
				}
			}
		}
	}
	*p=0;

	if (stmp!=NULL) free(stmp);
	return 0;
}


EXPORT BOOL WINAPI csvstr( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		csvstr val,csvdata,sepchr  (type$202)
	//
	int p3,len,limit,i,a,b;
	unsigned char *pbase;
	unsigned char *s;
	unsigned char a1;
	unsigned char spchr;
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );	// パラメータ2:変数
	p3 = hei->HspFunc_prm_getdi( 0 );		// パラメータ3:数値

	if ( p3==0 ) spchr=','; else spchr=p3;

//	len=p1->len[1]<<2;

	i = pv->len[1];
	if (i<=0) return -1;

//	limit=len-1;
	pbase=(unsigned char *)Hsp3GetBlockSize( hei, pv, 0, &limit );
	limit--;
	//p1->flag = 2;
	s=(unsigned char *)Hsp3GetBlockSize( hei, pv2, ap2, &len );
	a=0;b=1;

	while(1) {
		a1=*s++;if (a1==0) break;
		if (a1==spchr) {
			if (b<i) {
				pbase[a]=0;
				pbase=(unsigned char *)Hsp3GetBlockSize( hei, pv, b, &limit );
				limit--;
				a=0;b++;
			}
		}
		else {
			if (a<limit) pbase[a++]=a1;

			if (a1>=129) {					// 全角文字チェック
				if ((a1<=159)||(a1>=224)) {
					a1=*s++;if (a1==0) break;
					if (a<limit) pbase[a++]=a1;
				}
			}
		}
	}
	pbase[a]=0;
	return 0;
}


EXPORT BOOL WINAPI sortbye( int p1, int p2, int p3, int p4 )
{
	//
	//		sortbye (type$100)
	//
	DataBye();
	return 0;
}

/*------------------------------------------------------------*/
/*
		Exclusive Data Routines
*/
/*------------------------------------------------------------*/

static	PVal	*xn_pval;
static	APTR	xn_aptr;
static	int		xn_count;

EXPORT BOOL WINAPI xnotesel( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//
	//		xnotesel notedat, maxnum  (type$202)
	//
	PVal *pv;
	APTR ap;
	int a,i;

	ap = hei->HspFunc_prm_getva( &pv );
	i = hei->HspFunc_prm_getdi( 0 );
	if (i==0) i=256;			// default value
	if (pv->flag!=HSPVAR_FLAG_STR) return -1;
	xn_pval = pv;
	xn_aptr = ap;

	DataIni( i );
	for(a=0;a<i;a++) {
		dtmp[a].as.ikey=0;
		dtmp[a].info=0;
	}
	xn_count=0;
	return 0;
}


static char *skipline( char *s )
{
	while (*s != '\0') {
		char c = *s++;
		if(c == '\n') break;
		if(c == '\r') {
			if(*s == '\n') s ++;
			break;
		}
	}
	return s;
}


static int lineeq( char *a, char *b )
{
	while (1) {
		char ca = *a++;
		char cb = *b++;
		if (ca == '\n' || ca == '\r') ca = '\0';
		if (ca != cb) {
			return 0;
		}
		if (ca == '\0') {
			return 1;
		}
	}
}

static void pv_allocblock( HSPEXINFO *hei, PVal *pv, int offset, int size );

static void addline( HSPEXINFO *hei, PVal *pval, APTR aptr, size_t len, char *str_to_add )
{
	size_t len_add = strlen(str_to_add);
	int size;
	char *buf, *p;
	pv_allocblock( hei, pval, aptr, (int)(len + len_add + 8) );
	buf = (char *)Hsp3GetBlockSize( hei, pval, aptr, &size );
	p = buf + len;
	if (len > 0 && buf[len-1] != '\r' && buf[len-1] != '\n') {
		strcpy(p, "\r\n");
		p += 2;
	}
	strcpy(p, str_to_add);
	p += len_add;
	strcpy(p, "\r\n");
	p += 2;
}


EXPORT BOOL WINAPI xnoteadd( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//
	//		xnoteadd "strings" (type$202)
	//
	char *buf, *p, *str_to_add;
	int size;
	int line;

	str_to_add = hei->HspFunc_prm_gets();
	if (xn_pval->flag!=HSPVAR_FLAG_STR) return -1;
	buf = (char *)Hsp3GetBlockSize( hei, xn_pval, xn_aptr, &size );
	p = buf;
	line=0;
	while (*p != '\0') {
		if (lineeq(p, str_to_add) ) {
			DataInc(line);
			return -line;
		}
		p = skipline(p);
		line ++;
	}

	addline( hei, xn_pval, xn_aptr, p - buf, str_to_add );
	DataInc(line);
	return -line;
}

/*------------------------------------------------------------*/
/*
		Extra CSV Routines
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI csvsel( char *p1, int p2, int p3, int p4 )
{
	//
	//		csvsel val,sepchr  (type$1)
	//			( sepchr=0 / "," )
	//
	csv->SetBuffer( p1 );
	if ( p2 != 0 ) csv->SetSeparate( p2 );
			  else csv->SetSeparate( ',' );
	return 0;
}


EXPORT BOOL WINAPI csvres( PVal *p1, int p2, int p3, int p4 )
{
	//
	//		csvres val  (type$83)
	//
	int i;
	i=p1->len[1]<<2;
	csv->SetResultBuffer( p1->pt, i );
	return -i;
}


EXPORT BOOL WINAPI csvflag( int p1, int p2, int p3, int p4 )
{
	//
	//		csvflag id,val  (type$0)
	//
	csv->SetFlag( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI csvopt( int p1, int p2, int p3, int p4 )
{
	//
	//		csvopt option  (type$0)
	//
	csv->SetOption( p1 );
	return 0;
}


EXPORT BOOL WINAPI csvfind( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		csvfind "strings",max,start (type$6)
	//
	int i;
	i = csv->Search( p1, p2, p3 );
	return -i;
}


/*------------------------------------------------------------*/
/*
		Additional string service
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		Extra random service
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI rndf_ini( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		rndf_ini seed  (type$202)
	//
	int p1;
	p1 = hei->HspFunc_prm_getdi( -1 );		// パラメータ1:数値
	MTRandInit( p1 );
	return 0;
}


EXPORT BOOL WINAPI rndf_get( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		rndf_get var  (type$202)
	//
	PVal *pv;
	APTR ap;
	double dval;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	dval = MTRandGet();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_DOUBLE, &dval );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI rndf_geti( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		rndf_geti var, range  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1,p2;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi( 100 );		// パラメータ2:数値
	p2 = MTRandGetInt( p1 );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &p2 );	// 変数に値を代入
	return 0;
}


/*------------------------------------------------------------*/
/*
		HSP Variable service
*/
/*------------------------------------------------------------*/

#define HSP3VARFILEVER 0x1000
#define HSP3VARFILECODE "hspv"
#define HSP3VARFILEFXCODE 0x55aa0000

typedef struct HSP3VARFILEHED
{
	//	VAR File Header structure
	//
	char magic[4];				// magic code 'hspv'
	int ver;					// version code
	int num;					// num of data structure
	int pt_data;				// data block offset

} HSP3VARFILEHED;


typedef struct HSP3VARFILEDATA
{
	//	VAR File Data structure
	//
	int	name;					// name ptr
	int data;					// data ptr
	int opt;					// option (reserved)
	int encode;					// encode param (reserved)
	PVal master;				// PVal Master Data

} HSP3VARFILEDATA;

static	HSP3VARFILEHED varhed;
static	CMemBuf *varinfo;
static	CMemBuf *vardata;
static	HSP3VARFILEHED *vmem;
static	char *vload_tmp;

/*------------------------------------------------------------*/

static void pv_dispose( HSPEXINFO *hei, PVal *pv )
{
	HspVarProc *varproc;
	varproc = hei->HspFunc_getproc( pv->flag );
	varproc->Free( pv );
}


static void pv_alloc( HSPEXINFO *hei, PVal *pv, PVal *pv2 )
{
	HspVarProc *varproc;
	varproc = hei->HspFunc_getproc( pv->flag );
	varproc->Alloc( pv, pv2 );
}

static char *pv_getblock( HSPEXINFO *hei, PVal *pv, int offset, int *size )
{
	PDAT *p;
	HspVarProc *varproc;
	pv->offset=offset;
	varproc = hei->HspFunc_getproc( pv->flag );
	p = varproc->GetPtr( pv );
	varproc->GetBlockSize( pv, p, size );
	return (char *)p;
}

static void pv_allocblock( HSPEXINFO *hei, PVal *pv, int offset, int size )
{
	PDAT *p;
	HspVarProc *varproc;
	pv->offset=offset;
	varproc = hei->HspFunc_getproc( pv->flag );
	p = varproc->GetPtr( pv );
	varproc->AllocBlock( pv, p, size );
}

static FlexValue *pv_getfv( HSPEXINFO *hei, PVal *pv, int offset )
{
	PDAT *p;
	HspVarProc *varproc;
	pv->offset=offset;
	varproc = hei->HspFunc_getproc( pv->flag );
	p = varproc->GetPtr( pv );
	return (FlexValue *)p;
}

static int pv_seekstruct( HSPEXINFO *hei, char *name )
{
	//		特定名称のモジュールを検索する
	//
	HSPCTX *hspctx;
	STRUCTDAT *st;
	char *p;
	int i,max;

	hspctx = hei->hspctx;
	max = hspctx->hsphed->max_finfo / sizeof(STRUCTDAT);

	for(i=0;i<max;i++) {
		st = &hspctx->mem_finfo[ i ];
		p = hspctx->mem_mds + st->nameidx;
		if ( st->index == STRUCTDAT_INDEX_STRUCT ) {
			if ( strcmp( p, name ) == 0 ) return i;
		}
	}
	return -1;
}

static void *pv_setmodvar( HSPEXINFO *hei, PVal *pv, int offset, int id, int size )
{
	//		モジュール変数の内容を新規に設定する
	//
	PDAT *p;
	HspVarProc *varproc;
	FlexValue fv;
	FlexValue *target;
	char *newmem;

	fv.customid = id;
	fv.clonetype = 0;
	fv.size = size;
	fv.ptr = NULL;

	pv->offset=offset;
	varproc = hei->HspFunc_getproc( pv->flag );
	p = varproc->GetPtr( pv );
	varproc->Set( pv, p, &fv );

	newmem = hei->HspFunc_malloc( size );

	target = (FlexValue *)p;
	target->type = FLEXVAL_TYPE_ALLOC;
	target->ptr = (void *)newmem;

	return newmem;
}


/*------------------------------------------------------------*/

static int varsave_putvar( HSPEXINFO *hei, PVal *pv, int encode, int opt );
static int varload_getvar( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt );

static void varsave_init( void )
{
	strcpy( varhed.magic, HSP3VARFILECODE );
	varhed.ver = HSP3VARFILEVER;
	varhed.num = 0;
	varhed.pt_data = 0;
	varinfo = new CMemBuf;
	vardata = new CMemBuf;
}


static int varsave_bye( char *fname )
{
	int res;
	FILE *fp;
	int hedsize, infosize, datasize;

	res = 0;
	fp=fopen( fname, "wb" );
	if (fp != NULL) {

		hedsize = sizeof( HSP3VARFILEHED );
		infosize = varinfo->GetSize();
		datasize = vardata->GetSize();

		varhed.pt_data = hedsize + infosize;

		fwrite( &varhed, hedsize, 1, fp );
		fwrite( varinfo->GetBuffer(), infosize, 1, fp );
		fwrite( vardata->GetBuffer(), datasize, 1, fp );

		fclose(fp);
	} else {
		res = -1;
	}

	delete vardata;
	delete varinfo;
	return res;
}


static int varsave_put_storage( HSPEXINFO *hei, PVal *pv, int encode, int opt )
{
	//		固定長ストレージの保存
	//
	if ( pv->mode != HSPVAR_MODE_MALLOC ) return -1;
	switch ( pv->flag ) {
	case HSPVAR_FLAG_LABEL:
		{
		unsigned short **labels = (unsigned short **)pv->pt;
		int len = pv->size / sizeof(unsigned short *);
		unsigned short *mem_mcs = hei->hspctx->mem_mcs;
		int i;
		for ( i = 0; i < len; i ++ ) {
			unsigned short *label = labels[i];
			if ( label == NULL ) {
				vardata->Put( -1 );
			} else {
				vardata->Put( (int)(label - mem_mcs) );
			}
		}
		}
		break;
	case HSPVAR_FLAG_STRUCT:
		{
		int i,j,max,vmax;
		HSPCTX *hspctx;
		FlexValue *fv;
		PVal *fvbase;
		STRUCTPRM *prm;
		STRUCTDAT *st;
		char modname[64];
		int prevcnt,nowcnt;
		int *cntbak;

		hspctx = hei->hspctx;
		fv = pv_getfv( hei, pv, 0 );
		prm = &hspctx->mem_minfo[ fv->customid ];
		st = &hspctx->mem_finfo[ prm->subid ];
		memset( modname, 0, 64 );
		strcpy( modname, hspctx->mem_mds + st->nameidx );
		max = st->prmmax - 1;
		prm++;
		vmax = pv->size / sizeof(FlexValue);

		//Alertf( "#%d(%d) size(%d,%d) %s",fv->customid,max, fv->size, st->size, modname );

		//		タグコード + モジュール変数個数, モジュール名(64byte)を記録する
		vardata->Put( (int)HSP3VARFILEFXCODE + max );
		vardata->PutData( modname, 64 );				// モジュール名を保存する

//		for(i=0;i<max;i++) {
//			Alertf( "#%d(%s) PRM%d",i,modname, prm->mptype );
//			prm++;
//		}

		for(i=0;i<vmax;i++) {
			fv = pv_getfv( hei, pv, i );
			fvbase = (PVal *)fv->ptr;

			//		タグコード + typeを記録する
			vardata->Put( (int)HSP3VARFILEFXCODE + fv->type );
			//		実データを記録する
			if ( fv->type == FLEXVAL_TYPE_ALLOC ) {
				for(j=0;j<max;j++) {
					int pos;
					vardata->PutData( fvbase, sizeof(PVal) );
					pos = vardata->GetSize();
					vardata->Put( (int)0 );
					prevcnt = vardata->GetSize();
					varsave_putvar( hei, fvbase, encode, opt );
					nowcnt = vardata->GetSize();
					cntbak = (int *)(vardata->GetBuffer() + pos);
					*cntbak = nowcnt - prevcnt;			// 実データサイズを記録する
					fvbase++;
				}
			}
		}
		break;
		}
	case HSPVAR_FLAG_COMSTRUCT:							// COMOBJ型は無効にする
	case 7:												// Variant型は無効にする
		return -1;
	default:
		vardata->PutData( pv->pt, pv->size );			// 変数の持つメモリ全体を保存する
		break;
	}
	return 0;
}


static int varsave_put_flexstorage( HSPEXINFO *hei, PVal *pv, int encode, int opt )
{
	//		可変長ストレージの保存
	//
	int i,max;
	int size;
	char *p;
	if ( pv->mode != HSPVAR_MODE_MALLOC ) return -1;

	max = pv->size / sizeof(char *);
	for(i=0;i<max;i++) {
		p = pv_getblock( hei, pv, i, &size );

		//		タグコード, size(int), 実データ(size)を記録する
		vardata->Put( (int)HSP3VARFILEFXCODE );
		vardata->Put( size );
		vardata->PutData( p, size );					// 変数の持つメモリ全体を保存する
	}
	return 0;
}


static int varsave_putvar( HSPEXINFO *hei, PVal *pv, int encode, int opt )
{
	int res;
	unsigned short	support;
	res = -1;
	support = pv->support;
	if ( support & HSPVAR_SUPPORT_STORAGE ) res = varsave_put_storage( hei, pv, encode, opt );
	if ( support & HSPVAR_SUPPORT_FLEXSTORAGE ) res = varsave_put_flexstorage( hei, pv, encode, opt );
	return res;
}


static int varsave_put( HSPEXINFO *hei, int varid, int encode, int opt )
{
	HSPCTX *hspctx;
	HSP3VARFILEDATA dat;
	PVal *mem_var;
	char *name;
	char tmp[64];
	int res;

	hspctx = hei->hspctx;
	if (( varid < 0 )||( varid >= hspctx->hsphed->max_val )) return -1;
	mem_var = hspctx->mem_var + varid;

	name = hei->HspFunc_varname( varid );
	if ( *name == 0 ) {
		sprintf( tmp,"var#%d",varid );
		name = tmp;
	}

	dat.master = *mem_var;					// とりあえずPValを保存する
	dat.encode = encode;
	dat.opt = opt;

	dat.name = vardata->GetSize();
	vardata->PutStrBlock( name );			// 変数名を保存する
	dat.data = vardata->GetSize();

	res = varsave_putvar( hei, mem_var, encode, opt );
	if ( res ) return res;

	varinfo->PutData( &dat, sizeof(HSP3VARFILEDATA) );
	varhed.num++;
	return 0;
}


static int varload_init( void *mem )
{
	vmem = (HSP3VARFILEHED *)mem;
	if ( strcmp( vmem->magic, HSP3VARFILECODE ) ) return -1;
	return 0;
}


static void varload_bye( void )
{
}


static int varload_get_storage_struct( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt )
{
	//		固定長ストレージ(STRUCT)の取得
	//
	int i,j,code,max,vmax,type;
	int custid;
	int orgmax;
	int nextcnt;
	char *mem;
	char modname[64];
	char *orgname;
	HSPCTX *hspctx;
	PVal *fvbase;
	PVal *fvmem;
	STRUCTDAT *st;

	pv_dispose( hei,pv );								// 変数を破棄
	*pv = *pv2;
	pv_alloc( hei, pv, NULL );							// 変数を再確保

	mem = vdata;
	vmax = pv->size / sizeof(FlexValue);

	//		タグコード + モジュール変数個数, モジュール名(64byte)を取得
	code = *(int *)mem;
	max = code & 0xffff;
	code &= 0xffff0000;
	if ( code != HSP3VARFILEFXCODE ) return -1;
	mem += sizeof(int);

	memcpy( modname, mem, 64 );
	mem += 64;

	custid = pv_seekstruct( hei, modname );
	if ( custid < 0 ) return -1;

	hspctx = hei->hspctx;
	st = &hspctx->mem_finfo[ custid ];
	orgname = hspctx->mem_mds + st->nameidx;
	orgmax = st->prmmax - 1;
	if ( orgmax != max ) return -1;

	//Alertf( "%d(prm%d=%d) %s=%s",custid,max,st->prmmax,orgname,modname );

	for(i=0;i<vmax;i++) {

		//		タグコード + typeを取得する
		code = *(int *)mem;
		type = code & 0xffff;
		code &= 0xffff0000;
		if ( code != HSP3VARFILEFXCODE ) return -1;
		mem += sizeof(int);

		if ( type == FLEXVAL_TYPE_ALLOC ) {
			fvmem = (PVal *)pv_setmodvar( hei, pv, i, st->prmindex, st->size );
			for(j=0;j<max;j++) {
				fvbase = (PVal *)mem;
				mem += sizeof(PVal);
				nextcnt = *(int *)mem;
				mem += sizeof(int);
				//Alertf( "#%d:%d (%s) flag%d next=%d", i, j, modname, fvbase->flag, nextcnt );

				fvmem->mode = HSPVAR_MODE_NONE;
				fvmem->flag = HSPVAR_FLAG_INT;
				varload_getvar( hei, mem, fvmem, fvbase, encode, opt );
				fvmem++;
				mem += nextcnt;
			}
		}

	}
	return 0;
}


static int varload_get_storage_label( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt )
{
	//		固定長ストレージ(LABEL)の取得
	//
	int *offsets;
	int len;
	int i;
	unsigned short **p;
	unsigned short *mem_mcs = hei->hspctx->mem_mcs;
	pv_dispose( hei,pv );								// 変数を破棄
	*pv = *pv2;
	pv_alloc( hei, pv, NULL );							// 変数を再確保
	offsets = (int *)vdata;
	p = (unsigned short **)pv->pt;
	len = pv->size / sizeof(unsigned short *);
	for ( i = 0; i < len; i ++ ) {
		int offset = offsets[i];
		if ( offset == -1 ) {
			p[i] = NULL;
		} else {
			p[i] = mem_mcs + offset;
		}
	}
	return 0;
}


static int varload_get_storage( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt )
{
	//		固定長ストレージの取得
	//
	pv_dispose( hei,pv );								// 変数を破棄
	*pv = *pv2;
	pv_alloc( hei, pv, NULL );							// 変数を再確保
	memcpy( pv->pt, vdata, pv->size );					// データをコピー
	return 0;
}


static int varload_get_flexstorage( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt )
{
	//		可変長ストレージの取得
	//
	int i,code,max;
	int size;
	char *p;
	char *mem;

	pv_dispose( hei,pv );								// 変数を破棄
	*pv = *pv2;
	pv_alloc( hei, pv, NULL );							// 変数を再確保

	mem = vdata;
	max = pv->size / sizeof(char *);
	for(i=0;i<max;i++) {
		code = *(int *)mem;
		if ( code != HSP3VARFILEFXCODE ) return -1;
		mem += sizeof(int);
		size = *(int *)mem;
		mem += sizeof(int);
		pv_allocblock( hei, pv, i, size );
		p = pv_getblock( hei, pv, i, &code );
		memcpy( p, mem, size );							// データをコピー
		mem += size;
	}

	return 0;
}


static int varload_getvar( HSPEXINFO *hei, char *vdata, PVal *pv, PVal *pv2, int encode, int opt )
{
	int res;
	unsigned short	support;
	support = pv2->support;
	res = -1;
	if ( support & HSPVAR_SUPPORT_STORAGE ) {
		switch( pv2->flag ) {
		case HSPVAR_FLAG_LABEL:
			res = varload_get_storage_label( hei, vdata, pv, pv2, encode, opt );
			break;
		case HSPVAR_FLAG_STRUCT:
			res = varload_get_storage_struct( hei, vdata, pv, pv2, encode, opt );
			break;
		default:
			res = varload_get_storage( hei, vdata, pv, pv2, encode, opt );
			break;
		}
	}
	if ( support & HSPVAR_SUPPORT_FLEXSTORAGE ) {
		res = varload_get_flexstorage( hei, vdata, pv, pv2, encode, opt );
	}
	return res;
}


static int varload_get( HSPEXINFO *hei, int varid, char *getname, int encode, int opt )
{
	HSPCTX *hspctx;
	HSP3VARFILEDATA *dat;
	PVal *mem_var;
	char *buf;
	char *name;
	char *p;
	char *vdata;
	char tmp[64];
	int i,max;
	int res;

	hspctx = hei->hspctx;
	if (( varid < 0 )||( varid >= hspctx->hsphed->max_val )) return -1;
	mem_var = hspctx->mem_var + varid;

	name = getname;
	if ( name == NULL ) {
		name = hei->HspFunc_varname( varid );
	}
	if ( *name == 0 ) {
		sprintf( tmp,"var#%d",varid );
		name = tmp;
	}

	max = vmem->num;
	dat = (HSP3VARFILEDATA *)(vmem+1);
	buf = ( (char *)vmem ) + vmem->pt_data;
	for(i=0;i<max;i++) {
		p = buf + dat->name;
		vdata = buf + dat->data;
		if ( strcmp( p, name ) == 0 ) {
			res = varload_getvar( hei, vdata, mem_var, &dat->master, encode, opt );
			return res;
		}
		dat++;
	}

	return -1;
}



/*------------------------------------------------------------*/

EXPORT BOOL WINAPI getvarid( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		getvarid var,"name"  (type$202)
	//
	PVal *pv;
	APTR ap;
	char *p1;
	int p2;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	p1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	p2 = hei->HspFunc_seekvar( p1 );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &p2 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI getvarname( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		getvarname var,id  (type$202)
	//
	PVal *pv;
	APTR ap;
	int p1;
	char *p2;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ2:数値
	p2 = hei->HspFunc_varname( p1 );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, p2 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI getmaxvar( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		getmaxvar var  (type$202)
	//
	PVal *pv;
	APTR ap;
	HSPCTX *hspctx;
	int p2;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	hspctx = hei->hspctx;
	p2 = hspctx->hsphed->max_val;
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &p2 );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI vsave( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vsave "filename"  (type$202)
	//
	int i,max,res;
	char *p1;
	HSPCTX *hspctx;

	p1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列

	hspctx = hei->hspctx;
	max = hspctx->hsphed->max_val;

	varsave_init();
	for(i=0;i<max;i++) {
		varsave_put( hei, i, 0, 0 );
	}
	res = varsave_bye( p1 );
	return res;
}


EXPORT BOOL WINAPI vload( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vload "filename"  (type$202)
	//
	int i,max,res;
	char *p1;
	char *tmp;
	HSPCTX *hspctx;

	p1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列

	hspctx = hei->hspctx;
	max = hspctx->hsphed->max_val;

	res = hei->HspFunc_fsize( p1 );
	if ( res <= 0 ) return -1;

	tmp = (char *)malloc( res );
	hei->HspFunc_fread( p1, tmp, res, 0 );

	res = varload_init( tmp );
	if ( res == 0 ) {
		for(i=0;i<max;i++) {
			varload_get( hei, i, NULL, 0, 0 );
		}
	}
	varload_bye();
	free( tmp );
	return res;
}


EXPORT BOOL WINAPI vsave_start( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vsave_start  (type$202)
	//
	varsave_init();
	return 0;
}


EXPORT BOOL WINAPI vsave_put( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vsave_put var  (type$202)
	//
	PVal *pv;
	APTR ap;
	int res;
	int val,type;

	type = *(hei->nptype);
	val = *(hei->npval);
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数

	if ( type != TYPE_VAR ) return -2;
	res = varsave_put( hei, val, 0, 0 );
	return res;
}


EXPORT BOOL WINAPI vsave_end( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vsave_end "filename"  (type$202)
	//
	int res;
	char *p1;
	p1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	res = varsave_bye( p1 );
	return res;
}


EXPORT BOOL WINAPI vload_start( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vload_start "filename"  (type$202)
	//
	int res;
	char *p1;

	p1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列

	res = hei->HspFunc_fsize( p1 );
	if ( res <= 0 ) return -1;

	vload_tmp = (char *)malloc( res );
	hei->HspFunc_fread( p1, vload_tmp, res, 0 );

	res = varload_init( vload_tmp );
	if ( res ) {
		varload_bye();
		free( vload_tmp );
	}
	return res;
}


EXPORT BOOL WINAPI vload_get( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vload_get var  (type$202)
	//
	PVal *pv;
	APTR ap;
	int res;
	int val,type;

	type = *(hei->nptype);
	val = *(hei->npval);
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数

	if ( type != TYPE_VAR ) return -2;
	res = varload_get( hei, val, NULL, 0, 0 );
	return res;
}


EXPORT BOOL WINAPI vload_end( HSPEXINFO *hei, int _p1, int _p2, int _p3 )
{
	//
	//		vload_end  (type$202)
	//
	varload_bye();
	free( vload_tmp );
	return 0;
}


/*------------------------------------------------------------*/


EXPORT BOOL WINAPI binmatch(HSPEXINFO* hei, int _p1, int _p2, int _p3)
{
	//
	//		binmatch var, var2, varsize, var2size, offset, option  (type$202)
	//
	int p1, p2, p3, p4, len, limit, i, j, cur, total, ptr;
	unsigned char* pbase;
	unsigned char* ptarget;

	unsigned char* s;
	unsigned char* p;
	unsigned char a1;
	unsigned char a2;

	HSPCTX* ctx;
	PVal* pv;
	APTR ap;
	PVal* pv2;
	APTR ap2;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva(&pv2);	// パラメータ2:変数

	ptarget = (unsigned char*)Hsp3GetBlockSize(hei, pv, ap, &len);
	pbase = (unsigned char*)Hsp3GetBlockSize(hei, pv2, ap2, &limit);

	p1 = hei->HspFunc_prm_getdi(len);		// パラメータ3:数値
	p2 = hei->HspFunc_prm_getdi(limit);		// パラメータ4:数値
	p3 = hei->HspFunc_prm_getdi(0);			// パラメータ5:数値
	p4 = hei->HspFunc_prm_getdi(0);			// パラメータ6:数値

	ctx = hei->hspctx;
	total = 0;
	ptr = -1;

	ptarget = (unsigned char*)Hsp3GetBlockSize(hei, pv, ap, &len);
	if (p1 > 0) {
		if (p1<len) len = p1;
	}
	pbase = (unsigned char*)Hsp3GetBlockSize(hei, pv2, ap2, &limit);
	if (p2 > 0) {
		if (p2 < limit) limit = p2;
	}
	cur = p3;
	while (1) {
		if (cur >= len) break;
		p = ptarget + cur;
		s = pbase;
		i = cur; j = 0;
		while (1) {
			if (i >= len) break;
			a1 = *s++;
			a2 = *p++;
			if (a1 != a2) break;
			i++; j++;
			if (j >= limit) {
				total++;
				ptr = cur;
				if (p4 == 0) cur = len;		// マッチしたら終了する
				break;
			}
		}
		cur++;								// 検索位置を移動
	}
	ctx->strsize = ptr;
	return -total;
}


