/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Aug 23 15:08:57 2002
 */
/* Compiler settings for C:\DOCUME~1\bosuke\LOCALS~1\Temp\IDLC7.tmp:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __vbscript_h__
#define __vbscript_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IRegExp_FWD_DEFINED__
#define __IRegExp_FWD_DEFINED__
typedef interface IRegExp IRegExp;
#endif 	/* __IRegExp_FWD_DEFINED__ */


#ifndef __IMatch_FWD_DEFINED__
#define __IMatch_FWD_DEFINED__
typedef interface IMatch IMatch;
#endif 	/* __IMatch_FWD_DEFINED__ */


#ifndef __IMatchCollection_FWD_DEFINED__
#define __IMatchCollection_FWD_DEFINED__
typedef interface IMatchCollection IMatchCollection;
#endif 	/* __IMatchCollection_FWD_DEFINED__ */


#ifndef __IRegExp2_FWD_DEFINED__
#define __IRegExp2_FWD_DEFINED__
typedef interface IRegExp2 IRegExp2;
#endif 	/* __IRegExp2_FWD_DEFINED__ */


#ifndef __IMatch2_FWD_DEFINED__
#define __IMatch2_FWD_DEFINED__
typedef interface IMatch2 IMatch2;
#endif 	/* __IMatch2_FWD_DEFINED__ */


#ifndef __IMatchCollection2_FWD_DEFINED__
#define __IMatchCollection2_FWD_DEFINED__
typedef interface IMatchCollection2 IMatchCollection2;
#endif 	/* __IMatchCollection2_FWD_DEFINED__ */


#ifndef __ISubMatches_FWD_DEFINED__
#define __ISubMatches_FWD_DEFINED__
typedef interface ISubMatches ISubMatches;
#endif 	/* __ISubMatches_FWD_DEFINED__ */


#ifndef __RegExp_FWD_DEFINED__
#define __RegExp_FWD_DEFINED__

#ifdef __cplusplus
typedef class RegExp RegExp;
#else
typedef struct RegExp RegExp;
#endif /* __cplusplus */

#endif 	/* __RegExp_FWD_DEFINED__ */


#ifndef __Match_FWD_DEFINED__
#define __Match_FWD_DEFINED__

#ifdef __cplusplus
typedef class Match Match;
#else
typedef struct Match Match;
#endif /* __cplusplus */

#endif 	/* __Match_FWD_DEFINED__ */


#ifndef __MatchCollection_FWD_DEFINED__
#define __MatchCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class MatchCollection MatchCollection;
#else
typedef struct MatchCollection MatchCollection;
#endif /* __cplusplus */

#endif 	/* __MatchCollection_FWD_DEFINED__ */


#ifndef __SubMatches_FWD_DEFINED__
#define __SubMatches_FWD_DEFINED__

#ifdef __cplusplus
typedef class SubMatches SubMatches;
#else
typedef struct SubMatches SubMatches;
#endif /* __cplusplus */

#endif 	/* __SubMatches_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __VBScript_RegExp_55_LIBRARY_DEFINED__
#define __VBScript_RegExp_55_LIBRARY_DEFINED__

/* library VBScript_RegExp_55 */
/* [helpstring][version][uuid] */ 









EXTERN_C const IID LIBID_VBScript_RegExp_55;

#ifndef __IRegExp_INTERFACE_DEFINED__
#define __IRegExp_INTERFACE_DEFINED__

/* interface IRegExp */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IRegExp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACA0-160D-11D2-A8E9-00104B365C9F")
    IRegExp : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Pattern( 
            /* [retval][out] */ BSTR __RPC_FAR *pPattern) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Pattern( 
            /* [in] */ BSTR pPattern) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IgnoreCase( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IgnoreCase( 
            /* [in] */ VARIANT_BOOL pIgnoreCase) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Global( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Global( 
            /* [in] */ VARIANT_BOOL pGlobal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Test( 
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Replace( 
            /* [in] */ BSTR sourceString,
            /* [in] */ BSTR replaceString,
            /* [retval][out] */ BSTR __RPC_FAR *pDestString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRegExpVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRegExp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRegExp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRegExp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pattern )( 
            IRegExp __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pPattern);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Pattern )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ BSTR pPattern);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IgnoreCase )( 
            IRegExp __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IgnoreCase )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL pIgnoreCase);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Global )( 
            IRegExp __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Global )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL pGlobal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Test )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Replace )( 
            IRegExp __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [in] */ BSTR replaceString,
            /* [retval][out] */ BSTR __RPC_FAR *pDestString);
        
        END_INTERFACE
    } IRegExpVtbl;

    interface IRegExp
    {
        CONST_VTBL struct IRegExpVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegExp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRegExp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRegExp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRegExp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRegExp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRegExp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRegExp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRegExp_get_Pattern(This,pPattern)	\
    (This)->lpVtbl -> get_Pattern(This,pPattern)

#define IRegExp_put_Pattern(This,pPattern)	\
    (This)->lpVtbl -> put_Pattern(This,pPattern)

#define IRegExp_get_IgnoreCase(This,pIgnoreCase)	\
    (This)->lpVtbl -> get_IgnoreCase(This,pIgnoreCase)

#define IRegExp_put_IgnoreCase(This,pIgnoreCase)	\
    (This)->lpVtbl -> put_IgnoreCase(This,pIgnoreCase)

#define IRegExp_get_Global(This,pGlobal)	\
    (This)->lpVtbl -> get_Global(This,pGlobal)

#define IRegExp_put_Global(This,pGlobal)	\
    (This)->lpVtbl -> put_Global(This,pGlobal)

#define IRegExp_Execute(This,sourceString,ppMatches)	\
    (This)->lpVtbl -> Execute(This,sourceString,ppMatches)

#define IRegExp_Test(This,sourceString,pMatch)	\
    (This)->lpVtbl -> Test(This,sourceString,pMatch)

#define IRegExp_Replace(This,sourceString,replaceString,pDestString)	\
    (This)->lpVtbl -> Replace(This,sourceString,replaceString,pDestString)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp_get_Pattern_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pPattern);


void __RPC_STUB IRegExp_get_Pattern_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp_put_Pattern_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ BSTR pPattern);


void __RPC_STUB IRegExp_put_Pattern_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp_get_IgnoreCase_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase);


void __RPC_STUB IRegExp_get_IgnoreCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp_put_IgnoreCase_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL pIgnoreCase);


void __RPC_STUB IRegExp_put_IgnoreCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp_get_Global_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal);


void __RPC_STUB IRegExp_get_Global_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp_put_Global_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL pGlobal);


void __RPC_STUB IRegExp_put_Global_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp_Execute_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches);


void __RPC_STUB IRegExp_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp_Test_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch);


void __RPC_STUB IRegExp_Test_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp_Replace_Proxy( 
    IRegExp __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [in] */ BSTR replaceString,
    /* [retval][out] */ BSTR __RPC_FAR *pDestString);


void __RPC_STUB IRegExp_Replace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRegExp_INTERFACE_DEFINED__ */


#ifndef __IMatch_INTERFACE_DEFINED__
#define __IMatch_INTERFACE_DEFINED__

/* interface IMatch */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IMatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACA1-160D-11D2-A8E9-00104B365C9F")
    IMatch : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ BSTR __RPC_FAR *pValue) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FirstIndex( 
            /* [retval][out] */ long __RPC_FAR *pFirstIndex) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ long __RPC_FAR *pLength) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IMatch __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pValue);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FirstIndex )( 
            IMatch __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFirstIndex);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Length )( 
            IMatch __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLength);
        
        END_INTERFACE
    } IMatchVtbl;

    interface IMatch
    {
        CONST_VTBL struct IMatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMatch_get_Value(This,pValue)	\
    (This)->lpVtbl -> get_Value(This,pValue)

#define IMatch_get_FirstIndex(This,pFirstIndex)	\
    (This)->lpVtbl -> get_FirstIndex(This,pFirstIndex)

#define IMatch_get_Length(This,pLength)	\
    (This)->lpVtbl -> get_Length(This,pLength)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch_get_Value_Proxy( 
    IMatch __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pValue);


void __RPC_STUB IMatch_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch_get_FirstIndex_Proxy( 
    IMatch __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pFirstIndex);


void __RPC_STUB IMatch_get_FirstIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch_get_Length_Proxy( 
    IMatch __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLength);


void __RPC_STUB IMatch_get_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMatch_INTERFACE_DEFINED__ */


#ifndef __IMatchCollection_INTERFACE_DEFINED__
#define __IMatchCollection_INTERFACE_DEFINED__

/* interface IMatchCollection */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IMatchCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACA2-160D-11D2-A8E9-00104B365C9F")
    IMatchCollection : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMatchCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMatchCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMatchCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMatchCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMatchCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMatchCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMatchCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMatchCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IMatchCollection __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMatchCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IMatchCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IMatchCollectionVtbl;

    interface IMatchCollection
    {
        CONST_VTBL struct IMatchCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMatchCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMatchCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMatchCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMatchCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMatchCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMatchCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMatchCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMatchCollection_get_Item(This,index,ppMatch)	\
    (This)->lpVtbl -> get_Item(This,index,ppMatch)

#define IMatchCollection_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IMatchCollection_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection_get_Item_Proxy( 
    IMatchCollection __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch);


void __RPC_STUB IMatchCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection_get_Count_Proxy( 
    IMatchCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IMatchCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection_get__NewEnum_Proxy( 
    IMatchCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IMatchCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMatchCollection_INTERFACE_DEFINED__ */


#ifndef __IRegExp2_INTERFACE_DEFINED__
#define __IRegExp2_INTERFACE_DEFINED__

/* interface IRegExp2 */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IRegExp2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACB0-160D-11D2-A8E9-00104B365C9F")
    IRegExp2 : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Pattern( 
            /* [retval][out] */ BSTR __RPC_FAR *pPattern) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Pattern( 
            /* [in] */ BSTR pPattern) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_IgnoreCase( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_IgnoreCase( 
            /* [in] */ VARIANT_BOOL pIgnoreCase) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Global( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Global( 
            /* [in] */ VARIANT_BOOL pGlobal) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Multiline( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMultiline) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Multiline( 
            /* [in] */ VARIANT_BOOL pMultiline) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Test( 
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Replace( 
            /* [in] */ BSTR sourceString,
            /* [in] */ VARIANT replaceVar,
            /* [retval][out] */ BSTR __RPC_FAR *pDestString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRegExp2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRegExp2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRegExp2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRegExp2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Pattern )( 
            IRegExp2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pPattern);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Pattern )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ BSTR pPattern);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IgnoreCase )( 
            IRegExp2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IgnoreCase )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL pIgnoreCase);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Global )( 
            IRegExp2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Global )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL pGlobal);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Multiline )( 
            IRegExp2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMultiline);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Multiline )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL pMultiline);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Execute )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Test )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Replace )( 
            IRegExp2 __RPC_FAR * This,
            /* [in] */ BSTR sourceString,
            /* [in] */ VARIANT replaceVar,
            /* [retval][out] */ BSTR __RPC_FAR *pDestString);
        
        END_INTERFACE
    } IRegExp2Vtbl;

    interface IRegExp2
    {
        CONST_VTBL struct IRegExp2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegExp2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRegExp2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRegExp2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRegExp2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRegExp2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRegExp2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRegExp2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRegExp2_get_Pattern(This,pPattern)	\
    (This)->lpVtbl -> get_Pattern(This,pPattern)

#define IRegExp2_put_Pattern(This,pPattern)	\
    (This)->lpVtbl -> put_Pattern(This,pPattern)

#define IRegExp2_get_IgnoreCase(This,pIgnoreCase)	\
    (This)->lpVtbl -> get_IgnoreCase(This,pIgnoreCase)

#define IRegExp2_put_IgnoreCase(This,pIgnoreCase)	\
    (This)->lpVtbl -> put_IgnoreCase(This,pIgnoreCase)

#define IRegExp2_get_Global(This,pGlobal)	\
    (This)->lpVtbl -> get_Global(This,pGlobal)

#define IRegExp2_put_Global(This,pGlobal)	\
    (This)->lpVtbl -> put_Global(This,pGlobal)

#define IRegExp2_get_Multiline(This,pMultiline)	\
    (This)->lpVtbl -> get_Multiline(This,pMultiline)

#define IRegExp2_put_Multiline(This,pMultiline)	\
    (This)->lpVtbl -> put_Multiline(This,pMultiline)

#define IRegExp2_Execute(This,sourceString,ppMatches)	\
    (This)->lpVtbl -> Execute(This,sourceString,ppMatches)

#define IRegExp2_Test(This,sourceString,pMatch)	\
    (This)->lpVtbl -> Test(This,sourceString,pMatch)

#define IRegExp2_Replace(This,sourceString,replaceVar,pDestString)	\
    (This)->lpVtbl -> Replace(This,sourceString,replaceVar,pDestString)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_get_Pattern_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pPattern);


void __RPC_STUB IRegExp2_get_Pattern_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_put_Pattern_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ BSTR pPattern);


void __RPC_STUB IRegExp2_put_Pattern_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_get_IgnoreCase_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pIgnoreCase);


void __RPC_STUB IRegExp2_get_IgnoreCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_put_IgnoreCase_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL pIgnoreCase);


void __RPC_STUB IRegExp2_put_IgnoreCase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_get_Global_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pGlobal);


void __RPC_STUB IRegExp2_get_Global_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_put_Global_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL pGlobal);


void __RPC_STUB IRegExp2_put_Global_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_get_Multiline_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMultiline);


void __RPC_STUB IRegExp2_get_Multiline_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IRegExp2_put_Multiline_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL pMultiline);


void __RPC_STUB IRegExp2_put_Multiline_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp2_Execute_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatches);


void __RPC_STUB IRegExp2_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp2_Test_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pMatch);


void __RPC_STUB IRegExp2_Test_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IRegExp2_Replace_Proxy( 
    IRegExp2 __RPC_FAR * This,
    /* [in] */ BSTR sourceString,
    /* [in] */ VARIANT replaceVar,
    /* [retval][out] */ BSTR __RPC_FAR *pDestString);


void __RPC_STUB IRegExp2_Replace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRegExp2_INTERFACE_DEFINED__ */


#ifndef __IMatch2_INTERFACE_DEFINED__
#define __IMatch2_INTERFACE_DEFINED__

/* interface IMatch2 */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IMatch2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACB1-160D-11D2-A8E9-00104B365C9F")
    IMatch2 : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ BSTR __RPC_FAR *pValue) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FirstIndex( 
            /* [retval][out] */ long __RPC_FAR *pFirstIndex) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ long __RPC_FAR *pLength) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_SubMatches( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppSubMatches) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMatch2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMatch2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMatch2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMatch2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMatch2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMatch2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMatch2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMatch2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IMatch2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pValue);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FirstIndex )( 
            IMatch2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pFirstIndex);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Length )( 
            IMatch2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pLength);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubMatches )( 
            IMatch2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppSubMatches);
        
        END_INTERFACE
    } IMatch2Vtbl;

    interface IMatch2
    {
        CONST_VTBL struct IMatch2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMatch2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMatch2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMatch2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMatch2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMatch2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMatch2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMatch2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMatch2_get_Value(This,pValue)	\
    (This)->lpVtbl -> get_Value(This,pValue)

#define IMatch2_get_FirstIndex(This,pFirstIndex)	\
    (This)->lpVtbl -> get_FirstIndex(This,pFirstIndex)

#define IMatch2_get_Length(This,pLength)	\
    (This)->lpVtbl -> get_Length(This,pLength)

#define IMatch2_get_SubMatches(This,ppSubMatches)	\
    (This)->lpVtbl -> get_SubMatches(This,ppSubMatches)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch2_get_Value_Proxy( 
    IMatch2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pValue);


void __RPC_STUB IMatch2_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch2_get_FirstIndex_Proxy( 
    IMatch2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pFirstIndex);


void __RPC_STUB IMatch2_get_FirstIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch2_get_Length_Proxy( 
    IMatch2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pLength);


void __RPC_STUB IMatch2_get_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatch2_get_SubMatches_Proxy( 
    IMatch2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppSubMatches);


void __RPC_STUB IMatch2_get_SubMatches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMatch2_INTERFACE_DEFINED__ */


#ifndef __IMatchCollection2_INTERFACE_DEFINED__
#define __IMatchCollection2_INTERFACE_DEFINED__

/* interface IMatchCollection2 */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IMatchCollection2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACB2-160D-11D2-A8E9-00104B365C9F")
    IMatchCollection2 : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMatchCollection2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMatchCollection2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMatchCollection2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IMatchCollection2 __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } IMatchCollection2Vtbl;

    interface IMatchCollection2
    {
        CONST_VTBL struct IMatchCollection2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMatchCollection2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMatchCollection2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMatchCollection2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMatchCollection2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMatchCollection2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMatchCollection2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMatchCollection2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMatchCollection2_get_Item(This,index,ppMatch)	\
    (This)->lpVtbl -> get_Item(This,index,ppMatch)

#define IMatchCollection2_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IMatchCollection2_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection2_get_Item_Proxy( 
    IMatchCollection2 __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppMatch);


void __RPC_STUB IMatchCollection2_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection2_get_Count_Proxy( 
    IMatchCollection2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IMatchCollection2_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IMatchCollection2_get__NewEnum_Proxy( 
    IMatchCollection2 __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IMatchCollection2_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMatchCollection2_INTERFACE_DEFINED__ */


#ifndef __ISubMatches_INTERFACE_DEFINED__
#define __ISubMatches_INTERFACE_DEFINED__

/* interface ISubMatches */
/* [object][oleautomation][nonextensible][dual][hidden][uuid] */ 


EXTERN_C const IID IID_ISubMatches;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3F4DACB3-160D-11D2-A8E9-00104B365C9F")
    ISubMatches : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long index,
            /* [retval][out] */ VARIANT __RPC_FAR *pSubMatch) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISubMatchesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISubMatches __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISubMatches __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISubMatches __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISubMatches __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISubMatches __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISubMatches __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISubMatches __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISubMatches __RPC_FAR * This,
            /* [in] */ long index,
            /* [retval][out] */ VARIANT __RPC_FAR *pSubMatch);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISubMatches __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISubMatches __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } ISubMatchesVtbl;

    interface ISubMatches
    {
        CONST_VTBL struct ISubMatchesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISubMatches_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISubMatches_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISubMatches_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISubMatches_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISubMatches_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISubMatches_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISubMatches_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISubMatches_get_Item(This,index,pSubMatch)	\
    (This)->lpVtbl -> get_Item(This,index,pSubMatch)

#define ISubMatches_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ISubMatches_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE ISubMatches_get_Item_Proxy( 
    ISubMatches __RPC_FAR * This,
    /* [in] */ long index,
    /* [retval][out] */ VARIANT __RPC_FAR *pSubMatch);


void __RPC_STUB ISubMatches_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE ISubMatches_get_Count_Proxy( 
    ISubMatches __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ISubMatches_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE ISubMatches_get__NewEnum_Proxy( 
    ISubMatches __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ISubMatches_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISubMatches_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_RegExp;

#ifdef __cplusplus

class DECLSPEC_UUID("3F4DACA4-160D-11D2-A8E9-00104B365C9F")
RegExp;
#endif

EXTERN_C const CLSID CLSID_Match;

#ifdef __cplusplus

class DECLSPEC_UUID("3F4DACA5-160D-11D2-A8E9-00104B365C9F")
Match;
#endif

EXTERN_C const CLSID CLSID_MatchCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("3F4DACA6-160D-11D2-A8E9-00104B365C9F")
MatchCollection;
#endif

EXTERN_C const CLSID CLSID_SubMatches;

#ifdef __cplusplus

class DECLSPEC_UUID("3F4DACC0-160D-11D2-A8E9-00104B365C9F")
SubMatches;
#endif
#endif /* __VBScript_RegExp_55_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
