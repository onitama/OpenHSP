
//
//	HSPVAR core module
//	onion software/onitama 2004/10
//

#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ocidl.h>

#include "../hsp3code.h"
#include "../hsp3struct.h"
#include "../hsp3ext.h"
#include "../hspvar_core.h"
#include "comobj.h"
#include "hspvar_comobj.h"
#include "comobj_event.h"
#include "../strbuf.h"
#include "../supio.h"

/*------------------------------------------------------------*/
/*
		HSPVAR core interface (COM Object)
*/
/*------------------------------------------------------------*/

static HspVarProc *myproc;

// Core
static PDAT *HspVarComobj_GetPtr( PVal *pval )
{
	return (PDAT *)(( (IUnknown **)(pval->pt))+pval->offset);
}

static void *HspVarComobj_Cnv( const void *buffer, int flag )
{
	//		リクエストされた型 -> 自分の型への変換を行なう
	//		(組み込み型にのみ対応でOK)
	//		(参照元のデータを破壊しないこと)
	//
	throw HSPERR_INVALID_TYPE;
	return (void *)buffer;
}


static void *HspVarComobj_CnvCustom( const void *buffer, int flag )
{
	//		(カスタムタイプのみ)
	//		自分の型 -> リクエストされた型 への変換を行なう
	//		(組み込み型に対応させる)
	//		(参照元のデータを破壊しないこと)
	//
	throw HSPERR_INVALID_TYPE;
	return (void *)buffer;
}

static void HspVarComobj_Free( PVal *pval )
{
	//		PVALポインタの変数メモリを解放する
	//
	IUnknown** ppunk;
	if ( pval->master ) {
		FreeDispParams( (ComDispParams *)pval->master );
	}
	if ( pval->mode == HSPVAR_MODE_MALLOC ) {
		// (一時変数に含まれるオブジェクトは Release しない)
		if ( (pval->support & HSPVAR_SUPPORT_TEMPVAR) == 0 ) {
#ifdef HSP_COMOBJ_DEBUG
			COM_DBG_MSG( "HspVarComobj_Free()\n" );
#endif
			int count = HspVarCoreCountElems( pval );
			ppunk = (IUnknown **)pval->pt;
			for (int i=0; i<count; i++) {
				ReleaseComPtr( &ppunk[i] );
			}
		}
		sbFree( pval->pt );
	}
	pval->pt   = NULL;
	pval->flag = HSPVAR_MODE_NONE;
}


static void HspVarComobj_Alloc( PVal *pval, const PVal *pval2 )
{
	//		pval変数が必要とするサイズを確保する。
	//		(pvalがすでに確保されているメモリ解放は呼び出し側が行なう)
	//		(pval2がNULLの場合は、新規データ)
	//		(pval2が指定されている場合は、pval2の内容を継承して再確保)
	//
	int count,size;
	IUnknown **ppunk;
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "HspVarComobj_Alloc()\n" );
#endif
	if ( pval->len[1] < 1 ) pval->len[1] = 1;		// 配列を最低 1 は確保する
	count = HspVarCoreCountElems(pval);
	size  = count * sizeof( IUnknown* );
	ppunk = (IUnknown **)sbAlloc( size );
	pval->mode = HSPVAR_MODE_MALLOC;
	for (int i=0; i<count; i++) { ppunk[i] = NULL; }
	if ( pval2 != NULL ) {
		memcpy( ppunk, pval->pt, pval->size );
		sbFree( pval->pt );
		if ( pval->master ) sbFree( pval->master );
	}
	pval->master = NULL;		// ComDispParams 用
	pval->pt = (char *)ppunk;
	pval->size = size;

}

static void HspVarComobj_ObjectMethod( PVal *pval )
{
	//		メソッドの実行
	//
	VARIANT vres;
	HRESULT hr;
	char *ps;
	DISPID dispid;
	IUnknown** ppunk;
	BOOL bVariantRet;

//	ppunk = (IUnknown**)HspVarCorePtrAPTR( pval, pval->offset );
	ppunk = (IUnknown **)pval->pt;
	if ( ! IsVaridComPtr(ppunk) ) throw HSPERR_COMDLL_ERROR;

	// メソッド名から DISPID を取得
	ps = code_gets();
	dispid = get_dispid( *ppunk, ps, &bVariantRet );
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "ObjectMethod() : pObj=0x%p : PropName=\"%s\" (DISPID=%d)\n", *ppunk, ps, dispid);
#endif
	// メソッドパラメータの取得・メソッド実行
	VariantInit( &vres );
	hr = CallDispMethod( *ppunk, dispid, &vres );
	comget_variantres( &vres, hr, bVariantRet );				// 返値を取得
	VariantClear( &vres );
}

static int code_get_element( PVal *pval )
{
	// 変数の配列要素の取得
	//
	PVal pvalTemp;
	int chk, idx;
	HspVarCoreReset(pval);
	while (1) {
		HspVarCoreCopyArrayInfo( &pvalTemp, pval );			// 状態を保存
		chk = code_get();
		HspVarCoreCopyArrayInfo( pval, &pvalTemp );			// 状態を復帰
		if ( chk == PARAM_ENDSPLIT ) {
			if ( pval->arraycnt == 0 ) throw HSPERR_BAD_ARRAY_EXPRESSION;	// a() 表記はエラー
			break;
		}
		if ( chk != PARAM_OK && chk != PARAM_SPLIT ) throw HSPERR_ARRAY_OVERFLOW;
		if ( mpval->flag != HSPVAR_FLAG_INT ) break;
		idx = *(int *)(mpval->pt);
		HspVarCoreArray( pval, idx );
	}
	return chk;
}

static void HspVarComobj_ArrayObject( PVal *pval )
{
	//		配列要素の指定 (連想配列用)
	//
	IUnknown** ppunk;
	int chk;
	DISPID dispid;
	ComDispParams *paramdata;

	// 配列要素の取得
	chk = code_get_element( pval );
	if ( chk == PARAM_ENDSPLIT ) return;	// 配列要素が指定された場合はそのまま

	// プロパティ設定時
	ppunk = (IUnknown **)HspVarComobj_GetPtr( pval );
	if ( ! IsVaridComPtr(ppunk) ) throw HSPERR_COMDLL_ERROR;
	// プロパティ名から DISPID を取得
	if ( mpval->flag != HSPVAR_FLAG_STR ) throw HSPERR_TYPE_MISMATCH;
	dispid = get_dispid( *ppunk, (char *)(mpval->pt), NULL );
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "ArrayObject() : pObj=0x%p : PropName=\"%s\" (DISPID=%d)\n", *ppunk, mpval->pt, dispid);
#endif
	// パラメータ取得し保持しておく
	paramdata = PrepForPutDispProp( *ppunk, dispid );
	if ( pval->master ) FreeDispParams( (ComDispParams *)pval->master );
	pval->master = paramdata;
}

static void HspVarComobj_ObjectWrite( PVal *pval, void *data, int vtype )
{
	//		可変型の代入
	//
	ComDispParams *paramdata;
	HRESULT hr;
#ifdef HSP_COMOBJ_DEBUG
	COM_DBG_MSG( "ObjectWrite()\n" );
#endif
	// プロパティ設定時
	paramdata = (ComDispParams *)pval->master;
	if ( paramdata == NULL ) throw ( HSPERR_COMDLL_ERROR );
	hr = PutDispProp( paramdata, data, vtype );
	FreeDispParams( paramdata );
	pval->master = NULL;

	if ( FAILED(hr) ) throw ( HSPERR_COMDLL_ERROR );
}


static void get_coclassname( IUnknown *punk, VARIANT *vres )
{
	HRESULT hr;
	IProvideClassInfo *pPCI;
	ITypeInfo *pTI;
	BSTR bstr = NULL;
	hr = punk->QueryInterface( IID_IProvideClassInfo, (void **)&pPCI );
	if ( SUCCEEDED(hr) && pPCI != NULL ) {
		hr = pPCI->GetClassInfo( &pTI );
		if ( SUCCEEDED(hr) && pTI != NULL ) {
			hr = pTI->GetDocumentation( MEMBERID_NIL, &bstr, NULL, NULL, NULL );
			pTI->Release();
		}
		pPCI->Release();
	}
	if ( bstr == NULL ) {
		bstr = SysAllocString( L"" );
	}
	vres->bstrVal = bstr;
	vres->vt = VT_BSTR;
}
static void get_interfacename( IUnknown *punk, VARIANT *vres )
{
	HRESULT hr;
	IDispatch *pDisp;
	ITypeInfo *pTI;
	BSTR bstr = NULL;
	hr = punk->QueryInterface( IID_IDispatch, (void **)&pDisp );
	if ( SUCCEEDED(hr) && pDisp != NULL ) {
		hr = pDisp->GetTypeInfo( 0, LOCALE_USER_DEFAULT, &pTI );
		if ( SUCCEEDED(hr) && pTI != NULL ) {
			hr = pTI->GetDocumentation( MEMBERID_NIL, &bstr, NULL, NULL, NULL );
			pTI->Release();
		}
		pDisp->Release();
	}
	if ( bstr == NULL ) {
		bstr = SysAllocString( L"" );
	}
	vres->bstrVal = bstr;
	vres->vt = VT_BSTR;
}

static void *HspVarComobj_ArrayObjectRead( PVal *pval, int *mptype )
{
	//		配列要素の指定 (連想配列/読み出し)
	//
	void *ptr;
	IUnknown **ppunk;
	int chk;
	DISPID dispid;
	VARIANT vres;
	BOOL noconvret;
	char *propname;
	HRESULT hr = S_OK;

	// 配列要素の取得
	chk = code_get_element( pval );
	ppunk = (IUnknown **)HspVarComobj_GetPtr( pval );
	if ( chk == PARAM_ENDSPLIT ) return ppunk;		// 配列要素が指定された場合はそのまま

	// プロパティ取得時
	// プロパティ名から DISPID を取得
	if ( ! IsVaridComPtr(ppunk) ) throw ( HSPERR_COMDLL_ERROR );
	if ( mpval->flag != HSPVAR_FLAG_STR ) throw ( HSPERR_TYPE_MISMATCH );
	propname = (char *)(mpval->pt);
	VariantInit( &vres );
	if ( propname[0] == '$' ) {
		noconvret = FALSE;
		if ( stricmp( propname, "$coclass" ) == 0 ) {
			get_coclassname( *ppunk, &vres );
		} else if ( stricmp( propname, "$interface" ) == 0 ) {
			get_interfacename( *ppunk, &vres );
		} else {
			throw HSPERR_INVALID_PARAMETER;
		}
	} else {
		dispid = get_dispid( *ppunk, propname, &noconvret );
#ifdef HSP_COMOBJ_DEBUG
		COM_DBG_MSG( "ArrayObjectRead() : pObj=0x%p : PropName=\"%s\" (DISPID=%d)\n", *ppunk, mpval->pt, dispid);
#endif
	// パラメータを取得・プロパティ取得
		hr = GetDispProp( *ppunk, dispid, &vres );
	}
	ptr = comget_variant( &vres, mptype, noconvret );
	VariantClear( &vres );
	if ( FAILED(hr) ) throw ( HSPERR_COMDLL_ERROR );
	return ptr;
}


// Size
static int HspVarComobj_GetSize( const PDAT *pdatl )
{
	//		(実態のポインタが渡されます)
	return sizeof(IUnknown*);
}

// Using
static int HspVarComobj_GetUsing( const PDAT *pdat )
{
	//		(実態のポインタが渡されます)
	return IsVaridComPtr((IUnknown**)pdat);
}

// Set
static void HspVarComobj_Set( PVal *pval, PDAT *pdat, const void *in )
{
	IUnknown **ppunkSrc, **ppunkDst;

	ppunkDst = (IUnknown **)pdat;
	ppunkSrc = (IUnknown **)in;
	if ( pval->support & HSPVAR_SUPPORT_TEMPVAR ) {
		// 代入先が一時変数の場合は AddRef() しない
		*ppunkDst = *ppunkSrc;
#ifdef HSP_COMOBJ_DEBUG
		COM_DBG_MSG( "ppunkDst is Temporary objedct.\n");
#endif
	} else {
		CopyComPtr( ppunkDst, *ppunkSrc );
	}
}


static void *GetBlockSize( PVal *pval, PDAT *pdat, int *size )
{
	*size = pval->size - ( ((char *)pdat) - pval->pt );
	return (pdat);
}

static void AllocBlock( PVal *pval, PDAT *pdat, int size )
{
}

// Eq
static void HspVarComobj_EqI( PDAT *pdat, const void *val )
{
	*(int *)pdat = IsSameComObject( (IUnknown **)pdat, (IUnknown **)val );
	myproc->aftertype = HSPVAR_FLAG_INT;
}

// Ne
static void HspVarComobj_NeI( PDAT *pdat, const void *val )
{
	*(int *)pdat = !IsSameComObject( (IUnknown **)pdat, (IUnknown **)val );
	myproc->aftertype = HSPVAR_FLAG_INT;
}


/*------------------------------------------------------------*/

void HspVarComobj_Init( HspVarProc *p )
{
	myproc = p;

	p->Set = HspVarComobj_Set;
	p->Cnv = HspVarComobj_Cnv;
	p->GetPtr = HspVarComobj_GetPtr;
	p->CnvCustom = HspVarComobj_CnvCustom;
	p->GetSize = HspVarComobj_GetSize;
	p->GetUsing = HspVarComobj_GetUsing;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

	p->ArrayObject = HspVarComobj_ArrayObject;
	p->ArrayObjectRead = HspVarComobj_ArrayObjectRead;
	p->ObjectWrite = HspVarComobj_ObjectWrite;
	p->ObjectMethod = HspVarComobj_ObjectMethod;

	p->Alloc = HspVarComobj_Alloc;
	p->Free = HspVarComobj_Free;
	p->EqI = HspVarComobj_EqI;
	p->NeI = HspVarComobj_NeI;

/*
	p->AddI = HspVarComobj_Invalid;
	p->SubI = HspVarComobj_Invalid;
	p->MulI = HspVarComobj_Invalid;
	p->DivI = HspVarComobj_Invalid;
	p->ModI = HspVarComobj_Invalid;

	p->AndI = HspVarComobj_Invalid;
	p->OrI  = HspVarComobj_Invalid;
	p->XorI = HspVarComobj_Invalid;

	p->EqI = HspVarComobj_Invalid;
	p->NeI = HspVarComobj_Invalid;
	p->GtI = HspVarComobj_Invalid;
	p->LtI = HspVarComobj_Invalid;
	p->GtEqI = HspVarComobj_Invalid;
	p->LtEqI = HspVarComobj_Invalid;

	p->RrI = HspVarComobj_Invalid;
	p->LrI = HspVarComobj_Invalid;
*/
	p->vartype_name = "comobj";				// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_STORAGE | HSPVAR_SUPPORT_ARRAYOBJ | HSPVAR_SUPPORT_NOCONVERT | HSPVAR_SUPPORT_VARUSE;
										// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = sizeof(void*);	// １つのデータが使用するサイズ(byte) / 可変長の時は-1
}

/*------------------------------------------------------------*/





#endif	// !defined( HSP_COM_UNSUPPORTED )

