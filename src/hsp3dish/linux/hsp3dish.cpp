/*--------------------------------------------------------
	HSP3dish main (Linux/SDL/EGL/OpenGL)
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

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
#include "../../hsp3/linux/hsp3ext_sock.h"

struct engine;

//#include "../emscripten/appengine.h"

#include "../../hsp3/linux/devctrl_io.h"
#define DEVCTRL_IO			// GPIO,I2C control

#ifdef HSPDISHGP
#include "../win32gp/gamehsp.h"
#endif

//#include <GL/gl.h>
//#include <GL/glut.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

//#define USE_OBAQ

#ifdef USE_OBAQ
#include "../obaq/hsp3dw.h"
#endif

extern char *hsp_mainpath;

//typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);
void hsp3dish_msgfunc( HSPCTX *hspctx );

/*----------------------------------------------------------*/

static Hsp3 *hsp = NULL;
static HSPCTX *ctx;
static HSPEXINFO *exinfo;								// Info for Plugins

static char fpas[]={ 'H'-48,'S'-48,'P'-48,'H'-48,
					 'E'-48,'D'-48,'~'-48,'~'-48 };
static char optmes[] = "HSPHED~~\0_1_________2_________3______";

static int hsp_wx, hsp_wy, hsp_wd, hsp_ss;
static int hsp_wposx, hsp_wposy, hsp_wstyle;
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
SDL_Window *window = NULL;
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
	}
}

#endif

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
static int handleEvent( void ) {
	int res=0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
			{
				//int id;
				float x,y;
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				x = event.tfinger.x * bm->sx;
				y = event.tfinger.y * bm->sy;
				//id = event.tfinger.touchId;
				//hgio_mtouchid( id, (int)x, (int)y, 1, 0 );
				hgio_touch( (int)x, (int)y, 1 );
				break;
			}
		case SDL_FINGERUP:
			{
				//int id;
				float x,y;
				Bmscr *bm;
				bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
				x = event.tfinger.x * bm->sx;
				y = event.tfinger.y * bm->sy;
				//id = event.tfinger.touchId;
				//hgio_mtouchid( id, (int)x, (int)y, 1, 0 );
				hgio_touch( (int)x, (int)y, 0 );
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
					bm->setMTouchByPointId( -1, x, y, true );

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
			int code = (int)event.key.keysym.scancode;
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
			if (event.key.keysym.scancode < SDLK_SCANCODE_MAX) {
				keys[event.key.keysym.scancode] = false;
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
			{
			int id,retval;
			id = 0;
			if (code_isirq(HSPIRQ_ONEXIT)) {
				int iparam = 0;
				retval = code_sendirq(HSPIRQ_ONEXIT, iparam, id, 0);
				if (retval == RUNMODE_INTJUMP) retval = code_execcmd2();	// onexit goto時は実行してみる
				if (retval != RUNMODE_END) return 0;
			}
			code_puterror(HSPERR_NONE);
			res = -1;
			break;
			}
		}
	}
	return res;
}

bool get_key_state(int sym)
{
	return keys[sym];
}

void hsp3dish_dialog( char *mes )
{
	//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", mes, window);
	printf( "%s\n", mes );
}

static int hsp3dish_initwindow( engine* p_engine, int sx, int sy, int autoscale, char *windowtitle )
{
	int flags;
	int hsp_fullscr = hsp_wstyle & 0x100;

	flags = SDL_WINDOW_OPENGL;
	if (hsp_fullscr) {
		flags |= SDL_WINDOW_FULLSCREEN;
		hsp_wposx = 0;
		hsp_wposy = 0;
	} else {
		if ( hsp_wstyle & 0x10000) {
			flags |= SDL_WINDOW_BORDERLESS;
		}
	}
	window = SDL_CreateWindow( windowtitle, hsp_wposx, hsp_wposy, sx, sy, flags );
	if ( window==NULL ) {
		printf("Unable to set window: %s\n", SDL_GetError());
		return -1;
	}
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	context = SDL_GL_CreateContext(window);
	if ( window==NULL ) {
		printf("Unable to set GLContext: %s\n", SDL_GetError());
		return -1;
	}

	// 描画APIに渡す
	hgio_init( 0, sx, sy, p_engine );
	hgio_clsmode( CLSMODE_SOLID, 0xffffff, 0 );

	if ( sx != hsp_wx || sy != hsp_wy ) {
#ifndef HSPDISHGP
		hgio_view( sx, sy );
		hgio_size( hsp_wx, hsp_wy );
		hgio_autoscale( autoscale );
#endif
	}

	// マルチタッチ初期化
	//MTouchInit( m_hWnd );
	return 0;
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
	devinfo->devname = "linux";
	devinfo->error = "";
	devinfo->devprm = hsp3dish_devprm;
	devinfo->devcontrol = hsp3dish_devcontrol;
	devinfo->devinfo = hsp3dish_devinfo;
	devinfo->devinfoi = hsp3dish_devinfoi;
}

/*----------------------------------------------------------*/

#ifdef HSPDISHGP
static void hsp3dish_savelog( void )
{
	//		ログをファイルに出力する
	//
	if (game != NULL) {
		const char *logs;
#ifdef GP_USE_MEM_LEAK_DETECTION
		printMemoryLeaks();
#endif
		logs = gplog.c_str();
		mem_save("hsp3gp.log", (void *)logs, (int)strlen(logs), -1);
	}
}
#endif

int hsp3dish_init_sub( int sx, int sy, int autoscale )
{
	//		システム関連の初期化(HSP以外)
	//
	int res;
	for (int i = 0; i < SDLK_SCANCODE_MAX; i++) {
		keys[i] = false;
	}

	//		Register Type
	//
	drawflag = 0;
	ctx->msgfunc = hsp3dish_msgfunc;

	//		Initalize Window
	//
	res = hsp3dish_initwindow( NULL, sx, sy, autoscale, "HSPDish ver" hspver );
	if (res) return res;

	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

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
		//hsp3dish_dialog( (char *)gplog.c_str() );
		hsp3dish_dialog( "OpenGL initalize failed." );
		hsp3dish_savelog();
		return 1;
	}
	platform->enterMessagePump();
	game->frame();
#endif

	return 0;
}

int hsp3dish_init( char *startfile )
{
	//		システム関連の初期化
	//		( mode:0=debug/1=release )
	//
	int a,orgexe, mode;
	int hsp_sum, hsp_dec;
	int autoscale,sx,sy;
	char a1;
#ifdef HSPDEBUG
	int i;
#endif
	InitSysReq();

#ifdef HSPDISHGP
	SetSysReq( SYSREQ_MAXMATERIAL, 128 );            // マテリアルのデフォルト値

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

	if ( hsp->Reset( mode ) ) {
		hsp3dish_dialog( "Startup failed." );
		return 1;
	}

	sx = 0; sy = 0; autoscale = 0;
//#ifdef HSPDEBUG
	if ( OpenIniFile( "hsp3dish.ini" ) == 0 ) {
		int iprm;
		iprm = GetIniFileInt( "wx" );if ( iprm > 0 ) hsp_wx = iprm;
		iprm = GetIniFileInt( "wy" );if ( iprm > 0 ) hsp_wy = iprm;
		iprm = GetIniFileInt( "vx" );if ( iprm > 0 ) sx = iprm;
		iprm = GetIniFileInt( "vy" );if ( iprm > 0 ) sy = iprm;
		iprm = GetIniFileInt( "autoscale" );if ( iprm > 0 ) autoscale = iprm;
		CloseIniFile();
	}

	if ( sx == 0 ) sx = hsp_wx;
	if ( sy == 0 ) sy = hsp_wy;

//#endif
	ctx = &hsp->hspctx;

	//		コマンドライン関連
	hsp->SetCommandLinePrm( cl_cmdline );		// コマンドラインパラメーターを保存
	hsp->SetModuleFilePrm( cl_modname );			// モジュール名を保存

	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

	// Slightly different SDL initialization
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		hsp3dish_dialog("Unable to initialize SDL");
		return 1;
	}

	//		Window initalize
	//
	hsp_wstyle = 0;
	hsp_wposx = SDL_WINDOWPOS_UNDEFINED;
	hsp_wposy = SDL_WINDOWPOS_UNDEFINED;
	if ( hsp3dish_init_sub(sx,sy,autoscale) ) {
		return 1;
	}

	//		Initalize GUI System
	//
	hsp3typeinit_extcmd( code_gettypeinfo( TYPE_EXTCMD ) );
	hsp3typeinit_extfunc( code_gettypeinfo( TYPE_EXTSYSVAR ) );

	exinfo = ctx->exinfo2;

#ifdef USE_OBAQ
	{
	HSP3TYPEINFO *tinfo = code_gettypeinfo( TYPE_USERDEF );
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_dw_extcmd( tinfo );
	}
#endif

	{
	HSP3TYPEINFO *tinfo = code_gettypeinfo( -1 ); //TYPE_USERDEF
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_sock_extcmd( tinfo );
	}

	//		Initalize DEVINFO
	HSP3DEVINFO *devinfo;
	devinfo = hsp3extcmd_getdevinfo();

#ifdef DEVCTRL_IO
	hsp3dish_setdevinfo_io( devinfo );
#else
	hsp3dish_setdevinfo( devinfo );
#endif

#ifdef HSPDISHGP
	gameplay::Logger::log(gameplay::Logger::LEVEL_INFO, "HGIMG4 %s initalized : %s\n", hspver, devinfo->devname);
#endif

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
		sprintf( errmsg, "#Error %d\n-->%s\n",(int)err,msg );
		fname = NULL;
	} else {
		sprintf( errmsg, "#Error %d in line %d (%s)\n-->%s\n",(int)err, ln, fname, msg );
	}
	hsp3dish_debugopen();
	hsp3dish_dialog( errmsg );
}


static void hsp3dish_bye_sub( void )
{
	//		クリーンアップ
	//

	//		Window関連の解放
	//
	hsp3dish_drawoff();
	hgio_term();

#ifdef HSPDISHGP
	//		gameplay関連の解放
	//
	if (platform != NULL) {
		platform->shutdownInternal();
		delete platform;
	}
	if (game != NULL) {
		//game->exit();
	    delete game;
	}
#endif

	if (window) {
		SDL_DestroyWindow(window);
	}
}

static void hsp3dish_bye( void )
{
#ifdef HSPDISHGP
	if (GetSysReq(SYSREQ_LOGWRITE)) {
		hsp3dish_savelog();
	}
#endif

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

	hsp3dish_bye_sub();
	SDL_Quit();

#ifdef DEVCTRL_IO
	hsp3dish_termdevinfo_io();
#endif
	//		HSP関連の解放
	//
	if ( hsp != NULL ) { delete hsp; hsp = NULL; }
}


char *hsp3dish_getlog(void)
{
#ifdef HSPDISHGP
	return (char *)gplog.c_str();
#else
	return "";
#endif
}


int hsp3dish_exec( void )
{
	//		実行メインを呼び出す
	//
	int runmode;
	int endcode;

	hsp3dish_msgfunc( ctx );

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

	endcode = ctx->endcode;
	hsp3dish_bye();
	return endcode;
}


void hsp3dish_msgfunc( HSPCTX *hspctx )
{
	int tick;

	if ( handleEvent() ) {
		hspctx->runmode = RUNMODE_END;
	}

	while(1) {
		// logmes なら先に処理する
		if ( hspctx->runmode == RUNMODE_LOGMES ) {
			printf( "%s\r\n",ctx->stmp );
			hspctx->runmode = RUNMODE_RUN;
			return;
		}

		switch( hspctx->runmode ) {
		case RUNMODE_STOP:
			return;
		case RUNMODE_WAIT:
			tick = hgio_gettick();
			hspctx->runmode = code_exec_wait( tick );
		case RUNMODE_AWAIT:
			//	高精度タイマー
			tick = hgio_gettick();					// すこし早めに抜けるようにする
			if ( code_exec_await( tick ) != RUNMODE_RUN ) {
					SDL_Delay( ( hspctx->waittick - tick) / 2 );
			} else {
				tick = hgio_gettick();
				while( tick < hspctx->waittick ) {	// 細かいwaitを取る
					SDL_Delay(1);
					tick = hgio_gettick();
				}
				hspctx->lasttick = tick;
				hspctx->runmode = RUNMODE_RUN;
#ifndef HSPDEBUG
				if ( ctx->hspstat & HSPSTAT_SSAVER ) {
					if ( hsp_sscnt ) hsp_sscnt--;
				}
#endif
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
		case RUNMODE_RESTART:
		{
			//	rebuild window

			Bmscr* bm;
			bm = (Bmscr*)exinfo->HspFunc_getbmscr(0);
			hsp_wx = bm->sx;
			hsp_wy = bm->sy;
			hsp_wposx = bm->cx;
			hsp_wposy = bm->cy;
			if ( hsp_wposx < 0 ) hsp_wposx = SDL_WINDOWPOS_UNDEFINED;
			if ( hsp_wposy < 0 ) hsp_wposy = SDL_WINDOWPOS_UNDEFINED;
			hsp_wstyle = bm->buffer_option;
			hsp3dish_bye_sub();

			if ( hsp_wstyle & 0x100 ) {
				SDL_DisplayMode dm;
				SDL_GetDesktopDisplayMode(0,&dm);
				hsp_wx = dm.w; hsp_wy = dm.h;
			}
			if ( hsp3dish_init_sub(hsp_wx,hsp_wy,0) ) {
				return;
			}
			hsp3excmd_rebuild_window();
#ifdef USE_OBAQ
			HSP3TYPEINFO *tinfo = code_gettypeinfo( TYPE_USERDEF );
			hsp3typeinit_dw_restart( tinfo );
#endif
			//		Initalize DEVINFO
			HSP3DEVINFO *devinfo;
			devinfo = hsp3extcmd_getdevinfo();

#ifdef DEVCTRL_IO
			hsp3dish_setdevinfo_io( devinfo );
#else
			hsp3dish_setdevinfo( devinfo );
#endif
			hspctx->runmode = RUNMODE_RUN;
			break;
		}
		default:
			return;
		}
	}
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





