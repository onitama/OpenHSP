
//
//	HSP3 graphics command
//	(GUI関連コマンド・関数処理)
//	onion software/onitama 2004/6
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <tchar.h>
#include <direct.h>
#include <shlobj.h>

#include "../hspwnd.h"
#include "mmman.h"
#include "filedlg.h"
#include "../supio.h"
#include "../dpmread.h"
#include "../stack.h"
#include "../strbuf.h"
#include "fcpoly.h"

#include "../hsp3gr.h"
#include "../hsp3code.h"
#include "../hsp3debug.h"

#ifdef HSPUTF8
#pragma execution_character_set("utf-8")
#endif

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HspWnd *wnd;
static MMMan *mmman;
static Bmscr *bmscr;
static HSPCTX *ctx;
static int *type;
static int *val;
static int cur_window;
static int ckey,cklast,cktrg;
static int msact;
static int dispflg;

extern int resY0, resY1;				// "fcpoly.h"のパラメーター

#define GSQUARE_MODE_TEXTURE 0
#define GSQUARE_MODE_COLORFILL 1
#define GSQUARE_MODE_GRADFILL 2

/*----------------------------------------------------------*/
//					HSP system support
/*----------------------------------------------------------*/

void ExecFile( char *stmp, char *ps, int mode )
{
	int i,j;
	HSPAPICHAR *hactmp1 = 0;
	HSPAPICHAR *hactmp2 = 0;
	char *p;
	j=SW_SHOWDEFAULT;if (mode&2) j=SW_SHOWMINIMIZED;

	if ( *ps != 0 ) {
		SHELLEXECUTEINFO exinfo;
		memset( &exinfo, 0, sizeof(SHELLEXECUTEINFO) );
		exinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		exinfo.fMask = SEE_MASK_INVOKEIDLIST;
		exinfo.hwnd = bmscr->hwnd;
		exinfo.lpVerb = chartoapichar(ps,&hactmp1);
		exinfo.lpFile = chartoapichar(stmp,&hactmp2);
		exinfo.nShow = SW_SHOWNORMAL;
		if ( ShellExecuteEx( &exinfo ) == false ) {
					freehac(&hactmp1);
					freehac(&hactmp2);
					throw HSPERR_EXTERNAL_EXECUTE;
		}
		freehac(&hactmp1);
		freehac(&hactmp2);
		return;
	}
		
	if ( mode&16 ) {
		i = (int)(INT_PTR)ShellExecute( NULL,NULL,chartoapichar(stmp,&hactmp1),TEXT(""),TEXT(""),j );
		freehac(&hactmp1);
	}
	else if ( mode&32 ) {
		i = (int)(INT_PTR)ShellExecute( NULL,TEXT("print"),chartoapichar(stmp,&hactmp1),TEXT(""),TEXT(""),j );
		freehac(&hactmp1);
	}
	else {
		apichartohspchar(chartoapichar(stmp,&hactmp1),&p);
		freehac(&hactmp1);
		i=WinExec( p,j );
		freehc(&p);
		
	}
	if (i<32) throw HSPERR_EXTERNAL_EXECUTE;
}



/*
#define CSIDL_DESKTOP                   0x0000
#define CSIDL_INTERNET                  0x0001
#define CSIDL_PROGRAMS                  0x0002
#define CSIDL_CONTROLS                  0x0003
#define CSIDL_PRINTERS                  0x0004
#define CSIDL_PERSONAL                  0x0005
#define CSIDL_FAVORITES                 0x0006
#define CSIDL_STARTUP                   0x0007
#define CSIDL_RECENT                    0x0008
#define CSIDL_SENDTO                    0x0009
#define CSIDL_BITBUCKET                 0x000a
#define CSIDL_STARTMENU                 0x000b
#define CSIDL_DESKTOPDIRECTORY          0x0010
#define CSIDL_DRIVES                    0x0011
#define CSIDL_NETWORK                   0x0012
#define CSIDL_NETHOOD                   0x0013
#define CSIDL_FONTS                     0x0014
#define CSIDL_TEMPLATES                 0x0015
#define CSIDL_COMMON_STARTMENU          0x0016
#define CSIDL_COMMON_PROGRAMS           0X0017
#define CSIDL_COMMON_STARTUP            0x0018
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019
#define CSIDL_APPDATA                   0x001a
#define CSIDL_PRINTHOOD                 0x001b
#define CSIDL_ALTSTARTUP                0x001d         // DBCS
#define CSIDL_COMMON_ALTSTARTUP         0x001e         // DBCS
#define CSIDL_COMMON_FAVORITES          0x001f
#define CSIDL_INTERNET_CACHE            0x0020
#define CSIDL_COOKIES                   0x0021
#define CSIDL_HISTORY                   0x0022
*/

static char *getdir( int id )
{
	//		dirinfo命令の内容をstmpに設定する
	//
	char *p;
	TCHAR pw[_MAX_PATH+1];
	char *ss;
	TCHAR fname[_MAX_PATH+1];
	char *resp8;
	p = ctx->stmp;

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
		_tgetcwd( pw, _MAX_PATH );
		break;
	case 1:				//    HSPの実行ファイルがあるディレクトリ
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpathW( fname, pw, 32 );
		break;
	case 2:				//    Windowsディレクトリ
		GetWindowsDirectory( pw, _MAX_PATH );
		break;
	case 3:				//    Windowsのシステムディレクトリ
		GetSystemDirectory( pw, _MAX_PATH );
		break;
	case 4:				//    コマンドライン文字列
		ss = ctx->cmdline;
		sbStrCopy( &(ctx->stmp), ss );
		p = ctx->stmp;
		return p;
	case 5:				//    HSPTV素材があるディレクトリ
#if defined(HSPDEBUG)||defined(HSP3IMP)
		GetModuleFileName( NULL,fname,_MAX_PATH );
		apichartohspchar(fname,&resp8);
		getpath( resp8, p, 32 );
		freehc(&resp8);
		CutLastChr( p, '\\' );
		strcat( p, "\\hsptv\\" );
		return p;
#else
		p[0] = '\0';
		return p;
#endif
		break;
	default:
		if ( id & 0x10000 ) {
			SHGetSpecialFolderPath( NULL, pw, id & 0xffff, FALSE );
			break;
		}
		throw HSPERR_ILLEGAL_FUNCTION;
	}
	apichartohspchar(pw,&resp8);
	sbStrCopy( &(ctx->stmp),resp8);
	freehc(&resp8);
	p=ctx->stmp;
	//		最後の'\\'を取り除く
	//
	CutLastChr( p, '\\' );
	return p;
}


static int sysinfo( int p2 )
{
	//		System strings get
	//
	int fl;
	TCHAR pp[128];
	char *p3;
	BOOL success;
	DWORD version;
	DWORD size;
	DWORD *mss;
	SYSTEM_INFO si;
	MEMORYSTATUS ms;
	int plen;
	char *p;

	fl = HSPVAR_FLAG_INT;
	p3 = ctx->stmp;
	size = _MAX_PATH;

	if (p2&16) {
		GetSystemInfo(&si);
	}
	if (p2&32) {
		GlobalMemoryStatus(&ms);
		mss=(DWORD *)&ms;
		*(int *)p3 = (int)mss[p2&15];
		return fl;
	}

	switch(p2) {
	case 0:
		_tcscpy((TCHAR*)p3,TEXT("Windows"));
		version = GetVersion();
		if ((version & 0x80000000) == 0) _tcscat((TCHAR*)p3,TEXT("NT"));
									else _tcscat((TCHAR*)p3,TEXT("9X"));
/*
	rev 43
	mingw : warning : 仮引数int 実引数long unsigned
	に対処
*/
		_stprintf( pp,TEXT(" ver%d.%d"), static_cast< int >( version&0xff ), static_cast< int >( (version&0xff00)>>8 ) );
		_tcscat( (TCHAR*)p3, pp );
		apichartohspchar((TCHAR*)p3,&p);
		plen = strlen(p);
		if (p3 != p){
			memcpy(p3, p, plen);
			p3[plen] = '\0';
		}
		freehc(&p);
		fl=HSPVAR_FLAG_STR;
		break;
	case 1:
		success = GetUserName( (TCHAR*)p3,&size );
		apichartohspchar((TCHAR*)p3,&p);
		plen = strlen(p);
		if (p3 != p){
			memcpy(p3, p, plen);
			p3[plen] = '\0';
		}
		freehc(&p);
		fl = HSPVAR_FLAG_STR;
		break;
	case 2:
		success = GetComputerName((TCHAR*)p3, &size );
		apichartohspchar((TCHAR*)p3,&p);
		plen = strlen(p);
		if (p3 != p){
			memcpy(p3, p, plen);
			p3[plen] = '\0';
		}
		freehc(&p);
		fl = HSPVAR_FLAG_STR;
		break;
	case 16:
		*(int *)p3 = (int)si.dwProcessorType;
		break;
	case 17:
		*(int *)p3 = (int)si.dwNumberOfProcessors;
		break;
	default:
		throw HSPERR_ILLEGAL_FUNCTION;
	}
	return fl;
}


static int chgdisp( int mode, int sx, int sy )
{
	//		change display setting
	//			mode : 0=return to original
	//				   1=640x480 full-color mode
	//				   2=640x480 pal-mode
	DEVMODE dev;
	int a,c,f,sc,sel,res;
	int maxcol;
	if (mode) {
		sc=mode-1;
	} else {
		if (dispflg==0) return 1;
		ChangeDisplaySettings( NULL, 0 );
		dispflg = 0;
		return 0;
	}
	a=0;sel=-1;maxcol=0;
	while(1) {
		f=EnumDisplaySettings( NULL,a,&dev );
		if (f==0) break;
/*
	rev 43
	mingw : warning : 有符号型と無符号型の比較
	に対処
*/
		if ( static_cast< int >( dev.dmPelsWidth ) == sx )
		  if ( static_cast< int >( dev.dmPelsHeight ) == sy ) {
				c=(int)dev.dmBitsPerPel;
				if (sc) {
					if (c==8) sel=a;
				} else {
					if (c!=8) {
						if ( c>maxcol ) { maxcol=c;sel=a; }
					}
				}
		}
		a++;
	}
	if (sel==-1) return 1;
	EnumDisplaySettings( NULL,sel,&dev );
	dev.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
	ctx->stat = 0;

	res=ChangeDisplaySettings( &dev,CDS_TEST );
	if ( res == DISP_CHANGE_SUCCESSFUL ) {
		res=ChangeDisplaySettings( &dev, CDS_FULLSCREEN );
	} else {
		ctx->stat = 2;
		return 0;
	}
	dispflg++;
	return 0;
}


/*------------------------------------------------------------*/
/*
		for polygon process interface
*/
/*------------------------------------------------------------*/

#ifndef HSP_COMPACT

static POLY4 mem_poly4;

static void SetPolyAxis( POLY4 *poly, int num, int x, int y, int tx, int ty )
{
	IAXIS2 *iv;
	iv = &poly->v[num];
	iv->x = x;
	iv->y = y;
	iv->tx = tx;
	iv->ty = ty;
}


static void DrawSquareEx( int mode, int color, int attr, int alpha, int *x, int *y, int *texx, int *texy )
{
	//		自由変形スプライト
	//		mode : 0=fullcolor/1=palette
	//
	POLY4 *poly;
	int i,xx,yy,tx,ty;

	poly = (POLY4 *)&mem_poly4;
	for(i=0;i<4;i++) {
		xx = x[i]; yy = y[i];
		tx = texx[i];
		ty = texy[i];
		SetPolyAxis( poly, i, xx, yy, tx, ty );
	}
	poly->tex = 0;
	poly->attr = attr;
	poly->npoly = 1;
	poly->alpha = alpha;
	if ( color == -1 ) {
		poly->color = 0xffffff;
		if ( mode ) {
			DrawPolygonTexP( poly );
		} else {
			DrawPolygonTex( poly );
		}
	} else {
		poly->color = color;
		if ( mode ) {
			DrawPolygonF4P( poly );
		} else {
			DrawPolygonF4( poly );
		}
	}
}


static void DrawSpriteEx( int mode, int attr, int alpha, int x, int y, int sx, int sy, double ang, int tx0, int ty0, int tx1, int ty1, int opx, int opy )
{
	//		拡大回転スプライト
	//		mode : 0=fullcolor/1=palette
	//		(x,y)基点座標 / (sx,sy)サイズ / (opx,opy)オフセット / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	double xx,yy,x0,y0,x1,y1,ofsx,ofsy,mx0,mx1,my0,my1;

	poly = (POLY4 *)&mem_poly4;

	mx0=-(double)sin( ang );
	my0=(double)cos( ang );
	mx1 = -my0;
	my1 = mx0;

	ofsx = (double)-opx;
	ofsy = (double)-opy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	//		基点の算出
	xx = ( (double)x - (-x0+x1) );
	yy = ( (double)y - (-y0+y1) );

	/*-------------------------------*/

	//		回転座標の算出
	ofsx = (double)-sx;
	ofsy = (double)-sy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	/*-------------------------------*/

	iv = &poly->v[0];
	iv->x = (short)((-x0+x1) + xx);
	iv->y = (short)((-y0+y1) + yy);
	iv->tx = tx1;
	iv->ty = ty0;

	/*-------------------------------*/

	iv = &poly->v[1];
	iv->x = (short)((x1) + xx);
	iv->y = (short)((y1) + yy);
	iv->tx = tx1;
	iv->ty = ty1;

	/*-------------------------------*/

	iv = &poly->v[2];
	iv->x = (short)(xx);
	iv->y = (short)(yy);
	iv->tx = tx0;
	iv->ty = ty1;

	/*-------------------------------*/

	iv = &poly->v[3];
	iv->x = (short)((-x0) + xx);
	iv->y = (short)((-y0) + yy);
	iv->tx = tx0;
	iv->ty = ty0;

	/*-------------------------------*/

	poly->tex = 0;
	poly->attr = attr;
	poly->npoly = 1;
	poly->color = 0xffffff;
	poly->alpha = alpha;

	if ( mode ) {
		DrawPolygonTexP( poly );
	} else {
		DrawPolygonTex( poly );
	}
}


static void DrawRectEx( int mode, int color, int alpha, int x, int y, int sx, int sy, double ang )
{
	//		回転fill
	//		mode : 0=fullcolor/1=palette
	//		(x,y)中心座標 / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	double x0,y0,x1,y1,ofsx,ofsy;
	short xx,yy,tx0,ty0,tx1,ty1;

	poly = (POLY4 *)&mem_poly4;
	xx = (short)x;
	yy = (short)y;

	tx0 = 0;
	ty0 = 0;
	tx1 = sx-1; if ( tx1 < 0 ) tx1=0;
	ty1 = sy-1; if ( ty1 < 0 ) ty1=0;
	ofsx = (double)( tx1 - tx0 + 1 );
	ofsy = (double)( ty1 - ty0 + 1 );
	x0=-(double)sin( ang );
	y0=(double)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

	iv = &poly->v[0];
	iv->x = (short)(-x0+x1) + xx;
	iv->y = (short)(-y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty1;

	iv = &poly->v[1];
	iv->x = (short)(x0+x1) + xx;
	iv->y = (short)(y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty0;

	iv = &poly->v[2];
	iv->x = (short)(x0-x1) + xx;
	iv->y = (short)(y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty0;

	iv = &poly->v[3];
	iv->x = (short)(-x0-x1) + xx;
	iv->y = (short)(-y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty1;

	poly->tex = 0;
	poly->attr = 0;
	poly->npoly = 1;
	poly->alpha = alpha;
	poly->color = color;

	if ( mode ) {
		DrawPolygonF4P( poly );
	} else {
		DrawPolygonF4( poly );
	}
}


static int CnvRGB( int color )
{
	int res=0;
	res = color & 0x00ff00;
	res |= (color>>16) & 0xff;
	res |= (color & 0xff)<<16;
	return res;
}


static int GetAttrOperation( void )
{
	//		gmodeのモードをHGIMG互換のattr値に変換する
	//
	int attr;
	attr = 0;
	if ( bmscr->gmode == 2 ) { attr = NODE_ATTR_COLKEY; }
	if ( bmscr->gmode == 4 ) { attr = NODE_ATTR_COLKEY; SetPolyColorKey( bmscr->color ); }
	return attr;
}


static void GRotateSub( Bmscr *bm2, int x, int y, int sx, int sy, int sizex, int sizey, double rot, int opx, int opy )
{
	int tx0,ty0,tx1,ty1;
	int attr;

	SetPolyDest( bmscr->pBit, bmscr->sx, bmscr->sy );
	SetPolySource( bm2->pBit, bm2->sx, bm2->sy );

	tx0 = GetLimit( x, 0, bm2->sx );
	ty0 = GetLimit( y, 0, bm2->sy );
	tx1 = GetLimit( tx0 + sx - 1, 0, bm2->sx );
	ty1 = GetLimit( ty0 + sy - 1, 0, bm2->sy );
	ty0 = bm2->sy - 1 - ty0;
	ty1 = bm2->sy - 1 - ty1;
	attr = GetAttrOperation();
	DrawSpriteEx( bmscr->palmode, attr, bmscr->GetAlphaOperation(), bmscr->cx, bmscr->cy, sizex, sizey, rot, tx0, ty1, tx1, ty0, opx, opy );
	if ( resY0 >= 0 ) {
		bmscr->Send( 0, resY0, bmscr->sx, resY1-resY0+1 );
	}
}


static int *code_getiv( void )
{
	//		変数パラメーターを取得(PDATポインタ)
	//
	PVal *pval;
	pval = code_getpval();
	if ( pval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
	return (int *)HspVarCorePtrAPTR( pval, 0 );
}

#endif

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static void cmdfunc_dialog( void )
{
	// dialog
	int i;
	int p1,p2;
	char *ptr;
	char *ps;
	char stmp[0x4000];
	HSPAPICHAR *hactmp1 = 0;
	HSPAPICHAR *hactmp2 = 0;
	ptr = code_getdsi( "" );
	strncpy( stmp, ptr, 0x4000-1 );
	p1 = code_getdi( 0 );
	ps = code_getds("");
	p2 = code_getdi( 0 );

	if ( p1 >= 64 ) {
		return;
	}
	if (p1&16) {
		ctx->stat = fd_dialog( bmscr->hwnd, p1&3, stmp, ps );
			if ( ctx->stat == 0 ) {
				ctx->refstr[0] = 0;
			} else {
				strncpy( ctx->refstr, fd_getfname(), HSPCTX_REFSTR_MAX-1 );
			}
	}
	else if (p1&32) {
		i = (int)fd_selcolor( bmscr->hwnd,p1&1 );
		if ( i == -1 ) ctx->stat = 0;
		else {
			bmscr->color = i;
			ctx->stat = 1;
		}
	}
	else {
		i=0;
		if (p1&1) i|=MB_ICONEXCLAMATION; else i|=MB_ICONINFORMATION;
		if (p1&2) i|=MB_YESNO; else i|=MB_OK;
		ctx->stat = MessageBox( bmscr->hwnd, 
			chartoapichar(stmp,&hactmp1), chartoapichar(ps,&hactmp2), i );
		freehac(&hactmp1);
		freehac(&hactmp2);
	}
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
		Bmscr *bmsrc;

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
			bmsrc = wnd->GetBmscrSafe( p1 );
			bmscr->SetButtonImage( ctx->stat, p1, bmscr->btn_x1, bmscr->btn_y1, bmscr->btn_x2, bmscr->btn_y2, bmscr->btn_x3, bmscr->btn_y3 );
		}
		break;
		}

	case 0x01:								// chgdisp
		p1 = code_getdi( 0 );
		p2 = code_getdi( 640 );
		p3 = code_getdi( 480 );
		ctx->stat = chgdisp( p1, p2, p3 );
		break;

	case 0x02:								// exec
		{
		char *ps;
		char *fname;
		fname = code_stmpstr( code_gets() );
		p1 = code_getdi( 0 );
		ps = code_getds( "" );
		ExecFile( fname, ps, p1 );
		break;
		}

	case 0x03:								// dialog
		cmdfunc_dialog();
		break;

	case 0x08:								// mmload
		{
		int i;
		char fname[_MAX_PATH];
		strncpy( fname, code_gets(), _MAX_PATH-1 );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		i = mmman->Load( fname, p1, p2 );
		if (i) throw HSPERR_FILE_IO;
		break;
		}
	case 0x09:								// mmplay
		p1 = code_getdi( 0 );
//		if ( p1 < 0 ) {
//			ctx->stat = mmman->GetBusy();
//		} else {
			mmman->SetWindow( bmscr->hwnd, bmscr->cx, bmscr->cy, bmscr->sx, bmscr->sy );
			mmman->Play( p1 );
//		}
		break;

	case 0x0a:								// mmstop
		mmman->Stop();
		break;

	case 0x0b:								// mci
		ctx->stat = mmman->SendMCI( code_gets() );
		strncpy( ctx->refstr, mmman->GetMCIResult(), HSPCTX_REFSTR_MAX-1 );
		break;

	case 0x0c:								// pset
		p1 = code_getdi( bmscr->cx );
		p2 = code_getdi( bmscr->cy );
		bmscr->Pset( p1, p2 );
		break;

	case 0x0d:								// pget
		p1 = code_getdi( bmscr->cx );
		p2 = code_getdi( bmscr->cy );
		bmscr->Pget( p1, p2 );
		break;

	case 0x0e:								// syscolor
		p1 = code_getdi( 0 );
		bmscr->SetSystemcolor( p1 );
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
			bmscr->cx = x + bmscr->printsize.cx;
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
		ctx->stat = bmscr->Newfont( fontname, p1, p2, 0 );
		break;
		}
	case 0x15:								// sysfont
		p1 = code_getdi( 0 );
		bmscr->Sysfont( p1 );
		break;
	case 0x16:								// objsize
		p1 = code_getdi( 64 );
		p2 = code_getdi( 24 );
		p3 = code_getdi( 0 );
		bmscr->ox=p1;bmscr->oy=p2;bmscr->py=p3;
		break;

	case 0x17:								// picload
		{
		int i,wid;
		char fname[_MAX_PATH];
		strncpy( fname, code_gets(), _MAX_PATH-1 );
		p1 = code_getdi( 0 );
		wid = bmscr->wid;
		i = wnd->Picload( wid, fname, p1 );
		if ( i ) throw HSPERR_PICTURE_MISSING;
		bmscr = wnd->GetBmscr( wid );
		//cur_window = wid;
		break;
		}
	case 0x18:								// color
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		bmscr->Setcolor(p1,p2,p3);
		break;
	case 0x19:								// palcolor
		p1 = code_getdi( 0 );
		bmscr->SetPalcolor( p1 );
		break;
	case 0x1a:								// palette
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( 0 );
		if ( p1 >= 0 ) {
			bmscr->SetPalette( p1, p2, p3, p4 );
		}
		if ( p5 ) bmscr->UpdatePalette();
		break;

		break;
	case 0x1b:								// redraw
		p1 = code_getdi( 1 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( 0 );
		if (p1==0) p1=2;
		if (p1<2) {
			bmscr->fl_udraw=1;
			if ((p4==0)||(p5==0)) bmscr->Update();
			else {
				bmscr->Send( p2, p3, p4, p5 );
			}
		}
		bmscr->fl_udraw=p1&1;
		break;

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

		if (p2<0) {
			ShowWindow( bmscr->hwnd, SW_HIDE );
		}
		else if (p2>0) {
			HWND i;
			if (p2==1) i=HWND_NOTOPMOST; else i=HWND_TOPMOST;
			ShowWindow( bmscr->hwnd, SW_SHOW );
			SetActiveWindow( bmscr->hwnd );
			SetWindowPos( bmscr->hwnd, i, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
		}
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
		bmscr->gmode = p1;
		bmscr->gx = p2;
		bmscr->gy = p3;
		bmscr->gfrate = p4;
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

		if ( p1 == VK_LBUTTON || p1 == VK_RBUTTON ) {
			if ( GetSystemMetrics(SM_SWAPBUTTON) ) { p1 ^= VK_LBUTTON | VK_RBUTTON; }
		}

		if ( code_event( HSPEVENT_GETKEY, p1, 0, &p2 ) == 0 ) {
			if ( GetAsyncKeyState(p1)&0x8000 ) p2=1; else p2=0;
		}
		code_setva( pval, aptr, TYPE_INUM, &p2 );
		break;
		}

	case 0x25:								// chkbox
		{
		char name[256];
		PVal *pval;
		APTR aptr;
		strncpy( name, code_gets(), 255 );
		aptr = code_getva( &pval );
		if ( pval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		ctx->stat = bmscr->AddHSPObjectCheckBox( name, pval, aptr );
		break;
		}

	case 0x24:								// listbox
	case 0x26:								// combox
		{
		PVal *pval;
		APTR aptr;
		char *p;
		aptr = code_getva( &pval );
		if ( pval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		p1 = code_getdi( 100 );
		p = code_gets();
		ctx->stat = bmscr->AddHSPObjectMultiBox( pval, aptr, p1, p, cmd==0x26 );
		break;
		}

	case 0x27:								// input
		{
		PVal *pval;
		APTR aptr;
		char *ptr;
		int type,size;
		aptr = code_getva( &pval );

		p1 = code_getdi( bmscr->ox );
		p2 = code_getdi( bmscr->oy );
		size = 32;
		type = pval->flag;
		ptr = (char *)HspVarCorePtrAPTR( pval, aptr );
		if ( type == TYPE_STRING ) {
			ptr = (char *)HspVarCoreGetBlockSize( pval, (PDAT *)ptr, &size );
		} else {
			ptr = (char *)HspVarCoreCnv( pval->flag, HSPVAR_FLAG_STR, ptr );	// 文字列に変換
		}
		p3 = code_getdi( size );
		ctx->stat = bmscr->AddHSPObjectInput( pval, aptr, p1, p2, ptr, p3, type );
		break;
		}

	case 0x28:								// mesbox
		{
		PVal *pval;
		APTR aptr;
		char *ptr;
		int i;
		int size;

		aptr = code_getva( &pval );
		if ( pval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
		ptr = (char *)HspVarCoreGetBlockSize( pval, HspVarCorePtrAPTR( pval, aptr ), &size );
		p1=code_getdi(bmscr->ox);
		p2=code_getdi(bmscr->oy);
		p3=code_getdi( 1 );
		p4=code_getdi( -1 );
		i = HSPOBJ_INPUT_MULTILINE;
		if ((p3&1)==0) i |= HSPOBJ_INPUT_READONLY;
		if (p3&4) i |= HSPOBJ_INPUT_HSCROLL;
		if ( p4 < 0 ) p4 = size-1;
		ctx->stat = bmscr->AddHSPObjectInput( pval, aptr, p1, p2, ptr, p4, (pval->flag)|i );
		break;
		}

	case 0x29:								// buffer
	case 0x2a:								// screen
	case 0x2b:								// bgscr
		{
		int p7,p8,t;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 640 );
		p3 = code_getdi( 480 );
		p4 = code_getdi( 0 );
		p5 = code_getdi( -1 );
		p6 = code_getdi( -1 );
		p7 = code_getdi( p2 );
		p8 = code_getdi( p3 );

#ifdef HSP3IMP
		if ( p1 == 0 ) {
			bmscr = wnd->GetBmscr( p1 );
			cur_window = p1;
			break;
		}
#endif
		if ( cmd == 0x29 ) {
			t = HSPWND_TYPE_BUFFER;
		} else if ( cmd == 0x2b ) {
			t = HSPWND_TYPE_BGSCR;
		} else {
			t = HSPWND_TYPE_MAIN;
		}
		wnd->MakeBmscr( p1, t, p5, p6, p7, p8, p2, p3, p4 );
		bmscr = wnd->GetBmscr( p1 );
		bmscr->Width( p7, p8, p5, p6, 1 );

		cur_window = p1;

		if ( bmscr->type == HSPWND_TYPE_BUFFER ) break;

		ctx->waitcount = 0;
		ctx->waittick = -1;
		ctx->runmode = RUNMODE_AWAIT;
		return RUNMODE_AWAIT;
		}

	case 0x2c:								// mouse
		{
		POINT pt;
		GetCursorPos(&pt);
		p1 = code_getdi( pt.x );
		p2 = code_getdi( pt.y );
		if ((p1<0)||(p2<0)) {
			msact = ShowCursor(0);
			break;
		}
		SetCursorPos( p1, p2 );
		if ( msact < 0 ) { msact = ShowCursor(1); }
		break;
		}

	case 0x2d:								// objsel
		p1 = code_getdi( 0 );
		ctx->stat = bmscr->ActivateHSPObject( p1 );
		break;

	case 0x2e:								// groll
		p1=code_getdi(0);
		p2=code_getdi(0);
		bmscr->SetScroll( p1, p2 );
		break;

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

	case 0x32:								// objprm
		p1=code_getdi(0);
		if ( code_get() <= PARAM_END ) throw HSPERR_NO_DEFAULT;
		bmscr->UpdateHSPObject( p1, mpval->flag, mpval->pt );
		break;
	case 0x33:								// objmode (ver2.5 enhanced )
		p1=code_getdi(0);
		p2=code_getdi( bmscr->tabmove );
		bmscr->objmode = p1;
		bmscr->tabmove = p2;
		break;

	case 0x34:								// stick
		{
		PVal *pval;
		APTR aptr;
		int res;
		aptr = code_getva( &pval );
		p1 = code_getdi(0);
		p2 = code_getdi(1);

		ckey = 0; res = 0;
		if (p2) {
			if ( wnd->GetActive() < 0 ) {
				code_setva( pval, aptr, TYPE_INUM, &res );
				break;
			}
		}
		static int stick_keys[] = {
			VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SPACE, VK_RETURN,
			VK_CONTROL, VK_ESCAPE,
			(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON),
			(GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON),
			VK_TAB
		};

		for ( size_t i = 0; i < sizeof(stick_keys) / sizeof(int); i++ ) {
			if ( GetAsyncKeyState(stick_keys[i]) & 0x8000 ) { ckey |= 1 << i; }
		}
		cktrg = (ckey^cklast)&ckey;
		cklast = ckey;
		res = cktrg|(ckey&p1);
		code_setva( pval, aptr, TYPE_INUM, &res );
		break;
		}

#ifndef HSP_COMPACT
	case 0x35:								// grect
		{
		double rot;
		p1 = code_getdi(0);				// パラメータ1:数値
		p2 = code_getdi(0);				// パラメータ2:数値
		rot = code_getdd(0.0);			// パラメータ5:数値
		p3 = code_getdi(bmscr->gx);		// パラメータ3:数値
		p4 = code_getdi(bmscr->gy);		// パラメータ4:数値
		SetPolyDest( bmscr->pBit, bmscr->sx, bmscr->sy );
		if ( bmscr->palmode ) p6 = bmscr->palcolor; else p6 = CnvRGB( bmscr->color );
		DrawRectEx( bmscr->palmode, p6, bmscr->GetAlphaOperation(), p1, p2, p3, p4, rot );
		if ( resY0 >= 0 ) {
			bmscr->Send( 0, resY0, bmscr->sx, resY1-resY0+1 );
		}
		break;
		}
	case 0x36:								// grotate
		{
		Bmscr *bm2;
		double rot;

		p1 = code_getdi(0);			// パラメータ1:数値
		p2 = code_getdi(0);			// パラメータ2:数値
		p3 = code_getdi(0);			// パラメータ3:数値
		rot = code_getdd(0.0);		// パラメータ6:数値
		p4 = code_getdi(bmscr->gx);	// パラメータ4:数値
		p5 = code_getdi(bmscr->gy);	// パラメータ5:数値

		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得
		if ( bmscr->palmode != bm2->palmode ) throw HSPERR_ILLEGAL_FUNCTION;
#if 0
		int tx0,ty0,tx1,ty1;
		int attr;
		SetPolyDest( bmscr->pBit, bmscr->sx, bmscr->sy );
		SetPolySource( bm2->pBit, bm2->sx, bm2->sy );

		tx0 = GetLimit( p2, 0, bm2->sx );
		ty0 = GetLimit( p3, 0, bm2->sy );
		tx1 = GetLimit( tx0+bmscr->gx-1, 0, bm2->sx );
		ty1 = GetLimit( ty0+bmscr->gy-1, 0, bm2->sy );
		ty0 = bm2->sy - 1 - ty0;
		ty1 = bm2->sy - 1 - ty1;
		attr = GetAttrOperation();
		DrawSpriteEx( bmscr->palmode, attr, bmscr->GetAlphaOperation(), bmscr->cx, bmscr->cy, p4, p5, rot, tx0, ty1, tx1, ty0, p4, p5 );
		if ( resY0 >= 0 ) {
			bmscr->Send( 0, resY0, bmscr->sx, resY1-resY0+1 );
		}
#else
		GRotateSub( bm2, p2, p3, bmscr->gx, bmscr->gy, p4, p5, rot, p4/2, p5/2 );
#endif
		break;
		}
	case 0x37:								// gsquare
		{
		Bmscr *bm2;
		int ep1,i;
		int attr;
		int tmp_x[4], tmp_y[4], tmp_tx[4], tmp_ty[4];
		int color;
		int *px;
		int *py;
		int *ptx;
		int *pty;
		int sqmode;

		bm2 = NULL;
		ep1 = code_getdi(0);				// パラメータ1:数値
		px = code_getiv();
		py = code_getiv();

		sqmode = GSQUARE_MODE_TEXTURE;

		if ( ep1 >= 0 ) {
			bm2 = wnd->GetBmscrSafe( ep1 );	// 転送元のBMSCRを取得
			if ( bmscr->palmode != bm2->palmode ) throw HSPERR_ILLEGAL_FUNCTION;
			SetPolySource( bm2->pBit, bm2->sx, bm2->sy );
			color = -1;
			ptx = code_getiv();
			pty = code_getiv();
		} else {
			if ( ep1 <= -257 ) {			// グラデーションモード
				ptx = code_getiv();
				bmscr->GradFillEx( px, py, ptx );
				break;
			} else {
				if ( bmscr->palmode ) { color = (- ep1 ) - 1; } else { color = CnvRGB( bmscr->color ); }
				sqmode = GSQUARE_MODE_COLORFILL;
			}
		}

		SetPolyDest( bmscr->pBit, bmscr->sx, bmscr->sy );

		for(i=0;i<4;i++) {
			tmp_x[i] = px[i];
			tmp_y[i] = py[i];
			switch( sqmode ) {
			case GSQUARE_MODE_TEXTURE:
				tmp_tx[i] = GetLimit( ptx[i], 0, bm2->sx );
				tmp_ty[i] = bm2->sy - 1 - GetLimit( pty[i], 0, bm2->sy );	// UVの上下逆にする
				break;
			case GSQUARE_MODE_COLORFILL:
				tmp_tx[i] = 0;
				tmp_ty[i] = 0;
				break;
			}
		}

		attr = GetAttrOperation();
		DrawSquareEx( bmscr->palmode, color, attr, bmscr->GetAlphaOperation(), tmp_x, tmp_y, tmp_tx, tmp_ty );

		if ( resY0 >= 0 ) {
			bmscr->Send( 0, resY0, bmscr->sx, resY1-resY0+1 );
		}
		break;
		}

	case 0x38:								// gradf
		{
		int gradmode;
		int col;
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( bmscr->sx );
		p4 = code_getdi( bmscr->sy );
		gradmode = code_getdi( 0 );
		col = CnvRGB( bmscr->color );
		p5 = code_getdi( col );
		p6 = code_getdi( col );
		bmscr->GradFill( p1, p2, p3, p4, gradmode, p5, p6 );
		break;
		}
#endif

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

#ifndef HSP_COMPACT
	case 0x3c:								// celload
		{
		int i;
		char fname[_MAX_PATH];
		strncpy( fname, code_gets(), _MAX_PATH-1 );
		p1 = code_getdi( -1 );
		p2 = code_getdi( 0 );
		if ( p1 < 0 ) p1 = wnd->GetEmptyBufferId();

		wnd->MakeBmscrOff( p1, 32, 32, p2 );
		i = wnd->Picload( p1, fname, 2 );
		if ( i ) throw HSPERR_PICTURE_MISSING;

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
		bm2 = wnd->GetBmscrSafe( p1 );
		bm2->SetCelDivideSize( p2, p3, p4, p5 );
		break;
		}
	case 0x3e:								// celput
		{
		Bmscr *bm2;
		double zx,zy,rot;
		int x,y,srcsx,srcsy,putsx,putsy,centerx,centery;

		p1=code_getdi(1);
		p2=code_getdi(0);
		zx = code_getdd(1.0);
		zy = code_getdd(1.0);
		rot = code_getdd(0.0);
		bm2 = wnd->GetBmscrSafe( p1 );	// 転送元のBMSCRを取得

		if (( rot == 0.0 )&&( zx == 1.0 )&&( zy == 1.0 )) {
			//		変形なし
			if ( bmscr->CelPut( bm2, p2 ) ) throw HSPERR_UNSUPPORTED_FUNCTION;
			break;
		}

		//	変形あり
		if ( bmscr->palmode != bm2->palmode ) throw HSPERR_ILLEGAL_FUNCTION;

		srcsx = bm2->divsx;
		srcsy = bm2->divsy;
		x = ( p2 % bm2->divx ) * srcsx;
		y = ( p2 / bm2->divx ) * srcsy;
		putsx = (int)((double)srcsx * zx );
		putsy = (int)((double)srcsy * zy );
		centerx = (int)((double)bm2->celofsx * zx );
		centery = (int)((double)bm2->celofsy * zy );
		GRotateSub( bm2, x, y, srcsx, srcsy, putsx, putsy, rot, centerx, centery );
		bmscr->cx += putsx;
		break;
		}
#endif

	case 0x3f:								// gfilter
	case 0x40:								// setreq
	case 0x41:								// getreq
		// HSP3Dish用の機能
		throw HSPERR_UNSUPPORTED_FUNCTION;
		break;

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
	case 0x46:								// mtinfo
	case 0x47:								// devinfo
	case 0x48:								// devinfoi
	case 0x49:								// devprm
	case 0x4a:								// devcontrol
		// HSP3Dish用の機能
		throw HSPERR_UNSUPPORTED_FUNCTION;
		break;



	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


static int get_ginfo( int arg )
{
	HDC hdc;
	RECT rect;
	POINT pt;
	int i,j;

	if (( arg>=4 )&&( arg<=11 )) GetWindowRect( bmscr->hwnd, &rect);

	switch( arg ) {
	case 0:
		GetCursorPos(&pt);
		return pt.x;
	case 1:
		GetCursorPos(&pt);
		return pt.y;
	case 2:
		return wnd->GetActive();
	case 3:
		return cur_window;
	case 4:
		return rect.left;
	case 5:
		return rect.top;
	case 6:
		return rect.right;
	case 7:
		return rect.bottom;
	case 8:
		return bmscr->viewx;
	case 9:
		return bmscr->viewy;
	case 10:
		return rect.right - rect.left;
	case 11:
		return rect.bottom - rect.top;
	case 12:
		if ( bmscr->type != HSPWND_TYPE_BUFFER ) {
			bmscr->GetClientSize( &i, &j );
			return i;
		}
	case 26:
		return bmscr->sx;
	case 13:
		if ( bmscr->type != HSPWND_TYPE_BUFFER ) {
			bmscr->GetClientSize( &i, &j );
			return j;
		}
	case 27:
		return bmscr->sy;
	case 14:
		return bmscr->printsize.cx;
	case 15:
		return bmscr->printsize.cy;
	case 16:
		return GetRValue( bmscr->color );
	case 17:
		return GetGValue( bmscr->color );
	case 18:
		return GetBValue( bmscr->color );
	case 19:
		hdc=GetDC(NULL);
		i = 0;
		if ( GetDeviceCaps( hdc,RASTERCAPS ) & RC_PALETTE ) i = 1;
		ReleaseDC( NULL, hdc );
		return i;
	case 20:
		return GetSystemMetrics( SM_CXSCREEN );
	case 21:
		return GetSystemMetrics( SM_CYSCREEN );
	case 22:
		return bmscr->cx;
	case 23:
		return bmscr->cy;
	case 24:
		return ctx->intwnd_id;
	case 25:
		return wnd->GetEmptyBufferId();

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return 0;
}


static int reffunc_intfunc_ivalue;

static void *reffunc_function( int *type_res, int arg )
{
	void *ptr;
	int p1,p2;

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
		reffunc_intfunc_ivalue = get_ginfo( code_geti() );
		break;

	case 0x001:								// objinfo
		{
		int *iptr;
		p1 = code_geti();
		p2 = code_geti();
		if (( p1 < 0 )||( p1 >= bmscr->objmax )) throw HSPERR_ILLEGAL_FUNCTION;
		iptr = (int *)bmscr->GetHSPObject( p1 );
		reffunc_intfunc_ivalue = iptr[p2];
		break;
		}

	case 0x002:								// dirinfo
		p1 = code_geti();
		ptr = getdir( p1 );
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
		ptr = (void *)(&(bmscr->hwnd));
		break;
	case 0x004:								// hinstance
		ptr = (void *)(&(bmscr->hInst));
		break;
	case 0x005:								// hdc
		ptr = (void *)(&(bmscr->hdc));
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return ptr;
}


static void mmnfunc( void *hwnd )
{
	//		マルチメディアコールバック
	//
	mmman->Notify();
}


/*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

void *ex_getbmscr( int wid )
{
	Bmscr *bm;
	bm = wnd->GetBmscr( wid );
	return bm->GetBMSCR();
}

int ex_getobj( int wid, int id, void *inf )
{
	Bmscr *bm;
	HSPOBJINFO *obj;
	bm = wnd->GetBmscrSafe( wid );
	obj = bm->GetHSPObject( id );
	memcpy( inf, obj, sizeof(HSPOBJINFO) );
	return 0;
}

int ex_setobj( int wid, int id, const void *inf )
{
	Bmscr *bm;
	HSPOBJINFO *obj;
	bm = wnd->GetBmscrSafe( wid );
	obj = bm->GetHSPObject( id );
	memcpy( obj, inf, sizeof(HSPOBJINFO) );
	return 0;
}

int ex_addobj( int wid )
{
	Bmscr *bm;
	bm = wnd->GetBmscrSafe( wid );
	return bm->NewHSPObject();
}


void ex_mref( PVal *pval, int prm )
{
	int t,size;
	void *ptr;
	const int GETBM=0x60;
	t = HSPVAR_FLAG_INT;
	size = sizeof( Bmscr );
	if ( prm >= GETBM ) {
		ptr = ex_getbmscr( prm - GETBM );
	} else {
		switch( prm ) {
		case 0x40:
			size = sizeof(int);
			ptr = &ctx->stat;
			break;
		case 0x41:
			ptr = ctx->refstr;
			t = HSPVAR_FLAG_STR;
			size = 1024;
			break;
		case 0x42:
			size = bmscr->bmpsize;
			ptr = bmscr->pBit;
			break;
		case 0x43:
			ptr = bmscr;
			break;
		case 0x44:
			ptr = ctx; size = sizeof(HSPCTX);
			break;
		case 0x45:
			size = 0x300;
			ptr = wnd->pstpt;
			break;
		default:
			throw HSPERR_UNSUPPORTED_FUNCTION;
		}
	}
	HspVarCoreDupPtr( pval, t, ptr, size );
}


static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//		(内蔵GUI)
	//
#ifndef HSP_COMPACT
	hgiof_term();
#endif
	delete wnd;
	chgdisp( 0, 0, 0 );
	delete mmman;
	return 0;
}

void hsp3typeinit_extcmd( HSP3TYPEINFO *info, int sx, int sy, int wd, int xx, int yy )
{
	HINSTANCE hInstance;
	HSPEXINFO *exinfo;								// Info for Plugins
	char cname[32];
	int flag;

	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	hInstance = (HINSTANCE)ctx->instance;

#ifdef HSP3IMP
	strcpy( cname, "hspwndi" );
#else
	strcpy( cname, "hspwnd0" );
#endif
	if ( ctx->hspstat & HSPSTAT_SSAVER ) cname[6]='p';

	mmman = new MMMan;
	wnd = new HspWnd( hInstance, cname );

	flag = wd & 0x100;								// スクリーンセーバープレビューフラグ
	if ( flag ) wnd->SetParentWindow( ctx->wnd_parent );
	if ( wd & 1 ) flag |= 2;						// 非表示フラグ

	wnd->MakeBmscr( 0,HSPWND_TYPE_MAIN, xx, yy, sx, sy, sx, sy, flag );

/*
	rev 43
	mingw : error : 関数ポインタから非関数ポインタへの変換
	に対処
*/
	wnd->SetNotifyFunc( fpconv( mmnfunc ) );
	wnd->SetEventNoticePtr( &ctx->stat );
	cur_window = 0;
	msact = 1;
	dispflg = 0;
	bmscr = wnd->GetBmscr( 0 );
	mmman->Reset( bmscr->hwnd );
#ifndef HSP_COMPACT
	hgiof_init();
#endif

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		HSPEXINFOに関数を登録する
	//
	exinfo->actscr = &cur_window;					// Active Window ID
	exinfo->HspFunc_getbmscr = ex_getbmscr;
	exinfo->HspFunc_getobj = ex_getobj;
	exinfo->HspFunc_setobj = ex_setobj;
	exinfo->HspFunc_addobj = ex_addobj;
	exinfo->HspFunc_mref = ex_mref;
}

void hsp3typeinit_extfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_sysvar;
}


void hsp3gr_nextobj( void )
{
	//		[TAB]を押して次のobjectを選択
	//
	Bmscr *bm;
	int id, plus;
	id = wnd->GetActive();
	if ( id<0 ) return;
	plus = 1;
	if ( GetAsyncKeyState(VK_SHIFT)&0x8000 ) plus=-1;		// [shift]
	bm = wnd->GetBmscr( id );
	if ( bm->tabmove ) {
		bm->NextObject( plus );
	}
}


#ifdef HSPDEBUG
void hsp3gr_dbg_gui( void )
{
	//		デバッグウインドゥ用情報
	//
	code_adddbg( "ディレクトリ", getdir(0) );
	code_adddbg( "コマンドライン", getdir(4) );
	code_adddbg( "ウインドゥ最大", wnd->GetBmscrMax() );
	code_adddbg( "カレントウインドゥ", cur_window );
}
#endif
