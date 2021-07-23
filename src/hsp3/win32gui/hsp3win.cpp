/*--------------------------------------------------------
	HSP3 main (Windows GUI)
									  2004/8  onitama
  --------------------------------------------------------*/

#define _WIN32_DCOM 

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <objbase.h>
#include <commctrl.h>

#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "hsp3win.h"
#include "../strbuf.h"
#include "../hsp3.h"
#include "../hsp3ext.h"
#include "../hsp3gr.h"

#include "hsp3extlib.h"
#include "hspvar_comobj.h"
#include "hspvar_variant.h"
#include "../supio.h"

typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

/*----------------------------------------------------------*/

static Hsp3 *hsp = NULL;
static HSPCTX *ctx;

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wx, hsp_wy, hsp_wd, hsp_ss;

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

//-------------------------------------------------------------
//		Sync Timer Routines
//-------------------------------------------------------------

static int	timer_period = -1;
static int	timerid = 0;

/*----------------------------------------------------------*/

void hsp3win_dialog( char *mes )
{
	HSPAPICHAR *hactmp1 = 0;
	MessageBox( NULL, chartoapichar(mes,&hactmp1), TEXT("Error"),MB_ICONEXCLAMATION | MB_OK );
	freehac(&hactmp1);
}


#ifdef HSPDEBUG
char *hsp3win_debug( int type )
{
	//		デバッグ情報取得
	//
	char *p;
	p = code_inidbg();

	switch( type ) {
	case DEBUGINFO_GENERAL:
		hsp3gr_dbg_gui();
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


int hsp3win_debugopen( void )
{
	//		デバッグウインドゥ表示
	//
#ifdef HSPDEBUG
	if ( h_dbgwin != NULL ) return 0;
#ifdef HSP64
	h_dbgwin = LoadLibrary( TEXT( HSP3DEBUG_MODULE "_64.dll" ) );
#else
#ifndef HSPUTF8
	h_dbgwin = LoadLibrary(TEXT( HSP3DEBUG_MODULE ".dll" ));
#else
	h_dbgwin = LoadLibrary(TEXT( HSP3DEBUG_MODULE ) TEXT("_u8.dll") );
#endif
#endif
	if ( h_dbgwin != NULL ) {
#ifdef HSP64
		dbgwin = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, HSP3DEBUG_INIT );
		dbgnotice = (HSP3DBGFUNC)GetProcAddress( h_dbgwin, HSP3DEBUG_NOTICE );
#else
		dbgwin = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, "_" HSP3DEBUG_INIT "@16");
		dbgnotice = (HSP3DBGFUNC)GetProcAddress(h_dbgwin, "_" HSP3DEBUG_NOTICE "@16");
#endif
		if ((dbgwin == NULL) || (dbgnotice == NULL)) h_dbgwin = NULL;
	}
	if ( h_dbgwin == NULL ) {
		hsp3win_dialog( "No debug module." );
		return -1;
	}
	dbginfo->get_value = hsp3win_debug;
	dbgwin( dbginfo, 0, 0, 0 );
	dbgwnd = (HWND)( dbginfo->dbgwin );
#endif
	return 0;
}


static void hsp3win_dispatch( MSG *msg )
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

	if (msg->message == WM_KEYDOWN) {	// ocheck onkey
		if (msg->wParam == 9) {
			hsp3gr_nextobj();
		}
		if (code_isirq(HSPIRQ_ONKEY)) {
#ifdef HSPERR_HANDLE
			try {
#endif
				code_sendirq(HSPIRQ_ONKEY, (int)MapVirtualKey(msg->wParam, 2), (int)msg->wParam, (int)msg->lParam);
#ifdef HSPERR_HANDLE
			}
			catch (HSPERROR code) {						// HSPエラー例外処理
				code_catcherror(code);
			}
#endif
		}
	}

}


void hsp3win_msgfunc( HSPCTX *hspctx )
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
			hsp3win_dispatch( &msg );
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
				hsp3win_dispatch( &msg );
				if ( hspctx->runmode != RUNMODE_STOP ) break;
			}

			MsgWaitForMultipleObjects(0, NULL, FALSE, 1000, QS_ALLINPUT );
			break;
		case RUNMODE_WAIT:
			if ( timer_period == -1 ) {
				//	通常のタイマー
				tick = GetTickCount();
			} else {
				//	高精度タイマー
				tick = timeGetTime();
			}
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
			hsp3win_debugopen();
#endif
			break;
	//	case RUNMODE_LOGMES:
		default:
			return;
		}

	}
}


int hsp3win_init( HINSTANCE hInstance, char *startfile )
{
	//		システム関連の初期化
	//		( mode:0=debug/1=release )
	//
	int a,orgexe, mode;
	int hsp_sum, hsp_dec;
	char a1;
#ifdef HSPDEBUG
	char fname[_MAX_PATH + 1];
#endif
#ifndef HSPDEBUG
	TCHAR fnamew[_MAX_PATH+1];
	TCHAR fnamew2[_MAX_PATH + 1];
#endif
	char *ss;
#ifdef HSPDEBUG
	int i;
#endif

#ifndef HSP_COM_UNSUPPORTED
	if ( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED) ) ) {
		return 1;
	}
	OleInitialize( NULL );
#endif
	InitCommonControls();

	//		HSP関連の初期化
	//
	hsp = new Hsp3();
	hsp->hspctx.instance = (void *)hInstance;
#ifdef HSPDEBUG
	h_dbgwin = NULL;
	dbgwnd = NULL;

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
	if (startfile != NULL) {
		hsp->SetFileName(startfile);
	}
#endif


	//		実行ファイルかデバッグ中かを調べる
	//
	mode = 0;
	orgexe=0;
	hsp_wx = 640;
	hsp_wy = 480;
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
#ifndef HSPWINGUIDLL
	//		起動ファイルのディレクトリをカレントにする
	//
#ifndef HSPDEBUG
	if (( hsp_wd & 2 ) == 0 ) {
		GetModuleFileName( NULL, fnamew, _MAX_PATH );
		getpathW( fnamew, fnamew2, 32 );
		changedirW( fnamew2 );
	}
#endif
#endif
	if ( hsp->Reset( mode ) ) {
		hsp3win_dialog( "Startup failed." );
		return 1;
	}

	ctx = &hsp->hspctx;

	//		SSaver proc
	//
#ifndef HSPDEBUG
	if ( hsp_ss ) {
		LPTSTR cl;
		cl = GetCommandLine();
		cl = strsp_cmdsW( cl );
		hsp_sscnt = 30;
		a1=tolower(*(cl+1));
		if (FindWindow(TEXT("oniwndp"), NULL) != NULL) {
			return 2;
		}
		if (a1==TEXT('p')) {
			HWND s_hwnd;
			RECT rPic;
			s_hwnd = (HWND)_tstoi( cl+3 );
			GetWindowRect( s_hwnd, &rPic );
			hsp_wx = rPic.right-rPic.left;
			hsp_wy = rPic.bottom-rPic.top;
			hsp_wd = 0x100;
			ctx->wnd_parent = s_hwnd;
		}
		if (a1==TEXT('s')) {
			ShowCursor(FALSE);
		} else {
			hsp_ss = 0;								// スクリーンセーバー時以外はモードOFF
		}
		ctx->hspstat |= hsp_ss;
	}
#endif

	//		Register Type
	//
	ctx->msgfunc = hsp3win_msgfunc;

#ifndef HSP_COM_UNSUPPORTED
	HspVarCoreRegisterType( TYPE_COMOBJ, HspVarComobj_Init );
	HspVarCoreRegisterType( TYPE_VARIANT, HspVarVariant_Init );
#endif


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

	//		Initalize GUI System
	//
	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );
	hsp3typeinit_extcmd( code_gettypeinfo( TYPE_EXTCMD ), hsp_wx, hsp_wy, hsp_wd, -1, -1 );
	hsp3typeinit_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

#ifdef HSPDEBUG
	dbginfo = code_getdbg();
#endif
	return 0;
}

void hsp3win_error(void)
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

	if (ln < 0) {
		sprintf(errmsg, "#Error %d\n-->%s\n", (int)err, msg);
		fname = NULL;
	}
	else {
		sprintf(errmsg, "#Error %d in line %d (%s)\n-->%s\n", (int)err, ln, fname, msg);
	}
	hsp3win_debugopen();
	hsp3win_dialog(errmsg);
}


void hsp3win_bye( void )
{
	//		タイマーの開放
	//
#ifdef HSPERR_HANDLE
	try {
#endif
		hsp->Dispose();
#ifdef HSPERR_HANDLE
	}
	catch (HSPERROR code) {						// HSPエラー例外処理
		hsp->hspctx.err = code;
		hsp3win_error();
	}
#endif

	if ( timer_period != -1 ) {
		timeEndPeriod( timer_period );
		timer_period = -1;
	}

#ifdef HSPDEBUG
	//		デバッグウインドゥの解放
	//
	if ( h_dbgwin != NULL ) { FreeLibrary( h_dbgwin ); h_dbgwin = NULL; }
#endif

	//		HSP関連の解放
	//
	if (hsp != NULL) { delete hsp; hsp = NULL; }
	DllManager().free_all_library();

	//		システム関連の解放
	//
#ifndef HSP_COM_UNSUPPORTED
	OleUninitialize();
	CoUninitialize();
#endif
}


int hsp3win_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;
rerun:
	hsp3win_msgfunc( ctx );

	//		デバッグウインドゥ用
	//
#ifdef HSPDEBUG
	if ( ctx->hsphed->bootoption & HSPHED_BOOTOPT_DEBUGWIN ) {
		if ( hsp3win_debugopen() ) return -1;
	}
#endif

	//		実行の開始
	//
	runmode = code_execcmd();
	if ( runmode == RUNMODE_ERROR ) {
		try {
			hsp3win_error();
		}
		catch( ... ) {
		}
		return -1;
	}
	if ( runmode == RUNMODE_EXITRUN ) {
		char fname[_MAX_PATH];
		char cmd[1024];
		HINSTANCE inst;
		int res;
		strncpy( fname, ctx->refstr, _MAX_PATH-1 );
		strncpy( cmd, ctx->stmp, 1023 );
		inst = (HINSTANCE)ctx->instance;

		hsp3win_bye();
		res = hsp3win_init( inst, fname );
		if ( res ) return res;

		strncpy( ctx->cmdline, cmd, 1023 );
		goto rerun;
	}
	endcode = ctx->endcode;
	hsp3win_bye();
	return endcode;
}

