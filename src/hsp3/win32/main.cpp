/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
  --------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "hsp3cl.h"

#ifdef HSPUTF8
#include <windows.h>
#include <shellapi.h>
#include "../hsp3utfcnv.h"
#endif

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	int res;
	char *p;
#ifdef HSPUTF8
	LPWSTR *szArglist = NULL;
	int nArgs = 0;
	char utf8filename[4096];
#endif

#ifdef HSPDEBUG
	if ( argc > 1 ) p = argv[ 1 ]; else p = "";
#ifdef HSPUTF8
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist) {
		if (nArgs > 1) {
			if (utf16_to_hsp3(utf8filename, szArglist[1], 4095)) {
				p = utf8filename;
			}
			else {
				p = "";
			}
		}
	}
#endif
#else
	p = NULL;
#endif

	res = hsp3cl_init( p );
#ifdef HSPUTF8
	if (szArglist) LocalFree(szArglist);
#endif
	if ( res ) return res;
	res = hsp3cl_exec();

	return res;
}
