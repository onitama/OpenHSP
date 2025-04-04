
//
//		HSP editor / onion software 1997-2002(c)
//				Started  1996/4 onitama
//				Win95ver 1997/7 onitama
//
/*----------------------------------------------------
//   Based on POPPAD.C -- Popup Editor
//               (c) Charles Petzold, 1996
//        and GADGETS.C -- Gadgets for a frame window.
//                 (c) Paul Yao, 1996
-----------------------------------------------------*/

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <direct.h>
#include <winuser.h>
#include <shlobj.h>
#include "comcthlp.h"
#include "poppad.h"

#pragma comment( lib,"comctl32.lib" )
#pragma comment( lib,"htmlhelp.lib" )

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ClientWndProc (HWND, UINT, WPARAM, LPARAM) ;

char szStartDir[_MAX_PATH];
char szExeDir[_MAX_PATH];
char szCmdline[_MAX_PATH];
char szDllDir[_MAX_PATH];
char szAppName[]  = "onipad" ;
int	 winflg,winx,winy,posx,posy,flg_toolbar,flg_statbar;
int  flag_xpstyle;
static HWND hWndMain;

int	startflag;
char startdir[_MAX_PATH];

BOOL bIgnoreSize = FALSE ;
HINSTANCE hInst ;
HWND hwndClient = NULL ;
HWND hwndToolBar = NULL ;
HWND hwndStatusBar = NULL ;
HWND hwndNotify = NULL ;

extern DWORD dwToolBarStyles ;
extern BOOL bStrings ;
extern BOOL bLargeIcons ;
extern BOOL bComboBox ;
extern DWORD dwStatusBarStyles ;
extern int cyToolBar ;

extern HWND hDlgModeless ;

// Read registry for setting
void reg_load( void );

// Toolbar functions.
HWND InitToolBar (HWND hwndParent, HINSTANCE hInst );
HWND RebuildToolBar (HWND hwndParent, WORD wFlag) ;
void ToolBarMessage (HWND hwndTB, WORD wMsg) ;
LRESULT ToolBarNotify (HWND hwnd, WPARAM wParam, LPARAM lParam) ;

// Tooltip functions.
BOOL InitToolTip (HWND hwndToolBar, HWND hwndComboBox) ;
BOOL RelayToolTipMessage (LPMSG pMsg) ;
void CopyToolTipText (LPTOOLTIPTEXT lpttt) ;

// Status bar functions.
HWND InitStatusBar (HWND hwndParent) ;
HWND RebuildStatusBar (HWND hwndParent, WORD wFlag) ;
void StatusBarMessage (HWND hwndSB, WORD wMsg) ;
LRESULT Statusbar_MenuSelect (HWND, WPARAM, LPARAM) ;

//-------------------------------------------------------------------
//	XP Visual Style 対策コード
//-------------------------------------------------------------------

int getUnicodeOffset( char *text, int offset )
{
	//		全角文字を1文字とした単位でのオフセットを求める(unicode offset対策)
	//
	int res;
	unsigned char *p;
	unsigned char a1;

	p = (unsigned char *)text;
	res = offset;
	while(1) {
		a1 = *p;
		if ( a1 == 0 ) break;
		if ( res == 0 ) break;
		p++;
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				p++;
			}
		}
		res--;
	}
	return ((int)(p-(unsigned char *)text));
}

int getUnicodeOffset2( char *text, int offset )
{
	//		mboffset->unicode offsetに変換する(unicode offset対策)
	//
	int res;
	unsigned char *p;
	unsigned char *p2;
	unsigned char a1;

	p = (unsigned char *)text;
	p2 = p + offset;
	res = 0;
	while(1) {
		if ( p>=p2 ) break;
		a1 = *p++;
		if ( a1 == 0 ) break;
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) {
				p++;
			}
		}
		res++;
	}
	return (res);
}

struct DLLVERSIONINFO{
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
};

typedef HRESULT ( CALLBACK* ProcDllGetVersion)( DLLVERSIONINFO *pdvi );
static int GetDllVersion( char *DllName )
{
HINSTANCE hInst;
DLLVERSIONINFO dvi;
ProcDllGetVersion DllGetVersion;
HRESULT hRes;
bool bRes;
DWORD Major;   // メジャーバージョン
DWORD Minor;   // マイナーバージョン
DWORD Build;   // ビルド番号

    hInst = LoadLibrary(DllName);
	bRes=false;
    if(hInst){
        DllGetVersion = (ProcDllGetVersion) GetProcAddress(hInst, "DllGetVersion" );

        if(DllGetVersion){
            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);
			hRes = (*DllGetVersion)(&dvi);
            if(SUCCEEDED(hRes)){
                Major = dvi.dwMajorVersion;
                Minor = dvi.dwMinorVersion;
                Build = dvi.dwBuildNumber;
                bRes = true;
            }
		}
        FreeLibrary(hInst);
    }
	if (bRes) return Major;
	return -1;
}


static int CheckXPEditBox( HINSTANCE hInstance )
{
	//		EditがXP仕様になっているかをチェックする
	//
	int result;
	result = 0;
	if ( GetDllVersion("ComCtl32.dll") > 5 ) { result = 1; }
	
/*	
	//		これだとダメみたい
	DWORD lP,wP;
    HWND hWnd;
	int result;
	hWnd = CreateWindowA("EDIT","漢字",0,0,0,0,0,NULL,NULL,hInstance,0);
    ShowWindow(hWnd,SW_HIDE);

    SendMessageA(hWnd,EM_SETSEL,(WPARAM)0,(LPARAM)-1);
    SendMessageA(hWnd,EM_GETSEL,(WPARAM)&wP,(LPARAM)&lP);
    if ( (int)GetWindowTextLengthA(hWnd) > (int)(lP - wP) ){
        result = 1;
    }else{
        result = 0;
    }
    PostMessage(hWnd,WM_CLOSE,0,0);
    return result;
*/
    return result;
}


/*
	 {
		char ss[128];
		wsprintf( ss,"%d__%d__%d",(int)IsThemeActive(), (int)IsWindowUnicode( hwndEdit ), GetDllVersion("ComCtl32.dll") );
		MessageBox( NULL, ss, "Startup error", MB_OK | MB_ICONEXCLAMATION) ;
	 }
*/

	 
//-------------------------------------------------------------------



//*********************************************************
// 関数SHGetSpecialFolderPath() が使用できない環境で特殊フォルダのパスを取得する。
// 引数は SHGetSpecialFolderPath() と同じ。
//*********************************************************
static BOOL GetSpecialFolderPath( HWND hWnd, int nFolder, char *Path )
{
	IMalloc    *pMalloc;
	ITEMIDLIST *pidl;

	Path[0] = 0;

	if ( NOERROR == SHGetMalloc( &pMalloc ) )
	{
		if ( NOERROR == SHGetSpecialFolderLocation( hWnd, nFolder, &pidl ) )
		{
			if ( SHGetPathFromIDList( pidl, Path ) )
			{
				pMalloc->Free( pidl );
				pMalloc->Release();
				return TRUE;
			}
			pMalloc->Free( pidl );
		}
		pMalloc->Release();
	}
	return FALSE;
}//GetSpecialFolder


//-------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                    LPSTR lpszCmdLine, int cmdShow)       
     {
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASSEX  wc ;
     HACCEL      hAccel ;
	 int		 a;
	 int		 scmd;
	 char		 a1;

     InitCommonControls () ;

     hInst = hInstance ;
	 strcpy( szCmdline,lpszCmdLine );

	 wc.cbSize        = sizeof (wc) ;
     wc.lpszClassName = szAppName ;
     wc.hInstance     = hInstance ;
     wc.lpfnWndProc   = WndProc ;
     wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wc.hIcon         = LoadIcon (hInst, szAppName) ;
     wc.lpszMenuName  = szAppName ;
     wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
     wc.style         = 0 ;
     wc.cbClsExtra    = 0 ;
     wc.cbWndExtra    = 0 ;
	 wc.hIconSm       = LoadIcon (hInst, szAppName) ;

     RegisterClassEx (&wc) ;

     wc.lpszClassName = "ClientWndProc" ;
     wc.hInstance     = hInstance ;
     wc.lpfnWndProc   = ClientWndProc ;
     wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wc.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
     wc.lpszMenuName  = NULL ;
     wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1) ;
     wc.style         = 0 ;
     wc.cbClsExtra    = 0 ;
     wc.cbWndExtra    = 0 ;
	 wc.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

     RegisterClassEx (&wc) ;


	GetModuleFileName( NULL,szExeDir, _MAX_PATH );
	a=strlen(szExeDir)-1;
	while(1) {
		a1=szExeDir[a];if (a1=='\\') break;
		a--;
	}
	szExeDir[a]=0;

	//		DLLを初期化

	strcpy( szDllDir, szExeDir );
	strcat( szDllDir, DEFAULT_DLL );
	a=dll_ini( szDllDir );
	if (a!=1) {
#ifdef JPMSG
		wsprintf( szCmdline,"%sが見つかりませんでした。",szDllDir );
#else
		wsprintf( szCmdline,"%s not found.",szDllDir );
#endif
		MessageBox( NULL, szCmdline, "Startup error", MB_OK | MB_ICONEXCLAMATION) ;
		dll_bye(); return -1;
	}

	//		XPのチェック

	flag_xpstyle = CheckXPEditBox( hInstance );

	//		Window設定

	flg_toolbar=1;flg_statbar=1;
	winflg=0;
	posx=CW_USEDEFAULT;
	posy=CW_USEDEFAULT;
	winx=CW_USEDEFAULT;
	winy=CW_USEDEFAULT;
	reg_load();

    hwnd = CreateWindowEx( 0L,
                           szAppName, szAppName,
                           WS_OVERLAPPEDWINDOW,  
                           posx, posy, winx, winy,
                           NULL, NULL, hInstance, NULL) ;

	hWndMain = hwnd;

	//		起動ディレクトリ設定
	//
	switch( startflag ) {
	case STARTDIR_MYDOC:
	 	GetSpecialFolderPath( hwnd, CSIDL_PERSONAL, szStartDir );
		break;
	case STARTDIR_USER:
		strcpy( szStartDir, startdir );
		break;
	default:
		szStartDir[0] = 0;
		break;
	}
	if ( szStartDir[0] == 0 ) {
		_getcwd( szStartDir, _MAX_PATH );
	} else {
		_chdir( szStartDir );
	}
	
	scmd=SW_SHOW;
	 if (winflg&1) scmd=SW_SHOWMAXIMIZED;
	 //if (winflg&2) scmd=SW_SHOWMINIMIZED;

     ShowWindow ( hwnd, scmd ) ;
     UpdateWindow (hwnd) ;

    hAccel = LoadAccelerators (hInstance, szAppName) ;

	poppad_reload();

	//	main loop

    while (GetMessage (&msg, NULL, 0, 0))
    {
      if (hDlgModeless == NULL || !IsDialogMessage (hDlgModeless, &msg))
      {
      if (!TranslateAccelerator (hwnd, hAccel, &msg))
           {
           TranslateMessage (&msg) ;
           DispatchMessage (&msg) ;
           }
       }
	}

	dll_bye();							// DLLを開放
	return msg.wParam ;
}

//-------------------------------------------------------------------
int CheckWndFlag ( void )
{
	winflg=0;
	if ( IsZoomed( hWndMain ) ) winflg|=1;
	if ( IsIconic( hWndMain ) ) winflg|=2;
	return winflg;
}

//-------------------------------------------------------------------
void MenuCheckMark (HMENU hmenu, int id, BOOL bCheck)
     {
     int iState ;
     iState = (bCheck) ? MF_CHECKED : MF_UNCHECKED ;
     CheckMenuItem (hmenu, id, iState) ;
     }

//-------------------------------------------------------------------
LRESULT CALLBACK
WndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam) 
     {
     switch (mMsg)
          {
          case WM_CREATE :
               {
			   pophwnd(hwnd);

               // Create toolbar (source resides in toolbar.c).
               hwndToolBar = InitToolBar (hwnd,hInst) ;

               // Create status bar (source resides in statbar.c).
               hwndStatusBar = InitStatusBar (hwnd) ;

			   if (flg_toolbar==0) ShowWindow (hwndToolBar, SW_HIDE) ;
			   if (flg_statbar==0) ShowWindow (hwndStatusBar, SW_HIDE) ;

               // Create client window (contains notify list).
               hwndClient = CreateWindowEx (WS_EX_CLIENTEDGE,
                                 "ClientWndProc", NULL,
                                 WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
                                 hwnd, (HMENU) 4, hInst, NULL) ;
               return 0 ;
               }

		  case WM_INITMENUPOPUP :
				return poppad_menupop( wParam,lParam );

          case WM_SETFOCUS :
               SetFocus (hwndClient) ;
               return 0 ;

          case WM_COMMAND :
               {
               // Menu item commands
               switch (LOWORD (wParam))
                    {
                     // Toggle display of toolbar
                    case IDM_VIEW_TOOLBAR :
                         {
                         RECT r ;

                         if (hwndToolBar && IsWindowVisible (hwndToolBar))
                              {
                              ShowWindow (hwndToolBar, SW_HIDE) ;
							  flg_toolbar=0;
                              }
                         else
                              {
                              ShowWindow (hwndToolBar, SW_SHOW) ;
							  flg_toolbar=1;
                              }

                         // Resize other windows.
                         GetClientRect (hwnd, &r) ;
                         PostMessage (hwnd, WM_SIZE, 0, 
                                      MAKELPARAM (r.right, r.bottom)) ;

                         break;
                         }

                    // Toggle display of status bar
                    case IDM_VIEW_STATUS :
                         {
                         RECT r;

                         if (hwndStatusBar && IsWindowVisible (hwndStatusBar))
                              {
                              ShowWindow (hwndStatusBar, SW_HIDE) ;
							  flg_statbar=0;
                              }
                         else
                              {
                              ShowWindow (hwndStatusBar, SW_SHOW) ;
							  flg_statbar=1;
                              }

                         // Resize other windows.
                         GetClientRect (hwnd, &r) ;
                         PostMessage (hwnd, WM_SIZE, 0, 
                                      MAKELPARAM (r.right, r.bottom)) ;

                         break;
                         }

					case IDM_HSCROLL:
						poppad_setsb(2);
						break;

					default:
		               return (EditProc (hwnd, mMsg, wParam, lParam)) ;

                     }

               return 0;
               }

          case WM_INITMENU :
               {
               BOOL bCheck ;
               HMENU hmenu = (HMENU) wParam ;

               // View menu items.
               bCheck = IsWindowVisible (hwndToolBar) ;
               MenuCheckMark (hmenu, IDM_VIEW_TOOLBAR, bCheck) ;
            
               bCheck = IsWindowVisible (hwndStatusBar) ;
               MenuCheckMark (hmenu, IDM_VIEW_STATUS, bCheck) ;

				bCheck=poppad_setsb(-1);
				MenuCheckMark (hmenu, IDM_HSCROLL, bCheck) ;

               return 0 ;
               }

          case WM_CLOSE :
          case WM_QUERYENDSESSION :
			   return poppad_term( mMsg );

          case WM_MENUSELECT :
               return Statusbar_MenuSelect (hwnd, wParam, lParam) ;

          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;

          case WM_NOTIFY :
               {
               LPNMHDR pnmh = (LPNMHDR) lParam ;
               int idCtrl = (int) wParam ;

               // Display notification details in notify window
               //DisplayNotificationDetails (wParam, lParam) ;

               // Toolbar notifications
               if ((pnmh->code >= TBN_LAST) &&
                   (pnmh->code <= TBN_FIRST))
                    {
                    return ToolBarNotify (hwnd, wParam, lParam) ;
                    }
            
               // Fetch tooltip text
               if (pnmh->code == TTN_NEEDTEXT)
                    {
                    LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT) lParam ;
                    CopyToolTipText (lpttt) ;
                    }

               return 0 ;
               }

          case WM_WINDOWPOSCHANGING :
               {
			   WINDOWPOS *wp;
				   wp=(WINDOWPOS *)lParam;
				   if ((wp->cy)&&(wp->cy)) {
					  posx=wp->x;posy=wp->y;
					  winx=wp->cx;winy=wp->cy;
				   }
			   }
			   break;

          case WM_SIZE :
               {
               int cx = LOWORD (lParam) ;
               int cy = HIWORD (lParam) ;
               int cyStatus ;
               int cyTB ;
               int x, y ;
               DWORD dwStyle ;
               RECT rWindow ;

               // Ignore size message to allow auto-move and auto-size
               // features to be more clearly seen.
               if (bIgnoreSize)
                    return 0 ;

               // Adjust toolbar size.
               if (IsWindowVisible (hwndToolBar))
                    {
                    dwStyle = GetWindowLong (hwndToolBar, GWL_STYLE) ;
                    if (dwStyle & CCS_NORESIZE)
                         {
                         MoveWindow (hwndToolBar, 
                                     0, 0, cx, cyToolBar, FALSE) ;
                         }
                    else
                         {
                         ToolBar_AutoSize (hwndToolBar) ;
                         }
                    InvalidateRect (hwndToolBar, NULL, TRUE) ;
                    GetWindowRect (hwndToolBar, &rWindow) ;
                    ScreenToClient (hwnd, (LPPOINT) &rWindow.left) ;
                    ScreenToClient (hwnd, (LPPOINT) &rWindow.right) ;
                    cyTB = rWindow.bottom - rWindow.top ;
                    } 
               else
                    {
                    cyTB = 0 ;
                    }

               // Adjust status bar size.
               if (IsWindowVisible (hwndStatusBar))
                    {
                    GetWindowRect (hwndStatusBar, &rWindow) ;
                    cyStatus = rWindow.bottom - rWindow.top ;
                    MoveWindow (hwndStatusBar, 0, cy - cyStatus, 
                                              cx, cyStatus, TRUE) ;
                    }
               else
                    {
                    cyStatus = 0 ;
                    }

               // Adjust client window size.
               x = 0 ;
               y = cyTB ;
               cy = cy - (cyStatus + cyTB) ;
               MoveWindow (hwndClient, x, y, cx, cy, TRUE) ;
               return 0;
               }

          default:
               return (EditDefProc (hwnd, mMsg, wParam, lParam)) ;
          }
     return DefWindowProc (hwnd, mMsg, wParam, lParam) ;
     }

//-------------------------------------------------------------------

static COLORREF crBack ;
static COLORREF crText ;
static HBRUSH hbr=NULL ;

void ConfigEditColor( COLORREF fore, COLORREF back )
{
	crText=fore; crBack=back;
	if (hbr!=NULL) DeleteObject( hbr );
    hbr = CreateSolidBrush (crBack) ;
}


LRESULT CALLBACK 
ClientWndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
     {
	HWND hwndNotify;

     switch (mMsg)
          {
          case WM_CREATE :
			   poppad_ini( hwnd,lParam );
               return 0 ;

          case WM_DESTROY :
               DeleteObject (hbr) ;
			   poppad_bye();
               return 0 ;

          case WM_CTLCOLOREDIT :
               {
               DefWindowProc (hwnd, mMsg, wParam, lParam) ;
               SetBkColor ((HDC) wParam, crBack) ;
               //SetBkMode ((HDC) wParam, TRANSPARENT) ;
			   SetTextColor( (HDC) wParam, crText );
               return (LRESULT) (HBRUSH) hbr ;
               }

          case WM_SETFOCUS :
               hwndNotify = GetWindow (hwnd, GW_CHILD) ;
               SetFocus (hwndNotify) ;
               return 0 ;

          case WM_SIZE :
               {
               hwndNotify = GetWindow (hwnd, GW_CHILD) ;
               int cx = LOWORD (lParam) ;
               int cy = HIWORD (lParam) ;

               // Ignore if notification window is absent.
               if (hwndNotify != NULL)
                    {
                    MoveWindow (hwndNotify, 0, 0, cx, cy, TRUE) ;
                    }

               return 0 ;
               }

          default :
               return (EditProc (hwnd, mMsg, wParam, lParam)) ;
               //return (DefWindowProc (hwnd, mMsg, wParam, lParam)) ;
          }
     }
