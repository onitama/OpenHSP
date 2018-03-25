
//
//	hspwnd.cpp header
//
#ifndef __hspwnd_linux_h
#define __hspwnd_linux_h

#include "../hsp3code.h"

//	Bmscr structure (same as Bmscr)
//
typedef struct BMSCR
{
	//
	//		Window data structure
	//
	int		flag;				// used flag
	int		sx;					// X-size
	int		sy;					// Y-size
	int		palmode;			// palmode
	void	*hdc;				// buffer HDC
	void	*pBit;				// bitmap pointer
	void	 *pbi;				// infoheader
	void	*dib;				// bitmap handle(DIB)
	void	*old;				// bitmap handle(OLD)
	void	*pal;				// palette table
	void	*hpal;				// palette handle
	void	*holdpal;			// palette handle (old)
	int		pals;				// palette entries
	void	*hwnd;				// window handle
	void	*hInst;				// Instance of program
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
	void	*hbr;				// BRUSH handle
	void	*hpn;				// PEN handle
	void	*hfont;				// FONT handle
	void	*holdfon;			// FONT handle (old)
	int		 color;				// text color code
	int		textspeed;			// slow text speed
	int		cx2,cy2;			// slow text cursor x,y
	int		tex,tey;			// slow text limit x,y
	char	*prtmes;			// slow message ptr
	int		focflg;				// focus set flag
	int		objmode;			// object set mode
//	LOGFONT	logfont;			// logical font
	int		style;				// extra window style
	int		gfrate;				// halftone copy rate
	int		tabmove;			// object TAB move mode
	int		sx2;				// actual bitmap X size
//	SIZE	printsize;			// print,mes extent size

	//		Class depend data
	//
	int		objstyle;			// objects style
//	HSPOBJINFO *mem_obj;		// Window objects
	int objmax;					// Max number of obj
	int objlimit;				// Limit number of obj
//	void *master_hspwnd;		// Parent hspwnd class
} BMSCR;

#endif
