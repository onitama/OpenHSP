/*--------------------------------------------------------
	HSP3 main (Console Version)
									  2004/8  onitama
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../strbuf.h"
#include "../hsp3.h"
#include "../hsp3config.h"
#include "../supio.h"
#include "../hsp3gr.h"
#include "../hsp3ext.h"

#include "hsp3cl.h"
#include "hsp3ext_sock.h"

/*----------------------------------------------------------*/

static Hsp3 *hsp;
static HSPCTX *ctx;
static HSPEXINFO *exinfo;

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wd;
static int cl_option;
static char *cl_cmdline = "";
static char *cl_modname = "";
static FILE *cl_fp;

#define HSP3CL_RESFILE ".hspres"

/*----------------------------------------------------------*/

static void usage1( void )
{
static 	char *p[] = {
	"usage: hsp3cl [options] [filename]",
	"       -r    output result file to .hspres",
	"       -p    pause when finished",
	NULL };
	int i;
	for(i=0; p[i]; i++)
		printf( "%s\n", p[i]);
}

/*----------------------------------------------------------*/

int gettick( void )
{
	int i;
	timespec ts;
	double nsec;
    clock_gettime(CLOCK_REALTIME,&ts);
    nsec = (double)(ts.tv_nsec) * 0.001 * 0.001;
    i = (int)ts.tv_sec * 1000 + (int)nsec;
    //return ((double)(ts.tv_sec) + (double)(ts.tv_nsec) * 0.001 * 0.001 * 0.001);
	return i;
}


void hsp3win_dialog( char *mes )
{
	printf( "%s\n", mes );
}


void hsp3cl_msgfunc( HSPCTX *hspctx )
{
	int tick;

	while(1) {

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
			//		stop命令
			if ( cl_option & HSP3CL_OPT1_ERRSTOP ) {
				hsp3win_dialog( "[STOP] Press any key..." );
				getchar();
			}
			throw HSPERR_NONE;

		case RUNMODE_WAIT:
			//		wait命令による時間待ち
			//		(実際はcode_exec_waitにtick countを渡す)
			usleep( ( hspctx->waitcount) * 10000 );
			//hspctx->runmode = code_exec_wait( tick );
			hspctx->runmode = RUNMODE_RUN;
			break;

		case RUNMODE_AWAIT:
			//		await命令による時間待ち
			//		(実際はcode_exec_awaitにtick countを渡す)
			tick = gettick();
			if ( code_exec_await( tick ) != RUNMODE_RUN ) {
					usleep( ( hspctx->waittick - tick) * 1000 );
			} else {
				tick = gettick();
				while( tick < hspctx->waittick ) {	// 細かいwaitを取る
					usleep( 10000 );
					tick = gettick();
				}
				hspctx->lasttick = tick;
				hspctx->runmode = RUNMODE_RUN;
				//hspctx->runmode = RUNMODE_RUN;
			}
			break;

		case RUNMODE_END:
			//		end命令
			if ( cl_option & HSP3CL_OPT1_ERRSTOP ) {
				hsp3win_dialog( "[END] Press any key..." );
				getchar();
			}
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
		printf( "OpenHSP CL ver%s / onion software 1997-2022\n", hspver );
		usage1();
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
	hsp->SetCommandLinePrm( cl_cmdline );		// コマンドラインパラメーターを保存
	hsp->SetModuleFilePrm( cl_modname );			// モジュール名を保存

	//		Register Type
	//
	ctx->msgfunc = hsp3cl_msgfunc;
	ctx->hspstat |= 16;

	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

	hsp3typeinit_cl_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_cl_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

	exinfo = ctx->exinfo2;

	HSP3TYPEINFO *tinfo = code_gettypeinfo( TYPE_USERDEF+1 );
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_sock_extcmd( tinfo );

	cl_option = 0;

	return 0;
}


static void hsp3cl_bye( void )
{
	//		HSP関連の解放
	//
	delete hsp;
}


void hsp3cl_option( int opt )
{
	//		HSP3CLオプション設定
	//
	cl_option = opt;
}


void hsp3cl_cmdline( char *cmdline )
{
	//		HSP3CLオプション設定
	//
	cl_cmdline = cmdline;						// コマンドラインパラメーターを入れる
}


void hsp3cl_modname( char *modname )
{
	//		HSP3CLオプション設定
	//
	cl_modname = modname;						// arg[0]パラメーターを入れる
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

	if ( cl_option & HSP3CL_OPT1_RESOUT ) {
		if ( cl_fp != NULL ) fputs( errmsg, cl_fp );
	}

	if ( cl_option & HSP3CL_OPT1_ERRSTOP ) {
		hsp3win_dialog( "[ERROR] Press any key..." );
		getchar();
	}
}


int hsp3cl_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;

	if ( cl_option & HSP3CL_OPT1_RESOUT ) {
		cl_fp = fopen( HSP3CL_RESFILE, "w" );
	}

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

	if ( cl_option & HSP3CL_OPT1_RESOUT ) {
		if ( cl_fp != NULL ) fclose( cl_fp );
	}

	endcode = ctx->endcode;
	hsp3cl_bye();
	return endcode;
}
