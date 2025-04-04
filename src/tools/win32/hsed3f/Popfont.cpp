/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include "FootyDLL.h"

static LOGFONT editfont ;
static LOGFONT tabfont ;
static LOGFONT tempeditfont ;
static LOGFONT temptabfont ;
static HFONT   hFont ;
static HFONT   hTabFont ;
extern int activeFootyID;
extern HWND hwndTab;

void PopFontSetELG ( LOGFONT lf ) { editfont =lf; }
LOGFONT PopFontGetELG ( void ) { return editfont; }
void PopFontSetTLG ( LOGFONT lf ) { tabfont =lf; }
LOGFONT PopFontGetTLG ( void ) { return tabfont; }
LOGFONT *PopFontGetTELG(){ return &tempeditfont; }
LOGFONT *PopFontGetTTLG(){ return &temptabfont; }

DWORD PopFontChooseColor(HWND hwnd, COLORREF crInitColor)
{
	 BOOL res;
     static CHOOSECOLOR cc ;
     static COLORREF    crCustColors[16] ;

     cc.lStructSize    = sizeof (CHOOSECOLOR) ;
     cc.hwndOwner      = hwnd ;
     cc.hInstance      = NULL ;
     cc.rgbResult      = crInitColor ;
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


LONG PopFontChooseEditFont (HWND hwnd)
     {
	 BOOL res;
     CHOOSEFONT cf;

     cf.lStructSize      = sizeof (CHOOSEFONT) ;
     cf.hwndOwner        = hwnd ;
     cf.hDC              = NULL ;
     cf.lpLogFont        = &tempeditfont ;
     cf.iPointSize       = 0 ;
     cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
												  | CF_FIXEDPITCHONLY;
     cf.rgbColors        = NULL ;
     cf.lCustData        = 0L ;
     cf.lpfnHook         = NULL ;
     cf.lpTemplateName   = NULL ;
     cf.hInstance        = NULL ;
     cf.lpszStyle        = NULL ;
     cf.nFontType        = 0 ;               // Returned from ChooseFont
     cf.nSizeMin         = 0 ;
     cf.nSizeMax         = 0 ;

     res=ChooseFont (&cf);
     return MAKELONG(res, cf.iPointSize);
     }
	 
LONG PopFontChooseTabFont (HWND hwnd)
     {
	 BOOL res;
     CHOOSEFONT cf;

     cf.lStructSize      = sizeof (CHOOSEFONT) ;
     cf.hwndOwner        = hwnd ;
     cf.hDC              = NULL ;
     cf.lpLogFont        = &temptabfont ;
     cf.iPointSize       = 0 ;
     cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
												  | CF_EFFECTS;
     cf.rgbColors        = NULL ;
     cf.lCustData        = 0L ;
     cf.lpfnHook         = NULL ;
     cf.lpTemplateName   = NULL ;
     cf.hInstance        = NULL ;
     cf.lpszStyle        = NULL ;
     cf.nFontType        = 0 ;               // Returned from ChooseFont
     cf.nSizeMin         = 0 ;
     cf.nSizeMax         = 0 ;

     res=ChooseFont (&cf);
     return MAKELONG(res, cf.iPointSize);
     }

void PopFontInitChooseFont(LOGFONT *neweditfont, LOGFONT *newtabfont)
{
	tempeditfont = (neweditfont != NULL) ? *neweditfont : editfont;
	temptabfont  = (newtabfont  != NULL) ? *newtabfont  : tabfont;
}

void PopFontDeinitialize (void)
{
     DeleteObject (hFont) ;
	 DeleteObject (hTabFont) ;
}

void PopFontSetEditFont()
{
	HDC hDC = CreateCompatibleDC(NULL);
	int FontSize = (MulDiv(abs(editfont.lfHeight), 720, GetDeviceCaps(hDC, LOGPIXELSY)) + 5) / 10;
	for(int i = 0; FootySetFont(i, editfont.lfFaceName, FontSize, true) != F_RET_OUTID; i++);
	DeleteDC(hDC);
}

void PopFontSetTabFont()
{
	HFONT hOldTabFont = hTabFont;
	hTabFont = CreateFontIndirect(&tabfont);
	SendMessage(hwndTab, WM_SETFONT, (WPARAM)hTabFont, TRUE);
	if(hOldTabFont != NULL)
		DeleteObject((HGDIOBJ)hOldTabFont);
}

void PopFontApplyEditFont()
{
	editfont = tempeditfont ;
	PopFontSetEditFont();
}

void PopFontApplyTabFont()
{
	tabfont = temptabfont;
	PopFontSetTabFont();
}