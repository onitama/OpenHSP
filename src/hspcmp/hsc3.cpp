
//
//		HSP compiler class rev.3
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"
#include "../hsp3/hsp3debug.h"
#include "../hsp3/hsp3struct.h"

#include "supio.h"
#include "hsc3.h"

#include "membuf.h"
#include "strnote.h"
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
	lb_info = NULL;
	addkw = NULL;
	common_path[0] = 0;
}


CHsc3::~CHsc3( void )
{
	if ( addkw != NULL ) { delete addkw; addkw=NULL; }
	if ( errbuf != NULL ) { delete errbuf; errbuf=NULL; }
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
		if ( option & HSC3_OPT_DEBUGMODE ) tk->RegistExtMacro( "_debug", "" );
	}
}


int CHsc3::PreProcessAht( char *fname, void *ahtoption, int mode )
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
	tk.SetCommonPath( common_path );
	tk.SetAHT( (AHTMODEL *)ahtoption );
	outbuf = new CMemBuf;

	if ( mode ) {
		ahtbuf = new CMemBuf;
		tk.SetAHTBuffer( ahtbuf );
	}

	sprintf( mm,"#AHT processor ver%s / onion software 1997-2019(c)", hspver );
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

int CHsc3::PreProcess( char *fname, char *outname, int option, char *rname, void *ahtoption )
{
	//		Preprocess execute
	//		(終了時にPreProcessEndを呼ぶこと)
	//			option : bit0=ver2.55 mode(ON)
	//			         bit1=debug mode(ON)
	//			         bit2=make packfile(ON)
	//					 bit3=read AHT file(on)
	//					 bit4=write AHT file(on)
	//					 bit5=UTF8(input)(入力ソースがUTF8であることを示す)
	//
	int res;
	char mm[512];
	CToken tk;
	CMemBuf *packbuf = NULL;

	lb_info = NULL;
	outbuf = new CMemBuf;
	ahtbuf = NULL;

	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path );
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

	sprintf( mm,"#%s ver%s / onion software 1997-2019(c)", HSC3TITLE, hspver );
	tk.Mes( mm );
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

	strcpy( hed_runtime, tk.GetHeaderRuntimeName() );
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
	if ( ahtbuf != NULL ) {
		delete ahtbuf;
		ahtbuf = NULL;
	}
}


int CHsc3::Compile( char *fname, char *outname, int mode )
{
	//		Compile
	//

	int res;
	int genmode;
	char mm[512];
	CToken tk;

	genmode = mode;
	if ( cmpopt & CMPMODE_UTF8OUT ) genmode |= HSC3_MODE_UTF8;

	if ( lb_info != NULL ) tk.SetLabelInfo( lb_info );		// プリプロセッサのラベル情報

	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path );
	tk.LabelRegist( hsp_prestr, 1 );
	tk.SetHeaderOption( hed_option, hed_runtime );
	tk.SetCmpOption( cmpopt );

	if ( process_option & HSC3_OPT_UTF8IN ) {
		tk.SetUTF8Input( 1 );
	}

	sprintf( mm,"#%s ver%s / onion software 1997-2019(c)", HSC3TITLE2, hspver );
	tk.Mes( mm );
	if (genmode & HSC3_MODE_UTF8) {
		tk.Mes("#use UTF-8 strings.");
	}
	if (genmode & HSC3_MODE_STRMAP) {
		tk.Mes("#output string map.");
	}

	if ( outbuf != NULL ) {
		res = tk.GenerateCode( outbuf, outname, genmode );
	} else {
		res = tk.GenerateCode( fname, outname, genmode| HSC3_MODE_STRMAP);
	}

	return res;
}


int CHsc3::CompileStrMap(char* fname, char* outname, int mode)
{
	return Compile(fname, outname, mode| HSC3_MODE_STRMAP);
}


void CHsc3::SetCommonPath( char *path )
{
	if ( path==NULL ) { common_path[0]=0; return; }
	strcpy( common_path, path );
}


int CHsc3::GetCmdList( int option )
{
	int res;
	CToken tk;
	CMemBuf outbuf;

	tk.SetErrorBuf( errbuf );
	tk.SetCommonPath( common_path );
	tk.LabelRegist3( hsp_prestr );			// 標準キーワード
	tk.LabelRegist3( hsp_prepp );			// プリプロセッサキーワード
	AddSystemMacros( &tk, option );

	res = tk.ExpandFile( &outbuf, "hspdef.as", "hspdef.as" );
//	if ( res<-1 ) return -1;
	tk.LabelDump( errbuf, DUMPMODE_ALL );

	//errbuf->PutStr("-----\r\n");
	//if ( addkw != NULL ) errbuf->PutStr( addkw->GetBuffer() );

	return 0;
}


int CHsc3::OpenPackfile( void )
{
	pfbuf = new CMemBuf( 0x1000 );
	if ( pfbuf->PutFile( "packfile" ) < 0 ) { delete pfbuf; return -1; }
	return 0;
}


void CHsc3::GetPackfileOption( char *out, char *keyword, char *defval )
{
	int max,i;
	char tmp[512];
	char *s;
	char a1;
	CStrNote note;
	note.Select( pfbuf->GetBuffer() );
	max = note.GetMaxLine();
	strcpy( out, defval );
	for( i=0;i<max;i++ ) {
		note.GetLine( tmp, i );
		if (( tmp[0]==';' )&&( tmp[1]=='!' )) {
			s = tmp+2;while(1) {
				a1 = *s;if (( a1==0 )||( a1=='=' )) break;
				s++;
			}
			if ( a1 != 0 ) {
				s[0]=0;
				if ( tstrcmp( tmp+2, keyword )) { strcpy( out, s+1 ); }
			}
		}
	}
}


int CHsc3::GetPackfileOptionInt( char *keyword, int defval )
{
	char tmp[512];
	char deftmp[32];
	sprintf( deftmp,"%d",defval );
	GetPackfileOption( tmp, keyword, deftmp );
	if (( tmp[0]>='0' )&&( tmp[0]<='9' )) return atoi( tmp );
	return defval;
}


void CHsc3::ClosePackfile( void )
{
	delete pfbuf;
}


int CHsc3::GetRuntimeFromHeader( char *fname, char *res )
{
	FILE *fp;
	HSPHED hsphed;
	int hedsize;
	int exsize;
	int ires;
	char *data;

	fp=fopen( fname, "rb" );
	if ( fp == NULL ) return -1;
	hedsize = sizeof(hsphed);
	fread( &hsphed, 1, hedsize, fp );
	exsize = hsphed.pt_cs - hedsize;

	if ( exsize == 0 ) {
		fclose(fp);
		return 0;
	}

	data = (char *)malloc( exsize );
	fread( data, 1, exsize, fp );
	fclose(fp);
	ires = 0;
	if ( hsphed.bootoption & HSPHED_BOOTOPT_RUNTIME ) {
		char runtime[HSP_MAX_PATH];
		strcpy( runtime, data + (hsphed.runtime - hedsize) );
		cutext( runtime );
		addext( runtime, "exe" );
		strcpy( res, runtime );
		ires = 1;
	}
	free( data );
	return ires;
}


int CHsc3::SaveOutbuf( char *fname )
{
	int res;
	res = outbuf->SaveFile( fname );
	if ( res<0 ) {
		return -1;
	}
	return 0;
}


int CHsc3::SaveAHTOutbuf( char *fname )
{
	int res;
	res = ahtbuf->SaveFile( fname );
	if ( res<0 ) {
		return -1;
	}
	return 0;
}


