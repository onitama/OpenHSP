
/*----------------------
   POPPAD.H header file
  ----------------------*/

#include <windows.h>
#include "hsed_config.h"

// Resource definitions.
#define IDM_VIEW_NOTIFICATIONS          2902
#define IDM_COMBOBOX                    2903
#define IDI_APP                         101    // -- Icons --
#define IDS_MAIN_MENU                   71     // -- Strings --
#define IDS_MAIN_MENU1                  72
#define IDS_MAIN_MENU2                  73
#define IDS_MAIN_MENU3                  74
#define IDS_MAIN_MENU4                  75
#define IDS_MAIN_MENU5                  76
#define IDS_MAIN_MENU6                  77
#define IDS_MAIN_MENU7                  78
#define IDS_TOOLBAR_MENU                80
#define IDS_TOOLBAR_MENU1               81
#define IDS_TOOLBAR_MENU2               82
#define IDS_TOOLBAR_MENU3               83
#define IDS_STATUSBAR_MENU              90
#define IDS_STATUSBAR_MENU1             91
#define IDS_STATUSBAR_MENU2             92
#define IDS_STATUSBAR_MENU3             93
#define IDM_IGNORESIZE                  300
#define IDM_STRINGS                     301
#define IDM_LARGEICONS                  302
#define IDM_SMALLICONS                  303
#define IDM_NODIVIDER                   400
#define IDM_WRAPABLE                    401
#define IDM_TOP                         402
#define IDM_BOTTOM                      403
#define IDM_NOMOVEY                     404
#define IDM_NOPARENTALIGN               405
#define IDM_NORESIZE                    406
#define IDM_ADJUSTABLE                  407
#define IDM_ALTDRAG                     408
#define IDM_TOOLTIPS                    409
#define IDM_TB_CHECK                    500
#define IDM_TB_ENABLE                   501
#define IDM_TB_HIDE                     502
#define IDM_TB_INDETERMINATE            503
#define IDM_TB_PRESS                    504
#define IDM_TB_BUTTONCOUNT              505
#define IDM_TB_GETROWS                  506
#define IDM_TB_CUSTOMIZE                507

#define IDM_ST_GETBORDERS               800
#define IDM_ST_GETPARTS                 801
#define IDM_ST_SETTEXT                  802
#define IDM_ST_SIMPLE                   803

#define IDC_TB_COMBOBOX                 4044  // -- Toolbar combo box

#define PM_APPLY        WM_APP
#define PM_ISAPPLICABLE WM_APP + 1
#define PM_SETDEFAULT   WM_APP + 2

#define POS_TABBASE                     6
#define POS_EXTTOOL                     7
#define POS_WINDOW                      8
#define POS_TOOLPOPUPSEP                14
#define POS_TOOLPOPUPBASE               (POS_TOOLPOPUPSEP + 1)
#define POS_TOOLMAINBASE                0

#define STARTDIR_MYDOC 0
#define STARTDIR_USER 1
#define STARTDIR_NONE 2

// Poppad.cpp functions

int poppad_ini( HWND hwnd, LPARAM lParam );
void poppad_bye( void );
int poppad_reload( int );
int poppad_menupop( WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK EditProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditDefProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void pophwnd( HWND hwnd );
LRESULT poppad_term( UINT iMsg );
int poppad_setsb( int flag );
void poppad_setsb_current( int FootyID );
void PutLineNumber( void );

// DLL functions

int dll_ini( char *libname );
void dll_bye( void );
