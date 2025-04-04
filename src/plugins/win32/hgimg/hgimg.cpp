
//
//		High performance Graphic Image access Plugin for HSP
//				onion software/onitama 2000-2001
//

#include <windows.h>
#include <stdio.h>
#include "hgcnf.h"
#include "hginc.h"
#include <math.h>

#ifdef HGIMG_DIRECTX
	#include "../hgimgx/hgiox.h"
#else
	#include "hgio.h"
	#include "hgiof.h"
#endif

#include "hgdraw.h"

#include "hspdll.h"
#include "mxfile.h"
#include "dxffile.h"
#include "moc.h"
#include "sysreq.h"

/*------------------------------------------------------------*/
/*
		Values
*/
/*------------------------------------------------------------*/

hgdraw *hg=NULL;
static VECTOR vfwork;
static int sel;
static int col;
static int tex;
static int tx0,ty0,tx1,ty1;
static float sizex, sizey;
static int bgsizex, bgsizey;
static int bgcsizex, bgcsizey;
static int fntmode, fntalpha;
static char *dstp;
static int dstsx, dstsy;
static int dstofsy;
static int drawflag;
static int updateflag;
static char *mx_texlist = NULL;
static int addmode = 0;
static int fcolor = 0xffffff;

#define ADDMODE_FLAT 0
#define ADDMODE_TEX 1

static BMSCR *bmsrc;
static BMSCR *bmdst;
static BMSCR *bmtarget;
static HDC hdcsrc;
static HDC hdcdst;

/*------------------------------------------------------------*/
/*
		Routines
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		HSP interface
*/
/*------------------------------------------------------------*/

int WINAPI hgimg_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		hg = new hgdraw();
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		delete hg;
		hg = NULL;
	}
	return TRUE ;
}


EXPORT BOOL WINAPI hgini( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		hgini mode, yofs, ysize(type$02)
	//
	int yofs;
	int ysize;
	char *p;
	yofs = p2;
	ysize = p3;
	if ( ysize==0 ) ysize = bm->sy - yofs;

	SetSysReq( SYSREQ_DXHWND, (int)bm->hwnd );
	hg->Startup( p1 );
	p = (char *)bm->pBit + ( yofs * bm->sx );
	hg->setDest( p, bm->sx, ysize );
	bmdst = bm;
	hdcdst = bm->hdc;
	dstp = p;
	dstofsy = yofs;
	dstsx = bm->sx;
	dstsy = ysize;
	updateflag = -1;
	return 0;
}


EXPORT BOOL WINAPI hgreset( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		hgreset (type$02)
	//
	hg->Restart();
	hg->setDest( dstp, dstsx, dstsy );
	updateflag = -1;
	return 0;
}


EXPORT BOOL WINAPI hgsrc( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		hgsrc (type$02)
	//
	hg->setSrc( bm->pBit, bm->sx, bm->sy );
	return 0;
}


EXPORT BOOL WINAPI hgdst( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		hgdst mode (type$02)
	//
	bmtarget = bm;
	updateflag = p1;
	return 0;
}


EXPORT BOOL WINAPI hgsetreq( int p1, int p2, int p3, int p4 )
{
	//
	//		hgsetreq type,val (type$00)
	//
	SetSysReq( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI hggetreq( int *p1, int p2, int p3, int p4 )
{
	//
	//		hggetreq val,type (type$01)
	//
	*p1 = GetSysReq( p2 );
	return 0;
}


EXPORT BOOL WINAPI hgdraw( int p1, int p2, int p3, int p4 )
{
	//
	//		hgdraw (type$00)
	//
	hg->DrawStart();
	drawflag = hg->DrawEnd();
	if ( drawflag == 0 ) return 0;
	return -1;
}


EXPORT BOOL WINAPI hgbye( int p1, int p2, int p3, int p4 )
{
	//
	//		hgbye (type$100)
	//
	if ( hg != NULL ) {
		hg->Terminate();
	}
	return 0;
}


EXPORT BOOL WINAPI setborder( float p1, float p2, float p3, float p4 )
{
	//
	//		setborder sx,sy,sz  (type$00)
	//
	float x,y,z;
	x = p1 * 0.5f;
	y = p2 * 0.5f;
	z = p3 * 0.5f;
	hg->SetBorder( -x, x, -y, y, -z, z );
	return 0;
}


EXPORT BOOL WINAPI selmoc( int p1, int p2, int p3, int p4 )
{
	//
	//		selmoc id, mocofs (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + p2;
	return 0;
}


EXPORT BOOL WINAPI selpos( int p1, int p2, int p3, int p4 )
{
	//
	//		selpos id  (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + SCN_MOC_POS;
	return 0;
}


EXPORT BOOL WINAPI selang( int p1, int p2, int p3, int p4 )
{
	//
	//		selang id  (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + SCN_MOC_ANG;
	return 0;
}


EXPORT BOOL WINAPI selscale( int p1, int p2, int p3, int p4 )
{
	//
	//		selscale id  (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + SCN_MOC_SCALE;
	return 0;
}


EXPORT BOOL WINAPI seldir( int p1, int p2, int p3, int p4 )
{
	//
	//		seldir id  (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + SCN_MOC_DIR;
	return 0;
}


EXPORT BOOL WINAPI selefx( int p1, int p2, int p3, int p4 )
{
	//
	//		selefx id  (type$00)
	//
	sel = hg->GetObjMoc( p1 ) + SCN_MOC_EFX;
	return 0;
}


EXPORT BOOL WINAPI objsetf3( float p1, float p2, float p3, float p4 )
{
	//
	//		objsetf3 x,y,z  (type$00)
	//
	VECTOR v;
	v.x = p1; v.y = p2; v.z = p3; v.w = 1.0f;
	MocSetKeyFV( sel, &v );
	return 0;
}


EXPORT BOOL WINAPI objmovf3( int p1, float p2, float p3, float p4 )
{
	//
	//		objmovf3 times,x,y,z  (type$00)
	//
	VECTOR v;
	v.x = p2; v.y = p3; v.z = p4; v.w = 1.0f;
	MocNewKeyFV( sel, &v, p1 );
	return 0;
}


EXPORT BOOL WINAPI objaddf3( float p1, float p2, float p3, float p4 )
{
	//
	//		objaddf3 x,y,z  (type$00)
	//
	VECTOR v;
	VECTOR org;
	MocGetKeyFV( sel, &org );
	v.x = p1; v.y = p2; v.z = p3; v.w = 0.0f;
	AddVector( &v, &org, &v );
	MocSetKeyFV( sel, &v );
	return 0;
}



EXPORT BOOL WINAPI objset3( int p1, int p2, int p3, int p4 )
{
	//
	//		objset3 x,y,z  (type$00)
	//
	VECTOR v;
	v.x = (float)p1;
	v.y = (float)p2;
	v.z = (float)p3;
	v.w = 1.0f;
	MocSetKeyFV( sel, &v );
	return 0;
}


EXPORT BOOL WINAPI objmov3( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov3 times,x,y,z  (type$00)
	//
	VECTOR v;
	v.x = (float)p2;
	v.y = (float)p3;
	v.z = (float)p4;
	v.w = 1.0f;
	MocNewKeyFV( sel, &v, p1 );
	return 0;
}


EXPORT BOOL WINAPI objadd3( int p1, int p2, int p3, int p4 )
{
	//
	//		objadd3 x,y,z  (type$00)
	//
	VECTOR v;
	VECTOR org;
	MocGetKeyFV( sel, &org );
	v.x = (float)p1;
	v.y = (float)p2;
	v.z = (float)p3;
	v.w = 0.0f;
	AddVector( &v, &org, &v );
	MocSetKeyFV( sel, &v );
	return 0;
}



EXPORT BOOL WINAPI objset3r( int p1, int p2, int p3, int p4 )
{
	//
	//		objset3r x,y,z  (type$00)
	//
	VECTOR v;
	v.x = (float)p1*(PI2/256.0f);
	v.y = (float)p2*(PI2/256.0f);
	v.z = (float)p3*(PI2/256.0f);
	v.w = 1.0f;
	MocSetKeyFV( sel, &v );
	return 0;
}


EXPORT BOOL WINAPI objmov3r( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov3r times,x,y,z  (type$00)
	//
	VECTOR v;
	v.x = (float)p2*(PI2/256.0f);
	v.y = (float)p3*(PI2/256.0f);
	v.z = (float)p4*(PI2/256.0f);
	v.w = 1.0f;
	MocNewKeyFV( sel, &v, p1 );
	return 0;
}


EXPORT BOOL WINAPI objsetf2( int p1, float p2, float p3, float p4 )
{
	//
	//		objsetf2 ofs,x,y  (type$00)
	//
	MocSetKey( sel, p1, p2 );
	MocSetKey( sel, p1+1, p3 );
	return 0;
}


EXPORT BOOL WINAPI objmovf2( int p1, int p2, float p3, float p4 )
{
	//
	//		objmovf2 ofs,times,x,y  (type$00)
	//
	MocNewKey( sel, p1, p3, p2 );
	MocNewKey( sel, p1+1, p4, p2 );
	return 0;
}


EXPORT BOOL WINAPI objaddf2( int p1, float p2, float p3, float p4 )
{
	//
	//		objaddf2 ofs,x,y  (type$00)
	//
	float fp1,fp2;
	fp1 = MocGetKey( sel, p1 ) + p2;
	fp2 = MocGetKey( sel, p1+1 ) + p3;
	MocSetKey( sel, p1, fp1 );
	MocSetKey( sel, p1+1, fp2 );
	return 0;
}


EXPORT BOOL WINAPI objset2( int p1, int p2, int p3, int p4 )
{
	//
	//		objset2 ofs,x,y  (type$00)
	//
	MocSetKey( sel, p1, (float)p2 );
	MocSetKey( sel, p1+1, (float)p3 );
	return 0;
}


EXPORT BOOL WINAPI objmov2( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov2 ofs,times,x,y  (type$00)
	//
	MocNewKey( sel, p1, (float)p3, p2 );
	MocNewKey( sel, p1+1, (float)p4, p2 );
	return 0;
}


EXPORT BOOL WINAPI objset2r( int p1, int p2, int p3, int p4 )
{
	//
	//		objset2r ofs,x,y  (type$00)
	//
	MocSetKey( sel, p1, (float)p2*(PI2/256.0f) );
	MocSetKey( sel, p1+1, (float)p3*(PI2/256.0f) );
	return 0;
}


EXPORT BOOL WINAPI objmov2r( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov2r ofs,times,x,y  (type$00)
	//
	MocNewKey( sel, p1, (float)p3*(PI2/256.0f), p2 );
	MocNewKey( sel, p1+1, (float)p4*(PI2/256.0f), p2 );
	return 0;
}


EXPORT BOOL WINAPI objadd2( int p1, int p2, int p3, int p4 )
{
	//
	//		objadd2 ofs,x,y  (type$00)
	//
	float fp1,fp2;
	fp1 = MocGetKey( sel, p1 ) + (float)p2;
	fp2 = MocGetKey( sel, p1+1 ) + (float)p3;
	MocSetKey( sel, p1, fp1 );
	MocSetKey( sel, p1+1, fp2 );
	return 0;
}



EXPORT BOOL WINAPI objsetf1( int p1, float p2, float p3, float p4 )
{
	//
	//		objsetf1 ofs,x  (type$00)
	//
	MocSetKey( sel, p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI objmovf1( int p1, int p2, float p3, float p4 )
{
	//
	//		objmovf1 ofs,times,x  (type$00)
	//
	MocNewKey( sel, p1, p3, p2 );
	return 0;
}


EXPORT BOOL WINAPI objaddf1( int p1, float p2, float p3, float p4 )
{
	//
	//		objaddf1 ofs,x  (type$00)
	//
	float fp1;
	fp1 = MocGetKey( sel, p1 ) + p2;
	MocSetKey( sel, p1, fp1 );
	return 0;
}


EXPORT BOOL WINAPI objset1( int p1, int p2, int p3, int p4 )
{
	//
	//		objset1 ofs,x  (type$00)
	//
	MocSetKey( sel, p1, (float)p2 );
	return 0;
}


EXPORT BOOL WINAPI objmov1( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov1 ofs,times,x  (type$00)
	//
	MocNewKey( sel, p1, (float)p3, p2 );
	return 0;
}


EXPORT BOOL WINAPI objset1r( int p1, int p2, int p3, int p4 )
{
	//
	//		objset1r ofs,x  (type$00)
	//
	MocSetKey( sel, p1, (float)p2*(PI2/256.0f) );
	return 0;
}


EXPORT BOOL WINAPI objmov1r( int p1, int p2, int p3, int p4 )
{
	//
	//		objmov1r ofs,times,x  (type$00)
	//
	MocNewKey( sel, p1, (float)p3*(PI2/256.0f), p2 );
	return 0;
}


EXPORT BOOL WINAPI objadd1( int p1, int p2, int p3, int p4 )
{
	//
	//		objadd1 ofs,x  (type$00)
	//
	float fp1;
	fp1 = MocGetKey( sel, p1 ) + (float)p2;
	MocSetKey( sel, p1, fp1 );
	return 0;
}


EXPORT BOOL WINAPI objcheck( int *p1, int p2, int p3, int p4 )
{
	//
	//		objcheck val  (type$01)
	//
	*p1 = MocGetFinishAll( sel );
	return 0;
}


EXPORT BOOL WINAPI objmovmode( int p1, int p2, int p3, int p4 )
{
	//
	//		objmovmode mode  (type$00)
	//
	MocSetAttr( sel, p1 );
	return 0;
}


EXPORT BOOL WINAPI objmovopt( int p1, int p2, int p3, int p4 )
{
	//
	//		objmovopt ofs,option  (type$00)
	//
	MocSetOption( sel, p1, p2 );
	return 0;
}



EXPORT BOOL WINAPI fvset( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvset fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR v;
	fv[0] = (double)p1; fv[1] = (double)p2; fv[2] = (double)p3; fv[3] = 0.0;
	//SetVector( (VECTOR *)fv, p1, p2, p3, 0.0f );
	return 0;
}


EXPORT BOOL WINAPI fvseti( double *fv, int p1, int p2, int p3 )
{
	//
	//		fvseti fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR v;
	fv[0] = (double)p1; fv[1] = (double)p2; fv[2] = (double)p3; fv[3] = 0.0;
	//SetVector( (VECTOR *)fv, (float)p1, (float)p2, (float)p3, 0.0f );
	return 0;
}


EXPORT BOOL WINAPI fvadd( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvadd fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	//v->x += p1;
	//v->y += p2;
	//v->z += p3;
	fv[0] += (double)p1; fv[1] += (double)p2; fv[2] += (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvsub( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvsub fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	//v->x -= p1;
	//v->y -= p2;
	//v->z -= p3;
	fv[0] -= (double)p1; fv[1] -= (double)p2; fv[2] -= (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvmul( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvmul fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	//v->x *= p1;
	//v->y *= p2;
	//v->z *= p3;
	fv[0] *= (double)p1; fv[1] *= (double)p2; fv[2] *= (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvdiv( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvdiv fv,x,y,z  (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	//v->x /= p1;
	//v->y /= p2;
	//v->z /= p3;
	fv[0] /= (double)p1; fv[1] /= (double)p2; fv[2] /= (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvget( void *fv, float p1, float p2, float p3 )
{
	//
	//		fvget (type$01)
	//
	return 0;
}


EXPORT BOOL WINAPI fvdir( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvdir (type$01)(forHSP3)
	//
	VECTOR v;
	VECTOR res;
	VECTOR *ang;
	ang = (VECTOR *)&res;
//	v.x = p1;	v.y = p2;	v.z = p3;	v.w = 1.0f;
	SetVector( &v, p1, p2, p3, 1.0f );
	InitMatrix();
	RotZ( ang->x );
	RotY( ang->y );
	RotX( ang->z );
//	RotZ( ang->z );
//	RotY( ang->y );
//	RotX( ang->x );
	ApplyMatrix( ang, &v );

	fv[0] = ang->x;
	fv[1] = ang->y;
	fv[2] = ang->z;
	fv[3] = 0.0f;

	return 0;
}


EXPORT BOOL WINAPI fvmin( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvmin fv,x,y,z (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	if ( fv[0] < (double)p1 ) fv[0] = (double)p1;
	if ( fv[1] < (double)p2 ) fv[1] = (double)p2;
	if ( fv[2] < (double)p3 ) fv[2] = (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvmax( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvmax fv,x,y,z (type$01)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)fv;
	if ( fv[0] > (double)p1 ) fv[0] = (double)p1;
	if ( fv[1] > (double)p2 ) fv[1] = (double)p2;
	if ( fv[2] > (double)p3 ) fv[2] = (double)p3;
	return 0;
}


EXPORT BOOL WINAPI fvunit( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvunit fv (type$01)(forHSP3)
	//
	VECTOR v;
	//v = (VECTOR *)fv;
	v.x = (float)fv[0];
	v.y = (float)fv[1];
	v.z = (float)fv[2];
	v.w = (float)fv[3];
	UnitVector( &v );
	fv[0] = (double)v.x;
	fv[1] = (double)v.y;
	fv[2] = (double)v.z;
	fv[3] = (double)v.w;
	return 0;
}


EXPORT BOOL WINAPI fvouter( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvouter fv,x,y,z (type$01)(forHSP3)
	//
	VECTOR v;
	VECTOR v2;
	//v = (VECTOR *)fv;
	v.x = (float)fv[0];
	v.y = (float)fv[1];
	v.z = (float)fv[2];
	v.w = (float)fv[3];
	SetVector( &v2, p1, p2, p3, 1.0f );
	OuterProduct( &v, &v2, &v );
	fv[0] = (double)v.x;
	fv[1] = (double)v.y;
	fv[2] = (double)v.z;
	fv[3] = (double)v.w;
	return 0;
}


EXPORT BOOL WINAPI fvinner( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvinner fv,x,y,z (type$01)(forHSP3)
	//
	VECTOR v;
	VECTOR v2;
	//v = (VECTOR *)fv;
	v.x = (float)fv[0];
	v.y = (float)fv[1];
	v.z = (float)fv[2];
	v.w = (float)fv[3];
	SetVector( &v2, p1, p2, p3, 1.0f );
	fv[0] = (double)InnerProduct( &v, &v2 );
	return 0;
}


EXPORT BOOL WINAPI fvface( double *fv, float p1, float p2, float p3 )
{
	//
	//		fvface fv,x,y,z (type$01)(forHSP3)
	//
	VECTOR v;
	VECTOR v2;
	//v = (VECTOR *)fv;
	v.x = (float)fv[0];
	v.y = (float)fv[1];
	v.z = (float)fv[2];
	v.w = (float)fv[3];
	SetVector( &v2, p1, p2, p3, 1.0f );
	hg->GetTargetAngle( &v, &v, &v2 );
	fv[0] = (double)v.x;
	fv[1] = (double)v.y;
	fv[2] = (double)v.z;
	fv[3] = (double)v.w;
	return 0;
}


EXPORT BOOL WINAPI f2i( void *v, float p1, float p2, float p3 )
{
	//
	//		f2i v,x,y,z (type$01)
	//
	IVECTOR *iv;
	iv = (IVECTOR *)v;
	iv->x = (int)p1;
	iv->y = (int)p2;
	iv->z = (int)p3;
	return 0;
}


EXPORT BOOL WINAPI fsin( double *fv, float p1, float p2, float p3 )
{
	//
	//		fsin fval,rot (type$01)(forHSP3)
	//
	*fv = (double)sin( p1 );
	return 0;
}


EXPORT BOOL WINAPI fcos( double *fv, float p1, float p2, float p3 )
{
	//
	//		fcos fval,rot (type$01)(forHSP3)
	//
	*fv = (double)cos( p1 );
	return 0;
}


EXPORT BOOL WINAPI fsqr( double *fv, float p1, float p2, float p3 )
{
	//
	//		fsqr fval,fprm (type$01)(forHSP3)
	//
	*fv = (double)sqrt( p1 );
	return 0;
}


EXPORT BOOL WINAPI froti( double *fv, int p1, int p2, int p3 )
{
	//
	//		froti frot,introt (type$01)(forHSP3)
	//
	*fv = (double)p1 / 1024.0 * (double)PI2;
	return 0;
}


EXPORT BOOL WINAPI fadd( double *fv, float p1, float p2, float p3 )
{
	//
	//		fadd fv,x  (type$01)(forHSP3)
	//
	*fv += (double)p1;
	return 0;
}


EXPORT BOOL WINAPI fsub( double *fv, float p1, float p2, float p3 )
{
	//
	//		fsub fv,x  (type$01)(forHSP3)
	//
	*fv -= (double)p1;
	return 0;
}


EXPORT BOOL WINAPI fmul( double *fv, float p1, float p2, float p3 )
{
	//
	//		fmul fv,x  (type$01)(forHSP3)
	//
	*fv *= (double)p1;
	return 0;
}


EXPORT BOOL WINAPI fdiv( double *fv, float p1, float p2, float p3 )
{
	//
	//		fdiv fv,x  (type$01)(forHSP3)
	//
	*fv /= (double)p1;
	return 0;
}


EXPORT BOOL WINAPI fcmp( int *v, float p1, float p2, float p3 )
{
	//
	//		fdiv fv,p1,p2  (type$01)(forHSP3)
	//
	int a;
	if ( p1 == p2 ) a=0;
	else {
		if ( p1>p2 ) a=1; else a=-1;
	}
	*v = a;
	return 0;
}


EXPORT BOOL WINAPI fv2str( double *fv, float p1, float p2, char *str )
{
	//
	//		fv2str vec (type$11)(forHSP3)
	//			->refstr
	//
	sprintf( str,"%f,%f,%f",fv[0],fv[1],fv[2] );
	return 0;
}


EXPORT BOOL WINAPI f2str( char *str, float p1, float p2, float p3 )
{
	//
	//		f2str val,prm (type1)(forHSP3)
	//
	sprintf( str,"%f",p1 );
	return 0;
}


EXPORT BOOL WINAPI str2fv( double *fv, char *p2, float p3, float p4 )
{
	//
	//		str2fv fv,"val" (type5)(forHSP3)
	//
	//VECTOR *v;
	//v = (VECTOR *)p1;
	sscanf( p2,"%f,%f,%f",&fv[0],&fv[1],&fv[2] );
	return 0;
}


EXPORT BOOL WINAPI str2f( double *p1, char *p2, float p3, float p4 )
{
	//
	//		str2f fval,"val" (type5)(forHSP3)
	//
	sscanf( p2,"%f",p1 );
	return 0;
}


EXPORT BOOL WINAPI objgetstr( char *str, float p1, float p2, float p3 )
{
	//
	//		objgetstr val  (type$01)(forHSP3)
	//
	VECTOR v;
	MocGetKeyFV( sel, &v );
	sprintf( str,"%f,%f,%f",v.x,v.y,v.z );
	return 0;
}


EXPORT BOOL WINAPI objgetfv( double *fv, float p1, float p2, float p3 )
{
	//
	//		objgetfv fv  (type$01)(forHSP3)
	//
	VECTOR v;
	MocGetKeyFV( sel, &v );
	fv[0] = (double)v.x;
	fv[1] = (double)v.y;
	fv[2] = (double)v.z;
	fv[3] = 1.0;
	return 0;
}


EXPORT BOOL WINAPI objgetv( int *vv, float p1, float p2, float p3 )
{
	//
	//		objgetv v  (type$01)
	//
	IVECTOR *iv;
	iv = (IVECTOR *)vv;
	MocGetKeyIV( sel, iv );
	return 0;
}


EXPORT BOOL WINAPI objsetfv( double *fv, float p1, float p2, float p3 )
{
	//
	//		objsetfv fv  (type$01)(forHSP3)
	//
	VECTOR v;
	v.x = (float)fv[0];
	v.y = (float)fv[1];
	v.z = (float)fv[2];
	v.w = (float)fv[3];
	MocSetKeyFV( sel, &v );
	return 0;
}


EXPORT BOOL WINAPI objsetv( void *vv, float p1, float p2, float p3 )
{
	//
	//		objsetv v  (type$01)
	//
	IVECTOR *iv;
	iv = (IVECTOR *)vv;
	MocSetKeyIV( sel, iv );
	return 0;
}


EXPORT BOOL WINAPI objaddfv( void *fv, float p1, float p2, float p3 )
{
	//
	//		objaddfv fv  (type$01)
	//
	VECTOR vt;
	VECTOR *v;
	v = (VECTOR *)fv;
	MocGetKeyFV( sel, &vt );
	AddVector( &vt, &vt, v );
	MocSetKeyFV( sel, &vt );
	return 0;
}


EXPORT BOOL WINAPI objmovfv( void *fv, int p1, float p2, float p3 )
{
	//
	//		objmovfv fv,times (type$01)
	//
	MocNewKeyFV( sel, &vfwork, p1 );
	return 0;
}


/*-------------------------------------------------------*/

EXPORT BOOL WINAPI setuv( int p1, int p2, int p3, int p4 )
{
	//
	//		setuv tx0,ty0,tx1,ty1 (type0)
	//
	tx0 = p1;
	ty0 = p2;
	tx1 = p3;
	ty1 = p4;
	hg->SetUVAnimSize( tx1 - tx0 + 1, ty1 - ty0 + 1 );
	addmode = ADDMODE_TEX;
	return 0;
}


EXPORT BOOL WINAPI setcolor( int p1, int p2, int p3, int p4 )
{
	//
	//		setcolor r,g,b (type0)
	//
	fcolor = (p1<<16)|(p2<<8)|p3;
	addmode = ADDMODE_FLAT;
	return 0;
}


EXPORT BOOL WINAPI setsizef( float p1, float p2, int p3, int p4 )
{
	//
	//		setsizef sx,sy (type0)
	//
	sizex = p1;
	sizey = p2;
	return 0;
}


EXPORT BOOL WINAPI setbg( int p1, int p2, int p3, int p4 )
{
	//
	//		setbg sx,sy,cx,cy (type0)
	//
	bgsizex = p1;
	bgsizey = p2;
	bgcsizex = p3;
	bgcsizey = p4;
	return 0;
}


EXPORT BOOL WINAPI clscolor( int p1, int p2, int p3, int p4 )
{
	//
	//		clscolor color (type0)
	//
	hg->DrawSetBG( p1 );
	return 0;
}


EXPORT BOOL WINAPI clsblur( int p1, int p2, int p3, int p4 )
{
	//
	//		clsblur val (type0)
	//
	hg->DrawSetBGBlur( p1 );
	return 0;
}


EXPORT BOOL WINAPI clstex( int p1, int p2, int p3, int p4 )
{
	//
	//		clstex mode (type0)
	//			0=regist/-1=none/1=resume
	//
	if ( p1<0 ) {
		hg->DrawSetBG( -1 );
		return 0;
	}
	if ( p1>0 ) {
		hg->DrawSetBGTex( -1 );
		return 0;
	}
	hg->DrawSetBGTex( hg->io.GetNextTexID() );
	return 0;
}


EXPORT BOOL WINAPI setfont( int p1, int p2, int p3, int p4 )
{
	//
	//		setfont cx,cy,px,sw (type0)
	//
	fntmode = 0;
	fntalpha = 0;
	if ( p4&1 ) fntmode |= NODE_ATTR_COLKEY;
	hg->SetFont( hg->io.GetNextTexID(), p1, p2, p3, fntmode );
	return 0;
}


EXPORT BOOL WINAPI falpha( int p1, int p2, int p3, int p4 )
{
	//
	//		falpha alpha (type0)
	//
	if ( ( p1&0xf00 )==0x100 ) {
		fntalpha = 0;
	} else {
		fntalpha = NODE_ATTR_USEALPHA | p1;
	}
	return 0;
}


EXPORT BOOL WINAPI fprt( BMSCR *bm, char *p2, int p3, int p4 )
{
	//
	//		fprt "mes",x,y (type6)
	//
//	if ( fntmode ) {
//		hg->DrawFontEx( p3,p4,p2, (char *)bm->pBit, bm->sx, bm->sy );
//	}
//	else {
		hg->DrawFont( p3,p4,fntalpha,p2 );
//	}
	return 0;
}


EXPORT BOOL WINAPI getbg( PVAL2 *p1, int p2, int p3, int p4 )
{
	//
	//		getbg val, modelID (type$83)
	//
	char *p;
	p = hg->GetBGVram( hg->GetModel( p2 ) );
	if ( p == NULL ) return -1;
	p1->flag = 4;				// val
	p1->mode = 1;				// clone
	p1->len[1] = 0xffff;		// size
	p1->pt = p;					// PTR
	return 0;
}


EXPORT BOOL WINAPI setmap( int p1, int p2, int p3, int p4 )
{
	//
	//		setmap modelID, x, y (type0)
	//
	hg->SetBGView( hg->GetModel( p1 ), p2, p3 );
	return 0;
}


EXPORT BOOL WINAPI addbox( int *p1, int p2, int p3, int p4 )
{
	//
	//		addbox val,sw (type1)
	//			sizex,sizey     : 立方体サイズ
	//			tx0,ty0,tx1,ty1 : TexUV
	//
	//
	int id,sw;
	MODEL *model;
	sw = 0;
	if ( p2&1 ) sw = NODE_ATTR_COLKEY;
	if ( p2&2 ) sw = NODE_ATTR_SHADE_FLAT;
	id = hg->RegistModel();
	model = hg->GetModel( id );
	if ( addmode == ADDMODE_TEX ) {
		hg->MakeBoxNode( model, sizex, sizey, tx0, ty0, tx1, ty1, sw );
	} else {
		hg->MakeBoxNodeF( model, sizex, sizey, fcolor, sw );
	}
	hg->MakeModelNormals( id, 0 );
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI addmesh( int *p1, int p2, int p3, int p4 )
{
	//
	//		addmesh val,divx,divy,sw (type1)
	//			sizex,sizey     : 立方体サイズ
	//			tx0,ty0,tx1,ty1 : TexUV
	//
	//
	int id,sw;
	MODEL *model;
	sw = p4&0xff0;
	if ( p4&1 ) sw = NODE_ATTR_COLKEY;
	if ( p4&2 ) sw = NODE_ATTR_SHADE_FLAT;
	id = hg->RegistModel();
	model = hg->GetModel( id );
	hg->MakeMeshNode( model, sizex, sizey, p2, p3, tx1-tx0+1, ty1-ty0+1, sw );
	hg->MakeModelNormals( id, 0 );
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI addplate( int *p1, int p2, int p3, int p4 )
{
	//
	//		addplate val,sw (type1)
	//			sizex,sizey     : 立方体サイズ
	//			tx0,ty0,tx1,ty1 : TexUV
	//			sw : material option
	//
	int id,sw;
	MODEL *model;
	VECTOR pos,ang;
	sw = 0;
	id = hg->RegistModel();
	model = hg->GetModel( id );
	SetVector( &pos, 0.0f, 0.0f, 0.0f, 0.0f );
	SetVector( &ang, 0.0f, PI, 0.0f, 0.0f );
	if ( p2&1 ) sw = NODE_ATTR_COLKEY;
	if ( p2&2 ) sw = NODE_ATTR_SHADE_FLAT;
	if ( addmode == ADDMODE_TEX ) {
		hg->MakePlaneNode( model, PRIM_POLY4T, sizex, sizey, tx0, ty0, tx1, ty1, &pos, &ang, sw );
	} else {
		hg->MakePlaneNodeF( model, PRIM_POLY4F, sizex, sizey, fcolor, &pos, &ang, sw );
	}
	hg->SetModelModeDef( id, OBJ_MODE_TREE );
	hg->MakeModelNormals( id, 0 );
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI addspr( int *p1, int p2, int p3, int p4 )
{
	//
	//		addspr val,sw (type1)
	//			tx0,ty0,tx1,ty1 : TexUV
	//
	//
	int id,sw;
	MODEL *model;
	sw = 0;
	id = hg->RegistModel();
	model = hg->GetModel( id );
	if (p2) sw |= NODE_ATTR_COLKEY;
	hg->MakeSpriteNode( model, tx0, ty0, tx1, ty1, sw );
	hg->SetModelModeDef( id, OBJ_MODE_2D );
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI addbg( int *p1, int p2, int p3, int p4 )
{
	//
	//		addbg val, winsizex, winsizey (type1)
	//
	//
	int id,sw;
	MODEL *model;
	sw = 0; if (p4) sw |= NODE_ATTR_COLKEY;
	id = hg->RegistModel();
	model = hg->GetModel( id );
	hg->MakeBGNode( model, bgsizex, bgsizey, p2, p3, bgcsizex, bgcsizey, sw );
	hg->SetModelModeDef( id, OBJ_MODE_2D );
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI evmodel( int p1, int p2, int p3, int p4 )
{
	//
	//		evmodel eventID, modelID (type0)
	//
	hg->SetEventModel( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI setmode( int p1, int p2, int p3, int p4 )
{
	//
	//		setmode ModelID, mode (type0)
	//
	hg->SetModelModeDef( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI settimer( int p1, int p2, int p3, int p4 )
{
	//
	//		settimer ModelID, timer (type0)
	//
	hg->SetModelTimerDef( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI regobj( int *p1, int p2, int p3, int p4 )
{
	//
	//		regobj result, ModelID, mode (type1)
	//
	*p1 = hg->RegistObj( p3, p2 );
	if ( *p1 < 0 ) return -1;
	return 0;
}


EXPORT BOOL WINAPI delobj( int p1, int p2, int p3, int p4 )
{
	//
	//		delobj ObjectID (type0)
	//
	hg->DeleteObj( p1 );
	return 0;
}


EXPORT BOOL WINAPI setobjm( int p1, int p2, int p3, int p4 )
{
	//
	//		setobjm ObjID, ModelID (type0)
	//
	hg->SetObjModel( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI setobjmode( int p1, int p2, int p3, int p4 )
{
	//
	//		setobjmode id,mode,sw  (type$00)
	//
	hg->SetObjMode( p1, p2, p3 );
	return 0;
}


EXPORT BOOL WINAPI setcoli( int p1, int p2, int p3, int p4 )
{
	//
	//		setcoli id,mygroup,enegroup  (type$00)
	//
	hg->SetObjColi( p1, p2, p3 );
	return 0;
}


EXPORT BOOL WINAPI getcoli( int *p1, int p2, float p3, int p4 )
{
	//
	//		setcoli val,id,distance  (type$01)
	//
	*p1 = hg->UpdateObjColi( p2, p3 );
	return 0;
}


EXPORT BOOL WINAPI findobj( int p1, int p2, float p3, int p4 )
{
	//
	//		findobj exmode,group  (type$00)
	//
	hg->StartObjFind( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI nextobj( int *p1, int p2, float p3, int p4 )
{
	//
	//		nextobj val  (type$01)
	//
	*p1 = hg->FindObj();
	return 0;
}


EXPORT BOOL WINAPI objact( int p1, int p2, int p3, int p4 )
{
	//
	//		objact ObjID, anim (type0)
	//
	hg->SetObjAnim( p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI uvanim( int p1, int p2, int p3, int p4 )
{
	//
	//		uvanim id, wait, times, sw (type0)
	//
	hg->SetModelAnimDef( p1,p4,p2,p3 );
	return 0;
}


EXPORT BOOL WINAPI setmtex( int p1, int p2, int p3, int p4 )
{
	//
	//		setmtex modelid, nodeid, tex, shade (type0)
	//
	if (p2<0) {
		int i;
		MODEL *m;
		m = hg->GetModel( p1 );
		for(i=0;i<m->num;i++) {
			hg->SetModelTexEx( m,i,p3,p4 );
		}
		return 0;
	}
	hg->SetModelTexEx( hg->GetModel( p1 ),p2,p3,p4 );
	return 0;
}


EXPORT BOOL WINAPI getmtex( int *p1, int p2, int p3, int p4 )
{
	//
	//		getmtex val, modelid, nodeid (type0)
	//
	*p1 = hg->SetModelTex( hg->GetModel( p2 ),p3,-1 );
	return 0;
}


/*
EXPORT BOOL WINAPI getnode( PVAL2 *p1, int p2, int p3, int p4 )
{
	//
	//		getnode val, modelID, nodeID (type$83)
	//
	char *p;
	p = (char *)hg->GetModelNode( hg->GetModel( p2 ), p3 );
	if ( p == NULL ) return -1;
	p1->flag = 4;				// val
	p1->mode = 1;				// clone
	p1->len[1] = 0xffff;		// size
	p1->pt = p;					// PTR
	return 0;
}
*/

/*-------------------------------------------------------*/

static void CopySub0( BMSCR *bm )
{
	//		x2 copy(full) (pal->pal)
	//
	char *p0;
	char *p1;
	char *tp1;
	char a1;
	int sx,sy;
	int dsx,dsy;
	int x,y;
	p0 = dstp; sx = dstsx; sy = dstsy;
	//p0 = (char *)bmdst->pBit;
	//sx = bmdst->sx; sy = bmdst->sy;
	p1 = (char *)bm->pBit;
	p1 += dstofsy * bm->sx * 2;
	dsx = bm->sx; dsy = bm->sy;
	for(y=0;y<sy;y++) {
		tp1 = p1;
		for(x=0;x<sx;x++) {
			a1 = *p0++;
			*(tp1+dsx) = a1;
			*tp1++ = a1;
			*(tp1+dsx) = a1;
			*tp1++ = a1;
		}
		p1+=dsx<<1;
	}
}


static void CopySub1( BMSCR *bm )
{
	//		x2 copy(half) (pal->pal)
	//
	char *p0;
	char *p1;
	char *tp1;
	int sx,sy;
	int dsx,dsy;
	int x,y;
	p0 = dstp; sx = dstsx; sy = dstsy;
	//p0 = (char *)bmdst->pBit;
	//sx = bmdst->sx; sy = bmdst->sy;
	p1 = (char *)bm->pBit;
	p1 += dstofsy * bm->sx * 2;
	dsx = bm->sx; dsy = bm->sy;
	for(y=0;y<sy;y++) {
		tp1 = p1;
		for(x=0;x<sx;x++) {
			*tp1++ = *p0;
			*tp1++ = *p0++;
		}
		p1+=dsx<<1;
	}
}


static void CopySub2( BMSCR *bm )
{
	//		x2 copy(half x half) (pal->pal)
	//
	char *p0;
	char *p1;
	char *tp1;
	int sx,sy;
	int dsx,dsy;
	int x,y;
	p0 = dstp; sx = dstsx; sy = dstsy;
	//p0 = (char *)bmdst->pBit;
	//sx = bmdst->sx; sy = bmdst->sy;
	p1 = (char *)bm->pBit;
	p1 += dstofsy * bm->sx * 2;
	dsx = bm->sx; dsy = bm->sy;
	for(y=0;y<sy;y++) {
		tp1 = p1;
		for(x=0;x<sx;x++) {
			*tp1++ = *p0++;
			*tp1++;
		}
		p1+=dsx<<1;
	}
}


EXPORT BOOL WINAPI copybuf( BMSCR *bm, int mode, int opt1, int opt2 )
{
	//
	//		copybuf mode  (type0)
	//
	switch( mode ) {
	case 0:
		CopySub1( bm );
		break;
	case 1:
		CopySub0( bm );
		break;
	case 2:
		CopySub2( bm );
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI getsync( int *p1, int p2, int p3, int p4 )
{
	//
	//		getsync val,mode (type$01)
	//
	switch( p2 ) {
	case 0:
		*p1 = hg->GetTimerCount();
		break;
	case 1:
		*p1 = hg->GetTimerResolution();
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI sync( int p1, int p2, int p3, int p4 )
{
	//
	//		sync val (type$00)
	//
	hg->Sync( p1 );
	return 0x10000;					// await 0
}


EXPORT BOOL WINAPI getdebug( int *p1, int p2, int p3, int p4 )
{
	//
	//		getdebug val (type$01)
	//
	*p1 = hg->debug;
	return 0;
}


EXPORT BOOL WINAPI hgsync( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		hgsync time (type$02)
	//
	hg->Sync( p1 );
#ifdef HGIMG_DIRECTX
	if ( drawflag ) {
		hg->UpdateScreen();
	}
#else
	HDC hdc;
	if ( drawflag ) {
		if ( updateflag>=0 ) {
			copybuf( bmtarget, updateflag, 0, 0 );
		}
//		if (bm->fl_udraw) {
			hdc=GetDC( bm->hwnd );
			BitBlt( hdc, 0, 0, bm->wx, bm->wy, bm->hdc, bm->xx, bm->yy, SRCCOPY );
			ReleaseDC( bm->hwnd,hdc );
//		}
	}
#endif
	return 0x10000;					// await 0
}


/*-------------------------------------------------------*/

EXPORT BOOL WINAPI selcam( int p1, int p2, int p3, int p4 )
{
	//
	//		selcam ofs  (type$00)
	//
	sel = hg->GetCamMoc() + p1;
	return 0;
}


EXPORT BOOL WINAPI selcpos( int p1, int p2, int p3, int p4 )
{
	//
	//		selcpos  (type$00)
	//
	sel = hg->GetCamMoc() + SCN_MOC_POS;
	return 0;
}


EXPORT BOOL WINAPI selcang( int p1, int p2, int p3, int p4 )
{
	//
	//		selcang  (type$00)
	//
	sel = hg->GetCamMoc() + SCN_MOC_ANG;
	return 0;
}


EXPORT BOOL WINAPI selcint( int p1, int p2, int p3, int p4 )
{
	//
	//		selcint  (type$00)
	//
	sel = hg->GetCamMoc() + SCN_MOC_SCALE;
	return 0;
}


EXPORT BOOL WINAPI cammode( int p1, int p2, int p3, int p4 )
{
	//
	//		cammode  (type$00)
	//
	hg->SetCamMode( p1 );
	return 0;
}


/*-------------------------------------------------------*/

EXPORT BOOL WINAPI sellight( int p1, int p2, int p3, int p4 )
{
	//
	//		sellight id,ofs  (type$00)
	//
	sel = hg->GetLightMoc(p1) + p2;
	return 0;
}


EXPORT BOOL WINAPI sellpos( int p1, int p2, int p3, int p4 )
{
	//
	//		sellpos id  (type$00)
	//
	sel = hg->GetLightMoc(p1) + LIGHT_MOC_POS;
	return 0;
}


EXPORT BOOL WINAPI sellang( int p1, int p2, int p3, int p4 )
{
	//
	//		sellang id  (type$00)
	//
	sel = hg->GetLightMoc(p1) + LIGHT_MOC_ANG;
	return 0;
}


EXPORT BOOL WINAPI sellcolor( int p1, int p2, int p3, int p4 )
{
	//
	//		sellcolor id  (type$00)
	//
	sel = hg->GetLightMoc(p1) + LIGHT_MOC_COLOR;
	return 0;
}


/*-------------------------------------------------------*/

EXPORT BOOL WINAPI mxsend( char *p1, int p2, int p3, int p4 )
{
	//
	//		mxsend val,mode  (type$01)
	//
	mxInit( hg, p1, p2 );
	return 0;
}


EXPORT BOOL WINAPI mxconv( int *p1, int p2, int p3, int p4 )
{
	//
	//		mxconv val  (type$01)
	//
	int id;
	id = mxConvHG( hg );
	mxTerm();
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI mxaconv( char *p1, int p2, int p3, int p4 )
{
	//
	//		mxaconv val (type$01)
	//
	mxConvHGAnim( hg, p1 );
	return 0;
}


EXPORT BOOL WINAPI mxgetpoly( int *p1, int p2, int p3, int p4 )
{
	//
	//		mxgetpoly val  (type$01)
	//
	*p1 = mxGetNumPoly();
	return 0;
}


EXPORT BOOL WINAPI mxgetname( char *p1, int p2, int p3, int p4 )
{
	//
	//		mxgetname val,id  (type$01)
	//
	char *src;
	char *dst;
	char a1;
	if ( p2>=mxGetMaxImage() ) {
		*p1 = 0;return -1;
	}
	src = mxGetImage( p2 );
	dst = p1;
	while(1) {
		a1 = *src++;
		if (( a1==0 )||( a1=='/' )) break;
		*dst++ = a1;
	}
	*dst=0;
	return 0;
}


EXPORT BOOL WINAPI settex( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		settex x,y,sw  (type$02)
	//
	char *buf;
	int a;
	buf = (char *)bm->pBit;
#ifdef HGIMG_DIRECTX
	if ( bm->palmode == 0 ) {
		a = hg->io.RegistTex( buf,bm->sx,bm->sy,p1,p2,p3 );
	}
	else {
		a = hgiox_RegistTexIndex( buf, (char *)bm->pal, bm->sx, bm->sy, p1, p2, p3, bm->pals );
	}
#else
	a = hg->io.RegistTex( buf,bm->sx,bm->sy,p1,p2,p3 );
#endif
	if ( a<0 ) return a;
	return 0;
}

/*
EXPORT BOOL WINAPI settexfile( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		settex "filename",sw  (type$06)
	//
	char *buf;
	buf = (char *)bm->pBit;
	hg->io.RegistTex( p1,64,64,p2 );
	return 0;
}
*/

EXPORT BOOL WINAPI gettex( int *p1, int p2, int p3, int p4 )
{
	//
	//		gettex val  (type$01)
	//
	*p1 = hg->io.GetNextTexID();
	return 0;
}

/*-------------------------------------------------------*/

EXPORT BOOL WINAPI modelmovef( int p1, float p2, float p3, float p4 )
{
	//
	//		modelmovef id,x,y,z (type0)
	//
	VECTOR pos;
	SetVector( &pos, p2, p3, p4, 0.0f );
	hg->ModelOffset( p1, &pos );
	return 0;
}


EXPORT BOOL WINAPI modelshade( int p1, int p2, int p3, int p4 )
{
	//
	//		modelshade id,mode,sw (type0)
	//			( 0=no shade/1=simple shade/2=simple shade2/3=enhanced shade)
	//			( sw:0=normal calc/1=no calc )
	//
	int smode;
	hg->ModelSetShade( p1, p2 );
	if ( p3 == 0 ) {
		smode=0;
		if ( p2>=2 ) smode=1;
		hg->MakeModelNormals( p1, smode );
	}
	return 0;
}


EXPORT BOOL WINAPI dxfconv( char *p1, int p2, int p3, int p4 )
{
	//
	//		dxfconv val  (type$01)
	//
	int id;
	id = dxfConvHG( hg, p1 );
	if ( id<0 ) return 0;
	return -id;
}


EXPORT BOOL WINAPI dxfgetpoly( int *p1, int p2, int p3, int p4 )
{
	//
	//		dxfgetpoly val  (type$01)
	//
	*p1 = dxfGetNumPoly();
	return 0;
}


EXPORT BOOL WINAPI objscanf2( char *p1, float p2, float p3, float p4 )
{
	//
	//		objscanf2 res,x,y (type1)
	//
	char *p;
	p = (char *)hg->ProcessObjAll( 0, p2, p3, p4 );
	memcpy( p1, p, sizeof( SCANOBJ ) );
	return 0;
}


EXPORT BOOL WINAPI objscan2( char *p1, int p2, int p3, int p4 )
{
	//
	//		objscan2 res,x,y (type1)
	//
	char *p;
	p = (char *)hg->ProcessObjAll( 0, (float)p2, (float)p3, (float)p4 );
	memcpy( p1, p, sizeof( SCANOBJ ) );
	return 0;
}


EXPORT BOOL WINAPI mxsave( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		mxsave "file",model_id  (type$06)
	//
	int id;
	id = mxSaveHG( hg, p1, p2, mx_texlist );
	if ( id<0 ) return -1;
	return 0;
}

EXPORT BOOL WINAPI mxtex( char *p1, int p2, int p3, int p4 )
{
	//
	//		mxtex val  (type$01)
	//
	mx_texlist = p1;
	return 0;
}

EXPORT BOOL WINAPI setmfv( void *fv, int p1, int p2, int p3 )
{
	//
	//		setmfv fv,model_id,sub_id,type (type$01)
	//
	MODEL *m;
	VECTOR *v;
	m = hg->GetModelEx( p1, p2 );
	if ( m==NULL ) return -1;
	v = &m->pos;
	CopyVector( v+p3, (VECTOR *)fv );
	return 0;
}


EXPORT BOOL WINAPI getmfv( void *fv, int p1, int p2, int p3 )
{
	//
	//		getmfv fv,model_id,sub_id,type (type$01)
	//
	MODEL *m;
	VECTOR *v;
	m = hg->GetModelEx( p1, p2 );
	if ( m==NULL ) return -1;
	v = &m->pos;
	CopyVector( (VECTOR *)fv, v+p3 );
	return 0;
}


EXPORT BOOL WINAPI setmchild( int p1, int p2, int p3, int p4 )
{
	//
	//		setmchild model_id,sub_id, model_id (type$00)
	//
	hg->ArrangeModelTree( p1, p2, p3, 0 );
	return 0;
}


EXPORT BOOL WINAPI setmsibling( int p1, int p2, int p3, int p4 )
{
	//
	//		setmsibling model_id,sub_id, model_id (type$00)
	//
	hg->ArrangeModelTree( p1, p2, p3, 1 );
	return 0;
}


EXPORT BOOL WINAPI getmchild( int *p1, int p2, int p3, int p4 )
{
	//
	//		getmchild val, model_id,sub_id (type$00)
	//
	int id;
	MODEL *m;
	m = hg->GetModelEx( p2, p3 );
	if ( m==NULL ) return -1;
	id = hg->GetModelSubID( p2, (MODEL *)m->child );
	if ( id<0 ) { *p1 = 0; return -1; }
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI getmsibling( int *p1, int p2, int p3, int p4 )
{
	//
	//		getmsibling val, model_id,sub_id (type$00)
	//
	int id;
	MODEL *m;
	m = hg->GetModelEx( p2, p3 );
	if ( m==NULL ) return -1;
	id = hg->GetModelSubID( p2, (MODEL *)m->sibling );
	if ( id<0 ) { *p1 = 0; return -1; }
	*p1 = id;
	return 0;
}


EXPORT BOOL WINAPI getmodel( char *p1, int p2, int p3, int p4 )
{
	//
	//		getmodel val, model_id, sub_id (type$01)
	//
	MODEL *m;
	m = hg->GetModelEx( p2, p3 );
	if ( m==NULL ) return -1;
	memcpy( p1, m, sizeof(MODEL) );
	return 0;
}


EXPORT BOOL WINAPI putmodel( char *p1, int p2, int p3, int p4 )
{
	//
	//		putmodel val, model_id, sub_id (type$01)
	//
	MODEL *m;
	m = hg->GetModelEx( p2, p3 );
	if ( m==NULL ) return -1;
	memcpy( m, p1, sizeof(MODEL) );
	return 0;
}


EXPORT BOOL WINAPI dupnode( PVAL2 *p1, int p2, int p3, int p4 )
{
	//
	//		dupnode val, modelID, subID, nodeID (type$83)
	//
	char *p;
	p = (char *)hg->GetModelNode( hg->GetModelEx( p2,p3 ), p4 );
	if ( p == NULL ) return -1;
	p1->flag = 4;				// val
	p1->mode = 1;				// clone
	p1->len[1] = 0xffff;		// size
	p1->pt = p;					// PTR
	return 0;
}


EXPORT BOOL WINAPI gettree( char *p1, int p2, int p3, int p4 )
{
	//
	//		putmodel val, model_id (type$01)
	//
	hg->RebuildModelTree( p2, p1 );
	return 0;
}


EXPORT BOOL WINAPI getmuv( int *p1, int p2, int p3, int p4 )
{
	//
	//		getmuv val, modelid, nodeid (type1)
	//
	hg->GetModelUV( hg->GetModel( p2 ), p3, p1 );
	return 0;
}


EXPORT BOOL WINAPI setmuv( int *p1, int p2, int p3, int p4 )
{
	//
	//		setmuv val, modelid, nodeid (type1)
	//
	hg->SetModelUV( hg->GetModel( p2 ), p3, p1 );
	return 0;
}


EXPORT BOOL WINAPI nodemax( int *p1, int p2, int p3, int p4 )
{
	//
	//		nodemax val, modelid, sub_id (type1)
	//
	*p1 = hg->GetModelEx( p2, p3 )->num;
	return 0;
}


EXPORT BOOL WINAPI setmpoly( float *p1, int p2, int p3, int p4 )
{
	//
	//		setmpoly val, modelid, nodeid, mode (type1)
	//
	hg->SetModelVector( hg->GetModel( p2 ), p3, p4, p1 );
	return 0;
}


EXPORT BOOL WINAPI getmpoly( float *p1, int p2, int p3, int p4 )
{
	//
	//		getmpoly val, modelid, nodeid, mode (type1)
	//
	hg->GetModelVector( hg->GetModel( p2 ), p3, p4, p1 );
	return 0;
}


/*-------------------------------------------------------*/

EXPORT BOOL WINAPI modelscalef( int p1, float p2, float p3, float p4 )
{
	//
	//		modelscalef id,x,y,z (type0)
	//
	VECTOR scale;
	SetVector( &scale, p2, p3, p4, 0.0f );
	hg->ModelScale( p1, &scale );
	hg->RebuildModelTree( p1, NULL );
	return 0;
}

/*-------------------------------------------------------*/
