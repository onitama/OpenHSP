
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
#include <direct.h>
#include <winuser.h>
#include <shlobj.h>
#include "poppad.h"
#include "resource.h"
#include <htmlhelp.h>

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
DLLFUNC hsc3_getruntime;		// 3.0用の追加
DLLFUNC hsc3_run;				// 3.0用の追加

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
	hsc3_getruntime = (DLLFUNC)SetDllFunc("hsc3_getruntime");		// 3.0用の追加
	hsc3_run = (DLLFUNC)SetDllFunc("hsc3_run");						// 3.0用の追加

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
#define UNTITLED "(untitled)"

static BOOL      bNeedSave = FALSE ;
static char      szDirName[_MAX_PATH] ;
static char      szFileName[_MAX_PATH] ;
static char      szTitleName[_MAX_FNAME + _MAX_EXT] ;
static HINSTANCE hInst ;
static HWND      hwndEdit ;
static int       iOffset ;
static UINT      iMsgFindReplace ;
static char		 kwstr[512];
static char		 hdir[_MAX_PATH];


extern char szExeDir[_MAX_PATH];
extern char szStartDir[_MAX_PATH];
extern char szCmdline[_MAX_PATH];
extern int	winx;
extern int	winy;
extern int	posx;
extern int	posy;
extern int	winflg;
extern int	flg_toolbar;
extern int	flg_statbar;
extern int	startflag;
extern char startdir[_MAX_PATH];


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyEditProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ErrDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PlistDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LogcompDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK StartDirDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void ConfigEditColor( COLORREF fore, COLORREF back );
int CheckWndFlag ( void );

          // Functions in POPFILE.C

void PopFileInitialize (HWND) ;
BOOL PopFileOpenDlg    (HWND, PSTR, PSTR) ;
BOOL PopFileSaveDlg    (HWND, PSTR, PSTR) ;
BOOL PopFileRead       (HWND, PSTR) ;
BOOL PopFileWrite      (HWND, PSTR) ;

          // Functions in POPFIND.C

HWND PopFindFindDlg     (HWND) ;
HWND PopFindReplaceDlg  (HWND) ;
BOOL PopFindFindText    (HWND, int *, LPFINDREPLACE) ;
BOOL PopFindReplaceText (HWND, int *, LPFINDREPLACE) ;
BOOL PopFindNextText    (HWND, int *) ;
BOOL PopFindValidFind   (void) ;
BOOL PopFindReplaceAll (HWND hwndEdit, int *piSearchOffset, LPFINDREPLACE pfr);

          // Functions in POPFONT.C

void PopFontInitialize   (HWND) ;
BOOL PopFontChooseFont   (HWND) ;
void PopFontSetFont      (HWND) ;
void PopFontDeinitialize (void) ;
void PopFontSetLG ( LOGFONT lf );
LOGFONT PopFontGetLG ( void );
void PopFontSetColor( DWORD col );
DWORD PopFontGetColor( void );
DWORD PopFontChooseColor( HWND hwnd );

          // Functions in POPPRNT.C

BOOL PopPrntPrintFile (HINSTANCE, HWND, HWND, PSTR) ;

          // Functions in STATBAR.C

void Statusbar_mes( char *mes );


          // Global variables

static char szAppName[] = "OniPad" ;

HWND hDlgModeless ;
static int cln;
static HWND hwbak ;

WNDPROC	Org_EditProc;		// サブクラス化の初期値


/*
		HSP related service
*/

static char *errbuf=NULL;					// error message buffer

static int		hsp_fullscr = 0;
static int		hsp_fnstr = 0;

#define TMPSIZE 2048

static char		hsp_laststr[TMPSIZE];
static char		hsp_cmdopt[TMPSIZE];
static char		compfile[TMPSIZE];
static char		hsp_helpdir[TMPSIZE];
static char		execmd[TMPSIZE];
static char		objname[TMPSIZE];
static char		helpopt[TMPSIZE];
static char		hsp_extstr[TMPSIZE];

static int		hscroll_flag;
static int		flg_font;
static LOGFONT	chg_font;
static COLORREF	fgcolor;
static COLORREF	bgcolor;
static int		exemode;
static int		hsp_wx;
static int		hsp_wy;
static int		hsp_wd;
static int		hsp_orgpath;
static int		hsp_debug;
static int		hsp_extobj;
static int		hsp_helpmode;

static int		hsp_logflag;
static int		hsp_logmode;
static int		hsp_logadd;
static int		hsed_ver;
static int		hsp_extmacro;
static int		hsp_clmode;

/*
		Registry I/O routines
*/

static int reg_getkey( HKEY hKey, char *readkey, int *value )
{
	unsigned long RVSize = 4;
	DWORD	RVal, RKind = REG_DWORD;
	if (RegQueryValueEx(hKey, readkey, NULL, &RKind, (BYTE *)(&RVal), &RVSize) == ERROR_SUCCESS) {
		*value=RVal;
		return 0;
	}
	else return -1;
}

static int reg_sgetkey( HKEY hKey, char *readkey, char *strbuf )
{
	unsigned long RVSize = 128;
	DWORD	RKind = REG_SZ;
	if (RegQueryValueEx(hKey, readkey, NULL, &RKind, (BYTE *)strbuf, &RVSize) == ERROR_SUCCESS)
	return 0;
	else return -1;
}

static int reg_bgetkey( HKEY hKey, char *readkey, BYTE *buf, int length )
{
	unsigned long RVSize;
	DWORD	RKind = REG_BINARY;
	RVSize=(unsigned long)length;
	if (RegQueryValueEx(hKey, readkey, NULL, &RKind, buf, &RVSize) == ERROR_SUCCESS)
	return 0;
	else return -1;
}

static void reg_setkey( HKEY hKey, char *writekey, int value )
{
	int		RVSize = 4;
	DWORD	dw = REG_DWORD;
	RegSetValueEx(hKey, writekey, 0, dw, (BYTE*)&value, RVSize);
}

static void reg_ssetkey( HKEY hKey, char *writekey, char *string )
{
	int		RVSize = 128;
	DWORD	sz = REG_SZ;
	RegSetValueEx(hKey, writekey, 0, sz, (BYTE*)string, RVSize);
}

static void reg_bsetkey( HKEY hKey, char *writekey, BYTE *buf, int length )
{
	RegSetValueEx(hKey, writekey, 0, REG_BINARY, buf, length);
}


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

static void err_prt( HWND hwnd )
{
	//	error message print
	//
	//MessageBox (hwnd, errbuf, "HSP error message", MB_OK | MB_ICONEXCLAMATION) ;
	DialogBox (hInst, "ErrBox", hwnd, (DLGPROC)ErrDlgProc);
}

static void OkMessage (char *szMessage, char *szTitleName)
{
     char szBuffer[256] ;
     wsprintf (szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED) ;
     MessageBox (hwbak, szBuffer, "script editor message", MB_OK | MB_ICONEXCLAMATION) ;
}

static void TMes( char *prtmes )
{
	OkMessage ( prtmes,"\0" );
}

static int fileok( char *fname )
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
#ifdef JPMSG
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
		if (a1=='\\') b=a;
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
#ifdef JPMSG
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
#ifdef JPMSG
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
#ifdef JPMSG
		TMes( "EXEファイルを作るためにはパックするファイル名一覧(PACKFILE)を\n作成しておく必要があります。" );
#else
		TMes( "[PACKFILE] needed." );
#endif
		return;
	}
#ifdef JPMSG
	expack( hsp_fullscr, exname, "EXEファイルを作成しました。" );
#else
	expack( hsp_fullscr, exname, "Create EXE file successfully." );
#endif
}


static void mkscr( char *exname )
{
	if (hsp_clmode) {
#ifdef JPMSG
		TMes( "コンソールモードでスクリーンセーバーの作成はできません。" );
#else
		TMes( "No operation for console mode." );
#endif
		return;
	}
	if (filechk("packfile")) {
#ifdef JPMSG
		TMes( "スクリーンセーバーを作るためにはパックするファイル名一覧(PACKFILE)を\n作成しておく必要があります。" );
#else
		TMes( "[PACKFILE] needed." );
#endif
		return;
	}
#ifdef JPMSG
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
	//		execute HSP3 process
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
	if ( i ) {
#ifdef JPMSG
			TMes("実行用ランタイムファイルが見つかりません。");
			TMes(execmd);
#else
			TMes("Runtime executable file missing.");
#endif
	}
}


static void hsprun_log( char *objname )
{
	//		execute HSP3 process (with LOG)
	//
	char dbopt[64];
	wsprintf( execmd,"\"%s\\%s\" ",szExeDir, DEFAULT_RUNTIME );
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
	strcat( execmd,ftmp );
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
	a=hsc_comp( 0,hsp_extmacro^1,0,0 );
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
	a=hsc_comp( 0,hsp_extmacro^1,0,0 );
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
	a=hsc_comp( 0,hsp_extmacro^1 | 4,0,0 );
	if ( a ) return a;

	sprintf( ftmp, "%s\\%s.dpm", szExeDir, srcfn );
	a=hsc3_make( 0,(int)ftmp,1,0 );
	if ( a ) return a;
	return 0;
}


/*
		Editor related routines
*/

static void gethdir( void )
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


static int GetCursorOffset( void )
{
	//		カーソル位置のオフセット値を得る(XP対策込み)
	//
	int buflen,ofs;
	char *strbuf;
	SendMessage ( hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs );
	if ( flag_xpstyle ) {
		buflen = GetWindowTextLength (hwndEdit);
		strbuf=(char *)malloc(buflen+16);
		GetWindowText (hwndEdit, strbuf, buflen + 1) ;
		ofs = getUnicodeOffset( strbuf, ofs );
		free(strbuf);
	}
	return ofs;
}


static void getkw( void )
{
	//		Get help keyword
	//
	int buflen,ofs,org,a,b;
	char *strbuf;
	char a1;

	SendMessage ( hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs );
	buflen = GetWindowTextLength (hwndEdit);
	strbuf=(char *)malloc(buflen+16);
	GetWindowText (hwndEdit, strbuf, buflen + 1) ;
	org = ofs;
	if ( flag_xpstyle ) ofs = getUnicodeOffset( strbuf, ofs );
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
#ifdef JPMSG
			TMes("ヘルプのためのHTMLファイルが見つかりません。\nディレクトリ設定を確認してください。");
#else
			TMes("Help data missing.Check help preference.");
#endif
			return;
		}
		ShellExecute( NULL,"open",helpopt,"","",SW_SHOW );
		return;
	}

	if (hsp_helpmode==1) {
		wsprintf(helpopt,"%shsp.hlp",hdir);
		if (fileok(helpopt)) {
#ifdef JPMSG
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
#ifdef JPMSG
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
#ifdef JPMSG
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
#ifdef JPMSG
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


void reg_save( void )
{
	//		レジストリに書き込む
	//
	HKEY	hKey;
	int		RVSize = 4;
	DWORD	dw = REG_DWORD;
	DWORD	sz = REG_SZ;

	RegCreateKeyEx(HKEY_CURRENT_USER,
		"Software\\OnionSoftware\\hsed", 0, "",
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
		NULL, &hKey, NULL);
	reg_setkey( hKey,"fullscr", hsp_fullscr );
	reg_setkey( hKey,"hscroll", hscroll_flag );
	reg_ssetkey( hKey,"cmdopt", hsp_cmdopt );
	reg_ssetkey( hKey,"laststr", hsp_laststr );
	chg_font=PopFontGetLG();
	reg_bsetkey( hKey,"font", (BYTE *)&chg_font, sizeof(LOGFONT) );
	reg_setkey( hKey,"textcolor", fgcolor );
	reg_setkey( hKey,"backcolor", bgcolor );

	CheckWndFlag();						// Window attribute check
	reg_setkey( hKey,"winflg", winflg );

	if (winflg==0) {
		reg_setkey( hKey,"winx", winx );
		reg_setkey( hKey,"winy", winy );
		reg_setkey( hKey,"posx", posx );
		reg_setkey( hKey,"posy", posy );
	}

	reg_setkey( hKey,"toolbar", flg_toolbar );
	reg_setkey( hKey,"statbar", flg_statbar );
	reg_ssetkey( hKey,"helpdir", hsp_helpdir );
	reg_setkey( hKey,"exewx", hsp_wx );
	reg_setkey( hKey,"exewy", hsp_wy );
	reg_setkey( hKey,"exewd", hsp_wd );
	reg_setkey( hKey,"debug", hsp_debug );
	reg_setkey( hKey,"extobj", hsp_extobj );
	reg_ssetkey( hKey,"extstr", hsp_extstr );
	reg_setkey( hKey,"helpmode", hsp_helpmode );
	reg_setkey( hKey,"logmode", hsp_logmode );
	reg_setkey( hKey,"logadd", hsp_logadd );
	reg_setkey( hKey,"hsedver", hsed_ver );
	reg_setkey( hKey,"extmacro", hsp_extmacro );
	reg_setkey( hKey,"clmode", hsp_clmode );
	reg_setkey( hKey,"orgpath", hsp_orgpath );

	reg_setkey( hKey,"startmode", startflag );
	reg_ssetkey( hKey,"startdir", startdir );
}


void reg_load( void )
{
	//		レジストリから読み込む
	//
	HKEY	hKey;

	//		Reset to default value

	flg_font=-1;
	fgcolor=RGB(255,255,255);
	bgcolor=RGB(0,0,0);
	hsp_cmdopt[0]=0;
	hsp_laststr[0]=0;
	hscroll_flag=1;
	hsp_helpdir[0]=0;
	hsp_wx=640;hsp_wy=480;
	hsp_wd=0;
	hsp_orgpath=0;
	hsp_debug=0;
	hsp_extobj=0;
	hsp_extstr[0]=0;
	hsp_helpmode=2;
	hsp_logmode = 15;
	hsp_logadd = 1;
	hsp_extmacro = 1;
	hsp_clmode = 0;
	hsed_ver = 0;
	startflag = STARTDIR_MYDOC;
	startdir[0] = 0;

	//		Read keys

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed",
		0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		reg_getkey( hKey,"fullscr", &hsp_fullscr );
		reg_getkey( hKey,"hscroll", &hscroll_flag );
		reg_sgetkey( hKey,"cmdopt", hsp_cmdopt );
		reg_sgetkey( hKey,"laststr", hsp_laststr );
		flg_font = reg_bgetkey( hKey,"font", (BYTE *)&chg_font, sizeof(LOGFONT) );
		reg_getkey( hKey,"textcolor", (int *)&fgcolor );
		reg_getkey( hKey,"backcolor", (int *)&bgcolor );
		reg_getkey( hKey,"winflg", &winflg );
		reg_getkey( hKey,"winx", &winx );
		reg_getkey( hKey,"winy", &winy );
		reg_getkey( hKey,"posx", &posx );
		reg_getkey( hKey,"posy", &posy );
		reg_getkey( hKey,"toolbar", &flg_toolbar );
		reg_getkey( hKey,"statbar", &flg_statbar );
		reg_sgetkey( hKey,"helpdir", hsp_helpdir );
		reg_getkey( hKey,"exewx", &hsp_wx );
		reg_getkey( hKey,"exewy", &hsp_wy );
		reg_getkey( hKey,"exewd", &hsp_wd );
		reg_getkey( hKey,"debug", &hsp_debug );
		reg_sgetkey( hKey,"extstr", hsp_extstr );
		reg_getkey( hKey,"extobj", &hsp_extobj );
		reg_getkey( hKey,"helpmode", &hsp_helpmode );
		reg_getkey( hKey,"logmode", &hsp_logmode );
		reg_getkey( hKey,"logadd", &hsp_logadd );
		//reg_getkey( hKey,"extmacro", &hsp_extmacro );
		reg_getkey( hKey,"clmode", &hsp_clmode );
		reg_getkey( hKey,"orgpath", &hsp_orgpath );

		reg_getkey( hKey,"startmode", &startflag );
		reg_sgetkey( hKey,"startdir", startdir );

		if ( reg_getkey( hKey,"hsedver", &hsed_ver ) ) {
			hsp_helpmode=2;
		}
	}
	hsed_ver = 0x0261;

	//		Check helpdir

	if (hsp_helpmode==0) {
		gethdir();
		wsprintf(helpopt,"%shsppidx.htm",hdir);
		if (fileok(helpopt)) {
			hsp_helpdir[0]=0;
		}
	}

}


void pophwnd( HWND hwnd )
{
	hwbak=hwnd;
}

void DoCaption ( char *szTitleName )
     {
     char szCaption[_MAX_PATH+128] ;

#ifdef JPMSG
     wsprintf (szCaption, "ＨＳＰスクリプトエディタ - %s",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#else
     wsprintf (szCaption, "HSP Script Editor - %s",
               szTitleName[0] ? szTitleName : UNTITLED) ;
#endif

     SetWindowText (hwbak, szCaption) ;
     }

short AskAboutSave (HWND hwnd, char *szTitleName)
     {
     char szBuffer[64 + _MAX_FNAME + _MAX_EXT] ;
     int  iReturn ;

#ifdef JPMSG
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


BOOL CALLBACK DirDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,hsp_helpdir );
				SetFocus( GetDlgItem(hDlg,IDC_EDIT1) );
				CheckDlgButton( hDlg, IDC_RADIO1+hsp_helpmode, BST_CHECKED );
				return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDC_BUTTON1:
						 if ( selfolder( hdir )==0 )
							 SetDlgItemText( hDlg,IDC_EDIT1,hdir );
                         return TRUE;
                    case IDOK:
						 hsp_helpmode=CheckRadio( hDlg,IDC_RADIO1,4 );
						 GetDlgItemText( hDlg,IDC_EDIT1,hsp_helpdir,128 );
						 /*
						 gethdir();
						 wsprintf(helpopt,"%shsppidx.htm",hdir);
						 if (fileok(helpopt)) {
							TMes("指定したディレクトリにはヘルプファイルが存在しません。\n入力を確認してください。");
							return TRUE;
						 }
						 */
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
                         EndDialog (hDlg, 0);
                         return TRUE;
                    }
               break ;
          }
     return FALSE ;
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
	int lab;
	int tag;
	int mytag;
	int myline, ln_s, ln_e;
	SendDlgItemMessage( hDlg,IDC_LIST2,LB_RESETCONTENT, 0, 0L );

	len = GetWindowTextLength (hwndEdit);
	buffer = (char *) malloc ( len + 1 );
	GetWindowText( hwndEdit, buffer, len+1 );

	SendMessage ( hwndEdit, EM_GETSEL, (WPARAM)&ln_s, (LPARAM)&ln_e );
	myline = SendMessage ( hwndEdit, EM_LINEFROMCHAR, (WPARAM)ln_s, 0 )+1;

	line = 1;
	wp = buffer;
	tag = -1;
	while(1) {
		a1=*wp;
		if ( a1=='*' ) {
			wp++;lab=0;
		} else lab=-1;

		while(1) {
			a1=*wp++;
			if ( a1==0 ) { line=-1; break; }
			if ( a1==13 ) {
				if ( *wp==10 ) wp++;
				break;
			}
			if (lab>=0) {
				if (( a1==':' )||( a1==';' )) {
					lab=-1;
				}
				else {
					lname[lab++]=a1;
				}
			}
		}
		if ( line<0 ) break;
		if (lab>=0) {
			lname[lab++]=0;
			wsprintf( st,"%5d : %s",line,lname );
			SendDlgItemMessage( hDlg,IDC_LIST2,LB_ADDSTRING,-1,(LPARAM)st );
			tag++;
		}
		if ( line<=myline) mytag=tag;
		line++;
	}

	if ( mytag>=0 ) {
		SendDlgItemMessage( hDlg, IDC_LIST2, LB_SETCURSEL, mytag, 0L );
	}
	free (buffer) ;
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
						i=SendDlgItemMessage( hDlg,IDC_LIST2, LB_GETCURSEL,0,0L );
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


int poppad_setsb( int flag )
{
	int chk;
	if (flag<0) chk=hscroll_flag;
	else if (flag<2) chk=flag;
	else chk=hscroll_flag^1;
	ShowScrollBar( hwndEdit,SB_HORZ,chk );
	hscroll_flag=chk;
	return chk;
}


int poppad_ini( HWND hwnd, LPARAM lParam )
{
               hInst = ((LPCREATESTRUCT) lParam) -> hInstance ;

               // Initalize script editor vals

			   ConfigEditColor( fgcolor,bgcolor );

               // Create the edit control child window

               hwndEdit = CreateWindowEx ( WS_EX_ACCEPTFILES, "EDIT", NULL,
                         WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                              ES_LEFT | ES_MULTILINE |
                              ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         0, 0, 0, 0,
                         hwnd, (HMENU) EDITID, hInst, NULL) ;

               SendMessage (hwndEdit, EM_LIMITTEXT, 0, 0L) ;
               Org_EditProc = (WNDPROC)GetWindowLong( hwndEdit, GWL_WNDPROC );
               SetWindowLong( hwndEdit, GWL_WNDPROC, (LONG)MyEditProc );
			   DragAcceptFiles( hwndEdit, TRUE );

               // Initialize common dialog box stuff

               PopFileInitialize (hwnd) ;
               PopFontInitialize (hwndEdit) ;

               iMsgFindReplace = RegisterWindowMessage (FINDMSGSTRING) ;

               // Process command line

               //lstrcpy (szFileName, (PSTR) (((LPCREATESTRUCT) lParam)->lpCreateParams)) ;

			   if ( szCmdline[0]==0x22 ) {
					strcpy( szFileName, szCmdline+1 );
			   }
			   else {
				   strcpy( szFileName, szCmdline );
			   }
               DoCaption (szTitleName) ;
			   err_ini();

               poppad_setsb( hscroll_flag );

               if (flg_font==0) {
					PopFontSetColor( fgcolor );
					PopFontSetLG( chg_font );
					PopFontSetFont( hwndEdit );
               }

               return 0 ;
}


void poppad_bye( void )
{
    //case WM_DESTROY :
	err_bye();
	PopFontDeinitialize () ;
}


void poppad_reload( void )
{
	if (strlen (szFileName) > 0)
	{
				    char a1,len;
					len=strlen( szFileName );
					a1=szFileName[len-1];
					if (a1==0x22) szFileName[len-1]=0;

                    GetFileTitle2( szFileName, szTitleName ) ;
					strcpy( szDirName, szFileName );
					len=strlen(szTitleName );
                    szDirName[strlen(szDirName)-len]=0;
                    _chdir(szDirName);
					if (!PopFileRead (hwndEdit, szTitleName)) {
#ifdef JPMSG
                         OkMessage ( "%s が読み込めませんでした",
                                          szTitleName ) ;
#else
                         OkMessage ( "%s not found.",
                                          szTitleName ) ;
#endif
						 return;
					}
		bNeedSave = FALSE ;
    }
	DoCaption (szTitleName) ;
}


LRESULT CALLBACK MyEditProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	//		Subclassed Edit Proc
	//
	char tmpfn[_MAX_PATH];
	HDROP hDrop;
	switch (msg) {
	case WM_DROPFILES:
         if ( bNeedSave==TRUE ) {
			 if ( AskAboutSave (hwbak, szTitleName)==IDCANCEL ) return 0;
		 }

		 hDrop = (HDROP)wParam;
		 DragQueryFile( hDrop, 0, tmpfn, _MAX_PATH );
		 DragFinish( hDrop );
		 strcpy( szFileName, tmpfn );
		 poppad_reload();
		 return 0;						// breakだとWin9xで止まる
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		 PutLineNumber();
		 break;
	}

	return (CallWindowProc( Org_EditProc, hwnd, msg, wParam, lParam ));
}


int poppad_menupop( WPARAM wParam, LPARAM lParam )
{
    int              iSelBeg, iSelEnd, iEnable ;

	//case WM_INITMENUPOPUP :
               switch (lParam)
                    {
                    case 1 :        // Edit menu

                              // Enable Undo if edit control can do it

                         EnableMenuItem ((HMENU) wParam, IDM_UNDO,
                              SendMessage (hwndEdit, EM_CANUNDO, 0, 0L) ?
                                   MF_ENABLED : MF_GRAYED) ;

                              // Enable Paste if text is in the clipboard

                         EnableMenuItem ((HMENU) wParam, IDM_PASTE,
                              IsClipboardFormatAvailable (CF_TEXT) ?
                                   MF_ENABLED : MF_GRAYED) ;

                              // Enable Cut, Copy, and Del if text is selected

                         SendMessage (hwndEdit, EM_GETSEL, (WPARAM) &iSelBeg,
                                                           (LPARAM) &iSelEnd) ;

                         iEnable = iSelBeg != iSelEnd ? MF_ENABLED : MF_GRAYED ;

                         EnableMenuItem ((HMENU) wParam, IDM_CUT,   iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_COPY,  iEnable) ;
                         EnableMenuItem ((HMENU) wParam, IDM_CLEAR, iEnable) ;
                         break ;

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
                        //iEnable = hsp_extmacro ? MF_CHECKED : MF_UNCHECKED ;
						//CheckMenuItem ((HMENU) wParam, IDM_HSPEXTMACRO, iEnable) ;
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
               if (!bNeedSave || IDCANCEL != AskAboutSave (hwbak, szTitleName))
			   {
					reg_save();					// config save to registry
                    SetWindowLong( hwndEdit, GWL_WNDPROC, (LONG)Org_EditProc );
                    DestroyWindow (hwbak) ;
			   }
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
	int ln,ln_s,ln_e;
	SendMessage ( hwndEdit, EM_GETSEL, (WPARAM)&ln_s, (LPARAM)&ln_e );
	ln=SendMessage ( hwndEdit, EM_LINEFROMCHAR, (WPARAM)ln_s, 0 );
	wsprintf (szBuffer, "line : %d", ln+1 ) ;
	Statusbar_mes( szBuffer );
}

							 
LRESULT CALLBACK EditDefProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
     LPFINDREPLACE    pfr ;

     // Process "Find-Replace" iMsgs

    if (iMsg == iMsgFindReplace)
     {
         pfr = (LPFINDREPLACE) lParam ;

         if (pfr->Flags & FR_DIALOGTERM) hDlgModeless = NULL ;

         if (pfr->Flags & FR_FINDNEXT)
            if (!PopFindFindText (hwndEdit, &iOffset, pfr))
#ifdef JPMSG
                      OkMessage ( "終わりまで検索しました", "") ;
#else
                      OkMessage ( "Not found.", "") ;
#endif

         if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL)
            if (!PopFindReplaceText (hwndEdit, &iOffset, pfr))
#ifdef JPMSG
                      OkMessage ( "終わりまで置換しました", "") ;
#else
                      OkMessage ( "Replace finished.", "") ;
#endif

         if (pfr->Flags & FR_REPLACEALL)
				while (PopFindReplaceText (hwndEdit, &iOffset, pfr) );

       return 0 ;
     }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}



LRESULT CALLBACK EditProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
     {
     int a;
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
                            // Messages from edit control

               if (lParam && LOWORD (wParam) == EDITID)
                    {
                    switch (HIWORD (wParam))
                         {
                         case EN_UPDATE :
							  PutLineNumber();
                              bNeedSave = TRUE ;
                              return 0 ;

                         case EN_ERRSPACE :
                         case EN_MAXTEXT :
#ifdef JPMSG
                              MessageBox (hwnd, "テキストの容量がオーバーしました",
                                        "script editor warning", MB_OK | MB_ICONSTOP) ;
#else
                              MessageBox (hwnd, "Editor memory size overflow.",
                                        "script editor warning", MB_OK | MB_ICONSTOP) ;
#endif
                              return 0 ;
                         }
                    break ;
                    }

               switch (LOWORD (wParam))
                    {
                              // Messages from File menu

                    case IDM_NEW :
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szTitleName))
                              return 0 ;

                         SetWindowText (hwndEdit, "\0") ;
                         szFileName[0]  = '\0' ;
                         szTitleName[0] = '\0' ;
                         DoCaption (szTitleName) ;
                         bNeedSave = FALSE ;
                         return 0 ;

                    case IDM_OPEN :
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szTitleName))
                              return 0 ;

						 strcpy( tmpfn, szFileName );
                         //szFileName[0]  = '\0' ;
                         if (PopFileOpenDlg (hwnd, szFileName, szTitleName))
                              {
                              if (!PopFileRead (hwndEdit, szFileName))
                                   {
#ifdef JPMSG
                                   OkMessage ( "%s をロードできませんでした。",
                                                    szTitleName) ;
#else
                                   OkMessage ( "Loading %s fault.",
                                                    szTitleName) ;
#endif
                                   //szFileName[0]  = '\0' ;
                                   //szTitleName[0] = '\0' ;
								   strcpy( szFileName, tmpfn );
								   break;
                                   }
							  //_chdir(szStartDir);
	                          DoCaption (szTitleName) ;
	                          bNeedSave = FALSE ;
                              }

                         return 0 ;

                    case IDM_SAVE :
                         if (szFileName[0])
                              {
                              if (PopFileWrite (hwndEdit, szFileName))
                                   {
                                   bNeedSave = FALSE ;
                                   return 1 ;
                                   }
                              else {
#ifdef JPMSG
									OkMessage ( "セーブに失敗しました。\n[%s]",
                                      szFileName ) ;
#else
									OkMessage ( "Error happened in saving.\n[%s]",
                                      szFileName ) ;
#endif
								}
                              return 0 ;
                              }
                                                  // fall through
                    case IDM_SAVEAS :
                         if (PopFileSaveDlg (hwnd, szFileName, szTitleName))
                              {
                              DoCaption (szTitleName) ;

                              if (PopFileWrite (hwndEdit, szFileName))
                                   {
                                   bNeedSave = FALSE ;
                                   return 1 ;
                                   }
                              else
#ifdef JPMSG
                                   OkMessage ( "%s のセーブに失敗しました。",
                                                    szTitleName) ;
#else
								   OkMessage ( "Error happened in saving.\n[%s]",
													szFileName ) ;
#endif
                              }
                         return 0 ;

                    case IDM_PRINT :
                         if (!PopPrntPrintFile (hInst, hwnd, hwndEdit,
                                                szTitleName))
#ifdef JPMSG
                              OkMessage ( "%s をプリントアウトできません。",
                                        szTitleName) ;
#else
							   OkMessage ( "Error happened in printing.\n[%s]",
										szFileName ) ;
#endif
                         return 0 ;

                    case IDM_EXIT :
                         SendMessage (hwnd, WM_CLOSE, 0, 0) ;
                         return 0 ;

                              // Messages from Edit menu

                    case IDM_UNDO :
                         SendMessage (hwndEdit, WM_UNDO, 0, 0) ;
                         return 0 ;

                    case IDM_CUT :
                         SendMessage (hwndEdit, WM_CUT, 0, 0) ;
                         return 0 ;

                    case IDM_COPY :
                         SendMessage (hwndEdit, WM_COPY, 0, 0) ;
                         return 0 ;

                    case IDM_PASTE :
                         SendMessage (hwndEdit, WM_PASTE, 0, 0) ;
                         return 0 ;

                    case IDM_CLEAR :
                         SendMessage (hwndEdit, WM_CLEAR, 0, 0) ;
                         return 0 ;

                    case IDM_SELALL :
                         SendMessage (hwndEdit, EM_SETSEL, 0, -1) ;
                         return 0 ;

                              // Messages from Search menu

                    case IDM_FIND :
						if ( hDlgModeless == NULL ) {
							 iOffset = GetCursorOffset();
	                         hDlgModeless = PopFindFindDlg (hwnd) ;
						}
                        return 0 ;

                    case IDM_NEXT :
						 iOffset = GetCursorOffset();
                         if (PopFindValidFind ())
                              PopFindNextText (hwndEdit, &iOffset) ;
                         else
                              hDlgModeless = PopFindFindDlg (hwnd) ;

                         return 0 ;

                    case IDM_REPLACE :
						if ( hDlgModeless == NULL ) {
							iOffset = GetCursorOffset();
	                        hDlgModeless = PopFindReplaceDlg (hwnd) ;
						}
                        return 0 ;

                    case IDM_FONT :
                         if (PopFontChooseFont (hwnd)) {
							  fgcolor=(COLORREF)PopFontGetColor();
							  ConfigEditColor( fgcolor, bgcolor );
                              PopFontSetFont (hwndEdit);
						 }
                         return 0 ;

                    case IDM_BGCOLOR :
                         a=PopFontChooseColor(hwnd);
						 if (a!=-1) {
							  bgcolor=(COLORREF)a;
							  ConfigEditColor( fgcolor, bgcolor );
                              PopFontSetFont (hwndEdit);
						 }
                         return 0 ;

					case IDM_STARTDIR:
						DialogBox (hInst, "StartDir", hwnd, (DLGPROC)StartDirDlgProc);
						return 0;

					//	Message from cursor menu

					case IDM_JUMP:
						DialogBox (hInst, "JumpBox", hwnd, (DLGPROC)JumpDlgProc);
						if (cln==-1) return 0;
						cln = (int) SendMessage( hwndEdit, EM_LINEINDEX, cln, 0L );
						SendMessage( hwndEdit, EM_SETSEL, cln, cln );
						SendMessage( hwndEdit, EM_SCROLLCARET, 0, 0 );
						PutLineNumber();
						return 0;

					case IDM_LBTM:
						cln = (int) SendMessage( hwndEdit, EM_GETLINECOUNT, 0, 0L );
						cln = (int) SendMessage( hwndEdit, EM_LINEINDEX, cln-1, 0L );
						SendMessage( hwndEdit, EM_SETSEL, cln,cln );
						SendMessage( hwndEdit, EM_SCROLLCARET, 0, 0 );
						PutLineNumber();
						return 0;

					case IDM_LTOP:
						SendMessage( hwndEdit, EM_SETSEL, 0, 0 );
						SendMessage( hwndEdit, EM_SCROLLCARET, 0, 0 );
						PutLineNumber();
						return 0;

					case IDM_LABEL:
						DialogBox (hInst, "LabelBox", hwnd, (DLGPROC)LabelDlgProc);
						if (cln==-1) return 0;
						cln = (int) SendMessage( hwndEdit, EM_LINEINDEX, cln, 0L );
						SendMessage( hwndEdit, EM_SETSEL, cln, cln );
						SendMessage( hwndEdit, EM_SCROLLCARET, 0, 0 );
						PutLineNumber();
						return 0;

						// Messages for HSP

					case IDM_MKOBJ:
						if (szFileName[0]!=0) {
                         PopFileWrite (hwndEdit, szFileName);
                         bNeedSave = FALSE ;
						 if ( mkobjfile( szFileName ) )
								{ err_prt(hwnd);return 0; }
#ifdef JPMSG
						 TMes("オブジェクトファイルを作成しました");
#else
						 TMes("Object file generated.");
#endif
						}
						return 0;

					case IDM_MKOBJ2:
						PopFileWrite ( hwndEdit, "hsptmp" );
						if ( mkobjfile2( "hsptmp" ) )
								{ err_prt(hwnd);return 0; }
#ifdef JPMSG
						TMes("START.AXを作成しました");
#else
						 TMes("START.AX generated.");
#endif
						return 0;

					case IDM_AUTOMAKE:
						PopFileWrite ( hwndEdit, "hsptmp" );
						if ( mkexefile2( "hsptmp" ) )
								{ err_prt(hwnd);return 0; }
#ifdef JPMSG
						TMes("実行ファイルを作成しました");
#else
						 TMes("Executable file generated.");
#endif
						return 0;

					case IDM_COMP:
					case IDM_COMP2:
					case IDM_LOGCOMP:
						PopFileWrite ( hwndEdit, "hsptmp" );
						strcpy(tmpfn,"hsptmp");
						hsc_ini( 0,(int)tmpfn, 0,0 );
						myfile();
						hsc_refname( 0,(int)compfile, 0,0 );
						strcpy( objname,"obj" );
						hsc_objname( 0,(int)objname, 0,0 );
						a=hsc_comp( 1,hsp_extmacro^1,hsp_debug,0 );
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
							a=hsc_comp( 0,hsp_extmacro^1,0,0 );
							//a=tcomp_main( hsp_extstr, hsp_extstr, objname, errbuf,0 );
							if (a) { err_prt(hwnd);return 0; }
#ifdef JPMSG
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
						a=hsc_comp( 1,hsp_extmacro^1,hsp_debug,0 );
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
						a=hsc3_getsym( hsp_extmacro^1,0,0,0 );
						if (a) {
#ifdef JPMSG
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

//					case IDM_HSPEXTMACRO:
//						hsp_extmacro^=1;
//						return 0;

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
						//wsprintf( tmpfn,"%s\\hsperun", szExeDir );
						wsprintf( tmpfn,"\"%s\\hspat.exe\"", szExeDir );
						WinExec( tmpfn, SW_SHOW );
						return 0;

					case IDM_OPEN_SRCFOLDER:
						 _getcwd( tmpfn, _MAX_PATH );
						ShellExecute( NULL,"explore", tmpfn, NULL, NULL, SW_SHOWNORMAL );
						return 0;

					// Messages from Help menu

					case IDM_HELPDIR:
						DialogBox (hInst, "HelpDir", hwnd, (DLGPROC)DirDlgProc);
						return 0;

					case IDM_KWHELP:
						 getkw();gethdir();
						 callhelp();
						 return 0;

                    case IDM_HSPMAN1 :
						wsprintf( helpopt,"%s\\docs", szExeDir );
						ShellExecute( NULL,"open","hspprog.htm", "", helpopt, SW_SHOW );
						//fileexe("start","hspprog.htm");
						return 0 ;

                    case IDM_HSPMAN2 :
						wsprintf( tmpfn,"\"%s\\hsphelp\\helpman.exe\"", szExeDir );
						WinExec( tmpfn, SW_SHOW );
						//wsprintf( helpopt,"%s\\hsphelp", szExeDir );
						//ShellExecute( NULL,"open","hspref.htm", "", helpopt, SW_SHOW );
						//fileexe("start","hspref.htm");
						return 0 ;

                    case IDM_HSPMAN3 :
						wsprintf( helpopt,"%s", szExeDir );
						ShellExecute( NULL,"open","index.htm", "", helpopt, SW_SHOW );
						//fileexe("start","hsppidx.htm");
						return 0 ;

                    case IDM_ABOUT :
                         DialogBox (hInst, "AboutBox", hwnd, (DLGPROC)AboutDlgProc) ;
                         return 0 ;
                    }
               break ;

          }
     return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
     }

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
     {
     switch (iMsg)
          {
          case WM_INITDIALOG :
               return TRUE ;

          case WM_COMMAND :
               switch (LOWORD (wParam))
                    {
                    case IDOK :
                         EndDialog (hDlg, 0) ;
                         return TRUE ;
                    }
               break ;
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
						ShellExecute( NULL,"open","hsplog.txt", "", "", SW_SHOW );
						break;
					}
               break ;
          }
     return FALSE ;
     }


BOOL CALLBACK StartDirDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
     switch (message)
          {
          case WM_INITDIALOG:
				SetDlgItemText( hDlg,IDC_EDIT1,startdir );
				SetFocus( GetDlgItem(hDlg,IDC_EDIT1) );
				CheckDlgButton( hDlg, IDC_RADIO1+startflag, BST_CHECKED );
				return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDC_BUTTON1:
						 if ( selfolder( startdir )==0 )
							 SetDlgItemText( hDlg,IDC_EDIT1,startdir );
                         return TRUE;
                    case IDOK:
						 startflag = CheckRadio( hDlg,IDC_RADIO1,4 );
						 GetDlgItemText( hDlg,IDC_EDIT1,startdir,128 );
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


