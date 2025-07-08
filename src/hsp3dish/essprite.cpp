
//
//		HPDX Compatible Sprite access (essprite)
//			onion software/onitama 2020/3
//

#include "../hsp3/hsp3config.h"

#ifdef HSPWIN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <algorithm>

#include "essprite.h"
#include "supio.h"
#include "sysreq.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/


/*------------------------------------------------------------*/
/*
		ESHitRect interface
*/
/*------------------------------------------------------------*/

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

ESVec2 ESVec2::normalize() const {
	double len = sqrt(x * x + y * y);
	if (len > 0) {
		return ESVec2(x / len, y / len);
	}
	return ESVec2(0, 0);
}


ESRectAxis::ESRectAxis(void)
{
	_rect = NULL;
}


void ESRectAxis::setRect(ESHitRect* rect)
{
	_rect = rect;
}


void ESRectAxis::setRect( int x, int y, int sx, int sy, int ofsx, int ofsy )
{
	_master.center = ESVec2((double)x, (double)y);
	_master.halfSize = ESVec2(((double)sx)*0.5, ((double)sy)*0.5 );
	_master.offset = ESVec2((double)ofsx, (double)ofsy);
	setRect(&_master);
}


void ESRectAxis::setRotation(double rotation)
{
	//	回転角度に応じた頂点を生成する
	//
	_rotation = rotation;
	double s = sin(_rotation);
	double c = cos(_rotation);

	double cx = _rect->center.x;
	double cy = _rect->center.y;
	double halfx = _rect->halfSize.x;
	double halfy = _rect->halfSize.y;
	double halfnx = -halfx;
	double halfny = -halfy;
	double centerx = _rect->center.x;
	double centery = _rect->center.y;
	double offsetx = _rect->offset.x;
	double offsety = _rect->offset.y;

	if ((offsetx != 0.0) || (offsety != 0.0)) {
		halfx += offsetx; halfy += offsety;
		halfnx += offsetx; halfny += offsety;
	}

	_corner[0].x = cx + halfnx * c - halfny * s;
	_corner[0].y = cy + halfnx * s + halfny * c;

	_corner[1].x = cx + halfx * c - halfny * s;
	_corner[1].y = cy + halfx * s + halfny * c;

	_corner[2].x = cx + halfnx * c - halfy * s;
	_corner[2].y = cy + halfnx * s + halfy * c;

	_corner[3].x = cx + halfx * c - halfy * s;
	_corner[3].y = cy + halfx * s + halfy * c;
}


ESVec2 *ESRectAxis::getCorner(int index)
{
	return &_corner[index];
}


ESVec2 ESRectAxis::getAxis(int index)
{
	double s = sin(_rotation);
	double c = cos(_rotation);
	if (index == 0) return ESVec2(c, s).normalize(); // X axis
	return ESVec2(-s, c).normalize(); // Y axis
}


// Checks if projections of two rectangles onto an axis overlap
bool ESRectAxis::checkAxisOverlap( ESRectAxis *rect1, ESRectAxis *rect2,  ESVec2 *axis) {
	double min1 = DBL_MAX, max1 = -DBL_MAX;
	double min2 = DBL_MAX, max2 = -DBL_MAX;

	// Project rect1 onto axis
	for (int i = 0; i < 4; i++) {
		ESVec2 *corner = rect1->getCorner(i);
		double proj = axis->dot(ESVec2(corner->x, corner->y));
		min1 = min(min1, proj);
		max1 = max(max1, proj);
	}

	// Project rect2 onto axis
	for (int i = 0; i < 4; i++) {
		ESVec2 *corner = rect2->getCorner(i);
		double proj = axis->dot(ESVec2(corner->x, corner->y));
		min2 = min(min2, proj);
		max2 = max(max2, proj);
	}

	// Check if projections overlap
	return !(max1 < min2 || max2 < min1);
}


// Main collision check function using SAT
bool ESRectAxis::checkRotatedCollision( ESRectAxis *rect1, ESRectAxis *rect2) {
	// Check rect1's axes
	for (int i = 0; i < 2; i++) {
		ESVec2 axis = rect1->getAxis(i);
		if (!checkAxisOverlap(rect1, rect2, &axis)) {
			return false; // Separation found
		}
	}

	// Check rect2's axes
	for (int i = 0; i < 2; i++) {
		ESVec2 axis = rect2->getAxis(i);
		if (!checkAxisOverlap(rect1, rect2, &axis)) {
			return false; // Separation found
		}
	}

	// No separation found on any axis, must be colliding
	return true;
}


/*------------------------------------------------------------*/
/*
		hgemitter interface
*/
/*------------------------------------------------------------*/

essprite::essprite( void )
{
	mem_sp = NULL;
	mem_chr = NULL;
	mem_map = NULL;
	vpx = NULL;
	vpy = NULL;
	sprite_enable = false;
}


essprite::~essprite( void )
{
	reset();
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int essprite::init(int maxsprite, int maxchr, int rotrate, int maxmap)
{
	reset();
	dotshift = 16;
	dotshift_base = 1 << dotshift;

	spkaz = maxsprite;
	if (spkaz <= 0) spkaz = 512;
	chrkaz = maxchr;
	if (chrkaz <= 0) chrkaz = 1024;
	mapkaz = maxmap;
	if (mapkaz <= 0) mapkaz = 16;

	rrate = rotrate;
	if (rrate > 255) {
		if ((rrate % 256 != 0) && (rrate % 360 != 0)) {
			int r256 = (rrate << 8) >> 8;
			int r360 = (rrate / 360) * 360;
			rrate = ((rrate - r256) < (rrate - r360)) ? r256 : r360;
		}
	}
	else {
		rrate = 64;
	}
	if (rrate > 4096) rrate = 4096;

	//	Memory alloc
	//
	mem_sp = (SPOBJ*)malloc(sizeof(SPOBJ) * spkaz);
	if (mem_sp == NULL) {
		return -1;
	}

	mem_chr = (CHRREF*)malloc(sizeof(CHRREF) * chrkaz);
	if (mem_chr == NULL) {
		return -1;
	}

	mem_map = (BGMAP*)malloc(sizeof(BGMAP) * mapkaz);
	if (mem_map == NULL) {
		return -1;
	}
	initMap();

	vpx = (int*)malloc(sizeof(int) * rrate);
	if (vpx == NULL) {
		return -1;
	}

	vpy = (int*)malloc(sizeof(int) * rrate);
	if (vpy == NULL) {
		return -1;
	}

	//	direction aim parameters
	//
	pi = (double)3.14159264; pans = pi * 2 / rrate;

	int a;
	for (a = 0; a < rrate; a++) {
		vpx[a] = (int)(cos(pi / 2 - pans * a) * 1024);
		vpy[a] = (int)(sin(pi / 2 - pans * a) * 1024);
	}

	//	Object initalize
	//
	for (a = 0; a < spkaz; a++) {
		mem_sp[a].fl = 0;
	}
	for (a = 0; a < chrkaz; a++) {
		chr = &mem_chr[a];
		chr->wid = 0;
		chr->bx = 0; chr->by = 0;
		chr->bsx = 32; chr->bsy = 32;
		chr->colx = 0; chr->coly = 0;
		chr->colsx = 32; chr->colsy = 32;
		chr->lktime = 0; chr->lknum = 0;
		chr->putofsx = 0; chr->putofsy = 0;
	}

	def_fspx = 0;
	def_fspy = 0;
	def_bound = 128;
	def_boundflag = 3;
	framecount = 0;

	def_maplink = -1;
	def_maphitflag = 0;

	setOffset(0, 0);
	sprite_enable = true;
	setSize(16, 16, 100, 0x3ff);

	mem_deco.clear();
	setSpriteScaleOption(0, 0);

	setDebugColor(0, 0xffffff);
	setDebugColor(1, 0x00ff00);
	setDebugColor(2, 0xff0000);
	setDebugColor(3, 0x0000ff);
	return 0;
}

void essprite::setSpriteScaleOption(int scaleopt, int clipopt)
{
	//	Sprite Scaling Option
	//
	sprite_opt = scaleopt;
	sprite_newfunc = (sprite_opt & 1) != 0;
	sprite_clip = (clipopt==0);
}

void essprite::reset(void)
{
	//	Memory release
	//
	sprite_enable = false;

	if (mem_map != NULL) {
		resetMap();
		free(mem_map);
		mem_map = NULL;
	}
	if (mem_chr != NULL) {
		free(mem_chr);
		mem_chr = NULL;
	}
	if (mem_sp != NULL) {
		free(mem_sp);
		mem_sp = NULL;
	}
	if (vpx != NULL) {
		free(vpx);
		vpx = NULL;
	}
	if (vpy != NULL) {
		free(vpy);
		vpy = NULL;
	}

}


void essprite::updateFrame(void)
{
	framecount++;
}


SPOBJ *essprite::getObj(int id)
{
	if ((id < 0) || (id >= spkaz)) return NULL;
	return &mem_sp[id];
}


CHRREF* essprite::getChr(int id)
{
	if ((id < 0) || (id >= chrkaz)) return NULL;
	return &mem_chr[id];
}


int essprite::getEmptyChrNo(void)
{
	int a;
	for (a = 0; a < chrkaz; a++) {
		chr = &mem_chr[a];
		if (chr->wid == 0) return a;
	}
	return -1;
}


int essprite::getEmptySpriteNo(void)
{
	int a;
	SPOBJ* sp;
	for (a = 0; a < spkaz; a++) {
		sp = &mem_sp[a];
		if (sp->fl == 0) return a;
	}
	return -1;
}

int essprite::getEmptySpriteNo(int sp_from, int sp_to, int step)
{
	int b;
	b = sp_from;
	while (1) {
		SPOBJ *sp = &mem_sp[b];
		if (sp->fl == 0) return b;
		b += step;
		if ((b < 0) ||
			(b >= spkaz) ||
			((step < 0) && (b < sp_to)) ||
			((step < 0) && (b > sp_from)) ||
			((step > 0) && (b > sp_to)) ||
			((step > 0) && (b < sp_from)) ||
			(step == 0)) {
			break;
		}
	}
	return -1;
}


int essprite::setResolution(HspWnd* wnd, int sx, int sy, int bufferid)
{
	int sizex, sizey;
	hspwnd = wnd;
	bmscr = wnd->GetBmscr(bufferid);

	if (bmscr == NULL) return -1;

	sizex = sx;
	sizey = sy;
	if (sizex <= 0) sizex = bmscr->sx;
	if (sizey <= 0) sizey = bmscr->sy;

	main_sx = sizex;
	main_sy = sizey;

	setArea(-128, -128, (sizex + 128), (sizey + 128));
	land_x = sizex;
	land_y = sizey;
	return 0;
}


void essprite::setArea(int x, int y, int x2, int y2)
{
	ox1 = x; oy1 = y;
	ox2 = x2; oy2 = y2;
}


void essprite::setSize(int p1, int p2, int p3, int p4)
{
	int rate, nx, ny, bsx, bsy;
	rate = p3;
	if (rate == 0) rate = 100;

	bsx = p1; bsy = p2;
	if (bsx == 0) bsx = df_bsx;
	if (bsy == 0) bsy = df_bsy;
	df_bsx = bsx; df_bsy = bsy;

	nx = bsx *rate / 100;
	ny = bsy *rate / 100;
	df_colx = (bsx - nx) / 2;
	df_coly = (bsy - ny) / 2;
	df_colsx = nx;
	df_colsy = ny;
	df_tpflag = p4;
	df_putofsx = 0;
	df_putofsy = 0;
}


void essprite::setSizeEx(int xsize, int ysize, int tpflag, int colsizex, int colsizey, int colx, int coly, int putx, int puty)
{
	int bsx, bsy;
	bsx = xsize; bsy = ysize;
	if (bsx == 0) bsx = df_bsx;
	if (bsy == 0) bsy = df_bsy;
	df_bsx = bsx; df_bsy = bsy;
	df_tpflag = tpflag;

	df_colx = colx;
	df_coly = coly;
	df_colsx = colsizex;
	df_colsy = colsizey;
	if (df_colsx == 0) df_colsx = bsx;
	if (df_colsy == 0) df_colsy = bsy;

	df_putofsx = putx;
	df_putofsy = puty;
}


void essprite::setLand(int p1, int p2)
{
	land_x = p1;
	land_y = p2;
}


int essprite::setPattern(int p1, int p2, int p3, int p4, int window_id)
{
	int a, x, y, wt;
	a = p1; if (a < 0) { a = getEmptyChrNo(); }
	if ((a < 0) || (a >= chrkaz)) return -1;
	x = p2; y = p3; wt = p4;
	chr = &mem_chr[a];
	chr->bx = x; chr->by = y;
	chr->bsx = df_bsx; chr->bsy = df_bsy;
	chr->colx = df_colx; chr->coly = df_coly;
	chr->colsx = df_colsx; chr->colsy = df_colsy;
	chr->putofsx = df_putofsx;
	chr->putofsy = df_putofsy;
	chr->lktime = wt; chr->lknum = a + 1;
	chr->wid = window_id;
	chr->tpflag = df_tpflag;
	return a;
}


void essprite::getDefaultPatternSize(int* xsize, int* ysize)
{
	*xsize = df_bsx;
	*ysize = df_bsy;
}


int essprite::setLink(int p1, int p2)
{
	int a = p1;
	if ((a < 0) || (a >= chrkaz)) return -1;
	chr = &mem_chr[p1];
	chr->lknum = p2;
	return a;
}


void essprite::setSpritePriority(int id, int pri)
{
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return;
	sp->priority = pri;
}


void essprite::setSpriteCallback(int p1, unsigned short *callback)
{
	SPOBJ *sp = getObj(p1);
	if (sp == NULL) return;
	sp->sbr = callback;
}


void essprite::clear(int spno)
{
	SPOBJ *sp = getObj(spno);
	if (sp == NULL) return;
	sp->type = 0;
	sp->fl = 0;
}


void essprite::clear(int p1, int p2)
{
	int a, b, n;
	b = p2; if (b <= 0) b = spkaz;
	n = p1;
	for (a = 0; a < b; a++) {
		mem_sp[n].type = 0;
		mem_sp[n].fl = 0;
		n++;
		if (n >= spkaz) break;
	}
}


void essprite::setTransparentMode(int tp)
{
	bmscr->gmode = ((tp>>8) & 15 );
	bmscr->gfrate = ( tp & 255 );
}


int essprite::put(int xx, int yy, int chrno, int tpflag, int zoomx, int zoomy, int rotz, int mulcolor, int mode)
{
	//		sprite put
	//
	bool deform;
	int x, y, ix, iy, nx, ny;
	int vx, vy;
	int ofsx, ofsy;
	double rot;

	deform = false;
	x = xx; y = yy;
	if ((chrno < 0) || (chrno >= chrkaz)) return -1;
	chr = &mem_chr[chrno];
	nx = chr->bsx; ny = chr->bsy;
	ix = chr->bx; iy = chr->by;
	ofsx = chr->putofsx;
	ofsy = chr->putofsy;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base) || (rotz != 0)) {
		deform = true;
		x += nx / 2;
		y += ny / 2;
		vx = (nx * zoomx) >> dotshift;
		vy = (ny * zoomy) >> dotshift;
		rot = pans * -rotz;
		
		if (ofsx != 0) ofsx = (ofsx * zoomx) >> dotshift;
		if (ofsy != 0) ofsy = (ofsy * zoomy) >> dotshift;

		x += ofsx; y += ofsy;
		if (sprite_clip) {
			if ((x + vx) <= 0) return -1;
			if ((x - vx) > main_sx) return -1;
			if ((y + vy) <= 0) return -1;
			if ((y - vy) > main_sy) return -1;
		}
	}
	else {
		vx = nx; vy = ny;
		x += ofsx; y += ofsy;
		if (sprite_clip) {
			if ((x + vx) <= 0) return -1;
			if (x > main_sx) return -1;
			if ((y + vy) <= 0) return -1;
			if (y > main_sy) return -1;
		}
	}

	Bmscr* src = hspwnd->GetBmscrSafe(chr->wid);
	if (src == NULL) return -1;

	bmscr->cx = x;
	bmscr->cy = y;

	if ( bmscr->mulcolor != ( mulcolor & 0xffffff ) ) {
		int a1 = (mulcolor>>16) & 0xff;
		int a2 = (mulcolor>>8) & 0xff;
		int a3 = mulcolor & 0xff;
		bmscr->SetMulcolor(a1,a2,a3);
	}

	int tp = tpflag;
	if (tp < 0) tp = chr->tpflag;
	setTransparentMode(tp);

	if (mode & ESDRAW_DEBUG) {
		// normal draw
		if (deform) {
			bmscr->FillRotTex(vx, vy, (float)rot, src, ix, iy, nx, ny);
			return 0;
		}
		bmscr->Copy(src, ix, iy, nx, ny);
		return 0;
	}

	//	DEBUG draw
	int colsx = chr->colsx;
	int colsy = chr->colsy;
	int colx = xx + chr->colx;
	int coly = yy + chr->coly;

	if (deform) {
		//bmscr->FillRotTex(vx, vy, (float)rot, src, ix, iy, nx, ny);

		ESVec2* corner;
		ESVec2* corner_start;

		int colvx = ((chr->colx) * zoomx) >> dotshift;
		int colvy = ((chr->coly) * zoomy) >> dotshift;
		colsx = (chr->colsx * zoomx) >> dotshift;
		colsy = (chr->colsy * zoomy) >> dotshift;

		int basex = ((colsx - vx)>>1) + colvx;
		int basey = ((colsy - vy)>>1) + colvy;
		rectaxis.setRect(x, y, colsx, colsy, basex-ofsx, basey-ofsy);
		rectaxis.setRotation(rot);
		bmscr->Setcolor(debugcolor_line[1]);

		corner_start = rectaxis.getCorner(0);
		bmscr->cx = (int)corner_start->x; bmscr->cy = (int)corner_start->y;
		corner = rectaxis.getCorner(1);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(3);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(2);
		bmscr->Line((int)corner->x, (int)corner->y);
		bmscr->Line((int)corner_start->x, (int)corner_start->y);

		rectaxis.setRect(x, y, vx, vy);
		rectaxis.setRotation(rot);
		bmscr->Setcolor(debugcolor_line[0]);

		corner_start = rectaxis.getCorner(0);
		bmscr->cx = (int)corner_start->x; bmscr->cy = (int)corner_start->y;
		corner = rectaxis.getCorner(1);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(3);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(2);
		bmscr->Line((int)corner->x, (int)corner->y);
		bmscr->Line((int)corner_start->x, (int)corner_start->y);

		return 0;
	}
	//bmscr->Copy(src, ix, iy, nx, ny);

	bmscr->Setcolor(debugcolor_line[1]);
	bmscr->cx = colx;
	bmscr->cy = coly;
	bmscr->Line(colx + colsx, coly);
	bmscr->Line(colx + colsx, coly + colsy);
	bmscr->Line(colx, coly + colsy);
	bmscr->Line(colx, coly);

	bmscr->Setcolor(debugcolor_line[0]);
	bmscr->cx = x;
	bmscr->cy = y;
	bmscr->Line(x + nx, y);
	bmscr->Line(x + nx, y + ny);
	bmscr->Line(x, y + ny);
	bmscr->Line(x, y);

	return 0;
}


int essprite::put2(int xx, int yy, int chrno, int tpflag, int zoomx, int zoomy, int rotz, int mulcolor, int mode)
{
	//		sprite put
	//
	bool deform;
	int x, y, ix, iy, nx, ny;
	int vx, vy;
	int ofsx, ofsy;
	double rot;

	deform = false;
	x = xx; y = yy;
	if ((chrno < 0) || (chrno >= chrkaz)) return -1;
	chr = &mem_chr[chrno];
	nx = chr->bsx; ny = chr->bsy;
	ix = chr->bx; iy = chr->by;
	ofsx = chr->putofsx;
	ofsy = chr->putofsy;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base) || (rotz != 0)) {
		deform = true;
		x += nx / 2;
		y += ny / 2;
		vx = (nx * zoomx) >> dotshift;
		vy = (ny * zoomy) >> dotshift;
		rot = pans * -rotz;

		x += ofsx; y += ofsy;
		if (sprite_clip) {
			if ((x + vx) <= 0) return -1;
			if ((x - vx) > main_sx) return -1;
			if ((y + vy) <= 0) return -1;
			if ((y - vy) > main_sy) return -1;
		}
	}
	else {
		vx = nx; vy = ny;
		x += ofsx; y += ofsy;
		if (sprite_clip) {
			if ((x + vx) <= 0) return -1;
			if (x > main_sx) return -1;
			if ((y + vy) <= 0) return -1;
			if (y > main_sy) return -1;
		}
	}

	Bmscr* src = hspwnd->GetBmscrSafe(chr->wid);
	if (src == NULL) return -1;

	bmscr->cx = x;
	bmscr->cy = y;

	if (bmscr->mulcolor != (mulcolor & 0xffffff)) {
		int a1 = (mulcolor >> 16) & 0xff;
		int a2 = (mulcolor >> 8) & 0xff;
		int a3 = mulcolor & 0xff;
		bmscr->SetMulcolor(a1, a2, a3);
	}

	int tp = tpflag;
	if (tp < 0) tp = chr->tpflag;
	setTransparentMode(tp);

	if (mode & ESDRAW_DEBUG) {
		// normal draw
		if (deform) {
			bmscr->FillRotTex(vx, vy, (float)rot, src, ix, iy, nx, ny);
			return 0;
		}
		bmscr->Copy(src, ix, iy, nx, ny);
		return 0;
	}

	//	DEBUG draw
	int colsx = chr->colsx;
	int colsy = chr->colsy;
	int colx = xx + chr->colx;
	int coly = yy + chr->coly;

	if (deform) {
		//bmscr->FillRotTex(vx, vy, (float)rot, src, ix, iy, nx, ny);

		ESVec2* corner;
		ESVec2* corner_start;

		int colvx = ((chr->colx) * zoomx) >> dotshift;
		int colvy = ((chr->coly) * zoomy) >> dotshift;
		colsx = (chr->colsx * zoomx) >> dotshift;
		colsy = (chr->colsy * zoomy) >> dotshift;

		int basex = ((colsx - vx) >> 1) + colvx;
		int basey = ((colsy - vy) >> 1) + colvy;
		rectaxis.setRect(x, y, colsx, colsy, basex - ((ofsx * zoomx) >> dotshift), basey - ((ofsy * zoomy) >> dotshift));

		rectaxis.setRotation(rot);
		bmscr->Setcolor(debugcolor_line[1]);

		corner_start = rectaxis.getCorner(0);
		bmscr->cx = (int)corner_start->x; bmscr->cy = (int)corner_start->y;
		corner = rectaxis.getCorner(1);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(3);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(2);
		bmscr->Line((int)corner->x, (int)corner->y);
		bmscr->Line((int)corner_start->x, (int)corner_start->y);

		rectaxis.setRect(x, y, vx, vy);
		rectaxis.setRotation(rot);
		bmscr->Setcolor(debugcolor_line[0]);

		corner_start = rectaxis.getCorner(0);
		bmscr->cx = (int)corner_start->x; bmscr->cy = (int)corner_start->y;
		corner = rectaxis.getCorner(1);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(3);
		bmscr->Line((int)corner->x, (int)corner->y);
		corner = rectaxis.getCorner(2);
		bmscr->Line((int)corner->x, (int)corner->y);
		bmscr->Line((int)corner_start->x, (int)corner_start->y);

		return 0;
	}
	//bmscr->Copy(src, ix, iy, nx, ny);

	bmscr->Setcolor(debugcolor_line[1]);
	bmscr->cx = colx;
	bmscr->cy = coly;
	bmscr->Line(colx + colsx, coly);
	bmscr->Line(colx + colsx, coly + colsy);
	bmscr->Line(colx, coly + colsy);
	bmscr->Line(colx, coly);

	bmscr->Setcolor(debugcolor_line[0]);
	bmscr->cx = x;
	bmscr->cy = y;
	bmscr->Line(x + nx, y);
	bmscr->Line(x + nx, y + ny);
	bmscr->Line(x, y + ny);
	bmscr->Line(x, y);

	return 0;
}


int essprite::find(int chktype, int spno, int endspno, int step)
{
	int a, res;
	int aplus, spno_end;
	int cc;
	a = spno;
	cc = chktype;
	if (cc == 0) cc = -1;
	spno_end = endspno;
	if (spno_end < 0) spno_end = spkaz - 1;
	aplus = step;
	if (aplus == 0) {
		aplus = 1;
		if (spno_end<a) aplus = -1;
	}

	res = -1;
	while (1) {
		if (a >= spkaz) break;
		SPOBJ* sp = getObj(a);
		if (sp == NULL) break;
		if (sp->fl) {
			if (sp->type & cc) { res = a; break; }
		}
		if (a == spno_end) break;
		a+=aplus;
	}
	return res;
}


int essprite::checkCollisionSub(SPOBJ* sp)
{
	int xx, yy, x1, y1;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chr = &mem_chr[sp->chr];

	int zoomx = sp->zoomx;
	int zoomy = sp->zoomy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;

	getSpriteParentAxis(sp, xx, yy, 0);
	x1 = (xx) + chrcolx;
	y1 = (yy) + chrcoly;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
		int px = (chr->bsx << dotshift) - (chr->bsx * zoomx);
		int py = (chr->bsy << dotshift) - (chr->bsy * zoomy);
		x1 += px / 2;
		y1 += py / 2;
	}

	if (colex <= x1) return 0;
	if (coley <= y1) return 0;

	if ((x1 + chrcolsx) > colx) {
		if ((y1 + chrcolsy) > coly) {
			return 1;
		}
	}
	return 0;
}


int essprite::checkCollisionSub2(SPOBJ* sp)
{
	int xx, yy, x1, y1;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chr = &mem_chr[sp->chr];

	int zoomx = sp->zoomx;
	int zoomy = sp->zoomy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;

	getSpriteParentAxis(sp, xx, yy, 0);
	x1 = (xx)+chrcolx;
	y1 = (yy)+chrcoly;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
		x1 -= zoomx >> 1;
		y1 -= zoomy >> 1;
	}

	if (colex <= x1) return 0;
	if (coley <= y1) return 0;

	if ((x1 + chrcolsx) > colx) {
		if ((y1 + chrcolsy) > coly) {
			return 1;
		}
	}
	return 0;
}


int essprite::checkCollision(int spno, int chktype, int rotflag, int startno, int endno)
{
	//		sprite collision check
	//
	int a, cc;
	int x, y;
	int n_start, n_end;

	cc = chktype;
	if (cc == 0) cc = -1;
	SPOBJ* ssp = getObj(spno);
	if (ssp==NULL) return -1;
	chr = getChr(ssp->chr);
	if (chr == NULL) return -1;

	int zoomx = ssp->zoomx;
	int zoomy = ssp->zoomy;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;

	getSpriteParentAxis(ssp, x, y, 0);
	if (sprite_newfunc) {
		colx = (x)+chrcolx;
		coly = (y)+chrcoly;
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			colx -= zoomx>>1;
			coly -= zoomy>>1;
		}
	}
	else {
		colx = (x)+chrcolx;
		coly = (y)+chrcoly;
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			int px = (chr->bsx << dotshift) - (chr->bsx * zoomx);
			int py = (chr->bsy << dotshift) - (chr->bsy * zoomy);
			colx += px>>1;
			coly += py>>1;
		}
	}

	colex = colx + chrcolsx;
	coley = coly + chrcolsy;

	n_start = startno;
	n_end = endno;
	if ((n_end < 0)||(n_end >= spkaz)) {
		n_end = spkaz-1;
	}

	a = n_start;
	SPOBJ* sp = getObj(a);
	if (sp == NULL) return -1;

	if (sprite_newfunc) {
		while (1) {
			if (a > n_end) break;
			if (sp->fl) {
				if (sp->type & cc) {
					if (spno != a) {
						if (checkCollisionSub2(sp)) return a;
					}
				}
			}
			sp++;
			a++;
		}
	}
	else {
		while (1) {
			if (a > n_end) break;
			if (sp->fl) {
				if (sp->type & cc) {
					if (spno != a) {
						if (checkCollisionSub(sp)) return a;
					}
				}
			}
			sp++;
			a++;
		}
	}
	return -1;
}


int essprite::checkCollisionRotateSub(SPOBJ* sp, ESRectAxis* rect, ESRectAxis* rect2)
{
	//		check 4-corner collision (rotate)
	//
	int xx, yy, x1, y1, vx, vy;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chr = &mem_chr[sp->chr];

	int zoomx = sp->zoomx;
	int zoomy = sp->zoomy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;
	vx = chr->bsx * zoomx;
	vy = chr->bsy * zoomy;

	getSpriteParentAxis(sp, xx, yy, 0);
	x1 = (xx)+(vx>>1);
	y1 = (yy)+(vy>>1);
	if (sprite_newfunc) {
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			x1 -= zoomx >> 1;
			y1 -= zoomy >> 1;
		}
	}
	else {
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			int px = (chr->bsx << dotshift) - (chr->bsx * zoomx);
			int py = (chr->bsy << dotshift) - (chr->bsy * zoomy);
			x1 += px >> 1;
			y1 += py >> 1;
		}
	}

	int basex = ((chrcolsx - vx) >> 1) + chrcolx;
	int basey = ((chrcolsy - vy) >> 1) + chrcoly;

	rect2->setRect(x1, y1, chrcolsx, chrcolsy, basex, basey);
	rect2->setRotation(pans * -sp->rotz);

	if (rect->checkRotatedCollision(rect, rect2)) return 1;

	return 0;
}


int essprite::checkCollisionRotate(int spno, int chktype, int rotflag, int startno, int endno)
{
	//		sprite collision check (rotate)
	//
	int a, cc;
	int x, y, x1, y1, vx, vy;
	int n_start, n_end;
	ESRectAxis rect;
	ESRectAxis rect2;

	cc = chktype;
	if (cc == 0) cc = -1;
	SPOBJ* ssp = getObj(spno);
	if (ssp == NULL) return -1;
	chr = getChr(ssp->chr);
	if (chr == NULL) return -1;

	int zoomx = ssp->zoomx;
	int zoomy = ssp->zoomy;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;
	vx = chr->bsx * zoomx;
	vy = chr->bsy * zoomy;

	getSpriteParentAxis(ssp, x, y, 0);
	x1 = x+(vx >> 1);
	y1 = y+(vy >> 1);
	if (sprite_newfunc) {
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			x1 -= zoomx >> 1;
			y1 -= zoomy >> 1;
		}
	}
	else {
		if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
			int px = (chr->bsx << dotshift) - (chr->bsx * zoomx);
			int py = (chr->bsy << dotshift) - (chr->bsy * zoomy);
			x1 += px >> 1;
			y1 += py >> 1;
		}
	}

	int basex = ((chrcolsx - vx) >> 1) + chrcolx;
	int basey = ((chrcolsy - vy) >> 1) + chrcoly;

	rect.setRect( x1, y1, chrcolsx, chrcolsy, basex, basey );
	rect.setRotation( pans * -ssp->rotz );

	n_start = startno;
	n_end = endno;
	if ((n_end < 0) || (n_end >= spkaz)) {
		n_end = spkaz - 1;
	}

	a = n_start;
	SPOBJ* sp = getObj(a);
	if (sp == NULL) return -1;

	while (1) {
		if (a > n_end) break;
		if (sp->fl) {
			if (sp->type & cc) {
				if (spno != a) {
					if (checkCollisionRotateSub(sp,&rect,&rect2)) return a;
				}
			}
		}
		sp++;
		a++;
	}
	return -1;
}


int essprite::getNear(int spno, int chktype, int range)
{
	//		最も近くにあるスプライトを取得する
	//
	int a,cc,res;
	double max;

	res = -1;
	cc = chktype;
	if (cc == 0) cc = -1;
	SPOBJ* ssp = getObj(spno);
	if (ssp == NULL) return -1;
	if (range == 0) {
		max = 9999.0;
	}
	else {
		max = (double)(range << dotshift);
	}

	SPOBJ* sp = getObj(0);
	if (sp == NULL) return -1;

	for (a = 0; a < spkaz; a++) {
		if (sp->fl) {
			if (sp->type & cc) {
				if (spno != a) {
					double dist = utilGetDistanceFloat(sp->xx,sp->yy,ssp->xx,ssp->yy);
					if (dist < max) {
						res = a; max = dist;
					}
				}
			}
		}
		sp++;
	}
	return res;
}


int essprite::getSpriteParentAxis(SPOBJ *sp, int& xx, int& yy, int depth)
{
	//		親座標を含めたスプライトの座標を取得する(再帰)
	//
	if (sp->fl & ESSPFLAG_SPLINK) {
		int link = sp->splink;
		if (link & ESSPLINK_BGMAP) {
			BGMAP* bg = getMap(link & (ESSPLINK_BGMAP-1));
			if (bg == NULL) {
				xx = 0; yy = 0;
				return -1;
			}
			xx = sp->xx - ( bg->viewx << dotshift );
			yy = sp->yy - ( bg->viewy << dotshift );
			return 0;
		}
		SPOBJ* p = getObj(link);
		int px, py;
		if (p == NULL) {
			xx = 0; yy = 0;
			return -1;
		}
		if (depth < 16) {		// Nest loop cancel
			int res = getSpriteParentAxis(p, px, py, depth + 1);
			if (res < 0) return res;
			xx = sp->xx + px;
			yy = sp->yy + py;
			return 0;
		}
	}
	xx = sp->xx; yy = sp->yy;
	return 0;
}


int essprite::modifySpriteAxis(int spno, int endspno, int type, int x, int y, int option)
{
	int a, res;
	int spno_end;
	int cc;
	int px, py;
	a = spno;
	cc = type;
	res = 0;
	if (cc == 0) cc = -1;
	spno_end = endspno;
	if (spno_end < 0) spno_end = spkaz - 1;
	SPOBJ* sp = getObj(a);
	if (sp == NULL) return -1;
	if (option & ESSPSET_DIRECT) {
		px = x; py = y;
	}
	else {
		px = x << dotshift; py = y << dotshift;
	}
	while (1) {
		if (a > spno_end) break;
		if (sp->fl) {
			if (sp->type & cc) {
				switch (option & (ESSPSET_DIRECT-1)) {
				case ESSPSET_POS:
					sp->xx += px;
					sp->yy += py;
					break;
				case ESSPSET_ADDPOS:
					sp->px += px;
					sp->py += py;
					break;
				case ESSPSET_FALL:
					sp->fspx += px;
					sp->fspy += py;
					break;
				case ESSPSET_ZOOM:
					sp->zoomx += px;
					sp->zoomy += py;
					break;
				default:
					break;
				}
				res++;
			}
		}
		sp++;
		a++;
	}
	return res;
}


int essprite::drawSubMoveGravity(SPOBJ* sp)
{
	//		hspdx互換性のためサポート
	//
	int fl, x, y, land, res, xx, yy, prevxx, prevyy;
	int bsx, bsy;

	chr = getChr(sp->chr);
	if (chr == NULL) return 0;

	res = 0;
	sp->moveres = 0;

	bsx = chr->bsx;
	bsy = chr->bsy;

	fl = sp->fl;

	prevxx = sp->xx;
	prevyy = sp->yy;

	sp->xx += sp->px;
	sp->yy += sp->py;

	xx = sp->xx;
	yy = sp->yy;

	if (sp->fspy != 0) {
		sp->py += sp->fspy << 8;
		if (sp->boundflag & 2) {
			if (sp->fspy > 0) {
				land = (land_y - bsy) << dotshift;
				if (yy > land) {
					sp->yy = prevyy;
					y = sp->py;
					if (y < 0) y = 0;
					if (y < 0x18000) {
						sp->fl = (fl ^ ESSPFLAG_GRAVITY) | 0x300;
						sp->py = 0;
					}
					else sp->py = -((int)(y & 0xffff0000) * sp->bound >> 8);
				}
			}
			else {
				if (yy < 0) {
					sp->yy = prevyy;
					y = abs(sp->py);
					if (y < 0) y = 0;
					if (y < 0x18000) {
						sp->fl = (fl ^ ESSPFLAG_GRAVITY) | 0x300;
						sp->py = 0;
					}
					else sp->py = ((int)(y & 0xffff0000) * sp->bound >> 8);
				}
			}
		}
	}

	if (sp->fspx != 0) {
		sp->px += sp->fspx << 8;
		if (sp->boundflag & 1) {
			if (sp->fspx > 0) {
				land = (land_x - bsx) << dotshift;
				if (xx > land) {
					sp->xx = prevxx;
					x = sp->px;
					if (x < 0) x = 0;
					if (x < 0x18000) {
						sp->fl = (fl ^ ESSPFLAG_GRAVITY) | 0x300;
						sp->px = 0;
					}
					else sp->px = -((int)(x & 0xffff0000) * sp->bound >> 8);
				}
			}
			else {
				if (xx < 0) {
					sp->xx = prevxx;
					x = abs(sp->px);
					if (x < 0) x = 0;
					if (x < 0x18000) {
						sp->fl = (fl ^ ESSPFLAG_GRAVITY) | 0x300;
						sp->px = 0;
					}
					else sp->px = ((int)(x & 0xffff0000) * sp->bound >> 8);
				}
			}
		}
	}

	return res;
}


int essprite::drawSubMoveVector(SPOBJ* sp)
{
	//		スプライト移動モード処理
	//		(BGマップとの当たりを考慮)
	//
	int px, py, res, i;
	int bak_x, bak_y;
	int myx, myy;
	int ofsx, ofsy;

	sp->px += sp->fspx << 8;
	sp->py += sp->fspy << 8;
	bak_x = sp->xx;
	bak_y = sp->yy;
	px = sp->px; py = sp->py;

	//		重力がある場合、+-1にする
	if (sp->fspx < 0) {
		if ((px >> 16) == 0) {
			px = -0x10000;
		}
	}
	if (sp->fspx > 0) {
		if ((px >> 16) == 0) {
			px = 0x10000;
		}
	}
	if (sp->fspy < 0) {
		if ((py >> 16) == 0) {
			py = -0x10000;
		}
	}
	if (sp->fspy > 0) {
		if ((py >> 16) == 0) {
			py = 0x10000;
		}
	}

	//if (sp->fspy>0) py += 0x10000;
	sp->moveres = 0;

	if (sp->spstick >= 0) {				// 吸着している場合
		SPOBJ* tsp = getObj(sp->spstick);
		if (tsp) {
			int x = tsp->xx - sp->spst_x;
			int y = tsp->yy - sp->spst_y;
			px += x - sp->xx;
			py += y - sp->yy;
			sp->moveres |= ESSPRES_GROUND;
		}
		else {
			setSpriteStick(sp,-1);
			bak_x = sp->xx;
			bak_y = sp->yy;
		}
	}

	int bgid = sp->splink;
	if ((bgid & ESSPLINK_BGMAP) == 0) return 1;
	bgid = bgid & (ESSPLINK_BGMAP - 1);
	BGMAP* bg = getMap(bgid);
	if (bg == NULL) return 2;

	CHRREF* chr = getChr(sp->chr);
	if (chr == NULL) return 4;
	ofsx = (chr->colx << dotshift);
	ofsy = (chr->coly << dotshift);
	myx = sp->xx + ofsx;
	myy = sp->yy + ofsy;

	int myattr = getMapAttrFromPos(bgid, myx>>dotshift, myy>>dotshift);
	sp->moveres |= (myattr & 0xff);

	if ((px == 0) && (py == 0)) return 3;

	if (sp->maphit & ESSPMAPHIT_BLOCKBIT) {
		bg->blockbit = bg->blockbit_org;
	}
	else {
		bg->blockbit = 0;
	}

	res = getMapMaskHit32(bgid, myx, myy, chr->colsx, chr->colsy, px, py);
	if (res <= 0) return -1;

	bool hit = false;
	bool flipx = false;
	bool flipy = false;
	int targetsp = -1;
	i = 0;
	while (1) {
		if (i >= res) break;
		BGHITINFO* info = getMapHitInfo(bgid, i);
		switch (info->result) {
		case ESMAPHIT_NONE:
			sp->xx = info->x - ofsx;
			sp->yy = info->y - ofsy;
			break;
		case ESMAPHIT_HITX:
			hit = true;
			flipx = true;
			sp->moveres |= ESSPRES_XBLOCK;
			if (sp->fspx > 0) {
				if (px > 0) sp->moveres |= ESSPRES_GROUND;
			}
			if (sp->fspx < 0) {
				if (px < 0) sp->moveres |= ESSPRES_GROUND;
			}
			break;
		case ESMAPHIT_HITY:
			hit = true;
			flipy = true;
			sp->moveres |= ESSPRES_YBLOCK;
			if (sp->fspy > 0) {
				if (py > 0) {
					sp->moveres |= ESSPRES_GROUND;
				}
			}
			if (sp->fspy < 0) {
				if (py < 0) {
					sp->moveres |= ESSPRES_GROUND;
				}
			}
			break;
		case ESMAPHIT_SPHIT:
			if (sp->maphit & ESSPMAPHIT_STICKSP) {
				targetsp = info->celid;
			}
			break;
		case ESMAPHIT_EVENT:
			if (sp->maphit & ESSPMAPHIT_WIPEEVENT) {
				int* p = bg->varptr;
				p[info->myy * bg->mapsx + info->myx] = 0;
			}
			break;
		default:
			break;
		}
		i++;
	}

	if (hit) {
		if (sp->maphit & ESSPMAPHIT_HITWIPE) {
			sp->fl = 0;
			if (bg->spwipe_chr >= 0) {
				setSpriteDecoration(sp->xx >> dotshift, sp->yy >> dotshift, bg->spwipe_chr);
			}
			return 0;
		}
	}

	if (flipx) {
		if (sp->fl & ESSPFLAG_XBOUNCE) {
			if (sp->boundflag & ESSPBOUND_BOUNDX) {
				int mov = abs(sp->px);
				if (sp->px < 0) {
					sp->px = mov;
				}
				else {
					if (sp->px > 0) sp->px = -mov;
				}
			}
			else {
				sp->px = -sp->px;
			}
		}
		else {
			sp->px = 0;
		}
	}
	if (flipy) {
		if (sp->fl & ESSPFLAG_YBOUNCE) {
			if (sp->boundflag & ESSPBOUND_BOUNDY) {
				int mov = (abs(sp->py) * sp->bound) >> 8;
				if (sp->py < 0) {
					sp->py = mov;
				}
				else {
					if (sp->py > 0) sp->py = -mov;
				}
			}
			else {
				sp->py = -sp->py;
			}
		}
		else {
			sp->py = 0;
		}
	}

	if (sp->spstick >= 0) {				// 吸着している場合
		if (targetsp < 0) {
			setSpriteStick(sp, -1);
			return 0;
		}
	}

	if (targetsp >= 0) {
		SPOBJ* ssp = getObj(targetsp);
		if (ssp) {
			int yy = ssp->yy;
			sp->yy = yy - ( chr->bsy << dotshift );
			sp->moveres |= ESSPRES_GROUND;
		}
		setSpriteStick(sp, targetsp);
	}

	return 0;
}


int essprite::execSingle(int spno)
{
	//		1 sprite move (move only)
	//
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	bool ok = false;
	if (sp->maphit & ESSPMAPHIT_BGHIT) {
		sp->fl &= ~ESSPFLAG_MOVE;
		if (drawSubMoveVector(sp) == 0) {		// BGマップとの当たりを取る
			ok = true;
		}
	}
	if (ok) {
		return 0;
	}
	return 1;
}


int essprite::drawSubMove(SPOBJ *sp, int mode)
{
	//		1 sprite move (on sp)
	//
	int fl, x, y, res, xx, yy;

	//		flag pre check
	//
	res = 0;
	fl = sp->fl;
	if ((fl & ESSPFLAG_STATIC) == 0) return -1;

	//		add pos move
	//
	if (fl & ESSPFLAG_MOVE) {
		if (sp->maphit & ESSPMAPHIT_BGHIT) {
			if (drawSubMoveVector(sp)) {		// BGマップとの当たりを取る
				sp->xx += sp->px;
				sp->yy += sp->py;
			}
			if (sp->fl == 0) return 0;
		}
		else {
			sp->xx += sp->px;
			sp->yy += sp->py;
		}
	}
	if (fl & ESSPFLAG_MOVEROT) {
		sp->rotz += sp->protz;
		sp->zoomx += sp->pzoomx;
		sp->zoomy += sp->pzoomy;
	}

	//getSpriteAxis(sp, xx, yy);
	getSpriteParentAxis(sp, xx, yy, 0);

	if (fl & ESSPFLAG_GRAVITY) {
		//	なんだこりゃ・・・
		//
		drawSubMoveGravity(sp);
		xx = sp->xx;
		yy = sp->yy;
	}

	//		border check
	//
	x = xx >> dotshift; y = yy >> dotshift;
	if (fl & ESSPFLAG_NOWIPE) {
		if (fl & ESSPFLAG_XBOUNCE) {
			if (x < ox1) { x = ox1; sp->px = abs(sp->px); }
			if (x > ox2) { x = ox2; sp->px = -abs(sp->px); }
		}
		if (fl & ESSPFLAG_YBOUNCE) {
			if (y < oy1) { y = oy1; sp->py = abs(sp->py); }
			if (y > oy2) { y = oy2; sp->py = -abs(sp->py); }
		}
	}
	else {
		if (((int)((x - ox1) | (y - oy1) | (ox2 - x) | (oy2 - y))) >> 31) {
			sp->fl = 0;
			return 0;
		}
	}

	//		Flag timer check
	//
	if (sp->timer) {
		if (fl & (ESSPFLAG_FADEIN | ESSPFLAG_FADEOUT)) {
			execTimerFade(sp);
		}
		if (fl & (ESSPFLAG_EFADE | ESSPFLAG_EFADE2)) {
			execTimerEndFade(sp);
		}
		if (sp->timer > 0) {
			sp->timer--;
			if (sp->timer == 0) {
				if (fl & ESSPFLAG_TIMERWIPE) {
					sp->fl = 0;
				}
				resetTimer(sp);
			}
		}
	}
	sp->progress++;

	return res;
}


void essprite::execTimerFade(SPOBJ* sp)
{
	//		Execute timer fade event
	//
	int fl = sp->fl;
	int tpmode = (sp->tpflag & 0xffffff00);
	int timer = sp->timer;
	int fadeprm = sp->fadeprm;
	int res = 0;
	bool fadedone = false;

	if (timer <= 1) fadedone = true;

	res = timer * fadeprm / sp->timer_base;
	if (res < 0) res = 0;
	if (res > fadeprm) res = fadedone;
	if (fadedone) res = 0;

	if (fl & ESSPFLAG_FADEIN) {
		res = fadeprm - res;
	}
	sp->tpflag = res | tpmode;
}


void essprite::execTimerEndFade(SPOBJ* sp)
{
	//		Execute timer fade event
	//
	int fl = sp->fl & (ESSPFLAG_EFADE | ESSPFLAG_EFADE2);
	int tpmode = (sp->tpflag & 0xffffff00);
	int timer = sp->timer;
	int fadeprm = sp->fadeprm;
	int endstart,endframe;
	int res = 0;

	endstart = 2;
	if (fl == ESSPFLAG_EFADE) {
		endstart = 16;
	}
	if (fl == ESSPFLAG_EFADE2) {
		endstart = 8;
	}

	endframe = fadeprm / endstart;
	if (timer >= endframe) return;

	timer--;
	if (timer < 0) timer = 0;
	res = timer * endstart;
	if (res < 0) res = 0;
	if (res > fadeprm) res = fadeprm;
	sp->tpflag = res | tpmode;
}


int essprite::drawSubPut(SPOBJ *sp, int mode)
{
	//		1 sprite draw (on sp)
	//
	int res = 0;
	int fl,x,y,xx,yy,blink;
	int next;

	fl = sp->fl;
	if (fl & ESSPFLAG_NODISP) return 0;
	if ( fl & (ESSPFLAG_BLINK | ESSPFLAG_BLINK2)) {
		int flbase = fl & (ESSPFLAG_BLINK | ESSPFLAG_BLINK2);
		blink = 1;
		if (flbase == ESSPFLAG_BLINK2) blink = 2;
		if (flbase == (ESSPFLAG_BLINK | ESSPFLAG_BLINK2)) blink = 4;
		if (framecount & blink) return 0;
	}

	//getSpriteAxis(sp, xx, yy);
	int err = getSpriteParentAxis(sp, xx, yy, 0);
	if (err < 0) {
		sp->fl = 0; return res;
	}

	x = xx >> dotshift; y = yy >> dotshift;

	//		send to screen
	//
	if (mode & ESDRAW_NODISP) {
		int chr = sp->chr;
		if (sp->xrevchr != 0) {
			int add = sp->xrevchr;
			if (add < 0) {
				if (sp->px <= 0) {
					chr += -add;
				}
			}
			else {
				if (sp->px > 0) {
					chr += add;
				}
			}
		}
		if (sp->yrevchr != 0) {
			int add = sp->yrevchr;
			if (add < 0) {
				if (sp->py <= 0) {
					chr += -add;
				}
			}
			else {
				if (sp->py > 0) {
					chr += add;
				}
			}
		}
		if (sprite_newfunc) {
			put2(x + ofsx, y + ofsy, chr, sp->tpflag, sp->zoomx, sp->zoomy, sp->rotz, sp->mulcolor, mode);
		}
		else {
			put(x + ofsx, y + ofsy, chr, sp->tpflag, sp->zoomx, sp->zoomy, sp->rotz, sp->mulcolor, mode);
		}
		res++;
	}

	//		chr animation process
	//
	if (mode & ESDRAW_NOANIM) {
		if (sp->ani) {
			sp->ani--;
			if (sp->ani == 0) {
				chr = &mem_chr[sp->chr];
				next = chr->lknum;
				if (next < 0) { sp->fl = 0; return 0; }	// next<0 wipe
				sp->chr = next;
				chr = &mem_chr[sp->chr];
				sp->ani = chr->lktime;
			}
		}
	}

	if (fl & 255) {						// HSPDX互換のカウントダウンタイマー(互換維持用)
		fl--;
		if ((fl & 255) == 0) fl = 0;
		sp->fl = fl;
	}

	return res;
}


static bool less_int_1(SPRDATA const& lhs, SPRDATA const& rhs)
{
	int cmp = (lhs.ikey - rhs.ikey);
	return (cmp < 0);
}


int essprite::draw(int start, int num, int mode, int start_pri, int end_pri)
{
	//		draw all sprites
	//
	SPOBJ* sp;
	int i, a, a1, a2, mode_p;
	bool priselect = false;
	HSPCTX *ctx = hspwnd->GetHSPCTX();
	SPRDATA *selspr = new SPRDATA[spkaz];
	SPRDATA *spr;
	int maxspr = 0;

	mode_p = mode ^ -1;
	a1 = start; a2 = num;
	if (a1 < 0) a1 = 0;
	if (a2 < 0) a2 = spkaz;
	if (a1 >= spkaz) return -1;
	if ((a1 + a2) >= spkaz) a2 = spkaz-a1;

	if ((start_pri >= 0) && (end_pri >= 0)) {
		if (start_pri <= end_pri) {
			priselect = true;
		}
	}

	spr = selspr;

	a = a1 + a2 - 1;
	sp = getObj(a);
	for (i = 0; i < a2; i++) {
		if ((sp->fl & ESSPFLAG_DECORATE) == 0) {
			if (priselect) {
				if ((start_pri<=sp->priority) && (end_pri >= sp->priority)) {
					spr->ikey = sp->priority;
					spr->info = a;
					spr++;
					maxspr++;
				}
			}
			else {
				spr->info = a;
				spr++;
				maxspr++;
			}
		}
		sp--;
		a--;
	}

	a = a1 + a2 - 1;
	sp = getObj(a);
	for (i = 0; i < a2; i++) {
		if (sp->fl) {
			if (sp->fl & ESSPFLAG_DECORATE) {
				if (priselect) {
					if ((start_pri <= sp->priority) && (end_pri >= sp->priority)) {
						spr->ikey = sp->priority;
						spr->info = a;
						spr++;
						maxspr++;
					}
				}
				else {
					spr->info = a;
					spr++;
					maxspr++;
				}
			}
		}
		sp--;
		a--;
	}

	if (mode & ESDRAW_NOSORT) priselect = false;

	if (priselect) {
		std::sort(selspr, selspr + maxspr, less_int_1);
	}

	// for normal draw
	spr = selspr;
	for (i = 0; i < maxspr; i++) {
		sp = getObj(spr->info);
		if (sp->fl) {
			if (mode_p & ESDRAW_NOMOVE) {
				drawSubMove(sp, mode_p);
			}
			if (sp->fl) {
				if (mode_p & ESDRAW_NOCALLBACK) {
					if (sp->sbr) {
						ctx->iparam = spr->info;
						ctx->wparam = sp->type;
						ctx->lparam = sp->chr;
						code_callback(sp->sbr);
					}
				}
			}
			if (sp->fl) {
				drawSubPut(sp, mode_p);
			}
		}
		spr++;
	}

	delete [] selspr;
	return maxspr;
}


int essprite::getSpriteAttrHit(int xx, int yy, int xsize, int ysize, bool plane)
{
	//		hit cjeck to all sprites ( for ESSPMAPHIT_BGOBJ )
	//		plane=trueの場合はスプライトを板の平面(ysize=1)として扱う
	//
	SPOBJ* sp;
	int i;
	int xpos, ypos, xr, yr;

	sp = getObj(0);
	for (i = 0; i < spkaz; i++) {
		if (sp->fl) {
			if (sp->maphit & ESSPMAPHIT_BGOBJ) {
				bool chk = false;
				if (sp->chr >= 0) {
					CHRREF* chr = getChr(sp->chr);
					if (chr) {
						xr = chr->bsx;
						if (plane) {
							yr = 1;
						}
						else {
							yr = chr->bsy;
						}
						int fl = sp->fl;
						if (!(fl & ESSPFLAG_NODISP)) {
							chk = true;
						}
					}
				}
				if (chk) {
					xpos = sp->xx >> dotshift;
					ypos = sp->yy >> dotshift;
					if ((xx < (xpos + xr)) && ((xx + xsize) >= xpos)) {
						if ((yy < (ypos + yr)) && ((yy + ysize) >= ypos)) {
							return i;
						}
					}
				}
			}
		}
		sp++;
	}
	return -1;
}


int essprite::getParameter(int spno, int prmid, int* value)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	int* prm = (int *)sp;
	if (prmid < 0) return -1;
	if (prmid >= (sizeof(SPOBJ) / 4)) return -1;
	*value = prm[prmid];
	return 0;
}


int essprite::setParameter(int spno, int prmid, int value, int op)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	int* prm = (int*)sp;
	if (prmid < 0) return -1;
	if (prmid >= (sizeof(SPOBJ)/4)) return -1;
	switch (op) {
	case 1:
		prm[prmid] |= value;
		break;
	case 2:
		prm[prmid] &= ~value;
		break;
	default:
		prm[prmid] = value;
		break;
	}
	return 0;
}


int essprite::setSpriteFlag(int spno, int flag, int op)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	int prev = sp->fl;
	switch (op) {
	case 1:
		sp->fl |= flag;
		break;
	case 2:
		sp->fl &= ~flag;
		break;
	default:
		sp->fl = flag;
		break;
	}
	return prev;
}


int essprite::setSpriteType(int spno, int type)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	sp->type = type;
	return spno;
}


int essprite::setSpritePos(int spno, int xx, int yy, int option)
{
	int opt, dotmask;
	int x, y;
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;

	bool biton = ( option & ESSPSET_MASKBIT ) != 0;
	if (option & ESSPSET_DIRECT) {
		x = xx;
		y = yy;
	} else {
		x = xx << dotshift;
		y = yy << dotshift;
		dotmask = dotshift_base - 1;
	}
	opt = option & (ESSPSET_DIRECT - 1);
	switch (opt) {
	case ESSPSET_POS:
		if (biton) {
			x |= sp->xx & dotmask;
			y |= sp->yy & dotmask;
		}
		sp->xx = x;
		sp->yy = y;
		break;
	case ESSPSET_ADDPOS:
		if (biton) {
			x |= sp->px & dotmask;
			y |= sp->py & dotmask;
		}
		sp->px = x;
		sp->py = y;
		break;
	case ESSPSET_FALL:
		if (biton) {
			x |= sp->fspx & dotmask;
			y |= sp->fspy & dotmask;
		}
		sp->fspx = x;
		sp->fspy = y;
		break;
	case ESSPSET_BOUNCE:
		if (biton) {
			x |= sp->bound & dotmask;
			y |= sp->boundflag & dotmask;
		}
		sp->bound = x;
		sp->boundflag = y;
		break;
	case ESSPSET_ZOOM:
		if (biton) {
			x |= sp->zoomx & dotmask;
			y |= sp->zoomy & dotmask;
		}
		sp->zoomx = x;
		sp->zoomy = y;
		break;
	case ESSPSET_ADDPOS2:
		sp->px += x;
		sp->py += y;
		break;
	case ESSPSET_POS2:
		sp->xx += x;
		sp->yy += y;
		break;
	default:
		return -1;
	}

	return spno;
}


void essprite::getSpriteAxis(SPOBJ* sp, int& xx, int& yy)
{
	int x, y;
	x = sp->xx;
	y = sp->yy;
/*
	if (sp->spstick >= 0) {
		SPOBJ* tsp = getObj(sp->spstick);
		if (tsp) {
			x = tsp->xx - sp->spst_x;
			y = tsp->yy - sp->spst_y;
		}
	}
*/
	xx = x;
	yy = y;
}


void essprite::getSpriteCenterAxis(SPOBJ* sp, int& xx, int& yy)
{
	int x, y;
	getSpriteAxis( sp, x, y );

	if (sp->chr >= 0) {
		CHRREF* chr = getChr(sp->chr);
		if (chr) {
			x += chr->bsx / 2;
			y += chr->bsy / 2;
		}
	}
	xx = x;
	yy = y;
}


int essprite::getSpritePos(int* xpos, int* ypos, int spno, int option)
{
	int x,y,opt;
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if (sp->fl == 0) return -1;

	opt = option & (ESSPSET_DIRECT - 1);
	switch (opt) {
	case ESSPSET_POS:
		x = sp->xx;
		y = sp->yy;
		break;
	case ESSPSET_ADDPOS:
		x = sp->px;
		y = sp->py;
		break;
	case ESSPSET_FALL:
		x = sp->fspx;
		y = sp->fspy;
		break;
	case ESSPSET_BOUNCE:
		x = sp->bound;
		y = sp->boundflag;
		break;
	case ESSPSET_ZOOM:
		x = sp->zoomx;
		y = sp->zoomy;
		break;
	case ESSPSET_ADDZOOM:
		x = sp->pzoomx;
		y = sp->pzoomy;
		break;
	case ESSPSET_CENTER:
		getSpriteCenterAxis(sp, x, y);
		break;
	case ESSPSET_PUTPOS:
		//getSpriteAxis(sp, x, y);
		getSpriteParentAxis(sp, x, y, 0);
		break;
	default:
		return -1;
	}
	if ((option & ESSPSET_DIRECT)==0) {
		x = x >> dotshift;
		y = y >> dotshift;
	}
	*xpos = x;
	*ypos = y;
	return 0;
}


int essprite::setSpriteAddPos(int spno, int xx, int yy, bool realaxis)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if ((sp->fl & ESSPFLAG_STATIC) == 0) return -1;
	if (realaxis) {
		sp->px = xx;
		sp->py = yy;
	}
	else {
		sp->px = xx << dotshift;
		sp->py = yy << dotshift;
	}
	sp->fl |= ESSPFLAG_MOVE;
	return spno;
}


int essprite::setSpriteAddPosRate(int spno, int xx, int yy, int rate)
{
	int aa = rate;
	int ax = (xx << dotshift) * aa / 100;
	int ay = (yy << dotshift) * aa / 100;
	return setSpriteAddPos(spno, ax, ay, true);
}


int essprite::setSpriteAddRotZoom(int spno, int rotz, int zoomx, int zoomy)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if ((sp->fl & ESSPFLAG_STATIC) == 0) return -1;
	sp->protz = rotz;

	int ax = (1 << dotshift) * zoomx / 100;
	int ay = (1 << dotshift) * zoomy / 100;
	sp->pzoomx = ax;
	sp->pzoomy = ay;

	if ((rotz==0) && (zoomx==0) && (zoomy==0)) {
		sp->fl &= ~ESSPFLAG_MOVEROT;
		return spno;
	}
	sp->fl |= ESSPFLAG_MOVEROT;
	return spno;
}


int essprite::setSpriteChr(int spno, int chrno)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;

	chr = getChr(chrno);
	if (chr == NULL) return -1;
	sp->chr = chrno;
	sp->ani = chr->lktime;
	sp->tpflag = chr->tpflag;
	return spno;
}


int essprite::setSpriteAddDir(int spno, int direction, int dirrate)
{
	int ax, ay, aa, adir, rot;

	aa = dirrate;
	rot = direction;
	while (1) {
		if ( rot >= 0 ) break;
		rot += rrate;
	}
	adir = rot % rrate;
	ax = (vpx[adir] << (dotshift - 10)) * aa / 100;
	ay = (vpy[adir] << (dotshift - 10)) * aa / 100;

	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if ((sp->fl & ESSPFLAG_STATIC) == 0) return -1;

	sp->px = ax;
	sp->py = ay;
	sp->fl |= ESSPFLAG_MOVE;
	return spno;
}


int essprite::setSpriteAim(int spno, int xx, int yy, int dirrate)
{
	int lx, ly, x, y;
	int ang;
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if (sp->fl == 0) return -1;

	lx = sp->xx; ly = sp->yy;
	x = xx << dotshift; y = yy << dotshift;
	ang = (int)((atan2((double)(lx - x), (double)(ly - y)) + pi + (3.141592 / rrate)) / pans + rrate) % rrate;
	setSpriteAddDir( spno, ang, dirrate );
	return spno;
}


int essprite::setSpriteParent(int spno, int parent, int option)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	if (sp->fl == 0) return -1;

	if (parent < 0) {
		sp->fl &= ~ESSPFLAG_SPLINK;
		sp->splink = 0;
		return 0;
	}

	switch (option) {
	case 0:
		sp->splink = parent;
		break;
	case 1:
		sp->splink = parent | ESSPLINK_BGMAP;
		break;
	default:
		return -1;
	}
	sp->fl |= ESSPFLAG_SPLINK;
	return 0;
}


SPOBJ *essprite::resetSprite(int spno)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return NULL;

	sp->fl = ESSPFLAG_STATIC;
	sp->type = 1;
	sp->px = 0; sp->py = 0;
	sp->progress = 0;
	sp->fspx = def_fspx;
	sp->fspy = def_fspy;
	sp->bound = def_bound;
	sp->boundflag = def_boundflag;
	sp->option = 0;
	sp->priority = 0;
	sp->tpflag = 0;
	sp->fadeprm = 0;
	sp->zoomx = dotshift_base;
	sp->zoomy = dotshift_base;
	sp->rotz = 0;
	sp->timer = 0;
	sp->timer_base = 0;
	sp->sbr = NULL;
	sp->life = 100;
	sp->lifemax = 100;
	sp->power = 1;

	if ( def_maplink >= 0 ) {
		sp->splink = def_maplink | ESSPLINK_BGMAP;
		sp->fl |= ESSPFLAG_SPLINK;
	}
	else {
		sp->splink = 0;
	}
	sp->maphit = def_maphitflag;

	sp->xx = 0;
	sp->yy = 0;
	sp->chr = -1;
	sp->ani = 0;
	sp->protz = 0;
	sp->pzoomx = 0;
	sp->pzoomy = 0;
	sp->moveres = 0;

	sp->spstick = -1;
	sp->spst_x = 0;
	sp->spst_y = 0;

	sp->xrevchr = 0;
	sp->yrevchr = 0;
	sp->mulcolor = -1;

	return sp;
}


int essprite::setSpritePosChr(int def_spno, int xx, int yy, int chrno, int option, int pri)
{
	//		sprite set
	//
	int spno = def_spno;
	if (spno < 0) { spno = getEmptySpriteNo(); }
	if (spno < 0) return -1;

	SPOBJ *sp = resetSprite(spno);
	if (sp == NULL) return -1;

	sp->option = option;
	sp->priority = pri;

	setSpritePos(spno, xx, yy);
	setSpriteChr(spno, chrno);

	return spno;
}


int essprite::registSpriteDecoration(int chr, int opt, int direction, int speed, int life, int entry)
{
	//		decoration sprite regist
	//
	int res = -1;
	int id = entry;

	SPDECOINFO deco;
	deco.chr = chr;
	deco.option = opt;
	deco.direction = direction;
	deco.speed = speed;
	deco.life = life;
	deco.rotp = 0;
	deco.zoomxp = 0;
	deco.zoomyp = 0;

	if (opt & ESDECO_ZOOM) {
		deco.zoomxp = 0x200;
		deco.zoomyp = 0x200;
	}
	if (opt & ESDECO_ROTATE) {
		deco.rotp = 2;
	}

	res = (int)mem_deco.size();
	if (res == 0) id = -1;
	if (id >= 0) {
		if (id < res) {
			mem_deco[id] = deco;
			return id;
		}
	}
	mem_deco.push_back(deco);
	return res;
}


int essprite::setSpriteDecoration(int x, int y, int decoid)
{
	//		decoration sprite set
	//
	int multi = 1;
	int res;

	if ((decoid < 0) || (decoid >= (int)mem_deco.size())) return -1;

	SPDECOINFO* info = &mem_deco.at(decoid);

	int sw = info->option;

	if (sw & ESDECO_MULTI4) multi = 4;
	if (sw & ESDECO_MULTI8) multi = 8;
	if (sw & ESDECO_MULTI16) multi = 16;

	int dir = info->direction;
	int diradd = 0;
	if (multi > 1) {
		if (dir >= 0) {
			diradd = rrate / multi;
		}
	}

	while (1) {
		if (multi <= 0) break;
		res = setSpriteDecorationSub( x, y, dir, info );
		multi--;
		dir += diradd;
	}
	return res;
}


int essprite::setSpriteDecorationSub(int x, int y, int direction, SPDECOINFO* info)
{
	int spno = getEmptySpriteNo();
	if (spno < 0) return -1;

	SPOBJ* sp = resetSprite(spno);
	if (sp == NULL) return -1;

	int chrno = info->chr;
	int sw = info->option;

	if (sw & ESDECO_CHR2) {
		chrno += rand() % 2;
	}
	if (sw & ESDECO_CHR4) {
		chrno += rand() % 4;
	}
	setSpritePos(spno, x, y);
	setSpriteChr(spno, chrno);

	if (sw & ESDECO_FRONT) {
		sp->fl |= ESSPFLAG_DECORATE;
		sp->priority = 100;
	}
	if ((sw & ESDECO_MAPHIT)==0) {
		sp->maphit = 0;
	}
	if ((sw & ESDECO_GRAVITY) == 0) {
		sp->fspx = 0;
		sp->fspy = 0;
	}

	int dir, spd;
	if (direction >= 0) {
		dir = direction;
	}
	else {
		dir = rand() % rrate;
	}
	if (info->speed >= 0) {
		spd = info->speed;
	}
	else {
		spd = (rand() % 50) + 50;
		if (sw & ESDECO_BOOST) {
			spd *= 2;
		}
	}
	setSpriteAddDir(spno, dir, spd);
	if (info->life >= 0) {
		if (sw & ESDECO_FADEOUT) {
			setSpriteFade(spno, ESSPF_EFADEWIPE, info->life);
		}
		else {
			setSpriteFade(spno, ESSPF_TIMEWIPE, info->life);
		}
	}
	if (sw & ESDECO_SCATTER) {
		sp->rotz = rand() % rrate;
	}

	if (info->rotp) {
		sp->protz = info->rotp;
		sp->fl |= ESSPFLAG_MOVEROT;
	}
	if (sw & ESDECO_ZOOM) {
		sp->pzoomx = info->zoomxp;
		sp->pzoomy = info->zoomyp;
		sp->fl |= ESSPFLAG_MOVEROT;
	}
	if (sw & ESDECO_EPADD) {
		sp->tpflag = 0x5ff;
	}
	return spno;
}


void essprite::setOffset(int p1, int p2)
{
	ofsx = p1; ofsy = p2;
}


int essprite::setGravity(int p1, int p2, int p3)
{
	if (p1 < 0) {
		def_fspx = p2;
		def_fspy = p3;
		return 0;
	}

	SPOBJ *sp = getObj(p1);
	if (sp == NULL) return -1;

	if (p2 > 32767) p2 = 32767;
	if (p2 < -32767) p2 = -32767;
	if (p3 > 32767) p3 = 32767;
	if (p3 < -32767) p3 = -32767;
	sp->fspx = p2;
	sp->fspy = p3;
	return 0;
}


int essprite::setBound(int p1, int p2, int p3)
{
	if (p1 < 0) {
		def_bound = p2;
		def_boundflag = p3 & 3;
		return 0;
	}

	SPOBJ *sp = getObj(p1);
	if (sp == NULL) return -1;

	if (p2 > 0x100) p2 = 0x100;
	if (p2 < 0) p2 = 0;
	if (p3 < 0) p3 = 0;
	//if (p3 > 3) p3 = 3;

	sp->bound = p2;
	sp->boundflag = p3 & 3;

	if ((p3 & 16) == 0) {
		int fl = sp->fl;
		fl &= ~(ESSPFLAG_XBOUNCE | ESSPFLAG_YBOUNCE);
		if (p3 & ESSPBOUND_BOUNDX) fl |= ESSPFLAG_XBOUNCE;
		if (p3 & ESSPBOUND_BOUNDY) fl |= ESSPFLAG_YBOUNCE;
		sp->fl = fl;
	}

	return 0;
}


void essprite::resetTimer( SPOBJ *sp )
{
	sp->fl = sp->fl & ~(ESSPFLAG_BLINK | ESSPFLAG_FADEIN | ESSPFLAG_FADEOUT | ESSPFLAG_TIMERWIPE | ESSPFLAG_BLINK2 | ESSPFLAG_EFADE | ESSPFLAG_EFADE2 );
}


int essprite::setSpriteFade(int id, int sw, int timer)
{
	int fl;
	int i;
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;

	i = timer;
	resetTimer(sp);
	fl = sp->fl;
	if ((fl & ESSPFLAG_STATIC) == 0) return -1;		// スプライトが存在しない

	if (sw & 1) {
		fl |= ESSPFLAG_TIMERWIPE;
	}

	switch (sw) {
	case ESSPF_TIMEWIPE:
		break;
	case ESSPF_FADEOUT:
	case ESSPF_FADEOUTWIPE:
		if (i > 0) {
			fl |= ESSPFLAG_FADEOUT;
			sp->timer_base = i;
			sp->fadeprm = sp->tpflag & 255;
		}
		break;
	case ESSPF_FADEIN:
	case ESSPF_FADEINWIPE:
		if (i > 0) {
			fl |= ESSPFLAG_FADEIN;
			sp->timer_base = i;
			sp->fadeprm = sp->tpflag & 255;
		}
		break;
	case ESSPF_BLINK:
	case ESSPF_BLINKWIPE:
		fl |= ESSPFLAG_BLINK;
		break;
	case ESSPF_BLINK2:
	case ESSPF_BLINKWIPE2:
		fl |= ESSPFLAG_BLINK2;
		break;
	case ESSPF_EFADE:
	case ESSPF_EFADEWIPE:
		fl |= ESSPFLAG_EFADE;
		sp->fadeprm = sp->tpflag & 255;
		break;
	case ESSPF_EFADE2:
	case ESSPF_EFADEWIPE2:
		fl |= ESSPFLAG_EFADE2;
		sp->fadeprm = sp->tpflag & 255;
		break;
	default:
		return -1;
	}
	sp->timer = i;
	sp->fl = fl;
	return 0;
}


int essprite::setSpriteEffect(int id, int tpflag, int mulcolor)
{
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;

	sp->tpflag = tpflag;
	sp->mulcolor = mulcolor;
	return 0;
}


int essprite::setSpriteRotate(int id, int angle, int zoomx, int zoomy, int rate)
{
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;
	if ((sp->fl & ESSPFLAG_STATIC) == 0) return -1;

	if (zoomx >= 0) {
		int ax = (1 << dotshift) * zoomx / 100;
		sp->zoomx = ax;
	}
	if (zoomy >= 0) {
		int ay = (1 << dotshift) * zoomy / 100;
		sp->zoomy = ay;
	}

	sp->rotz = angle;
	sp->fl &= ~ESSPFLAG_MOVEROT;

	return 0;
}


int essprite::setSpriteStick(SPOBJ* sp, int targetsp)
{
	if (sp == NULL) return -1;
	if (targetsp >= 0) {
		SPOBJ* tsp = getObj(targetsp);
		if (tsp == NULL) return -2;
		sp->spst_x = tsp->xx - sp->xx;
		sp->spst_y = tsp->yy - sp->yy;
		sp->spstick = targetsp;
	}
	else {
		if (sp->spstick < 0) return 0;
		int x, y;
		getSpriteAxis(sp, x, y);
		sp->spstick = -1;
		sp->xx = x;
		sp->yy = y;
	}
	return 0;
}


int essprite::setSpriteStick(int spno, int targetsp)
{
	SPOBJ* sp = getObj(spno);
	return setSpriteStick(sp, targetsp);
}


int essprite::setSpriteLife(int spno, int life, int option)
{
	int attack = 0;
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	switch (option) {
	case 0:
		sp->life = life;
		attack = 0;
		break;
	case 1:
		sp->power = life;
		return sp->life;
	case 2:
		sp->lifemax = life;
		if ( sp->lifemax < 0 ) sp->lifemax = 0;
		return sp->life;
	case 4:
		attack = life;
		break;
	case 5:
		{
		SPOBJ* sp2 = getObj(life);
		if (sp2 == NULL) return -1;
		attack = sp2->power;
		break;
		}
	default:
		break;
	}
	sp->life -= attack;
	if ( sp->life > sp->lifemax) sp->life = sp->lifemax;
	if ( sp->life < 0 ) sp->life = 0;
	return sp->life;
}


/*------------------------------------------------------------*/
/*
		BG Map Process
*/
/*------------------------------------------------------------*/


void essprite::initMap(void)
{
	int a;
	for (a = 0; a < mapkaz; a++) {
		mem_map[a].varptr = NULL;
		mem_map[a].maskptr = NULL;
		mem_map[a].attr = NULL;
		mem_map[a].mem_bghitinfo = NULL;
	}
}


void essprite::resetMap(void)
{
	int a;
	for (a = 0; a < mapkaz; a++) {
		deleteMap(a);
	}
}


void essprite::deleteMap(int id)
{
	if ((id < 0) || (id >= mapkaz)) return;
	mem_map[id].varptr = NULL;
	if (mem_map[id].mem_bghitinfo != NULL) {
		delete mem_map[id].mem_bghitinfo;
		mem_map[id].mem_bghitinfo = NULL;
	}
	deleteMapMask(id);
	deleteMapAttribute(id);
}


void essprite::deleteMapMask(int id)
{
	if ((id < 0) || (id >= mapkaz)) return;
	if (mem_map[id].maskptr != NULL) {
		free(mem_map[id].maskptr);
	}
	mem_map[id].maskptr = NULL;
}


void essprite::deleteMapAttribute(int id)
{
	if ((id < 0) || (id >= mapkaz)) return;
	if (mem_map[id].attr != NULL) {
		free(mem_map[id].attr);
	}
	mem_map[id].attr = NULL;
}


BGMAP* essprite::getMap(int id)
{
	if ((id < 0) || (id >= mapkaz)) return NULL;
	return &mem_map[id];
}


int essprite::setMap(int def_bgno, int* varptr, int mapsx, int mapsy, int sx, int sy, int buffer, int option)
{
	int bgno = def_bgno;
	if (bgno < 0) { bgno = 0; }
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	if ((mapsx < 1) || (mapsy < 1)) return -1;
	if ((sx < 1) || (sy < 1)) return -1;

	deleteMap(bgno);

	bg->varptr = varptr;
	bg->maskptr = NULL;
	bg->mapsx = mapsx;
	bg->mapsy = mapsy;
	bg->sizex = sx;
	bg->sizey = sy;
	bg->viewx = 0;
	bg->viewy = 0;
	bg->buferid = buffer;
	bg->bgoption = option;
	bg->tpflag = 0x3ff;
	bg->maphit_cnt = 0;
	bg->animation = 0;
	bg->group = -1;
	bg->notice = 0;
	bg->spwipe_chr = -1;
	bg->blockbit = 0;
	bg->blockbit_org = 0;
	bg->def_gravity = 0;

	Bmscr* bm = hspwnd->GetBmscrSafe(bg->buferid);
	if (bm == NULL) return -1;
	bg->divx = bm->divsx; bg->divy = bm->divsy;
	bg->sx = mapsx * bg->divx;
	bg->sy = mapsy * bg->divy;

	if (option & ESMAP_OPT_NOTRANS) {
		bg->tpflag = 0;
	}
	if (option & ESMAP_OPT_USEMASK) {
		updateMapMask(bgno);
	}

	bg->mem_bghitinfo = new std::vector<BGHITINFO>;

	return bgno;
}


void essprite::resetMapAttribute(BGMAP* map)
{
	if (map == NULL) return;
	if (map->attr != NULL) {
		free(map->attr);
		return;
	}
	map->attr = (unsigned short*)calloc(ESMAP_ATTR_MAX, 2);		// alloc map attriubte
}


int essprite::setMapPos(int bgno, int x, int y)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;
	if (bg->varptr == NULL) return -1;

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	bg->viewx = x;
	bg->viewy = y;

	return 0;
}


int essprite::setMapMes(int bgno, int x, int y, char* msg, int offset)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	int* mapsrc;
	int* p;
	char a1;
	char* st;
	int sx, sy, xx, yy;
	xx = x; yy = y;
	sx = bg->sizex;
	sy = bg->sizey;

	if (bg->varptr==NULL) return -1;
	mapsrc = bg->varptr + (bg->mapsx * yy ) + xx;
	p = mapsrc;

	st = msg;
	while (1) {
		if ((yy<0)||(yy >= bg->mapsy)) break;

		a1 = *st++;
		if (a1 == 0) break;
		if (a1 == 13) {
			if (*st == 10) st++;
			mapsrc += bg->mapsx;
			p = mapsrc;
			xx = x; yy++;
		}

		if (xx < bg->mapsx) {
			*p++ = ((int)a1)+offset;
		}
		xx++;
	}

	return 0;
}


int essprite::putMap(int xx, int yy, int bgno )
{
	//		putMAP
	//
	int x,y,i,j,sx,sy,vx,vy,vpx,vpy,ofsx,ofsy,divx,divy;
	int* mapsrc;
	int* p;
	int celno;
	unsigned short* p_attr;
	int attr;
	int group;
	int num_event;
	bool transflag = true;
	bool selgroup = false;
	bool selgroupbit = false;

	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	Bmscr* bm = hspwnd->GetBmscrSafe(bg->buferid);
	if (bm == NULL) return -1;
	divx = bg->divx; divy = bg->divy;

	mapsrc = bg->varptr;
	if (mapsrc == NULL) return -1;

	sx = bg->sizex;
	sy = bg->sizey;
	vx = bg->viewx;
	vy = bg->viewy;
	if (vx < 0) vx = 0;
	if (vy < 0) vy = 0;
	vpx = vx % divx;
	vpy = vy % divy;
	if (vpx > 0) sx++;
	if (vpy > 0) sy++;
	x = xx - vpx;
	y = yy - vpy;

	p_attr = bg->attr;
	group = bg->group;
	if (group >= 0) {
		if (group & 16) {
			group = group & 15;
			selgroupbit = true;
		}
		else {
			selgroup = true;
		}
	}
	setTransparentMode(bg->tpflag);
	if ((bg->tpflag >> 8) == 0) transflag = false;

	num_event = 0;
	ofsy = vy / divy;
	for (j = 0; j < sy;j++) {
		bmscr->cx = x;
		bmscr->cy = y;
		p = mapsrc;
		p += bg->mapsx * (ofsy % bg->mapsy);
		ofsx = vx/divx;
		for (i = 0; i < sx;i++) {
			celno = ( p[ofsx % bg->mapsx] ) & 0xffff;
			if (celno == 0) {
				if (transflag) {					// cel=0はスキップ
					bmscr->cx += divx;
					ofsx++;
					continue;
				}
			}
			if (p_attr) {
				attr = (int)p_attr[celno];
				if (selgroup) {
					if (group != (attr & 15)) {		// 指定グループのみ表示する
						bmscr->cx += divx;
						ofsx++;
						continue;
					}
				}
				if (selgroupbit) {
					if ((group & attr)==0) {		// 指定グループビットのみ表示する
						bmscr->cx += divx;
						ofsx++;
						continue;
					}
				}
				if (attr & ESMAP_ATTR_EVENT) {
					if ((attr & ESMAP_ATTR_HOLD)==0) num_event++;
				}
				if (attr & ESMAP_ATTR_ANIM) celno += bg->animation;
			}
			bmscr->CelPut(bm, celno);
			ofsx++;
		}
		y+=bm->divsy;
		ofsy++;
	}
	return num_event;
}


int essprite::fetchMap(int bgno, int dir, int size, int evtype)
{
	//		fetch MAP
	//		(0=down,1=right,2=up,3=left,4=all)
	//
	int i, j, xx, yy, sx, sy, vx, vy, vpx, vpy, ofsx, ofsy, divx, divy;
	int lx, ly;
	int* mapsrc;
	int* p;
	int celno, mapval;
	unsigned short* p_attr;
	int attr;
	int group;
	bool transflag = true;
	bool selgroup = false;
	bool selgroupbit = false;

	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	Bmscr* bm = hspwnd->GetBmscrSafe(bg->buferid);
	if (bm == NULL) return -1;
	divx = bg->divx; divy = bg->divy;

	mapsrc = bg->varptr;
	if (mapsrc == NULL) return -1;

	sx = bg->sizex;
	sy = bg->sizey;
	vx = bg->viewx;
	vy = bg->viewy;
	if (vx < 0) vx = 0;
	if (vy < 0) vy = 0;
	vpx = vx % divx;
	vpy = vy % divy;
	if (vpx > 0) sx++;
	if (vpy > 0) sy++;

	lx = sx; ly = sy;
	vx = vx / divx;
	vy = vy / divy;


	switch (dir) {
	case 0:
		vy += sy;
		ly = size;
		break;
	case 1:
		vx += sx;
		lx = size;
		break;
	case 2:
		vy -= size;
		ly = size;
		if (vy < 0) {
			ly += vy;
			vy = 0;
		}
		break;
	case 3:
		vx -= size;
		lx = size;
		if (vx < 0) {
			lx += vx;
			vx = 0;
		}
		break;
	case 4:
		break;
	default:
		return -2;
	}
	xx = vx * divx;
	yy = vy * divy;

	if ((dir == 0) || (dir == 2)) {
		if (xx > 0) {
			xx--;
			lx += 2;
		}
	}
	if ((dir == 1) || (dir == 3)) {
		if (yy > 0) {
			yy--;
			ly += 2;
		}
	}

	p_attr = bg->attr;
	if (p_attr == NULL) return -3;
	group = bg->group;
	if (group >= 0) {
		if (group & 16) {
			group = group & 15;
			selgroupbit = true;
		}
		else {
			selgroup = true;
		}
	}

	resetMapHitInfo(bgno);

	ofsy = vy;
	for (j = 0; j < ly; j++) {
		p = mapsrc;
		p += bg->mapsx * (ofsy % bg->mapsy);
		ofsx = vx;
		for (i = 0; i < lx; i++) {
			mapval = (p[ofsx % bg->mapsx]);
			celno = mapval & 0xffff;
			if (celno == 0) {
				if (transflag) {					// cel=0はスキップ
					ofsx++;
					continue;
				}
			}
			if (p_attr) {
				attr = (int)p_attr[celno];
				if (selgroup) {
					if (group != (attr & 15)) {		// 指定グループのみ表示する
						ofsx++;
						continue;
					}
				}
				if (selgroupbit) {
					if ((group & attr) == 0) {		// 指定グループビットのみ表示する
						ofsx++;
						continue;
					}
				}
				if (attr & ESMAP_ATTR_NOTICE) {
					if ((mapval & ESMAP_CEL_IGNORE) == 0) {
						addMapHitInfo(bgno, ESMAPHIT_NOTICE, celno, attr, ofsx, ofsy, ofsx * divx, ofsy * divy);	// NOTICE
						if (bg->bgoption & ESMAP_OPT_WIPENOTICE) {
							p[ofsx % bg->mapsx] |= ESMAP_CEL_IGNORE;
						}
					}
				}
				if (bg->bgoption & ESMAP_OPT_GETEVENT) {
					if (attr & ESMAP_ATTR_EVENT) {
						if ((attr & ESMAP_ATTR_HOLD)==0) {
							addMapHitInfo(bgno, ESMAPHIT_EVENT, celno, attr, ofsx, ofsy, ofsx * divx, ofsy * divy);	// NOTICE
							if (bg->bgoption & ESMAP_OPT_WIPEEVENT) {
								celno = 0;
								p[ofsx % bg->mapsx] = 0;
							}
						}
					}
				}
			}
			ofsx++;
		}
		ofsy++;
	}
	return bg->maphit_cnt;
}


int essprite::setMapParam(int bgno, int tp, int option)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	switch (option) {
	case ESMAP_PRM_GMODE:
		bg->tpflag = tp;
		break;
	case ESMAP_PRM_ANIM:
		bg->animation = tp;
		break;
	case ESMAP_PRM_GROUP:
		bg->group = tp;
		break;
	case ESMAP_PRM_NOTICE:
		bg->notice = tp;
		break;
	case ESMAP_PRM_OPTION:
		bg->bgoption = tp;
		break;
	case ESMAP_PRM_WIPEDECO:
		bg->spwipe_chr = tp;
		break;
	case ESMAP_PRM_BLOCKBIT:
		bg->blockbit_org = tp;
		break;
	case ESMAP_PRM_GRAVITY:
		bg->def_gravity = tp;
	default:
		return -1;
	}
	return 0;
}


int essprite::setMapAttribute(int bgno, int start, int end, int attribute)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;
	if (bg->attr == NULL) {
		resetMapAttribute(bg);
	}
	int i = start;
	while (1) {
		if ((i < 0) || (i >= ESMAP_ATTR_MAX)) return -1;
		if (i > end) break;
		bg->attr[i] = (unsigned short)attribute;
		i++;
	}
	return 0;
}


int essprite::getMapAttribute(int bgno, int celid)
{
	unsigned short a1;
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;
	if ((celid<0)||(celid>=ESMAP_ATTR_MAX)) return -1;
	if (bg->attr == NULL) return 0;
	a1 = bg->attr[celid];
	return (int)a1;
}


int essprite::updateMapMask(int bgno)
{
	return 0;
}


void essprite::resetMapHitInfo(int bgno)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return;

	bg->mem_bghitinfo->clear();
	bg->maphit_cnt = 0;

	bak_hitmapx = -1;
	bak_hitmapy = -1;
}

BGHITINFO* essprite::addMapHitInfo(int bgno, int result, int celid, int attr, int myx, int myy, int xx, int yy)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return NULL;

	BGHITINFO data;
	data.result = result;
	data.celid = celid;
	data.attr = attr;
	data.myx = myx;
	data.myy = myy;
	data.x = xx;
	data.y = yy;
	bg->mem_bghitinfo->push_back(data);
	BGHITINFO* info = &bg->mem_bghitinfo->at(bg->maphit_cnt);
	if (info == NULL) {
		return NULL;
	}
	bg->maphit_cnt++;
	return info;
}


BGHITINFO* essprite::getMapHitInfo(int bgno, int index)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return NULL;

	if ((index < 0) || (index >= (int)bg->mem_bghitinfo->size())) return NULL;
	BGHITINFO* info = &bg->mem_bghitinfo->at(index);
	return info;
}


int essprite::getMapMask(BGMAP* map, int x, int y)
{
	unsigned short* attr = map->attr;
	if (attr == NULL) return 0;

	if ((x < 0) || (x >= map->sx)) return 0;
	if ((y < 0) || (y >= map->sy)) return 0;

	int* p = map->varptr;
	hitmapx = (x / map->divx);
	hitmapy = (y / map->divy);
	hitcelid = p[ hitmapy * map->mapsx + hitmapx ] & 0xffff;
	hitattr_org = (int)attr[hitcelid];
	hitattr = hitattr_org & 0xff;

	if (hitattr_org & map->blockbit) {		// ブロックビットと一致していれば壁と認識する
		hitattr |= ESMAP_ATTR_WALL;
		return ESMAPHIT_HIT;
	}

	if (hitattr >= ESMAP_ATTR_WALL) {
		return ESMAPHIT_HIT;
	}
	if (hitattr >= ESMAP_ATTR_EVENT) return ESMAPHIT_EVENT;
	return ESMAPHIT_NONE;
}


int essprite::setSpriteMapLink(int bgno, int hitoption)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	def_maplink = bgno;
	def_maphitflag = hitoption;

	return 0;
}


int essprite::getMapMaskHitSub(int bgno, int x, int y, int sizex, int sizey, bool wallonly, bool downdir)
{
	int i, xx, yy;
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return 0;

	if (sizex == 1) {							// 横方向チェック
		for (yy = 0; yy < sizey; yy++) {
			i = getMapMask(bg, x, y + yy);
			if (i) {
				if (wallonly) {
					if (i == ESMAPHIT_HIT) return ESMAPHIT_HIT;
				}
				else {
					if (hitattr < ESMAP_ATTR_HOLD) {
						if ((bak_hitmapx != hitmapx) || (bak_hitmapy != hitmapy)) {
							addMapHitInfo(bgno, ESMAPHIT_EVENT, hitcelid, hitattr_org, hitmapx, hitmapy, x, y + yy);	// ITEM
							bak_hitmapx = hitmapx; bak_hitmapy = hitmapy;
						}
					}
				}
			}
		}
		return ESMAPHIT_NONE;
	}
	if (sizey == 1) {							// 縦方向チェック
		for (xx = 0; xx < sizex; xx++) {
			i = getMapMask(bg, x + xx, y);
			if (i) {
				if (wallonly) {
					if (downdir) {		// 下方向のみ足場をヒットとする
						int yadj = (y % bg->divy);
						if (yadj == 0) {				// 上部1ドットの平面だけを当たり領域とする
							if (hitattr >= ESMAP_ATTR_HOLD) return ESMAPHIT_HIT;
						}
						else {
							if (i == ESMAPHIT_HIT) return ESMAPHIT_HIT;
						}
					}
					else {
						if (i == ESMAPHIT_HIT) return ESMAPHIT_HIT;
					}
				}
				else {
					if (hitattr < ESMAP_ATTR_HOLD) {
						if ((bak_hitmapx != hitmapx) || (bak_hitmapy != hitmapy)) {
							addMapHitInfo(bgno, ESMAPHIT_EVENT, hitcelid, hitattr_org, hitmapx, hitmapy, x+xx, y );	// ITEM
							bak_hitmapx = hitmapx; bak_hitmapy = hitmapy;
						}
					}
				}
			}
		}
		if (downdir) {
			i = getSpriteAttrHit(x, y, sizex, 1, false);
			if (i>=0) {
				SPOBJ* sp = getObj(i);
				if (sp) {
					addMapHitInfo(bgno, ESMAPHIT_SPHIT, i, 0, sp->xx >> dotshift, sp->yy >> dotshift, x, y);	// SPRITE
					return ESMAPHIT_HIT;		// スプライトの足場をヒットとする
				}
			}
		}

		return ESMAPHIT_NONE;
	}
	return ESMAPHIT_NONE;
}


int essprite::getMapMaskHit(int bgno, int x, int y, int sizex, int sizey, int px, int py)
{
	//		(x,y)から(sx,sy)サイズの領域をマップパーツと接触判定する
	//		移動量を(px,py)に入れる、結果はBGHITINFOに返る
	//
	int res, xx, yy, orgx, orgy;
	int left;
	int curadd;

	orgx = x; orgy = y;
	xx = x; yy = y;

	curadd = 0;

	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return 0;

	resetMapHitInfo(bgno);

	res = 0;

	if (px != 0) {
		if (px > 0) {
			left = px;
			xx = x + sizex;
			curadd = 1;
		}
		if (px < 0) {
			left = -px;
			xx = x - 1;
			curadd = -1;
		}
		while (1) {
			if (left == 0) break;
			left--;
			res = getMapMaskHitSub(bgno, xx, yy, 1, sizey, true, false);
			if (res) {
				addMapHitInfo(bgno, ESMAPHIT_HITX, hitcelid, hitattr_org, hitmapx, hitmapy, orgx, orgy);
				break;
			}
			getMapMaskHitSub(bgno, xx, yy, 1, sizey);				// EVENTを調べる
			xx += curadd;
			orgx += curadd;
		}
	}

	xx = orgx; yy = orgy;
	if (py != 0) {
		if (py > 0) {
			left = py;
			yy = yy + sizey;
			curadd = 1;
		}
		if (py < 0) {
			left = -py;
			yy = yy - 1;
			curadd = -1;
		}
		while (1) {
			if (left == 0) break;
			left--;
			res = getMapMaskHitSub(bgno, xx, yy, sizex, 1, true, curadd==1);
			if (res) {
				addMapHitInfo(bgno, ESMAPHIT_HITY, hitcelid, hitattr_org, hitmapx, hitmapy, orgx, orgy);
				break;
			}
			getMapMaskHitSub(bgno, xx, yy, sizex, 1);				// EVENTを調べる
			yy += curadd;
			orgy += curadd;
		}
	}

	addMapHitInfo(bgno, ESMAPHIT_NONE, 0, 0, 0, 0, orgx, orgy);
	return bg->maphit_cnt;
}


int essprite::getMapMaskHitSprite(int bgno, int spno, int px, int py)
{
	//		スプライト座標からマップパーツと接触判定する(32bit座標)
	//		移動量を(px,py)に入れる、結果はBGHITINFOに返る
	//
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return 0;
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return 0;
	CHRREF* chr = getChr(sp->chr);
	if (chr == NULL) return 0;

	int xx, yy;
	xx = sp->xx;
	yy = sp->yy;
	return getMapMaskHit(bgno, xx+ chr->colx, yy+ chr->coly, chr->colsx, chr->colsy, px, py);
}


int essprite::getMapAttrFromPos(int bgno, int x, int y)
{
	//		(x,y)位置のマップアトリビュート値を得る
	//
	BGMAP* map = getMap(bgno);
	if (map == NULL) return 0;

	unsigned short* attr = map->attr;
	if (attr == NULL) return 0;

	if ((x < 0) || (x >= map->sx)) return 0;
	if ((y < 0) || (y >= map->sy)) return 0;

	int* p = map->varptr;
	hitmapx = (x / map->divx);
	hitmapy = (y / map->divy);
	hitcelid = p[hitmapy * map->mapsx + hitmapx] & 0xffff;
	hitattr_org = (int)attr[hitcelid];
	return hitattr_org;
}


int essprite::getMapMaskHit32(int bgno, int x, int y, int p_sizex, int p_sizey, int px, int py)
{
	//		(x,y)から(sx,sy)サイズの領域をマップパーツと接触判定する(32bit座標)
	//		移動量を(px,py)に入れる、結果はBGHITINFOに返る
	//
	int res, xx, yy, orgx, orgy;
	int left, modmax;
	int curadd;
	int sizex, sizey;

	orgx = x; orgy = y;
	xx = x; yy = y;
	sizex = p_sizex << dotshift;
	sizey = p_sizey << dotshift;

	curadd = 0;

	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return 0;

	resetMapHitInfo(bgno);

	modmax = xx + px;
	left = (modmax >> dotshift) - (xx >> dotshift);
	//left = px >> dotshift;

	res = 0;
	if (left != 0) {
		if (left > 0) {
			xx = x + sizex;
			curadd = 0x10000;
		} else {
			left = -left;
			xx = x - 0x10000;
			curadd = -0x10000;
		}
		while (1) {
			if (left == 0) break;
			left--;
			res = getMapMaskHitSub(bgno, xx >> dotshift, yy >> dotshift, 1, p_sizey, true, false);
			if (res) {
				addMapHitInfo(bgno, ESMAPHIT_HITX, hitcelid, hitattr_org, hitmapx, hitmapy, orgx, orgy);
				break;
			}
			getMapMaskHitSub(bgno, xx >> dotshift, yy >> dotshift, 1, p_sizey);				// EVENTを調べる
			xx += curadd;
			orgx += curadd;
			if (curadd > 0) {
				if (orgx > modmax) orgx = modmax;
			}
			else {
				if (orgx < modmax) orgx = modmax;
			}
		}
	}
	else {
		orgx = modmax;
	}

	xx = orgx; yy = orgy;
	modmax = yy + py;
	left = (modmax >> dotshift) - (yy >> dotshift);
	//left = py >> dotshift;

	if (left != 0) {
		if (left > 0) {
			yy = yy + sizey;
			curadd = 0x10000;
		} else {
			left = -left;
			yy = yy - 0x10000;
			curadd = -0x10000;
		}
		while (1) {
			if (left == 0) break;
			left--;
			res = getMapMaskHitSub(bgno, xx >> dotshift, yy >> dotshift, p_sizex, 1, true, curadd == 0x10000);
			if (res) {
				addMapHitInfo(bgno, ESMAPHIT_HITY, hitcelid, hitattr_org, hitmapx, hitmapy, orgx, orgy);
				break;
			}
			getMapMaskHitSub(bgno, xx >> dotshift, yy >> dotshift, p_sizex, 1);				// EVENTを調べる
			yy += curadd;
			orgy += curadd;
		}
		if (curadd > 0) {
			if (orgy > modmax) orgy = modmax;
		}
		else {
			if (orgy < modmax) orgy = modmax;
		}
	}
	else {
		orgy = modmax;
	}
	addMapHitInfo(bgno, ESMAPHIT_NONE, 0, 0, 0, 0, orgx, orgy);
	return bg->maphit_cnt;
}


/*------------------------------------------------------------*/
/*
		utility
*/
/*------------------------------------------------------------*/

int essprite::utilGetAngle(int p1, int p2, int p3, int p4)
{
	int ang = ((int)((atan2((double)(p1 - p3), (double)(p2 - p4)) + pi + (3.141592 / rrate)) / pans + rrate) % rrate);
	return ang;
}


int essprite::utilGetSin(int p1)
{
	return vpy[(p1 + rrate) % rrate];
}


int essprite::utilGetCos(int p1)
{
	return vpx[(p1 + rrate) % rrate];
}


int essprite::utilGetDistance(int x1, int y1, int x2, int y2)
{
	double px, py, dist;
	px = (double)abs(x2 - x1);
	py = (double)abs(y2 - y1);
	dist = sqrt(px * px + py * py);
	return (int)(dist * 256.0);
}


double essprite::utilGetDistanceFloat(int x1, int y1, int x2, int y2)
{
	double px, py, dist;
	px = (double)abs(x2 - x1);
	py = (double)abs(y2 - y1);
	dist = sqrt(px * px + py * py);
	return dist;
}


