
//
//		HSP compiler class rev.3
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/hsp3struct.h"
#include "../hsp3/strnote.h"

#include "supio.h"
#include "hsc3.h"

#include "membuf.h"
#include "label.h"
#include "token.h"
#include "localinfo.h"

extern char *hsp_prestr[];
extern char *hsp_prepp[];

#define ERRBUF_SIZE 0x10000

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

char *CHsc3::GetError( void )
{
	return errbuf->GetBuffer();
}


int CHsc3::GetErrorSize( void )
{
	return errbuf->GetSize() + 1;
}


char* CHsc3::GetAnalysisInfo(void)
{
	if (anabuf == NULL) return NULL;
	return anabuf->GetBuffer();
}


int CHsc3::GetAnalysisInfoSize(void)
{
	if (anabuf == NULL) return 0;
	return anabuf->GetSize() + 1;
}


char* CHsc3::GetAnalysisLineInfo(int type)
{
	char* p="";
	if (anabuf == NULL) return p;
	switch (type) {
	case 0:
		p = analyse_module;
		break;
	case 1:
		if (analyse_caseflag) p = "*";
		break;
	default:
		break;
	}
	return p;
}


void CHsc3::InitAnalysisInfo(int mode, char *match, int line)
{
	DeleteAnalysisInfo();
	anabuf = new CMemBuf;
	analyse_mode = mode;
	analyse_line = line;
	*analyse_keyword = 0;
	*analyse_module = 0;
	analyse_match = NULL;
	if (match) {
		if (*match != 0) {
			strncpy(analyse_keyword, match, 255);
			analyse_match = analyse_keyword;
			strcase(analyse_match);
		}
	}
}


void CHsc3::DeleteAnalysisInfo(void)
{
	if (anabuf != NULL) {
		delete anabuf;
		anabuf = NULL;
	}
}


void CHsc3::ResetError( void )
{
	//		エラーメッセージ消去
	//
	if ( errbuf != NULL ) { delete errbuf; errbuf=NULL; }
	errbuf = new CMemBuf( ERRBUF_SIZE );
	hed_option = 0;
	hed_runtime[0] = 0;
}


//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CHsc3::CHsc3( void )
{
	errbuf = new CMemBuf( ERRBUF_SIZE );
	ahtbuf = NULL;
	anabuf = NULL;
	lb_info = NULL;
	addkw = NULL;
	common_path.clear();
	analyse_mode = 0;
	analyse_line = 0;
	analyse_caseflag = 0;
	analyse_match = NULL;
}


CHsc3::~CHsc3( void )
{
	DeleteAnalysisInfo();
	if (addkw != NULL) { delete addkw; addkw = NULL; }
	if (errbuf != NULL) { delete errbuf; errbuf = NULL; }
}



void CHsc3::AddSystemMacros( CToken *tk, int option )
{
	process_option = option;
	if (( option & HSC3_OPT_NOHSPDEF )==0 ) {
		CLocalInfo linfo;
		tk->RegistExtMacro( "__hspver__", vercode );
		tk->RegistExtMacro( "__hsp30__","" );
		tk->RegistExtMacro( "__date__",linfo.CurrentDate() );
		tk->RegistExtMacro( "__time__",linfo.CurrentTime() );
	    tk->RegistExtMacro( "__line__", 0 );
		tk->RegistExtMacro( "__file__", "" );
		tk->RegistExtMacro( "__runtime__", "\"hsp3\"" );
		if ( option & HSC3_OPT_UTF8IN ) tk->RegistExtMacro("_hsputf8", "");
		if ( option & HSC3_OPT_DEBUGMODE ) tk->RegistExtMacro( "_debug", "" );

#ifdef HSPWIN		// Windows(WIN32) version flag
		tk->RegistExtMacro("_hspwin", "");
#endif
#ifdef HSPMAC		// Macintosh version flag
		tk->RegistExtMacro("_hspmac", "");
#endif
#ifdef HSPLINUX		// Linux(CLI) version flag
		tk->RegistExtMacro("_hsplinux", "");
#endif
#ifdef HSPIOS		// iOS version flag
		tk->RegistExtMacro("_hspios", "");
#endif
#ifdef HSPNDK		// android NDK version flag
		tk->RegistExtMacro("_hspndk", "");
#endif
#ifdef HSPEMSCRIPTEN	// EMSCRIPTEN version flag
		tk->RegistExtMacro("_hspemscripten", "");
#else
		if (option & HSC3_OPT_EMSCRIPTEN) tk->RegistExtMacro("_hspemscripten", "");
#endif
	}
}


int CHsc3::PreProcessAht( const char *fname, void *ahtoption, int mode )
{
	//		Preprocess execute (AHT)
	//		(終了時にPreProcessEndを呼ぶこと)
	//
	int res;
	char mm[512];
	CToken tk;

	lb_info = NULL;
	ahtbuf = NULL;
	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path.c_str() );
	tk.SetAHT( (AHTMODEL *)ahtoption );
	outbuf = new CMemBuf;

	if ( mode ) {
		ahtbuf = new CMemBuf;
		tk.SetAHTBuffer( ahtbuf );
	}

	sprintf( mm,"#AHT processor ver%s / onion software 1997-2025(c)", hspver );
	tk.Mes( mm );
	res = tk.ExpandFile( outbuf, fname, fname );
	if ( res < 0 ) return -1;
	return 0;
}


/*
	rev 54
	mingw : warning : packbuf は未初期化で使用されうる
	問題なさそう、一応対処。
*/

int CHsc3::PreProcess( const char *fname, const char *outname, int option, const char *rname, void *ahtoption )
{
	//		Preprocess execute
	//		(終了時にPreProcessEndを呼ぶこと)
	//			option : bit0=ver2.55 mode(ON)
	//			         bit1=debug mode(ON)
	//			         bit2=make packfile(ON)
	//					 bit3=read AHT file(on)
	//					 bit4=write AHT file(on)
	//					 bit5=UTF8(input)(入力ソースがUTF8であることを示す)
	//					 bit7=64bit runtime(ON)(64bitランタイムを規定にする)
	//					 bit8=Emscripten mode(ON)(Emscripten向けであることを示す)
	//
	int res;
	char mm[512];
	CToken tk;
	CMemBuf *packbuf = NULL;

	lb_info = NULL;
	outbuf = new CMemBuf;
	ahtbuf = NULL;

	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path.c_str() );
	tk.LabelRegist2( hsp_prestr );
	AddSystemMacros( &tk, option );

	if ( option & HSC3_OPT_MAKEPACK ) {
		packbuf = new CMemBuf( 0x1000 );
		tk.SetPackfileOut( packbuf );
	}
	if ( option & (HSC3_OPT_READAHT|HSC3_OPT_MAKEAHT) ) {
		tk.SetAHT( (AHTMODEL *)ahtoption );
	}

	if ( option & HSC3_OPT_UTF8IN ) {
		tk.SetUTF8Input( 1 );
	}
	if ( option & HSC3_OPT_RUNTIME64 ) {
		tk.SetHeaderOption(HEDINFO_HSP64);
		tk.SetRuntime( "hsp3_64");
		tk.RegistExtMacro("__hsp64__", "");
		tk.RegistExtMacro("_hsputf8", "");
	}

	sprintf( mm,"#%s ver%s / onion software 1997-2026(c)", HSC3TITLE, hspver );
	tk.Mes( mm );

	if (anabuf) {
		tk.SetLabelListBuffer(anabuf, analyse_mode, analyse_match, analyse_line, rname);
	}
	tk.SetAdditionMode( 1 );
	res = tk.ExpandFile( outbuf, "hspdef.as", "hspdef.as" );
	tk.SetAdditionMode( 0 );
	if ( res<-1 ) return -1;
	res = tk.ExpandFile( outbuf, fname, rname );
	if ( res<0 ) return -1;
	tk.FinishPreprocess( outbuf );

	cmpopt = tk.GetCmpOption();
	if ( cmpopt & CMPMODE_PPOUT	 ) {
		res = outbuf->SaveFile( outname );
		if ( res<0 ) {
#ifdef JPNMSG
			tk.Mes( "#プリプロセッサファイルの出力に失敗しました" );
#else
			tk.Mes( "#Can't write output file." );
#endif
			return -2;
		}
	}
	outbuf->Put( (int)0 );

	if (anabuf) {
		strcpy( analyse_module, tk.GetLabelListLineModule() );
		analyse_caseflag = tk.GetLabelListLineCaseFlag();
	}
#if 0
	//		ソースのラベルを追加(停止中)
	if ( addkw != NULL ) { delete addkw; addkw=NULL; }
	addkw = new CMemBuf( 0x1000 );
	tk.LabelDump( addkw, DUMPMODE_DLLCMD );
#endif

	//sprintf( mm,"#Macro buffer %x.", tk.GetLabelBufferSize() );
	//tk.Mes( mm );

	if ( option & HSC3_OPT_MAKEPACK ) {
		tk.AddPackfile( "start.ax", 1 );
		res = packbuf->SaveFile( "packfile" );
		delete packbuf;
		if ( res<0 ) {
#ifdef JPNMSG
			tk.Mes( "#packfileの出力に失敗しました" );
#else
			tk.Mes( "#Can't write packfile." );
#endif
			return -3;
		}
		tk.Mes( "#packfile generated." );
	}

	hed_option = tk.GetHeaderOption();
	if ( cmpopt & CMPMODE_UTF8OUT ) hed_option |= HEDINFO_UTF8;

	strncpy(hed_runtime, tk.GetHeaderRuntimeName(), sizeof(hed_runtime) - 1);
	hed_runtime[sizeof(hed_runtime) - 1] = 0;
	lb_info = tk.GetLabelInfo();

	return 0;
}


void CHsc3::PreProcessEnd( void )
{
	if ( lb_info != NULL ) {
		delete lb_info;
		lb_info = NULL;
	}
	if ( outbuf != NULL ) {
		delete outbuf;
		outbuf = NULL;
	}
	if (ahtbuf != NULL) {
		delete ahtbuf;
		ahtbuf = NULL;
	}
}


int CHsc3::GetHeaderOption(void)
{
	return hed_option;
}


int CHsc3::Compile( const char *fname, const char *outname, int mode )
{
	//		Compile
	//

	int res;
	int genmode;
	char mm[512];
	CToken tk;

	genmode = mode;
	if (cmpopt & CMPMODE_UTF8OUT) {
		genmode |= HSC3_MODE_UTF8;
	}

	if ( lb_info != NULL ) tk.SetLabelInfo( lb_info );		// プリプロセッサのラベル情報

	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path.c_str() );
	tk.LabelRegist( hsp_prestr, 1 );
	tk.SetHeaderOption( hed_option, hed_runtime );
	tk.SetCmpOption( cmpopt );

	if ( process_option & HSC3_OPT_UTF8IN ) {
		tk.SetUTF8Input( 1 );
	}

	sprintf( mm,"#%s ver%s / onion software 1997-2026(c)", HSC3TITLE2, hspver );
	tk.Mes( mm );

	if (genmode & HSC3_MODE_LABOUT) {
		tk.delCmpMode(CMPMODE_OPTCODE);
		tk.SetLabelListBuffer( anabuf, analyse_mode, analyse_match );
		res = tk.GenerateCode(outbuf, outname, genmode| COMP_MODE_SKIPERROR);
		return res;
	}

	if ( outbuf != NULL ) {
		res = tk.GenerateCode( outbuf, outname, genmode );
	} else {
		res = tk.GenerateCode( fname, outname, genmode| COMP_MODE_STRMAP);
	}

	return res;
}


int CHsc3::CompileStrMap(const char* fname, const char* outname, int mode)
{
	return Compile(fname, outname, mode | HSC3_MODE_STRMAP);
}


int CHsc3::CompileLabelOut(const char* fname, int mode)
{
	return Compile(fname, "", mode | HSC3_MODE_LABOUT);
}


void CHsc3::SetCommonPath( const char *path )
{
	common_path = path != NULL ? path : "";
}


int CHsc3::GetCmdList( int option, char* match )
{
	int res;
	CToken tk;
	CMemBuf outbuf;

	tk.SetErrorBuf(errbuf);
	tk.SetCommonPath(common_path.c_str());
	tk.LabelRegist3(hsp_prestr);			// 標準キーワード
	tk.LabelRegist3(hsp_prepp);			// プリプロセッサキーワード
	AddSystemMacros(&tk, option);

	res = tk.ExpandFile(&outbuf, "hspdef.as", "hspdef.as");
	tk.LabelDump(errbuf, DUMPMODE_ALL, match);

	return 0;
}



int CHsc3::OpenPackfile( void )
{
	pfbuf = new CMemBuf( 0x1000 );
	if ( pfbuf->PutFile( "packfile" ) < 0 ) { delete pfbuf; return -1; }
	return 0;
}


int CHsc3::GetPackfileOption( char *out, int out_size, char *keyword, char *defval )
{
	if (out == NULL || out_size <= 0 || defval == NULL || keyword == NULL) return -1;
	std::string result;
	if (GetPackfileOption(result, keyword, defval) != 0) return -1;
	if ((int)result.size() >= out_size) return -1;
	strcpy(out, result.c_str());
	return 0;
}

int CHsc3::GetPackfileOption(std::string& out, const char* keyword, const char* defval)
{
	if (keyword == NULL || defval == NULL || pfbuf == NULL) return -1;
	out = defval;
	CStrNote note;
	note.Select(pfbuf->GetBuffer());
	for (int i = 0; i < note.GetMaxLine(); ++i) {
		char* line = note.GetLineDirect(i);
		std::string text = line != NULL ? line : "";
		note.ResumeLineDirect();
		if (text.size() < 2 || text[0] != ';' || text[1] != '!') continue;
		size_t separator = text.find('=', 2);
		if (separator != std::string::npos && text.compare(2, separator - 2, keyword) == 0) {
			out = text.substr(separator + 1);
		}
	}
	return 0;
}


int CHsc3::GetPackfileOptionInt( char *keyword, int defval )
{
	char tmp[512];
	char deftmp[32];
	sprintf( deftmp,"%d",defval );
	if (GetPackfileOption( tmp, sizeof(tmp), keyword, deftmp) != 0) return defval;
	if (( tmp[0]>='0' )&&( tmp[0]<='9' )) return atoi( tmp );
	return defval;
}


void CHsc3::ClosePackfile( void )
{
	delete pfbuf;
}


int CHsc3::GetRuntimeFromHeader( const char *fname, std::string& res )
{
	FILE *fp;
	HSPHED hsphed;
	const int hedsize = sizeof(hsphed);
	res.clear();

	fp=hsp_path_fopen(hsp_path::path_view(fname), "rb");
	if ( fp == NULL ) return -1;
	int64_t file_size = hsp_path_filesize(hsp_path::path_view(fname));
	if (file_size < hedsize || fread(&hsphed, 1, hedsize, fp) != (size_t)hedsize) {
		fclose(fp);
		return -1;
	}
	if (hsphed.h1 != 'H' || hsphed.h2 != 'S' || hsphed.h3 != 'P' || hsphed.h4 != '3' ||
		hsphed.pt_cs < hedsize || (int64_t)hsphed.pt_cs > file_size) {
		fclose(fp);
		return -1;
	}
	if ((hsphed.bootoption & HSPHED_BOOTOPT_RUNTIME) == 0) {
		fclose(fp);
		return 0;
	}
	if (hsphed.runtime < hedsize || hsphed.runtime >= hsphed.pt_cs ||
		fseek(fp, hsphed.runtime, SEEK_SET) != 0) {
		fclose(fp);
		return -1;
	}

	int64_t remaining = (int64_t)hsphed.pt_cs - hsphed.runtime;
	const size_t max_runtime_length = HSC3_RUNTIME_OUTPUT_SIZE - sizeof(".exe");
	size_t read_size = (size_t)std::min<int64_t>(remaining, max_runtime_length + 1);
	std::vector<char> runtime_data(read_size);
	if (runtime_data.empty() || fread(runtime_data.data(), 1, read_size, fp) != read_size) {
		fclose(fp);
		return -1;
	}
	fclose(fp);
	char* terminator = (char*)memchr(runtime_data.data(), 0, runtime_data.size());
	if (terminator == NULL) return -1;
	res.assign(runtime_data.data(), (size_t)(terminator - runtime_data.data()));
	hsp_path_cut_extension(res);
	res += ".exe";
	return 1;
}


int CHsc3::SaveOutbuf( const char *fname )
{
	int res;
	res = outbuf->SaveFile( fname );
	if ( res<0 ) {
		return -1;
	}
	return 0;
}


int CHsc3::SaveAHTOutbuf( const char *fname )
{
	int res;
	res = ahtbuf->SaveFile( fname );
	if ( res<0 ) {
		return -1;
	}
	return 0;
}


void CHsc3::Print(char* mes)
{
	errbuf->PutStr(mes);
	errbuf->PutStr("\r\n");
}
