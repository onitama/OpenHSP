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
//#include "../hsp3ext.h"
#include "../../hsp3/strnote.h"

struct engine;

//#include "../emscripten/appengine.h"

#ifdef HSPDISHGP
#include "../win32gp/gamehsp.h"
#endif

//#include <GL/gl.h>
//#include <GL/glut.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
//#include "SDL/SDL_opengl.h"

//#define USE_OBAQ

#ifdef USE_OBAQ
#include "../obaq/hsp3dw.h"
#endif

//typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

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

//static	HWND m_hWnd;

#ifndef HSPDEBUG
static int hsp_sscnt, hsp_ssx, hsp_ssy;
#endif

static bool keys[SDLK_LAST];

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
		gplog += msg;
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
		case SDL_MOUSEMOTION:
			{
				Bmscr *bm;
				if ( exinfo != NULL ) {
					SDL_MouseMotionEvent *m = (SDL_MouseMotionEvent*)&event;
					int x, y;
#ifdef HSPDISHGP
					x = m->x;
					y = m->y;
#else
					hgio_scale_point( m->x, m->y, x, y );
#endif

					bm = (Bmscr *)exinfo->HspFunc_getbmscr(0);
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
			if (!keys[event.key.keysym.sym]) {
				keys[event.key.keysym.sym] = true;
				//printf("key down: sym %d scancode %d\n", event.key.keysym.sym, event.key.keysym.scancode);
			}
			break;
		case SDL_KEYUP:
			if (keys[event.key.keysym.sym]) {
				keys[event.key.keysym.sym] = false;
				//printf("key up: sym %d scancode %d\n", event.key.keysym.sym, event.key.keysym.scancode);
			}
			break;
		case SDL_QUIT: /** ウィンドウのxボタンやctrl-Cを押した場合 */
			res = -1;
		}
	}
	return res;
}

bool get_key_state(int sym)
{
	return keys[sym];
}

static void hsp3dish_initwindow( engine* p_engine, int sx, int sy, char *windowtitle )
{
	printf("INIT %dx%d %s\n", sx,sy,windowtitle);
	// glutInit(NULL, NULL);
	// glutInitWindowSize(sx, sy);

	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// glutCreateWindow(windowtitle);

	SDL_Surface *screen;

	// Slightly different SDL initialization
	if ( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	screen = SDL_SetVideoMode( sx, sy, 16, SDL_OPENGL );
	if ( !screen ) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return;
	}
	SDL_WM_SetCaption( "HSPDish ver" hspver, NULL );

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

	if ( handleEvent() ) {
		hspctx->runmode = RUNMODE_END;
	}

	while(1) {
		// logmes なら先に処理する
		if ( hspctx->runmode == RUNMODE_LOGMES ) {
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
	devinfo->devname = "linux";
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
	int autoscale,sx,sy;
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

	for (int i = 0; i < SDLK_LAST; i++) {
		keys[i] = false;
	}

	ctx = &hsp->hspctx;

	//		Register Type
	//
	drawflag = 0;
	ctx->msgfunc = hsp3dish_msgfunc;

	//		Initalize Window
	//
	hsp3dish_initwindow( NULL, sx, sy, "HSPDish ver" hspver );

	if ( sx != hsp_wx || sy != hsp_wy ) {
#ifndef HSPDISHGP
		hgio_view( hsp_wx, hsp_wy );
		hgio_size( sx, sy );
		hgio_autoscale( autoscale );
#endif
	}

//	hsp3typeinit_dllcmd( code_gettypeinfo( TYPE_DLLFUNC ) );
//	hsp3typeinit_dllctrl( code_gettypeinfo( TYPE_DLLCTRL ) );

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
	HSP3TYPEINFO *tinfo = code_gettypeinfo( -1 );// TYPE_USERDEF
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_dw_extcmd( tinfo );
#endif

	//		Initalize DEVINFO
	HSP3DEVINFO *devinfo;
	devinfo = hsp3extcmd_getdevinfo();
	hsp3dish_setdevinfo( devinfo );

	return 0;
}


static void hsp3dish_bye( void )
{
	//		Window関連の解放
	//
	hsp3dish_drawoff();

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

