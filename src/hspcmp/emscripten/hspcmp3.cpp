
//
//		HSP compile/package functions for HSP3
//				onion software/onitama 2002
//

#include <stdio.h>
#include <emscripten.h>

#include "../../hsp3/hsp3config.h"

#include "../../hsp3/hsp3debug.h"			// hsp3 error code
#include "../../hsp3/hsp3struct.h"			// hsp3 core define
#include "../../hsp3/hspwnd.h"				// hsp3 windows define

#include "../supio.h"
#include "../hsc3.h"
#include "../token.h"
#include "../ahtobj.h"

#define DPM_SUPPORT		// DPMファイルマネージャをサポート
#include "../win32dll/dpm.h"

#define EXPORT extern "C" EMSCRIPTEN_KEEPALIVE
#define BOOL int

static char fname[HSP_MAX_PATH];
static char rname[HSP_MAX_PATH];
static char oname[HSP_MAX_PATH];
static char hspexe[HSP_MAX_PATH];
static int opt1,opt2,opt3;

static int orgcompath=0;
static char compath[HSP_MAX_PATH];

static CHsc3 *hsc3=NULL;

extern char *hsp_prestr[];

int main()
{
	hsc3 = new CHsc3;
	return 0;
}

//----------------------------------------------------------

static int GetFilePath( char *bname )
{
	//		フルパス名から、ファイルパスの取得(\を残す)
	//
	int a,b,len;
	char a1;
	b=-1;
	len=strlen(bname);
	for(a=0;a<len;a++) {
		a1=bname[a];
		if (a1=='/') b=a;
		if (a1<0) a++; 
	}
	if (b<0) return 1;
	bname[b+1]=0;
	return 0;
}

//----------------------------------------------------------

EXPORT BOOL hsc_ini ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		hsc_ini "src-file"  (type6)
	//
	strcpy(fname,p1);
	strcpy(rname,p1);
	strcpy(oname,p1);
	cutext(oname);
	strcat(oname,".ax");
	return 0;
}


EXPORT BOOL hsc_refname ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		hsc_refname "ref-file"  (type6)
	//
	strcpy(rname,p1);
	return 0;
}


EXPORT BOOL hsc_objname ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		hsc_objname "obj-file"  (type6)
	//
	strcpy(oname,p1);
	return 0;
}


EXPORT BOOL hsc_ver ( int p1, int p2, int p3, char *p4 )
{
	//
	//		hsc_ver (type$10)
	//
	sprintf( p4,"%s ver%s", HSC3TITLE, hspver );
	return 0;
}


EXPORT BOOL hsc_bye ( int p1, int p2, int p3, int p4 )
{
	//
	//		hsc_bye (type$100)
	//
	return 0;
}


EXPORT BOOL hsc_getmes ( char *p1, int p2, int p3, int p4 )
{
	//
	//		hsc_getmes val (type1)
	//
	strcpy( p1, hsc3->GetError() );
	return 0;
}


EXPORT BOOL hsc_clrmes ( int p1, int p2, int p3, int p4 )
{
	//
	//		hsc_clrmes (type0)
	//
	hsc3->ResetError();
	return 0;
}


EXPORT BOOL hsc_compath ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		hsc_compath "common-path"  (type6)
	//
	strcpy(compath,p1);
	orgcompath=1;
	return 0;
}


EXPORT BOOL hsc_comp ( int p1, int p2, int p3, int p4 )
{
	//
	//		hsc_comp mode,ppopt,dbgopt  (type0)
	//			( mode: 1=debug/0=no debug )
	//			(       2=preprocessor only )
	//			( ppopt = preprocessor option )
	//			(       0=default/1=ver2.6 mode )
	//			( dbgopt = debug window option )
	//			(       0=default/1=debug mode )
/*
	int st;
	st=tcomp_main( rname, fname, oname, mesbuf, p1, compath );
*/
	int st;
	int ppopt;
	int cmpmode;
	char fname2[HSP_MAX_PATH];

	hsc3->ResetError();
	if (orgcompath==0) {
		strcpy( compath,"/common/" );
	}
	strcpy( fname2, fname );
	strcat( fname2, ".i" );
	printf("common:%s\n", compath);
	hsc3->SetCommonPath( compath );
	ppopt = HSC3_OPT_UTF8IN;
	if ( p1 ) ppopt|=HSC3_OPT_DEBUGMODE;
	if ( p2&1 ) ppopt|=HSC3_OPT_NOHSPDEF;
	if ( p2&4 ) ppopt|=HSC3_OPT_MAKEPACK;
	if ( p2&8 ) ppopt|=HSC3_OPT_READAHT;
	if ( p2&16 ) ppopt|=HSC3_OPT_MAKEAHT;
	printf("fname:%s, fname2:%s\n", fname, fname2);
	st = hsc3->PreProcess( fname, fname2, ppopt, rname );
	if ( st != 0 ) {
		hsc3->PreProcessEnd();
		return st;
	}
	if ( p1 == 2 ) {
		hsc3->PreProcessEnd();
		return 0;
	}

	cmpmode = p1 & HSC3_MODE_DEBUG;
	cmpmode |= HSC3_MODE_UTF8;
	if ( p3 ) cmpmode |= HSC3_MODE_DEBUGWIN;
	printf("fname2:%s, oname:%s\n", fname2, oname);
	st = hsc3->Compile( fname2, oname, cmpmode );
	hsc3->PreProcessEnd();
	printf("st:%d\n", st);
	if ( st != 0 ) return st;
	return 0;
}


//----------------------------------------------------------

EXPORT BOOL pack_ini ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		pack_ini "src-file"  (type6)
	//
#ifdef DPM_SUPPORT
	strcpy(fname,p1);
	cutext(fname);
	if ( hsc3==NULL ) Alert( "#No way." );
	hsc3->ResetError();
	dpmc_ini( hsc3->errbuf, fname );
	opt1=640;opt2=480;opt3=0;
	strcpy(hspexe,"hsprt");
#endif
	return 0;
}


EXPORT BOOL pack_view ( int p1, int p2, int p3, int p4 )
{
	//
	//		pack_view (type0)
	//
	int st;
#ifdef DPM_SUPPORT
	st = dpmc_view();
#else
	st = 0;
#endif
	return -st;
}


EXPORT BOOL pack_make ( int p1, int p2, int p3, int p4 )
{
	//
	//		pack_make mode, key(type0)
	//		     mode : (1=ForDPM/0=ForExecutable)
	//		     key  : (0=Default/other=New Seed)
	//
	int st;
#ifdef DPM_SUPPORT
	if ( p2 != 0 ) dpmc_dpmkey( p2 );
	st=dpmc_pack(p1);
#else
	st = 0;
#endif
	return -st;
}


EXPORT BOOL pack_opt ( int p1, int p2, int p3, int p4 )
{
	//
	//		pack_opt sx,sy,disp_sw (type0)
	//
	opt1=p1;if (opt1==0) opt1=640;
	opt2=p2;if (opt2==0) opt2=480;
	opt3=p3;							// disp SW (1=blank window)
	return 0;
}


EXPORT BOOL pack_rt ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		pack_rt "runtime-file"  (type6)
	//
	strcpy(hspexe,p1);
	return 0;
}


EXPORT BOOL pack_exe ( int p1, int p2, int p3, int p4 )
{
	//
	//		pack_exe mode (type0)
	//
	int st;
#ifdef DPM_SUPPORT
	st=dpmc_mkexe(p1,hspexe,opt1,opt2,opt3);
#else
	st = 0;
#endif
	return -st;
}


EXPORT BOOL pack_get ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		pack_get "get-file"  (type6)
	//
	int st;
#ifdef DPM_SUPPORT
	st=dpmc_get(p1);
#else
	st = 0;
#endif
	return -st;
}


//----------------------------------------------------------
//		Additional service on 2.6
//----------------------------------------------------------

EXPORT BOOL hsc3_getsym ( int p1, int p2, int p3, int p4 )
{
	//
	//		hsc3_getsym val  (type1)
	//
	hsc3->ResetError();
	if (orgcompath==0) {
		strcpy( compath,"/common/" );
	}
	hsc3->SetCommonPath( compath );
	if ( hsc3->GetCmdList( p1|2 ) ) return -1;
	return 0;
}


EXPORT BOOL hsc3_messize ( int *p1, int p2, int p3, int p4 )
{
	//
	//		hsc3_messize val  (type1)
	//
	*p1 = hsc3->GetErrorSize();
	return 0;
}


EXPORT BOOL hsc3_make ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		hsc3_make "myname"  (type6)
	//
	char libpath[HSP_MAX_PATH];
	int i,type;
	int opt3a,opt3b;
	int st;

	if ( hsc3==NULL ) Alert( "#No way." );
	hsc3->ResetError();

	strcpy( libpath, p1 );
	GetFilePath( libpath );

	i = hsc3->OpenPackfile();
	if (i) { Alert( "packfileが見つかりません" ); return -1; }
	hsc3->GetPackfileOption( hspexe, "runtime", "hsprt" );
	strcat( libpath, hspexe );
	strcpy( hspexe, libpath );
	hsc3->GetPackfileOption( fname, "name", "hsptmp" );
	cutext( fname );
	type = hsc3->GetPackfileOptionInt( "type", 0 );
	opt1 = hsc3->GetPackfileOptionInt( "xsize", 640 );
	opt2 = hsc3->GetPackfileOptionInt( "ysize", 480 );
	opt3a = hsc3->GetPackfileOptionInt( "hide", 0 );
	opt3b = hsc3->GetPackfileOptionInt( "orgpath", 0 );
	opt3 = 0;
	if ( opt3a ) opt3 |= 1;
	if ( opt3b ) opt3 |= 2;

	hsc3->ClosePackfile();

	//		exeを作成
#ifdef DPM_SUPPORT
	dpmc_ini( hsc3->errbuf, fname );
	st=dpmc_pack( 0 );
	if ( st ) return -st;
	st=dpmc_mkexe( type, hspexe, opt1, opt2, opt3 );
	strcat( fname, ".dpm" );
	delfile( fname );
#else
	st = 0;
#endif
	return -st;
}


//----------------------------------------------------------
//		Additional service on 3.0
//----------------------------------------------------------

EXPORT BOOL hsc3_getruntime ( char *p1, char *p2, int p3, int p4 )
{
	//
	//		hsc3_getruntime val  (type5)
	//
	int i;
	i = hsc3->GetRuntimeFromHeader( p2, p1 );
	if ( i != 1 ) { *p1 = 0; }
	return 0;
}
