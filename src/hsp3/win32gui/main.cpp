/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
  --------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "../hsp3config.h"
#include "../supio.h"
#include "hsp3win.h"

#ifdef HSPUTF8
#include <shellapi.h>
#include "../hsp3utfcnv.h"
static LPWSTR* szArglist;
static int nArgs;
#endif

/*----------------------------------------------------------*/

int APIENTRY WinMain ( HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPSTR lpCmdParam,
					   int iCmdShow )
{
	int res;
#ifdef HSPDEBUG
#ifdef HSPUTF8
	char* sptr = lpCmdParam;
	char utf8filename[4096];
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist) {
		if (nArgs > 1) {
			utf16_to_hsp3(utf8filename, szArglist[1], 4095);
			sptr = utf8filename;
		}
	}
	res = hsp3win_init( hInstance, sptr);
	LocalFree(szArglist);
#else
	res = hsp3win_init(hInstance, lpCmdParam);
#endif
#else
	res = hsp3win_init( hInstance, NULL );
#endif
	if ( res == 0 ) {
		res = hsp3win_exec();
	}
	return res;
}



