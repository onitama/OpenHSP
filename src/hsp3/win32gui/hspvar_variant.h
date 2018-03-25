
//
//	hspvar_comobj.cpp header
//
#ifndef __hspvar_variant_h
#define __hspvar_variant_h


#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <objbase.h>
#include "comobj.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TYPE_VARIANT 7
#define HSPVAR_FLAG_VARIANT 7

// use as property ID
enum {
	HSPVAR_VARIANT_UNKNOWN = 0,
	HSPVAR_VARIANT_VARTYPE,
	HSPVAR_VARIANT_VARTYPE_MASKED,
	HSPVAR_VARIANT_VALUE,
	HSPVAR_VARIANT_REFPTR,
	HSPVAR_VARIANT_IS_ARRAY,
	HSPVAR_VARIANT_IS_BYREF,
	HSPVAR_VARIANT_ARRAY_ELEMENT,
	HSPVAR_VARIANT_ARRAY_PTR,
	HSPVAR_VARIANT_ARRAY_LBOUND,
	HSPVAR_VARIANT_ARRAY_UBOUND,
	HSPVAR_VARIANT_ARRAY_COUNT,
	HSPVAR_VARIANT_BSTR_PTR
};

#define HSPVAR_VARIANT_PARAM_MAX 12

typedef struct {
	VARIANT *var;							// 変数の配列要素 (VARIANT)
	int id;									// プロパティID
	int dimcount;							// SafeArray 次元数
	long index[HSPVAR_VARIANT_PARAM_MAX];	// SafeArray インデックス
} VariantParam;

void HspVarVariant_Init( HspVarProc *p );

#ifdef __cplusplus
}
#endif

#endif	// !defined( HSP_COM_UNSUPPORTED )

#endif		// __hspvar_variant_h
