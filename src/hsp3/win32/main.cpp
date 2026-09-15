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
#include "../hsp3pathio.h"
#endif

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	int res;
	const char *p;
#if defined(HSPDEBUG) && defined(HSPUTF8)
	std::string startfile;
#endif

#ifdef HSPDEBUG
	if ( argc > 1 ) p = argv[ 1 ]; else p = "";
#ifdef HSPUTF8
	if (hsp_path_get_command_line_argument_utf8(startfile, 1) == 0) p = startfile.c_str();
	else p = "";
#endif
#else
	p = NULL;
#endif

	res = hsp3cl_init( p );
	if ( res ) return res;
	res = hsp3cl_exec();

	return res;
}
