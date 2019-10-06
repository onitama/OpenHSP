
//
//	HSP3dish window manager
//	onion software/onitama 2011/4
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/dpmread.h"
#include "../hsp3/strbuf.h"
#include "../hsp3/strnote.h"

#include "hgio.h"
#include "supio.h"
#include "hspwnd.h"

HspWnd *curwnd;

#ifdef HSPWIN
#include <windows.h>
HWND hgio_gethwnd( void );
#endif

#define COLORRATE ( 1.0f / 255.0f )

/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

HspWnd::HspWnd()
{
	//		初期化
	//
	Reset();
}

HspWnd::~HspWnd()
{
	//		すべて破棄
	//
	Dispose();
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void HspWnd::Dispose( void )
{
	//		破棄
	//
	int i;
	Bmscr *bm;
	for(i=0;i<bmscr_max;i++) {
		bm = mem_bm[i];
		if ( bm != NULL ) {
			delete bm;
		}
	}
	free( mem_bm );
}


int HspWnd::GetActive( void )
{
	//
	//		detect active window
#ifdef HSPWIN
	HWND hwnd;
	hwnd = hgio_gethwnd();
	if ( GetActiveWindow() != hwnd ) return -1;
#endif

	return 0;
}


void HspWnd::ExpandScreen( int id )
{
	int i;
	int idmax;
	Bmscr **new_bm;

	//Alertf("Expand:%d:%d",idmax,bmscr_max);
	idmax = id + 1;
	if ( idmax <= bmscr_max ) return;
	new_bm = (Bmscr **)malloc( sizeof( Bmscr * ) * idmax );

	for(i=0;i<idmax;i++) {
		if (( i >= bmscr_max )||( bmscr_max == 0 )) {
			new_bm[i] = NULL;
		} else {
			//if ( mem_bm[i] != NULL ) 
			new_bm[i] = mem_bm[i];
		}
	}
	if ( mem_bm != NULL ) free( mem_bm );

	bmscr_max = idmax;
	mem_bm = new_bm;
}


void HspWnd::Reset( void )
{
	//		all window initalize
	//
	int sx,sy;

	//		alloc Bmscr
	//
	bmscr_max = 0;
	mem_bm = NULL;
	ExpandScreen( 16 );									// とりあえず

	sx = hgio_getWidth();
	sy = hgio_getHeight();

	MakeBmscr( 0, HSPWND_TYPE_MAIN, 0, 0, sx, sy, 0 );
	
	//		global vals
	//
#if 0
	wfy=GetSystemMetrics( SM_CYCAPTION )+GetSystemMetrics( SM_CYFRAME )*2;
	wfx=GetSystemMetrics( SM_CXFRAME )*2;
	wbx=GetSystemMetrics( SM_CXHTHUMB );
	wby=GetSystemMetrics( SM_CYVTHUMB );
	mwfy=GetSystemMetrics( SM_CYCAPTION )+GetSystemMetrics( SM_CYFIXEDFRAME )*2;
	mwfx=GetSystemMetrics( SM_CXFIXEDFRAME )*2;
#endif

	curwnd = this;

	//	Reset DevInfo
	memset( &devinfo, 0, sizeof(HSP3DEVINFO) );
}


void HspWnd::MakeBmscr( int id, int type, int x, int y, int sx, int sy, int option )
{
	//		Bmscr(オフスクリーン)生成
	//
	ExpandScreen( id );

	if ( mem_bm[ id ] != NULL ) {
		delete mem_bm[ id ];
	}
	Bmscr * bm = new Bmscr;
	mem_bm[ id ] = bm;

	bm->wid = id;
	bm->type = type;
	bm->texid = -1;
	bm->Init( sx, sy );
	bm->master_hspwnd = static_cast< void * >( this );
	bm->buffer_option = option;

	if (type == HSPWND_TYPE_OFFSCREEN) {
		sprintf( bm->resname, "buffer%d", bm->wid );
		hgio_buffer( (BMSCR *)bm );
	}
}


void HspWnd::MakeBmscrFromResource( int id, char *fname )
{
	//		Bmscr(リソース)生成
	//
	ExpandScreen( id );

	if ( mem_bm[ id ] != NULL ) {
		delete mem_bm[ id ];
	}
	Bmscr * bm = new Bmscr;
	mem_bm[ id ] = bm;

	bm->wid = id;
	bm->type = HSPWND_TYPE_BUFFER;
	bm->texid = -1;
	bm->Init( fname );
	bm->master_hspwnd = static_cast< void * >( this );
}


int HspWnd::Picload( int id, char *fname, int mode )
{
	//		picload
	//		( mode:0=resize/1=overwrite )
	//
	Bmscr *bm;

	bm = GetBmscr( id );
	if ( bm == NULL ) return 1;
	if ( bm->flag == BMSCR_FLAG_NOUSE ) return 1;
	switch( bm->type ) {
	case HSPWND_TYPE_MAIN:
		break;
	case HSPWND_TYPE_BUFFER:
		MakeBmscrFromResource( id, fname );
		break;
	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return 0;
}


Bmscr *HspWnd::GetBmscrSafe( int id )
{
	//		安全なbmscr取得
	//
	Bmscr *bm;
	if (( id < 0 )||( id >= bmscr_max )) throw HSPERR_ILLEGAL_FUNCTION;
	bm = GetBmscr( id );
	if ( bm == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
	if ( bm->flag == BMSCR_FLAG_NOUSE ) throw HSPERR_ILLEGAL_FUNCTION;
	return bm;
}


int HspWnd::GetEmptyBufferId( void )
{
	//		空きIDを取得
	//
	int i;
	Bmscr *bm;
	for(i=0;i<bmscr_max;i++) {
		bm = GetBmscr(i);
		if ( bm == NULL ) return i;
		if ( bm->flag == BMSCR_FLAG_NOUSE ) return i;
	}
	return bmscr_max;
}


void HspWnd::Resume( void )
{
	//		画面の再構築(フレームバッファ破棄時用)
	//
	int i;
	Bmscr *bm;

	bm = GetBmscr(0);
	hgio_screen( (BMSCR *)bm );
	hgio_resume();

	bm->tapstat = 0;
	bm->tapinvalid = 0;
	bm->cur_obj = NULL;

	//		リソースを読み込み直す
	//
	for(i=1;i<bmscr_max;i++) {
		bm = GetBmscr(i);
		if ( bm != NULL ) {
			if ( bm->type == HSPWND_TYPE_BUFFER ) {
				bm->flag = BMSCR_FLAG_NOUSE;
				hgio_texload( (BMSCR *)bm, bm->resname );
				bm->flag = BMSCR_FLAG_INUSE;
			}
		}
	}

}


/*------------------------------------------------------------*/
/*
		Bmscr interface
*/
/*------------------------------------------------------------*/

Bmscr::Bmscr()
{
	//		bmscr初期化
	//
	flag = BMSCR_FLAG_NOUSE;
}

Bmscr::~Bmscr()
{
	//		Bmscr破棄
	//
	if ( flag == BMSCR_FLAG_INUSE ) {
		ResetHSPObject();					//		object remove
		hgio_delscreen( (BMSCR *)this );
	}
}

/*----------------------------------------------------------------*/
//		font&text related routines
/*----------------------------------------------------------------*/

void Bmscr::Init( int p_sx, int p_sy )
{
	//		bitmap buffer make
	//
	flag = BMSCR_FLAG_INUSE;

	objmax = 0;
	mem_obj = NULL;
	sx = p_sx; sy = p_sy;
	sx2 = sx;
	master_buffer = NULL;
	buffer_option = 0;

	Cls(0);

	imgbtn = -1;

	objmode = 1;
	fl_dispw = 0;

	fl_dispw = 1;
	fl_udraw = 1;

	resname[0] = 0;
}


void Bmscr::Init( char *fname )
{
	int i;
	i = hgio_texload( (BMSCR *)this, fname );
	if ( i < 0 ) {
		throw HSPERR_PICTURE_MISSING;
	}
	Init( sx, sy );
	strncpy( resname, fname, RESNAME_MAX-1 );
	//Alertf( "(%d,%d)",sx,sy );
}


void Bmscr::Cls( int mode )
{
	//		screen setting reset
	//
	int i;

	//		Font initalize
	//
	SetFont( "", 18, 0 );
	//Sysfont(0);

	//		object initalize
	//
	ResetHSPObject();
	tapstat = 0;
	tapinvalid = 0;
	cur_obj = NULL;

	//		text setting initalize
	//
	cx=0;cy=0;
	Setcolor(0,0,0);
	SetMulcolor(255,255,255);

	//		vals initalize
	//
	textspeed=0;
	ox=64;oy=32;py=0;
	gx=32;gy=32;gmode=0;
	objstyle = 00;
	for (i = 0; i<BMSCR_SAVEPOS_MAX; i++) {
		savepos[i] = 0;
		accel_value[i] = (HSPREAL)0.0;
	}

	//		CEL initalize
	//
	SetCelDivideSize( 0, 0, 0, 0 );

	//		all update
	//
	fl_udraw = fl_dispw;

	//		Update HGI/O
	//
	hgio_screen( (BMSCR *)this );

	//		Multi-Touch Reset
	resetMTouch();
}


void Bmscr::Title( char *str )
{
	hgio_title( str );
}


void Bmscr::Width( int x, int y, int wposx, int wposy, int mode )
{
}


void Bmscr::Posinc( int pp )
{
	if ( pp<py ) { cy+=py; } else { cy+=pp; }
}


void Bmscr::Setcolor( int a1, int a2, int a3 )
{
	color = 0xff000000|((a1&0xff)<<16)|((a2&0xff)<<8)|(a3&0xff);

#ifdef HSPDISHGP
	colorvalue[0] = ((float)a1) * COLORRATE;
	colorvalue[1] = ((float)a2) * COLORRATE;
	colorvalue[2] = ((float)a3) * COLORRATE;
	colorvalue[3] = 1.0f;
#endif

}


void Bmscr::Setcolor( int icolor )
{
	color = icolor;

#ifdef HSPDISHGP
	int a1 = ( icolor >> 16 ) & 0xff;
	int a2 = ( icolor >>  8 ) & 0xff;
	int a3 = ( icolor ) & 0xff;
	colorvalue[0] = ((float)a1) * COLORRATE;
	colorvalue[1] = ((float)a2) * COLORRATE;
	colorvalue[2] = ((float)a3) * COLORRATE;
	colorvalue[3] = 1.0f;
#endif

}


void Bmscr::SetMulcolor( int a1, int a2, int a3 )
{
	mulcolor = ((a1&0xff)<<16)|((a2&0xff)<<8)|(a3&0xff);


#ifdef HSPDISHGP
	mulcolorvalue[0] = ((float)a1) * COLORRATE;
	mulcolorvalue[1] = ((float)a2) * COLORRATE;
	mulcolorvalue[2] = ((float)a3) * COLORRATE;
	mulcolorvalue[3] = 1.0f;
#else
#if defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
	hgio_setColorTex( a1, a2, a3 );
#endif
#endif

}


void Bmscr::SetFont( char *fontname, int size, int style )
{
	strncpy( font_curname, fontname, RESNAME_MAX-1 );
	font_cursize = size;
	font_curstyle = style;

	hgio_font( fontname, size, style );
}


void Bmscr::SetDefaultFont( void )
{
	SetFont( font_curname, font_cursize, font_curstyle );
}


void Bmscr::SetFontInternal( char *fontname, int size, int style )
{
	//	内部用のフォント変更(カレントを残す)
	hgio_font( fontname, size, style );
}


void Bmscr::Print( char *mes )
{
	hgio_mes( (BMSCR *)this, mes );
	Posinc( printsizey );
}


void Bmscr::Boxfill( int x1,int y1,int x2,int y2 )
{
	//		boxf
	hgio_boxf( (BMSCR *)this, (float)x1, (float)y1, (float)x2, (float)y2 );
}


int Bmscr::Pget( int xx, int yy )
{
	//		pget
	//
	return 0;
}


void Bmscr::Pset( int xx,int yy )
{
	//		pset
	//
	hgio_line( (BMSCR *)this, (float)xx, (float)yy );
	hgio_line2( (float)xx, (float)yy );
	hgio_line( NULL, 0.0f, 0.0f );
}


void Bmscr::Line( int xx,int yy )
{
	//		line
	//
	hgio_line( (BMSCR *)this, (float)cx, (float)cy );
	hgio_line2( (float)xx, (float)yy );
	hgio_line( NULL, 0.0f, 0.0f );
	cx=xx;cy=yy;
#if 0
	HPEN oldpen;
	int x,y,x1,y1,x2,y2;
	x=cx;y=cy;
	MoveToEx( hdc,x,y,NULL );
	if (cx<xx)   { x1=x;x2=xx-x; }
				else { x1=xx;x2=x-xx; }
	if (cy<yy)   { y1=y;y2=yy-y; }
				else { y1=yy;y2=y-yy; }
	oldpen=(HPEN)SelectObject(hdc,hpn);
	LineTo( hdc,xx,yy );
	SelectObject(hdc,oldpen);
	Send( x1,y1,x2+1,y2+1 );
	cx=xx;cy=yy;
#endif
}


void Bmscr::Circle( int x1,int y1,int x2,int y2, int mode )
{
	//		circle
	//		mode: 0=outline/1=fill
	//
	hgio_circle( (BMSCR *)this, (float)x1, (float)y1, (float)x2, (float)y2, mode );
}


/*----------------------------------------------------------------*/

int Bmscr::Copy( Bmscr *src, int xx, int yy, int s_psx, int s_psy )
{
	//		copy
	//
	int texpx,texpy,psx,psy;
	psx = s_psx;
	psy = s_psy;
	texpx = xx + s_psx;
	texpy = yy + s_psy;
	if ( texpx < 0 ) return -1;
	if ( texpx >= src->sx ) {
		if ( xx >= src->sx ) return -1;
		psx = src->sx - xx;
	}
	if ( texpy < 0 ) return -1;
	if ( texpy >= src->sy ) {
		if ( yy >= src->sy ) return -1;
		psy = src->sy - yy;
	}
	hgio_copy( (BMSCR *)this, xx, yy, psx, psy, (BMSCR *)src, (float)psx, (float)psy );
	return 0;
}


int Bmscr::Zoom( int dx, int dy, Bmscr *src, int xx, int yy, int s_psx, int s_psy, int mode )
{
	//		zoom
	//		(mode:0=normal/1=halftone)
	//
	int texpx,texpy,psx,psy;
	psx = s_psx;
	psy = s_psy;
	texpx = xx + s_psx;
	texpy = yy + s_psy;
	if ( texpx < 0 ) return -1;
	if ( texpx >= src->sx ) {
		if ( xx >= src->sx ) return -1;
		psx = src->sx - xx;
	}
	if ( texpy < 0 ) return -1;
	if ( texpy >= src->sy ) {
		if ( yy >= src->sy ) return -1;
		psy = src->sy - yy;
	}
	hgio_copy( (BMSCR *)this, xx, yy, psx, psy, (BMSCR *)src, (float)dx, (float)dy );
	return 0;
}


int Bmscr::BmpSave( char *fname )
{
	//		save BMP,DIB file
	//
	return 0;
}


void Bmscr::SetHSVColor( int hval, int sval, int vval )
{
	//		hsvによる色指定
	//			h(0-191)/s(0-255)/v(0-255)
	//
	int h,s,v;
	int save_r, save_g, save_b;
	int t,i,v1,v2,v3;
	int mv=8160;		// 255*32
	int mp=4080;		// 255*16
	//		overflow check
	//
	v = vval & 255;
	s = sval & 255;		// /8
	//		hsv -> rgb 変換
	//
	h = hval % 192;
	i = h/32;
	t = h % 32;
	v1 = (v*(mv-s*32) 	+mp)/mv;
	v2 = (v*(mv-s*t) 	+mp)/mv;
	v3 = (v*(mv-s*(32-t))+mp)/mv;
	switch(i){
		case 0:
		case 6:
				save_r=v;	save_g=v3;	save_b=v1;	break;
		case 1:
				save_r=v2;	save_g=v;	save_b=v1;	break;
		case 2:
				save_r=v1;	save_g=v;	save_b=v3;	break;
		case 3:
				save_r=v1;	save_g=v2;	save_b=v;	break;
		case 4:
				save_r=v3;	save_g=v1;	save_b=v;	break;
		case 5:
				save_r=v;	save_g=v1;	save_b=v2;	break;
	}
	Setcolor( save_r, save_g, save_b );
}


void Bmscr::GetClientSize( int *xsize, int *ysize )
{
	//		ウィンドウのクライアント領域のサイズを求める
	//
#if 0
	RECT rw;
	GetClientRect( hwnd, &rw );
	*xsize = rw.right;
	*ysize = rw.bottom;
#endif
}


void Bmscr::GradFill( int _x, int _y, int _sx, int _sy, int mode, int col1, int col2 )
{
	//		グラデーション塗りつぶし
	//
	int posx[4];
	int posy[4];
	int vcol[4];

	posx[0] = _x;		posy[0] = _y;
	posx[1] = _x+_sx;	posy[1] = _y;
	posx[2] = _x+_sx;	posy[2] = _y+_sy;
	posx[3] = _x;		posy[3] = _y+_sy;

	if ( mode ) {
		vcol[0] = vcol[1] = col1;
		vcol[2] = vcol[3] = col2;
	} else {
		vcol[0] = vcol[3] = col1;
		vcol[1] = vcol[2] = col2;
	}
	SquareTex( posx, posy, NULL, vcol, NULL, -257 );
}


void Bmscr::FillRot( int x, int y, int dst_sx, int dst_sy, float ang )
{
	//		回転矩形塗りつぶし(grect用)
	//
	hgio_fillrot( (BMSCR *)this, (float)x, (float)y, (float)dst_sx, (float)dst_sy, ang );
}


void Bmscr::FillRotTex( int dst_sx, int dst_sy, float ang, Bmscr *src, int tx, int ty, int srcx, int srcy )
{
	//		回転矩形塗りつぶし(grotate用)
	//
	hgio_copyrot( (BMSCR *)this, tx, ty, srcx, srcy, (float)(dst_sx>>1), (float)(dst_sy>>1), (BMSCR *)src, (float)dst_sx, (float)dst_sy, ang );
}


void Bmscr::SetCelDivideSize( int new_divsx, int new_divsy, int new_ofsx, int new_ofsy )
{
	//		セル分割サイズを設定
	//
	if ( new_divsx > 0 ) divsx = new_divsx; else divsx = sx;
	if ( new_divsy > 0 ) divsy = new_divsy; else divsy = sy;
	divx = sx / divsx;
	divy = sy / divsy;
	celofsx = new_ofsx;
	celofsy = new_ofsy;
}


int Bmscr::CelPut( Bmscr *src, int id, float destx, float desty, float ang )
{
	//		セルをコピー(変倍・回転)
	//
	int xx,yy,texpx,texpy,psx,psy;
	int bak_cx, bak_cy;
	float dsx,dsy;

	psx = src->divsx;
	psy = src->divsy;
	xx = ( id % src->divx ) * psx;
	yy = ( id / src->divx ) * psy;
	texpx = xx + psx;
	texpy = yy + psy;
	if ( texpx < 0 ) return -1;
	if ( texpx >= src->sx ) {
		if ( xx >= src->sx ) return -1;
		psx = src->sx - xx;
	}
	if ( texpy < 0 ) return -1;
	if ( texpy >= src->sy ) {
		if ( yy >= src->sy ) return -1;
		psy = src->sy - yy;
	}

	dsx = (float)psx * destx;
	dsy = (float)psy * desty;

	bak_cx = cx + (int)dsx;
	bak_cy = cy;
	hgio_copyrot( (BMSCR *)this, xx, yy, psx, psy, (float)src->celofsx * destx, (float)src->celofsy * desty, (BMSCR *)src, dsx, dsy, ang );
	cx = bak_cx;
	cy = bak_cy;
	return 0;
}


int Bmscr::CelPut( Bmscr *src, int id )
{
	//		セルをコピー(固定サイズ)
	//
	int xx,yy,texpx,texpy,psx,psy;
	int bak_cx, bak_cy;

	psx = src->divsx;
	psy = src->divsy;
	xx = ( id % src->divx ) * psx;
	yy = ( id / src->divx ) * psy;
	texpx = xx + psx;
	texpy = yy + psy;
	if ( texpx < 0 ) return -1;
	if ( texpx >= src->sx ) {
		if ( xx >= src->sx ) return -1;
		psx = src->sx - xx;
	}
	if ( texpy < 0 ) return -1;
	if ( texpy >= src->sy ) {
		if ( yy >= src->sy ) return -1;
		psy = src->sy - yy;
	}

	bak_cx = cx + psx;
	bak_cy = cy;
	cx -= src->celofsx;
	cy -= src->celofsy;

	hgio_copy( (BMSCR *)this, xx, yy, psx, psy, (BMSCR *)src, (float)psx, (float)psy );
	cx = bak_cx;
	cy = bak_cy;
	return 0;
}


void Bmscr::SetFilter( int type )
{
	//		補間フィルタ設定
	//
	hgio_setfilter( type, 0 );
}


void Bmscr::SquareTex( int *dst_x, int *dst_y, Bmscr *src, int *src_x, int *src_y, int mode )
{
	//		頂点指定による四角形描画(gsquare用)
	//
	int coltmp[4];

	if ( mode < 0 ) {
		if ( mode == -257 ) {
			hgio_square( (BMSCR *)this, dst_x, dst_y, src_x );
		} else {
			coltmp[0] = coltmp[1] = coltmp[2] = coltmp[3] = color;
			hgio_square( (BMSCR *)this, dst_x, dst_y, coltmp );
		}
		return;
	}
	hgio_square_tex( (BMSCR *)this, dst_x, dst_y, (BMSCR *)src, src_x, src_y );
}

int Bmscr::BufferOp(int mode, char *ptr)
{
	//		テクスチャバッファの操作
	//
	return hgio_bufferop((BMSCR*)this, mode, ptr);
}

/*----------------------------------------------------------------*/

void Bmscr::setMTouch( HSP3MTOUCH *mt, int x, int y, bool touch )
{
	if ( mt == NULL ) return;
	if ( touch == false ) {
		mt->flag = 0;
		return;
	}
	mt->flag = 1;
	mt->x = x;
	mt->y = y;
}


void Bmscr::setMTouchByPoint( int old_x, int old_y, int x, int y, bool touch )
{
	HSP3MTOUCH *mt;
	mt = getMTouchByPoint( old_x, old_y );
	if ( mt == NULL ) {
		mt = getMTouchNew();
	}
	setMTouch( mt, x, y, touch );
}


void Bmscr::setMTouchByPointId( int pointid, int x, int y, bool touch )
{
	HSP3MTOUCH *mt;
	mt = getMTouchByPointId( pointid );
	if ( mt == NULL ) {
		mt = getMTouchNew();
		if ( mt == NULL ) return;
		mt->pointid = pointid;
	}
	setMTouch( mt, x, y, touch );
}


HSP3MTOUCH *Bmscr::getMTouch( int touchid )
{
	if ( touchid < 0 ) return NULL;
	if ( touchid >= BMSCR_MAX_MTOUCH ) return NULL;
	return &mtouch[touchid];
}


void Bmscr::resetMTouch( void )
{
	int i;
	HSP3MTOUCH *mt;
	mt = mtouch;
	mtouch_num = 0;
	for(i=0;i<BMSCR_MAX_MTOUCH;i++) {
		mt->flag = 0;
		mt->x = 0; mt->y = 0;
		mt->pointid = -2;
		mt++;
	}

	//	Test
	//addMTouch( 1, 123,456, true );
	//addMTouch( 5, 222,333, true );
}


HSP3MTOUCH *Bmscr::getMTouchByPointId( int pointid )
{
	int i;
	HSP3MTOUCH *mt;
	mt = mtouch;
	for(i=0;i<BMSCR_MAX_MTOUCH;i++) {
		if ( mt->flag ) {
			if ( mt->pointid == pointid ) {
				return mt;
			}
		}
		mt++;
	}
	return NULL;
}


HSP3MTOUCH *Bmscr::getMTouchByPoint( int x, int y )
{
	int i;
	HSP3MTOUCH *mt;
	mt = mtouch;
	for(i=0;i<BMSCR_MAX_MTOUCH;i++) {
		if ( mt->flag ) {
			if ( mt->y == y ) {
				if ( mt->x == x ) {
					return mt;
				}
			}
		}
		mt++;
	}
	return NULL;
}


HSP3MTOUCH *Bmscr::getMTouchNew( void )
{
	int i;
	HSP3MTOUCH *mt;
	mt = mtouch;
	for(i=0;i<BMSCR_MAX_MTOUCH;i++) {
		if ( mt->flag == 0 ) {
			return mt;
		}
		mt++;
	}
	return NULL;
}


int Bmscr::listMTouch( int *outbuf )
{
	//	タッチ情報のIDリストを返す
	//	outbufからint配列を書き出す、返値はIDの個数
	//
	int i;
	int *buf;
	HSP3MTOUCH *mt;
	buf = outbuf;
	mt = mtouch;
	mtouch_num = 0;
	for(i=0;i<BMSCR_MAX_MTOUCH;i++) {
		if ( mt->flag ) {
			*buf++ = i;
			mtouch_num++;
		}
		mt++;
	}
	return mtouch_num;
}

