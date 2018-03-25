/*--------------------------------------------------------
	HSP3 main (Console Version)
									  2004/8  onitama
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../strbuf.h"
#include "../hsp3.h"
#include "../hsp3config.h"
#include "../supio.h"
#include "../hsp3gr.h"

/*----------------------------------------------------------*/

static Hsp3 *hsp;
static HSPCTX *ctx;

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wd;

/*----------------------------------------------------------*/

void hsp3win_dialog( char *mes )
{
	printf( "%s\n", mes );
}


void hsp3cl_msgfunc( HSPCTX *hspctx )
{
	while(1) {

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
			//		stop命令
			hsp3win_dialog( "[STOP] Press any key..." );
			getchar();
			throw HSPERR_NONE;

		case RUNMODE_WAIT:
			//		wait命令による時間待ち
			//		(実際はcode_exec_waitにtick countを渡す)
			hspctx->runmode = RUNMODE_RUN;
			//hspctx->runmode = code_exec_wait( tick );
			break;

		case RUNMODE_AWAIT:
			//		await命令による時間待ち
			//		(実際はcode_exec_awaitにtick countを渡す)
			hspctx->runmode = RUNMODE_RUN;
			//hspctx->runmode = code_exec_await( tick );
			break;

		case RUNMODE_END:
			//		end命令
#if 0
			hsp3win_dialog( "[END] Press any key..." );
			getchar();
#endif
			throw HSPERR_NONE;

		case RUNMODE_RETURN:
			throw HSPERR_RETURN_WITHOUT_GOSUB;

		case RUNMODE_ASSERT:
			//		assertで中断
			hspctx->runmode = RUNMODE_STOP;
			break;

		case RUNMODE_LOGMES:
			//		logmes命令
			hspctx->runmode = RUNMODE_RUN;
			break;

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

	//		HSP関連の初期化
	//
	hsp = new Hsp3();

#ifdef HSPDEBUG

	if ( *startfile == 0 ) {
		printf( "OpenHSP CL ver%s / onion software 1997-2017\n", hspver );
		return -1;
	}
	hsp->SetFileName( startfile );
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

	if ( hsp->Reset( mode ) ) {
		hsp3win_dialog( "Startup failed." );
		return -1;
	}

	ctx = &hsp->hspctx;

	//		コマンドライン関連
	ss = "";								// コマンドラインパラメーターを入れる
	sbStrCopy( &ctx->cmdline, ss );			// コマンドラインパラメーターを保存

	//		Register Type
	//
	ctx->msgfunc = hsp3cl_msgfunc;
	ctx->hspstat |= 16;

	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

	hsp3typeinit_cl_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_cl_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

	return 0;
}


static void hsp3cl_bye( void )
{
	//		HSP関連の解放
	//
	delete hsp;
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

	hsp3win_dialog( errmsg );
	hsp3win_dialog( "[ERROR] Press any key..." );
	getchar();
}


int hsp3cl_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;
rerun:

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
		char fname[HSP_MAX_PATH];
		char cmd[1024];
		int res;
		strncpy( fname, ctx->refstr, HSP_MAX_PATH-1 );
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
