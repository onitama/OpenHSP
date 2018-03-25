/*------------------------------------------------------------*/
/*
		event handler
*/
/*------------------------------------------------------------*/
/*
    (for HSP 3.0)
    COM オブジェクトのイベントハンドラオブジェクト

    COM オブジェクトのイベントを受け取るオブジェクトです。

    イベントハンドラオブジェクトは、イベント取得に必要となる
    IDispatch インターフェースを提供します。

    イベントハンドラインターフェースポインタ (IID_IEventHandler)
    のメンバ関数は以下の通り。

    HRESULT IEventHandler::Set(IUnknown *pObj, IID* iid, USHORT* callback);
    void    IEventHandler::Reset();
    void    IEventHandler::IncInnerRef();
    void    IEventHandler::DecInnerRef();

      Set() はイベントハンドラの設定を変更します。以前の設定は
      上書きされます。

      Reset() は既存のハンドラ設定を解除します。

      IncInnerRef(), DecInnerRef() はHSPのCOM型変数からの参照
      カウントを管理するため使用されます。

    関数

    DISPID GetEventDispID( void* iptr );

      イベント割り込み処理中に、指定されたオブジェクトから通知
      されたイベントの DISPID を取得します。多重割り込み処理中
      に iptr に NULL を指定した場合には、もっとも新しいイベン
      トを発生させたオブジェクトが対象になります。

    VARIANT* GetEventArg( void* iptr, int idx );

      イベント割り込み処理中に、指定されたオブジェクトから通知
      されたイベントの指定インデックスの引数を表す  VARIANT 構
      造体へのポインタを返します。多重割り込み処理中に iptr に
      NULL  を指定した場合には、もっとも新しいイベントを発生さ
      せたオブジェクトが対象になります。

*/


#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ocidl.h>

/*
	rev 43
	mingw : error : GUIDKIND_DEFAULT_SOURCE_DISP_IIDが未定義
	に対処
*/
#if defined( __GNUC__ )
#include <olectl.h>
#endif

#include "../hsp3code.h"
#include "comobj_event.h"
#include "hspvar_comobj.h"


#ifdef HSP_COMEVENT_DEBUG
/* for Debug */
/* GUID を文字列形式に変換 */
static int sprintGuid( char *szguid, REFGUID rguid )
{
	if ( rguid == IID_IUnknown  ) return sprintf( szguid, "IID_IUnknown" );
	if ( rguid == IID_IDispatch ) return sprintf( szguid, "IID_IDispatch" );
	if ( rguid == IID_IEventHandler ) return sprintf( szguid, "IID_IEventHandler" );

	return sprintf( szguid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		rguid.Data1, rguid.Data2, rguid.Data3, rguid.Data4[0], rguid.Data4[1],
		rguid.Data4[2], rguid.Data4[3], rguid.Data4[4], rguid.Data4[5],
		rguid.Data4[6], rguid.Data4[7]);
}

static DWORD g_dwThreadId;

#endif	// HSP_COMEVENT_DEBUG

// IID_IEventHandler の実体
IID IID_IEventHandler = {
	0x4c7f4354, 0x8a07, 0x4d51, {0xb9, 0xf0, 0x47, 0xba, 0x5c, 0xbe, 0xfe, 0xd7}
};

struct ComEventData;

// Enevnt Handler object

/*
	rev 43
	mingw : warning : ComEventHandler のデストラクタが仮想でない。
	問題ないのか？わかりません。(naznyark)
*/
class ComEventHandler : public IEventHandler {
	ULONG m_ref;				// イベントオブジェクト参照カウンタ
	int m_refInner;				// HSP COMオブジェクト変数からの参照カウンタ
	IUnknown* m_punkObj;		// オブジェクトのIUnknownインターフェースポインタ
	IConnectionPoint* m_pCP;	// IConnectionPoint インターフェースポインタ
	DWORD m_cookie;				// Cookie 値( Advise() より返される)
	IID m_CPGuid;				// コネクションポイントIID
	unsigned short* m_callback;	// HSPサブルーチン(コールバック用)

#ifdef HSP_COMEVENT_DEBUG
	/* For Debug */
	FILE *fpDebug;
#endif

public:
	ComEventHandler();
	~ComEventHandler();

	// IDispatch methods (イベント処理用)
	STDMETHOD(QueryInterface)(REFIID, void**);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(GetTypeInfoCount)(UINT*);
	STDMETHOD(GetTypeInfo)(UINT, LCID, ITypeInfo**);
	STDMETHOD(GetIDsOfNames)(REFIID, OLECHAR**, UINT, LCID, DISPID*);
	STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*);
	STDMETHOD_(void, IncInnerRef)();
	STDMETHOD_(void, DecInnerRef)();
	STDMETHOD(Set)(IUnknown*, const IID*, unsigned short* );
	STDMETHOD_(void,Reset)();

/*
	rev 43
	mingw : error : friend static修飾は不正
	に対処
*/
#if defined(__GNUC__)
	friend ComEventData* SearchEventData( void *iptr );
#else
	friend static ComEventData* SearchEventData( void *iptr );
#endif

};


/*-----------------------------------------------------------------------------------*/

// Event Data structure (defined as class object)
//
// note: エラー時に例外が throw された場合でもポインタのつなぎ替えが
//       正常に行われるようにクラスオブジェクトとして定義してあります
//

struct ComEventData {
	ComEventHandler* handler;
	DISPID dispid;
	DISPPARAMS* params;
	VARIANT* result;

	// (多重割り込みに備えて)
	ComEventData* prev;			// １つ前のイベント
	static ComEventData* now;	// 現在処理中 (ない場合 NULL )
	// static UINT count;			// 多重割り込みイベント数 (未使用)

	ComEventData(ComEventHandler*, DISPID, DISPPARAMS*, VARIANT*);
	~ComEventData();
};

// UINT ComEventData::count = 0;
ComEventData* ComEventData::now = NULL;

ComEventData::ComEventData(
	ComEventHandler *handler_s,
	DISPID dispid_s,
	DISPPARAMS *params_s,
	VARIANT *result_s )
 : handler(handler_s),dispid(dispid_s),params(params_s),result(result_s)
{
	if (handler) { handler->AddRef(); }

	prev = now;
	now = this;
//	count++;
}

ComEventData::~ComEventData()
{
	if (handler) { handler->Release(); }
	now = prev;
//	count--;
}

/*-----------------------------------------------------------------------------------*/

/*
	rev 43
	mingw : warning : メンバの宣言順と初期化子リストの並び順が異なる
	に対処
*/
ComEventHandler::ComEventHandler()
 : m_ref(0), m_refInner(0), m_punkObj(NULL), m_pCP(NULL), m_cookie(0), m_CPGuid( IID_NULL ), m_callback(0)
{
	// 参照カウンタ 0 の状態で作成

#ifdef HSP_COMEVENT_DEBUG
	char fname[64];
	sprintf( fname, "dbg_evnt_%p.txt", this );
	fpDebug = fopen( fname, "w" );
	fprintf( fpDebug, "An event handler is created. : this = %p\n", this );
#endif	// HSP_COMEVENT_DEBUG
}

ComEventHandler::~ComEventHandler()
{
	Reset();

#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "This handler object is deleted.\n" );
	fclose( fpDebug );
#endif
}


// initialize connection of event handler object
HRESULT STDMETHODCALLTYPE ComEventHandler::Set( IUnknown *pObj, const IID* pCPGuid, unsigned short* callback )
{
	IProvideClassInfo2* pPCI;
	IConnectionPointContainer* pCPC;
	HRESULT hr;

	Reset();

	// connection point IID preparation
	if ( pCPGuid ) {
		m_CPGuid = *pCPGuid;
	} else {
		// search for a default connection point IID.
		hr = pObj->QueryInterface(IID_IProvideClassInfo2, reinterpret_cast<void**>(&pPCI) );
		if ( SUCCEEDED(hr) ) {
			hr = pPCI->GetGUID( GUIDKIND_DEFAULT_SOURCE_DISP_IID, &m_CPGuid );
			pPCI->Release();
		}
		if ( FAILED(hr) ) return hr;
	}

	// 内部では IUnknown インターフェースを保持
	pObj->QueryInterface( IID_IUnknown, reinterpret_cast<void**>(&m_punkObj) );
	m_callback = callback;

	// IConnectionPointContainer から IConnectionPoint を取得
	hr = pObj->QueryInterface( IID_IConnectionPointContainer, reinterpret_cast<void**>(&pCPC) );
	if ( SUCCEEDED(hr) ) {
		hr = pCPC->FindConnectionPoint( m_CPGuid, &m_pCP );
		if ( SUCCEEDED(hr) ) {
			hr = m_pCP->Advise( static_cast<IDispatch*>(this), &m_cookie );
		}
		pCPC->Release();
	}
	if ( FAILED(hr) ) Reset();

#ifdef HSP_COMEVENT_DEBUG
	char guid[64];
	sprintGuid( guid, *pCPGuid );
	fprintf( fpDebug, "Set() : pObj=%p, Connection Point IID = %s : ", pObj, guid );
	if ( SUCCEEDED(hr) ) {
		fprintf( fpDebug, "Initialized. m_ref=%d\n", m_ref );
	} else {
		fprintf( fpDebug, "Initialization is failed.\n" );
	}
#endif	// HSP_COMEVENT_DEBUG

	return hr;
}

void STDMETHODCALLTYPE ComEventHandler::Reset()
{
	if ( m_pCP ) {
		if ( m_cookie != 0 )  {
			m_pCP->Unadvise( m_cookie );
			m_cookie = 0;
		}
		m_pCP->Release();
		m_pCP = NULL;
	}
	if ( m_punkObj ) {
		m_punkObj->Release();
		m_punkObj = NULL;
	}
	m_CPGuid = IID_NULL;
	m_callback = NULL;

#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "Reset() : All Setting is reset.\n" );
#endif
}

void STDMETHODCALLTYPE ComEventHandler::IncInnerRef()
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "IncInnerRef() : m_refInner=%d :: ", m_refInner+1);
#endif

	// HSP COM変数からの参照カウンタをインクリメント
	AddRef();
	m_refInner++;
}

void STDMETHODCALLTYPE ComEventHandler::DecInnerRef()
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "DecInnerRef() : m_refInner=%d :: ", m_refInner-1);
#endif

	// HSP COM変数からの参照カウンタをデクリメント
	if ( (--m_refInner) <= 0 ) {
		// もうどのCOMオブジェクト変数からも参照されていない場合
		Reset();
	}
	// この Release() でオブジェクト自身 (this) が破棄される可能性もある
	// (この Release() 以降で自身のメンバを参照しないこと)
	Release();
}

HRESULT STDMETHODCALLTYPE ComEventHandler::QueryInterface (
	REFIID riid,
	void** ppv )
{
#ifdef HSP_COMEVENT_DEBUG
	char guid[64];
	sprintGuid( guid, riid );
	fprintf( fpDebug, "QueryInterface( %s ) : ", guid );
#endif

	if ( ppv == NULL ) return E_POINTER;

	if ( riid == IID_IEventHandler ||
		 riid == m_CPGuid ||
		 riid == IID_IUnknown ||
		 riid == IID_IDispatch )
	{
		*ppv = static_cast<IEventHandler*>(this);
		AddRef();
		return S_OK;
	}
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "Error (m_ref=%d)\n", m_ref);
#endif

	*ppv = NULL;
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE ComEventHandler::AddRef ()
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "AddRef() : m_ref=%d\n", m_ref+1 );
#endif

	return ++m_ref;
}

ULONG STDMETHODCALLTYPE ComEventHandler::Release ()
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "Release() : m_ref=%d\n", m_ref-1 );
#endif

	if ( --m_ref ) return m_ref;
	delete this;
	return 0;
}

HRESULT STDMETHODCALLTYPE ComEventHandler::GetTypeInfoCount (UINT* pctinfo)
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "GetTypeInfoCount()\n");
#endif

	if ( pctinfo == NULL )
		return E_INVALIDARG;
	*pctinfo = 0;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE ComEventHandler::GetTypeInfo (UINT, LCID, ITypeInfo** ppTInfo)
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "GetTypeInfo()\n");
#endif

	if ( ppTInfo == NULL )
		return E_INVALIDARG;
	*ppTInfo = NULL;
	return S_OK;
}


HRESULT STDMETHODCALLTYPE ComEventHandler::GetIDsOfNames (
	REFIID, OLECHAR**, UINT, LCID, DISPID* )
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "GetIDsOfNames()\n");
#endif

	return DISP_E_UNKNOWNNAME;
}

HRESULT STDMETHODCALLTYPE ComEventHandler::Invoke (
	DISPID dispid, REFIID, LCID, WORD wFlags,
	DISPPARAMS *dispparams, VARIANT* result, EXCEPINFO*, UINT*)
{
#ifdef HSP_COMEVENT_DEBUG
	fprintf( fpDebug, "Invoke() : DISPID=%d wFlags=0x%04x\n", dispid, wFlags);
#endif
	if ( wFlags != DISPATCH_METHOD )
		return DISP_E_MEMBERNOTFOUND;
	if ( dispparams->cNamedArgs )
		return DISP_E_NONAMEDARGS;

	ComEventData eventdata( this, dispid, dispparams, result);
	code_call( m_callback );
	return S_OK;
}


static ComEventData* SearchEventData( IUnknown* punkEvent )
{
	ComEventData *p, *ret;
//	IEventHandler *event;
//	HRESULT hr;

	if ( punkEvent == NULL ) return ComEventData::now;

	ret = NULL;
	/*
	hr = punkEvent->QueryInterface( IID_IEventHandler, (void**)&event );
	if ( SUCCEEDED(hr) && event != NULL ) {
		for ( p=ComEventData::now; p!=NULL; p=p->prev) {
			if ( p->handler == static_cast<ComEventHandler*>(event) ) {
				ret = p;
				break;
			}
		}
		event->Release();
	}
	*/
	for ( p=ComEventData::now; p!=NULL; p=p->prev) {
		if ( p->handler == static_cast<ComEventHandler*>(punkEvent) ) {
			ret = p;
			break;
		}
	}
	return ret;
}


DISPID GetEventDispID( IUnknown* punkEvent )
{
	ComEventData *pData = SearchEventData( punkEvent );
	if ( pData ) {
		return pData->dispid;
	}
	return DISPID_UNKNOWN;
}

VARIANT* GetEventArg( IUnknown* punkEvent , int idx )
{
#ifdef HSP_COMOBJ_DEBUG
	fprintf(fpComDbg, "GetEventArg() : pEvent=%p : index=%d\n", punkEvent, idx);
#endif

	VARIANT *varArg = NULL;
	ComEventData *pData = SearchEventData( punkEvent );
	if ( pData ) {
		int cArgs = pData->params->cArgs;
		if ( idx >= 0 && idx < cArgs ) {
			varArg = &pData->params->rgvarg[cArgs - idx - 1];
#ifdef HSP_COMOBJ_DEBUG
			fprintf(fpComDbg, "cArgs=%d, index=%d\n", cArgs, idx);
#endif
		}
	}
	return varArg;
}

IEventHandler* CreateEventHandler( IUnknown* obj, const IID* iid, unsigned short* subr )
{
	IEventHandler *event;
	HRESULT hr;
	event = new ComEventHandler;
	event->AddRef();
	hr = event->Set( obj, iid, subr );
	if ( FAILED(hr) ) {
		delete event;
		event = NULL;
	}
	return event;
}




#endif	// !defined( HSP_COM_UNSUPPORTED )

