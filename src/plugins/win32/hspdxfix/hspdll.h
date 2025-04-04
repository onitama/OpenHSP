
//
//	HSPDLL.H  header file for HSP ver2.6 or later
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
	HINSTANCE	hInst;				// Instance of program
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

	//		Enhanced in ver2.4
	//
	int		focflg;				// focus set flag

	//		Enhanced in ver2.5
	//
	int		objmode;			// object set mode
	LOGFONT	logfont;			// logical font

	//		Enhanced in ver2.6
	//
	int		style;				// extra window style
	int		gfrate;				// halftone copy rate
	int		tabmove;			// object TAB move mode
	int		sx2;				// actual bitmap X size
	SIZE	printsize;			// print,mes extent size
	char	*owpval[objkazz];	// object‚ÌPVAL Ptr.
	char	owts[objkazz];		// object‚ÌTAB stop flag

} BMSCR;


typedef struct PVAL2
{
	//		Memory Val structure (ver2.5 Type)
	//
	short	flag;				// type of val
	short	mode;				// mode (0=normal/1=clone/2=alloced)
	int		len[5];				// length of array 4byte align (dim)
	int		size;				// size of Val (not used)
	char	*pt;				// (direct val) or (ptr to array)
} PVAL2;

//		master value of memory ptr.

typedef struct PVAL
{
	//		Memory Val structure (Old Type)
	//
	short	flag;				// type of val
	short	mode;				// mode (0=normal/1=clone/2=alloced)
	short	len[5];				// length of array 4byte align (dim)
	short	version;			// version check code (2.4 = 0)
	char	*pt;				// (direct val) or (ptr to array)
	PVAL2	*realptr;			// real ptr to ver2.5 PVAL
} PVAL;


//		HSP extra info structure ( ver2.6 )

typedef struct HSPOBJINFO
{
	//		Object Info (2.6)
	//
	void	*hCld;		// object‚Ìhandle
	int	owid;		// object‚Ìjump ID
	char	*owpval;	// object‚ÌPVAL Ptr.
	short	owts;		// object‚ÌTAB stop flag
	short	owmode;		// object‚Ìmode(–¢Žg—p)
	//
} HSPOBJINFO;

typedef struct HSPEXINFO
{
	//		HSP internal info data (2.6)
	//
	short ver;		// Version Code
	short min;		// Minor Version
	//
	int *er;		// Parameter Error Flag
	char *pstr;		// String Buffer (master)
	char *stmp;		// String Buffer (sub)
	PVAL2 **pval;	// Master PVAL ptr.
	//
	int *actscr;	// Active Window ID
	int *nptype;	// Next Parameter Type
	int *npval;		// Next Parameter Value
	int *strsize;	// StrSize Buffer
	char *refstr;	// RefStr Buffer
	//
	int (*HspFunc_prm_getv)( void );
	int (*HspFunc_prm_geti)( void );
	int (*HspFunc_prm_getdi)( int defval );
	char *(*HspFunc_prm_gets)( void );
	char *(*HspFunc_prm_getds)( char *defstr );
	int (*HspFunc_val_realloc)( PVAL2 *pv, int size, int mode );
	int (*HspFunc_fread)( char *fname, void *readmem, int rlen, int seekofs );
	int (*HspFunc_fsize)( char *fname );
	void *(*HspFunc_getbmscr)( int wid );
	int (*HspFunc_getobj)( int wid, int id, HSPOBJINFO *inf );
	int (*HspFunc_setobj)( int wid, int id, HSPOBJINFO *inf );
	//
} HSPEXINFO;


