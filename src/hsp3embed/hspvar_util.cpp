
//
//	HSPVAR utility
//	onion software/onitama 2008/4
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HSPWIN
#include <windows.h>
#endif

#include "../hsp3/hsp3config.h"

#include "../hsp3/hsp3struct.h"
#include "../hsp3/stack.h"
#include "../hsp3/strbuf.h"
#include "../hsp3/hsp3code.h"
#include "../hsp3/supio.h"
#include "hspvar_util.h"

/*------------------------------------------------------------*/
/*
		extra header
*/
/*------------------------------------------------------------*/

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

extern CHSP3_TASK __HspTaskFunc[];		// hsp3cnvで生成されるタスク関数リスト
void __HspEntry( void );				// hsp3cnvで生成されるエントリーポイント

extern char *__HspFuncName[];			// hsp3cnvで生成される定義文字列リスト

static	HSPCTX *hspctx;					// HSPのコンテキスト
static	CHSP3_TASK curtask;				// 次に実行されるタスク関数
static int *c_type;
static int *c_val;
static HSPEXINFO *exinfo;				// Info for Plugins
static int lasttask;

PVal *mem_var;							// 変数用のメモリ
int	prmstacks;							// パラメータースタック数(モジュール呼び出し用)

PVal var_proxy;							// 代理変数用のメモリ


static	HSP3TYPEINFO *intcmd_info;
static	HSP3TYPEINFO *extcmd_info;
static	HSP3TYPEINFO *extsysvar_info;
static	HSP3TYPEINFO *intfunc_info;
static	HSP3TYPEINFO *sysvar_info;
static	HSP3TYPEINFO *progfunc_info;
static	HSP3TYPEINFO *modfunc_info;
static	HSP3TYPEINFO *dllfunc_info;
static	HSP3TYPEINFO *dllctrl_info;
static	HSP3TYPEINFO *usrfunc_info;

static	HSP3_CMDFUNC intcmd_func;
static	HSP3_CMDFUNC extcmd_func;
static	HSP3_CMDFUNC progcmd_func;
static	HSP3_CMDFUNC modcmd_func;
static	HSP3_CMDFUNC dllcmd_func;
static	HSP3_CMDFUNC dllctl_func;
static	HSP3_CMDFUNC usrcmd_func;

/*------------------------------------------------------------*/

static HspVarProc *varproc;
static STMDATA *stm1;
static STMDATA *stm2;
static int tflag;
static int arrayobj_flag;


static void HspVarCoreArray2( PVal *pval, int offset )
{
	//		配列要素の指定 (index)
	//		( Reset後に次元数だけ連続で呼ばれます )
	//
	if ( pval->arraycnt >= 5 ) throw HSPVAR_ERROR_ARRAYOVER;
	if ( pval->arraycnt == 0 ) {
		pval->arraymul = 1;			// 最初の値
	} else {
		pval->arraymul *= pval->len[ pval->arraycnt ];
	}
	pval->arraycnt++;
	if ( offset < 0 ) throw HSPVAR_ERROR_ARRAYOVER;
	if ( offset >= (pval->len[ pval->arraycnt ]) ) {
		if ((pval->arraycnt >= 4 )||( pval->len[ pval->arraycnt+1 ]==0 )) {
			if ( pval->support & HSPVAR_SUPPORT_FLEXARRAY ) {
				//Alertf("Expand.(%d)",offset);
				HspVarCoreReDim( pval, pval->arraycnt, offset+1 );	// 配列を拡張する
				pval->offset += offset * pval->arraymul;
				return;
			}
		}
		throw HSPVAR_ERROR_ARRAYOVER;
	}
	pval->offset += offset * pval->arraymul;
}


static char *PrepareCalc( void )
{
	//		スタックから２項目取り出し計算の準備を行なう
	//
	char *calc_ptr;

#if 0
	if ( StackGetLevel < 2 ) {
		Alertf("Stack underflow(%d)",StackGetLevel);
	}
#endif

	stm2 = StackPeek;
	stm1 = StackPeek2;
	tflag = stm1->type;

	if ( tflag == HSPVAR_FLAG_INT ) {
		if ( stm2->type == HSPVAR_FLAG_INT ) {					// HSPVAR_FLAG_INT のみ高速化
			return NULL;
		}
	}

	mpval = HspVarCoreGetPVal( tflag );
	varproc = HspVarCoreGetProc( tflag );

	if ( mpval->mode == HSPVAR_MODE_NONE ) {					// 型に合わせたテンポラリ変数を初期化
		if ( varproc->flag == 0 ) {
			throw HSPERR_TYPE_INITALIZATION_FAILED;
		}
		HspVarCoreClearTemp( mpval, tflag );					// 最小サイズのメモリを確保
	}

	varproc->Set( mpval, (PDAT *)mpval->pt, STM_GETPTR(stm1) );	// テンポラリ変数に初期値を設定

	calc_ptr = STM_GETPTR(stm2);
	if ( tflag != stm2->type ) {								// 型が一致しない場合は変換
		if ( stm2->type >= HSPVAR_FLAG_USERDEF ) {
			calc_ptr = (char *)HspVarCoreGetProc(stm2->type)->CnvCustom( calc_ptr, tflag );
		} else {
			calc_ptr = (char *)varproc->Cnv( calc_ptr, stm2->type );
		}
	}
	//calcprm( varproc, (PDAT *)mpval->pt, op, ptr );				// 計算を行なう
	return calc_ptr;
}


static void AfterCalc( void )
{
	//		計算後のスタック処理
	//
	int basesize;
	StackPop();
	StackPop();

	if ( varproc->aftertype != tflag ) {						// 演算後に型が変わる場合
		tflag = varproc->aftertype;
		varproc = HspVarCoreGetProc( tflag );
	}
	basesize = varproc->basesize;
	if ( basesize < 0 ) {
		basesize = varproc->GetSize( (PDAT *)mpval->pt );
	}
	StackPush( tflag, mpval->pt, basesize );
}


static inline void code_arrayint2( PVal *pval, int offset )
{
	//		配列要素の指定 (index)
	//		( Reset後に次元数だけ連続で呼ばれます )
	//
	if ( pval->arraycnt >= 5 ) throw HSPVAR_ERROR_ARRAYOVER;
	if ( pval->arraycnt == 0 ) {
		pval->arraymul = 1;			// 最初の値
	} else {
		pval->arraymul *= pval->len[ pval->arraycnt ];
	}
	pval->arraycnt++;
	if ( offset < 0 ) throw HSPVAR_ERROR_ARRAYOVER;
	if ( offset >= (pval->len[ pval->arraycnt ]) ) {
		if ((pval->arraycnt >= 4 )||( pval->len[ pval->arraycnt+1 ]==0 )) {
			if ( pval->support & HSPVAR_SUPPORT_FLEXARRAY ) {
				//Alertf("Expand.(%d)",offset);
				HspVarCoreReDim( pval, pval->arraycnt, offset+1 );	// 配列を拡張する
				pval->offset += offset * pval->arraymul;
				return;
			}
		}
		throw HSPVAR_ERROR_ARRAYOVER;
	}
	pval->offset += offset * pval->arraymul;
}


static inline APTR CheckArray( PVal *pval, int ar )
{
	//		Check PVal Array information
	//		(配列要素(int)の取り出し)
	//
	int chk,i;
	int val;
	PVal temp;
	arrayobj_flag = 0;
	HspVarCoreReset( pval );							// 配列ポインタをリセットする
	if ( ar == 0 ) return 0;

	if ( pval->support & HSPVAR_SUPPORT_MISCTYPE ) {	// 連想配列の場合
		return 0;
	}

	for(i=0;i<ar;i++) {
		HspVarCoreCopyArrayInfo( &temp, pval );			// 状態を保存
		chk = code_get();
		if ( chk<=PARAM_END ) { throw HSPERR_BAD_ARRAY_EXPRESSION; }
		if ( mpval->flag != HSPVAR_FLAG_INT ) { throw HSPERR_TYPE_MISMATCH; }
		HspVarCoreCopyArrayInfo( pval, &temp );			// 状態を復帰
		val = *(int *)(mpval->pt);
		HspVarCoreArray2( pval, val );					// 配列要素指定(整数)
	}
	return HspVarCoreGetAPTR( pval );
}


/*------------------------------------------------------------*/

void VarUtilInit( void )
{
	//		HSPVAR utilityの初期化
	//
	hspctx = code_getctx();
	mem_var = hspctx->mem_var;
	exinfo = hspctx->exinfo2;
	c_type = exinfo->nptype;
	c_val = exinfo->npval;

	//		typeinfoを取得しておく
	intcmd_info = code_gettypeinfo( TYPE_INTCMD );
	extcmd_info = code_gettypeinfo( TYPE_EXTCMD );
	extsysvar_info = code_gettypeinfo( TYPE_EXTSYSVAR );
	intfunc_info = code_gettypeinfo( TYPE_INTFUNC );
	sysvar_info = code_gettypeinfo( TYPE_SYSVAR );
	progfunc_info = code_gettypeinfo( TYPE_PROGCMD );
	modfunc_info = code_gettypeinfo( TYPE_MODCMD );
	dllfunc_info = code_gettypeinfo( TYPE_DLLFUNC );
	dllctrl_info = code_gettypeinfo( TYPE_DLLCTRL );
	usrfunc_info = code_gettypeinfo( TYPE_USERDEF );

	//		実行用関数を抽出
	intcmd_func = intcmd_info->cmdfunc;
	extcmd_func = extcmd_info->cmdfunc;
	progcmd_func = progfunc_info->cmdfunc;
	modcmd_func = modfunc_info->cmdfunc;
	if ( dllfunc_info ) dllcmd_func = dllfunc_info->cmdfunc;
	if ( dllctrl_info ) dllctl_func = dllctrl_info->cmdfunc;
	if ( usrfunc_info ) usrcmd_func = usrfunc_info->cmdfunc;

	//		最初のタスク実行関数をセット
	curtask = (CHSP3_TASK)__HspEntry;

	//		代理変数を初期化(パラメーター渡し用)
	var_proxy.offset = 0;
}


void VarUtilTerm( void )
{
	//		HSPVAR utilityの終了処理
	//
}


/*------------------------------------------------------------*/
/*
		stack operation
*/
/*------------------------------------------------------------*/

#if 0
void PushInt( int val )
{
	StackPushi( val );
}

void PushDouble( double val )
{
	//StackPush( HSPVAR_FLAG_DOUBLE, (char *)&val, sizeof(double) );
	StackPushd( val );
}

void PushLabel( int val )
{
	//StackPush( HSPVAR_FLAG_LABEL, (char *)&val, sizeof(int)  );
	StackPushl( val );
}
#endif


void PushStr( char *st )
{
	StackPushStr( st );
}


void PushVar( PVal *pval, int aval )
{
	//	変数の値をpushする
	int basesize;
	APTR aptr;
	PDAT *ptr;

	aptr = CheckArray( pval, aval );

	tflag = pval->flag;
	if ( tflag == HSPVAR_FLAG_INT ) {
		ptr = (PDAT *)(( (int *)(pval->pt))+ aptr );		// 自前で計算
		StackPushi( *(int *)ptr );
		return;
	}
	if ( tflag == HSPVAR_FLAG_DOUBLE ) {
		ptr = (PDAT *)(( (double *)(pval->pt))+ aptr );		// 自前で計算
		StackPushd( *(double *)ptr );
		return;
	}

	ptr = HspVarCorePtrAPTR( pval, aptr );
	varproc = HspVarCoreGetProc( tflag );
	basesize = varproc->basesize;
	if ( basesize < 0 ) { basesize = varproc->GetSize( ptr ); }
	StackPush( tflag, (char *)ptr, basesize );
}


void PushVAP( PVal *pval, int aval )
{
	//	変数そのもののポインタをpushする
	APTR aptr;
	//PDAT *ptr;
	aptr = CheckArray( pval, aval );
	//ptr = HspVarCorePtrAPTR( pval, aptr );
	//StackPushTypeVal( HSPVAR_FLAG_VAR, (int)(size_t)pval, (int)(size_t)aptr );
    StackPushVar( pval, aptr );
}


void PushVarFromVAP( PVal *pval, int aval )
{
	//	PushVAPされている変数の値をpushする
	int basesize;
	APTR aptr;
	PDAT *ptr;

	aptr = (APTR)aval;

	tflag = pval->flag;
	if ( tflag == HSPVAR_FLAG_INT ) {
		ptr = (PDAT *)(( (int *)(pval->pt))+ aptr );		// 自前で計算
		StackPushi( *(int *)ptr );
		return;
	}
	if ( tflag == HSPVAR_FLAG_DOUBLE ) {
		ptr = (PDAT *)(( (double *)(pval->pt))+ aptr );		// 自前で計算
		StackPushd( *(double *)ptr );
		return;
	}

	ptr = HspVarCorePtrAPTR( pval, aptr );
	varproc = HspVarCoreGetProc( tflag );
	basesize = varproc->basesize;
	if ( basesize < 0 ) { basesize = varproc->GetSize( ptr ); }
	StackPush( tflag, (char *)ptr, basesize );
}


void PushDefault( void )
{
	StackPushTypeVal( HSPVAR_FLAG_DEFAULT, (int)'?', 0 );
}


void PushFuncEnd( void )
{
	StackPushTypeVal( HSPVAR_FLAG_MARK, (int)')', 0 );
}


void PushExtvar( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	ptr = (char *)extsysvar_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushIntfunc( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	ptr = (char *)intfunc_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																	// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushSysvar( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	ptr = (char *)sysvar_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	//StackPop();																// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushModcmd( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	prmstacks = pnum;
	ptr = (char *)modfunc_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																	// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushDllfunc( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	prmstacks = pnum;
	ptr = (char *)dllfunc_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																	// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushDllctrl( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	prmstacks = pnum;
	ptr = (char *)dllctrl_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																	// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushUsrfunc( int val, int pnum )
{
	char *ptr;
	int resflag;
	int basesize;

	*c_type = TYPE_MARK;
	*c_val = '(';
	prmstacks = pnum;
	ptr = (char *)usrfunc_info->reffunc( &resflag, val );						// タイプごとの関数振り分け
	StackPop();																	// PushFuncEndを取り除く
	//code_next();
	if ( resflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
	} else {
		basesize = HspVarCoreGetProc( resflag )->GetSize( (PDAT *)ptr );
		StackPush( resflag, ptr, basesize );
	}
}


void PushVarOffset( PVal *pval )
{
	//	変数のoffset値をpushする
	//
	StackPushi( pval->offset );
}


void CalcAddI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival += stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->AddI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcSubI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival -= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->SubI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcMulI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival *= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->MulI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcDivI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		if ( stm2->ival == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		stm1->ival /= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->DivI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcModI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		if ( stm2->ival == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		stm1->ival %= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->ModI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcAndI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival &= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->AndI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcOrI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival |= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->OrI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcXorI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival ^= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->XorI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcEqI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival == stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->EqI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcNeI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival != stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->NeI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcGtI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival > stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->GtI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcLtI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival < stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->LtI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcGtEqI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival >= stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->GtEqI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcLtEqI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival = ( stm1->ival <= stm2->ival );
		StackDecLevel;
		return;
	}
	varproc->LtEqI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcRrI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival >>= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->RrI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}


void CalcLrI( void )
{
	char *ptr;
	ptr = PrepareCalc();
	if ( ptr == NULL ) {									// 高速化のため
		stm1->ival <<= stm2->ival;
		StackDecLevel;
		return;
	}
	varproc->LrI( (PDAT *)mpval->pt, ptr );
	AfterCalc();
}



void VarSet( PVal *m_pval, int aval )
{
	//	変数代入(var=???)
	//		aval=配列要素のスタック数
	//
	int chk;
	PVal *pval;
	HspVarProc *proc;
	APTR aptr;
	void *ptr;
	PDAT *dst;
	//int pleft;
	//int baseaptr;
	int tflag;

	if ( m_pval == &var_proxy ) {
		STMDATA *stm = (STMDATA *)m_pval->master;
		pval = (PVal *)( stm->pval );
		aptr = stm->ival;
		if ( aval != 0 ) throw HSPERR_SYNTAX;
	} else {
		pval = m_pval;
		aptr = CheckArray( pval, aval );
	}
	dst = HspVarCorePtrAPTR( pval, aptr );
	tflag = pval->flag;

	chk = code_get();									// パラメーター値を取得
	if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }

	ptr = mpval->pt;

	if ( tflag != mpval->flag ) {

		proc = HspVarCoreGetProc( tflag );
		if ( pval->support & HSPVAR_SUPPORT_NOCONVERT ) {	// 型変換なしの場合
			if ( arrayobj_flag ) {
				proc->ObjectWrite( pval, ptr, mpval->flag );
				return;
			}
		}
		if ( aptr != 0 ) throw HSPERR_INVALID_ARRAYSTORE;	// 型変更の場合は配列要素0のみ
		HspVarCoreClear( pval, mpval->flag );		// 最小サイズのメモリを確保
		proc = HspVarCoreGetProc( pval->flag );
		dst = proc->GetPtr( pval );					// PDATポインタを取得

	} else {
		if ( tflag == HSPVAR_FLAG_INT ) {
			*(int *)dst = *(int *)ptr;
			return;
		}
		proc = HspVarCoreGetProc( tflag );
	}
	proc->Set( pval, dst, ptr );
}


void VarSet( PVal *m_pval, int aval, int pnum )
{
	//	変数代入(var=???)
	//		aval=配列要素のスタック数
	//		pnum=パラメーターのスタック数
	//
	int chk;
	HspVarProc *proc;
	APTR aptr;
	void *ptr;
	PDAT *dst;
	int pleft;
	int baseaptr;
	PVal *pval;

	if ( m_pval == &var_proxy ) {
		STMDATA *stm = (STMDATA *)m_pval->master;
		pval = (PVal *)( stm->pval );
		aptr = stm->ival;
		if ( aval != 0 ) throw HSPERR_SYNTAX;
	} else {
		pval = m_pval;
		aptr = CheckArray( pval, aval );
	}
	dst = HspVarCorePtrAPTR( pval, aptr );
	proc = HspVarCoreGetProc( pval->flag );

	chk = code_get();									// パラメーター値を取得
	if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }

	ptr = mpval->pt;
	if ( pval->flag != mpval->flag ) {

		if ( pval->support & HSPVAR_SUPPORT_NOCONVERT ) {	// 型変換なしの場合
			if ( arrayobj_flag ) {
				proc->ObjectWrite( pval, ptr, mpval->flag );
				return;
			}
		}
		if ( aptr != 0 ) throw HSPERR_INVALID_ARRAYSTORE;	// 型変更の場合は配列要素0のみ
		HspVarCoreClear( pval, mpval->flag );		// 最小サイズのメモリを確保
		proc = HspVarCoreGetProc( pval->flag );
		dst = proc->GetPtr( pval );					// PDATポインタを取得
	}
	proc->Set( pval, dst, ptr );

	if ( pnum < 2 ) return;

	//	複数パラメーターがある場合
	//
	pleft = pnum - 1;
	chk = pval->len[1];
	if ( chk == 0 ) baseaptr = aptr; else baseaptr = aptr % chk;
	aptr -= baseaptr;

	while(1) {
		if ( pleft == 0 ) break;

		chk = code_get();							// パラメーター値を取得
		if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }
		if ( pval->flag != mpval->flag ) {
				throw HSPERR_INVALID_ARRAYSTORE;	// 型変更はできない
		}
		ptr = mpval->pt;
		baseaptr++;

		pval->arraycnt = 0;							// 配列指定カウンタをリセット
		pval->offset = aptr;
		code_arrayint2( pval, baseaptr );			// 配列チェック

		dst = HspVarCorePtr( pval );
		proc->Set( pval, dst, ptr );				// 次の配列にたたき込む
		pleft--;
	}
}


void VarSet2( PVal *pval )
{
	//	変数代入(連続代入用)(var=???)
	//
	int chk;
	HspVarProc *proc;
	//APTR aptr;
	void *ptr;
	PDAT *dst;
	int baseaptr;
	STMDATA *stm;
	int *iptr;
	int tflag;

	proc = HspVarCoreGetProc( pval->flag );

	chk = code_get();							// パラメーター値を取得
	if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }
	if ( pval->flag != mpval->flag ) {
			throw HSPERR_INVALID_ARRAYSTORE;	// 型変更はできない
	}
	ptr = mpval->pt;

	stm = StackPeek;
	tflag = stm->type;
	if ( tflag != HSPVAR_FLAG_INT ) { throw HSPERR_SYNTAX; }
	iptr = (int *)STM_GETPTR(stm);
	baseaptr = *iptr;							// PushVarOffsetであらかじめ入れてあるoffset値
	StackDecLevel;
	code_next();

	baseaptr++;

	pval->arraycnt = 0;							// 配列指定カウンタをリセット
	pval->offset = 0;
	code_arrayint2( pval, baseaptr );			// 配列チェック

	dst = HspVarCorePtr( pval );
	proc->Set( pval, dst, ptr );				// 次の配列にたたき込む
}


void VarInc( PVal *m_pval, int aval )
{
	//	変数インクリメント(var++)
	//
	HspVarProc *proc;
	APTR aptr;
	int incval;
	void *ptr;
	PDAT *dst;
	PVal *pval;

	if ( m_pval == &var_proxy ) {
		STMDATA *stm = (STMDATA *)m_pval->master;
		pval = (PVal *)( stm->pval );
		aptr = stm->ival;
		if ( aval != 0 ) throw HSPERR_SYNTAX;
	} else {
		pval = m_pval;
		aptr = CheckArray( pval, aval );
	}
	proc = HspVarCoreGetProc( pval->flag );

	incval = 1;
	if ( pval->flag == HSPVAR_FLAG_INT ) { ptr = &incval; } else {
		ptr = (int *)proc->Cnv( &incval, HSPVAR_FLAG_INT );	// 型がINTでない場合は変換
	}
	dst = HspVarCorePtrAPTR( pval, aptr );
	proc->AddI( dst, ptr );
}


void VarDec( PVal *m_pval, int aval )
{
	//	変数デクリメント(var--)
	//
	HspVarProc *proc;
	APTR aptr;
	int incval;
	void *ptr;
	PDAT *dst;
	PVal *pval;

	if ( m_pval == &var_proxy ) {
		STMDATA *stm = (STMDATA *)m_pval->master;
		pval = (PVal *)( stm->pval );
		aptr = stm->ival;
		if ( aval != 0 ) throw HSPERR_SYNTAX;
	} else {
		pval = m_pval;
		aptr = CheckArray( pval, aval );
	}
	proc = HspVarCoreGetProc( pval->flag );

	incval = 1;
	if ( pval->flag == HSPVAR_FLAG_INT ) { ptr = &incval; } else {
		ptr = (int *)proc->Cnv( &incval, HSPVAR_FLAG_INT );	// 型がINTでない場合は変換
	}
	dst = HspVarCorePtrAPTR( pval, aptr );
	proc->SubI( dst, ptr );
}


static inline int calcprmf( int mval, int exp, int p )
{
	//		Caluculate parameter args (int)
	//
	switch(exp) {
	case CALCCODE_ADD:
		return mval + p;
	case CALCCODE_SUB:
		return mval - p;
	case CALCCODE_MUL:
		return mval * p;
	case CALCCODE_DIV:
		if ( p == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		return mval / p;
	case CALCCODE_MOD:						// '%'
		if ( p == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		return mval % p;

	case CALCCODE_AND:
		return mval & p;
		break;
	case CALCCODE_OR:
		return mval | p;
	case CALCCODE_XOR:
		return mval ^ p;

	case CALCCODE_EQ:
		return (mval==p);
	case CALCCODE_NE:
		return (mval!=p);
	case CALCCODE_GT:
		return (mval>p);
	case CALCCODE_LT:
		return (mval<p);
	case CALCCODE_GTEQ:						// '>='
		return (mval>=p);
	case CALCCODE_LTEQ:						// '<='
		return (mval<=p);

	case CALCCODE_RR:						// '>>'
		return mval >> p;
	case CALCCODE_LR:						// '<<'
		return mval << p;

	case '(':
		throw HSPERR_INVALID_ARRAY;
	default:
		throw HSPVAR_ERROR_INVALID;
	}
}


void VarCalc( PVal *m_pval, int aval, int op )
{
	//	変数演算代入(var*=???等)
	//		aval=配列要素のスタック数
	//		op=演算子コード
	//
	int chk;
	HspVarProc *proc;
	APTR aptr;
	void *ptr;
	PDAT *dst;
	int *iptr;
	PVal *pval;

	if ( m_pval == &var_proxy ) {
		STMDATA *stm = (STMDATA *)m_pval->master;
		pval = (PVal *)( stm->pval );
		aptr = stm->ival;
		if ( aval != 0 ) throw HSPERR_SYNTAX;
	} else {
		pval = m_pval;
		aptr = CheckArray( pval, aval );
	}

	proc = HspVarCoreGetProc( pval->flag );
	dst = HspVarCorePtrAPTR( pval, aptr );

	chk = code_get();									// パラメーター値を取得
	if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }

	ptr = mpval->pt;
	if ( pval->flag != mpval->flag ) {					// 型が一致しない場合は変換
		ptr = HspVarCoreCnvPtr( mpval, pval->flag );
	}
	if ( pval->flag == HSPVAR_FLAG_INT ) {
		iptr = (int *)dst;
		*iptr = calcprmf( *iptr, op, *(int *)ptr );
		return;
	}

	switch(op) {
	case CALCCODE_ADD:
		proc->AddI( dst, ptr );
		break;
	case CALCCODE_SUB:
		proc->SubI( dst, ptr );
		break;
	case CALCCODE_MUL:
		proc->MulI( dst, ptr );
		break;
	case CALCCODE_DIV:
		proc->DivI( dst, ptr );
		break;
	case CALCCODE_MOD:						// '%'
		proc->ModI( dst, ptr );
		break;

	case CALCCODE_AND:
		proc->AndI( dst, ptr );
		break;
	case CALCCODE_OR:
		proc->OrI( dst, ptr );
		break;
	case CALCCODE_XOR:
		proc->XorI( dst, ptr );
		break;

	case CALCCODE_EQ:
		proc->EqI( dst, ptr );
		break;
	case CALCCODE_NE:
		proc->NeI( dst, ptr );
		break;
	case CALCCODE_GT:
		proc->GtI( dst, ptr );
		break;
	case CALCCODE_LT:
		proc->LtI( dst, ptr );
		break;
	case CALCCODE_GTEQ:						// '>='
		proc->GtEqI( dst, ptr );
		break;
	case CALCCODE_LTEQ:						// '<='
		proc->LtEqI( dst, ptr );
		break;

	case CALCCODE_RR:						// '>>'
		proc->RrI( dst, ptr );
		break;
	case CALCCODE_LR:						// '<<'
		proc->LrI( dst, ptr );
		break;
	case '(':
		throw HSPERR_INVALID_ARRAY;
	default:
		throw HSPVAR_ERROR_INVALID;
	}

	if ( proc->aftertype != pval->flag ) {				// 演算後に型が変わる場合
		throw HSPERR_TYPE_MISMATCH;
	}
}


void PushFuncPrm( int num )
{
	//		引数(num)をスタックにpushする
	//
	STMDATA *stm;
	int tflag, basesize;
	char *ptr;
	//HspVarProc *proc;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;

	tflag = stm->type;
	if ( tflag == HSPVAR_FLAG_VAR ) {
		PushVarFromVAP( (PVal *)( stm->pval ), stm->ival );
		//PushVAP( (PVal *)( stm->ival ), *(int *)stm->itemp );
		return;
	}

	ptr = stm->ptr;
	varproc = HspVarCoreGetProc( tflag );
	basesize = varproc->basesize;
	if ( basesize < 0 ) { basesize = varproc->GetSize( (PDAT *)ptr ); }
	StackPush( tflag, ptr, basesize );
}


void PushFuncPrmI( int num )
{
	//		引数(num)をスタックにpushする(int)
	//
	STMDATA *stm;
	int tflag;
	int i_val;
	int *ptr;
	PVal *pval;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) {
		StackPushi( 0 ); return;
	}
	stm -= num;

	tflag = stm->type;
	if ( tflag == HSPVAR_FLAG_VAR ) {
		pval = (PVal *)( stm->pval );
		tflag = pval->flag;
		ptr = (int *)HspVarCorePtrAPTR( pval, stm->ival );
	} else {
		ptr = (int *)stm->ptr;
	}

	if ( tflag != TYPE_INUM ) {
		if ( tflag != TYPE_DNUM ) throw HSPERR_TYPE_MISMATCH;
		i_val = (int)*(double *)ptr;
		ptr = &i_val;
	}
	StackPushi( *ptr );
}


void PushFuncPrmD( int num )
{
	//		引数(num)をスタックにpushする(double)
	//
	STMDATA *stm;
	int tflag;
	double d_val;
	double *ptr;
	PVal *pval;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) {
		d_val = 0.0;
		StackPush( TYPE_DNUM, (char *)&d_val, sizeof(double) );
		return;
	}
	stm -= num;

	tflag = stm->type;
	if ( tflag == HSPVAR_FLAG_VAR ) {
		pval = (PVal *)( stm->pval );
		tflag = pval->flag;
		ptr = (double *)HspVarCorePtrAPTR( pval, stm->ival );
	} else {
		ptr = (double *)stm->ptr;
	}

	if ( tflag != TYPE_DNUM ) {
		if ( tflag != TYPE_INUM ) throw HSPERR_TYPE_MISMATCH;
		d_val = (double)*(int *)ptr;
		ptr = &d_val;
	}
	StackPush( TYPE_DNUM, (char *)ptr, sizeof(double) );
}


void PushFuncPrm( int num, int aval )
{
	//		ローカル変数の引数(num)をスタックにpushする
	//		(PushVar相当)
	STMDATA *stm;
	int tflag;
	PVal *pval;
	int basesize;
	APTR aptr;
	PDAT *ptr;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;
	tflag = stm->type;

	switch( tflag ) {
	case TYPE_EX_LOCAL_VARS:
		pval = (PVal *)( stm->ptr );
		break;
	case HSPVAR_FLAG_VAR:
		pval = (PVal *)( stm->pval );
		break;
	default:
		throw HSPVAR_ERROR_INVALID;
	}
	if ( aval == 0 ) {
		aptr = stm->ival;
	} else {
		aptr = CheckArray( pval, aval );
	}
	ptr = HspVarCorePtrAPTR( pval, aptr );

	tflag = pval->flag;
	if ( tflag == HSPVAR_FLAG_INT ) {
		StackPushi( *(int *)ptr );
		return;
	}

	varproc = HspVarCoreGetProc( tflag );
	basesize = varproc->basesize;
	if ( basesize < 0 ) { basesize = varproc->GetSize( ptr ); }
	StackPush( tflag, (char *)ptr, basesize );
}


void PushFuncPAP( int num, int aval )
{
	//		ローカル変数の引数(num)をスタックにpushする
	//		(PushVAP相当)
	STMDATA *stm;
	int tflag;
	PVal *pval;
	APTR aptr;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;
	tflag = stm->type;

	switch( tflag ) {
	case TYPE_EX_LOCAL_VARS:
		pval = (PVal *)( stm->ptr );
		aptr = CheckArray( pval, aval );
		//StackPushTypeVal( HSPVAR_FLAG_VAR, (int)(size_t)pval, (int)(size_t)aptr );
        StackPushVar( pval, aptr );
		break;
	case HSPVAR_FLAG_VAR:
		if ( aval == 0 ) {
			pval = (PVal *)( stm->pval );
			aptr = stm->ival;
		} else {
			pval = (PVal *)( stm->pval );
			aptr = CheckArray( pval, aval );
		}
		//StackPushTypeVal( HSPVAR_FLAG_VAR, (int)(size_t)pval, (int)(size_t)aptr );
        StackPushVar( pval, aptr );
		break;
	default:
		throw HSPVAR_ERROR_INVALID;
	}
}


PVal *FuncPrmVA( int num )
{
	//		変数の引数(num)を得る(var用)
	//
	STMDATA *stm;
	int tflag;
	//char *ptr;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;

	tflag = stm->type;
	if ( tflag != HSPVAR_FLAG_VAR ) throw HSPVAR_ERROR_INVALID;

	//ptr = stm->itemp;

	var_proxy.master = stm;
	return &var_proxy;

	//return (PVal *)( stm->ival );
}


PVal *FuncPrm( int num )
{
	//		変数の引数(num)を得る(array用)
	//
	STMDATA *stm;
	int tflag;
	//char *ptr;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;

	tflag = stm->type;
	if ( tflag != HSPVAR_FLAG_VAR ) throw HSPVAR_ERROR_INVALID;

	//ptr = stm->itemp;
	return (PVal *)( stm->pval );
}


PVal *LocalPrm( int num )
{
	//		ローカル変数の引数(num)を得る
	//
	STMDATA *stm;
	int tflag;

	stm = (STMDATA *)hspctx->prmstack;
	if ( stm == NULL ) throw HSPERR_INVALID_FUNCPARAM;
	if ( num >= hspctx->prmstack_max ) throw HSPERR_INVALID_FUNCPARAM;
	stm -= num;

	tflag = stm->type;
	if ( tflag != TYPE_EX_LOCAL_VARS ) throw HSPVAR_ERROR_INVALID;

	return (PVal *)( stm->ptr );
}


/*------------------------------------------------------------*/
/*
		Program Control Process
*/
/*------------------------------------------------------------*/


void TaskSwitch( int label )
{
	//		次のタスク関数をセット
	//		(label=タスク関数ID)
	//
#if 0
	{
	//char ss[128];
	//sprintf( ss,"[%d]\n",label );
	//DebugMsg( ss );
	Alertf( "[%d]\n",label );
	}
#endif

	lasttask = label;
	curtask = __HspTaskFunc[label];
}

void TaskExec( void )
{
	//		セットされたタスク関数を実行する
	//
	curtask();
}


int GetTaskID( void )
{
	//		タスク関数IDを取得
	//
	return lasttask;
}


/*------------------------------------------------------------*/
/*
		Normal HSP Process
*/
/*------------------------------------------------------------*/

void HspPostExec( void )
{
	//		コマンド実行後の処理
	//
#if 0
	if ( hspctx->runmode == RUNMODE_RETURN ) {
		//cmdfunc_return();
	} else {
		hspctx->msgfunc( hspctx );
	}
#endif
}

bool HspIf( void )
{
	//		スタックから値を取り出して真偽を返す
	//		(真偽が逆になっているので注意)
	//
	int i;
	i = code_geti();
	return (i==0);
}


void Extcmd( int cmd, int pnum )
{
	//if ( extcmd_info->cmdfunc( cmd ) ) HspPostExec();
	if ( extcmd_func( cmd ) ) HspPostExec();
}


void Modcmd( int cmd, int pnum )
{
	//int i;
	//if ( modfunc_info->cmdfunc( cmd ) ) HspPostExec();
	//Alertf("CMD=%d (lev%d)", cmd, StackGetLevel );
	prmstacks = pnum;							// hsp3codeに渡すパラメーター数
	if ( modcmd_func( cmd ) ) HspPostExec();
}


void Dllfunc( int cmd, int pnum )
{
	if ( dllcmd_func( cmd ) ) HspPostExec();
}


void Dllctrl( int cmd, int pnum )
{
	if ( dllctl_func( cmd ) ) HspPostExec();
}


void Usrfunc( int cmd, int pnum )
{
	if ( usrcmd_func( cmd ) ) HspPostExec();
}


void Prgcmd( int cmd, int pnum )
{
	//if ( progfunc_info->cmdfunc( cmd ) ) HspPostExec();
	if ( progcmd_func( cmd ) ) HspPostExec();
}


void Intcmd( int cmd, int pnum )
{
	//if ( intcmd_info->cmdfunc( cmd ) ) HspPostExec();
	if ( intcmd_func( cmd ) ) HspPostExec();
}

/*------------------------------------------------------------*/
/*
		For Debug
*/
/*------------------------------------------------------------*/

void DebugStackPeek( void )
{
	STMDATA *stm;
	char s1[256];
	char dbg[4096];
	char *p;
	int i;
	i = 0;
	p = dbg; *p = 0;
	stm = mem_stm;
	while(1) {
		if ( stm >= stm_cur ) break;
		sprintf( s1, "STM#%d type:%d mode:%d ival:%d\r\n", i, stm->type, stm->mode, stm->ival );
		i++;
		strcpy( p, s1 );
		p+=strlen(s1);
		stm++;
	}
	//Alertf( "%s", dbg );
}

void DebugMsg( char *msg )
{
#ifdef HSPWIN
	OutputDebugString( msg );
	//Alertf( "%s", msg );
#else
	Alertf( "%s", msg );
#endif
}

