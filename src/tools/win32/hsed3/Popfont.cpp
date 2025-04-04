/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>

static LOGFONT logfont ;
static HFONT   hFont ;
static DWORD   FontColor ;

void PopFontSetLG ( LOGFONT lf ) { logfont =lf; }
LOGFONT PopFontGetLG ( void ) { return logfont; }
void PopFontSetColor( DWORD col ) { FontColor=col; }
DWORD PopFontGetColor( void ) { return FontColor; }


DWORD PopFontChooseColor( HWND hwnd )
     {
	 BOOL res;
     static CHOOSECOLOR cc ;
     static COLORREF    crCustColors[16] ;

     cc.lStructSize    = sizeof (CHOOSECOLOR) ;
     cc.hwndOwner      = hwnd ;
     cc.hInstance      = NULL ;
     cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
     cc.lpCustColors   = crCustColors ;
     //cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
     cc.Flags          = CC_RGBINIT;
     cc.lCustData      = 0L ;
     cc.lpfnHook       = NULL ;
     cc.lpTemplateName = NULL ;

     res=ChooseColor(&cc) ;
	 if (res) {
		return (DWORD)cc.rgbResult;
	 }
	 return -1;
     }


BOOL PopFontChooseFont (HWND hwnd)
     {
	 BOOL res;
     CHOOSEFONT cf;

     cf.lStructSize      = sizeof (CHOOSEFONT) ;
     cf.hwndOwner        = hwnd ;
     cf.hDC              = NULL ;
     cf.lpLogFont        = &logfont ;
     cf.iPointSize       = 0 ;
     cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
                                                  | CF_EFFECTS ;
     cf.rgbColors        = FontColor;
     cf.lCustData        = 0L ;
     cf.lpfnHook         = NULL ;
     cf.lpTemplateName   = NULL ;
     cf.hInstance        = NULL ;
     cf.lpszStyle        = NULL ;
     cf.nFontType        = 0 ;               // Returned from ChooseFont
     cf.nSizeMin         = 0 ;
     cf.nSizeMax         = 0 ;

     res=ChooseFont (&cf);
	 FontColor=cf.rgbColors;
     return res;
     }

void PopFontInitialize (HWND hwndEdit)
     {
	 FontColor=0;
     GetObject (GetStockObject (SYSTEM_FIXED_FONT), sizeof (LOGFONT),
                                              (PSTR) &logfont) ;
     hFont = CreateFontIndirect (&logfont) ;
     SendMessage (hwndEdit, WM_SETFONT, (WPARAM) hFont, 0) ;
     }

void PopFontSetFont (HWND hwndEdit)
     {
     HFONT hFontNew ;
     RECT  rect ;

     hFontNew = CreateFontIndirect (&logfont) ;
     SendMessage (hwndEdit, WM_SETFONT, (WPARAM) hFontNew, 0) ;
     DeleteObject (hFont) ;
     hFont = hFontNew ;
     GetClientRect (hwndEdit, &rect) ;
     InvalidateRect (hwndEdit, &rect, TRUE) ;
     }

void PopFontDeinitialize (void)
     {
     DeleteObject (hFont) ;
     }
