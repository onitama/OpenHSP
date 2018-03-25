
//
//	HSP3 External DLL manager
//	onion software/onitama 2004/6
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ocidl.h>

//	Atlのヘッダ
//#include <atlbase.h>
//#include <atlwin.h>
//#include <atlhost.h>

//	AtlをDLLからリンクする場合
#if 0
#pragma comment(lib, "atl.lib")
#include <atldef.h>
#define _ATL_DLL_IMPL
#include <atliface.h>
#endif

#include "../supio.h"
#include "../hsp3ext.h"
#include "hsp3extlib.h"
#include "../hspwnd.h"

#ifdef HSPDISH
#include "../../hsp3/strbuf.h"
#else
#include "../strbuf.h"
#include "hsp3win.h"
#endif


static HSPCTX *hspctx;		// Current Context
static HSPEXINFO *exinfo;	// Info for Plugins
static int *type;
static int *val;
static int *exflg;
static int reffunc_intfunc_ivalue;
//static PVal **pmpval;


#ifndef HSP_COM_UNSUPPORTED

#include "hspvar_comobj.h"
#include "hspvar_variant.h"
#include "comobj_event.h"

static PVal *comres_pval;
static APTR comres_aptr;

typedef void (CALLBACK *_ATXDLL_INIT)(void);
typedef HRESULT (CALLBACK *_ATXDLL_GETCTRL)( HWND, void **res );
static _ATXDLL_INIT fn_atxinit = NULL;
static _ATXDLL_GETCTRL fn_atxgetctrl = NULL;
static HINSTANCE hinst_atxdll = NULL;
static LPTSTR atxwndclass = NULL;

#endif	// !defined(HSP_COM_UNSUPPORTED)


#define GetPRM(id) (&hspctx->mem_finfo[id])
#define strp(dsptr) &hspctx->mem_mds[dsptr]

/*------------------------------------------------------------*/
/*
		ATL initialization / unitialization routines
*/
/*------------------------------------------------------------*/

#ifndef HSP_COM_UNSUPPORTED


static void InitAtxDll( void )
{
	if ( hinst_atxdll != NULL ) return;

	hinst_atxdll = LoadLibrary( TEXT("Atl71.dll") );
	if ( hinst_atxdll ) {
		atxwndclass = TEXT("AtlAxWin71");
	} else {
		hinst_atxdll = LoadLibrary( TEXT("Atl.dll") );
		if ( hinst_atxdll == NULL ) return;
		atxwndclass = TEXT("AtlAxWin");
	}

	fn_atxinit = (_ATXDLL_INIT)GetProcAddress( hinst_atxdll, "AtlAxWinInit" );
	fn_atxgetctrl = (_ATXDLL_GETCTRL)GetProcAddress( hinst_atxdll, "AtlAxGetControl" );
	if ( fn_atxinit != NULL) fn_atxinit();
}

static void TermAtxDll( void )
{
	// Atl.dll の解放
	//
	// ここで解放すると強制終了する場合があるかも
	// (変数のクリーンアップより先に実行されるのが原因？)
	//
	if ( hinst_atxdll == NULL ) return;
	FreeLibrary( hinst_atxdll );
	hinst_atxdll = NULL;
}

#endif	// !defined(HSP_COM_UNSUPPORTED)


/*------------------------------------------------------------*/
/*
		window object support
*/
/*------------------------------------------------------------*/

#ifndef HSPDISH

static BMSCR *GetBMSCR( void )
{
	HSPEXINFO *exinfo;
	exinfo = hspctx->exinfo2;
	return (BMSCR *)exinfo->HspFunc_getbmscr( *(exinfo->actscr) );
}


static void Object_WindowDelete( HSPOBJINFO *info )
{
	DestroyWindow( info->hCld );
	info->hCld = NULL;
}


// static int AddHSPObject( HWND handle, int mode, int addy, PVal *pv, APTR aptr )
static int AddHSPObject( HWND handle, int mode, int addy )
{
	//		ウインドゥオブジェクトを追加する
	//
	HSPEXINFO *exinfo;
	HSPOBJINFO obj;
	BMSCR *bm;
	int wid, entry, ppy;

	entry = -1;

	if ( handle != NULL ) {

		exinfo = hspctx->exinfo2;
		wid = *(exinfo->actscr);				// アクティブなウインドゥID
		entry = exinfo->HspFunc_addobj( wid );	// エントリーIDを得る
		bm = GetBMSCR();

		obj.hCld = handle;
		obj.owmode = mode;
		obj.option = 0;
		obj.bm = bm;
		obj.func_notice = NULL;
		obj.func_objprm = NULL;
		obj.func_delete = Object_WindowDelete;

		exinfo->HspFunc_setobj( wid, entry, &obj );

		ppy = addy; if ( ppy < bm->py ) ppy = bm->py;
		bm->cy += ppy;
	}

	hspctx->stat = entry;					// システム変数statにIDを入れる
	return entry;
}

#endif

/*------------------------------------------------------------*/
/*
		COM Object interface
*/
/*------------------------------------------------------------*/

#ifndef HSP_COM_UNSUPPORTED

// 一時領域として使用する VARIANT
static VARIANT comconv_var;

void *comget_variant( VARIANT *var, int *restype, BOOL fvariantret /* = FALSE*/ )
{
	//	VARIANT型→HSPの型に変換する
	//
	int size;
	BSTR bstr;
/*
	rev 43
	mingw : warning : 定数へのポインタを非定数へのポインタに代入
	に対処
*/
	void const *ptr;
	if ( fvariantret ) {
		VariantCopy( &comconv_var, var );
		*restype = HSPVAR_FLAG_VARIANT;
		return &comconv_var;
	}

	VariantCopyInd( &comconv_var, var );
	switch ( comconv_var.vt ) {

	case VT_R4:
		VariantChangeType( &comconv_var, &comconv_var, VARIANT_NOVALUEPROP, VT_R8 );
	case VT_R8:
		*restype = HSPVAR_FLAG_DOUBLE;
		return &comconv_var.dblVal;

	case VT_BSTR:
		// 文字列全体を返すため、ANSI文字列をバイナリデータBSTRとして格納
		ptr = comconv_var.bstrVal;
		if ( ptr == NULL ) ptr = L"";
#ifndef HSPUTF8
		size = cnvsjis( NULL, (char *)ptr, 0 );
#else
		size = cnvu8( NULL, (HSPAPICHAR *)ptr, 0);
#endif
		bstr = SysAllocStringByteLen( NULL, size );
		if ( bstr == NULL ) throw HSPERR_OUT_OF_MEMORY;
#ifndef HSPUTF8
		cnvsjis( (char *)bstr, (char *)ptr, size );
#else
		cnvu8( (char *)bstr, (HSPAPICHAR *)ptr, size);
#endif
		SysFreeString( comconv_var.bstrVal );
		comconv_var.bstrVal = bstr;
		*restype = HSPVAR_FLAG_STR;
		return comconv_var.bstrVal;

	case VT_DISPATCH:
	case VT_UNKNOWN:
		*restype = HSPVAR_FLAG_COMSTRUCT;
		return &comconv_var.punkVal;

	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
	case VT_I8:
	case VT_UI8:
	case VT_BOOL:
		VariantChangeType( &comconv_var, &comconv_var, VARIANT_NOVALUEPROP, VT_I4 );
	case VT_I4:
	case VT_UI4:
	case VT_ERROR:
	case VT_INT:
	case VT_UINT:
		*restype = HSPVAR_FLAG_INT;
		return &comconv_var.lVal;

	default:
		if ( comconv_var.vt & VT_ARRAY ) {
			*restype = HSPVAR_FLAG_VARIANT;		// SafeArray は Variant 型変数で扱う
			return &comconv_var;
		}
		throw HSPERR_INVALID_TYPE;
	}
}

void comset_variant( VARIANT *var, void *data, int vtype )
{
	//		HSPの型→VARIANT 型に変換する
	//
	IUnknown *punk;

	VariantClear( var );
	switch( vtype ) {
	case HSPVAR_FLAG_INT:
		var->vt = VT_I4;
		var->lVal = *(int *)data;
		break;
	case HSPVAR_FLAG_DOUBLE:
		var->vt = VT_R8;
		var->dblVal = *(double *)data;
		break;
	case HSPVAR_FLAG_STR:
		var->vt = VT_BSTR;
		var->bstrVal = comget_bstr( (char *)data );
		break;
	case HSPVAR_FLAG_COMSTRUCT:
		var->vt = VT_UNKNOWN;
		var->punkVal = NULL;
		punk = *(IUnknown **)data;
		if ( punk == NULL ) break;
		punk->QueryInterface( IID_IDispatch, (void**)&var->pdispVal );
		if ( var->pdispVal != NULL ) {
			var->vt = VT_DISPATCH;
		} else {
			var->punkVal = punk;
			punk->AddRef();
		}
		break;
	case HSPVAR_FLAG_VARIANT:
		VariantCopy( var, (VARIANT *)data );
		break;
	default:
		throw HSPERR_INVALID_TYPE;
	}
}

void comget_variantres( VARIANT *var, HRESULT hr, BOOL noconv )
{
	//		返値のVARIANTを変数に反映させる
	//
	void *ptr;
	int restype;
	hspctx->stat = (int)hr;
	if ( comres_pval == NULL ) return;
	if ( noconv ) {
		// Variant のまま返す
		code_setva( comres_pval, comres_aptr, HSPVAR_FLAG_VARIANT, var );
	} else {
		// 変数を対応する型に変換
		if ( var->vt != VT_EMPTY ) {
			ptr = comget_variant( var, &restype );
			code_setva( comres_pval, comres_aptr, restype, ptr );
		}
	}
}

DISPID get_dispid( IUnknown* punk, char *propname, BOOL *bVariant )
{
	//		IDispatch の DISPID を取得する
	//
	HRESULT hr;
	IDispatch *disp;
	DISPID dispid;
	char *name = propname;
	LPOLESTR list[]={ (LPOLESTR)(hspctx->stmp) };

	if ( name[0] == '\0' ) return DISPID_VALUE;

	if ( bVariant != NULL ) *bVariant = ( name[0] == '.' );
	if ( name[0] == '.' ) name++;

	disp = (IDispatch *)punk;
	cnvwstr( hspctx->stmp, name, HSPCTX_REFSTR_MAX/sizeof(WCHAR)-1 );
	hr = disp->GetIDsOfNames( IID_NULL, list, 1, LOCALE_SYSTEM_DEFAULT, &dispid );

	if ( FAILED(hr) || dispid == DISPID_UNKNOWN ) throw HSPERR_INVALID_PARAMETER;
	return dispid;
}

BSTR comget_bstr( char *ps )
{
	int size;
	BSTR bstr;
	void *temp;
	size = cnvwstr( NULL, ps, 0 ) + 1;
	if ( size * sizeof(WCHAR) > HSPCTX_REFSTR_MAX ) {
		temp = sbAlloc( size * sizeof(WCHAR) );
		cnvwstr( temp, ps, size );
		bstr = SysAllocString( (LPOLESTR)temp );
		sbFree( temp );
	} else {
		cnvwstr( hspctx->stmp, ps, size );
		bstr = SysAllocString( (LPOLESTR)hspctx->stmp );
	}
	return bstr;
}


int call_method( void *iptr, int index, int *prm, int count )
{
	int *proc;
	proc = (*(int **)iptr);
	proc += index;
	//Alertf( "%x:%x:%d",proc,*proc,index );
	return call_extfunc( (void*)*proc, prm, count );
}

int call_method2( char *prmbuf, const STRUCTDAT *st )
{
	// 指定 IID を取得してメソッド呼び出し
	// (code_expand_next() から呼ばれる)
	//
	const LIBDAT *lib;
	const IID *piid;
	IUnknown *punk, *punk2;
	int result;
	HRESULT hr;
	// IID 情報を取ってくる
	lib = &hspctx->mem_linfo[ st->index ];
	piid = (IID *)strp( lib->nameidx );
	punk = *(IUnknown **)prmbuf;
	if ( st->otindex < 0 || punk == NULL ) throw ( HSPERR_COMDLL_ERROR );
	hr = punk->QueryInterface( *piid, (void**)&punk2 );
	if ( FAILED(hr) || punk2 == NULL ) throw ( HSPERR_COMDLL_ERROR );
	*(IUnknown **)prmbuf = punk2;
	result = call_method( punk2, st->otindex, (int*)prmbuf, st->size / 4 );
	punk2->Release();
	return result;
}

static BOOL GetIIDFromString( IID *iid, char *ps, bool fClsid = false )
{
	//		SJIS文字列 IID から IID 構造体を得る
	//
	//		fClsid が真のとき、ProgID からの CLSID 取得を試みる
	//		ProgID の変換に失敗した場合は FALSE を返す
	//		(それ以外のエラーはエラーを throw)
	//
	HRESULT hr;
	cnvwstr( hspctx->stmp, ps, HSPCTX_REFSTR_MAX/sizeof(WCHAR)-1 );
	if ( *ps == '{' ) {
		// GUID 文字列 → GUID 構造体
		hr = IIDFromString( (LPOLESTR)hspctx->stmp, iid );
		if ( SUCCEEDED(hr) ) return TRUE;
	}
	if ( fClsid ) {
		// ProgID 文字列 → GUID 構造体
		hr = CLSIDFromProgID( (LPOLESTR)hspctx->stmp, iid );
		if ( SUCCEEDED(hr) ) return TRUE;
		return FALSE;
	}
	throw ( HSPERR_ILLEGAL_FUNCTION );
	return FALSE;
}

#endif	// !defined(HSP_COM_UNSUPPORTED)


/*------------------------------------------------------------*/
/*
		type function
*/
/*------------------------------------------------------------*/

static int cmdfunc_ctrlcmd( int cmd )
{
	//		cmdfunc : TYPE_DLLCTRL
	//		(拡張DLLコントロールコマンド)
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)

	if ( cmd >= TYPE_OFFSET_COMOBJ ) {
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		// COM インターフェースメソッドの呼び出し
		STRUCTDAT *st;
		st = GetPRM( cmd - TYPE_OFFSET_COMOBJ );
		hspctx->stat = code_expand_and_call( st );
		return RUNMODE_RUN;
#endif
	}

	switch( cmd ) {							// サブコマンドごとの分岐


	case 0x00:								// newcom
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval;
		APTR aptr;
		IUnknown **ppunkNew, *punkDef;
		CLSID clsid;
		char *clsid_name;
		const IID *piid, *piid2;
		void *iptr;
		LIBDAT *lib;
		STRUCTDAT *st;
		int inimode;

		// 第１パラメータ：新しいインターフェースポインタを格納する変数
		// (変数にNULLポインタを格納)
		aptr = code_getva( &pval );
		iptr = NULL;
		code_setva( pval, aptr, TYPE_COMOBJ, &iptr );
		ppunkNew = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );

		// CLSID / IID 情報を取得
		if ( !code_getexflg() && *type == TYPE_DLLCTRL ) {
			// 第２パラメータ：#usecom 登録情報
			st = code_getcomst();
			if ( st->otindex != -1 ) throw HSPERR_TYPE_MISMATCH;
			lib = &hspctx->mem_linfo[ st->index ];
			if ( lib->clsid == -1 ) throw HSPERR_INVALID_PARAMETER;
			clsid_name = strp(lib->clsid);
			piid  = (IID *)strp( lib->nameidx );
			piid2 = NULL;
		} else {
			// 第２パラメータ：文字列 CLSID
			// (IID はデフォルトで IID_IDispatch, サポートされていなければ IID_IUnknown )
			clsid_name = code_getds("");
			piid  = &IID_IDispatch;
			piid2 = &IID_IUnknown;
		}
		inimode = code_getdi(0);				// 初期化モード
		punkDef = (IUnknown *)code_getdi(0);	// デフォルトオブジェクト

		// 新規CLSIDからインスタンスを作成
		hspctx->stat = 0;
		switch ( inimode ) {
		 case 0:
			// 新規にロード
			if ( clsid_name[0]!='\0' ) {
				if ( GetIIDFromString(&clsid,clsid_name,true) != FALSE &&
					 SUCCEEDED( CoCreateInstance( clsid, NULL, CLSCTX_SERVER, *piid, (void**)ppunkNew )) &&
					 *ppunkNew != NULL )
				{
					break;
				}
				if ( piid2 != NULL &&
					 SUCCEEDED( CoCreateInstance( clsid, NULL, CLSCTX_SERVER, *piid2, (void**)ppunkNew )) &&
					 *ppunkNew != NULL )
				{
					break;
				}
				hspctx->stat = 1;
			}
			break;
		 case -2:
			// オブジェクトを明示的に指定する場合 ( AddRef() あり)
			if ( punkDef != NULL ) punkDef->AddRef();
		 case -1:
			// オブジェクトを明示的に指定する場合 ( AddRef() なし)
			*ppunkNew = punkDef;
			break;
		 default:
			throw HSPERR_UNSUPPORTED_FUNCTION;
		}
	#ifdef HSP_COMOBJ_DEBUG
		COM_DBG_MSG( "newcom : pObj=%p : &pObj=%p\n", *ppunkNew, ppunkNew);
	#endif
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x01:								// querycom
#ifdef HSP_COM_UNSUPPORTED
		throw HSPERR_UNSUPPORTED_FUNCTION;
#else
		{
		PVal *pval, *pvalNew;
		APTR aptr, aptrNew;
		IUnknown **ppunkDst, **ppunkSrc;
		IID iid;
		const IID *piid;
		void *iptr;
		STRUCTDAT *st;
		LIBDAT *lib;

		// 第１パラメータ：新しいインターフェースポインタを格納する変数
		aptrNew = code_getva( &pvalNew );

		// 第２パラメータ：既存のCOMオブジェクト
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		ppunkSrc = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );
		if ( ! IsVaridComPtr(ppunkSrc) ) throw ( HSPERR_COMDLL_ERROR );

		// IID 情報を取得
		if ( *type == TYPE_DLLCTRL ) {
			// 第３パラメータ：#usecom 登録情報
			st = code_getcomst();
			if ( st->otindex != -1 ) throw ( HSPERR_TYPE_MISMATCH );
			lib = &hspctx->mem_linfo[ st->index ];
			piid = (IID *)strp( lib->nameidx );
		} else {
			// 第３パラメータ：文字列 IID
			GetIIDFromString( &iid, code_gets() );
			piid = &iid;
		}

		if ( pvalNew->flag != TYPE_COMOBJ ) {
			// 代入により型変換
			iptr = NULL;
			code_setva( pvalNew, aptrNew, TYPE_COMOBJ, &iptr );
		}
		ppunkDst = (IUnknown **)HspVarCorePtrAPTR( pvalNew, aptrNew );

		// query によりインスタンスを得る
		QueryComPtr( ppunkDst, *ppunkSrc, piid );
		if ( IsVaridComPtr(ppunkSrc) )
			hspctx->stat = 0;
		else
			hspctx->stat = 1;
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x02:								// delcom
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval;
		APTR aptr;
		IUnknown **ppunk;
		VARIANT *var;
		void *ptr;

		// 第１パラメータ：解放するCOMオブジェクト変数
		aptr = code_getva( &pval );
		ptr = HspVarCorePtrAPTR( pval, aptr );
		switch ( pval->flag ) {
		case TYPE_COMOBJ:
			ppunk = (IUnknown **)ptr;
			ReleaseComPtr( ppunk );
			break;
		case TYPE_VARIANT:
			var = (VARIANT *)ptr;
			VariantClear( var );
			break;
		default:
			throw HSPERR_TYPE_MISMATCH;
		}

		// このタイミングで一時オブジェクトも削除しちゃう
		if ( comconv_var.vt != VT_EMPTY ) VariantClear( &comconv_var );
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x03:								// 	cnvstow
		{
#ifndef HSPUTF8
		PVal *pval;
		char *ptr;
		char *ps;
		int size;
		ptr = code_getvptr( &pval, &size );
		ps = code_gets();
		cnvwstr( ptr, ps, size/2 );
		break;
#else
        PVal *pval;
        char *ptr;
        char *ps;
        int size;
        int sizew;
		HSPAPICHAR *hactmp1 = 0;
        ptr = code_getvptr(&pval, &size);
        ps = code_gets();
        chartoapichar(ps, &hactmp1);
        sizew = wcslen(hactmp1) + 1;
        if (size <= sizew*(int)sizeof(HSPAPICHAR)){
            memcpy(ptr, hactmp1, size);
			((HSPAPICHAR*)ptr)[size - 1] = TEXT('\0');
            hspctx->stat = -sizew*sizeof(HSPAPICHAR);
		}
        else{
            memcpy(ptr, hactmp1, (sizew - 1)*sizeof(HSPAPICHAR));
			((HSPAPICHAR*)ptr)[sizew - 1] = TEXT('\0');
            hspctx->stat = sizew*sizeof(HSPAPICHAR);
		}
        freehac(&hactmp1);
        break;
#endif
		}

	case 0x04:								// 	comres
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		if ( code_getexflg() ) {
			comres_pval = NULL;
			comres_aptr = 0;
		} else {
			comres_aptr = code_getva( &comres_pval );
		}
		break;
#endif	// HSP_COM_UNSUPPORTED

	case 0x05:								// 	axobj
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval;
		APTR aptr;
		const IID *piid;
		void *iptr;
		BMSCR *bm;
		HWND hwnd;
		int id,sx,sy;
		char clsid_name8[1024];
		HSPAPICHAR *clsid_name = 0;
		IUnknown **ppunk, *punkObj, *punkObj2;
		STRUCTDAT *st;
		LIBDAT *lib;
		HRESULT hr;

		bm = GetBMSCR();

		// 第１パラメータ：新しいインターフェースポインタを格納する変数
		// (あらかじめ変数にNULLを格納)
		aptr = code_getva( &pval );
		iptr = NULL;
		code_setva( pval, aptr, TYPE_COMOBJ, &iptr );
		ppunk = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );

		// オブジェクトの CLSID, ProgID, etc.
		if ( *type == TYPE_DLLCTRL ) {
			// 第２パラメータ：#usecom 登録情報から取得
			st = code_getcomst();
			if ( st->otindex != -1 ) throw ( HSPERR_TYPE_MISMATCH );
			lib = &hspctx->mem_linfo[ st->index ];
			piid = (IID *)strp( lib->nameidx );
			if ( lib->clsid == -1 ) throw ( HSPERR_INVALID_PARAMETER );
			strncpy( clsid_name8, strp(lib->clsid), sizeof(clsid_name8)-1 );
		} else {
			// 第２パラメータ：文字列 CLSID or ProgID を取得 (IID は IDispatch)
			piid = &IID_IDispatch;
			strncpy( clsid_name8, code_gets(), sizeof(clsid_name8)-1 );
		}

		// コントロールのサイズ
		sx = code_getdi( bm->sx );
		sy = code_getdi( bm->sy );

		//		ActiveXとしてロード
		//
		if ( fn_atxinit == NULL ) throw ( HSPERR_UNSUPPORTED_FUNCTION );
		hwnd = CreateWindow( atxwndclass, chartoapichar(clsid_name8,&clsid_name),
				WS_CHILD, 			// 最初は WS_VISIBLE なし (後で ShowWindow() )
				bm->cx, bm->cy, sx, sy,
				bm->hwnd, (HMENU)0, (HINSTANCE)hspctx->instance, NULL );
		freehac(&clsid_name);

		punkObj2 = NULL;
		if ( hwnd ) {
			punkObj = NULL;
			fn_atxgetctrl( hwnd, (void**)&punkObj );
			if ( punkObj ) {
				// 指定 IID が存在するかどうか
				hr = punkObj->QueryInterface( *piid, (void**)&punkObj2 );
				punkObj->Release();
			}
		}
		if ( punkObj2 == NULL ) {
			// 目的オブジェクトではないときコントロールを削除
			if (hwnd) { DestroyWindow( hwnd ); }
			hspctx->stat = -1;
			break;
		}
		// COM 型変数に格納
		*ppunk = punkObj2;

		// HSPのウインドゥオブジェクトとして登録する
		ShowWindow( hwnd, SW_SHOW );
		id = AddHSPObject( hwnd, HSPOBJ_TAB_SKIP, sy );
#ifdef HSP_COMOBJ_DEBUG
		Alertf( "axobj : pObj=%p : &pObj=%p\n", *ppunk, ppunk);
	#endif
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x06:								// 	winobj
		{
#ifdef HSPDISH
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		char clsname8[1024];
		HSPAPICHAR *clsname = 0;
		char winname8[1024];
		HSPAPICHAR *winname = 0;
		HWND hwnd;
		char *ps;
		BMSCR *bm;
		int i;
		int prm[6];

		ps = code_gets(); strncpy( clsname8, ps, 1023 );
		ps = code_gets(); strncpy( winname8, ps, 1023 );

		bm = GetBMSCR();
		for(i=0;i<6;i++) {
			prm[i] = code_getdi(0);
		}
		if ( prm[2] <= 0 ) prm[2] = bm->ox;
		if ( prm[3] <= 0 ) prm[3] = bm->oy;

		hwnd = CreateWindowEx(
		    (DWORD) prm[0],			// 拡張ウィンドウスタイル
		    chartoapichar(clsname8,&clsname),	// ウィンドウクラス名
		    chartoapichar(winname8,&winname),	// ウィンドウ名
		    (DWORD) prm[1],			// ウィンドウスタイル
			bm->cx, bm->cy, prm[2], prm[3],		// X,Y,SIZEX,SIZEY
			bm->hwnd,				// 親ウィンドウのハンドル
		    (HMENU) prm[4],			// メニューハンドルまたは子ウィンドウID
			bm->hInst,				// インスタンスハンドル
		    (PVOID) prm[5]			// ウィンドウ作成データ
			);
			freehac(&clsname);
			freehac(&winname);

		// AddHSPObject( hwnd, HSPOBJ_TAB_SKIP, prm[3], NULL, 0 );			// HSPのウインドゥオブジェクトとして登録する
		AddHSPObject( hwnd, HSPOBJ_TAB_SKIP, prm[3] );
		break;
#endif	// HSPDISH
		}

	case 0x07:								// 	sendmsg
		{
		int p1;
		WPARAM p2;
		LPARAM p3;
		HWND hw;
		int fl;
		char *vptr;
		HSPAPICHAR *hactmp1 = 0;
		HSPAPICHAR *hactmp2 = 0;
		hw = (HWND)code_getdi(0);
		p1 = code_getdi(0);

		vptr = code_getsptr( &fl );
		if ( fl == TYPE_STRING ) {
			p2 = (WPARAM)chartoapichar(vptr,&hactmp2);
		} else {
			p2 = *(WPARAM *)vptr;
		}

		vptr = code_getsptr( &fl );
		if ( fl == TYPE_STRING ) {
			p3 = (LPARAM)chartoapichar(vptr,&hactmp2);
		} else {
			p3 = *(LPARAM *)vptr;
		}

		//Alertf( "SEND[%x][%x][%x]",p1,p2,p3 );
		hspctx->stat = (int)SendMessage( hw, p1, p2, p3 );
		freehac(&hactmp1);
		freehac(&hactmp2);
		break;
		}

	case 0x08:								// 	comevent
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval;
		APTR aptr;
		void* iptr;
		char *ps;
		IID iid, *piid;
		unsigned short *subr;
		IUnknown **ppunk, **ppunkEvent;

		// 第１パラメータ：イベントハンドラオブジェクト (IEventHandler) を格納する変数
		// (あらかじめ NULL で初期化)
		aptr = code_getva( &pval );
		iptr = NULL;
		code_setva( pval, aptr, TYPE_COMOBJ, &iptr );
		ppunkEvent = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );

		// 第２パラメータ：COMオブジェクトを格納した変数
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		ppunk = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );
		if ( ! IsVaridComPtr(ppunk) ) throw ( HSPERR_COMDLL_ERROR );

		// 第３パラメータ：コネクションポイントIID (文字列形式)
		ps = code_getds("");
		if ( ps[0] != '\0' ) {
			piid = &iid;
			GetIIDFromString( piid, ps );
		} else {
			piid = NULL;	// NULL のときデフォルトIID が自動的に取得される
		}

		// 第４パラメータ：コールバック用のサブルーチンラベル
		subr = code_getlb2();

		// イベントハンドラ作成・接続
		SetComEvent( ppunkEvent, ppunk, piid, subr );
	#ifdef HSP_COMOBJ_DEBUG
		COM_DBG_MSG( "comevent : pEvent=%p : pObj=%p\n", *ppunkEvent, *ppunk);
	#endif
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x09:								// 	comevarg
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval, *pval2;
		APTR aptr, aptr2;
		VARIANT *v;
		void *ptr;
		int p1,p2;
		int res;
		IUnknown **ppunk;
		VARIANT varTemp;

		// 第１パラメータ：イベントのパラメータを格納する変数
		aptr = code_getva( &pval );

		// 第２パラメータ：イベントハンドラオブジェクト変数
		aptr2 = code_getva( &pval2 );
		if ( pval2->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		ppunk = (IUnknown **)HspVarCorePtrAPTR( pval2, aptr2 );
		if ( ! IsVaridComPtr(ppunk) ) throw ( HSPERR_COMDLL_ERROR );

		// 第３パラメータ：パラメータインデックス
		p1 = code_getdi(0);

		// 第４パラメータ：文字列変換フラグ
		p2 = code_getdi(0);

		// イベントのパラメータ取得
		v = GetEventArg( *ppunk, p1 );
		if ( v == NULL ) throw ( HSPERR_ILLEGAL_FUNCTION );
		switch ( p2 ) {
		case 0:
			VariantInit( &varTemp );
			VariantCopyInd( &varTemp, v );
			ptr = comget_variant( &varTemp, &res );
			VariantClear( &varTemp );
			break;
		case 1:
			VariantInit( &varTemp );
			if FAILED( VariantChangeType( &varTemp, v, VARIANT_ALPHABOOL, VT_BSTR ) )
				throw ( HSPERR_TYPE_INITALIZATION_FAILED );
			ptr = comget_variant( &varTemp, &res );
			VariantClear( &varTemp );
			break;
		case 2:
			ptr = v;
			res = HSPVAR_FLAG_VARIANT;
			break;
		default:
			throw ( HSPERR_ILLEGAL_FUNCTION );
		}
		code_setva( pval, aptr, res, ptr );
		hspctx->stat = res;
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x0a:								// 	sarrayconv
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval1, *pval2;
		APTR aptr1, aptr2;
		int convdir, size;
		VARIANT *variant, varTemp;
		VARTYPE vt;
		SAFEARRAY *psa;
		long lbound, ubound;
		HRESULT hr;

		aptr1 = code_getva( &pval1 );
		aptr2 = code_getva( &pval2 );
		convdir = code_getdi(0);
		size = code_getdi(0);

		switch ( convdir ) {
		case 0:
		case 2:
		case 4:
			// 配列変数から SafeArray に変換
			VariantInit( &varTemp );
			code_setva( pval1, aptr1, HSPVAR_FLAG_VARIANT, &varTemp );
			variant = (VARIANT *)HspVarCorePtrAPTR( pval1, aptr1 );
			VariantClear( variant );		// 一応
			if ( convdir == 2 ) {
				// バイナリデータ（一次元のみ）
				void *ptr = HspVarCorePtrAPTR( pval2, aptr2 );
				psa = CreateBinarySafeArray( ptr, size, &vt );
			} else {
				BOOL bVariant = ( convdir == 4 );
				psa = ConvVar2SafeArray( pval2, bVariant, &vt );
			}
			variant->vt = vt | VT_ARRAY;
			variant->parray = psa;
			break;
		case 1:
		case 3:
			// SafeArray から配列変数に変換
			if ( pval2->flag != HSPVAR_FLAG_VARIANT ) throw HSPERR_INVALID_TYPE;
			variant = (VARIANT *)HspVarCorePtrAPTR( pval2, aptr2 );
			if ( (variant->vt & VT_ARRAY) == 0 ) throw HSPERR_INVALID_TYPE;
			psa = variant->parray;
			if ( psa == NULL ) throw HSPERR_ARRAY_OVERFLOW;
			vt = variant->vt & VT_TYPEMASK;
			if ( vt == VT_EMPTY ) {
				hr = SafeArrayGetVartype( psa, &vt );
				if ( FAILED(hr) || vt == VT_EMPTY ) throw HSPERR_INVALID_ARRAYSTORE;
			}
			if ( convdir == 1 ) {
				ConvSafeArray2Var( pval1, psa, vt );
			} else {
				// バイナリデータ（一次元のみ）
				int varsize;
				void *ptr = HspVarCorePtrAPTR( pval1, aptr1 );				if ( vt != VT_UI1 && vt != VT_I1 ) throw HSPERR_INVALID_TYPE;
				SafeArrayGetLBound( psa, 1, &lbound );
				hr = SafeArrayGetUBound( psa, 1, &ubound );
				if ( FAILED(hr) ) throw HSPERR_ARRAY_OVERFLOW;
				size = ubound - lbound + 1;
				HspVarCoreGetBlockSize( pval1, (PDAT*)ptr, &varsize );
				if ( varsize < size ) throw HSPERR_BUFFER_OVERFLOW;
				GetBinarySafeArray( ptr, size, psa );
			}
			break;
		default:
			throw ( HSPERR_UNSUPPORTED_FUNCTION );
		}
		break;
		}
#endif	// HSP_COM_UNSUPPORTED
	case 0x0c:								//  cnvstoa
#ifndef HSPUTF8
		throw (HSPERR_UNSUPPORTED_FUNCTION);
#else
		{
		PVal *pval;
		char *ptr;
		char *ps;
		int size;
		int sizea;
		HSPAPICHAR *hactmp1 = 0;
		char *actmp1 = 0;
		ptr = code_getvptr(&pval, &size);
		ps = code_gets();
		chartoapichar(ps, &hactmp1);
		apichartoansichar(hactmp1, &actmp1);
		sizea = strlen(actmp1) + 1;
		if (size <= sizea){
			memcpy(ptr, actmp1, size - 1);
			((char*)ptr)[size - 1] = '\0';
			hspctx->stat = -sizea;
		}
		else{
			memcpy(ptr, actmp1, sizea - 1);
			((char*)ptr)[sizea - 1] = '\0';
			hspctx->stat = sizea;
		}
		freeac(&actmp1);
		freehac(&hactmp1);
		break;
		}
#endif
	default:
		throw ( HSPERR_SYNTAX );
	}
	return RUNMODE_RUN;
}


static void *reffunc_ctrlfunc( int *type_res, int arg )
{
	//		reffunc : TYPE_DLLCTRL
	//		(拡張DLLコントロール関数)
	//
	void *ptr;
	int p1,p2;

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != '(' ) throw ( HSPERR_INVALID_FUNCPARAM );
	code_next();

	ptr = &reffunc_intfunc_ivalue;
	*type_res = HSPVAR_FLAG_INT;

	switch( arg ) {							// サブコマンドごとの分岐
	case 0x100:								// callfunc
		{
		PVal *pval;
		PDAT *p;
		pval = code_getpval();
		p = HspVarCorePtrAPTR( pval, 0 );
		p1 = code_geti();
		p2 = code_geti();
		reffunc_intfunc_ivalue = call_extfunc( (void *)p1, (int *)p, p2 );
		break;
		}
	case 0x101:								// cnvwtos
		{
#ifndef HSPUTF8
		PVal *pval;
		char *sptr;
		int size;
		sptr = code_getvptr( &pval, &size );
		hspctx->stmp = sbExpand( hspctx->stmp, size );
		ptr = hspctx->stmp;
		cnvsjis( ptr, sptr, size  );
		*type_res = HSPVAR_FLAG_STR;
		break;
#else
        PVal *pval;
        wchar_t *sptr;
        int size;
		int len;
        sptr = (wchar_t*)code_getvptr(&pval, &size);
		len = lstrlen(sptr) * 6 + 1;
        hspctx->stmp = sbExpand(hspctx->stmp, len);
        ptr = hspctx->stmp;
        cnvu8(ptr, sptr, len);
        *type_res = HSPVAR_FLAG_STR;
        break;
#endif
		}

	case 0x102:								// 	comevdisp
#ifdef HSP_COM_UNSUPPORTED
		throw ( HSPERR_UNSUPPORTED_FUNCTION );
#else
		{
		PVal *pval;
		APTR aptr;
		IUnknown **ppunk;
		// 第１パラメータ：COMオブジェクト変数
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_COMOBJ ) throw ( HSPERR_TYPE_MISMATCH );
		ppunk = (IUnknown **)HspVarCorePtrAPTR( pval, aptr );
		if ( !IsVaridComPtr(ppunk) ) throw ( HSPERR_COMDLL_ERROR );

		// イベントの DISPID 取得
		reffunc_intfunc_ivalue = GetEventDispID( *ppunk );
		break;
		}
#endif	// HSP_COM_UNSUPPORTED

	case 0x103:								// 	libptr
		{
		//LIBDAT *lib;
		STRUCTDAT *st;
		switch( *type ) {
		case TYPE_DLLFUNC:
		case TYPE_MODCMD:
			p1 = *val;
			break;
		case TYPE_DLLCTRL:
			p1 = *val;
			if ( p1 >= TYPE_OFFSET_COMOBJ ) {
				p1 -= TYPE_OFFSET_COMOBJ;
				break;
			}
		default:
			throw ( HSPERR_TYPE_MISMATCH );
		}
		code_next();
		st = GetPRM( p1 );
		//lib = &hspctx->mem_linfo[ st->index ];
		reffunc_intfunc_ivalue = (int)((INT_PTR)st);
		break;
		}

	case 0x104:								//  cnvatos
#ifndef HSPUTF8
		throw (HSPERR_UNSUPPORTED_FUNCTION);
#else
		{
			PVal *pval;
			char *sptr;
			int size;
			HSPAPICHAR *hactmp1 = 0;
			int len;
			sptr = code_getvptr(&pval, &size);
			ansichartoapichar(sptr, &hactmp1);
			len = lstrlen(hactmp1) * 6 + 1;
			hspctx->stmp = sbExpand(hspctx->stmp, len);
			ptr = hspctx->stmp;
			cnvu8(ptr, hactmp1, len);
			freehac(&hactmp1);
			*type_res = HSPVAR_FLAG_STR;
			break;
		}
#endif

	default:
		throw ( HSPERR_SYNTAX );
	}

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != ')' ) throw ( HSPERR_INVALID_FUNCPARAM );
	code_next();

	return ptr;
}


static void *reffunc_dllcmd( int *type_res, int arg )
{
	//		reffunc : TYPE_DLLFUNC
	//		(拡張DLL関数)
	//

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != '(' ) throw ( HSPERR_INVALID_FUNCPARAM );

	*type_res = HSPVAR_FLAG_INT;
	exec_dllcmd( arg, STRUCTDAT_OT_FUNCTION );
	reffunc_intfunc_ivalue = hspctx->stat;

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != ')' ) throw ( HSPERR_INVALID_FUNCPARAM );
	code_next();

	return &reffunc_intfunc_ivalue;
}


static int termfunc_dllcmd( int option )
{
	//		termfunc : TYPE_DLLCMD
	//
#ifndef HSP_COM_UNSUPPORTED
	VariantClear( &comconv_var );
	// TermAtxDll();
#endif

	Hsp3ExtLibTerm();
	return 0;
}

void hsp3typeinit_dllcmd( HSP3TYPEINFO *info )
{
#ifndef HSP_COM_UNSUPPORTED
#ifdef HSP_COMOBJ_DEBUG
	if (fpComDbg == NULL) fpComDbg = fopen( "comobj_dbg.txt", "w");
#endif
	InitAtxDll();
	VariantInit( &comconv_var );
	comres_pval = NULL;
#endif	// !defined( HSP_COM_UNSUPPORTED )

	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	exflg = exinfo->npexflg;

	info->cmdfunc = cmdfunc_dllcmd;
	info->reffunc = reffunc_dllcmd;
	info->termfunc = termfunc_dllcmd;

	Hsp3ExtLibInit( info );
}

void hsp3typeinit_dllctrl( HSP3TYPEINFO *info )
{
	info->cmdfunc = cmdfunc_ctrlcmd;
	info->reffunc = reffunc_ctrlfunc;
}

