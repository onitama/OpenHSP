/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
									  2011/5  onitama
  --------------------------------------------------------*/

#include <stdio.h>

#include "hsp3dish.h"

/*----------------------------------------------------------*/

char *hsp_mainpath;

int main( int argc, char *argv[] )
{
	int res;
	char *p;

	hsp_mainpath = argv[ 0 ];
	if ( argc > 1 ) p = argv[ 1 ]; else p = "";

	res = hsp3dish_init( p );
	if ( res == 0 ) {
		res = hsp3dish_exec();
	}
	return res;
}
