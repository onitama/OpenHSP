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

int cyToolBar ;

BOOL bComboBox   = FALSE ;
BOOL bStrings    = FALSE ;
BOOL bLargeIcons = FALSE ;

DWORD dwToolBarStyles = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                        CCS_TOP | CCS_NODIVIDER | TBSTYLE_TOOLTIPS ;

extern HINSTANCE hInst ;

#ifdef JPMSG
char szTbStrings[] = "新規\0ロード\0セーブ\0"
                     "カット\0コピー\0ペースト\0アンドゥ\0"
					 "検索\0ＨＳＰ実行\0オブジェクト作成\0外部ファイル実行\0";
#else
char szTbStrings[] = "New\0Open\0Save\0"
                     "Cut\0Copy\0Paste\0Undo\0"
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
     7, IDM_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 7,

     0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, 0,

     8, IDM_COMP, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 8,
     9, IDM_MKOBJ, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 9,
     10, IDM_COMP3, TBSTATE_ENABLED, TBSTYLE_BUTTON,
        0, 0, 0, 10
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
LRESULT ToolBarNotify (HWND hwnd, WPARAM wParam, LPARAM lParam)
     {
     LPNMHDR pnmh = (LPNMHDR) lParam ;
     int idCtrl = (int) wParam ;

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
     //UINT uiBitmap;

	 instbak=hInst;
     ptbb = &tbb[21] ;
     iNumButtons = 14 ;

/*
     if (bLargeIcons) uiBitmap=IDB_STD_LARGE_COLOR;
                 else uiBitmap=IDB_STD_SMALL_COLOR;

     hwndTB = CreateToolbarEx (hwndParent,
                   dwToolBarStyles,
                   1, 15,
                   HINST_COMMCTRL,
                   uiBitmap,
                   ptbb,
                   iNumButtons,
                   0, 0, 0, 0,
                   sizeof (TBBUTTON)) ;
*/

     hwndTB = CreateToolbarEx (hwndParent,
                   dwToolBarStyles,
                   1, 15,
                   hInst,
                   IDB_BITMAP1,
                   ptbb,
                   iNumButtons,
                   0, 0, 0, 0,
                   sizeof (TBBUTTON)) ;

     // If requested, add to string list
     if (bStrings)
          ToolBar_AddString (hwndTB, 0, szTbStrings) ;

     // Store handle to tooltip control
     hwndToolTip = ToolBar_GetToolTips (hwndTB) ;

     return hwndTB ;
     }

//-------------------------------------------------------------------
void static FlipStyleFlag (LPDWORD dwStyle, DWORD flag)
     {
     if (*dwStyle & flag)  // Flag on -- turn off
          {
          *dwStyle &= (~flag) ;
          }
     else                  // Flag off -- turn on
          {
          *dwStyle |= flag ;
          }
     }

//-------------------------------------------------------------------
HWND RebuildToolBar (HWND hwndParent, WORD wFlag)
     {
     HWND hwndTB ;
     RECT r ;

     switch (wFlag)
          {
          case IDM_STRINGS :
               bStrings = !bStrings ;
               break ;

          case IDM_LARGEICONS :
               bLargeIcons = TRUE ;
               break ;

          case IDM_SMALLICONS :
               bLargeIcons = FALSE ;
               break ;

          case IDM_NODIVIDER :
               FlipStyleFlag (&dwToolBarStyles, CCS_NODIVIDER) ;
               break ;

          case IDM_WRAPABLE :
               FlipStyleFlag (&dwToolBarStyles, TBSTYLE_WRAPABLE) ;
               break ;

          case IDM_TOP :
               dwToolBarStyles &= 0xFFFFFFFC ;
               dwToolBarStyles |= CCS_TOP ;
               break ;

          case IDM_BOTTOM :
               dwToolBarStyles &= 0xFFFFFFFC ;
               dwToolBarStyles |= CCS_BOTTOM ;
               break ;

          case IDM_NOMOVEY :
               dwToolBarStyles &= 0xFFFFFFFC ;
               dwToolBarStyles |= CCS_NOMOVEY ;
               break ;

          case IDM_NOPARENTALIGN :
               FlipStyleFlag (&dwToolBarStyles, CCS_NOPARENTALIGN) ;
               break ;

          case IDM_NORESIZE :
               FlipStyleFlag (&dwToolBarStyles, CCS_NORESIZE) ;
               break ;

          case IDM_ADJUSTABLE :
               FlipStyleFlag (&dwToolBarStyles, CCS_ADJUSTABLE) ;
               break ;

          case IDM_ALTDRAG :
               FlipStyleFlag (&dwToolBarStyles, TBSTYLE_ALTDRAG) ;
               break ;

          case IDM_TOOLTIPS :
               FlipStyleFlag (&dwToolBarStyles, TBSTYLE_TOOLTIPS) ;
               break ;

          case IDM_COMBOBOX :
               bComboBox = (!bComboBox) ;
          }

     hwndTB = InitToolBar (hwndParent,instbak) ;

     // Post parent a WM_SIZE message to resize children
     GetClientRect (hwndParent, &r) ;
     PostMessage (hwndParent, WM_SIZE, 0, 
                  MAKELPARAM (r.right, r.bottom)) ;

     return hwndTB ;
     }


//-------------------------------------------------------------------
void ToolBarMessage (HWND hwndTB, WORD wMsg)
     {
     switch (wMsg)
          {
          case IDM_TB_CHECK :
               {
               int nState = ToolBar_GetState (hwndTB, 1) ;
               BOOL bCheck = (!(nState & TBSTATE_CHECKED)) ;
               ToolBar_CheckButton (hwndTB, 1, bCheck ) ;
               break ;
               }

          case IDM_TB_ENABLE :
               {
               int nState = ToolBar_GetState (hwndTB, 2) ;
               BOOL bEnabled = (!(nState & TBSTATE_ENABLED)) ;
               ToolBar_EnableButton (hwndTB, 2, bEnabled) ;
               break ;
               }

          case IDM_TB_HIDE :
               {
               int nState = ToolBar_GetState (hwndTB, 3) ;
               BOOL bShow = (!(nState & TBSTATE_HIDDEN)) ;
               ToolBar_HideButton (hwndTB, 3, bShow) ;
               break ;
               }

          case IDM_TB_INDETERMINATE :
               {
               int nState = ToolBar_GetState (hwndTB, 4) ;
               BOOL bInd = (!(nState & TBSTATE_INDETERMINATE)) ;
               ToolBar_Indeterminate (hwndTB, 4, bInd) ;
               break ;
               }

          case IDM_TB_PRESS :
               {
               int nState = ToolBar_GetState (hwndTB, 5) ;
               BOOL bPress = (!(nState & TBSTATE_PRESSED)) ;
               ToolBar_PressButton (hwndTB, 5, bPress) ;
               break ;
               }

          case IDM_TB_BUTTONCOUNT :
               {
               int nButtons = ToolBar_ButtonCount (hwndTB) ;
               char ach[80] ;
               wsprintf (ach, "Button Count = %d", nButtons) ;
               MessageBox (GetParent (hwndTB), ach, 
                           "TB_BUTTONCOUNT", MB_OK) ;
               break ;
               }

          case IDM_TB_GETROWS :
               {
               int nRows = ToolBar_GetRows (hwndTB) ;
               char ach[80] ;
               wsprintf (ach, "Row Count = %d", nRows) ;
               MessageBox (GetParent (hwndTB), ach, 
                           "TB_GETROWS", MB_OK) ;
               break ;
               }
          case IDM_TB_CUSTOMIZE :
               // ToolBar_Customize (hwndTB) ;
               SendMessage (hwndTB, TB_CUSTOMIZE, (LPARAM) &tbb[25], 5) ;
               break ;
          }
     }
