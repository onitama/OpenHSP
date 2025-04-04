/*--------------------------------------------------------
	HSP3 main (Embed)
									  2011/3  onitama
  --------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>

#include "../hsp3/hsp3config.h"

#include "../hsp3/hsp3ext.h"
#include "../hsp3/supio.h"
#include "../hsp3/strbuf.h"

#include "hsp3r.h"
#include "hsp3embed.h"
#include "../hsp3dish/sysreq.h"
#include "../hsp3dish/hsp3gr.h"

//#define USE_OBAQ

#ifdef USE_OBAQ
#include "../hsp3dish/obaq/hsp3dw.h"
#endif


#ifdef HSPDISHGP
#include "../hsp3dish/win32gp/gamehsp.h"
#endif

#ifdef HSPDISHGP
gamehsp *game;
gameplay::Platform *platform;

//-------------------------------------------------------------
//		gameplay Log
//-------------------------------------------------------------

static std::string gplog;

extern "C" {
	static void logfunc( gameplay::Logger::Level level, const char *msg )
	{
		if (GetSysReq(SYSREQ_LOGWRITE)) {
			gplog += msg;
		}
#if defined(HSPIOS)||defined(HSPNDK)
        Alert( msg );
#endif
	}
}

#endif

extern void __HspInit( Hsp3r *hsp3 );
//typedef BOOL (*HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

/*----------------------------------------------------------*/

static Hsp3r *hsp = NULL;
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

/*----------------------------------------------------------*/

void hsp3eb_dialog( char *mes )
{
	Alert( (const char *)mes );
}

int hsp3eb_wait( int tick )
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


int hsp3eb_await( int tick )
{
	//		時間待ち(await)
	//
	if ( ctx->waittick == -1 ) {
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

/*----------------------------------------------------------*/

void hsp3eb_execstart( void )
{
    
    //		Register Type
    //
    hsp3typeinit_extcmd( code_gettypeinfo( TYPE_EXTCMD )  );
    hsp3typeinit_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );
    
	//		Initalize external DLL System
	//
	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

#ifdef USE_OBAQ
    hsp3typeinit_dw_extcmd( code_gettypeinfo( -1 ) ); // TYPE_USERDEF
    //hsp3typeinit_dw_extfunc( code_gettypeinfo( TYPE_USERDEF+1 ) );
#endif
    
    //		Utility setup
    VarUtilInit();
    
#ifdef HSPDEBUG
    dbginfo = code_getdbg();
#endif
    
    ctx->endcode = 0;
    ctx->looplev = 0;
    ctx->sublev = 0;
    
    StackReset();
    
#ifdef HSPDISHGP
    game->frame();
    game->setMultiTouch(true);
#endif
}


int hsp3eb_init( void )
{
	//		システム関連の初期化
	//
//	int a,orgexe, mode;
//	int hsp_sum, hsp_dec;
//	char a1;
#ifdef HSPDEBUG
	int i;
#endif

	//		HSP関連の初期化
	//
	Alertf( "---Init HSP3\n" );
	hsp = new Hsp3r();
	__HspInit( hsp );


	//		実行ファイルかデバッグ中かを調べる
	//
//	mode = 0;
//	orgexe=0;
	hsp_wx = 480;
	hsp_wy = 640;
	hsp_wd = 0;
	hsp_ss = 0;

	ctx = &hsp->hspctx;

#ifdef HSPDISHGP
	//		Initalize gameplay
	//
    game = new gamehsp;

	gplog[0] = 0;
	gameplay::Logger::set(gameplay::Logger::LEVEL_INFO, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_WARN, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_ERROR, logfunc);
	
	platform = gameplay::Platform::create( game, NULL, hsp_wx, hsp_wy, false );
//	platform = gameplay::Platform::create( game, m_hWnd, hsp_wx, hsp_wy, false );
	if ( platform == NULL ) {
//		Alertf( gplog );
		Alertf( "OpenGL initalize failed." );
		return 1;
	}
//	Alertf( "---Init HSP3\n" );

#ifndef HSPIOS
	platform->enterMessagePump();
#endif
//	Alertf( "---enterMessagePump OK\n" );

       if (Game::getInstance()->getState() == Game::UNINITIALIZED)
            {
                Game::getInstance()->run();
            }
            else
            {
                Game::getInstance()->resume();
            }


	game->frame();

#endif

    hsp3eb_execstart();
    return 0;
}


void hsp3eb_bye( void )
{
	//		HSP関連の解放
	//
	if ( hsp != NULL ) { delete hsp; hsp = NULL; }

#ifdef HSPDEBUG
	//		デバッグウインドゥの解放
	//
	if ( h_dbgwin != NULL ) { FreeLibrary( h_dbgwin ); h_dbgwin = NULL; }
#endif

	Alertf( "---Term HSP3\n" );

#ifdef HSPDISHGP
	//		gameplay関連の解放
	//
	if ( platform != NULL ) {
		platform->shutdownInternal();
	    delete platform;
	}
	if ( game != NULL ) {
	    delete game;
	}
#endif

	//		システム関連の解放
	//
//	DllManager().free_all_library();

}

char *hsp3dish_getlog(void)
{
#ifdef HSPDISHGP
	return (char *)gplog.c_str();
#else
	return "";
#endif
}

void hsp3eb_error( void )
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
//	hsp3win_debugopen();
	hsp3eb_dialog( errmsg );
}


int hsp3eb_exec( void )
{
	//		実行メインを呼び出す
	//
	int i;
	if ( ctx->runmode == RUNMODE_ERROR ) {
		return ctx->runmode;
	}
	ctx->runmode = RUNMODE_RUN;

	try {
		while(1) {
			TaskExec();
			if ( ctx->runmode != 0 ) {
                if ( ctx->runmode != RUNMODE_RETURN ) break;  
            }
		}
	}

		catch( HSPERROR code ) {						// HSPエラー例外処理
		if ( code == HSPERR_NONE ) {
			ctx->runmode = RUNMODE_END;
		} else {
			i = RUNMODE_ERROR;
			ctx->err = code;
			ctx->runmode = i;
			hsp3eb_error();
		}
	}

	//Alertf( "RUN=%d",ctx->runmode );
	return ctx->runmode;
}


int hsp3eb_exectime( int tick )
{
	//		実行メインを呼び出す
	//		(time=経過時間)
	//
	//Alertf( "hsp3eb_exectime=%d",ctx->runmode );
	switch( ctx->runmode ) {
		case RUNMODE_STOP:
		case RUNMODE_ASSERT:
			return ctx->runmode;
		case RUNMODE_WAIT:
			//	高精度タイマー
			ctx->runmode = hsp3eb_wait( tick );
            if ( ctx->runmode == RUNMODE_RUN ) break;
			return ctx->runmode;
		case RUNMODE_AWAIT:
			//	高精度タイマー
			ctx->runmode = hsp3eb_await( tick );
            if ( ctx->runmode == RUNMODE_RUN ) break;
			return ctx->runmode;
		case RUNMODE_END:
			return ctx->runmode;
		case RUNMODE_INTJUMP:
			break;
		default:
			break;
		}
	return hsp3eb_exec();
}


void hsp3eb_setstat( int stat )
{
	//		stat値を設定
	//
    ctx->stat = stat;
}


HSPCTX *hsp3eb_getctx( void )
{
	//		HSPCTXを返す
	//
	return ctx;
}


void *hsp3eb_getDevInfo( void )
{
	//		DEVINFOを返す
	//
	return hsp3extcmd_getdevinfo();
}


char *hsp3eb_stmp( char *str )
{
	//		HSPCTXのstmpに文字列を格納する
	//
	code_stmpstr( str );
	return ctx->stmp;
}


void hsp3eb_pause( void )
{
	//		アプリケーションの一時停止
	//
	hsp3extcmd_pause();
}


void hsp3eb_resume( void )
{
	//		アプリケーションの再開
	//
	hsp3extcmd_resume();
}


int hsp3eb_gettask( void )
{
	return GetTaskID();
}



