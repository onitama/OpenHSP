
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>
#include <string.h>
#include <math.h>

#include "../hpi3sample/hsp3plugin.h"

#include "fcpoly.h"

extern int resY0, resY1;		// fcpoly.cppで定義

/*------------------------------------------------------------*/
/*
		full-color mode routines
*/
/*------------------------------------------------------------*/

static	UCHAR	*vram;
static	UCHAR	*vram2;
static	int		gfsx,gfsy,gfvx,gfvy,gfvp,gfvp2,gfmode;

static int calcofs( int px, int py )
{
	return (gfvy-py-1)*gfvp+(px*3);
}

static int calcofs2( BMSCR *bm, int px, int py )
{
	int vx,vy;
	vx=bm->sx;vy=bm->sy;
	gfvp2=vx*3;						// Y方向の増分byte数
	return (vy-py-1)*gfvp2+(px*3);
}


EXPORT BOOL WINAPI gfini ( BMSCR *bm, int p1, int p2, int p3 )
{
	//		clear vram area (type2)
	//			gfini xsize,ysize
	//
	gfmode=p1;
	gfsx=p1;gfsy=p2;
	gfvx=bm->sx;gfvy=bm->sy;
	if (gfsx==0) gfsx=gfvx;
	if (gfsy==0) gfsy=gfvy;
	gfvp=gfvx*3;						// Y方向の増分byte数
	vram=(bm->pBit)+calcofs(bm->cx,bm->cy);
	return 0;
}


EXPORT BOOL WINAPI gfcopy( BMSCR *bm, int p1, int p2, int p3 )
{
	//		copy vram area with halftone (type2)
	//			gfcopy grade(0-100)
	//
	int x,y,n,grade,agrade;
	UCHAR *v;
	UCHAR *t;
	UCHAR *v2;
	UCHAR *t2;
	short a1,a2,a3;
	short a;
	v=(UCHAR *)vram;
	v2=(UCHAR *)bm->pBit;v2+=calcofs2(bm,bm->cx,bm->cy);
	n=p1*256/100;
	if (n<0) n=0; else if (n>256) n=256;
	grade=n;agrade=256-n;
	for(y=0;y<gfsy;y++) {
		t=v;v-=gfvp;
		t2=v2;v2-=gfvp2;
		for(x=0;x<gfsx;x++) {
			a1=(short)*t2++;a2=(short)*t2++;a3=(short)*t2++;
			if (a1|a2|a3) {
				a=(short)*t;a=(a*agrade>>8)+(a1*grade>>8);
				if (a>255) a=255;
				*t++=(UCHAR)a;
				a=(short)*t;a=(a*agrade>>8)+(a2*grade>>8);
				if (a>255) a=255;
				*t++=(UCHAR)a;
				a=(short)*t;a=(a*agrade>>8)+(a3*grade>>8);
				if (a>255) a=255;
				*t++=(UCHAR)a;
			} else {
				t+=3;			// when RGB=0 transparent
			}
		}
	}
	return 0;
}


EXPORT BOOL WINAPI gfdec ( int p1, int p2, int p3, int p4 )
{
	//		clear vram area (type0)
	//			gfdec r,g,b
	//
	int x,y;
	UCHAR *v;
	UCHAR *t;
	UCHAR a1,a2,a3;
	short a;
	a3=(UCHAR)p1; a2=(UCHAR)p2; a1=(UCHAR)p3;
	v=vram;
	for(y=0;y<gfsy;y++) {
		t=v;v-=gfvp;
		for(x=0;x<gfsx;x++) {
			a=(short)*t;a-=(short)a1;if (a<0) a=0;
			*t++=(UCHAR)a;
			a=(short)*t;a-=(short)a2;if (a<0) a=0;
			*t++=(UCHAR)a;
			a=(short)*t;a-=(short)a3;if (a<0) a=0;
			*t++=(UCHAR)a;
		}
	}
	return 0;
}


EXPORT BOOL WINAPI gfinc ( int p1, int p2, int p3, int p4 )
{
	//		clear vram area (type0)
	//			gfdec r,g,b
	//
	int x,y;
	UCHAR *v;
	UCHAR *t;
	UCHAR a1,a2,a3;
	short a;
	a3=(UCHAR)p1; a2=(UCHAR)p2; a1=(UCHAR)p3;
	v=vram;
	for(y=0;y<gfsy;y++) {
		t=v;v-=gfvp;
		for(x=0;x<gfsx;x++) {
			a=(short)*t;a+=(short)a1;if (a>255) a=255;
			*t++=(UCHAR)a;
			a=(short)*t;a+=(short)a2;if (a>255) a=255;
			*t++=(UCHAR)a;
			a=(short)*t;a+=(short)a3;if (a>255) a=255;
			*t++=(UCHAR)a;
		}
	}
	return 0;
}


/*------------------------------------------------------------*/
/*
		for ver2.4g new feature
*/
/*------------------------------------------------------------*/

static void bms_send( BMSCR *bm, int x, int y, int sx, int sy )
{
	HDC hdc;
	if ( bm->fl_udraw == 0 ) return;
	if ( bm->fl_dispw == 0 ) return;
	hdc=GetDC( bm->hwnd );
	BitBlt( hdc, x-bm->viewx, y-bm->viewy, sx, sy, bm->hdc,x,y, SRCCOPY );
	ReleaseDC( bm->hwnd,hdc );
}

static void bms_setcol( BMSCR *bm, int a1, int a2, int a3 )
{
	if (a1==-1) bm->color=PALETTEINDEX(a2);
		   else bm->color=RGB(a1,a2,a3);
	SetBkMode( bm->hdc,TRANSPARENT );
	SetTextColor( bm->hdc, bm->color );
	if ( bm->hbr != NULL ) DeleteObject( bm->hbr );
	bm->hbr = CreateSolidBrush( bm->color );
	if ( bm->hpn != NULL ) DeleteObject( bm->hpn );
	bm->hpn = CreatePen( PS_SOLID,0,bm->color );
}

EXPORT BOOL WINAPI hsvcolor( BMSCR *bm, int h, int s, int v )
{
	//
	//		hsvによる色指定
	//			h(0-191)/s(0-255)/v(0-255)
	//
	//
	int save_r, save_g, save_b;
	int t,i,v1,v2,v3;
	int mv=8160;		// 255*32
	int mp=4080;		// 255*16

	//		overflow check
	//
	v = v&255;
	s = s&255;		// /8

	//		hsv -> rgb 変換
	//
	h %= 192;
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
	bms_setcol( bm, save_r, save_g, save_b );
	return 0;
}


/*------------------------------------------------------------*/
/*
		for ver2.61 new feature
*/
/*------------------------------------------------------------*/

static POLY4 mem_poly4;

static void SetPolyAxis( POLY4 *poly, int num, int x, int y, int tx, int ty )
{
	IAXIS2 *iv;
	iv = &poly->v[num];
	iv->x = x;
	iv->y = y;
	iv->tx = tx;
	iv->ty = ty;
}


static void DrawSquareEx( int mode, int color, int attr, int alpha, int *x, int *y, int *texx, int *texy )
{
	//		自由変形スプライト
	//		mode : 0=fullcolor/1=palette
	//
	POLY4 *poly;
	int i,xx,yy,tx,ty;

	poly = (POLY4 *)&mem_poly4;
	for(i=0;i<4;i++) {
		xx = x[i]; yy = y[i];
		tx = texx[i];
		ty = texy[i];
		SetPolyAxis( poly, i, xx, yy, tx, ty );
	}
	poly->tex = 0;
	poly->attr = attr;
	poly->npoly = 1;
	poly->alpha = alpha;
	if ( color == -1 ) {
		poly->color = 0xffffff;
		if ( mode ) {
			DrawPolygonTexP( poly );
		} else {
			DrawPolygonTex( poly );
		}
	} else {
		poly->color = color;
		if ( mode ) {
			DrawPolygonF4P( poly );
		} else {
			DrawPolygonF4( poly );
		}
	}
}


static void DrawSpriteEx( int mode, int attr, int alpha, int x, int y, int sx, int sy, float ang, int tx0, int ty0, int tx1, int ty1 )
{
	//		拡大回転スプライト
	//		mode : 0=fullcolor/1=palette
	//		(x,y)中心座標 / (sx,sy)サイズ / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	float x0,y0,x1,y1,ofsx,ofsy;
	short xx,yy;

	poly = (POLY4 *)&mem_poly4;
	xx = (short)x;
	yy = (short)y;

	ofsx = (float)sx;
	ofsy = (float)sy;
	x0=-(float)sin( ang );
	y0=(float)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

	iv = &poly->v[0];
	iv->x = (short)(-x0+x1) + xx;
	iv->y = (short)(-y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty0;

	iv = &poly->v[1];
	iv->x = (short)(x0+x1) + xx;
	iv->y = (short)(y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty1;

	iv = &poly->v[2];
	iv->x = (short)(x0-x1) + xx;
	iv->y = (short)(y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty1;

	iv = &poly->v[3];
	iv->x = (short)(-x0-x1) + xx;
	iv->y = (short)(-y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty0;

	poly->tex = 0;
	poly->attr = attr;
	poly->npoly = 1;
	poly->color = 0xffffff;
	poly->alpha = alpha;

	if ( mode ) {
		DrawPolygonTexP( poly );
	} else {
		DrawPolygonTex( poly );
	}
}


static void DrawRectEx( int mode, int color, int alpha, int x, int y, int sx, int sy, float ang )
{
	//		回転fill
	//		mode : 0=fullcolor/1=palette
	//		(x,y)中心座標 / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	float x0,y0,x1,y1,ofsx,ofsy;
	short xx,yy,tx0,ty0,tx1,ty1;

	poly = (POLY4 *)&mem_poly4;
	xx = (short)x;
	yy = (short)y;

	tx0 = 0;
	ty0 = 0;
	tx1 = sx-1; if ( tx1 < 0 ) tx1=0;
	ty1 = sy-1; if ( ty1 < 0 ) ty1=0;
	ofsx = (float)( tx1 - tx0 + 1 );
	ofsy = (float)( ty1 - ty0 + 1 );
	x0=-(float)sin( ang );
	y0=(float)cos( ang );
	x1 = -y0;
	y1 = x0;

	ofsx *= -0.5f;
	ofsy *= -0.5f;
	x0 *= ofsy;
	y0 *= ofsy;
	x1 *= ofsx;
	y1 *= ofsx;

	iv = &poly->v[0];
	iv->x = (short)(-x0+x1) + xx;
	iv->y = (short)(-y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty1;

	iv = &poly->v[1];
	iv->x = (short)(x0+x1) + xx;
	iv->y = (short)(y0+y1) + yy;
	iv->tx = tx1;
	iv->ty = ty0;

	iv = &poly->v[2];
	iv->x = (short)(x0-x1) + xx;
	iv->y = (short)(y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty0;

	iv = &poly->v[3];
	iv->x = (short)(-x0-x1) + xx;
	iv->y = (short)(-y0-y1) + yy;
	iv->tx = tx0;
	iv->ty = ty1;

	poly->tex = 0;
	poly->attr = 0;
	poly->npoly = 1;
	poly->alpha = alpha;
	poly->color = color;

	if ( mode ) {
		DrawPolygonF4P( poly );
	} else {
		DrawPolygonF4( poly );
	}
}


static int GetAlphaOperation( BMSCR *bm )
{
	//		gmodeのモードをAlphaOperationに変換する
	//
	int alpha;
	alpha = bm->gfrate;
	if ( alpha < 0 ) alpha = 0;
	if ( alpha > 255 ) {
		alpha = 256;
		if ( bm->gmode >= 4 ) alpha = 255;
	}
	switch( bm->gmode ) {
	case 3:					// 半透明blend
		break;
	case 4:					// 半透明blend+透明色
		break;
	case 5:					// 色加算
		alpha |= 0x200;
		break;
	case 6:					// 色減算
		alpha |= 0x300;
		break;
	default:
		alpha = 0x100;		// 標準
		break;
	}
	return alpha;
}

static int GetLimit( int num, int min, int max )
{
	if ( num > max ) return max;
	if ( num < min ) return min;
	return num;
}


static int CnvRGB( int color )
{
	int res=0;
	res = color & 0x00ff00;
	res |= (color>>16) & 0xff;
	res |= (color & 0xff)<<16;
	return res;
}


EXPORT BOOL WINAPI grect( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//		grect (type$202)
	//
	//			grect cx, cy, rot, sx, sy, palcolor
	//
	BMSCR *bm;
	int ep1,ep2,ep3,ep4,ep5,ep6;
	float rot;
	bm = (BMSCR *)hei->HspFunc_getbmscr( *hei->actscr );	// 現在のBMSCRを取得

	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(0);			// パラメータ2:数値
	ep5 = hei->HspFunc_prm_getdi(0);			// パラメータ5:数値
	ep3 = hei->HspFunc_prm_getdi(bm->gx);		// パラメータ3:数値
	ep4 = hei->HspFunc_prm_getdi(bm->gy);		// パラメータ4:数値
	ep6 = hei->HspFunc_prm_getdi(0);			// パラメータ6:数値
	if ( *hei->er ) return *hei->er;			// エラーチェック
	rot = ((float)ep5) * (PI2/4096.0f);

	SetPolyDest( bm->pBit, bm->sx, bm->sy );
	if ( bm->palmode == 0 ) ep6 = CnvRGB( bm->color );
	DrawRectEx( bm->palmode, ep6, GetAlphaOperation(bm), ep1, ep2, ep3, ep4, rot );
	if ( resY0 >= 0 ) {
		bms_send( bm, 0, resY0, bm->sx, resY1-resY0+1 );
	}
	return 0;
}


EXPORT BOOL WINAPI grotate( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//		grotate (type$202)
	//
	//			grotate id, bx, by, rot, sx, sy
	//
	BMSCR *bm;
	BMSCR *bm2;
	int ep1,ep2,ep3,ep4,ep5,ep6;
	int tx0,ty0,tx1,ty1;
	int attr;
	float rot;
	bm = (BMSCR *)hei->HspFunc_getbmscr( *hei->actscr );	// 現在のBMSCRを取得

	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi(0);			// パラメータ2:数値
	ep3 = hei->HspFunc_prm_getdi(0);			// パラメータ3:数値
	ep6 = hei->HspFunc_prm_getdi(0);			// パラメータ6:数値
	ep4 = hei->HspFunc_prm_getdi(bm->gx);		// パラメータ4:数値
	ep5 = hei->HspFunc_prm_getdi(bm->gy);		// パラメータ5:数値
	if ( *hei->er ) return *hei->er;			// エラーチェック

	if (( ep1<0 )||( ep1>31 )) return 3;			// バッファIDチェック
	bm2 = (BMSCR *)hei->HspFunc_getbmscr( ep1 );	// 転送元のBMSCRを取得
	rot = ((float)ep6) * (PI2/4096.0f);

	if ( bm->palmode != bm2->palmode ) return 21;
	SetPolyDest( bm->pBit, bm->sx, bm->sy );
	SetPolySource( bm2->pBit, bm2->sx, bm2->sy );

	tx0 = GetLimit( ep2, 0, bm2->sx );
	ty0 = GetLimit( ep3, 0, bm2->sy );
	tx1 = GetLimit( tx0+bm->gx-1, 0, bm2->sx );
	ty1 = GetLimit( ty0+bm->gy-1, 0, bm2->sy );
	ty0 = bm2->sy - 1 - ty0;
	ty1 = bm2->sy - 1 - ty1;
	attr = 0;
	if ( bm->gmode == 2 ) { attr = NODE_ATTR_COLKEY; }
	if ( bm->gmode == 4 ) { attr = NODE_ATTR_COLKEY; SetPolyColorKey( bm->color ); }

	DrawSpriteEx( bm->palmode, attr, GetAlphaOperation(bm), bm->cx, bm->cy, ep4, ep5, rot, tx0, ty1, tx1, ty0 );
	if ( resY0 >= 0 ) {
		bms_send( bm, 0, resY0, bm->sx, resY1-resY0+1 );
	}
	return 0;
}


EXPORT BOOL WINAPI gsquare( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//		gsquare (type$202)
	//
	//			gsquare id, x, y, tx, ty
	//
	BMSCR *bm;
	BMSCR *bm2;
	int ep1,i;
	int attr;
	int *px;
	int *py;
	int *ptx;
	int *pty;
	int tmp_x[4], tmp_y[4], tmp_tx[4], tmp_ty[4];
	int color;
	bm = (BMSCR *)hei->HspFunc_getbmscr( *hei->actscr );	// 現在のBMSCRを取得
	bm2 = NULL;
	ptx = NULL;
	pty = NULL;

	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値

	px = (int *)hei->HspFunc_prm_getv();		// パラメータ2:変数
	py = (int *)hei->HspFunc_prm_getv();		// パラメータ3:変数

	if ( ep1 >= 0 ) {
		ptx = (int *)hei->HspFunc_prm_getv();		// パラメータ4:変数
		pty = (int *)hei->HspFunc_prm_getv();		// パラメータ5:変数
		if ( ep1>31 ) return 3;						// バッファIDチェック
		bm2 = (BMSCR *)hei->HspFunc_getbmscr( ep1 );	// 転送元のBMSCRを取得
		if ( bm->palmode != bm2->palmode ) return 21;
		SetPolySource( bm2->pBit, bm2->sx, bm2->sy );
	}
	if ( *hei->er ) return *hei->er;			// エラーチェック

	for(i=0;i<4;i++) {
		tmp_x[i] = px[i];
		tmp_y[i] = py[i];
		if ( ep1 >= 0 ) {
			tmp_tx[i] = GetLimit( ptx[i], 0, bm2->sx );
			tmp_ty[i] = bm2->sy - 1 - GetLimit( pty[i], 0, bm2->sy );	// UVの上下逆にする
		} else {
			tmp_tx[i] = 0;
			tmp_ty[i] = 0;
		}
	}

	attr = 0;
	if ( bm->gmode == 2 ) { attr = NODE_ATTR_COLKEY; }
	if ( bm->gmode == 4 ) { attr = NODE_ATTR_COLKEY; SetPolyColorKey( bm->color ); }

	SetPolyDest( bm->pBit, bm->sx, bm->sy );
	if ( ep1 >= 0 ) {
		color = -1;
	} else {
		color = CnvRGB( bm->color );
		if ( bm->palmode ) color = (-ep1)-1;
	}
	DrawSquareEx( bm->palmode, color, attr, GetAlphaOperation(bm), tmp_x, tmp_y, tmp_tx, tmp_ty );
	if ( resY0 >= 0 ) {
		bms_send( bm, 0, resY0, bm->sx, resY1-resY0+1 );
	}
	return 0;
}




