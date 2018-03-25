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

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	int res;
	char *p;

#ifdef HSPDEBUG
	if ( argc > 1 ) p = argv[ 1 ]; else p = "";
#else
	p = NULL;
#endif

	res = hsp3cl_init( p );
	if ( res ) return res;
	res = hsp3cl_exec();

	return res;
}



