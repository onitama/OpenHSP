/*----------------------------------------
   TOOLBAR.C -- Toolbar helper functions.
                (c) Paul Yao, 1996
  ----------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include "comcthlp.h"
#include "poppad.h"
#include "resource.h"

// Tooltip functions.
BOOL InitToolTip (HWND hwndToolBar, HWND hwndComboBox) ;

HWND hwndCombo ;
HWND hwndEdit ;
HWND hwndToolTip ;
HWND hwndTB ;

HINSTANCE instbak;

BOOL bComboBox   = FALSE ;
BOOL bStrings    = FALSE ;
BOOL bLargeIcons = FALSE ;

DWORD dwToolBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                        CCS_TOP | CCS_NODIVIDER | TBSTYLE_TOOLTIPS ;

extern HINSTANCE hInst ;

#ifdef JPNMSG
char szTbStrings[] = "新規\0ロード\0セーブ\0"
                     "切り取り\0コピー\0貼り付け\0元に戻す\0やり直し\0"
					 "検索\0ＨＳＰ実行\0オブジェクト作成\0外部ファイル実行\0";
#else
char szTbStrings[] = "New\0Open\0Save\0"
                     "Cut\0Copy\0Paste\0Undo\0Redo\0"
					 "Find\0Run\0Make Object\0External file\0";
#endif

TBBUTTON tbb[] = 
     {
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,
     0, IDM_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 0,
     1, IDM_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 1,
     2, IDM_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 2,
 
     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,

     3, IDM_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 3,
     4, IDM_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 4,
     5, IDM_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 
        0, 0, 0, 5,

     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,


     6, IDM_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 6,
	 7, IDM_REDO, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 7,
     8, IDM_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 8,

     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,

     9, IDM_COMP, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 9,
     10, IDM_MKOBJ, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 10,
     11, IDM_COMP3, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 11
     } ;

int nCust[] = { 21, 22, 23, 24, 25, 27, 28, 29, 30, 31, 32, 33, 34, -1} ;

TBADDBITMAP tbStdLarge[] = 
     {
     HINST_COMMCTRL, IDB_STD_LARGE_COLOR,
     } ;

TBADDBITMAP tbStdSmall[] = 
     {
     HINST_COMMCTRL, IDB_STD_SMALL_COLOR,
     } ;

//-------------------------------------------------------------------
LPSTR GetString (int iString)
     {
     int i, cb ;
     LPSTR pString ;

     // Cycle through to requested string
     pString = szTbStrings ;
     for (i = 0 ; i < iString ; i++)
          {
          cb = lstrlen (pString) ;
          pString += (cb + 1) ;
          }

     return pString ;
     }

//-------------------------------------------------------------------
LRESULT ToolBarNotify (HWND /*hwnd*/, WPARAM /*wParam*/, LPARAM lParam)
     {
     LPNMHDR pnmh = (LPNMHDR) lParam ;
//     int idCtrl = (int) wParam ;

     // Allow toolbar to be customized
     if ((pnmh->code == TBN_QUERYDELETE) ||
         (pnmh->code == TBN_QUERYINSERT))
          {
          return 1 ; // We always say "yes"
          }

     // Provide details of allowable toolbar buttons
     if (pnmh->code == TBN_GETBUTTONINFO)
          {
          LPTBNOTIFY ptbn = (LPTBNOTIFY) lParam ;
          int iButton = nCust[ptbn->iItem] ;

          if (iButton != -1)
               {
               lstrcpy (ptbn->pszText, GetString (ptbn->iItem)) ;
               memcpy (&ptbn->tbButton, &tbb[iButton], sizeof (TBBUTTON)) ;
               return 1 ;
               }
          }

     return 0 ;
     }

//-------------------------------------------------------------------
HWND InitToolBar (HWND hwndParent, HINSTANCE hInst )
     {
     int  iNumButtons ;
     LPTBBUTTON ptbb ;

	 instbak=hInst;
     ptbb = &tbb[21] ;
     iNumButtons = 15 ;

     hwndTB = CreateToolbarEx (hwndParent,
                   dwToolBarStyles,
                   1, 15,
                   hInst,
                   IDB_TOOLBAR,
                   ptbb,
                   iNumButtons,
                   0, 0, 0, 0,
                   sizeof (TBBUTTON)) ;

     // If requested, add to string list
     if (bStrings)
          ToolBar_AddString (hwndTB, 0, szTbStrings) ;

     // Store handle to tooltip control
     hwndToolTip = ToolBar_GetToolTips (hwndTB) ;

	 // Set flat style to toolbar
	 SetWindowLong(hwndTB, GWL_STYLE, GetWindowLong(hwndTB, GWL_STYLE) | TBSTYLE_FLAT);

	 return hwndTB ;
     }

//-------------------------------------------------------------------
