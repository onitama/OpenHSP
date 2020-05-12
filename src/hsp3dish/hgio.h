
//
//	hgio(HSP graphics I/O) functions
//
#ifndef __hgio_h
#define __hgio_h

#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif
#include "../hsp3/dpmread.h"
#include "hspwnd_dish.h"
#include "geometry.h"

//		universal draw command function
//
void hgio_init( int mode, int sx, int sy, void *hwnd );
void hgio_term( void );
void hgio_resume( void );
int hgio_gsel( BMSCR *bm );
int hgio_buffer(BMSCR *bm);
int hgio_bufferop(BMSCR* bm, int mode, char *ptr);

void hgio_size( int sx, int sy );
void hgio_view( int sx, int sy );
void hgio_scale( float xx, float yy );
void hgio_autoscale( int mode );
void hgio_uvfix( int mode );

int hgio_render_start( void );
int hgio_render_end( void );
void hgio_screen( BMSCR *bm );
void hgio_delscreen( BMSCR *bm );
int hgio_redraw( BMSCR *bm, int flag );
int hgio_texload( BMSCR *bm, char *fname );
void hgio_setback( BMSCR *bm );

int hgio_dialog( int mode, char *str1, char *str2 );
int hgio_title( char *str1 );
int hgio_stick( int actsw );

int hgio_font( char *fontname, int size, int style );
int hgio_mes(BMSCR *bm, char *str1);
int hgio_mestex(BMSCR *bm, texmesPos *tpos);

void hgio_line( BMSCR *bm, float x, float y );
void hgio_line2( float x, float y );
void hgio_boxfAlpha(BMSCR *bm, float x1, float y1, float x2, float y2, int alphamode);
void hgio_boxf( BMSCR *bm, float x1, float y1, float x2, float y2 );
void hgio_circle( BMSCR *bm, float x1, float y1, float x2, float y2, int mode );
void hgio_copy( BMSCR *bm, short xx, short yy, short srcsx, short srcsy, BMSCR *bmsrc, float psx, float psy );
void hgio_copyrot( BMSCR *bm, short xx, short yy, short srcsx, short srcsy, float ofsx, float ofsy, BMSCR *bmsrc, float psx, float psy, float ang );
void hgio_fillrot( BMSCR *bm, float x, float y, float sx, float sy, float ang );

int hgio_celputmulti( BMSCR *bm, int *xpos, int *ypos, int *cel, int count, BMSCR *bmsrc );

void hgio_setcenter( float x, float y );

void hgio_clsmode( int mode, int color, int tex );
int hgio_getWidth( void );
int hgio_getHeight( void );
void hgio_setfilter( int type, int opt );

void hgio_square( BMSCR *bm, int *posx, int *posy, int *color );
void hgio_square_tex( BMSCR *bm, int *posx, int *posy, BMSCR *bmsrc, int *uvx, int *uvy );

int hgio_gettick( void );
int hgio_exec( char *msg, char *option, int mode );

void hgio_setinfo( int type, HSPREAL val );
HSPREAL hgio_getinfo( int type );
char *hgio_sysinfo( int p2, int *res, char *outbuf );
void hgio_setstorage( char *path );
char *hgio_getstorage( char *fname );
char *hgio_getdir( int id );
void hgio_setmainarg( char *hsp_mainpath, char *cmdprm );

// for Viewport setting
void hgio_setview(BMSCR* bm);
void hgio_cnvview(BMSCR* bm, int *xaxis, int *yaxis);

// for HGIMG4
void hgio_draw_gpsprite( Bmscr *bmscr, bool lateflag );
void hgio_draw_all(Bmscr *bmscr, int option);

// for texmes
void hgio_fontsystem_init(char* fontname, int size, int style);
void hgio_fontsystem_term(void);
void hgio_fontsystem_delete(int id);
int hgio_fontsystem_setup(int sx, int sy, void *buffer);
int hgio_fontsystem_exec(char* msg, unsigned char* buffer, int pitch, int* out_sx, int* out_sy, texmesPos *lineinfo=NULL);

// for edit component
void hgio_editputclip(BMSCR* bm, char *str);
char *hgio_editgetclip(BMSCR* bm);


enum {
CLSMODE_NONE = 0,
CLSMODE_SOLID,
CLSMODE_TEXTURE,
CLSMODE_BLUR,
CLSMODE_MAX,
};

#define HGIO_FILTER_TYPE_NONE 0
#define HGIO_FILTER_TYPE_LINEAR 1
#define HGIO_FILTER_TYPE_LINEAR2 2


#ifdef HSPWIN
#include "win32/hgiox.h"
#endif

#ifdef HSPIOS
#include "ios/hgiox.h"
#endif

#ifdef HSPNDK
#include "ndk/hgiox.h"
#endif

#ifdef HSPLINUX
#include "emscripten/hgiox.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "emscripten/hgiox.h"
#endif


#endif

