
//
//	hsp3extlib.cpp header
//
#ifndef __hsp3extlib_h
#define __hsp3extlib_h

#include "../hsp3code.h"

#include <deque>

#ifdef _WIN32
typedef HINSTANCE HANDLE_MODULE;
#else
typedef void * HANDLE_MODULE;
#endif
//------------------------------------------------------------//

namespace hsp3 {

//------------------------------------------------------------//
/*
	拡張プラグイン・dllの管理クラス。
*/
//------------------------------------------------------------//

class CDllManager
{
	typedef std::deque< HANDLE_MODULE > holder_type;

	//............................//

public:
	CDllManager();
	~CDllManager();

	HANDLE_MODULE load_library( const char *lpFileName );
	bool free_library( HANDLE_MODULE hModule );
	bool free_all_library();
	HANDLE_MODULE get_error() const;

private:
	// uncopyable;
	CDllManager( CDllManager const & );
	CDllManager const & operator =( CDllManager const & );

	//............................//

private:
	holder_type mModules;
	HANDLE_MODULE mError;
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

/*
#if defined(HSP64) || defined(PTR64BIT)

extern "C" int CallFunc64(int **, void*, int);
#define call_extfunc(externalFunction, arguments, numberOfArguments)	CallFunc64((int**)(arguments), (void*)(externalFunction), numberOfArguments)

#else
int call_extfunc( void *proc, int **prm, int prms );
#endif
*/

int cnvwstr( void *out, char *in, int bufsize );
int cnvsjis( void *out, char *in, int bufsize );
int cnvu8( void *out, wchar_t *in, int bufsize);

#endif
