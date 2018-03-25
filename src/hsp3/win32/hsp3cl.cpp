/*--------------------------------------------------------
	HSP3 main (Windows Console)
									  2004/8  onitama
  --------------------------------------------------------*/

#ifndef HSPCL_WIN
#define HSPCL_WIN
#endif

#define _WIN32_DCOM 

#include <windows.h>
#include <objbase.h>
#include <conio.h>
#include <commctrl.h>

#if defined( _MSC_VER )
#pragma comment(lib, "comctl32.lib")
#endif

#include <stdio.h>
#include <string.h>

#include "hsp3gr_win.h"

#include "../hsp3config.h"

#include "../strbuf.h"
#include "../hsp3.h"
#include "../hsp3gr.h"
#include "../supio.h"

#include "../win32gui/hsp3ext_win.h"
#include "../win32gui/hsp3extlib.h"
#include "../win32gui/hspvar_comobj.h"
#include "../win32gui/hspvar_variant.h"

typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

/*----------------------------------------------------------*/

static Hsp3 *hsp;
static HSPCTX *ctx;

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wd;

#ifdef HSPDEBUG
static HSP3DBGFUNC dbgwin;
static HSP3DBGFUNC dbgnotice;
static HINSTANCE h_dbgwin;
static HWND dbgwnd;
static HSP3DEBUG *dbginfo;
#endif

/*----------------------------------------------------------*/

void hsp3win_dialog( char *mes )
{
	printf( "%s\n", mes );
}


#ifdef HSPCL_WIN
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
#endif


int hsp3win_debugopen( void )
{
	//		デバッグウインドゥ表示
	//
#ifdef HSPDEBUG
	if ( h_dbgwin != NULL ) return 0;
	h_dbgwin = LoadLibrary( "hsp3debug.dll" );
	if ( h_dbgwin != NULL ) {
		dbgwin = (HSP3DBGFUNC)GetProcAddress( h_dbgwin, "_debugini@16" );
		dbgnotice = (HSP3DBGFUNC)GetProcAddress( h_dbgwin, "_debug_notice@16" );
		if (( dbgwin == NULL )||( dbgnotice == NULL )) h_dbgwin = NULL;
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


#ifdef HSPCL_WIN
static void hsp3win_dispatch( MSG *msg )
{
	TranslateMessage( msg );
	DispatchMessage( msg );
}

static void hsp3cl_stop( HSPCTX *hspctx )
{
	MSG msg;

#ifdef HSPDEBUG
	if ( h_dbgwin != NULL ) dbgnotice( dbginfo, 0, 0, 0 );		// Debug Window Notice
#endif
	while(1) {
		GetMessage( &msg, NULL, 0, 0 );
		if ( msg.message == WM_QUIT ) throw HSPERR_NONE;
		hsp3win_dispatch( &msg );
		if ( hspctx->runmode != RUNMODE_STOP ) break;
	}
}
#endif


void hsp3cl_msgfunc( HSPCTX *hspctx )
{
	while(1) {

#ifdef HSPCL_WIN
		MSG msg;
		int tick;
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if (msg.message == WM_QUIT ) throw HSPERR_NONE;
			hsp3win_dispatch( &msg );
			continue;
		}
#endif

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
#ifdef HSPDEBUG
#ifdef HSPCL_WIN
			if ( h_dbgwin != NULL ) {
				hsp3cl_stop( hspctx );
				break;
			} else {
				hsp3win_dialog( "[STOP] Press any key..." );
				_getch();
			}
#else
			hsp3win_dialog( "[STOP] Press any key..." );
			_getch();
#endif
#endif
			throw HSPERR_NONE;
		case RUNMODE_WAIT:
#ifdef HSPCL_WIN
			tick = GetTickCount();
			hspctx->runmode = code_exec_wait( tick );
#endif
		case RUNMODE_AWAIT:
#ifdef HSPCL_WIN
			tick = GetTickCount();
			if ( code_exec_await( tick ) != RUNMODE_RUN ) {
				MsgWaitForMultipleObjects(0, NULL, FALSE, hspctx->waittick - tick, QS_ALLINPUT );
			}
#endif
			break;
		case RUNMODE_END:
#if 0
			if ( h_dbgwin != NULL ) {
				hsp3cl_stop( hspctx );
			} else {
				hsp3win_dialog( "[END] Press any key..." );
				_getch();
			}
#endif
			throw HSPERR_NONE;
		case RUNMODE_RETURN:
			throw HSPERR_RETURN_WITHOUT_GOSUB;


#ifdef HSPCL_WIN
		case RUNMODE_ASSERT:
			hspctx->runmode = RUNMODE_STOP;
#ifdef HSPDEBUG
			hsp3win_debugopen();
#endif
			break;
		case RUNMODE_LOGMES:
			hspctx->runmode = RUNMODE_RUN;
#ifdef HSPDEBUG
			if ( h_dbgwin != NULL ) dbgnotice( dbginfo, 1, 0, 0 );		// Debug Window Notice
#endif
			return;
#endif

		default:
			return;
		}
	}
}


int hsp3cl_init( char *startfile )
{
	//		システム関連の初期化
	//		( mode:0=debug/1=release )
	//
	int a,orgexe, mode;
	int hsp_sum, hsp_dec;
	char a1;
	char *ss;
#ifdef HSPDEBUG
	char fname[_MAX_PATH+1];
	int i;
#endif

#ifdef HSPCL_WIN
#ifndef HSP_COM_UNSUPPORTED
	if ( FAILED( CoInitializeEx( NULL, COINIT_APARTMENTTHREADED) ) ) {
		return -1;
	}
	OleInitialize( NULL );
#endif
	InitCommonControls();
#endif

	//		HSP関連の初期化
	//
	hsp = new Hsp3();
#ifdef HSPCL_WIN
	hsp->hspctx.instance = (HINSTANCE)GetModuleHandle(NULL);
#endif

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
	strncpy( fname, ss, i );
	fname[i] = 0;
	hsp->SetFileName( fname );

	if ( i == 0 ) {
		printf( "OpenHSP CL ver%s / onion software 1997-2009\n", hspver );
		return -1;
	}
#else
	if ( startfile != NULL ) {
		hsp->SetFileName( startfile );
	}
#endif

	//		実行ファイルかデバッグ中かを調べる
	//
	mode = 0;
	orgexe = 0;
	hsp_wd = 0;
	for( a=0 ; a<8; a++) {
		a1=optmes[a]-48;if (a1==fpas[a]) orgexe++;
	}
	if ( orgexe == 0 ) {
		mode = atoi(optmes+9) + 0x10000;
		hsp_wd=( *(short *)(optmes+26) );
		hsp_sum=*(unsigned short *)(optmes+29);
		hsp_dec=*(int *)(optmes+32);
		hsp->SetPackValue( hsp_sum, hsp_dec );
	}

	//		起動ファイルのディレクトリをカレントにする
	//
#ifndef HSPDEBUG
	if (( hsp_wd & 2 ) == 0 ) {
		char fname[_MAX_PATH+1];
		GetModuleFileName( NULL, fname, _MAX_PATH );
		getpath( fname, fname, 32 );
		changedir( fname );
	}
#endif

	if ( hsp->Reset( mode ) ) {
		hsp3win_dialog( "Startup failed." );
		return -1;
	}

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

	//		Register Type
	//
	ctx->msgfunc = hsp3cl_msgfunc;
	ctx->hspstat |= 16;

#ifdef HSPCL_WIN
#ifndef HSP_COM_UNSUPPORTED
	HspVarCoreRegisterType( TYPE_COMOBJ, HspVarComobj_Init );
	HspVarCoreRegisterType( TYPE_VARIANT, HspVarVariant_Init );
#endif
	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );
#endif
	hsp3typeinit_cl_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_cl_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

#ifdef HSPCL_WIN
#ifdef HSPDEBUG
	dbginfo = code_getdbg();
#endif
#endif
	return 0;
}


static void hsp3cl_bye( void )
{
	//		HSP関連の解放
	//
	delete hsp;

	DllManager().free_all_library();

	//		システム関連の解放
	//
#ifdef HSPCL_WIN
#ifndef HSP_COM_UNSUPPORTED
	OleUninitialize();
	CoUninitialize();
#endif
#endif
}


void hsp3cl_error( void )
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

#ifdef HSPCL_WIN
#ifdef HSPDEBUG
	hsp3win_debugopen();
	hsp3win_dialog( errmsg );
	MessageBox( NULL, errmsg, "Error",MB_ICONEXCLAMATION | MB_OK );
#else
	hsp3win_dialog( errmsg );
#endif
#else
	hsp3win_dialog( errmsg );
	hsp3win_dialog( "[ERROR] Press any key..." );
	_getch();
#endif
}


int hsp3cl_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;
rerun:

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
			hsp3cl_error();
		}
		catch( ... ) {
		}
		return -1;
	}
	if ( runmode == RUNMODE_EXITRUN ) {
		char fname[_MAX_PATH];
		char cmd[1024];
		int res;
		strncpy( fname, ctx->refstr, _MAX_PATH-1 );
		strncpy( cmd, ctx->stmp, 1023 );

		hsp3cl_bye();
		res = hsp3cl_init( fname );
		if ( res ) return res;

		strncpy( ctx->cmdline, cmd, 1023 );
		ctx->runmode = RUNMODE_RUN;
		goto rerun;
	}
	endcode = ctx->endcode;
	hsp3cl_bye();
	return endcode;
}
