//
//	HSP3 External COM manager
//	onion software/onitama 2004/6
//	               chokuto 2005/3
//
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN            // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <objbase.h>
#include <tchar.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <dlfcn.h>

#include <ffi.h>

#include "../hsp3config.h"

#include "../supio.h"
#include "../hsp3ext.h"
#include "hsp3extlib_ffi.h"

#ifdef HSPDISH
#include "../../hsp3dish/hspwnd.h"
#include "../dpmread.h"
#include "../strbuf.h"
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

#ifdef _WIN32
typedef void (CALLBACK *DLLFUNC)(HSP3TYPEINFO *);
#else
typedef void (*DLLFUNC)(HSP3TYPEINFO *);
#endif
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
#ifdef _WIN32
		FreeLibrary( *i );
#else
		dlclose( *i );
#endif
	}
}


HANDLE_MODULE CDllManager::load_library( const char *lpFileName )
{
	mError = NULL;
	HANDLE_MODULE h;

#ifdef _WIN32
#ifdef HSPUTF8
	HSPAPICHAR *hactmp1 = 0;
	chartoapichar( lpFileName, &hactmp1);
	h = LoadLibrary(hactmp1);
	freehac(&hactmp1);
#else
	h = LoadLibrary( lpFileName );
#endif
#else
	h = dlopen( lpFileName, RTLD_LAZY );
	char *err = dlerror();
	if (err != NULL) {
		Alertf("erroro: %s\n", err);
		return NULL;
	}
#endif

	try {
		if ( h != NULL ) mModules.push_front( h );
	}
	catch ( ... ) {
#ifdef _WIN32
		if ( !FreeLibrary( h ) ) mError = h;
#else
		if ( dlclose( h ) != 0 ) mError = h;
#endif
		h = NULL;
	}
	return h;
}


bool CDllManager::free_library( HANDLE_MODULE hModule )
{
	typedef holder_type::iterator Iter;
	mError = NULL;
	Iter i = std::find( mModules.begin(), mModules.end(), hModule );
	if ( i == mModules.end() ) return false;
#ifdef _WIN32
	BOOL res = FreeLibrary( hModule );
#else
	bool res = dlclose( hModule ) == 0;
#endif
	if ( res ) {
		mModules.erase( i );
	} else {
		mError = hModule;
	}
	return res;
}


bool CDllManager::free_all_library()
{
	typedef holder_type::iterator Iter;
	for ( Iter i = mModules.begin(); i != mModules.end(); ++i ) {
		if ( dlclose( *i ) == 0 ) *i = NULL;
	}
	mModules.erase( std::remove( mModules.begin(), mModules.end(),
		static_cast< HANDLE_MODULE >( NULL ) ), mModules.end() );
	return ( mModules.empty() ? true : false );
}


HANDLE_MODULE CDllManager::get_error() const
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
	HANDLE_MODULE hd;
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
	HANDLE_MODULE hd;
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
	hd = (HANDLE_MODULE)(lib->hlib);
	if ( hd == NULL ) return 1;
#ifdef _WIN32
	st->proc = (void *)GetProcAddress( hd, n );
#else
	st->proc = (void *)dlsym( hd, n );
#endif
	char *err = dlerror();
	if (err != NULL) printf("erroro: %s\n", err);
	if ( st->proc == NULL ) return 1;
	st->subid--;
	return 0;
}


static void ExitFunc( STRUCTDAT *st )
{
	//		終了時関数の呼び出し
	//
	void *p[16];
	int zero = 0;
	ffi_type *args[16];
	BindFUNC( st, NULL );
	if ( st->proc == NULL ) return;
	for (int i = 0; i < st->prmmax; i++) {
		p[i] = &zero;
		args[i] = &ffi_type_sint;
	}

	ffi_cif cif;
	ffi_prep_cif(&cif, FFI_DEFAULT_ABI, st->prmmax, &ffi_type_sint32, args);

	int result = 0;
	ffi_call(&cif, FFI_FN(st->proc), &result, p);
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
	HANDLE_MODULE hd;

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
#if defined(HSPUTF8) && defined(_WIN32)
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
#if defined(HSPUTF8) && defined(_WIN32)
			HSPAPICHAR *hacfuncname = 0;
			char tmp2[512];
			chartoapichar(funcname,&hacfuncname);
			cnvsjis(tmp2,(char*)hacfuncname,512);
			func = (DLLFUNC)GetProcAddress( hd, tmp2 );
#else
#  if defined(_WIN32)
			func = (DLLFUNC)GetProcAddress( hd, funcname );
#  else
			func = (DLLFUNC)dlsym( hd, funcname );
#  endif
#endif
			if ( func == NULL ) {
#if defined(HSPUTF8) && defined(_WIN32)
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
#if defined(HSPUTF8) && defined(_WIN32)
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

	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	pmpval = exinfo->mpval;

	libmax = hspctx->hsphed->max_linfo / sizeof(LIBDAT);
	prmmax = hspctx->hsphed->max_finfo / sizeof(STRUCTDAT);

	hpidat = NULL;

	if ( Hsp3ExtAddPlugin() ) return 1;

	for(i=0;i<prmmax;i++) {
		st = GetPRM(i);
		int r = BindFUNC( st, NULL );
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

#if defined(_WIN32)
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
#endif


static char *prepare_localstr( char *src, int mode )
{
	//	DLL 渡しのための文字列を準備する
	//		mode:0=ansi/1=unicode
	//
	//	使用後は sbFree() で解放すること
	//
	int srcsize;
	char *dst;

#if !defined(_WIN32)
	if ( mode ) {
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
	} else {
		dst = sbAlloc( (int)strlen(src)+1 );
		strcpy( dst, src );
	}
#elif !defined(HSPUTF8)

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

static int code_expand_next( ffi_type **prm_args, void **prm_values, const STRUCTDAT *, int );

// libffi用引数をスタック上に保持する
union FfiParam {
	int i;
	double d;
	float f;
	void *ptr;
};

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

	ffi_type **prm_args = (ffi_type **) sbAlloc(st->prmmax * sizeof(ffi_type *));
	void **prm_values = (void **) sbAlloc(st->prmmax * sizeof(void *));

	try {
		result = code_expand_next( prm_args, prm_values, st, 0 );
	}
	catch (...) {
		sbFree( prm_args );
		sbFree( prm_values );
		throw;
	}
	sbFree( prm_args );
	sbFree( prm_values );
	return result;
}

static int code_expand_next( ffi_type **prm_args, void **prm_values, const STRUCTDAT *st, int index )
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
			//Alertf("%s:%d call_extfun(%p, *, %d)\n", __func__, __LINE__, st->proc, st->prmmax);
			ffi_cif cif;
			// TODO intと互換性のない返り値の受け取り
			ffi_prep_cif(&cif, FFI_DEFAULT_ABI, st->prmmax, &ffi_type_sint, prm_args);
			ffi_call(&cif, FFI_FN(st->proc), &result, prm_values);
			break;
#ifndef HSP_COM_UNSUPPORTED
		case STRUCTPRM_SUBID_COMOBJ:
			// COM メソッドの呼び出し
			result = call_method2( prmbuf, st );
			break;
#endif
		default:
			printf("### Unsupported %s:%d %d\n", __func__, __LINE__, index);
			throw ( HSPERR_UNSUPPORTED_FUNCTION );
		}
		return result;
	}

	STRUCTPRM *prm = &hspctx->mem_minfo[ st->prmindex + index ];

	int srcsize;
	PVal *pval_dst, *mpval;
	APTR aptr;
	PVal *pval;
	int chk;
	// 以下のポインタ（またはオブジェクト）は呼出し後に解放
	void *localbuf = NULL;
#ifndef HSP_COM_UNSUPPORTED
	IUnknown *punklocal = NULL;
#endif

	FfiParam p;

	switch ( prm->mptype ) {

	case MPTYPE_INUM:
		p.i = (int)code_getdi(0);
		prm_values[index] = &p.i;
		prm_args[index] = &ffi_type_sint;
		break;
	case MPTYPE_PVARPTR:
		aptr = code_getva( &pval );
		p.ptr = HspVarCorePtrAPTR( pval, aptr );
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_LOCALSTRING:
	case MPTYPE_LOCALWSTR:
		p.ptr = localbuf = prepare_localstr( code_gets(), prm->mptype == MPTYPE_LOCALWSTR );
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_DNUM:
		p.d = code_getdd(0.0);
		prm_values[index] = &p.d;
		prm_args[index] = &ffi_type_double;
		break;
	case MPTYPE_FLOAT:
		p.f = (float)code_getdd(0.0);
		prm_values[index] = &p.f;
		prm_args[index] = &ffi_type_float;
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
		prm_values[index] = &pval_dst;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_PBMSCR:
		p.ptr = GetBMSCR();
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_FLEXSPTR:
	case MPTYPE_FLEXWPTR:
		chk = code_get();
		if ( chk<=PARAM_END ) throw ( HSPERR_NO_DEFAULT );
		mpval = *pmpval;
		switch( mpval->flag ) {
		case HSPVAR_FLAG_INT:
			p.i = *(int *)(mpval->pt);
			prm_values[index] = &p.i;
			prm_args[index] = &ffi_type_sint;
			break;
		case HSPVAR_FLAG_STR:
			p.ptr = localbuf = prepare_localstr( mpval->pt, prm->mptype == MPTYPE_FLEXWPTR );
			prm_values[index] = &p.ptr;
			prm_args[index] = &ffi_type_pointer;
			break;
		default:
			throw ( HSPERR_TYPE_MISMATCH );
		}
		break;
	case MPTYPE_PTR_REFSTR:
		p.ptr = hspctx->refstr;
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_PTR_EXINFO:
		p.ptr = exinfo;
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_PTR_DPMINFO:
		dpm_getinf( hspctx->refstr );
		p.ptr = hspctx->refstr;
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
	case MPTYPE_NULLPTR:
		p.ptr = NULL;
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
#ifndef HSP_COM_UNSUPPORTED
	case MPTYPE_IOBJECTVAR:
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		punklocal = *(IUnknown **)HspVarCorePtrAPTR( pval, aptr );
		if ( punklocal ) punklocal->AddRef();	// 呼出し後に解放する
		p.ptr = (void *)punklocal;
		prm_values[index] = &p.ptr;
		prm_args[index] = &ffi_type_pointer;
		break;
#endif
	default:
		Alertf("### Unsupported %s:%d %d\n", __func__, __LINE__, prm->mptype);
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
	}

	// 次のパラメータの取り出し（再帰的に処理）
	// (例外処理により動的確保したオブジェクトを確実に解放する)
	try {
		result = code_expand_next( prm_args, prm_values, st, index + 1 );
	}
	catch (...) {
		if ( localbuf ) sbFree( localbuf );
#ifndef HSP_COM_UNSUPPORTED
		if ( punklocal ) punklocal->Release();
#endif
		throw;
	}
	if ( localbuf ) sbFree( localbuf );
#ifndef HSP_COM_UNSUPPORTED
	if ( punklocal ) punklocal->Release();
#endif
	return result;
}

int exec_dllcmd( int cmd, int mask )
{
	STRUCTDAT *st;
	void* pFn;
	int result;

	code_next();							// 次のコードを取得(最初に必ず必要です)

	if ( cmd >= prmmax ) {
		printf("### Unsupported %s:%d %d/%d\n", __func__, __LINE__, cmd, prmmax);
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
	}

	st = GetPRM(cmd);
	pFn = st->proc;
	if ( pFn == NULL ) {
		if ( BindFUNC( st, NULL ) ) throw ( HSPERR_DLL_ERROR );
		pFn = st->proc;
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

