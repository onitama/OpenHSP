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
#include "../hsp3pathio.h"
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
	std::string startfile;
	const char *sptr = "";
	if (hsp_path_get_command_line_argument_utf8(startfile, 1) == 0) {
		sptr = startfile.c_str();
	}
	res = hsp3win_init(hInstance, sptr);
#else
	char fname[_MAX_PATH + 1];
	char *ss = lpCmdParam;
	if (lpCmdParam != NULL) {
		ss = strsp_cmds(lpCmdParam);
		int i = (int)(ss - lpCmdParam);
		ss = lpCmdParam;
		if (i > 0 && ss[i - 1] == 32) i--;
		if (*ss == 0x22) {
			ss++;
			i = i >= 2 ? i - 2 : 0;
		}
		if (i > _MAX_PATH) return 1;
		if (i > 0) memcpy(fname, ss, (size_t)i);
		fname[i] = 0;
		ss = fname;
	}
	res = hsp3win_init(hInstance, ss);
#endif
#else
	res = hsp3win_init( hInstance, NULL );
#endif
	if ( res == 0 ) {
		res = hsp3win_exec();
	}
	return res;
}
