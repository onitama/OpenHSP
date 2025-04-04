
//
//		HSP editor / onion software 1997(c)
//				Started  1996/4 onitama
//				Win95ver 1997/7 onitama
//				DLLver   1999/2 onitama
//

/*---------------------------------------
//   Based on POPPAD.C -- Popup Editor
//               (c) Charles Petzold, 1996
---------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <direct.h>
#include <winuser.h>
#include <shlobj.h>
#include "Footy2.h"
#include <htmlhelp.h>
#if _MSC_VER >= 1400 // VC++ .NET 2005 or later
#include <mbctype.h> // for _ismbblead()
#endif

#include "poppad.h"
#include "resource.h"

#include "tabmanager.h"
#include "support.h"
#include "config.h"
#include "classify.h"
#include "exttool.h"
#include "supio.h"

#include "PackIconResource.h"

#define SUPPORT_DIR "SUPPORT\\"

#define HSPTMP "hsptmp"
#define HSPOBJ "obj"

// PathIsDirectoryで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
/*
		XP support routines
*/
//extern int  flag_xpstyle;
int getUnicodeOffset( char *text, int offset );


/*
		DLL support routines
*/

typedef BOOL (CALLBACK *DLLFUNC)(int,int,int,int);

DLLFUNC hsc_ini;
DLLFUNC hsc_refname;
DLLFUNC hsc_objname;
DLLFUNC hsc_comp;
DLLFUNC hsc_getmes;
DLLFUNC hsc_clrmes;
DLLFUNC hsc_ver;
DLLFUNC hsc_bye;
DLLFUNC pack_ini;
DLLFUNC pack_make;
DLLFUNC pack_exe;
DLLFUNC pack_opt;
DLLFUNC pack_rt;
DLLFUNC hsc3_getsym;
DLLFUNC hsc3_make;
DLLFUNC hsc3_messize;
DLLFUNC hsc3_getruntime;  // 3.0用の追加
DLLFUNC hsc3_run;		  // 3.0用の追加
DLLFUNC hsc3_analysis;	  // 3.7用の追加
DLLFUNC hsc3_kwlbuf;	  // 3.7用の追加
DLLFUNC hsc3_kwlsize;	  // 3.7用の追加
DLLFUNC hsc3_kwlclose;	  // 3.7用の追加
DLLFUNC hsc3_kwlineinfo;  // 3.7用の追加


static	int dllflg=0;			// DLL uses flag
static	HINSTANCE hDLL;			// Handle to DLL

#define PIC_LINEBUFFER_MAX (0x1000)
static	char pick_linebuffer[PIC_LINEBUFFER_MAX];

static char *SetDllFunc( char *name )
{
	//		DLL関数を割り当てる
	//
	char *ent;
	char fncname[128];
	fncname[0]='_';
	strcpy( fncname+1,name );
	strcat( fncname,"@16" );
	ent = (char *)GetProcAddress( hDLL, fncname );
	if (ent == NULL) {
		dllflg = -1;				// error flag
	}
	return ent;
}

int dll_ini( char *libname )
{
	//		Initalize DLL entry
	//			(result:1=ok)
	//
	hDLL = LoadLibrary( libname );
	if ( hDLL==NULL ) return 0;

	dllflg=1;
	hsc_ini = (DLLFUNC)SetDllFunc("hsc_ini");
	hsc_refname = (DLLFUNC)SetDllFunc("hsc_refname");
	hsc_objname = (DLLFUNC)SetDllFunc("hsc_objname");
	hsc_comp = (DLLFUNC)SetDllFunc("hsc_comp");
	hsc_getmes = (DLLFUNC)SetDllFunc("hsc_getmes");
	hsc_clrmes = (DLLFUNC)SetDllFunc("hsc_clrmes");
	hsc_ver = (DLLFUNC)SetDllFunc("hsc_ver");
	hsc_bye = (DLLFUNC)SetDllFunc("hsc_bye");
	pack_ini = (DLLFUNC)SetDllFunc("pack_ini");
	pack_make = (DLLFUNC)SetDllFunc("pack_make");
	pack_exe = (DLLFUNC)SetDllFunc("pack_exe");
	pack_opt = (DLLFUNC)SetDllFunc("pack_opt");
	pack_rt = (DLLFUNC)SetDllFunc("pack_rt");
	hsc3_getsym = (DLLFUNC)SetDllFunc("hsc3_getsym");
	hsc3_make = (DLLFUNC)SetDllFunc("hsc3_make");
	hsc3_messize = (DLLFUNC)SetDllFunc("hsc3_messize");
	hsc3_analysis = (DLLFUNC)SetDllFunc("hsc3_analysis");
	hsc3_kwlbuf = (DLLFUNC)SetDllFunc("hsc3_kwlbuf");
	hsc3_kwlsize = (DLLFUNC)SetDllFunc("hsc3_kwlsize");
	hsc3_kwlclose = (DLLFUNC)SetDllFunc("hsc3_kwlclose");
	hsc3_kwlineinfo = (DLLFUNC)SetDllFunc("hsc3_kwlineinfo");

	if ( dllflg > 0 ) {
		hsc3_getruntime = (DLLFUNC)SetDllFunc("hsc3_getruntime");  // 3.0用の追加
		hsc3_run = (DLLFUNC)SetDllFunc("hsc3_run");				   // 3.0用の追加
		return dllflg;
	}
	return dllflg;
}

void dll_bye( void )
{
	//		Release DLL entry
	//
	if (dllflg==0) return;
	if (dllflg==1) {
		hsc_bye(0,0,0,0);					// 後始末routine
	}
	FreeLibrary( hDLL );
	dllflg=0;
}


/*
		special dialog routines
*/

static int selfolder( char *pname )
{
	//
	//		フォルダ選択ダイアログ
	//
	BROWSEINFO BrowsingInfo;
	char DirPath[_MAX_PATH];
	char FolderName[_MAX_PATH];
	LPITEMIDLIST ItemID;
	memset( &BrowsingInfo, 0, sizeof(BROWSEINFO) );
	memset( DirPath, 0, _MAX_PATH );
	memset( FolderName, 0, _MAX_PATH );
	BrowsingInfo.hwndOwner      = NULL;
	BrowsingInfo.pszDisplayName = FolderName;
#ifdef JPNMSG
	BrowsingInfo.lpszTitle      = "フォルダを選択してください";
#else
	BrowsingInfo.lpszTitle	    = "Select Folder";
#endif
	BrowsingInfo.ulFlags        = BIF_RETURNONLYFSDIRS;
	ItemID = SHBrowseForFolder( &BrowsingInfo );
	if (ItemID==NULL) return -1;

	SHGetPathFromIDList(ItemID, DirPath );
	LocalFree( ItemID );						//ITEMIDLISTの解放
	strcpy( pname,DirPath );
	//strcpy( p4,FolderName );
	return 0;
}


/*
		Editor related routines
*/

#define EDITID   1

       BOOL      bNeedSave ;
	   char      szDirName[_MAX_PATH] ;
       char      szFileName[_MAX_PATH] ;
       char      szTitleName[_MAX_FNAME + _MAX_EXT] ;
static HINSTANCE hInst ;
static HWND      hwndEdit ;
static UINT      iMsgFindReplace ;
static UINT      iMsgHelp ;
static char		 kwstr[512];
char		 hdir[_MAX_PATH];
static int		 kwstr_line;

extern HWND      hwndFindReplace;
extern UINT      iMsgNotifyFindReplaceWindowHandle;	// ウィンドウハンドル通知

extern char szExeDir[_MAX_PATH];
extern char szStartDir[_MAX_PATH];
extern int	winx;
extern int	winy;
extern int	posx;
extern int	posy;
extern int	winflg;
extern int	flg_toolbar;
extern int	flg_statbar;
extern int	startflag;
extern int	flg_hspat;
extern char startdir[_MAX_PATH];

int  ClickID = -1;

extern HWND          hwndClient;
extern HWND          hwndTab;
extern HWND          hWndMain;
       HWND          hConfigDlg = NULL;
	   HWND          hConfigPage = NULL;
       int           activeID = -1;
	   int           activeFootyID = -1;
	   bool			 bUseIni;
	   int			 ColumnWidth[2];

extern FileList filelist;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyEditProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK MyTabProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ErrDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PlistDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LogcompDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigAddinPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigBehaviorPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigColorPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigDirectoryPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigExtToolsPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigFontPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigKeywordPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfigVisualPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT FileDrop(WPARAM wParam, LPARAM lParam);

//void ConfigEditColor( COLORREF fore, COLORREF back );

void LoadFromCommandLine(char *);

          // Functions in POPFILE.C

void PopFileInitialize (HWND) ;
BOOL PopFileOpenDlg    (HWND, PSTR, PSTR) ;
BOOL PopFileOpenDlg2   (HWND, PSTR, PSTR) ;
BOOL PopFileOpenDlgImg (HWND, PSTR, PSTR) ;
BOOL PopFileSaveDlg    (HWND, PSTR, PSTR) ;
BOOL PopFileRead       (int, PSTR) ;
BOOL PopFileWrite      (int,  PSTR) ;

          // Functions in POPFIND.C

HWND PopFindFindDlg     (HWND, int) ;
HWND PopFindReplaceDlg  (HWND) ;
BOOL PopFindFindText    (HWND, int, LPFINDREPLACE, bool) ;
BOOL PopFindReplaceText (HWND, int, LPFINDREPLACE) ;
BOOL PopFindReplaceAllText(HWND, int, LPFINDREPLACE);
void PopFindDlgTerm     (DWORD);
BOOL PopFindNextText    (HWND, int, bool) ;
BOOL PopFindValidFind   (void) ;
BOOL PopFindReplaceAll  (HWND hwndEdit, int iSearchOffset, LPFINDREPLACE pfr);

          // Functions in POPFONT.C

void PopFontSetELG(LOGFONT);
LOGFONT PopFontGetELG(void);
void PopFontSetTLG(LOGFONT);
LOGFONT PopFontGetTLG(void);
LOGFONT *PopFontGetTELG();
LOGFONT *PopFontGetTTLG();

DWORD PopFontChooseColor(HWND, COLORREF);
LONG PopFontChooseEditFont(HWND);
LONG PopFontChooseTabFont(HWND);
void PopFontInitChooseFont(LOGFONT *, LOGFONT *);
void PopFontDeinitialize(void);
void PopFontSetEditFont();
void PopFontSetTabFont();
void PopFontApplyEditFont();
void PopFontApplyTabFont();


          // Functions in POPPRNT.C

BOOL PopPrntPrintFile (HINSTANCE, HWND, HWND, PSTR) ;

          // Functions in STATBAR.C

void Statusbar_mes( char *mes );

          // Functions in main.cpp

int ExecEzInputMenu( int id );
void UpdateViewOption( int toolbar_flag, int stbar_flag );
HWND main_aplsel( char *p1 );
int ExectMenuTool(int id);

          // Global variables

static char szAppName[] = "OniPad" ;

HWND hDlgModeless ;
static int cln;
HWND hwbak ;
extern HMENU hSubMenu ;
extern HMENU hMenu_context;


WNDPROC	Org_EditProc;		// サブクラス化の初期値
WNDPROC	Org_TabProc;		// サブクラス化の初期値


/*
		HSP related service
*/

static char *errbuf=NULL;					// error message buffer

int		hsp_fullscr = 0;
static int		hsp_fnstr = 0;

char		hsp_laststr[TMPSIZE];
char		hsp_cmdopt[TMPSIZE];
static char		compfile[TMPSIZE];
char		hsp_helpdir[TMPSIZE];
static char		execmd[TMPSIZE];
static char		objname[TMPSIZE];
char		helpopt[TMPSIZE];
char		hsp_extstr[TMPSIZE];

int     hscroll_flag;
LOGFONT	chg_font;
LOGFONT tchg_font;
static int		exemode;
int		hsp_wx;
int		hsp_wy;
int		hsp_wd;
int		hsp_orgpath;
int		hsp_debug;
int		hsp_extobj;
int		hsp_helpmode;

static int		hsp_logflag;
int		hsp_logmode;
int		hsp_logadd;
int		hsp_extmacro;
int		hsp_clmode;
extern MYCOLOR color;

int tabEnabled;
int hsEnabled;
int fsEnabled;
int nlEnabled;
int eofEnabled;
int ulEnabled;
int tabsize;
int rulerheight;
int linewidth;
int linespace;

BOOL bAutoIndent;

int forcefont;		// フォント強制 by inovia
BOOL bCustomColor;	// カスタムカラー by inovia
int autobackup;		// 自動バックアップ by inovia
BOOL BackupNoDelete;// 復旧時バックアップファイルを削除せず保存する by inovia
BOOL NotifyBackup;	// バックアップしたことをステータスバーで通知
extern int AutoBackupTimer;	// タイマー
int speedDraw;		// 高速描画
BOOL bDrawUnderline;	// 非フォーカス時にもアンダーライン描画
BOOL UseNewReplace; // 高速な置換 by Tetr@pod
BOOL CustomMatchCase; // カスタム色分け機能の大文字小文字の区別 by Tetr@pod
int nowViewMode = VIEWMODE_NORMAL;// ウィンドウ分割 by Tetr@pod
BOOL UseClickableLabel; // リンクラベルを使用する by Tetr@pod
BOOL UseSearchFunction; // ユーザー定義命令・関数を色分けする by Tetr@pod

BOOL ChangeColor_func; // #func、#cfunc、#cmd を色分けしない by Tetr@pod
BOOL ChangeColor_define; // #define、#define ctype を色分けしない by Tetr@pod

char BgImagePath[_MAX_PATH+1]; // 背景画像パス by inovia
int      BgImageFlag;			// 背景画像の使用フラグ by onitama

/*
		Error message process
*/

static void err_ini( void )
{
	//	init error message buffer
	//
	if (errbuf==NULL) errbuf=(char *)malloc(0x10000);
	errbuf[0]=0;
}

static void err_bye( void )
{
	//	error message buffer release
	//
	free( errbuf );
}

/*static*/ void err_prt( HWND hwnd )
{
	//	error message print
	//
	DialogBox (hInst, "ErrBox", hwnd, (DLGPROC)ErrDlgProc);
}

static void OkMessage ( char *szMessage, char *szTitleName)
{
	msgboxf(hwbak, szMessage, "script editor message", MB_OK | MB_ICONEXCLAMATION,
		szTitleName[0] ? szTitleName : UNTITLED);
}

static void OkMessage2(char* szMessage, char* szTitleName, HWND hwnd)
{
	msgboxf(hwnd, szMessage, "script editor message", MB_OK | MB_ICONEXCLAMATION,
		szTitleName[0] ? szTitleName : UNTITLED);
}

static void TMes( char *prtmes )
{
	OkMessage ( prtmes,"\0" );
}

int fileok( char *fname )
{
	//		File exist check
	//
	FILE *fp;
	int er=0;
	fp=fopen(fname,"rb");
	if (fp==NULL) er++; else fclose(fp);
	return er;
}


static int filechk( char *fname )
{
	//		File exist check
	//
	int er=0;
	er=fileok( fname );
	if (er) {
#ifdef JPNMSG
		OkMessage ( "カレントディレクトリに、[%s]が見つかりませんでした。\nコマンドは実行できません。",
					fname ) ;
#else
		OkMessage ( "Not exist file [%s].\nOperation invalid.",
					fname ) ;
#endif
	}
	return er;
}


static char *myfile( void )
{
	//		Current filename get
	//
	if (szTitleName[0]==0) {
		strcpy( compfile, "???" );
	}
	else {
		strcpy( compfile, szTitleName );
	}
	return compfile;
}


static int GetFileTitle2( char *bname, char *tname )
{
	//		GetFileTitleの替わり
	//		(ファイル拡張子表示ON/OFFの影響を受けない)
	//
	int a,b,len;
	unsigned char a1;
	b=-1;
	len=(int)strlen(bname);
	for(a=0;a<len;a++) {
		a1=(unsigned char)bname[a];
		if (a1=='\\' || a1=='/') b=a;
		if ((a1>=129)&&(a1<=159)) a++; 
		if ((a1>=224)&&(a1<=252)) a++; 
	}
	if (b<0) return 1;
	strcpy( tname,bname+b+1 );
	return 0;
}


static void SetFileName(char* titleName, char* fileName, char* dirName) {
	if (titleName != NULL) lstrcpy(szTitleName, titleName);
	if (fileName != NULL) lstrcpy(szFileName, fileName);
	if (dirName != NULL) lstrcpy(szDirName, fileName);
	SetTabInfo(activeID, titleName, fileName, dirName, -1);
}


void DoCaption(char* szTitleName, int TabID)
{
	char szCaption[_MAX_PATH + 128];

	if (GetTabInfo(0) == NULL) {
#ifdef JPNMSG
		lstrcpy(szCaption, "ＨＳＰスクリプトエディタ");
#else
		lstrcpy(szCaption, "HSP Script Editor");
#endif
	}
	else {
#ifdef JPNMSG
		sprintf(szCaption, "ＨＳＰスクリプトエディタ - %s%s",
			szTitleName[0] ? szTitleName : UNTITLED, bNeedSave ? " *" : "");
#else
		sprintf(szCaption, "HSP Script Editor - %s%s",
			szTitleName[0] ? szTitleName : UNTITLED, bNeedSave ? " *" : "");
#endif
	}
	SetWindowText(hwbak, szCaption);

	if (TabID >= 0) {
		char szTabCaption[_MAX_PATH * 5 + 128];	// 下の置き換え処理を実行した場合最悪で、５倍必要な為
		TCITEM tc_item;
		TABINFO* lpTabInfo;

		lpTabInfo = GetTabInfo(TabID);
		sprintf(szTabCaption, "%s%s",
			lpTabInfo->TitleName[0] ? lpTabInfo->TitleName : TABUNTITLED, lpTabInfo->NeedSave ? " *" : "");

		// &を&&に置き換える処理(これしないと&が含まれているとき、タブの文字列がアンダーバーが付く)
		StrReplaceALL(szTabCaption, "&", ":amp:");
		StrReplaceALL(szTabCaption, ":amp:", "&&");

		tc_item.mask = TCIF_TEXT;
		tc_item.pszText = szTabCaption;

		TabCtrl_SetItem(hwndTab, TabID, &tc_item);
	}
}


static void GoToLine(int id, int jumpline)
{
	int i = jumpline;
	if (jumpline > 0) {
		int maxline = Footy2GetLines(id) - 1;
		if (i > maxline) i = maxline;
		Footy2SetCaretPosition(id, i-1, 0);
		PutLineNumber();
	}
}


static int OpenHSPFile( char *fname, int jumpline )
{
	int nTabNumber, nFootyID;
	TABINFO* lpTabInfo;
	bool bCreated;
	ULONGLONG ullFileIndex;

	if (*fname == '?') {
		GoToLine(activeFootyID, jumpline);
		return 0;
	}

	strcpy(szFileName, fname);
	getpath(fname, szTitleName, 8);
	GetDirName(szDirName, szFileName);
	if (*szDirName == 0) {
		GetCurrentDirectory(_MAX_PATH, szDirName);
	}
	SetCurrentDirectory(szDirName);
	ullFileIndex = GetFileIndex(szFileName);
	if ((nTabNumber = SearchTab(NULL, NULL, NULL, ullFileIndex)) >= 0) {
		ActivateTab(activeID, nTabNumber);
		GoToLine(activeFootyID, jumpline);
		return 0;
	}

	lpTabInfo = GetTabInfo(activeID);

	nFootyID = activeFootyID;
	bCreated = false;

	if (lpTabInfo != NULL) {
		if (lpTabInfo->NeedSave) { bCreated = true; }
	}
	if (lpTabInfo == NULL
		|| lpTabInfo->FileName[0] != '\0'
		|| Footy2IsEdited(activeFootyID)) {
		bCreated = true;
	}
	if (bCreated) {
		CreateTab(activeID, szTitleName, szFileName, szDirName);
	}
	else {
		SetTabInfo(activeID, szTitleName, szFileName, szDirName, FALSE);
	}
	if (!PopFileRead(activeFootyID, szFileName)) {
#ifdef JPNMSG
		OkMessage("%s をロードできませんでした。", szTitleName);
#else
		OkMessage("Loading %s fault.", szTitleName);
#endif
		if (bCreated) {
			DeleteTab(activeID);
		}
		else {
			SetFileName("", "", "");
		}
		return -1;
	}

	GetTabInfo(activeID)->FileIndex = ullFileIndex;

	bNeedSave = FALSE;
	DoCaption(szTitleName, activeID);
	GoToLine(activeFootyID, jumpline);

	return 0;
}


static void expack( int mode, char *exname, char *finmes )
{
	//		make DPM->EXE file
	//
	int a;
	char hh[_MAX_PATH];
	char ftmp[_MAX_PATH];

	strcpy(ftmp,exname);strcat(ftmp,".dpm");
	pack_ini( 0,(int)exname,0,0 );	
	a=pack_make( 0,0,0,0 );
	//dpmc_ini(errbuf,exname);
	//a=dpmc_pack();
	if (a) { err_prt(hwbak);return; }

	if ( hsp_clmode==0 ) {
		sprintf(hh,"%s\\hsprt",szExeDir);
	} else {
		sprintf(hh,"%s\\runtime\\hspcl.hrt",szExeDir);
	}

	pack_rt( 0,(int)hh,0,0 );
	pack_opt( hsp_wx,hsp_wy,(hsp_wd)|(hsp_orgpath<<1),0 );
	a=pack_exe( mode,0,0,0 );
	//a=dpmc_mkexe( hsp_fullscr,hh,hsp_wx,hsp_wy,hsp_wd );
	if (a) { err_prt(hwbak);return; }
	TMes(finmes);
	DeleteFile(ftmp);
}
	
	
static void chklstr( char *laststr )
{
	//		hsp_laststr modify
	//
	int a;
	char a1;
	a=0;
	for(;;) {
		a1=laststr[a];
		if (a1==0) return;
		if (a1=='.') break;
		a++;
	}
	laststr[a]=0;
}

static void hsprun( char *objname )
{
	//  execute HSP3 process
	//
	int i;
	char cfname[256];
	*cfname = 0;
	if ( hsc3_getruntime != NULL ) {
		hsc3_getruntime( (int)cfname, (int)objname, 0, 0 );
	}
	if ( *cfname == 0 ) {
		sprintf( execmd,"\"%s\\%s\" ",szExeDir, DEFAULT_RUNTIME );
	} else {
		sprintf( execmd,"\"%s\\%s\" ",szExeDir, cfname );
	}

	strcat( execmd,objname );
	if (hsp_cmdopt[0]!=0) {
		strcat( execmd," " );
		strcat( execmd,hsp_cmdopt );
	}

	i = hsc3_run( (int)execmd, hsp_debug, 0, 0 );
	if (i) {
#ifdef JPNMSG
		TMes("実行用ランタイムファイルが見つかりません。");
#else
		TMes("Runtime executable file missing.");
#endif
	}
}

static void hsprun_log( char *objname )
{
	//		execute HSP2 process (with LOG)
	//
	char dbopt[64];
	sprintf( execmd,"\"%s\\%s\" ",szExeDir,DEFAULT_RUNTIME );
	sprintf( dbopt, "%03d",hsp_logmode );
	strcat( execmd, "%" );
	strcat( execmd, dbopt );
	if (hsp_logadd==0) DeleteFile( "hsplog.txt" );

	if (hsp_fullscr) strcat( execmd,"*" );
	if (hsp_debug) strcat( execmd,"@" );
	strcat( execmd,objname );
	if (hsp_cmdopt[0]!=0) {
		strcat( execmd," " );
		strcat( execmd,hsp_cmdopt );
	}
	WinExec( execmd,SW_SHOW );
}


static void hsprun_cl( char *objname )
{
	//		execute HSP2CL process
	//
	char ftmp[_MAX_PATH];
	GetShortPathName( szExeDir, ftmp, MAX_PATH );

	sprintf( execmd,"%s\\hspcli.bat ",ftmp );

	strcat( execmd,objname );
	strcat( execmd," " );
	strcat( execmd,"\\" );

	if (hsp_cmdopt[0]!=0) {
		strcat( execmd," " );
		strcat( execmd,hsp_cmdopt );
	}
	WinExec( execmd,SW_SHOW );
}


static void hsprun_log_cl( char *objname )
{
	//		execute HSP2CL process (with LOG)
	//
	char dbopt[64];
	char ftmp[_MAX_PATH];
	GetShortPathName( szExeDir, ftmp, MAX_PATH );

	sprintf( execmd,"%s\\hspcli.bat ",ftmp );
	sprintf( dbopt, "%03d",hsp_logmode );
	strcat( execmd, "%" );
	strcat( execmd, dbopt );
	if (hsp_logadd==0) DeleteFile( "hsplog.txt" );

	strcat( execmd,objname );
	strcat( execmd," " );
	strcat( execmd,ftmp );
	strcat( execmd,"\\" );

	if (hsp_cmdopt[0]!=0) {
		strcat( execmd," " );
		strcat( execmd,hsp_cmdopt );
	}
	WinExec( execmd,SW_SHOW );
}


static int mkobjfile( char *fname )
{
	//	make object file
	//
	int a;
	char a1;
	char tmpst[_MAX_PATH];
	char srcfn[_MAX_PATH];
	a=(int)strlen(fname)-1;
	for(;;) {
		a1=fname[a];
		if (a1==0x5c) { a++;break; }
		a--;if (a==0) break;
	}
	strcpy(srcfn,&(fname[a]));
	strcpy(tmpst,srcfn);
	for(a=0;;) {
		a1=tmpst[a];
		if ((a1==0)||(a1=='.')) break;
		a++;
	}
	tmpst[a]=0;strcat(tmpst,".ax");

	hsc_ini( 0,(int)srcfn, 0,0 );
	hsc_refname( 0,(int)myfile(), 0,0 );
	hsc_objname( 0,(int)tmpst, 0,0 );
	a=hsc_comp( 0,0,0,0 );
	return a;
}


static int mkobjfile2( char *fname )
{
	//	make object file
	//
	int a;
	char tmpst[_MAX_PATH];
	char srcfn[_MAX_PATH];
	strcpy(srcfn,fname);
	strcpy(tmpst,"start.ax");

	hsc_ini( 0,(int)srcfn, 0,0 );
	hsc_refname( 0,(int)myfile(), 0,0 );
	hsc_objname( 0,(int)tmpst, 0,0 );
	a=hsc_comp( 0,0,0,0 );
	return a;
}


static int mkexefile2(char* fname)
{
	//	auto make exe file (ver2.6)
	//
	int a;
	char tmpst[_MAX_PATH];
	char srcfn[_MAX_PATH];
	char ftmp[_MAX_PATH];
	strcpy(srcfn, fname);
	strcpy(tmpst, "start.ax");

	hsc_ini(0, (int)srcfn, 0, 0);
	hsc_refname(0, (int)myfile(), 0, 0);
	hsc_objname(0, (int)tmpst, 0, 0);
	a = hsc_comp(0, 4, 0, 0);
	if (a) return a;

	sprintf(ftmp, "%s\\%s.dpm", szExeDir, srcfn);
	a = hsc3_make(0, (int)ftmp, 1, 0);
	if (a) return a;
	return 0;
}

static char analysis_header[16];
static char analysis_keyword[64];
static char analysis_filename[512];
static int analysis_line;
static int analysis_baseline;
static char *analysis_base;
static char analysis_linemodule[256];
static BYTE* buffResult = NULL;

static void closeHSPAnalysis(void)
{
	hsc3_kwlclose(0, 0, 0, 0);
	if (buffResult != NULL) {
		delete[] buffResult;
		buffResult = NULL;
	}
}

static char* getHSPAnalysis(int mode, char *key, int line)
{
	int a, size;
	char tmpfn[2048];
	*analysis_linemodule = 0;
	PopFileWrite(activeFootyID, HSPTMP);
	strcpy(tmpfn, HSPTMP);
	hsc_ini(0, (int)tmpfn, 0, 0);
	myfile();
	//MessageBox(NULL, compfile, szTitleName, 0);
	hsc_refname(0, (int)compfile, 0, 0);
	strcpy(objname, HSPOBJ);
	hsc_objname(0, (int)objname, 0, 0);
	hsc3_analysis(0, (int)key, mode, line);
	a = hsc_comp(1 + 16, 0, 0, 0);
	if (a) {
#ifdef JPNMSG
		sprintf(tmpfn, "ソースの解析中にエラーが発生しました[%s]", compfile);
#else
		sprintf(tmpfn, "Error during source analysis[%s]", compfile);
#endif
		MessageBox(NULL, tmpfn, "", 0);
		analysis_base = "";
		analysis_baseline = 0;
		return analysis_base;
	}

	bool modcheck = false;
	if (*key != 0) modcheck = true;

	if ((line > 0)&&(modcheck)) {
		hsc3_kwlineinfo((int)analysis_linemodule, 0, 0, 0);

		if (*analysis_linemodule != 0) {		// カーソル位置がモジュール内にあるか調べる

			char* chkmod = strstr2(key, "@");
			if (chkmod == NULL) {
				//	現在の検索結果を保存しておく
				BYTE* prevResult = NULL;
				int prevSize;
				hsc3_kwlsize((int)&prevSize, 0, 0, 0);
				prevResult = new BYTE[prevSize + 1];
				hsc3_kwlbuf((int)prevResult, 0, 0, 0);

				//	モジュール指定をしていない場合はモジュール名を付加して再度検索する
				char tmpkey[512];
				strcpy(tmpkey, key);
				strcat(tmpkey, analysis_linemodule);
				chkmod = strstr2(tmpkey, "@");	// 念のため'@'が含まれるか調べる
				if (chkmod != NULL) {
					closeHSPAnalysis();
					analysis_base = getHSPAnalysis(mode, tmpkey, line);
					if (strlen(analysis_base)>1) {		// モジュール付加の結果がある場合はそちらを返す
						delete[] prevResult;
						analysis_baseline = 0;
						return analysis_base;
					}
				}
				//	以前の結果を返す
				buffResult = prevResult;
				analysis_base = (char*)buffResult;
				analysis_baseline = 0;
				return analysis_base;
			}
		}
		//sprintf(tmpfn, "[%s]%d", analysis_linemodule, line);
		//MessageBox(NULL, tmpfn, "", 0);
	}
	hsc3_kwlsize((int)&size, 0, 0, 0);
	buffResult = new BYTE[size + 1];
	hsc3_kwlbuf((int)buffResult, 0, 0, 0);
	analysis_base = (char*)buffResult;
	analysis_baseline = 0;
	return analysis_base;
}

static char *getKeywordLine(char *buf)
{
	char line[64];
	char* p = buf;
	char* akw = analysis_keyword;

	analysis_line = 0;
	*analysis_header = 0;
	*analysis_keyword = 0;
	*analysis_filename = 0;

	strsp_ini();
	if (*p == 0) return NULL;
	strsp_get( p, analysis_header, 32, 15 );
	strsp_get( p, line, 32, 63 );
	*akw = 0;
	if (analysis_header[0] == 'r') {
		*akw = '-'; akw++;
		*akw = '>'; akw++;
	}
	if (analysis_header[1] == 'l') {
		*akw = '*'; akw++;
	}
	strcpy(akw,line);
	strsp_get( p, line, ':', 63 );
	analysis_line = atoi(line);
	strsp_get( p, analysis_filename, 0, 511 );
	//sprintf(pick_linebuffer,"%s %s %d:%s", analysis_header, analysis_keyword, analysis_line, analysis_filename);
	return p;
}

static char* getAnalysisLine(void)
{
	//	結果バッファから1行を取り出す
	if (analysis_base == NULL) return NULL;
	if (*analysis_base == 0) return NULL;

	strsp_ini();
	strsp_get(analysis_base, pick_linebuffer, 0, PIC_LINEBUFFER_MAX);
	analysis_base += strsp_getptr();
	return pick_linebuffer;
}

static int execGoToDefinition(void)
{
	if (*kwstr==0) return -2;
	char *result = getHSPAnalysis(2,kwstr,kwstr_line);
	char* buf = getKeywordLine(result);
	if (buf == NULL) return -1;
	closeHSPAnalysis();
	if (analysis_line > 0) {
		OpenHSPFile(analysis_filename, analysis_line);
	}
	return 0;
}

/*
		Editor related routines
*/

void gethdir( void )
{
	//		Get help directory
	//
	int b;
	char a1;
	char *ss;
	char *ls;

	ss=hsp_helpdir;
	if (*ss==0) {
		sprintf( hdir,"%s\\hsphelp\\",szExeDir );
		return;
	}
	b=0;ls=ss;
	for(;;) {
		a1=*ss++;hdir[b++]=a1;
		if (a1==0) break;
		if (a1=='\\') ls=ss;
	}
	if (hdir[b-2]!='\\') {
		hdir[b-1]='\\';hdir[b++]=0;
	}
}

static void getkw( void )
{
	//		Get help keyword
	//
// 2008-02-23 Shark++ 書き直そうとしたけど途中で元のコードを手直しすれば動くことに気が付く
// 2008-02-25 Shark++ やっぱ書き換える
	int line = 0, pos = 0, linesize, len;
	wchar_t *linebuff = NULL, *p, *kwstart;

	// 動作メモ
	//  キーワードには、
	//   数字
	//   大小英字
	//   '[','\\',']','^','_','`','{','|','}','~'
	//   0x7F - 0xFF
	//  が含まれる
	//  hsp_helpmode が 非0の場合は、プリプロセッサの'#'もキーワードに含める

	Footy2GetCaretPosition(activeFootyID, (size_t*)&line, (size_t*)&pos);
	kwstr_line = line+1;
	linesize = Footy2GetLineLengthW(activeFootyID, line);
	linebuff = (wchar_t *)Footy2GetLineW(activeFootyID, line); // Footy2内の行バッファを取得
	if( !linebuff ) {
		goto abort;
	}

	kwstart = linebuff + pos;

	for(p = kwstart - 1; linebuff <= p; p--)
	{
		if( !(L'0' <= *p && *p <= L'9') &&	// 数字以外 かつ
			*p < L'A')						// 0x40以下のコード
		{
			if( hsp_helpmode && L'#' == *p ) {
				kwstart = p;
			}
			break;
		}
		kwstart = p;
	}

	for(p = kwstart;
		*p && ( (L'0' <= *p && *p <= L'9') || L'A' <= *p || L'#' == *p );
		p++);

	len = (int)(p - kwstart);
	len = WideCharToMultiByte(CP_ACP, 0, kwstart, len, kwstr, 512, NULL, NULL);
	kwstr[len] = '\0';

abort:
	;
//	MessageBox( NULL,kwstr,"WOW",0 );
}

static void callhelp( void )
{
	//		HELP呼び出し
	//
	int a;
	char mesb[512];

	if (hsp_helpmode==0) {
		a=(int)strlen(kwstr);

		// 最初の6文字キー検索を廃止
		//if (a>6) kwstr[6]=0;							// 始めの6文字

		sprintf( helpopt,"%ss_%s.htm",hdir,kwstr );
		if (fileok(helpopt)) {
			sprintf(helpopt,"%shsppidx.htm",hdir);
		}
		if (fileok(helpopt)) {
#ifdef JPNMSG
			TMes("ヘルプのためのHTMLファイルが見つかりません。\nディレクトリ設定を確認してください。");
#else
			TMes("Help data missing.Check help preference.");
#endif
			return;
		}
		ShellExecute( NULL,NULL,helpopt,"","",SW_SHOW );
		return;
	}

	if (hsp_helpmode==1) {
		sprintf(helpopt,"%shsp.hlp",hdir);
		if (fileok(helpopt)) {
#ifdef JPNMSG
			TMes("HSP.HLPファイルが見つかりません。\nWinHelp形式のファイルを確認してください。");
#else
			TMes("Help data missing.Check help preference.");
#endif
			return;
		}
		WinHelp( hwndEdit, helpopt, HELP_KEY, (ULONG_PTR)kwstr );
		return;
	}

	if (hsp_helpmode==3) {

		//		"S_" + keyword先頭６文字 +".htm" のファイルを開く。
		sprintf( helpopt,"%shsp.chm",hdir );
		if (fileok(helpopt)) {
#ifdef JPNMSG
			TMes("ヘルプのためのchmファイルが見つかりません。\nディレクトリ設定を確認してください。");
#else
			TMes("Help data missing.Check help preference.");
#endif
			return;
		}
		{
		HWND rhw;
		HH_AKLINK link;
		link.cbStruct =  sizeof(HH_AKLINK);
		link.fReserved = FALSE;
		link.pszKeywords = kwstr;
		link.pszUrl =  NULL;
		link.pszMsgText = NULL;
		link.pszMsgTitle = NULL;
		link.pszWindow = NULL;
		link.fIndexOnFail = TRUE;
		rhw=HtmlHelp( GetDesktopWindow(), helpopt, HH_KEYWORD_LOOKUP, (DWORD)&link);
		if (rhw==NULL) {
#ifdef JPNMSG
			TMes("HtmlHelpがインストールされていません。");
#else
			TMes("HtmlHelp not installed.");
#endif
			return;
		}
		}
		return;
	}

	sprintf( helpopt,"%shelpman.exe",hdir );
	if (fileok(helpopt)) {
#ifdef JPNMSG
		sprintf( mesb, "HSPヘルプマネージャが見つかりません。\n%sを確認してください。",helpopt );
#else
		sprintf( mesb, "HSP help manager not found.\nCheck %s.",helpopt );
#endif
		TMes( mesb );
		return;
	}
	sprintf( helpopt,"\"%shelpman.exe\" %s",hdir,kwstr );
	WinExec( helpopt, SW_SHOW );
	return;
}


static void ExecMkDPM(void)
{
	char tmpfn[2048];
	sprintf(tmpfn, "\"%s\\hsp3.exe\" \"%s\\%spackdir_make_list.ax\"", szExeDir, szExeDir, SUPPORT_DIR);
	WinExec(tmpfn, SW_SHOW);
}


static void ExecMkPackOpt(void)
{
	char tmpfn[2048];
	sprintf(tmpfn, "\"%s\\hsp3.exe\" \"%s\\%spackopt_make_list.ax\"", szExeDir, szExeDir, SUPPORT_DIR);
	WinExec(tmpfn, SW_SHOW);
}


static void ExecPaint(void)
{
	char tmpfn[2048];
	sprintf(tmpfn, "mspaint");
	WinExec(tmpfn, SW_SHOW);
}


static void ExecHSP3Update(void)
{
	char tmpfn[_MAX_PATH];
	sprintf(tmpfn, "\"%s\\hsp3upd.exe\"", szExeDir);
	WinExec(tmpfn, SW_SHOW);
}


static void ExecMapTool(void)
{
	char tmpfn[2048];
	sprintf(tmpfn, "\"%s\\hsp3dish.exe\" \"%s\\%stamamap.ax\"", szExeDir, szExeDir, SUPPORT_DIR);
	WinExec(tmpfn, SW_SHOW);
}


static void ExecHSPAssistant( void )
{
	char tmpfn[_MAX_PATH];
	sprintf( tmpfn, "\"%s\\hspat.exe\"", szExeDir );
	WinExec( tmpfn, SW_SHOW );
}


static void CloseHSPAssistant( void )
{
	HWND hw;
	hw = main_aplsel( "HSP assistant ver" );
	if ( hw == NULL ) return;
	PostMessage( hw, WM_CLOSE, 0, 0 );
	//SendMessage( hw, WM_CLOSE, 0, 0 );
	while(1) {
		Sleep( 500 );
		hw = main_aplsel( "HSP assistant ver" );
		if ( hw == NULL ) break;
	}
}


void pophwnd( HWND hwnd )
{
	hwbak=hwnd;
}

short AskAboutSave (HWND hwnd, char *szTitleName)
     {
     char szBuffer[64 + _MAX_FNAME + _MAX_EXT] ;
     int  iReturn ;

#ifdef JPNMSG
     sprintf (szBuffer, "%sは変更されています。セーブしますか？",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#else
     sprintf (szBuffer, "%s has been modified. Save?",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#endif

     iReturn = MessageBox (hwnd, szBuffer, "Warning",
                           MB_YESNOCANCEL | MB_ICONQUESTION) ;

     if (iReturn == IDYES)
          if (!SendMessage (hwnd, WM_COMMAND, IDM_SAVE, 0L))
               iReturn = IDCANCEL ;

     return (short)iReturn ;
     }


BOOL CALLBACK JumpDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
	 char s1[10];

     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
						 GetDlgItemText( hDlg,IDC_EDIT1,s1,8 );
                         EndDialog (hDlg, 0);
						 cln=atoi(s1)-1;
                         return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
						 cln=-1;
						 return TRUE;
                    }
               break ;
          }
     return FALSE ;
     }


BOOL CALLBACK OptDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_cmdopt );
				return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
						 GetDlgItemText( hDlg,IDC_EDIT1,hsp_cmdopt,TMPSIZE );
                         EndDialog (hDlg, 0);
                         return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
						 return TRUE;
                    }
               break ;
          }
     return FALSE ;
     }


BOOL CALLBACK FnameDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
	 int flg;
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_laststr );
				if (exemode) {
					SetDlgItemInt( hDlg,IDC_EDIT2,hsp_wx,0 );
					SetDlgItemInt( hDlg,IDC_EDIT3,hsp_wy,0 );
					SetDlgItemInt( hDlg,IDC_EDIT3,hsp_wy,0 );
					CheckDlgButton( hDlg, IDC_CHECK1, hsp_wd );
					CheckDlgButton( hDlg, IDC_CHECK2, hsp_orgpath );
				}
				return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
						 GetDlgItemText( hDlg,IDC_EDIT1,hsp_laststr,TMPSIZE );
						 if (exemode) {
							 hsp_wx=GetDlgItemInt( hDlg,IDC_EDIT2,&flg,0 );
							 hsp_wy=GetDlgItemInt( hDlg,IDC_EDIT3,&flg,0 );
							 if (hsp_wx<64) hsp_wx=64;
							 if (hsp_wy<8) hsp_wy=8;
							 if ( IsDlgButtonChecked( hDlg,IDC_CHECK1 )==BST_CHECKED ) hsp_wd=1; else hsp_wd=0;
							 if ( IsDlgButtonChecked( hDlg,IDC_CHECK2 )==BST_CHECKED ) hsp_orgpath=1; else hsp_orgpath=0;
						 }
                         EndDialog (hDlg, 0);
					     hsp_fnstr=1;
                         return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
					     hsp_fnstr=0;
						 return TRUE;
                    }
               break ;
          }
     return FALSE ;
     }


BOOL CALLBACK ExtcmpDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_extstr );
				CheckDlgButton( hDlg, IDC_CHECK1, hsp_extobj );
				return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
						 GetDlgItemText( hDlg,IDC_EDIT1,hsp_extstr,TMPSIZE );
						 if ( IsDlgButtonChecked( hDlg,IDC_CHECK1 )==BST_CHECKED )
								hsp_extobj=1; else hsp_extobj=0;
                         EndDialog (hDlg, 0);
					     hsp_fnstr=1;
                         return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
					     hsp_fnstr=0;
						 return TRUE;
                    }
               break ;
          }
     return FALSE ;
     }


int CheckRadio( HWND hDlg, int idc, int num )
{
	int a,i;
	i=0;
	for(a=0;a<num;a++) {
	 if ( IsDlgButtonChecked( hDlg,idc+a )==BST_CHECKED ) i=a;
	}
	return i;
}

BOOL CALLBACK ErrDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				hsc_getmes( (int)errbuf,0,0,0 );
				SendMessage( GetDlgItem( hDlg, IDC_EDIT1 ), EM_LIMITTEXT, 0, 0L);
				SetDlgItemText( hDlg,IDC_EDIT1,errbuf );
				return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
					case IDCANCEL:
                         EndDialog (hDlg, 0);
                         return TRUE;
                    }
               break ;
          }
     return FALSE ;
     }


static void set_labellist2(HWND hList, int mode, char *search, bool extsw, int line)
{
	//		ラベル一覧を検索(hspcmpを使用)
	//
	ListView_DeleteAllItems(hList);
	getHSPAnalysis(mode, search,line);
	int tag = 0;
	char st[128];

	while (1) {
		char* buf = getAnalysisLine();
		if (buf == NULL) break;
		getKeywordLine(buf);

		if (strcmp(analysis_filename, szTitleName) != 0) {
			if ( extsw == false ) analysis_line = 0;
		}
		if (analysis_line > 0) {
			LV_ITEM lvi = { LVIF_TEXT | LVIF_PARAM, tag, 0, 0, 0, st, };
			lvi.lParam = tag;
			sprintf(st, "%d", analysis_line);
			ListView_InsertItem(hList, &lvi);
			lvi.mask = LVIF_TEXT;
			lvi.iSubItem++;
			sprintf(st, "%s", analysis_header);
			ListView_SetItem(hList, &lvi);
			lvi.iSubItem++;
			sprintf(st, "%s", analysis_keyword);
			ListView_SetItem(hList, &lvi);
			lvi.iSubItem++;
			sprintf(st, "%s", analysis_filename);
			ListView_SetItem(hList, &lvi);
			tag++;
		}
	}
	//analysis_line = 0;
	//*analysis_header = 0;
	//*analysis_keyword = 0;
	//*analysis_filename = 0;
	ListView_SetColumnWidth(hList, 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hList, 1, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hList, 2, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(hList, 3, LVSCW_AUTOSIZE_USEHEADER);
	closeHSPAnalysis();
}

static void set_labellist( HWND hList, HWND /*hwndEdit*/ )
{
	//		ラベル一覧を検索(従来の方法)
	//
	char st[128];
	char lname[256];
	char *buffer;
	char *wp;
	int len;
	int line;
	int tag;
	int mytag = -1;
	int myline= -2;
	int ret;

	ListView_DeleteAllItems(hList);

	len = Footy2GetTextLength(activeFootyID, LM_CRLF);
	if(len >= 0){
		buffer = (char *) calloc( len + 2 /* 文字列終端と番兵 */, sizeof(char) );
		Footy2GetText(activeFootyID, buffer, LM_CRLF, len + 1);
		ret = Footy2GetSel(activeFootyID, (size_t*)&myline, NULL, NULL, NULL);
		if(FOOTY2ERR_NOTSELECTED == ret){
			Footy2GetCaretPosition(activeFootyID, (size_t*)&myline, NULL);
		}
		myline++; // Footy2 は行が0ベースなので

		line = 1;
		wp = buffer;
		tag	= -1;
		for(; *wp; line++, wp++) {
			// 1行読み込み
			for(; *wp && 0x0d != *wp; wp++) {
				// 先頭の空白を無視
				if( ' ' == *wp || '\t' == *wp ) {
					continue;
				}
				// プリプロセッサ読み飛ばし
				if( '#' == *wp ) {
					static const char * func_define_pp[] = {
						"deffunc", "defcfunc",
						"modinit", "modterm", "modfunc", //"modcfunc",
					};
				//	static const char * func_define_pp_type[] = {
				//		"命令", "関数",
				//		"module初期化", "module破棄", "module命令", //"module関数",
				//	};
					int namelen = 0;
					do {
						for(wp++; *wp && 0x0d != *wp ; wp++) {
							if( (0 < namelen || (' ' != *wp && '\t' != *wp)) &&
								('\\' != *wp || (0x0a != wp[1] && 0x0d != wp[1])) &&
								0x0a != *wp && 0x0d != *wp &&
								namelen < sizeof(lname)/sizeof(lname[0]) - 1 ) {
								lname[namelen++] = *wp;
							}
						}
						line++;
					} while ( '\\' == *(wp - 1) && *wp );
					line--;
					lname[namelen] = '\0';
					//
					for(int i = 0; i < sizeof(func_define_pp)/sizeof(func_define_pp[0]); i++) {
						int comp_len = min((int)strlen(func_define_pp[i]), namelen);
						if( !strncmp(lname, func_define_pp[i], comp_len) ) {
							char *pa, *pb;
							pb = lname + comp_len;
							do {
								pa = pb;
								for(; '\t' == *pa || ' ' == *pa; pa++, pb++);	// '#'の直後の空白をスキップ
								for(; '\t' != *pb && ' ' != *pb && ',' != *pb && *pb; pb++);	// 空白までを取得
								namelen = (int)(pb - pa);
								if( sizeof(lname) <= (int)(pa - lname) + namelen ) { DebugBreak(); }
								memcpy(lname, pa, namelen);
								lname[namelen] = '\0';
							} while( !strcmpi(lname, "local") );
							//
							if( *lname ) {
								tag++;
								LV_ITEM lvi = { LVIF_TEXT, tag, 0, 0, 0, st, };
								sprintf(st, "%d",line);
								ListView_InsertItem(hList, &lvi);
								lvi.iSubItem++;
								sprintf(st, "#%s", func_define_pp[i]);
							//	sprintf(st, "%s", func_define_pp_type[i]);
								ListView_SetItem(hList, &lvi);
								lvi.iSubItem++;
								sprintf(st, "%s", lname);
							//	sprintf(st, "#%s %s", func_define_pp[i], lname);
								ListView_SetItem(hList, &lvi);
							}
							break;
						}
					}
					break;
				}
				// ラベル読み込み
				if( '*' == *wp ) {
					char *pa = wp;
					int namelen;
					for(pa++;*pa &&
							(':' > (unsigned char)*pa || '>' < (unsigned char)*pa) &&
							'/' < (unsigned char)*pa &&
							' ' != *pa && '\t' != *pa && 
							0x0d != *pa; pa++) { }
					namelen = (int)(pa - wp);
					if( 1 < namelen) {
						strncpy(lname, wp, namelen);
						lname[namelen] = '\0';
						tag++;
						LV_ITEM lvi = { LVIF_TEXT, tag, 0, 0, 0, st, };
						sprintf(st, "%d",line);
						ListView_InsertItem(hList, &lvi);
						lvi.iSubItem++;
						sprintf(st, "ラベル");
						ListView_SetItem(hList, &lvi);
						lvi.iSubItem++;
						sprintf(st, "%s", lname);
						ListView_SetItem(hList, &lvi);
					}
					wp = pa;
					if( 0x0d == *wp ) {
						break;
					}
				}
				// コメント読み飛ばし
				if( ';' == *wp || ('/' == *wp && '/' == wp[1]) ) {
					// 行末まで読み飛ばし
					for(wp++; *wp && 0x0d != *wp ; wp++);
					break;
				}
				if( '/' == *wp && '*' == wp[1] ) {
					for(wp++; *wp ; wp++) {
						if( '*' == *wp && '/' == wp[1] ) {
							wp++;
							break;
						}
						if( 0x0d == *wp ) {
							line++;
						}
					}
					continue;
				}
				// 次のステートメントまで読み飛ばし
				for(bool bEscape = false;
					*wp && 0x0d != *wp && ':' != *wp &&
					';' != *wp && ('/' != *wp || '*' != wp[1]) && ('/' != *wp || '/' != wp[1]); wp++)
				{
					if( '\"' == *wp && !bEscape ) {
						bool bDameChk = false;
						// ダメ文字チェック
						if (wp - 2 >= buffer)
							bDameChk = IsDBCSLeadByte((unsigned char) *(wp - 2));
						// 文字列読み飛ばし
						if( ('{' == *(wp - 1)) && (!bDameChk)) { // 複数行文字列
						//if( '{' == *(wp - 1) ) { // 複数行文字列
							wp += 2;
							for(bool bEscape = false; *wp && ('\"' != *wp || '}' != wp[1] || bEscape); ) {
								if( '\\' == *wp ) {
									bEscape = !bEscape;
								}
								wp += IsDBCSLeadByte(*wp) ? 2 : 1;
							}
						} else {
							wp++;
							for(bool bEscape = false; *wp && ('\"' != *wp || bEscape) && 0x0d != *wp; ) {
								if( '\\' == *wp ) {
									bEscape = !bEscape;
								}
								wp += IsDBCSLeadByte(*wp) ? 2 : 1;
							}
						}
						wp--;
						continue;
					}
				}
				if( 0x0d == *wp ) {
					break;
				}
				if( ':' != *wp ) {
					wp--;
				}
			}
			if( 0x0d == *wp && 0x0a == wp[1] ) {
				wp++;
			}
			if ( line<=myline) mytag=tag;
		}

		if ( mytag>=0 ) {
			ListView_SetItemState(hList, mytag, LVIS_SELECTED, LVIS_SELECTED);
			ListView_EnsureVisible(hList, mytag, FALSE);
		}
		ListView_SetColumnWidth(hList, 0, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(hList, 1, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(hList, 2, LVSCW_AUTOSIZE_USEHEADER);
		ListView_SetColumnWidth(hList, 3, LVSCW_AUTOSIZE_USEHEADER);
		free (buffer) ;
	}
}


static HWND mycomp_hList;

// アプリケーション定義比較関数
int CALLBACK MyCompProc(LPARAM lp1, LPARAM lp2, LPARAM lp3)
{
	static LV_FINDINFO lvf;
	static char buf1[30], buf2[30];

	ListView_GetItemText(mycomp_hList, (int)lp1, 3, buf1, sizeof(buf1));
	ListView_GetItemText(mycomp_hList, (int)lp2, 3, buf2, sizeof(buf2));
	int cmp = strcmp(buf1, buf2);
	if (cmp == 0) {
		ListView_GetItemText(mycomp_hList, (int)lp1, (int)lp3, buf1, sizeof(buf1));
		ListView_GetItemText(mycomp_hList, (int)lp2, (int)lp3, buf2, sizeof(buf2));

		int a1 = atoi(buf1);
		int a2 = atoi(buf2);
		return(a1 - a2);
	}
	return cmp;
}

BOOL CALLBACK LabelDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	 int i;
	 char s1[128];
	 static bool extlbchk = false;
     switch (message)
          {
          case WM_INITDIALOG: {
               HWND hList = GetDlgItem(hDlg, IDC_LIST1);
			   LV_COLUMN lvc = { LVCF_FMT, LVCFMT_LEFT, };
			   // 一行全てを反転表示するスタイルに変更
               ListView_SetExtendedListViewStyle(hList, ListView_GetExtendedListViewStyle(hList) | LVS_EX_FULLROWSELECT);
			   // カラム追加
			   lvc.fmt = LVCFMT_RIGHT;
			   ListView_InsertColumn(hList, 0, &lvc);
			   ListView_SetColumn(hList, 0, &lvc); // LVCFMT_RIGHTを反映させるため
			   lvc.fmt = LVCFMT_LEFT;
			   ListView_InsertColumn(hList, 1, &lvc);
			   lvc.fmt = LVCFMT_LEFT;
			   ListView_InsertColumn(hList, 2, &lvc);
			   lvc.fmt = LVCFMT_LEFT;
			   ListView_InsertColumn(hList, 3, &lvc);
			   // ラベル列挙
			   //set_labellist(hList, hwndEdit);
			   set_labellist2(hList, 0, "", false, 0);
			   mycomp_hList = hList;
			   ListView_SortItems(hList, MyCompProc, 0);
			   SetFocus(hList);
               return TRUE; }

		  case WM_COMMAND: {
               HWND hList = GetDlgItem(hDlg, IDC_LIST1);
               switch (LOWORD(wParam))
                    {
                    case IDOK:
						i = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
						if ( i != LB_ERR ) {
							ListView_GetItemText(mycomp_hList, i, 0, s1, sizeof(s1));
							ListView_GetItemText(mycomp_hList, i, 3, analysis_filename, 512);
							analysis_line =atoi(s1);
							cln = 0;
						}
						else {
							cln=-1;
						}
                         EndDialog (hDlg, 0);
                         return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
						 cln=-1;
						 return TRUE;
                    }
			   break ; }
		  case WM_NOTIFY: {
			   // ダブルクリックでOKを送りジャンプ
			   LPNMHDR pnmh = (LPNMHDR) lParam; 
			   if( NM_DBLCLK == pnmh->code ) {
					FORWARD_WM_COMMAND(hDlg, IDOK, NULL, BN_CLICKED, PostMessage);
			   }
			   bool nowcheck = ( IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED );
			   if (nowcheck != extlbchk) {
				   extlbchk = nowcheck;
				   HWND hList = GetDlgItem(hDlg, IDC_LIST1);
				   set_labellist2(hList, 0, "", extlbchk, 0);
				   mycomp_hList = hList;
				   ListView_SortItems(hList, MyCompProc, 0);
			   }
			   break ; }
          }
     return FALSE ;
}

BOOL CALLBACK KeywordRefDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	char s1[128];
	static bool extlbchk = false;
	switch (message)
	{
	case WM_INITDIALOG: {
		char tmsg[256];
#ifdef JPNMSG
		sprintf( tmsg, "%sの参照を検索",kwstr );
#else
		sprintf( tmsg, "Find all references of [%s].", kwstr );
#endif
		SetWindowText(hDlg,tmsg);
		HWND hList = GetDlgItem(hDlg, IDC_LIST1);
		LV_COLUMN lvc = { LVCF_FMT, LVCFMT_LEFT, };
		// 一行全てを反転表示するスタイルに変更
		ListView_SetExtendedListViewStyle(hList, ListView_GetExtendedListViewStyle(hList) | LVS_EX_FULLROWSELECT);
		// カラム追加
		lvc.fmt = LVCFMT_RIGHT;
		ListView_InsertColumn(hList, 0, &lvc);
		ListView_SetColumn(hList, 0, &lvc); // LVCFMT_RIGHTを反映させるため
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hList, 1, &lvc);
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hList, 2, &lvc);
		lvc.fmt = LVCFMT_LEFT;
		ListView_InsertColumn(hList, 3, &lvc);
		// ラベル列挙
		//set_labellist(hList, hwndEdit);
		set_labellist2(hList, 2+16, kwstr, false, kwstr_line);
		mycomp_hList = hList;
		ListView_SortItems(hList, MyCompProc, 0);
		SetFocus(hList);
		return TRUE; }

	case WM_COMMAND: {
		HWND hList = GetDlgItem(hDlg, IDC_LIST1);
		switch (LOWORD(wParam))
		{
		case IDOK:
			i = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
			if (i != LB_ERR) {
				ListView_GetItemText(mycomp_hList, i, 0, s1, sizeof(s1));
				ListView_GetItemText(mycomp_hList, i, 3, analysis_filename, 512);
				analysis_line = atoi(s1);
				cln = 0;
			}
			else {
				cln = -1;
			}
			EndDialog(hDlg, 0);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			cln = -1;
			return TRUE;
		}
		break; }
	case WM_NOTIFY: {
		// ダブルクリックでOKを送りジャンプ
		LPNMHDR pnmh = (LPNMHDR)lParam;
		if (NM_DBLCLK == pnmh->code) {
			FORWARD_WM_COMMAND(hDlg, IDOK, NULL, BN_CLICKED, PostMessage);
		}
		bool nowcheck = (IsDlgButtonChecked(hDlg, IDC_CHECK1) == BST_CHECKED);
		if (nowcheck != extlbchk) {
			extlbchk = nowcheck;
			HWND hList = GetDlgItem(hDlg, IDC_LIST1);
			set_labellist2(hList, 2+16, kwstr, extlbchk, kwstr_line);
			mycomp_hList = hList;
			ListView_SortItems(hList, MyCompProc, 0);
		}
		break; }
	}
	return FALSE;
}

void poppad_setedit(int FootyID)
{
	int VisibleStatus = EDM_SHOW_NONE;

	VisibleStatus |= !tabEnabled ? 0 : EDM_TAB;
	VisibleStatus |= !hsEnabled  ? 0 : EDM_HALF_SPACE;
	VisibleStatus |= !fsEnabled  ? 0 : EDM_FULL_SPACE;
	VisibleStatus |= !nlEnabled  ? 0 : EDM_LINE;
	VisibleStatus |= !eofEnabled ? 0 : EDM_EOF;
	Footy2SetMetrics(FootyID, SM_UNDERLINE_VISIBLE, ulEnabled, false);
	Footy2SetMetrics(FootyID, SM_MARK_VISIBLE, VisibleStatus, false);
	Footy2SetMetrics(FootyID, SM_TAB_WIDTH, tabsize, false);
	Footy2SetMetrics(FootyID, SM_RULER_HEIGHT, rulerheight, false);
	Footy2SetMetrics(FootyID, SM_LINENUM_WIDTH, linewidth, false);
	Footy2SetMetrics(FootyID, SM_MARGIN_HEIGHT, linespace, false);

	//	FootySetMetrics(FootyID, F_SM_LINESPACE, linespace, false);	// 2008-02-17 Shark++ 代替機能未実装
	//Footy2Refresh(FootyID);
}

static void poppad_setalledit()
{
	int num = TabCtrl_GetItemCount(hwndTab);
	TABINFO *lpTabInfo;
	for(int i = 0; i < num; i++){
		lpTabInfo = GetTabInfo(i);
		if(lpTabInfo != NULL)
            poppad_setedit(lpTabInfo->FootyID);
	}
}

static void poppad_setsb0( int chk, int FootyID )
{
	/*
	for(int i = 0; i > 4; i++){
		HWND hWnd = Footy2GetWnd(FootyID, i);
		ShowScrollBar(hWnd, SB_HORZ, chk);
	}
	*/
}

int poppad_setsb( int flag )
{
	int chk;
	if (flag<0) chk=hscroll_flag;
	else if (flag<2) chk=flag;
	else chk=hscroll_flag^1;

	int num = TabCtrl_GetItemCount(hwndTab);
	TABINFO *lpTabInfo;
	for(int i = 0; i < num; i++){
		lpTabInfo = GetTabInfo(i);
		if(lpTabInfo != NULL)
			poppad_setsb0(chk, lpTabInfo->FootyID);
	}


	hscroll_flag=chk;
	return chk;
}

void poppad_setsb_current( int FootyID )
{
	poppad_setsb0(hscroll_flag, FootyID);
}


int poppad_ini( HWND hwnd, LPARAM lParam )
{
               hInst = ((LPCREATESTRUCT) lParam) -> hInstance ;
			   hwndClient = hwnd;

               // Initalize script editor val
			   PopFileInitialize (hwnd);

			   //ConfigEditColor( fgcolor,bgcolor );
			   InitClassify();

   			   if ( flg_hspat ) ExecHSPAssistant();		// HSPアシスタントを自動起動

               hwndTab = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN,
                   0, 0, 10, 10, hwnd, (HMENU)0x10, hInst, NULL);
			   PopFontApplyTabFont();
			   Org_TabProc = (WNDPROC)GetWindowLong(hwndTab, GWL_WNDPROC);
			   SetWindowLong(hwndTab, GWL_WNDPROC, (LONG)MyTabProc);
			   DragAcceptFiles(hwndTab, TRUE);

               iMsgFindReplace = RegisterWindowMessage (FINDMSGSTRING) ;
			   iMsgHelp        = RegisterWindowMessage (HELPMSGSTRING) ;

			   PopFontSetELG( chg_font );
			   PopFontSetEditFont();

			   PopFontSetTLG( tchg_font );
			   PopFontSetTabFont();

			   CreateTab(0, "", "", "");
			   TabCtrl_SetCurFocus(hwndTab, 0);

			   DoCaption (szTitleName, activeID) ;
			   err_ini();

               return 0 ;
}


void poppad_bye( void )
{
    //case WM_DESTROY :

	if (flg_hspat) {						// HSPアシスタントを終了させる
		CloseHSPAssistant();
	}

	err_bye();
	PopFontDeinitialize () ;
	ByeClassify();
	DeleteObject((HGDIOBJ)(HFONT)SendMessage(hwndTab, WM_GETFONT, 0, 0));

}


int poppad_reload( int nTabID )
{
	TABINFO *lpTabInfo;
	char a1;
	int len;

	lpTabInfo = GetTabInfo(nTabID);
	if(lpTabInfo == NULL) return 1;

	if(lstrlen(lpTabInfo->FileName) > 0){
		len = lstrlen(lpTabInfo->FileName);
		a1 = lpTabInfo->FileName[len-1];
		if(a1 == '"') lpTabInfo->FileName[len-1]=0;

		if(GetFileTitle2(lpTabInfo->FileName, lpTabInfo->TitleName)){
			lstrcpy(lpTabInfo->TitleName, lpTabInfo->FileName );
		}
		GetDirName(lpTabInfo->DirName, lpTabInfo->FileName);
		if (!PopFileRead(lpTabInfo->FootyID, lpTabInfo->FileName)){
#ifdef JPNMSG
			OkMessage ( "%s が読み込めませんでした", lpTabInfo->TitleName ) ;
#else
			OkMessage ( "%s not found.", lpTabInfo->TitleName ) ;
#endif
			return 1;
		}
		SetTabInfo(nTabID, NULL, NULL, NULL, (lpTabInfo->NeedSave = FALSE));
		lpTabInfo->LatestUndoNum = 0;
		lpTabInfo->FileIndex     = GetFileIndex(lpTabInfo->FileName);
		if(nTabID == activeID) {
			lstrcpy(szTitleName, lpTabInfo->TitleName);
			lstrcpy(szFileName, lpTabInfo->FileName);
			lstrcpy(szDirName, lpTabInfo->DirName);
			SetCurrentDirectory(szDirName);
		}
    }
	DoCaption (lpTabInfo->TitleName, nTabID) ;
	return 0;
}


LRESULT CALLBACK MyEditProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	//		Subclassed Edit Proc
	//
	switch (msg) {
	case WM_DROPFILES:
		return FileDrop(wParam, lParam);

	case WM_KEYDOWN:
	{
		wchar_t *szSpaceBuf;
		const wchar_t *szLine;
		int nsLine, nsPos, neLine, nePos, nLength, i, j, ret;

		if(bAutoIndent && wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0){
			/*
			 * 処理内容
			 *  
			 */

			ret = Footy2GetSel(activeFootyID, (size_t*)&nsLine, (size_t*)&nsPos, (size_t*)&neLine, (size_t*)&nePos);
			if(FOOTY2ERR_NOTSELECTED == ret){
				Footy2GetCaretPosition(activeFootyID, (size_t*)&nsLine, (size_t*)&nsPos);
				neLine = nsLine;
				nePos  = nsPos;
			}

			szLine = Footy2GetLineW(activeFootyID, neLine);
			nLength = Footy2GetLineLengthW(activeFootyID, neLine);
			for(i = nePos; i < nLength && (szLine[i] == ' ' || szLine[i] == '\t'); i++)
				nePos++;

			nLength = min(nLength, nsPos);

			szSpaceBuf = (wchar_t *)calloc(nLength + 3, sizeof(wchar_t));
			lstrcpyW(szSpaceBuf, L"\r\n");

			for(i = nLength - 1, j = 2; i >= 0 && (szLine[i] == ' ' || szLine[i] == '\t'); i--);
			if(i >= 0 && szLine[i] == '{')
				szSpaceBuf[j++] = '\t';
			for(i = 0; i < nLength && (szLine[i] == ' ' || szLine[i] == '\t'); i++, j++)
				szSpaceBuf[j] = szLine[i];
			if(szLine[i] == '\0' || i >= nsPos)
				nsPos = 0;//1
			else if(szLine[i] == '*' && i < nsPos)
				szSpaceBuf[j++] = '\t';
            szSpaceBuf[j] = '\0';

			Footy2SetSel(activeFootyID, nsLine, nsPos, neLine, nePos, false);
			Footy2SetSelTextW(activeFootyID, szSpaceBuf);
			free(szSpaceBuf);
			return 0;
 	// 2008-03-17 Shark++ 要動作確認
		}
		break;
	}

	case WM_CHAR:
	{
		wchar_t szInsBuf[2] = { '\0' };
		const wchar_t *szLine;
		int nsLine, nsPos, neLine, nePos, nLength, i, ret;
		static char chPrevByte;

		if((wParam == '*' || wParam == '}') && !_ismbblead(chPrevByte)){
			ret = Footy2GetSel(activeFootyID, (size_t*)&nsLine, (size_t*)&nsPos, (size_t*)&neLine, (size_t*)&nePos);
			if(FOOTY2ERR_NOTSELECTED == ret){
				Footy2GetCaretPosition(activeFootyID, (size_t*)&nsLine, (size_t*)&nsPos);
				neLine = nsLine;
				nePos  = nsPos;
			}
			szLine  = Footy2GetLineW(activeFootyID, nsLine);
			nLength = Footy2GetLineLengthW(activeFootyID, nsLine);

			for(i = nsPos - 1; i >= 0 && (szLine[i] == ' ' || szLine[i] == '\t'); i--);
			if(i < 0)
				Footy2SetSel(activeFootyID, nsLine, (wParam == '*' ? 0 : (nsPos > 0 ? nsPos - 1 : 0)), neLine, nePos, false);
			if( lParam & 0xFF0000 ) { // 直接入力の場合に処理
				szInsBuf[0] = (wchar_t)(TCHAR)wParam;
			}
			Footy2SetSelTextW(activeFootyID, szInsBuf);
			return 0;
		} else {
			chPrevByte = (char)wParam;
		}
 	// 2008-03-17 Shark++ 要動作確認
		break;
	}

	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		 PutLineNumber();
		 break;

	case WM_CONTEXTMENU:
		{
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenu(GetSubMenu(hMenu_context, 0), TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndClient, NULL);
			break;
		}
	}

	return (CallWindowProc( Org_EditProc, hwnd, msg, wParam, lParam ));
}

LRESULT CALLBACK MyTabProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	//		Subclassed Tab Proc
	//
	switch(msg){
		case WM_DROPFILES:
			return FileDrop(wParam, lParam);
	}
	return (CallWindowProc( Org_TabProc, hwnd, msg, wParam, lParam ));
}

int poppad_menupop( WPARAM wParam, LPARAM lParam )
{
	int				 iSelBeg, iSelEnd, iEnable, iResult;
	int				 iSelLBeg, iSelLEnd/*, iLength*/;
	int				 iNum;
	MENUITEMINFO	 mii;
	HIMC			 hIMC;
	HWND			 hFooty;
	//case WM_INITMENUPOPUP :
	switch (lParam)
	{
		case -1 :		// Popup menu
			 do{
				hFooty = Footy2GetWnd(activeFootyID, 0);
				if(hFooty == NULL) break;
				hIMC = ImmGetContext(hFooty);
				if(hIMC == NULL) break;
				mii.cbSize = sizeof(MENUITEMINFO);
				mii.fMask = MIIM_STRING;
#ifdef JPNMSG
				mii.dwTypeData = ImmGetOpenStatus(hIMC) ? "IME を閉じる(&L)" : "IME を開く(&O)";
#else
				mii.dwTypeData = ImmGetOpenStatus(hIMC) ? "C&lose IME" : "&Open IME";
#endif
				SetMenuItemInfo((HMENU) wParam, IDM_OPENIME, FALSE, &mii);
				ImmReleaseContext(hFooty, hIMC);
			 }while(0);

		case 0 :		// Edit menu
		case 1 :		// Edit menu
			{

			// Enable Undo if edit control can do it
			iNum = 0;
			Footy2GetMetrics(activeFootyID, SM_UNDOREM, &iNum);
			EnableMenuItem ((HMENU) wParam, IDM_UNDO,
//				FootyGetMetrics(activeFootyID, F_GM_UNDOREM) > 0 ? MF_ENABLED : MF_GRAYED) ;	// 2008-02-17 Shark++ 代替機能未実装
				iNum > 0 ? MF_ENABLED : MF_GRAYED) ;

			// Enable Redo if edit control can do it

			iNum = 0;
			Footy2GetMetrics(activeFootyID, SM_REDOREM, &iNum);
			EnableMenuItem ((HMENU) wParam, IDM_REDO,
//				FootyGetMetrics(activeFootyID, F_GM_REDOREM) > 0 ? MF_ENABLED : MF_GRAYED) ;	// 2008-02-17 Shark++ 代替機能未実装
				iNum > 0 ? MF_ENABLED : MF_GRAYED) ;


			// Enable Paste if text is in the clipboard

			EnableMenuItem ((HMENU) wParam, IDM_PASTE,
				  IsClipboardFormatAvailable (CF_TEXT) ?
					   MF_ENABLED : MF_GRAYED) ;

			// Enable Cut, Copy, and Del if text is selected

//			SendMessage (hwndEdit, EM_GETSEL, (WPARAM) &iSelBeg,
//				(LPARAM) &iSelEnd) ;
			iSelLBeg = iSelBeg = iSelLEnd = iSelEnd = 0;
			iResult = Footy2GetSel(activeFootyID, (size_t*)&iSelLBeg, (size_t*)&iSelBeg, (size_t*)&iSelLEnd, (size_t*)&iSelEnd);
			if( FOOTY2ERR_NOTSELECTED == iResult ) {
				Footy2GetCaretPosition(activeFootyID, (size_t*)&iSelLBeg, (size_t*)&iSelBeg);
				iSelLEnd = iSelLBeg;
				iSelEnd  = iSelBeg;
			}

			iEnable = iSelBeg != iSelEnd  || iSelLBeg != iSelLEnd ? MF_ENABLED : MF_GRAYED ;

			EnableMenuItem ((HMENU) wParam, IDM_CUT,	 iEnable) ;
			EnableMenuItem ((HMENU) wParam, IDM_COPY,	 iEnable) ;
			EnableMenuItem ((HMENU) wParam, IDM_CLEAR,	 iEnable) ;

			EnableMenuItem ((HMENU) wParam, IDM_SELALL,
				 (0 < iSelLBeg || 0 < iSelBeg || iSelLEnd < Footy2GetLines(activeFootyID) - 1 ||
					iSelEnd < Footy2GetLineLengthW(activeFootyID, Footy2GetLines(activeFootyID) - 1))
						? MF_ENABLED : MF_GRAYED) ;
	int nA, nB;
	Footy2GetMetrics(activeFootyID, SM_REDOREM, &nA);
	Footy2GetMetrics(activeFootyID, SM_UNDOREM, &nB);
	nA = nA;
			break ;
			}

		case 2 :		// Search menu

			// Enable Find, Next, and Replace if modeless
			//   dialogs are not already active

			 iEnable = hDlgModeless == NULL ?
							MF_ENABLED : MF_GRAYED ;

			 EnableMenuItem ((HMENU) wParam, IDM_FIND,	  iEnable) ;
			 EnableMenuItem ((HMENU) wParam, IDM_NEXT,	  iEnable) ;
			 EnableMenuItem ((HMENU) wParam, IDM_REPLACE, iEnable) ;
			 break ;

		case 3 :		// HSP menu

			iEnable = hsp_fullscr ? MF_CHECKED : MF_UNCHECKED ;
			CheckMenuItem ((HMENU) wParam, IDM_FULLSCR, iEnable) ;
			iEnable = hsp_debug ? MF_CHECKED : MF_UNCHECKED ;
			CheckMenuItem ((HMENU) wParam, IDM_DEBUG, iEnable) ;
//			iEnable = hsp_extmacro ? MF_CHECKED : MF_UNCHECKED ;
//			CheckMenuItem ((HMENU) wParam, IDM_HSPEXTMACRO, iEnable) ;
			iEnable = hsp_clmode ? MF_CHECKED : MF_UNCHECKED ;
			CheckMenuItem ((HMENU) wParam, IDM_HSPCLMODE, iEnable) ;
			break;

	}

	return 0 ;
}

LRESULT poppad_term( UINT iMsg )
{
	switch(iMsg) {
          case WM_CLOSE :
			  TABINFO *lpTabInfo;
			  NMHDR nmhdr;
			  nmhdr.code = TCN_SELCHANGE;
			  for(int i = 0; (lpTabInfo = GetTabInfo(i)) != NULL; i++){
				  if(lpTabInfo->NeedSave){
					  TabCtrl_SetCurFocus(hwndTab, i);
					  SendMessage(hwndClient, WM_NOTIFY, 0, (LPARAM)&nmhdr);
					  if(IDCANCEL == AskAboutSave (hwbak, lpTabInfo->TitleName)) return 0;
				  }
			   }
			   SaveConfig();					// config save
			   ReleaseExtTool();
		       SetWindowLong( hwndEdit, GWL_WNDPROC, (LONG)Org_EditProc );
		       DestroyWindow (hwbak) ;
	           break;

          case WM_QUERYENDSESSION :
               if (!bNeedSave || IDCANCEL != AskAboutSave (hwbak, szTitleName))
                    return 1 ;
              break;
	}
	return 0;
}


void PutLineNumber( void )
{
	char szBuffer[256] ;
	int ln = 0;
	Footy2GetCaretPosition(activeFootyID, (size_t*)&ln, NULL);
	sprintf (szBuffer, "line : %d", ln + 1) ;
	Statusbar_mes( szBuffer );
}

int Footy2GetSelB(int FootyID, size_t * StartOffset, size_t * EndOffset);
						 
LRESULT CALLBACK EditDefProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int iOffset, SelStart, SelEnd;
	LPFINDREPLACE pfr ;

	// Process "Find-Replace" iMsgs

	if (iMsg == iMsgFindReplace)
	{
		pfr = (LPFINDREPLACE) lParam ;
	//	FootyGetSelB(activeFootyID, &SelStart, &SelEnd);
		Footy2GetSelB(activeFootyID, (size_t*)&SelStart, (size_t*)&SelEnd);

		// 新しい検索方式のために調整 (fixed@1.06β1 byLonelyWolf)
		iOffset = (pfr->Flags & FR_DOWN) ? SelEnd : SelStart;

		if (pfr->Flags & FR_DIALOGTERM) hDlgModeless = NULL ;

		if (pfr->Flags & FR_FINDNEXT)
			if (!PopFindFindText (hwndEdit, iOffset, pfr, true)){
#ifdef JPNMSG
                OkMessage2 ( "終わりまで検索しました", "", hwndFindReplace) ;
#else
                OkMessage2 ( "Not found.", "", hwndFindReplace) ;
#endif
			}

		if (pfr->Flags & FR_REPLACEALL)
            iOffset = 0;

        if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL)
			if (pfr->Flags & FR_REPLACE || !UseNewReplace ) {
				if (!PopFindReplaceText (hwndEdit, iOffset, pfr)) {
#ifdef JPNMSG
					OkMessage2 ( "終わりまで置換しました", "", hwndFindReplace) ;
#else
					OkMessage2 ( "Replace finished.", "", hwndFindReplace) ;
#endif
				}
			}
			if (pfr->Flags & FR_REPLACEALL) {// elseでは代替できない

				// 2008-03-11 Shark++ 他のエディタに比べると置換がすごく遅い気がする(それでも現行版よりは早いけど)
			//	::SendMessage(Footy2GetWnd(activeFootyID, 0), WM_SETREDRAW, 0, 0); // 2008-03-11 Shark++ 速度を稼ぐために描画を止めてみたけどあまり変わらない

				if ( UseNewReplace ) {// 設定により処理を振り分ける
					if (!PopFindFindText (hwndEdit, iOffset, pfr, false)) {
#ifdef JPNMSG
						OkMessage2 ( "終わりまで置換しました", "", hwndFindReplace) ;
#else
						OkMessage2 ( "Replace finished.", "", hwndFindReplace) ;
#endif
					}

					PopFindReplaceAllText(hwndEdit, iOffset, pfr);// by Tetr@pod
				} else {
					while ( PopFindReplaceText(hwndEdit, iOffset, pfr) );
					PopFindReplaceText(hwndEdit, iOffset, pfr);
				}
			//	::SendMessage(Footy2GetWnd(activeFootyID, 0), WM_SETREDRAW, 1, 0);
			//	Footy2Refresh(activeFootyID);
			}
	// 2008-02-17 Shark++ 後回し
		return 0 ;
	}
	else if (iMsg == iMsgHelp)
	{
		/*
			fixed

			2006/09/06 ワイルドカードに関する記述を削除。
		*/
		MessageBox(hwnd, 
			"-検索モード(未実装)-\n"
			"標準: 標準的な検索方法です。\n"
			"正規表現: 正規表現(RegEx クラスを使用?)を使用した検索方法です。\n"
			"\n"
			"-各チェックボックス-\n"
			"エスケープシーケンスを有効にする: \\\\, \\n, \\tに対応しています。\\nはCR,LF,CR+LFを自動で認識します。\n"
			"大文字と小文字を区別する: 書いてある通りです。\n"
			"\n"
			"* これは暫定的なヘルプです *"
			, "Help", MB_OK);
	}
	return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}



LRESULT CALLBACK EditProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
     int a;
	 int low;
	 char tmpfn[_MAX_PATH];

	 switch(iMsg) {
		case WM_COMMAND :
			low = LOWORD (wParam);
			if ( low >= IDM_AHTEZINPUT ) {
				ExecEzInputMenu( low );
				return 0;
			}
			if ((low >= IDM_EXTHSPTOOL)&&(low < (IDM_EXTHSPTOOL+1000))) {
				ExectMenuTool(low);
				return 0;
			}
			 switch (low){
				 // Messages from File menu


				case IDM_NEW :
					CreateTab(activeID, "", "", "");
					szFileName[0]  = '\0' ;
					szTitleName[0] = '\0' ;
					szDirName[0]   = '\0' ;
					bNeedSave = FALSE ;
					DoCaption (szTitleName, activeID) ;
					return 0 ;

				case IDM_OPEN :
				{
					if (PopFileOpenDlg (hwnd, szFileName, szTitleName)){
						OpenHSPFile(szFileName,0);
					}
					return 0 ;
				}

				case IDM_RELOAD:
				{
					bool bUndo;

					if(szFileName[0] == '\0')
						return 0;

//					bUndo = (FootyGetMetrics(activeID, F_GM_UNDOREM) > 0);
					bUndo = Footy2IsEdited(activeID);	// 2008-02-17 Shark++ 要動作確認
#ifdef JPNMSG
					if((!bNeedSave && !bUndo) || 
						msgboxf(hwnd, "再読込を行う上で、下記のことが起こります。よろしいですか？\n\n"
						"・「元に戻す」の情報が破棄され、再読込以前に戻れなくなります。%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, bNeedSave ? "\n・変更が無視されます" : "") == IDYES)
                        	poppad_reload(activeID);
#else
					if((!bNeedSave && !bUndo) || 
						msgboxf(hwnd, "Are you sure you want to reload the text? It will lead to these things.\n\n"
						"\tYou lose the 'Undo' history and cannot take back any changes before the reloading.%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, bNeedSave ? "\nlose all unsaved changes" : "") == IDYES)
                        	poppad_reload(activeID);
#endif
					return 0;
				}

				case IDM_RELOADTAB:
				{
					TABINFO *lpTabInfo = GetTabInfo(ClickID);
					bool bUndo;

					if(lpTabInfo == NULL || lpTabInfo->FileName[0] == '\0')
						return 0;

//					bUndo = (FootyGetMetrics(ClickID, F_GM_UNDOREM) > 0);
					bUndo = Footy2IsEdited(ClickID);	// 2008-02-17 Shark++ 要動作確認
#ifdef JPNMSG
					if((!lpTabInfo->NeedSave && !bUndo) || 
						msgboxf(hwnd, "再読込を行う上で、下記のことが起こります。よろしいですか？\n\n"
						"・「元に戻す」の情報が破棄され、再読込以前に戻れなくなります。%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, lpTabInfo->NeedSave ? "\n・変更が無視されます" : "") == IDYES)
                        	poppad_reload(ClickID);
#else
					if((!lpTabInfo->NeedSave && !bUndo) || 
						msgboxf(hwnd, "Are you sure you want to reload the text? It will lead to these things.\n\n"
						"\tYou lose the 'Undo' history and cannot take back any changes before the reloading.%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, lpTabInfo->NeedSave ? "\nlose all unsaved changes" : "") == IDYES)
                        	poppad_reload(ClickID);
#endif
					return 0;
				}

				case IDM_SAVE :
					if (szFileName[0]){
                        if (PopFileWrite (activeFootyID, szFileName)){
							SetTabInfo(activeID, NULL, NULL, NULL, (bNeedSave = FALSE));
//							GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeFootyID, F_GM_UNDOREM);
							GetTabInfo(activeID)->LatestUndoNum = 0;	// 2008-02-17 Shark++ 代替機能未実装
							Footy2GetMetrics(activeFootyID, SM_UNDOREM, &GetTabInfo(activeID)->LatestUndoNum);
							GetTabInfo(activeID)->FileIndex = GetFileIndex(szFileName);
							DoCaption (szTitleName, activeID) ;
							return 1 ;
						} else {
#ifdef JPNMSG
                            OkMessage ( "セーブに失敗しました。\n[%s]", szFileName ) ;
#else
                            OkMessage ( "Error happened in saving.\n[%s]", szFileName ) ;
#endif
						}
						return 0;
					}

				// fall through
				case IDM_SAVEAS :
					if (PopFileSaveDlg (hwnd, szFileName, szTitleName)){
						GetDirName(szDirName, szFileName);
						SetCurrentDirectory(szDirName);
						if (PopFileWrite (activeFootyID, szFileName)){
							SetTabInfo(activeID, szTitleName, szFileName, szDirName, (bNeedSave = FALSE));
//							GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeFootyID, F_GM_UNDOREM);
							GetTabInfo(activeID)->LatestUndoNum = 0;	// 2008-02-17 Shark++ 代替機能未実装
							Footy2GetMetrics(activeFootyID, SM_UNDOREM, &GetTabInfo(activeID)->LatestUndoNum);
							GetTabInfo(activeID)->FileIndex = GetFileIndex(szFileName);
							DoCaption (szTitleName, activeID) ;
							return 1 ;
						} else {
#ifdef JPNMSG
							OkMessage ( "%s のセーブに失敗しました。", szTitleName) ;
#else
							OkMessage ( "Error happened in saving.\n[%s]", szFileName ) ;
#endif
						}
					}
					return 0 ;

				case IDM_SAVEALL:
					for(ClickID = 0; ClickID < TabCtrl_GetItemCount(hwndTab); ClickID++)
						SendMessage(hwbak, WM_COMMAND, IDM_SAVETAB, 0L);
					return 0;

				case IDM_PRINT :
					if (!PopPrntPrintFile (hInst, hwnd, hwndEdit, szTitleName)){
#ifdef JPNMSG
						OkMessage ( "%s をプリントアウトできません。", szTitleName) ;
#else
						OkMessage ( "Error happened in printing.\n[%s]", szFileName ) ;
#endif
					}
					return 0 ;

				case IDM_EXIT :
					SendMessage (hwnd, WM_CLOSE, 0, 0) ;
					return 0 ;

					// Messages from Edit menu

				case IDM_UNDO :
					Footy2Undo(activeFootyID);
					return 0 ;

				case IDM_REDO :
					Footy2Redo(activeFootyID);
					return 0;

				case IDM_CUT :
					Footy2Cut(activeFootyID);
					return 0 ;

				case IDM_COPY :
					Footy2Copy(activeFootyID);
					return 0 ;

				case IDM_PASTE :
					Footy2Paste(activeFootyID);
					return 0 ;

				case IDM_CLEAR :
					Footy2SetSelText(activeFootyID, "");
					return 0 ;

				case IDM_SELALL :
					Footy2SelectAll(activeFootyID);
					return 0 ;

					// Messages from Search menu

				case IDM_FIND :
					if ( hDlgModeless == NULL )
						hDlgModeless = PopFindFindDlg (hwnd, -1) ;
					else if(GetDlgItem(hDlgModeless, 1153) != NULL){
						DestroyWindow(hDlgModeless);
						hDlgModeless = PopFindFindDlg (hwnd, -1) ;
					}
					else SetFocus(GetDlgItem(hDlgModeless, 1152));
					return 0 ;

				case IDM_NEXT : case IDM_BACK :
				{
					int SelStart, SelEnd, iOffset;
					bool down = (LOWORD(wParam) == IDM_NEXT);

//					FootyGetSelB(activeFootyID, &SelStart, &SelEnd);
					SelStart = SelEnd = 0;	// 2008-02-17 Shark++ 代替機能未実装
					if (PopFindValidFind ()){
						// 新しい検索方式のために調整 (fixed@1.06β1 byLonelyWolf)
						iOffset = ((LOWORD(wParam) == IDM_NEXT) ? SelEnd - 1 : SelStart - 1);
						PopFindNextText (hwndEdit, iOffset, down) ;
					}
					else if(hDlgModeless == NULL)
                        hDlgModeless = PopFindFindDlg (hwnd, down) ;
					else if(GetDlgItem(hDlgModeless, 1153) != NULL){
						DestroyWindow(hDlgModeless);
						hDlgModeless = PopFindFindDlg (hwnd, down) ;
					}
					else{
						CheckDlgButton(hDlgModeless, 1056, !down);
						CheckDlgButton(hDlgModeless, 1057, down);
						SetFocus(GetDlgItem(hDlgModeless, 1152));
					}
					return 0 ;
				}

				case IDM_REPLACE :
					if ( hDlgModeless == NULL )
                        hDlgModeless = PopFindReplaceDlg (hwnd) ;
					else if(GetDlgItem(hDlgModeless, 1153) == NULL){
						DestroyWindow(hDlgModeless);
						hDlgModeless = PopFindReplaceDlg (hwnd);
					}
					else SetFocus(GetDlgItem(hDlgModeless, 1152));
                    return 0 ;

                case IDM_FONT :
					if (LOWORD(PopFontChooseEditFont (hwnd)))
                            PopFontApplyEditFont ();
                        return 0 ;

				//	Message from cursor menu

				case IDM_JUMP:
				{
					DialogBox(hInst, "JumpBox", hwnd, (DLGPROC)JumpDlgProc);
					if (cln == -1) return 0;
					GoToLine(activeFootyID,cln+1);
					return 0;
				}
				case IDM_LBTM:
					Footy2SetCaretPosition(activeFootyID, Footy2GetLines(activeFootyID)-1, 0);
					PutLineNumber();
					return 0;

				case IDM_LTOP:
					Footy2SetCaretPosition(activeFootyID, 0, 0);
					PutLineNumber();
					return 0;

				case IDM_LABEL:
					DialogBox (hInst, "LabelBox", hwnd, (DLGPROC)LabelDlgProc);
					if (cln < 0) return 0;
					OpenHSPFile(analysis_filename, analysis_line);
					return 0;

					// Messages for HSP

				case IDM_MKOBJ:
					if (szFileName[0]!=0) {
						PopFileWrite (activeFootyID, szFileName);

						SetTabInfo(activeID, NULL, NULL, NULL, (bNeedSave = FALSE));
//						GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeID, F_GM_UNDOREM);
						GetTabInfo(activeID)->LatestUndoNum = 0;	// 2008-02-17 Shark++ 代替機能未実装
						Footy2GetMetrics(activeFootyID, SM_UNDOREM, &GetTabInfo(activeID)->LatestUndoNum);
						GetTabInfo(activeID)->FileIndex = GetFileIndex(szFileName);

						if ( mkobjfile( szFileName ) ){
							err_prt(hwnd);
							return 0;
						}
#ifdef JPNMSG
                        TMes("オブジェクトファイルを作成しました");
#else
                        TMes("Object file generated.");
#endif
					}
					return 0;

				case IDM_MKOBJ2:
					PopFileWrite ( activeFootyID, HSPTMP);
					if ( mkobjfile2(HSPTMP) ){
						err_prt(hwnd);
						return 0;
					}
#ifdef JPNMSG
                    TMes("START.AXを作成しました");
#else
					TMes("START.AX generated.");
#endif
					return 0;

				case IDM_AUTOMAKE:
					PopFileWrite ( activeFootyID, HSPTMP);
					if ( mkexefile2(HSPTMP) ){
						err_prt(hwnd);
						return 0;
					}
#ifdef JPNMSG
					TMes("実行ファイルを作成しました");
#else
					TMes("Executable file generated.");
#endif
					return 0;

				case IDM_COMP:
				case IDM_COMP2:
				case IDM_LOGCOMP:
					PopFileWrite ( activeFootyID, HSPTMP);
					strcpy(tmpfn, HSPTMP);
					hsc_ini( 0,(int)tmpfn, 0,0 );
					myfile();
					hsc_refname( 0,(int)compfile, 0,0 );
					strcpy( objname, HSPOBJ);
					hsc_objname( 0,(int)objname, 0,0 );
					a=hsc_comp( 1, 0, hsp_debug, 0 );
					if (a) {
						err_prt(hwnd);
						return 0;
					}				
					if (LOWORD (wParam)==IDM_COMP2) {
						err_prt(hwnd);
						return 0;
					}	
					if (LOWORD (wParam)==IDM_LOGCOMP) {
						DialogBox (hInst, "Logcomp", hwnd, (DLGPROC)LogcompDlgProc );
						if (hsp_logflag==0) return 0;
						if (hsp_clmode==0) { hsprun_log(HSPOBJ); } else { hsprun_log_cl(HSPOBJ); }
						return 0;
					}

				case IDM_RUN:
					if (hsp_clmode==0) { hsprun(HSPOBJ); } else { hsprun_cl(HSPOBJ); }
					return 0;

				case IDM_COMP3:
					exemode=0;
					DialogBox (hInst, "ExtComp", hwnd, (DLGPROC)ExtcmpDlgProc);
					if (hsp_fnstr==0) return 0;
					chklstr(hsp_extstr);
					if ( hsp_extobj ) {
						strcpy( objname,hsp_extstr );
						strcat( objname,".ax" );
						strcat( hsp_extstr,".hsp" );
						hsc_ini( 0,(int)hsp_extstr, 0,0 );
						hsc_objname( 0,(int)objname, 0,0 );
						a=hsc_comp( 0,0,0,0 );
						//a=tcomp_main( hsp_extstr, hsp_extstr, objname, errbuf,0 );
						if (a) { err_prt(hwnd);return 0; }
#ifdef JPNMSG
						TMes("オブジェクトファイルが作成されました");
#else
						TMes("Object file generated.");
#endif
						return 0;
					}
					strcpy( objname, HSPOBJ);
					strcat( hsp_extstr,".hsp" );
					hsc_ini( 0,(int)hsp_extstr, 0,0 );
					hsc_objname( 0,(int)objname, 0,0 );
					a=hsc_comp( 1, 0, hsp_debug, 0 );
					//a=tcomp_main( hsp_extstr, hsp_extstr, objname, errbuf,1 );
					if (a) { err_prt(hwnd);return 0; }
					if (hsp_clmode==0) { hsprun(objname); } else { hsprun_cl(objname); }
					return 0;

				case IDM_HSPERR:
					err_prt(hwnd);
					return 0;

				case IDM_HSPSYM:
					strcpy(tmpfn, HSPTMP);
					hsc_ini( 0,(int)tmpfn, 0,0 );
					myfile();
					hsc_refname( 0,(int)compfile, 0,0 );
					strcpy( objname, HSPOBJ);
					hsc_objname( 0,(int)objname, 0,0 );
					a=hsc3_getsym( 0,0,0,0 );
					if (a) {
#ifdef JPNMSG
						TMes("キーワードの取得に失敗しました");
#else
						TMes("Keyword analysis failed.");
#endif
						return 0;
					}
					err_prt(hwnd);
					return 0;

				case IDM_FULLSCR:
					hsp_fullscr^=1;
					return 0;
				case IDM_HSPCLMODE:
					hsp_clmode^=1;
					return 0;

				case IDM_DEBUG:
					hsp_debug^=1;
					return 0;

				case IDM_CMDOPT:
					DialogBox (hInst, "CmdBox", hwnd, (DLGPROC)OptDlgProc);
					return 0;

				case IDM_START_RUNTIMEMAN:
					ExecHSPAssistant();
					return 0;

				case IDM_OPEN_SRCFOLDER:
						GetCurrentDirectory( _MAX_PATH, tmpfn );
					ShellExecute( NULL, "explore", tmpfn, NULL, NULL, SW_SHOWNORMAL );
					return 0;

                case IDM_HSPTV:
					sprintf( tmpfn, "\"%s\\hsptv.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0;

				case IDM_DIRDPM:
					ExecMkDPM();
					return 0;

				case IDM_PACKOPT:
					ExecMkPackOpt();
					return 0;

				case IDM_SRCCNV:
					sprintf(tmpfn, "\"%s\\hsp3dh.exe\"", szExeDir);
					WinExec(tmpfn, SW_SHOW);
					return 0;

				case IDM_PAINT:
					ExecPaint();
					return 0;

				case IDM_HSP3UPDATE:
					ExecHSP3Update();
					return 0;

				case IDM_HGIMG4TOOL:
					sprintf(tmpfn, "\"%s\\gpbconv.exe\"", szExeDir);
					WinExec(tmpfn, SW_SHOW);
					return 0;

				case IDM_MAPTOOL:
					ExecMapTool();
					return 0;

				// Messages from Help menu

				case IDM_KWHELP:
					getkw();gethdir();
					callhelp();
					return 0;

                case IDM_HSPMAN1 :
					sprintf( helpopt,"%s\\doclib\\hspprog.htm", szExeDir );
					ShellExecute( NULL, NULL, helpopt, NULL, NULL, SW_SHOW );
					return 0 ;

                case IDM_HSPMAN2 :
					sprintf( tmpfn, "\"%s\\hdl.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0 ;

                case IDM_HSPMAN3 :
					sprintf( helpopt,"%s\\index.htm", szExeDir );
					ShellExecute( NULL, NULL, helpopt, NULL, NULL, SW_SHOW );
					return 0 ;

                case IDM_ABOUT :
                    DialogBox (hInst, "AboutBox", hwnd, (DLGPROC)AboutDlgProc) ;
                    return 0 ;

				// Messages for Tag Control
				case IDM_SAVETAB :
				{
                    TABINFO *lpTabInfo = GetTabInfo(ClickID);
					if (lpTabInfo->FileName[0]){
						if (PopFileWrite (lpTabInfo->FootyID, lpTabInfo->FileName)){
                            if(activeID == ClickID)
								bNeedSave = FALSE ;

							SetTabInfo(ClickID, NULL, NULL, NULL, FALSE);
//							lpTabInfo->LatestUndoNum = FootyGetMetrics(lpTabInfo->FootyID, F_GM_UNDOREM);
							lpTabInfo->LatestUndoNum = 0;	// 2008-02-17 Shark++ 代替機能未実装
							Footy2GetMetrics(activeFootyID, SM_UNDOREM, &lpTabInfo->LatestUndoNum);
							lpTabInfo->FileIndex = GetFileIndex(szFileName);
							DoCaption (szTitleName, ClickID) ;
							return 1 ;
						} else {
#ifdef JPNMSG
							OkMessage ( "セーブに失敗しました。\n[%s]", GetTabInfo(ClickID)->FileName ) ;
#else
                            OkMessage ( "Error happened in saving.\n[%s]", GetTabInfo(ClickID)->FileName ) ;
#endif
						}
						return 0 ;
					}
				}

					// fall through
                case IDM_SAVETABAS :
                        if (PopFileSaveDlg (hwnd, szFileName, szTitleName)){
							GetDirName(szDirName, szFileName);
							SetCurrentDirectory(szDirName);
							if (PopFileWrite (GetTabInfo(ClickID)->FootyID, szFileName)){
								SetTabInfo(ClickID, szTitleName, szFileName, szDirName, (bNeedSave = FALSE));

								TABINFO *lpTabInfo = GetTabInfo(ClickID);
//								lpTabInfo->LatestUndoNum = FootyGetMetrics(lpTabInfo->FootyID, F_GM_UNDOREM);
								lpTabInfo->LatestUndoNum = 0;	// 2008-02-17 Shark++ 代替機能未実装
								Footy2GetMetrics(activeFootyID, SM_UNDOREM, &lpTabInfo->LatestUndoNum);
								lpTabInfo->FileIndex = GetFileIndex(szFileName);
								DoCaption( szTitleName, ClickID );
								return 1 ;
							} else {
#ifdef JPNMSG
                                OkMessage ( "%s のセーブに失敗しました。", szTitleName) ;
#else
                                OkMessage ( "Error happened in saving.\n[%s]", szFileName ) ;
#endif
							}
						}
						return 0 ;

				case IDM_CLOSE :
				{
					TABINFO *lpTabInfo = GetTabInfo(ClickID);
					if(lpTabInfo == NULL) break;
					if(lpTabInfo->NeedSave){
						if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(ClickID);
					} else DeleteTab(ClickID);
					return 0;
				}

				case IDM_CLOSEACT :
				{
					TABINFO *lpTabInfo = GetTabInfo(activeID);
					if(lpTabInfo == NULL) break;
					if(lpTabInfo->NeedSave){
						if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(activeID);
					} else DeleteTab(activeID);
					return 0;
				}

				case IDM_CLOSEALL :
				{
					int num = TabCtrl_GetItemCount(hwndTab);
					TABINFO *lpTabInfo;

					for(int i = 0; i < num; i++){
						lpTabInfo = GetTabInfo(0);
						if(lpTabInfo == NULL) break;
						if(lpTabInfo->NeedSave){
                            if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(0);
							else break;
						} else DeleteTab(0);
					}
                    return 0;
				}

				case IDM_CLOSELEFT :
				{
					TABINFO *lpTabInfo;

					for(int i = ClickID-1; i >= 0; i--){
						lpTabInfo = GetTabInfo(i);
						if(lpTabInfo->NeedSave){
							if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(i);
							else break;
						} else DeleteTab(i);
					}
					return 0;
				}

				case IDM_CLOSERIGHT :
				{
					TABINFO *lpTabInfo;

					for(;;){
						lpTabInfo = GetTabInfo(ClickID+1);
						if(lpTabInfo == NULL) break;
						if(lpTabInfo->NeedSave){
                            if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(ClickID+1);
							else break;
						} else DeleteTab(ClickID+1);
					}
					return 0;
				}

				case IDM_CLOSEALEFT :
				{						
					TABINFO *lpTabInfo;

					for(int i = activeID-1; i >= 0; i--){
						lpTabInfo = GetTabInfo(i);
						if(lpTabInfo->NeedSave){
							if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(i);
							else break;
						} else DeleteTab(i);
					}
					return 0;
				}

				case IDM_CLOSEARIGHT :
				{
					TABINFO *lpTabInfo;

					for(;;){
						lpTabInfo = GetTabInfo(activeID+1);
						if(lpTabInfo == NULL) break;
						if(lpTabInfo->NeedSave){
                            if(IDCANCEL != AskAboutSave (hwbak, lpTabInfo->TitleName)) DeleteTab(activeID+1);
							else break;
						} else DeleteTab(activeID+1);
					}
					return 0;
				}
				

				// by Tetr@pod
				case IDM_SEPARATEWINDOWN :
				{
					Footy2ChangeView(activeFootyID, VIEWMODE_NORMAL);
					nowViewMode = VIEWMODE_NORMAL;
					return 0;
				}
				
				case IDM_SEPARATEWINDOWV :
				{
					Footy2ChangeView(activeFootyID, VIEWMODE_VERTICAL);
					nowViewMode = VIEWMODE_VERTICAL;
					return 0;
				}

				case IDM_SEPARATEWINDOWH :
				{
					Footy2ChangeView(activeFootyID, VIEWMODE_HORIZONTAL);
					nowViewMode = VIEWMODE_HORIZONTAL;
					return 0;
				}
				
				case IDM_SEPARATEWINDOWQ :
				{				
					Footy2ChangeView(activeFootyID, VIEWMODE_QUAD);
					nowViewMode = VIEWMODE_QUAD;
					return 0;
				}

				

				case IDM_OPTION:
					EnableWindow(hWndMain, FALSE);
					hConfigDlg = CreateDialog(hInst, "CONFIG", hwnd, ConfigDlgProc);
					return 0;

				case IDM_OPENIME:
				{
					HIMC hIMC;
					HWND hFooty;

					hFooty = Footy2GetWnd(activeFootyID, 0);
					if(hFooty == NULL) return 0;
					hIMC = ImmGetContext(hFooty);

					if(hIMC == NULL) return 0;
					ImmSetOpenStatus(hIMC, !ImmGetOpenStatus(hIMC));
					ImmReleaseContext(hFooty, hIMC);
					return 0;
				}
				case IDM_GOOGLE:
				{
					// Google検索
					getkw();
					sprintf(helpopt, "http://www.google.co.jp/search?q=%s&ie=Shift_JIS", kwstr);
					ShellExecute(NULL, NULL, helpopt, "", "", SW_SHOW);
					return 0;
				}

				case IDM_GODEFINE:
				{
					// 定義へ移動
					getkw();
					execGoToDefinition();
					return 0;
				}

				case IDM_ALLREF:
				{
					// すべての参照を検索
					getkw();
					if (*kwstr != 0) {
						DialogBox(hInst, "LabelBox", hwnd, (DLGPROC)KeywordRefDlgProc);
						if (cln < 0) return 0;
						OpenHSPFile(analysis_filename, analysis_line);
					}
					return 0;
				}

				case IDM_RECONVERT:
				{
					// 未実装
					keybd_event(28, 0, 0, 0);
					return 0;
				}

				case IDM_PREVTAB:
				{
					int PrevID = activeID;
					PrevID--;
					if(PrevID < 0)
						PrevID = TabCtrl_GetItemCount(hwndTab) - 1;
					if(PrevID >= 0)
						ActivateTab(activeID, PrevID);
					return 0;
				}

				case IDM_NEXTTAB:
				{
					int NextID = activeID;
					if(TabCtrl_GetItemCount(hwndTab) == 0)
						return 0;
					if(++NextID >= TabCtrl_GetItemCount(hwndTab))
						NextID = 0;
					ActivateTab(activeID, NextID);
					return 0;
				}

				case IDM_ZPREVTAB:
				{
					TABINFO *lpTabInfo = GetTabInfo(activeID);
					if(lpTabInfo == NULL)
						return 0;
					int PrevID = GetTabID(lpTabInfo->ZOrder.prev->FootyID);
					if(PrevID >= 0)
						ActivateTab(activeID, PrevID);
					return 0;
				}

				case IDM_ZNEXTTAB:
				{
					TABINFO *lpTabInfo = GetTabInfo(activeID);
					if(lpTabInfo == NULL)
						return 0;
					int NextID = GetTabID(lpTabInfo->ZOrder.next->FootyID);
					if(NextID >= 0)
						ActivateTab(activeID, NextID);
					return 0;
				}

				default:
					if(LOWORD(wParam) >= IDM_ACTIVATETAB && LOWORD(wParam) < IDM_ACTIVATETAB + 10000){
						ActivateTab(activeID, LOWORD(wParam) - IDM_ACTIVATETAB);
						return 0;
					}
					else if(LOWORD(wParam) >= IDM_EXTTOOL && LOWORD(wParam) < IDM_EXTTOOL + 10000){
						ExecExtTool(hwnd, LOWORD(wParam) - IDM_EXTTOOL, false);
					}
					break;
			}
			break ;

		}
	return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
    }

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HFONT	hStaticFont;
	static COLORREF	crStaticFont;
	switch (iMsg){
        case WM_INITDIALOG :
        {
            LOGFONT logfont;
			HWND hLink;

			hLink = GetDlgItem(hDlg, IDC_STATICLINK);
			hStaticFont = (HFONT)SendMessage(hLink, WM_GETFONT, 0, 0);
			GetObject(hStaticFont, sizeof(logfont), &logfont);
			logfont.lfUnderline=1;
			hStaticFont = CreateFontIndirect(&logfont);
			SendMessage(hLink, WM_SETFONT, (WPARAM)hStaticFont, TRUE);
			crStaticFont = RGB(0, 0, 255);
			SetClassLong(hLink, GCL_HCURSOR, (long)LoadCursor(NULL, IDC_HAND));
			return TRUE ;
		}

		case WM_CLOSE:
			DeleteObject(hStaticFont);
			break ;

		case WM_COMMAND :
			switch (LOWORD (wParam)){
				case IDOK :
                case IDCANCEL :
                    EndDialog (hDlg, 0) ;
					return TRUE ;
				case IDC_STATICLINK:
					if(HIWORD(wParam) == STN_CLICKED){
						ShellExecute(NULL, NULL, "https://hsp.tv/", NULL, NULL, SW_SHOW);
						return TRUE;
					}
					return  TRUE;
			}
			break ;

		case WM_SETCURSOR:
			if(GetDlgItem(hDlg, IDC_STATICLINK) == (HWND)wParam){
                if(crStaticFont != RGB(255, 0, 0)){
					crStaticFont = RGB(255, 0, 0);
					InvalidateRect(GetDlgItem(hDlg, IDC_STATICLINK), NULL, true);
					UpdateWindow(GetDlgItem(hDlg, IDC_STATICLINK));
				}
                return TRUE;
			} else if(GetDlgItem(hDlg, IDC_STATICLINK) != (HWND)wParam && crStaticFont == RGB(255, 0, 0)){
				crStaticFont = RGB(0, 0, 255);
				InvalidateRect(GetDlgItem(hDlg, IDC_STATICLINK), NULL, false);
				return TRUE;
			}
            break;

		case WM_CTLCOLORSTATIC:
			if(GetDlgItem(hDlg, IDC_STATICLINK) == (HWND)lParam){
				SetTextColor((HDC)wParam, crStaticFont);
				SetBkMode((HDC)wParam, TRANSPARENT);
				return (BOOL)/*(HBRUSH)*/GetStockObject(NULL_BRUSH);
			}
			break;
	}
    return FALSE ;
}

BOOL CALLBACK LogcompDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				if ( hsp_logmode & 1 ) CheckDlgButton( hDlg, IDC_CHECK1, 1 );
				if ( hsp_logmode & 2 ) CheckDlgButton( hDlg, IDC_CHECK2, 1 );
				if ( hsp_logmode & 4 ) CheckDlgButton( hDlg, IDC_CHECK3, 1 );
				if ( hsp_logmode & 8 ) CheckDlgButton( hDlg, IDC_CHECK4, 1 );
				if ( hsp_logadd ) CheckDlgButton( hDlg, IDC_CHECK5, 1 );
				return TRUE ;

          case WM_COMMAND:
               switch (GET_WM_COMMAND_ID(wParam, lParam))
                    {
                    case IDOK:
						hsp_logmode = 0;
						hsp_logadd = 0;
						if ( IsDlgButtonChecked( hDlg,IDC_CHECK1 )==BST_CHECKED ) hsp_logmode|=1;
						if ( IsDlgButtonChecked( hDlg,IDC_CHECK2 )==BST_CHECKED ) hsp_logmode|=2;
						if ( IsDlgButtonChecked( hDlg,IDC_CHECK3 )==BST_CHECKED ) hsp_logmode|=4;
						if ( IsDlgButtonChecked( hDlg,IDC_CHECK4 )==BST_CHECKED ) hsp_logmode|=8;
						if ( IsDlgButtonChecked( hDlg,IDC_CHECK5 )==BST_CHECKED ) hsp_logadd = 1;
                        EndDialog (hDlg, 0);
					    hsp_logflag=1;
                       return TRUE;
					case IDCANCEL:
						 EndDialog (hDlg, 0);
					     hsp_logflag=0;
						 return TRUE;
					case IDC_BUTTON1:
						ShellExecute( NULL,NULL,"hsplog.txt", "", "", SW_SHOW );
						break;
					}
               break ;
          }
     return FALSE ;
     }

BOOL CALLBACK ConfigDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush;
	static HFONT hFont, hOrgFont;
	static HWND hOldPage, hPage[8];
	static HIMAGELIST hil;

	switch(message){
		case WM_INITDIALOG:
		{
			HWND hTree, hSubject, hFrame;
			TVINSERTSTRUCT tvis;
			HTREEITEM hSelItem;
			RECT rect;
			POINT point;

			// ツリービューに項目を追加する
			hTree = GetDlgItem(hDlg, IDC_TREE1);

			// ページの領域計算
			// Calculate rect of page
			hFrame = GetDlgItem(hDlg, IDC_FRAME);
			GetWindowRect(hFrame, &rect);

			point.x = rect.left, point.y = rect.top;
			ScreenToClient(hDlg, &point);
			rect.left = point.x, rect.top = point.y;

			point.x = rect.right, point.y = rect.bottom;
			ScreenToClient(hDlg, &point);
			rect.right = point.x, rect.bottom = point.y;

			rect.right -= rect.left;
			rect.bottom -= rect.top;

			// ガイドとして使うためだけに有るので破棄
			// Destroy a frame window because it's used as only a guide
			DestroyWindow(hFrame);

			// ツリービューに項目を追加し、各ページを作成
			// Add items to tree view, and create pages for each items
			tvis.hInsertAfter        = TVI_LAST;
			tvis.item.mask           = TVIF_TEXT | TVIF_PARAM;

			tvis.hParent      = TVI_ROOT;
#ifdef JPNMSG
			tvis.item.pszText = "全般";
#else
			tvis.item.pszText = "Global";
#endif
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
			tvis.item.pszText = "動作";
#else
			tvis.item.pszText = "Behavior";
#endif
			tvis.item.lParam  = (LPARAM)(hPage[0] = CreateDialog(hInst, "PP_BEHAVIOR", hDlg, ConfigBehaviorPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				hSelItem = TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "ディレクトリ";
#else
				tvis.item.pszText = "Directory";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[1] = CreateDialog(hInst, "PP_DIRECTORY", hDlg, ConfigDirectoryPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
			TreeView_Expand(hTree, tvis.hParent, TVE_EXPAND);

			tvis.hParent      = TVI_ROOT;
#ifdef JPNMSG
			tvis.item.pszText = "エディタ";
#else
			tvis.item.pszText = "Editor";
#endif
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "フォント";
#else
			tvis.item.pszText = "Font";
#endif
			tvis.item.lParam  = (LPARAM)(hPage[2] = CreateDialog(hInst, "PP_FONT", hDlg, ConfigFontPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "色";
#else
				tvis.item.pszText = "Color";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[3] = CreateDialog(hInst, "PP_COLOR", hDlg, ConfigColorPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "表示";
#else
				tvis.item.pszText = "Display";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[4] = CreateDialog(hInst, "PP_NONCHARACTOR", hDlg, ConfigVisualPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "色分けキーワード";
#else
				tvis.item.pszText = "Keywords";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[5] = CreateDialog(hInst, "PP_KEYWORD", hDlg, ConfigKeywordPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
			TreeView_Expand(hTree, tvis.hParent, TVE_EXPAND);

			tvis.hParent      = TVI_ROOT;
#ifdef JPNMSG
			tvis.item.pszText = "ツール";
#else
			tvis.item.pszText = "Tool";
#endif
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "アドイン";
#else
			tvis.item.pszText = "AddOn";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[6] = CreateDialog(hInst, "PP_ADDIN", hDlg, ConfigAddinPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
#ifdef JPNMSG
				tvis.item.pszText = "外部ツール";
#else
				tvis.item.pszText = "ExtTool";
#endif
				tvis.item.lParam  = (LPARAM)(hPage[7] = CreateDialog(hInst, "PP_EXTTOOLS", hDlg, ConfigExtToolsPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
			
				TreeView_Expand(hTree, tvis.hParent, TVE_EXPAND);

			// スタティックコントロールの背景用のブラシ
			// Brush for back ground of static control
			hBrush = CreateSolidBrush(RGB(0, 32, 128));

			// スタティックコントロールのフォント
			// Font for static control
			hSubject = GetDlgItem(hDlg, IDC_SUBJECT);
			GetClientRect(hSubject, &rect);
			hOrgFont = (HFONT)SendMessage(hSubject, WM_GETFONT, 0, 0);
#ifdef JPNMSG
			hFont = CreateFont((rect.bottom-rect.top) * 3 / 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ Ｐゴシック");
#else
			hFont = CreateFont((rect.bottom - rect.top) * 3 / 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "MS Shell Dlg");
#endif
			SendMessage(hSubject, WM_SETFONT, (WPARAM)hFont, TRUE);

			// デフォルトのページを表示させる
			// Show default page
			TreeView_SelectItem(hTree, hSelItem);
			return TRUE;
		}

		case WM_CTLCOLORSTATIC:
			// スタティックコントロールの背景色を設定
			// Set color of static control
			SetTextColor((HDC)wParam, RGB(255,255,255));
			SetBkColor((HDC)wParam, RGB(0, 32, 128));
			return (BOOL)hBrush;

		case WM_DESTROY:
			// スタティックコントロールに使ったオブジェクトの解放
			// Release objects used for static control
			DeleteObject(hBrush);
			SendMessage(GetDlgItem(hDlg, IDC_SUBJECT), WM_SETFONT, (WPARAM)hOrgFont, TRUE);
			DeleteObject(hFont);

			ImageList_Destroy(hil);

			// 後始末
			// Clean up
			hConfigDlg  = NULL;
			hConfigPage = NULL;
			break;

		case WM_COMMAND:
			switch(wParam){
				case IDOK:
				{
					BOOL bSuccess = TRUE;
					for(int i = 0; i < 8; i++)
						if(SendMessage(hPage[i], PM_ISAPPLICABLE, 0, 0L) == FALSE)
							bSuccess = FALSE;
					if(bSuccess == FALSE)
						return TRUE;

					for(int i = 0; i < 8; i++)
						SendMessage(hPage[i], PM_APPLY, 0, 0L);
				}
				
				case IDCANCEL:
					EnableWindow(hWndMain, TRUE);
					SetForegroundWindow(hWndMain);
					Footy2SetFocus(activeFootyID, 0);
					DestroyWindow(hDlg);
					return TRUE;

				case IDM_BUTTON1:
#ifdef JPNMSG
					if(MessageBox(hDlg, "現在開いているページの設定を初期状態に戻しますが、よろしいですか？\n"
						"([OK]ボタンを押さなければ、エディタには反映されません)", "確認", MB_YESNO | MB_ICONQUESTION) == IDYES)
#else
					if (MessageBox(hDlg, "Restore default. Are you sure?\n"
						"( Press OK to save setting )", "Notice", MB_YESNO | MB_ICONQUESTION) == IDYES)
#endif
							SendMessage(hOldPage, PM_SETDEFAULT, 0, 0L);
					return TRUE;
			}
			break;

		case WM_NOTIFY:
		{
			NMTREEVIEW *pnmtv = (LPNMTREEVIEW)lParam;
			TVITEM tvi;
			char buf[256];

			// 設定のページを変更する
			// Change the page to new config page
			if(pnmtv->hdr.code == TVN_SELCHANGED && pnmtv->itemNew.lParam != NULL && (HWND)pnmtv->itemNew.lParam != hOldPage){
				hConfigPage = (HWND)pnmtv->itemNew.lParam;
				ShowWindow(hConfigPage, SW_SHOW);
				ShowWindow(hOldPage, SW_HIDE);

				tvi.mask = TVIF_TEXT;
				tvi.hItem = pnmtv->itemNew.hItem;
				tvi.pszText = buf;
				tvi.cchTextMax = sizeof(buf);
				TreeView_GetItem(pnmtv->hdr.hwndFrom, &tvi);
				SetDlgItemText(hDlg, IDC_SUBJECT, tvi.pszText);

				hOldPage = hConfigPage;
				return TRUE;
			}
			break;
		}
	}
	return FALSE;
}

BOOL CALLBACK ConfigBehaviorPageProc (HWND hDlg, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(message){
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_RADIO1, !bUseIni);
			CheckDlgButton(hDlg, IDC_RADIO2, bUseIni);
			CheckDlgButton(hDlg, IDC_RADIO3+hsp_helpmode, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK1, bAutoIndent != FALSE);
			CheckDlgButton(hDlg, IDC_CHECK2, flg_hspat );
			CheckDlgButton(hDlg, IDC_CHECK3, BackupNoDelete );
			CheckDlgButton(hDlg, IDC_CHECK4, NotifyBackup );
			SetDlgItemInt(hDlg, IDC_EDIT1, autobackup, FALSE);
			CheckDlgButton(hDlg, IDC_CHECK5, UseNewReplace);// by Tetr@pod
			return TRUE;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			bUseIni     = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_RADIO2);
			bAutoIndent = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK1);
			hsp_helpmode= CheckRadio(hDlg, IDC_RADIO3, 4);
			flg_hspat   = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			autobackup = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
			BackupNoDelete = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK3);
			NotifyBackup = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK4);
			UseNewReplace = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK5);// by Tetr@pod
			// タイマー削除
			KillTimer(hWndMain, AutoBackupTimer);
			if (autobackup > 0){
				// 再登録
				AutoBackupTimer = SetTimer(hWndMain, TIMERID_AUTOBACKUP, autobackup*1000, 0);
			}
			return TRUE;

		case PM_SETDEFAULT:
			CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO4, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO5, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO6, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK2, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK4, BST_UNCHECKED);
			SetDlgItemInt(hDlg, IDC_EDIT1, 0, FALSE);
			CheckDlgButton(hDlg, IDC_CHECK5, BST_UNCHECKED);// by Tetr@pod
			return TRUE;
	}
	return FALSE;
}
BOOL CALLBACK ConfigAddinPageProc (HWND hDlg, UINT message, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch(message){
		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			return TRUE;

		case PM_SETDEFAULT:
			return TRUE;
	}
	return FALSE;
}

COLORREF crDefColor[] = {
	RGB(  0,   0,   0), // 黒
	RGB(255, 255, 255), // 白
	RGB(255,   0,   0), // 赤
	RGB(  0, 128,   0), // 緑
	RGB(  0,   0, 255), // 青
	RGB(255, 255,   0), // 黄
	RGB(  0, 255,   0), // 黄緑
	RGB(  0, 255, 255), // 水色
	RGB(128,   0, 128), // 紫
	RGB(255,   0, 255), // マゼンタ
	RGB(255, 128,   0), // 橙
	RGB(192, 255, 255), // 深緑
	RGB(  0, 128, 128), // 空色
	RGB(128,   0,   0), // こげ茶色
	RGB( 49, 117, 189), // 行番号の既定色
	RGB( 41, 178, 132), // 行番号とエディタの境界線の既定色
	RGB(239, 231,  49), // キャレット行の行番号強調の既定色
	RGB(198,  48,  90), // キャレット行の下線の既定色
	RGB(239, 235, 222), // ルーラー背景の既定色
	RGB(  0, 203, 206), // ルーラーの選択位置強調の既定色

};

BOOL CALLBACK ConfigColorPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static MYCOLORREF crColor[21];
#ifdef JPNMSG
	const char *szCategory[] = { "文字の色分け", "特殊な記号の配色", "エディタの配色", "行番号表示領域の配色", "ルーラーの配色", "リンクの配色", "ユーザー定義命令・関数の色分け"};
	const char *szCharItem[] = { "通常の文字", "命令/関数", "プリプロセッサ命令", "文字列", "マクロ", "コメント", "ラベル" };
	const char *szNonCharItem[] = { "半角スペース", "全角スペース", "TAB文字", "改行記号", "[EOF](ファイル終端)記号" };
	const char *szEditItem[] = { "全体の背景色", "キャレット行の下線", "行番号とエディタの境界線" };
	const char *szLineNumItem[] = { "行番号", "キャレット行の強調" };
	const char *szRolerItem[] = { "数字", "背景色", "目盛り", "キャレット位置の強調" };

	const char *szClickableItem[] = { "URL", "URLのアンダーバー", "メールアドレス", "メールアドレスのアンダーバー", "リンクラベル", "リンクラベルのアンダーバー" };// 便宜上リンクラベルと呼びます by Tetr@pod
#else
	const char *szCategory[] = { "Words color", "Special symbol", "Editor color", "Line Number color", "ruler color", "link color", "User command/function" };
	const char *szCharItem[] = { "Words", "command/function", "preprocessor", "strings", "macro", "comment", "label" };
	const char *szNonCharItem[] = { "Space", "JPN Space", "Tab", "CR", "[EOF](End of File)" };
	const char *szEditItem[] = { "Background color", "Caret underline", "Divider" };
	const char *szLineNumItem[] = { "Line Number", "Line Emphasize" };
	const char *szRolerItem[] = { "Numeric", "Background color", "Scaler", "Caret Emphasize" };

	const char *szClickableItem[] = { "URL", "URL underline", "Mail Address", "Mail Address underline", "Link Label", "Link Label underline" };// 便宜上リンクラベルと呼びます by Tetr@pod
#endif
	const char *szUserFuncItem[] = { "#deffunc, #defcfunc", "#modinit, #modterm", "#modfunc, #modcfunc", "#func, #cfunc", "#cmd", "#comfunc", "#define", "#define ctype" };// by Tetr@pod

	typedef struct tagItems{
		const char **item;
		int num;
	} ITEMS;

	ITEMS items[] = {
		szCharItem,      sizeof(szCharItem)      / sizeof(szCharItem[0]),
		szNonCharItem,   sizeof(szNonCharItem)   / sizeof(szNonCharItem[0]),
		szEditItem,      sizeof(szEditItem)      / sizeof(szEditItem[0]),
		szLineNumItem,   sizeof(szLineNumItem)   / sizeof(szLineNumItem[0]),
		szRolerItem,     sizeof(szRolerItem)     / sizeof(szRolerItem[0]),

		szClickableItem, sizeof(szClickableItem) / sizeof(szClickableItem[0]),// by Tetr@pod
		szUserFuncItem,  sizeof(szUserFuncItem)  / sizeof(szUserFuncItem[0]),// by Tetr@pod
	};

	switch(message){
		case WM_INITDIALOG:
		{
			int i, num;
			num = sizeof(szCategory) / sizeof(szCategory[0]);
			for(i = 0; i < num; i++)
				SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)szCategory[i]);
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 0, 0L);

#ifdef JPNMSG
			const char *szColorName[] = {
				"黒",
				"白",
				"赤",
				"緑",
				"青",
				"黄",
				"黄緑",
				"水色",
				"紫",
				"マゼンタ",
				"橙",
				"深緑",
				"空色",
				"こげ茶色",
				"行番号の既定色",
				"行番号とエディタの境界線の既定色",
				"キャレット行の行番号強調の既定色",
				"キャレット行の下線の既定色",
				"ルーラー背景の既定色",
				"ルーラー強調の既定色",
				"ユーザー定義...",
			};
#else
			const char *szColorName[] = {
				"Black",
				"White",
				"Red",
				"Green",
				"Blue",
				"Yellow",
				"Yellow Green",
				"Light Blue",
				"Pink",
				"Magenta",
				"Orange",
				"Dark Green",
				"Sky Blue",
				"Dark Brown",
				"Line Number Default",
				"Divider Default",
				"Caret Line Number Default",
				"Caret Line Underline Default",
				"Ruler Background Default",
				"Ruler Emphasis Default",
				"User Define...",
			};
#endif

			num = sizeof(szColorName) / sizeof(szColorName[0]);
			for(i = 0; i < num; i++)
				SendDlgItemMessage(hDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)szColorName[i]);

			CopyMemory(crColor, &color, sizeof(crColor));
			PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_COMBO1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_COMBO1));

			if (bCustomColor == TRUE){
				CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
			}else{
				CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			}
			CheckDlgButton(hDlg, IDC_CHECK2, speedDraw );

			CheckDlgButton(hDlg, IDC_CHECK3, CustomMatchCase );// by Tetr@pod
			CheckDlgButton(hDlg, IDC_CHECK4, UseClickableLabel );// by Tetr@pod
			CheckDlgButton(hDlg, IDC_CHECK5, UseSearchFunction );// by Tetr@pod

			CheckDlgButton(hDlg, IDC_CHECK6, ChangeColor_func );// by Tetr@pod
			CheckDlgButton(hDlg, IDC_CHECK7, ChangeColor_define );// by Tetr@pod

			return TRUE;
		}

		case WM_COMMAND:
			switch(HIWORD(wParam)){
				case CBN_SELCHANGE:
				{
					int cursel = (int)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0L);
					switch(LOWORD(wParam)){
						case IDC_COMBO1:
						{
							SendDlgItemMessage(hDlg, IDC_COMBO2, CB_RESETCONTENT, 0, 0L);
							int num = items[cursel].num;
							for(int i = 0; i < num; i++)
								SendDlgItemMessage(hDlg, IDC_COMBO2, CB_ADDSTRING, 0, (LPARAM)items[cursel].item[i]);
							SendDlgItemMessage(hDlg, IDC_COMBO2, CB_SETCURSEL, 0, 0L);
							PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_COMBO2, CBN_SELCHANGE),
								(LPARAM)GetDlgItem(hDlg, IDC_COMBO2));
							return TRUE;
						}
						
						case IDC_COMBO2:
						{
							int offset = 0, num = (int)SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0L), i;
							for(i = 0; i < num; i++)
								offset += items[i].num;
							offset += cursel;
							num = sizeof(crDefColor) / sizeof(crDefColor[0]);
							for(i = 0; i < num; i++)
								if(crDefColor[i] == crColor[offset].Conf)
                                    break;
							SendDlgItemMessage(hDlg, IDC_COMBO3, CB_SETCURSEL, i, 0L);
							return TRUE;
						}

						case IDC_COMBO3:
						{
							int offset = 0, num = (int)SendDlgItemMessage(hDlg, IDC_COMBO1, CB_GETCURSEL, 0, 0L);
							for(int i = 0; i < num; i++)
								offset += items[i].num;
							offset += (int)SendDlgItemMessage(hDlg, IDC_COMBO2, CB_GETCURSEL, 0, 0L);

							COLORREF ret;
							if(cursel < 20)
								crColor[offset].Conf = crDefColor[cursel];
							else if((ret = PopFontChooseColor(hDlg, crColor[cursel].Combo)) != -1)
								crColor[offset].Conf = crColor[offset].Combo = ret;
							else
								crColor[offset].Conf = crColor[offset].Combo;
							return TRUE;
						}
					}
					break;
				}
			}
		return FALSE;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			CopyMemory(&color, crColor, sizeof(crColor));
			SetAllEditColor();
			bCustomColor       = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK1);
			speedDraw          = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK2);

			CustomMatchCase    = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK3);// by Tetr@pod
			UseClickableLabel  = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK4);// by Tetr@pod
			UseSearchFunction  = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK5);// by Tetr@pod

			ChangeColor_func     = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK6);// by Tetr@pod
			ChangeColor_define   = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK7);// by Tetr@pod


			ResetClassify();
			return TRUE;

		case PM_SETDEFAULT:
			DefaultColor((MYCOLOR *)crColor);
			PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_COMBO1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_COMBO1));
			CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);// by Tetr@pod
			CheckDlgButton(hDlg, IDC_CHECK4, BST_UNCHECKED);// by Tetr@pod
			CheckDlgButton(hDlg, IDC_CHECK5, BST_UNCHECKED);// by Tetr@pod
			SetDlgItemInt(hDlg, IDC_EDIT1, 0, FALSE);// by Tetr@pod
			return TRUE;

	}
	return FALSE;
}
BOOL CALLBACK ConfigDirectoryPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch(message){
		case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_EDIT1, startdir);
			SetDlgItemText(hDlg, IDC_EDIT2, hsp_helpdir);
			SetFocus(GetDlgItem(hDlg, IDC_EDIT1));
			CheckDlgButton(hDlg, IDC_RADIO1+startflag, BST_CHECKED);

			BOOL bEnabled = startflag == 1;
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), bEnabled);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), bEnabled);
			return TRUE;
		}

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)){
				case IDC_BUTTON1:
					if(selfolder(hdir) == 0)
						SetDlgItemText(hDlg, IDC_EDIT1, hdir);
					return TRUE;
				case IDC_BUTTON2:
					if(selfolder(hdir) == 0)
						SetDlgItemText(hDlg, IDC_EDIT2, hdir);
					return TRUE;
				case IDC_RADIO1:
				case IDC_RADIO2:
				case IDC_RADIO3:
				{
					BOOL bEnabled = IsDlgButtonChecked(hDlg, IDC_RADIO2);
					EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), bEnabled);
					EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), bEnabled);
					return TRUE;
				}
			}
			return FALSE;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			startflag = CheckRadio(hDlg, IDC_RADIO1, 4);
			GetDlgItemText(hDlg, IDC_EDIT1, startdir, 128);
			GetDlgItemText(hDlg, IDC_EDIT2, hsp_helpdir, 128);
			return TRUE;

		case PM_SETDEFAULT:
		{
			SetDlgItemText(hDlg, IDC_EDIT1, "");
			SetDlgItemText(hDlg, IDC_EDIT2, "");
			CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO3, BST_UNCHECKED);

			BOOL bEnabled = STARTDIR_MYDOC == 1;
			EnableWindow(GetDlgItem(hDlg, IDC_EDIT1), bEnabled);
			EnableWindow(GetDlgItem(hDlg, IDC_BUTTON1), bEnabled);
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL CALLBACK SelectExtToolProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static EXTTOOLINFO *lpExtToolInfo;

	switch(message){
		case WM_INITDIALOG:
		{
			lpExtToolInfo = (EXTTOOLINFO *)lParam;

			SetDlgItemText(hDlg, IDC_EDIT1, lpExtToolInfo->ToolName);
			SetDlgItemText(hDlg, IDC_EDIT2, lpExtToolInfo->FileName);
			SetDlgItemText(hDlg, IDC_EDIT3, lpExtToolInfo->CmdLine);
			SetDlgItemText(hDlg, IDC_EDIT4, lpExtToolInfo->WorkDir);
			CheckDlgButton(hDlg, IDC_CHECK1, lpExtToolInfo->ShowOnMainMenu    ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK2, lpExtToolInfo->ShowOnPopupMenu   ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK3, lpExtToolInfo->ExecOnStartup     ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK4, lpExtToolInfo->ExecWithOverwrite ? BST_CHECKED : BST_UNCHECKED);
            return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_BUTTON1:
				{
					char szFileName[_MAX_PATH + 1], szFileTitle[_MAX_PATH + 1] = "";

					GetDlgItemText(hDlg, IDC_EDIT2, szFileName, sizeof(szFileName) - 1);
					if(PopFileOpenDlg2(hDlg, szFileName, szFileTitle))
						SetDlgItemText(hDlg, IDC_EDIT2, szFileName);
					return TRUE;
				}
				
				case IDC_REF_CMDLINE:
				case IDC_REF_WORKDIR:
				{
					HMENU hMenuPopup;
					HWND hCtrl = GetDlgItem(hDlg, LOWORD(wParam));
					RECT rcCtrl;
					UINT uID;
					UINT uEditID;
					TCHAR szInsertText[64] = "";

					switch( LOWORD(wParam) ) {
						case IDC_REF_CMDLINE:
							hMenuPopup = LoadMenu(hInst, "CONTEXTMENU3");
							uEditID = IDC_EDIT3;
							break;
						case IDC_REF_WORKDIR:
							hMenuPopup = LoadMenu(hInst, "CONTEXTMENU4");
							uEditID = IDC_EDIT4;
							break;
					}

					GetWindowRect(hCtrl, &rcCtrl);
					uID = (UINT)TrackPopupMenu(
									  GetSubMenu(hMenuPopup, 0)
									, TPM_NONOTIFY|TPM_RETURNCMD
									, rcCtrl.right, rcCtrl.top
									, 0, hDlg, NULL);

					switch( LOWORD(wParam) ) {
						case IDC_REF_CMDLINE:
							switch( uID ) {
								case 1: lstrcpyn(szInsertText, "%F", 64); break;
								case 2: lstrcpyn(szInsertText, "%D", 64); break;
							}
							break;
						case IDC_REF_WORKDIR:
							switch( uID ) {
								case 1: lstrcpyn(szInsertText, "%F", 64); break;
								case 2: lstrcpyn(szInsertText, "%D", 64); break;
							}
							break;
					}
					if( *szInsertText ) {
						SendMessage(
							  GetDlgItem(hDlg, uEditID)
							, EM_REPLACESEL
							, (WPARAM)TRUE, (LPARAM)szInsertText);
					}

					DestroyMenu(hMenuPopup);

					return TRUE;
				}

				case IDOK:
					if(GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT2)) == 0){
#ifdef JPNMSG
						MessageBox(hDlg, "ファイル名を省略することはできません。", "error", MB_OK | MB_ICONERROR);
#else
						MessageBox(hDlg, "You must specify a file name.", "error", MB_OK | MB_ICONERROR);
#endif
						return TRUE;
					}

                    GetDlgItemText(hDlg, IDC_EDIT1, lpExtToolInfo->ToolName, SIZE_OF_TOOLNAME);
					GetDlgItemText(hDlg, IDC_EDIT2, lpExtToolInfo->FileName, SIZE_OF_FILENAME);
					GetDlgItemText(hDlg, IDC_EDIT3, lpExtToolInfo->CmdLine,  SIZE_OF_CMDLINE);
					GetDlgItemText(hDlg, IDC_EDIT4, lpExtToolInfo->WorkDir,  SIZE_OF_WORKDIR);
					lpExtToolInfo->ShowOnMainMenu    = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK1);
					lpExtToolInfo->ShowOnPopupMenu   = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK2);
					lpExtToolInfo->ExecOnStartup     = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK3);
					lpExtToolInfo->ExecWithOverwrite = BST_CHECKED == IsDlgButtonChecked(hDlg, IDC_CHECK4);

				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
			}

	}
	return FALSE;
}

BOOL CALLBACK ConfigExtToolsPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
		case WM_INITDIALOG:
		{
			HWND hListView;
			LVITEM lvi;
#ifdef JPNMSG
			char *szColumnTitle[] = { "名前", "ファイル パス", };
#else
			char *szColumnTitle[] = { "Name", "File Path", };
#endif
			LVCOLUMN lvc;
			int i, nSize;
			EXTTOOLINFO *lpExtToolInfo, *lpNewExtToolInfo;

			hListView = GetDlgItem(hDlg, IDC_LIST1);
			ListView_SetExtendedListViewStyle(hListView, ListView_GetExtendedListViewStyle(hListView) | LVS_EX_FULLROWSELECT);

            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			for(i = 0; i < sizeof(szColumnTitle) / sizeof(szColumnTitle[0]); i++){
                lvc.pszText  = szColumnTitle[i];
				lvc.cx       = ColumnWidth[i];
				lvc.iSubItem = i;
				ListView_InsertColumn(hListView, i, &lvc);
			}

			nSize = GetExtToolSize();
			for(i = 0; i < nSize; i++){
				lpExtToolInfo = GetExtTool(i);
				if(lpExtToolInfo == NULL) break;
				if(lpExtToolInfo->Used){
					lpNewExtToolInfo = (EXTTOOLINFO *)malloc(sizeof(EXTTOOLINFO));
					if(lpNewExtToolInfo == NULL) break;
					*lpNewExtToolInfo = *lpExtToolInfo;

					lvi.iItem  = i;

					lvi.mask     = LVIF_TEXT | LVIF_PARAM;
					lvi.iSubItem = 0;
					lvi.pszText  = lpExtToolInfo->ToolName;
					lvi.lParam   = (LPARAM)lpNewExtToolInfo;
					ListView_InsertItem(hListView, &lvi);

					lvi.mask     = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText  = lpExtToolInfo->FileName;
					ListView_SetItem(hListView, &lvi);
				}
			}
			return TRUE;
		}

		case WM_DESTROY:
		{
			HWND hListView;
			int nItemNum;
			LVITEM lvi;
			EXTTOOLINFO *lpExtToolInfo;

			hListView = GetDlgItem(hDlg, IDC_LIST1);
			ColumnWidth[0] = ListView_GetColumnWidth(hListView, 0);
			ColumnWidth[1] = ListView_GetColumnWidth(hListView, 1);

			lvi.mask = LVIF_PARAM;
			lvi.iSubItem = 0;
			nItemNum = ListView_GetItemCount(hListView);
			for(int i = 0; i < nItemNum; i++){
				lvi.iItem = i;
				ListView_GetItem(hListView, &lvi);
				lpExtToolInfo = (EXTTOOLINFO *)lvi.lParam;
				free(lpExtToolInfo);
			}
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_BUTTON1:
				{
					HWND hListView;
					LVITEM lvi;
					EXTTOOLINFO *lpExtToolInfo;

					lpExtToolInfo = (EXTTOOLINFO *)malloc(sizeof(EXTTOOLINFO));
					if(lpExtToolInfo == NULL) return 0;

					lstrcpy(lpExtToolInfo->ToolName, "");
					lstrcpy(lpExtToolInfo->FileName, "");
					lstrcpy(lpExtToolInfo->CmdLine, "");
					lstrcpy(lpExtToolInfo->WorkDir, "");
					lpExtToolInfo->ShowOnMainMenu    = true;
					lpExtToolInfo->ShowOnPopupMenu   = false;
					lpExtToolInfo->ExecOnStartup     = false;
					lpExtToolInfo->ExecWithOverwrite = false;
					if(DialogBoxParam(hInst, "EXTTOOLS", hDlg, SelectExtToolProc, (LPARAM)lpExtToolInfo) == IDOK){
						hListView = GetDlgItem(hDlg, IDC_LIST1);
						lvi.iItem  = 0;

						lvi.mask     = LVIF_TEXT | LVIF_PARAM;
						lvi.iSubItem = 0;
						lvi.pszText  = lpExtToolInfo->ToolName;
						lvi.lParam   = (LPARAM)lpExtToolInfo;
						ListView_InsertItem(hListView, &lvi);

						lvi.mask     = LVIF_TEXT;
						lvi.iSubItem = 1;
						lvi.pszText  = lpExtToolInfo->FileName;
						ListView_SetItem(hListView, &lvi);
					} else {
						free(lpExtToolInfo);
					}
					return TRUE;
				}

				case IDC_BUTTON2:
				{
					HWND hListView;
					int nCurSel;
					EXTTOOLINFO *lpExtToolInfo;
					LVITEM lvi;

					hListView = GetDlgItem(hDlg, IDC_LIST1);
					nCurSel = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
					if(nCurSel < 0) return 0;

					lvi.mask     = LVIF_PARAM;
					lvi.iItem    = nCurSel;
					lvi.iSubItem = 0;
					ListView_GetItem(hListView, &lvi);
					lpExtToolInfo = (EXTTOOLINFO *)lvi.lParam;
					if(lpExtToolInfo == NULL) return 0;

					if(DialogBoxParam(hInst, "EXTTOOLS", hDlg, SelectExtToolProc, lvi.lParam) == IDOK){
						lvi.mask = LVIF_TEXT;

						lvi.iSubItem = 0;
						lvi.pszText  = lpExtToolInfo->ToolName;
						ListView_SetItem(hListView, &lvi);

						lvi.iSubItem = 1;
						lvi.pszText  = lpExtToolInfo->FileName;
						ListView_SetItem(hListView, &lvi);
					}
					return TRUE;
				}

				case IDC_BUTTON3:
				{
					HWND hListView;
					int nCurSel;
					LVITEM lvi;

					hListView = GetDlgItem(hDlg, IDC_LIST1);
					nCurSel = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
					if(nCurSel < 0) return TRUE;

					lvi.mask     = LVIF_PARAM;
					lvi.iItem    = nCurSel;
					lvi.iSubItem = 0;
					ListView_GetItem(hListView, &lvi);

					free((void *)lvi.lParam);

					ListView_DeleteItem(hListView, nCurSel);
					return TRUE;
				}

				case IDC_MOVE_UP:
				{
					HWND hListView;
					int nCurSel;
					LVITEM lvi;

					hListView = GetDlgItem(hDlg, IDC_LIST1);
					nCurSel = ListView_GetNextItem(hListView, -1, LVNI_ALL | LVNI_SELECTED);
					if(nCurSel < 1) return TRUE;

					lvi.mask     = LVIF_PARAM;
					lvi.iItem    = nCurSel;
					lvi.iSubItem = 0;
					ListView_GetItem(hListView, &lvi);

					ListView_DeleteItem(hListView, nCurSel);
					
					lvi.iItem--;
					lvi.mask     = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
					lvi.pszText  = ((EXTTOOLINFO*)lvi.lParam)->ToolName;
					lvi.state    = LVIS_SELECTED|LVIS_FOCUSED;
					lvi.stateMask= LVIS_SELECTED|LVIS_FOCUSED;
					ListView_InsertItem(hListView, &lvi);
					
					lvi.mask     = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText  = ((EXTTOOLINFO*)lvi.lParam)->FileName;
					ListView_SetItem(hListView, &lvi);

					SetFocus(hListView);

					return TRUE;
				}

				case IDC_MOVE_DOWN:
				{
					HWND hListView;
					int nCurSel;
					LVITEM lvi;

					hListView = GetDlgItem(hDlg, IDC_LIST1);
					nCurSel = ListView_GetNextItem(hListView, -1, LVNI_ALL | LVNI_SELECTED);
					if(nCurSel < 0 || ListView_GetItemCount(hListView) <= nCurSel + 1) return TRUE;

					lvi.mask     = LVIF_PARAM;
					lvi.iItem    = nCurSel;
					lvi.iSubItem = 0;
					ListView_GetItem(hListView, &lvi);

					ListView_DeleteItem(hListView, nCurSel);
					
					lvi.iItem++;
					lvi.mask     = LVIF_TEXT | LVIF_PARAM | LVIF_STATE;
					lvi.pszText  = ((EXTTOOLINFO*)lvi.lParam)->ToolName;
					lvi.state    = LVIS_SELECTED|LVIS_FOCUSED;
					lvi.stateMask= LVIS_SELECTED|LVIS_FOCUSED;
					ListView_InsertItem(hListView, &lvi);
					
					lvi.mask     = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText  = ((EXTTOOLINFO*)lvi.lParam)->FileName;
					ListView_SetItem(hListView, &lvi);

					SetFocus(hListView);

					return TRUE;
				}
			}
			break;

		case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			if(lpnmhdr->idFrom == IDC_LIST1 && lpnmhdr->code == NM_DBLCLK){
				PostMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON2, 0), NULL);
                return TRUE;
			}
			break;
		}

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
		{
			HWND hListView;
			int nSize, i;
			LVITEM lvi;

			hListView = GetDlgItem(hDlg, IDC_LIST1);
			nSize = ListView_GetItemCount(hListView);

			InitExtTool(nSize);
			for(i = 0; i < nSize; i++){
				lvi.mask     = LVIF_PARAM;
				lvi.iItem    = i;
				lvi.iSubItem = 0;
				ListView_GetItem(hListView, &lvi);

				(EXTTOOLINFO *)lvi.lParam;
				AddExtTool(i, (EXTTOOLINFO *)lvi.lParam);
			}
			SetMenuExtTool();
			return TRUE;
		}

		case PM_SETDEFAULT:
			return TRUE;
	}
	return FALSE;
}
static void GetStyleStr(char *buf, LOGFONT *logfont)
{
	lstrcpy(buf, "");
	if (logfont->lfWeight >= FW_BOLD) {
#ifdef JPNMSG
		lstrcpy(buf, "太字 ");
#else
		lstrcpy(buf, "Bold ");
#endif
	}
	if (logfont->lfItalic) {
#ifdef JPNMSG
		lstrcat(buf, "斜体");
#else
		lstrcat(buf, "Italic");
#endif
	}
	if (lstrlen(buf) == 0) {
#ifdef JPNMSG
		lstrcpy(buf, "標準");
#else
		lstrcpy(buf, "Normal");
#endif
	}
}
static void GetExStyleStr(char *buf, LOGFONT *logfont)
{
	lstrcpy(buf, "");
	if (logfont->lfStrikeOut) {
#ifdef JPNMSG
		lstrcat(buf, "取り消し線 ");
#else
		lstrcat(buf, "Strikeout ");
#endif
	}
	if (logfont->lfUnderline) {
#ifdef JPNMSG
		lstrcat(buf, "下線");
#else
		lstrcat(buf, "Underline");
#endif
	}
	if (lstrlen(buf) == 0) {
#ifdef JPNMSG
		lstrcpy(buf, "無し");
#else
		lstrcpy(buf, "None");
#endif
	}
	return;
}
BOOL CALLBACK ConfigFontPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	static HFONT hEditFont, hTabFont, hOrgEditFont, hOrgTabFont;
	switch(message){
		case WM_INITDIALOG:
		{
			char str[1280];
			LOGFONT editfont, tabfont;
			HDC hDC = CreateCompatibleDC(NULL);
			editfont = PopFontGetELG();
			tabfont  = PopFontGetTLG();
			SetDlgItemText(hDlg, IDC_STATIC1, editfont.lfFaceName);
			SetDlgItemInt(hDlg, IDC_STATIC2, (MulDiv(abs(editfont.lfHeight), 720, GetDeviceCaps(hDC, LOGPIXELSY)) + 5) / 10, FALSE);
			SetDlgItemText(hDlg, IDC_STATIC3, tabfont.lfFaceName);
			SetDlgItemInt(hDlg, IDC_STATIC4, (MulDiv(abs(tabfont.lfHeight), 720, GetDeviceCaps(hDC, LOGPIXELSY)) + 5) / 10, FALSE);
			GetStyleStr(str, &tabfont);
			SetDlgItemText(hDlg, IDC_STATIC5, str);
			GetExStyleStr(str, &tabfont);
			SetDlgItemText(hDlg, IDC_STATIC6, str);
			DeleteDC(hDC);

			hEditFont = CreateFontIndirect(&editfont);
			hTabFont = CreateFontIndirect(&tabfont);
			SendDlgItemMessage(hDlg, IDC_STATIC7, WM_SETFONT, (WPARAM)hEditFont, TRUE);
			SendDlgItemMessage(hDlg, IDC_STATIC8, WM_SETFONT, (WPARAM)hTabFont, TRUE);				

			if (forcefont == 1){
				CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
			}else{
				CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			}

			PopFontInitChooseFont(NULL, NULL);

			return TRUE;
		}

		case WM_DESTROY:
			DeleteObject(hEditFont);
			DeleteObject(hTabFont);
			return TRUE;

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)){
				case IDC_BUTTON1:
				{
					LOGFONT *logfont;
					HFONT hOrgFont;
					LONG ret = PopFontChooseEditFont(hDlg);
					if(LOWORD(ret)){
						logfont = PopFontGetTELG();
						SetDlgItemText(hDlg, IDC_STATIC1, logfont->lfFaceName);
						SetDlgItemInt(hDlg, IDC_STATIC2, HIWORD(ret) / 10, FALSE);

						hOrgFont = hEditFont;
						hEditFont = CreateFontIndirect(logfont);
						SendDlgItemMessage(hDlg, IDC_STATIC7, WM_SETFONT, (WPARAM)hEditFont, TRUE);
						DeleteObject(hOrgFont);
					}
					return TRUE;
				}

				case IDC_BUTTON2:
				{
					char str[1280];
					LOGFONT *tabfont;
					HFONT hOrgFont;

					LONG ret = PopFontChooseTabFont(hDlg);
					if(LOWORD(ret)){
						tabfont = PopFontGetTTLG();
						SetDlgItemText(hDlg, IDC_STATIC3, tabfont->lfFaceName);
						SetDlgItemInt(hDlg, IDC_STATIC4, HIWORD(ret) / 10, FALSE);
						GetStyleStr(str, tabfont);
						SetDlgItemText(hDlg, IDC_STATIC5, str);
						GetExStyleStr(str, tabfont);
						SetDlgItemText(hDlg, IDC_STATIC6, str);

						hOrgFont = hEditFont;
						hTabFont = CreateFontIndirect(tabfont);
						SendDlgItemMessage(hDlg, IDC_STATIC8, WM_SETFONT, (WPARAM)hTabFont, TRUE);
						DeleteObject(hOrgFont);
					}
				}
                return TRUE;
			}
			return FALSE;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			PopFontApplyEditFont();
			PopFontApplyTabFont();
			
			forcefont = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			return TRUE;

		case PM_SETDEFAULT:
		{
			char str[1280];
			LOGFONT editfont, tabfont;
			HFONT hOrgEditFont, hOrgTabFont;

			HDC hDC = CreateCompatibleDC(NULL);
			DefaultFont(&editfont, &tabfont);

			SetDlgItemText(hDlg, IDC_STATIC1, editfont.lfFaceName);
			SetDlgItemInt(hDlg, IDC_STATIC2, (MulDiv(abs(editfont.lfHeight), 720, GetDeviceCaps(hDC, LOGPIXELSY)) + 5) / 10, FALSE);
			SetDlgItemText(hDlg, IDC_STATIC3, tabfont.lfFaceName);
			SetDlgItemInt(hDlg, IDC_STATIC4, (MulDiv(abs(tabfont.lfHeight), 720, GetDeviceCaps(hDC, LOGPIXELSY)) + 5) / 10, FALSE);
			GetStyleStr(str, &tabfont);
			SetDlgItemText(hDlg, IDC_STATIC5, str);
			GetExStyleStr(str, &tabfont);
			SetDlgItemText(hDlg, IDC_STATIC6, str);
			DeleteDC(hDC);

			hOrgEditFont = hEditFont;
			hOrgTabFont  = hTabFont;
			hEditFont = CreateFontIndirect(&editfont);
			hTabFont  = CreateFontIndirect(&tabfont);
			SendDlgItemMessage(hDlg, IDC_STATIC7, WM_SETFONT, (WPARAM)hEditFont, TRUE);
			SendDlgItemMessage(hDlg, IDC_STATIC8, WM_SETFONT, (WPARAM)hTabFont, TRUE);
			if(hOrgEditFont != NULL) DeleteObject(hOrgEditFont);
			if(hOrgTabFont != NULL)  DeleteObject(hOrgTabFont);

			PopFontInitChooseFont(&editfont, &tabfont);
			// 改造版のフォント強制
			CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CALLBACK SelectSourceProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char *lpFileName;
	switch(message){
		case WM_INITDIALOG:
			lpFileName = (char *)lParam;
			SetDlgItemText(hDlg, IDC_EDIT1, lpFileName);
            return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_BUTTON1:
				{
					char szFileName[_MAX_PATH + 1], szFileTitle[_MAX_PATH + 1] = "";

					GetDlgItemText(hDlg, IDC_EDIT1, szFileName, sizeof(szFileName) - 1);
					if(PopFileOpenDlg(hDlg, szFileName, szFileTitle))
						SetDlgItemText(hDlg, IDC_EDIT1, szFileName);
					return TRUE;
				}
				
				case IDOK:
                    GetDlgItemText(hDlg, IDC_EDIT1, lpFileName, _MAX_PATH);

				case IDCANCEL:
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
			}

	}
	return FALSE;
}

BOOL CALLBACK ConfigKeywordPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch(message){
		case WM_INITDIALOG:
		{
			HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
			const char *szFileName;

			for(size_t i = 0; i < filelist.num(); i++){
				szFileName = filelist.get((int)i);
				if(SendMessage(hListBox, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szFileName) == LB_ERR)
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szFileName);
			}
			
			return TRUE;
		}

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam)){
				case IDC_BUTTON1:
				{
					char szFileName[_MAX_PATH + 1] = "";
					HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
					int nFoundSel;

					if(DialogBoxParam(hInst, "SELSRC", hDlg, SelectSourceProc, (LPARAM)szFileName) == IDOK
						&& szFileName[0] != '\0'){
							nFoundSel = (int)SendMessage(hListBox, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szFileName);
							if(nFoundSel == LB_ERR)
								SendMessage(hListBox, LB_SETCURSEL, SendMessage(hListBox, LB_ADDSTRING, 0,
									(LPARAM)szFileName), 0L);
                            else
                                SendMessage(hListBox, LB_SETCURSEL, nFoundSel, 0L);
					}
					return TRUE;
				}

				case IDC_BUTTON2:
				{
					char szFileName[_MAX_PATH + 1];
					HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
					int nCurSel, nFoundSel;

					nCurSel = (int)SendMessage(hListBox, LB_GETCURSEL, 0, 0L);
					if(nCurSel != LB_ERR){
						SendMessage(hListBox, LB_GETTEXT, nCurSel, (LPARAM)szFileName);
						if(DialogBoxParam(hInst, "SELSRC", hDlg, SelectSourceProc, (LPARAM)szFileName) == IDOK
							&& szFileName[0] != '\0'){
								SendMessage(hListBox, LB_DELETESTRING, nCurSel, 0L);
								nFoundSel = (int)SendMessage(hListBox, LB_FINDSTRINGEXACT, (WPARAM)-1, (LPARAM)szFileName);
								if(nFoundSel == LB_ERR){
									SendMessage(hListBox, LB_INSERTSTRING, nCurSel, (LPARAM)szFileName);
									SendMessage(hListBox, LB_SETCURSEL, nCurSel, 0L); 
								} else
									SendMessage(hListBox, LB_SETCURSEL, nFoundSel, 0L);
						}
					}	
					return TRUE;
				}

				case IDC_BUTTON3:
				{
					HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);

					SendMessage(hListBox, LB_DELETESTRING, SendMessage(hListBox, LB_GETCURSEL, 0, 0L), 0L);
					return TRUE;
				}

				case IDC_LIST1:
					if(HIWORD(wParam) == LBN_DBLCLK){
						PostMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDC_BUTTON2, 0), NULL);
						return TRUE;
					}
					break;
			}
			break;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
		{
			HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
			char tmpfn[_MAX_PATH + 1];

			filelist.reset();
			int num = (int)SendMessage(hListBox, LB_GETCOUNT, 0, 0L);
			for(int i = 0; i < num; i++){
				SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)tmpfn);
                filelist.add(tmpfn);
			}
			//InitClassify();
			//ResetClassify();
			return TRUE;
		}

		case PM_SETDEFAULT:
			SendDlgItemMessage(hDlg, IDC_LIST1, LB_RESETCONTENT, 0, 0L);
			return TRUE;

	}
	return FALSE;
}

BOOL CALLBACK ConfigVisualPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
	switch(message){
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_CHECK1, tabEnabled);
			CheckDlgButton(hDlg, IDC_CHECK2, hsEnabled);
			CheckDlgButton(hDlg, IDC_CHECK3, fsEnabled);
			CheckDlgButton(hDlg, IDC_CHECK4, nlEnabled);
			CheckDlgButton(hDlg, IDC_CHECK5, eofEnabled);
			CheckDlgButton(hDlg, IDC_CHECK6, ulEnabled);

			CheckDlgButton(hDlg, IDC_CHECK7, flg_statbar );
			CheckDlgButton(hDlg, IDC_CHECK8, hscroll_flag );
			CheckDlgButton(hDlg, IDC_CHECK9, flg_toolbar );
			CheckDlgButton(hDlg, IDC_CHECK10, bDrawUnderline );

			SetDlgItemInt(hDlg, IDC_EDIT1, tabsize,     FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT2, rulerheight, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT3, linewidth,   FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, linespace,   FALSE);
			SetDlgItemText(hDlg, IDC_EDIT9, BgImagePath);
			CheckDlgButton(hDlg, IDC_CHECK11, BgImageFlag!=0);
			return TRUE;
		
		case PM_ISAPPLICABLE:
		{
			BOOL bSuccess;
			do{
				GetDlgItemInt(hDlg, IDC_EDIT1, &bSuccess, FALSE);
				if(!bSuccess) break;
				GetDlgItemInt(hDlg, IDC_EDIT2, &bSuccess, FALSE);
				if(!bSuccess) break;
				GetDlgItemInt(hDlg, IDC_EDIT3, &bSuccess, FALSE);
				if(!bSuccess) break;
				GetDlgItemInt(hDlg, IDC_EDIT4, &bSuccess, FALSE);
			} while(0);
			if(!bSuccess)
#ifdef JPNMSG
				MessageBox(hDlg, "表示設定の適用に失敗しました。\nサイズには0以上の整数を入力して下さい。", "Error",
					MB_OK | MB_ICONERROR);
#else
				MessageBox(hDlg, "You must specify integer above 0.", "Error",
					MB_OK | MB_ICONERROR);
#endif
			SetWindowLong(hDlg, DWL_MSGRESULT, bSuccess);
			return TRUE;
		}

		case PM_APPLY: 
			tabEnabled  = IsDlgButtonChecked(hDlg, IDC_CHECK1);
			hsEnabled   = IsDlgButtonChecked(hDlg, IDC_CHECK2);
			fsEnabled   = IsDlgButtonChecked(hDlg, IDC_CHECK3);
			nlEnabled   = IsDlgButtonChecked(hDlg, IDC_CHECK4);
			eofEnabled  = IsDlgButtonChecked(hDlg, IDC_CHECK5);
			ulEnabled   = IsDlgButtonChecked(hDlg, IDC_CHECK6);

			flg_statbar   = IsDlgButtonChecked(hDlg, IDC_CHECK7);
			hscroll_flag  = IsDlgButtonChecked(hDlg, IDC_CHECK8);
			flg_toolbar   = IsDlgButtonChecked(hDlg, IDC_CHECK9);
			bDrawUnderline = (BOOL)IsDlgButtonChecked(hDlg, IDC_CHECK10);

			tabsize     = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
			rulerheight = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
			linewidth   = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
			linespace   = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);
			GetDlgItemText(hDlg, IDC_EDIT9, (LPSTR)&BgImagePath, _MAX_PATH);
			BgImageFlag = IsDlgButtonChecked(hDlg, IDC_CHECK11);

			// これないと、アンダーバーの設定が適用されない
			PopFontApplyEditFont();

			poppad_setalledit();
			poppad_setsb( hscroll_flag );
			UpdateViewOption( flg_toolbar, flg_statbar );
			return TRUE;

		case PM_SETDEFAULT:
			CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK4, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK5, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK6, BST_CHECKED);

			CheckDlgButton(hDlg, IDC_CHECK7, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK8, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK9, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK10, BST_UNCHECKED);

			SetDlgItemInt(hDlg, IDC_EDIT1, 4,  FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT2, 10, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT3, 50, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, 0,  FALSE);
			SetDlgItemText(hDlg, IDC_EDIT9, "");
			return TRUE;
		case WM_COMMAND:
		switch(LOWORD(wParam)){
				case IDC_BUTTON1:
				{
					char szFileName[_MAX_PATH + 1] = "";
					char szFileTitle[_MAX_PATH + 1] = "";
					if(PopFileOpenDlgImg(hDlg, szFileName, szFileTitle))
						SetDlgItemText(hDlg, IDC_EDIT9, szFileName);
					return TRUE;
				}
		}
	}
	return FALSE;
}

void LoadFromCommandLine(char *lpCmdLine)
{
	TABINFO *lpTabInfo;
	int SearchResult, ActivateID;
	bool bActivate = false, bCreated;
	char szOldDir[_MAX_PATH + 1];
	char fullpathbuf[_MAX_PATH];

	int argc;
	char **argv = CommandLineToArgvA(lpCmdLine, &argc);

	for(int i = 1; i < argc; i++){
		char *path;
		GetCurrentDirectory(sizeof(szOldDir), szOldDir);
		SetCurrentDirectory(szExeDir);
		if(GetFullPathName(argv[i], sizeof(fullpathbuf), fullpathbuf, NULL)) {
			path = fullpathbuf;
		} else {
			path = argv[i];
		}
		SearchResult = SearchTab(NULL, NULL, NULL, GetFileIndex(path));
		SetCurrentDirectory(szOldDir);

		if(SearchResult < 0){
			lpTabInfo = GetTabInfo(activeID);

			bCreated = false;
			if(lpTabInfo != NULL ) {
				if(lpTabInfo->NeedSave){ bCreated = true; }
			}
			if( lpTabInfo == NULL
				|| lpTabInfo->FileName[0] != '\0'
				|| Footy2IsEdited(activeFootyID) ){
					bCreated = true;
			}
			if ( bCreated ) {
					CreateTab(activeID, "", path, "");
			} else {
					SetTabInfo(activeID, NULL, path, NULL, -1);
			}
			if(poppad_reload(activeID))
				if(bCreated)
                    DeleteTab(activeID);
				else
					SetTabInfo(activeID, "", "", NULL, -1);
			bActivate = false;
		} else {
			ActivateID = SearchResult;
			bActivate = true;
		}
	}
	if(bActivate && ActivateID >= 0)
		ActivateTab(activeID, ActivateID);

	GlobalFree((HGLOBAL)argv);
	return;
}

LRESULT FileDrop(WPARAM wParam, LPARAM /*lParam*/)
{
	char tmpfn[_MAX_PATH];
	HDROP hDrop = (HDROP)wParam;
	TABINFO *lpTabInfo;
	int SearchResult, ActivateID;
	bool bActivate = false, bCreated;

	int filenum = DragQueryFile( hDrop, 0xFFFFFFFF, tmpfn, _MAX_PATH );
	for(int i = 0; i < filenum; i++){
        DragQueryFile( hDrop, i, tmpfn, _MAX_PATH );
		SearchResult = SearchTab(NULL, NULL, NULL, GetFileIndex(tmpfn));
		if(SearchResult < 0){
			lpTabInfo = GetTabInfo(activeID);

			bCreated = false;
			if(lpTabInfo != NULL ) {
				if(lpTabInfo->NeedSave){ bCreated = true; }
			}
			if( lpTabInfo == NULL
				|| lpTabInfo->FileName[0] != '\0'
				|| Footy2IsEdited(activeFootyID) ){
					bCreated = true;
			}
			if ( bCreated ) {
					CreateTab(activeID, "", tmpfn, "");
			} else {
					SetTabInfo(activeID, NULL, tmpfn, NULL, -1);
			}
			if(poppad_reload(activeID))
				if(bCreated)
                    DeleteTab(activeID);
				else
					SetTabInfo(activeID, "", "", NULL, -1);

			bActivate = false;
		} else {
			ActivateID = SearchResult;
			bActivate = true;
		}
	}
	if(bActivate && ActivateID >= 0)
		ActivateTab(activeID, ActivateID);
    DragFinish( hDrop );
	return 0;						// breakだとWin9xで止まる
}

void __stdcall OnFooty2TextModified(int id, void * /*pParam*/, int /*nCause*/)
{
	int nTabID = GetTabID(id);

	TABINFO *lpTabInfo = GetTabInfo(nTabID);

	if(lpTabInfo == NULL)
		return;

	// 2008-02-17 Shark++ 要動作確認
	bNeedSave = (BOOL)Footy2IsEdited(id);
	if( lpTabInfo->NeedSave != bNeedSave ) {
		SetTabInfo(nTabID, NULL, NULL, NULL, bNeedSave);
		DoCaption(szTitleName, nTabID);
	}

	PutLineNumber();
	return;
}

void GetBackupPath(char *backuppath, int size){
	// EXEのあるディレクトリ
	char exepath[MAX_PATH+1];
	GetModuleFileNameA(0, exepath, MAX_PATH);
	PathRemoveFileSpec(exepath);

	// バックアップフォルダ
	strcpy_s(backuppath, size, exepath);
	strcat_s(backuppath, size,"\\backup\\");
}

void BackupDelete(const char *backuppath){
	int i;
	// 無限ループは怖いので縛っておく
	for (i = 0; i < 4096; i++){
		char txtpath[MAX_PATH*2];
		char hsppath[MAX_PATH*2];
		char number[256];
		sprintf_s(number, 256, "%d", i);
		strcpy_s(txtpath, MAX_PATH*2, backuppath);
		strcpy_s(hsppath, MAX_PATH*2, backuppath);
		strcat_s(txtpath, MAX_PATH*2, number);
		strcat_s(txtpath, MAX_PATH*2, ".txt");
		strcat_s(hsppath, MAX_PATH*2, number);
		strcat_s(hsppath, MAX_PATH*2, ".hsp");
		if (!PathFileExists(txtpath))
			break;

		DeleteFile(txtpath);
		DeleteFile(hsppath);
	}
}

void HSPBackupSave(const char *backuppath, const char *path, int nFootyID){
	if (!PathIsDirectoryA(backuppath)){
		CreateDirectory(backuppath, NULL);
		if (!PathIsDirectoryA(backuppath))
			return;
	}
	
	int size = Footy2GetTextLengthA(nFootyID, LM_CRLF);
	char *bufhsp = (char *)calloc(size + 2,sizeof(char));
	Footy2GetTextA(nFootyID, bufhsp, LM_CRLF, size + 1);

	HANDLE fp;
	fp = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if (fp == NULL)
		return;
	DWORD NumberOfBytesWritten = 0;
	WriteFile(fp, bufhsp, size, &NumberOfBytesWritten, NULL);
	FlushFileBuffers(fp);
	CloseHandle(fp);

	free(bufhsp);
}

void TXTBackupSave(const char *backuppath, const char *path, char *data){
	if (!PathIsDirectoryA(backuppath)){
		CreateDirectory(backuppath, NULL);
		if (!PathIsDirectoryA(backuppath))
			return;
	}
	HANDLE fp;
	fp = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
	if (fp == NULL)
		return;
	DWORD NumberOfBytesWritten = 0;
	WriteFile(fp, data, strlen(data), &NumberOfBytesWritten, NULL);
	FlushFileBuffers(fp);
	CloseHandle(fp);
}

// 自動バックアップ by inovia
void AutoBackUp(void){
	TABINFO *lpTabInfo;
	int n = TabCtrl_GetItemCount(hwndTab), i, nFootyID;

	// 未保存の場合はファイルパスと内容を記録(1度も保存しておらずパスがない場合は無題というか空欄にしておく)
	// 保存済みの場合は内容は保存せず、ファイルパスのみ記録
	// 0 番から始めるので、 0.txt がパスを格納、 0.hsp がファイルの内容を格納するファイルとする
	// フォルダ名はbackupに保存する

	char backuppath[MAX_PATH*2] = {0};
	GetBackupPath(backuppath, MAX_PATH*2);
	BackupDelete(backuppath);

	// タブの数だけループする
	for (i = 0; i < n; i++){
		lpTabInfo = GetTabInfo(i);

		nFootyID = lpTabInfo->FootyID;

		char txtpath[MAX_PATH*2] = {0};
		char hsppath[MAX_PATH*2] = {0};
		char number[256] = {0};
		sprintf_s(number, 256, "%d", i);
		strcpy_s(txtpath, MAX_PATH*2, backuppath);
		strcpy_s(hsppath, MAX_PATH*2, backuppath);
		strcat_s(txtpath, MAX_PATH*2, number);
		strcat_s(txtpath, MAX_PATH*2, ".txt");
		strcat_s(hsppath, MAX_PATH*2, number);
		strcat_s(hsppath, MAX_PATH*2, ".hsp");
		
		// 未保存の場合はファイルパスと内容を記録
		if (lpTabInfo->NeedSave == TRUE){
			TXTBackupSave(backuppath, txtpath, lpTabInfo->FileName);
			HSPBackupSave(backuppath, hsppath, nFootyID);
		}else{
			// 保存済みの場合はファイルパスのみ記録
			TXTBackupSave(backuppath, txtpath, lpTabInfo->FileName);
		}
		//MessageBoxA(0,"ループ内","",0);
		//
	}

	if (NotifyBackup == TRUE){
		char msg[256] = {0};
		SYSTEMTIME st;
		GetLocalTime(&st);
#ifdef JPNMSG
		sprintf_s(msg, 256, "%04d/%02d/%02d %02d:%02d:%02d に自動バックアップしました",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
#else
		sprintf_s(msg, 256, "Auto backup on %04d/%02d/%02d %02d:%02d:%02d.", st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
#endif
		Statusbar_mes(msg);
	}
	
}

// バックアップから復元する
void ReadBackup(void){
	// バックアップを削除せずコピーする場合
	if (BackupNoDelete == TRUE){
		BackupCopy();
	}
	
	char backuppath[MAX_PATH*2];
	GetBackupPath(backuppath, MAX_PATH*2);
	int i;
	// 無限ループは怖いので縛っておく
	for (i = 0; i < 4096; i++){
		char txtpath[MAX_PATH*2];
		char hsppath[MAX_PATH*2];
		char number[256];
		sprintf_s(number, 256, "%d", i);
		strcpy_s(txtpath, MAX_PATH*2,  backuppath);
		strcpy_s(hsppath, MAX_PATH*2, backuppath);
		strcat_s(txtpath, MAX_PATH*2, number);
		strcat_s(txtpath, MAX_PATH*2, ".txt");
		strcat_s(hsppath, MAX_PATH*2, number);
		strcat_s(hsppath, MAX_PATH*2, ".hsp");
		if (!PathFileExists(txtpath))
			break;
		

		// .txtを開く処理
		TABINFO *lpTabInfo;
		char *buf = NULL, *buf2 = NULL;
		if (ReadFileBuffer(txtpath, &buf) == -1){
			continue;
		}
		
		if (strcmp(buf, "") == 0){
			// 1度も保存していない
			CreateTab(activeID, "", "", "");
			lpTabInfo = GetTabInfo(activeID);
			if (ReadFileBuffer(hsppath, &buf2) == -1){
				CloseFileBuffer(buf);
				//MessageBoxA(0,"直読みから読み込み失敗！","",0);
				continue;
			}
			Footy2SetTextA(lpTabInfo->FootyID, buf2);
			lpTabInfo->NeedSave = TRUE;
			SetClassify(lpTabInfo->FootyID);
			DoCaption ("", activeID) ;
			CloseFileBuffer(buf);
			CloseFileBuffer(buf2);
			//MessageBoxA(0,"直読みから読み込み成功！","",0);
		}else if (!PathFileExists(buf)){
			// 存在しないパスの場合はスルー
			CloseFileBuffer(buf);
			//MessageBoxA(0,"そんなの存在しない","",0);
			continue;
		}else{
			// ファイルが存在する場合
			char TitleName[512], DirName[512];
			// コポー
			strcpy_s(DirName, 512, buf);
			// ディレクトリのみにする
			PathRemoveFileSpec(DirName);
			// コポー
			strcpy_s(TitleName, 512, buf);
			// ファイル名のみにする
			PathStripPath(TitleName);
			CreateTab(activeID, TitleName, buf, DirName);
			lpTabInfo = GetTabInfo(activeID);
			if (!PathFileExists(hsppath)){
				if (!PopFileRead(lpTabInfo->FootyID, buf)){
					//MessageBoxA(0,"PopFileReadから読み込み失敗","",0);
					CloseFileBuffer(buf);
					continue;
				}
				//MessageBoxA(0,"PopFileReadから読み込み成功","",0);
			}else{
				// 見つからない場合
				if (ReadFileBuffer(hsppath, &buf2) == -1){
					CloseFileBuffer(buf);
					//MessageBoxA(0,"直読みから読み込み失敗したｗｗｗ！","",0);
					continue;
				}
				//MessageBoxA(0,buf2,buf,0);
				Footy2SetTextA(lpTabInfo->FootyID, buf2);
				lpTabInfo->NeedSave = TRUE;
				CloseFileBuffer(buf2);
				//MessageBoxA(0,"直読みから読み込み成功","",0);
			}
			DoCaption (TitleName, activeID) ;
			SetClassify(lpTabInfo->FootyID);
			CloseFileBuffer(buf);
		}

		ULONGLONG ullFileIndex;
		ullFileIndex = GetFileIndex(szFileName);
		lpTabInfo->FileIndex = ullFileIndex;

	}
}

void BackupCopy(void){
	char backuppath[MAX_PATH*2];
	char backupcopyfolder[MAX_PATH*2];
	GetBackupPath(backuppath, MAX_PATH*2);
	strcpy_s(backupcopyfolder, MAX_PATH*2, backuppath);
	char t[256] = {0};
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(t, 256, "%04d%02d%02d %02d%02d%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	strcat_s(backupcopyfolder, MAX_PATH*2, t);
	strcat_s(backupcopyfolder, MAX_PATH*2, "\\");
	//MessageBoxA(hWndMain,backupcopyfolder,"",0);
	if (!CreateDirectory(backupcopyfolder, NULL)){
#ifdef JPNMSG
		MessageBoxA(hWndMain,"フォルダの作成に失敗しました。バックアップのコピーは作成されませんでした。\nファイル保全のため強制終了します。再度このメッセージが出る場合は、backupフォルダを削除するか移動してください。","",MB_OK | MB_ICONERROR);
#else
		MessageBoxA(hWndMain, "Failed to make folders for backup.", "", MB_OK | MB_ICONERROR);
#endif
		ExitProcess( 0 );
		return;
	}

	int i;
	// 無限ループは怖いので縛っておく
	for (i = 0; i < 4096; i++){
		char txtpath[MAX_PATH*2];
		char txtpath_n[MAX_PATH*2];
		char hsppath[MAX_PATH*2];
		char hsppath_n[MAX_PATH*2];
		char number[256];
		sprintf_s(number, 256, "%d", i);
		strcpy_s(txtpath, MAX_PATH*2, backuppath);
		strcpy_s(txtpath_n, MAX_PATH*2, backupcopyfolder);
		strcpy_s(hsppath, MAX_PATH*2, backuppath);
		strcpy_s(hsppath_n, MAX_PATH*2, backupcopyfolder);
		strcat_s(txtpath, MAX_PATH*2, number);
		strcat_s(txtpath, MAX_PATH*2, ".txt");
		strcat_s(txtpath_n, MAX_PATH*2, number);
		strcat_s(txtpath_n, MAX_PATH*2, ".txt");
		strcat_s(hsppath, MAX_PATH*2, number);
		strcat_s(hsppath, MAX_PATH*2, ".hsp");
		strcat_s(hsppath_n, MAX_PATH*2, number);
		strcat_s(hsppath_n, MAX_PATH*2, ".hsp");

		if (!PathFileExists(txtpath))
			break;
		if (PathFileExists(txtpath))
			CopyFile(txtpath, txtpath_n, FALSE);
		if (PathFileExists(hsppath))
			CopyFile(hsppath, hsppath_n, FALSE);

	}
}

int ReadFileBuffer(const char *filepath, char **buf){

	HANDLE fp;
	int fsize;
	fp = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == NULL)
		return -1;
	// ファイルサイズ
	fsize = GetFileSize(fp, NULL);
	*buf = (char *)calloc(fsize*2+2, sizeof(char));
	//char *tmp = (char *)calloc(fsize*2+2, sizeof(char));
	if (fsize > 0){
		DWORD NumberOfBytesRead = 0;
		ReadFile(fp, *buf, fsize, &NumberOfBytesRead, NULL);
	}
	//free(tmp);
	CloseHandle(fp);
	return fsize;
}

void CloseFileBuffer(char *buf){
	if (buf != NULL){
		free(buf);
		buf = NULL;
	}

}

int ExistBackupFile(char *backuppath){
	int res = 0;
	char txtpath[MAX_PATH*2];
	char number[256];
	sprintf_s(number, 256, "%d", 0);
	strcpy_s(txtpath, MAX_PATH*2, backuppath);;
	strcat_s(txtpath, MAX_PATH*2, number);
	strcat_s(txtpath, MAX_PATH*2, ".txt");
	if (!PathFileExists(txtpath)){
		return -1;
	}
	return res;
}