//
//		Draw lib (gameplay)
//			onion software/onitama 2013/3
//
#include "gamehsp.h"

#include <stdio.h>
#include <math.h>

#define STRICT

#ifdef HSPWIN
#include <windows.h>
#include <mmsystem.h>
#include <string.h>
#include <objbase.h>
#include <commctrl.h>
#include <shellapi.h>
#endif

#ifdef HSPNDK
//#define USE_JAVA_FONT
#define FONT_TEX_SX 512
#define FONT_TEX_SY 128
#include "../../appengine.h"
#include "../../javafunc.h"
//#include "font_data.h"
#endif

#ifdef HSPIOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include <CoreFoundation/CoreFoundation.h>

#include "iOSgpBridge.h"
#endif

#ifdef HSPEMSCRIPTEN
int hgio_fontsystem_get_texid(void);
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
//#include <GL/glut.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"

#include <SDL/SDL_ttf.h>
#define TTF_FONTFILE "/ipaexg.ttf"

extern bool get_key_state(int sym);
extern SDL_Window *window;

#endif

#if defined(HSPLINUX)
#include <unistd.h>
#include <GL/gl.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
//#include "SDL/SDL_opengl.h"

#include <SDL2/SDL_ttf.h>
#define TTF_FONTFILE "/ipaexg.ttf"

extern bool get_key_state(int sym);
extern SDL_Window *window;

#endif


#include "../../hsp3/hsp3config.h"
#include "../hgio.h"
#include "../supio.h"
#include "../sysreq.h"

#ifdef HSPWIN
void hgio_fontsystem_win32_init(HWND wnd);
#endif

#define RELEASE(x) 	if(x){x->Release();x=NULL;}

#ifdef HSPWIN
#ifdef GP_USE_ANGLE
#pragma comment(lib, "libEGL.lib")
#pragma comment(lib, "libGLESv2.lib")
#pragma comment(lib, "gameplay_angle.lib")
#else
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "gameplay.lib")
#endif

//#pragma comment(lib, "glew32.lib")
//#pragma comment(lib, "libpng16.lib")
//#pragma comment(lib, "zlib.lib")
//#pragma comment(lib, "BulletDynamics.lib")
//#pragma comment(lib, "BulletCollision.lib")
//#pragma comment(lib, "LinearMath.lib")

#define M_PI	(3.14159265358979323846f)
#endif

extern gamehsp *game;
extern gameplay::Platform *platform;

static int		mouse_x;
static int		mouse_y;
static int		mouse_btn;
static int   _originX; 	//原点X
static int   _originY; 	//原点Y
static float _scaleX;	// スケールX
static float _scaleY;	// スケールY
static float _rateX;	// 1/スケールX
static float _rateY;	// 1/スケールY

#ifdef HSPNDK
static engine	*appengine;
#endif

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
extern bool get_key_state(int sym);
#endif

#ifdef HSPIOS
static	double  total_tick;
static	CFAbsoluteTime  lastTime;
#endif

#define USE_STAR_FIELD

/*------------------------------------------------------------*/
/*
		HSP File Service
*/
/*------------------------------------------------------------*/

#define MFPTR_MAX 8
static char *mfptr[MFPTR_MAX];
static int mfptr_depth;

void InitMemFile( void )
{
	mfptr_depth = 0;
	mfptr[0] = NULL;
}


int OpenMemFilePtr( char *fname )
{
	int fsize;
	fsize = dpm_exist( fname );		// ファイルのサイズを取得
	if ( fsize <= 0 ) return -1;
	mfptr_depth++;
	if ( mfptr_depth >= MFPTR_MAX ) return -1;
	mfptr[mfptr_depth] = (char *)malloc( fsize );
	dpm_read( fname, mfptr[mfptr_depth], fsize, 0 );	// ファイル読み込み
	return fsize;
}


char *GetMemFilePtr( void )
{
	return mfptr[mfptr_depth];
}


void CloseMemFilePtr( void )
{
	if ( mfptr_depth == 0 ) return;
	if ( mfptr[mfptr_depth] != NULL ) {
		free( mfptr[mfptr_depth] ); mfptr[mfptr_depth]=NULL;
		mfptr_depth--;
	}
}


/*------------------------------------------------------------*/
/*
		Star Field
*/
/*------------------------------------------------------------*/

#ifdef USE_STAR_FIELD

#define STAR_RNG_PERIOD     ((1 << 17) - 1)
#define RGB_MAXIMUM         224
#define STAR_SX         256
#define STAR_SY         256

static unsigned char m_stars[STAR_RNG_PERIOD];
static int m_stars_enabled = 0;
static int m_stars_count;
static int m_star_rng_origin;
static unsigned int m_star_color[64];

static void star_init(void)
{
	//	星(StarField)の初期化
	m_stars_count = 0;
	m_stars_enabled = 1;

	//	テーブル作成
	unsigned int shiftreg;
	int i;

	shiftreg = 0;
	for (i = 0; i < STAR_RNG_PERIOD; i++)
	{
		int enabled = ((shiftreg & 0x1fe01) == 0x1fe00);
		int color = (~shiftreg & 0x1f8) >> 3;
		m_stars[i] = color | (enabled << 7);
		// LFSRによる乱数生成
		shiftreg = (shiftreg >> 1) | ((((shiftreg >> 12) ^ ~shiftreg) & 1) << 16);
	}

	unsigned int minval = RGB_MAXIMUM * 130 / 150;
	unsigned int midval = RGB_MAXIMUM * 130 / 100;
	unsigned int maxval = RGB_MAXIMUM * 130 / 60;

	unsigned int starmap[4]{
			0,
			minval,
			minval + (255 - minval) * (midval - minval) / (maxval - minval),
			255 };

	for (i = 0; i < 64; i++)
	{
		int bit0, bit1;

		bit0 = (i >> 5) & 1;
		bit1 = (i >> 4) & 1;
		int r = starmap[(bit1 << 1) | bit0];
		bit0 = (i >> 3) & 1;
		bit1 = (i >> 2) & 1;
		int g = starmap[(bit1 << 1) | bit0];
		bit0 = (i >> 1) & 1;
		bit1 = i & 1;
		int b = starmap[(bit1 << 1) | bit0];
		m_star_color[i] = 0xff000000 + (r << 16) + (g << 8) + (b);
	}

}

static void star_draw_y(unsigned char *dest, int y, int maxx, int offset)
{
	//	星(StarField)の描画(1line)
	int x;
	int ofs;
	unsigned int *ptr;
	unsigned char star;

	ofs = offset %= STAR_RNG_PERIOD;
	ptr = (unsigned int*)dest;

	/* iterate over the specified number of 6MHz pixels */
	for (x = 0; x < maxx; x++)
	{
		int enable_star = (y ^ (x >> 3)) & 1;
		star = m_stars[ofs++];
		if (ofs >= STAR_RNG_PERIOD) ofs = 0;
		if (enable_star && (star & 0x80) != 0 && (star & 0xff) != 0) {
			*ptr++ = m_star_color[star & 63];
		}
		else {
			*ptr++ = 0;
		}
	}

}

static void star_draw(char *dest, int sx, int sy, int mode)
{
	//	星(StarField)の描画
	if (m_stars_enabled == 0) return;

	int y;
	unsigned char *ptr = (unsigned char*)dest;

	for (y = 0; y < 224; y++)
	{
		int star_offs = (m_star_rng_origin >> 1) + y * 512;
		star_draw_y(ptr, y, 256, star_offs);
		ptr += sx;
	}

	m_star_rng_origin += (mode & 3);
	if (mode & 4) {
		if ((m_stars_count & 63) == 63) {
			m_star_rng_origin = rand() + rand();
		}
	}
	m_stars_count++;
}

#endif

/*------------------------------------------------------------*/
/*
		gameplay Service
*/
/*------------------------------------------------------------*/

//		Settings
//
static		int nDestWidth;		// 描画座標幅
static		int nDestHeight;	// 描画座標高さ
static		float _rate_sx,_center_sx;
static		float _rate_sy, _center_sy;

#ifdef HSPWIN
static		HWND master_wnd;	// 表示対象Window
static		HCURSOR cursor_arrow;	// 通常カーソル
static		HCURSOR cursor_ibeam;	// テキストエリア用カーソル
#endif
static		int drawflag;		// レンダー開始フラグ
static		BMSCR mestexbm;		// テキスト表示用ダミーBMSCR

static		BMSCR *mainbm;		// メインスクリーンのBMSCR
static		HSPREAL infoval[GINFO_EXINFO_MAX];
static		BMSCR *backbm;		// 背景消去用のBMSCR(null=NC)
static		BMSCR *gselbm;		// 描画先のBMSCR

static		char m_tfont[256];	// テキスト使用フォント
static		int m_tsize;		// テキスト使用フォントのサイズ
static		int m_tstyle;		// テキスト使用フォントのスタイル指定

static		float center_x,center_y;
static		float linebasex,linebasey;

static		MATRIX mat_proj;	// プロジェクションマトリクス
static		MATRIX mat_unproj;	// プロジェクション逆変換マトリクス

#define CIRCLE_DIV 16
#define DEFAULT_FONT_NAME ""
//#define DEFAULT_FONT_NAME "Arial"
#define DEFAULT_FONT_SIZE 16
#define DEFAULT_FONT_STYLE 0


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void hgio_init( int mode, int sx, int sy, void *hwnd )
{
	//		ファイルサービス設定
	//
	InitMemFile();

	//		設定の初期化
	//
	SetSysReq( SYSREQ_RESULT, 0 );
	SetSysReq( SYSREQ_RESVMODE, 0 );

#ifdef HSPWIN
	master_wnd = (HWND)hwnd;
	hgio_fontsystem_win32_init(master_wnd);
#endif

#ifdef HSPNDK
	appengine = (engine *)hwnd;
#endif

	mainbm = NULL;
	backbm = NULL;
	gselbm = NULL;
	drawflag = 0;
	nDestWidth = sx;
	nDestHeight = sy;
	_rate_sx = 2.0f / (float)sx;
	_rate_sy = 2.0f / (float)sy;
	_center_sx = (float)sx / 2;
	_center_sy = (float)sy / 2;

#if defined(HSPNDK) || defined(HSPIOS) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
	_originX = 0;
	_originY = 0;
	_scaleX = 1.0f;
	_scaleY = 1.0f;
	_rateX = 1.0f;
	_rateY = 1.0f;
#endif
	GeometryInit();

	//		infovalをリセット
	//
	int i;
	for(i=0;i<GINFO_EXINFO_MAX;i++) {
		infoval[i] = 0.0;
	}

    //  timer initalize
#ifdef HSPIOS
    total_tick = 0.0;
    lastTime = CFAbsoluteTimeGetCurrent();
#endif

#if defined(HSPLINUX)
	//TTF初期化
	char fontpath[HSP_MAX_PATH+1];
	*fontpath = 0;
	strcpy( fontpath, hgio_getdir(1) );
	strcat( fontpath, TTF_FONTFILE );

	if ( TTF_Init() ) {
		Alertf( "Init:TTF_Init error" );
	}
	TexFontInit( fontpath, 18 );
#endif

#ifdef HSPWIN
	//		カーソル読み込み
	//
	cursor_arrow = LoadCursor(NULL, IDC_ARROW);
	cursor_ibeam = LoadCursor(NULL, IDC_IBEAM);
#endif

#ifdef USE_STAR_FIELD
	star_init();
#endif
}


void hgio_clsmode( int mode, int color, int tex )
{
	SetSysReq( SYSREQ_CLSMODE, mode );
	SetSysReq( SYSREQ_CLSCOLOR, color );
	SetSysReq( SYSREQ_CLSTEX, tex );
}


int hgio_device_restore( void )
{
	//	デバイスの修復
	//		(0=OK/1=NG)
	//
	return 0;
}


void hgio_resume( void )
{
	hgio_device_restore();
}


int hgio_render_end( void )
{
	//printf("hgio_render_end\n");
	int res;

	if ( drawflag == 0 ) return 0;

	game->texmesProc();


	res = 0;

	if (gselbm == mainbm) {
		// メイン画面の場合はフリップ
		if (platform) platform->swapBuffers();
	}
	else {
		// オフスクリーンの場合
		game->resumeFrameBuffer();
	}

	drawflag = 0;
	return res;
}


int hgio_render_start( void )
{
	//printf("hgio_render_start\n");
	if ( drawflag ) {
		hgio_render_end();
	}

	if ((gselbm->type != HSPWND_TYPE_MAIN) && (gselbm->type != HSPWND_TYPE_OFFSCREEN)) {
		return -1;
	}

#ifdef HSPIOS
    if ( game->hasAccelerometer()) {
        float accx,accy,accz,gyx,gyy,gyz;
        game->getSensorValues(&accx, &accy, &accz, &gyx, &gyy, &gyz);
        hgio_setinfo( GINFO_EXINFO_ACCEL_X, accx );
        hgio_setinfo( GINFO_EXINFO_ACCEL_Y, accy );
        hgio_setinfo( GINFO_EXINFO_ACCEL_Z, accz );
        hgio_setinfo( GINFO_EXINFO_GYRO_X, gyx );
        hgio_setinfo( GINFO_EXINFO_GYRO_Y, gyy );
        hgio_setinfo( GINFO_EXINFO_GYRO_Z, gyz );
    }
#endif
	//シーンレンダー開始
	if (game) {
		if (gselbm == mainbm) {
			// メイン画面の場合
			game->frame();
		}
		else {
			// オフスクリーンの場合
			game->selectFrameBuffer((gameplay::FrameBuffer *)gselbm->master_buffer, gselbm->sx, gselbm->sy);
		}
	}

	hgio_setview(gselbm);
	drawflag = 1;
	return 0;
}


int hgio_gsel(BMSCR *bm)
{
	//		描画先設定
	//		(gsel相当)
	//
	hgio_render_end();
	gselbm = bm;

	// プロジェクションの初期化
	int id = gselbm->texid;
	if (id >= 0) {
		gpmat *mat;
		mat = game->getMat(id);
		game->update2DRenderProjectionSystem(&mat->_projectionMatrix2D);
	}

	return 0;
}


int hgio_buffer(BMSCR *bm)
{
	//		オフスクリーン作成
	//		(buffer相当)
	//
	int option = 0;
	if (bm->type == HSPWND_TYPE_OFFSCREEN) {
		bm->master_buffer = game->makeFremeBuffer(bm->resname, bm->sx, bm->sy);
		if (bm->master_buffer  == NULL) {
			return -1;
		}
		if (bm->buffer_option & HSPWND_OPTION_USERSHADER) option |= GPOBJ_MATOPT_USERSHADER;
		bm->texid = game->makeNewMatFromFB((gameplay::FrameBuffer *)bm->master_buffer, option);
		if (bm->texid < 0) return bm->texid;
		return 0;
	}
	return -1;
}


void hgio_screen( BMSCR *bm )
{
	//		スクリーン再設定
	//		(cls相当)
	//
	drawflag = 0;
	if (bm->type == HSPWND_TYPE_MAIN) {
		mainbm = bm;
		gselbm = bm;
	}
	hgio_font( DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE );
}


void hgio_setback(BMSCR *bm)
{
	//		背景画像の設定
	//		(NULL=なし)
	//
	backbm = bm;
}


void hgio_delscreen(BMSCR *bm)
{
	//		スクリーンを破棄
	//		(Bmscrクラスのdelete時)
	//
	if ( bm->flag == BMSCR_FLAG_NOUSE ) return;
	if ( bm->texid != -1 ) {
		game->deleteMat( bm->texid );
		bm->texid = -1;
	}
	if (bm->type == HSPWND_TYPE_OFFSCREEN) {
		if (bm->master_buffer) {
			game->deleteFrameBuffer((gameplay::FrameBuffer *)bm->master_buffer);
		}
	}
	bm->flag = BMSCR_FLAG_NOUSE;
}


int hgio_getWidth( void )
{
	return nDestWidth;
}


int hgio_getHeight( void )
{
	return nDestHeight;
}


int hgio_getDesktopWidth( void )
{
#ifdef HSPLINUX
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0,&dm);
	return dm.w;
#endif
	return nDestWidth;
}


int hgio_getDesktopHeight( void )
{
#ifdef HSPLINUX
	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0,&dm);
	return dm.h;
#endif
	return nDestHeight;
}


void hgio_term( void )
{
	hgio_render_end();
	GeometryTerm();
#ifdef HSPWIN
	hgio_fontsystem_term();
#endif

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
	TexFontTerm();
#endif
}


void hgio_setColorTex( int rval, int gval ,int bval )
{
}


int hgio_stick( int actsw )
{
	//		stick用の入力を返す
	//
	int ckey = 0;

#if defined(HSPNDK)||defined(HSPIOS)
	if ( mouse_btn ) ckey|=256;	// mouse_l
#endif

#ifdef HSPWIN
	HWND hwnd;
	if ( actsw ) {
		hwnd = GetActiveWindow();
		if ( hwnd != master_wnd ) return 0;
	}

	if ( GetAsyncKeyState(37)&0x8000 ) ckey|=1;		// [left]
	if ( GetAsyncKeyState(38)&0x8000 ) ckey|=2;		// [up]
	if ( GetAsyncKeyState(39)&0x8000 ) ckey|=4;		// [right]
	if ( GetAsyncKeyState(40)&0x8000 ) ckey|=8;		// [down]
	if ( GetAsyncKeyState(32)&0x8000 ) ckey|=16;	// [spc]
	if ( GetAsyncKeyState(13)&0x8000 ) ckey|=32;	// [ent]
	if ( GetAsyncKeyState(17)&0x8000 ) ckey|=64;	// [ctrl]
	if ( GetAsyncKeyState(27)&0x8000 ) ckey|=128;	// [esc]
	if ( GetAsyncKeyState(1)&0x8000 )  ckey|=256;	// mouse_l
	if ( GetAsyncKeyState(2)&0x8000 )  ckey|=512;	// mouse_r
	if ( GetAsyncKeyState(9)&0x8000 )  ckey|=1024;	// [tab]

	if (GetAsyncKeyState(90) & 0x8000)  ckey |= 1 << 11;	// [z]
	if (GetAsyncKeyState(88) & 0x8000)  ckey |= 1 << 12;	// [x]
	if (GetAsyncKeyState(67) & 0x8000)  ckey |= 1 << 13;	// [c]

	if (GetAsyncKeyState(65) & 0x8000)  ckey |= 1 << 14;	// [a]
	if (GetAsyncKeyState(87) & 0x8000)  ckey |= 1 << 15;	// [w]
	if (GetAsyncKeyState(68) & 0x8000)  ckey |= 1 << 16;	// [d]
	if (GetAsyncKeyState(83) & 0x8000)  ckey |= 1 << 17;	// [s]
#endif

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
	if ( get_key_state(SDL_SCANCODE_LEFT) )  ckey|=1;		// [left]
	if ( get_key_state(SDL_SCANCODE_UP) )    ckey|=1<<1;		// [up]
	if ( get_key_state(SDL_SCANCODE_RIGHT) ) ckey|=1<<2;		// [right]
	if ( get_key_state(SDL_SCANCODE_DOWN) )  ckey|=1<<3;		// [down]
	if ( get_key_state(SDL_SCANCODE_SPACE) ) ckey|=1<<4;		// [spc]
	if ( get_key_state(SDL_SCANCODE_RETURN) )ckey|=1<<5;		// [ent]
	if ( get_key_state(SDL_SCANCODE_LCTRL) || get_key_state(SDL_SCANCODE_RCTRL) ) ckey|=1<<6;		// [ctrl]
	if ( get_key_state(SDL_SCANCODE_ESCAPE) )ckey|=1<<7;	// [esc]
	if ( mouse_btn & SDL_BUTTON_LMASK ) ckey|=1<<8;	// mouse_l
	if ( mouse_btn & SDL_BUTTON_RMASK ) ckey|=1<<9;	// mouse_r
	if ( get_key_state(SDL_SCANCODE_TAB) )   ckey|=1<<10;	// [tab]
	
	if ( get_key_state(SDL_SCANCODE_Z) )     ckey|=1<<11;
	if ( get_key_state(SDL_SCANCODE_X) )     ckey|=1<<12;
	if ( get_key_state(SDL_SCANCODE_C) )     ckey|=1<<13;
	
	if ( get_key_state(SDL_SCANCODE_A) )     ckey|=1<<14;
	if ( get_key_state(SDL_SCANCODE_W) )     ckey|=1<<15;
	if ( get_key_state(SDL_SCANCODE_D) )     ckey|=1<<16;
	if ( get_key_state(SDL_SCANCODE_S) )     ckey|=1<<17;
#endif

	return ckey;
}

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
static const unsigned int key_map[256]={
	/* 0- */
	0, 0, 0, 3, 0, 0, 0, 0, SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_TAB, 0, 0, 12, SDL_SCANCODE_RETURN, 0, 0,
	0, 0, 0, SDL_SCANCODE_PAUSE, SDL_SCANCODE_CAPSLOCK, 0, 0, 0, 0, 0, 0, SDL_SCANCODE_ESCAPE, 0, 0, 0, 0,
	/* 32- */
	SDL_SCANCODE_SPACE, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN, SDL_SCANCODE_END, SDL_SCANCODE_HOME,
	SDL_SCANCODE_LEFT, SDL_SCANCODE_UP, SDL_SCANCODE_RIGHT, SDL_SCANCODE_DOWN, 0, SDL_SCANCODE_PRINTSCREEN, 0, 0, SDL_SCANCODE_INSERT, SDL_SCANCODE_DELETE, SDL_SCANCODE_HELP,
	/* 48- */
	SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9,
	0, 0, 0, 0, 0, 0, 0,
	/* 65- */
	SDL_SCANCODE_A, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H, SDL_SCANCODE_I,
	SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L, SDL_SCANCODE_M, SDL_SCANCODE_N, SDL_SCANCODE_O, SDL_SCANCODE_P, SDL_SCANCODE_Q, SDL_SCANCODE_R,
	SDL_SCANCODE_S, SDL_SCANCODE_T, SDL_SCANCODE_U, SDL_SCANCODE_V, SDL_SCANCODE_W, SDL_SCANCODE_X, SDL_SCANCODE_Y, SDL_SCANCODE_Z,
	/* 91- */
	SDL_SCANCODE_LGUI, SDL_SCANCODE_RGUI, 0, 0, 0,
	SDL_SCANCODE_KP_0, SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_9,
	SDL_SCANCODE_KP_MULTIPLY, SDL_SCANCODE_KP_PLUS, 0, SDL_SCANCODE_KP_MINUS, SDL_SCANCODE_KP_PERIOD, SDL_SCANCODE_KP_DIVIDE, 
	/* 112- */
	SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4, SDL_SCANCODE_F5, SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8, SDL_SCANCODE_F9, SDL_SCANCODE_F10,
	SDL_SCANCODE_F11, SDL_SCANCODE_F12, SDL_SCANCODE_F13, SDL_SCANCODE_F14, SDL_SCANCODE_F15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 136- */
	0, 0, 0, 0, 0, 0, 0, 0, SDL_SCANCODE_NUMLOCKCLEAR, 145,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 160- */
	SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL, SDL_SCANCODE_LALT, SDL_SCANCODE_RALT,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 186- */
	SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_COMMA, SDL_SCANCODE_MINUS, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* 219- */
	SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_BACKSLASH, SDL_SCANCODE_RIGHTBRACKET, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

bool hgio_getkey( int kcode )
{
	bool res = false;
	switch( kcode ){
		case 1: res = (mouse_btn & SDL_BUTTON_LMASK) > 0; break;
		case 2: res = (mouse_btn & SDL_BUTTON_RMASK) > 0; break;
		case 4: res = (mouse_btn & SDL_BUTTON_MMASK) > 0; break;
		case 5: res = (mouse_btn & SDL_BUTTON_X1MASK) > 0; break;
		case 6: res = (mouse_btn & SDL_BUTTON_X2MASK) > 0; break;
		case 16: res = get_key_state(SDL_SCANCODE_LSHIFT) | get_key_state(SDL_SCANCODE_RSHIFT); break;
		case 17: res = get_key_state(SDL_SCANCODE_LCTRL) | get_key_state(SDL_SCANCODE_RCTRL); break;
		case 18: res = get_key_state(SDL_SCANCODE_LALT) | get_key_state(SDL_SCANCODE_RALT); break;
		default: res = get_key_state( key_map[ kcode & 255 ] ); break;
	}
	return res;
}
#endif


int hgio_redraw( BMSCR *bm, int flag )
{
	//		redrawモード設定
	//		(必ずredraw 0～redraw 1をペアにすること)
	//
	if ( bm == NULL ) return -1;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return -1;

	if ( flag & 1 ) {
		hgio_render_end();
		int curtick = hgio_gettick();
		if (bm->prevtime) {
			bm->passed_time = curtick - bm->prevtime;
		}
		bm->prevtime = curtick;
	} else {
		hgio_render_start();
	}

#ifdef HSPWIN
	//	ウインドウアクティブの更新
	//
	HWND hwnd;
	hwnd = GetActiveWindow();
	if (hwnd != master_wnd) {
		bm->window_active = 0;
	}
	else {
		bm->window_active = 1;
	}

	//	カーソルの更新
	//
	HSPOBJINFO *info = bm->cur_mo_obj;
	HCURSOR hc = cursor_arrow;

	if (info) {
		if (info->owmode & (HSPOBJ_OPTION_EDITSEL | HSPOBJ_OPTION_MULTISEL)) {
			hc = cursor_ibeam;
		}
	}
	SetCursor(hc);
	SetClassLong(hwnd, -12, (LONG)hc);
#endif

	return 0;
}


int hgio_dialog( int mode, char *str1, char *str2 )
{
	//		dialog表示
	//
#ifdef HSPWIN
	int i,res;
	i = 0;
	if (mode&1) i|=MB_ICONEXCLAMATION; else i|=MB_ICONINFORMATION;
	if (mode&2) i|=MB_YESNO; else i|=MB_OK;
	res = MessageBox( master_wnd, str1, str2, i );
	return res;
#endif
#ifdef HSPNDK
	j_dispDialog( str1, str2, mode );
#endif
#ifdef HSPIOS
    gpb_dialog( mode, str1, str2 );
    //Alertf( str1 );
#endif
#ifdef HSPLINUX
	{
	int i = 0;
	if (mode>=16) return 0;
	if (mode&1) i|=SDL_MESSAGEBOX_WARNING; else i|=SDL_MESSAGEBOX_INFORMATION;
	SDL_ShowSimpleMessageBox(i, str2, str1, NULL);
	}
#endif
	return 0;
}


int hgio_title( char *str1 )
{
	//		title変更
	//
#ifdef HSPWIN
	SetWindowText( master_wnd, str1 );
#endif

#if defined(HSPEMSCRIPTEN)
	SDL_SetWindowTitle( window, (const char *)str1 );
	//SDL_WM_SetCaption( (const char *)str1, NULL );
#endif
#if defined(HSPLINUX)
#ifndef HSPRASPBIAN
	SDL_SetWindowTitle( window, (const char *)str1 );
	//SDL_WM_SetCaption( (const char *)str1, NULL );
#endif
#endif

	return 0;
}


int hgio_texload( BMSCR *bm, char *fname )
{
	//		テクスチャ読み込み
	//
	gpmat *mat;

	bm->texid = game->makeNewMat2D( fname, 0 );
	if ( bm->texid < 0 ) return bm->texid;

	mat = game->getMat( bm->texid );

	bm->sx = mat->_sx;
	bm->sy = mat->_sy;

	return 0;
}


int hgio_mestex(BMSCR *bm, texmesPos *tpos)
{
	//		TEXMESPOSによる文字表示
	//
	int mode, x, y, sx, sy;
	int orgx, orgy;
	int tx, ty;
	int xsize, ysize;
	int esx, esy;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return -1;
	if (drawflag == 0) hgio_render_start();

	texmesManager *tmes = game->getTexmesManager();

	// print per line
	orgx = bm->cx;
	orgy = bm->cy;
	mode = tpos->mode;

	sx = tpos->sx;
	if (sx <= 0) {
		sx = bm->sx - orgx;
		if (sx <= 0) return -1;
	}
	sy = tpos->sy;
	if (sy <= 0) {
		sy = bm->sy - orgy;
		if (sy <= 0) return -1;
	}

	int id = tpos->texid;
	if (id < 0) {
		char *str = tpos->getString();
		id = tmes->texmesRegist(str, tpos);
		if (id < 0) {
			ysize = tmes->_fontsize;
			x = orgx; y = orgy;
			if (mode & TEXMES_MODE_CENTERX) {
				int px = sx / 2;
				x += px;
			}
			if (mode & TEXMES_MODE_CENTERY) {
				int py = (sy - ysize) / 2;
				if (py < 0) { py = 0; }
				y += py;
			}
			tpos->lastcx = x;
			tpos->lastcy = y;
			tpos->printysize = ysize;
			return -1;
		}
		tpos->texid = id;
	}

	texmes* tex;
	tex = tmes->texmesUpdateLife(id);
	if (tex == NULL) return -1;

	xsize = tex->sx;
	ysize = tex->sy;
	tpos->printysize = ysize;

	x = orgx; y = orgy;
	tx = 0; ty = 0;

	esx = x + sx;
	esy = y + sy;

	if (mode & TEXMES_MODE_CENTERX) {
		int px = (sx - xsize) / 2;
		if (px < 0) { px = 0; }
		x += px;
	}
	if (mode & TEXMES_MODE_CENTERY) {
		int py = (sy - ysize) / 2;
		if (py < 0) { py = 0; }
		y += py;
	}
	tpos->lastcx = x;
	tpos->lastcy = y;

	if (tpos->attribute == NULL) {
		if ((x + xsize) >= esx) {
			xsize = esx - x;
			if (xsize <= 0) return -1;
		}
		if ((y + ysize) >= esy) {
			ysize = esy - y;
			if (ysize <= 0) return -1;
		}
		game->texmesDrawClip( bm, x, y, xsize, ysize, tex, tx, ty);
	}

	bm->cy += ysize;
	bm->printsizex = xsize;
	bm->printsizey = ysize;
	return 0;
}


int hgio_mes(BMSCR* bm, char* msg)
{
	//		mes,print 文字表示
	//
	int xsize, ysize;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return -1;
	if (drawflag == 0) hgio_render_start();

	if (GetSysReq(SYSREQ_USEGPBFONT)) {
		xsize = game->drawFont( bm, bm->cx, bm->cy, msg, &ysize );
		bm->printsizex = xsize;
		bm->printsizey = ysize;
		bm->cy += ysize;
		return 0;
	}

	texmesManager *tmes = game->getTexmesManager();

	// print per line
	if (bm->cy >= bm->sy) return -1;

	if (*msg == 0) {
		ysize = tmes->_fontsize;
		bm->printsizey += ysize;
		bm->cy += ysize;
		return 0;
	}

	int id;
	texmes* tex;
	id = tmes->texmesRegist(msg);
	if (id < 0) return -1;
	tex = tmes->texmesGet(id);
	if (tex == NULL) return -1;

	xsize = tex->sx;
	ysize = tex->sy;

	if (bm->printoffsetx > 0) {			// センタリングを行う(X)
		int offset = (bm->printoffsetx - xsize) / 2;
		if (offset > 0) {
			bm->cx += offset;
		}
		bm->printoffsetx = 0;
	}
	if (bm->printoffsety > 0) {			// センタリングを行う(Y)
		int offset = (bm->printoffsety - ysize) / 2;
		if (offset > 0) {
			bm->cy += offset;
		}
		bm->printoffsety = 0;
	}

	game->texmesDrawClip(bm, bm->cx, bm->cy, xsize, ysize, tex, 0, 0);

	//xsize = game->drawFont(bm->cx, bm->cy, str1, (gameplay::Vector4*)bm->colorvalue, &ysize);
	if (xsize > bm->printsizex) bm->printsizex = xsize;
	bm->printsizey += ysize;
	bm->cy += ysize;
	return 0;
}


/*------------------------------------------------------------*/
/*
		Universal Draw Service
*/
/*------------------------------------------------------------*/

void hgio_line( BMSCR *bm, float x, float y )
{
	//		ライン描画
	//		(bm!=NULL の場合、ライン描画開始)
	//		(bm==NULL の場合、ライン描画完了)
	//		(ラインの座標は必要な数だけhgio_line2を呼び出す)
	//
	if ( bm == NULL ) {
		return;
	}
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	float r_val = bm->colorvalue[0];
	float g_val = bm->colorvalue[1];
	float b_val = bm->colorvalue[2];
	game->setPolyDiffuse2D( r_val, g_val, b_val, 1.0f );

	linebasex = x + 0.5f;
	linebasey = y + 0.5f;
}


void hgio_line2( float x, float y )
{
	//		ライン描画
	//		(hgio_lineで開始後に必要な回数呼ぶ、hgio_line(NULL)で終了すること)
	//

	float *v = game->startLineColor2D();

	*v++ = linebasex; *v++ = linebasey; v++;
	v+=4;
	linebasex = x + 0.5f;
	linebasey = y + 0.5f;
	*v++ = linebasex; *v++ = linebasey; v++;

	game->drawLineColor2D();

}


void hgio_boxfAlpha(BMSCR *bm, float x1, float y1, float x2, float y2, int alphamode)
{
	//		矩形描画
	//
	if (bm == NULL) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	float *v = game->startPolyColor2D();
	float r_val = bm->colorvalue[0];
	float g_val = bm->colorvalue[1];
	float b_val = bm->colorvalue[2];
	//float a_val = bm->colorvalue[3];

	float a_val;
	if (alphamode) {
		a_val = game->setPolyColorBlend(bm->gmode, bm->gfrate);
	}
	else {
		a_val = game->setPolyColorBlend(0, 0);
	}

	game->setPolyDiffuse2D(r_val, g_val, b_val, a_val);

	*v++ = x1; *v++ = y2; v++;
	v += 4;
	//*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x1; *v++ = y1; v++;
	v += 4;
	//*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x2; *v++ = y2; v++;
	v += 4;
	//*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;
	*v++ = x2; *v++ = y1; v++;
	//*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	game->drawPolyColor2D();
}


void hgio_boxf( BMSCR *bm, float x1, float y1, float x2, float y2 )
{
	hgio_boxfAlpha(bm, x1, y1, x2, y2, 0);
}


void hgio_circle( BMSCR *bm, float x1, float y1, float x2, float y2, int mode )
{
	//		円描画
	//
	float x,y,rx,ry,sx,sy,rate;
	if ( bm == NULL ) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	float *v;
	float *v_master = game->startPolyColor2D();
	float r_val = bm->colorvalue[0];
	float g_val = bm->colorvalue[1];
	float b_val = bm->colorvalue[2];
	float a_val = game->setPolyColorBlend( 0, 0 );
	game->setPolyDiffuse2D( r_val, g_val, b_val, a_val );

	rate = M_PI * 2.0f / (float)CIRCLE_DIV;
	sx = abs(x2-x1); sy = abs(y2-y1);
	rx = sx * 0.5f;
	ry = sy * 0.5f;
	x = x1 + rx;
	y = y1 + ry;

	for(int i = 1; i<=CIRCLE_DIV; i ++) {

		v = v_master;

		*v++ = x;
		*v++ = y;
		v++;
		v+=4;

		*v++ = x + cos((float)i * rate)*rx;
		*v++ = y + sin((float)i * rate)*ry;
		v++;
		v+=4;

		*v++ = x + cos((float)(i+1) * rate)*rx;
		*v++ = y + sin((float)(i+1) * rate)*ry;
		v++;
		v+=4;

		game->addPolyColor2D( 3 );
	}

	game->finishPolyColor2D();
}


void hgio_fillrot( BMSCR *bm, float x, float y, float sx, float sy, float ang )
{
	//		矩形(回転)描画
	//
	if ( bm == NULL ) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	float x0,y0,x1,y1,ofsx,ofsy;
	float *v = game->startPolyColor2D();
	float r_val = bm->colorvalue[0];
	float g_val = bm->colorvalue[1];
	float b_val = bm->colorvalue[2];
	//float a_val = bm->colorvalue[3];

	float a_val = game->setPolyColorBlend( bm->gmode, bm->gfrate );

	ofsx = sx;
	ofsy = sy;
	x0 = -(float)sin( ang );
	y0 = (float)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

	*v++ = (-x0+x1) + x;
	*v++ = (-y0+y1) + y;
	v++;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	*v++ = (-x0-x1) + x;
	*v++ = (-y0-y1) + y;
	v++;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	*v++ = (x0+x1) + x;
	*v++ = (y0+y1) + y;
	v++;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	*v++ = (x0-x1) + x;
	*v++ = (y0-y1) + y;
	v++;
	*v++ = r_val; *v++ = g_val; *v++ = b_val; *v++ = a_val;

	game->drawPolyColor2D();
}


void hgio_copy(BMSCR *bm, short xx, short yy, short srcsx, short srcsy, BMSCR *bmsrc, float s_psx, float s_psy)
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	float psx, psy;
	float x1, y1, x2, y2, sx, sy;
	float tx0, ty0, tx1, ty1;

	if (bm == NULL) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	gpmat *mat = game->getMat(bmsrc->texid);
	if (mat == NULL) return;
	float *v = game->startPolyTex2D(mat, bm->texid);
	if (v == NULL) return;

	float a_val = game->setMaterialBlend(mat->_material, bm->gmode, bm->gfrate);

	game->setPolyDiffuseTex2D(bm->mulcolorvalue[0], bm->mulcolorvalue[1], bm->mulcolorvalue[2], a_val);

	if (s_psx < 0.0) {
		psx = -s_psx;
		tx1 = ((float)xx);
		tx0 = ((float)(xx + srcsx));
	}
	else {
		psx = s_psx;
		tx0 = ((float)xx);
		tx1 = ((float)(xx + srcsx));
	}
	if (s_psy < 0.0) {
		psy = -s_psy;
		ty1 = ((float)yy);
		ty0 = ((float)(yy + srcsy));
	}
	else {
		psy = s_psy;
		ty0 = ((float)yy);
		ty1 = ((float)(yy + srcsy));
	}

	x1 = ((float)bm->cx);
	y1 = ((float)bm->cy);
	x2 = x1 + psx;
	y2 = y1 + psy;

	sx = mat->_texratex;
	sy = mat->_texratey;

	tx0 *= sx;
	tx1 *= sx;
	ty0 = 1.0f - ty0 * sy;
	ty1 = 1.0f - ty1 * sy;

	*v++ = x1; *v++ = y2; v++;
	*v++ = tx0; *v++ = ty1;
	v += 4;
	//*v++ = c_val; *v++ = c_val; *v++ = c_val; *v++ = a_val;
	*v++ = x1; *v++ = y1; v++;
	*v++ = tx0; *v++ = ty0;
	v += 4;
	//*v++ = c_val; *v++ = c_val; *v++ = c_val; *v++ = a_val;
	*v++ = x2; *v++ = y2; v++;
	*v++ = tx1; *v++ = ty1;
	v += 4;
	//*v++ = c_val; *v++ = c_val; *v++ = c_val; *v++ = a_val;
	*v++ = x2; *v++ = y1; v++;
	*v++ = tx1; *v++ = ty0;
	//v+=4;
	//*v++ = c_val; *v++ = c_val; *v++ = c_val; *v++ = a_val;

	game->drawPolyTex2D(mat);
}


int hgio_celputmulti(BMSCR *bm, int *xpos, int *ypos, int *cel, int count, BMSCR *bmsrc)
{
	//		マルチ画像コピー
	//		int配列内のX,Y,CelIDを元に等倍コピーを行なう(count=個数)
	//		カレントポジション、描画モードはBMSCRから取得
	//
	int psx,psy;
	float f_psx,f_psy;
	float x1,y1,x2,y2,sx,sy;
	float tx0,ty0,tx1,ty1;
	float *master_v;
	float *v;
	int i;
	int id;
	int *p_xpos;
	int *p_ypos;
	int *p_cel;
	int xx,yy;
	int total;

	if ( bm == NULL ) return 0;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return 0;
	if (drawflag == 0) hgio_render_start();

	gpmat *mat = game->getMat(bmsrc->texid);
	if (mat == NULL) return 0;
	master_v = game->startPolyTex2D(mat, bm->texid);
	if (master_v == NULL) return 0;

	float a_val = game->setMaterialBlend( mat->_material, bm->gmode, bm->gfrate );
	game->setPolyDiffuseTex2D(bm->mulcolorvalue[0], bm->mulcolorvalue[1], bm->mulcolorvalue[2], a_val);

	total =0;

	p_xpos = xpos;
	p_ypos = ypos;
	p_cel = cel;

	sx = mat->_texratex;
	sy = mat->_texratey;
	psx = bmsrc->divsx;
	psy = bmsrc->divsy;
	f_psx = (float)psx;
	f_psy = (float)psy;

	for(i=0;i<count;i++) {

		id = *p_cel;

		if ( id >= 0 ) {

			xx = ( id % bmsrc->divx ) * psx;
			yy = ( id / bmsrc->divx ) * psy;

			tx0 = ((float)xx);
			tx1 = tx0 + f_psx;

			ty0 = ((float)yy);
			ty1 = ty0 + f_psy;

			x1 = ((float)(*p_xpos - bmsrc->celofsx));
			y1 = ((float)(*p_ypos - bmsrc->celofsy));
			x2 = x1 + f_psx;
			y2 = y1 + f_psy;

			tx0 *= sx;
			tx1 *= sx;
			ty0 = 1.0f - ty0 * sy;
			ty1 = 1.0f - ty1 * sy;

			v = master_v;

			*v++ = x1; *v++ = y2; v++;
			*v++ = tx0; *v++ = ty1;
			v+=4;
			*v++ = x1; *v++ = y1; v++;
			*v++ = tx0; *v++ = ty0;
			v+=4;
			*v++ = x2; *v++ = y2; v++;
			*v++ = tx1; *v++ = ty1;
			v+=4;
			*v++ = x2; *v++ = y1; v++;
			*v++ = tx1; *v++ = ty0;

			game->addPolyTex2D( mat );
			total++;
		}

		p_xpos++;
		p_ypos++;
		p_cel++;

	}

	game->finishPolyTex2D( mat );
	return total;
}


void hgio_copyrot( BMSCR *bm, short xx, short yy, short srcsx, short srcsy, float s_ofsx, float s_ofsy, BMSCR *bmsrc, float psx, float psy, float ang )
{
	//		画像コピー
	//		texid内の(xx,yy)-(xx+srcsx,yy+srcsy)を現在の画面に(psx,psy)サイズでコピー
	//		カレントポジション、描画モードはBMSCRから取得
	//
	float x,y,x0,y0,x1,y1,ofsx,ofsy,mx0,mx1,my0,my1;
	float tx0,ty0,tx1,ty1,sx,sy;

	if ( bm == NULL ) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	gpmat *mat = game->getMat(bmsrc->texid);
	if (mat == NULL) return;
	float *v = game->startPolyTex2D(mat, bm->texid);
	if (v == NULL) return;

	float a_val = game->setMaterialBlend( mat->_material, bm->gmode, bm->gfrate );

	game->setPolyDiffuseTex2D(bm->mulcolorvalue[0], bm->mulcolorvalue[1], bm->mulcolorvalue[2], a_val);

	mx0=-(float)sin( ang );
	my0=(float)cos( ang );
	mx1 = -my0;
	my1 = mx0;

	ofsx = -s_ofsx;
	ofsy = -s_ofsy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	//		基点の算出
	x = ( (float)bm->cx - (-x0+x1) );
	y = ( (float)bm->cy - (-y0+y1) );

	//		回転座標の算出
	ofsx = -psx;
	ofsy = -psy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	sx = mat->_texratex;
	sy = mat->_texratey;

	tx0 = (float)xx;
	ty0 = (float)yy;
	tx1 = (float)(xx+srcsx);
	ty1 = (float)(yy+srcsy);

	tx0 *= sx;
	tx1 *= sx;
	ty0 = 1.0f - ty0 * sy;
	ty1 = 1.0f - ty1 * sy;

	*v++ = ((-x0) + x);
	*v++ = ((-y0) + y);
	v++;
	*v++ = tx0;
	*v++ = ty1;
	v+=4;

	*v++ = ((-x0+x1) + x);
	*v++ = ((-y0+y1) + y);
	v++;
	*v++ = tx1;
	*v++ = ty1;
	v+=4;

	*v++ = (x);
	*v++ = (y);
	v++;
	*v++ = tx0;
	*v++ = ty0;
	v+=4;

	*v++ = ((x1) + x);
	*v++ = ((y1) + y);
	v++;
	*v++ = tx1;
	*v++ = ty0;

	game->drawPolyTex2D( mat );
}


void hgio_setfilter( int type, int opt )
{
	int curid;
	if (gselbm == NULL) return;

	curid = gselbm->texid;
	if (curid < 0) return;
	gpmat *mat = game->getMat(curid);
	if (mat == NULL) return;
	switch( type ) {
	case HGIO_FILTER_TYPE_LINEAR:
	case HGIO_FILTER_TYPE_LINEAR2:
		mat->setFilter(Texture::Filter::LINEAR);
		break;
	default:
		mat->setFilter(Texture::Filter::NEAREST);
		break;
	}
}


void hgio_square_tex( BMSCR *bm, int *posx, int *posy, BMSCR *bmsrc, int *uvx, int *uvy )
{
	//		四角形(square)テクスチャ描画
	//
	float sx,sy;
	if ( bm == NULL ) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	gpmat *mat = game->getMat(bmsrc->texid);
	if (mat == NULL) return;
	float *v = game->startPolyTex2D(mat, bm->texid);
	if (v == NULL) return;

	float a_val = game->setMaterialBlend( mat->_material, bm->gmode, bm->gfrate );

	game->setPolyDiffuseTex2D( 1.0f, 1.0f, 1.0f, a_val );

	sx = mat->_texratex;
	sy = mat->_texratey;

	*v++ = (float)posx[3];
	*v++ = (float)posy[3];
	v++;
	*v++ = ((float)uvx[3]) * sx;
	*v++ = ((float)uvy[3]) * sy;
	v+=4;

	*v++ = (float)posx[0];
	*v++ = (float)posy[0];
	v++;
	*v++ = ((float)uvx[0]) * sx;
	*v++ = ((float)uvy[0]) * sy;
	v+=4;

	*v++ = (float)posx[2];
	*v++ = (float)posy[2];
	v++;
	*v++ = ((float)uvx[2]) * sx;
	*v++ = ((float)uvy[2]) * sy;
	v+=4;

	*v++ = (float)posx[1];
	*v++ = (float)posy[1];
	v++;
	*v++ = ((float)uvx[1]) * sx;
	*v++ = ((float)uvy[1]) * sy;

	game->drawPolyTex2D( mat );
}


void hgio_square( BMSCR *bm, int *posx, int *posy, int *color )
{
	//		四角形(square)単色描画
	//
	if ( bm == NULL ) return;
	if ((bm->type != HSPWND_TYPE_MAIN) && (bm->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	gameplay::Vector4 colvec;
	float *v = game->startPolyColor2D();
	float valw = game->setPolyColorBlend( bm->gmode, bm->gfrate );

	game->colorVector3( color[3], colvec );
	*v++ = (float)posx[3];
	*v++ = (float)posy[3];
	v++;
	*v++ = colvec.x; *v++ = colvec.y; *v++ = colvec.z; *v++ = valw;

	game->colorVector3( color[0], colvec );
	*v++ = (float)posx[0];
	*v++ = (float)posy[0];
	v++;
	*v++ = colvec.x; *v++ = colvec.y; *v++ = colvec.z; *v++ = valw;

	game->colorVector3( color[2], colvec );
	*v++ = (float)posx[2];
	*v++ = (float)posy[2];
	v++;
	*v++ = colvec.x; *v++ = colvec.y; *v++ = colvec.z; *v++ = valw;

	game->colorVector3( color[1], colvec );
	*v++ = (float)posx[1];
	*v++ = (float)posy[1];
	v++;
	*v++ = colvec.x; *v++ = colvec.y; *v++ = colvec.z; *v++ = valw;

	game->drawPolyColor2D();
}


#if defined(HSPLINUX) || defined(HSPNDK)
    static time_t basetick;
    static bool tick_reset = false;
#endif

int hgio_gettick( void )
{
    // 経過時間の計測
#ifdef HSPWIN
	return timeGetTime();
#endif

#if defined(HSPLINUX) || defined(HSPNDK) || defined(HSPEMSCRIPTEN)
	int i;
	timespec ts;
	double nsec;
    clock_gettime(CLOCK_REALTIME,&ts);
    nsec = (double)(ts.tv_nsec) * 0.001 * 0.001;
    //i = (int)ts.tv_sec * 1000 + (int)nsec;
    time_t sec = ts.tv_sec;
    if ( tick_reset ) {
	sec -= basetick;
    } else {
	tick_reset = true;
	basetick = sec;
	sec = 0;
     }
    i =((int)sec) * 1000 + (int)nsec;
    return i;
#endif

#ifdef HSPIOS
    CFAbsoluteTime now;
    now = CFAbsoluteTimeGetCurrent();
    total_tick += now - lastTime;
    lastTime = now;
    return (int)(total_tick * 1000.0 );
#endif

}


int hgio_exec( char *stmp, char *option, int mode )
{
#ifdef HSPWIN
	int i,j;
	j=SW_SHOWDEFAULT;if (mode&2) j=SW_SHOWMINIMIZED;

	if ( *option != 0 ) {
		SHELLEXECUTEINFO exinfo;
		memset( &exinfo, 0, sizeof(SHELLEXECUTEINFO) );
		exinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		exinfo.fMask = SEE_MASK_INVOKEIDLIST;
		exinfo.hwnd = master_wnd;
		exinfo.lpVerb = option;
		exinfo.lpFile = stmp;
		exinfo.nShow = SW_SHOWNORMAL;
		if (ShellExecuteEx(&exinfo) == false) return-1;
		return 0;
	}
		
	if ( mode&16 ) {
		i = (int)ShellExecute( NULL,NULL,stmp,"","",j );
	}
	else if ( mode&32 ) {
		i = (int)ShellExecute( NULL,"print",stmp,"","",j );
	}
	else {
		i=WinExec( stmp,j );
	}
	if (i < 32) return -1;
#endif
#ifdef HSPIOS
    gpb_exec( mode, stmp );
#endif
	return 0;
}


HSPREAL hgio_getinfo( int type )
{
	int i;
	i = type - GINFO_EXINFO_BASE;
	if (( i >= 0 )&&( i < GINFO_EXINFO_MAX)) {
		return infoval[i];
	}
	return 0.0;
}

void hgio_setinfo( int type, HSPREAL val )
{
	int i;
	i = type - GINFO_EXINFO_BASE;
	if (( i >= 0 )&&( i < GINFO_EXINFO_MAX)) {
		infoval[i] = val;
	}
}

char *hgio_sysinfo( int p2, int *res, char *outbuf )
{
	//		System strings get
	//
#if HSPWIN
	int fl;
	char pp[128];
	char *p1;
	BOOL success;
	DWORD version;
	DWORD size;
	DWORD *mss;
	SYSTEM_INFO si;
	MEMORYSTATUS ms;

	fl = HSPVAR_FLAG_INT;
	p1 = outbuf;
	size = HSP_MAX_PATH;

	if (p2&16) {
		GetSystemInfo(&si);
	}
	if (p2&32) {
		GlobalMemoryStatus(&ms);
		mss=(DWORD *)&ms;
		*(int *)p1 = (int)mss[p2&15];
		*res = fl;
		return p1;
	}

	switch(p2) {
	case 0:
		strcpy(p1, "Windows");
		version = GetVersion();
		if ((version & 0x80000000) == 0) strcat(p1,"NT");
									else strcat(p1,"9X");
		sprintf( pp," ver%d.%d", static_cast< int >( version&0xff ), static_cast< int >( (version&0xff00)>>8 ) );
		strcat( p1, pp );
		fl=HSPVAR_FLAG_STR;
		break;
	case 1:
		success = GetUserName( p1,&size );
		fl = HSPVAR_FLAG_STR;
		break;
	case 2:
		success = GetComputerName(p1, &size );
		fl = HSPVAR_FLAG_STR;
		break;
	case 16:
		*(int *)p1 = (int)si.dwProcessorType;
		break;
	case 17:
		*(int *)p1 = (int)si.dwNumberOfProcessors;
		break;
	default:
		return NULL;
	}
	*res = fl;
	return p1;
#else
	int fl;
	char *p1;
	fl = HSPVAR_FLAG_STR;
	p1 = outbuf;
	*p1=0;

	switch(p2) {
	case 0:
#ifdef HSPNDK
		strcpy(p1, "Android");
#endif
#ifdef HSPIOS
        //strcpy(p1, "iOS");
        gpb_getSysVer( p1 );
#endif
		fl=HSPVAR_FLAG_STR;
		break;
	case 1:
		break;
	case 2:
		break;
	default:
		return NULL;
	}
	*res = fl;
	return p1;
#endif
}

#ifdef HSPWIN
HWND hgio_gethwnd( void )
{
	return master_wnd;
}
#endif


/*------------------------------------------------------------*/
/*
		HGIMG4 Sprite service
*/
/*------------------------------------------------------------*/

void hgio_draw_gpsprite(Bmscr *bmscr, bool lateflag)
{
	float zx, zy, rot;
	gpobj *obj;
	gpspr *spr;

	if (bmscr == NULL) return;
	if ((bmscr->type != HSPWND_TYPE_MAIN) && (bmscr->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	game->findSpriteObj(lateflag);
	while (1) {
		obj = game->getNextSpriteObj();
		if (obj == NULL) break;
		spr = obj->_spr;
		if (spr) {

			zx = spr->_scale.x;
			zy = spr->_scale.y;
			rot = spr->_ang.z;

			bmscr->cx = (int)spr->_pos.x;
			bmscr->cy = (int)spr->_pos.y;
			bmscr->gmode = spr->_gmode;
			bmscr->gfrate = obj->_transparent;

			if ((rot == 0.0f) && (zx == 1.0f) && (zy == 1.0f)) {
				//	変形なし
				bmscr->CelPut((Bmscr *)spr->_bmscr, spr->_celid);
			}
			else {
				//	変形あり
				bmscr->CelPut((Bmscr *)spr->_bmscr, spr->_celid, zx, zy, rot);
			}
		}
	}

}


void hgio_draw_all(Bmscr *bmscr, int option)
{
	if (bmscr == NULL) return;
	if ((bmscr->type != HSPWND_TYPE_MAIN) && (bmscr->type != HSPWND_TYPE_OFFSCREEN)) return;
	if (drawflag == 0) hgio_render_start();

	game->drawAll(option);
}


#ifdef HSPNDK
//
//		FILE I/O Service
//
static char storage_path[256];
static char my_storage_path[256+64];

int hgio_file_exist( char *fname )
{
	int size;
	AAssetManager* mgr = appengine->app->activity->assetManager;
	if (mgr == NULL) return -1;
	AAsset* asset = AAssetManager_open(mgr, (const char *)fname, AASSET_MODE_UNKNOWN);
	if (asset == NULL) return -1;
    size = (int)AAsset_getLength(asset);
    AAsset_close(asset);
	//Alertf( "[EXIST]%s:%d",fname,size );
    return size;
}


int hgio_file_read( char *fname, void *ptr, int size, int offset )
{
	int readsize;
	AAssetManager* mgr = appengine->app->activity->assetManager;
	if (mgr == NULL) return -1;
	AAsset* asset = AAssetManager_open(mgr, (const char *)fname, AASSET_MODE_UNKNOWN);
	if (asset == NULL) return -1;
    readsize = (int)AAsset_getLength(asset);
	if ( readsize > size ) readsize = size;
	if ( offset>0 ) AAsset_seek( asset, offset, SEEK_SET );
	AAsset_read( asset, ptr, readsize );
    AAsset_close(asset);
    return readsize;
}


void hgio_setstorage( char *path )
{
	int i;
	*storage_path = 0;
	i = strlen(path);if (( i<=0 )||( i>=255 )) return;
	strcpy( storage_path, path );
	if ( storage_path[i-1]!='/' ) {
		storage_path[i] = '/';
		storage_path[i+1] = 0;
	}
}


char *hgio_getstorage( char *fname )
{
	strcpy( my_storage_path, storage_path );
	strcat( my_storage_path, fname );
	return my_storage_path;
}
#endif

/*-------------------------------------------------------------------------------*/

#if defined(HSPNDK)||defined(HSPIOS)

void hgio_touch( int xx, int yy, int button )
{
    Bmscr *bm;
	mouse_x = ( xx - _originX ) * _rateX;
	mouse_y = ( yy - _originY ) * _rateY;
	mouse_btn = button;
    if ( mainbm != NULL ) {
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEX] = mouse_x;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEY] = mouse_y;
        mainbm->tapstat = button;
        bm = (Bmscr *)mainbm;
        bm->UpdateAllObjects();
        bm->setMTouchByPointId( 0, mouse_x, mouse_y, button!=0 );
    }
}

void hgio_mtouch( int old_x, int old_y, int xx, int yy, int button, int opt )
{
    Bmscr *bm;
    int x,y,old_x2,old_y2;
    if ( mainbm == NULL ) return;
    bm = (Bmscr *)mainbm;
	x = ( xx - _originX ) * _rateX;
	y = ( yy - _originY ) * _rateY;
    if ( opt == 0) {
        mouse_x = x;
        mouse_y = y;
        mouse_btn = button;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEX] = mouse_x;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEY] = mouse_y;
        mainbm->tapstat = button;
        bm->UpdateAllObjects();
    }
    if ( old_x >= 0 ) {
        old_x2 = ( old_x - _originX ) * _rateX;
    } else {
        old_x2 = old_x;
    }
    if ( old_y >= 0 ) {
        old_y2 = ( old_y - _originY ) * _rateY;
    } else {
        old_y2 = old_y;
    }
    bm->setMTouchByPoint( old_x2, old_y2, x, y, button!=0 );
}

void hgio_mtouchid( int pointid, int xx, int yy, int button, int opt )
{
    Bmscr *bm;
    int x,y;

    if ( mainbm == NULL ) return;
    bm = (Bmscr *)mainbm;
	x = ( xx - _originX ) * _rateX;
	y = ( yy - _originY ) * _rateY;
    if ( opt == 0 ) {
        mouse_x = x;
        mouse_y = y;
        mouse_btn = button;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEX] = mouse_x;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEY] = mouse_y;
        mainbm->tapstat = button;
        bm->UpdateAllObjects();
    }
    bm->setMTouchByPointId( pointid, x, y, button!=0 );
}

int hgio_getmousex( void )
{
	return mouse_x;
}


int hgio_getmousey( void )
{
	return mouse_y;
}


int hgio_getmousebtn( void )
{
	return mouse_btn;
}

#endif

/*-------------------------------------------------------------------------------*/

#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)

char *hgio_getstorage( char *fname )
{
	return fname;
}

void hgio_touch( int xx, int yy, int button )
{
    Bmscr *bm;
	mouse_x = ( xx - _originX ) * _rateX;
	mouse_y = ( yy - _originY ) * _rateY;
	mouse_btn = button;
    if ( mainbm != NULL ) {
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEX] = mouse_x;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEY] = mouse_y;
        mainbm->tapstat = button;
        bm = (Bmscr *)mainbm;
        bm->UpdateAllObjects();
        bm->setMTouchByPointId( 0, mouse_x, mouse_y, button!=0 );
    }
}

void hgio_mtouchid( int pointid, int xx, int yy, int button, int opt )
{
    Bmscr *bm;
    int x,y;

    if ( mainbm == NULL ) return;
    bm = (Bmscr *)mainbm;
	x = ( xx - _originX ) * _rateX;
	y = ( yy - _originY ) * _rateY;
    if ( opt == 0 ) {
        mouse_x = x;
        mouse_y = y;
        mouse_btn = button;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEX] = mouse_x;
        mainbm->savepos[BMSCR_SAVEPOS_MOSUEY] = mouse_y;
        mainbm->tapstat = button;
        bm->UpdateAllObjects();
    }
    bm->setMTouchByPointId( pointid, x, y, button!=0 );
}

void hgio_mtouchidf( int pointid, float xx, float yy, int button, int opt )
{
    int realx,realy;
    realx=(int)(xx*nDestWidth);
    realy=(int)(yy*nDestHeight);
	hgio_mtouchid(pointid,realx,realy,button,opt);
}

#endif


/*-------------------------------------------------------------------------------*/

#if defined(HSPLINUX)

static	char dir_hsp[HSP_MAX_PATH+1];
static	char dir_cmdline[HSP_MAX_PATH+1];

void hgio_setmainarg( char *hsp_mainpath, char *cmdprm )
{
	int p,i;
	strcpy( dir_hsp, hsp_mainpath );

	p = 0; i = 0;
	while(dir_hsp[i]){
		if(dir_hsp[i]=='/' || dir_hsp[i]=='\\') p=i;
		i++;
	}
	dir_hsp[p]=0;

	strcpy( dir_cmdline, cmdprm );
}

char *hgio_getdir( int id )
{
	//		dirinfo命令の内容を設定する
	//
	char dirtmp[HSP_MAX_PATH+1];
	char *p;
	
	*dirtmp = 0;
	p = dirtmp;

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
		getcwd( p, HSP_MAX_PATH );
		break;
	case 1:				//    HSPの実行ファイルがあるディレクトリ
		p = dir_hsp;
		break;
	case 2:				//    Windowsディレクトリ
		break;
	case 3:				//    Windowsのシステムディレクトリ
		break;
	case 4:				//    コマンドライン文字列
		p = dir_cmdline;
		break;
	case 5:				//    HSPTV素材があるディレクトリ
		strcpy( p, dir_hsp );
		strcat( p, "/hsptv" );
		break;
	default:
		throw HSPERR_ILLEGAL_FUNCTION;
	}
	return p;
}

#endif


static int GetSurface(int x, int y, int sx, int sy, int px, int py, void *res, int mode)
{
	//	VRAMの情報を取得する
	//
	int ybase = nDestHeight - (sy - y);

#ifdef	GP_USE_ANGLE
	return -1;
#else
#if defined(HSPWIN)||defined(HSPLINUX)
	glReadBuffer(GL_BACK);
#endif
#endif

	// OpenGLで画面に描画されている内容をバッファに格納
	glReadPixels(
		x,              //読み取る領域の左下隅のx座標
		ybase,          //読み取る領域の左下隅のy座標
		sx,             //読み取る領域の幅
		sy,             //読み取る領域の高さ
		GL_RGBA,		//取得したい色情報の形式
		GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
		res                //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
	);

	return 0;
}

int hgio_bufferop(BMSCR* bm, int mode, char* ptr)
{
	//		オフスクリーンバッファを操作
	//
	gpmat* mat;
	int texid = bm->texid;
	if (texid < 0) return -1;

#ifdef USE_STAR_FIELD
	if (mode & 0x1000) {
		star_draw(ptr, STAR_SX * 4, STAR_SY, mode & 0xfff);
		mat = game->getMat(texid);
		return mat->updateTex32(ptr, 0);
	}
#endif

	switch (mode) {
	case 0:
	case 1:
		mat = game->getMat(texid);
		return mat->updateTex32(ptr, mode);
	case 16:
	case 17:
		GetSurface(0, 0, bm->sx, bm->sy, 1, 1, ptr, mode & 15);
		return 0;
	default:
		return -2;
	}

	return 0;
}


void hgio_text_render(void)
{
	//		テキストバッファのレンダリング(WIN32のみ)
	//
}


int hgio_font(char* fontname, int size, int style)
{
	//		文字フォント指定
	//
	if (game) {
		game->setFont(fontname, size, style);
	}
	return 0;
}


#if defined(HSPEMSCRIPTEN)
int hgio_fontsystem_setup(int sx, int sy, void *buffer)
{
	return hgio_fontsystem_get_texid();
}
#endif


void hgio_editputclip(BMSCR* bm, char *str)
{
	//		クリップボードコピー
	//
#ifdef HSPWIN
	HGLOBAL hg;
	char *strMem;
	if (!OpenClipboard(master_wnd)) return;
	EmptyClipboard();

	int len = strlen(str) + 1;
	hg = GlobalAlloc(GHND | GMEM_SHARE, len);
	strMem = (char *)GlobalLock(hg);
	strcpy(strMem, str);
	GlobalUnlock(hg);

	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
#endif
#if (defined(HSPLINUX)||defined(HSPEMSCRIPTEN))
	SDL_SetClipboardText( (const char *)str );
#endif
}


char *hgio_editgetclip(BMSCR* bm)
{
	//		クリップボードペースト文字列取得
	//
#ifdef HSPWIN
	HGLOBAL hg;
	char *strClip;
	if (OpenClipboard(master_wnd) && (hg = GetClipboardData(CF_TEXT))) {
		char *p = code_stmp(GlobalSize(hg));
		strClip = (char *)GlobalLock(hg);
		strcpy(p, strClip);
		GlobalUnlock(hg);
		CloseClipboard();
		return p;
	}
#endif
#if (defined(HSPLINUX)||defined(HSPEMSCRIPTEN))
	if ( SDL_HasClipboardText() ) {
		return (SDL_GetClipboardText());
	}
#endif
	return NULL;
}

/*-------------------------------------------------------------------------------*/

void hgio_setview(BMSCR* bm)
{
	// vp_flagに応じたビューポートの設定を行う
	//
	int i;
	MATRIX* vmat;
	MATRIX tmpmat;
	float* vp;
	vmat = &mat_proj;
	vp = (float*)vmat;
	float* mat = (float*)GetCurrentMatrixPtr();

	switch (bm->vp_flag) {
	case BMSCR_VPFLAG_2D:
		//	2D projection mode
		UnitMatrix();
		RotZ(bm->vp_viewrotate[2]);
		GetCurrentMatrix(&tmpmat);
		OrthoMatrix(-bm->vp_viewtrans[0], -bm->vp_viewtrans[1], (float)nDestWidth / bm->vp_viewscale[0], (float)nDestHeight / bm->vp_viewscale[1], 0.0f, 1.0f);
		MulMatrix(&tmpmat);
		break;
	case BMSCR_VPFLAG_3D:
		//	3D projection mode
		UnitMatrix();
		RotZ(bm->vp_viewrotate[2]);
		RotY(bm->vp_viewrotate[1]);
		RotX(bm->vp_viewrotate[0]);
		Scale(bm->vp_viewscale[0], bm->vp_viewscale[1], bm->vp_viewscale[2]);
		Trans(bm->vp_viewtrans[0], bm->vp_viewtrans[1], bm->vp_viewtrans[2]);
		GetCurrentMatrix(&tmpmat);
		PerspectiveFOV(bm->vp_view3dprm[0], bm->vp_view3dprm[1], bm->vp_view3dprm[2], 0.0f, 0.0f, (float)nDestWidth / 10, (float)nDestHeight / 10);
		//PerspectiveFOV(45.0f, 1.0f, 500.0f, 0.0f, 0.0f, (float)nDestWidth / 10, (float)nDestHeight / 10);
		//PerspectiveWithZBuffer(10.0f, 0.0f, 60.0f, 1.0f, 0.0f);
		MulMatrix(&tmpmat);
		break;
	case BMSCR_VPFLAG_MATRIX:
		//	user matrix mode
		mat = &bm->vp_viewtrans[0];
		break;
	case BMSCR_VPFLAG_NOUSE:
	default:
		return;
	}

	//	mat_projに設定する
	for (i = 0; i < 16; i++) {
		*vp++ = *mat++;
	}

	bool setinv = (bm == mainbm);

	mat = (float*)GetCurrentMatrixPtr();
	Matrix dstmat(
		mat[0], mat[4], mat[8], mat[12],
		mat[1], mat[5], mat[9], mat[13],
		mat[2], mat[6], mat[10], mat[14],
		mat[3], mat[7], mat[11], mat[15]
		);
	game->setUser2DRenderProjectionSystem(&dstmat, setinv);

#if 1
	if (setinv) {
		//	投影マトリクスの逆行列を設定する
		SetCurrentMatrix(vmat);
		InverseMatrix(&mat_unproj);
	}
#endif
}


void hgio_cnvview(BMSCR* bm, int* xaxis, int* yaxis)
{
	//	ビュー変換後の座標 -> 元の座標に変換する
	//	(タッチ位置再現のため)
	//

	if (bm->vp_flag == BMSCR_VPFLAG_NOUSE) return;
	if (bm->vp_flag == BMSCR_VPFLAG_3D) return;			// 3Dの変換には未対応

#if 0
	Vector4 v1;
	v1.x = (float)*xaxis;
	v1.y = (float)(nDestHeight - *yaxis);
	v1.z = 1.0f;
	v1.w = 0.0f;

	v1.x -= _center_sx;
	v1.y -= _center_sy;
	v1.x *= _rate_sx;
	v1.y *= _rate_sy;

	game->convert2DRenderProjection(v1);
	*xaxis = (int)v1.x;
	*yaxis = (int)v1.y;
#endif

#if 1
	VECTOR v1, v2;
	v1.x = (float)*xaxis;
	v1.y = (float)(nDestHeight - *yaxis);
	v1.z = 1.0f;
	v1.w = 0.0f;

	v1.x -= _center_sx;
	v1.y -= _center_sy;
	v1.x *= _rate_sx;
	v1.y *= _rate_sy;

	ApplyMatrix(&mat_unproj, &v2, &v1);
	*xaxis = (int)v2.x;
	*yaxis = (int)v2.y;
#endif
}



