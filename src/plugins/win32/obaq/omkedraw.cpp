
//
//		OMKE Draw Support
//		onion software/onitama 2009/6
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "omkedraw.h"
#include "fcpoly.h"
#include "supio.h"

/*----------------------------------------------------------------*/

static	HPEN oldpen;

void OmkeDrawColor( BMSCR *bm, int color )
{
	COLORREF c;
	char *cref = (char *)&c;
	//c = (COLORREF)(color & 0xffffff);
	cref[0] = (color>>16) & 0xff;
	cref[1] = (color>>8) & 0xff;
	cref[2] = color & 0xff;
	cref[3] = 0;
	SetBkMode( bm->hdc,TRANSPARENT );
	SetTextColor( bm->hdc, c );
	if ( bm->hbr != NULL ) DeleteObject( bm->hbr );
	bm->hbr = CreateSolidBrush( c );
	if ( bm->hpn != NULL ) DeleteObject( bm->hpn );
	bm->hpn = CreatePen( PS_SOLID,0,c );
}


void OmkeDrawLineStart( BMSCR *bm, int col )
{
	OmkeDrawColor( bm, col );
	oldpen=(HPEN) SelectObject( bm->hdc, bm->hpn );
}


void OmkeDrawLineEnd( BMSCR *bm )
{
	SelectObject( bm->hdc, oldpen );
}


void OmkeDrawLine( BMSCR *bm, int cx, int cy, int xx,int yy )
{
	//		line
	//
	MoveToEx( bm->hdc,cx,cy,NULL );
	LineTo( bm->hdc,xx,yy );
}

void OmkeDrawCircle( BMSCR *bm, int x,int y,int range, int col )
{
	//		circle
	//
	HPEN oldpen;
	HBRUSH old;
	int x1,y1,x2,y2;

	x1 = x - range;
	y1 = y - range;
	x2 = x + range;
	y2 = y + range;

	OmkeDrawColor( bm, col );
	oldpen = (HPEN)SelectObject(bm->hdc,bm->hpn);
	old = (HBRUSH)SelectObject( bm->hdc, bm->hbr );
	::Ellipse( bm->hdc,x1,y1,x2,y2 );
	SelectObject( bm->hdc, old );
	SelectObject( bm->hdc, oldpen );
}


/*------------------------------------------------------------*/
/*
		for polygon process interface
*/
/*------------------------------------------------------------*/

static POLY4 mem_poly4;
static int mem_gmode;
static int mem_grate;
static int mem_ofsx;
static int mem_ofsy;

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


static void DrawSpriteEx( int mode, int attr, int alpha, int x, int y, int sx, int sy, double ang, int tx0, int ty0, int tx1, int ty1, int opx, int opy )
{
	//		拡大回転スプライト
	//		mode : 0=fullcolor/1=palette
	//		(x,y)基点座標 / (sx,sy)サイズ / (opx,opy)オフセット / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	double xx,yy,x0,y0,x1,y1,ofsx,ofsy,mx0,mx1,my0,my1;

	poly = (POLY4 *)&mem_poly4;

	mx0=-(double)sin( ang );
	my0=(double)cos( ang );
	mx1 = -my0;
	my1 = mx0;

	ofsx = (double)-opx;
	ofsy = (double)-opy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	//		基点の算出
	xx = ( (double)x - (-x0+x1) );
	yy = ( (double)y - (-y0+y1) );

	/*-------------------------------*/

	//		回転座標の算出
	ofsx = (double)-sx;
	ofsy = (double)-sy;
	x0 = mx0 * ofsy;
	y0 = my0 * ofsy;
	x1 = mx1 * ofsx;
	y1 = my1 * ofsx;

	/*-------------------------------*/

	iv = &poly->v[0];
	iv->x = (short)((-x0+x1) + xx);
	iv->y = (short)((-y0+y1) + yy);
	iv->tx = tx1;
	iv->ty = ty0;

	/*-------------------------------*/

	iv = &poly->v[1];
	iv->x = (short)((x1) + xx);
	iv->y = (short)((y1) + yy);
	iv->tx = tx1;
	iv->ty = ty1;

	/*-------------------------------*/

	iv = &poly->v[2];
	iv->x = (short)(xx);
	iv->y = (short)(yy);
	iv->tx = tx0;
	iv->ty = ty1;

	/*-------------------------------*/

	iv = &poly->v[3];
	iv->x = (short)((-x0) + xx);
	iv->y = (short)((-y0) + yy);
	iv->tx = tx0;
	iv->ty = ty0;

	/*-------------------------------*/

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


static void DrawRectEx( int mode, int color, int alpha, int x, int y, int sx, int sy, double ang )
{
	//		回転fill
	//		mode : 0=fullcolor/1=palette
	//		(x,y)中心座標 / ang=角度
	//
	POLY4 *poly;
	IAXIS2 *iv;
	double x0,y0,x1,y1,ofsx,ofsy;
	short xx,yy,tx0,ty0,tx1,ty1;

	poly = (POLY4 *)&mem_poly4;
	xx = (short)x;
	yy = (short)y;

	tx0 = 0;
	ty0 = 0;
	tx1 = sx-1; if ( tx1 < 0 ) tx1=0;
	ty1 = sy-1; if ( ty1 < 0 ) ty1=0;
	ofsx = (double)( tx1 - tx0 + 1 );
	ofsy = (double)( ty1 - ty0 + 1 );
	x0=-(double)sin( ang );
	y0=(double)cos( ang );
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


static int CnvRGB( int color )
{
	int res=0;
	res = color & 0x00ff00;
	res |= (color>>16) & 0xff;
	res |= (color & 0xff)<<16;
	return res;
}


static int GetAttrOperation( void )
{
	//		gmodeのモードをHGIMG互換のattr値に変換する
	//
	int attr;
	attr = 0;
	if ( mem_gmode == 2 ) { attr = NODE_ATTR_COLKEY; }
	//if ( mem_gmode == 4 ) { attr = NODE_ATTR_COLKEY; SetPolyColorKey( bmscr->color ); }
	return attr;
}

static int GetAlphaOperation( void )
{
	//		gmodeのモードをHGIMG互換のAlphaOperationに変換する
	//
	int alpha;
	alpha = mem_grate;
	if ( alpha < 0 ) alpha = 0;
	if ( alpha > 255 ) {
		alpha = 256;
		if ( mem_gmode >= 4 ) alpha = 255;
	}
	switch( mem_gmode ) {
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


static void GRotateSub( BMSCR *bmscr, BMSCR *bm2, int putx, int puty, int x, int y, int sx, int sy, int sizex, int sizey, double rot, int opx, int opy )
{
	int tx0,ty0,tx1,ty1;
	int attr;

	SetPolyDest( bmscr->pBit, bmscr->sx, bmscr->sy );
	SetPolySource( bm2->pBit, bm2->sx, bm2->sy );

	tx0 = GetLimit( x, 0, bm2->sx );
	ty0 = GetLimit( y, 0, bm2->sy );
	tx1 = GetLimit( tx0 + sx - 1, 0, bm2->sx );
	ty1 = GetLimit( ty0 + sy - 1, 0, bm2->sy );
	ty0 = bm2->sy - 1 - ty0;
	ty1 = bm2->sy - 1 - ty1;
	attr = GetAttrOperation();
	DrawSpriteEx( bmscr->palmode, attr, GetAlphaOperation(), putx, puty, sizex, sizey, rot, tx0, ty1, tx1, ty0, opx, opy );
}


void OmkeDrawSetGMode( int gmode, int grate, int ofsx, int ofsy )
{
	//		set gmode
	//
	mem_gmode = gmode;
	mem_grate = grate;
	mem_ofsx = ofsx;
	mem_ofsy = ofsy;
}


void OmkeDrawSprite( BMSCR *bm, BMSCR *bm2, int celid, int putx, int puty, float rot, float zx, float zy )
{
	//		sprite
	//		(CELで設定された中心座標を使用する)
	//
	int x,y,srcsx,srcsy,putsx,putsy,centerx,centery;

	srcsx = bm2->divsx;
	srcsy = bm2->divsy;
	x = ( celid % bm2->divx ) * srcsx;
	y = ( celid / bm2->divx ) * srcsy;
	putsx = (int)((float)srcsx * zx );
	putsy = (int)((float)srcsy * zy );
	centerx = (int)((float)(bm2->celofsx - mem_ofsx) * zx );
	centery = (int)((float)(bm2->celofsy - mem_ofsy) * zy );
	GRotateSub( bm, bm2, putx, puty, x, y, srcsx, srcsy, putsx, putsy, rot, centerx, centery );
}


void OmkeDrawSprite2( BMSCR *bm, BMSCR *bm2, int celid, int putx, int puty, float rot, float zx, float zy )
{
	//		sprite
	//		(CELで設定された中心座標を使用しない)
	//
	int x,y,srcsx,srcsy,putsx,putsy;
	//int centerx,centery;

	srcsx = bm2->divsx;
	srcsy = bm2->divsy;
	x = ( celid % bm2->divx ) * srcsx;
	y = ( celid / bm2->divx ) * srcsy;
	putsx = (int)((float)srcsx * zx );
	putsy = (int)((float)srcsy * zy );
	//centerx = (int)((float)mem_ofsx * zx );
	//centery = (int)((float)mem_ofsy * zy );
	GRotateSub( bm, bm2, putx, puty, x, y, srcsx, srcsy, putsx, putsy, rot, mem_ofsx, mem_ofsy );
}



/*------------------------------------------------------------------------------------*/

