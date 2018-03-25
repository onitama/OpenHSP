
//
//	HSPDLL.H  header file for DLL
//

#define EXPORT extern "C" __declspec (dllexport)

#define objkazz 64
typedef struct BMSCR
{
	//		Bitmap buffer structure
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
	HANDLE	hInst;				// Instance of program
	int		infsize;			// *pbi alloc memory size
	int		bmpsize;			// *pBit alloc memory size

	//		Window object setting
	//
	int		type;				// setting type
	int		wid;				// window ID
	short	fl_dispw;			// display window flag
	short	fl_udraw;			// update draw window
	int		wx,wy,wchg;			// actual window size x,y
	int		xx,yy;				// buffer view point x,y
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

	HANDLE	hCld[objkazz];		// button‚Ìhandle
	int		owid[objkazz];		// button‚Ìjump ID
	int		owb;				// handle‚Ìindex

	int		textspeed;			// slow text speed
	int		cx2,cy2;			// slow text cursor x,y
	int		tex,tey;			// slow text limit x,y
	char	*prtmes;			// slow message ptr

} BMSCR;


typedef struct PVAL
{
	//		Memory Val structure
	//
	short	flag;				// type of val (2=str/4=num)
	short	mode;				// mode (0=normal/1=clone/2=alloced)
	short	len[5];				// length of array 4byte align (dim)
	char	*pt;				// ptr to Val
} PVAL;

