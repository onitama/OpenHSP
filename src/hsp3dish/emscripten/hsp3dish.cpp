/*--------------------------------------------------------
	HSP3dish main (emscripten & SDL)
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#if defined( __GNUC__ )
#include <ctype.h>
#endif

#ifdef HSPDISHGP
#include "gamehsp.h"
#endif

#include "hsp3dish.h"
#include "../../hsp3/hsp3config.h"
#include "../../hsp3/strbuf.h"
#include "../../hsp3/hsp3.h"
#include "../hsp3gr.h"
#include "../supio.h"
#include "../hgio.h"
#include "../sysreq.h"
//#include "../hsp3ext.h"
#include "../../hsp3/strnote.h"
#include "appengine.h"

#include <GL/gl.h>
//#include <GL/glut.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <emscripten.h>

//#define USE_OBAQ

#ifdef USE_OBAQ
#include "../obaq/hsp3dw.h"
#endif


/*----------------------------------------------------------*/

static Hsp3 *hsp = NULL;
static HSPCTX *ctx;
static HSPEXINFO *exinfo;								// Info for Plugins

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wx, hsp_wy, hsp_wd, hsp_ss;
static int drawflag;
static int hsp_fps;
static int hsp_limit_step_per_frame;
static std::string syncdir;
static bool fs_initialized = false;

static int cl_option;
static char *cl_cmdline = "";
static char *cl_modname = "";

//static	HWND m_hWnd;

#ifndef HSPDEBUG
static int hsp_sscnt, hsp_ssx, hsp_ssy;
#endif

#define SDLK_SCANCODE_MAX 0x200
static bool keys[SDLK_SCANCODE_MAX];
SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_GLContext context;

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
		if (GetSysReq(SYSREQ_LOGWRITE)) gplog += msg;
		if (( level == gameplay::Logger::LEVEL_ERROR )||( level == gameplay::Logger::LEVEL_WARN )) printf( "#%s\n",msg );
	}
}

#endif

/*----------------------------------------------------------*/

/*----------------------------------------------------------*/
static int handleEvent( void ) {
	int res=0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_FINGERDOWN:
			//Alertf("down: %f,%f  %d\n", event.tfinger.x,event.tfinger.y,event.tfinger.fingerId);
		case SDL_FINGERMOTION:
			{
				int xx,yy;
				float x,y;
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				x = event.tfinger.x * bm->sx;
				y = event.tfinger.y * bm->sy;
				int id = event.tfinger.fingerId;
				xx = (int)x; yy = (int)y;
				hgio_cnvview((BMSCR *)bm,&xx,&yy);
				hgio_mtouchid( id, xx, yy, 1, 0 );
				//hgio_touch( xx, yy, 1 );
				break;
			}
		case SDL_FINGERUP:
			{
				int xx,yy;
				float x,y;
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				x = event.tfinger.x * bm->sx;
				y = event.tfinger.y * bm->sy;
				int id = event.tfinger.fingerId;
				xx = (int)x; yy = (int)y;
				hgio_cnvview((BMSCR *)bm,&xx,&yy);
				hgio_mtouchid( id, xx, yy, 0, 0 );
				//hgio_touch( xx, yy, 0 );
				//Alertf("up  : %f,%f  %d\n", event.tfinger.x,event.tfinger.y,event.tfinger.fingerId);
				break;
			}

		case SDL_MOUSEMOTION:
			{
				Bmscr *bm;
				if ( exinfo != NULL ) {
					SDL_MouseMotionEvent *m = (SDL_MouseMotionEvent*)&event;
					int x, y;
					bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
#ifdef HSPDISHGP
					x = m->x;
					y = m->y;
#else
					hgio_scale_point( m->x, m->y, x, y );
					hgio_cnvview((BMSCR *)bm,&x,&y);
#endif

					bm->savepos[BMSCR_SAVEPOS_MOSUEX] = x;
					bm->savepos[BMSCR_SAVEPOS_MOSUEY] = y;
					bm->UpdateAllObjects();
					HSP3MTOUCH *mt = bm->getMTouchByPointId(-1);
					if (mt) {
						bm->setMTouchByPointId( -1, x, y, true );
					}
					//printf("motion: %d,%d  %d,%d\n", m->x, m->y, m->xrel, m->yrel);
				}
				//assert(x == m->x && y == m->y);
				//result += 2 * (m->x + m->y + m->xrel + m->yrel);
				break;
			}
		case SDL_MOUSEBUTTONDOWN:
			{
				SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)&event;
				//printf("button down: %d,%d  %d,%d\n", m->button, m->state, m->x, m->y);
				hgio_touch( m->x, m->y, 1 );
				break;
			}
		case SDL_MOUSEBUTTONUP:
			{
				SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)&event;
				//printf("button up: %d,%d  %d,%d\n", m->button, m->state, m->x, m->y);
				hgio_touch( m->x, m->y, 0 );
				break;
			}

		case SDL_KEYDOWN:
			{
			int wparam = 0;
			int code = (int)event.key.keysym.sym;
			if (code < SDLK_SCANCODE_MAX) {
				keys[code] = true;
			}
			switch(code) {
			case SDL_SCANCODE_BACKSPACE:
				wparam = HSPOBJ_NOTICE_KEY_BS;
				break;
			case SDL_SCANCODE_TAB:
				wparam = HSPOBJ_NOTICE_KEY_TAB;
				break;
			case SDL_SCANCODE_RETURN:
				wparam = HSPOBJ_NOTICE_KEY_CR;
				break;
			case SDL_SCANCODE_DELETE:
				wparam = HSPOBJ_NOTICE_KEY_DEL;
				break;
			case SDL_SCANCODE_INSERT:
				wparam = HSPOBJ_NOTICE_KEY_INS;
				break;
			case SDL_SCANCODE_F1:
				wparam = HSPOBJ_NOTICE_KEY_F1 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F2:
				wparam = HSPOBJ_NOTICE_KEY_F2 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F3:
				wparam = HSPOBJ_NOTICE_KEY_F3 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F4:
				wparam = HSPOBJ_NOTICE_KEY_F4 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F5:
				wparam = HSPOBJ_NOTICE_KEY_F5 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F6:
				wparam = HSPOBJ_NOTICE_KEY_F6 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F7:
				wparam = HSPOBJ_NOTICE_KEY_F7 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F8:
				wparam = HSPOBJ_NOTICE_KEY_F8 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F9:
				wparam = HSPOBJ_NOTICE_KEY_F9 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F10:
				wparam = HSPOBJ_NOTICE_KEY_F10 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F11:
				wparam = HSPOBJ_NOTICE_KEY_F11 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_F12:
				wparam = HSPOBJ_NOTICE_KEY_F12 + HSPOBJ_NOTICE_KEY_CTRLADD;
				break;
			case SDL_SCANCODE_LEFT:
				wparam = HSPOBJ_NOTICE_KEY_LEFT;
				break;
			case SDL_SCANCODE_UP:
				wparam = HSPOBJ_NOTICE_KEY_UP;
				break;
			case SDL_SCANCODE_RIGHT:
				wparam = HSPOBJ_NOTICE_KEY_RIGHT;
				break;
			case SDL_SCANCODE_DOWN:
				wparam = HSPOBJ_NOTICE_KEY_DOWN;
				break;
			case SDL_SCANCODE_HOME:
				wparam = HSPOBJ_NOTICE_KEY_HOME;
				break;
			case SDL_SCANCODE_END:
				wparam = HSPOBJ_NOTICE_KEY_END;
				break;
			case SDL_SCANCODE_PAGEUP:
				wparam = HSPOBJ_NOTICE_KEY_SCROLL_UP;
				break;
			case SDL_SCANCODE_PAGEDOWN:
				wparam = HSPOBJ_NOTICE_KEY_SCROLL_DOWN;
				break;
			}
			if ((code >= SDL_SCANCODE_A)&&(code <= SDL_SCANCODE_Z)) {
				if (keys[SDL_SCANCODE_LCTRL]||keys[SDL_SCANCODE_RCTRL]) {
					wparam = (code-SDL_SCANCODE_A) + 1 + HSPOBJ_NOTICE_KEY_CTRLADD;
				}
			}
			if ( wparam > 0 ) {
				if (keys[SDL_SCANCODE_LSHIFT]||keys[SDL_SCANCODE_RSHIFT]) {
					wparam += HSPOBJ_NOTICE_KEY_SHIFTADD;
				}
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				if (bm) {
					bm->SendHSPObjectNotice(wparam);
				}
			}
			//printf("key down: sym %d scancode %d\n", event.key.keysym.sym, event.key.keysym.scancode);
			break;
			}
		case SDL_KEYUP:
			if (event.key.keysym.sym < SDLK_SCANCODE_MAX) {
				keys[event.key.keysym.sym] = false;
			}
			//printf("key up: sym %d scancode %d\n", event.key.keysym.sym, event.key.keysym.scancode);
			break;

		case SDL_TEXTINPUT:
			{
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				strcpy((char *)bm->keybuf,event.text.text);
				bm->keybuf_index = 0;
				bm->SendHSPObjectNotice(HSPOBJ_NOTICE_KEY_BUFFER);
				break;
			}

		case SDL_QUIT: /** ウィンドウのxボタンやctrl-Cを押した場合 */
			res = -1;
		}
	}
	return res;
}

bool get_key_state(int sym)
{
	if (sym >= 0 && sym < SDLK_SCANCODE_MAX) {
		return keys[sym];
	} else {
		return false;
	}
}

static void hsp3dish_initwindow( engine* p_engine, int sx, int sy, char *windowtitle )
{
	printf("INIT %dx%d %s\n", sx,sy,windowtitle);

	// Slightly different SDL initialization
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return;
	}

	window = SDL_CreateWindow( "HSPDish ver" hspver, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sx, sy, SDL_WINDOW_OPENGL );
	if ( window==NULL ) {
		printf("Unable to set window: %s\n", SDL_GetError());
		return;
	}
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	context = SDL_GL_CreateContext(window);
	if ( window==NULL ) {
		printf("Unable to set GLContext: %s\n", SDL_GetError());
		return;
	}

	// 描画APIに渡す
	hgio_init( 0, sx, sy, p_engine );
	hgio_clsmode( CLSMODE_SOLID, 0xffffff, 0 );

	// マルチタッチ初期化
	//MTouchInit( m_hWnd );
}


void hsp3dish_dialog( char *mes )
{
	//MessageBox( NULL, mes, "Error",MB_ICONEXCLAMATION | MB_OK );
	printf( "%s\n", mes );
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
	return 0;
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
	int tick;

	while(1) {
		// logmes なら先に処理する
		if ( hspctx->runmode == RUNMODE_LOGMES ) {
			hspctx->runmode = RUNMODE_RUN;
			return;
		}

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
			// while(1) {
			// 	GetMessage( &msg, NULL, 0, 0 );
			// 	if ( msg.message == WM_QUIT ) throw HSPERR_NONE;
			// 	hsp3dish_dispatch( &msg );
			// 	if ( hspctx->runmode != RUNMODE_STOP ) break;
			// }
			// MsgWaitForMultipleObjects(0, NULL, FALSE, 1000, QS_ALLINPUT );
			return;
		case RUNMODE_WAIT:
			tick = hgio_gettick();
			hspctx->runmode = code_exec_wait( tick );
		case RUNMODE_AWAIT:
			tick = hgio_gettick();
			if ( code_exec_await( tick ) != RUNMODE_RUN ) {
				//MsgWaitForMultipleObjects(0, NULL, FALSE, hspctx->waittick - tick, QS_ALLINPUT );
				//printf("AWAIT WAIT %d < %d\n", tick, ctx->waittick);
			} else {
				//printf("AWAIT RUN %d < %d\n", tick, ctx->waittick);
				ctx->runmode = RUNMODE_AWAIT;
#ifndef HSPDEBUG
				if ( ctx->hspstat & HSPSTAT_SSAVER ) {
					if ( hsp_sscnt ) hsp_sscnt--;
				}
#endif
			}
				return;
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
		case RUNMODE_RESTART:
		{
			//	rebuild window

			hspctx->runmode = RUNMODE_RUN;
			break;
		}
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
	if ( strcmp( cmd, "syncfs" )==0 ) {
		if (syncdir.size() > 0) {
			// IDBに保存
			EM_ASM_({
				var dir = UTF8ToString($0);
				FS.syncfs(function (err) {
					console.log("syncfs", err);
					});
				}, syncdir.c_str());
			return 0;
		}
	}
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
	devinfo->devname = "emscripten";
	devinfo->error = "";
	devinfo->devprm = hsp3dish_devprm;
	devinfo->devcontrol = hsp3dish_devcontrol;
	devinfo->devinfo = hsp3dish_devinfo;
	devinfo->devinfoi = hsp3dish_devinfoi;
}

/*----------------------------------------------------------*/

int hsp3dish_init( char *startfile )
{
	//		システム関連の初期化
	//		( mode:0=debug/1=release )
	//
	int a,orgexe, mode;
	int hsp_sum, hsp_dec;
	char a1;
#ifdef HSPDEBUG
	int i;
#endif
	InitSysReq();

#ifdef HSPDISHGP
	SetSysReq( SYSREQ_MAXMATERIAL, 64 );            // マテリアルのデフォルト値

	game = NULL;
	platform = NULL;
#endif

	//		HSP関連の初期化
	//
	hsp = new Hsp3();

	if ( startfile != NULL ) {
		hsp->SetFileName( startfile );
	}

	//		実行ファイルかデバッグ中かを調べる
	//
	mode = 0;
	orgexe=0;
#ifdef HSPEMSCRIPTEN
	hsp_wx = 960;
	hsp_wy = 640;
#else
	hsp_wx = 320;
	hsp_wy = 480;
#endif
//	hsp_wx = 640;
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

	char *env_wx = getenv( "HSP_WX" );
	if ( env_wx ) {
		int v = atoi( env_wx );
		if ( v > 0 ) {
			hsp_wx = v;
		}
	}

	char *env_wy = getenv( "HSP_WY" );
	if ( env_wy ) {
		int v = atoi( env_wy );
		if ( v > 0 ) {
			hsp_wy = v;
		}
	}

	float sx = 0, sy = 0;

	char *env_sx = getenv( "HSP_SX" );
	if ( env_sx ) {
		sx = atof( env_sx );
	}

	char *env_sy = getenv( "HSP_SY" );
	if ( env_sy ) {
		sy = atof( env_sy );
	}

	if ( sx > 0 && sy > 0 ) {
		//OK
	} else {
		sx = hsp_wx;
		sy = hsp_wy;
	}

	char *env_autoscale = getenv( "HSP_AUTOSCALE" );
	int autoscale = 0;
	if ( env_autoscale ) {
		autoscale = atoi( env_autoscale );
	}

	char *env_fps = getenv( "HSP_FPS" );
	hsp_fps = 0;
	if ( env_fps ) {
		hsp_fps = atoi( env_fps );
	}

	char *env_step = getenv( "HSP_LIMIT_STEP" );
	hsp_limit_step_per_frame = 5000;
	if ( env_step ) {
		hsp_limit_step_per_frame = atoi( env_step );
	}

	// printf("Screen %f %f\n", sx, sy);

	char *env_syncdir = getenv( "HSP_SYNC_DIR" );
	if ( env_syncdir ) {
		syncdir = env_syncdir;
	}

	if ( hsp->Reset( mode ) ) {
		hsp3dish_dialog( "Startup failed." );
		return 1;
	}

	for (int i = 0; i < SDLK_SCANCODE_MAX; i++) {
		keys[i] = false;
	}

	ctx = &hsp->hspctx;

	//		コマンドライン関連
	hsp->SetCommandLinePrm( cl_cmdline );		// コマンドラインパラメーターを保存
	hsp->SetModuleFilePrm( cl_modname );			// モジュール名を保存
	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

	//		Register Type
	//
	drawflag = 0;
	ctx->msgfunc = hsp3dish_msgfunc;

	//		Initalize Window
	//
	hsp3dish_initwindow( NULL, sx, sy, "HSPDish ver" hspver);

	if ( sx != hsp_wx || sy != hsp_wy ) {
#ifndef HSPDISHGP
		hgio_view( hsp_wx, hsp_wy );
		hgio_size( sx, sy );
		hgio_autoscale( autoscale );
#endif
	}

#ifdef HSPDISHGP
	//		Initalize gameplay
	//
	game = new gamehsp;

	gameplay::Logger::set(gameplay::Logger::LEVEL_INFO, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_WARN, logfunc);
	gameplay::Logger::set(gameplay::Logger::LEVEL_ERROR, logfunc);

	//	platform = gameplay::Platform::create( game, NULL, hsp_wx, hsp_wy, false );
	platform = gameplay::Platform::create( game, NULL, hsp_wx, hsp_wy, false );
	if ( platform == NULL ) {
		hsp3dish_dialog( (char *)gplog.c_str() );
		hsp3dish_dialog( "OpenGL initalize failed." );
		return 1;
	}
	platform->enterMessagePump();
	game->frame();
#endif

	//		Initalize GUI System
	//
	hsp3typeinit_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

	exinfo = ctx->exinfo2;

#ifdef USE_OBAQ
	HSP3TYPEINFO *tinfo = code_gettypeinfo( TYPE_USERDEF );
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_dw_extcmd( tinfo );
	//hsp3typeinit_dw_extfunc( code_gettypeinfo( TYPE_USERDEF+1 ) );
#endif


	//		Initalize DEVINFO
	HSP3DEVINFO *devinfo;
	devinfo = hsp3extcmd_getdevinfo();
	hsp3dish_setdevinfo( devinfo );

	return 0;
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
		sprintf( errmsg, "#Error %d --> %s\n",(int)err,msg );
		fname = NULL;
	} else {
		sprintf( errmsg, "#Error %d in line %d (%s)\n-->%s\n",(int)err, ln, fname, msg );
	}
	hsp3dish_debugopen();
	hsp3dish_dialog( errmsg );
}


static void hsp3dish_bye( void )
{
	//		Window関連の解放
	//
	hsp3dish_drawoff();

	//		クリーンアップ
	//
#ifdef HSPERR_HANDLE
	try {
#endif
		hsp->Dispose();
#ifdef HSPERR_HANDLE
	}
	catch (HSPERROR code) {						// HSPエラー例外処理
		hsp->hspctx.err = code;
		hsp3dish_error();
	}
#endif

	//		タイマーの開放
	//
	emscripten_cancel_main_loop();

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

	//		HSP関連の解放
	//
	if ( hsp != NULL ) { delete hsp; hsp = NULL; }

	// if ( m_hWnd != NULL ) {
	// 	hgio_term();
	// 	DestroyWindow( m_hWnd );
	// 	m_hWnd = NULL;
	// }
}


char *hsp3dish_getlog(void)
{
#ifdef HSPDISHGP
	return (char *)gplog.c_str();
#else
	return "";
#endif
}


extern int code_execcmd_one( int& prev );

void hsp3dish_exec_one( void )
{
	if (!fs_initialized) {
		printf("Sync\n");
		return;
	}
	// hgio_test();
	// return;
	int tick;
	switch( ctx->runmode ) {
	case RUNMODE_WAIT:
		tick = hgio_gettick();
		if ( code_exec_wait( tick ) != RUNMODE_RUN ) {
			return;
		}
		break;
	case RUNMODE_AWAIT:
		tick = hgio_gettick();
		if ( code_exec_await( tick ) != RUNMODE_RUN ) {
			//printf("AWAIT %d < %d\n", tick, ctx->waittick);
			return;
		}
		break;
	}
	handleEvent();

	//		実行の開始
	//
	static int code_execcmd_state = 0;
	int runmode;
	bool stop = false;
	int i;
	for (i = 0; !stop && i < hsp_limit_step_per_frame; i++) {
	//for (int i = 0; !stop; i++) {
		runmode = code_execcmd_one(code_execcmd_state);
		switch ( ctx->runmode ){
		case RUNMODE_RUN:
			break;
		case RUNMODE_WAIT:
		case RUNMODE_AWAIT:
			return;
		case RUNMODE_END:
		case RUNMODE_ERROR:
			//printf("BREAK #%d %d %d\n", i, runmode, ctx->runmode);
			stop = true;
			break;
		}
	}
	if (i == hsp_limit_step_per_frame) {
		fprintf(stderr, "OVER HSP_LIMIT_STEP %d\n", hsp_limit_step_per_frame);
	}
	//exit(-1);
	//printf("RUN %d %d\n", runmode, ctx->runmode);
	if ( runmode == RUNMODE_RUN ) {
		return;
	}
	if ( runmode == RUNMODE_ERROR ) {
		try {
			hsp3dish_error();
		}
		catch( ... ) {
		}
		emscripten_cancel_main_loop();
		exit(-1);
		return;
	}
	int endcode = ctx->endcode;
	hsp3dish_bye();
	exit(0);
}

extern "C"
{
void EMSCRIPTEN_KEEPALIVE hsp3dish_sync_done( void )
{
	fs_initialized = true;
}
}

int hsp3dish_exec( void )
{
	if (syncdir.size() > 0) {
		// IDBから読み込み
		fs_initialized = false;
		EM_ASM_({
			var dir = UTF8ToString($0);
			FS.mkdir(dir);
			FS.mount(IDBFS, {}, dir);
			FS.syncfs(true, function (err) {
					console.log(err);
					ccall('hsp3dish_sync_done', 'v', '', []);
				});
			}, syncdir.c_str());
	} else {
		fs_initialized = true;
	}

	//		実行メインを呼び出す
	//
	hsp3dish_msgfunc( ctx );

	//		実行の開始
	//
	emscripten_set_main_loop(hsp3dish_exec_one, hsp_fps, 1);

	return 0;
}


void hsp3dish_option( int opt )
{
	//		HSP3オプション設定
	//
	cl_option = opt;
}


void hsp3dish_cmdline( char *cmdline )
{
	//		HSP3オプション設定
	//
	cl_cmdline = cmdline;						// コマンドラインパラメーターを入れる
}


void hsp3dish_modname( char *modname )
{
	//		HSP3オプション設定
	//
	cl_modname = modname;						// arg[0]パラメーターを入れる
}



