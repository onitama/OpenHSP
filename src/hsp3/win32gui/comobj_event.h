
//
//	comobj_event.cpp header
//
#ifndef __comobj_event_h
#define __comobj_event_h

// デバッグ用の場合に定義
// #define HSP_COMEVENT_DEBUG


#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）

/*-----------------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
struct IEventHandler;
#endif

// IID_IEventHandler = {4c7f4354-8a07-4d51-b9f0-47ba5cbefed7};
extern IID IID_IEventHandler;

#undef INTERFACE
#define INTERFACE IEventHandler

DECLARE_INTERFACE_(IEventHandler, IDispatch)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)  (THIS_ REFIID, LPVOID*) PURE;
	STDMETHOD_(ULONG,AddRef)   (THIS) PURE;
	STDMETHOD_(ULONG,Release)  (THIS) PURE;

	// IDispatch methods
	STDMETHOD(GetTypeInfoCount)(THIS_ UINT*) PURE;
	STDMETHOD(GetTypeInfo)(THIS_ UINT, LCID, ITypeInfo**) PURE;
	STDMETHOD(GetIDsOfNames)(THIS_ REFIID, OLECHAR**, UINT, LCID, DISPID*) PURE;
	STDMETHOD(Invoke)(THIS_ DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*, EXCEPINFO*, UINT*) PURE;

	// IEventHandler methods
	STDMETHOD_(void, IncInnerRef) (THIS) PURE;
	STDMETHOD_(void, DecInnerRef) (THIS) PURE;
	STDMETHOD(Set)         (THIS_ IUnknown*, const IID*, unsigned short* ) PURE;
	STDMETHOD_(void,Reset) (THIS) PURE;
};

DISPID GetEventDispID( IUnknown* );
VARIANT* GetEventArg( IUnknown*, int );
IEventHandler* CreateEventHandler( IUnknown*, const IID*, unsigned short* );

#ifdef __cplusplus
}
#endif



#endif	// !defined( HSP_COM_UNSUPPORTED )


#endif	// __comobj_event_h
