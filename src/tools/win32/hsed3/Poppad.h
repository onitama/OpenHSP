
/*----------------------
   POPPAD.H header file
  ----------------------*/

//		when Japanese mode
#define JPMSG

#define DEFAULT_RUNTIME "hsp3.exe"			// HSP3 runtime
#define DEFAULT_DLL "\\hspcmp.dll"			// compiler DLL


#define IDM_NEW          110
#define IDM_OPEN         111
#define IDM_SAVE         112
#define IDM_SAVEAS       113
#define IDM_PRINT        114
#define IDM_EXIT         115

#define IDM_UNDO         120
#define IDM_CUT          121
#define IDM_COPY         122
#define IDM_PASTE        123
#define IDM_CLEAR        124
#define IDM_SELALL       125
#define IDM_DEL          126

#define IDM_FIND         130
#define IDM_NEXT         131
#define IDM_REPLACE      132

#define IDM_FONT         140
#define IDM_HSCROLL      141
#define IDM_BGCOLOR      142

#define IDM_HELP         150
#define IDM_ABOUT        151
#define IDM_KWHELP       152

#define IDM_LTOP         160
#define IDM_LBTM         161
#define IDM_JUMP         162

#define IDM_COMP         170
#define IDM_RUN          171
#define IDM_COMP2        172
#define IDM_HSPERR       173
#define IDM_MKOBJ        174

#define IDD_FNAME        10

// Resource definitions.
#define IDM_VIEW_TOOLBAR                1000
#define IDM_VIEW_STATUS                 1001
#define IDM_VIEW_NOTIFICATIONS          1002
#define IDM_COMBOBOX                    4000
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

#define IDC_TB_COMBOBOX                 2000  // -- Toolbar combo box


#define STARTDIR_MYDOC 0
#define STARTDIR_USER 1
#define STARTDIR_NONE 2

// Poppad.cpp functions

int poppad_ini( HWND hwnd, LPARAM lParam );
void poppad_bye( void );
void poppad_reload( void );
int poppad_menupop( WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK EditProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditDefProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void pophwnd( HWND hwnd );
LRESULT poppad_term( UINT iMsg );
int poppad_setsb( int flag );
void PutLineNumber( void );

// DLL functions

int dll_ini( char *libname );
void dll_bye( void );
