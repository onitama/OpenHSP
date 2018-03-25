/*------------------------------------------------------------*/
/*
		comdata.cpp
*/
/*------------------------------------------------------------*/


#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ocidl.h>

#include "../hsp3code.h"

#include "comobj.h"
#include "comobj_event.h"

#include "../hsp3ext.h"
#include "../strbuf.h"

/*-----------------------------------------------------------------------------------*/
/*
			ComObj interface
*/
/*-----------------------------------------------------------------------------------*/

#ifdef HSP_COMOBJ_DEBUG
FILE *fpComDbg;

void COM_DBG_MSG( const char *sz, ... )
{
	va_list args;
	va_start(args, sz);
	vfprintf(fpComDbg, sz, args);
	va_end(args);
	fflush(fpComDbg);
}
#endif

// オブジェクト解放
void ReleaseComPtr( IUnknown** ppunk )
{
	HRESULT hr;
	IEventHandler *event;

	if ( ! IsVaridComPtr(ppunk) ) return;

	hr = (*ppunk)->QueryInterface( IID_IEventHandler, (void**)&event );
	if ( SUCCEEDED(hr) && event != NULL ) {
		// イベントオブジェクトの場合は内部カウンタをデクリメント
		event->DecInnerRef();
		event->Release();
	}
	UINT ref = (*ppunk)->Release();
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "ReleaseComPtr() : pObj=0x%p is released. : m_ref=%d\n", *ppunk, ref );
#endif
	*ppunk = NULL;
}

// QueryInterface
void QueryComPtr( IUnknown **ppunkDest, IUnknown *punkSrc, const IID *iid )
{
	IEventHandler *event;
	IUnknown *punkNew;
	HRESULT hr;
	if ( punkSrc == NULL ) {
		ReleaseComPtr( ppunkDest );
		return;
	}
	if ( iid != NULL ) {
		// iid が有効ポインタの場合は QueryInterface()
		hr = punkSrc->QueryInterface( *iid, (void**)&punkNew );
		ReleaseComPtr( ppunkDest );
		*ppunkDest = ( SUCCEEDED(hr) ? punkNew : NULL );
	} else {
		// iid が NULL の場合は単なるコピー
		punkSrc->AddRef();		// 先に AddRef() しておく（querycom に同じ変数を指定した場合に備えて）
		ReleaseComPtr( ppunkDest );
		*ppunkDest = punkNew = punkSrc;
	}
	// イベントオブジェクトの場合は内部カウンタをインクリメント
	if ( punkNew ) {
		hr = punkNew->QueryInterface( IID_IEventHandler, (void**)&event );
		if ( SUCCEEDED(hr) && event != NULL ) {
			event->IncInnerRef();
			event->Release();
		}
	}
#ifdef HSP_COMOBJ_DEBUG
	if ( punkNew ) {
		punkNew->AddRef();
		UINT ref = (short)punkNew->Release();
		COM_DBG_MSG( "Query/CopyComPtr() : pObj=%p : &pObj=%p : m_ref=%d\n", punkNew, ppunkDest, ref);
	}
#endif
}

// COMイベントを設定
void SetComEvent( IUnknown **ppEv, IUnknown **ppunk, const IID *pCPGuid, unsigned short *callback )
{
	IEventHandler *event;
	ReleaseComPtr( ppEv );
	if ( ! IsVaridComPtr(ppunk) ) throw ( HSPERR_COMDLL_ERROR );
	event = CreateEventHandler( *ppunk, pCPGuid, callback );
	if ( event == NULL ) throw ( HSPERR_COMDLL_ERROR );
	event->IncInnerRef();
	*ppEv = static_cast<IUnknown*>(event);
}

// COMイベントの設定を解除（現在はこの関数を使用していない）
void ReleaseComEvent( IUnknown **ppEv )
{
	IEventHandler *event;
	HRESULT hr;
	if ( ! IsVaridComPtr(ppEv) ) throw ( HSPERR_COMDLL_ERROR );
	hr = (*ppEv)->QueryInterface( IID_IEventHandler, (void**)&event );
	if ( FAILED(hr) || event == NULL ) throw ( HSPERR_COMDLL_ERROR );
	event->Reset();
	event->Release();
	ReleaseComPtr( ppEv );
}

// 同一オブジェクトかどうか
BOOL IsSameComObject( IUnknown **ppunk1, IUnknown **ppunk2 )
{
	IUnknown *punk1, *punk2;
	BOOL result = FALSE;
	if ( IsVaridComPtr(ppunk1) && IsVaridComPtr(ppunk2) ) {
		(*ppunk1)->QueryInterface( IID_IUnknown, (void**)&punk1 );
		(*ppunk2)->QueryInterface( IID_IUnknown, (void**)&punk2 );
		if ( punk1 == punk2 ) result = TRUE;
		punk1->Release();
		punk2->Release();
	} else if ( *ppunk1 == NULL && *ppunk2 == NULL ) {
		result = TRUE;
	}
	return result;
}




/*-----------------------------------------------------------------------------------*/
/*
			オートメーション（IDispatch）関連
*/
/*-----------------------------------------------------------------------------------*/


static void get_prm_for_method( ComDispParams *data );
static void get_prm_for_prop( ComDispParams *data );
static void free_dispparams( ComDispParams *data );
static void set_prm_for_invoke( ComDispParams *data, int chk );


HRESULT CallDispMethod( IUnknown* punk, DISPID dispid, VARIANT *result )
{
	// メソッド呼び出し (スクリプトからパラメータの取得を行いつつ)
	//
	HRESULT hr;
	ComDispParams DispData = { 0 };

	DispData.disp = (IDispatch *)punk;
	punk->AddRef();

#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "CallDispMethod() : pObj=0x%p, dispid=%d\n", punk, dispid);
#endif
	// VARIANT と IDispatch の確実な解放のための例外処理
	try {
		get_prm_for_method( &DispData );
		hr = DispData.disp->Invoke( dispid, IID_NULL, LOCALE_USER_DEFAULT,
							DISPATCH_METHOD, &DispData.params,
							result, NULL, NULL );
	}
	catch (...) {
		free_dispparams( &DispData );
		throw;
	}
	free_dispparams( &DispData );
	return hr;
}

HRESULT GetDispProp( IUnknown* punk, DISPID dispid, VARIANT *result )
{
	// プロパティ取得 (スクリプトからパラメータの取得を行いつつ)
	//
	HRESULT hr;
	ComDispParams DispData = { 0 };

	DispData.disp = (IDispatch *)punk;
	punk->AddRef();

#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG("GetDispProp() : pObj=0x%p, dispid=%d\n", punk, dispid);
#endif
	// VARIANT と IDispatch の確実な解放のための例外処理
	try {
		get_prm_for_prop( &DispData );
		hr = DispData.disp->Invoke( dispid, IID_NULL, LOCALE_USER_DEFAULT,
							DISPATCH_METHOD | DISPATCH_PROPERTYGET,
							&DispData.params,result, NULL, NULL );
	}
	catch (...) {
		free_dispparams( &DispData );
		throw;
	}
	free_dispparams( &DispData );
	return hr;
}

ComDispParams *PrepForPutDispProp( IUnknown *punk, DISPID dispid  )
{
	// プロパティ設定のための準備 (スクリプトからパラメータの取得を行いつつ)
	//
	// 返される ComDispParams 構造体は後で PutDispProp() に渡す
	// それまでは PVal の master メンバに保存しておく
	//
	ComDispParams *data = (ComDispParams *)sbAlloc( sizeof(ComDispParams) );;

	data->disp = (IDispatch *)punk;
	punk->AddRef();

	data->dispid = dispid;
	data->flag = DISPATCH_PROPERTYPUT;
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "PrepForPutDispProp() : pObj=0x%p, dispid=%d\n", punk, dispid);
#endif
	try {
		get_prm_for_prop( data );
	}
	catch (...) {
		free_dispparams( data );
		sbFree( data );
		throw;
	}
	return data;
}

HRESULT PutDispProp( ComDispParams *data, void *setdata, int settype )
{
	// プロパティ設定
	//
	VARIANT *var;
	HRESULT hr;
	DISPID DispIdNamed[] = { DISPID_PROPERTYPUT };
	if ( data->flag != DISPATCH_PROPERTYPUT ) throw ( HSPERR_COMDLL_ERROR );
	data->params.rgdispidNamedArgs = DispIdNamed;
	data->params.cNamedArgs = 1;
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "PutDispProp() : pdispObj=0x%p, dispid=%d\n", data->disp, data->dispid);
#endif
	// VARIANT と IDispatch の確実な解放のための例外処理
	try {
		// 設定するデータをパラメータとして設定
		if ( data->params.cArgs >= TMP_VARIANT_MAX ) {
			throw ( HSPERR_TOO_MANY_PARAMETERS );
		}
		var = &data->prms[ TMP_VARIANT_MAX - 1 - data->params.cArgs ];
		VariantInit( var );
		comset_variant( var, setdata, settype );
		data->params.rgvarg = var;
		data->params.cArgs++;
		hr = data->disp->Invoke( data->dispid, IID_NULL, LOCALE_USER_DEFAULT,
			DISPATCH_PROPERTYPUT, &data->params, NULL, NULL, NULL );
	}
	catch (...) {
		free_dispparams( data );
		throw;
	}
	free_dispparams( data );
	return hr;
}
static void free_dispparams( ComDispParams *data )
{
	// パラメータが格納された VARIANT 配列の解放
	//
	VARIANT *var;
	int count = data->params.cArgs;
	if ( count > 0 ) {
		var = data->params.rgvarg;
		for (int i=0; i<count; i++) { VariantClear( &var[i] ); }
		data->params.cArgs = 0;
	}
	if (data->disp) {
		data->disp->Release();
		data->disp = NULL;
	}
}

void FreeDispParams( ComDispParams* data )
{
	// PrepForPutDispProp() が返した ComDispParams 構造体を解放
	//
	if ( data == NULL ) return;
	free_dispparams( data );
	sbFree( data );
}

static void get_prm_for_prop( ComDispParams *data )
{
	// invoke 呼び出しパラメータを取得（プロパティ用）
	//
	int chk;

	data->params.rgvarg = NULL;
	data->params.cArgs = 0;
	while (1) {
		chk = code_get();
		switch ( chk ) {
		case PARAM_OK:
		case PARAM_SPLIT:
		case PARAM_DEFAULT:
			set_prm_for_invoke( data, chk );
#ifdef HSP_COMOBJ_DEBUG
			COM_DBG_MSG( "get_prm_for_prop() : %d 個目のパラメータ : chk=%2d, flag=%d\n", data->params.cArgs, chk, mpval->flag);
#endif
			break;
		case PARAM_ENDSPLIT:			// パラメータ終端
			return;
		default:
			throw ( HSPERR_BAD_ARRAY_EXPRESSION );
		}
	}
}

static void get_prm_for_method( ComDispParams *data )
{
	// invoke 呼び出しパラメータを取得（メソッド呼び出し用）
	//
	int chk;

	data->params.rgvarg = NULL;
	data->params.cArgs = 0;
	while (1) {
		chk = code_get();
		switch ( chk ) {

		case PARAM_OK:				// パラメータ取得時
		case PARAM_DEFAULT:			// パラメータ省略時
			set_prm_for_invoke( data, chk );
#ifdef HSP_COMOBJ_DEBUG
			COM_DBG_MSG( "get_prm_for_method() : %d 個目のパラメータ : chk=%2d, flag=%d\n", data->params.cArgs, chk, mpval->flag);
#endif
			break;

		case PARAM_END:				// パラメータ終端
			return;

		default:
			throw ( HSPERR_SYNTAX );
		}
	}
}
static void set_prm_for_invoke( ComDispParams *data, int chk )
{
	VARIANT *var;
	if (data->params.cArgs >= TMP_VARIANT_MAX) {
		throw ( HSPERR_TOO_MANY_PARAMETERS );
	}
	var = &data->prms[ TMP_VARIANT_MAX - 1 - data->params.cArgs ];
	VariantInit( var );
	if ( chk == PARAM_DEFAULT ) {
		var->vt = VT_ERROR;								// デフォルト値をセット
		var->scode = DISP_E_PARAMNOTFOUND;
	} else {
		comset_variant( var, mpval->pt, mpval->flag );	// データ → VARIANT
	}
	data->params.rgvarg = var;
	data->params.cArgs++;
}


/*-----------------------------------------------------------------------------------*/
/*
			SafeArray 関連
*/
/*-----------------------------------------------------------------------------------*/

void ConvSafeArray2Var( PVal *pval, SAFEARRAY *psa, VARTYPE vt )
{
	// SafeArray から配列変数に変換
	//
	int i, dimcount, totalcount, elemcount[4]={0,0,0,0};
	void *pSrc, *pDst;

	dimcount = SafeArrayGetDim( psa );
	if ( dimcount > 4 ) throw ( HSPERR_UNSUPPORTED_FUNCTION );
	totalcount = 1;
	for ( i=0; i<dimcount; i++ ) {
		elemcount[i] = psa->rgsabound[i].cElements;
		totalcount *= elemcount[i];
	}
	SafeArrayAccessData( psa, &pSrc );
	switch ( vt ) {
	case VT_NULL:
	case VT_EMPTY:
		HspVarCoreDim( pval, HSPVAR_FLAG_INT, 1,0,0,0 );
		break;
	case VT_I4:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
	case VT_ERROR:
	case VT_I2:
	case VT_UI2:
	case VT_I1:
	case VT_UI1:
		HspVarCoreDim( pval, HSPVAR_FLAG_INT, elemcount[0],elemcount[1],elemcount[2],elemcount[3] );
		pDst = pval->pt;
		switch ( vt ) {
		case VT_I4:
		case VT_UI4:
		case VT_INT:
		case VT_UINT:
		case VT_ERROR:
			memcpy( pDst, pSrc, totalcount * sizeof(long) );
			break;
		case VT_I2:
			for (i=0; i<totalcount; i++) { ((long*)pDst)[i] = ((short*)pSrc)[i]; }
			break;
		case VT_UI2:
			for (i=0; i<totalcount; i++) { ((long*)pDst)[i] = ((USHORT*)pSrc)[i]; }
			break;
		case VT_I1:
			for (i=0; i<totalcount; i++) { ((long*)pDst)[i] = ((char*)pSrc)[i]; }
			break;
		case VT_UI1:
			for (i=0; i<totalcount; i++) { ((long*)pDst)[i] = ((UCHAR*)pSrc)[i]; }
			break;
		}
		break;
	case VT_R8:
	case VT_R4:
		HspVarCoreDim( pval, HSPVAR_FLAG_DOUBLE, elemcount[0],elemcount[1],elemcount[2],elemcount[3] );
		pDst = pval->pt;
		switch ( vt ) {
		case VT_R8:
			memcpy( pDst, pSrc, totalcount * sizeof(double) );
			break;
		case VT_R4:
			for (i=0; i<totalcount; i++) { ((double*)pDst)[i] = ((float*)pSrc)[i]; }
			break;
		}
		break;
	default:
		throw ( HSPERR_INVALID_TYPE );
	}
}

SAFEARRAY *ConvVar2SafeArray( PVal *pval, BOOL bVariant, VARTYPE *vtRet )
{
	// 配列変数から SafeArray に変換
	// bVariant = TRUE のとき VARIANT に変換
	//
	SAFEARRAY *psa;
	VARTYPE vt;
	int i, dimcount, totalcount;
	void *pSrc, *pDst;
	SAFEARRAYBOUND sabound[4];

	// 変数型チェックと変換後の VARTYPE
	switch ( pval->flag ) {
	case HSPVAR_FLAG_STR:
		vt = VT_BSTR;
		break;
	case HSPVAR_FLAG_DOUBLE:
		vt = VT_R8;
		break;
	case HSPVAR_FLAG_INT:
		vt = VT_I4;
		break;
	case HSPVAR_FLAG_COMSTRUCT:
		vt = VT_UNKNOWN;
		break;
	default:
		throw HSPERR_INVALID_TYPE;
	}
	if ( bVariant ) vt = VT_VARIANT;

	// 配列変数の次元数・要素数を調べる
	dimcount = 0;
	totalcount = 1;
	for (i=0; i<4; i++) {
		sabound[i].cElements = pval->len[i+1];
		if ( sabound[i].cElements == 0 ) break;
		totalcount *= sabound[i].cElements;
		sabound[i].lLbound = 0;
		dimcount++;
	}

	// SafeArray 作成・データコピー
	psa = SafeArrayCreate( vt, dimcount, sabound );
	if ( psa == NULL ) throw ( HSPERR_OUT_OF_MEMORY );
	SafeArrayAccessData( psa, &pDst );
	pSrc = pval->pt;
	switch ( pval->flag ) {
	case HSPVAR_FLAG_STR:
		for (i=0; i<totalcount; i++) {
			BSTR *pbstrDst;
			char *pszSrc;
			if ( bVariant ) {
				((VARIANT *)pDst)[i].vt = VT_BSTR;
				pbstrDst = &((VARIANT *)pDst)[i].bstrVal;
			} else {
				pbstrDst = &((BSTR *)pDst)[i];
			}
			pval->offset = i;
			pszSrc = (char *)HspVarCorePtr( pval );
			*pbstrDst = comget_bstr( pszSrc );
		}
		break;
	case HSPVAR_FLAG_DOUBLE:
		if ( bVariant ) {
			for ( i=0; i<totalcount; i++ ) {
				((VARIANT *)pDst)[i].vt = VT_R8;
				((VARIANT *)pDst)[i].dblVal = ((double*)pSrc)[i];
			}
		} else {
			memcpy( pDst, pSrc, totalcount * sizeof(double) );
		}
		break;
	case HSPVAR_FLAG_INT:
		if ( bVariant ) {
			for ( i=0; i<totalcount; i++ ) {
				((VARIANT *)pDst)[i].vt = VT_I4;
				((VARIANT *)pDst)[i].lVal = ((long*)pSrc)[i];
			}
		} else {
			memcpy( pDst, pSrc, totalcount * sizeof(int) );
		}
		break;
	case HSPVAR_FLAG_COMSTRUCT:
		for (i=0; i<totalcount; i++) {
			IUnknown *punkSrc = ((IUnknown**)pSrc)[i];
			if ( bVariant ) {
				((VARIANT *)pDst)[i].vt = VT_UNKNOWN;
				((VARIANT *)pDst)[i].punkVal = punkSrc;
			} else {
				((IUnknown **)pDst)[i] = punkSrc;
			}
			if ( punkSrc != NULL ) {
				punkSrc->AddRef();
			}
		}
		break;
	}
	SafeArrayUnaccessData( psa );
	*vtRet = vt;
	return psa;
}

SAFEARRAY *CreateBinarySafeArray( void *data, int size, VARTYPE *vt )
{
	SAFEARRAY *psa;
	SAFEARRAYBOUND rgsabound[1];
	void *ptr;

	if ( size <= 0 ) throw HSPERR_ILLEGAL_FUNCTION;

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = size;
	psa = SafeArrayCreate( VT_UI1, 1, rgsabound );
	if ( psa == NULL ) throw HSPERR_OUT_OF_MEMORY;
	SafeArrayAccessData( psa, &ptr );
	memcpy( ptr, data, size );
	SafeArrayUnaccessData( psa );
	*vt = VT_UI1;
	return psa;
}

void GetBinarySafeArray( void *ptr, int size, SAFEARRAY *psa )
{
	void *data;
	HRESULT hr;

	hr = SafeArrayAccessData( psa, &data );
	if ( FAILED(hr) ) throw HSPERR_ARRAY_OVERFLOW;
	memcpy( ptr, data, size );
	SafeArrayUnaccessData( psa );
}

#endif	// !defined( HSP_COM_UNSUPPORTED )

