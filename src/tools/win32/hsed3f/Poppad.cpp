
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
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <direct.h>
#include <winuser.h>
#include <shlobj.h>
#include "FootyDLL.h"
#include <htmlhelp.h>

#include "poppad.h"
#include "resource.h"

#include "tabmanager.h"
#include "support.h"
#include "config.h"
#include "classify.h"
#include "exttool.h"


/*
		XP support routines
*/
extern int  flag_xpstyle;
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

static	int dllflg=0;			// DLL uses flag
static	HINSTANCE hDLL;			// Handle to DLL

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
	if (ent==NULL) dllflg=-1;				// error flag
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
	BrowsingInfo.lpszTitle      = "フォルダを選択してください";
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
BOOL PopFileSaveDlg    (HWND, PSTR, PSTR) ;
BOOL PopFileRead       (int, PSTR) ;
BOOL PopFileWrite      (int,  PSTR) ;

          // Functions in POPFIND.C

HWND PopFindFindDlg     (HWND, int) ;
HWND PopFindReplaceDlg  (HWND) ;
BOOL PopFindFindText    (HWND, int, LPFINDREPLACE) ;
BOOL PopFindReplaceText (HWND, int, LPFINDREPLACE) ;
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

          // Global variables

static char szAppName[] = "OniPad" ;

HWND hDlgModeless ;
static int cln;
HWND hwbak ;
extern HMENU hSubMenu ;

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
	//MessageBox (hwnd, errbuf, "HSP error message", MB_OK | MB_ICONEXCLAMATION) ;
	DialogBox (hInst, "ErrBox", hwnd, (DLGPROC)ErrDlgProc);
}

static void OkMessage ( char *szMessage, char *szTitleName)
{
	msgboxf(hwbak, szMessage, "script editor message", MB_OK | MB_ICONEXCLAMATION,
		szTitleName[0] ? szTitleName : UNTITLED);
}

static void OkMessage2 ( char *szMessage, char *szTitleName)
{
	msgboxf(NULL, szMessage, "script editor message", MB_OK | MB_ICONEXCLAMATION,
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


static int fileexe( char *appname, char *fname )
{
	//		Execute application
	//
	if ( filechk(fname) ) return 1;
	strcpy( execmd,appname );
	strcat( execmd," " );
	strcat( execmd,fname );
	WinExec( execmd,SW_SHOW );
	return 0;
}


static int GetFileTitle2( char *bname, char *tname )
{
	//		GetFileTitleの替わり
	//		(ファイル拡張子表示ON/OFFの影響を受けない)
	//
	int a,b,len;
	unsigned char a1;
	b=-1;
	len=strlen(bname);
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


static void packgo( void )
{
	int a;
	if (filechk("packfile")) {
#ifdef JPNMSG
		TMes( "DPMファイルを作るためにはパックするファイル名一覧(PACKFILE)を\n作成しておく必要があります。" );
#else
		TMes( "[PACKFILE] needed." );
#endif
		return;
	}

	pack_ini( 0,(int)"data",0,0 );	
	a=pack_make( 1,0,0,0 );
	//dpmc_ini(errbuf,"data");
	//a=dpmc_pack();
	if (a) { err_prt(hwbak);return; }
#ifdef JPNMSG
	TMes("[DATA.DPM]ファイルを作成しました。");
#else
	TMes("Create [DATA.DPM] Successfully.");
#endif
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
		wsprintf(hh,"%s\\hsprt",szExeDir);
	} else {
		wsprintf(hh,"%s\\runtime\\hspcl.hrt",szExeDir);
	}

	pack_rt( 0,(int)hh,0,0 );
	pack_opt( hsp_wx,hsp_wy,(hsp_wd)|(hsp_orgpath<<1),0 );
	a=pack_exe( mode,0,0,0 );
	//a=dpmc_mkexe( hsp_fullscr,hh,hsp_wx,hsp_wy,hsp_wd );
	if (a) { err_prt(hwbak);return; }
	TMes(finmes);
	DeleteFile(ftmp);
}
	
	
static void mkexe( char *exname )
{
	if (filechk("packfile")) {
#ifdef JPNMSG
		TMes( "EXEファイルを作るためにはパックするファイル名一覧(PACKFILE)を\n作成しておく必要があります。" );
#else
		TMes( "[PACKFILE] needed." );
#endif
		return;
	}
#ifdef JPNMSG
	expack( hsp_fullscr, exname, "EXEファイルを作成しました。" );
#else
	expack( hsp_fullscr, exname, "Create EXE file successfully." );
#endif
}


static void mkscr( char *exname )
{
	if (hsp_clmode) {
#ifdef JPNMSG
		TMes( "コンソールモードでスクリーンセーバーの作成はできません。" );
#else
		TMes( "No operation for console mode." );
#endif
		return;
	}
	if (filechk("packfile")) {
#ifdef JPNMSG
		TMes( "スクリーンセーバーを作るためにはパックするファイル名一覧(PACKFILE)を\n作成しておく必要があります。" );
#else
		TMes( "[PACKFILE] needed." );
#endif
		return;
	}
#ifdef JPNMSG
	expack( 2, exname, "SCRファイルを作成しました。" );
#else
	expack( 2, exname, "Create SCR file successfully." );
#endif
}


static void chklstr( char *laststr )
{
	//		hsp_laststr modify
	//
	int a;
	char a1;
	a=0;
	while(1) {
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
		wsprintf( execmd,"\"%s\\%s\" ",szExeDir, DEFAULT_RUNTIME );
	} else {
		wsprintf( execmd,"\"%s\\%s\" ",szExeDir, cfname );
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
/*
	MessageBox(0, execmd, 0, 0);
	ShellExecute(NULL, "open", execmd, hsp_cmdopt, 
	*/
	}
}

static void hsprun_log( char *objname )
{
	//		execute HSP2 process (with LOG)
	//
	char dbopt[64];
	wsprintf( execmd,"\"%s\\%s\" ",szExeDir,DEFAULT_RUNTIME );
	wsprintf( dbopt, "%03d",hsp_logmode );
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

	wsprintf( execmd,"%s\\hspcli.bat ",ftmp );
	//if (hsp_fullscr) strcat( execmd,"*" );
	//if (hsp_debug) strcat( execmd,"@" );

	//strcat( execmd,szDirName );
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

	wsprintf( execmd,"%s\\hspcli.bat ",ftmp );
	wsprintf( dbopt, "%03d",hsp_logmode );
	strcat( execmd, "%" );
	strcat( execmd, dbopt );
	if (hsp_logadd==0) DeleteFile( "hsplog.txt" );

	//if (hsp_fullscr) strcat( execmd,"*" );
	//if (hsp_debug) strcat( execmd,"@" );
	//strcat( execmd,szDirName );
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
	a=strlen(fname)-1;
	while(1) {
		a1=fname[a];
		if (a1==0x5c) { a++;break; }
		a--;if (a==0) break;
	}
	strcpy(srcfn,&(fname[a]));
	strcpy(tmpst,srcfn);
	a=0;while(1) {
		a1=tmpst[a];
		if ((a1==0)||(a1=='.')) break;
		a++;
	}
	tmpst[a]=0;strcat(tmpst,".ax");

	hsc_ini( 0,(int)srcfn, 0,0 );
	hsc_refname( 0,(int)myfile(), 0,0 );
	hsc_objname( 0,(int)tmpst, 0,0 );
	a=hsc_comp( 0,0,0,0 );
	//a=tcomp_main( myfile(), srcfn, tmpst, errbuf, 0 );
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
	//a=tcomp_main( myfile(), srcfn, tmpst, errbuf, 0 );
	return a;
}


static int mkexefile2( char *fname )
{
	//	auto make exe file (ver2.6)
	//
	int a;
	char tmpst[_MAX_PATH];
	char srcfn[_MAX_PATH];
	char ftmp[_MAX_PATH];
	strcpy(srcfn,fname);
	strcpy(tmpst,"start.ax");

	hsc_ini( 0,(int)srcfn, 0,0 );
	hsc_refname( 0,(int)myfile(), 0,0 );
	hsc_objname( 0,(int)tmpst, 0,0 );
	a=hsc_comp( 0,4,0,0 );
	if ( a ) return a;

	sprintf( ftmp, "%s\\%s.dpm", szExeDir, srcfn );
	a=hsc3_make( 0,(int)ftmp,0,0 );
	if ( a ) return a;
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
		wsprintf( hdir,"%s\\hsphelp\\",szExeDir );
		return;
	}
	b=0;ls=ss;
	while(1) {
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
	int buflen,ofs,org,a,b;
	char *strbuf;
	char a1;

//	SendMessage ( hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs );
	ofs = FootyGetCaretThrough(activeFootyID) - 1;
//	buflen = GetWindowTextLength (hwndEdit);
	buflen = FootyGetTextLength(activeFootyID);
	strbuf=(char *)malloc(buflen+16);
//	GetWindowText (hwndEdit, strbuf, buflen + 1) ;
	FootyGetText (activeFootyID, strbuf, RETLINE_CRLF);
	org = ofs;
//	if ( flag_xpstyle ) ofs = getUnicodeOffset( strbuf, ofs );
	a=ofs;b=0;
	while(1) {
		if (a==0) break;
		a1=strbuf[a-1];
		if ((a1>=0x30)&&(a1<=0x39)) { a--; continue; }
		if (hsp_helpmode!=0) {
			if ( a1 == '#' ) { a--; break; }	// #を含める
		}
		if (a1<65) break;
		a--;
	}
	while(1) {
		a1=strbuf[a++];
		if ((a1>=0x30)&&(a1<=0x39)) {
			kwstr[b++]=a1;
		}
		else if (a1=='#') {
			kwstr[b++]=a1;
		}
		else if (a1>=65) {
			kwstr[b++]=a1;
		}
		else {
			break;
		}
	}
	kwstr[b++]=0;
	//MessageBox( NULL,kwstr,"WOW",0 );
	free(strbuf);
}

static void callhelp( void )
{
	//		HELP呼び出し
	//
	int a;
	char mesb[512];

	if (hsp_helpmode==0) {
		a=strlen(kwstr);

		// 最初の6文字キー検索を廃止
		//if (a>6) kwstr[6]=0;							// 始めの6文字

		wsprintf( helpopt,"%ss_%s.htm",hdir,kwstr );
		if (fileok(helpopt)) {
			wsprintf(helpopt,"%shsppidx.htm",hdir);
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
		wsprintf(helpopt,"%shsp.hlp",hdir);
		if (fileok(helpopt)) {
#ifdef JPNMSG
			TMes("HSP.HLPファイルが見つかりません。\nWinHelp形式のファイルを確認してください。");
#else
			TMes("Help data missing.Check help preference.");
#endif
			return;
		}
		WinHelp( hwndEdit, helpopt, HELP_KEY, (DWORD)kwstr );
		return;
	}

	if (hsp_helpmode==3) {

		//		"S_" + keyword先頭６文字 +".htm" のファイルを開く。
		wsprintf( helpopt,"%shsp.chm",hdir );
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

	wsprintf( helpopt,"%shelpman.exe",hdir );
	if (fileok(helpopt)) {
#ifdef JPNMSG
		wsprintf( mesb, "HSPヘルプマネージャが見つかりません。\n%sを確認してください。",helpopt );
#else
		wsprintf( mesb, "HSP help manager not found.\nCheck %s.",helpopt );
#endif
		TMes( mesb );
		return;
	}
	wsprintf( helpopt,"\"%shelpman.exe\" %s",hdir,kwstr );
	WinExec( helpopt, SW_SHOW );
	return;
}


void pophwnd( HWND hwnd )
{
	hwbak=hwnd;
}

void DoCaption ( char *szTitleName, int TabID )
     {
     char szCaption[_MAX_PATH+128] ;

	 if(GetTabInfo(0) == NULL){
#ifdef JPNMSG
	     lstrcpy (szCaption, "ＨＳＰスクリプトエディタ") ;
#else
	     lstrcpy (szCaption, "HSP Script Editor") ;
#endif
	 } else {
#ifdef JPNMSG
	     wsprintf (szCaption, "ＨＳＰスクリプトエディタ - %s%s",
			 szTitleName[0] ? szTitleName : UNTITLED, bNeedSave ? " *": "") ;
#else
	     wsprintf (szCaption, "HSP Script Editor - %s%s",
			 szTitleName[0] ? szTitleName : UNTITLED, bNeedSave ? " *": "") ;
#endif
	 }
	 SetWindowText (hwbak, szCaption) ;

	 if(TabID >= 0){
		 char szTabCaption[_MAX_PATH+128] ;
		 TCITEM tc_item;
		 TABINFO *lpTabInfo;

		 lpTabInfo = GetTabInfo(TabID);
	     wsprintf (szTabCaption, "%s%s",
			 lpTabInfo->TitleName[0] ? lpTabInfo->TitleName : TABUNTITLED, lpTabInfo->NeedSave ? " *" : "") ;

		 tc_item.mask = TCIF_TEXT;
		 tc_item.pszText = szTabCaption;

		 TabCtrl_SetItem(hwndTab, TabID, &tc_item);
     }
	 }

short AskAboutSave (HWND hwnd, char *szTitleName)
     {
     char szBuffer[64 + _MAX_FNAME + _MAX_EXT] ;
     int  iReturn ;

#ifdef JPNMSG
     wsprintf (szBuffer, "%sは変更されています。セーブしますか？",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#else
     wsprintf (szBuffer, "%s has been modified. Save?",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#endif

     iReturn = MessageBox (hwnd, szBuffer, "Warning",
                           MB_YESNOCANCEL | MB_ICONQUESTION) ;

     if (iReturn == IDYES)
          if (!SendMessage (hwnd, WM_COMMAND, IDM_SAVE, 0L))
               iReturn = IDCANCEL ;

     return iReturn ;
     }


BOOL CALLBACK JumpDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
	 char s1[10];

     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
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


BOOL CALLBACK OptDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_cmdopt );
				return TRUE ;

          case WM_COMMAND:
               switch (wParam)
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


BOOL CALLBACK FnameDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
               switch (wParam)
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


BOOL CALLBACK ExtcmpDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_extstr );
				CheckDlgButton( hDlg, IDC_CHECK1, hsp_extobj );
				return TRUE ;

          case WM_COMMAND:
               switch (wParam)
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

BOOL CALLBACK ErrDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
				hsc_getmes( (int)errbuf,0,0,0 );
				SendMessage( GetDlgItem( hDlg, IDC_EDIT1 ), EM_LIMITTEXT, 0, 0L);
				SetDlgItemText( hDlg,IDC_EDIT1,errbuf );
				return TRUE ;

          case WM_COMMAND:
               switch (wParam)
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

/*
	Fixed
	
	2006/09/06 コメントが存在するときとEOF付近のときのラベルが無視される不具合を修正(LonelyWolf)
*/

static bool IsSJisLeadByte( BYTE a1 )
{
	if (a1>=129) {					// 全角文字チェック
		if ((a1<=159)||(a1>=224)) return 1;
	}
	return 0;
}


static void set_labellist( HWND hDlg, HWND hwndEdit )
{
	char st[128];
	char lname[256];
	char *buffer;
	char *wp;
	char a1;
	int len;
	int line;
	int tag;
	int mytag = -1;
	int myline;
	SendDlgItemMessage( hDlg,IDC_LIST2,LB_RESETCONTENT, 0, 0L );

	len = FootyGetTextLength(activeFootyID);
	if(len >= 0){
		buffer = (char *) calloc( len + 2 /* 文字列終端と番兵 */, sizeof(char) );
		FootyGetText(activeFootyID, buffer, RETLINE_CRLF);
		FootyGetSelA(activeFootyID, &myline, NULL, NULL, NULL);

		line = 1;
		wp = buffer;
		tag	= -1;
		while(1) {
			// 読み込み
			a1 = *wp;
			if ( a1 == 0 ) break;
			if ( a1 == 13 ) {
				if ( line<=myline) mytag=tag;
				line++;
				wp++;
				if ( *wp==10 ) wp++;
				continue;
			}
			if ( IsSJisLeadByte(a1) ) {
				wp+=2; continue;
			}
			// 先頭の空白を無視
			if( a1 == ' ' || a1 == '\t' ) {
				wp++;
				continue;
			}
			// プリプロセッサ読み飛ばし
			if( a1 == '#' ) {
				// 行末まで読み飛ばし
				for(wp++; *wp && 0x0d != *wp ; wp++);
				continue;
			}
			// ラベル読み込み
			if( a1 == '*' ) {
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
					wsprintf(st, "%5d : %s",line, lname);
					SendDlgItemMessage( hDlg,IDC_LIST2,LB_ADDSTRING,-1,(LPARAM)st );
					tag++;
				}
				wp = pa;
				continue;
			}
			// コメント読み飛ばし
			if( ';' == a1 || ('/' == a1 && '/' == *(wp + 1)) ) {
				// 行末まで読み飛ばし
				for(wp++; *wp && 0x0d != *wp ; wp++);
				continue;
			}
			if( '/' == *wp && '*' == *(wp + 1) ) {
				for(wp++; *wp ; wp++) {
					if( '*' == *wp && '/' == *(wp + 1) ) {
						wp+=2;
						break;
					}
					if( 0x0d == *wp ) {
						line++;
					}
				}
				continue;
			}
			// 次のステートメントまで読み飛ばし
			while(1) {
				a1 = *wp;
				if (( a1 == 0 )||( a1 == 13 )) break;
				if ( a1 == ':' ) { wp++; break; }
				if ( a1=='/' ) {
					if ( *(wp+1) == '*' ) break;
					if ( *(wp+1) == '/' ) break;
				}
				if( a1 == '{' ) {
					if( '\"' == *(wp + 1) ) { // 複数行文字列
						wp+=2;
						while(1) {
							a1 = *wp;
							if ( a1 == 0 ) break;
							if ( a1 == 13 ) line++;
							if ( a1 == '\"' ) {
								if ( *(wp+1) == '}' ) { wp+=2; break; }
							}
							if ( a1 == '\\' ) wp++;
							if (IsSJisLeadByte(a1)) wp++;
							wp++;
						}
						continue;
					}
				}
				if( a1 == '\"' ) {
					// 文字列読み飛ばし
					wp++;
					while(1) {
						a1 = *wp;
						if (( a1 == 0 )||( a1 == 13 )) break;
						if ( a1 == '\"' ) { wp++; break; }
						if ( a1 == '\\' ) wp++;
						if (IsSJisLeadByte(a1)) wp++;
						wp++;
					}
					continue;
				}
				if (IsSJisLeadByte(*wp)) wp++;
				wp++;
			}
		}

		if ( mytag>=0 ) {
			SendDlgItemMessage( hDlg, IDC_LIST2, LB_SETCURSEL, mytag, 0L );
		}
        free (buffer) ;
	}
}


BOOL CALLBACK LabelDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	 int i;
	 char s1[128];
     switch (message)
          {
          case WM_INITDIALOG:
               set_labellist( hDlg, hwndEdit );
               SetFocus( GetDlgItem( hDlg,IDC_LIST2 ) );
               return TRUE;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
						i=(int)SendDlgItemMessage( hDlg,IDC_LIST2, LB_GETCURSEL,0,0L );
						if ( i != LB_ERR ) {
							SendDlgItemMessage( hDlg,IDC_LIST2, LB_GETTEXT,i,(LPARAM)s1 );
							cln=atoi(s1)-1;
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
               break ;
          }
     return FALSE ;
}

void poppad_setedit(int FootyID)
{
	FootySetMarkVisible(FootyID, F_SC_TAB, tabEnabled, false);
	FootySetMarkVisible(FootyID, F_SC_HALFSPACE, hsEnabled, false);
	FootySetMarkVisible(FootyID, F_SC_NORMALSPACE, fsEnabled, false);
	FootySetMarkVisible(FootyID, F_SC_CRLF, nlEnabled, false);
	FootySetMarkVisible(FootyID, F_SC_EOF, eofEnabled, false);
	FootySetMarkVisible(FootyID, F_SC_UNDERLINE, ulEnabled, false);
	FootySetMetrics(FootyID, F_SM_TAB, tabsize, false);
	FootySetMetrics(FootyID, F_SM_RULER, rulerheight, false);
	FootySetMetrics(FootyID, F_SM_LINENUM, linewidth, false);
	FootySetMetrics(FootyID, F_SM_LINESPACE, linespace, false);
	//FootyRefresh(FootyID);
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
	HWND hWnd = FootyGetWnd(FootyID);
	ShowScrollBar(hWnd, SB_HORZ, chk);
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

               // Create the edit control child window

               //hwndEdit = CreateWindowEx ( WS_EX_ACCEPTFILES, "EDIT", NULL,
               //          WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
               //               ES_LEFT | ES_MULTILINE |
               //               ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
               //          0, 0, 0, 0,
               //          hwnd, (HMENU) EDITID, hInst, NULL) ;
               hwndTab = CreateWindowEx(0, WC_TABCONTROL, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CLIPCHILDREN,
                   0, 0, 10, 10, hwnd, (HMENU)0x10, hInst, NULL);
			   PopFontApplyTabFont();
			   Org_TabProc = (WNDPROC)GetWindowLong(hwndTab, GWL_WNDPROC);
			   SetWindowLong(hwndTab, GWL_WNDPROC, (LONG)MyTabProc);
			   DragAcceptFiles(hwndTab, TRUE);

//  Moved to CreateTab function in tabmanager.cpp
//			   Org_EditProc = (WNDPROC)GetWindowLong( hwndEdit, GWL_WNDPROC );
//			   SetWindowLong( hwndEdit, GWL_WNDPROC, (LONG)MyEditProc );
//			   DragAcceptFiles( hwndEdit, TRUE );

               // Initialize common dialog box stuff

               iMsgFindReplace = RegisterWindowMessage (FINDMSGSTRING) ;
			   iMsgHelp        = RegisterWindowMessage (HELPMSGSTRING) ;

               // Process command line

               //lstrcpy (szFileName, (PSTR) (((LPCREATESTRUCT) lParam)->lpCreateParams)) ;

/*
				if ( szCmdline[0]==0x22 ) {
					strcpy( szFileName, szCmdline+1 );
			   }
			   else {
				   strcpy( szFileName, szCmdline );
			   }
*/

			   PopFontSetELG( chg_font );
			   PopFontSetEditFont();

			   PopFontSetTLG( tchg_font );
			   PopFontSetTabFont();

			   CreateTab(0, "", "", "");

			   DoCaption (szTitleName, activeID) ;
			   err_ini();

               return 0 ;
}


void poppad_bye( void )
{
    //case WM_DESTROY :
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


static void SetFileName(char *titleName, char *fileName, char *dirName) {
	if ( titleName != NULL ) lstrcpy(szTitleName, titleName);
	if ( fileName != NULL ) lstrcpy(szFileName, fileName);
	if ( dirName != NULL ) lstrcpy(szDirName, fileName);
	SetTabInfo(activeID, titleName, fileName, dirName, -1);
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
		char *szSpaceBuf;
		const char *szLine;
		int nsLine, nsPos, neLine, nePos, nLength, i, j;

		if(bAutoIndent && wParam == VK_RETURN && GetKeyState(VK_CONTROL) >= 0){
			FootyGetSelA(activeFootyID, &nsLine, &nsPos, &neLine, &nePos);

			szLine = FootyGetLineData(activeFootyID, neLine);
			nLength = FootyGetLineLen(activeFootyID, neLine);
			for(i = nePos - 1; i < nLength && (szLine[i] == ' ' || szLine[i] == '\t'); i++)
				nePos++;

            szLine = FootyGetLineData(activeFootyID, nsLine);
			nLength = min(FootyGetLineLen(activeFootyID, nsLine), nsPos);

			szSpaceBuf = (char *)malloc(nLength + 3);
			lstrcpy(szSpaceBuf, "\r\n");

			for(i = nLength - 1, j = 2; i >= 0 && (szLine[i] == ' ' || szLine[i] == '\t'); i--);
			if(i >= 0 && szLine[i] == '{')
				szSpaceBuf[j++] = '\t';
			for(i = 0; i < nLength && (szLine[i] == ' ' || szLine[i] == '\t'); i++, j++)
				szSpaceBuf[j] = szLine[i];
			if(szLine[i] == '\0' || i >= nsPos - 1)
				nsPos = 1;
			else if(szLine[i] == '*' && i < nsPos)
				szSpaceBuf[j++] = '\t';
            szSpaceBuf[j] = '\0';

			FootySetSelA(activeFootyID, nsLine, nsPos, neLine, nePos);
			FootySetSelText(activeFootyID, szSpaceBuf);
			free(szSpaceBuf);
			return 0;

		}
		break;
	}

	case WM_CHAR:
	{
		char szInsBuf[2] = { '\0' };
		const char *szLine;
		int nsLine, nsPos, neLine, nePos, nLength, i;
		static char chPrevByte;

		if((wParam == '*' || wParam == '}') && !_ismbblead(chPrevByte)){
			FootyGetSelA(activeFootyID, &nsLine, &nsPos, &neLine, &nePos);
            szLine = FootyGetLineData(activeFootyID, nsLine);
			nLength = FootyGetLineLen(activeFootyID, nsLine);

			for(i = nsPos - 2; i >= 0 && (szLine[i] == ' ' || szLine[i] == '\t'); i--);
			if(i < 0)
				FootySetSelA(activeFootyID, nsLine, (wParam == '*' ? 1 : (nsPos > 1 ? nsPos - 1 : 1)), neLine, nePos);
			if( lParam & 0xFF0000 ) { // 直接入力の場合に処理
				szInsBuf[0] = (char)(TCHAR)wParam;
			}
            FootySetSelText(activeFootyID, szInsBuf);
			return 0;
		} else {
			chPrevByte = wParam;
		}
		break;
	}

	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		 PutLineNumber();
		 break;

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
    int              iSelBeg, iSelEnd, iEnable ;
	int				 iSelLBeg, iSelLEnd, iLength;
	MENUITEMINFO	 mii;
	HIMC			 hIMC;
	HWND			 hFooty;
	//case WM_INITMENUPOPUP :
			switch (lParam)
                    {
					case -1 :		// Popup menu
						 do{
                             hFooty = FootyGetWnd(activeFootyID);
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

                    case 0 :        // Edit menu
                    case 1 :        // Edit menu
						{

                              // Enable Undo if edit control can do it
                        EnableMenuItem ((HMENU) wParam, IDM_UNDO,
                              FootyGetMetrics(activeFootyID, F_GM_UNDOREM) > 0 ? MF_ENABLED : MF_GRAYED) ;

						      // Enable Redo if edit control can do it

//                         EnableMenuItem ((HMENU) wParam, IDM_REDO, nUndoNum > 0 ?
//								   MF_ENABLED : MF_GRAYED) ;
//                         EnableMenuItem ((HMENU) wParam, IDM_REDO,
//                              SendMessage (hwndEdit, WM_USER + 85/*(EM_CANREDO)*/, 0, 0L) ?
//                                   MF_ENABLED : MF_GRAYED) ;
						 EnableMenuItem ((HMENU) wParam, IDM_REDO,
                              FootyGetMetrics(activeFootyID, F_GM_REDOREM) > 0 ? MF_ENABLED : MF_GRAYED) ;


                              // Enable Paste if text is in the clipboard

                         EnableMenuItem ((HMENU) wParam, IDM_PASTE,
                              IsClipboardFormatAvailable (CF_TEXT) ?
                                   MF_ENABLED : MF_GRAYED) ;

                              // Enable Cut, Copy, and Del if text is selected

//                         SendMessage (hwndEdit, EM_GETSEL, (WPARAM) &iSelBeg,
//                                                           (LPARAM) &iSelEnd) ;
						 FootyGetSelA(activeFootyID, &iSelLBeg, &iSelBeg, &iSelLEnd, &iSelEnd);

                         iEnable = iSelBeg != iSelEnd  || iSelLBeg != iSelLEnd ? MF_ENABLED : MF_GRAYED ;

                         EnableMenuItem ((HMENU) wParam, IDM_CUT,    iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_COPY,   iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_CLEAR,  iEnable) ;

						 iLength = FootyGetTextLength(activeFootyID);
						 if(FootyGetSelB(activeFootyID, &iSelBeg, &iSelEnd) == F_RET_FAILED)
							 iSelEnd = 0;
						 EnableMenuItem ((HMENU) wParam, IDM_SELALL,
							 (iLength > 0 && iLength != iSelEnd - iSelBeg) ? MF_ENABLED : MF_GRAYED) ;
                         break ;
						}

                    case 2 :        // Search menu

                              // Enable Find, Next, and Replace if modeless
                              //   dialogs are not already active

                         iEnable = hDlgModeless == NULL ?
                                        MF_ENABLED : MF_GRAYED ;

                         EnableMenuItem ((HMENU) wParam, IDM_FIND,    iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_NEXT,    iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_REPLACE, iEnable) ;
                         break ;

					case 5 :		// HSP menu

                        iEnable = hsp_fullscr ? MF_CHECKED : MF_UNCHECKED ;
						CheckMenuItem ((HMENU) wParam, IDM_FULLSCR, iEnable) ;
                        iEnable = hsp_debug ? MF_CHECKED : MF_UNCHECKED ;
						CheckMenuItem ((HMENU) wParam, IDM_DEBUG, iEnable) ;
//                      iEnable = hsp_extmacro ? MF_CHECKED : MF_UNCHECKED ;
//						CheckMenuItem ((HMENU) wParam, IDM_HSPEXTMACRO, iEnable) ;
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
	int ln;
	ln = FootyGetCaretLine(activeFootyID);
	wsprintf (szBuffer, "line : %d", ln) ;
	Statusbar_mes( szBuffer );
}

							 
LRESULT CALLBACK EditDefProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int iOffset, SelStart, SelEnd;
	LPFINDREPLACE pfr ;

	// Process "Find-Replace" iMsgs

	if (iMsg == iMsgFindReplace)
	{
		pfr = (LPFINDREPLACE) lParam ;
		FootyGetSelB(activeFootyID, &SelStart, &SelEnd);

		// 新しい検索方式のために調整 (fixed@1.06β1 byLonelyWolf)
		iOffset = (pfr->Flags & FR_DOWN) ? SelEnd - 1 : SelStart - 1 ;

		if (pfr->Flags & FR_DIALOGTERM) hDlgModeless = NULL ;

		if (pfr->Flags & FR_FINDNEXT)
			if (!PopFindFindText (hwndEdit, iOffset, pfr)){
#ifdef JPNMSG
                OkMessage2 ( "終わりまで検索しました", "") ;
#else
                OkMessage2 ( "Not found.", "") ;
#endif
			}

		if (pfr->Flags & FR_REPLACEALL)
            iOffset = 0;

        if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL)
			if (!PopFindReplaceText (hwndEdit, iOffset, pfr))
#ifdef JPNMSG
				OkMessage2 ( "終わりまで置換しました", "") ;
#else
				OkMessage2 ( "Replace finished.", "") ;
#endif

        if (pfr->Flags & FR_REPLACEALL)
			while (PopFindReplaceText (hwndEdit, iOffset, pfr) );

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

/*
     if (iMsg == WM_KEYDOWN )
			 {
				 PutLineNumber();
                 //OkMessage ( "Huh??", "") ;
			 }
*/

	 switch(iMsg) {
		case WM_COMMAND :
			low = LOWORD (wParam);
			if ( low >= IDM_AHTEZINPUT ) {
				ExecEzInputMenu( low );
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
					int nTabNumber, nFootyID;
					TABINFO *lpTabInfo;
					bool bCreated;
					ULONGLONG ullFileIndex;

					if (PopFileOpenDlg (hwnd, szFileName, szTitleName)){
						GetDirName(szDirName, szFileName);
						SetCurrentDirectory(szDirName);
						ullFileIndex = GetFileIndex(szFileName);
						if((nTabNumber = SearchTab(NULL, NULL, NULL, ullFileIndex)) >= 0){
							ActivateTab(activeID, nTabNumber);
							return 0;
						}

                        lpTabInfo = GetTabInfo(activeID);

						nFootyID = activeFootyID;
						if(lpTabInfo == NULL
							|| lpTabInfo->FileName[0] != '\0'
							|| FootyGetMetrics(activeFootyID, F_GM_UNDOREM) > 0
							|| FootyGetMetrics(activeFootyID, F_GM_REDOREM) > 0){
								CreateTab(activeID, szTitleName, szFileName, szDirName);
								bCreated = true;
							} else {
								SetTabInfo(activeID, szTitleName, szFileName, szDirName, FALSE);
								bCreated = false;
							}
						if (!PopFileRead (activeFootyID, szFileName)){
#ifdef JPNMSG
                            OkMessage ( "%s をロードできませんでした。", szTitleName) ;
#else
                            OkMessage ( "Loading %s fault.", szTitleName) ;
#endif
							if(bCreated) {
								DeleteTab(activeID);
							} else {
								SetFileName( "", "", "" );
							}
							break;
						}

						GetTabInfo(activeID)->FileIndex = ullFileIndex;

						bNeedSave = FALSE ;
						DoCaption (szTitleName, activeID) ;
					}

					return 0 ;
				}

				case IDM_RELOAD:
				{
					bool bUndo;

					if(szFileName[0] == '\0')
						return 0;

					bUndo = (FootyGetMetrics(activeID, F_GM_UNDOREM) > 0);
					if((!bNeedSave && !bUndo) || 
						msgboxf(hwnd, "再読込を行う上で、下記のことが起こります。よろしいですか？\n\n"
						"・「元に戻す」の情報が破棄され、再読込以前に戻れなくなります。%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, bNeedSave ? "\n・変更が無視されます" : "") == IDYES)
                        	poppad_reload(activeID);
					return 0;
				}

				case IDM_RELOADTAB:
				{
					TABINFO *lpTabInfo = GetTabInfo(ClickID);
					bool bUndo;

					if(lpTabInfo == NULL || lpTabInfo->FileName[0] == '\0')
						return 0;

					bUndo = (FootyGetMetrics(ClickID, F_GM_UNDOREM) > 0);
					if((!lpTabInfo->NeedSave && !bUndo) || 
						msgboxf(hwnd, "再読込を行う上で、下記のことが起こります。よろしいですか？\n\n"
						"・「元に戻す」の情報が破棄され、再読込以前に戻れなくなります。%s",
						"Warning", MB_YESNO | MB_ICONQUESTION, lpTabInfo->NeedSave ? "\n・変更が無視されます" : "") == IDYES)
                        	poppad_reload(ClickID);
					return 0;
				}

				case IDM_SAVE :
					if (szFileName[0]){
                        if (PopFileWrite (activeFootyID, szFileName)){
							SetTabInfo(activeID, NULL, NULL, NULL, (bNeedSave = FALSE));
							GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeFootyID, F_GM_UNDOREM);
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
							GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeFootyID, F_GM_UNDOREM);
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
					FootyUndo(activeFootyID);
					return 0 ;

				case IDM_REDO :
					FootyRedo(activeFootyID);
					return 0;

				case IDM_CUT :
					FootyCut(activeFootyID);
					return 0 ;

				case IDM_COPY :
					FootyCopy(activeFootyID);
					return 0 ;

				case IDM_PASTE :
					FootyPaste(activeFootyID);
					return 0 ;

				case IDM_CLEAR :
					FootySetSelText(activeFootyID, "");
					return 0 ;

				case IDM_SELALL :
					FootySelectAll(activeFootyID);
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

					FootyGetSelB(activeFootyID, &SelStart, &SelEnd);
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
					DialogBox (hInst, "JumpBox", hwnd, (DLGPROC)JumpDlgProc);
					if (cln == -1) return 0;
					FootySetCaretLine(activeFootyID, cln+1);
					PutLineNumber();
					return 0;

				case IDM_LBTM:
					FootySetCaretLine(activeFootyID, FootyGetLines(activeFootyID));
					PutLineNumber();
					return 0;

				case IDM_LTOP:
					FootySetCaretLine(activeFootyID, 1);
					PutLineNumber();
					return 0;

				case IDM_LABEL:
					DialogBox (hInst, "LabelBox", hwnd, (DLGPROC)LabelDlgProc);
					if (cln == -1) return 0;
					FootySetCaretLine(activeFootyID, cln+1);
					PutLineNumber();
					return 0;

					// Messages for HSP

				case IDM_MKOBJ:
					if (szFileName[0]!=0) {
						PopFileWrite (activeFootyID, szFileName);

						SetTabInfo(activeID, NULL, NULL, NULL, (bNeedSave = FALSE));
						GetTabInfo(activeID)->LatestUndoNum = FootyGetMetrics(activeID, F_GM_UNDOREM);
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
					PopFileWrite ( activeFootyID, "hsptmp" );
					if ( mkobjfile2( "hsptmp" ) ){
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
					PopFileWrite ( activeFootyID, "hsptmp" );
					if ( mkexefile2( "hsptmp" ) ){
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
					PopFileWrite ( activeFootyID, "hsptmp" );
					strcpy(tmpfn,"hsptmp");
					hsc_ini( 0,(int)tmpfn, 0,0 );
					myfile();
					hsc_refname( 0,(int)compfile, 0,0 );
					strcpy( objname,"obj" );
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
						if (hsp_clmode==0) { hsprun_log("obj"); } else { hsprun_log_cl("obj"); }
						return 0;
					}

				case IDM_RUN:
					if (hsp_clmode==0) { hsprun("obj"); } else { hsprun_cl("obj"); }
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
					strcpy( objname,"obj" );
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
					strcpy(tmpfn,"hsptmp");
					hsc_ini( 0,(int)tmpfn, 0,0 );
					myfile();
					hsc_refname( 0,(int)compfile, 0,0 );
					strcpy( objname,"obj" );
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
/*
				case IDM_HSPEXTMACRO:
					hsp_extmacro^=1;
					return 0;
*/
				case IDM_HSPCLMODE:
					hsp_clmode^=1;
					return 0;

				case IDM_DEBUG:
					hsp_debug^=1;
					return 0;

				case IDM_CMDOPT:
					DialogBox (hInst, "CmdBox", hwnd, (DLGPROC)OptDlgProc);
					return 0;

				case IDM_PACKED:
					DialogBox (hInst, "FileBox", hwnd, (DLGPROC)PlistDlgProc);
					return 0;

				case IDM_PACKGO:
					packgo();
					return 0;

				case IDM_MKEXE1:
					exemode=1;
					DialogBox (hInst, "MkExe", hwnd, (DLGPROC)FnameDlgProc);
					chklstr(hsp_laststr);
					if (hsp_fnstr) mkexe(hsp_laststr);
					return 0;

				case IDM_MKEXE2:
					exemode=1;
					DialogBox (hInst, "MkExe", hwnd, (DLGPROC)FnameDlgProc);
					chklstr(hsp_laststr);
					if (hsp_fnstr) mkscr(hsp_laststr);
					return 0;

				case IDM_START_RUNTIMEMAN:
					wsprintf( tmpfn, "\"%s\\hspat.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0;

				case IDM_OPEN_SRCFOLDER:
						GetCurrentDirectory( _MAX_PATH, tmpfn );
					ShellExecute( NULL, "explore", tmpfn, NULL, NULL, SW_SHOWNORMAL );
					return 0;

				case IDM_AHTTOOL:
					wsprintf( tmpfn, "\"%s\\ahtman.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0;

                case IDM_HSPTV:
					wsprintf( tmpfn, "\"%s\\hsptv.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0 ;

				// Messages from Help menu

				case IDM_KWHELP:
					getkw();gethdir();
					callhelp();
					return 0;

                case IDM_HSPMAN1 :
					wsprintf( helpopt,"%s\\docs\\hspprog.htm", szExeDir );
					ShellExecute( NULL, NULL, helpopt, NULL, NULL, SW_SHOW );
					return 0 ;

                case IDM_HSPMAN2 :
					wsprintf( tmpfn, "\"%s\\hsphelp\\helpman.exe\"", szExeDir );
					WinExec( tmpfn, SW_SHOW );
					return 0 ;

                case IDM_HSPMAN3 :
					wsprintf( helpopt,"%s\\index.htm", szExeDir );
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
							lpTabInfo->LatestUndoNum = FootyGetMetrics(lpTabInfo->FootyID, F_GM_UNDOREM);
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
								lpTabInfo->LatestUndoNum = FootyGetMetrics(lpTabInfo->FootyID, F_GM_UNDOREM);
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

				case IDM_OPTION:
					EnableWindow(hWndMain, FALSE);
					hConfigDlg = CreateDialog(hInst, "CONFIG", hwnd, ConfigDlgProc);
					return 0;

				case IDM_OPENIME:
				{
					HIMC hIMC;
					HWND hFooty;

					hFooty = FootyGetWnd(activeFootyID);
					if(hFooty == NULL) return 0;
					hIMC = ImmGetContext(hFooty);

					if(hIMC == NULL) return 0;
					ImmSetOpenStatus(hIMC, !ImmGetOpenStatus(hIMC));
					ImmReleaseContext(hFooty, hIMC);
					return 0;
				}

				case IDM_RECONVERT:
				{
					// 未実装
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
						ShellExecute(NULL, NULL, "http://hsp.tv/", NULL, NULL, SW_SHOW);
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
				return (BOOL)(HBRUSH)GetStockObject(NULL_BRUSH);
			}
			break;
	}
    return FALSE ;
}

BOOL CALLBACK LogcompDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
               switch (wParam)
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
			tvis.item.pszText = "全般";
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "動作";
				tvis.item.lParam  = (LPARAM)(hPage[0] = CreateDialog(hInst, "PP_BEHAVIOR", hDlg, ConfigBehaviorPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				hSelItem = TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "ディレクトリ";
				tvis.item.lParam  = (LPARAM)(hPage[1] = CreateDialog(hInst, "PP_DIRECTORY", hDlg, ConfigDirectoryPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
			TreeView_Expand(hTree, tvis.hParent, TVE_EXPAND);

			tvis.hParent      = TVI_ROOT;
			tvis.item.pszText = "エディタ";
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "フォント";
				tvis.item.lParam  = (LPARAM)(hPage[2] = CreateDialog(hInst, "PP_FONT", hDlg, ConfigFontPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "色";
				tvis.item.lParam  = (LPARAM)(hPage[3] = CreateDialog(hInst, "PP_COLOR", hDlg, ConfigColorPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "表示";
				tvis.item.lParam  = (LPARAM)(hPage[4] = CreateDialog(hInst, "PP_NONCHARACTOR", hDlg, ConfigVisualPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "色分けキーワード";
				tvis.item.lParam  = (LPARAM)(hPage[5] = CreateDialog(hInst, "PP_KEYWORD", hDlg, ConfigKeywordPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
			TreeView_Expand(hTree, tvis.hParent, TVE_EXPAND);

			tvis.hParent      = TVI_ROOT;
			tvis.item.pszText = "ツール";
			tvis.item.lParam  = NULL;
			tvis.hParent = TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "アドイン";
				tvis.item.lParam  = (LPARAM)(hPage[6] = CreateDialog(hInst, "PP_ADDIN", hDlg, ConfigAddinPageProc));
				MoveWindow((HWND)tvis.item.lParam, rect.left, rect.top, rect.right, rect.bottom, TRUE);
				TreeView_InsertItem(hTree, &tvis);
				tvis.item.pszText = "外部ツール";
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
			hFont = CreateFont((rect.bottom-rect.top) * 3 / 5, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ Ｐゴシック");
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
					FootySetFocus(activeFootyID);
					DestroyWindow(hDlg);
					return TRUE;

				case IDM_BUTTON1:
					if(MessageBox(hDlg, "現在開いているページの設定を初期状態に戻しますが、よろしいですか？\n"
						"([OK]ボタンを押さなければ、エディタには反映されません)", "確認", MB_YESNO | MB_ICONQUESTION) == IDYES)
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
			/*else if(pnmtv->hdr.code == TVN_SELCHANGED){
				hTree = GetDlgItem(hDlg, IDC_TREE1);
				hti = TreeView_GetChild(hTree, pnmtv->itemNew.hItem);
				if(hti != NULL)
					TreeView_SelectItem(hTree, hti);
				return TRUE;
			}*/
			break;
		}
	}
	return FALSE;
}

BOOL CALLBACK ConfigBehaviorPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_RADIO1, !bUseIni);
			CheckDlgButton(hDlg, IDC_RADIO2, bUseIni);
			CheckDlgButton(hDlg, IDC_RADIO3+hsp_helpmode, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK1, bAutoIndent != FALSE);
			return TRUE;

		case PM_ISAPPLICABLE:
			SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case PM_APPLY:
			bUseIni = (bool)IsDlgButtonChecked(hDlg, IDC_RADIO2);
			bAutoIndent = (bool)IsDlgButtonChecked(hDlg, IDC_CHECK1);
			hsp_helpmode=CheckRadio(hDlg, IDC_RADIO3, 4);
			return TRUE;

		case PM_SETDEFAULT:
			CheckDlgButton(hDlg, IDC_RADIO1, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO4, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RADIO5, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_RADIO6, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
			return TRUE;
	}
	return FALSE;
}
BOOL CALLBACK ConfigAddinPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	const char *szCategory[] = { "文字の色分け", "特殊な記号の配色", "エディタの配色", "行番号表示領域の配色", "ルーラーの配色"};
	const char *szCharItem[] = { "通常の文字", "命令/関数", "プリプロセッサ命令", "文字列", "マクロ", "コメント", "ラベル" };
	const char *szNonCharItem[] = { "半角スペース", "全角スペース", "TAB文字", "改行記号", "[EOF](ファイル終端)記号" };
	const char *szEditItem[] = { "全体の背景色", "キャレット行の下線", "行番号とエディタの境界線" };
	const char *szLineNumItem[] = { "行番号", "キャレット行の強調" };
	const char *szRolerItem[] = { "数字", "背景色", "目盛り", "キャレット位置の強調" };

	typedef struct tagItems{
		const char **item;
		int num;
	} ITEMS;

	ITEMS items[] = {
		szCharItem,    sizeof(szCharItem)    / sizeof(szCharItem[0]),
		szNonCharItem, sizeof(szNonCharItem) / sizeof(szNonCharItem[0]),
		szEditItem,    sizeof(szEditItem)    / sizeof(szEditItem[0]),
		szLineNumItem, sizeof(szLineNumItem) / sizeof(szLineNumItem[0]),
		szRolerItem,   sizeof(szRolerItem)   / sizeof(szRolerItem[0]),
	};

	switch(message){
		case WM_INITDIALOG:
		{
			int i, num;
			num = sizeof(szCategory) / sizeof(szCategory[0]);
			for(i = 0; i < num; i++)
				SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)szCategory[i]);
			SendDlgItemMessage(hDlg, IDC_COMBO1, CB_SETCURSEL, 0, 0L);

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

			num = sizeof(szColorName) / sizeof(szColorName[0]);
			for(i = 0; i < num; i++)
				SendDlgItemMessage(hDlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)szColorName[i]);

			CopyMemory(crColor, &color, sizeof(crColor));
			PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_COMBO1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_COMBO1));

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
			ResetClassify();
			return TRUE;

		case PM_SETDEFAULT:
			DefaultColor((MYCOLOR *)crColor);
			PostMessage(hDlg, WM_COMMAND, MAKELONG(IDC_COMBO1, CBN_SELCHANGE), (LPARAM)GetDlgItem(hDlg, IDC_COMBO1));
			return TRUE;

	}
	return FALSE;
}
BOOL CALLBACK ConfigDirectoryPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			switch(LOWORD(wParam)){
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
						MessageBox(hDlg, "ファイル名を省略することはできません。", "error", MB_OK | MB_ICONERROR);
						return TRUE;
					}

                    GetDlgItemText(hDlg, IDC_EDIT1, lpExtToolInfo->ToolName, SIZE_OF_TOOLNAME);
					GetDlgItemText(hDlg, IDC_EDIT2, lpExtToolInfo->FileName, SIZE_OF_FILENAME);
					GetDlgItemText(hDlg, IDC_EDIT3, lpExtToolInfo->CmdLine,  SIZE_OF_CMDLINE);
					GetDlgItemText(hDlg, IDC_EDIT4, lpExtToolInfo->WorkDir,  SIZE_OF_WORKDIR);
					lpExtToolInfo->ShowOnMainMenu    = IsDlgButtonChecked(hDlg, IDC_CHECK1);
					lpExtToolInfo->ShowOnPopupMenu   = IsDlgButtonChecked(hDlg, IDC_CHECK2);
					lpExtToolInfo->ExecOnStartup     = IsDlgButtonChecked(hDlg, IDC_CHECK3);
					lpExtToolInfo->ExecWithOverwrite = IsDlgButtonChecked(hDlg, IDC_CHECK4);

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
			char *szColumnTitle[] = { "名前", "ファイル パス", };
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
	if(logfont->lfWeight >= FW_BOLD) lstrcpy(buf, "太字 ");
	if(logfont->lfItalic) lstrcat(buf, "斜体");
	if(lstrlen(buf) == 0) lstrcpy(buf, "標準");
}
static void GetExStyleStr(char *buf, LOGFONT *logfont)
{
	lstrcpy(buf, "");
	if(logfont->lfStrikeOut) lstrcat(buf, "取り消し線 ");
	if(logfont->lfUnderline) lstrcat(buf, "下線");
	if(lstrlen(buf) == 0) lstrcpy(buf, "無し");
	return;
}
BOOL CALLBACK ConfigFontPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

			PopFontInitChooseFont(NULL, NULL);
			return TRUE;
		}

		case WM_DESTROY:
			DeleteObject(hEditFont);
			DeleteObject(hTabFont);
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)){
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

BOOL CALLBACK ConfigKeywordPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
		case WM_INITDIALOG:
		{
			HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
			const char *szFileName;

			for(size_t i = 0; i < filelist.num(); i++){
				szFileName = filelist.get(i);
				if(SendMessage(hListBox, LB_FINDSTRINGEXACT, -1, (LPARAM)szFileName) == LB_ERR)
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szFileName);
			}
			
			return TRUE;
		}

		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_BUTTON1:
				{
					char szFileName[_MAX_PATH + 1] = "";
					HWND hListBox = GetDlgItem(hDlg, IDC_LIST1);
					int nFoundSel;

					if(DialogBoxParam(hInst, "SELSRC", hDlg, SelectSourceProc, (LPARAM)szFileName) == IDOK
						&& szFileName[0] != '\0'){
							nFoundSel = (int)SendMessage(hListBox, LB_FINDSTRINGEXACT, -1, (LPARAM)szFileName);
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
								nFoundSel = (int)SendMessage(hListBox, LB_FINDSTRINGEXACT, -1, (LPARAM)szFileName);
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

BOOL CALLBACK ConfigVisualPageProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_CHECK1, tabEnabled);
			CheckDlgButton(hDlg, IDC_CHECK2, hsEnabled);
			CheckDlgButton(hDlg, IDC_CHECK3, fsEnabled);
			CheckDlgButton(hDlg, IDC_CHECK4, nlEnabled);
			CheckDlgButton(hDlg, IDC_CHECK5, eofEnabled);
			CheckDlgButton(hDlg, IDC_CHECK6, ulEnabled);
			SetDlgItemInt(hDlg, IDC_EDIT1, tabsize,     FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT2, rulerheight, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT3, linewidth,   FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, linespace,   FALSE);
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
				MessageBox(hDlg, "表示設定の適用に失敗しました。\nサイズには0以上の整数を入力して下さい。", "Error",
					MB_OK | MB_ICONERROR);
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
			tabsize     = GetDlgItemInt(hDlg, IDC_EDIT1, NULL, FALSE);
			rulerheight = GetDlgItemInt(hDlg, IDC_EDIT2, NULL, FALSE);
			linewidth   = GetDlgItemInt(hDlg, IDC_EDIT3, NULL, FALSE);
			linespace   = GetDlgItemInt(hDlg, IDC_EDIT4, NULL, FALSE);
			poppad_setalledit();
			return TRUE;

		case PM_SETDEFAULT:
			CheckDlgButton(hDlg, IDC_CHECK1, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK2, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK3, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK4, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_CHECK5, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_CHECK6, BST_CHECKED);
			SetDlgItemInt(hDlg, IDC_EDIT1, 4,  FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT2, 10, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT3, 50, FALSE);
			SetDlgItemInt(hDlg, IDC_EDIT4, 0,  FALSE);
			return TRUE;
	}
	return FALSE;
}

void LoadFromCommandLine(char *lpCmdLine)
{
	TABINFO *lpTabInfo;
	int SearchResult, ActivateID;
	BOOL bFileLoad = FALSE;
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
			if(lpTabInfo == NULL || lpTabInfo->FileName[0] != '\0' || FootyGetMetrics(activeFootyID, F_GM_UNDOREM) > 0
				|| FootyGetMetrics(activeFootyID, F_GM_REDOREM) > 0){
					CreateTab(activeID, "", path, "");
					bCreated = true;
				} else {
					SetTabInfo(activeID, NULL, path, NULL, -1);
                    bCreated = false;
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

LRESULT FileDrop(WPARAM wParam, LPARAM lParam)
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
			if(lpTabInfo == NULL || lpTabInfo->FileName[0] != '\0' || FootyGetMetrics(activeFootyID, F_GM_UNDOREM) > 0
				|| FootyGetMetrics(activeFootyID, F_GM_REDOREM) > 0){
					CreateTab(activeID, "", tmpfn, "");
					bCreated = true;
				} else {
					SetTabInfo(activeID, NULL, tmpfn, NULL, -1);
					bCreated = false;
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

void __stdcall OnFootyChange(int id, void *pParam, int nStatus)
{
	int nTabID = GetTabID(id);

	TABINFO *lpTabInfo = GetTabInfo(nTabID);

	if(lpTabInfo == NULL)
		return;

	if(FootyGetMetrics(id, F_GM_UNDOREM) == lpTabInfo->LatestUndoNum && lpTabInfo->NeedSave == TRUE){
		SetTabInfo(nTabID, NULL, NULL, NULL, (bNeedSave = FALSE));
		DoCaption(szTitleName, nTabID);
	} else if(lpTabInfo->NeedSave == FALSE && nStatus != FECH_SETTEXT){
		SetTabInfo(nTabID, NULL, NULL, NULL, (bNeedSave = TRUE));
		DoCaption(szTitleName, nTabID);
	}

	PutLineNumber();
	return;
}

void __stdcall OnFootyContextMenu(void *pParam, int id)
{
	POINT pt;

	GetCursorPos(&pt);
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndClient, NULL);
	return;
}