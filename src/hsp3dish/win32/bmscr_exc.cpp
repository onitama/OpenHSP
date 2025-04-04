//
//		Draw lib (directX8+TEXMES)
//			onion software/onitama 2001/6
//			               onitama 2011/5
//
#include <stdio.h>
#define STRICT
#include <windows.h>
#include <math.h>

#include "../hgio.h"
#include "../supio.h"
#include "../sysreq.h"

#include "bmscr_exc.h"


/*------------------------------------------------------------*/
/*
		HSP BMSCR exchange Service
*/
/*------------------------------------------------------------*/

//	Bmscr structure (same as Bmscr)
//
typedef struct BMSCR3
{
	//
	//		Window data structure
	//
	int		flag;				// used flag
	int		sx;					// X-size
	int		sy;					// Y-size
	int		palmode;			// palmode
	HDC		hdc;				// buffer HDC
	BYTE	*pBit;				// bitmap pointer
	BITMAPINFOHEADER *pbi;		// infoheader
	HBITMAP	dib;				// bitmap handle(DIB)
	HBITMAP	old;				// bitmap handle(OLD)
	RGBQUAD *pal;				// palette table
	HPALETTE hpal;				// palette handle
	HPALETTE holdpal;			// palette handle (old)
	int		pals;				// palette entries
	HWND	hwnd;				// window handle
	HINSTANCE hInst;			// Instance of program
	int		infsize;			// *pbi alloc memory size
	int		bmpsize;			// *pBit alloc memory size

	//		Window object setting
	//
	int		type;				// setting type
	int		wid;				// window ID
	short	fl_dispw;			// display window flag
	short	fl_udraw;			// update draw window
	int		wx,wy,wchg;			// actual window size x,y
	int		viewx,viewy;		// buffer view point x,y
	int		lx,ly;				// buffer view size x,y
	int		cx,cy;				// object cursor x,y
	int		ox,oy,py;			// object size x,y,py
	int		texty;				// text Y-axis size
	int		gx,gy,gmode;		// gcopy size
	HBRUSH	hbr;				// BRUSH handle
	HPEN	hpn;				// PEN handle
	HFONT	hfont;				// FONT handle
	HFONT	holdfon;			// FONT handle (old)
	COLORREF color;				// text color code
	int		textspeed;			// slow text speed
	int		cx2,cy2;			// slow text cursor x,y
	int		tex,tey;			// slow text limit x,y
	char	*prtmes;			// slow message ptr
	int		focflg;				// focus set flag
	int		objmode;			// object set mode

	LOGFONT	*logfont;			// logical font
	int		logopt[14];			// dummy padding

	int		style;				// extra window style
	int		gfrate;				// halftone copy rate
	int		tabmove;			// object TAB move mode
	int		sx2;				// actual bitmap X size
	SIZE	printsize;			// print,mes extent size

	//		Class depend data
	//
	int		objstyle;					// objects style
	HSPOBJINFO *mem_obj;				// Window objects
	int objmax;							// Max number of obj
	int objlimit;						// Limit number of obj
	short savepos[BMSCR_SAVEPOS_MAX];	// saved position
	void *master_hspwnd;				// Parent hspwnd class
	short	palcolor;					// Palette color code
	short	textstyle;					// Extra text style
	short	framesx, framesy;			// Window frame xy-size

	int		imgbtn;						// Custom Button Flag (-1=none)
	short	btn_x1, btn_y1;				// Custom Button Image X,Y
	short	btn_x2, btn_y2;				// Custom Button Image X,Y (press)
	short	btn_x3, btn_y3;				// Custom Button Image X,Y (mouse over)
	short	divx, divy;					// Divide value for CEL
	short	divsx, divsy;				// CEL size
	short	celofsx, celofsy;			// CEL center offset

	COLORREF objcolor;					// object color code
	int		fonteff_size;				// effect size for font

	int		vp_flag;					// Viewport enable flag (0=none)
	float	vp_matrix[16];				// Viewport matrix

} BMSCR3;


static BMSCR3 bmscr;


void init_bmscr3(Bmscr* bm, int _inst, int _wnd, int _hdc)
{
	//		HSP3DishのBMSCRをHSP3のBMSCR型に変換する
	//
	memset(&bmscr, 0, sizeof(BMSCR3));
	bmscr.hInst = (HINSTANCE)_inst;
	bmscr.hwnd = (HWND)_wnd;
	bmscr.hdc = (HDC)_hdc;
	bmscr.sx = bm->sx;
	bmscr.sx2 = bm->sx;
	bmscr.sy = bm->sy;
}


BMSCR3* get_bmscr3(void)
{
	//		HSP3のBMSCR型を取得する
	//
	return &bmscr;
}

