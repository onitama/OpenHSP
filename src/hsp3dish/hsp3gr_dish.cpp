
//
//	HSP3 dish graphics command
//	(GUI関連コマンド・関数処理)
//	onion software/onitama 2011/3
//
#ifdef HSPDISHGP
#ifdef HSPWIN
#include "win32gp/gamehsp.h"
#endif
#ifdef HSPNDK
#include "ndkgp/gamehsp.h"
#endif
#ifdef HSPIOS
#include "iosgp/gamehsp.h"
#endif
#ifdef HSPLINUX
#include "win32gp/gamehsp.h"
#endif
#ifdef HSPEMSCRIPTEN
#include "win32gp/gamehsp.h"
#endif
char *hsp3dish_getlog(void);		// for gameplay3d log
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"

#ifdef HSPWIN
#include <windows.h>
#include <direct.h>
#include <shlobj.h>
#endif

#include "../hsp3/hsp3code.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/strbuf.h"

#include "hsp3gr.h"
#include "hspwnd.h"
#include "hgio.h"
#include "supio.h"
#include "sysreq.h"
#include "webtask.h"

#ifdef HSPWIN
#include "win32/dxsnd.h"
#endif

#define USE_WEBTASK
#define USE_MMAN


/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HspWnd *wnd;
static Bmscr *bmscr;
static Bmscr *master_bmscr;
static HSPCTX *ctx;
static int *type;
static int *val;
static int cur_window;
static int ckey,cklast,cktrg;
static int msact;
static int dispflg;
static int sys_inst, sys_hwnd, sys_hdc;

extern int resY0, resY1;

#ifdef USE_MMAN
#ifdef HSPWIN
#include "win32/mmman.h"
#endif
#ifdef HSPIOS
#include "ios/mmman.h"
#endif
#ifdef HSPNDK
#include "ndk/mmman.h"
#endif
#ifdef HSPLINUX
#include "emscripten/mmman.h"
#endif
#ifdef HSPEMSCRIPTEN
#include "emscripten/mmman.h"
#endif

static MMMan *mmman;
#endif

static int dxsnd_flag;

#ifdef USE_WEBTASK
static WebTask *webtask;
#endif


/*----------------------------------------------------------*/
//					HGIMG4 system support
/*----------------------------------------------------------*/

#ifdef HSPDISHGP

extern gamehsp *game;
extern gameplay::Platform *platform;

#define CnvIntRot(val) ((float)val)*(PI2/256.0f)
#define CnvRotInt(val) ((int)(val*(256.0f/PI2)))

#define MOVEMODE_LINEAR 0
#define MOVEMODE_SPLINE 1
#define MOVEMODE_LINEAR_REL 2
#define MOVEMODE_SPLINE_REL 3

static float fp1,fp2,fp3,fp4;
static HSPREAL *p_vec;
static gameplay::Vector4 p_vec1;
static gameplay::Vector4 p_vec2;

static	HSPREAL dp1,dp2,dp3,dp4;

static int select_objid;
static int select_objmoc;

#endif


/*----------------------------------------------------------*/
//					HSP system support
/*----------------------------------------------------------*/

static void ExecFile( char *stmp, char *ps, int mode )
{
	//	外部ファイル実行
	hgio_exec( stmp, ps, mode );
}

static char *getdir( int id )
{
	//		dirinfo命令の内容をstmpに設定する
	//
	char *p;
#ifdef HSPWIN
	char *ss;
	char fname[HSP_MAX_PATH+1];
#endif
	p = ctx->stmp;

	*p = 0;

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
#ifdef HSPWIN
		_getcwd( p, _MAX_PATH );
#endif
		break;
	case 1:				//    HSPの実行ファイルがあるディレクトリ
#ifdef HSPWIN
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpath( fname, p, 32 );
#endif
		break;
	case 2:				//    Windowsディレクトリ
#ifdef HSPWIN
		GetWindowsDirectory( p, _MAX_PATH );
#endif
		break;
	case 3:				//    Windowsのシステムディレクトリ
#ifdef HSPWIN
		GetSystemDirectory( p, _MAX_PATH );
#endif
		break;
	case 4:				//    コマンドライン文字列
#ifdef HSPWIN
		ss = ctx->cmdline;
		sbStrCopy( &(ctx->stmp), ss );
		p = ctx->stmp;
		return p;
#endif
		break;
	case 5:				//    HSPTV素材があるディレクトリ
#ifdef HSPWIN
#if defined(HSPDEBUG)||defined(HSP3IMP)
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpath( fname, p, 32 );
		CutLastChr( p, '\\' );
		strcat( p, "\\hsptv\\" );
		return p;
#else
		*p = 0;
		return p;
#endif
#endif
		break;
	default:
#ifdef HSPWIN
		if ( id & 0x10000 ) {
			SHGetSpecialFolderPath( NULL, p, id & 0xffff, FALSE );
			break;
		}
#endif
		throw HSPERR_ILLEGAL_FUNCTION;
	}

#ifdef HSPWIN
	//		最後の'\\'を取り除く
	//
	CutLastChr( p, '\\' );
#endif
	return p;
}


static int sysinfo( int p2 )
{
	//		System strings get
	//
	int fl;
	char *p1;

	p1 = hgio_sysinfo( p2, &fl, ctx->stmp );
	if ( p1 == NULL ) {
		p1 = ctx->stmp;
		*p1 = 0;
		return HSPVAR_FLAG_INT;
	}
	return fl;
}


void *ex_getbmscr( int wid )
{
	Bmscr *bm;
	bm = wnd->GetBmscr( wid );
	return bm;
}

void ex_mref( PVal *pval, int prm )
{
	int t,size;
	void *ptr;
	const int GETBM=0x60;
	t = HSPVAR_FLAG_INT;
	size = 4;
	if ( prm >= GETBM ) {
		throw HSPERR_UNSUPPORTED_FUNCTION;
	} else {
		switch( prm ) {
		case 0x40:
			ptr = &ctx->stat;
			break;
		case 0x41:
			ptr = ctx->refstr;
			t = HSPVAR_FLAG_STR;
			size = 1024;
			break;
		case 0x44:
			ptr = ctx; size = sizeof(HSPCTX);
			break;
		default:
			throw HSPERR_UNSUPPORTED_FUNCTION;
		}
	}
	HspVarCoreDupPtr( pval, t, ptr, size );
}


/*------------------------------------------------------------*/
/*
		HSP Array support
*/
/*------------------------------------------------------------*/

#ifdef HSPDISHGP

static void code_getvec( gameplay::Vector4 *vec )
{
	vec->x = (float)code_getdd( 0.0 );
	vec->y = (float)code_getdd( 0.0 );
	vec->z = (float)code_getdd( 0.0 );
	vec->w = 1.0f;
}

static void code_getvec4(gameplay::Vector4 *vec)
{
	vec->x = (float)code_getdd(0.0);
	vec->y = (float)code_getdd(0.0);
	vec->z = (float)code_getdd(0.0);
	vec->w = (float)code_getdd(0.0);
}

static void code_setvec(HSPREAL *ptr, VECTOR *vec)
{
	ptr[0] = (HSPREAL)vec->x;
	ptr[1] = (HSPREAL)vec->y;
	ptr[2] = (HSPREAL)vec->z;
	ptr[3] = (HSPREAL)vec->w;
}

static void code_setivec( int *ptr, VECTOR *vec )
{
	ptr[0] = (int)vec->x;
	ptr[1] = (int)vec->y;
	ptr[2] = (int)vec->z;
	ptr[3] = (int)vec->w;
}

static HSPREAL *code_getvvec( void )
{
	PVal *pval;
	int size,inisize;
	HSPREAL dummy;
	HSPREAL *v;

	v = (HSPREAL *)code_getvptr( &pval, &size );
	dummy = (HSPREAL)0.0;
	if ( pval->flag != HSPVAR_FLAG_DOUBLE ) {
		code_setva( pval, 0, HSPVAR_FLAG_DOUBLE, &dummy );
	}
	inisize = pval->len[1];
	if ( inisize < 4 ) {
			pval->len[1] = 4;						// ちょっと強引に配列を拡張
			pval->size = 4 * sizeof(HSPREAL);
			code_setva( pval, 3, HSPVAR_FLAG_DOUBLE, &dummy );
			if ( inisize < 3 ) code_setva( pval, 2, HSPVAR_FLAG_DOUBLE, &dummy );
			if ( inisize < 2 ) code_setva( pval, 1, HSPVAR_FLAG_DOUBLE, &dummy );
	}
	v = (HSPREAL *)HspVarCorePtrAPTR( pval, 0 );

	return v;
}

static int *code_getivec( void )
{
	PVal *pval;
	int dummy;
	int size,inisize;
	int *v;

	v = (int *)code_getvptr( &pval, &size );
	dummy = 0;
	if ( pval->flag != HSPVAR_FLAG_INT ) {
		code_setva( pval, 0, HSPVAR_FLAG_INT, &dummy );
	}
	inisize = pval->len[1];
	if ( inisize < 4 ) {
			pval->len[1] = 4;						// ちょっと強引に配列を拡張
			pval->size = 4 * sizeof(int);
			code_setva( pval, 3, HSPVAR_FLAG_INT, &dummy );
			if ( inisize < 3 ) code_setva( pval, 2, HSPVAR_FLAG_INT, &dummy );
			if ( inisize < 2 ) code_setva( pval, 1, HSPVAR_FLAG_INT, &dummy );
	}
	v = (int *)HspVarCorePtrAPTR( pval, 0 );
	return v;
}



#endif

static int *code_getiv( void )
{
	//		変数パラメーターを取得(int,PDATポインタ)
	//
	PVal *pval;
	pval = code_getpval();
	if ( pval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
	return (int *)HspVarCorePtrAPTR( pval, 0 );
}

static int* code_getiv_sizecheck(int minsize)
{
	//		変数パラメーターを取得(int,PDATポインタ)(最低サイズを確認)
	//
	PVal* pval;
	pval = code_getpval();
	if (pval->flag != HSPVAR_FLAG_INT) throw HSPERR_TYPE_MISMATCH;
	if ( pval->len[1] < minsize ) throw HSPERR_TYPE_MISMATCH;
	return (int*)HspVarCorePtrAPTR(pval, 0);
}

static int *code_getiv2( PVal **out_pval )
{
	//		変数パラメーターを取得(PDATポインタ)(初期化あり)
	//
	PVal *pval;
	int *v;
	int size;
	int dummy;

	v = (int *)code_getvptr( &pval, &size );
	if ( pval->flag != HSPVAR_FLAG_INT ) {
		dummy = 0;
		code_setva( pval, 0, HSPVAR_FLAG_INT, &dummy );
		v = (int *)HspVarCorePtrAPTR( pval, 0 );
	}
	*out_pval = pval;
	return v;
}

static void code_setivlen( PVal *pval, int len )
{
	//		配列変数を拡張(intのみ)
	//
	int ilen;
	ilen = len;
	if ( ilen < 1 ) ilen = 1;
	pval->len[1] = ilen;						// ちょっと強引に配列を拡張
	pval->size = ilen * sizeof(int);
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static void cmdfunc_dialog( void )
{
	// dialog
	//int i;
	char *ptr;
	char *ps;
	char stmp[0x4000];
	int p1;
	ptr = code_getdsi( "" );
	strncpy( stmp, ptr, 0x4000-1 );
	p1 = code_getdi( 0 );
	ps = code_getds("");
	ctx->stat = hgio_dialog( p1, stmp, ps );
}


static int cmdfunc_extcmd( int cmd )
{
	//		cmdfunc : TYPE_EXTCMD
	//		(内蔵GUIコマンド)
	//
	int p1,p2,p3,p4,p5,p6;
	code_next();							// 次のコードを取得(最初に必ず必要です)
	switch( cmd ) {							// サブコマンドごとの分岐

	case 0x00:								// button
		{
		int i;
		char btnname[256];
		unsigned short *sbr;
		//Bmscr *bmsrc;

#ifndef HSPEMBED
		i = 0;
		if ( *type == TYPE_PROGCMD ) {
			i = *val;
			if ( i >= 2 ) throw HSPERR_SYNTAX;
			code_next();
		}
#else
		i = code_geti();
#endif
		strncpy( btnname, code_gets(), 255 );
		sbr = code_getlb();
		code_next();
		ctx->stat = bmscr->AddHSPObjectButton( btnname, i, (void *)sbr );
		p1 = bmscr->imgbtn;
		if ( p1 >= 0 ) {
			//bmsrc = wnd->GetBmscrSafe( p1 );
			bmscr->SetButtonImage( ctx->stat, p1, bmscr->btn_x1, bmscr->btn_y1, bmscr->btn_x2, bmscr->btn_y2, bmscr->btn_x3, bmscr->btn_y3 );
		}
		break;
		}

	case 0x02:								// exec
		{
		char *ps;
		char *fname;
		fname = code_stmpstr( code_gets() );
		p1 = code_getdi( 0 );
		ps = code_getds( "" );
		ExecFile( fname, ps, p1 );

        ctx->waitcount = 0;
        ctx->runmode = RUNMODE_WAIT;
        return RUNMODE_WAIT;
		}

	case 0x03:								// dialog
#ifdef HSPWIN
		cmdfunc_dialog();
		break;
#else
		ctx->waitcount = 0;
		ctx->runmode = RUNMODE_WAIT;
		cmdfunc_dialog();
		return ctx->runmode;
#endif

#ifdef USE_MMAN
	case 0x08:								// mmload
		{
		int i;
		char fname[HSP_MAX_PATH];
		strncpy( fname, code_gets(), HSP_MAX_PATH-1 );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
#if defined(HSPEMSCRIPTEN)
		p3 = code_getdi( 0 );
		p4 = code_getdi( 3600*10*1000 );
		i = mmman->Load( fname, p1, p2, p3, p4 );
#else
		i = mmman->Load( fname, p1, p2 );
#endif
		if (i) throw HSPERR_FILE_IO;
		break;
		}
	case 0x09:								// mmplay
		p1 = code_getdi( 0 );
		//mmman->SetWindow( bmscr->hwnd, bmscr->cx, bmscr->cy, bmscr->sx, bmscr->sy );
#if defined(HSPEMSCRIPTEN)
		p2 = code_getdi( -1 );
		mmman->Play( p1, p2 );
#else
		mmman->Play( p1 );
#endif
		break;

	case 0x0a:								// mmstop
		p1 = code_getdi( -1 );
		mmman->StopBank( p1 );
		break;
#endif


#if 0
	case 0x0d:								// pget
		p1 = code_getdi( bmscr->cx );
		p2 = code_getdi( bmscr->cy );
		bmscr->Pget( p1, p2 );
		break;
#endif

	case 0x0c:								// pset
		p1 = code_getdi( bmscr->cx );
		p2 = code_getdi( bmscr->cy );
		bmscr->Pset( p1, p2 );
		break;

	case 0x0f:								// mes,print
		{
		int chk;
		int sw,x,y;
		char *ptr;
		ptr = code_getdsi( "" );
		sw = code_getdi(0);
		strsp_ini();
		while(1) {
			chk = strsp_get( ptr, ctx->stmp, 0, 1022 );
			x = bmscr->cx; y = bmscr->cy;
			bmscr->Print( ctx->stmp );
			if ( chk == 0 ) break;
		}
		if ( sw ) {		// 改行しない
			bmscr->cx = x + bmscr->printsizex;
			bmscr->cy = y;
		}
		break;
		}
	case 0x10:								// title
		{
		char *p;
		p = code_gets();
		bmscr->Title( p );
		break;
		}
	case 0x11:								// pos
		bmscr->cx = code_getdi( bmscr->cx );
		bmscr->cy = code_getdi( bmscr->cy );
		break;

	case 0x12:								// circle
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( bmscr->sx );
		p4 = code_getdi( bmscr->sy );
		p5 = code_getdi( 1 );
		bmscr->Circle( p1,p2,p3,p4,p5 );
		break;
	case 0x13:								// cls
		p1 = code_getdi( 0 );
		bmscr->Cls( p1 );
		break;
	case 0x14:								// font
		{
		char fontname[256];
		strncpy( fontname, code_gets(), 255 );
		p1 = code_getdi( 12 );
		p2 = code_getdi( 0 );
		bmscr->SetFont( fontname, p1, p2 );
		break;
		}
	case 0x16:								// objsize
		p1 = code_getdi( 64 );
		p2 = code_getdi( 24 );
		p3 = code_getdi( 0 );
		bmscr->ox=p1;bmscr->oy=p2;bmscr->py=p3;
		break;

	case 0x17:								// picload
		{
		int wid;
		char fname[64];
		strncpy( fname, code_gets(), 63 );
		p1 = code_getdi( 0 );
		wid = bmscr->wid;
		wnd->Picload( wid, fname, p1 );
		//if ( i ) throw HSPERR_PICTURE_MISSING;
		//bmscr = wnd->GetBmscr( wid );
		//cur_window = wid;
		break;
		}
	case 0x18:								// color
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		bmscr->Setcolor(p1,p2,p3);
		break;
	case 0x1b:								// redraw
		{
		Bmscr *src;
		int bgtex;
		p1 = code_getdi( 1 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( 0 );
		if ( p1&1 ) {
			if (( p1 & 16 ) == 0 ) {
				bmscr->DrawAllObjects();	// オブジェクトを描画する
				bmscr->SetDefaultFont();	// フォントを元に戻す
#ifdef HSPWIN
				hgio_text_render();
#endif
			}
		} else {
			if ( p1 & 16 ) {
				bmscr->DrawAllObjects();	// オブジェクトを描画する
				bmscr->SetDefaultFont();	// フォントを元に戻す
#ifdef HSPWIN
				hgio_text_render();
#endif
				break;
			}
		}
		bgtex = GetSysReq( SYSREQ_CLSTEX );
		if ( bgtex >= 0 ) {
			src = wnd->GetBmscrSafe( bgtex );
		} else {
			src = NULL;
		}

#ifdef HSPLINUX
#ifdef HSPRASPBIAN
		if ((p1&1)==0) {
			if ( hgio_getkey( 27 ) ) {
				Alertf("[ESC] Abort\n");
				code_puterror( HSPERR_NONE );
				return RUNMODE_END;
			}
		}
#endif
#endif

		hgio_setback( (BMSCR *)src );
		ctx->stat = hgio_redraw( (BMSCR *)bmscr, p1 );
		break;
		}

	case 0x1c:								// width
		p1 = code_getdi( -1 );
		p2 = code_getdi( -1 );
		p3 = code_getdi( -1 );
		p4 = code_getdi( -1 );
		bmscr->Width( p1, p2, p3, p4, 1 );
		break;

	case 0x1d:								// gsel
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );

		bmscr = wnd->GetBmscrSafe( p1 );
		cur_window = p1;
		hgio_gsel((BMSCR *)bmscr);
		break;

	case 0x1e:								// gcopy
		{
		Bmscr *src;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = code_getdi( bmscr->gx );
		p5 = code_getdi( bmscr->gy );
		src = wnd->GetBmscrSafe( p1 );
		if ( bmscr->Copy( src, p2, p3, p4, p5 ) ) throw HSPERR_UNSUPPORTED_FUNCTION;
		break;
		}

	case 0x1f:								// gzoom
		{
		int p7,p8;
		Bmscr *src;
		p1 = code_getdi( bmscr->sx );
		p2 = code_getdi( bmscr->sy );
		p3 = code_getdi( 0 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( 0 );
		p6 = code_getdi( bmscr->gx );
		p7 = code_getdi( bmscr->gy );
		p8 = code_getdi( 0 );
		src = wnd->GetBmscrSafe( p3 );
		if ( bmscr->Zoom( p1, p2, src, p4, p5, p6, p7, p8 ) ) throw HSPERR_UNSUPPORTED_FUNCTION;
		break;
		}

	case 0x20:								// gmode
		p1 = code_getdi( 0 );
		p2 = code_getdi( 32 );
		p3 = code_getdi( 32 );
		p4 = code_getdi( 0 );
		//Alertf( "gmode %d,%d,%d,%d(%x)\n", p1, p2, p3, p4, bmscr );
		bmscr->gmode = p1;
		bmscr->gx = p2;
		bmscr->gy = p3;
		bmscr->gfrate = p4;
		//Alertf("OK");
		break;


	case 0x21:								// bmpsave
		if ( bmscr->BmpSave( code_gets() ) ) throw HSPERR_FILE_IO;
		break;

	case 0x22:								// hsvcolor
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		bmscr->SetHSVColor( p1, p2, p3 );
		break;

	case 0x23:								// getkey
		{
		PVal *pval;
		APTR aptr;
		aptr = code_getva( &pval );
		p1=code_getdi(1);
		p2 = 0;
#ifdef HSPWIN
		if ( code_event( HSPEVENT_GETKEY, p1, 0, &p2 ) == 0 ) {
			if ( GetAsyncKeyState(p1)&0x8000 ) p2=1;
		}
#endif
#ifdef HSPIOS
		if ( p1 == 1 ) {
			p2 = ( hgio_stick(0)&256 )>>8;
		}
#endif
#ifdef HSPNDK
		if ( p1 == 1 ) {
			p2 = ( hgio_stick(0)&256 )>>8;
		}
#endif
#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
		if ( hgio_getkey( p1 ) ) p2 = 1;
#endif
		code_setva( pval, aptr, TYPE_INUM, &p2 );
		break;
		}

	case 0x27:								// input (console)
		{
		PVal *pval;
		APTR aptr;
		char *pp2;
		char *vptr;
		int strsize;
		int a;
		strsize = 0;
		aptr = code_getva( &pval );
		//pp2 = code_getvptr( &pval, &size );
		p2 = code_getdi( 0x4000 );
		p3 = code_getdi( 0 );

		if ( p2 < 64 ) p2 = 64;
		pp2 = code_stmp( p2+1 );

		switch( p3 & 15 ) {
		case 0:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a==EOF ) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 1:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 2:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a == '\r' ) {
					int c = getchar();
					if( c != '\n' ) {
						ungetc(c, stdin);
					}
					break;
				}
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		}

		*pp2 = 0;
		ctx->strsize = strsize + 1;

		if ( p3 & 16 ) {
			if (( pval->support & HSPVAR_SUPPORT_FLEXSTORAGE ) == 0 ) throw HSPERR_TYPE_MISMATCH;
			//HspVarCoreAllocBlock( pval, (PDAT *)vptr, strsize );
			vptr = (char *)HspVarCorePtrAPTR( pval, aptr );
			memcpy( vptr, ctx->stmp, strsize );
		} else {
			code_setva( pval, aptr, TYPE_STRING, ctx->stmp );
		}
		break;
		}

	case 0x29:								// buffer
	case 0x2a:								// screen
	case 0x2b:								// bgscr
		{
		int p7,p8,typeval;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 640 );
		p3 = code_getdi( 480 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( -1 );
		p6 = code_getdi( -1 );
		p7 = code_getdi( p2 );
		p8 = code_getdi( p3 );

		if ( cmd == 0x29 ) {
			if ( p1 == 0 ) throw HSPERR_ILLEGAL_FUNCTION;
			typeval = HSPWND_TYPE_BUFFER;
			if (p4 & HSPWND_OPTION_OFFSCREEN) typeval = HSPWND_TYPE_OFFSCREEN;
			wnd->MakeBmscr(p1, typeval, p5, p6, p2, p3, p4);
		}
		else {
			if (p1 != 0) throw HSPERR_ILLEGAL_FUNCTION;
			bmscr = wnd->GetBmscr(p1);
			bmscr->sx = p2;
			bmscr->sx2 = p2;
			bmscr->sy = p3;
			bmscr->buffer_option = p4;
			bmscr->cx = p5;
			bmscr->cy = p6;
			bmscr->gx = p7;
			bmscr->gy = p8;
			if (cmd == 0x2b) {
				bmscr->buffer_option |= 0x10000;
			}
#ifdef HSPWIN
			ctx->runmode = RUNMODE_RESTART;
			return RUNMODE_RESTART;
#endif
		}
		bmscr = wnd->GetBmscr( p1 );
		cur_window = p1;
		hgio_gsel( (BMSCR *)bmscr );
		//
		//Alertf("screen---(%x)\n",bmscr);
		break;
		}

	case 0x2c:								// mouse
	{
#ifdef HSPWIN
		POINT pt;
		int setdef = 0;			// 既にマイナスの値か?
		GetCursorPos(&pt);
		if ((pt.x < 0) || (pt.x < 0)) {
			if (msact >= 0) setdef = 1;
		}
		p1 = code_getdi(pt.x);
		p2 = code_getdi(pt.y);
		p3 = code_getdi(setdef);
		if (p3 == 0) {
			if (msact >= 0) {
				if ((p1 < 0) || (p2 < 0)) {
					msact = ShowCursor(0);
					break;
				}
			}
		}

		SetCursorPos(p1, p2);

		if (p3 < 0) {
			msact = ShowCursor(0);
			break;
		}
		if (p3 > 0) {
			if (msact < 0) { msact = ShowCursor(1); }
		}
#else
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
#endif
		break;
	}

	case 0x2e:								// groll
	{
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		bmscr->SetScroll(p1, p2);
		break;
	}
	case 0x2f:								// line
		p1=code_getdi(0);
		p2=code_getdi(0);
		bmscr->cx=code_getdi(bmscr->cx);
		bmscr->cy=code_getdi(bmscr->cy);
		bmscr->Line( p1, p2 );
		break;

	case 0x30:								// clrobj
		p1 = code_getdi( 0 );
		p2 = code_getdi( -1 );
		p4 = bmscr->objmax-1;
		if ( p4 < 0 ) break;
		if (( p1<0 )|( p2>p4 )|( p1>p4 )) throw HSPERR_ILLEGAL_FUNCTION;
		if ( p2<0 ) p2 = p4;
		for( p3=p1; p3<=p2; p3++ ) {
			bmscr->DeleteHSPObject( p3 );
		}
		break;

	case 0x31:								// boxf
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( bmscr->sx );
		p4 = code_getdi( bmscr->sy );
		bmscr->Boxfill( p1, p2, p3, p4 );
		break;

	case 0x34:								// stick
		{
		PVal *pval;
		APTR aptr;
		int res;
		aptr = code_getva( &pval );
		p1 = code_getdi(0);
		p2 = code_getdi(1);

		res = 0;

		ckey = hgio_stick( p2 );
		cktrg = (ckey^cklast)&ckey;
		cklast = ckey;
		res = cktrg|(ckey&p1);
		code_setva( pval, aptr, TYPE_INUM, &res );
		break;
		}

	case 0x35:								// grect
		{
		double rot;
		p1 = code_getdi(0);				// パラメータ1:数値
		p2 = code_getdi(0);				// パラメータ2:数値
		rot = code_getdd(0.0);			// パラメータ5:数値
		p3 = code_getdi(bmscr->gx);		// パラメータ3:数値
		p4 = code_getdi(bmscr->gy);		// パラメータ4:数値
		bmscr->FillRot( p1, p2, p3, p4, (float)rot );
		break;
		}
	case 0x36:								// grotate
		{
		Bmscr *bm2;
		double rot;

		p1 = code_getdi(0);			// パラメータ1:数値
		p2 = code_getdi(0);			// パラメータ2:数値
		p3 = code_getdi(0);			// パラメータ3:数値
		rot = code_getdd(0.0);		// パラメータ4:数値
		p4 = code_getdi(bmscr->gx);	// パラメータ5:数値
		p5 = code_getdi(bmscr->gy);	// パラメータ6:数値

		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得
		bmscr->FillRotTex( p4, p5, (float)rot, bm2, p2, p3, bmscr->gx, bmscr->gy );
		break;
		}

	case 0x37:								// gsquare
		{
		Bmscr *bm2;
		int ep1;
		int *px;
		int *py;
		int *ptx;
		int *pty;

		ep1 = code_getdi(0);				// パラメータ1:数値
		px = code_getiv();
		py = code_getiv();

		if ( ep1 >= 0 ) {
			bm2 = wnd->GetBmscrSafe( ep1 );	// 転送元のBMSCRを取得
			ptx = code_getiv();
			pty = code_getiv();
		} else {
			bm2 = NULL;
			ptx = NULL;
			pty = NULL;
			if ( ep1 == -257 ) {
				ptx = code_getiv();
			}
		}
		bmscr->SquareTex( px, py, bm2, ptx, pty, ep1 );
		break;
		}

	case 0x38:								// gradf
		{
		int gradmode;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( bmscr->sx );
		p4 = code_getdi( bmscr->sy );
		gradmode = code_getdi( 0 );
		p5 = code_getdi( 0 );
		p6 = code_getdi( 0 );
		bmscr->GradFill( p1, p2, p3, p4, gradmode, p5, p6 );
		break;
		}
	case 0x39:								// objimage
		p1 = code_getdi( -1 );
		bmscr->imgbtn = p1;
		bmscr->btn_x1 = (short)code_getdi( 0 );
		bmscr->btn_y1 = (short)code_getdi( 0 );
		bmscr->btn_x2 = (short)code_getdi( 0 );
		bmscr->btn_y2 = (short)code_getdi( 0 );
		bmscr->btn_x3 = (short)code_getdi( bmscr->btn_x1 );
		bmscr->btn_y3 = (short)code_getdi( bmscr->btn_y1 );
		break;

	case 0x3a:								// objskip
		{
		p1=code_getdi(0);
		p2=code_getdi(2);
		bmscr->SetObjectMode( p1, p2 );
		break;
		}

	case 0x3b:								// objenable
		{
		p1=code_getdi(0);
		p2=code_getdi(1);
		bmscr->EnableObject( p1, p2 );
		break;
		}

	case 0x3c:								// celload
		{
		//int i;
		char fname[64];
		strncpy( fname, code_gets(), 63 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( 0 );
		if ( p1 < 0 ) p1 = wnd->GetEmptyBufferId();
		//Alertf( "celload[%s],%d,%d\n", fname, p1, p2 );

		wnd->MakeBmscrFromResource( p1, fname );
		//i = wnd->Picload( p1, fname, 0 );
		//if ( i ) throw HSPERR_PICTURE_MISSING;

		ctx->stat = p1;
		break;
		}
	case 0x3d:								// celdiv
		{
		Bmscr *bm2;
		p1=code_getdi(1);
		p2=code_getdi(0);
		p3=code_getdi(0);
		p4=code_getdi(0);
		p5=code_getdi(0);
		//Alertf("celdiv %d,%d,%d,%d,%d\n",p1,p2,p3,p4,p5);

		bm2 = wnd->GetBmscrSafe( p1 );
		bm2->SetCelDivideSize( p2, p3, p4, p5 );
		break;
		}
	case 0x3e:								// celput
		{
		Bmscr *bm2;
		HSPREAL zx,zy,rot;

		p1=code_getdi(1);
		p2=code_getdi(0);
		zx = code_getdd(1.0);
		zy = code_getdd(1.0);
		rot = code_getdd(0.0);
		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得

		if (( rot == 0.0 )&&( zx == 1.0 )&&( zy == 1.0 )) {
			//		変形なし
			bmscr->CelPut( bm2, p2 );
			break;
		}

		//	変形あり
		bmscr->CelPut( bm2, p2, (float)zx, (float)zy, (float)rot );
		break;
		}

	case 0x3f:								// gfilter
		p1=code_getdi(0);
		//	変形あり
		bmscr->SetFilter( p1 );
		break;
	case 0x40:								// setreq
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
#ifdef HSPDISHGP
		game->hookSetSysReq( p1, p2 );
#endif
		SetSysReq( p1, p2 );
		break;
	case 0x41:								// getreq
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
//		if ( p1 & 0x10000 ) {
//			code_setva( p_pval, p_aptr, HSPVAR_FLAG_STR, GetDebug() );
//			break;
//		}
#ifdef HSPDISHGP
		game->hookGetSysReq( p1 );
#endif
		p2 = GetSysReq( p1 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p2 );
		break;
		}

	case 0x42:								// mmvol
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		mmman->SetVol( p1, p2 );
		break;
	case 0x43:								// mmpan
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		mmman->SetPan( p1, p2 );
		break;
	case 0x44:								// mmstat
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = mmman->GetStatus( p1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p3 );
		break;
		}
	case 0x45:								// mtlist
		{
		int *p_ptr;
		int p_size;
		PVal *p_pval;
		p_ptr = code_getiv2( &p_pval );				// 変数ポインタ取得
		p_size = bmscr->listMTouch( p_ptr );		// マルチタッチリスト取得
		code_setivlen( p_pval, p_size );			// 要素数を設定
		ctx->stat = p_size;							// statに要素数を代入
		break;
		}
	case 0x46:								// mtinfo
		{
		int *p_ptr;
		HSP3MTOUCH *mt;
		PVal *p_pval;
		p_ptr = code_getiv2( &p_pval );				// 変数ポインタ取得
		p1 = code_getdi( 0 );
		mt = bmscr->getMTouch( p1 );
		code_setivlen( p_pval, 4 );					// 要素数を設定
		if ( mt ) {
			p_ptr[0] = mt->flag;
			p_ptr[1] = mt->x;
			p_ptr[2] = mt->y;
			p_ptr[3] = mt->pointid;
			ctx->stat = mt->flag;
		} else {
			p_ptr[0] = -1;
			p_ptr[1] = 0;
			p_ptr[2] = 0;
			p_ptr[3] = 0;
			ctx->stat = -1;
		}
		break;
		}
	case 0x47:								// devinfo
		{
		PVal *p_pval;
		APTR p_aptr;
		char *ps;
		char *s_res;
		int p_res;
		p_aptr = code_getva( &p_pval );
		ps = code_gets();
		p_res = 0;
		s_res = wnd->getDevInfo()->devinfo( ps );
		if ( s_res == NULL ) {
			p_res = -1;
		} else {
			code_setva( p_pval, p_aptr, TYPE_STRING, s_res );
		}
		ctx->stat = p_res;
		break;
		}
	case 0x48:								// devinfoi
		{
		PVal *p_pval;
		int *p_ptr;
		char *ps;
		int p_size;
		int *i_res;
		p_ptr = code_getiv2( &p_pval );				// 変数ポインタ取得
		ps = code_gets();
		i_res = wnd->getDevInfo()->devinfoi( ps, &p_size );
		if ( i_res == NULL ) {
			p_size = -1;
		} else {
			code_setivlen( p_pval, p_size );			// 要素数を設定
			memcpy( p_ptr, i_res, sizeof(int)*p_size );
		}
		ctx->stat = p_size;
		break;
		}
	case 0x49:								// devprm
		{
		char *ps;
		char prmname[256];
		int p_res;
		strncpy( prmname, code_gets(), 255 );
		ps = code_gets();
		p_res = wnd->getDevInfo()->devprm( prmname, ps );
		ctx->stat = p_res;
		break;
		}
	case 0x4a:								// devcontrol
		{
		char *cname;
		int p_res;
		cname = code_stmpstr( code_gets() );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p_res = wnd->getDevInfo()->devcontrol( cname, p1, p2, p3 );
		ctx->stat = p_res;
		break;
		}

#ifdef USE_WEBTASK
	case 0x4b:								// httpload
		{
		char *ss;
		char *ss_post;
		ss = code_stmpstr( code_gets() );			// パラメータ1:文字列
		ss_post = code_getds( "" );					// パラメータ2:文字列
		if ( *ss_post == 0 ) ss_post = NULL;
		ctx->stat = webtask->Request( ss, ss_post );
		break;
		}
	case 0x4c:								// httpinfo
		{
		PVal *pv;
		APTR ap;
		char *ss;
		char *dst;
		int size;
		ap = code_getva( &pv );					// パラメータ1:変数
		p1 = code_getdi( 0 );					// パラメータ2:数値
		if ( p1 & 16 ) {
			ss = webtask->getData( p1 );
			if ( p1 == HTTPINFO_DATA ) {
				//	結果データをバイナリで取得する
				if ( pv->flag != HSPVAR_FLAG_STR ) {
					code_setva( pv, ap, TYPE_STRING, "" );
				}
				size = webtask->getStatus( HTTPINFO_SIZE );
				dst = (char *)HspVarCorePtrAPTR( pv, ap );
				HspVarCoreAllocBlock( pv, (PDAT *)dst, size+1 );
				dst = (char *)HspVarCorePtrAPTR( pv, ap );
				memcpy( dst, ss, size );
				dst[size] = 0;
				webtask->setData( HTTPINFO_DATA, "" );	// 受信データを破棄する
				break;
			}
			code_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
		} else {
			p2 = webtask->getStatus( p1 );
			code_setva( pv, ap, HSPVAR_FLAG_INT, &p2 );	// 変数に値を代入
		}
		break;
		}
#endif

	case 0x4d:								// objcolor
		p1 = code_getdi( 255 );
		p2 = code_getdi( 255 );
		p3 = code_getdi( 255 );
		//bmscr->Setcolor2( RGB(p1,p2,p3) );
		break;
	case 0x4e:								// rgbcolor
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		bmscr->Setcolor((p1 >> 16) & 0xff, (p1>>8) & 0xff, p1 & 0xff );
		break;

	case 0x4f:								// viewcalc
		{
		HSPREAL dp1, dp2, dp3, dp4;
		p1 = code_getdi(0);
		dp1 = code_getdd(0.0);
		dp2 = code_getdd(0.0);
		dp3 = code_getdd(0.0);
		dp4 = code_getdd(0.0);
		p1 = bmscr->Viewcalc_set(p1, dp1, dp2, dp3, dp4);
		if (p1) throw HSPERR_ILLEGAL_FUNCTION;
		break;
		}

	case 0x5c:								// celbitmap
		{
		Bmscr* bm2;
		char *vptr;
		int needsize;
		p1 = code_getdi(0);
		bm2 = wnd->GetBmscrSafe(p1);	// 転送元のBMSCRを取得
		needsize = bm2->sx * bm2->sy;
		vptr = (char*)code_getiv_sizecheck(needsize);
		p2 = code_getdi(0);
		ctx->stat = bm2->BufferOp(p2, vptr);
		break;
		}

	case 0x5d:								// gmulcolor
		p1 = code_getdi( 255 );
		p2 = code_getdi( 255 );
		p3 = code_getdi( 255 );
		bmscr->SetMulcolor(p1,p2,p3);
		break;
	case 0x5e:								// setcls
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( -1 );
		hgio_clsmode( p1, p2, p3 );
		break;

	case 0x5f:								// celputm
		int *p_ptr1;
		int *p_ptr2;
		int *p_ptr3;
		int p_res;
		Bmscr *bm2;
		p_ptr1 = code_getiv();
		p_ptr2 = code_getiv();
		p_ptr3 = code_getiv();
		p1 = code_getdi( 0 );
		p2 = code_getdi( 1 );

		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得
		p_res = hgio_celputmulti( (BMSCR *)bmscr, p_ptr1, p_ptr2, p_ptr3, p2, (BMSCR *)bm2 );
		ctx->stat = p_res;
		break;

#ifdef HSPDISHGP

	case 0x60:								// gpreset
		p1 = code_getdi( 0 );
		game->resetScreen( p1 );
		break;
	case 0x61:								// gpdraw
		p1 = code_getdi( -1 );
		if ( p1 & GPDRAW_OPT_OBJUPDATE ) {
			game->updateAll();
		}
		hgio_draw_all(bmscr, p1);

		if ( p1 & GPDRAW_OPT_DRAW2D ) {
			hgio_draw_gpsprite( bmscr, false );
		}
		if ( p1 & GPDRAW_OPT_DRAW2D_LATE ) {
			hgio_draw_gpsprite( bmscr, true );
		}
		break;
	case 0x62:								// gpusescene
		p1 = code_getdi( 0 );
		game->selectScene( p1 );
		break;
	case 0x63:								// gpsetprm
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = game->setObjectPrm( p1, p2, p3 );
		if ( p4 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0x64:								// gpgetprm
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p4 = game->getObjectPrm( p1, p2, &p6 );
		if ( p4 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x65:								// gppostefx
		break;
	case 0x66:								// gpuselight
		p1 = code_getdi( 0 );
		p2 = game->selectLight( p1 );
		if (p2) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0x67:								// gpusecamera
		p1 = code_getdi( 0 );
		p2 = game->selectCamera( p1 );
		if (p2) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0x68:								// gpmatprm
		{
		char fname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi( 0 );
		ps = code_gets();
		strncpy( fname, ps, 256 );
		code_getvec( &p_vec1 );
		mat = game->getMat( p1 );
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setParameter(fname, (gameplay::Vector3 *)&p_vec1,-1);
		}
		else {
			ctx->stat = mat->setParameter(fname, (gameplay::Vector3 *)&p_vec1);
		}
		break;
		}
	case 0x69:								// gpmatstate
		{
		char fname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi( 0 );
		ps = code_gets();
		strncpy( fname, ps, 256 );
		ps = code_gets();
		mat = game->getMat( p1 );
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setState(fname, ps, -1);
		}
		else {
			ctx->stat = mat->setState(fname, ps);
		}
		break;
		}
	case 0x6a:								// gpviewport
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( bmscr->sx );
		p4 = code_getdi( bmscr->sy );
		game->updateViewport( p1, p2, p3, p4 );
		break;
	case 0x6b:								// setobjname
		{
		gameplay::Node *node;
		char *ps;
		p1 = code_getdi( 0 );
		ps = code_gets();
		node = game->getNode( p1 );
		if ( node == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		node->setId( (const char *)ps );
		break;
		}
	case 0x6c:								// getobjname
		{
		gameplay::Node *node;
		PVal *p_pval;
		APTR p_aptr;
		char *str;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		node = game->getNode( p1 );
		if ( node == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		str = (char *)node->getId();
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_STR, str );
		break;
		}
	case 0x6d:								// setborder
		{
		float x,y,z;
		gameplay::Vector3 v1;
		gameplay::Vector3 v2;
		code_getvec( &p_vec1 );
		p1 = code_getdi( 0 );
		switch( p1 ) {
		case 0:
			x = p_vec1.x * 0.5f;
			y = p_vec1.y * 0.5f;
			z = p_vec1.z * 0.5f;
			game->setBorder( -x, x, -y, y, -z, z );
			break;
		case 1:
			game->getBorder( &v1, &v2 );
			game->setBorder( p_vec1.x, v2.x, p_vec1.y, v2.y, p_vec1.z, v2.z );
			break;
		case 2:
			game->getBorder( &v1, &v2 );
			game->setBorder( v1.x, p_vec1.x, v1.y, p_vec1.y, v1.z, p_vec1.z );
			break;
		}
		break;
		}
	case 0x6e:								// findobj
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		game->findeObj( p1, p2 );
		break;
	case 0x6f:								// nextobj
		{
		PVal *p_pval;
		APTR p_aptr;
		gpobj *obj;
		p_aptr = code_getva( &p_pval );
		obj = game->getNextObj();
		if ( obj ) { p1 = obj->_id; } else { p1 = -1; }
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p1 );
		break;
		}

	case 0x70:								// gpdelobj
		p1 = code_getdi( 0 );
		game->deleteObjectID( p1 );
		break;
	case 0x71:								// gpcolormat
		{
		gameplay::Material *mat;
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( -1 );
		p2 = code_getdi( 0 );
		mat = game->makeMaterialColor( p1, p2 );
		if ( mat == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		p6 = game->makeNewMat(mat, GPMAT_MODE_3D, p1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x72:								// gptexmat
		{
		gameplay::Material *mat;
		char fname[HSP_MAX_PATH];
		char *ps;
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva(&p_pval);
		ps = code_gets();
		strncpy(fname, ps, HSP_MAX_PATH);
		p1 = code_getdi(0);
		mat = game->makeMaterialTexture(fname, p1,NULL);
		if (mat == NULL) throw HSPERR_ILLEGAL_FUNCTION;
		p6 = game->makeNewMat(mat, GPMAT_MODE_3D, -1, p1);
		code_setva(p_pval, p_aptr, HSPVAR_FLAG_INT, &p6);
		break;
		}
	case 0x73:								// gpusermat
		{
		gameplay::Material *mat;
		char vshname[HSP_MAX_PATH];
		char fshname[HSP_MAX_PATH];
		char defname[512];
		char *ps;
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		ps = code_getds( game->getUserVSH() );
		strncpy( vshname, ps, HSP_MAX_PATH );
		ps = code_getds(game->getUserFSH());
		strncpy( fshname, ps, HSP_MAX_PATH );
		ps = code_getds(game->getUserDefines());
		strncpy( defname, ps, 512 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( 0 );
		mat = game->makeMaterialFromShader( vshname, fshname, defname );
		if (mat == NULL) {
			//throw HSPERR_ILLEGAL_FUNCTION;
			p6 = -1;
		}
		else {
			game->setMaterialDefaultBinding(mat, p1, p2);
			p6 = game->makeNewMat(mat, GPMAT_MODE_3D, p1, p2);
		}
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x74:								// gpclone
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva(&p_pval);
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(-1);
		p6 = game->makeCloneNode(p1,p2,p3);
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x75:								// gpload
		{
		PVal *p_pval;
		APTR p_aptr;
		char fname[HSP_MAX_PATH];
		char objfname[512];
		char *ps;
		char *defname;
		p_aptr = code_getva( &p_pval );
		ps = code_gets();
		strncpy( fname, ps, HSP_MAX_PATH );
		ps = code_getds("");
		strncpy(objfname, ps, 512);
		defname = code_getds("");
		ps = objfname;
		if (*ps == 0) { ps = NULL; }
		p6 = game->makeModelNode( fname, ps, defname );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x76:								// gpplate
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		fp1 = (float)code_getdd( 1.0 );
		fp2 = (float)code_getdd( 1.0 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( -1 );
		p6 = game->makePlateNode( fp1, fp2, p1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x77:								// gpfloor
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		fp1 = (float)code_getdd( 1.0 );
		fp2 = (float)code_getdd( 1.0 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( -1 );
		p6 = game->makeFloorNode( fp1, fp2, p1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x78:								// gpbox
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		fp1 = (float)code_getdd( 1.0 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( -1 );
		p6 = game->makeBoxNode( fp1, p1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x79:								// gpspr
		{
		Bmscr *bm2;
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 3 );
		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得
		if ( bm2 == NULL ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		p6 = game->makeSpriteObj( p2, p3, bm2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0x7a:								// gplight
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		fp1 = (float)code_getdd( 1.0 );
		fp2 = (float)code_getdd( 0.5 );
		fp3 = (float)code_getdd( 1.0 );
		p6 = game->makeNewLgt( p1, p2, fp1, fp2, fp3 );
		if ( p6 < 0 ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		break;
	case 0x7b:								// setobjmode
		{
		gpobj *o;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		o = game->getSceneObj( p1 );
		if ( o == NULL ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		ctx->stat = o->_mode;
		switch(p3) {
		case 0:
			o->_mode |= p2;
			break;
		case 1:
			o->_mode &= ~p2;
			break;
		default:
			o->_mode = p2;
			break;
		}
		break;
		}
	case 0x7c:								// gplookat
		p1 = code_getdi( 0 );
		code_getvec( &p_vec1 );
		p6 = game->lookAtObject( p1, &p_vec1 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0x7d:								// gppbind
		p1 = code_getdi( 0 );
		fp1 = (float)code_getdd( 1.0 );
		fp2 = (float)code_getdd( 0.5 );
		ctx->stat = game->setObjectBindPhysics( p1, fp1, fp2 );
		break;
	case 0x7e:								// gpcamera
		p1 = code_getdi( 0 );
		fp1 = (float)code_getdd( 45.0 );
		fp2 = (float)code_getdd( 1.5 );
		fp3 = (float)code_getdd( 0.5 );
		fp4 = (float)code_getdd( 768.0 );
		p2 = code_getdi(0);
		p6 = game->makeNewCam( p1, fp1, fp2, fp3, fp4, p2 );
		if ( p6 < 0 ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		break;
	case 0x7f:								// gpnull
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p6 = game->makeNullNode();
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}


	case 0x80:								// getpos
	case 0x82:								// getscale
	case 0x83:								// getdir
	case 0x84:								// getefx
	case 0x85:								// getcolor
	case 0x86:								// getwork
	case 0x87:								// getwork2
	case 0x89:								// getang
	{
		PVal *pv1;
		PVal *pv2;
		PVal *pv3;
		APTR aptr1;
		APTR aptr2;
		APTR aptr3;
		gameplay::Vector4 v;
		HSPREAL dp1,dp2,dp3;
		p1 = code_getdi( 0 );
		aptr1 = code_getva( &pv1 );
		aptr2 = code_getva( &pv2 );
		aptr3 = code_getva( &pv3 );
		p6 = game->getObjectVector( p1, cmd - 0x80, &v );
		if ( p6 < 0 ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		dp1 = (HSPREAL)v.x;
		dp2 = (HSPREAL)v.y;
		dp3 = (HSPREAL)v.z;
		code_setva( pv1, aptr1, HSPVAR_FLAG_DOUBLE, &dp1 );
		code_setva( pv2, aptr2, HSPVAR_FLAG_DOUBLE, &dp2 );
		code_setva( pv3, aptr3, HSPVAR_FLAG_DOUBLE, &dp3 );
		break;
		}
	case 0x81:								// getquat
	{
		PVal *pv1;
		PVal *pv2;
		PVal *pv3;
		PVal *pv4;
		APTR aptr1;
		APTR aptr2;
		APTR aptr3;
		APTR aptr4;
		gameplay::Vector4 v;
		HSPREAL dp1, dp2, dp3, dp4;
		p1 = code_getdi(0);
		aptr1 = code_getva(&pv1);
		aptr2 = code_getva(&pv2);
		aptr3 = code_getva(&pv3);
		aptr4 = code_getva(&pv4);
		p6 = game->getObjectVector(p1, cmd - 0x80, &v);
		if (p6 < 0) code_puterror(HSPERR_ILLEGAL_FUNCTION);
		dp1 = (HSPREAL)v.x;
		dp2 = (HSPREAL)v.y;
		dp3 = (HSPREAL)v.z;
		dp4 = (HSPREAL)v.w;
		code_setva(pv1, aptr1, HSPVAR_FLAG_DOUBLE, &dp1);
		code_setva(pv2, aptr2, HSPVAR_FLAG_DOUBLE, &dp2);
		code_setva(pv3, aptr3, HSPVAR_FLAG_DOUBLE, &dp3);
		code_setva(pv4, aptr4, HSPVAR_FLAG_DOUBLE, &dp4);
		break;
	}

	case 0x90:								// getposi
	case 0x91:								// getquati
	case 0x92:								// getscalei
	case 0x93:								// getdiri
	case 0x94:								// getefxi
	case 0x95:								// getcolori
	case 0x96:								// getworki
	case 0x97:								// getwork2i
		{
		PVal *pv1;
		PVal *pv2;
		PVal *pv3;
		APTR aptr1;
		APTR aptr2;
		APTR aptr3;
		gameplay::Vector4 v;
		p1 = code_getdi( 0 );
		aptr1 = code_getva( &pv1 );
		aptr2 = code_getva( &pv2 );
		aptr3 = code_getva( &pv3 );
		p6 = game->getObjectVector( p1, cmd - 0x90, &v );
		if ( p6 < 0 ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		p1 = (int)v.x;
		p2 = (int)v.y;
		p3 = (int)v.z;
		code_setva( pv1, aptr1, HSPVAR_FLAG_INT, &p1 );
		code_setva( pv2, aptr2, HSPVAR_FLAG_INT, &p2 );
		code_setva( pv3, aptr3, HSPVAR_FLAG_INT, &p3 );
		break;
		}

	case 0xa0:								// selpos
	case 0xa1:								// selquat
	case 0xa2:								// selscale
	case 0xa3:								// seldir
	case 0xa4:								// selefx
	case 0xa5:								// selcolor
	case 0xa6:								// selwork
	case 0xa7:								// selwork2
		p1 = code_getdi( 0 );
		select_objid = p1;
		select_objmoc = cmd - 0xa0;
		break;

	case 0xb0:								// setpos
	case 0xb2:								// setscale
	case 0xb3:								// setdir
	case 0xb4:								// setefx
	case 0xb5:								// setcolor
	case 0xb6:								// setwork
	case 0xb7:								// setwork2
	case 0xb8:								// setaxang
	case 0xb9:								// setangx
	case 0xba:								// setangy
	case 0xbb:								// setangz
		p1 = code_getdi( 0 );
		code_getvec( &p_vec1 );
		p6 = game->setObjectVector( p1, cmd - 0xb0, &p_vec1 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0xb1:								// setquat
		p1 = code_getdi(0);
		code_getvec4(&p_vec1);
		p6 = game->setObjectVector(p1, cmd - 0xb0, &p_vec1);
		if (p6 < 0) throw HSPERR_ILLEGAL_FUNCTION;
		break;

	case 0xc0:								// addpos
	case 0xc1:								// addquat
	case 0xc2:								// addscale
	case 0xc3:								// adddir
	case 0xc4:								// addefx
	case 0xc5:								// addcolor
	case 0xc6:								// addwork
	case 0xc7:								// addwork2
	case 0xc8:								// addaxang
	case 0xc9:								// addangx
	case 0xca:								// addangy
	case 0xcc:								// addangz
		p1 = code_getdi( 0 );
		code_getvec( &p_vec1 );
		p6 = game->addObjectVector( p1, cmd - 0xc0, &p_vec1 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;

	case 0xd0:								// objset3
		code_getvec( &p_vec1 );
		p6 = game->setObjectVector( select_objid, select_objmoc, &p_vec1 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0xd1:								// objadd3
		code_getvec( &p_vec1 );
		p6 = game->addObjectVector( select_objid, select_objmoc, &p_vec1 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0xd2:								// objsetfv
		p_vec = code_getvvec();
		p_vec2.set((float)p_vec[0], (float)p_vec[1], (float)p_vec[2], (float)p_vec[3]);
		p6 = game->setObjectVector( select_objid, select_objmoc, &p_vec2 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0xd3:								// objaddfv
		p_vec = code_getvvec();
		p_vec2.set((float)p_vec[0], (float)p_vec[1], (float)p_vec[2], (float)p_vec[3]);
		p6 = game->addObjectVector( select_objid, select_objmoc, &p_vec2 );
		if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;
	case 0xd4:								// objgetfv
		{
		gameplay::Vector4 v;
		p_vec = code_getvvec();
		p6 = game->getObjectVector( select_objid, select_objmoc, &v );
		if ( p6 < 0 ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		code_setvec( p_vec, (VECTOR *)&v );
		break;
		}

	case 0xd5:								// setangr
	case 0xd6:								// addangr
		{
		Vector4 vec;
		p1 = code_getdi( 0 );
		vec.x = CnvIntRot( code_getdi( 0 ) );
		vec.y = CnvIntRot( code_getdi( 0 ) );
		vec.z = CnvIntRot( code_getdi( 0 ) );
		vec.w = 1.0f;

		if ( cmd == 0xd5 ) {
			p6 = game->setObjectVector( p1, MOC_ANGX, &vec );
			if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		} else {
			p6 = game->addObjectVector( p1, MOC_ANGX, &vec );
			if ( p6 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		}
		break;
		}
	case 0xd7:								// selmoc
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		select_objid = p1;
		select_objmoc = p2;
		break;
	case 0xd8:								// gpcnvaxis
		{
		PVal *pv1;
		PVal *pv2;
		PVal *pv3;
		APTR aptr1;
		APTR aptr2;
		APTR aptr3;
		gameplay::Vector3 v;
		gameplay::Vector3 v2;
		HSPREAL dp1, dp2, dp3;
		aptr1 = code_getva(&pv1);
		aptr2 = code_getva(&pv2);
		aptr3 = code_getva(&pv3);

		dp1 = code_getdd(0.0);
		dp2 = code_getdd(0.0);
		dp3 = code_getdd(0.0);
		p1 = code_getdi(0);

		v.set( dp1, dp2, dp3 );
		game->convertAxis( &v2, &v, p1 );
		//p6 = game->getObjectVector(p1, cmd - 0x80, &v);
		//if (p6 < 0) code_puterror(HSPERR_ILLEGAL_FUNCTION);

		dp1 = (HSPREAL)v2.x;
		dp2 = (HSPREAL)v2.y;
		dp3 = (HSPREAL)v2.z;
		code_setva(pv1, aptr1, HSPVAR_FLAG_DOUBLE, &dp1);
		code_setva(pv2, aptr2, HSPVAR_FLAG_DOUBLE, &dp2);
		code_setva(pv3, aptr3, HSPVAR_FLAG_DOUBLE, &dp3);
		break;
		}
	case 0xd9:								// getcoli
		{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		fp1 = (float)code_getdd( 1.0 );
		p2 = code_getdi( 0 );
		p6 = game->updateObjColi( p1, fp1, p2 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p6 );
		break;
		}
	case 0xda:								// setcoli
		{
		gpobj *obj;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		obj = game->getSceneObj( p1 );
		if ( obj == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		obj->_mygroup = p2;
		obj->_colgroup = p3;
		break;
		}
	case 0xdb:								// getobjcoli
		{
		PVal *p_pval;
		APTR p_aptr;
		gpobj *obj;
		p_aptr = code_getva( &p_pval );
		p1 = code_getdi( 0 );
		p2 = code_getdi(0);
		obj = game->getSceneObj( p1 );
		if ( obj == NULL ) code_puterror( HSPERR_ILLEGAL_FUNCTION );
		switch (p2) {
		case 0:
			p1 = obj->_mygroup;
			break;
		case 1:
			p1 = obj->_colgroup;
			break;
		case 2:
			p1 = obj->_rendergroup;
			break;
		case 3:
			p1 = obj->_lightgroup;
			break;
		default:
			code_puterror(HSPERR_ILLEGAL_FUNCTION);
			break;
		}
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p1 );
		break;
		}
	case 0xdc:								// objexist
		{
		gpobj *obj;
		p1 = code_getdi( 0 );
		p2 = 0;
		obj = game->getSceneObj( p1 );
		if ( obj == NULL ) p2 = -1;
		ctx->stat = p2;
		break;
		}


	case 0xe0:								// fvset
		p_vec = code_getvvec();
		code_getvec( &p_vec1 );
		code_setvec( p_vec, (VECTOR *)&p_vec1 );
		break;
	case 0xe1:								// fvadd
		p_vec = code_getvvec();
		code_getvec( &p_vec1 );
		p_vec2.set( (float)p_vec[0],(float)p_vec[1],(float)p_vec[2], 1.0f );
		p_vec2.add( p_vec1 );
		code_setvec( p_vec, (VECTOR *)&p_vec2 );
		break;
	case 0xe2:								// fvsub
		p_vec = code_getvvec();
		code_getvec( &p_vec1 );
		p_vec2.set( (float)p_vec[0],(float)p_vec[1],(float)p_vec[2], 1.0f );
		p_vec2.subtract( p_vec1 );
		code_setvec( p_vec, (VECTOR *)&p_vec2 );
		break;
	case 0xe3:								// fvmul
		p_vec = code_getvvec();
		code_getvec( &p_vec1 );
		p_vec2.set( (float)p_vec[0] * p_vec1.x,(float)p_vec[1] * p_vec1.y,(float)p_vec[2] * p_vec1.z, 1.0f );
		code_setvec( p_vec, (VECTOR *)&p_vec2 );
		break;
	case 0xe4:								// fvdiv
		p_vec = code_getvvec();
		code_getvec( &p_vec1 );
		p_vec2.set( (float)p_vec[0] / p_vec1.x,(float)p_vec[1] / p_vec1.y,(float)p_vec[2] / p_vec1.z, 1.0f );
		code_setvec( p_vec, (VECTOR *)&p_vec2 );
		break;
	case 0xe5:								// fvdir
		{
/*
		VECTOR v;
		VECTOR ang;
		p_vec = code_getvvec();
		code_getvec( &v );
		p1 = code_getdi( 0 );
		SetVector( &ang, (float)p_vec[0], (float)p_vec[1], (float)p_vec[2], 1.0f );
		InitMatrix();
		switch( p1 ) {
		case HGMODEL_ROTORDER_ZYX:
			RotZ( ang.z );
			RotY( ang.y );
			RotX( ang.x );
			break;
		case HGMODEL_ROTORDER_XYZ:
			RotX( ang.x );
			RotY( ang.y );
			RotZ( ang.z );
			break;
		case HGMODEL_ROTORDER_YXZ:
			RotY( ang.y );
			RotX( ang.x );
			RotZ( ang.z );
			break;
		}
		ApplyMatrix( &ang, &v );
		code_setvec( p_vec, &ang );
*/
		break;
		}
	case 0xe6:								// fvmin
		p_vec = code_getvvec();
		dp1 = code_getdd( 0.0 );
		dp2 = code_getdd( 0.0 );
		dp3 = code_getdd( 0.0 );
		if ( p_vec[0] < dp1 ) p_vec[0] = dp1;
		if ( p_vec[1] < dp2 ) p_vec[1] = dp2;
		if ( p_vec[2] < dp3 ) p_vec[2] = dp3;
		break;
	case 0xe7:								// fvmax
		p_vec = code_getvvec();
		dp1 = code_getdd( 0.0 );
		dp2 = code_getdd( 0.0 );
		dp3 = code_getdd( 0.0 );
		if ( p_vec[0] > dp1 ) p_vec[0] = dp1;
		if ( p_vec[1] > dp2 ) p_vec[1] = dp2;
		if ( p_vec[2] > dp3 ) p_vec[2] = dp3;
		break;
	case 0xe8:								// fvunit
		{
		VECTOR v;
		p_vec = code_getvvec();
		SetVector( &v, (float)p_vec[0], (float)p_vec[1], (float)p_vec[2], 1.0f );
		UnitVector( &v );
		code_setvec( p_vec, &v );
		break;
		}
	case 0xe9:								// fvouter
		{
		VECTOR v;
		VECTOR v1;
		gameplay::Vector4 v2;
		p_vec = code_getvvec();
		code_getvec( &v2 );
		SetVector( &v1, (float)p_vec[0], (float)p_vec[1], (float)p_vec[2], 1.0f );
		OuterProduct( &v, &v1, (VECTOR *)&v2 );
		code_setvec( p_vec, &v );
		break;
		}
	case 0xea:								// fvinner
		{
		VECTOR v;
		gameplay::Vector4 v2;
		p_vec = code_getvvec();
		code_getvec( &v2 );
		SetVector( &v, (float)p_vec[0], (float)p_vec[1], (float)p_vec[2], 1.0f );
		p_vec[0] = (double)InnerProduct( &v, (VECTOR *)&v2 );
		break;
		}
	case 0xeb:								// fvface
		{
		VECTOR v;
		gameplay::Vector4 v2;
		p_vec = code_getvvec();
		code_getvec( &v2 );
		SetVector( &v, (float)p_vec[0], (float)p_vec[1], (float)p_vec[2], 1.0f );
		GetTargetAngle( &v, &v, (VECTOR *)&v2 );
		code_setvec( p_vec, &v );
		break;
		}


	case 0xec:								// fv2str
		p_vec = code_getvvec();
		sprintf( ctx->refstr, "%f,%f,%f",p_vec[0],p_vec[1],p_vec[2] );
		break;
	case 0xed:								// f2str
		{
		PVal *p_pval;
		APTR p_aptr;
		char str[64];
		p_aptr = code_getva( &p_pval );
		dp1 = code_getdd( 0.0 );
		sprintf( str, "%f", dp1 );
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_STR, &str );
		break;
		}
	case 0xee:								// str2fv
		{
		VECTOR v;
		char *ps;
		p_vec = code_getvvec();
		ps = code_gets();
		sscanf( ps,"%f,%f,%f",&v.x,&v.y,&v.z );
		code_setvec( p_vec, &v );
		break;
		}
	case 0xef:								// str2f
		{
		float fp;
		char *ps;
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		ps = code_gets();
		sscanf( ps, "%f", &fp );
		dp1 = (double)fp;
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_DOUBLE, &dp1 );
		break;
		}


	case 0xf0:								// gppset
		{
		gpphy *phy;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		code_getvec( &p_vec1 );
		phy = game->getPhy( p1 );
		if ( phy == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		ctx->stat = phy->setParameter( p2, (Vector3 *)&p_vec1 );
		break;
		}

	case 0xf1:								// gpobjpool
		p1 = code_getdi( 0 );
		p2 = code_getdi( -1 );
		p3 = game->setObjectPool( p1, p2 );
		if ( p3 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;

	case 0xf2:								// gppapply
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		code_getvec( &p_vec1 );
		p3 = game->objectPhysicsApply( p1, p2, (Vector3 *)&p_vec1 );
		if ( p3 < 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		break;

	case 0xf3:								// setalpha
		{
		gpobj *obj;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 255 );
		obj = game->getObj( p1 );
		if ( obj == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
		if ( p2 < 0 ) p2 = 0;
		if ( p2 > 255 ) p2 = 255;
		obj->_transparent = p2;
		break;
		}
	case 0xf4:								// gpmatprm1
		{
		char fname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi( 0 );
		ps = code_gets();
		strncpy( fname, ps, 256 );
		dp1 = code_getdd( 0.0 );
		mat = game->getMat( p1 );
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setParameter(fname, dp1, -1);
		}
		else {
			ctx->stat = mat->setParameter(fname, dp1);
		}
		break;
		}
	case 0xf5:								// gpmatprm4
		{
		char fname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi( 0 );
		ps = code_gets();
		strncpy( fname, ps, 256 );
		code_getvec( &p_vec1 );
		dp1 = code_getdd( 0.0 );
		p_vec1.w = (float)dp1;
		mat = game->getMat(p1);
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setParameter(fname, &p_vec1, -1);
		}
		else {
			ctx->stat = mat->setParameter(fname, &p_vec1);
		}
		break;
		}
	case 0xf6:								// gpgetlog
	{
		PVal *p_pval;
		APTR p_aptr;
		char *ps;
		p_aptr = code_getva(&p_pval);
		ps = hsp3dish_getlog();
		code_setva(p_pval, p_aptr, HSPVAR_FLAG_STR, ps);
		break;
	}

	case 0xf7:								// gpaddanim
	{
		char name[256];
		char *ps;
		p1 = code_getdi(0);
		ps = code_gets();
		strncpy(name, ps, 256);
		p2 = code_getdi(0);
		p3 = code_getdi(-1);
		p4 = code_getdi(0);
		ctx->stat = game->addAnimId(p1, name, p2, p3, p4);
		break;
	}

	case 0xf8:								// gpgetanim
	{
		PVal *p_pval;
		APTR p_aptr;
		char *ps;
		p_aptr = code_getva(&p_pval);
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		if (p3 & 16){
			ps = game->getAnimId(p1,p2,p3&15);
			if (ps == NULL) {
				ctx->stat = -1;
			}
			else {
				ctx->stat = 0;
				code_setva(p_pval, p_aptr, HSPVAR_FLAG_STR, ps);
			}
		}
		else {
			int res = 0;
			ctx->stat = game->getAnimPrm(p1, p2, p3, &res);
			code_setva(p_pval, p_aptr, HSPVAR_FLAG_INT, &res);
		}
		break;
	}

	case 0xf9:								// gpsetanim
	{
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		p4 = code_getdi(0);
		ctx->stat = game->setAnimPrm(p1, p2, p3, p4);
		break;
	}

	case 0xfa:								// gpact
	{
		char name[256];
		char *ps;
		p1 = code_getdi(0);
		ps = code_getds("");
		strncpy(name, ps, 256);
		p2 = code_getdi(1);
		ctx->stat = game->playAnimId(p1, ps, p2);
		break;
	}

	case 0xfb:								// gpmatprm16
	{
		char fname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi(0);
		ps = code_gets();
		strncpy(fname, ps, 256);
		code_getvec(&p_vec1);
		p2 = code_getdi(1);
		mat = game->getMat(p1);
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setParameter(fname, (gameplay::Matrix *)&p_vec1, p2, -1);
		}
		else {
			ctx->stat = mat->setParameter(fname, (gameplay::Matrix *)&p_vec1, p2);
		}
		break;
	}

	case 0xfc:								// gpmatprmt
	{
		char fname[256];
		char texname[256];
		char *ps;
		gpmat *mat;
		p1 = code_getdi(0);
		ps = code_gets();
		strncpy(fname, ps, 256);
		ps = code_gets();
		strncpy(texname, ps, 256);
		p2 = code_getdi(0);
		mat = game->getMat(p1);
		if (mat == NULL) {
			gpobj *obj = game->getObj(p1);
			if (obj == NULL) throw HSPERR_ILLEGAL_FUNCTION;
			ctx->stat = obj->setParameter(fname, texname, p2, -1);
		}
		else {
			ctx->stat = mat->setParameter(fname, texname, p2);
		}
		break;
	}

	case 0xfd:								// gpusershader
	{
		char vsh[256];
		char fsh[256];
		char *ps;
		ps = code_gets();
		strncpy(vsh, ps, 256);
		ps = code_gets();
		strncpy(fsh, ps, 256);
		ps = code_gets();
		game->setUserShader2D(vsh, fsh, ps);
		break;
	}

	case 0xfe:								// gpgetmat
	{
		PVal *p_pval;
		APTR p_aptr;
		int res = -1;

		p_aptr = code_getva(&p_pval);
		p1 = code_getdi(0);
		p2 = code_getdi(0);

		switch (p2) {
		case 0:
		{
			gpobj *obj;
			obj = game->getObj(p1);
			if (obj) {
				res = obj->_usegpmat;
			}
			break;
		}
		case 1:
		{
			Bmscr *bm2;
			bm2 = wnd->GetBmscrSafe(p1);	// 転送元のBMSCRを取得
			if (bm2) {
				res = bm2->texid;
			}
			break;
		}
		case 2:
		{
			gpobj *obj;
			obj = game->getObj(p1);
			if (obj) {
				res = obj->_usegpmat;
			}
			break;
		}
		default:
			throw HSPERR_ILLEGAL_FUNCTION;
		}
		code_setva(p_pval, p_aptr, HSPVAR_FLAG_INT, &res);
		break;
	}
	case 0xff:								// gpreport
		p1 = code_getdi(0);
		game->pickupAll(p1);
		break;


	case 0x100:								// setevent
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( -1 );
		ctx->stat = game->AttachEvent(p1, p2, p3);
		break;
	case 0x101:								// delevent
		p1 = code_getdi( 0 );
		game->deleteEvent(p1);
		break;
	case 0x102:								// event_wait
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		ctx->stat = game->AddWaitEvent(p1, p2);
		break;
	case 0x103:								// event_uv
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		ctx->stat = game->AddUVEvent(p1, p2, p3);
		break;
	case 0x104:								// newevent
	{
		PVal *p_pval;
		APTR p_aptr;
		p_aptr = code_getva( &p_pval );
		p1 = game->GetEmptyEventId();
		code_setva( p_pval, p_aptr, HSPVAR_FLAG_INT, &p1 );
		break;
	}
	case 0x105:								// event_jump
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		ctx->stat = game->AddJumpEvent(p1, p2, p3);
		break;
	case 0x106:								// event_prmset
	case 0x107:								// event_prmon
	case 0x108:								// event_prmoff
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		ctx->stat = game->AddParamEvent( p1, cmd-0x106, p2, p3 );
		break;
	case 0x109:								// event_prmadd
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		p4 = code_getdi(0);
		p5 = code_getdi(255);
		ctx->stat = game->AddParamEvent2(p1, p2, p3, p4, p5);
		break;
	case 0x10a:								// event_suicide
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		ctx->stat = game->AddSuicideEvent(p1, p2);
		break;

	case 0x10b:								// event_aim
		break;
	case 0x10c:								// objaim
		break;
	case 0x10d:								// gpscrmat
	{
		gameplay::Material *mat;
		PVal *p_pval;
		APTR p_aptr;
		gameplay::Texture *opttex = NULL;
		p_aptr = code_getva(&p_pval);
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		Bmscr *bm = wnd->GetBmscrSafe(p1);
		gameplay::FrameBuffer *fb = (gameplay::FrameBuffer *)bm->master_buffer;
		if (fb) {
			gameplay::RenderTarget *target = fb->getRenderTarget();
			if (target) {
				opttex = target->getTexture();
			}
		}
		if (opttex == NULL) throw HSPERR_ILLEGAL_FUNCTION;
		p2 |= GPOBJ_MATOPT_USERBUFFER;
		mat = game->makeMaterialTexture("", p2, opttex);
		if (mat == NULL) throw HSPERR_ILLEGAL_FUNCTION;
		p6 = game->makeNewMat(mat, GPMAT_MODE_3D, -1, p2);
		code_setva(p_pval, p_aptr, HSPVAR_FLAG_INT, &p6);
		break;
	}
	case 0x10e:								// setobjrender
	{
		p1 = code_getdi(0);
		p2 = code_getdi(1);
		p3 = code_getdi(1);
		gpobj *obj = game->getSceneObj(p1);
		if (obj == NULL) code_puterror(HSPERR_ILLEGAL_FUNCTION);
		obj->_rendergroup = p2;
		obj->_lightgroup = p3;
		break;
	}
	case 0x10f:								// getangr
	{
		PVal* pv1;
		PVal* pv2;
		PVal* pv3;
		APTR aptr1;
		APTR aptr2;
		APTR aptr3;
		gameplay::Vector4 v;
		int ip1, ip2, ip3;
		p1 = code_getdi(0);
		aptr1 = code_getva(&pv1);
		aptr2 = code_getva(&pv2);
		aptr3 = code_getva(&pv3);
		p6 = game->getObjectVector(p1, MOC_ANGX, &v);
		if (p6 < 0) code_puterror(HSPERR_ILLEGAL_FUNCTION);
		ip1 = CnvRotInt(v.x);
		ip2 = CnvRotInt(v.y);
		ip3 = CnvRotInt(v.z);
		code_setva(pv1, aptr1, HSPVAR_FLAG_INT, &ip1);
		code_setva(pv2, aptr2, HSPVAR_FLAG_INT, &ip2);
		code_setva(pv3, aptr3, HSPVAR_FLAG_INT, &ip3);
		break;
	}

	case 0x110:								// event_pos
	case 0x111:								// event_quat
	case 0x112:								// event_scale
	case 0x113:								// event_dir
	case 0x114:								// event_efx
	case 0x115:								// event_color
	case 0x116:								// event_work
	case 0x117:								// event_work2
	case 0x118:								// event_axang
	case 0x119:								// event_angx
	case 0x11a:								// event_angy
	case 0x11b:								// event_angz
	case 0x11c:								// event_angr
		p1 = code_getdi(0);
		p2 = code_getdi( 10 );
		dp1 = code_getdd( 0.0 );
		dp2 = code_getdd( 0.0 );
		dp3 = code_getdd( 0.0 );
		//dp4 = code_getdd(0.0);
		dp4 = 1.0f;

		p6 = cmd - 0x110;
		if (cmd == 0x11c) {
			dp1 = CnvIntRot((int)dp1);
			dp2 = CnvIntRot((int)dp2);
			dp3 = CnvIntRot((int)dp3);
			p6 = MOC_ANGX;
		}
		p3 = code_getdi(MOVEMODE_LINEAR);
		switch( p3 & 15 ) {
		case MOVEMODE_LINEAR:
			ctx->stat = game->AddMoveEvent( p1, p6, (float)dp1, (float)dp2, (float)dp3, p2, 0 );
			break;
		case MOVEMODE_SPLINE:
			ctx->stat = game->AddSplineMoveEvent(p1, p6, (float)dp1, (float)dp2, (float)dp3, p2, 0);
			break;
		case MOVEMODE_LINEAR_REL:
			ctx->stat = game->AddMoveEvent(p1, p6, (float)dp1, (float)dp2, (float)dp3, p2, 1);
			break;
		case MOVEMODE_SPLINE_REL:
			ctx->stat = game->AddSplineMoveEvent(p1, p6, (float)dp1, (float)dp2, (float)dp3, p2, 1);
			break;
		}
		break;

	case 0x120:								// event_addpos
	case 0x121:								// event_addquat
	case 0x122:								// event_addscale
	case 0x123:								// event_adddir
	case 0x124:								// event_addefx
	case 0x125:								// event_addcolor
	case 0x126:								// event_addwork
	case 0x127:								// event_addwork2
		p1 = code_getdi(0);
		dp1 = code_getdd(0.0);
		dp2 = code_getdd(0.0);
		dp3 = code_getdd(0.0);
		ctx->stat = game->AddPlusEvent(p1, cmd - 0x120, (float)dp1, (float)dp2, (float)dp3);
		break;

	case 0x130:								// event_setpos
	case 0x131:								// event_setquat
	case 0x132:								// event_setscale
	case 0x133:								// event_setdir
	case 0x134:								// event_setefx
	case 0x135:								// event_setcolor
	case 0x136:								// event_setwork
	case 0x137:								// event_setwork2
	case 0x138:								// event_setaxang
	case 0x139:								// event_setangx
	case 0x13a:								// event_setangy
	case 0x13b:								// event_setangz
	{
		double dp4, dp5, dp6;
		p1 = code_getdi(0);
		dp1 = code_getdd(0.0);
		dp2 = code_getdd(0.0);
		dp3 = code_getdd(0.0);
		dp4 = code_getdd(dp1);
		dp5 = code_getdd(dp2);
		dp6 = code_getdd(dp3);
		ctx->stat = game->AddChangeEvent(p1, cmd - 0x130, (float)dp1, (float)dp2, (float)dp3, (float)dp4, (float)dp5, (float)dp6);
		break;
	}
	case 0x13c:								// event_setangr
	{
		double dp4, dp5, dp6;
		p1 = code_getdi(0);
		p2 = code_getdi(0);
		p3 = code_getdi(0);
		p4 = code_getdi(0);
		dp1 = CnvIntRot(p2);
		dp2 = CnvIntRot(p3);
		dp3 = CnvIntRot(p4);
		dp4 = CnvIntRot(code_getdi(p2));
		dp5 = CnvIntRot(code_getdi(p3));
		dp6 = CnvIntRot(code_getdi(p4));
		ctx->stat = game->AddChangeEvent(p1, MOC_ANGX, (float)dp1, (float)dp2, (float)dp3, (float)dp4, (float)dp5, (float)dp6);
		break;
	}


#endif

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


static int get_ginfo( int arg )
{
	//HDC hdc;
	//RECT rect;
	//POINT pt;
	//int i,j;

	//if (( arg>=4 )&&( arg<=11 )) GetWindowRect( bmscr->hwnd, &rect);

	switch( arg ) {
	case 0:
		//GetCursorPos(&pt);
		//return pt.x;
		return 0;
	case 1:
		//GetCursorPos(&pt);
		//return pt.y;
		return 0;
	case 2:
		return wnd->GetActive();
	case 3:
		return cur_window;
	case 4:
		//return rect.left;
	case 5:
		//return rect.top;
	case 6:
		//return rect.right;
	case 7:
		//return rect.bottom;
		return 0;
	case 8:
	case 9:
		//return bmscr->viewx;
		//return bmscr->viewy;
		return 0;
	case 10:
		//return rect.right - rect.left;
	case 11:
		//return rect.bottom - rect.top;
		return 0;
	case 12:
		//if ( bmscr->type != HSPWND_TYPE_BUFFER ) {
		//	bmscr->GetClientSize( &i, &j );
		//	return i;
		//}
	case 26:
		return bmscr->sx;
		//return hgio_getWidth();
		//return bmscr->sx;
	case 13:
		//if ( bmscr->type != HSPWND_TYPE_BUFFER ) {
		//	bmscr->GetClientSize( &i, &j );
		//	return j;
		//}
	case 27:
		return bmscr->sy;
		//return hgio_getHeight();
		//return bmscr->sy;
	case 14:
		return bmscr->printsizex;
	case 15:
		return bmscr->printsizey;
	case 16:
		//return GetRValue( bmscr->color );
		return ( (bmscr->color>>16)&0xff );
	case 17:
		//return GetGValue( bmscr->color );
		return ( (bmscr->color>>8)&0xff );
	case 18:
		//return GetBValue( bmscr->color );
		return ( (bmscr->color)&0xff );
	case 19:
//		hdc=GetDC(NULL);
//		i = 0;
//		if ( GetDeviceCaps( hdc,RASTERCAPS ) & RC_PALETTE ) i = 1;
//		ReleaseDC( NULL, hdc );
//		return i;
		return 0;
	case 20:
		return hgio_getWidth();
		//return GetSystemMetrics( SM_CXSCREEN );
	case 21:
		return hgio_getHeight();
		//return GetSystemMetrics( SM_CYSCREEN );
	case 22:
		return bmscr->cx;
	case 23:
		return bmscr->cy;
	case 24:
		//return ctx->intwnd_id;
		return 0;
	case 25:
		return wnd->GetEmptyBufferId();
	case 28:
	case 29:
	case 30:
	case 31:
		return 0;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return 0;
}


static int reffunc_intfunc_ivalue;
static HSPREAL reffunc_intfunc_dvalue;

static void *reffunc_function( int *type_res, int arg )
{
	int i;
	int p1;
	void *ptr;

	//		返値のタイプを設定する
	//
	*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
	ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != '(' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	switch( arg & 0xff ) {

	//	int function
	case 0x000:								// ginfo
		i = code_geti();
		if ( i < 0x100 ) {
			reffunc_intfunc_ivalue = get_ginfo( i );
		} else {
			reffunc_intfunc_dvalue = hgio_getinfo( i );
			ptr = &reffunc_intfunc_dvalue;
			*type_res = HSPVAR_FLAG_DOUBLE;
		}
		break;


	case 0x002:								// dirinfo
		p1 = code_geti();
#if defined(HSPLINUX)
		ptr = hgio_getdir( p1 );
#else
		ptr = getdir( p1 );
#endif
		*type_res = HSPVAR_FLAG_STR;
		break;

	case 0x003:								// sysinfo
		p1 = code_geti();
		*type_res = sysinfo( p1 );
		ptr = ctx->stmp;
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	return ptr;
}


static void *reffunc_sysvar( int *type_res, int arg )
{
	//		reffunc : TYPE_EXTSYSVAR
	//		(拡張システム変数)
	//
	void *ptr;
	if ( arg & 0x100 ) return reffunc_function( type_res, arg );

	//		返値のタイプを設定する
	//
	*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
	ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ

	switch( arg ) {

	//	int function
	case 0x000:								// mousex
		reffunc_intfunc_ivalue = bmscr->savepos[ BMSCR_SAVEPOS_MOSUEX ];
		break;
	case 0x001:								// mousey
		reffunc_intfunc_ivalue = bmscr->savepos[ BMSCR_SAVEPOS_MOSUEY ];
		break;
	case 0x002:								// mousew
		reffunc_intfunc_ivalue = bmscr->savepos[ BMSCR_SAVEPOS_MOSUEW ];
		bmscr->savepos[ BMSCR_SAVEPOS_MOSUEW ] = 0;
		break;
	case 0x003:								// hwnd
		//ptr = (void *)(&(bmscr->hwnd));
		reffunc_intfunc_ivalue = sys_hwnd;
		break;
	case 0x004:								// hinstance
		//ptr = (void *)(&(bmscr->hInst));
		reffunc_intfunc_ivalue = sys_inst;
		break;
	case 0x005:								// hdc
		//ptr = (void *)(&(bmscr->hdc));
		reffunc_intfunc_ivalue = sys_hdc;
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return ptr;
}


static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//		(内蔵GUI)
	//
#ifdef USE_MMAN
	delete mmman;
#endif
#ifdef USE_WEBTASK
	delete webtask;
#endif
	delete wnd;
	return 0;
}

void hsp3typeinit_extcmd( HSP3TYPEINFO *info )
{
	HSPEXINFO *exinfo;								// Info for Plugins

	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	wnd = new HspWnd();
	bmscr = wnd->GetBmscr( 0 );
	SetObjectEventNoticePtr( &ctx->stat );

	sys_inst = 0;
	sys_hwnd = 0;
	sys_hdc = 0;
	msact = 0;

#ifdef USE_MMAN
	mmman = new MMMan;
	mmman->Reset( ctx->wnd_parent );
#endif
#ifdef USE_WEBTASK
	webtask = new WebTask;
#endif

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		HSPEXINFOに関数を登録する
	//
	exinfo->actscr = &cur_window;					// Active Window ID
	exinfo->HspFunc_getbmscr = ex_getbmscr;
	exinfo->HspFunc_mref = ex_mref;

	//		バイナリモードを設定
	//
	//_setmode( _fileno(stdin),  _O_BINARY );
}

void hsp3typeinit_extfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_sysvar;
}

HSP3DEVINFO *hsp3extcmd_getdevinfo( void )
{
	return wnd->getDevInfo();
}

void hsp3notify_extcmd( void )
{
#ifdef USE_MMAN
	mmman->Notify();
#endif
}


void hsp3excmd_rebuild_window(void)
{
	if (wnd) delete wnd;
	wnd = new HspWnd();
	bmscr = wnd->GetBmscr(0);
}


void hsp3extcmd_pause( void )
{
#if defined(HSPNDK) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#ifdef USE_MMAN
	mmman->Pause();
#endif
#endif
}


void hsp3extcmd_resume( void )
{
#if defined(HSPNDK) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#ifdef USE_MMAN
	mmman->Resume();
	wnd->Resume();
	bmscr = wnd->GetBmscr( 0 );
#endif
#endif
}


void hsp3extcmd_sysvars(int inst, int hwnd, int hdc)
{
	sys_inst = inst;
	sys_hwnd = hwnd;
	sys_hdc = hdc;
}

