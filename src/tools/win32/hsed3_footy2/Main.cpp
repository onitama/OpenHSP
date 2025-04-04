
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
#include "resource.h"
#include "tabmanager.h"
#include "support.h"
#include "config.h"
#include "interface.h"
#include "exttool.h"
#include "membuf.h"
#include "supio.h"

#include <crtdbg.h>

#pragma comment( lib,"comctl32.lib" )
#pragma comment( lib,"htmlhelp.lib" )

#define FOOTYSTATIC
#include "Footy2.h"
#pragma comment( lib,"Footy2.lib" )
#pragma comment( lib,"imm32.lib" )

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK ClientWndProc (HWND, UINT, WPARAM, LPARAM) ;

extern BOOL      bNeedSave ;
extern char      szDirName[_MAX_PATH] ;
extern char      szFileName[_MAX_PATH] ;
extern char      szTitleName[_MAX_FNAME + _MAX_EXT] ;

char szStartDir[_MAX_PATH];
char szExeDir[_MAX_PATH];
char szDllDir[_MAX_PATH];
char szAppName[]  = "onipad" ;
int	 winflg,winx,winy,posx,posy,flg_toolbar,flg_statbar,flg_hspat;
//int  flag_xpstyle;
HWND hWndMain;
HWND hwndConfigDlg = NULL;

int	startflag;
char startdir[_MAX_PATH];

BOOL bIgnoreSize = FALSE ;
HINSTANCE hInst ;
HMENU hMenu ;
HMENU hMenu2 ;
HMENU hMenu_context;
HWND hwndOwner = NULL ;
HWND hwndClient = NULL ;
HWND hwndToolBar = NULL ;
HWND hwndStatusBar = NULL ;
HWND hwndNotify = NULL ;
HWND hwndTab = NULL ;
HWND hwndDummy = NULL ;

extern DWORD dwStatusBarStyles ;
extern TABINFO TabInfo ;
extern int activeID ;
extern int activeFootyID ;

extern HWND hConfigDlg ;
extern HWND hConfigPage ;
extern HWND hDlgModeless ;

HWND hwndFindReplace;
UINT iMsgNotifyFindReplaceWindowHandle;	// ウィンドウハンドル通知

HMENU hSubMenu ;
static HMENU hSubMenu2 ;
static int EzMenuId;
CMemBuf *AhtMenuBuf;
extern int ClickID;
static int cyToolBar ;
CMemBuf* ToolMenuBuf;

extern int autobackup;
int AutoBackupTimer;

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

// Poppad functions.
void DoCaption (char *, int);
void LoadFromCommandLine(char *);

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
	for(;;) {
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
	for(;;) {
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
int CALLBACK EnumWindowsProc(HWND hWnd, LPARAM /*lParam*/)
{
	COPYDATASTRUCT cds;
	char *lpCmdLine;

	if(GetProp(hWnd, PROP_NAME)){
		// コマンドラインを既存のウィンドウに送信
		// Send command-line to 
		lpCmdLine = GetCommandLine();
		cds.dwData = 0;
		cds.lpData = (void *)lpCmdLine;
		cds.cbData = lstrlen(lpCmdLine) + 1;
		SendMessage(hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);

		// ウィンドウをアクティブにしてフォーカスを移す
		// Activate window and set focus to window.
		if(IsMinimized(hWnd))
			OpenIcon(hWnd);
		SetForegroundWindow(hWnd);
		return FALSE;
	}
	return TRUE;
}


//-------------------------------------------------------------------

int BuildEzInputMenu( HMENU menu, char *fname, char *dirname )
{
	//		かんたん入力メニュー構築
	//
	char *p;
	int fl;
	int stat_main;
	HANDLE sh;
	WIN32_FIND_DATA fd;
	DWORD fmask;
	BOOL ff;
	HMENU popmenu;
	char wname[_MAX_PATH];
	char tmp[_MAX_PATH];

	fmask=0;
	//if (p3&1) fmask|=FILE_ATTRIBUTE_DIRECTORY;
	//if (p3&2) fmask|=FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

	stat_main=0;
	sprintf( wname, "%s\\*.*", fname );

	sh=FindFirstFile( wname, &fd );
	if (sh==INVALID_HANDLE_VALUE) {
		int size = strlen(dirname) + 64;
		char *tmp = (char *)calloc(size + 1, sizeof(char));
		strcat_s(tmp, size, dirname);
#ifdef JPNMSG
		strcat_s(tmp, size, " は使用できません");
#else
		strcat_s(tmp, size, " is invalid.");
#endif
		int res = AppendMenu( menu, MF_GRAYED | MF_DISABLED, 0xFFFF, tmp );
		free(tmp);
        FindClose( sh );
		return 0;
	}

	popmenu = CreatePopupMenu();

	for(;;) {
		ff = fd.dwFileAttributes;
		p = fd.cFileName; fl = 1;
		if ( *p==0 ) fl=0;			// 空行を除外
		if ( *p=='.') {				// '.','..'を除外
			if ( p[1]==0 ) fl=0;
			if ((p[1]=='.')&&(p[2]==0)) fl=0;
		}
		if (fl) {
			if ( ff & FILE_ATTRIBUTE_DIRECTORY ) {
				sprintf( tmp, "%s\\%s", fname, p );
				BuildEzInputMenu( popmenu, tmp, p );		// 再帰で検索
			} else {
				sprintf( tmp, "%s\\%s", fname, p );
				AhtMenuBuf->Index();
				AhtMenuBuf->PutStrBlock( tmp );

				//msgboxf(NULL, "%s"	, "ADD", MB_OK | MB_ICONEXCLAMATION, p);
				getpath( p, tmp, 1 );
				AppendMenu( popmenu, MF_STRING | MF_ENABLED, EzMenuId, tmp );
				stat_main++;
				EzMenuId++;
			}
		}
		if ( !FindNextFile(sh,&fd) ) break;
	}
	FindClose(sh);

	AppendMenu(menu, MF_POPUP, (UINT_PTR)popmenu, dirname);
	return stat_main;
}


int ExecEzInputMenu( int id )
{
	char *p;
	char tmpfn[1024];
	int num,i;
	num = id - IDM_AHTEZINPUT;
	if ( num < 0 ) return -1;
	if ( num >= AhtMenuBuf->GetIndexBufferSize() ) return -1;
	i = AhtMenuBuf->GetIndex( num );
	p = AhtMenuBuf->GetBuffer() + i;
	//msgboxf(NULL, "%s"	, "EXEC", MB_OK | MB_ICONEXCLAMATION, p);

	sprintf( tmpfn, "\"%s\\ahtman.exe\" %s", szExeDir,p );
	WinExec( tmpfn, SW_SHOW );

	return 0;
}

static char	szMenuToolName[_MAX_PATH];
static char	szMenuToolExec[_MAX_PATH];
static char	szMenuToolPath[_MAX_PATH];
static int iMenuToolMax;

static int GetMenuTool(int mode)
{
	if (mode < 0) {
		strsp_ini();
		return 0;
	}
	char* p = ToolMenuBuf->GetBuffer();
	int pt = strsp_getptr();
	if (p[pt] == 0) return -1;
	strsp_get(p, szMenuToolName, ',', _MAX_PATH);
	strsp_get(p, szMenuToolExec, ',', _MAX_PATH);
	strsp_get(p, szMenuToolPath, 0, _MAX_PATH);
	return 0;
}

static void SetMenuTool(HWND hwnd)
{
	char		 tmp[_MAX_PATH];
	ToolMenuBuf = new CMemBuf;
#ifdef JPNMSG
	char* menufile = "menu.txt";
#else
	char* menufile = "menu_en.txt";
#endif
	sprintf(tmp, "%s\\support\\%s", szExeDir, menufile);
	ToolMenuBuf->PutFile(tmp);
	ToolMenuBuf->Put((int)0);

	MENUITEMINFO mii;

	HMENU hMainMenu = GetSubMenu(GetMenu(hwnd), POS_EXTTOOL-1);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_ID | MIIM_TYPE;

	iMenuToolMax = 0;
	GetMenuTool(-1);
	while (1) {
		if (GetMenuTool(0)) break;
		mii.fType = MFT_STRING;
		mii.wID = IDM_EXTHSPTOOL+ iMenuToolMax;
		mii.dwTypeData = szMenuToolName;
		InsertMenuItem(hMainMenu, POS_HSPTOOLMAINBASE+ iMenuToolMax, TRUE, &mii);
		iMenuToolMax++;
	}
}


int ExectMenuTool(int id)
{
	char tmpfn[1024];
	int num, i;
	num = id - IDM_EXTHSPTOOL;
	if (num < 0) return -1;
	if (num >= iMenuToolMax) return -1;

	GetMenuTool(-1);
	for (i = 0; i <= num; i++) {
		GetMenuTool(0);
	}

	sprintf(tmpfn, "\"%s\\%s.exe\" \"%s\\support\\%s\"", szExeDir, szMenuToolExec, szExeDir, szMenuToolPath);
	WinExec(tmpfn, SW_SHOW);

	return 0;
}


//-------------------------------------------------------------------

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, 
                    LPSTR /*lpszCmdLine*/, int /*cmdShow*/)       
     {
	 HWND        hwnd ;
     MSG         msg ;
     WNDCLASSEX  wc ;
     HACCEL      hAccel ;
	 int		 a;
	 int		 scmd;
	 char		 a1;
	 HANDLE      hMutex ;
	 char		 tmp[_MAX_PATH];

// デバッグ時無効化
#ifndef _DEBUG
	 hMutex = CreateMutex(NULL, TRUE, MUTEX_NAME);
	 if(GetLastError() == ERROR_ALREADY_EXISTS){
		 EnumWindows((WNDENUMPROC)EnumWindowsProc, NULL);
		 return 0;
	 }
#endif

	 // 検索ダイアログ用メッセージ登録
	 iMsgNotifyFindReplaceWindowHandle =
		 ::RegisterWindowMessage("NotifyFindReplaceWindowHandle");

	 InitCommonControls () ;
	 OleInitialize(NULL);
#ifdef FOOTYSTATIC
	 Footy2Start(hInstance);
#endif	/*FOOTYSTATIC*/
//	 FootySetCursor(IDC_ONLINE, IDC_ONURL);	// 2008-02-17 Shark++ 代替機能不明
	 //FootySetMetrics(0, F_SM_CREATESHOW, F_CS_HIDE, false);

	// 日本だったら日本語、それ以外はすべて英語
	//UINT localeId = GetUserDefaultLCID();
	//if(0x411 != localeId){
	//	localeId = 0x409; // 日本語以外は英語UIとして表示する
	//}
	//SetThreadUILanguage(localeId);
	//SetThreadLocale(localeId);

	 hInst = hInstance ;

	 wc.cbSize        = sizeof (wc) ;
     wc.lpszClassName = szAppName ;
     wc.hInstance     = hInstance ;
     wc.lpfnWndProc   = WndProc ;
     wc.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wc.hIcon         = LoadIcon (hInst, szAppName) ;
     wc.lpszMenuName  = szAppName ;
     wc.hbrBackground = NULL;
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
     wc.hbrBackground = NULL ;
     wc.style         = 0 ;
     wc.cbClsExtra    = 0 ;
     wc.cbWndExtra    = 0 ;
	 wc.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

     RegisterClassEx (&wc) ;

	 wc.lpszClassName = "DummyWndProc" ;
     wc.hInstance     = hInstance ;
     wc.lpfnWndProc   = DefWindowProc ;
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
	a=(int)strlen(szExeDir)-1;
	for(;;) {
		a1=szExeDir[a];if (a1=='\\') break;
		a--;
	}
	szExeDir[a]=0;

	//		DLLを初期化

	strcpy( szDllDir,szExeDir );
	strcat( szDllDir,"\\" FILE_HSPCMP );
	a=dll_ini( szDllDir );
	if (a!=1) {
		msgboxf(NULL,
#ifdef JPNMSG
		"%sが見つかりませんでした。"
#else
		"%s not found."
#endif
		, "Startup error", MB_OK | MB_ICONEXCLAMATION, szDllDir);
		dll_bye(); return -1;
	}

	//		XPのチェック

//	flag_xpstyle = CheckXPEditBox( hInstance );

	//		Window設定

	flg_toolbar=1;flg_statbar=1;
	winflg=0;
	posx=CW_USEDEFAULT;
	posy=CW_USEDEFAULT;
	winx=CW_USEDEFAULT;
	winy=CW_USEDEFAULT;
    LoadConfig();

	hwndDummy = CreateWindow("DummyWndProc", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
		NULL, NULL, hInstance, NULL);
    hwnd = CreateWindowEx( 0L,
                           szAppName, szAppName,
                           WS_OVERLAPPEDWINDOW,  
                           posx, posy, winx, winy,
                           NULL, NULL, hInstance, NULL) ;

	hWndMain = hwnd;
	SetProp(hWndMain, PROP_NAME, (HANDLE)1);

	//      コンテキストメニューの作成
	AhtMenuBuf = new CMemBuf;
	AhtMenuBuf->AddIndexBuffer();
	EzMenuId = IDM_AHTEZINPUT;
	hMenu_context = LoadMenu(hInstance, "CONTEXTMENU");
	hSubMenu = GetSubMenu( hMenu_context, 0 );
	sprintf( tmp, "%s\\ezinput", szExeDir );
#ifdef JPNMSG
	BuildEzInputMenu( hSubMenu, tmp, "かんたん入力" );
#else
	BuildEzInputMenu(hSubMenu, tmp, "EasyInput");
#endif

	hMenu2 = LoadMenu(hInstance, "CONTEXTMENU2");
	hSubMenu2 = GetSubMenu(hMenu2, 0);

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

	SetMenuTool(hWndMain);

	SetMenuExtTool();
	ExecStartupTools(hwnd);



	LoadFromCommandLine(GetCommandLine());
	
	scmd=SW_SHOW;
	if (winflg&1) scmd=SW_SHOWMAXIMIZED;
	//if (winflg&2) scmd=SW_SHOWMINIMIZED;

    ShowWindow ( hwnd, scmd ) ;
    UpdateWindow (hwnd) ;

    hAccel = LoadAccelerators (hInstance, szAppName) ;

	if(!InitInterface(hInstance))
		MessageBox(hwnd, 
#ifdef JPNMSG
			"外部ツール用のウィンドウの初期化に失敗しました。\n"
			"一部の外部ツールを使用することはできません。"
#else
			"Failed to initialize a window for extension tools.\n"
			"You can't use some extending tools."
#endif
			, "start up error", MB_OK | MB_ICONEXCLAMATION);
	
	// バックアップファイルがある場合は読む
	char _backuppath[MAX_PATH*2];
	GetBackupPath(_backuppath, MAX_PATH*2);
	if (ExistBackupFile(_backuppath) != -1){
#ifdef JPNMSG
		int res = MessageBoxA(hwnd, "前回正しく終了されませんでした。バックアップファイルから復旧しますか？", "", MB_ICONQUESTION | MB_YESNO);
#else
		int res = MessageBoxA(hwnd, "Invalid exit detected. Will you resume from backup?", "", MB_ICONQUESTION | MB_YESNO);
#endif
		if (res == IDYES){
			ReadBackup();
		}else{
#ifdef JPNMSG
			res = MessageBoxA(hwnd, "復元しない場合はバックアップファイルが消えますがよろしいですか？", "", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
#else
			res = MessageBoxA(hwnd, "Are you sure to delete backup files?", "", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2);
#endif
			if (res == IDNO){
				ReadBackup();
			}
		}
	}
	
	// タイマーセット
	if (autobackup > 0){
		AutoBackupTimer = SetTimer(hWndMain, TIMERID_AUTOBACKUP, autobackup*1000, 0);
	}else{
		
	}
	

	//	main loop

    while (GetMessage (&msg, NULL, 0, 0))
    {
	  if ((hDlgModeless  == NULL || !IsDialogMessage (hDlgModeless, &msg))
		  &&(hConfigDlg  == NULL || !IsDialogMessage (hConfigDlg,   &msg))
		  &&(hConfigPage == NULL || !IsDialogMessage (hConfigPage,  &msg)))
      {
      if (!TranslateAccelerator (hwnd, hAccel, &msg))
           {
           TranslateMessage (&msg) ;
           DispatchMessage (&msg) ;
           }
       }
	}
	KillTimer(hwnd, AutoBackupTimer);
	dll_bye();							// DLLを開放
#ifdef FOOTYSTATIC
	Footy2End();
#endif	/*FOOTYSTATIC*/
	
	// バックアップファイル削除
	char backuppath[MAX_PATH*2];
	GetBackupPath(backuppath, MAX_PATH*2);
	BackupDelete(backuppath);
	
	

	delete ToolMenuBuf;
	delete AhtMenuBuf;
	return (int)msg.wParam ;
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
void UpdateViewOption( int toolbar_flag, int stbar_flag )
{
	RECT r ;
	if ( toolbar_flag ) {
		ShowWindow (hwndToolBar, SW_SHOW);
	} else {
		ShowWindow (hwndToolBar, SW_HIDE);
	}
	flg_toolbar = toolbar_flag;

	if ( stbar_flag ) {
		ShowWindow (hwndStatusBar, SW_SHOW);
	} else {
		ShowWindow (hwndStatusBar, SW_HIDE);
	}
	flg_statbar = stbar_flag;

	// Resize other windows.
	GetClientRect (hWndMain, &r) ;
	PostMessage (hWndMain, WM_SIZE, 0, MAKELPARAM (r.right, r.bottom));
}
//-------------------------------------------------------------------
LRESULT CALLBACK
WndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam) 
     {
	 // 自動バックアップが有効な場合
	 if (autobackup > 0){
		 if (mMsg == WM_TIMER){
			if (wParam == TIMERID_AUTOBACKUP)
				AutoBackUp();
		 }else if (mMsg == WM_QUERYENDSESSION){
			 return TRUE;
		 }else if (mMsg == WM_ENDSESSION){
			// HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\RunOnce
			HKEY hKey;
			DWORD dwDisposition;
			LONG result;
			result = RegCreateKeyExA( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
			char data[MAX_PATH*2] = {0};
			GetModuleFileNameA(NULL, data, sizeof(data));
			result = RegSetValueExA(hKey, "!HSED3", 0, REG_SZ, (CONST BYTE*)(LPCTSTR)data, strlen(data));
			RegCloseKey(hKey);
			ExitProcess( 0 );
		 }
	 }
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
			   RemoveProp(hwnd, PROP_NAME);
               PostQuitMessage (0) ;
               return 0 ;

          case WM_NOTIFY :
               {
               LPNMHDR pnmh = (LPNMHDR) lParam ;
//               int idCtrl = (int) wParam ;

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
               cy -= cyStatus + cyTB;
               MoveWindow (hwndClient, x, y, cx, cy, TRUE) ;
               return 0;
               }

		  case WM_COPYDATA:
			  {
                  COPYDATASTRUCT *pcds = (COPYDATASTRUCT*)lParam;
				  LoadFromCommandLine((char *)pcds->lpData);
                  return 0;
			  }

          default:
			  if (mMsg == iMsgNotifyFindReplaceWindowHandle) {
				  hwndFindReplace = (HWND)wParam;
			  } else {
				  return (EditDefProc(hwnd, mMsg, wParam, lParam));
			  }
          }
     return DefWindowProc (hwnd, mMsg, wParam, lParam) ;
     }

//-------------------------------------------------------------------

LRESULT CALLBACK 
ClientWndProc (HWND hwnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
     {
	RECT rect;

     switch (mMsg)
          {
          case WM_CREATE :
			   poppad_ini( hwnd,lParam );
               return 0 ;

          case WM_DESTROY :
               //DeleteObject (hbr) ;
			   DestroyMenu(hMenu) ;
			   poppad_bye();
               return 0 ;

		  case WM_INITMENUPOPUP :
			   return poppad_menupop( (WPARAM)hSubMenu, -1 );

          case WM_SETFOCUS :
			   Footy2SetFocus(activeFootyID, 0);
               return 0 ;

          case WM_SIZE :
               {
               int cx = LOWORD (lParam) ;
               int cy = HIWORD (lParam) ;

			   MoveWindow(hwndTab, 0, 0, cx, cy, TRUE);
			   GetClientRect(hwndTab, &rect);
			   TabCtrl_AdjustRect(hwndTab, FALSE, &rect);
			   Footy2Move(activeFootyID, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
			   poppad_setsb_current(activeFootyID);

			   return 0 ;
               }

          case WM_NOTIFY:
			  switch (((NMHDR *)lParam)->code){
				   case TCN_SELCHANGE:
				   {
					   int presID = activeID, newID = TabCtrl_GetCurSel(hwndTab);
					   ActivateTab(presID, newID);
					   ChangeZOrder(presID, newID);
					   return 0;
				   }

				   case NM_RCLICK:
					   POINT pt, cpt;
					   int i;

					   GetCursorPos(&pt);
					   cpt = pt;
					   ScreenToClient(hwndTab, &cpt);

					   for(i = 0; TabCtrl_GetItemCount(hwndTab); i++){
						   TabCtrl_GetItemRect(hwndTab, i, &rect);
						   if(rect.left <= cpt.x && cpt.x <= rect.right && rect.top <= cpt.y && cpt.y <= rect.bottom){
							   TrackPopupMenu(hSubMenu2, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndClient, NULL);
							   ClickID = i;
							   break;
						   }

					   }
 					   return 0;
			  }
			  return 0;

          default :
               return (EditProc (hwnd, mMsg, wParam, lParam)) ;
          }
     }



/*------------------------------------------------------------*/
/*
		external application capture functions
*/
/*------------------------------------------------------------*/

static	HWND tarhw=NULL;
static	char wtitle[256];
static	int stobj, objcnt;

static BOOL CALLBACK cbWins( HWND hwnd, LPARAM lParam )
{
	int a;
	char a1;
	char namtmp[256];

	objcnt++;
	if (objcnt<stobj) return TRUE;
	GetWindowText( hwnd,namtmp,256 );
	a=0;
	while(1) {
		a1=wtitle[a];if (a1==0) break;
		if (a1!=namtmp[a]) return TRUE;
		a++;
	}
	tarhw=hwnd;
	return FALSE;
}

HWND main_aplsel( char *p1 )
{
	tarhw=NULL;
	objcnt=0;stobj=1;
	strcpy( wtitle,p1 );
	EnumWindows( (WNDENUMPROC) &cbWins, 0 );
	if (tarhw==NULL) return NULL;
	return tarhw;
}


