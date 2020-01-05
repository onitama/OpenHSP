//
//	hgiox.cpp header
//
#ifndef __hgiox_h
#define __hgiox_h

#include "hgtex.h"

//フリップ定数
#define GRAPHICS_FLIP_NONE       -1
#define GRAPHICS_FLIP_HORIZONTAL  0
#define GRAPHICS_FLIP_VERTICAL    2

struct BMSCR;

//	for public use

bool hgio_getkey( int kcode );

//	for internal use

void hgio_clear( void );
void hgio_reset( void );

void hgio_setClear( int rval, int gval ,int bval );
void hgio_setFilterMode( int mode );

//ライン幅の指定
void hgio_setLineWidth( int lineWidth );
//フリップモードの指定
void hgio_setFlipMode( int flipMode );
//原点の指定
void hgio_setOrigin( int x, int y );
//スケーリング
void hgio_scale_point( int xx, int yy, int &x, int & y );


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

#endif
