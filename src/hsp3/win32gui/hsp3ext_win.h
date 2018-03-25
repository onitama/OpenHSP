
//
//	hsp3ext.cpp header
//
#ifndef __hsp3ext_win_h
#define __hsp3ext_win_h

#include "../hsp3code.h"
#include "hspvar_comobj.h"

void hsp3typeinit_dllcmd( HSP3TYPEINFO *info );
void hsp3typeinit_dllctrl( HSP3TYPEINFO *info );

void DeleteHSPObject( int wid, int objid );

#ifndef HSP_COM_UNSUPPORTED

int call_method( void *iptr, int index, int *prm, int prms );
int call_method2( char *prmbuf, const STRUCTDAT *st );

DISPID get_dispid( IUnknown *punk, char *name, BOOL *bVariant );
void comset_variant( VARIANT *var, void *data, int vtype );
void *comget_variant( VARIANT *var, int *restype, BOOL variantret = FALSE );
BSTR comget_bstr( char *ps );

void comget_variantres( VARIANT *var, HRESULT hr, BOOL noconvret );

#endif	// !defined( HSP_COM_UNSUPPORTED )



#endif
