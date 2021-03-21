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
#include <string>
#include "hsp3cl.h"
#include "../hsp3config.h"

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	int res;
	char *p;

#ifdef HSPDEBUG
	char a1,a2,a3;
	int b,st,index;
	char *cl;
	std::string clopt;

	p = "";
	cl = "";
	st = 0;
	index = 0;
	for (b=1;b<argc;b++) {
		a1=*argv[b];a2=tolower(*(argv[b]+1));
#ifdef HSPLINUX
		if (a1!='-') {
#else
		if ((a1!='/')&&(a1!='-')) {
#endif
			if ( index == 0 ) {
				p = argv[b];
			} else {
				if ( index > 1 ) clopt+=" ";
				clopt += argv[b];
			}
			index++;
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

	cl = (char *)clopt.c_str();
	hsp3cl_cmdline(cl);
	res = hsp3cl_init( p );
	if ( res ) return res;
	hsp3cl_option( st );
	res = hsp3cl_exec();

	return res;
}



