/*-------------------------------------------
   STATBAR.C -- Status bar helper functions.
                (c) Paul Yao, 1996
  -------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include "comcthlp.h"
#include "poppad.h"

typedef struct tagPOPUPSTRING
     {
     HMENU hMenu ;
	 UINT uiString ;
     } POPUPSTRING ;

#define MAX_MENUS 5

static POPUPSTRING popstr[MAX_MENUS] ;

DWORD dwStatusBarStyles = WS_CHILD | WS_VISIBLE | 
                          WS_CLIPSIBLINGS | CCS_BOTTOM |
                          SBARS_SIZEGRIP ;
extern HINSTANCE hInst ;
extern HWND hwndStatusBar ;

//-------------------------------------------------------------------
HWND InitStatusBar (HWND hwndParent)
     {
     HWND hwndSB ;

     // Initialize values for WM_MENUSELECT message handling
     HMENU hMenu = GetMenu (hwndParent) ;
     HMENU hMenuTB = GetSubMenu (hMenu, 2) ;
     HMENU hMenuSB = GetSubMenu (hMenu, 3) ;
     popstr[0].hMenu    = 0 ;
     popstr[0].uiString = 0 ;
     popstr[1].hMenu    = hMenu ;
     popstr[1].uiString = IDS_MAIN_MENU ;
     popstr[2].hMenu    = hMenuTB ;
     popstr[2].uiString = IDS_TOOLBAR_MENU ;
     popstr[3].hMenu    = hMenuSB ;
     popstr[3].uiString = IDS_STATUSBAR_MENU ;
     popstr[4].hMenu    = 0 ;
     popstr[4].uiString = 0 ;

     hwndSB = CreateStatusWindow (dwStatusBarStyles,
                                  "Ready",
                                  hwndParent,
                                  2) ;
     return hwndSB ;
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
HWND RebuildStatusBar (HWND hwndParent, WORD wFlag)
     {
     HWND hwndSB ;
     RECT r ;

     hwndSB = InitStatusBar (hwndParent) ;

     // Post parent a WM_SIZE message to resize children
     GetClientRect (hwndParent, &r) ;
     PostMessage (hwndParent, WM_SIZE, 0, 
                  MAKELPARAM (r.right, r.bottom)) ;

     return hwndSB ;
     }


//-------------------------------------------------------------------
void StatusBarMessage (HWND hwndSB, WORD wMsg)
     {
     switch (wMsg)
          {
          case IDM_ST_GETBORDERS :
               {
               char ach[180] ;
               int aiBorders[3] ;

               Status_GetBorders (hwndSB, &aiBorders) ;
               wsprintf (ach, "Horiz Width = %d\n"
                         "Vert Width = %d\n"
                         "Separator Width = %d",
                         aiBorders[0], aiBorders[1],
                         aiBorders[2]) ;
               MessageBox (GetParent (hwndSB), ach, 
                           "SB_GETBORDERS", MB_OK) ;
               break ;
               }

          case IDM_ST_GETPARTS :
               {
               char ach[80] ;
               int nParts = Status_GetParts (hwndSB, 0, 0) ;
               wsprintf (ach, "Part Count = %d", nParts) ;
               MessageBox (GetParent (hwndSB), ach, 
                           "SB_GETPARTS", MB_OK) ;
               break ;
               }

          case IDM_ST_SETTEXT :
               Status_SetText (hwndSB, 0, 0, 
                               "SB_SETTEXT Message Sent") ;
               break;

          case IDM_ST_SIMPLE :
               {
               static BOOL bSimple = TRUE ;
               Status_Simple (hwndSB, bSimple) ;
               bSimple = (!bSimple) ;
               break ;
               }
          }
     }

//-------------------------------------------------------------------

void Statusbar_mes( char *mes )
{
	Status_SetText ( hwndStatusBar, 0, 0, mes ) ;
}

//-------------------------------------------------------------------
LRESULT 
Statusbar_MenuSelect (HWND hwnd, WPARAM wParam, LPARAM lParam)
     {
     UINT fuFlags = (UINT) HIWORD (wParam) ;
     HMENU hMainMenu = NULL ;
     int iMenu = 0 ;

     // Handle non-system popup menu descriptions.
     if ((fuFlags & MF_POPUP) &&
         (!(fuFlags & MF_SYSMENU)))
          {
          for (iMenu = 1 ; iMenu < MAX_MENUS ; iMenu++)
               {
               if ((HMENU) lParam == popstr[iMenu].hMenu)
                    {
                    hMainMenu = (HMENU) lParam ;
                    break ;
                    }
               }
          }

     // Display helpful text in status bar
     MenuHelp (WM_MENUSELECT, wParam, lParam, hMainMenu, hInst, 
               hwndStatusBar, (UINT *) &popstr[iMenu]) ;

     return 0 ;
     }
