
//
//		HSP compile/package functions for HSP3
//				onion software/onitama 2002-2017
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>
#include <string>
#include <vector>
#include <windows.h>
#include <direct.h>

#include "../../hsp3/hsp3config.h"

#include "../../hsp3/hsp3debug.h"			// hsp3 error code
#include "../../hsp3/hsp3struct.h"			// hsp3 core define
#include "../../hsp3/hspwnd.h"				// hsp3 windows define

#include "../supio.h"
#include "../hsc3.h"
#include "../token.h"
#include "../ahtobj.h"

#define USE_HSMANAGER
#ifdef USE_HSMANAGER
#include "../hsmanager.h"
static HspHelpManager hsman;
#endif

//#define DPM_SUPPORT		// DPMファイルマネージャをサポート
#ifdef DPM_SUPPORT
#include "dpm.h"
#endif

#define DPM2_SUPPORT		// DPM2ファイルマネージャをサポート
#include "../../hsp3/filepack.h"
#define PACKFILE "packfile"
#define DPMFILE "data"

#define ICONINS_SUPPORT	// ICONINSツールをサポート

//	VC++の場合
#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif

static std::string fname;
static std::string rname;
static std::string oname;
static std::string hspexe;
static int opt1,opt2,opt3;

static int orgcompath=0;
static std::string compath;

static CHsc3 *hsc3=NULL;
static CAht *aht=NULL;
static int homeid;				// Home object
static int ahtbuild_error;		// Error code
static char analysis_keyword[_MAX_PATH];
static char* analysis_name;
static int analysis_mode;

extern char *hsp_prestr[];

#ifdef DPM2_SUPPORT
static FilePack filepack;		// File Pack Manager
#endif

static void ensure_trailing_backslash(std::string& path)
{
	if (!path.empty() && path.back() != '\\') path.push_back('\\');
}

static int copy_ansi_path_to_utf8(std::string& destination, const char* source)
{
	destination.clear();
	if (source == NULL) return -1;
	return hsp_path_from_ansi(destination, hsp_path::ansi_view(source));
}

static int copy_legacy_aht_text(std::string& destination, const char* source)
{
	destination.clear();
	if (source == NULL) return -1;
	destination.assign(source);
	return 0;
}

static int copy_ansi_path_directory_to_utf8(std::string& destination, const char* source)
{
	std::string utf8_path;
	if (copy_ansi_path_to_utf8(utf8_path, source) != 0) return -1;
	try {
		destination = std::filesystem::u8path(utf8_path).parent_path().u8string();
		if (destination.empty() && !utf8_path.empty()) {
			destination = utf8_path;
		}
		else {
			ensure_trailing_backslash(destination);
		}
		return 0;
	}
	catch (const std::exception&) {
		return -1;
	}
}

#if defined( __GNUC__ ) && defined( __cplusplus )
extern "C"
#endif
BOOL WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		hsc3 = new CHsc3;
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		if ( hsc3 != NULL ) { delete hsc3; hsc3=NULL; }
		if ( aht != NULL ) { delete aht; aht=NULL; }
	}
	return TRUE ;
}

/*
	rev 54
	supio_win.cpp と supio_linux.cpp でも cutext が定義されている。
	そちらを使うようにしてここの cutext は削除。
*/


static void _sendstr( HWND hw, char *p1 )
{
	//		Win9x用
	LPARAM lprm;
	char *mes;
	char a1;
	int a;

	mes = p1;
	lprm=1;
	a=0;
	while(1) {
		a1=mes[a];if (a1==0) break;
		PostMessage( hw,WM_CHAR, a1, lprm );
		a++;
	}
}


static void _sendstr2( HWND hw, char *p1 )
{
	//		Win2000以降用
	LPARAM lprm;
	unsigned char *mes;
	unsigned char a1;
	int acode;
	int a;

	mes = (unsigned char *)p1;

	lprm=1;
	a=0;
	while(1) {
		a1=mes[a];if (a1==0) break;
		if ( a1 & 0x80 ) {
			acode = (int)mes[a+1];
			acode = (acode<<8) | a1;
			PostMessage( hw,WM_CHAR, acode, 0 );
			a+=2;
		} else {
			PostMessage( hw,WM_CHAR, a1, lprm );
			a++;
		}
	}
}


//----------------------------------------------------------

EXPORT BOOL WINAPI hsc_ini ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		hsc_ini "src-file"  (type6)
	//
	std::string path;
	if (copy_ansi_path_to_utf8(path, p1) != 0) {
		fname.clear();
		rname.clear();
		oname.clear();
		return -1;
	}
	fname = path;
	rname = path;
	oname = path;
	hsp_path_cut_extension(oname);
	oname += ".ax";
	analysis_name = NULL;
	analysis_mode = 0;
	return 0;
}


EXPORT BOOL WINAPI hsc_refname ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		hsc_refname "ref-file"  (type6)
	//
	if (copy_ansi_path_to_utf8(rname, p1) != 0) return -1;
	return 0;
}


EXPORT BOOL WINAPI hsc_objname(BMSCR* bm, char* p1, HSPPTRINT p2, HSPPTRINT p3)
{
	//
	//		hsc_objname "obj-file"  (type6)
	//
	if (copy_ansi_path_to_utf8(oname, p1) != 0) return -1;
	return 0;
}


EXPORT BOOL WINAPI hsc3_analysis(BMSCR* bm, char* p1, HSPPTRINT p2, HSPPTRINT p3)
{
	//
	//		hsc3_analysisname "name", mode, line  (type6)
	//
	if (*p1 == 0) {
		analysis_name = NULL;
	}
	else {
		strncpy(analysis_keyword, p1, _MAX_PATH - 1);
		analysis_name = analysis_keyword;
	}
	analysis_mode = (int)p2;
	hsc3->InitAnalysisInfo(analysis_mode, analysis_name, (int)p3);
	return 0;
}


EXPORT BOOL WINAPI hsc3_kwlineinfo(char* p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{
	//
	//		hsc3_kwlineinfo val, opt (type1)
	//
	strcpy(p1, hsc3->GetAnalysisLineInfo((int)p2));
	return 0;
}


EXPORT BOOL WINAPI hsc_ver (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, char *p4 )
{
	//
	//		hsc_ver (type$10)
	//
	sprintf( p4,"%s ver%s", HSC3TITLE, hspver );
	return 0;
}


EXPORT BOOL WINAPI hsc_bye (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc_bye (type$100)
	//
	return 0;
}


EXPORT BOOL WINAPI hsc_getmes ( char *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc_getmes val (type1)
	//
	strcpy( p1, hsc3->GetError() );
	return 0;
}


EXPORT BOOL WINAPI hsc_clrmes (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc_clrmes (type0)
	//
	hsc3->ResetError();
	return 0;
}


EXPORT BOOL WINAPI hsc_compath ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		hsc_compath "common-path"  (type6)
	//
	if (copy_ansi_path_to_utf8(compath, p1) != 0) return -1;
	orgcompath=1;
	return 0;
}


static int hsc_comp_sub(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{

}


EXPORT BOOL WINAPI hsc_comp (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc_comp mode,ppopt,dbgopt  (type0)
	//			( mode: 1=debug/0=no debug )
	//			(       2=preprocessor only )
	//			(       4=UTF8 output mode )
	//			(       8=strmap output mode )
	//			(      16=keyword list mode )
	//			(     128=64bit runtime mode )
	//			(     256=emscripten mode )
	//			( ppopt = preprocessor option )
	//			(       0=default/1=ver2.6 mode )
	//			(       32=UTF8 input mode )
	//			( dbgopt = debug window option )
	//			(       0=default/1=debug mode )
/*

p1が1(bit0)の場合は、デバッグ情報が付加されます。
p1が2(bit1)の場合はプリプロセス処理のみ行います。
p1が4(bit2)の場合は文字列データをUTF-8コードに変換して出力します。
p1が8(bit3)の場合は使用している文字列データファイル(strmap)を出力します
p1が16(bit4)の場合はキーワード解析リストを出力します
p1が128(bit7)の場合はデフォルトで64bitランタイムを選択します

*/
	int st;
	int ppopt;
	int cmpmode;
	std::string fname2;

	hsc3->ResetError();

	if (orgcompath==0) {
		if (hsp_path_get_module_directory(compath) != 0) return -1;
		ensure_trailing_backslash(compath);
		compath += "common\\";
	}
	fname2 = fname + ".i";
	hsc3->SetCommonPath( compath.c_str() );
	ppopt = 0;
	if (p1 & 1) ppopt |= HSC3_OPT_DEBUGMODE;
	if (p1 & 4) ppopt |= HSC3_OPT_UTF8OUT;
	if (p1 & 128) ppopt |= HSC3_OPT_UTF8OUT | HSC3_OPT_RUNTIME64;
	if (p1 & 256) ppopt |= HSC3_OPT_EMSCRIPTEN;

	if ( p2&1 ) ppopt|=HSC3_OPT_NOHSPDEF;
	if ( p2&4 ) ppopt|=HSC3_OPT_MAKEPACK;
	if ( p2&8 ) ppopt|=HSC3_OPT_READAHT;
	if ( p2&16 ) ppopt|=HSC3_OPT_MAKEAHT;
	if ( p2&32 ) ppopt|=HSC3_OPT_UTF8IN;

	st = hsc3->PreProcess( fname.c_str(), fname2.c_str(), ppopt, rname.c_str() );
	if ( st != 0 ) {
		hsc3->PreProcessEnd();
		return st;
	}
	if ( p1 & 2 ) {
		hsc3->PreProcessEnd();
		return 0;
	}

	cmpmode = p1 & HSC3_MODE_DEBUG;
	if (p1 & 4) cmpmode |= HSC3_MODE_UTF8;
	if (p1 & 8) cmpmode |= HSC3_MODE_STRMAP;
	if (p1 & 128) {
		if (hsc3->GetHeaderOption() & HEDINFO_HSP64) {
			cmpmode |= HSC3_MODE_RUNTIME64 | HSC3_MODE_UTF8;
		}
	}

	if (p1 & 16) {
		st = hsc3->CompileLabelOut(fname2.c_str(), cmpmode);
	}
	else {
		if (p3) cmpmode |= HSC3_MODE_DEBUGWIN;
		if (p1 & 8) {
			st = hsc3->CompileStrMap(fname2.c_str(), oname.c_str(), cmpmode);
		}
		else {
			st = hsc3->Compile(fname2.c_str(), oname.c_str(), cmpmode);
		}
	}

	hsc3->PreProcessEnd();
	return st;
}


//----------------------------------------------------------

EXPORT BOOL WINAPI pack_ini ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		pack_ini "src-file"  (type6)
	//
	if (copy_ansi_path_to_utf8(fname, p1) != 0) return -1;
	hsp_path_cut_extension(fname);
	if (hsc3 == NULL) Alert("#No way.");
	hsc3->ResetError();
	opt1 = 640; opt2 = 480; opt3 = 0;
	hspexe = "hsprt";

#ifdef DPM_SUPPORT
	dpmc_ini( hsc3->errbuf, fname.c_str() );
#endif
#ifdef DPM2_SUPPORT
	filepack.Reset();
	filepack.SetErrorBuffer(hsc3->errbuf);
#endif
	return 0;
}


EXPORT BOOL WINAPI pack_view (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		pack_view encode  (type0)
	//
	int st;
	st = 0;
#ifdef DPM_SUPPORT
	st = dpmc_view();
#endif
#ifdef DPM2_SUPPORT
	std::string dpmname = fname + ".dpm";
	char tmp[1024];

	if (p1 == 0) p1 = -1;
		int res = filepack.LoadPackFile(dpmname.c_str(), (int)p1);
	if (res<0) {
		sprintf(tmp,"#Error %d in loading [%s].",res, dpmname.c_str());
		filepack.Print(tmp);
		st = 1;
	}
	else {
		sprintf(tmp, "#[%s] Loaded.", dpmname.c_str());
		filepack.Print(tmp);
		filepack.PrintFiles();
	}
#endif
	return -st;
}


EXPORT BOOL WINAPI pack_make (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		pack_make mode, key(type0)
	//		     mode : (1=ForDPM/0=ForExecutable)
	//		     key  : (0=Default/other=New Seed)
	//
	int st;
	st = 0;
#ifdef DPM_SUPPORT
	if ( p2 != 0 ) dpmc_dpmkey( p2 );
	st=dpmc_pack(p1);
#endif

#ifdef DPM2_SUPPORT
	if (p2 == 0) p2 = -1;
#ifdef HSPWIN
	p1 = (int)GetTickCount();	// Windowsの場合はtickをシード値とする
#else
	p1 = (int)time(0);			// Windows以外のランダムシード値
#endif
	if (filepack.SavePackFile(fname.c_str(), PACKFILE, (int)p1, (int)p2) < 0) {
		st = 1;
	}
#endif
	return -st;
}


EXPORT BOOL WINAPI pack_opt (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		pack_opt sx,sy,disp_sw (type0)
	//
	opt1=(int)p1;if (opt1==0) opt1=640;
	opt2= (int)p2;if (opt2==0) opt2=480;
	opt3= (int)p3;							// disp SW (1=blank window)
	return 0;
}


EXPORT BOOL WINAPI pack_rt ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		pack_rt "runtime-file"  (type6)
	//
	if (copy_ansi_path_to_utf8(hspexe, p1) != 0) return -1;
	return 0;
}


EXPORT BOOL WINAPI pack_exe (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		pack_exe mode (type0)
	//
	int st;
	st = 0;
#ifdef DPM_SUPPORT
	st=dpmc_mkexe((int)p1,hspexe.c_str(),opt1,opt2,opt3);
#endif
	return -st;
}


EXPORT BOOL WINAPI pack_get ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		pack_get "get-file", enc  (type6)
	//
	int st;
	std::string path;
	st = 0;
	if (copy_ansi_path_to_utf8(path, p1) != 0) return -1;
#ifdef DPM_SUPPORT
	st=dpmc_get((int)p1);
#endif
#ifdef DPM2_SUPPORT
	if (filepack.ExtractFile(path.c_str(),NULL, (int)p2) < 0) {
		st = 1;
	}
#endif
	return -st;
}


//----------------------------------------------------------
//		Additional service on 2.6
//----------------------------------------------------------

EXPORT BOOL WINAPI hsc3_getsym(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{
	//
	//		hsc3_getsym val  (type1)
	//
	hsc3->ResetError();
	if (orgcompath == 0) {
		if (hsp_path_get_module_directory(compath) != 0) return -1;
		ensure_trailing_backslash(compath);
		compath += "common\\";
	}
	hsc3->SetCommonPath(compath.c_str());
	if (hsc3->GetCmdList(((int)p1 | 2))) return -1;
	return 0;
}


EXPORT BOOL WINAPI hsc3_kwlbuf(char* p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{
	//
	//		hsc3_kwlbuf bufvar, maxsize  (type1)
	//
	char* p = hsc3->GetAnalysisInfo();
	if (p == NULL) {
		return -1;
	}
	if (p2) {
		if (hsc3->GetAnalysisInfoSize() > (int)p2) {
			return -1;
		}
	}
	strcpy(p1, p );
	return 0;
}


EXPORT BOOL WINAPI hsc3_kwlsize(int* p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{
	//
	//		hsc3_kwlsize var  (type1)
	//
	*p1 = hsc3->GetAnalysisInfoSize();
	if (*p1 == 0) return -1;
	return 0;
}


EXPORT BOOL WINAPI hsc3_kwlclose(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4)
{
	//
	//		hsc3_kwlclose var  (type0)
	//
	hsc3->DeleteAnalysisInfo();
	return 0;
}


EXPORT BOOL WINAPI hsc3_messize ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc3_messize val  (type1)
	//
	*p1 = hsc3->GetErrorSize();
	return 0;
}


EXPORT BOOL WINAPI hsc3_make ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		hsc3_make "myname",sw,0  (type6)
	//		(sw=1の場合はiconinsを呼び出す)
	//
	std::string libpath;
	int i,type;
	int opt3a,opt3b;
	int st;
#ifdef ICONINS_SUPPORT
	std::string ici_opt;
	std::string ici_current;
	std::string ici_target;
	std::string ici_icon;
	std::string ici_version;
	std::string ici_manifest;
	std::string ici_lang;
	std::string ici_upx;
	int ici_use_icon = 0;
	int ici_use_version = 0;
	int ici_use_manifest = 0;
	int ici_use_lang = 0;
	int ici_use_upx = 0;
#endif

	if ( hsc3==NULL ) Alert( "#No way." );
	hsc3->ResetError();

	if (copy_ansi_path_directory_to_utf8(libpath, p1) != 0) return -1;

#ifdef ICONINS_SUPPORT
	if (hsp_path_get_module_directory(ici_opt) != 0) return -1;
	ensure_trailing_backslash(ici_opt);
	ici_opt += "iconins.exe";
	if (hsp_path_get_current_directory(ici_current) != 0) return -1;
	ici_current += "\\";
#endif

	i = hsc3->OpenPackfile();
	if (i) { Alert( "packfileが見つかりません" ); return -1; }
	if (hsc3->GetPackfileOption( hspexe, "runtime", "hsprt" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	libpath += hspexe;
	hspexe = libpath;
	if (hsc3->GetPackfileOption( fname, "name", "hsptmp" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	hsp_path_cut_extension(fname);
	type = hsc3->GetPackfileOptionInt( "type", 0 );
	opt1 = hsc3->GetPackfileOptionInt( "xsize", 640 );
	opt2 = hsc3->GetPackfileOptionInt( "ysize", 480 );
	opt3a = hsc3->GetPackfileOptionInt( "hide", 0 );
	opt3b = hsc3->GetPackfileOptionInt( "orgpath", 0 );
	opt3 = 0;
	if ( opt3a ) opt3 |= 1;
	if ( opt3b ) opt3 |= 2;

#ifdef ICONINS_SUPPORT
	if (hsc3->GetPackfileOption( ici_icon, "icon", "" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	if ( !ici_icon.empty() ) { ici_use_icon = 1; }
	if (hsc3->GetPackfileOption( ici_version, "version", "" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	if ( !ici_version.empty() ) { ici_use_version = 1; }
	if (hsc3->GetPackfileOption( ici_manifest, "manifest", "" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	if ( !ici_manifest.empty() ) { ici_use_manifest = 1; }
	if (hsc3->GetPackfileOption( ici_lang, "lang", "" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	if ( !ici_lang.empty() ) { ici_use_lang = 1; }
	if (hsc3->GetPackfileOption( ici_upx, "upx", "" ) != 0) {
		hsc3->ClosePackfile();
		return -1;
	}
	if ( !ici_upx.empty() ) { ici_use_upx = 1; }

	ici_target = ici_current;
	ici_target += fname;
	if (type==2) {
		ici_target += ".scr";
	}
	else {
		ici_target += ".exe";
	}
#endif

	hsc3->ClosePackfile();

	//		exeを作成
	st = 0;
#ifdef DPM_SUPPORT
	dpmc_ini( hsc3->errbuf, fname.c_str() );
	st=dpmc_pack( 0 );
	if ( st ) return -st;
	st=dpmc_mkexe( type, hspexe.c_str(), opt1, opt2, opt3 );
	fname += ".dpm";
	hsp_path_remove( hsp_path::path_view(fname.c_str()) );
#endif
#ifdef DPM2_SUPPORT
	int myseed1,myseed2;
#ifdef HSPWIN
	myseed1 = (int)GetTickCount();	// Windowsの場合はtickをシード値とする
#else
	myseed1 = (int)time(0);			// Windows以外のランダムシード値
#endif
	myseed2 = (int)hsp_path_filesize(hsp_path::path_view(PACKFILE));

	filepack.Reset();
	filepack.SetErrorBuffer(hsc3->errbuf);
	st = filepack.SavePackFile(fname.c_str(), PACKFILE, myseed1, myseed2);
	if (st < 0) {
		return -1;
	}
	st = filepack.MakeEXEFile(type, hspexe.c_str(), fname.c_str(), myseed2, opt1, opt2, opt3);
	fname += ".dpm";
	hsp_path_remove(hsp_path::path_view(fname.c_str()));
#endif

	//		iconins process
#ifdef ICONINS_SUPPORT
	if ((ici_use_icon+ici_use_version+ici_use_manifest+ici_use_lang+ici_use_upx)>0) {

		ici_opt += " -e\"";
		ici_opt += ici_target;
		ici_opt += "\"";

		if ( ici_use_icon ) {
			ici_opt += " -i\"";
			ici_opt += ici_current;
			ici_opt += ici_icon;
			ici_opt += "\"";
		}
		if ( ici_use_version ) {
			ici_opt += " -v\"";
			ici_opt += ici_current;
			ici_opt += ici_version;
			ici_opt += "\"";
		}
		if ( ici_use_manifest ) {
			ici_opt += " -m\"";
			ici_opt += ici_current;
			ici_opt += ici_manifest;
			ici_opt += "\"";
		}
		if ( ici_use_lang ) {
			ici_opt += " -l\"";
			ici_opt += ici_lang;
			ici_opt += "\"";
		}
		if ( ici_use_upx ) {
			ici_opt += " -u\"";
			ici_opt += ici_upx;
			ici_opt += "\"";
		}
		if ( p2 ) {
			i = 1;
			while (1) {
				Sleep(100);
				CMemBuf dummy;
				int res = dummy.PutFile(ici_target.c_str());
				if (res > 0) break;
				i++;
				if (i >= 50) break;
			}
			i = hsp_path_exec_utf8( hsp_path::utf8_view(ici_opt.c_str()) );
			if ( i < 32 ) return -1;
		}
	}
#endif

	return -st;
}


//----------------------------------------------------------
//		Additional service on 3.0
//----------------------------------------------------------

EXPORT BOOL WINAPI hsc3_getruntime ( char *p1, char *p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc3_getruntime val  (type5)
	//
	int i;
	std::string path;
	std::string runtime;
	std::string ansi_runtime;
	if (copy_ansi_path_to_utf8(path, p2) != 0) return -1;
	i = hsc3->GetRuntimeFromHeader( path.c_str(), runtime );
	if ( i != 1 ) {
		*p1 = 0;
		return 0;
	}
	if (hsp_path_to_ansi(ansi_runtime, hsp_path::utf8_view(runtime.c_str())) != 0) return -1;
	strcpy2(p1, ansi_runtime.c_str(), HSC3_RUNTIME_OUTPUT_SIZE);
	return 0;
}


EXPORT BOOL WINAPI hsc3_run ( char *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		hsc3_run path, debug_flag  (type1)
	//
	std::string utf8_command;
	if (hsp_path_from_ansi(utf8_command, hsp_path::ansi_view(p1)) != 0) return -1;
	int i = hsp_path_exec_utf8(hsp_path::utf8_view(utf8_command.c_str()));
	if ( i < 32 ) return -1;
	return 0;
}

//----------------------------------------------------------
//		Additional service on 3.1
//----------------------------------------------------------

EXPORT BOOL WINAPI aht_source( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_source var, "aht_file", "path", id (type$202)
	//		(id<0の場合は自動確保、そうでなければ指定IDに確保)
	//
	PVal *pv;
	APTR ap;
	char *p;
	int ep1;
	int res;

	int st;
	std::string fn;
	std::string fpath;
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	p = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	if (copy_ansi_path_to_utf8(fn, p) != 0) return -1;
	p = hei->HspFunc_prm_gets();			// パラメータ3:文字列
	if (copy_ansi_path_to_utf8(fpath, p) != 0) return -1;
	ep1 = hei->HspFunc_prm_getdi( -1 );		// パラメータ4:数値

	if ( ep1 < 0 ) {
		ahtmodel =aht->AddModel();
	} else {
		ahtmodel =aht->EntryModel( ep1 );
	}

	ahtmodel->SetName( fn.c_str() );
	ahtmodel->SetSource( fn.c_str() );
	ahtmodel->SetSourcePath( fpath.c_str() );
	res = ahtmodel->GetId();
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入

	//		AHTを解析
	hsc3->ResetError();
	if (orgcompath==0) {
		if (hsp_path_get_module_directory(compath) != 0) return -1;
		ensure_trailing_backslash(compath);
		compath += fpath;
	}
	hsc3->SetCommonPath( compath.c_str() );
	st = hsc3->PreProcessAht( fn.c_str(), ahtmodel, 0 );

	aht->Mesf( "%s", hsc3->GetError() );

	if ( st != 0 ) {
		hsc3->PreProcessEnd();
		return st;
	}

	ahtmodel->TerminateExp();
	hsc3->PreProcessEnd();

	aht->BuildGlobalID();				// グローバルIDを更新する

	if ( st != 0 ) return st;

	return 0;
}


EXPORT BOOL WINAPI aht_ini ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_ini "prj_file" (type6)
	//
	std::string path;
	if (copy_ansi_path_to_utf8(path, p1) != 0) return -1;
	if ( aht != NULL ) { delete aht; aht=NULL; }
	aht = new CAht;
	aht->SetPrjFile( path.c_str() );
	return 0;
}


EXPORT BOOL WINAPI aht_stdbuf ( char *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_stdbuf debug_buf  (type1)
	//
	if ( aht == NULL ) return -1;
	strcpy( p1, aht->GetStdBuffer() );
	return 0;
}


EXPORT BOOL WINAPI aht_stdsize ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_stdsize var  (type1)
	//
	if ( aht == NULL ) return -1;
	*p1 = (int)strlen( aht->GetStdBuffer() ) + 1;
	return 0;
}


EXPORT BOOL WINAPI aht_getopt( char *p1, char *p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getopt var, "parameter", modelID, maxstr (type5)
	//
	int max;
	char *p;
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;

	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;
	p = ahtmodel->GetAHTOption( p2 );
	max = (int)p4;
	if ( max <= 0 ) max = 64;
	strcpy2( p1, p, max );

	return 0;
}


EXPORT BOOL WINAPI aht_getpropcnt ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getpropcnt var,modelID,sw  (type1)
	//		( sw=0:個数/1:編集行数 )
	//
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;
	ahtmodel =aht->GetModel((int)p2 );
	if ( ahtmodel == NULL ) return -1;

	switch( p3 ) {
	case 1:
		*p1 = ahtmodel->GetPropertyPossibleLines();
		break;
	default:
		*p1 = ahtmodel->GetPropCount();
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI aht_getpropid ( int *p1, char *p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getpropid var,"name",modelID  (type5)
	//
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;
	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;
	*p1 = ahtmodel->GetPropertyID( p2 );
	return 0;
}


EXPORT BOOL WINAPI aht_getprop( char *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getprop var, DataID, propID, modelID (type1)
	//
	int max;
	AHTMODEL *ahtmodel;
	AHTPROP *prop;
	char **p;
	char *t;
	if ( aht == NULL ) return -1;

	ahtmodel =aht->GetModel((int)p4 );
	if ( ahtmodel == NULL ) return -1;
	prop = ahtmodel->GetProperty((int)p3 );
	p = (char **)(&prop->name);

	max = 512;
	t = p[p2];
	if ( p2 == 2 ) t = prop->GetValue();

	strcpy2( p1, t, max );

	return (-(prop->ahttype));
}


EXPORT BOOL WINAPI aht_getproptype ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getproptype var, propID, modelID  (type1)
	//
	AHTPROP *prop;
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;
	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;
	prop = ahtmodel->GetProperty((int)p2 );
	*p1 = prop->ahttype;
	return 0;
}


EXPORT BOOL WINAPI aht_getpropmode ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getpropmode var, propID, modelID  (type1)
	//
	AHTPROP *prop;
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;
	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;
	prop = ahtmodel->GetProperty((int)p2 );
	*p1 = prop->ahtmode;
	return 0;
}


EXPORT BOOL WINAPI aht_make ( int *p1, char *p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_make var, "outfile", modelID, mode (type5)
	//					( mode:bit0="hsptmp"out/bit1=ahtout/bit2=HSP source build)
	//
	int st;
	int res;
	AHTMODEL *ahtmodel;
	std::string fname2;
	std::string output_path;

	if ( aht == NULL ) return -1;
	if (copy_ansi_path_to_utf8(output_path, p2) != 0) return -1;
	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;

	fname2 = ahtmodel->GetSource();
	res = 0;
	hsc3->ResetError();
	if (orgcompath==0) {
		if (hsp_path_get_module_directory(compath) != 0) return -1;
		ensure_trailing_backslash(compath);
		compath += ahtmodel->GetSourcePath();
	}
	hsc3->SetCommonPath( compath.c_str() );
	st = hsc3->PreProcessAht( fname2.c_str(), ahtmodel, 1 );
	aht->Mesf( "%s", hsc3->GetError() );
	if ( st != 0 ) {
		hsc3->PreProcessEnd();
		*p1 = 1;
		return st;
	}

	if ( p4&1 ) {
		res |= hsc3->SaveOutbuf( "hsptmp" );
	}
	if ( p4&2 ) {
		res |= hsc3->SaveAHTOutbuf( output_path.c_str() );
	}
	if ( p4&4 ) {
		//			AHTマネージャー用にソースを構築する
		//
		aht->AddMakeBufferInit( hsc3->ahtbuf->GetBuffer(), hsc3->ahtbuf->GetSize() );
		aht->AddMakeBufferMain( hsc3->outbuf->GetBuffer(), hsc3->outbuf->GetSize() );
	}

	hsc3->PreProcessEnd();
	*p1 = res;
	if ( st != 0 ) return st;
	return 0;
}


EXPORT BOOL WINAPI aht_makeinit (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_makeinit (type0)
	//
	aht->InitMakeBuffer();
	return 0;
}


EXPORT BOOL WINAPI aht_makeend ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_makeend "fname" (type6)
	//
	int res;
	std::string path;
	if (copy_ansi_path_to_utf8(path, p1) != 0) return -1;
	res = aht->SaveMakeBuffer( path.c_str() );
	aht->DisposeMakeBuffer();
	if ( res ) return -1;
	return 0;
}


EXPORT BOOL WINAPI aht_makeput ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_makeput "message",sw (type6)
	//
	if ( p2 ) {
		aht->AddMakeBufferInit( p1 );
	} else {
		aht->AddMakeBufferMain( p1 );
	}
	return 0;
}


EXPORT BOOL WINAPI aht_setprop ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_setprop "defval", propID, modelID (type6)
	//
	AHTMODEL *ahtmodel;
	AHTPROP *prop;
	if ( aht == NULL ) return -1;

	ahtmodel =aht->GetModel((int)p3 );
	if ( ahtmodel == NULL ) return -1;
	prop = ahtmodel->GetProperty((int)p2 );
	if ( prop == NULL ) return -1;
	prop->SetNewVal( p1 );
	return 0;
}


EXPORT BOOL WINAPI aht_sendstr ( char *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//		send key event (type1)
	//			aht_sendstr sendbuf, hwnd
	//
	LPARAM lprm;
	DWORD version;
	version = GetVersion();
	if ( LOBYTE(LOWORD(version)) > 4 ) {
		_sendstr2( (HWND)p2, p1 );
		return 0;
	}
	_sendstr( (HWND)p2, p1 );

	lprm=0xd0000001;

	PostMessage( (HWND)p2, WM_KEYDOWN, VK_RETURN, lprm );
	PostMessage( (HWND)p2, WM_CHAR, VK_RETURN, lprm );
	PostMessage( (HWND)p2, WM_KEYUP, VK_RETURN, lprm );

	return 0;
}


EXPORT BOOL WINAPI aht_getmodcnt ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getmodcnt var  (type1)
	//
	if ( aht == NULL ) return -1;
	*p1 = aht->GetModelCount();
	return 0;
}


EXPORT BOOL WINAPI aht_getmodaxis ( int *p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_getmodaxis var,modelID  (type1)
	//
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;

	ahtmodel =aht->GetModel((int)p2 );
	if ( ahtmodel == NULL ) return -1;

	p1[0] = ahtmodel->GetCurX();
	p1[1] = ahtmodel->GetCurY();
	p1[2] = ahtmodel->GetIconId();
	p1[3] = ahtmodel->GetPage();
	p1[4] = ahtmodel->GetNextID();
	p1[5] = ahtmodel->GetPrevID();
	return 0;
}


EXPORT BOOL WINAPI aht_setmodaxis (HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_setmodaxis modelID, x, y, page  (type0)
	//
	AHTMODEL *ahtmodel;
	if ( aht == NULL ) return -1;

	ahtmodel =aht->GetModel((int)p1 );
	if ( ahtmodel == NULL ) return -1;

	ahtmodel->SetCur((int)p2, (int)p3 );
	ahtmodel->SetPage((int)p4 );
	return 0;
}


EXPORT BOOL WINAPI aht_prjload ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_prjload "prj_file" (type6)
	//
	int res;
	std::string path;
	if ( aht == NULL ) return -1;
	if (copy_ansi_path_to_utf8(path, p1) != 0) return -1;
	res = aht->LoadProject( path.c_str() );
	if ( res ) return res;
	return 0;
}


EXPORT BOOL WINAPI aht_prjsave ( BMSCR *bm, char *p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_prjsave "prj_file" (type6)
	//
	int res;
	std::string path;
	if ( aht == NULL ) return -1;
	if (copy_ansi_path_to_utf8(path, p1) != 0) return -1;
	res = aht->SaveProject( path.c_str() );
	if ( res ) return -1;
	return 0;
}


EXPORT BOOL WINAPI aht_getprjmax( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_getprjmax var (type$202)
	//		varにmodel数を返す
	//
	PVal *pv;
	APTR ap;
	int res;

	if ( aht == NULL ) return -1;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数

	res = aht->GetProjectFileModelMax();
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI aht_getprjsrc( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_getprjsrc var, var2, var3, id (type$202)
	//		varにfname、var2にfpath、var3にObjectIDを返す。
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int ep1;
	char *p;
	std::string ansi_name;
	std::string ansi_path;
	int res;

	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );	// パラメータ2:変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );	// パラメータ3:変数
	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ4:数値

	p = aht->GetProjectFileModel( ep1 );
	if (copy_legacy_aht_text(ansi_name, p) != 0) return -1;
	p = aht->GetProjectFileModelPath( ep1 );
	if (copy_legacy_aht_text(ansi_path, p) != 0) return -1;
	res = aht->GetProjectFileModelID( ep1 );
	hei->HspFunc_prm_setva( pv, ap, TYPE_STRING, ansi_name.c_str() );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_STRING, ansi_path.c_str() );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_INUM, &res );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI aht_prjload2( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_prjload2 model_id, id (type$202)
	//		( モデルデータの更新。aht_prjloadの後にモデルごとに実行する。 )
	//
	int ep1,ep2;
	int res;

	if ( aht == NULL ) return -1;

	res = 0;
	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ1:数値
	ep2 = hei->HspFunc_prm_getdi( 0 );		// パラメータ2:数値

	res = aht->LoadProjectApply( ep1, ep2 );
	return res;
}


EXPORT BOOL WINAPI aht_prjloade( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_prjloade (type$202)
	//
	if ( aht == NULL ) return -1;

	aht->LoadProjectEnd();
	return 0;
}


EXPORT BOOL WINAPI aht_delmod(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_delmod modelID  (type0)
	//
	if ( aht == NULL ) return -1;
	aht->DeleteModel((int)p1 );
	return 0;
}


EXPORT BOOL WINAPI aht_linkmod(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_linkmod modelID, NextID  (type0)
	//
	if ( aht == NULL ) return -1;
	aht->LinkModel((int)p1, (int)p2 );
	return 0;
}


EXPORT BOOL WINAPI aht_unlinkmod(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_unlinkmod modelID  (type0)
	//
	if ( aht == NULL ) return -1;
	aht->UnlinkModel((int)p1 );
	return 0;
}


EXPORT BOOL WINAPI aht_setpage(HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3, HSPPTRINT p4 )
{
	//
	//		aht_setpage cur,max  (type0)
	//
	if ( aht == NULL ) return -1;
	aht->SetPage((int)p1, (int)p2 );
	return 0;
}


EXPORT BOOL WINAPI aht_getpage( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_getpage var,var2 (type$202)
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int res;

	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );	// パラメータ2:変数

	res = aht->GetCurrentPage();
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入
	res = aht->GetMaxPage();
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_INUM, &res );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI aht_propupdate( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_propupdate model_id (type$202)
	//		( プロパティの更新 )
	//
	int ep1;
	if ( aht == NULL ) return -1;
	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ1:数値
	aht->UpdateModelProperty( ep1 );
	return 0;
}


EXPORT BOOL WINAPI aht_parts( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_parts "path","list" (type$202)
	//
	char *ep1;
	char *ep2;
	std::string utf8_list;
	std::string path;
	if ( aht == NULL ) return -1;
	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	if (copy_ansi_path_to_utf8(path, ep1) != 0) return -1;
	ep2 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	if (hsp_path_from_ansi(utf8_list, hsp_path::ansi_view(ep2)) != 0) return -1;
	return aht->BuildParts( utf8_list.data(), path.c_str() ) < 0 ? -1 : 0;
}


EXPORT BOOL WINAPI aht_getparts( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_getparts id, var,var2,var3 (type$202)
	//					 ( ICONID,name,classnameが代入される )
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	PVal *pv3;
	APTR ap3;
	int ep1;
	int res;
	char *p;
	std::string ansi_name;
	std::string ansi_classname;

	if ( aht == NULL ) return -1;

	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ1:数値
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ2:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );	// パラメータ3:変数
	ap3 = hei->HspFunc_prm_getva( &pv3 );	// パラメータ4:変数

	if (aht->GetParts(ep1) == NULL) return -1;

	res = aht->GetPartsIconID(ep1);
	p = aht->GetPartsName(ep1);
	if (copy_legacy_aht_text(ansi_name, p) != 0) return -1;
	p = aht->GetPartsClassName(ep1);
	if (copy_legacy_aht_text(ansi_classname, p) != 0) return -1;
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, TYPE_STRING, ansi_name.c_str() );	// 変数に値を代入
	hei->HspFunc_prm_setva( pv3, ap3, TYPE_STRING, ansi_classname.c_str() );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI aht_listparts( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_listparts var,"clsname" (type$202)
	//
	PVal *pv;
	APTR ap;
	char *ep1;
	char *p;

	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	p = aht->SearchModelByClassName( ep1 );
	hei->HspFunc_prm_setva( pv, ap, TYPE_STRING, p );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI aht_findstart( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_findstart var (type$202)
	//
	if ( aht == NULL ) return -1;
	aht->FindModelStart();
	homeid = -1;
	ahtbuild_error = 0;
	return 0;
}


EXPORT BOOL WINAPI aht_findparts( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_findparts var (type$202)
	//
	PVal *pv;
	APTR ap;
	int res;
	AHTMODEL *m;
	char *p;
	int len;
	int i;
	int statval;

	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	res = aht->FindModel();
	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入

	if ( res >= 0 ) {
		m = aht->GetModel( res );
		p = m->GetClass();
		len = (int)strlen( p ) - 5; if ( len < 0 ) len = 0;
		if ( strcmp( p+len, ".home" )==0 ) {		// homeクラスかどうか確認する
			if ( homeid != -1 ) ahtbuild_error = 2;
			homeid = res;
		}
	}

	statval = 0;
	i = aht->FindModelGetParentId();				// リンク元IDを取得する
	if ( i >= 0 ) {
		m = aht->GetModel( i );
		p = m->GetClass();
		len = (int)strlen( p ) - 8; if ( len < 0 ) len = 0;
		if ( strcmp( p+len, ".routine" )==0 ) {		// routineクラスかどうか確認する
			statval = -1;
		}
	}

	return statval;
}


EXPORT BOOL WINAPI aht_findend( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_findend var, mode (type$202)
	//		( mode=0:ERROR表示あり/1:なし )
	//
	PVal *pv;
	APTR ap;
	int ep1;
	char *err;

	if ( aht == NULL ) return -1;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ2:数値

	if ( homeid == -1 ) ahtbuild_error = 1;

	if ( ep1 == 0 ) {
		err = NULL;
		switch( ahtbuild_error ) {
		case 1:
			err = "ホームのパーツが配置されていません。\n最初に実行するホームを置いてください。";
			break;
		case 2:
			err = "ホームのパーツが複数配置されています。\nホームは１つだけ置いてください。";
			break;
		default:
			break;
		}
		if ( err != NULL ) Alert( err );
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &ahtbuild_error );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI aht_getexid( HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3 )
{
	//
	//		aht_getexid var, mode (type$202)
	//			( varにモデルIDを代入する )
	//			( mode:0=リンク元ID/1=HOMEのID )
	//
	PVal *pv;
	APTR ap;
	int ep1;
	int res;

	if ( aht == NULL ) return -1;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_getdi( 0 );		// パラメータ2:数値

	res = -1;
	switch( ep1 ) {
	case 0:
		res = aht->FindModelGetParentId();
		break;
	case 1:
		res = homeid;
		break;
	default:
		break;
	}

	hei->HspFunc_prm_setva( pv, ap, TYPE_INUM, &res );	// 変数に値を代入

	return 0;
}


//----------------------------------------------------------

#ifdef USE_HSMANAGER

EXPORT BOOL WINAPI hman_init(HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3)
{
	//
	//		hman_init "pathname", mode (type$202)
	//			( mode:未使用 )
	//
	char *ep1;
	int ep2;
	int res;
	std::string path;

	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	if (copy_ansi_path_to_utf8(path, ep1) != 0) return -1;
	ep2 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値

	res = hsman.initalize( path.c_str() );
	if (res < 0) return -1;

	return 0;
}

EXPORT BOOL WINAPI hman_search(HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3)
{
	//
	//		hman_search "keyword" (type$202)
	//			( 文字列を指定してヘルプを検索する )
	//
	char *ep1;
	int res;
	std::string key;

	ep1 = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	if (copy_ansi_path_to_utf8(key, ep1) != 0) return -1;

	res = hsman.searchIndex(key.c_str());
	if (res < 0) return -1;

	return 0;
}

EXPORT BOOL WINAPI hman_getresult(HSPEXINFO *hei, HSPPTRINT p1, HSPPTRINT p2, HSPPTRINT p3)
{
	//
	//		hman_getresult var,option (type$202)
	//			( varに結果文字列を代入する )
	//
	PVal *pv;
	APTR ap;
	char *res;
	std::string ansi_message;
	int ep1;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値
	res = hsman.getMessage();
	if (res == NULL || hsp_path_to_ansi(ansi_message, hsp_path::utf8_view(res)) != 0) return -1;
	hei->HspFunc_prm_setva(pv, ap, TYPE_STRING, ansi_message.c_str());	// 変数に値を代入

	return 0;
}

#endif

//----------------------------------------------------------
