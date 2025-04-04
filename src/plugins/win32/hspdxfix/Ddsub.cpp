//
// ddsub.cpp
//
#define	DIRECTDRAW_VERSION 0x0700

#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <malloc.h>
#include <memory.h>
#include <mmsystem.h>

//
//	structures
//
typedef struct _DDraw
{
	HWND					hwndApp;			// 使用するウィンドウ
	LPDIRECTDRAW7			lpdd;				// DirectDrawオブジェクトを指すポインタ
	LPDIRECTDRAWSURFACE7	lpFrontBuffer;		// フロントバッファを指すポインタ
	LPDIRECTDRAWSURFACE7	lpBackBuffer;		// バックバッファを指すポインタ
	BOOL					fEnabled;			// DirectDrawが使用できる状態
	int						nCooperation;		// 協調レベル
	int						cx;					// ウィンドウの幅
	int						cy;					// ウィンドウの高さ
	int						bpp;				// ピクセル当たりのビット数
	int						nBuffers;			// 作成するバッファの個数
} DDraw, *LPDDraw;

LPDIRECTDRAWCLIPPER lpClipper = NULL;

//
// prototypes
//
LPDIRECTDRAW7			DDCreate(HWND hwndApp, int wx, int wy, int bpp );
void					DDDestroy();
BOOL					DDEnable();
void					DDDisable();
LPDIRECTDRAWSURFACE7	DDGetFrontBuffer();
LPDIRECTDRAWSURFACE7	DDGetBackBuffer();
DWORD					DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb);
int						DDGetWidth();
int						DDGetHeight();
BOOL					DDSetCooperativeLevel(int level);
int						DDGetCooperativeLevel();
BOOL					DDFlip(HWND hdum);
void					release_allbuffer();

//
// DDrawのデータのポインタ
//
LPDDraw lpdraw = NULL;

//
// スクリーンに関する情報
//
static char		wflag;
static int		xsize;
static int		ysize;
static char		winbpp;
static char		fullbpp;
static char		dxflag;
static char		sysmemflag;
static char		screen_saver=0;
static POINT	cursor,ss_cursor;
#define	MAX_SURF	64

//
FARPROC org_proc;
static HWND hdum;

// debug mode
#ifdef DEBUG_BUILD
void DEBUG(const char *txt){
	FILE	*debugtxt;
	if((debugtxt=fopen("DEBUG.TXT","a"))!=NULL){
		fprintf(debugtxt,"%s\n",txt);
		fclose(debugtxt);
	}
}
#endif

//
// 前方参照
//
static BOOL DDCreateSurfaces();

LPDIRECTDRAW7 DDCreate(HWND hwndApp, int wx, int wy, int bpp )
{
	HRESULT hr;

	if( (lpdraw = (LPDDraw)malloc(sizeof(DDraw))) == NULL ){
#ifdef DEBUG_BUILD
		DEBUG("DDcreate : lpdrawの確保に失敗");
#endif
		return NULL;
	}
	memset(lpdraw,0,sizeof(DDraw));

	//
	// DirectDrawオブジェクトを作成します
	//
    hr = DirectDrawCreateEx( NULL, (void **)&lpdraw->lpdd, IID_IDirectDraw7, NULL );
	if( hr == DD_OK )
	{
		lpdraw->hwndApp = hwndApp;
		//
		// デフォルト値に初期化します
		//
		lpdraw->cx = wx;
		lpdraw->cy = wy;
		lpdraw->bpp = bpp;
		lpdraw->nBuffers = 2;
	} else {
#ifdef DEBUG_BUILD
		DEBUG("DDCreate : DDオブジェクト作成に失敗");
#endif
		free(lpdraw);
		lpdraw = NULL;
		return NULL;
	}
	return lpdraw->lpdd;
}

void DDDestroy()
{
	if( lpdraw == NULL )
		return;
	if( lpdraw->lpdd != NULL )
		lpdraw->lpdd->Release();
	free(lpdraw);
	lpdraw = NULL;
}


BOOL DDEnable()
{
	HRESULT hr;

	if( lpdraw == NULL ){
#ifdef DEBUG_BUILD
		DEBUG("DDEnable : DDオブジェクトが作成されていない");
#endif
		return FALSE;
	}

	hr = lpdraw->lpdd->SetCooperativeLevel( lpdraw->hwndApp, lpdraw->nCooperation );
	if( hr != DD_OK ){
#ifdef DEBUG_BUILD
		DEBUG("DDEnable : 協調レベルの設定に失敗");
#endif
		return FALSE;
	}

	if(wflag==FALSE){
		hr = lpdraw->lpdd->SetDisplayMode( lpdraw->cx, lpdraw->cy, lpdraw->bpp,0,0 );
		if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDEnable : 解像度の変更に失敗");
#endif
			return FALSE;
		}
	}

	return DDCreateSurfaces();
}

void DDDisable()
{
	if( lpdraw == NULL )
		return;

	if( lpdraw->nCooperation & DDSCL_EXCLUSIVE ){
		lpdraw->lpdd->RestoreDisplayMode();
	}

    if( lpdraw->lpFrontBuffer )
    {
		lpdraw->lpFrontBuffer->Release();
        lpdraw->lpFrontBuffer = NULL;
    }

	release_allbuffer();
}

int DDGetWidth()
{
	return lpdraw->cx;
}

int DDGetHeight()
{
	return lpdraw->cy;
}

BOOL DDSetCooperativeLevel(int level)
{
   BOOL fWasEnabled = lpdraw->fEnabled;

   if( fWasEnabled )
      DDDisable();

   lpdraw->nCooperation = level;

   if( fWasEnabled )
      DDEnable();
   return TRUE;
}

int DDGetCooperativeLevel()
{
	return lpdraw->nCooperation;
}

LPDIRECTDRAWSURFACE7	DDGetFrontBuffer()
{
	if( lpdraw == NULL )
		return NULL;
	return lpdraw->lpFrontBuffer;
}

LPDIRECTDRAWSURFACE7	DDGetBackBuffer()
{
	if( lpdraw == NULL )
		return NULL;
	return lpdraw->lpBackBuffer;
}

DWORD DDColorMatch(LPDIRECTDRAWSURFACE7 pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC2 ddsd;
    HRESULT hres;

    //
    //  GDI SetPixel()を使用してカラーマッチングを行います
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);             // 現在のピクセル値を保存します。
        SetPixel(hdc, 0, 0, rgb);               // 値を設定します。
        pdds->ReleaseDC(hdc);
    }

    //
    // サーフェイスをロックして、変換した色を読み取れるようにします
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;

    if (hres == DD_OK)
    {
		// DWORDを取得します
        dw  = *(DWORD *)ddsd.lpSurface;
        // bppにマスキングします
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;
        pdds->Unlock(NULL);
    }

    //
    //  元の色に戻します
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }

    return dw;
}


static BOOL DDCreateSurfaces()
{
	LPDIRECTDRAW7		lpdd = lpdraw->lpdd;
    HRESULT             hr;
    DDSURFACEDESC2       ddsd;
    DDSCAPS2             ddscaps;

    memset( (VOID *)&ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

	if(wflag==FALSE){
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = lpdraw->nBuffers-1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
				DDSCAPS_FLIP | DDSCAPS_COMPLEX;

		hr = lpdd->CreateSurface(
					&ddsd, 
					&lpdraw->lpFrontBuffer, 
					NULL );

		if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDcreateSurfaces : フロントバッファの作成に失敗");
#endif
			return FALSE;
		}

	    //
		// バックバッファを指すポインタを取得します
		//
		ZeroMemory(&ddscaps,sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		hr = lpdraw->lpFrontBuffer->GetAttachedSurface(
					&ddscaps,
					&lpdraw->lpBackBuffer );

		if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDcreateSurfaces : ポインタの取得に失敗");
#endif
			return FALSE;
		}else
			return TRUE;
	}else{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		hr = lpdd->CreateSurface(
					&ddsd, 
					&lpdraw->lpFrontBuffer, 
					NULL );

		if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDcreateSurfaces : フロントバッファの作成に失敗");
#endif
			return FALSE;
		}

		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth = xsize;
		ddsd.dwHeight = ysize;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		if(sysmemflag!=0)
			ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;

		hr = lpdd->CreateSurface(&ddsd, &lpdraw->lpBackBuffer, NULL);

		if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDcreateSurfaces : バックバッファの作成に失敗");
#endif
			return FALSE;
		}else
			return TRUE;
	}
}		


LPDIRECTDRAWSURFACE7 DDCreateSurface(DWORD width, DWORD height, BOOL sysmem )
{
    DDSURFACEDESC2			ddsd;
    HRESULT					hr;
    LPDIRECTDRAWSURFACE7	lpSurface;


    //
    // サーフェイスディスクリプタに値を設定
    //
    memset( &ddsd, 0, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;

    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	if(sysmemflag!=0)
		ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
	else{
		if (sysmem==1) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
		if (sysmem==2) ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	}

    ddsd.dwHeight = height;
    ddsd.dwWidth = width;

    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	if(winbpp==8){
		ddsd.dwFlags |= DDSD_PIXELFORMAT;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 8;}

    hr = lpdraw->lpdd->CreateSurface( &ddsd, &lpSurface, NULL );

	if( hr != DD_OK ){
#ifdef DEBUG_BUILD
			DEBUG("DDcreateSurface : サーフェスの作成に失敗");
#endif
		lpSurface = NULL;
	}
	return lpSurface;
}


BOOL DDFlip(HDC hdc,int vflag)
{
	HRESULT hr;

	if(wflag==FALSE){
		if(vflag==FALSE){
			hr = lpdraw->lpFrontBuffer->Flip(NULL,DDFLIP_WAIT);
		}else{
/*			RECT Src;
			SetRect(&Src,0,0,xsize,ysize);
			hr=lpdraw->lpFrontBuffer->BltFast(0,0,lpdraw->lpBackBuffer,&Src,
				DDBLTFAST_NOCOLORKEY | DDBLTFAST_WAIT);
*/
			hr=lpdraw->lpFrontBuffer->Flip(NULL,DDFLIP_NOVSYNC);
		}
	}else{
		RECT rcSrc, rcDst;
		GetClientRect(hdum, &rcDst);
		ClientToScreen(hdum,(POINT*)&rcDst.left);
		ClientToScreen(hdum,(POINT*)&rcDst.right);
		SetRect(&rcSrc, 0, 0, xsize, ysize);
		hr=lpdraw->lpFrontBuffer->Blt(
		&rcDst,lpdraw->lpBackBuffer, &rcSrc,
		DDBLT_WAIT,NULL);
	}

    return hr == DD_OK;
}


//
// FillBuffer --- サーフェイスを指定された色で塗りつぶす
//
BOOL FillBufferRect( LPDIRECTDRAWSURFACE7 lpSurface, LPRECT lprect, COLORREF color)
{
    DDBLTFX     ddbltfx;
    HRESULT     hr;
	DWORD		cc;

	cc = DDColorMatch(lpSurface,color);
    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = cc;

    hr = lpSurface->Blt( lprect,
                         NULL,
                         NULL,
					     DDBLT_COLORFILL | DDBLT_WAIT,
						 &ddbltfx);

	return hr == DD_OK;
}


//
// FillBuffer2 --- サーフェイスを指定された色で塗りつぶす
//
BOOL FillBufferRect2( LPDIRECTDRAWSURFACE7 lpSurface, LPRECT lprect, int color )
{
    DDBLTFX     ddbltfx;
    HRESULT     hr;

    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = (DWORD)color;

    hr = lpSurface->Blt( lprect,
                         NULL,
                         NULL,
					     DDBLT_COLORFILL | DDBLT_WAIT,
						 &ddbltfx);

	return hr == DD_OK;
}


int dd_getcaps( int id, int emu )
{
	int a;
	DDCAPS caps;
	DDCAPS ecap;
	DWORD *cpt;
	memset(&caps,0,sizeof(DDCAPS));
	caps.dwSize=sizeof(DDCAPS);
	ecap=caps;
	lpdraw->lpdd->GetCaps(&caps,&ecap);
	if (emu==0) cpt=(DWORD *)&caps;
			else cpt=(DWORD *)&ecap;
	a=(int)cpt[id];
	return a;
}


//-------------------------------------------------------------
//		Sync Timer Routines
//-------------------------------------------------------------

static int	timecnt;
static int	timecnt2;
static int	timeres;
static int	timerid;


//
// TimerFunc --- タイマーコールバック関数
//
static void CALLBACK TimerFunc( UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	timecnt++;
	timecnt2++;
}
//
// 高精度タイマーを開始します
//
static void StartTimer()
{
	TIMECAPS caps;
	timeGetDevCaps( &caps, sizeof(caps) );
	timeres=caps.wPeriodMin;

	timeBeginPeriod( caps.wPeriodMin );
	timerid =
      timeSetEvent( caps.wPeriodMin,
					caps.wPeriodMin,
					TimerFunc,
					0,
					(UINT)TIME_PERIODIC );
}

//
// 高精度タイマーを停止します
//
static void StopTimer()
{
	TIMECAPS caps;
	if( timerid != 0 )
	{
		timeKillEvent( timerid );
		timerid = 0;
		timeGetDevCaps( &caps, sizeof(caps) );
		timeEndPeriod( caps.wPeriodMin );
   }
}


//-------------------------------------------------------------
//		useful functions for Ddrawex.cpp
//-------------------------------------------------------------

static LPDIRECTDRAW7 lpdd;
static LPDIRECTDRAWPALETTE ddpal;
static PALETTEENTRY	pe[256];
static LPDIRECTDRAWSURFACE7	ddsurf[MAX_SURF];
static int	col_st,col_len;
static int	df_wid;
static int	dbg_p1;
static char ClassName[] = "HSPDX";

long FAR PASCAL DXProc( HWND hwnd, UINT message,
                            WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
        case WM_CREATE:
            break;

        case WM_DESTROY:
		case WM_KEYDOWN:
			// スクリーンセイバーモードなら終了
			if(screen_saver==TRUE)
				PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
			if(screen_saver==TRUE){
				GetCursorPos(&cursor);
				if((ss_cursor.x!=cursor.x)||(ss_cursor.y!=cursor.y)){
#ifdef DEBUG_BUILD
	char str[80];
	sprintf(str,"スクリーンセーバー終了。X=%d Y=%d",cursor.x,cursor.y);
	DEBUG(str);
#endif
					PostQuitMessage(0);
				}
			}
			break;

		case WM_SYSCOMMAND:
			// スクリーンセイバー抑制
			if(wParam==SC_SCREENSAVE){
				return 1;
			}
/*			if(wParam==SC_CLOSE){
				return 1;
			}
*/
			break;

        default:
			break;
    }

	//return DefWindowProc (hwnd,message,wParam,lParam);
    return(CallWindowProc((WNDPROC)org_proc,hwnd,message,wParam,lParam));

}//WindowProc


int dd_dwindow( HINSTANCE hInst,HWND ghwnd)
{
	if(wflag==FALSE){
/*	    HWND        hwnd;
	    WNDCLASS    wc;
		WNDPROC		wproc;

	    // 表示ウィンドウの登録と実現
	    // ◇register and realize our display window
		wproc=(WNDPROC)GetWindowLong(ghwnd,GWL_WNDPROC);
		wc.style = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc = wproc;
	    wc.cbClsExtra = 0;
	    wc.cbWndExtra = 0;
	    wc.hInstance = hInst;
	    wc.hIcon = NULL;
	    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	    wc.lpszMenuName = ClassName;
	    wc.lpszClassName = ClassName;
	    RegisterClass( &wc );

	    // GDIが呼び出されないように全画面ウィンドウを作成する
	    // ◇create a full screen window so that GDI won't ever be called
		hwnd = CreateWindowEx(
			WS_EX_TOPMOST,
			ClassName,
			ClassName,
			WS_POPUP,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),
			NULL,
			NULL,
			hInst,
			NULL );

		if( !hwnd ) return -1;
*/
//		hdum=ghwnd;
		// ウィンドウをフルスクリーン化する
		SetWindowLong(hdum,GWL_STYLE,WS_POPUP);
		MoveWindow(hdum,0,0,GetSystemMetrics(SM_CXSCREEN),
					GetSystemMetrics(SM_CYSCREEN),TRUE);

		ShowWindow (hdum, SW_MAXIMIZE);
		UpdateWindow( hdum );
		SetFocus( hdum );

		// 全画面ウィンドウをサブクラス化する
		org_proc=(FARPROC)GetWindowLong(hdum,GWL_WNDPROC);
		SetWindowLong(hdum,GWL_WNDPROC,(long)DXProc);

	}else{
		hdum=ghwnd;
		SetWindowLong(hdum,GWL_STYLE,WS_CAPTION | WS_MINIMIZEBOX | WS_POPUPWINDOW);
		int wxs=xsize+GetSystemMetrics(SM_CXFIXEDFRAME)*2;
		int wys=ysize+GetSystemMetrics(SM_CYFIXEDFRAME)*2+GetSystemMetrics(SM_CYCAPTION);
		if(SetWindowPos(hdum,HWND_TOP,0,0,wxs,wys,SWP_NOMOVE)==0)
			return -1;
		ShowWindow (hdum, SW_SHOWNORMAL);
		UpdateWindow( hdum );
		SetFocus(hdum);
		// ウィンドウをサブクラス化(する必要あるのか)
		org_proc=(FARPROC)GetWindowLong(hdum,GWL_WNDPROC);
		SetWindowLong(hdum,GWL_WNDPROC,(long)DXProc);

		lpdd->CreateClipper(0, &lpClipper, NULL);
		lpClipper->SetHWnd(0, hdum);
		lpdraw->lpFrontBuffer->SetClipper(lpClipper);
	}

	SetForegroundWindow(hdum);
	return 0;
}


int dd_ini( HWND hwnd, int wx, int wy, int bpp, char winmode, char sysflag )
{
	//
	// DirectDrawオブジェクトを作成し、データを初期化
	//
#ifdef DEBUG_BUILD
	DEBUG("--------------------\nHSPDXFIX実行開始");
#endif
	int a;
	if( (lpdd = DDCreate(hwnd,wx,wy,bpp)) == NULL ) return -1;

	//
	// ウィンドウハンドルを握る
	//
	hdum=hwnd;

	//
	// 画面解像度、モードを取得し、放り込む
	//
	wflag=winmode;
	sysmemflag=sysflag;
	xsize=wx;
	ysize=wy;
	if(wflag==TRUE){
		HWND wnd=GetDesktopWindow();
		HDC hdc=GetDC(wnd);
		if(hdc!=NULL){
			winbpp=GetDeviceCaps(hdc,BITSPIXEL);
			ReleaseDC(wnd,hdc);
		}
	}else
		winbpp=bpp;
	
	fullbpp=bpp;
	if((wflag==TRUE)&&(fullbpp!=8)&&(winbpp==8))//(fullbpp!=winbpp))
		return -2;

	//
	// フルスクリーンの排他モードに設定
	//
	if(wflag==FALSE)
		DDSetCooperativeLevel( DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX );
	else
		DDSetCooperativeLevel(DDSCL_NORMAL);

	//
	//  失敗したら終了
	//
	if( DDEnable() == FALSE ) {
		DDDestroy();
		return -2;
	}
	//
	//	デフォルト値の設定
	//
	df_wid=0;
	for(a=0;a<MAX_SURF;a++) {
		ddsurf[a]=NULL;
	}

	//
	//	Timer initalize
	//
	timecnt=0;
	timerid=0;
	StartTimer();
	dxflag=1;

	return 0;
}


void dd_bye( void )
{
	//
	//	Timer stop
	//
	StopTimer();

	//
	//	DDRAWを解放
	//
	DDDisable();
	DDDestroy();
	SetWindowLong(hdum,GWL_WNDPROC,(long)org_proc);

	//HWND wnd=GetDesktopWindow();
	//RedrawWindow(NULL,NULL,NULL,RDW_INTERNALPAINT|RDW_UPDATENOW|RDW_ALLCHILDREN);
	//UpdateWindow(wnd);

	dxflag=0;
#ifdef DEBUG_BUILD
	DEBUG("HSPDXFIX実行終了\n--------------------");
#endif
}


void dd_release( int wid )
{
	//
	//	Surfaceを解放する
	//
	LPDIRECTDRAWSURFACE7 lpBuffer;
	if(wid<0){
		release_allbuffer();
		return;
	}
	if(wid>=MAX_SURF)
		return;

	lpBuffer=ddsurf[wid];
	if (lpBuffer!=NULL) {				// 以前のbufferは解放する
		lpBuffer->Release();
		ddsurf[wid]=NULL;
	}
}


void dd_palarea( int cst, int clen )
{
	//
	//	palette範囲を設定
	//
	int a;
	col_st=cst;
	col_len=clen;
	if(wflag==FALSE){
		for(a=0;a<256;a++) {
			pe[a].peRed=0;
			pe[a].peGreen=0;
			pe[a].peBlue=0;
			pe[a].peFlags=PC_NOCOLLAPSE;
//			pe[a].peFlags=0;
		}
		pe[255].peRed=255;
		pe[255].peGreen=255;
		pe[255].peBlue=255;
	}else{
		for(a=0;a<10;a++){
			pe[a].peRed = (BYTE)a;
			pe[a].peGreen = 0;
			pe[a].peBlue = 0;
			pe[a].peFlags = PC_EXPLICIT;
			pe[a+246].peRed = (BYTE)a+246;
			pe[a+246].peGreen = 0;
			pe[a+246].peBlue = 0;
			pe[a+246].peFlags = PC_EXPLICIT;
		}
		for(a=10;a<246;a++){
			pe[a].peRed = (BYTE)a;
			pe[a].peGreen = (BYTE)a;
			pe[a].peBlue = (BYTE)a;
			pe[a].peFlags = PC_NOCOLLAPSE;
//			pe[a].peFlags = 0;
		}
	}
}


void dd_palreal( PALETTEENTRY *ptmp )
{
	//
	//	paletteを更新(realize)
	//
	int a;
	HRESULT hr;
	dbg_p1=0;
	if(wflag==TRUE)
		ptmp+=col_st;
	for(a=0;a<1000;a++) {
		hr=ddpal->SetEntries( 0,0,256,(LPPALETTEENTRY)ptmp );
		if (hr==DD_OK){
			return;
		}
		dbg_p1++;
		Sleep(1);
	}
}


void dd_palinit( void )
{
	//
	//	paletteを初期化
	//
	lpdraw->lpdd->CreatePalette( DDPCAPS_8BIT|DDPCAPS_ALLOW256,
			(LPPALETTEENTRY)&pe[0], &ddpal, NULL );
	DDGetFrontBuffer()->SetPalette(ddpal);
	dd_palreal(&pe[0]);
}


void dd_palset( int pal, int rval, int gval, int bval )
{
	//
	//	paletteを設定
	//
	int a;
	a=pal+col_st;
	if((a<0)||(a>255)) return;
	pe[a].peRed=rval;
	pe[a].peGreen=gval;
	pe[a].peBlue=bval;
	//pe[a].peFlags=PC_NOCOLLAPSE;
}


int dd_getdbg( void )
{
	//
	//	debug valueを取得
	//
	return dbg_p1;
}


BYTE addclump( BYTE v,int p )
{
	//		-256～+256 % palette adjust
	//
	int a,b,pp;
	a=(int)v;
	pp=p;
	if (pp>0) {
		if (pp>256) pp=256;
		b=((255-a)*pp)>>8;
		a+=b;
	}
	else {
		pp+=256;
		if (pp<0) pp=0;
		a=(a*pp)>>8;
	}
	if (a<0) a=0; else if (a>255) a=255;
	return (BYTE)a;
}

void dd_palfade( int val )
{
	//
	//	All palette fade
	//
	int a;
	BYTE *pstpt;
	BYTE a1,a2,a3;
	PALETTEENTRY ptmp[256];

	pstpt=(BYTE *)&pe[0];
	for(a=0;a<256;a++) {
		a1 = addclump( *pstpt++, val );
		a2 = addclump( *pstpt++, val );
		a3 = addclump( *pstpt++, val );
		pstpt++;
		ptmp[a].peRed   = a1;
		ptmp[a].peGreen = a2;
		ptmp[a].peBlue  = a3;
		ptmp[a].peFlags = 0;
	}
	dd_palreal(&ptmp[0]);
}


void dd_palsethsp(char *ctbl)
{
	RGBQUAD	*quad;
	int a,b;
	quad=(RGBQUAD *)ctbl;
	a=col_st;
	for(b=0;b<col_len;b++) {
		pe[a].peRed=quad[b].rgbRed;
		pe[a].peGreen=quad[b].rgbGreen;
		pe[a].peBlue=quad[b].rgbBlue;
//		pe[a].peFlags=PC_NOCOLLAPSE;
		a++;
	}
	dd_palreal(&pe[0]);
}


int dd_buffer( int wid, HDC shdc, int sx, int sy, int xpitch, char *ctbl, int sysmem,int trans,unsigned char *srcbuf,unsigned char pmflag)
{
	//
	//	HSPの画面と同じ内容を持つ
	//	DDRAWオフスクリーンバッファを作成
	//
	LPDIRECTDRAWSURFACE7 lpBuffer;
	DDSURFACEDESC2	desc;
	DDCOLORKEY		ddkey;
	RGBQUAD			*quad;
	int a,b;
	int x,y;
	int colkey;
	int tpflag;
	unsigned char *p;
	unsigned char *p2;
	unsigned char *buff;
	unsigned char *buff2;
	int *palpt;
	unsigned char gcflag=0;
	unsigned char tr,tg,tb;
	DWORD gmask,pixel,bdepth;
	int dist,neardist,rd,gd,bd,i,j,ncolor;
	unsigned char plt[256];

	//
	//	Palette設定
	//
	quad=(RGBQUAD *)ctbl;
	palpt=(int *)quad;
	tpflag=1;

	if((fullbpp==8)||(winbpp==8)){
		colkey=-1;							// 透明色コード
		a=col_st;
		switch(pmflag){

		case 1:
			//パレットを読み込まない
			for(b=0;b<col_len;b++){
				if(((pe[a].peRed | pe[a].peGreen | pe[a].peBlue)==0)&&(colkey==-1))
					colkey=a;
				a++;
			}
			break;

		case 2:
			//近似色を探す
			for(i=0;i<255;i++){
				ncolor=0;
				neardist=255*255*3;
				for(j=0;j<256;j++){
					rd=(int)quad[i].rgbRed-(int)pe[j].peRed;
					gd=(int)quad[i].rgbGreen-(int)pe[j].peGreen;
					bd=(int)quad[i].rgbBlue-(int)pe[j].peBlue;
					dist=rd*rd+gd*gd+bd*bd;
					if(dist<neardist){
						neardist=dist;
						ncolor=j;
					}
				}
				plt[i]=ncolor;
				if(((pe[ncolor].peRed | pe[ncolor].peGreen | pe[ncolor].peBlue)==0)&&(colkey==-1))
					colkey=ncolor;
			}
			break;

		case 0:
		default:
			//パレットを読み込む
			for(b=0;b<col_len;b++) {
				pe[a].peRed=quad[b].rgbRed;
				pe[a].peGreen=quad[b].rgbGreen;
				pe[a].peBlue=quad[b].rgbBlue;
		//		pe[a].peFlags=PC_NOCOLLAPSE;
				if ((palpt[b]&0xffffff)==0) if(colkey==-1) colkey=a;
				a++;
			}
			break;
		}

		if((trans>=0)&&(trans+col_st<=255))	colkey=trans+col_st;
		dd_palreal(&pe[0]);
		if (colkey==-1) tpflag=0;
	//	dd_palinit();
		if(winbpp!=8){
			tr=quad[colkey-col_st].rgbRed;
			tg=quad[colkey-col_st].rgbGreen;
			tb=quad[colkey-col_st].rgbBlue;
		}
	}else{
		tr=(trans>>16)&0xff;
		tg=(trans>>8 )&0xff;
		tb= trans     &0xff;
	}

	//
	//	新規バッファ作成
	//
	lpBuffer=ddsurf[wid];
	if (lpBuffer!=NULL) {				// 以前のbufferは解放する
		lpBuffer->Release();
		ddsurf[wid]=NULL;
	}
	lpBuffer=DDCreateSurface(sx,sy,sysmem);
	if ( lpBuffer==NULL ) return -1;
	ddsurf[wid]=lpBuffer;
	df_wid=wid;

	//
	//	新規バッファに画像をコピー
	//

	memset(&desc,0,sizeof(desc));
	desc.dwSize=sizeof(desc);
	if(lpBuffer->Lock( NULL,&desc,
			DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL )!=DD_OK)
			return -1;
	gmask=desc.ddpfPixelFormat.dwGBitMask;
	bdepth=desc.ddpfPixelFormat.dwRGBBitCount;
	p=(unsigned char *)desc.lpSurface;
	buff=(unsigned char *)srcbuf;
	if((winbpp==8)&&(fullbpp==8)){
		//8bit COLORの場合、サーフェスロックしてコピー
		for(y=0;y<sy;y++) {
			buff2=buff; p2=p;
			for(x=0;x<sx;x++) {
				if(pmflag!=2){
					*p2++=(*buff2)+col_st;
				}else{
					*p2++=plt[*buff2];
				}
				buff2++;
			}
			p+=desc.lPitch;
			buff+=xpitch;
		}
	}else gcflag=1;			//16bit以上ならGDIでコピー

/*
	//		color bar testing
	//
	p=(unsigned char *)desc.lpSurface;
	for(y=0;y<256;y++) {
	  p2=p;a=y>>4;
	  for(x=0;x<32;x++) {
		*p2++=(unsigned char)a;
	  }
	  p+=desc.lPitch;
	}
*/
	lpBuffer->Unlock(NULL);

	//16bit以上ならここでコピー
	if(gcflag==1){
		HDC dhdc;
		if(lpBuffer->GetDC(&dhdc)==DD_OK){
			BitBlt(dhdc,0,0,sx,sy,shdc,0,0,SRCCOPY);
			lpBuffer->ReleaseDC(dhdc);
		}else
			return -1;
	}

	//
	//	透過色を指定
	//
	if (tpflag) {
		if(winbpp==8){
			ddkey.dwColorSpaceLowValue=colkey;
			ddkey.dwColorSpaceHighValue=colkey;
		}else{
			if(trans==-1){
				//色を拾う
				memset(&desc,0,sizeof(desc));
				desc.dwSize=sizeof(desc);
				if(lpBuffer->Lock( NULL,&desc,
					DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL )!=DD_OK)
					return -1;
				p=(unsigned char *)desc.lpSurface;
				pixel=*p;
				lpBuffer->Unlock(NULL);
				ddkey.dwColorSpaceLowValue=pixel;
				ddkey.dwColorSpaceHighValue=pixel;
			}else{
				switch(bdepth){
				case 16:
					//16bit
					if(gmask==0x000003e0){
						//555の場合
						pixel =(tr&0xf8)<<7;
						pixel|=(tg&0xf8)<<2;
						pixel|= tb      >>3;
					}else{
						//565の場合
						pixel =(tr&0xf8)<<8;
						pixel|=(tg&0xfc)<<3;
						pixel|= tb      >>3;
					}
					ddkey.dwColorSpaceLowValue=pixel;
					ddkey.dwColorSpaceHighValue=pixel;
					break;
				
				case 24:
				case 32:
					//24or32bit
					ddkey.dwColorSpaceLowValue=RGB(tr,tg,tb);
					ddkey.dwColorSpaceHighValue=ddkey.dwColorSpaceLowValue;
					break;
				}
			}
		}
		lpBuffer->SetColorKey( DDCKEY_SRCBLT, &ddkey );
	}

	return 0;
}


LPDIRECTDRAWSURFACE7 getsurf( int id )
{
	if((wflag==TRUE)&&(id<0)) id=-2;
	if (id==-1) return lpdraw->lpFrontBuffer;
	if (id==-2) return lpdraw->lpBackBuffer;
	return ddsurf[id];
}

void release_allbuffer(void)
{
	int i;
	LPDIRECTDRAWSURFACE7 surf;

	for(i=0;i<MAX_SURF;i++){
		surf=getsurf(i);
		if(surf!=NULL){
			surf->Release();
			surf=NULL;
		}
	}
}

int dd_getbuf( int wid, HDC dhdc, int sx, int sy, int xpitch )
{
	//
	//	DDRAWバッファをHSPスクリーンにコピー
	//		wid=DDraw buffer ID (-1=front/-2=back)
	//
	LPDIRECTDRAWSURFACE7 lpBuffer;
//	DDSURFACEDESC2	desc;
	HDC shdc;
//	int x,y,lx,ly;
//	unsigned char *p;
//	unsigned char *p2;
//	unsigned char *buff;
//	unsigned char *buff2;

	//
	//	新規バッファに画像をコピー
	//
	lpBuffer=getsurf(wid);

	if ( lpBuffer->GetDC(&shdc) == DD_OK) {
		BitBlt(dhdc,0,0,sx,sy,shdc,0,0,SRCCOPY);
		lpBuffer->ReleaseDC(shdc);
	}
	return 0;
}


int dd_getwid( void )
{
	return df_wid;
}


int dd_flip( HDC hdc,int times, int vsync )
{
//	int b;
	int a=abs(times);
	int sflag=0;
/*	DWORD nowtime;
	static int sync;
	static int losstime;*/

//	if (a>0) {
//		b=0;
//		while(a>timecnt) {
//			if (b++>100) break;
//			Sleep(5);
//		}
//	}

	DDFlip(hdc,vsync);

/*	nowtime=timeGetTime()-sync+losstime;

	if(a>nowtime){
		Sleep(a-nowtime);
		if(times>0) sflag=1;
	}
*/
	if(a>timecnt){
		Sleep(a-timecnt);
		if(times>0) sflag=1;
	}
	a=timecnt;
	timecnt=0;
	if((a>times)&&(sflag==1))
		timecnt=a-times;
	a=timecnt2;
	timecnt2=0;

	/*b=timeGetTime();
	a=b-sync;
	sync=b;
	losstime=0;
	if((a>times)&&(sflag==1))
		losstime=a-times;*/
	return a;
}


//-------------------------------------------------------------

void dd_fcopy( int x, int y, int wid, LPRECT src )
{
	//		Rect copy (fast)
	//
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();
	lpBackBuffer->BltFast( x,y, ddsurf[wid], src,
//		DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY );
		DDBLTFAST_SRCCOLORKEY );
}

void dd_fcopyn( int x, int y, int wid, LPRECT src )
{
	//		Rect copy (fast)
	//
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();
	lpBackBuffer->BltFast( x,y, ddsurf[wid], src,
//		DDBLTFAST_WAIT );
		NULL );
}

int dd_copy( LPRECT dst, int wid, LPRECT src )
{
	//		Rect copy
	//
	HRESULT hr;
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();
	hr=lpBackBuffer->Blt( dst, ddsurf[wid], src,
//		DDBLT_WAIT | DDBLT_KEYSRC, NULL );
		DDBLT_KEYSRC, NULL );
	return hr!=DD_OK;
}


int dd_copyn( LPRECT dst, int wid, LPRECT src )
{
	//		Rect copy ( no transparent )
	//
	HRESULT hr;
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();
	hr=lpBackBuffer->Blt( dst, ddsurf[wid], src,
//		DDBLT_WAIT, NULL );
		NULL , NULL );
	return hr!=DD_OK;
}


int dd_bufcopy( int tarid, LPRECT dst, int srcid, LPRECT src, int opt )
{
	//		Rect copy ( each buf->buf )
	//
	HRESULT hr;
	LPDIRECTDRAWSURFACE7 lpSrcBuffer = getsurf(srcid);
	LPDIRECTDRAWSURFACE7 lpTarBuffer = getsurf(tarid);

	if (opt) {
		hr=lpTarBuffer->Blt( dst, lpSrcBuffer, src,
		//DDBLT_WAIT | DDBLT_KEYSRC, NULL );
		DDBLT_KEYSRC, NULL );
	}
	else {
		hr=lpTarBuffer->Blt( dst, lpSrcBuffer, src,
		//DDBLT_WAIT, NULL );
		NULL, NULL );
	}
	return hr!=DD_OK;
}


void dd_boxf( LPRECT lprect, COLORREF col )
{
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();

	//
	// Shouldn't be here if no buffers, but avoid GPFs anyway
	//
	if( lpBackBuffer == NULL ) return;

	//
	// バッファから前回の内容をクリアします
	//
	FillBufferRect(lpBackBuffer, lprect, col );
}


void dd_boxf2( LPRECT lprect, int col )
{
	LPDIRECTDRAWSURFACE7 lpBackBuffer = DDGetBackBuffer();
	if( lpBackBuffer == NULL ) return;
	FillBufferRect2(lpBackBuffer, lprect, col+col_st );
}


void dd_print( int x, int y, char *mes, int id )
{
	//		Rect copy
	//
	int a;
	HDC hdc;
	LPDIRECTDRAWSURFACE7 lpBackBuffer = getsurf(id);
	if ( lpBackBuffer->GetDC(&hdc) == DD_OK) {
		SetBkMode( hdc,TRANSPARENT );
		SetTextColor( hdc, RGB(255,255,255) );
		a=strlen(mes);
		TextOut( hdc, x, y , mes, a );
		lpBackBuffer->ReleaseDC(hdc);
	}
}


void dd_fprint( int x, int y, char *mes, HFONT fon, COLORREF col, int id )
{
	//		Rect copy
	//
	int a;
	HDC hdc;
	LPDIRECTDRAWSURFACE7 lpBackBuffer = getsurf(id);
	if ( lpBackBuffer->GetDC(&hdc) == DD_OK) {
		SetBkMode( hdc,TRANSPARENT );
		SetTextColor( hdc, col );
		SelectObject( hdc, fon );
		a=strlen(mes);
		TextOut( hdc, x, y , mes, a );
		lpBackBuffer->ReleaseDC(hdc);
	}
}

int dd_islost(int begin,int end)
{
	char lost=0;
	int i;
	LPDIRECTDRAWSURFACE7 surf;

	if(begin>end){
		int tmp=end;
		end=begin;
		begin=end;
	}
	if(begin<-2) begin=-2;
	if(end>=MAX_SURF) end=MAX_SURF-1;

	for(i=begin;i<=end;i++){
		surf=getsurf(i);
		if(surf==NULL) continue;
		if(surf->IsLost()==DDERR_SURFACELOST){
			//surf->Restore();
			lost=TRUE;
			break;
		}
	}
	if(lost==TRUE)
		lpdraw->lpdd->RestoreAllSurfaces();

	return lost;
}

void dd_saver(void)
{
	GetCursorPos(&ss_cursor);
#ifdef DEBUG_BUILD
	char str[80];
	sprintf(str,"スクリーンセーバーモードに設定。X=%d Y=%d",ss_cursor.x,ss_cursor.y);
	DEBUG(str);
#endif
	screen_saver=1;
}

//-------------------------------------------------------------

