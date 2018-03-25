//
//	hgiox.cpp header
//
#ifndef __hgiox_h
#define __hgiox_h

#ifdef __cplusplus
extern "C" {
#endif

#include "../hginc.h"
#include "hgtex.h"

//フリップ定数
#define GRAPHICS_FLIP_NONE       -1
#define GRAPHICS_FLIP_HORIZONTAL  0
#define GRAPHICS_FLIP_VERTICAL    2

struct BMSCR;

//	for public use

#if 0
void hgio_init( int mode, int sx, int sy, void *hwnd );
void hgio_term( void );

int hgio_render_start( void );
int hgio_render_end( void );
void hgio_screen( struct BMSCR *bm );
void hgio_delscreen( struct BMSCR *bm );
int hgio_redraw( struct BMSCR *bm, int flag );
int hgio_texload( struct BMSCR *bm, char *fname );

int hgio_dialog( int mode, char *str1, char *str2 );
int hgio_title( char *str1 );
int hgio_stick( int actsw );

int hgio_font( char *fontname, int size, int style );
int hgio_mes( struct BMSCR *bm, char *str1 );

void hgio_clsmode( int mode, int color, int tex );
int hgio_getWidth( void );
int hgio_getHeight( void );
void hgio_setfilter( int type, int opt );

void hgio_fillrot( struct BMSCR *bm, float x, float y, float sx, float sy, float ang );
void hgio_fcopy( float distx, float disty, short xx, short yy, short srcsx, short srcsy, int texid, int color );
void hgio_copy( struct BMSCR *bm, short xx, short yy, short srcsx, short srcsy, struct BMSCR *bmsrc, float psx, float psy );
void hgio_copyrot( struct BMSCR *bm, short xx, short yy, short srcsx, short srcsy, float ofsx, float ofsy, struct BMSCR *bmsrc, float psx, float psy, float ang );
#endif

bool hgio_getkey( int kcode );
void hgio_setColorTex( int rval, int gval ,int bval ); 

//	for internal use

void hgio_clear( void );
void hgio_reset( void );

void hgio_setClear( int rval, int gval ,int bval );
void hgio_setFilterMode( int mode );
void hgio_setTexBlendMode( int mode, int aval );
void hgio_putTexFont( int x, int y, char *msg, int color );

//色の指定
void hgio_setColor( int color );
//ライン幅の指定
void hgio_setLineWidth( int lineWidth );
//フリップモードの指定
void hgio_setFlipMode( int flipMode );
//原点の指定
void hgio_setOrigin( int x, int y );
//スケーリング
void hgio_scale_point( int xx, int yy, int &x, int & y );
//ポイントカラー設定
void hgio_panelcolor( int color, int aval );


//ポイント描画
void hgio_pset( float x, float y );
//矩形の描画
void hgio_rect( float x, float y, float w, float h );
//矩形の塗り潰し
void hgio_boxfill( float x, float y, float w, float h );
//円の描画
void hgio_circleLine( float x, float y, float rx, float ry );
//円の塗り潰し
void hgio_circleFill( float x, float y, float rx, float ry );

void hgio_touch( int xx, int yy, int button );
void hgio_mtouch( int old_x, int old_y, int xx, int yy, int button, int opt );
void hgio_mtouchid( int pointid, int xx, int yy, int button, int opt );

int hgio_getmousex( void );
int hgio_getmousey( void );
int hgio_getmousebtn( void );

void hgio_test(void);

int hgio_file_exist( char *fname );
int hgio_file_read( char *fname, void *ptr, int size, int offset );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
