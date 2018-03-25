/*--------------------------------------------------------
	HSP3dish main (gameplay3d)
									  2011/5  onitama
  --------------------------------------------------------*/

#include "gamehsp.h"

//using namespace gameplay;

#include <stdio.h>

#define _WIN32_DCOM 

#include <windows.h>
#include <mmsystem.h>
#include <string.h>
#include <objbase.h>
#include <commctrl.h>

//#define GP_USE_MEM_LEAK_DETECTION			// Memory Leak Check

#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "hsp3dish.h"
#include "../../hsp3/hsp3config.h"
#include "../../hsp3/strbuf.h"
#include "../../hsp3/hsp3.h"
#include "../hsp3gr.h"
#include "../supio.h"
#include "../hgio.h"
#include "../sysreq.h"
#include "../hsp3ext.h"
#include "../../hsp3/strnote.h"

#include "../../hsp3/win32gui/hsp3extlib.h"

#ifndef HSP_COM_UNSUPPORTED
#include "hspvar_comobj.h"
#include "hspvar_variant.h"
#endif

typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

/*----------------------------------------------------------*/

static Hsp3 *hsp = NULL;
static HSPCTX *ctx;
static HSPEXINFO *exinfo;								// Info for Plugins

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wx, hsp_wy, hsp_wd, hsp_ss;
static int drawflag;

static	HWND m_hWnd;

#ifndef HSPDEBUG
static int hsp_sscnt, hsp_ssx, hsp_ssy;
#endif

#ifdef HSPDEBUG
static HSP3DBGFUNC dbgwin;
static HSP3DBGFUNC dbgnotice;
static HINSTANCE h_dbgwin;
static HWND dbgwnd;
static HSP3DEBUG *dbginfo;
#endif

gamehsp *game;
gameplay::Platform *platform;

//-------------------------------------------------------------
//		Sync Timer Routines
//-------------------------------------------------------------

static int	timecnt;
static int	timer_period = -1;
static int	timerid = 0;

//
// TimerFunc --- タイマーコールバック関数
//
static void CALLBACK TimerFunc( UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	timecnt++;
}

//-------------------------------------------------------------
//		gameplay Log
//-------------------------------------------------------------

static std::string gplog;

extern "C" {
	static void logfunc( gameplay::Logger::Level level, const char *msg )
	{
		gplog += msg;
	}
}

//-------------------------------------------------------------
//		Multi-Touch Interface
//-------------------------------------------------------------

#ifndef WM_TOUCH

#define NID_MULTI_INPUT		0x40	// マルチタッチ可能フラグ
#define NID_READY		0x80		// タッチ入力可能フラグ

#define WM_GESTURE		0x0119
#define WM_TOUCH		0x0240
#define SM_DIGITIZER		94
#define SM_MAXIMUMTOUCHES	95

#define TOUCHEVENTF_MOVE	0x0001	//Movement has occurred. Cannot be combined with TOUCHEVENTF_DOWN.
#define TOUCHEVENTF_DOWN	0x0002	//The corresponding touch point was established through a new contact. Cannot be combined with TOUCHEVENTF_MOVE or TOUCHEVENTF_UP.
#define TOUCHEVENTF_UP	0x0004	//A touch point was removed.
#define TOUCHEVENTF_INRANGE	0x0008	//A touch point is in range. This flag is used to enable touch hover support on compatible hardware.Applications that do not want support for hover can ignore this flag.
#define TOUCHEVENTF_PRIMARY	0x0010	//Indicates that this TOUCHINPUT structure corresponds to a primary contact point. See the following text for more information on primary touch points.
#define TOUCHEVENTF_NOCOALESCE	0x0020	//When received using GetTouchInputInfo, this input was not coalesced.
#define TOUCHEVENTF_PALM	0x0080	//The touch event came from the user's palm.

typedef struct _TOUCHINPUT { 
LONG x; 
LONG y; 
HANDLE hSource; 
DWORD dwID; 
DWORD dwFlags; 
DWORD dwMask; 
DWORD dwTime; 
ULONG_PTR dwExtraInfo; 
DWORD cxContact; 
DWORD cyContact; 
} TOUCHINPUT;

#endif

static	int mt_flag;				// マルチタッチ初期化モード(1=マルチタッチ/0=NORMAL)
static	HMODULE h_user32;
static	bool (WINAPI *i_RegisterTouchWindow)( HWND, int ); 
static	bool (WINAPI *i_GetTouchInputInfo)( HANDLE, int, TOUCHINPUT *, int );
static	bool (WINAPI *i_CloseTouchInputHandle)( HANDLE ); 
static	TOUCHINPUT touchinput[BMSCR_MAX_MTOUCH];

static void	MTouchInit( HWND hwnd )
{
	int sysmet;
	mt_flag = 0;
	i_RegisterTouchWindow = NULL;
	i_GetTouchInputInfo = NULL;
	i_CloseTouchInputHandle = NULL;
	sysmet = GetSystemMetrics( SM_DIGITIZER );
	if (( sysmet & NID_READY ) == 0 ) return;
	if (( sysmet & NID_MULTI_INPUT ) == 0 ) return;
	h_user32 = GetModuleHandle("USER32.DLL");
	if ( h_user32 ) {
		i_RegisterTouchWindow = (bool (WINAPI *)( HWND, int )) GetProcAddress(h_user32, "RegisterTouchWindow" ); 
		i_CloseTouchInputHandle =(bool (WINAPI *)( HANDLE )) GetProcAddress(h_user32, "CloseTouchInputHandle" ); 
		i_GetTouchInputInfo = (bool (WINAPI *)( HANDLE, int, TOUCHINPUT *, int )) GetProcAddress(h_user32, "GetTouchInputInfo" ); 
		if ( i_RegisterTouchWindow ) {
			i_RegisterTouchWindow( hwnd, 0 );
			mt_flag = 1;
		}
	}
}

/*----------------------------------------------------------*/

#define MAX_INIFILE_LINESTR 1024

static	char *mem_inifile = NULL;
static	CStrNote *note_ini = NULL;
static	int lines_inifile;
static	char s_inifile[MAX_INIFILE_LINESTR];

static void	CloseIniFile( void )
{
	if ( mem_inifile != NULL ) {
		mem_bye( mem_inifile );
		mem_inifile = NULL;
	}
	if ( note_ini != NULL ) {
		delete note_ini;
		note_ini = NULL;
	}
}

static int	OpenIniFile( char *fname )
{
	CloseIniFile();
	mem_inifile = dpm_readalloc( fname );
	if ( mem_inifile == NULL ) return -1;
	note_ini = new CStrNote;
	note_ini->Select( mem_inifile );
	lines_inifile = note_ini->GetMaxLine();
	return 0;
}

static char *GetIniFileStr( char *keyword )
{
	int i;
	char *s;
	for(i=0;i<lines_inifile;i++) {
		note_ini->GetLine( s_inifile, i, MAX_INIFILE_LINESTR );
		if ( strncmp( s_inifile, keyword, strlen(keyword) ) == 0 ) {
			s = strchr2( s_inifile, '=' ) + 1;
			return s;
		}
	}
	return NULL;
}

static int	GetIniFileInt( char *keyword )
{
	char *s;
	s = GetIniFileStr( keyword );
	if ( s == NULL ) return 0;
	return atoi( s );
}

/*----------------------------------------------------------*/

LRESULT CALLBACK WndProc( HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam )
{
	switch (uMessage)
	{
	case WM_PAINT:
		//		Display 全描画
		//
		break;
/*
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
*/
	case WM_MOUSEMOVE:
		{
		Bmscr *bm;
		int x,y;
		if ( exinfo != NULL ) {
			bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
			x = LOWORD(lParam); y = HIWORD(lParam);
			bm->savepos[BMSCR_SAVEPOS_MOSUEX] = x;
			bm->savepos[BMSCR_SAVEPOS_MOSUEY] = y;
			bm->UpdateAllObjects();
			if ( bm->tapstat ) {
				if ( mt_flag ) {
					if( GetMessageExtraInfo() == 0 ) {
						bm->setMTouchByPointId( -1, x, y, true );
					}
				} else {
					bm->setMTouchByPointId( -1, x, y, true );
				}
			}
		}
		return 0;
		}
	case WM_MOUSELEAVE:
		{
		Bmscr *bm;
		if ( exinfo != NULL ) {
			bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
			bm->tapstat = 0;
			bm->savepos[BMSCR_SAVEPOS_MOSUEX] = -1;
			bm->savepos[BMSCR_SAVEPOS_MOSUEY] = -1;
			bm->UpdateAllObjects();
			if ( mt_flag ) {
				if( GetMessageExtraInfo() == 0 ) {
					bm->setMTouchByPointId( -1, -1, -1, false );
				}
			} else {
				bm->setMTouchByPointId( -1, -1, -1, false );
			}
		}
		break;
		}
	case WM_LBUTTONUP:
		{
		Bmscr *bm;
		if ( exinfo != NULL ) {
			bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
			bm->tapstat = 0;
			bm->UpdateAllObjects();
			if ( mt_flag ) {
				if( GetMessageExtraInfo() == 0 ) {
					bm->setMTouchByPointId( -1, -1, -1, false );
				}
			} else {
				bm->setMTouchByPointId( -1, -1, -1, false );
			}
		}
		break;
		}
	case WM_LBUTTONDOWN:
		{
		Bmscr *bm;
		if ( exinfo != NULL ) {
			bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
			bm->tapstat = 1;
			bm->UpdateAllObjects();
			if ( mt_flag ) {
				if( GetMessageExtraInfo() == 0 ) {
					bm->setMTouchByPointId( -1, bm->savepos[BMSCR_SAVEPOS_MOSUEX], bm->savepos[BMSCR_SAVEPOS_MOSUEY], true );
				}
			} else {
				bm->setMTouchByPointId( -1, bm->savepos[BMSCR_SAVEPOS_MOSUEX], bm->savepos[BMSCR_SAVEPOS_MOSUEY], true );
			}

			// WM_MOUSELEAVE メッセージの登録処理
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof( TRACKMOUSEEVENT );
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hwnd;
			tme.dwHoverTime = HOVER_DEFAULT;
			_TrackMouseEvent( &tme );
		}
		break;
		}

	case WM_TOUCH:
		{
		//			マルチタッチ入力の処理
		int i;
		bool res;
		Bmscr *bm;
		int num = LOWORD(wParam);
		HANDLE ti = (HANDLE)lParam;
		POINT touchpos;
		TOUCHINPUT *tdata;
		if ( exinfo == NULL ) break;
		bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
		if ( bm == NULL ) break;

		res = i_GetTouchInputInfo( ti, num, touchinput, sizeof(TOUCHINPUT) );
		if ( res ) {
			tdata = touchinput;
			for(i=0;i<num;i++) {
				touchpos.x = tdata->x / 100;
				touchpos.y = tdata->y / 100;
				ScreenToClient( hwnd, &touchpos );
				if ( tdata->dwFlags & ( TOUCHEVENTF_DOWN|TOUCHEVENTF_MOVE) ) {
					bm->setMTouchByPointId( tdata->dwID, touchpos.x, touchpos.y, true );
				} else {
					if ( tdata->dwFlags & TOUCHEVENTF_UP ) {
						bm->setMTouchByPointId( tdata->dwID, touchpos.x, touchpos.y, false );
					}
				}
				tdata++;
			}
			i_CloseTouchInputHandle( ti );
			return 0;
		}
		break;
		}

	case MM_MCINOTIFY:
		if ( wParam == MCI_NOTIFY_SUCCESSFUL ) {
			hsp3notify_extcmd();
		}
		return 0;


	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		{
		int id,retval;
		id = GetWindowLong( hwnd, GWL_USERDATA );
		if ( code_isirq( HSPIRQ_ONEXIT ) ) {
			int iparam = 0;
			if ( uMessage == WM_QUERYENDSESSION ) iparam++;
			retval = code_sendirq( HSPIRQ_ONEXIT, iparam, id, 0 );
			if ( retval == RUNMODE_INTJUMP ) retval = code_execcmd2();	// onexit goto時は実行してみる
			if ( retval != RUNMODE_END ) return 0;
			//break;
		}
		code_puterror( HSPERR_NONE );
		PostQuitMessage(0);
		return 0;
		//break;
		}

	}

	return DefWindowProc (hwnd, uMessage, wParam, lParam) ;
}


static void hsp3dish_initwindow( HINSTANCE hInstance, int sx, int sy, char *windowtitle )
{
	// Register the windows class
	WNDCLASS wndClass = { 0, WndProc, 0, 0, hInstance,
							LoadIcon( hInstance, MAKEINTRESOURCE(128) ),
							LoadCursor( NULL, IDC_ARROW ),
							(HBRUSH)GetStockObject(WHITE_BRUSH),
							NULL, "HSP3DishWindow" };
	RegisterClass( &wndClass );

	// Set the window's initial style
	DWORD m_dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | /* WS_THICKFRAME | */
						WS_MINIMIZEBOX | /*WS_MAXIMIZEBOX |*/ WS_VISIBLE | WS_CLIPCHILDREN;

	// Set the window's initial width
	RECT rc;
	SetRect( &rc, 0, 0, sx, sy );
	AdjustWindowRect( &rc, m_dwWindowStyle, false );

	// Create the render window
	m_hWnd = CreateWindow( "HSP3DishWindow", windowtitle, m_dwWindowStyle,
								CW_USEDEFAULT, CW_USEDEFAULT,
								(rc.right-rc.left), (rc.bottom-rc.top), 0,
								NULL, hInstance, 0 );

	// 描画APIに渡す
	hgio_init( 0, sx, sy, m_hWnd );
	hgio_clsmode( CLSMODE_SOLID, 0xffffff, 0 );

	// マルチタッチ初期化
	MTouchInit( m_hWnd );

	// HWNDをHSPCTXに保存する
	ctx->wnd_parent = m_hWnd;
}


void hsp3dish_dialog( char *mes )
{
	MessageBox( NULL, mes, "Error",MB_ICONEXCLAMATION | MB_OK );
}


#ifdef HSPDEBUG
char *hsp3dish_debug( int type )
{
	//		デバッグ情報取得
	//
	char *p;
	p = code_inidbg();

	switch( type ) {
	case DEBUGINFO_GENERAL:
//		hsp3gr_dbg_gui();
		code_dbg_global();
		break;
	case DEBUGINFO_VARNAME:
		break;
	case DEBUGINFO_INTINFO:
		break;
	case DEBUGINFO_GRINFO:
		break;
	case DEBUGINFO_MMINFO:
		break;
	}
	return p;
}
#endif


void hsp3dish_drawon( void )
{
	//		描画開始指示
	//
	if ( drawflag == 0 ) {
		hgio_render_start();
		drawflag = 1;
	}
}


void hsp3dish_drawoff( void )
{
	//		描画終了指示
	//
	if ( drawflag ) {
		hgio_render_end();
		drawflag = 0;
	}
}


int hsp3dish_debugopen( void )
{
	//		デバッグウインドゥ表示
	//
#ifdef HSPDEBUG
	if ( h_dbgwin != NULL ) return 0;
#ifdef HSP64
	h_dbgwin = LoadLibrary( "hsp3debug_64.dll" );
#else
	h_dbgwin = LoadLibrary("hsp3debug.dll");
#endif
	if ( h_dbgwin != NULL ) {
#ifdef HSP64
		dbgwin = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, "debugini");
		dbgnotice = (HSP3DBGFUNC)GetProcAddress( h_dbgwin, "debug_notice" );
#else
		dbgwin = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, "_debugini@16");
		dbgnotice = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, "_debug_notice@16");
#endif
		if ((dbgwin == NULL) || (dbgnotice == NULL)) h_dbgwin = NULL;
	}
	if ( h_dbgwin == NULL ) {
		hsp3dish_dialog( "No debug module." );
		return -1;
	}
	dbginfo->get_value = hsp3dish_debug;
	dbgwin( dbginfo, 0, 0, 0 );
	dbgwnd = (HWND)( dbginfo->dbgwin );
#endif
	return 0;
}


static void hsp3dish_dispatch( MSG *msg )
{
	TranslateMessage( msg );
	DispatchMessage( msg );

#ifndef HSPDEBUG
	//		スクリーンセーバー終了チェック
	//
	if ( ctx->hspstat & HSPSTAT_SSAVER ) {
		int x,y;
		if ( msg->message==WM_MOUSEMOVE ) {
			x = LOWORD(msg->lParam);
			y = HIWORD(msg->lParam);
			if ( hsp_sscnt == 0 ) {
				if (( hsp_ssx != x )||( hsp_ssy != y )) throw HSPERR_NONE;
			} else {
				hsp_ssx = x;
				hsp_ssy = y;
			}
		}
		if ( msg->message==WM_KEYDOWN ) {
			if ( hsp_sscnt == 0 ) throw HSPERR_NONE;
		}
	}
#endif

#if 0
	if ( msg->message==WM_KEYDOWN ) {	// ocheck onkey
		if ( msg->wParam == 9 ) {
//			hsp3gr_nextobj();
		}
		if ( code_isirq( HSPIRQ_ONKEY ) ) {
			code_sendirq( HSPIRQ_ONKEY, (int)MapVirtualKey( msg->wParam, 2 ), (int)msg->wParam, (int)msg->lParam );
		}
	}
#endif
}


int hsp3dish_wait( int tick )
{
	//		時間待ち(wait)
	//		(awaitに変換します)
	//
	if ( ctx->waitcount <= 0 ) {
		ctx->runmode = RUNMODE_RUN;
		return RUNMODE_RUN;
	}
	ctx->waittick = tick + ( ctx->waitcount * 10 );
	return RUNMODE_AWAIT;
}


int hsp3dish_await( int tick )
{
	//		時間待ち(await)
	//
	if ( ctx->waittick < 0 ) {
		if ( ctx->lasttick == 0 ) ctx->lasttick = tick;
		ctx->waittick = ctx->lasttick + ctx->waitcount;
	}
	if ( tick >= ctx->waittick ) {
		ctx->lasttick = tick;
		ctx->runmode = RUNMODE_RUN;
		return RUNMODE_RUN;
	}
	return RUNMODE_AWAIT;
}


void hsp3dish_msgfunc( HSPCTX *hspctx )
{
	MSG msg;
	int tick;

	while(1) {
		// logmes なら先に処理する
		if ( hspctx->runmode == RUNMODE_LOGMES ) {
			hspctx->runmode = RUNMODE_RUN;
#ifdef HSPDEBUG
			if ( h_dbgwin != NULL ) dbgnotice( dbginfo, 1, 0, 0 );		// Debug Window Notice
#endif
			return;
		}
		
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if (msg.message == WM_QUIT ) throw HSPERR_NONE;
			hsp3dish_dispatch( &msg );
			continue;
		}

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
#ifdef HSPDEBUG
			if ( h_dbgwin != NULL ) dbgnotice( dbginfo, 0, 0, 0 );		// Debug Window Notice
#endif
			while(1) {
				GetMessage( &msg, NULL, 0, 0 );
				if ( msg.message == WM_QUIT ) throw HSPERR_NONE;
				hsp3dish_dispatch( &msg );
				if ( hspctx->runmode != RUNMODE_STOP ) break;
			}

			MsgWaitForMultipleObjects(0, NULL, FALSE, 1000, QS_ALLINPUT );
			break;
		case RUNMODE_WAIT:
			tick = GetTickCount();
			hspctx->runmode = code_exec_wait( tick );
		case RUNMODE_AWAIT:
			if ( timer_period == -1 ) {

				//	通常のタイマー
				tick = GetTickCount();
				if ( code_exec_await( tick ) != RUNMODE_RUN ) {
					MsgWaitForMultipleObjects(0, NULL, FALSE, hspctx->waittick - tick, QS_ALLINPUT );
				} else {
#ifndef HSPDEBUG
					if ( ctx->hspstat & HSPSTAT_SSAVER ) {
						if ( hsp_sscnt ) hsp_sscnt--;
					}
#endif
				}
			} else {
				//	高精度タイマー
				tick = timeGetTime()+5;				// すこし早めに抜けるようにする
				if ( code_exec_await( tick ) != RUNMODE_RUN ) {
					MsgWaitForMultipleObjects(0, NULL, FALSE, hspctx->waittick - tick, QS_ALLINPUT );
				} else {
					tick = timeGetTime();
					while( tick < hspctx->waittick ) {	// 細かいwaitを取る
						Sleep(1);
						tick = timeGetTime();
					}
					hspctx->lasttick = tick;
					hspctx->runmode = RUNMODE_RUN;
#ifndef HSPDEBUG
					if ( ctx->hspstat & HSPSTAT_SSAVER ) {
						if ( hsp_sscnt ) hsp_sscnt--;
					}
#endif
				}
			}
			break;
//		case RUNMODE_END:
//			throw HSPERR_NONE;
		case RUNMODE_RETURN:
			throw HSPERR_RETURN_WITHOUT_GOSUB;
		case RUNMODE_INTJUMP:
			throw HSPERR_INTJUMP;
		case RUNMODE_ASSERT:
			hspctx->runmode = RUNMODE_STOP;
#ifdef HSPDEBUG
			hsp3dish_debugopen();
#endif
			break;
	//	case RUNMODE_LOGMES:
		default:
			return;
		}

	}
}


/*----------------------------------------------------------*/
//		デバイスコントロール関連
/*----------------------------------------------------------*/
static HSP3DEVINFO *mem_devinfo;
static int devinfo_dummy;

static int hsp3dish_devprm( char *name, char *value )
{
	return -1;
}

static int hsp3dish_devcontrol( char *cmd, int p1, int p2, int p3 )
{
	return -1;
}

static int *hsp3dish_devinfoi( char *name, int *size )
{
	devinfo_dummy = 0;
	*size = -1;
	return NULL;
//	return &devinfo_dummy;
}

static char *hsp3dish_devinfo( char *name )
{
	if ( strcmp( name, "name" )==0 ) {
		return mem_devinfo->devname;
	}
	if ( strcmp( name, "error" )==0 ) {
		return mem_devinfo->error;
	}
	return NULL;
}

static void hsp3dish_setdevinfo( HSP3DEVINFO *devinfo )
{
	//		Initalize DEVINFO
	mem_devinfo = devinfo;
#ifdef GP_USE_ANGLE
	devinfo->devname = "win32dx9";
#else
	devinfo->devname = "win32opengl";
#endif
	devinfo->error = "";
	devinfo->devprm = hsp3dish_devprm;
	devinfo->devcontrol = hsp3dish_devcontrol;
	devinfo->devinfo = hsp3dish_devinfo;
	devinfo->devinfoi = hsp3dish_devinfoi;
}

/*----------------------------------------------------------*/

static void hsp3dish_savelog( void )
{
	//		ログをファイルに出力する
	//
	if (game != NULL) {
		char fname[_MAX_PATH + 1];
		const char *logs;
#ifdef GP_USE_MEM_LEAK_DETECTION
		printMemoryLeaks();
#endif
		logs = gplog.c_str();
		GetModuleFileName(NULL, fname, _MAX_PATH);
		getpath(fname, fname, 32);
		changedir(fname);
		mem_save("hsp3gp.log", (void *)logs, (int)strlen(logs), -1);
	}
}

int hsp3dish_init( HINSTANCE hInstance, char *startfile )
{
	//		システム関連の初期化
	//		( mode:0=debug/1=release )
	//
	int a,orgexe, mode;
	int hsp_sum, hsp_dec;
	char a1;
	char fname[_MAX_PATH+1];
	char *ss;
#ifdef HSPDEBUG
	int i;
#endif

	if ( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED) ) ) {
		return 1;
	}
	OleInitialize( NULL );

	InitCommonControls();
	InitSysReq();
	
	SetSysReq( SYSREQ_MAXMATERIAL, 64 );		// マテリアルのデフォルト値

	game = NULL;
	platform = NULL;

	//		HSP関連の初期化
	//
	hsp = new Hsp3();
	hsp->hspctx.instance = (void *)hInstance;
#ifdef HSPDEBUG
	h_dbgwin = NULL;
	dbgwnd = NULL;
	m_hWnd = NULL;
	exinfo = NULL;

	ss = strsp_cmds( startfile );
	i = (int)( ss - startfile );
	ss = startfile;
	if ( ss[i-1] == 32 ) i--;
	if ( *ss == 0x22 ) {
		ss++;i-=2;
	}
	if ( i > 0 ) {
		strncpy( fname, ss, i );
		fname[i] = 0;
		hsp->SetFileName( fname );
	}
#else
	if ( startfile != NULL ) {
		hsp->SetFileName( startfile );
	}
#endif


	//		実行ファイルかデバッグ中かを調べる
	//
	mode = 0;
	orgexe=0;
	hsp_wx = 960;
	hsp_wy = 640;
//	hsp_wx = 320;
//	hsp_wy = 480;
	hsp_wd = 0;
	hsp_ss = 0;

	for( a=0 ; a<8; a++) {
		a1=optmes[a]-48;if (a1==fpas[a]) orgexe++;
	}
	if ( orgexe == 0 ) {
		mode = atoi(optmes+9) + 0x10000;
		a1=*(optmes+17);
		if ( a1 == 's' ) hsp_ss = HSPSTAT_SSAVER;
		hsp_wx=*(short *)(optmes+20);
		hsp_wy=*(short *)(optmes+23);
		hsp_wd=( *(short *)(optmes+26) );
		hsp_sum=*(unsigned short *)(optmes+29);
		hsp_dec=*(int *)(optmes+32);
		hsp->SetPackValue( hsp_sum, hsp_dec );
	}

	//		起動ファイルのディレクトリをカレントにする
	//
#ifndef HSPDEBUG
	if (( hsp_wd & 2 ) == 0 ) {
		GetModuleFileName( NULL, fname, _MAX_PATH );
		getpath( fname, fname, 32 );
		changedir( fname );
	}
#endif

	if ( hsp->Reset( mode ) ) {
		hsp3dish_dialog( "Startup failed." );
		return 1;
	}

#ifdef HSPDEBUG
	if ( OpenIniFile( "hsp3dish.ini" ) == 0 ) {
		hsp_wx = GetIniFileInt( "wx" );
		hsp_wy = GetIniFileInt( "wy" );
		CloseIniFile();
	}
#endif

	ctx = &hsp->hspctx;

	{
	//		コマンドライン関連
	ss = GetCommandLine();
	ss = strsp_cmds( ss );
#ifdef HSPDEBUG
	ss = strsp_cmds( ss );
#endif
	sbStrCopy( &ctx->cmdline, ss );					// コマンドラインパラメーターを保存
	}

	//		SSaver proc
	//
#if 0
	if ( hsp_ss ) {
		ss = GetCommandLine();
		ss = strsp_cmds( ss );
		hsp_sscnt = 30;
		a1=tolower(*(ss+1));
		if( FindWindow("oniwndp",NULL) != NULL ) {
			return 2;
		}
		if (a1=='p') {
			HWND s_hwnd;
			RECT rPic;
			s_hwnd = (HWND)atoi( ss+3 );
			GetWindowRect( s_hwnd, &rPic );
			hsp_wx = rPic.right-rPic.left;
			hsp_wy = rPic.bottom-rPic.top;
			hsp_wd = 0x100;
			ctx->wnd_parent = s_hwnd;
		}
		if (a1=='s') {
			ShowCursor(FALSE);
		} else {
			hsp_ss = 0;								// スクリーンセーバー時以外はモードOFF
		}
		ctx->hspstat |= hsp_ss;
	}
#endif

	//		Register Type
	//
	drawflag = 0;
	ctx->msgfunc = hsp3dish_msgfunc;

	//		Initalize Window
	//
#ifdef HSPDEBUG
	char * pc = "HGIMG4 ver" hspver;
#else
	char * pc = NULL;
#endif

	hsp3dish_initwindow( hInstance, hsp_wx, hsp_wy, pc );


#ifndef HSP_COM_UNSUPPORTED
	HspVarCoreRegisterType( TYPE_COMOBJ, HspVarComobj_Init );
	HspVarCoreRegisterType( TYPE_VARIANT, HspVarVariant_Init );
#endif

	//		Start Timer
	//
	// timerGetTime関数による精度アップ(μ秒単位)
	timer_period = -1;
	if (( ctx->hsphed->bootoption & HSPHED_BOOTOPT_NOMMTIMER ) == 0 ) {
		TIMECAPS caps;
		if ( timeGetDevCaps(&caps,sizeof(TIMECAPS)) == TIMERR_NOERROR ){
			// マルチメディアタイマーのサービス精度を最大に
			timer_period = caps.wPeriodMin;
			timeBeginPeriod( timer_period );
		}
	}

	//		Initalize external DLL System
	//
	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

	//		Initalize gameplay
	//
    game = new gamehsp;

	gameplay::Logger::set(gameplay::Logger::LEVEL_INFO, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_WARN, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_ERROR, logfunc);

//	platform = gameplay::Platform::create( game, NULL, hsp_wx, hsp_wy, false );
	platform = gameplay::Platform::create( game, m_hWnd, hsp_wx, hsp_wy, false );
	if ( platform == NULL ) {
		//hsp3dish_dialog( (char *)gplog.c_str() );
		hsp3dish_dialog( "OpenGL initalize failed." );
		hsp3dish_savelog();
		return 1;
	}
	platform->enterMessagePump();
	game->frame();

	//		Initalize GUI System
	//
	hsp3typeinit_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

	//		Initalize external DLL System
	//
	hsp3typeinit_dllcmd(code_gettypeinfo(TYPE_DLLFUNC));
	hsp3typeinit_dllctrl(code_gettypeinfo(TYPE_DLLCTRL));

	exinfo = ctx->exinfo2;

	//		Initalize DEVINFO
	HSP3DEVINFO *devinfo;
	devinfo = hsp3extcmd_getdevinfo();
	hsp3dish_setdevinfo( devinfo );
	hsp3extcmd_sysvars((int)hInstance, (int)m_hWnd, 0);

	gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "HGIMG4 %s initalized : %s\n", hspver, devinfo->devname);

#ifdef HSPDEBUG
	dbginfo = code_getdbg();
#endif
	return 0;
}


static void hsp3dish_bye(void)
{
	//		Window関連の解放
	//
	hsp3dish_drawoff();

	//		タイマーの開放
	//
	if (timer_period != -1) {
		timeEndPeriod(timer_period);
		timer_period = -1;
	}

	//		HSP関連の解放
	//
	if (hsp != NULL) { delete hsp; hsp = NULL; }

	if (m_hWnd != NULL) {
		hgio_term();
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	//		gameplay関連の解放
	//
	if (platform != NULL) {
		platform->shutdownInternal();
		delete platform;
	}

	if (GetSysReq(SYSREQ_LOGWRITE)) {
		hsp3dish_savelog();
	}

	if (game != NULL) {
		//game->exit();
	    delete game;
	}

#ifdef HSPDEBUG
	//		デバッグウインドゥの解放
	//
	if ( h_dbgwin != NULL ) { FreeLibrary( h_dbgwin ); h_dbgwin = NULL; }
#endif

	DllManager().free_all_library();

	//		システム関連の解放
	//
#ifndef HSP_COM_UNSUPPORTED
	OleUninitialize();
	CoUninitialize();
#endif


}


char *hsp3dish_getlog(void)
{
	return (char *)gplog.c_str();
}


void hsp3dish_error( void )
{
	char errmsg[1024];
	char *msg;
	char *fname;
	HSPERROR err;
	int ln;
	err = code_geterror();
	ln = code_getdebug_line();
	msg = hspd_geterror(err);
	fname = code_getdebug_name();

	if ( ln < 0 ) {
		sprintf( errmsg, "#Error %d\n-->%s\n",(int)err,msg );
		fname = NULL;
	} else {
		sprintf( errmsg, "#Error %d in line %d (%s)\n-->%s\n",(int)err, ln, fname, msg );
	}
	hsp3dish_debugopen();
	hsp3dish_dialog( errmsg );

	{
		// エラーログを記録
		char *p = (char *)gplog.c_str();
		mem_save("hsp3gp_error.log", p, strlen(p), -1);
		//hsp3dish_dialog((char *)gplog.c_str());
	}
}


int hsp3dish_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;

	hsp3dish_msgfunc( ctx );

	//		デバッグウインドゥ用
	//
#ifdef HSPDEBUG
	if ( ctx->hsphed->bootoption & HSPHED_BOOTOPT_DEBUGWIN ) {
		if ( hsp3dish_debugopen() ) return -1;
	}
#endif

	//		実行の開始
	//
	runmode = code_execcmd();
	if ( runmode == RUNMODE_ERROR ) {
		try {
			hsp3dish_error();
		}
		catch( ... ) {
		}
		hsp3dish_bye();
		return -1;
	}

#if 0
	if ( runmode == RUNMODE_EXITRUN ) {
		char fname[_MAX_PATH];
		char cmd[1024];
		HINSTANCE inst;
		int res;
		strncpy( fname, ctx->refstr, _MAX_PATH-1 );
		strncpy( cmd, ctx->stmp, 1023 );
		inst = (HINSTANCE)ctx->instance;

		hsp3dish_bye();
		res = hsp3dish_init( inst, fname );
		if ( res ) return res;

		strncpy( ctx->cmdline, cmd, 1023 );
		goto rerun;
	}
#endif
	endcode = ctx->endcode;
	hsp3dish_bye();
	return endcode;
}


