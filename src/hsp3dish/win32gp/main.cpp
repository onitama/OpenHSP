/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
									  2011/5  onitama
  --------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "hsp3dish.h"

/*----------------------------------------------------------*/

int APIENTRY WinMain ( HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPSTR lpCmdParam,
					   int iCmdShow )
{
	int res;
#ifdef HSPDEBUG
	res = hsp3dish_init( hInstance, lpCmdParam );
#else
	res = hsp3dish_init( hInstance, NULL );
#endif
	if ( res == 0 ) {
		res = hsp3dish_exec();
	}
	return res;
}



