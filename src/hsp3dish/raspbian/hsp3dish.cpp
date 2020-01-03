/*--------------------------------------------------------
	HSP3dish main (raspberry pi/raspbian/OpenGLES)
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <termios.h>

#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <linux/input.h>
#include <stdbool.h>


#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "bcm_host.h"

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

#include "../emscripten/appengine.h"

#include "../../hsp3/linux/devctrl_io.h"
#define DEVCTRL_IO			// GPIO,I2C control

#ifdef HSPDISHGP
#include "../win32gp/gamehsp.h"
#endif

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL/SDL_opengl.h"


//#define USE_OBAQ

#ifdef USE_OBAQ
#include "../obaq/hsp3dw.h"
#endif

//typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

extern char *hsp_mainpath;

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

static engine	mem_engine;
static DISPMANX_ELEMENT_HANDLE_T dispman_element;
static DISPMANX_DISPLAY_HANDLE_T dispman_display;
//static	HWND m_hWnd;

#ifndef HSPDEBUG
static int hsp_sscnt, hsp_ssx, hsp_ssy;
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

static int mouseFd = -1;
static int keyboardFd = -1;
static int quit_flag = 0;
static int mouse_x, mouse_y, mouse_btn1, mouse_btn2;

#ifndef KEY_MAX
#define KEY_MAX 256
#endif
static int key_map[KEY_MAX];

struct input_event ev[64];

static void initKeyboard( void )
{
	DIR *dirp;
	struct dirent *dp;
	regex_t kbd,mouse;

	char fullPath[1024];
	static char *dirName = "/dev/input/by-id";
	int i;

	if(regcomp(&kbd,"event-kbd",0)!=0)
	{
	    printf("regcomp for kbd failed\n");
	    return;

	}
	if(regcomp(&mouse,"event-mouse",0)!=0)
	{
	    printf("regcomp for mouse failed\n");
	    return;

	}

	if ((dirp = opendir(dirName)) == NULL) {
	    perror("couldn't open '/dev/input/by-id'");
	    return;
	}

	// Find any files that match the regex for keyboard or mouse

	do {
	    errno = 0;
	    if ((dp = readdir(dirp)) != NULL) 
	    {
		//printf("readdir (%s)\n",dp->d_name);
		if(regexec (&kbd, dp->d_name, 0, NULL, 0) == 0)
		{
		    //printf("match for the kbd = %s\n",dp->d_name);
		    sprintf(fullPath,"%s/%s",dirName,dp->d_name);
		    keyboardFd = open(fullPath,O_RDONLY | O_NONBLOCK);
		    //printf("%s Fd = %d\n",fullPath,keyboardFd);

		}
		if(regexec (&mouse, dp->d_name, 0, NULL, 0) == 0)
		{
		    //printf("match for the kbd = %s\n",dp->d_name);
		    sprintf(fullPath,"%s/%s",dirName,dp->d_name);
		    mouseFd = open(fullPath,O_RDONLY | O_NONBLOCK);
		    //printf("%s Fd = %d\n",fullPath,mouseFd);
		    //printf("Getting exclusive access: ");
		    ioctl(mouseFd, EVIOCGRAB, 1);
		    //printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");
		}

	    }
	} while (dp != NULL);

	closedir(dirp);

	regfree(&kbd);
	regfree(&mouse);

	mouse_x = (int)mem_engine.width / 2;
	mouse_y = (int)mem_engine.height / 2;
	mouse_btn1 = 0;
	mouse_btn2 = 0;
	for(i=0;i<KEY_MAX;i++) {
		key_map[i] = 0;
	}

}

static void updateKeyboard( void )
{
    int rd;
    int sx,sy;
	if((keyboardFd == -1) || (mouseFd == -1)) return;

	sx = (int)mem_engine.width;
	sy = (int)mem_engine.height;

    // Read events from mouse

    rd = read(mouseFd,ev,sizeof(ev));
    if(rd > 0) {
		int count,n;
		struct input_event *evp;

		count = rd / sizeof(struct input_event);
		n = 0;
		while(count--) {
			evp = &ev[n++];
			if(evp->type == 1) {
				if(evp->code == BTN_LEFT)  {
					//printf("Left button(%d)\n",evp->value);
					mouse_btn1 = evp->value;
			    }
				if(evp->code == BTN_RIGHT)  {
					//printf("Right button(%d)\n",evp->value);
					mouse_btn2 = evp->value;
			    }
			}
	
			if(evp->code == 0) {
			    // Mouse Left/Right
			    //printf("Mouse moved left/right %d\n",evp->value);
			    mouse_x += evp->value;
			    if ( mouse_x < 0 ) mouse_x = 0;
			    if ( mouse_x >= sx ) mouse_x = sx-1;
			}
		
			if(evp->code == 1) {
			    // Mouse Up/Down
			    //printf("Mouse moved up/down %d\n",evp->value);
			    mouse_y += evp->value;
			    if ( mouse_y < 0 ) mouse_y = 0;
			    if ( mouse_y >= sy ) mouse_y = sy-1;
			}
	    }
	}

    // Read events from keyboard

    rd = read(keyboardFd,ev,sizeof(ev));
    if(rd > 0) {
		int count,n;
		struct input_event *evp;
		count = rd / sizeof(struct input_event);
		n = 0;
		while(count--) {
		    evp = &ev[n++];
		    if(evp->type == 1) {
				if (( evp->code >= 0 )&&( evp->code < KEY_MAX )) {
					key_map[evp->code] = evp->value;
				}
				if((evp->code == KEY_ESC) && (evp->value == 1)) {
				    quit_flag = 1;
				}
			}
		}
    }

}


static void doneKeyboard( void )
{
	if (keyboardFd!=-1) close(keyboardFd);
	if (mouseFd!=-1) {
	    ioctl(mouseFd, EVIOCGRAB, 0);
		close(mouseFd);
	}
}


/*----------------------------------------------------------*/

static const int key_cnv[256]={
	/* 0- */
	0, 0, 0, 3, 0, 0, 0, 0, KEY_BACKSPACE, KEY_TAB, 0, 0, 12, KEY_ENTER, 0, 0,
	0, 0, 0, KEY_PAUSE, KEY_CAPSLOCK, 0, 0, 0, 0, 0, 0, KEY_ESC, 0, 0, 0, 0,
	/* 32- */
	KEY_SPACE, KEY_PAGEUP, KEY_PAGEDOWN, KEY_END, KEY_HOME,
	KEY_LEFT, KEY_UP, KEY_RIGHT, KEY_DOWN, 0, KEY_PRINT, 0, 0, KEY_INSERT, KEY_DELETE, KEY_HELP,
	/* 48- */
	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	0, 0, 0, 0, 0, 0, 0,
	/* 65- */
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I,
	KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
	KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	/* 91- */
	KEY_LEFTMETA, KEY_RIGHTMETA, 0, 0, 0,
	KEY_KP0, KEY_KP1, KEY_KP2, KEY_KP3, KEY_KP4, KEY_KP5, KEY_KP6, KEY_KP7, KEY_KP8, KEY_KP9,
	KEY_KPASTERISK, KEY_KPPLUS, 0, KEY_KPMINUS, KEY_KPDOT, KEY_KPSLASH, 
	/* 112- */
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
	KEY_F11, KEY_F12, KEY_F13, KEY_F14, KEY_F15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 136- */
	0, 0, 0, 0, 0, 0, 0, 0, KEY_NUMLOCK, 145,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 160- */
	KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTCTRL, KEY_RIGHTCTRL, KEY_LEFTALT, KEY_RIGHTALT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 186- */
	KEY_APOSTROPHE, KEY_SEMICOLON, KEY_COMMA, KEY_MINUS, KEY_DOT, KEY_SLASH, KEY_LEFTBRACE, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 219- */
	KEY_LEFTBRACE, KEY_BACKSLASH, KEY_RIGHTBRACE, KEY_EQUAL,
	0, 0, 0, KEY_DOLLAR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};



bool get_key_state(int sym)
{
	switch( sym ){
		case 1:
			return (mouse_btn1>0);
		case 2:
			return (mouse_btn2>0);
	}

	int i;
	if ((sym<0)||(sym>255)) return false;
	i = key_cnv[sym];
	return (key_map[i]>0);
}

/*----------------------------------------------------------*/

static void hsp3dish_initwindow( engine* p_engine, int sx, int sy, char *windowtitle )
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };

	static VC_DISPMANX_ALPHA_T alpha= {
	  DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS,
	  255,
	  NULL
	};

	EGLConfig config;
	uint32_t width, height;

	bcm_host_init();
	
   // get an EGL display connection
   p_engine->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(p_engine->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(p_engine->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(p_engine->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   p_engine->context = eglCreateContext(p_engine->display, config, EGL_NO_CONTEXT, NULL);
   assert(p_engine->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &width, &height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = width;
   dst_rect.height = height;

   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = width << 16;
   src_rect.height = height << 16;

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, &alpha /*alpha*/, 0/*clamp*/, DISPMANX_NO_ROTATE /*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = width;
   nativewindow.height = height;
   vc_dispmanx_update_submit_sync( dispman_update );

   p_engine->surface = eglCreateWindowSurface( p_engine->display, config, &nativewindow, NULL );
   assert(p_engine->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(p_engine->display, p_engine->surface, p_engine->surface, p_engine->context);
   assert(EGL_FALSE != result);

	p_engine->width = (int32_t)width;
	p_engine->height = (int32_t)height;

	// 描画APIに渡す
	hgio_init( 0, width, height, p_engine );
	hgio_clsmode( CLSMODE_SOLID, 0xffffff, 0 );
	hgio_title( windowtitle );
}


void hsp3dish_dialog( char *mes )
{
	//MessageBox( NULL, mes, "Error",MB_ICONEXCLAMATION | MB_OK );
	printf( "%s\r\n", mes );
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
	useconds_t usec;

	updateKeyboard();
	hgio_touch( mouse_x, mouse_y, mouse_btn1 );

	//int x, y, btn;
	//btn = get_mouse(&x, &y);
	//hgio_touch( x, y, btn );
#ifdef HSPDEBUG
	//if ( btn ) {
	//	hspctx->runmode = RUNMODE_END;
	//}
	//if ( get_key_state(KEY_ESCAPE) ){	;	// [esc] to Quit
	if ( quit_flag ){	;	// Quit
		hspctx->runmode = RUNMODE_END;
		return;
	}
#endif

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
				usec = ( hspctx->waittick - tick) / 2;
				usleep( usec*1000 );
			} else {
				tick = hgio_gettick();
				while( tick < hspctx->waittick ) {	// 細かいwaitを取る
					usleep( 1000 );
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

			hspctx->runmode = RUNMODE_RUN;
			break;
		}
		default:
			return;
		}
	}
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

	hgio_setmainarg( hsp_mainpath, startfile );

	//		Initalize Window
	//
	hsp3dish_initwindow( &mem_engine, -1, -1, "HSPDish ver" hspver );
	sx = (int)mem_engine.width;
	sy = (int)mem_engine.height;
	autoscale = 0;

	initKeyboard();

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

	//		Register Type
	//
	drawflag = 0;
	ctx->msgfunc = hsp3dish_msgfunc;


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
	HSP3TYPEINFO *tinfo = code_gettypeinfo( -1 ); //TYPE_USERDEF
	tinfo->hspctx = ctx;
	tinfo->hspexinfo = exinfo;
	hsp3typeinit_dw_extcmd( tinfo );
	//hsp3typeinit_dw_extfunc( code_gettypeinfo( TYPE_USERDEF+1 ) );
#endif

	//		Initalize DEVINFO
#ifdef DEVCTRL_IO
	HSP3DEVINFO *devinfo;
	devinfo = hsp3extcmd_getdevinfo();
	hsp3dish_setdevinfo_io( devinfo );
#endif
	return 0;
}


static void hsp3dish_bye( void )
{
	//		Window関連の解放
	//
   engine* p_engine;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   int s;

   p_engine = &mem_engine;
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

   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(p_engine->display, p_engine->surface);

   eglDestroySurface( p_engine->display, p_engine->surface );

   dispman_update = vc_dispmanx_update_start( 0 );
   s = vc_dispmanx_element_remove(dispman_update, dispman_element);
   assert(s == 0);
   vc_dispmanx_update_submit_sync( dispman_update );
   s = vc_dispmanx_display_close( dispman_display );
   assert (s == 0);

   // Release OpenGL resources
   eglMakeCurrent( p_engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroyContext( p_engine->display, p_engine->context );
   eglTerminate( p_engine->display );

	doneKeyboard();
#ifdef DEVCTRL_IO
	hsp3dish_termdevinfo_io();
#endif

	bcm_host_deinit();

	//		HSP関連の解放
	//
	if ( hsp != NULL ) { delete hsp; hsp = NULL; }
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

