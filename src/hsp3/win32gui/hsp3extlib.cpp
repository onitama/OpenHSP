
//
//	HSP3 External COM manager
//	onion software/onitama 2004/6
//	               chokuto 2005/3
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <objbase.h>
#include <tchar.h>

#include <algorithm>

#include "../hsp3config.h"

#include "../supio.h"
#include "../hsp3ext.h"
#include "hsp3extlib.h"

#ifdef HSPDISH
#include "../../hsp3dish/hspwnd.h"
#include "../../hsp3/dpmread.h"
#include "../../hsp3/strbuf.h"
#else
#include "../hspwnd.h"
//#include "hspvar_comobj.h"
//#include "hsp3win.h"
#include "../dpmread.h"
#include "../strbuf.h"
#endif

static HSPCTX *hspctx;		// Current Context
static HSPEXINFO *exinfo;	// Info for Plugins
static PVal **pmpval;

static int libmax, prmmax, hpimax;
static MEM_HPIDAT *hpidat;

#define GetPRM(id) (&hspctx->mem_finfo[id])
#define GetLIB(id) (&hspctx->mem_linfo[id])
#define strp(dsptr) &hspctx->mem_mds[dsptr]

typedef void (CALLBACK *DLLFUNC)(HSP3TYPEINFO *);
static DLLFUNC func;

//------------------------------------------------------------//

namespace hsp3 {

//------------------------------------------------------------//
/*
	CDllManager
*/
//------------------------------------------------------------//

CDllManager::CDllManager()
 : mModules(), mError( NULL )
{}


CDllManager::~CDllManager()
{
	typedef holder_type::iterator Iter;
	for ( Iter i = mModules.begin(); i != mModules.end(); ++i ) {
		FreeLibrary( *i );
	}
}


HMODULE CDllManager::load_library( const char *lpFileName )
{
	mError = NULL;
	HMODULE h;

#ifdef HSPUTF8
	HSPAPICHAR *hactmp1 = 0;
	chartoapichar( lpFileName, &hactmp1);
	h = LoadLibrary(hactmp1);
	freehac(&hactmp1);
#else
	h = LoadLibrary( lpFileName );
#endif

	try {
		if ( h != NULL ) mModules.push_front( h );
	}
	catch ( ... ) {
		if ( !FreeLibrary( h ) ) mError = h;
		h = NULL;
	}
	return h;
}


BOOL CDllManager::free_library( HMODULE hModule )
{
	typedef holder_type::iterator Iter;
	mError = NULL;
	Iter i = std::find( mModules.begin(), mModules.end(), hModule );
	if ( i == mModules.end() ) return FALSE;
	BOOL res = FreeLibrary( hModule );
	if ( res ) {
		mModules.erase( i );
	} else {
		mError = hModule;
	}
	return res;
}


BOOL CDllManager::free_all_library()
{
	typedef holder_type::iterator Iter;
	for ( Iter i = mModules.begin(); i != mModules.end(); ++i ) {
		if ( FreeLibrary( *i ) ) *i = NULL;
	}
	mModules.erase( std::remove( mModules.begin(), mModules.end(),
	 static_cast< HMODULE >( NULL ) ), mModules.end() );
	return ( mModules.empty() ? TRUE : FALSE );
}


HMODULE CDllManager::get_error() const
{
	return mError;
}

//------------------------------------------------------------//

};	//namespace hsp3 {

//------------------------------------------------------------//

hsp3::CDllManager & DllManager()
{
	static hsp3::CDllManager dm;
	return dm;
}

/*------------------------------------------------------------*/
/*
		routines
*/
/*------------------------------------------------------------*/

static void BindLIB( LIBDAT *lib, char *name )
{
	//		ライブラリのバインドを行なう
	//		(name:後から与える時のライブラリ名)
	//
	int i;
	char *n;
	HINSTANCE hd;
	if ( lib->flag != LIBDAT_FLAG_DLL ) return;
	i = lib->nameidx;
	if ( i < 0 ) {
		if ( name == NULL ) return;
		n = name;
	} else {
		n = strp(i);
	}
 	hd = DllManager().load_library( n );
	if ( hd == NULL ) return;
	lib->hlib = (void *)hd;
	lib->flag = LIBDAT_FLAG_DLLINIT;
}


static int BindFUNC( STRUCTDAT *st, char *name )
{
	//		ファンクションのバインドを行なう
	//		(name:後から与える時のファンクション名)
	//
	int i;
	char *n;
	LIBDAT *lib;
	HINSTANCE hd;
	if (( st->subid != STRUCTPRM_SUBID_DLL )&&( st->subid != STRUCTPRM_SUBID_OLDDLL )) return 4;
	i = st->nameidx;
	if ( i < 0 ) {
		if ( name == NULL ) return 3;
		n = name;
	} else {
		n = strp(i);
	}
	lib = GetLIB( st->index );
	if ( lib->flag != LIBDAT_FLAG_DLLINIT ) {
		BindLIB( lib, NULL );
		if ( lib->flag != LIBDAT_FLAG_DLLINIT ) return 2;
	}
	hd = (HINSTANCE)(lib->hlib);
	if ( hd == NULL ) return 1;
#ifdef HSPUTF8
	HSPAPICHAR *hactmp1 = 0;
	char tmp1[512];
	chartoapichar(n,&hactmp1);
	cnvsjis(tmp1,(char*)hactmp1,512);
	st->proc = (void *)GetProcAddress( hd, tmp1 );
	freehac(&hactmp1);
#else
	st->proc = (void *)GetProcAddress( hd, n );
#endif
	if ( st->proc == NULL ) return 1;
	st->subid--;
	return 0;
}


static void ExitFunc( STRUCTDAT *st )
{
	//		終了時関数の呼び出し
	//
	int p[16];
	FARPROC pFn;
	BindFUNC( st, NULL );
	pFn = (FARPROC)st->proc;
	if ( pFn == NULL ) return;
	p[0] = p[1] = p[2] = p[3] = 0;
#ifdef HSP64
	p[4] = p[5] = p[6] = p[7] = 0;
#endif
	call_extfunc(fpconv(pFn), p, st->size / 4);
}


static int Hsp3ExtAddPlugin( void )
{
	//		プラグインの登録
	//
	int i;
	HSPHED *hed;
	char *ptr;
	char *libname;
	char *funcname;
	HPIDAT *org_hpi;
	MEM_HPIDAT *hpi;
	HSP3TYPEINFO *info;
	HINSTANCE hd;
	TCHAR tmp[512];

	hed = hspctx->hsphed; ptr = (char *)hed;
	org_hpi = (HPIDAT *)(ptr + hed->pt_hpidat);
	hpimax = hed->max_hpi / sizeof(HPIDAT);

	if ( hpimax == 0 ) return 0;
	hpidat = (MEM_HPIDAT *)malloc(hpimax * sizeof(MEM_HPIDAT));
	hpi = hpidat;

	for ( i=0;i<hpimax;i++ ) {

		hpi->flag = org_hpi->flag;
		hpi->option = org_hpi->option;
		hpi->libname = org_hpi->libname;
		hpi->funcname = org_hpi->funcname;
		hpi->libptr = NULL;

		libname = strp(hpi->libname);
		funcname = strp(hpi->funcname);
		info = code_gettypeinfo(-1);

		if ( hpi->flag == HPIDAT_FLAG_TYPEFUNC ) {
		 	hd = DllManager().load_library( libname );
			if ( hd == NULL ) {
#ifdef HSPUTF8
				TCHAR tmp[512];
				HSPAPICHAR *haclibname = 0;
				chartoapichar(libname, &haclibname);
				_stprintf(tmp, TEXT("No DLL:%s"), haclibname);
				freehac(&haclibname);
				AlertW(tmp);
#else
				Alertf( "No DLL:%s", libname );
#endif
				return 1;
			}
			hpi->libptr = (void *)hd;
#ifdef HSPUTF8
			HSPAPICHAR *hacfuncname = 0;
			char tmp2[512];
			chartoapichar(funcname,&hacfuncname);
			cnvsjis(tmp2,(char*)hacfuncname,512);
			func = (DLLFUNC)GetProcAddress( hd, tmp2 );
#else
			func = (DLLFUNC)GetProcAddress( hd, funcname );
#endif
			if ( func == NULL ) {
#ifdef HSPUTF8
				TCHAR tmp[512];
				HSPAPICHAR *haclibname = 0;
				chartoapichar(libname, &haclibname);
				_stprintf(tmp, TEXT("No DLL:%s:%s"), haclibname, hacfuncname);
				freehac(&haclibname);
				AlertW(tmp);
				freehac(&hacfuncname);
#else
				Alertf("No DLL:%s:%s", libname, funcname);
#endif
				return 1;
			}
			func( info );
			code_enable_typeinfo( info );
			//Alertf( "%d_%d [%s][%s]", i, info->type, libname, funcname );
#ifdef HSPUTF8
			freehac(&hacfuncname);
#endif
		}
		hpi++;
		org_hpi++;
	}
	return 0;
}


/*------------------------------------------------------------*/
/*
		window object support
*/
/*------------------------------------------------------------*/

static BMSCR *GetBMSCR( void )
{
	HSPEXINFO *exinfo;
	exinfo = hspctx->exinfo2;
	return (BMSCR *)exinfo->HspFunc_getbmscr( *(exinfo->actscr) );
}


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int Hsp3ExtLibInit( HSP3TYPEINFO *info )
{
	int i;
	STRUCTDAT *st;
	char tmp[1024];

	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	pmpval = exinfo->mpval;

	libmax = hspctx->hsphed->max_linfo / sizeof(LIBDAT);
	prmmax = hspctx->hsphed->max_finfo / sizeof(STRUCTDAT);

	hpidat = NULL;

	if ( Hsp3ExtAddPlugin() ) return 1;

	for(i=0;i<prmmax;i++) {
		st = GetPRM(i);
		BindFUNC( st, NULL );
	}
	return 0;
}


void Hsp3ExtLibTerm( void )
{
	int i;
	STRUCTDAT *st;

	// クリーンアップ登録されているユーザー定義関数・命令呼び出し
	for(i=0;i<prmmax;i++) {
		st = GetPRM(i);
		if ( st->index >= 0 ) {
			if ( st->otindex & STRUCTDAT_OT_CLEANUP ) {
				ExitFunc( st );			// クリーンアップ関数を呼び出す
			}
		}
	}

	//	HPIDATの解放
	if (hpidat != NULL) { free( hpidat); hpidat = NULL; }

}


/*------------------------------------------------------------*/
/*
		code expand function
*/
/*------------------------------------------------------------*/

/*
	rev 43
	mingw(gcc) 用のコード追加
*/

#ifndef HSP64

#ifdef _MSC_VER

__declspec( naked ) int __cdecl call_extfunc( void *proc, int *prm, int prms )
{
	// 外部関数呼び出し（VC++ のインラインアセンブラを使用）
	//
	__asm {
		push	ebp
		mov		ebp,esp

		;# ebp+8	: 関数のポインタ
		;# ebp+12	: 引数が入ったINTの配列
		;# ebp+16	: 引数の数（pushする回数）

		;# パラメータをnp個pushする
		mov		eax, dword ptr [ebp+12]
		mov		ecx, dword ptr [ebp+16]
		jmp		_$push_chk

	_$push:
		push	dword ptr [eax+ecx*4]

	_$push_chk:
		dec		ecx
		jge		_$push

		;# 関数呼び出し
		call	dword ptr [ebp+8]

		;# 戻り値は eax に入るのでそのままリターン
		leave
		ret
	}
}

#elif defined( __GNUC__ )

int __cdecl call_extfunc( void * proc, int * prm, int prms )
{
	// 外部関数呼び出し（GCC の拡張インラインアセンブラを使用）
    int ret = 0;
    __asm__ volatile (
		"pushl  %%ebp;"
		"movl   %%esp, %%ebp;"
		"jmp    _push_chk;"

		// パラメータをprms個pushする
	"_push:"
		"pushl  ( %2, %3, 4 );"

	"_push_chk:"
		"decl   %3;"
		"jge    _push;"

		"calll  *%1;"
		"leave;"

		: "=a" ( ret )
        : "r" ( proc ) , "r" ( prm ), "r" ( prms )
    );
    return ret;
}

#else

int __cdecl call_extfunc( void * proc, int * prm, int prms )
{
	return 0;
}

#endif

#endif


int cnvwstr( void *out, char *in, int bufsize )
{
	//	hspchar->unicode に変換
	//
#ifndef HSPUTF8 
	return MultiByteToWideChar( CP_ACP, 0, in, -1, (LPWSTR)out, bufsize );
#else
	return MultiByteToWideChar(CP_UTF8, 0, in, -1, (LPWSTR)out, bufsize); 
#endif
}


int cnvsjis( void *out, char *in, int bufsize )
{
	//	unicode->sjis に変換
	//
	return WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, NULL);
}

int cnvu8(void *out, wchar_t *in, int bufsize)
{
	//  unicode->utf8に変換
	//
	return WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, NULL);
}


static char *prepare_localstr( char *src, int mode )
{
	//	DLL 渡しのための文字列を準備する
	//		mode:0=ansi/1=unicode
	//
	//	使用後は sbFree() で解放すること
	//
	int srcsize;
	char *dst;

#ifndef HSPUTF8

	if ( mode ) {
		dst = sbAlloc( (srcsize = cnvwstr(NULL, src, 0)) * sizeof(wchar_t) );
		cnvwstr( dst, src, srcsize );
	} else {
		dst = sbAlloc( (int)strlen(src)+1 );
		strcpy( dst, src );
	}

#else

	dst = sbAlloc( (srcsize = cnvwstr(NULL, src, 0)) * sizeof(wchar_t) );
	cnvwstr( dst, src, srcsize );

	if (mode == 0) {
		int bufferSize;
		char * buffer;

		buffer = sbAlloc(bufferSize = cnvsjis(NULL, dst, 0));
		cnvsjis(buffer, dst, bufferSize);

		sbFree(dst);
		dst = buffer;
	}

#endif

	return dst;
}

static int code_expand_next( char *, const STRUCTDAT *, int );

int code_expand_and_call( const STRUCTDAT *st )
{
	//	パラメータの取得および関数呼び出し（再帰処理による）
	//
	//	通常の DLL 関数呼び出しか COM メソッド呼び出しかどうかは
	//	STRUCTDAT の内容から判断します。
	//
	//	DLL 関数呼び出し時は st->proc に関数アドレスをセットして
	//	おかなければなりません（ BindFUNC() により）。
	//
	int result;

#ifdef HSP64
	char *prmbuf = sbAlloc(st->prmmax * sizeof(INT_PTR));
#else
	char *prmbuf = sbAlloc(st->size);
#endif

	try {
		result = code_expand_next( prmbuf, st, 0 );
	}
	catch (...) {
		sbFree( prmbuf );
		throw;
	}
	sbFree( prmbuf );
	return result;
}

static int code_expand_next( char *prmbuf, const STRUCTDAT *st, int index )
{
	//	次のパラメータを取得（および関数呼び出し）（再帰処理）
	//
	int result;
	HSPAPICHAR *hactmp1 = 0;
	if ( index == st->prmmax ) {
		// 関数（またはメソッド）の呼び出し
		//if ( !code_getexflg() ) throw HSPERR_TOO_MANY_PARAMETERS;
		switch ( st->subid ) {
		case STRUCTPRM_SUBID_DLL:
		case STRUCTPRM_SUBID_DLLINIT:
		case STRUCTPRM_SUBID_OLDDLL:
		case STRUCTPRM_SUBID_OLDDLLINIT:
			// 外部 DLL 関数の呼び出し
#ifdef HSP64
			result = call_extfunc(st->proc, (INT_PTR *)prmbuf, st->prmmax);
#else
			result = call_extfunc(st->proc, (INT_PTR *)prmbuf, st->size / sizeof(INT_PTR));
#endif
			break;
#ifndef HSP_COM_UNSUPPORTED
		case STRUCTPRM_SUBID_COMOBJ:
			// COM メソッドの呼び出し
			result = call_method2( prmbuf, st );
			break;
#endif
		default:
			throw ( HSPERR_UNSUPPORTED_FUNCTION );
		}
		return result;
	}

	STRUCTPRM *prm = &hspctx->mem_minfo[ st->prmindex + index ];
	void *out;
#ifdef HSP64
	out = &((INT_PTR *)prmbuf)[index];
#else
	out = prmbuf + prm->offset;
#endif

	int srcsize;
	PVal *pval_dst, *mpval;
	APTR aptr;
	PVal *pval;
	int chk;
	// 以下のポインタ（またはオブジェクト）は呼出し後に解放
	void *localbuf = NULL;
	IUnknown *punklocal = NULL;

	switch ( prm->mptype ) {

	case MPTYPE_INUM:
		*(UINT_PTR *)out = (UINT_PTR)code_getdi(0);
		break;
	case MPTYPE_PVARPTR:
		aptr = code_getva( &pval );
		*(void **)out = HspVarCorePtrAPTR( pval, aptr );
		break;
	case MPTYPE_LOCALSTRING:
	case MPTYPE_LOCALWSTR:
		*(void **)out = localbuf = prepare_localstr( code_gets(), prm->mptype == MPTYPE_LOCALWSTR );
		break;
	case MPTYPE_DNUM:
		*(double *)out = code_getdd(0.0);
		break;
	case MPTYPE_FLOAT:
		*(float *)out = (float)code_getdd(0.0);
		break;
	case MPTYPE_PPVAL:
		aptr = code_getva( &pval );
		localbuf = sbAlloc( sizeof(PVal) );
		pval_dst = (PVal *)localbuf;
		*pval_dst = *pval;
		if ( pval->flag & HSPVAR_SUPPORT_FLEXSTORAGE ) {	// ver2.5互換のための変換
			HspVarCoreGetBlockSize( pval, HspVarCorePtrAPTR( pval, aptr ), &srcsize );
			pval_dst->len[1] = (srcsize+3)/4;
			pval_dst->len[2] = 1;
			pval_dst->len[3] = 0;
			pval_dst->len[4] = 0;
		}
		*(void **)out = pval_dst;
		break;
	case MPTYPE_PBMSCR:
		*(void **)out = GetBMSCR();
		break;
	case MPTYPE_FLEXSPTR:
	case MPTYPE_FLEXWPTR:
		chk = code_get();
		if ( chk<=PARAM_END ) throw ( HSPERR_NO_DEFAULT );
		mpval = *pmpval;
		switch( mpval->flag ) {
		case HSPVAR_FLAG_INT:
			*(UINT_PTR *)out = (UINT_PTR)(*(int *)(mpval->pt));
			break;
		case HSPVAR_FLAG_STR:
			*(void ** )out = localbuf = prepare_localstr( mpval->pt, prm->mptype == MPTYPE_FLEXWPTR );
			break;
		default:
			throw ( HSPERR_TYPE_MISMATCH );
		}
		break;
	case MPTYPE_PTR_REFSTR:
		*(void **)out = hspctx->refstr;
		break;
	case MPTYPE_PTR_EXINFO:
		*(void **)out = exinfo;
		break;
	case MPTYPE_PTR_DPMINFO:
		dpm_getinf( hspctx->refstr );
		*(void **)out = hspctx->refstr;
		break;
	case MPTYPE_NULLPTR:
		*(void **)out = NULL;
		break;
#ifndef HSP_COM_UNSUPPORTED
	case MPTYPE_IOBJECTVAR:
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		punklocal = *(IUnknown **)HspVarCorePtrAPTR( pval, aptr );
		if ( punklocal ) punklocal->AddRef();	// 呼出し後に解放する
		*(void **)out = (void *)punklocal;
		break;
#endif
	default:
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
	}

	// 次のパラメータの取り出し（再帰的に処理）
	// (例外処理により動的確保したオブジェクトを確実に解放する)
	try {
		result = code_expand_next( prmbuf, st, index + 1 );
	}
	catch (...) {
		if ( localbuf ) sbFree( localbuf );
		if ( punklocal ) punklocal->Release();
		throw;
	}
	if ( localbuf ) sbFree( localbuf );
	if ( punklocal ) punklocal->Release();
	return result;
}

int exec_dllcmd( int cmd, int mask )
{
	STRUCTDAT *st;
	FARPROC pFn;
	int result;

	code_next();							// 次のコードを取得(最初に必ず必要です)

	if ( cmd >= prmmax ) {
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
	}

	st = GetPRM(cmd);
	pFn = (FARPROC)st->proc;
	if ( pFn == NULL ) {
		if ( BindFUNC( st, NULL ) ) throw ( HSPERR_DLL_ERROR );
		pFn = (FARPROC)st->proc;
	}
	if (( st->otindex & mask ) == 0 ) throw ( HSPERR_SYNTAX );

	result = code_expand_and_call( st );

	if ( st->subid == STRUCTPRM_SUBID_OLDDLLINIT ) {
		if ( result > 0 ) {
			if ( result & 0x20000 ) {
				result &= 0x1ffff;
			} else if ( result & 0x10000 ) {
				result = ( result & 0xffff ) * 10;
			} else {
				throw ( HSPERR_DLL_ERROR );
			}
			hspctx->waitcount = result;
			hspctx->waittick = -1;
			hspctx->runmode = RUNMODE_AWAIT;
			return RUNMODE_AWAIT;
		}
		hspctx->stat = -result;
	} else {
		hspctx->stat = result;
	}

	return RUNMODE_RUN;
}

int cmdfunc_dllcmd( int cmd )
{
	//		cmdfunc : TYPE_DLLCMD
	//		(拡張DLLコマンド)
	//
	return exec_dllcmd( cmd, STRUCTDAT_OT_STATEMENT );
}

