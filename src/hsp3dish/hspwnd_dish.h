
//
//	hspwnd.cpp header (dish)
//
#ifndef __hspwnd_dish_h
#define __hspwnd_dish_h

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3code.h"

//	Window Object Info
//
#define HSPOBJ_LIMIT_DEFAULT	128

#define HSPOBJ_INPUT_STR 2
#define HSPOBJ_INPUT_DOUBLE 3
#define HSPOBJ_INPUT_INT 4
#define HSPOBJ_INPUT_MULTILINE 0x100
#define HSPOBJ_INPUT_READONLY 0x200
#define HSPOBJ_INPUT_HSCROLL 0x400

#define HSPOBJ_NONE 0
#define HSPOBJ_TAB_ENABLE 1
#define HSPOBJ_TAB_DISABLE 2
#define HSPOBJ_TAB_SKIP 3
#define HSPOBJ_TAB_SELALLTEXT 4

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
	char name[64];				// button name
	short messx, messy;			// message rect size

	//	参照元
	short normal_x, normal_y;	// 通常時
	short push_x, push_y;		// 押下時
	short focus_x, focus_y;		// フォーカス時

	short jumpmode;				// jump mode
	short ext;					// dummy
	void *ptr;					// jump 呼び出し先
} HSP3BTNSET;

typedef struct HSPOBJINFO
{
	//		Object Info (3.0)
	//
	short	owmode;		// objectのmode
	short	enableflag;	// objectの有効フラグ

	void	*bm;		// objectが配置されているBMSCR構造体のポインタ
	void	*hCld;		// objectのhandle
	int		owid;		// objectのValue(汎用)
	int		owsize;		// objectの使用サイズ(汎用)

	short x,y;			// 左上座標
	short sx,sy;		// サイズ
	short tapflag;		// タップフラグ
	short srcid;		// 参照BufferID

	HSP3BTNSET *btnset;	// objectから設定される情報

	//		callback function
	void	(*func_draw)( struct HSPOBJINFO * );
	void	(*func_notice)( struct HSPOBJINFO *, int );
	void	(*func_objprm)( struct HSPOBJINFO *, int, void * );
	void	(*func_delete)( struct HSPOBJINFO * );

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
#define HSPWND_TYPE_OFFSCREEN 5

#define HSPWND_OPTION_OFFSCREEN 32
#define HSPWND_OPTION_USERSHADER 64

#define BMSCR_MAX_MTOUCH	16		// Max Points of Multi-Touch
typedef struct HSP3MTOUCH
{
	//	HSP3VARSET structure
	//
	int flag;		// Touch Flag (1=ON/0=OFF)
	int x;			// X Position
	int y;			// Y Position
	int pointid;	// Touch point ID
} HSP3MTOUCH;


enum {
BMSCR_SAVEPOS_MOSUEX,
BMSCR_SAVEPOS_MOSUEY,
BMSCR_SAVEPOS_MOSUEZ,
BMSCR_SAVEPOS_MOSUEW,
BMSCR_SAVEPOS_MAX,
};

typedef struct {
	//	デバイスごとの情報
	//	(*の項目は、親アプリケーションで設定されます)
	//
	char *devname;				// *デバイスランタイム名
	char *error;				// *エラーメッセージ

	//	ファンクション情報
	//
	int (*devprm)( char *name, char *value );	// パラメーター設定ファンクション
	int (*devcontrol)( char *cmd, int p1, int p2, int p3 );	// コマンド受け取りファンクション
	int *(*devinfoi)( char *name, int *size );	// int情報受け取りファンクション
	char *(*devinfo)( char *name );				// str情報受け取りファンクション

} HSP3DEVINFO;

#define RESNAME_MAX 64

void SetObjectEventNoticePtr( int *ptr );

//	Bmscr class
//
class Bmscr {
public:
	//	Functions
	//
	Bmscr( void );
	~Bmscr( void );
	void *GetBMSCR( void ) { return (void *)(&this->flag); };
	void Init( int p_sx, int p_sy );
	void Init( char *fname );
	void Cls( int mode );

	void Posinc( int pp );
	void Width( int x, int y, int wposx, int wposy, int mode );
	void Title( char *str );
	void Setcolor( int a1, int a2, int a3 );
	void Setcolor( int icolor );
	void SetMulcolor( int a1, int a2, int a3 );
	void SetHSVColor( int hval, int sval, int vval );
	int BmpSave( char *fname );
	void GetClientSize( int *xsize, int *ysize );
	void SetFont( char *fontname, int size, int style );
	void SetFontInternal( char *fontname, int size, int style );
	void SetDefaultFont( void );

	void Print( char *mes );
	void Boxfill( int x1,int y1,int x2,int y2 );
	void Circle( int x1,int y1,int x2,int y2, int mode );
	int Pget( int xx, int yy );
	void Pset( int xx,int yy );
	void Line( int xx,int yy );
	int Copy( Bmscr *src, int xx, int yy, int psx, int psy );
	int Zoom( int dx, int dy, Bmscr *src, int xx, int yy, int psx, int psy, int mode );

	void SetCelDivideSize( int new_divsx, int new_divsy, int new_ofsx, int new_ofsy );
	int CelPut( Bmscr *src, int id );
	int CelPut( Bmscr *src, int id, float destx, float desty, float ang );

	void FillRot( int x, int y, int dst_sx, int dst_sy, float ang );
	void FillRotTex( int dst_sx, int dst_sy, float ang, Bmscr *src, int tx, int ty, int srcx, int scry );
	void SquareTex( int *dst_x, int *dst_y, Bmscr *src, int *src_x, int *src_y, int mode );
	void GradFill( int x, int y, int sx, int sy, int mode, int col1, int col2 );

	void SetFilter( int type );

	int NewHSPObject( void );
	void ResetHSPObject( void );
	void NextObject( int plus );

	HSPOBJINFO *AddHSPObject( int id, int mode );
	HSPOBJINFO *GetHSPObject( int id );
	HSPOBJINFO *GetHSPObjectSafe( int id );
	void DeleteHSPObject( int id );
	void EnableObject( int id, int sw );
	void SetObjectMode( int id, int owmode );
	int DrawAllObjects( void );
	int UpdateAllObjects( void );

	int AddHSPObjectButton( char *name, int eventid, void *callptr );
	void SetButtonImage( int id, int bufid, int x1, int y1, int x2, int y2, int x3, int y3 );

	void setMTouch( HSP3MTOUCH *mt, int x, int y, bool touch );
	void setMTouchByPoint( int old_x, int old_y, int x, int y, bool touch );
	void setMTouchByPointId( int pointid, int x, int y, bool touch );
	HSP3MTOUCH *getMTouch( int id );
	HSP3MTOUCH *getMTouchByPointId( int pointid );
	HSP3MTOUCH *getMTouchByPoint( int x, int y );
	HSP3MTOUCH *getMTouchNew( void );
	void resetMTouch( void );
	int listMTouch( int *outbuf );


	//
	//		Window data structure
	//
	int		flag;				// used flag
	int		sx;					// X-size
	int		sy;					// Y-size
	int		palmode;			// palmode
//	HDC		hdc;				// buffer HDC
//	BYTE	*pBit;				// bitmap pointer
//	BITMAPINFOHEADER *pbi;		// infoheader
//	HBITMAP	dib;				// bitmap handle(DIB)
//	HBITMAP	old;				// bitmap handle(OLD)
//	RGBQUAD *pal;				// palette table
//	HPALETTE hpal;				// palette handle
//	HPALETTE holdpal;			// palette handle (old)
	int		pals;				// palette entries
//	HWND	hwnd;				// window handle
//	HINSTANCE hInst;			// Instance of program
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
//	HBRUSH	hbr;				// BRUSH handle
//	HPEN	hpn;				// PEN handle
//	HFONT	hfont;				// FONT handle
//	HFONT	holdfon;			// FONT handle (old)
//	COLORREF color;				// text color code
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
	short	printsizex;			// print,mes extent X size
	short	printsizey;			// print,mes extent Y size
//	SIZE	printsize;			// print,mes extent size

	//		Class depend data
	//
	int		objstyle;					// objects style
	HSPOBJINFO *mem_obj;				// Window objects
	int objmax;							// Max number of obj
	int objlimit;						// Limit number of obj
	short savepos[BMSCR_SAVEPOS_MAX];	// saved position
	void *master_hspwnd;				// Parent hspwnd class

	int		imgbtn;						// Custom Button Flag (-1=none)
	short	btn_x1, btn_y1;				// Custom Button Image X,Y
	short	btn_x2, btn_y2;				// Custom Button Image X,Y (press)
	short	btn_x3, btn_y3;				// Custom Button Image X,Y (mouse over)

	short	divx, divy;					// Divide value for CEL
	short	divsx, divsy;				// CEL size
	short	celofsx, celofsy;			// CEL center offset

	char	resname[RESNAME_MAX];		// Resource Name
	int		texid;						// Texture ID

	short	tapstat;					// TapStatus
	short	tapinvalid;					// Invalid Tap Flag
	HSPOBJINFO *cur_obj;				// Tap active objects

	int		mtouch_num;					// Active Multi-Touch points
	HSP3MTOUCH mtouch[BMSCR_MAX_MTOUCH];	// Multi-Touch Info

	float	colorvalue[4];				// ColorRGB value each 0.0～1.0

	char	font_curname[RESNAME_MAX];	// Current Font Name
	int		font_cursize;				// Current Font Size
	int		font_curstyle;				// Current Font Style

	int		mulcolor;					// Multiplyer Color (RGB)
	float	mulcolorvalue[4];			// Multiplyer Color value each 0.0～1.0

	int		buffer_option;				// buffer options for off-screen
	void	*master_buffer;				// buffer pointer to off-screen
	HSPREAL	accel_value[BMSCR_SAVEPOS_MAX];		// Accelerometer sensor value

private:
//	void Blt( int mode, Bmscr *src, int xx, int yy, int asx, int asy );
//	void CnvRGB16( PTRIVERTEX target, DWORD src );

};


//	HspWnd Base class
//

class HspWnd {
public:
	//	Functions
	//
	HspWnd( void );
	~HspWnd( void );
	void Resume( void );

	void SetMasterSize( int m_sx, int m_sy );
	void SetMasterInstance( void *m_inst );

	void MakeBmscr( int id, int type, int x, int y, int sx, int sy, int option );
	void MakeBmscrFromResource( int id, char *fname );
	inline Bmscr *GetBmscr( int id ) { return mem_bm[id]; };
	Bmscr *GetBmscrSafe( int id );
	int Picload( int id, char *fname, int mode );
	int GetActive( void );
	int GetBmscrMax( void ) { return bmscr_max; };
	int GetEmptyBufferId( void );
	HSP3DEVINFO *getDevInfo( void ) { return &devinfo; }

	//	Data
	//
	int mouse_x, mouse_y;
	int sys_iprm, sys_wprm, sys_lprm;

private:
	void Reset( void );
	void Dispose( void );
	void ExpandScreen( int id );

	//	Data
	//
	int m_sx, m_sy;		// Master Window Size
	int m_handle;		// Master Handle

	Bmscr **mem_bm;
	int bmscr_max;
	int bmscr_res;

	//	Info for HSP3Dish Device
	HSP3DEVINFO devinfo;
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
//	HDC		hdc;				// buffer HDC
//	BYTE	*pBit;				// bitmap pointer
//	BITMAPINFOHEADER *pbi;		// infoheader
//	HBITMAP	dib;				// bitmap handle(DIB)
//	HBITMAP	old;				// bitmap handle(OLD)
//	RGBQUAD *pal;				// palette table
//	HPALETTE hpal;				// palette handle
//	HPALETTE holdpal;			// palette handle (old)
	int		pals;				// palette entries
//	HWND	hwnd;				// window handle
//	HINSTANCE hInst;			// Instance of program
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
//	HBRUSH	hbr;				// BRUSH handle
//	HPEN	hpn;				// PEN handle
//	HFONT	hfont;				// FONT handle
//	HFONT	holdfon;			// FONT handle (old)
//	COLORREF color;				// text color code
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
	short	printsizex;			// print,mes extent X size
	short	printsizey;			// print,mes extent Y size
//	SIZE	printsize;			// print,mes extent size

	//		Class depend data
	//
	int		objstyle;					// objects style
	HSPOBJINFO *mem_obj;				// Window objects
	int objmax;							// Max number of obj
	int objlimit;						// Limit number of obj
	short savepos[BMSCR_SAVEPOS_MAX];	// saved position
	void *master_hspwnd;				// Parent hspwnd class

	int		imgbtn;						// Custom Button Flag (-1=none)
	short	btn_x1, btn_y1;				// Custom Button Image X,Y
	short	btn_x2, btn_y2;				// Custom Button Image X,Y (press)
	short	btn_x3, btn_y3;				// Custom Button Image X,Y (mouse over)

	short	divx, divy;					// Divide value for CEL
	short	divsx, divsy;				// CEL size
	short	celofsx, celofsy;			// CEL center offset

	char	resname[RESNAME_MAX];		// Resource Name
	int		texid;						// Texture ID

	short	tapstat;					// TapStatus
	short	tapinvalid;					// Invalid Tap Flag
	HSPOBJINFO *cur_obj;				// Tap active objects

	int		mtouch_num;					// Active Multi-Touch points
	HSP3MTOUCH mtouch[BMSCR_MAX_MTOUCH];	// Multi-Touch Info

	float	colorvalue[4];				// ColorRGB value each 0.0～1.0

	char	font_curname[RESNAME_MAX];	// Current Font Name
	int		font_cursize;				// Current Font Size
	int		font_curstyle;				// Current Font Style

	int		mulcolor;					// Multiplyer Color (RGB)
	float	mulcolorvalue[4];			// Multiplyer Color value each 0.0～1.0

	int		buffer_option;				// buffer options for off-screen
	void	*master_buffer;				// buffer pointer to off-screen
	HSPREAL	accel_value[BMSCR_SAVEPOS_MAX];		// Accelerometer sensor value

} BMSCR;

#endif
