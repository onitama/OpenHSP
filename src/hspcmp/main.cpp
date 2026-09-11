
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
#include <string>
#ifdef HSPWIN
#include <vector>
#endif

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3pathio.h"
#include "supio.h"

#include "hsc3.h"
#include "token.h"
#include "hsmanager.h"

namespace {

static void addext(std::string& path, const char* extension)
{
	std::string current_extension;
	if (getpath(path, current_extension, 2)) {
		if (current_extension.empty()) {
			path += ".";
			path += extension;
		}
		return;
	}
	size_t separator = path.find_last_of("/\\");
	size_t dot = path.find_last_of('.');
	if (dot == std::string::npos || (separator != std::string::npos && dot <= separator)) {
		path += ".";
		path += extension;
	}
}

}

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
	"       -x    select HSP32 runtime",
	"       ---------------------------------",
	"       -h??? print command help",
	"       -lk???  print HSP3 keyword list",
	"       -ll???  print source label list",
	"       -lv???  print source variable list",
	"       -ls???  print source keyword list",
	"       -lr     include reference list",
	"       -lp     allow partial matches.",
	"       ---------------------------------",
	"       --syspath=??? set system folder for execute",
	"       --compath=??? set common path to ???",
	NULL };
	int i;
	for(i=0; p[i]; i++)
		printf( "%s\n", p[i]);
}

/*----------------------------------------------------------*/

#ifdef HSPWIN
int wmain( int argc, wchar_t *argv[] )
#else
int main( int argc, char *argv[] )
#endif
{
	char a1,a2,a3;
	int b,st;
	int cmpopt,ppopt,utfopt,pponly,execobj,strmap,hsphelp,hsp64;
	char *opt_lk = NULL;
	char *opt_ls = NULL;
	int opt_lsref, opt_lsmode;
	std::string fname;
	std::string fname2;
	std::string oname;
	std::string compath;
	std::string syspath;
	std::string helpkey;
	CHsc3 *hsc3=NULL;

#ifdef HSPWIN
	std::vector<std::string> utf8_args;
	utf8_args.reserve(argc);
	for (int i = 0; i < argc; ++i) {
		std::string converted;
		if (hsp_path_utf8_from_wide(converted, argv[i]) != 0) return 1;
		utf8_args.push_back(converted);
	}
#endif

	//	check switch and prm

	if (argc<2) { usage1();return -1; }

	st = 0; ppopt = 0; cmpopt = 0; utfopt = 0; pponly = 0; strmap = 0; hsphelp = 0; opt_lsref = 0; opt_lsmode = 0; hsp64 = 1;
	execobj = 0;
#ifdef HSPLINUX
	compath = "common/";
#else
	compath = "common\\";
#endif

	for (b=1;b<argc;b++) {
#ifdef HSPWIN
		const char* arg = utf8_args[b].c_str();
#else
		const char* arg = argv[b];
#endif
		a1=*arg;a2=tolower(*(arg+1));
#ifdef HSPLINUX
		if (a1!='-') {
#else
		if ((a1!='/')&&(a1!='-')) {
#endif
			fname = arg;
		} else {
			a3=tolower(*(arg+2));
			if (strncmp(arg, "--compath=", 10) == 0) {
				compath = arg + 10;
				continue;
			}
			if (strncmp(arg, "--syspath=", 10) == 0) {
				syspath = arg + 10;
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
				utfopt = 1; cmpopt |= HSC3_MODE_UTF8; break;
			case 'j':
				ppopt |= HSC3_OPT_UTF8IN; utfopt = 1; break;
			case 'w':
				cmpopt|=HSC3_MODE_DEBUGWIN; break;
			case 's':
				strmap = 1; cmpopt |= HSC3_MODE_STRMAP; break;
			case 'm':
				ppopt |= HSC3_OPT_EMSCRIPTEN; break;
			case 'o':
				oname = arg + 2;
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
				helpkey = arg + 2;
				break;
			case 'l':
				if (a3 == 'k') {
					opt_lk = (char*)arg + 3; break;
				}
				if (a3 == 'l') {
					opt_lsmode = 0;
					opt_ls = (char*)arg + 3; break;
				}
				if (a3 == 'v') {
					opt_lsmode = 1;
					opt_ls = (char*)arg + 3; break;
				}
				if (a3 == 's') {
					opt_lsmode = 2;
					opt_ls = (char*)arg + 3; break;
				}
				if (a3 == 'r') {
					opt_lsref = 16; break;
				}
				if (a3 == 'p') {
					opt_lsref = 32; break;
				}
				st = 1;
				break;
			case 'x':
				hsp64 = 0; break;
			default:
				st = 1;break;
			}
		}
	}

	if (st) { printf("Illegal switch selected.\n");return 1; }

	//		help main
	if (hsphelp) {
		int res;
		HspHelpManager hman;
		syspath += "hsphelp";
		res = hman.initalize(syspath.c_str());
		if (res == 0) {
			res = hman.searchIndex(helpkey.c_str());
		}
		puts(hman.getMessage());
		return res;
	}

	hsc3 = new CHsc3;
	hsc3->SetCommonPath(compath.c_str());

	//		keyword main
	if (opt_lk) {
		if (*opt_lk == 0) opt_lk = NULL;
		st = hsc3->GetCmdList(2, opt_lk);
		puts(hsc3->GetError());
		delete hsc3;
		return st;
	}

	if (fname.empty()) { printf("No file name selected.\n");return 1; }

	if (oname.empty()) {
		oname = fname;
		hsp_path_cut_extension(oname);
		if (strmap) {
			addext(oname, "strmap");
		}
		else {
			addext(oname, "ax");
		}
	}
	fname2 = fname;
	hsp_path_cut_extension(fname2);
	addext( fname2,"i" );
	addext( fname,"hsp" );			// 拡張子がなければ追加する

	//		HSP64 check
	if (hsp64) {
		ppopt |= HSC3_OPT_RUNTIME64 | HSC3_OPT_UTF8OUT;
	}

	//		label pick
	if (opt_ls) {
		if (*opt_ls == 0) opt_ls = NULL;

		//		通常のコンパイル
		hsc3->InitAnalysisInfo(opt_lsmode | opt_lsref, opt_ls);
		st = hsc3->PreProcess(fname.c_str(), fname2.c_str(), ppopt, fname.c_str());
		if ((pponly == 0) && (st == 0)) {
			if (hsp64) {
				if (hsc3->GetHeaderOption() & HEDINFO_HSP64) {
					cmpopt |= HSC3_MODE_RUNTIME64 | HSC3_MODE_UTF8;
				}
			}
			st = hsc3->CompileLabelOut(fname2.c_str(), cmpopt);
		}
		if (st >= 0) {
			puts(hsc3->GetAnalysisInfo());
		}
		else {
			printf("No match.\n");
		}
		hsc3->DeleteAnalysisInfo();
		hsc3->PreProcessEnd();
		delete hsc3;
		return st;
	}

	//		call main

	if ( execobj ) {
		//		ランタイムを起動
		std::string execmd;
		std::string runtime_name;
		st = hsc3->GetRuntimeFromHeader( fname.c_str(), runtime_name );
		if ( st != 1 ) {
			oname = "hsp3.exe";			// デフォルトランタイム
		}
		else {
			oname = runtime_name;
		}

#if defined(HSPLINUX)||defined(HSPMAC)
		hsp_path_cut_extension(oname);
		if ( execobj & 8 ) {
			printf("Runtime[%s].\n",oname.c_str());
		} else {
			int result;
			printf("Execute from %s runtime[%s](%d).\n",fname.c_str(),oname.c_str(),execobj);
			execmd = syspath + "./" + oname + " " + fname;
			//sprintf(execmd,"%s./%s %s >%s.hspres",syspath,oname,fname,syspath);
			
			result = system(execmd.c_str());
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
			printf("Runtime[%s].\n",oname.c_str());
		} else {
			execmd = oname + " " + fname;
			st = hsp_path_exec_utf8( hsp_path::utf8_view(execmd.c_str()) );
			if ( st < 32 ) {
				printf("Runtime file missing.\n");
			}
		}
#endif

	} else {
		//		通常のコンパイル
		st = hsc3->PreProcess( fname.c_str(), fname2.c_str(), ppopt, fname.c_str() );
		if (( pponly == 0 )&&( st == 0 )) {
			if (hsp64) {
				if (hsc3->GetHeaderOption() & HEDINFO_HSP64) {
					cmpopt |= HSC3_MODE_RUNTIME64 | HSC3_MODE_UTF8;
				}
			}
			st = hsc3->Compile( fname2.c_str(), oname.c_str(), cmpopt );
		}
		puts( hsc3->GetError() );
		hsc3->PreProcessEnd();
	}

	if ( hsc3 != NULL ) { delete hsc3; hsc3=NULL; }
	return st;
}
