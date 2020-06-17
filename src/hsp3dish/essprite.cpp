
//
//		HPDX Compatible Sprite access (essprite)
//			onion software/onitama 2020/3
//

#if defined(HSPWIN) || defined(HSPLINUX) || defined(HSPEMSCRIPTEN)
#include "../hsp3/hsp3config.h"
#else
#include "hsp3config.h"
#endif

#ifdef HSPWIN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "essprite.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		initalizer
*/
/*------------------------------------------------------------*/

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
	}
	for (a = 0; a < mapkaz; a++) {
		mem_map[a].varptr = NULL;
	}

	def_fspx = 0;
	def_fspy = 0x100;
	def_bound = 128;
	def_boundflag = 3;

	setOffset(0, 0);
	sprite_enable = true;
	return 0;
}

void essprite::reset(void)
{
	//	Memory release
	//
	sprite_enable = false;

	if (mem_map != NULL) {
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


void essprite::setResolution(HspWnd* wnd, int sx, int sy)
{
	hspwnd = wnd;
	bmscr = wnd->GetBmscr(0);
	main_sx = sx;
	main_sy = sy;

	setArea(-128, -128, (sx + 128), (sy + 128));
	land_x = sx;
	land_y = sy;
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
	df_bsx = p1; df_bsy = p2;
	bsx = df_bsx; bsy = df_bsy;
	nx = bsx *rate / 100;
	ny = bsy *rate / 100;
	df_colx = (bsx - nx) / 2;
	df_coly = (bsy - ny) / 2;
	df_colsx = nx;
	df_colsy = ny;
	df_tpflag = p4;
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


int essprite::put(int xx, int yy, int chrno, int tpflag, int zoomx, int zoomy, int rotz)
{
	//		sprite put
	//
	bool deform;
	int x, y, ix, iy, nx, ny;
	int vx, vy;
	double rot;

	deform = false;
	x = xx; y = yy;
	if ((chrno < 0) || (chrno >= chrkaz)) return -1;
	chr = &mem_chr[chrno];
	nx = chr->bsx; ny = chr->bsy;
	ix = chr->bx; iy = chr->by;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base) || (rotz != 0)) {
		deform = true;
		x += nx / 2;
		y += ny / 2;
		vx = (nx * zoomx) >> dotshift;
		vy = (ny * zoomy) >> dotshift;
		rot = pans * -rotz;
	}
	else {
		vx = nx; vy = ny;
	}

	if ((x + vx) <= 0) return -1;
	if (x > main_sx) return -1;
	if ((y + vy) <= 0) return -1;
	if (y > main_sx) return -1;

	Bmscr* src = hspwnd->GetBmscrSafe(chr->wid);
	if (src == NULL) return -1;

	bmscr->cx = x;
	bmscr->cy = y;

	int tp = tpflag;
	if (tp < 0) tp = chr->tpflag;
	setTransparentMode(tp);

	if (deform) {
		bmscr->FillRotTex(vx, vy, (float)rot, src, ix, iy, nx, ny);
		return 0;
	}
	bmscr->Copy(src, ix, iy, nx, ny);
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
	int x1, y1;
	int chrcolx, chrcoly, chrcolsx, chrcolsy;

	chr = &mem_chr[sp->chr];

	int zoomx = sp->zoomx;
	int zoomy = sp->zoomy;

	chrcolx = chr->colx * zoomx;
	chrcoly = chr->coly * zoomy;
	chrcolsx = chr->colsx * zoomx;
	chrcolsy = chr->colsy * zoomy;

	x1 = (sp->xx) + chrcolx;
	y1 = (sp->yy) + chrcoly;

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


int essprite::checkCollision(int spno, int chktype)
{
	//		sprite collision check
	//
	int a, cc;

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

	colx = (ssp->xx) + chrcolx;
	coly = (ssp->yy) + chrcoly;

	if ((zoomx != dotshift_base) || (zoomy != dotshift_base)) {
		int px = (chr->bsx << dotshift) - (chr->bsx * zoomx);
		int py = (chr->bsy << dotshift) - (chr->bsy * zoomy);
		colx += px / 2;
		coly += py / 2;
	}

	colex = colx + chrcolsx;
	coley = coly + chrcolsy;

	SPOBJ* sp = getObj(0);
	if (sp == NULL) return -1;

	for (a = 0; a < spkaz; a++) {
		if (sp->fl) {
			if (sp->type & cc) {
				if (spno != a) {
					if (checkCollisionSub(sp)) return a;
				}
			}
		}
		sp++;
	}
	return -1;
}


int essprite::getSpriteParentAxis(SPOBJ *sp, int* xx, int* yy, int depth)
{
	if (sp->fl & ESSPFLAG_SPLINK) {
		int link = sp->splink;
		if (link & ESSPLINK_BGMAP) {
			BGMAP* bg = getMap(link & (ESSPLINK_BGMAP-1));
			if (bg == NULL) return -1;
			xx -= bg->viewx;
			yy -= bg->viewy;
			return 0;
		}
		SPOBJ* p = getObj(link);
		if (p == NULL) return -1;
		xx += p->xx;
		yy += p->yy;
		if (depth < 16) {		// Nest loop cancel
			getSpriteParentAxis(sp, xx, yy, depth + 1);
		}
	}
	return 0;
}


int essprite::modifySpriteAxis(int spno, int endspno, int type, int x, int y, int option)
{
	int a, res;
	int spno_end;
	int cc;
	a = spno;
	cc = type;
	res = 0;
	if (cc == 0) cc = -1;
	spno_end = endspno;
	if (spno_end < 0) spno_end = spkaz - 1;
	SPOBJ* sp = getObj(a);
	if (sp == NULL) return -1;
	while (1) {
		if (a > spno_end) break;
		if (sp->fl) {
			if (sp->type & cc) {
				sp->xx += x;
				sp->yy += y;
				res++;
			}
		}
		sp++;
		a++;
	}
	return res;
}


int essprite::drawSubMove(SPOBJ *sp, int mode)
{
	//		1 sprite draw (on sp)
	//
	int fl, x, y, land,res, xx, yy, prevxx, prevyy;
	int bsx, bsy;

	//		flag pre check
	//
	res = 0;
	fl = sp->fl;

	chr = getChr(sp->chr);
	if (chr == NULL) return 0;
	bsx = chr->bsx;
	bsy = chr->bsy;

	//		add pos move
	//
	if (fl & ESSPFLAG_MOVE) {
		sp->xx += sp->px;
		sp->yy += sp->py;
	}

	prevxx = sp->xx;
	prevyy = sp->yy;

	if (fl & ESSPFLAG_GRAVITY) {
		sp->xx += sp->px;
		sp->yy += sp->py;
	}

	xx = sp->xx;
	yy = sp->yy;

	getSpriteParentAxis(sp, &xx, &yy, 0);

	if (fl & ESSPFLAG_GRAVITY) {
		//	なんだこりゃ・・・
		//
		if (sp->fspy != 0) {
			sp->py += sp->fspy << 8;
			if (sp->boundflag & 2) {
				if (sp->fspy > 0) {
					land = ( land_y - bsy) << dotshift;
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
	}

	//		border check
	//
	x = xx >> dotshift; y = yy >> dotshift;
	if (fl & ESSPFLAG_XBOUNCE) {
		if (x < ox1) { x = ox1; sp->px = abs(sp->px); }
		if (x > ox2) { x = ox2; sp->px = -abs(sp->px); }
	}
	if (fl & ESSPFLAG_YBOUNCE) {
		if (y < oy1) { y = oy1; sp->py = abs(sp->py); }
		if (y > oy2) { y = oy2; sp->py = -abs(sp->py); }
	}

	if ((fl & ESSPFLAG_NOWIPE) == 0) {
		if (((int)((x - ox1) | (y - oy1) | (ox2 - x) | (oy2 - y))) >> 31) {
			sp->fl = 0;
			return 0;
		}
	}

	//		Flag blink check
	//
	if (fl & 255) {
		fl--;
		if ((fl & 255) == 0) fl = 0;
		if ((fl & 255) == 128) fl ^= 128;
		sp->fl = fl;
	}

	return res;
}


int essprite::drawSubPut(SPOBJ *sp, int mode)
{
	//		1 sprite draw (on sp)
	//
	int res = 0;
	int fl,x,y,xx,yy;
	int next;

	fl = sp->fl;
	if (fl & ESSPFLAG_NODISP) return 0;
	if ((fl & (ESSPFLAG_BLINK | 1)) == 1) return 0;

	xx = sp->xx;
	yy = sp->yy;

	getSpriteParentAxis(sp, &xx, &yy, 0);

	x = xx >> dotshift; y = yy >> dotshift;

	//		send to screen
	//
	if (mode & ESDRAW_NODISP) {
		put(x + ofsx, y + ofsy, sp->chr, sp->tpflag, sp->zoomx, sp->zoomy, sp->rotz);
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
	mode_p = mode ^ -1;
	SPRDATA *selspr = new SPRDATA[spkaz];
	SPRDATA *spr;
	int maxspr = 0;

	a1 = start; a2 = num;
	if (a1 < 0) a1 = 0;
	if (a2 < 0) a2 = spkaz;
	if (a1 >= spkaz) return -1;
	if ((a1 + a2) >= spkaz) a2 = spkaz-a1;

	if ((start_pri >= 0) && (end_pri >= 0)) {
		if (start_pri <= end_pri) priselect = true;
	}

	a = a1 + a2 - 1;
	sp = getObj(a);
	spr = selspr;

	for (i = 0; i < a2; i++) {
		if (sp->fl) {
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

	if (mode_p & ESDRAW_NOSORT) priselect = false;

	if (priselect) {
		std::sort(selspr, selspr + maxspr, less_int_1);
	}

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
						ctx->iparam = a1;
						ctx->wparam = sp->type;
						ctx->lparam = sp->chr;
						code_call(sp->sbr);
					}
				}
			}
			if (sp->fl) {
				drawSubPut(sp, mode_p);
			}
		}
		spr++;
	}

	delete selspr;
	return maxspr;
}


int essprite::getParameter(int spno, int prmid)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return 0;
	int* prm = (int *)sp;
	return prm[prmid];
}


void essprite::setParameter(int spno, int prmid, int value)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return;
	int* prm = (int*)sp;
	prm[prmid] = value;
}


int essprite::setSpriteFlag(int spno, int flag)
{
	SPOBJ* sp = getObj(spno);
	if (sp == NULL) return -1;
	sp->fl = flag;
	return spno;
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
	default:
		return -1;
	}

	return spno;
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
	return setSpriteAddPos(spno,ax,ay,true);
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
	sp->prg = 0;
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
	sp->splink = 0;
	sp->sbr = NULL;

	sp->xx = 0;
	sp->yy = 0;
	sp->chr = -1;
	sp->ani = 0;

	return sp;
}


int essprite::setSpritePosChr(int def_spno, int xx, int yy, int chrno, int option, int pri)
{
	//		sprite set
	//
	int spno = def_spno;
	if (spno < 0) { spno = getEmptySpriteNo(); }

	SPOBJ *sp = resetSprite(spno);
	if (sp == NULL) return -1;

	sp->option = option;
	sp->priority = pri;

	setSpritePos(spno, xx, yy);
	setSpriteChr(spno, chrno);
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
		def_boundflag = p3;
		return 0;
	}

	SPOBJ *sp = getObj(p1);
	if (sp == NULL) return -1;

	if (p2 > 0x100) p2 = 0x100;
	if (p2 < 0) p2 = 0;
	if (p3 < 0) p3 = 0;
	if (p3 > 3) p3 = 3;

	sp->bound = p2;
	sp->boundflag = p3;
	return 0;
}


int essprite::setSpriteFade(int id, int sw, int timer)
{
	int fl;
	int i;
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;

	i = timer & 0xff;
	fl = sp->fl & ~(ESSPFLAG_BLINK | 0xff);
	switch (sw) {
	case 1:
		fl |= i;
		break;
	case 3:
		fl |= i | ESSPFLAG_BLINK;
		break;
	default:
		break;
	}
	sp->fl = fl;
	return 0;
}


int essprite::setSpriteEffect(int id, int tpflag, int mulcolor)
{
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;

	sp->tpflag = tpflag;
	return 0;
}


int essprite::setSpriteRotate(int id, int angle, int zoomx, int zoomy, int rate)
{
	SPOBJ *sp = getObj(id);
	if (sp == NULL) return -1;

	if (zoomx >= 0) {
		int ax = (1 << dotshift) * zoomx / 100;
		sp->zoomx = ax;
	}
	if (zoomy >= 0) {
		int ay = (1 << dotshift) * zoomy / 100;
		sp->zoomy = ay;
	}

	sp->rotz = angle;

	return 0;
}


/*------------------------------------------------------------*/
/*
		BG Map Process
*/
/*------------------------------------------------------------*/

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

	bg->varptr = varptr;
	bg->mapsx = mapsx;
	bg->mapsy = mapsy;
	bg->sizex = sx;
	bg->sizey = sy;
	bg->viewx = 0;
	bg->viewy = 0;
	bg->buferid = buffer;
	bg->bgoption = option;
	bg->tpflag = 0x3ff;

	return bgno;
}


int essprite::setMapPos(int bgno, int x, int y)
{
	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;
	if (bg->varptr == NULL) return -1;

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

	BGMAP* bg = getMap(bgno);
	if (bg == NULL) return -1;

	Bmscr* bm = hspwnd->GetBmscrSafe(bg->buferid);
	if (bm == NULL) return -1;
	divx = bm->divsx; divy = bm->divsy;

	mapsrc = bg->varptr;
	if (mapsrc == NULL) return -1;

	sx = bg->sizex;
	sy = bg->sizey;
	vx = bg->viewx;
	vy = bg->viewy;
	vpx = vx % divx;
	vpy = vy % divy;
	if (vpx > 0) sx++;
	if (vpy > 0) sy++;
	x = xx - vpx;
	y = yy - vpy;

	setTransparentMode(bg->tpflag);

	ofsy = vy/divy;
	for (j = 0; j < sy;j++) {
		bmscr->cx = x;
		bmscr->cy = y;
		p = mapsrc;
		p += bg->mapsx * (ofsy % bg->mapsy);
		ofsx = vx/divx;
		for (i = 0; i < sx;i++) {
			bmscr->CelPut(bm, p[ofsx % bg->mapsx]);
			ofsx++;
		}
		y+=bm->divsy;
		ofsy++;
	}
	return 0;
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
	px = (double)abs(x2-x1);
	py = (double)abs(y2-y1);
	dist = sqrt(px * px + py * py);
	return (int)(dist * 256.0);
}


