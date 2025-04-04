
//
//	HSP3 External program manager (dummy)
//	onion software/onitama 2011/11
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3config.h"
#include "hsp3ext_ios.h"

#include "../../hsp3dish/hgio.h"
#include "../../hsp3dish/sysreq.h"
#include "../../hsp3embed/hsp3embed.h"

#ifdef HSPDISHGP
#include "iOSgpBridge.h"
#else
#include "iOSBridge.h"
#endif

/*------------------------------------------------------------*/
/*
		System Information initialization
*/
/*------------------------------------------------------------*/

static HSPCTX *hspctx;		// Current Context
static HSPEXINFO *exinfo;	// Info for Plugins

static void InitSystemInformation(void)
{
	//		コマンドライン & システムフォルダ関連
	hspctx->modfilename = "";
	hspctx->homefoldername = "";
	hspctx->tvfoldername = "";

    gb_getLocale(hspctx->langcode);
    if ((hspctx->langcode[0]=='j')&&(hspctx->langcode[1]=='a')) {
        hspctx->language = HSPCTX_LANGUAGE_JP;
    }
}


/*------------------------------------------------------------*/
/*
		type function
*/
/*------------------------------------------------------------*/

static int cmdfunc_dllcmd( int cmd )
{
	//		cmdfunc : TYPE_DLLCTRL
	//		(拡張DLLコントロールコマンド)
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)

	switch( cmd ) {							// サブコマンドごとの分岐
	case 0x00:								// newcom
		throw (HSPERR_UNSUPPORTED_FUNCTION);

	default:
		throw ( HSPERR_SYNTAX );
	}

	return RUNMODE_RUN;
}


static int termfunc_dllcmd( int option )
{
	//		termfunc : TYPE_DLLCMD
	//
	return 0;
}

void hsp3typeinit_dllcmd( HSP3TYPEINFO *info )
{
	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	//type = exinfo->nptype;
	//val = exinfo->npval;
	//exflg = exinfo->npexflg;

	info->cmdfunc = cmdfunc_dllcmd;
	//info->reffunc = reffunc_dllcmd;
	info->termfunc = termfunc_dllcmd;

	InitSystemInformation();
}

void hsp3typeinit_dllctrl( HSP3TYPEINFO *info )
{
}


/*------------------------------------------------------------*/
/*
		Sysinfo, getdir service
*/
/*------------------------------------------------------------*/

char* hsp3ext_getdir(int id)
{
	char *p = "";
	switch (id) {
	case 0:				//    カレント(現在の)ディレクトリ
	case 1:				//    HSPの実行ファイルがあるディレクトリ
	case 2:				//    Windowsディレクトリ
	case 3:				//    Windowsのシステムディレクトリ
	case 4:				//    コマンドライン文字列
	case 5:				//    HSPTV素材があるディレクトリ
	case 0x10005:			//    マイドキュメント
		break;
	case 6:				//    ランゲージコード
		p = hspctx->langcode;
		break;
	default:
		throw HSPERR_ILLEGAL_FUNCTION;
	}
	return p;
}



char *hsp3ext_sysinfo(int p2, int* res, char* outbuf)
{
	//		System strings get
	//
	int fl;
	char *p1;
	fl = HSPVAR_FLAG_STR;
	p1 = outbuf;
	*p1=0;

	switch(p2) {
	case 0:
#ifdef HSPIOS
        gb_getSysVer( p1 );
#endif
        break;
	case 1:
		break;
	case 2:
#ifdef HSPIOS
        gb_getSysModel( p1 );
#endif
		break;
	case 3:
		fl = HSPVAR_FLAG_INT;
		*(int*)p1 = hspctx->language;
		break;
	default:
		return NULL;
	}
	*res = fl;
	return p1;
}


void hsp3ext_execfile(char* msg, char* option, int mode)
{
#ifdef HSPNDK
	j_callActivity( msg, option, mode );
#endif
#ifdef HSPIOS
    gb_exec( mode, msg );
#endif
}

