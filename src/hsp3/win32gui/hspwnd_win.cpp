
//
//	HSP3 window manager
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "../hsp3config.h"
#include "../hsp3debug.h"
#include "../hspwnd.h"
#include "../dpmread.h"
#include "../strbuf.h"
#include "../strnote.h"
#include "../supio.h"

#ifndef HSP_COMPACT
#include "fcpoly.h"
#define USE_STBIMAGE
#endif

#ifdef USE_STBIMAGE
#include "stb_image.h"
#endif

#include <ocidl.h>
#include <olectl.h>

#define HIMETRIC_INCH	2540
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_INCH/2) / HIMETRIC_INCH )


/*
	rev 43
	mingw : warning : WM_MOUSEWHEELの再定義
	に対処
*/

/*
* WM_MOUSEWHEELはWindows NT 4.0以降で有効で、Windows 95で有効でない。
*/

#if !defined( WM_MOUSEWHEEL )
#define WM_MOUSEWHEEL 0x020A
#endif


#if !defined(HSP_COMPACT)
#if defined( _MSC_VER )
#pragma comment(lib,"msimg32.lib")
#endif
#endif

HspWnd *curwnd;
static MM_NOTIFY_FUNC notifyfunc;

#if !defined(HSP_COMPACT)
extern int resY0, resY1;				// "fcpoly.h"のパラメーター
#endif

/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

HspWnd::HspWnd()
{
	//		初期化
	//
	Reset( NULL, "hspwnd0" );
}

HspWnd::HspWnd( HANDLE instance, char *wndcls )
{
	//		初期化
	//
	Reset( instance, wndcls );
}

HspWnd::~HspWnd()
{
	//		すべて破棄
	//
	Dispose();
}


/*------------------------------------------------------------*/
/*
		Window interface in C
*/
/*------------------------------------------------------------*/

Bmscr *TrackBmscr( HWND hwnd )
{
	int id;
	id = (int)GetWindowLongPtr( hwnd, GWLP_USERDATA );
	if ( id < 0 ) return NULL;
	return curwnd->GetBmscr( id );
}


void WM_Paint( HWND hwnd, Bmscr *bm )
{
	//		WM_PAINT process
	//
	PAINTSTRUCT ps;
	HDC hdc;
	if ( bm->flag == BMSCR_FLAG_NOUSE ) return;
	hdc = BeginPaint ( hwnd, &ps );
	bm->Bmspnt( hdc );
	EndPaint ( hwnd, &ps );
}


LRESULT CALLBACK WndProc( HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	int id;
	int retval;
	Bmscr *bm;

	if ( code_isuserirq() ) {
		if ( code_checkirq( (int)GetWindowLongPtr( hwnd, GWLP_USERDATA ), (int)uMessage, (int)wParam, (int)lParam ) ) {
			if ( code_irqresult( &retval ) ) return retval;
		}
	}

	switch (uMessage)
	{
	case WM_CREATE:
		SetWindowLongPtr( hwnd, GWLP_USERDATA, -1 );
		return 0;

	case WM_PALETTECHANGED:
		if ( wParam == (WPARAM)hwnd ) break;
	case WM_QUERYNEWPALETTE:
		bm = TrackBmscr( hwnd );
		if ( bm == NULL ) break;
		if (bm->hpal != NULL) {				// パレット変更時にもマッチングさせる
			int a;
			HDC hdc;
			HPALETTE opal;
			hdc=GetDC( hwnd );
			opal=SelectPalette( hdc, bm->hpal, 0 );
			a=RealizePalette( hdc );
			SelectPalette( hdc, opal, 0 );
			ReleaseDC( hwnd, hdc );
			if ( a > 0) InvalidateRect( hwnd, NULL, TRUE );
			return a;
		}
		break;

	case WM_PAINT:
		//		Display 全描画
		//
		bm = TrackBmscr( hwnd );
		if ( bm != NULL ) WM_Paint( hwnd, bm );
		return 0 ;

	case WM_GETMINMAXINFO:
		{
		LPMINMAXINFO lpmm;
		bm = TrackBmscr( hwnd );
		if ( bm != NULL ) {
			lpmm = (LPMINMAXINFO)lParam;
			lpmm->ptMaxTrackSize.x = bm->sx + bm->framesx;
			lpmm->ptMaxTrackSize.y = bm->sy + bm->framesy;
		}
		break;
		}

	case WM_SIZE:
		bm = TrackBmscr( hwnd );
		if ( bm != NULL ) {
			bm->wx = lParam & 0xFFFF;				// xサイズ
			bm->wy = (lParam >> 16) & 0xFFFF;		// yサイズ
			bm->SetScroll( bm->viewx, bm->viewy );
		}
		break;

	case WM_MOUSEWHEEL:
		bm = TrackBmscr( hwnd );
		if ( bm != NULL ) {
			bm->savepos[BMSCR_SAVEPOS_MOSUEZ] = LOWORD(wParam);
			bm->savepos[BMSCR_SAVEPOS_MOSUEW] = HIWORD(wParam);
		}
		return 0;

	case WM_MOUSEMOVE:
		bm = TrackBmscr( hwnd );
		if ( bm != NULL ) {
			bm->savepos[BMSCR_SAVEPOS_MOSUEX] = LOWORD(lParam);
			bm->savepos[BMSCR_SAVEPOS_MOSUEY] = HIWORD(lParam);
		}
		return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if ( code_isirq( HSPIRQ_ONCLICK ) ) {
			code_sendirq( HSPIRQ_ONCLICK, (int)uMessage - (int)WM_LBUTTONDOWN, (int)wParam, (int)lParam );
		}
		break;
	case WM_COMMAND:
		if ( wParam & MESSAGE_HSPOBJ ) {
			id = (int)GetWindowLongPtr( hwnd, GWLP_USERDATA );
			bm =curwnd->GetBmscr( id );
			//Alertf( "%d,%x,%x (%d)",id,wParam,lParam , ( wParam & (MESSAGE_HSPOBJ-1)) );
			bm->SendHSPObjectNotice( (int)wParam );
		}
		return 0;

	case MM_MCINOTIFY:
		if ( wParam == MCI_NOTIFY_SUCCESSFUL ) {
			if ( notifyfunc != NULL ) notifyfunc( hwnd );
		}
		return 0;

#ifdef HSPWINGUIDLL
	case WM_QUERYENDSESSION:
		return TRUE;
	case WM_CLOSE:
		return 0;
#else
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		id = (int)GetWindowLongPtr( hwnd, GWLP_USERDATA );
		if ( code_isirq( HSPIRQ_ONEXIT ) ) {
			int iparam = 0;
			if ( uMessage == WM_QUERYENDSESSION ) iparam++;
			retval = code_sendirq( HSPIRQ_ONEXIT, iparam, id, 0 );
			if ( retval == RUNMODE_INTJUMP ) retval = code_execcmd2();	// onexit goto時は実行してみる
			if ( retval != RUNMODE_END ) return 0;
		}
		code_puterror( HSPERR_NONE );
		PostQuitMessage(0);
		return (uMessage == WM_QUERYENDSESSION) ? true : false;
#endif

	case WM_DRAWITEM:
		if ( wParam & MESSAGE_HSPOBJ ) {
			id = (int)GetWindowLongPtr( hwnd, GWLP_USERDATA );
			bm =curwnd->GetBmscr( id );
			bm->SendHSPObjectDraw( (int)wParam, (LPDRAWITEMSTRUCT)lParam );
		}
		break;

	}

	return DefWindowProc (hwnd, uMessage, wParam, lParam) ;
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void HspWnd::Dispose( void )
{
	//		破棄
	//
	int i;
	HWND hwnd;
	Bmscr *bm;
	for(i=0;i<bmscr_max;i++) {
		bm = mem_bm[i];
		if ( bm != NULL ) {
			hwnd = bm->hwnd;
			if ( hwnd != NULL ) DestroyWindow( hwnd );
			delete bm;
		}
	}
	free( mem_bm );
	UnregisterClass( defcls, hInst );
}


int HspWnd::GetActive( void )
{
	//
	//		detect active window
	int i;
	HWND hwnd;
	Bmscr *bm;

	hwnd = GetActiveWindow();

	for(i=0;i<bmscr_max;i++) {
		bm = mem_bm[i];
		if ( bm != NULL ) {
			if ( bm->hwnd != NULL ) if ( bm->hwnd == hwnd ) return i;
		}
	}
	if (( wnd_parent != NULL )&&( wnd_parent == hwnd )) return 0;

	return -1;
}


void HspWnd::ExpandScreen( int id )
{
	int i;
	int idmax;
	Bmscr **new_bm;

	//Alertf("Expand:%d:%d",idmax,bmscr_max);
	idmax = id + 1;
	if ( idmax <= bmscr_max ) return;
	new_bm = (Bmscr **)malloc( sizeof( Bmscr * ) * idmax );

	for(i=0;i<idmax;i++) {
		if (( i >= bmscr_max )||( bmscr_max == 0 )) {
			new_bm[i] = NULL;
		} else {
			//if ( mem_bm[i] != NULL ) 
			new_bm[i] = mem_bm[i];
		}
	}
	if ( mem_bm != NULL ) free( mem_bm );

	bmscr_max = idmax;
	mem_bm = new_bm;
}


void HspWnd::Reset( HANDLE instance, char *wndcls )
{
	//		all window initalize
	//
	int i;
	WNDCLASS    wndclass ;
	HSPAPICHAR *hactmp1 = 0;
	hInst=(HINSTANCE)instance;
	_tcscpy( defcls, chartoapichar(wndcls,&hactmp1) );
	freehac(&hactmp1);

	//		alloc Bmscr
	//
	bmscr_max = 0;
	mem_bm = NULL;
	ExpandScreen( 31 );									// とりあえず

	//		global vals
	//
	wfy=GetSystemMetrics( SM_CYCAPTION )+GetSystemMetrics( SM_CYFRAME )*2;
	wfx=GetSystemMetrics( SM_CXFRAME )*2;
	wbx=GetSystemMetrics( SM_CXHTHUMB );
	wby=GetSystemMetrics( SM_CYVTHUMB );
	mwfy=GetSystemMetrics( SM_CYCAPTION )+GetSystemMetrics( SM_CYFIXEDFRAME )*2;
	mwfx=GetSystemMetrics( SM_CXFIXEDFRAME )*2;

	//		main window class
	//
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = (WNDPROC) WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInst;
	wndclass.hIcon         = LoadIcon( hInst, MAKEINTRESOURCE(128) );
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = defcls;
	RegisterClass (&wndclass);

	//		Reset palette
	//
	for(i=0;i<256*3;i++) {
		pstpt[i] = i/3;						// グレースケールパレットを作成
	}

	//		save pointer for C
	//
	SetNotifyFunc( NULL );
	SetEventNoticePtr( &bmscr_res );
	SetParentWindow( NULL );
	curwnd = this;
}


void HspWnd::SetEventNoticePtr( int *ptr )
{
	resptr = ptr;
	SetObjectEventNoticePtr( resptr );
}


void HspWnd::SetNotifyFunc( void *func )
{
	notifyfunc = (MM_NOTIFY_FUNC)func;
}


void HspWnd::MakeBmscrOff( int id, int sx, int sy, int mode )
{
	//		Bmscr(オフスクリーン)生成
	//
	ExpandScreen( id );

	HWND hwnd = NULL;

	if ( mem_bm[ id ] != NULL ) {
		hwnd = mem_bm[ id ]->hwnd;
		delete mem_bm[ id ];
		mem_bm[ id ] = NULL;

		ShowWindow( hwnd, SW_HIDE );
	}

	Bmscr * bm = new Bmscr;
	mem_bm[ id ] = bm;

	bm->wid = id;
	bm->master_hspwnd = static_cast< void * >( this );
	bm->type = HSPWND_TYPE_BUFFER;

	bm->Init( hInst, hwnd, sx, sy,
	 ( mode & 0x01 ? BMSCR_PALMODE_PALETTECOLOR : BMSCR_PALMODE_FULLCOLOR ) );

	bm->wx = 0;
	bm->wy = 0;
	bm->wchg = 0;
	bm->viewx=0;
	bm->viewy=0;
}


void HspWnd::MakeBmscrWnd( int id, int type, int xx, int yy, int wx, int wy, int sx, int sy, int mode )
{
	HSPAPICHAR *hactmp1 = 0;
	//		Bmscr(ウィンドウ)生成
	//
	ExpandScreen( id );

	int wndtype = type, style = 0, exstyle = 0;
	HWND par_hwnd = NULL;

	// スクリーンタイプごとのウィンドウスタイルの設定。
	if ( wndtype == HSPWND_TYPE_BGSCR ) {
		style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	} else {
		if ( mode & 0x100 ) {
			wndtype = HSPWND_TYPE_SSPREVIEW;
			style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			par_hwnd = static_cast< HWND >( wnd_parent );

		} else {
			style = WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX
			 | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			if ( id > 0 && !( mode & 0x04 ) ) {	// サイズ可変。
				style |= WS_THICKFRAME;
			}
			if ( mode & 0x08 ) {	// ツールウィンドウ。
				exstyle |= WS_EX_TOOLWINDOW;
			}
			if ( mode & 0x10 ) {	// 縁が深い。
				exstyle |= WS_EX_OVERLAPPEDWINDOW;
			}
		}
	}

	HWND hwnd = NULL;

	// ウィンドウの生成・再設定。
	if ( mem_bm[ id ] == NULL ) {

#ifdef HSPDEBUG
		char const * const pc = HSPTITLE hspver;
#else
		char const * const pc = NULL;
#endif

		hwnd = CreateWindowEx( exstyle,				// extra window style
		                       defcls,				// window class name
		                       chartoapichar(pc,&hactmp1),	// window caption
		                       style,				// window style
		                       ( xx != -1 ? xx : CW_USEDEFAULT ),
		                       		// initial x position
		                       ( yy != -1 ? yy : CW_USEDEFAULT ),
		                       		// initial y position
		                       wx,					// initial x size
		                       wy,					// initial y size
		                       par_hwnd,			// parent window handle
		                       NULL,				// window menu handle
		                       hInst,				// program instance handle
		                       NULL );				// creation parameters
		freehac(&hactmp1);

	} else {
		hwnd = mem_bm[ id ]->hwnd;
		delete mem_bm[ id ];
		mem_bm[ id ] = NULL;

		RECT rc;
		GetWindowRect( hwnd, &rc );
		SetWindowPos( hwnd, NULL,
		 ( xx != -1 ? xx : rc.left ), ( yy != -1 ? yy : rc.top ), wx, wy,
		 SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOZORDER );
		SetWindowLong( hwnd, GWL_STYLE, style );
		SetWindowLong( hwnd, GWL_EXSTYLE, exstyle );
	}

	SetWindowLongPtr( hwnd, GWLP_USERDATA, id );

	Bmscr * bm = new Bmscr;
	mem_bm[ id ] = bm;

	bm->wid = id;
	bm->master_hspwnd = static_cast< void * >( this );
	bm->type = wndtype;

	bm->Init( hInst, hwnd, sx, sy,
	 ( mode & 0x01 ? BMSCR_PALMODE_PALETTECOLOR : BMSCR_PALMODE_FULLCOLOR ) );

	bm->wchg = 0;
	bm->viewx = 0;
	bm->viewy = 0;

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = wx;
	rc.bottom = wy;
	if ( AdjustWindowRectEx( &rc, style, FALSE, exstyle ) ) {
		bm->framesx = rc.right - rc.left - wx;
		bm->framesy = rc.bottom - rc.top - wy;
	} else {
		bm->framesx = wfx;
		bm->framesy = wfy;
	}

	bm->Width( wx, wy, -1, -1, 1 );

	SetWindowPos( hwnd, HWND_TOP, 0, 0, 0, 0,
	 ( mode & 2 ? SWP_NOACTIVATE | SWP_NOZORDER : SWP_SHOWWINDOW ) |
	 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE );

	bm->Update();
}


void HspWnd::MakeBmscr( int id, int type, int xx, int yy, int wx, int wy, int sx, int sy, int mode )
{
	//		Bmscr生成
	//
	if ( id < 0 ) throw HSPERR_ILLEGAL_FUNCTION;

	Bmscr const * const pbm = ( id < bmscr_max ? mem_bm[ id ] : NULL );

	if ( type == HSPWND_TYPE_BUFFER ) {
		MakeBmscrOff( id, sx, sy, mode );

	} else if ( pbm == NULL || pbm->type != HSPWND_TYPE_BUFFER ) {
		MakeBmscrWnd( id, type, xx, yy, wx, wy, sx, sy, mode );

	} else {
		MakeBmscrOff( id, sx, sy, mode );
		//throw HSPERR_UNSUPPORTED_FUNCTION;
	}
}


int HspWnd::Picload( int id, char *fname, int mode )
{
	//		picload
	//		( mode:0=resize/1=overwrite )
	//
	Bmscr *bm;
	void *pBuf;
    HGLOBAL h;
	RECT rc;
	int i,size,x,y,psx,psy;
    HRESULT hr;
	long hmWidth, hmHeight;
	LPPICTURE gpPicture;							// IPicture
    LPSTREAM pstm = NULL;							// IStreamを取得する
	char fext[8];
	int stbmode;

	bm = GetBmscr( id );
	if ( bm == NULL ) return 1;
	if ( bm->flag == BMSCR_FLAG_NOUSE ) return 1;

	//　ファイルサイズを取得
	size = dpm_exist( fname );
	if ( size <= 0 ) return 1;

	// グローバル領域を確保
	h = GlobalAlloc( GMEM_MOVEABLE, size );
    if( h == NULL) return 2;
	pBuf = GlobalLock( h );
    if( pBuf == NULL ) return 2;
	i = dpm_read( fname, pBuf, size, 0 );
	if ( i <= 0 ) return 1;

#ifdef USE_STBIMAGE
	stbmode = 0;
	getpath(fname,fext,16+2);				// 拡張子を小文字で取り出す

	if (!strcmp(fext,".png")) stbmode++;	// ".png"の時
	if (!strcmp(fext,".psd")) stbmode++;	// ".psd"の時
	if (!strcmp(fext,".tga")) stbmode++;	// ".tga"の時

	if ( stbmode ) {						// stb_imageを使用して読み込む
		int components;
		unsigned char *sp_image;
		sp_image = stbi_load_from_memory( (unsigned char *)pBuf, size, &psx, &psy, &components, 4 );
		if ( sp_image == NULL ) return 3;

		if (( mode == 0 )||( mode == 2 )) {
			int palsw,type;
			type = bm->type; palsw = bm->palmode;
			MakeBmscr( id, type, -1, -1, psx, psy, psx, psy, palsw );
			bm = GetBmscr( id );
			if ( mode == 2 ) bm->Cls( 4 );	// 黒色でクリアしておく
		}

		x = bm->cx; y = bm->cy;
		bm->RenderAlphaBitmap( psx, psy, components, sp_image );
		bm->Send( x, y, psx, psy );

		stbi_image_free(sp_image);
		GlobalUnlock( h );
		GlobalFree(h);
		return 0;
	}
#endif

	GlobalUnlock( h );

	hr = CreateStreamOnHGlobal( h, TRUE, &pstm );	// グローバル領域からIStreamを作成
    if( !SUCCEEDED(hr) ) return 3;
    hr = OleLoadPicture( pstm, size, FALSE, IID_IPicture, (LPVOID *)&gpPicture );    // IPictureのオブジェクトのアドレスを取得
    pstm->Release();							    // IStreamオブジェクトを開放
	if( SUCCEEDED( hr ) == FALSE || gpPicture == NULL ) {
		return 3;
    }

	// get width and height of picture
	gpPicture->get_Width(&hmWidth);
	gpPicture->get_Height(&hmHeight);
	// convert himetric to pixels
	psx = MulDiv( hmWidth, GetDeviceCaps( bm->hdc, LOGPIXELSX ), HIMETRIC_INCH );
	psy = MulDiv( hmHeight, GetDeviceCaps( bm->hdc, LOGPIXELSY ), HIMETRIC_INCH );

	if (( mode == 0 )||( mode == 2 )) {
		int palsw,type;
		type = bm->type; palsw = bm->palmode;
		MakeBmscr( id, type, -1, -1, psx, psy, psx, psy, palsw );
		bm = GetBmscr( id );
		if ( mode == 2 ) bm->Cls( 4 );	// 黒色でクリアしておく
	}

	// setup initial position
	x = bm->cx; y = bm->cy;
	GetClientRect( bm->hwnd, &rc );
	// display picture using IPicture::Render
	gpPicture->Render( bm->hdc, x, y, psx, psy, 0, hmHeight, hmWidth, -hmHeight, &rc );
	bm->Send( x, y, psx, psy );

	gpPicture->Release();
	GlobalFree(h);
	return 0;
}


Bmscr *HspWnd::GetBmscrSafe( int id )
{
	//		安全なbmscr取得
	//
	Bmscr *bm;
	if (( id < 0 )||( id >= bmscr_max )) throw HSPERR_ILLEGAL_FUNCTION;
	bm = GetBmscr( id );
	if ( bm == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
	if ( bm->flag == BMSCR_FLAG_NOUSE ) throw HSPERR_ILLEGAL_FUNCTION;
	return bm;
}


int HspWnd::GetEmptyBufferId( void )
{
	//		空きIDを取得
	//
	int i;
	Bmscr *bm;
	for(i=0;i<bmscr_max;i++) {
		bm = GetBmscr(i);
		if ( bm == NULL ) return i;
		if ( bm->flag == BMSCR_FLAG_NOUSE ) return i;
	}
	return bmscr_max;
}


/*------------------------------------------------------------*/
/*
		Bmscr interface
*/
/*------------------------------------------------------------*/

Bmscr::Bmscr()
{
	//		bmscr初期化
	//
	flag = BMSCR_FLAG_NOUSE;
}

Bmscr::~Bmscr()
{
	//		Bmscr破棄
	//
	if ( flag == BMSCR_FLAG_INUSE ) {

		//		object remove
		//
		ResetHSPObject();

		//		resource remove
		//
		Delfont();

		//		remove HBRUSH
		//
		if ( hbr != NULL ) {
			DeleteObject( hbr );
		}

		//		remove HPEN
		//
		if ( hpn != NULL ) {
			DeleteObject( hpn );
		}

		//		remove HPALETTE
		//
		if ( hpal != NULL ) {
			SelectPalette( hdc, holdpal, TRUE );
			DeleteObject( hpal );
		}

		//		remove DIB section
		//
		SelectObject( hdc, old );
		DeleteObject( dib );
		DeleteDC( hdc );
		GlobalFree( pbi );
	}
}

/*----------------------------------------------------------------*/
//		font&text related routines
/*----------------------------------------------------------------*/

void Bmscr::Init( HANDLE instance, HWND p_hwnd, int p_sx, int p_sy, int palsw )
{
	//		bitmap buffer make
	//
	HDC srchdc;
 	HBITMAP hbDib,hbOld;
	int msize,bsize;

	hwnd = p_hwnd;
	hInst= (HINSTANCE)instance;

	flag = BMSCR_FLAG_INUSE;
	objmax = 0;
	mem_obj = NULL;
	sx = p_sx; sy = p_sy;

	palmode = palsw;
	bsize = sx;

	if ( palmode == BMSCR_PALMODE_FULLCOLOR ) {
		msize=sizeof(BITMAPINFOHEADER);
		pbi=(LPBITMAPINFOHEADER)GlobalAlloc( GPTR, msize );
		pbi->biBitCount = 24;
		bsize*=3;
	} else {
		msize=sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
		pbi=(LPBITMAPINFOHEADER)GlobalAlloc( GPTR, msize );
		pbi->biBitCount = 8;
	}
	bsize = ( bsize + 3 )&~3;
	sx2 = bsize;
	bsize *= sy;

	infsize = msize;
	bmpsize = bsize;
	
	pbi->biSize = sizeof(BITMAPINFOHEADER);
	pbi->biWidth = sx;
	pbi->biHeight = sy;
	pbi->biPlanes = 1;

	srchdc = GetDC(hwnd);
	hbDib=CreateDIBSection( srchdc,(BITMAPINFO *)pbi,DIB_RGB_COLORS,
			(void **)&pBit, NULL, 0 );
	hdc=CreateCompatibleDC(srchdc);
	hbOld = (HBITMAP)SelectObject( hdc, hbDib );
	ReleaseDC( hwnd, srchdc );

	dib = hbDib;
	old = hbOld;
	pal = (RGBQUAD *)(pbi+1);
	hpal = NULL;
	hbr  = NULL;
	hpn  = NULL;
	focflg = 0;
	objmode = 1;
	tabmove = 1;
	hfont=NULL;
	fl_dispw = 0;
	imgbtn = -1;

	Cls( 0 );

	fl_dispw = 1;
	fl_udraw = 1;
}


void Bmscr::Cls( int mode )
{
	//		screen setting reset
	//
	int i;
	HBRUSH hbr, hbrOld;

	//		Font initalize
	//
	Sysfont(0);

	//		object initalize
	//
	ResetHSPObject();

	//		text setting initalize
	//
	cx=0;cy=0;
	Setcolor(0,0,0);

	//		palette initalize
	//
	UpdatePalette();

	//		screen clear
	//
	hbr = (HBRUSH)GetStockObject( mode );
	hbrOld = (HBRUSH)SelectObject( hdc, hbr );
	Rectangle( hdc, -1, -1, sx+1, sy+1);
	SelectObject( hdc, hbrOld );

	//		vals initalize
	//
	textspeed=0;
	ox=64;oy=24;py=0;
	gx=32;gy=32;gmode=0;
	objstyle = WS_CHILD|WS_VISIBLE;
	for(i=0;i<BMSCR_SAVEPOS_MAX;i++) { savepos[i] = 0; }
	palcolor = 0;

	//		CEL initalize
	//
	SetCelDivideSize( 0, 0, 0, 0 );

	//		all update
	//
	fl_udraw = fl_dispw;
	Update();
}


void Bmscr::Bmspnt( HDC disthdc )
{
	HPALETTE opal;

	if ( hpal != NULL ) {
		opal=SelectPalette( disthdc, hpal, 0 );
		RealizePalette( disthdc );
	}
	BitBlt( disthdc, 0, 0, wx, wy, hdc, viewx, viewy, SRCCOPY );
	if ( hpal != NULL ) {
		SelectPalette( disthdc, opal, 0 );
	}
}


void Bmscr::Update( void )
{
	HDC disthdc;
	if ( fl_udraw == 0 ) return;
	if ( type < HSPWND_TYPE_MAIN ) return;
	disthdc=GetDC( hwnd );
	Bmspnt( disthdc );
	ReleaseDC( hwnd, disthdc );
}


void Bmscr::Title( char *str )
{
	HSPAPICHAR *hactmp1 = 0;
	if ( type < HSPWND_TYPE_MAIN ) return;
	SetWindowText( hwnd, chartoapichar(str,&hactmp1) );
	freehac(&hactmp1);
}


void Bmscr::Width( int x, int y, int wposx, int wposy, int mode )
{
	RECT rw;
	int sizex, sizey;

	if ( x > 0 ) {
		wx = x; if ( x > sx ) wx = sx;
	}
	if ( y > 0 ) {
		wy = y; if ( y > sy ) wy = sy;
	}

	if (( type < HSPWND_TYPE_MAIN )||( type == HSPWND_TYPE_SSPREVIEW )) return;

	sizex = wx + framesx;
	sizey = wy + framesy;
	GetWindowRect( hwnd,&rw );
	if ( wposx >= 0 ) rw.left = wposx;
	if ( wposy >= 0 ) rw.top = wposy;
	MoveWindow( hwnd, rw.left, rw.top, sizex, sizey, (mode>0) );
}


void Bmscr::Send( int x, int y, int p_sx, int p_sy )
{
	HDC dhdc;
	HPALETTE opal;
	if ( fl_udraw == 0 ) return;
	if ( type < HSPWND_TYPE_MAIN ) return;
	dhdc=GetDC( hwnd );
	if ( hpal != NULL ) {
		opal=SelectPalette( dhdc, hpal, 0 );
		RealizePalette( dhdc );
	}
	BitBlt( dhdc, x-viewx, y-viewy, p_sx, p_sy, hdc,x,y, SRCCOPY );
	if ( hpal != NULL ) {
		SelectPalette( dhdc, opal, 0 );
	}
	ReleaseDC( hwnd, dhdc );
}


void Bmscr::Posinc( int pp )
{
	if ( pp<py ) { cy+=py; } else { cy+=pp; }
}


void Bmscr::Delfont( void )
{
	//	destroy font
	//
	if (hfont!=NULL) {
		SelectObject( hdc, holdfon );
		DeleteObject( hfont );
		hfont=NULL;
	}
}


void Bmscr::Fontupdate( void )
{
	//	update new font
	//
	TEXTMETRIC	tm;
	if (hfont!=NULL) {
		holdfon = (HFONT)SelectObject( hdc, hfont );
		GetTextMetrics( hdc, &tm );
		texty = tm.tmHeight + tm.tmExternalLeading;
	}
/*
	{
		HDC hdcwnd = GetDC(hwndbmp);
		HFONT holdfontwnd = (HFONT)SelectObject(hdcwnd, hfont);

		holdfont = (HFONT)SelectObject(hdc, hfont);

		SelectObject(hdcwnd, holdfontwnd);
		ReleaseDC(hwndbmp, hdcwnd);
	}
*/

}


int Bmscr::Newfont( char *fonname, int fpts, int fopt, int angle )
{
	//	select new font
	//		fopt : bit0=BOLD       bit1=Italic
	//		       bit2=Underline  bit3=Strikeout
	//		       bit4=Anti-alias
	//		fpts : point size
	//		angle: rotation
	//
	int a;
	BYTE b;
	PLOGFONT pLogFont;			// logical FONT ptr
	unsigned char chk;
	HFONT hf_new;
	HSPAPICHAR *hactmp1 = 0;

	pLogFont=(PLOGFONT) &logfont;
	_tcscpy( pLogFont->lfFaceName, chartoapichar(fonname,&hactmp1) );
	freehac(&hactmp1);
	pLogFont->lfHeight			= -fpts;
	pLogFont->lfWidth			= 0;
	pLogFont->lfOutPrecision	= 0 ;
	pLogFont->lfClipPrecision	= 0 ;

	if (fopt&4) {
		pLogFont->lfUnderline		= TRUE;
	} else {
		pLogFont->lfUnderline		= FALSE;
	}

	if (fopt&8) {
		pLogFont->lfStrikeOut		= TRUE;
	} else {
		pLogFont->lfStrikeOut		= FALSE;
	}

	if ( fopt & 16 ) {
		pLogFont->lfQuality			= ANTIALIASED_QUALITY ;
	} else {
		pLogFont->lfQuality			= DEFAULT_QUALITY;
	}

	pLogFont->lfPitchAndFamily	= 0 ;
	pLogFont->lfEscapement		= angle ;
	pLogFont->lfOrientation		= 0 ;

	b=DEFAULT_CHARSET;
	a=0;while(1) {
		chk=fonname[a++];
		if (chk==0) break;
		if (chk>=0x80) { b=SHIFTJIS_CHARSET;break; }
	}
	pLogFont->lfCharSet = b;

	if (fopt&1) {
		pLogFont->lfWeight = FW_BOLD;
	} else {
		pLogFont->lfWeight = FW_NORMAL;
	}

	if (fopt&2) {
		pLogFont->lfItalic = TRUE;
	} else {
		pLogFont->lfItalic = FALSE;
	}

	hf_new = CreateFontIndirect( pLogFont );
	if ( hf_new == NULL ) return -1;

	Delfont();
	hfont = hf_new;
	Fontupdate();
	return 0;
}


void Bmscr::Sysfont( int p1 )
{
	/*
		Reserved Font List
	#define OEM_FIXED_FONT      10
	#define ANSI_FIXED_FONT     11
	#define ANSI_VAR_FONT       12
	#define SYSTEM_FONT         13
	#define DEVICE_DEFAULT_FONT 14
	#define DEFAULT_PALETTE     15
	#define SYSTEM_FIXED_FONT   16
	#define DEFAULT_GUI_FONT    17
	*/
	HFONT hf;
	Delfont();
	if (p1) {
		hf=(HFONT)GetStockObject(p1);
		if (hf!=NULL) {
			hfont=hf;
			Fontupdate();
			return;
		}
	}
	Newfont( "FixedSys", 14,0,0 );
}


void Bmscr::Setcolor( int a1, int a2, int a3 )
{
	COLORREF c;
	if (a1==-1) c = PALETTEINDEX(a2);
		   else c = color = RGB(a1,a2,a3);
	SetBkMode( hdc,TRANSPARENT );
	SetTextColor( hdc, c );
	if ( hbr != NULL ) DeleteObject( hbr );
	hbr = CreateSolidBrush( c );
	if ( hpn != NULL ) DeleteObject( hpn );
	hpn = CreatePen( PS_SOLID,0,c );
}


void Bmscr::Setcolor( COLORREF rgbcolor )
{
	Setcolor( GetRValue(rgbcolor), GetGValue(rgbcolor), GetBValue(rgbcolor) );
}


void Bmscr::SetSystemcolor( int id )
{
	Setcolor( (COLORREF)GetSysColor( id ) );
}


void Bmscr::UpdatePalette( void )
{
	//		set palette to DIB & HPALETTE
	//			*rgbptr : R,G,B order palette entry
	//
	int a,colors;
	BYTE *pt;
	RGBQUAD *bpal;
	LOGPALETTE *lpPal;
	HPALETTE hGetPal;
	HspWnd *wnd;
	void *rgbptr;
	//FILE *fp;

	wnd = (HspWnd *)master_hspwnd;
	rgbptr = wnd->pstpt;
	colors = 256;

	if ( palmode == 0 ) return;

	if ( hpal != NULL ) {
		SelectPalette( hdc, holdpal, TRUE );
		DeleteObject( hpal );
	}

	bpal = pal;
	pt=(BYTE *)rgbptr;
	for(a=0;a<colors;a++) {
		bpal[a].rgbBlue  = pt[2];
		bpal[a].rgbRed   = pt[0];
		bpal[a].rgbGreen = pt[1];
		bpal[a].rgbReserved = 0;
		pt+=3;
	}
	SetDIBColorTable( hdc, 0, colors, bpal );

	//		make HPALETTE
	//
	lpPal = (LOGPALETTE *) malloc( 0x800 );		// size of Pal buffer 
	lpPal->palVersion = 0x300;
	lpPal->palNumEntries = (WORD) colors;
	pt=(BYTE *)rgbptr;
	for( a=0; a<colors; a++ ) {
		lpPal->palPalEntry[a].peRed		= pt[0];
		lpPal->palPalEntry[a].peGreen	= pt[1];
		lpPal->palPalEntry[a].peBlue	= pt[2];
		lpPal->palPalEntry[a].peFlags	= PC_NOCOLLAPSE;
		pt+=3;
	}
	hGetPal = CreatePalette( lpPal );
	hpal = hGetPal;
	holdpal = SelectPalette( hdc, hGetPal, TRUE );
	free( lpPal );
	pals = colors;
}


void Bmscr::SetPalette( int palno, int rv, int gv, int bv )
{
	int a;
	BYTE a1,a2,a3;
	HspWnd *wnd;
	BYTE *pstpt;

	if ( palmode == 0 ) return;

	wnd = (HspWnd *)master_hspwnd;
	pstpt = wnd->pstpt;

	a=palno*3;
	a1=rv;a2=gv;a3=bv;
	pstpt[a++]=a1; pstpt[a++]=a2; pstpt[a++]=a3;
	//UpdatePalette();
}


void Bmscr::SetPalcolor( int palno )
{
	RGBQUAD *p;
	p=&pal[ palno ];
	color = RGB( p->rgbRed, p->rgbGreen, p->rgbBlue );
	Setcolor( -1, palno, 0 );
	palcolor = palno;
}


void Bmscr::Print( char *mes )
{
	int a;
	long res;
	SIZE *size;
	HSPAPICHAR *hactmp1 = 0;
	size = &printsize;
	a=(int)strlen(mes);
	if (a) {

		//GetTextExtentPoint32( hdc, mes, a, size );
		//HDC hdcwnd = GetDC( hwnd );
		//HFONT holdfontwnd = (HFONT)SelectObject( hdcwnd, hfont );

		//TextOut( hdc, cx, cy , mes, a );
		chartoapichar(mes,&hactmp1);

		res = TabbedTextOut( hdc, cx, cy, hactmp1 , _tcslen(hactmp1), 0, NULL, 0 );
		freehac(&hactmp1);
		size->cx = res & 0xffff;
		size->cy = res>>16;

		//SelectObject( hdcwnd, holdfontwnd );
		//ReleaseDC( hwnd, hdcwnd );

		Send( cx, cy, size->cx, size->cy );
	} else {
		GetTextExtentPoint32( hdc, TEXT(" "), 1, size );
	}
	Posinc( size->cy );
}


void Bmscr::Boxfill( int x1,int y1,int x2,int y2 )
{
	//		boxf
	RECT bx;
	bx.left=x1;bx.top=y1;bx.right=x2+1;bx.bottom=y2+1;
	FillRect( hdc, &bx, hbr );
	Send( x1,y1,x2-x1+1,y2-y1+1 );
}


COLORREF Bmscr::Pget( int xx, int yy )
{
	//		pget
	//
	COLORREF c;
	c = GetPixel( hdc, xx, yy );
	Setcolor( c );
	return c;
}


void Bmscr::Pset( int xx,int yy )
{
	//		pset
	//
	SetPixel( hdc,xx,yy,color );
	Send( xx,yy,1,1 );
}


void Bmscr::Line( int xx,int yy )
{
	//		line
	//
	HPEN oldpen;
	int x,y,x1,y1,x2,y2;
	x=cx;y=cy;
	MoveToEx( hdc,x,y,NULL );
	if (cx<xx)   { x1=x;x2=xx-x; }
				else { x1=xx;x2=x-xx; }
	if (cy<yy)   { y1=y;y2=yy-y; }
				else { y1=yy;y2=y-yy; }
	oldpen=(HPEN)SelectObject(hdc,hpn);
	LineTo( hdc,xx,yy );
	SelectObject(hdc,oldpen);
	Send( x1,y1,x2+1,y2+1 );
	cx=xx;cy=yy;
}


void Bmscr::Circle( int x1,int y1,int x2,int y2, int mode )
{
	//		circle
	//		mode: 0=outline/1=fill
	//
	HPEN oldpen;
	HBRUSH old;
	int ix,iy;
	oldpen = (HPEN)SelectObject(hdc,hpn);
	if ( mode ) {
		old = (HBRUSH)SelectObject( hdc, hbr );
	} else {
		old = (HBRUSH)SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
	}
	::Ellipse( hdc,x1,y1,x2,y2 );
	if ( mode ) SelectObject( hdc, old );
	SelectObject( hdc, oldpen );
	ix = x1; iy = y1;
	if (x2<x1) ix=x2;
	if (y2<y1) iy=y2;
	Send( ix,iy,abs(x2-x1)+1,abs(y2-y1)+1 );
}


/*----------------------------------------------------------------*/

void Bmscr::Blt( int mode, Bmscr *src, int xx, int yy, int asx, int asy )
{
	//		fast blt
	//
	BYTE *p;
	BYTE *p2;
	int *palpt;
	int a,b,bsx,bsy,tofs,sofs,pand;
	int vv,cx,cy,sx,sy;
	int ssx, ssy, ssx2;
	BYTE a1,a2,a3;

	ssx = src->sx; ssy = src->sy; ssx2 = src->sx2;

	cx=this->cx;sx=asx;bsx=this->sx;
	if (xx<0) { sx+=xx;cx-=xx;xx=0; }
	if ((xx+sx)>=ssx) sx-=(xx+sx)-ssx;
	if (cx>bsx) return;
	if (cx<0) {
		sx=cx+sx;
		if (sx<=0) return;
		xx-=cx;cx=0;
	}
	vv=bsx-sx;
	if (cx>vv) { sx-=cx-vv; }
	
	cy=this->cy;sy=asy;bsy=this->sy;
	if (yy<0) { sy+=yy;cy-=yy;yy=0; }
	if ((yy+sy)>=ssy) sy-=(yy+sy)-ssy;
	if (cy>bsy) return;
	if (cy<0) {
		sy=cy+sy;
		if (sy<=0) return;
		yy-=cy;cy=0;
	}
	vv=bsy-sy;
	if (cy>vv) { sy-=cy-vv; }

	p=(BYTE *)this->pBit;
	p+=(bsy-cy-1) * this->sx2;
	p2=(BYTE *)(src->pBit);
	p2+=(ssy-yy-1)*ssx2;

	if (!palmode) goto blt24;

	p+=cx; p2+=xx;
	tofs = this->sx2 + sx;
	sofs = ssx2 + sx;

	if (mode==2) goto tblt;

	for(b=0;b<sy;b++) {
		for(a=0;a<sx;a++) {
			*p++=*p2++;
		}
		p-=tofs;p2-=sofs;
	}
	return;

tblt:
	palpt=(int *)this->pal;pand=0xffffff;
	for(b=0;b<sy;b++) {
		for(a=0;a<sx;a++) {
			a1=*p2++;
			if (palpt[a1]&pand) *p++=a1; else p++;
		}
		p-=tofs;p2-=sofs;
	}
	return;

blt24:
	p+=cx*3; p2+=xx*3;
	tofs = this->sx2 + (sx*3);
	sofs = ssx2 + (sx*3);

	switch( mode ) {
	case 1:
		break;
	case 2:
		//		FC trans copy
		for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				a1=*p2++;a2=*p2++;a3=*p2++;
				if (a1|a2|a3) {
					*p++=a1;*p++=a2;*p++=a3;
				} else {
					p+=3;
				}
			}
			p-=tofs;p2-=sofs;
		}
		return;
	case 3:
		{
		// 半透明コピー(標準)
		short srcht,dstht,ha;
		srcht = this->gfrate;
		if ( srcht>256 ) srcht=256;
		if ( srcht<=0 ) return;
		dstht = 256-srcht;
		if ( dstht == 0 ) break;
		  sx*=3;
		  for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				ha=(((short)*p)*dstht>>8) + (((short)*p2++)*srcht>>8);
				if (ha>255) ha=255;
				*p++=(BYTE)ha;
			}
			p-=tofs;p2-=sofs;
		  }
		}
		return;
	case 4:
		{
		// 半透明コピー(カラー除外)
		short ha,ha1,ha2,ha3,cl1,cl2,cl3;
		short srcht,dstht;
		srcht = this->gfrate;
		if ( srcht>256 ) srcht=256;
		if ( srcht<=0 ) return;
		dstht = 256-srcht;

		cl3 = (short)( this->color & 0xff );
		  cl2 = (short)( (this->color>>8) & 0xff );
		  cl1 = (short)( (this->color>>16) & 0xff );
		  // 0x00bbggrr 
		  for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				ha1=(short)*p2++;ha2=(short)*p2++;ha3=(short)*p2++;
				if ( (ha1!=cl1)||(ha2!=cl2)||(ha3!=cl3)) {
					ha=(((short)*p)*dstht>>8) + (ha1*srcht>>8);
					if (ha>255) ha=255;
					*p++=(BYTE)ha;
					ha=(((short)*p)*dstht>>8) + (ha2*srcht>>8);
					if (ha>255) ha=255;
					*p++=(BYTE)ha;
					ha=(((short)*p)*dstht>>8) + (ha3*srcht>>8);
					if (ha>255) ha=255;
					*p++=(BYTE)ha;
				}
				else {
					p+=3;
				}
			}
			p-=tofs;p2-=sofs;
		  }
		}
		return;
	case 5:
		{
		// 色加算コピー
		short srcht,ha;
		srcht = this->gfrate;
		if ( srcht>256 ) srcht=256;
		if ( srcht<=0 ) return;
		sx*=3;
		  for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				ha=((short)*p) + (((short)*p2++)*srcht>>8);
				if (ha>255) ha=255;
				*p++=(BYTE)ha;
			}
			p-=tofs;p2-=sofs;
		  }
		}
		return;
	case 6:
		{
		// 色減算コピー
		short srcht,ha;
		srcht = this->gfrate;
		if ( srcht>256 ) srcht=256;
		if ( srcht<=0 ) return;
		sx*=3;
		  for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				ha=((short)*p) - (((short)*p2++)*srcht>>8);
				if (ha<0) ha=0;
				*p++=(BYTE)ha;
			}
			p-=tofs;p2-=sofs;
		  }
		}
		return;
	case 7:
		{
		// ピクセルアルファブレンドコピー
		int sx2;
		short srcht,ha;
		  sx *= 3;
		  sx2 = asx*3;
		  for(b=0;b<sy;b++) {
			for(a=0;a<sx;a++) {
				srcht=(short)( *(p2+sx2) );
				if ( srcht!=0 ) {
					if ( srcht==255 ) {
						*p = *p2;
					} else {
						ha=(((short)*p)*(255-srcht)>>8) + (((short)*p2)*srcht>>8);
						*p=(BYTE)ha;
					}
				}
				p++;p2++;
			}
			p-=tofs;p2-=sofs;
		  }
		}
		return;
	}

	//		FC normal copy
	sx*=3;
	for(b=0;b<sy;b++) {
		for(a=0;a<sx;a++) {
			*p++=*p2++;
		}
		p-=tofs;p2-=sofs;
	}
	return;
}


int Bmscr::Copy( Bmscr *src, int xx, int yy, int psx, int psy )
{
	//		copy
	//
	switch( gmode ) {
	case 0:
		BitBlt( hdc, cx, cy, psx, psy, src->hdc, xx, yy, SRCCOPY );
		break;
	case 1:
	case 2:
		if ( palmode != src->palmode ) return 1;
		Blt( gmode, src, xx, yy, psx, psy );
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		if (( palmode==BMSCR_PALMODE_PALETTECOLOR )|( src->palmode == BMSCR_PALMODE_PALETTECOLOR )) return 1;
		Blt( gmode, src, xx, yy, psx, psy );
		break;
	}
	Send( cx, cy, psx, psy );
	return 0;
}


int Bmscr::Zoom( int dx, int dy, Bmscr *src, int xx, int yy, int psx, int psy, int mode )
{
	//		zoom
	//		(mode:0=normal/1=halftone)
	//
	int i;
	if ( mode ) i = HALFTONE; else i = COLORONCOLOR;
	SetStretchBltMode( hdc, i );
	StretchBlt(  hdc, cx, cy, dx, dy,
				 src->hdc, xx, yy, psx, psy, SRCCOPY );
	Send( cx, cy, dx, dy );
	return 0;
}


int Bmscr::BmpSave( char *fname )
{
	//		save BMP,DIB file
	//
	BITMAPFILEHEADER bmFH;
	FILE *fp;
	int bsize;
	HSPAPICHAR *hactmp1 = 0;
	bsize = bmpsize;
	fp=_tfopen( chartoapichar(fname,&hactmp1), TEXT("wb") );
	freehac(&hactmp1);
	if (fp==NULL) return -1;
	bmFH.bfType = 0x4d42;
	bmFH.bfSize = (infsize) + (bsize) + sizeof(BITMAPFILEHEADER);
	bmFH.bfOffBits = (infsize) + sizeof(BITMAPFILEHEADER);
	bmFH.bfReserved1 = 0;
	bmFH.bfReserved2 = 0;
	fwrite( &bmFH, 1, sizeof(BITMAPFILEHEADER), fp );
	fwrite( pbi, 1, infsize, fp );
	fwrite( pBit, 1, bsize, fp );
	fclose(fp);
	return 0;
}


void Bmscr::SetHSVColor( int hval, int sval, int vval )
{
	//		hsvによる色指定
	//			h(0-191)/s(0-255)/v(0-255)
	//
	int h,s,v;
	int save_r, save_g, save_b;
	int t,i,v1,v2,v3;
	int mv=8160;		// 255*32
	int mp=4080;		// 255*16
	//		overflow check
	//
	v = vval & 255;
	s = sval & 255;		// /8
	//		hsv -> rgb 変換
	//
	h = hval % 192;
	i = h/32;
	t = h % 32;
	v1 = (v*(mv-s*32) 	+mp)/mv;
	v2 = (v*(mv-s*t) 	+mp)/mv;
	v3 = (v*(mv-s*(32-t))+mp)/mv;
	switch(i){
		case 0:
		case 6:
				save_r=v;	save_g=v3;	save_b=v1;	break;
		case 1:
				save_r=v2;	save_g=v;	save_b=v1;	break;
		case 2:
				save_r=v1;	save_g=v;	save_b=v3;	break;
		case 3:
				save_r=v1;	save_g=v2;	save_b=v;	break;
		case 4:
				save_r=v3;	save_g=v1;	save_b=v;	break;
		case 5:
				save_r=v;	save_g=v1;	save_b=v2;	break;
	}
	Setcolor( save_r, save_g, save_b );
}


void Bmscr::GetClientSize( int *xsize, int *ysize )
{
	//		ウィンドウのクライアント領域のサイズを求める
	//
	RECT rw;
	GetClientRect( hwnd, &rw );
	*xsize = rw.right;
	*ysize = rw.bottom;
}


void Bmscr::SetScroll( int xbase, int ybase )
{
	//		スクロール基点を設定
	//
	int ax,ay, _vx, _vy;
	_vx = viewx;
	_vy = viewy;
	viewx = xbase;
	if ( viewx < 0 ) viewx = 0;
	viewy = ybase;
	if ( viewy < 0 ) viewy = 0;

	GetClientSize( &ax, &ay );
	if ( ( viewx + ax ) > sx ) viewx = sx - ax;
	if ( ( viewy + ay ) > sy ) viewy = sy - ay;

	if (( _vx != viewx )||( _vy != viewy )) Update();
}


void Bmscr::CnvRGB16( PTRIVERTEX target, DWORD src )
{
	//		RGBAコードをTRIVERTEXのRGB16コードに変換して設定する
	//
	target->Alpha = (COLOR16)((src>>16) & 0xff00);
	target->Red   = (COLOR16)((src>>8) & 0xff00);
	target->Green = (COLOR16)((src) & 0xff00);
	target->Blue  = (COLOR16)((src<<8) & 0xff00);
}


void Bmscr::GradFill( int x, int y, int sx, int sy, int mode, DWORD col1, DWORD col2 )
{

#ifndef HSP_COMPACT
	//		グラデーション塗りつぶし
	//
	TRIVERTEX axis[2];
	PTRIVERTEX vtx;
	static int grad_rect[2] = { 0, 1 };

	vtx = &axis[1];
	vtx->x = x + sx; vtx->y = y + sy;
	CnvRGB16( vtx, col2 );

	vtx = &axis[0];
	vtx->x = x; vtx->y = y;
	CnvRGB16( vtx, col1 );

	GradientFill( hdc, axis, 2, &grad_rect, 1, mode );
	Send( x,y,sx,sy );

#else

	throw HSPERR_ILLEGAL_FUNCTION;

#endif

}


int Bmscr::GetAlphaOperation( void )
{
	//		gmodeのモードをHGIMG互換のAlphaOperationに変換する
	//
	int alpha;
	alpha = gfrate;
	if ( alpha < 0 ) alpha = 0;
	if ( alpha > 255 ) {
		alpha = 256;
		if ( gmode >= 4 ) alpha = 255;
	}
	switch( gmode ) {
	case 3:					// 半透明blend
		break;
	case 4:					// 半透明blend+透明色
		break;
	case 5:					// 色加算
		alpha |= 0x200;
		break;
	case 6:					// 色減算
		alpha |= 0x300;
		break;
	default:
		alpha = 0x100;		// 標準
		break;
	}
	return alpha;
}


void Bmscr::GradFillEx( int *vx, int *vy, int *vcol )
{

#ifndef HSP_COMPACT
	//		グラデーション塗りつぶし(gsquare用)
	//		Windows 2000以降
	TRIVERTEX axis[4];
	PTRIVERTEX vtx;
	static GRADIENT_TRIANGLE grad_square[2] = { {0, 1, 2}, {0, 2, 3} };		// 右上、左下 (正の値の場合)
	int i;
	int minx,miny,maxx,maxy, ax,ay;
	minx = sx;
	miny = sy;
	maxx = maxy = 0;
	for(i=0;i<4;i++) {
		vtx = &axis[i];
		vtx->x = vx[i];
		if ( vtx->x < minx ) { minx = vtx->x; }
		if ( vtx->x > maxx ) { maxx = vtx->x; }
		vtx->y = vy[i];
		if ( vtx->y < miny ) { miny = vtx->y; }
		if ( vtx->y > maxy ) { maxy = vtx->y; }
		CnvRGB16( vtx, (DWORD)vcol[i] );
	}
	GradientFill( hdc, axis, 4, &grad_square, 2, GRADIENT_FILL_TRIANGLE );
	ax = maxx - minx + 1; ay = maxy - miny + 1;
	if (( ax > 0 )&&( ay > 0 )) { Send( minx,miny,ax,ay ); }

#else

	throw HSPERR_ILLEGAL_FUNCTION;

#endif

}


void Bmscr::SetCelDivideSize( int new_divsx, int new_divsy, int new_ofsx, int new_ofsy )
{
	//		セル分割サイズを設定
	//
	if ( new_divsx > 0 ) divsx = new_divsx; else divsx = sx;
	if ( new_divsy > 0 ) divsy = new_divsy; else divsy = sy;
	divx = sx / divsx;
	divy = sy / divsy;
	celofsx = new_ofsx;
	celofsy = new_ofsy;
}


int Bmscr::CelPut( Bmscr *src, int id )
{
	//		セルをコピー
	//
	int x,y,srcsx,srcsy;
	int bak_cx, bak_cy, res;
	srcsx = src->divsx;
	srcsy = src->divsy;
	x = ( id % src->divx ) * srcsx;
	y = ( id / src->divx ) * srcsy;
	bak_cx = cx + srcsx;
	bak_cy = cy;
	cx -= src->celofsx;
	cy -= src->celofsy;
	res = Copy( src, x, y, srcsx, srcsy );
	cx = bak_cx;
	cy = bak_cy;
	return res;
}


int Bmscr::RenderAlphaBitmap( int t_psx, int t_psy, int components, unsigned char *src )
{
	//		Alpha付きbitmapデータを描画する(stb_image用)
	//
	int a,b,x,y,x2,y2;
	int psx,psy;
	BYTE *p;
	BYTE *p2;
	BYTE a1,a2,a3,a4,a4r;
	int p_ofs, p2_ofs;

	x = this->cx;
	y = this->cy;
	x2 = 0; y2 = 0;
	psx = t_psx;
	psy = t_psy;

	if ( x > this->sx ) return -1;
	if ( y > this->sy ) return -1;
	if ( x < 0 ) { x2 = -x; x = 0; psx -= x2; }
	if ( y < 0 ) { y2 = -y; y = 0; psy -= y2; }
	if ( (x+psx)>this->sx ) psx = this->sx - x;
	if ( (y+psy)>this->sy ) psy = this->sy - y;

	//Alertf( "(%d,%d)(%d,%d)%d",x,y,psx,psy,components );

	p=(BYTE *)this->pBit;
	p+=x*3;
	p+=(this->sy-y-1) * this->sx2;
	p2=(BYTE *)src;
	p2+=x2 * 4;
	p2+=t_psx * 4 * y2;

	p_ofs = ( this->sx2 ) + ( psx * 3 );
	p2_ofs = ( t_psx * 4 ) - ( psx * 4 );

	if ( components < 4 ) {
		//		24bit normal copy
		for(b=0;b<psy;b++) {
			for(a=0;a<psx;a++) {
				a1=*p2++;a2=*p2++;a3=*p2++;p2++;
				*p++=a3;
				*p++=a2;
				*p++=a1;
			}
			p-=p_ofs;
			p2+=p2_ofs;
		}
		return 0;
	}

	//		32bit copy
	short ha;
	for(b=0;b<psy;b++) {
		for(a=0;a<psx;a++) {
			a1=*p2++;a2=*p2++;a3=*p2++;a4=*p2++;a4r=255-a4;
			if ( a4r == 0 ) {
				*p++=a3;
				*p++=a2;
				*p++=a1;
			} else {
				ha=((((short)*p)* a4r)>>8) + (((short)a3)*a4>>8);
				*p++=(BYTE)ha;
				ha=((((short)*p)* a4r)>>8) + (((short)a2)*a4>>8);
				*p++=(BYTE)ha;
				ha=((((short)*p)* a4r)>>8) + (((short)a1)*a4>>8);
				*p++=(BYTE)ha;
			}
		}
		p-=p_ofs;
		p2+=p2_ofs;
	}

	return 0;
}


