
//
//	HSPCC : HSP Code Compiler Manager
//				onion software 2002/12
//
#ifdef HSPWIN
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef HSPLINUX
#include <unistd.h>
#endif

#ifdef HSPWIN
#include <direct.h>
#endif
#include <string.h>
#include <ctype.h>

#include "../hsp3/hsp3config.h"
#include "supio.h"

#include "hsc3.h"
#include "token.h"
#include "hsmanager.h"

/*----------------------------------------------------------*/

static void usage1( void )
{
static 	char *p[] = {
	"usage: hspcmp [options] [filename]",
	"       -o??? set output file to ???",
	"       -d    add debug information",
	"       -p    preprocessor only",
	"       -c    HSP2.55 compatible mode",
	"       -i    input UTF-8 source code",
	"       -u    output UTF-8 strings",
	"       -w    force debug window on",
	"       -e?   execute/view .ax runtime",
	"       -r    execute runtime with result",
	"       -s    output string map",
	"       -h??? output command help",
	"       --syspath=??? set system folder for execute",
	"       --compath=??? set common path to ???",
	NULL };
	int i;
	for(i=0; p[i]; i++)
		printf( "%s\n", p[i]);
}

/*----------------------------------------------------------*/

int main( int argc, char *argv[] )
{
	char a1,a2,a3;
	int b,st;
	int cmpopt,ppopt,utfopt,pponly,execobj,strmap,hsphelp;
	char fname[HSP_MAX_PATH];
	char fname2[HSP_MAX_PATH];
	char oname[HSP_MAX_PATH];
	char compath[HSP_MAX_PATH];
	char syspath[HSP_MAX_PATH];
	char helpkey[256];
	CHsc3 *hsc3=NULL;

	//	check switch and prm

	if (argc<2) { usage1();return -1; }

	st = 0; ppopt = 0; cmpopt = 0; utfopt = 0; pponly = 0; strmap = 0; hsphelp = 0;
	execobj = 0;
	fname[0]=0;
	fname2[0]=0;
	oname[0]=0;
	syspath[0]=0;
	helpkey[0] = 0;

#ifdef HSPLINUX
	strcpy( compath,"common/" );
#else
	strcpy( compath,"common\\" );
#endif

	for (b=1;b<argc;b++) {
		a1=*argv[b];a2=tolower(*(argv[b]+1));
#ifdef HSPLINUX
		if (a1!='-') {
#else
		if ((a1!='/')&&(a1!='-')) {
#endif
			strcpy(fname,argv[b]);
		} else {
			a3=tolower(*(argv[b]+2));
			if (strncmp(argv[b], "--compath=", 10) == 0) {
				strcpy( compath, argv[b] + 10 );
				continue;
			}
			if (strncmp(argv[b], "--syspath=", 10) == 0) {
				strcpy( syspath, argv[b] + 10 );
				continue;
			}
			switch (a2) {
			case 'c':
				ppopt |= HSC3_OPT_NOHSPDEF; break;
			case 'p':
				pponly=1; break;
			case 'd':
				ppopt |= HSC3_OPT_DEBUGMODE; cmpopt|=HSC3_MODE_DEBUG; break;
			case 'i':
				ppopt |= HSC3_OPT_UTF8IN; utfopt=1; cmpopt|=HSC3_MODE_UTF8; break;
			case 'u':
				utfopt=1; cmpopt|=HSC3_MODE_UTF8; break;
			case 'w':
				cmpopt|=HSC3_MODE_DEBUGWIN; break;
			case 's':
				strmap = 1; cmpopt |= HSC3_MODE_STRMAP; break;
			case 'o':
				strcpy(oname, argv[b] + 2);
				break;
			case 'e':
				execobj = 1;
				if ( a3=='0' ) execobj|=8;
				break;
			case 'r':
				execobj = 2;
				if ( a3=='0' ) execobj|=8;
				break;
			case 'h':
				hsphelp = 1;
				strcpy(helpkey, argv[b] + 2);
				break;
			default:
				st=1;break;
			}
		}
	}

	if (st) { printf("Illegal switch selected.\n");return 1; }

	//		help main
	if (hsphelp) {
		int res;
		HspHelpManager hman;
		strcat(syspath, "hsphelp");
		res = hman.initalize(syspath);
		if (res == 0) {
			res = hman.searchIndex(helpkey);
		}
		puts(hman.getMessage());
		return res;
	}

	if (fname[0]==0) { printf("No file name selected.\n");return 1; }

	if (oname[0]==0) {
		strcpy( oname,fname ); cutext( oname );
		if (strmap) {
			addext(oname, "strmap");
		}
		else {
			addext(oname, "ax");
		}
	}
	strcpy( fname2, fname ); cutext( fname2 ); addext( fname2,"i" );
	addext( fname,"hsp" );			// 拡張子がなければ追加する

	//		call main

	hsc3 = new CHsc3;

	hsc3->SetCommonPath( compath );

	if ( execobj ) {
		//		ランタイムを起動
		char execmd[4096];
		st = hsc3->GetRuntimeFromHeader( fname, oname );
		if ( st != 1 ) {
			strcpy( oname, "hsp3.exe" );			// デフォルトランタイム
		}

#ifdef HSPLINUX
		cutext( oname );
		if ( execobj & 8 ) {
			printf("Runtime[%s].\n",oname);
		} else {
			int result;
			printf("Execute from %s runtime[%s](%d).\n",fname,oname,execobj);
			sprintf(execmd,"%s./%s %s",syspath,oname,fname);
			//sprintf(execmd,"%s./%s %s >%s.hspres",syspath,oname,fname,syspath);
			
			result = system(execmd);
			if ( WIFEXITED(result) ) {
				result = WEXITSTATUS(result);
				printf("hsed: Process end %d.\n",result);
				if ( execobj==2 ) {
					if ( result != 0 ) {			// エラーがあった時
						while(1) {
							result = getchar();
							if (( result == 13 )||( result == 10 )) break;
						}
					}
				}
			} else {
				printf("hsed: Process error.\n");
			}
		}
#else
		if ( execobj & 8 ) {
			printf("Runtime[%s].\n",oname);
		} else {
			sprintf( execmd, "%s %s", oname, fname );
			st = WinExec( execmd, SW_SHOW );
			if ( st < 32 ) {
				printf("Runtime file missing.\n");
			}
		}
#endif

	} else {
		//		通常のコンパイル
		st = hsc3->PreProcess( fname, fname2, ppopt, fname );
		if (( pponly == 0 )&&( st == 0 )) {
			st = hsc3->Compile( fname2, oname, cmpopt );
		}
		puts( hsc3->GetError() );
		hsc3->PreProcessEnd();
	}

	if ( hsc3 != NULL ) { delete hsc3; hsc3=NULL; }
	return st;
}

