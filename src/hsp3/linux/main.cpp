/*--------------------------------------------------------
	HSP3 interpreter main
									  1995/10 onitama
									  1997/7  onitama
									  1999/8  onitama
									  2003/4  onitama
  --------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "hsp3cl.h"
#include "../hsp3config.h"

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	int res;
	char *p;

#ifdef HSPDEBUG
	char a1,a2,a3;
	int b,st;

	p = "";
	st = 0;
	for (b=1;b<argc;b++) {
		a1=*argv[b];a2=tolower(*(argv[b]+1));
#ifdef HSPLINUX
		if (a1!='-') {
#else
		if ((a1!='/')&&(a1!='-')) {
#endif
			p = argv[b];
		} else {
			switch (a2) {
			case 'r':
				st |= HSP3CL_OPT1_RESOUT; break;
			case 'p':
				st |= HSP3CL_OPT1_ERRSTOP; break;
			default:
				printf("Illegal switch selected.\n");
				break;
			}
		}
	}
#else
	p = NULL;
#endif

	res = hsp3cl_init( p );
	if ( res ) return res;
	hsp3cl_option( st );
	res = hsp3cl_exec();

	return res;
}



