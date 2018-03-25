
//
//	hspvar_comobj.cpp header
//
#ifndef __hspvar_comobj_h
#define __hspvar_comobj_h


#ifndef HSP_COM_UNSUPPORTED		//（COM サポートなし版のビルド時はファイル全体を無視）



#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <objbase.h>
#include "comobj.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TYPE_COMOBJ 6


void HspVarComobj_Init( HspVarProc *p );

#ifdef __cplusplus
}
#endif


#endif	// !defined( HSP_COM_UNSUPPORTED )


#endif	// __hspvar_comobj_h
