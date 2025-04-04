/*------------------------------------------------------------------------------
	Editor configration routines for HSP Script Editor
------------------------------------------------------------------------------*/
#include <windows.h>
#include <string.h>

#include "hsed_config.h"
#include "poppad.h"
#include "config.h"
#include "classify.h"
#include "support.h"
#include "exttool.h"

// Global variables
static char szIniFileName[_MAX_PATH + 1];

// Variable changed by this program
extern char	    hsp_cmdopt[TMPSIZE];
extern char	    hsp_laststr[TMPSIZE];
extern char	    hsp_helpdir[TMPSIZE];
extern int      hscroll_flag;
extern int      hsp_wx;
extern int      hsp_wy;
extern int      hsp_wd;
extern int      hsp_orgpath;
extern int	    hsp_debug;
extern int	    hsp_extobj;
extern char	    hsp_extstr[TMPSIZE];
extern int	    hsp_helpmode;
extern int	    hsp_logmode;
extern int	    hsp_logadd;
extern int      hsp_extmacro;
extern int      hsp_clmode;
extern int	    startflag;
extern char     startdir[_MAX_PATH];
extern LOGFONT  chg_font;
extern int      flg_statbar;
extern int      flg_toolbar;
extern char     hdir[_MAX_PATH];
extern char     helpopt[TMPSIZE];
extern int      hsp_fullscr;
extern int      posx;
extern int      posy;
extern int      winflg;
extern int      winx;
extern int      winy;
extern MYCOLOR  color;
extern LOGFONT	tchg_font;
extern int      tabEnabled;
extern int      hsEnabled;
extern int      fsEnabled;
extern int      nlEnabled;
extern int      eofEnabled;
extern int      ulEnabled;
extern int      tabsize;
extern int      rulerheight;
extern int      linewidth;
extern int      linespace;
extern FileList filelist;
extern int      ColumnWidth[2];
extern bool	    bUseIni;
extern BOOL     bAutoIndent;

static int      hsed_ver;
static int      hsed_private_ver;

// Registry I/O routines
static int reg_getkey( HKEY, char *, int * );
static int reg_sgetkey( HKEY, char *, char *, unsigned long );
static int reg_bgetkey( HKEY, char *, BYTE *, int );
static void reg_setkey( HKEY, char *, int );
static void reg_ssetkey( HKEY, char *, char * );
static void reg_bsetkey( HKEY, char *, BYTE *, int );
static void reg_save( void );
static void reg_load( void );

// INI file I/O routines
static inline void ini_getkey( const char *, const char *, char *, int * );
static inline DWORD ini_sgetkey( const char *, const char *,char *, char *, int );
static int ini_bgetkey( const char *, const char *, char *, BYTE *, int );
static void ini_setkey( const char *, const char *, char *, int );
static inline void ini_ssetkey( const char *, const char *, char *, char * );
static void ini_bsetkey( const char *, const char *, char *, BYTE *, int );
static void ini_save( void );
static void ini_load( void );

// Functions in main.cpp
int CheckWndFlag ( void );

// Global variable in main.cpp
extern char szExeDir[_MAX_PATH];

// Functions in poppad.cpp
int fileok( char *fname );
void gethdir( void );

// Functions in popfont.cpp
LOGFONT PopFontGetELG ( void );
LOGFONT PopFontGetTLG( void );

void LoadConfig()
{
	HKEY hKey;
	int nTemp = 0;

	char szFileName[_MAX_PATH + 1];
	wsprintf(szFileName, "%s\\%s", szExeDir, INI_FILE);

	bUseIni = false;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX, 0, KEY_READ, &hKey) == ERROR_SUCCESS){
		reg_getkey(hKey, "useini", &nTemp);
		if(nTemp)
			bUseIni = true;
		RegCloseKey(hKey);
	}
	else if(!fileok(szFileName))
		bUseIni = true;

	if(bUseIni)
		ini_load();
	else
		reg_load();
	return;
}

void SaveConfig()
{
	HKEY hKey;

	if(bUseIni){
		ini_save();
		if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS){
			reg_setkey(hKey, "useini", TRUE);
			RegCloseKey(hKey);
		}
	}
	else
		reg_save();
	return;
}

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

static int reg_sgetkey( HKEY hKey, char *readkey, char *strbuf, unsigned long RVSize )
{
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

static DWORD reg_getsize( HKEY hKey, char *readkey )
{
	DWORD dwType = REG_BINARY, nSize;

	RegQueryValueEx(hKey, readkey, NULL, &dwType, NULL, &nSize);
	return nSize;
}

/* DEFCOLOR_*は、classify.hにあります。 */
void DefaultColor(MYCOLOR *dest)
{
	ZeroMemory(dest, sizeof(MYCOLOR));
	dest->Character.Default.Conf       = DEFCOLOR_FONT;
	dest->Character.String.Conf        = DEFCOLOR_STRING;
	dest->Character.Function.Conf      = DEFCOLOR_FUNC;
	dest->Character.Preprocessor.Conf  = DEFCOLOR_PREPROCESSOR;
	dest->Character.Macro.Conf         = DEFCOLOR_MACRO;
	dest->Character.Comment.Conf       = DEFCOLOR_COMMENT;
	dest->Character.Label.Conf         = DEFCOLOR_LABEL;
	dest->NonCharacter.HalfSpace.Conf  = DEFCOLOR_HALF_SPACE;
	dest->NonCharacter.FullSpace.Conf  = DEFCOLOR_FULL_SPACE;
	dest->NonCharacter.Tab.Conf        = DEFCOLOR_TAB;
	dest->NonCharacter.NewLine.Conf    = DEFCOLOR_NEWLINE;
	dest->NonCharacter.EndOfFile.Conf  = DEFCOLOR_EOF;
	dest->Edit.Background.Conf         = DEFCOLOR_BACKGROUND;
	dest->Edit.CaretUnderLine.Conf     = DEFCOLOR_CARET_UNDERLINE;
	dest->Edit.BoundaryLineNumber.Conf = DEFCOLOR_BOUNDARY_LINENUM;
	dest->LineNumber.Number.Conf       = DEFCOLOR_LINENUM;
	dest->LineNumber.CaretLine.Conf    = DEFCOLOR_CARET_LINE;
	dest->Ruler.Number.Conf            = DEFCOLOR_RULER_FONT;
	dest->Ruler.Background.Conf        = DEFCOLOR_RULER_BACKGROUND;
	dest->Ruler.Division.Conf          = DEFCOLOR_RULER_DIVISION;
	dest->Ruler.Caret.Conf             = DEFCOLOR_RULER_CARET;
}

void DefaultFont(LOGFONT *editfont, LOGFONT *tabfont)
{
	GetObject (GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), (PSTR)editfont);
	
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	*tabfont = ncm.lfMenuFont;	
}

static void DefaultExtToolInfo(EXTTOOLINFO *lpExtToolInfo)
{
	lstrcpy(lpExtToolInfo->ToolName, "");
	lstrcpy(lpExtToolInfo->FileName, "");
	lstrcpy(lpExtToolInfo->CmdLine, "");
	lstrcpy(lpExtToolInfo->WorkDir, "");
	lpExtToolInfo->ShowOnMainMenu    = true;
	lpExtToolInfo->ShowOnPopupMenu   = false;
	lpExtToolInfo->ExecOnStartup     = false;
	lpExtToolInfo->ExecWithOverwrite = false;
	return;
}

static void for_obsolete( HKEY hKey )
{
	MYCOLORREF crObsoleteColor[19];
	int nSize;

	nSize = reg_getsize(hKey, "color");
	if(nSize == 19 * sizeof(MYCOLORREF)){ // - 1.02b3
		reg_bgetkey(hKey, "color", (BYTE *)crObsoleteColor, sizeof(crObsoleteColor));
		CopyMemory(&color, crObsoleteColor, sizeof(MYCOLORREF) * 2);
		CopyMemory(((MYCOLORREF *)&color) + 4, crObsoleteColor + 2, sizeof(MYCOLORREF) * 17);        
	} else if(nSize == 21 * sizeof(MYCOLORREF)){ // 1.02b4
		reg_bgetkey(hKey, "color", (BYTE *)&color, sizeof(MYCOLOR));
	}
}

static void set_default( void )
{
	//		デフォルトの設定を行なう(onitama追加:050218)
	//
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
	hsed_private_ver = 0;
	startflag = STARTDIR_MYDOC;
	startdir[0] = 0;

	tabEnabled = FALSE;
	hsEnabled  = FALSE;
	fsEnabled  = FALSE;
	nlEnabled  = FALSE;
	eofEnabled = TRUE;
	ulEnabled  = TRUE;
	tabsize = 4;
	rulerheight = 10;
	linewidth = 50;
	linespace = 0;

	bAutoIndent = TRUE;

	ColumnWidth[0] = 120;
	ColumnWidth[1] = 280;

	DefaultColor(&color);
	DefaultFont(&chg_font, &tchg_font);
}


static void reg_save( void )
{
	//		レジストリに書き込む
	//
	HKEY	hKey;
	int		RVSize = 4, nSize, i;
	DWORD	dw = REG_DWORD;
	DWORD	sz = REG_SZ;
	EXTTOOLINFO *lpExtToolInfo;
	char szKeyName[256];

	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX, 0, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	reg_setkey( hKey,"fullscr", hsp_fullscr );
	reg_setkey( hKey,"hscroll", hscroll_flag );
	reg_ssetkey( hKey,"cmdopt", hsp_cmdopt );
	reg_ssetkey( hKey,"laststr", hsp_laststr );
	chg_font=PopFontGetELG();
	reg_bsetkey(hKey,"font", (BYTE *)&chg_font, sizeof(LOGFONT) );
	tchg_font=PopFontGetTLG();
	reg_bsetkey( hKey,"tabfont", (BYTE *)&tchg_font, sizeof(LOGFONT) );

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
	reg_setkey( hKey,"hsedprivatever", hsed_private_ver );
	reg_setkey( hKey,"extmacro", hsp_extmacro );
	reg_setkey( hKey,"clmode", hsp_clmode );
	reg_setkey( hKey,"orgpath", hsp_orgpath );

	reg_setkey( hKey,"startmode", startflag );
	reg_ssetkey( hKey,"startdir", startdir );
	
	reg_setkey( hKey,"tabenabled",tabEnabled );
	reg_setkey( hKey,"hsenabled", hsEnabled );
	reg_setkey( hKey,"fsenabled", fsEnabled );
	reg_setkey( hKey,"nlenabled", nlEnabled );
	reg_setkey( hKey,"eofenabled", eofEnabled );
	reg_setkey( hKey,"underlineenabled", ulEnabled );
	reg_setkey( hKey,"tabsize", tabsize );
	reg_setkey( hKey,"rulerheight", rulerheight );
	reg_setkey( hKey,"linewidth", linewidth );
	reg_setkey( hKey,"linespace", linespace );

	reg_setkey( hKey,"column1width", ColumnWidth[0] );
	reg_setkey( hKey,"column2width", ColumnWidth[1] );

	reg_ssetkey( hKey,"keyfile", (char *)filelist.getlist() );
	reg_setkey( hKey,"useini", FALSE );

	reg_setkey( hKey, "autoindent", bAutoIndent );

	RegCloseKey(hKey);

	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\Color", 0, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	reg_setkey( hKey, "Default", color.Character.Default.Conf );
	reg_setkey( hKey, "String", color.Character.String.Conf );
	reg_setkey( hKey, "Function", color.Character.Function.Conf );
	reg_setkey( hKey, "PreProcessor", color.Character.Preprocessor.Conf );
	reg_setkey( hKey, "Macro", color.Character.Macro.Conf );
	reg_setkey( hKey, "Comment", color.Character.Comment.Conf );
	reg_setkey( hKey, "Label", color.Character.Label.Conf );
	reg_setkey( hKey, "HalfSpace", color.NonCharacter.HalfSpace.Conf );
	reg_setkey( hKey, "FullSpace", color.NonCharacter.FullSpace.Conf );
	reg_setkey( hKey, "Tab", color.NonCharacter.Tab.Conf );
	reg_setkey( hKey, "NewLine", color.NonCharacter.NewLine.Conf );
	reg_setkey( hKey, "EOF", color.NonCharacter.EndOfFile.Conf );
	reg_setkey( hKey, "BackGround", color.Edit.Background.Conf );
	reg_setkey( hKey, "UnderLine", color.Edit.CaretUnderLine.Conf );
	reg_setkey( hKey, "BoundaryLineNum", color.Edit.BoundaryLineNumber.Conf );
	reg_setkey( hKey, "LineNum", color.LineNumber.Number.Conf );
	reg_setkey( hKey, "LineNumCaret", color.LineNumber.CaretLine.Conf );
	reg_setkey( hKey, "RulerNum", color.Ruler.Number.Conf );
	reg_setkey( hKey, "RulerBackGround", color.Ruler.Background.Conf );
	reg_setkey( hKey, "RulerDivision", color.Ruler.Division.Conf );
	reg_setkey( hKey, "RulerCaret", color.Ruler.Caret.Conf );
	RegCloseKey(hKey);

	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\UserColor", 0, "", 
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	reg_setkey( hKey, "Default", color.Character.Default.Combo );
	reg_setkey( hKey, "String", color.Character.String.Combo );
	reg_setkey( hKey, "Function", color.Character.Function.Combo );
	reg_setkey( hKey, "PreProcessor", color.Character.Preprocessor.Combo );
	reg_setkey( hKey, "Macro", color.Character.Macro.Combo );
	reg_setkey( hKey, "Comment", color.Character.Comment.Combo );
	reg_setkey( hKey, "Label", color.Character.Label.Combo );
	reg_setkey( hKey, "HalfSpace", color.NonCharacter.HalfSpace.Combo );
	reg_setkey( hKey, "FullSpace", color.NonCharacter.FullSpace.Combo );
	reg_setkey( hKey, "Tab", color.NonCharacter.Tab.Combo );
	reg_setkey( hKey, "NewLine", color.NonCharacter.NewLine.Combo );
	reg_setkey( hKey, "EOF", color.NonCharacter.EndOfFile.Combo );
	reg_setkey( hKey, "BackGround", color.Edit.Background.Combo );
	reg_setkey( hKey, "UnderLine", color.Edit.CaretUnderLine.Combo );
	reg_setkey( hKey, "BoundaryLineNum", color.Edit.BoundaryLineNumber.Combo );
	reg_setkey( hKey, "LineNum", color.LineNumber.Number.Combo );
	reg_setkey( hKey, "LineNumCaret", color.LineNumber.CaretLine.Combo );
	reg_setkey( hKey, "RulerNum", color.Ruler.Number.Combo );
	reg_setkey( hKey, "RulerBackGround", color.Ruler.Background.Combo );
	reg_setkey( hKey, "RulerDivision", color.Ruler.Division.Combo );
	reg_setkey( hKey, "RulerCaret", color.Ruler.Caret.Combo );
	RegCloseKey(hKey);

	RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\ExtTools", 0, "",
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	nSize = GetExtToolSize();
	reg_setkey( hKey, "numoftools", nSize );
	for(i = 1; i <= nSize; i++){
		lpExtToolInfo = GetExtTool(i - 1);
		if(lpExtToolInfo->Used){
			wsprintf(szKeyName, "toolname%d", i);
			reg_ssetkey( hKey, szKeyName, lpExtToolInfo->ToolName);
			wsprintf(szKeyName, "filename%d", i);
			reg_ssetkey( hKey, szKeyName, lpExtToolInfo->FileName);
			wsprintf(szKeyName, "cmdline%d", i);
			reg_ssetkey( hKey, szKeyName, lpExtToolInfo->CmdLine);
			wsprintf(szKeyName, "workdir%d", i);
			reg_ssetkey( hKey, szKeyName, lpExtToolInfo->WorkDir);
			wsprintf(szKeyName, "mainmenu%d", i);
			reg_setkey( hKey, szKeyName, lpExtToolInfo->ShowOnMainMenu);
			wsprintf(szKeyName, "popupmenu%d", i);
			reg_setkey( hKey, szKeyName, lpExtToolInfo->ShowOnPopupMenu);
			wsprintf(szKeyName, "startup%d", i);
			reg_setkey( hKey, szKeyName, lpExtToolInfo->ExecOnStartup);
			wsprintf(szKeyName, "overwrite%d", i);
			reg_setkey( hKey, szKeyName, lpExtToolInfo->ExecWithOverwrite);
		} else {
			wsprintf(szKeyName, "toolname%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "filename%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "workdir%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "mainmenu%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "popupmenu%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "startup%d", i);
			RegDeleteValue( hKey, szKeyName );
			wsprintf(szKeyName, "overwrite%d", i);
			RegDeleteValue( hKey, szKeyName );
		}
	}


	RegCloseKey(hKey);
}

static void reg_load( void )
{
	//		レジストリから読み込む
	//
	HKEY	hKey;
	char    szKeyFile[32000] = "", szKeyName[1280];
	EXTTOOLINFO ExtToolInfo;
	int     nTemp, nSize, i;

	//		Reset to default value

	set_default();

	//		Read keys

	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX, 0,
		KEY_READ, &hKey) == ERROR_SUCCESS) {
			reg_getkey( hKey,"fullscr", &hsp_fullscr );
			reg_getkey( hKey,"hscroll", &hscroll_flag );
			reg_sgetkey( hKey,"cmdopt", hsp_cmdopt, sizeof(hsp_cmdopt) );
			reg_sgetkey( hKey,"laststr", hsp_laststr, sizeof(hsp_laststr) );
			reg_bgetkey( hKey,"font", (BYTE *)&chg_font, sizeof(LOGFONT) );
			reg_bgetkey( hKey,"tabfont", (BYTE *)&tchg_font, sizeof(LOGFONT) );
			reg_getkey( hKey,"winflg", &winflg );
			reg_getkey( hKey,"winx", &winx );
			reg_getkey( hKey,"winy", &winy );
			reg_getkey( hKey,"posx", &posx );
			reg_getkey( hKey,"posy", &posy );
			reg_getkey( hKey,"toolbar", &flg_toolbar );
			reg_getkey( hKey,"statbar", &flg_statbar );
			reg_sgetkey( hKey,"helpdir", hsp_helpdir, sizeof(hsp_helpdir) );
			reg_getkey( hKey,"exewx", &hsp_wx );
			reg_getkey( hKey,"exewy", &hsp_wy );
			reg_getkey( hKey,"exewd", &hsp_wd );
			reg_getkey( hKey,"debug", &hsp_debug );
			reg_sgetkey( hKey,"extstr", hsp_extstr, sizeof(hsp_extstr) );
			reg_getkey( hKey,"extobj", &hsp_extobj );
			reg_getkey( hKey,"helpmode", &hsp_helpmode );
			reg_getkey( hKey,"logmode", &hsp_logmode );
			reg_getkey( hKey,"logadd", &hsp_logadd );
			reg_getkey( hKey,"extmacro", &hsp_extmacro );
			reg_getkey( hKey,"clmode", &hsp_clmode );
			reg_getkey( hKey,"orgpath", &hsp_orgpath );

			reg_getkey( hKey,"startmode", &startflag );
			reg_sgetkey( hKey,"startdir", startdir, sizeof(startdir) );
			reg_getkey( hKey,"tabenabled",&tabEnabled );
			reg_getkey( hKey,"hsenabled", &hsEnabled );
			reg_getkey( hKey,"fsenabled", &fsEnabled );
			reg_getkey( hKey,"nlenabled", &nlEnabled );
			reg_getkey( hKey,"eofenabled", &eofEnabled );
			reg_getkey( hKey,"underlineenabled", &ulEnabled );
			reg_getkey( hKey,"tabsize", &tabsize );
			reg_getkey( hKey,"rulerheight", &rulerheight );
			reg_getkey( hKey,"linewidth", &linewidth );
			reg_getkey( hKey,"linespace", &linespace );

			reg_getkey( hKey,"autoindent", &bAutoIndent );
			
			reg_getkey( hKey,"column1width", &ColumnWidth[0] );
			reg_getkey( hKey,"column2width", &ColumnWidth[1] );

			for_obsolete( hKey );

			reg_sgetkey( hKey, "keyfile", szKeyFile, sizeof(szKeyFile) );
			filelist.setlist(szKeyFile);

			if ( reg_getkey( hKey,"hsedver", &hsed_ver ) ) {
				hsp_helpmode=2;
			}
			reg_getkey( hKey,"hsedprivatever", &hsed_private_ver );

			RegCloseKey(hKey);
	}

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\Color",
		0, KEY_READ, &hKey) == ERROR_SUCCESS){
			reg_getkey( hKey, "Default", (int *)&(color.Character.Default.Conf) );
			reg_getkey( hKey, "String", (int *)&(color.Character.String.Conf) );
			reg_getkey( hKey, "Function", (int *)&(color.Character.Function.Conf) );
			reg_getkey( hKey, "PreProcessor", (int *)&(color.Character.Preprocessor.Conf) );
			reg_getkey( hKey, "Macro", (int *)&(color.Character.Macro.Conf) );
			reg_getkey( hKey, "Comment", (int *)&(color.Character.Comment.Conf) );
			reg_getkey( hKey, "Label", (int *)&(color.Character.Label.Conf) );
			reg_getkey( hKey, "HalfSpace", (int *)&(color.NonCharacter.HalfSpace.Conf) );
			reg_getkey( hKey, "FullSpace", (int *)&(color.NonCharacter.FullSpace.Conf) );
			reg_getkey( hKey, "Tab", (int *)&(color.NonCharacter.Tab.Conf) );
			reg_getkey( hKey, "NewLine", (int *)&(color.NonCharacter.NewLine.Conf) );
			reg_getkey( hKey, "EOF", (int *)&(color.NonCharacter.EndOfFile.Conf) );
			reg_getkey( hKey, "BackGround", (int *)&(color.Edit.Background.Conf) );
			reg_getkey( hKey, "UnderLine", (int *)&(color.Edit.CaretUnderLine.Conf) );
			reg_getkey( hKey, "BoundaryLineNum", (int *)&(color.Edit.BoundaryLineNumber.Conf) );
			reg_getkey( hKey, "LineNum", (int *)&(color.LineNumber.Number.Conf) );
			reg_getkey( hKey, "LineNumCaret", (int *)&(color.LineNumber.CaretLine.Conf) );
			reg_getkey( hKey, "RulerNum", (int *)&(color.Ruler.Number.Conf) );
			reg_getkey( hKey, "RulerBackGround", (int *)&(color.Ruler.Background.Conf) );
			reg_getkey( hKey, "RulerDivision", (int *)&(color.Ruler.Division.Conf) );
			reg_getkey( hKey, "RulerCaret", (int *)&(color.Ruler.Caret.Conf) );

			RegCloseKey(hKey);
	}

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\UserColor", 
		0, KEY_READ, &hKey) == ERROR_SUCCESS){
			reg_getkey( hKey, "Default", (int *)&(color.Character.Default.Combo) );
			reg_getkey( hKey, "String", (int *)&(color.Character.String.Combo) );
			reg_getkey( hKey, "Function", (int *)&(color.Character.Function.Combo) );
			reg_getkey( hKey, "PreProcessor", (int *)&(color.Character.Preprocessor.Combo) );
			reg_getkey( hKey, "Macro", (int *)&(color.Character.Macro.Combo) );
			reg_getkey( hKey, "Comment", (int *)&(color.Character.Comment.Combo) );
			reg_getkey( hKey, "Label", (int *)&(color.Character.Label.Combo) );
			reg_getkey( hKey, "HalfSpace", (int *)&(color.NonCharacter.HalfSpace.Combo) );
			reg_getkey( hKey, "FullSpace", (int *)&(color.NonCharacter.FullSpace.Combo) );
			reg_getkey( hKey, "Tab", (int *)&(color.NonCharacter.Tab.Combo) );
			reg_getkey( hKey, "NewLine", (int *)&(color.NonCharacter.NewLine.Combo) );
			reg_getkey( hKey, "EOF", (int *)&(color.NonCharacter.EndOfFile.Combo) );
			reg_getkey( hKey, "BackGround", (int *)&(color.Edit.Background.Combo) );
			reg_getkey( hKey, "UnderLine", (int *)&(color.Edit.CaretUnderLine.Combo) );
			reg_getkey( hKey, "BoundaryLineNum", (int *)&(color.Edit.BoundaryLineNumber.Combo) );
			reg_getkey( hKey, "LineNum", (int *)&(color.LineNumber.Number.Combo) );
			reg_getkey( hKey, "LineNumCaret", (int *)&(color.LineNumber.CaretLine.Combo) );
			reg_getkey( hKey, "RulerNum", (int *)&(color.Ruler.Number.Combo) );
			reg_getkey( hKey, "RulerBackGround", (int *)&(color.Ruler.Background.Combo) );
			reg_getkey( hKey, "RulerDivision", (int *)&(color.Ruler.Division.Combo) );
			reg_getkey( hKey, "RulerCaret", (int *)&(color.Ruler.Caret.Combo) );

			RegCloseKey(hKey);
	}

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\OnionSoftware\\hsed" REG_SUFFIX "\\ExtTools",
		0, KEY_READ, &hKey) == ERROR_SUCCESS){
			nSize = 0;
			reg_getkey( hKey, "numoftools", &nSize );
			InitExtTool(nSize);
			for(i = 1; i <= nSize; i++){
				DefaultExtToolInfo(&ExtToolInfo);

				wsprintf(szKeyName, "filename%d", i);
				if(reg_sgetkey( hKey, szKeyName, ExtToolInfo.FileName, sizeof(ExtToolInfo.FileName) ) == -1) continue;

				wsprintf(szKeyName, "toolname%d", i);
				reg_sgetkey( hKey, szKeyName, ExtToolInfo.ToolName, sizeof(ExtToolInfo.ToolName) );

				wsprintf(szKeyName, "cmdline%d", i);
				reg_sgetkey( hKey, szKeyName, ExtToolInfo.CmdLine, sizeof(ExtToolInfo.CmdLine) );

				wsprintf(szKeyName, "workdir%d", i);
				reg_sgetkey( hKey, szKeyName, ExtToolInfo.WorkDir, sizeof(ExtToolInfo.WorkDir) );

				wsprintf(szKeyName, "mainmenu%d", i);
				reg_getkey( hKey, szKeyName,  &nTemp);
				ExtToolInfo.ShowOnMainMenu = nTemp;

				wsprintf(szKeyName, "popupmenu%d", i);
				reg_getkey( hKey, szKeyName, &nTemp );
				ExtToolInfo.ShowOnPopupMenu = nTemp;

				wsprintf(szKeyName, "startup%d", i);
				reg_getkey( hKey, szKeyName, &nTemp );
				ExtToolInfo.ExecOnStartup = nTemp;

				wsprintf(szKeyName, "overwrite%d", i);
				reg_getkey( hKey, szKeyName, &nTemp);
				ExtToolInfo.ExecWithOverwrite = nTemp;

				AddExtTool(i - 1, &ExtToolInfo);
			}
			RegCloseKey(hKey);
	} else {
		InitExtTool(0);
	}

	hsed_ver = HSED_VER;
	hsed_private_ver = HSED_PRIVATE_VER;

	//		Check helpdir

	if (hsp_helpmode==0) {
		gethdir();
		wsprintf(helpopt,"%shsppidx.htm",hdir);
		if (fileok(helpopt)) {
			hsp_helpdir[0]=0;
		}
	}
}

/*
		INI file I/O routines
*/

const char *base64_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef union tagBase64_Block{
	struct{
		unsigned int c1 : 6;
		unsigned int c2 : 6;
		unsigned int c3 : 6;
		unsigned int c4 : 6;
	} n;
	char c[3];
} BASE64_BLOCK;

static inline void ini_getkey( const char *filename, const char *secname, char *readkey, int *value )
{
	*value = GetPrivateProfileInt(secname, readkey, *value, filename);
}

static inline DWORD ini_sgetkey( const char *filename, const char *secname, char *readkey, char *strbuf, int size )
{
	return GetPrivateProfileString(secname, readkey, "", strbuf, size, filename);
}

static int ini_bgetkey( const char *filename, const char *secname, char *readkey, BYTE *buf, int length )
{
	char strbuf[1280], membuf[1280];
	const char *tmp;
	int offset, memoffset, buflen, validbuflen;
	BASE64_BLOCK block;
	ini_sgetkey(filename, secname, readkey, strbuf, sizeof(strbuf));

	// Base64 decode
	buflen = lstrlen(strbuf);
	if(buflen % 4 != 0)
		return -1;
	for(validbuflen = buflen; validbuflen > 0 && strbuf[validbuflen-1] == '='; validbuflen--);
	if(validbuflen * 3 / 4 != length) return -1;

	for(offset = 0, memoffset = 0; offset < buflen; offset += 4, memoffset += 3){
		tmp = strbuf[offset] == '=' ? base64_map : strchr(base64_map, strbuf[offset]);
		block.n.c1 = tmp - base64_map;
		tmp = strbuf[offset+1] == '=' ? base64_map : strchr(base64_map, strbuf[offset+1]);
		if(tmp == NULL) return -1;
		block.n.c2 = tmp - base64_map;
		tmp = strbuf[offset+2] == '=' ? base64_map : strchr(base64_map, strbuf[offset+2]);
		if(tmp == NULL) return -1;
		block.n.c3 = tmp - base64_map;
		tmp = strbuf[offset+3] == '=' ? base64_map : strchr(base64_map, strbuf[offset+3]);
		if(tmp == NULL) return -1;
		block.n.c4 = tmp - base64_map;
		memcpy(membuf + memoffset, block.c, sizeof(BASE64_BLOCK));
	}
	memcpy(buf, membuf, length);
	return 0;
}

static void ini_setkey( const char *filename, const char *secname, char *writekey, int value )
{
	char buf[32];
	wsprintf(buf, "%d", value);
	WritePrivateProfileString( filename, secname, writekey, buf );
}

static inline void ini_ssetkey( const char *filename, const char *secname, char *writekey, char *string )
{
	char buf[1280];
	wsprintf( buf, "\"%s\"", string );
	WritePrivateProfileString( secname, writekey, buf, filename );
}

static void ini_bsetkey( const char *filename, const char *secname, char *writekey, BYTE *buf, int length )
{
	char strbuf[1280];
	BASE64_BLOCK block;
	int offset = 0, i, n;

	// Base64 encode
	for(i = 2; i < length; i += 3, buf += 3){
		memcpy(block.c, buf, sizeof(BASE64_BLOCK));
		strbuf[offset++] = base64_map[block.n.c1];
		strbuf[offset++] = base64_map[block.n.c2];
		strbuf[offset++] = base64_map[block.n.c3];
		strbuf[offset++] = base64_map[block.n.c4];
	}
	n = length % 3;
	if(n > 0){
		ZeroMemory(&block, sizeof(BASE64_BLOCK));
		memcpy(block.c, buf, sizeof(BASE64_BLOCK));
		strbuf[offset++] = base64_map[block.n.c1];
		strbuf[offset++] = base64_map[block.n.c2];
		if(n == 2) strbuf[offset++] = base64_map[block.n.c3];
		else strbuf[offset++] = '=';
		strbuf[offset++] = '=';
	}
	strbuf[offset] = '\0';

	ini_ssetkey(filename, secname, writekey, strbuf);
}


static void ini_save( void )
{
	//		INIファイルに書き込む
	//

	// 暫定(a temporary step)
	char filename[_MAX_PATH + 1];
	wsprintf(filename, "%s\\%s", szExeDir, INI_FILE);

	int i;
	int nSize, nTemp;
	EXTTOOLINFO *lpExtToolInfo;
	char szKeyName[256];

	ini_setkey( filename, "Compile", "fullscr", hsp_fullscr );
	ini_setkey( filename, "View", "hscroll", hscroll_flag );
	ini_ssetkey( filename, "Compile", "cmdopt", hsp_cmdopt );
	ini_ssetkey( filename, "Compile", "laststr", hsp_laststr );
	chg_font=PopFontGetELG();
	ini_bsetkey( filename, "Edit", "font", (BYTE *)&chg_font, sizeof(LOGFONT) );
	tchg_font=PopFontGetTLG();
	ini_bsetkey( filename, "Edit", "tabfont", (BYTE *)&tchg_font, sizeof(LOGFONT) );

	CheckWndFlag();						// Window attribute check
	ini_setkey( filename, "Startup", "winflg", winflg );

	if (winflg==0) {
		ini_setkey( filename, "Startup", "winx", winx );
		ini_setkey( filename, "Startup", "winy", winy );
		ini_setkey( filename, "Startup", "posx", posx );
		ini_setkey( filename, "Startup", "posy", posy );
	}

	ini_setkey( filename, "View", "toolbar", flg_toolbar );
	ini_setkey( filename, "View", "statbar", flg_statbar );
	ini_ssetkey( filename, "Help", "helpdir", hsp_helpdir );
	ini_setkey( filename, "Compile", "exewx", hsp_wx );
	ini_setkey( filename, "Compile", "exewy", hsp_wy );
	ini_setkey( filename, "Compile", "exewd", hsp_wd );
	ini_setkey( filename, "Compile", "debug", hsp_debug );
	ini_setkey( filename, "Compile", "extobj", hsp_extobj );
	ini_ssetkey( filename, "Compile", "extstr", hsp_extstr );
	ini_setkey( filename, "Help", "helpmode", hsp_helpmode );
	ini_setkey( filename, "Compile", "logmode", hsp_logmode );
	ini_setkey( filename, "Compile", "logadd", hsp_logadd );
	ini_setkey( filename, "Version", "hsedver", hsed_ver );
	ini_setkey( filename, "Version", "hsedprivatever", hsed_private_ver );
	ini_setkey( filename, "Compile", "extmacro", hsp_extmacro );
	ini_setkey( filename, "Compile", "clmode", hsp_clmode );
	ini_setkey( filename, "Compile", "orgpath", hsp_orgpath );

	ini_setkey( filename, "Startup", "startmode", startflag );
	ini_ssetkey( filename, "Startup", "startdir", startdir );

	ini_setkey( filename, "Edit", "tabenabled",tabEnabled );
	ini_setkey( filename, "Edit", "hsenabled", hsEnabled );
	ini_setkey( filename, "Edit", "fsenabled", fsEnabled );
	ini_setkey( filename, "Edit", "nlenabled", nlEnabled );
	ini_setkey( filename, "Edit", "eofenabled", eofEnabled );
	ini_setkey( filename, "Edit", "underlineenabled", ulEnabled );
	ini_setkey( filename, "Edit", "tabsize", tabsize );
	ini_setkey( filename, "Edit", "rulerheight", rulerheight );
	ini_setkey( filename, "Edit", "linewidth", linewidth );
	ini_setkey( filename, "Edit", "linespace", linespace );

	ini_setkey( filename, "Behavior", "autoindent", bAutoIndent );

	ini_setkey( filename, "Option", "column1width", ColumnWidth[0] );
	ini_setkey( filename, "Option", "column2width", ColumnWidth[1] );

	ini_setkey( filename, "Color", "Default", color.Character.Default.Conf );
	ini_setkey( filename, "Color", "String", color.Character.String.Conf );
	ini_setkey( filename, "Color", "Function", color.Character.Function.Conf );
	ini_setkey( filename, "Color", "PreProcessor", color.Character.Preprocessor.Conf );
	ini_setkey( filename, "Color", "Macro", color.Character.Macro.Conf );
	ini_setkey( filename, "Color", "Comment", color.Character.Comment.Conf );
	ini_setkey( filename, "Color", "Label", color.Character.Label.Conf );
	ini_setkey( filename, "Color", "HalfSpace", color.NonCharacter.HalfSpace.Conf );
	ini_setkey( filename, "Color", "FullSpace", color.NonCharacter.FullSpace.Conf );
	ini_setkey( filename, "Color", "Tab", color.NonCharacter.Tab.Conf );
	ini_setkey( filename, "Color", "NewLine", color.NonCharacter.NewLine.Conf );
	ini_setkey( filename, "Color", "EOF", color.NonCharacter.EndOfFile.Conf );
	ini_setkey( filename, "Color", "BackGround", color.Edit.Background.Conf );
	ini_setkey( filename, "Color", "UnderLine", color.Edit.CaretUnderLine.Conf );
	ini_setkey( filename, "Color", "BoundaryLineNum", color.Edit.BoundaryLineNumber.Conf );
	ini_setkey( filename, "Color", "LineNum", color.LineNumber.Number.Conf );
	ini_setkey( filename, "Color", "LineNumCaret", color.LineNumber.CaretLine.Conf );
	ini_setkey( filename, "Color", "RulerNum", color.Ruler.Number.Conf );
	ini_setkey( filename, "Color", "RulerBackGround", color.Ruler.Background.Conf );
	ini_setkey( filename, "Color", "RulerDivision", color.Ruler.Division.Conf );
	ini_setkey( filename, "Color", "RulerCaret", color.Ruler.Caret.Conf );

	ini_setkey( filename, "UserColor", "Default", color.Character.Default.Combo );
	ini_setkey( filename, "UserColor", "String", color.Character.String.Combo );
	ini_setkey( filename, "UserColor", "Function", color.Character.Function.Combo );
	ini_setkey( filename, "UserColor", "PreProcessor", color.Character.Preprocessor.Combo );
	ini_setkey( filename, "UserColor", "Macro", color.Character.Macro.Combo );
	ini_setkey( filename, "UserColor", "Comment", color.Character.Comment.Combo );
	ini_setkey( filename, "UserColor", "Label", color.Character.Label.Combo );
	ini_setkey( filename, "UserColor", "HalfSpace", color.NonCharacter.HalfSpace.Combo );
	ini_setkey( filename, "UserColor", "FullSpace", color.NonCharacter.FullSpace.Combo );
	ini_setkey( filename, "UserColor", "Tab", color.NonCharacter.Tab.Combo );
	ini_setkey( filename, "UserColor", "NewLine", color.NonCharacter.NewLine.Combo );
	ini_setkey( filename, "UserColor", "EOF", color.NonCharacter.EndOfFile.Combo );
	ini_setkey( filename, "UserColor", "BackGround", color.Edit.Background.Combo );
	ini_setkey( filename, "UserColor", "UnderLine", color.Edit.CaretUnderLine.Combo );
	ini_setkey( filename, "UserColor", "BoundaryLineNum", color.Edit.BoundaryLineNumber.Combo );
	ini_setkey( filename, "UserColor", "LineNum", color.LineNumber.Number.Combo );
	ini_setkey( filename, "UserColor", "LineNumCaret", color.LineNumber.CaretLine.Combo );
	ini_setkey( filename, "UserColor", "RulerNum", color.Ruler.Number.Combo );
	ini_setkey( filename, "UserColor", "RulerBackGround", color.Ruler.Background.Combo );
	ini_setkey( filename, "UserColor", "RulerDivision", color.Ruler.Division.Combo );
	ini_setkey( filename, "UserColor", "RulerCaret", color.Ruler.Caret.Combo );

	nSize = GetExtToolSize();
	ini_setkey( filename, "ExtTools", "numoftools", nSize );
	for(i = 1; i <= nSize; i++){
		lpExtToolInfo = GetExtTool(i - 1);
		if(lpExtToolInfo->Used){
			wsprintf(szKeyName, "toolname%d", i);
			ini_ssetkey( filename, "ExtTools", szKeyName, lpExtToolInfo->ToolName);
			wsprintf(szKeyName, "filename%d", i);
			ini_ssetkey( filename, "ExtTools", szKeyName, lpExtToolInfo->FileName);
			wsprintf(szKeyName, "cmdline%d", i);
			ini_ssetkey( filename, "ExtTools", szKeyName, lpExtToolInfo->CmdLine);
			wsprintf(szKeyName, "workdir%d", i);
			ini_ssetkey( filename, "ExtTools", szKeyName, lpExtToolInfo->WorkDir);
			wsprintf(szKeyName, "mainmenu%d", i);
			ini_setkey( filename, "ExtTools", szKeyName, lpExtToolInfo->ShowOnMainMenu);
			wsprintf(szKeyName, "popupmenu%d", i);
			ini_setkey( filename, "ExtTools", szKeyName, lpExtToolInfo->ShowOnPopupMenu);
			wsprintf(szKeyName, "startup%d", i);
			ini_setkey( filename, "ExtTools", szKeyName, lpExtToolInfo->ExecOnStartup);
			wsprintf(szKeyName, "overwrite%d", i);
			ini_setkey( filename, "ExtTools", szKeyName, lpExtToolInfo->ExecWithOverwrite);
		} else {
			wsprintf(szKeyName, "filename%d", i);
			ini_ssetkey( filename, "ExtTools", szKeyName, "");
		}
	}

	ini_ssetkey( filename, "Keywords", "keyfile", (char *)filelist.getlist() );

}

static void ini_load()
{
	//		INIファイルから読み込む
	//

	char filename[_MAX_PATH + 1];
	wsprintf(filename, "%s\\%s", szExeDir, INI_FILE);

	char szKeyFile[32000];

	EXTTOOLINFO ExtToolInfo;
	int nSize, nTemp, i;
	char szKeyName[256];

	//		Reset to default value

	set_default();

	//		Read keys

	if (!fileok(filename)) {

		ini_getkey( filename, "Compile", "fullscr", &hsp_fullscr );
		ini_getkey( filename, "View", "hscroll", &hscroll_flag );
		ini_sgetkey( filename, "Compile", "cmdopt", hsp_cmdopt, sizeof(hsp_cmdopt) );
		ini_sgetkey( filename, "Compile", "laststr", hsp_laststr, sizeof(hsp_laststr) );
		ini_bgetkey( filename, "Edit", "font", (BYTE *)&chg_font, sizeof(LOGFONT) );
		ini_bgetkey( filename, "Edit", "tabfont", (BYTE *)&tchg_font, sizeof(LOGFONT) );
		ini_getkey( filename, "Startup", "winflg", &winflg );
		ini_getkey( filename, "Startup", "winx", &winx );
		ini_getkey( filename, "Startup", "winy", &winy );
		ini_getkey( filename, "Startup", "posx", &posx );
		ini_getkey( filename, "Startup", "posy", &posy );
		ini_getkey( filename, "View", "toolbar", &flg_toolbar );
		ini_getkey( filename, "View", "statbar", &flg_statbar );
		ini_sgetkey( filename, "Help", "helpdir", hsp_helpdir, sizeof(hsp_helpdir) );
		ini_getkey( filename, "Compile", "exewx", &hsp_wx );
		ini_getkey( filename, "Compile", "exewy", &hsp_wy );
		ini_getkey( filename, "Compile", "exewd", &hsp_wd );
		ini_getkey( filename, "Compile", "debug", &hsp_debug );
		ini_sgetkey( filename, "Compile", "extstr", hsp_extstr, sizeof(hsp_extstr) );
		ini_getkey( filename, "Compile", "extobj", &hsp_extobj );
		ini_getkey( filename, "Help", "helpmode", &hsp_helpmode );
		ini_getkey( filename, "Compile", "logmode", &hsp_logmode );
		ini_getkey( filename, "Compile", "logadd", &hsp_logadd );
		ini_getkey( filename, "Compile", "extmacro", &hsp_extmacro );
		ini_getkey( filename, "Compile", "clmode", &hsp_clmode );
		ini_getkey( filename, "Compile", "orgpath", &hsp_orgpath );

		ini_getkey( filename, "Startup", "startmode", &startflag );
		ini_sgetkey( filename, "Startup", "startdir", startdir, sizeof(startdir) );

		ini_getkey( filename, "Edit", "tabenabled",&tabEnabled );
		ini_getkey( filename, "Edit", "hsenabled", &hsEnabled );
		ini_getkey( filename, "Edit", "fsenabled", &fsEnabled );
		ini_getkey( filename, "Edit", "nlenabled", &nlEnabled );
		ini_getkey( filename, "Edit", "eofenabled", &eofEnabled );
		ini_getkey( filename, "Edit", "underlineenabled", &ulEnabled );
		ini_getkey( filename, "Edit", "tabsize", &tabsize );
		ini_getkey( filename, "Edit", "rulerheight", &rulerheight );
		ini_getkey( filename, "Edit", "linewidth", &linewidth );
		ini_getkey( filename, "Edit", "linespace", &linespace );

		ini_getkey( filename, "Behavior", "autoindent", &bAutoIndent );

		ini_getkey( filename, "Option", "column1width", &ColumnWidth[0] );
		ini_getkey( filename, "Option", "column2width", &ColumnWidth[1] );

		ini_getkey( filename, "Color", "Default", (int *)&(color.Character.Default.Conf) );
		ini_getkey( filename, "Color", "String", (int *)&(color.Character.String.Conf) );
		ini_getkey( filename, "Color", "Function", (int *)&(color.Character.Function.Conf) );
		ini_getkey( filename, "Color", "PreProcessor", (int *)&(color.Character.Preprocessor.Conf) );
		ini_getkey( filename, "Color", "Macro", (int *)&(color.Character.Macro.Conf) );
		ini_getkey( filename, "Color", "Comment", (int *)&(color.Character.Comment.Conf) );
		ini_getkey( filename, "Color", "Label", (int *)&(color.Character.Label.Conf) );
		ini_getkey( filename, "Color", "HalfSpace", (int *)&(color.NonCharacter.HalfSpace.Conf) );
		ini_getkey( filename, "Color", "FullSpace", (int *)&(color.NonCharacter.FullSpace.Conf) );
		ini_getkey( filename, "Color", "Tab", (int *)&(color.NonCharacter.Tab.Conf) );
		ini_getkey( filename, "Color", "NewLine", (int *)&(color.NonCharacter.NewLine.Conf) );
		ini_getkey( filename, "Color", "EOF", (int *)&(color.NonCharacter.EndOfFile.Conf) );
		ini_getkey( filename, "Color", "BackGround", (int *)&(color.Edit.Background.Conf) );
		ini_getkey( filename, "Color", "UnderLine", (int *)&(color.Edit.CaretUnderLine.Conf) );
		ini_getkey( filename, "Color", "BoundaryLineNum", (int *)&(color.Edit.BoundaryLineNumber.Conf) );
		ini_getkey( filename, "Color", "LineNum", (int *)&(color.LineNumber.Number.Conf) );
		ini_getkey( filename, "Color", "LineNumCaret", (int *)&(color.LineNumber.CaretLine.Conf) );
		ini_getkey( filename, "Color", "RulerNum", (int *)&(color.Ruler.Number.Conf) );
		ini_getkey( filename, "Color", "RulerBackGround", (int *)&(color.Ruler.Background.Conf) );
		ini_getkey( filename, "Color", "RulerDivision", (int *)&(color.Ruler.Division.Conf) );
		ini_getkey( filename, "Color", "RulerCaret", (int *)&(color.Ruler.Caret.Conf) );

		ini_getkey( filename, "UserColor", "Default", (int *)&(color.Character.Default.Combo) );
		ini_getkey( filename, "UserColor", "String", (int *)&(color.Character.String.Combo) );
		ini_getkey( filename, "UserColor", "Function", (int *)&(color.Character.Function.Combo) );
		ini_getkey( filename, "UserColor", "PreProcessor", (int *)&(color.Character.Preprocessor.Combo) );
		ini_getkey( filename, "UserColor", "Macro", (int *)&(color.Character.Macro.Combo) );
		ini_getkey( filename, "UserColor", "Comment", (int *)&(color.Character.Comment.Combo) );
		ini_getkey( filename, "UserColor", "Label", (int *)&(color.Character.Label.Combo) );
		ini_getkey( filename, "UserColor", "HalfSpace", (int *)&(color.NonCharacter.HalfSpace.Combo) );
		ini_getkey( filename, "UserColor", "FullSpace", (int *)&(color.NonCharacter.FullSpace.Combo) );
		ini_getkey( filename, "UserColor", "Tab", (int *)&(color.NonCharacter.Tab.Combo) );
		ini_getkey( filename, "UserColor", "NewLine", (int *)&(color.NonCharacter.NewLine.Combo) );
		ini_getkey( filename, "UserColor", "EOF", (int *)&(color.NonCharacter.EndOfFile.Combo) );
		ini_getkey( filename, "UserColor", "BackGround", (int *)&(color.Edit.Background.Combo) );
		ini_getkey( filename, "UserColor", "UnderLine", (int *)&(color.Edit.CaretUnderLine.Combo) );
		ini_getkey( filename, "UserColor", "BoundaryLineNum", (int *)&(color.Edit.BoundaryLineNumber.Combo) );
		ini_getkey( filename, "UserColor", "LineNum", (int *)&(color.LineNumber.Number.Combo) );
		ini_getkey( filename, "UserColor", "LineNumCaret", (int *)&(color.LineNumber.CaretLine.Combo) );
		ini_getkey( filename, "UserColor", "RulerNum", (int *)&(color.Ruler.Number.Combo) );
		ini_getkey( filename, "UserColor", "RulerBackGround", (int *)&(color.Ruler.Background.Combo) );
		ini_getkey( filename, "UserColor", "RulerDivision", (int *)&(color.Ruler.Division.Combo) );
		ini_getkey( filename, "UserColor", "RulerCaret", (int *)&(color.Ruler.Caret.Combo) );

		ini_sgetkey( filename, "Keywords", "keyfile", szKeyFile, sizeof(szKeyFile));

		nSize = 0;
		ini_getkey( filename, "ExtTools", "numoftools", &nSize );
		InitExtTool(nSize);
		for(i = 1; i <= nSize; i++){
			DefaultExtToolInfo(&ExtToolInfo);

			wsprintf(szKeyName, "filename%d", i);
			if(ini_sgetkey( filename, "ExtTools", szKeyName, ExtToolInfo.FileName, sizeof(ExtToolInfo.FileName) ) == 0) continue;

			wsprintf(szKeyName, "toolname%d", i);
			ini_sgetkey( filename, "ExtTools", szKeyName, ExtToolInfo.ToolName, sizeof(ExtToolInfo.ToolName) );

			wsprintf(szKeyName, "cmdline%d", i);
			ini_sgetkey( filename, "ExtTools", szKeyName, ExtToolInfo.CmdLine, sizeof(ExtToolInfo.CmdLine) );

			wsprintf(szKeyName, "workdir%d", i);
			ini_sgetkey( filename, "ExtTools", szKeyName, ExtToolInfo.WorkDir, sizeof(ExtToolInfo.WorkDir) );

			wsprintf(szKeyName, "mainmenu%d", i);
			ini_getkey( filename, "ExtTools", szKeyName,  &nTemp);
			ExtToolInfo.ShowOnMainMenu = nTemp;

			wsprintf(szKeyName, "popupmenu%d", i);
			ini_getkey( filename, "ExtTools", szKeyName, &nTemp );
			ExtToolInfo.ShowOnPopupMenu = nTemp;

			wsprintf(szKeyName, "startup%d", i);
			ini_getkey( filename, "ExtTools", szKeyName, &nTemp );
			ExtToolInfo.ExecOnStartup = nTemp;

			wsprintf(szKeyName, "overwrite%d", i);
			ini_getkey( filename, "ExtTools", szKeyName, &nTemp);
			ExtToolInfo.ExecWithOverwrite = nTemp;

			AddExtTool(i - 1, &ExtToolInfo);
		}

		ini_getkey( filename, "Version", "hsedver", &hsed_ver );
		ini_getkey( filename, "Version", "hsedprivatever", &hsed_private_ver );
	}
	hsed_ver = HSED_VER;
	hsed_private_ver = HSED_PRIVATE_VER;

	//		Check helpdir

	if (hsp_helpmode==0) {
		gethdir();
		wsprintf(helpopt,"%shsppidx.htm",hdir);
		if (fileok(helpopt)) {
			hsp_helpdir[0]=0;
		}
	}
}