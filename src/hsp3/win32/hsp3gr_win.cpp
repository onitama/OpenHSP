
//
//	HSP3 graphics command
//	(GUI関連コマンド・関数処理)
//	onion software/onitama 2004/6
//
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <direct.h>
#include <shlobj.h>

#include <io.h>
#include <fcntl.h>

#include "../hsp3code.h"
#include "../hsp3debug.h"
#include "../supio.h"
#include "../strbuf.h"

#include "../win32gui/filedlg.h"

#include "hsp3gr_win.h"

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HSPCTX *ctx;
static int *type;
static int *val;
static int cur_window;
static int p1,p2,p3,p4,p5,p6;
static int ckey,cklast,cktrg;
static int msact;
static int dispflg;

extern int resY0, resY1;

/*----------------------------------------------------------*/
//					HSP system support
/*----------------------------------------------------------*/

void ExecFile( char *stmp, char *ps, int mode )
{
	int i,j;
	j=SW_SHOWDEFAULT;if (p1&2) j=SW_SHOWMINIMIZED;

	if ( *ps != 0 ) {
		SHELLEXECUTEINFO exinfo;
		memset( &exinfo, 0, sizeof(SHELLEXECUTEINFO) );
		exinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		exinfo.fMask = SEE_MASK_INVOKEIDLIST;
		exinfo.hwnd = NULL;
		exinfo.lpVerb = ps;
		exinfo.lpFile = stmp;
		exinfo.nShow = SW_SHOWNORMAL;
		if ( ShellExecuteEx( &exinfo ) == false ) throw HSPERR_EXTERNAL_EXECUTE;
		return;
	}
		
	if ( mode&16 ) {
		i = (int)((INT_PTR)ShellExecute( NULL,NULL,stmp,"","",j ));
	}
	else if ( mode&32 ) {
		i = (int)((INT_PTR)ShellExecute( NULL,"print",stmp,"","",j ));
	}
	else {
		i=WinExec( stmp,j );
	}
	if (i<32) throw HSPERR_EXTERNAL_EXECUTE;
}


/*
#define CSIDL_DESKTOP                   0x0000
#define CSIDL_INTERNET                  0x0001
#define CSIDL_PROGRAMS                  0x0002
#define CSIDL_CONTROLS                  0x0003
#define CSIDL_PRINTERS                  0x0004
#define CSIDL_PERSONAL                  0x0005
#define CSIDL_FAVORITES                 0x0006
#define CSIDL_STARTUP                   0x0007
#define CSIDL_RECENT                    0x0008
#define CSIDL_SENDTO                    0x0009
#define CSIDL_BITBUCKET                 0x000a
#define CSIDL_STARTMENU                 0x000b
#define CSIDL_DESKTOPDIRECTORY          0x0010
#define CSIDL_DRIVES                    0x0011
#define CSIDL_NETWORK                   0x0012
#define CSIDL_NETHOOD                   0x0013
#define CSIDL_FONTS                     0x0014
#define CSIDL_TEMPLATES                 0x0015
#define CSIDL_COMMON_STARTMENU          0x0016
#define CSIDL_COMMON_PROGRAMS           0X0017
#define CSIDL_COMMON_STARTUP            0x0018
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019
#define CSIDL_APPDATA                   0x001a
#define CSIDL_PRINTHOOD                 0x001b
#define CSIDL_ALTSTARTUP                0x001d         // DBCS
#define CSIDL_COMMON_ALTSTARTUP         0x001e         // DBCS
#define CSIDL_COMMON_FAVORITES          0x001f
#define CSIDL_INTERNET_CACHE            0x0020
#define CSIDL_COOKIES                   0x0021
#define CSIDL_HISTORY                   0x0022
*/

static char *getdir( int id )
{
	//		dirinfo命令の内容をstmpに設定する
	//
	char *p;
	char *ss;
	char fname[_MAX_PATH+1];
	p = ctx->stmp;

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
		_getcwd( p, _MAX_PATH );
		break;
	case 1:				//    HSPの実行ファイルがあるディレクトリ
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpath( fname, p, 32 );
		break;
	case 2:				//    Windowsディレクトリ
		GetWindowsDirectory( p, _MAX_PATH );
		break;
	case 3:				//    Windowsのシステムディレクトリ
		GetSystemDirectory( p, _MAX_PATH );
		break;
	case 4:				//    コマンドライン文字列
		ss = GetCommandLine();
		ss = strsp_cmds( ss );
#ifdef HSPDEBUG
		ss = strsp_cmds( ss );
#endif
		sbStrCopy( &(ctx->stmp), ss );
		p = ctx->stmp;
		return p;
	case 5:				//    HSPTV素材があるディレクトリ
#if defined(HSPDEBUG)||defined(HSP3IMP)
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpath( fname, p, 32 );
		CutLastChr( p, '\\' );
		strcat( p, "\\hsptv\\" );
		return p;
#else
		*p = 0;
		return p;
#endif
		break;
	default:
		if ( id & 0x10000 ) {
			SHGetSpecialFolderPath( NULL, p, id & 0xffff, FALSE );
			break;
		}
		throw HSPERR_ILLEGAL_FUNCTION;
	}

	//		最後の'\\'を取り除く
	//
	CutLastChr( p, '\\' );
	return p;
}


static int sysinfo( int p2 )
{
	//		System strings get
	//
	int fl;
	char pp[128];
	char *p1;
	BOOL success;
	DWORD version;
	DWORD size;
	DWORD *mss;
	SYSTEM_INFO si;
	MEMORYSTATUS ms;

	fl = HSPVAR_FLAG_INT;
	p1 = ctx->stmp;
	size = _MAX_PATH;

	if (p2&16) {
		GetSystemInfo(&si);
	}
	if (p2&32) {
		GlobalMemoryStatus(&ms);
		mss=(DWORD *)&ms;
		*(int *)p1 = (int)mss[p2&15];
		return fl;
	}

	switch(p2) {
	case 0:
		strcpy(p1,"Windows");
		version = GetVersion();
		if ((version & 0x80000000) == 0) strcat(p1,"NT");
									else strcat(p1,"9X");
/*
	rev 43
	mingw : warning : 仮引数int 実引数long unsigned
	に対処
*/
		sprintf( pp," ver%d.%d", static_cast< int >( version&0xff ), static_cast< int >( (version&0xff00)>>8 ) );
		strcat( p1, pp );
		fl=HSPVAR_FLAG_STR;
		break;
	case 1:
		success = GetUserName( p1,&size );
		fl = HSPVAR_FLAG_STR;
		break;
	case 2:
		success = GetComputerName(p1, &size );
		fl = HSPVAR_FLAG_STR;
		break;
	case 16:
		*(int *)p1 = (int)si.dwProcessorType;
		break;
	case 17:
		*(int *)p1 = (int)si.dwNumberOfProcessors;
		break;
	default:
		throw HSPERR_ILLEGAL_FUNCTION;
	}
	return fl;
}


void *ex_getbmscr( int wid )
{
	return NULL;
}

void ex_mref( PVal *pval, int prm )
{
	int t,size;
	void *ptr;
	const int GETBM=0x60;
	t = HSPVAR_FLAG_INT;
	size = 4;
	if ( prm >= GETBM ) {
		throw HSPERR_UNSUPPORTED_FUNCTION;
	} else {
		switch( prm ) {
		case 0x40:
			ptr = &ctx->stat;
			break;
		case 0x41:
			ptr = ctx->refstr;
			t = HSPVAR_FLAG_STR;
			size = 1024;
			break;
		case 0x44:
			ptr = ctx; size = sizeof(HSPCTX);
			break;
		default:
			throw HSPERR_UNSUPPORTED_FUNCTION;
		}
	}
	HspVarCoreDupPtr( pval, t, ptr, size );
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static void cmdfunc_dialog( void )
{
	// dialog
	int i;
	char *ptr;
	char *ps;
	char stmp[0x4000];
	ptr = code_getdsi( "" );
	strncpy( stmp, ptr, 0x4000-1 );
	p1 = code_getdi( 0 );
	ps = code_getds("");

	if (p1&16) {
		ctx->stat = fd_dialog( NULL, p1&3, stmp, ps );
			if ( ctx->stat == 0 ) {
				ctx->refstr[0] = 0;
			} else {
				strncpy( ctx->refstr, fd_getfname(), HSPCTX_REFSTR_MAX-1 );
			}
	}
	else {
		i=0;
		if (p1&1) i|=MB_ICONEXCLAMATION; else i|=MB_ICONINFORMATION;
		if (p1&2) i|=MB_YESNO; else i|=MB_OK;
		ctx->stat = MessageBox( NULL, stmp, ps, i );
	}
}


static int cmdfunc_extcmd( int cmd )
{
	//		cmdfunc : TYPE_EXTCMD
	//		(内蔵GUIコマンド)
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)
	switch( cmd ) {							// サブコマンドごとの分岐

	case 0x02:								// exec
		{
		char *ps;
		char fname[_MAX_PATH];
		strncpy( fname, code_gets(), _MAX_PATH-1 );
		p1 = code_getdi( 0 );
		ps = code_getds( "" );
		ExecFile( fname, ps, p1 );
		break;
		}

	case 0x03:								// dialog
		cmdfunc_dialog();
		break;

	case 0x0f:								// mes,print
		{
		char *ptr;
		ptr = code_getdsi( "" );
		fputs(ptr, stdout);
		if (code_getdi(0) == 0) fputs("\n", stdout);
		break;
		}

	case 0x27:								// input (console)
		{
		PVal *pval;
		APTR aptr;
		char *pp2;
		char *vptr;
		int strsize;
		int a;
		strsize = 0;
		aptr = code_getva( &pval );
		//pp2 = code_getvptr( &pval, &size );
		p2 = code_getdi( 0x4000 );
		p3 = code_getdi( 0 );

		if ( p2 < 64 ) p2 = 64;
		pp2 = code_stmp( p2+1 );

		switch( p3 & 15 ) {
		case 0:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a==EOF ) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 1:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 2:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a == '\r' ) {
					int c = getchar();
					if( c != '\n' ) {
						ungetc(c, stdin);
					}
					break;
				}
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		}

		*pp2 = 0;
		ctx->strsize = strsize + 1;

		if ( p3 & 16 ) {
			if (( pval->support & HSPVAR_SUPPORT_FLEXSTORAGE ) == 0 ) throw HSPERR_TYPE_MISMATCH;
			//HspVarCoreAllocBlock( pval, (PDAT *)vptr, strsize );
			vptr = (char *)HspVarCorePtrAPTR( pval, aptr );
			memcpy( vptr, ctx->stmp, strsize );
		} else {
			code_setva( pval, aptr, TYPE_STRING, ctx->stmp );
		}
		break;
		}

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


static int reffunc_intfunc_ivalue;

static void *reffunc_function( int *type_res, int arg )
{
	void *ptr;

	//		返値のタイプを設定する
	//
	*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
	ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != '(' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	switch( arg & 0xff ) {

	//	int function

	case 0x002:								// dirinfo
		p1 = code_geti();
		ptr = getdir( p1 );
		*type_res = HSPVAR_FLAG_STR;
		break;

	case 0x003:								// sysinfo
		p1 = code_geti();
		*type_res = sysinfo( p1 );
		ptr = ctx->stmp;
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	return ptr;
}


static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//		(内蔵GUI)
	//
	return 0;
}

void hsp3typeinit_cl_extcmd( HSP3TYPEINFO *info )
{
	HSPEXINFO *exinfo;								// Info for Plugins

	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		HSPEXINFOに関数を登録する
	//
	exinfo->actscr = &cur_window;					// Active Window ID
	exinfo->HspFunc_getbmscr = ex_getbmscr;
	exinfo->HspFunc_mref = ex_mref;

	//		バイナリモードを設定
	//
	_setmode( _fileno(stdin),  _O_BINARY );
}

void hsp3typeinit_cl_extfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_function;
}


#ifdef HSPDEBUG
void hsp3gr_dbg_gui( void )
{
	//		デバッグウインドゥ用情報
	//
	code_adddbg( "ディレクトリ", getdir(0) );
	code_adddbg( "コマンドライン", getdir(4) );
}
#endif
