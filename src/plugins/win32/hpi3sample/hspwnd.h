
//
//	hspwnd.cpp header
//
#ifndef __hspwnd_h
#define __hspwnd_h

//	Window Object Info
//
#define HSPOBJ_OPTION_SETFONT	0x100

#define MESSAGE_HSPOBJ	0x4000
#define HSPOBJ_LIMIT_DEFAULT	1024

#define HSPOBJ_INPUT_STR 2
#define HSPOBJ_INPUT_DOUBLE 3
#define HSPOBJ_INPUT_INT 4
#define HSPOBJ_INPUT_MULTILINE 0x100
#define HSPOBJ_INPUT_READONLY 0x200
#define HSPOBJ_INPUT_HSCROLL 0x400
#define HSPOBJ_INPUT_NOWRAP 0x800
#define HSPOBJ_INPUT_BACKCOLOR 0x1000

#define HSPOBJ_NONE 0
#define HSPOBJ_TAB_ENABLE 1
#define HSPOBJ_TAB_DISABLE 2
#define HSPOBJ_TAB_SKIP 3
#define HSPOBJ_TAB_SELALLTEXT 4

#define HSPOBJ_OPTION_LAYEROBJ 0x8000
#define HSPOBJ_OPTION_LAYER_MIN 0
#define HSPOBJ_OPTION_LAYER_BG 1
#define HSPOBJ_OPTION_LAYER_NORMAL 2
#define HSPOBJ_OPTION_LAYER_POSTEFF 3
#define HSPOBJ_OPTION_LAYER_MAX 4
#define HSPOBJ_OPTION_LAYER_MULTI 0x100

#define HSPOBJ_LAYER_CMD_NONE (0)
#define HSPOBJ_LAYER_CMD_INIT (1)
#define HSPOBJ_LAYER_CMD_TERM (2)
#define HSPOBJ_LAYER_CMD_PRMI (3)
#define HSPOBJ_LAYER_CMD_PRMS (4)
#define HSPOBJ_LAYER_CMD_PRMD (5)
#define HSPOBJ_LAYER_CMD_DRAW (6)
#define HSPOBJ_LAYER_CMD_TIME (7)

#define HSPMES_FONT_EFFSIZE_DEFAULT (1)
#define HSPMES_NOCR (1)
#define HSPMES_SHADOW (2)
#define HSPMES_OUTLINE (4)
#define HSPMES_LIGHT (8)
#define HSPMES_GMODE (16)

typedef struct HSP3VARSET
{
	//	HSP3VARSET structure
	//
	int type;
	PVal *pval;
	APTR aptr;
	void *ptr;
} HSP3VARSET;

typedef struct HSP3BTNSET
{
	//	HSP3BTNSET structure
	//	(HSP3VARSETと同サイズにすること)
	//
	short normal_x, normal_y;	// 通常時
	short push_x, push_y;		// 押下時
	short focus_x, focus_y;		// フォーカス時
	void *ptr;	// 呼び出し先
} HSP3BTNSET;

typedef struct HSPOBJINFO
{
	//		Object Info (3.0)
	//
	short	owmode;		// objectのmode
	short	option;		// objectのoption(未使用・内部オブジェクトは0)
	void	*bm;		// objectが配置されているBMSCR構造体のポインタ
	HWND	hCld;		// objectのhandle
	int		owid;		// objectのValue(汎用)
	int		owsize;		// objectの使用サイズ(汎用)

	HSP3VARSET varset;	// objectから設定される変数の情報

	//		callback function
	void	(*func_notice)( struct HSPOBJINFO *, int );
	void	(*func_objprm)( struct HSPOBJINFO *, int, void * );
	void	(*func_delete)( struct HSPOBJINFO * );

	//		Extra Object Info (3.6)
	//
	HBRUSH	br_back;
	COLORREF color_back;
	COLORREF color_text;
	int		exinfo1,exinfo2;
	HSPCTX *hspctx;

} HSPOBJINFO;


#define BMSCR_FLAG_NOUSE	0
#define BMSCR_FLAG_INUSE	1
#define BMSCR_PALMODE_FULLCOLOR	0
#define BMSCR_PALMODE_PALETTECOLOR	1

#define HSPWND_TYPE_NONE 0
#define HSPWND_TYPE_BUFFER 1
#define HSPWND_TYPE_MAIN 2
#define HSPWND_TYPE_BGSCR 3
#define HSPWND_TYPE_SSPREVIEW 4

enum {
BMSCR_SAVEPOS_MOSUEX,
BMSCR_SAVEPOS_MOSUEY,
BMSCR_SAVEPOS_MOSUEZ,
BMSCR_SAVEPOS_MOSUEW,
BMSCR_SAVEPOS_MAX,
};


//	Bmscr class
//
class Bmscr {
public:
	//	Functions
	//
	Bmscr( void );
	~Bmscr( void );
	void *GetBMSCR( void ) { return (void *)(&this->flag); };
	void Init( HANDLE instance, HWND p_hwnd, int p_sx, int p_sy, int palsw );
	void Cls( int mode );
	void Bmspnt( HDC hdc );
	void Update( void );
	void Send( int x, int y, int p_sx, int p_sy );
	void Posinc( int pp );
	void Width( int x, int y, int wposx, int wposy, int mode );
	void Title( char *str );

	void Delfont( void );
	void Fontupdate( void );
	int Newfont( char *fonname, int fpts, int fopt, int angle );
	void Sysfont( int p1 );
	void Setcolor( int a1, int a2, int a3 );
	void Setcolor( COLORREF rgbcolor );
	void Setcolor2(  COLORREF rgbcolor );
	void SetHSVColor( int hval, int sval, int vval );
	void SetSystemcolor( int id );
	void SetPalette( int palno, int rv, int gv, int bv );
	void SetPalcolor( int palno );
	void UpdatePalette( void );
	int BmpSave( char *fname );
	void GetClientSize( int *xsize, int *ysize );

	void Print(char *mes, int option);
	void PrintLine(char *mes);
	int PrintSub(char *mes);
	int PrintSubMul(char *mes, int x, int y, int px, int py, int times);
	void Boxfill( int x1,int y1,int x2,int y2 );
	void Circle( int x1,int y1,int x2,int y2, int mode );
	COLORREF Pget( int xx, int yy );
	void Pset( int xx,int yy );
	void Line( int xx,int yy );
	int Copy( Bmscr *src, int xx, int yy, int psx, int psy );
	int Zoom( int dx, int dy, Bmscr *src, int xx, int yy, int psx, int psy, int mode );
	void SetScroll(int xbase, int ybase);

	int NewHSPObject( void );
	void ResetHSPObject( void );
	int ActivateHSPObject( int id );
	void NextObject( int plus );

	HSPOBJINFO *AddHSPObject( int id, HWND handle, int mode );
	HSPOBJINFO *AddHSPJumpEventObject( int id, HWND handle, int mode, int val, void *ptr );
	HSPOBJINFO *AddHSPVarEventObject( int id, HWND handle, int mode, PVal *pval, APTR aptr, int type, void *ptr );
	HSPOBJINFO *GetHSPObject( int id );
	HSPOBJINFO *GetHSPObjectSafe( int id );
	HSPOBJINFO *TrackHSPObject( HWND hwnd );

	void DeleteHSPObject( int id );
	void SetHSPObjectFont( int id );
	void SendHSPObjectNotice( int wparam );
	void UpdateHSPObject( int id, int type, void *ptr );
	void SendHSPLayerObjectNotice(int layer, int cmd);

	int AddHSPObjectButton( char *name, int eventid, void *callptr );
	int AddHSPObjectCheckBox( char *name, PVal *pval, APTR aptr );
	int AddHSPObjectInput( PVal *pval, APTR aptr, int sizex, int sizey, char *defval, int limit, int mode );
	int AddHSPObjectMultiBox( PVal *pval, APTR aptr, int psize, char *defval, int mode );
	int AddHSPObjectLayer(int sizex, int sizey, int layer, int val, int mode, void *callptr);
	void SetButtonImage( int id, int bufid, int x1, int y1, int x2, int y2, int x3, int y3 );
	void DrawHSPCustomButton( HSPOBJINFO *obj, HDC drawhdc, int flag );
	void SetHSPObjectColor( HSPOBJINFO *obj );
	void SendHSPObjectDraw( int wparam, LPDRAWITEMSTRUCT lparam );

	void EnableObject( int id, int sw );
	void SetObjectMode( int id, int owmode );
	void GradFill( int x, int y, int sx, int sy, int mode, DWORD col1, DWORD col2 );
	void GradFillEx( int *vx, int *vy, int *vcol );
	int GetAlphaOperation( void );

	void SetCelDivideSize( int new_divsx, int new_divsy, int new_ofsx, int new_ofsy );
	int CelPut( Bmscr *src, int id );

	int RenderAlphaBitmap( int psx, int psy, int components, unsigned char *src );

	void Viewcalc_reset(void);
	int Viewcalc_set(HSPREAL* viewmatrix);
	void Viewcalc_calc(HSPREAL& axisx, HSPREAL& axisy);

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

private:
	void Blt( int mode, Bmscr *src, int xx, int yy, int asx, int asy );
	void CnvRGB16( PTRIVERTEX target, DWORD src );

};


//	HspWnd Base class
//
typedef void (* MM_NOTIFY_FUNC) (void *);

class HspWnd {
public:
	//	Functions
	//
	HspWnd( void );
	HspWnd( HANDLE instance, char *wndcls );
	~HspWnd( void );
	void MakeBmscr( int id, int type, int xx, int yy, int wx, int wy,
	 int sx, int sy, int mode );
	void MakeBmscrWnd( int id, int type, int xx, int yy, int wx, int wy,
	 int sx, int sy, int mode );
	void MakeBmscrOff( int id, int sx, int sy, int palsw );
	inline Bmscr *GetBmscr( int id ) { return mem_bm[id]; };
	Bmscr *GetBmscrSafe( int id );
	int Picload( int id, char *fname, int mode );
	int GetActive( void );
	void SetNotifyFunc( void *func );
	int GetBmscrMax( void ) { return bmscr_max; };
	void SetEventNoticePtr( int *ptr );
	void SetParentWindow( void *hwnd ) { wnd_parent = hwnd; };
	int GetEmptyBufferId( void );

	//	Data
	//
	BYTE pstpt[256*3];			// Palette backup
	int mwfx,mwfy;
	int mouse_x, mouse_y;
	int sys_iprm, sys_wprm, sys_lprm;
	HSPCTX *hspctx;				// HSP context

private:
	void Reset( HANDLE instance, char *wndcls );
	void Dispose( void );
	void ExpandScreen( int idmax );

	//	Data
	//
	HINSTANCE hInst;
	Bmscr **mem_bm;
	int bmscr_max;
	int bmscr_res;
	int wfx,wfy,wbx,wby;
	int *resptr;
	TCHAR defcls[32];			// Default Window Class
	void *wnd_parent;			// Parent Window Handle
};




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

} BMSCR;

void SetObjectEventNoticePtr( int *ptr );

#endif
