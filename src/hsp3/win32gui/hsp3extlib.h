
//
//	hsp3extlib.cpp header
//
#ifndef __hsp3extlib_h
#define __hsp3extlib_h

#include "../hsp3code.h"

#include <deque>
#include <windows.h>

//------------------------------------------------------------//

namespace hsp3 {

//------------------------------------------------------------//
/*
	拡張プラグイン・dllの管理クラス。
*/
//------------------------------------------------------------//

class CDllManager
{
	typedef std::deque< HMODULE > holder_type;

	//............................//

public:
	CDllManager();
	~CDllManager();

	HMODULE load_library( const char *lpFileName );
	BOOL free_library( HMODULE hModule );
	BOOL free_all_library();
	HMODULE get_error() const;

private:
	// uncopyable;
	CDllManager( CDllManager const & );
	CDllManager const & operator =( CDllManager const & );

	//............................//

private:
	holder_type mModules;
	HMODULE mError;
};

//------------------------------------------------------------//

};	//namespace hsp3 {

//------------------------------------------------------------//

hsp3::CDllManager & DllManager();

int Hsp3ExtLibInit( HSP3TYPEINFO *info );
void Hsp3ExtLibTerm( void );

int cmdfunc_dllcmd( int cmd );
int exec_dllcmd( int cmd, int mask );
int code_expand_and_call( const STRUCTDAT *st );

#ifdef HSP64

extern "C" INT_PTR CallFunc64(INT_PTR *, FARPROC, int);
#define call_extfunc(externalFunction, arguments, numberOfArguments)	CallFunc64((INT_PTR *)arguments, (FARPROC)externalFunction, numberOfArguments)

#else

#if defined( __GNUC__ )
int __cdecl call_extfunc( void *proc, int *prm, int prms ) __attribute__(( noinline ));
#else
int __cdecl call_extfunc( void *proc, int *prm, int prms );
#endif

#endif


int cnvwstr( void *out, char *in, int bufsize );
int cnvsjis( void *out, char *in, int bufsize );
int cnvu8( void *out, wchar_t *in, int bufsize);

#endif
