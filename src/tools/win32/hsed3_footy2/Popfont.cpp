/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include "Footy2.h"
// PathFileExistsで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

static LOGFONT editfont ;
static LOGFONT tabfont ;
static LOGFONT tempeditfont ;
static LOGFONT temptabfont ;
static HFONT   hFont ;
static HFONT   hTabFont ;
extern int activeFootyID;
extern HWND hwndTab;
extern int forcefont;
extern int speedDraw;
extern BOOL bDrawUnderline;
extern char BgImagePath[_MAX_PATH+1];// 背景画像のパス by inovia
extern int BgImageFlag;			// 背景画像の使用フラグ by onitama

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
	 return (DWORD)-1;
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

	 	// Ctrlキーが押されている場合は一時的に固定フォントのみを解除
		if (GetAsyncKeyState(VK_CONTROL)){
		 cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
		}else{
		 cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
												  | CF_FIXEDPITCHONLY;
		}

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
	// 高速再描画の設定
	for(int i = 0; Footy2SetSpeedDraw(i, speedDraw) != FOOTY2ERR_NOID; i++);

	// フォント強制設定
	for(int i = 0; Footy2SetForceFont(i, forcefont) != FOOTY2ERR_NOID; i++);

	for(int i = 0; Footy2SetFontFace(i, FFM_ANSI_CHARSET, editfont.lfFaceName) != FOOTY2ERR_NOID; i++) {
		Footy2SetFontSize(i, FontSize);
	}
	// 日本語に対応できるように追加
	for(int i = 0; Footy2SetFontFace(i, FFM_SHIFTJIS_CHARSET, editfont.lfFaceName) != FOOTY2ERR_NOID; i++)

	// 非フォーカス時もアンダーバー表示
	for(int i = 0; Footy2SetUnderlineDraw(i, (bool)bDrawUnderline) != FOOTY2ERR_NOID; i++);

	// 背景画像読み込み by inovia
	char imgpath[_MAX_PATH * 2 + 1] = {0};
	if (BgImageFlag) {
		if (BgImagePath[0] != 0) {
			if (0 == strncmp(BgImagePath, "./", 2)){
				GetModuleFileNameA(0, imgpath, _MAX_PATH);
				PathRemoveFileSpec(imgpath);	// ディレクトリのみにする
			}
			strcat(imgpath, BgImagePath);
		}
	}
	for (int i = 0; Footy2SetBackgroundImageA(i, imgpath, false) != FOOTY2ERR_NOID; i++);

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

void PopFontMakeFont( LOGFONT *pLogFont, char *fonname, int fpts, int fopt, int angle )
{
	//	select new font
	//		fopt : bit0=BOLD       bit1=Italic
	//		       bit2=Underline  bit3=Strikeout
	//		       bit4=Anti-alias
	//		fpts : point size
	//		angle: rotation
	//
	int a;
	BYTE b;
	;			// logical FONT ptr
	unsigned char chk;

	strcpy( pLogFont->lfFaceName, fonname );
	pLogFont->lfHeight			= -fpts;
	pLogFont->lfWidth			= 0;
	pLogFont->lfOutPrecision	= 0 ;
	pLogFont->lfClipPrecision	= 0 ;

	if (fopt&4) {
		pLogFont->lfUnderline		= TRUE;
	} else {
		pLogFont->lfUnderline		= FALSE;
	}

	if (fopt&8) {
		pLogFont->lfStrikeOut		= TRUE;
	} else {
		pLogFont->lfStrikeOut		= FALSE;
	}

	if ( fopt & 16 ) {
		pLogFont->lfQuality			= ANTIALIASED_QUALITY ;
	} else {
		pLogFont->lfQuality			= DEFAULT_QUALITY;
	}

	pLogFont->lfPitchAndFamily	= 0 ;
	pLogFont->lfEscapement		= angle ;
	pLogFont->lfOrientation		= 0 ;

	b=DEFAULT_CHARSET;
	a=0;while(1) {
		chk=fonname[a++];
		if (chk==0) break;
		if (chk>=0x80) { b=SHIFTJIS_CHARSET;break; }
	}
	pLogFont->lfCharSet = b;

	if (fopt&1) {
		pLogFont->lfWeight = FW_BOLD;
	} else {
		pLogFont->lfWeight = FW_NORMAL;
	}

	if (fopt&2) {
		pLogFont->lfItalic = TRUE;
	} else {
		pLogFont->lfItalic = FALSE;
	}
}

