
//
//	HSP3 manager
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsp3.h"
#include "hsp3config.h"
#include "hsp3debug.h"
#include "dpmread.h"
#include "strbuf.h"
#include "supio.h"

#ifdef HSP3IMP
#include "../hspinet/czcrypt.h"
#endif

static char startax[]={ 'S'-40,'T'-40,'A'-40,'R'-40,'T'-40,
			 '.'-40,'A'-40,'X'-40, 0 };


#define GetPRM(id) (&hspctx.mem_finfo[id])


/*------------------------------------------------------------*/
/*
		constructor
*/
/*------------------------------------------------------------*/

Hsp3::Hsp3()
{
	//		初期化
	//
	memset( &hspctx, 0, sizeof(HSPCTX) );
	code_setctx( &hspctx );
	code_init();
	hspctx.mem_mcs = NULL;
	axfile = NULL;
	axname = NULL;

	//		文字列バッファの初期化
	//
	hspctx.refstr = sbAlloc(HSPCTX_REFSTR_MAX);
	hspctx.fnbuffer = sbAlloc(HSP_MAX_PATH);
	hspctx.stmp = sbAlloc(HSPCTX_REFSTR_MAX);
	hspctx.cmdline = sbAlloc(HSPCTX_CMDLINE_MAX);
	hspctx.modfilename = sbAlloc(HSPCTX_PATH_MAX);
	hspctx.tvfoldername = sbAlloc(HSPCTX_PATH_MAX);
	hspctx.homefoldername = sbAlloc(HSPCTX_PATH_MAX);

	//		language setup
	hspctx.language = HSPCTX_LANGUAGE_EN;				// Default Language

	hspctx.langcode[0] = 'e';
	hspctx.langcode[1] = 'n';
	hspctx.langcode[2] = 0;
	hspctx.langcode[3] = 0;
}

Hsp3::~Hsp3()
{
	//		すべて破棄
	//
	code_termfunc();
	Dispose();
	code_bye();
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void Hsp3::SetFileName( char *name )
{
	if ( *name == 0 ) { axname = NULL; return; }
	axname = name;
}


void Hsp3::Dispose( void )
{
	//		クリーンアップ
	//
	if (hspctx.mem_mcs == NULL) return;
	
	code_cleanup();

	//		axを破棄
	//
	if ( hspctx.mem_var != NULL ) {
		int i;
		for(i=0;i<maxvar;i++) {
			HspVarCoreDispose( &hspctx.mem_var[i] );
		}
		delete [] hspctx.mem_var;	hspctx.mem_var = NULL;
	}

	hspctx.mem_mcs = NULL;
	if ( axfile != NULL ) {
		mem_bye( axfile );
		axfile = NULL;
	}
}

int Hsp3::Reset( int mode )
{
	//		axを初期化
	//		mode: 0 = normal(debug) mode
	//		      other = packfile PTR
	//
	int i;
	char *ptr;
	char fname[512];
	HSPHED *hsphed;
	if ( hspctx.mem_mcs != NULL ) Dispose();

	//		load HSP execute object
	//
	axtype = HSP3_AXTYPE_NONE;
	if ( mode ) {									// "start.ax"を呼び出す
		i = dpm_ini("", mode, hsp_sum, hsp_dec);	// customized EXE mode
		//axname = NULL;
	} else {
		dpm_ini( "data.dpm",0,-1,-1 );				// original EXE mode
	}

#ifdef HSP3IMP
	//		HSP3IMP用読み込み(暗号化ax対応)
	if ( axname == NULL ) {
		ptr = dpm_readalloc( "start.ax" );
		if ( ptr == NULL ) {
			int sz;
			CzCrypt crypt;
			if ( crypt.DataLoad( "start.axe" ) ) return -1;
			crypt.SetSeed( hsp_sum, hsp_dec );
			crypt.Decrypt();
			sz = crypt.GetSize();
			ptr = mem_ini( sz );
			memcpy( ptr, crypt.GetData(), sz );
			axtype |= HSP3_AXTYPE_ENCRYPT;
		}
	} else {
		ptr = dpm_readalloc( axname );
		if ( ptr == NULL ) return -1;
	}
#else
	//		start.ax読み込み
	if ( axname == NULL ) {
		unsigned char *p;
		unsigned char *s;
		unsigned char ap;
		int sum;
		sum = 0;
		p = (unsigned char *)fname;
		s = (unsigned char *)startax;
		while(1) {
			ap = *s++;if ( ap==0 ) break;
			ap += 40; *p++ = ap;
			sum = sum*17 + (int)ap;
		}
		*p = 0;
		if ( sum != 0x6cced385 ) return -1;
		if ( mode ) {
			if (dpm_filebase(fname) != 1) return -1;	// DPM,packfileからのみstart.axを読み込む
		}
	}
	else {
		strcpy( fname, axname );
	}

	ptr = dpm_readalloc(fname);
	if (ptr == NULL) return -1;
#endif

	axfile = ptr;

	//		memory location set
	//
	hsphed = (HSPHED *)ptr;

	if ((hsphed->h1!='H')||(hsphed->h2!='S')||(hsphed->h3!='P')||(hsphed->h4!='3')) {
		mem_bye( axfile );
		return -1;
	}

	maxvar = hsphed->max_val;
	hspctx.hsphed = hsphed;
	hspctx.mem_mcs = (unsigned short *)copy_DAT(ptr + hsphed->pt_cs, hsphed->max_cs);
	hspctx.mem_mds = (char *)( ptr + hsphed->pt_ds );
	hspctx.mem_ot = (int *)copy_DAT(ptr + hsphed->pt_ot, hsphed->max_ot);
	hspctx.mem_di = (unsigned char *)copy_DAT(ptr + hsphed->pt_dinfo, hsphed->max_dinfo);

	hspctx.mem_linfo = (LIBDAT *)copy_LIBDAT(hsphed, ptr + hsphed->pt_linfo, hsphed->max_linfo);
	hspctx.mem_minfo = (STRUCTPRM *)copy_DAT(ptr + hsphed->pt_minfo, hsphed->max_minfo);
	hspctx.mem_finfo = (STRUCTDAT *)copy_STRUCTDAT(hsphed, ptr + hsphed->pt_finfo, hsphed->max_finfo);

	//		init strmap
	int *exopt_pt = (int *)(ptr + hsphed->pt_exopt);
	int* dsindex = NULL;
	int dsindex_size = 0;
	if (hsphed->max_exopt) {
		while (1) {
			int tag,bodysize;
			tag = *exopt_pt++;
			bodysize = tag >> 16;
			tag = tag & 0xffff;
			if (tag == HSPHED_EXOPTION_TAG_NONE) {
				break;
			}
			if (tag == HSPHED_EXOPTION_TAG_DSINDEX) {
				dsindex = exopt_pt;
				exopt_pt+= bodysize;
				dsindex_size = bodysize;
				continue;
			}
			break;
		}
	}
	hspctx.dsindex = dsindex;
	hspctx.dsindex_size = dsindex_size;

	HspVarCoreResetVartype( hsphed->max_varhpi );		// 型の初期化

	code_resetctx( &hspctx );		// hsp3code setup

	//		hspstat check
#ifdef HSPWIN
#ifndef HSPWINGUI
	hspctx.hspstat |= HSPSTAT_CONSOLE;
#endif
#endif

#ifdef HSPMAC
	hspctx.hspstat |= HSPSTAT_MAC;
#endif

#ifdef HSPLINUX
	hspctx.hspstat |= HSPSTAT_LINUX;
#ifndef HSPLINUXGUI
	hspctx.hspstat |= HSPSTAT_CONSOLE;
#endif
#endif

#ifdef HSPDISH
	hspctx.hspstat |= HSPSTAT_DISH;
#endif

#ifdef HSPDISH
	hspctx.hspstat |= HSPSTAT_DISH;
#endif

#ifdef HSPUTF8
	hspctx.hspstat |= HSPSTAT_UTF8;
#endif

#ifdef HSP64
	hspctx.hspstat |= HSPSTAT_HSP64;
#endif

	//		HspVar setup
	hspctx.mem_var = NULL;
	if ( maxvar ) {
		int i;
		hspctx.mem_var = new PVal[maxvar];

		for(i=0;i<maxvar;i++) {
			PVal *pval = &hspctx.mem_var[i];
			pval->mode = HSPVAR_MODE_NONE;
			pval->flag = HSPVAR_FLAG_INT;				// 仮の型
			HspVarCoreClear( pval, HSPVAR_FLAG_INT );	// グローバル変数を0にリセット
		}
	}

	//		debug
	//Alertf( "#HSP objcode initalized.(CS=%d/DS=%d/OT=%d/VAR=%d)\n",hsphed->max_cs, hsphed->max_ds, hsphed->max_ot, hsphed->max_val );
	code_setpc( hspctx.mem_mcs );
	code_debug_init();
	return 0;
}


void Hsp3::SetPackValue( int sum, int dec )
{
	hsp_sum = sum;
	hsp_dec = dec;
}


void Hsp3::SetCommandLinePrm(char *prm)
{
	// コマンドライン文字列を指定する
	sbStrCopy(&hspctx.cmdline, prm);
}


void Hsp3::SetModuleFilePrm(char *prm)
{
	// モジュールファイル名文字列を指定する
	sbStrCopy(&hspctx.modfilename, prm);
}


void Hsp3::SetHSPTVFolderPrm(char *prm)
{
	// HSPTVフォルダ名文字列を指定する
	sbStrCopy(&hspctx.tvfoldername, prm);
}


void Hsp3::SetHomeFolderPrm(char *prm)
{
	// システムフォルダ名文字列を指定する
	sbStrCopy(&hspctx.homefoldername, prm);
}


/*------------------------------------------------------------*/
/*
util
*/
/*------------------------------------------------------------*/

void* Hsp3::copy_DAT(char *ptr, size_t size)
{
	if (size <= 0) return ptr;

	void* dst = malloc(size);
	memcpy(dst, ptr, size);
	return dst;
}


LIBDAT *Hsp3::copy_LIBDAT(HSPHED *hsphed, char *ptr, size_t size )
{
	//	libdatの準備
	int i,max;
	int newsize;
	LIBDAT *mem_dst;
	LIBDAT *dst;
	HED_LIBDAT org_dat;

	max = (int)( size / sizeof(HED_LIBDAT));
	if (max <= 0) return (LIBDAT *)ptr;
	newsize = sizeof(LIBDAT)*max;
	mem_dst = (LIBDAT *)malloc( newsize );
	dst = mem_dst;
	for (i = 0; i < max; i++) {
		memcpy(&org_dat, ptr, sizeof(HED_LIBDAT));

		dst->flag = org_dat.flag;
		dst->nameidx = org_dat.nameidx;
		dst->clsid = org_dat.clsid;
		dst->hlib = NULL;

		dst++;
		ptr += sizeof(HED_LIBDAT);
	}
	hsphed->max_linfo = newsize;
	return mem_dst;
}


STRUCTDAT *Hsp3::copy_STRUCTDAT(HSPHED *hsphed, char *ptr, size_t size)
{
	//	structdatの準備
	int i, max;
	int newsize;
	STRUCTDAT *mem_dst;
	STRUCTDAT *dst;
	HED_STRUCTDAT org_dat;
	max = (int)(size / sizeof(HED_STRUCTDAT));
	if (max <= 0) return (STRUCTDAT *)ptr;
	newsize = sizeof(STRUCTDAT)*max;
	mem_dst = (STRUCTDAT *)malloc(sizeof(STRUCTDAT)*max);
	dst = mem_dst;
	for (i = 0; i < max; i++) {
		memcpy(&org_dat, ptr, sizeof(HED_STRUCTDAT));

		dst->index = org_dat.index;
		dst->subid = org_dat.subid;
		dst->prmindex = org_dat.prmindex;
		dst->prmmax = org_dat.prmmax;
		dst->nameidx = org_dat.nameidx;
		dst->size = org_dat.size;
		dst->otindex = org_dat.otindex;
		dst->funcflag = org_dat.funcflag;
#ifdef PTR64BIT
		dst->proc = NULL;
#endif

#ifdef HSP64
		if ((dst->index == STRUCTDAT_INDEX_FUNC) ||
			(dst->index == STRUCTDAT_INDEX_CFUNC) ||
			(dst->index == STRUCTDAT_INDEX_STRUCT)) {
			//	STRUCTPRMのoffset,size値を調整する (各メンバのサイズを2倍にする)
			int j;
			dst->size *= 2;
			for (j = 0; j < dst->prmmax; j++) {
				STRUCTPRM *prm = &hspctx.mem_minfo[dst->prmindex + j];
				if (prm->mptype == MPTYPE_STRUCTTAG) continue;
				//Alertf("INIT: type%d: subid:%d offset:%d", prm->mptype, prm->subid, prm->offset);
				prm->offset *= 2;
			}
		}
#endif

		dst++;
		ptr += sizeof(HED_STRUCTDAT);
	}
	hsphed->max_finfo = newsize;
	return mem_dst;
}


