
//
//	HSP3 External program manager (dummy)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hsp3ext_emscripten.h"

static HSPCTX *hspctx;		// Current Context
static HSPEXINFO *exinfo;	// Info for Plugins
static int *type;
static int *val;
static int *exflg;
static int reffunc_intfunc_ivalue;

/*------------------------------------------------------------*/
/*
		System Information initialization
*/
/*------------------------------------------------------------*/

static void InitSystemInformation(void)
{
	//		コマンドライン & システムフォルダ関連

}



/*------------------------------------------------------------*/
/*
		type function
*/
/*------------------------------------------------------------*/

static int cmdfunc_ctrlcmd( int cmd )
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


static void *reffunc_dllcmd( int *type_res, int arg )
{
	//		reffunc : TYPE_DLLFUNC
	//		(拡張DLL関数)
	//

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != '(' ) throw ( HSPERR_INVALID_FUNCPARAM );

	*type_res = HSPVAR_FLAG_INT;
	//exec_dllcmd( arg, STRUCTDAT_OT_FUNCTION );
	reffunc_intfunc_ivalue = hspctx->stat;

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw ( HSPERR_INVALID_FUNCPARAM );
	if ( *val != ')' ) throw ( HSPERR_INVALID_FUNCPARAM );
	code_next();

	return &reffunc_intfunc_ivalue;
}


static int termfunc_dllcmd( int option )
{
	//		termfunc : TYPE_DLLCMD
	//
	return 0;
}

void hsp3typeinit_dllcmd( HSP3TYPEINFO *info )
{
	InitSystemInformation();

	hspctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	exflg = exinfo->npexflg;

	info->cmdfunc = cmdfunc_dllcmd;
	info->reffunc = reffunc_dllcmd;
	info->termfunc = termfunc_dllcmd;

}

void hsp3typeinit_dllctrl( HSP3TYPEINFO *info )
{
}


/*------------------------------------------------------------*/
/*
		Sysinfo, getdir service
*/
/*------------------------------------------------------------*/

