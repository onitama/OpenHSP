
//
//	HSP3 code manager
//	(中間言語展開およびパラメーター取得)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hspwnd.h"
#include "supio.h"
#include "dpmread.h"
#include "stack.h"
#include "strbuf.h"
#include "hsp3code.h"
#include "hsp3debug.h"
#include "hsp3config.h"
#include "hsp3int.h"

#define strp(dsptr) &hspctx->mem_mds[dsptr]

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HSP3TYPEINFO *hsp3tinfo;	// HSP3 type info structure (strbuf)
static int tinfo_cur;			// Current type info ID
#define GetTypeInfoPtr( type ) (&hsp3tinfo[type])

static HSPCTX *hspctx;			// Current Context
static unsigned short *mcs;		// Current PC ptr
static unsigned short *mcsbak;
static int val,type,exflg;
static short csvalue, csvalue2;
static int hspevent_opt;		// Event enable flag
static MPModVarData modvar_init;
static int sptr_res;
static int arrayobj_flag;

static HSPEXINFO mem_exinfo;	// HSPEXINFO本体

#ifdef HSPDEBUG
static HSP3DEBUG dbginfo;
static int dbgmode;
#endif

PVal *plugin_pval;								// プラグインに渡される変数ポインタの実態
PVal *mpval;									// code_getで使用されたテンポラリ変数
static PVal *mpval_int;							// code_getで使用されたテンポラリ変数(int用)
static PVal prmvar;								// パラメーターテンポラリ変数の実態

static	unsigned char *mem_di_val;				// Debug VALS info ptr
static	int srcname;
static	int funcres;							// 関数の戻り値型

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/


static int getU32(unsigned short *mcs) {
	return (mcs[1] << 16) | (mcs[0]);
}

static inline void __code_next( void )
{
	//		Get 1 command block
	//		(ver3.0以降用)
	//
//	register unsigned short csvalue;
	mcsbak = mcs;
	csvalue = *mcs++;
	exflg = csvalue & (EXFLG_1|EXFLG_2);
	type = csvalue & CSTYPE;
	if ( csvalue & EXFLG_3 ) {
		//	 32bit val code
		//
		val = getU32(mcs);
		mcs+=2;
//		printf( "%08x | type[%d] val[%d] ex[%d]\n",(int)(mcs-hspctx->mem_mcs), type,val,exflg );
		return;
	}
	// 16bit val code
	val = (int)(*mcs++);

//	printf( "%08x : type[%d] val[%d] ex[%d]\n",(int)(mcs-hspctx->mem_mcs), type,val,exflg );
}


void code_next( void )
{
	__code_next();
}


void code_puterror( HSPERROR error )
{
	//		エラー例外を発生させる
	//
	if ( error == HSPERR_NONE ) {
		hspctx->runmode = RUNMODE_END;
		return;
	}
	throw error;
}


int code_getexflg( void )
{
	return exflg;
}


static inline void calcprm( HspVarProc *proc, PDAT *pval, int exp, void *ptr )
{
	//		Caluculate parameter args (valiant)
	//
	switch(exp) {
	case CALCCODE_ADD:
		proc->AddI( pval, ptr );
		break;
	case CALCCODE_SUB:
		proc->SubI( pval, ptr );
		break;
	case CALCCODE_MUL:
		proc->MulI( pval, ptr );
		break;
	case CALCCODE_DIV:
		proc->DivI( pval, ptr );
		break;
	case CALCCODE_MOD:						// '%'
		proc->ModI( pval, ptr );
		break;

	case CALCCODE_AND:
		proc->AndI( pval, ptr );
		break;
	case CALCCODE_OR:
		proc->OrI( pval, ptr );
		break;
	case CALCCODE_XOR:
		proc->XorI( pval, ptr );
		break;

	case CALCCODE_EQ:
		proc->EqI( pval, ptr );
		break;
	case CALCCODE_NE:
		proc->NeI( pval, ptr );
		break;
	case CALCCODE_GT:
		proc->GtI( pval, ptr );
		break;
	case CALCCODE_LT:
		proc->LtI( pval, ptr );
		break;
	case CALCCODE_GTEQ:						// '>='
		proc->GtEqI( pval, ptr );
		break;
	case CALCCODE_LTEQ:						// '<='
		proc->LtEqI( pval, ptr );
		break;

	case CALCCODE_RR:						// '>>'
		proc->RrI( pval, ptr );
		break;
	case CALCCODE_LR:						// '<<'
		proc->LrI( pval, ptr );
		break;
	case '(':
		throw HSPERR_INVALID_ARRAY;
	default:
		throw HSPVAR_ERROR_INVALID;
	}
}


static inline void calcprmf( int &mval, int exp, int p )
{
	//		Caluculate parameter args (int)
	//
	switch(exp) {
	case CALCCODE_ADD:
		mval += p;
		break;
	case CALCCODE_SUB:
		mval -= p;
		break;
	case CALCCODE_MUL:
		mval *= p;
		break;
	case CALCCODE_DIV:
		if ( p == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		mval /= p;
		break;
	case CALCCODE_MOD:						// '%'
		if ( p == 0 ) throw( HSPVAR_ERROR_DIVZERO );
		mval %= p;
		break;

	case CALCCODE_AND:
		mval &= p;
		break;
	case CALCCODE_OR:
		mval |= p;
		break;
	case CALCCODE_XOR:
		mval ^= p;
		break;

	case CALCCODE_EQ:
		mval = (mval==p);
		break;
	case CALCCODE_NE:
		mval = (mval!=p);
		break;
	case CALCCODE_GT:
		mval = (mval>p);
		break;
	case CALCCODE_LT:
		mval = (mval<p);
		break;
	case CALCCODE_GTEQ:						// '>='
		mval = (mval>=p);
		break;
	case CALCCODE_LTEQ:						// '<='
		mval = (mval<=p);
		break;

	case CALCCODE_RR:						// '>>'
		mval >>= p;
		break;
	case CALCCODE_LR:						// '<<'
		mval <<= p;
		break;

	case '(':
		throw HSPERR_INVALID_ARRAY;
	default:
		throw HSPVAR_ERROR_INVALID;
	}
}


static void inline code_calcop( int op )
{
	//		スタックから引数を２つPOPしたものを演算する
	//
	HspVarProc *varproc;
	STMDATA *stm1;
	STMDATA *stm2;
	char *ptr;
	int tflag;
	int basesize;

	stm2 = StackPeek;
	//stm1 = stm2-1;
	stm1 = StackPeek2;
	tflag = stm1->type;

	//if ( tflag > HSP3_FUNC_MAX ) throw HSPERR_UNKNOWN_CODE;
	//Alertf( "(%d) %d %d %d",tflag, stm1->ival, op, stm2->ival );

	if ( tflag == HSPVAR_FLAG_INT ) {
		if ( stm2->type == HSPVAR_FLAG_INT ) {					// HSPVAR_FLAG_INT のみ高速化
			calcprmf( stm1->ival, op, stm2->ival );				// 高速化された演算(intのみ)
			StackDecLevel;										// stack->Pop() の代わり(高速に)
			stm2->ival = stm1->ival;							// １段目スタックの値を入れ替える
			return;
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

	ptr = STM_GETPTR(stm2);
	if ( tflag != stm2->type ) {								// 型が一致しない場合は変換
		if ( stm2->type >= HSPVAR_FLAG_USERDEF ) {
			ptr = (char *)HspVarCoreGetProc(stm2->type)->CnvCustom( ptr, tflag );
		} else {
			ptr = (char *)varproc->Cnv( ptr, stm2->type );
		}
	}
	calcprm( varproc, (PDAT *)mpval->pt, op, ptr );				// 計算を行なう
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


static void code_checkarray( PVal *pval )
{
	//		Check PVal Array information
	//		(配列要素(int)の取り出し)
	//
	int chk,i;
	PVal temp;
	HspVarCoreReset( pval );										// 配列ポインタをリセットする

	if ( type == TYPE_MARK ) {
		if ( val == '(' ) {
			code_next();
			//			整数値のみサポート
			while(1) {
				HspVarCoreCopyArrayInfo( &temp, pval );			// 状態を保存
				chk = code_get();								// パラメーターを取り出す
				if ( chk<=PARAM_END ) { throw HSPERR_BAD_ARRAY_EXPRESSION; }
				if ( mpval->flag != HSPVAR_FLAG_INT ) { throw HSPERR_TYPE_MISMATCH; }
				HspVarCoreCopyArrayInfo( pval, &temp );			// 状態を復帰
				i = *(int *)(mpval->pt);
				HspVarCoreArray( pval, i );						// 配列要素指定(整数)
				if ( chk == PARAM_SPLIT ) break;
			}
			code_next();											// ')'を読み飛ばす
			return;
		}
	}
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


static void code_checkarray2( PVal *pval )
{
	//		Check PVal Array information
	//		(配列要素(int)の取り出し)(配列の拡張に対応)
	//
	int chk,i;
	PVal temp;
	HspVarCoreReset( pval );									// 配列ポインタをリセットする

	if ( type == TYPE_MARK ) {
		if ( val == '(' ) {
			code_next();
			//			整数値のみサポート
			while(1) {
				HspVarCoreCopyArrayInfo( &temp, pval );			// 状態を保存
				chk = code_get();								// パラメーターを取り出す
				if ( chk<=PARAM_END ) { throw HSPERR_BAD_ARRAY_EXPRESSION; }
				if ( mpval->flag != HSPVAR_FLAG_INT ) { throw HSPERR_TYPE_MISMATCH; }
				HspVarCoreCopyArrayInfo( pval, &temp );			// 状態を復帰
				i = *(int *)(mpval->pt);
				code_arrayint2( pval, i );
				if ( chk == PARAM_SPLIT ) break;
			}
			code_next();											// ')'を読み飛ばす
			return;
		}
	}
}


static char *code_checkarray_obj( PVal *pval, int *mptype )
{
	//		Check PVal Array object information
	//		( 配列要素(オブジェクト)の取り出し )
	//		( 返値 : 汎用データのポインタ )
	//		( mptype : 汎用データのタイプを返す )
	//
	char *ptr;
	HspVarProc *varproc;
/*
	FlexValue *fv;
	if ( pval->support & HSPVAR_SUPPORT_STRUCTACCEPT ) {			// 構造体受け付け
		code_checkarray( pval );
		if ( pval->support & HSPVAR_SUPPORT_CLONE ) {				// クローンのチェック
			fv = (FlexValue *)HspVarCorePtr( pval );
			*mptype = fv->clonetype;
			return (fv->ptr);
		}
		if (( type != TYPE_STRUCT )||( exflg )) {
			*mptype = MPTYPE_VAR;
			return HspVarCorePtr( pval );
		}
		varproc = HspVarCoreGetProc( pval->flag );
		ptr = varproc->ArrayObject( pval, mptype );
		return ptr;
	}

	if ( pval->support & HSPVAR_SUPPORT_CLONE ) {				// クローンのチェック
		fv = (FlexValue *)HspVarCorePtr( pval );
		*mptype = fv->clonetype;
		return (fv->ptr);
	}
*/

	*mptype = pval->flag;
	HspVarCoreReset( pval );										// 配列ポインタをリセットする

	if ( type == TYPE_MARK ) {
		if ( val == '(' ) {											// 配列がある場合
			code_next();
//			if ( pval->support & HSPVAR_SUPPORT_ARRAYOBJ ) {
				varproc = HspVarCoreGetProc( pval->flag );
				ptr = (char *)varproc->ArrayObjectRead( pval, mptype );
				code_next();											// ')'を読み飛ばす
				return ptr;
//			}
//			code_checkarray( pval );
		}
	}
	return (char *)HspVarCorePtr( pval );
}


/*
static char *code_get_varsub( PVal *pval, int *restype )
{
	//		pvalの実体を検索する(HSPVAR_SUPPORT_ARRAYOBJ時のみ)
	//		( 返値が実体ポインタとなる )
	//
	char *ptr;
	ptr = (char *)code_checkarray_obj( pval, restype );
	return code_get_proxyvar( ptr, restype );
}
*/


char *code_get_proxyvar( char *ptr, int *mptype )
{
	//		マルチパラメーターの変数を処理する
	//
	MPVarData *var;
	PVal *getv_pval;
	switch( *mptype ) {										// マルチパラメーターを取得
	case MPTYPE_SINGLEVAR:
		var = (MPVarData *)ptr;
		getv_pval = var->pval;
		getv_pval->offset = var->aptr;
		if ( type == TYPE_MARK ) if ( val == '(' ) throw HSPERR_INVALID_ARRAY;
		//HspVarCoreReset( getv_pval );
		break;
	case MPTYPE_LOCALSTRING:
		*mptype = MPTYPE_STRING;
		return *(char **)ptr;
	case MPTYPE_LABEL:
		*mptype = HSPVAR_FLAG_LABEL;
		return ptr;
	case MPTYPE_ARRAYVAR:
		var = (MPVarData *)ptr;
		getv_pval = var->pval;
		if ( getv_pval->support & HSPVAR_SUPPORT_MISCTYPE ) {
			return code_checkarray_obj( getv_pval, mptype );
		} else {
			code_checkarray( getv_pval );
		}
		break;
	case MPTYPE_LOCALVAR:
		getv_pval = (PVal *)ptr;
		if ( getv_pval->support & HSPVAR_SUPPORT_MISCTYPE ) {
			return code_checkarray_obj( getv_pval, mptype );
		} else {
			code_checkarray( getv_pval );
		}
		break;
	default:
		return ptr;
	}
	*mptype = getv_pval->flag;
	return (char *)HspVarCorePtr( getv_pval );
}


static void code_checkarray_obj2( PVal *pval )
{
	//		Check PVal Array object information
	//		( 配列要素(オブジェクト)の取り出し・変数指定時 )
	//		( 変数の内容を参照する場合にはcode_checkarray_objを使用します )
	//
	HspVarProc *varproc;
	HspVarCoreReset( pval );										// 配列ポインタをリセットする

	arrayobj_flag = 0;
	if ( type == TYPE_MARK ) {
		if ( val == '(' ) {											// 配列がある場合
			code_next();
			varproc = HspVarCoreGetProc( pval->flag );
			varproc->ArrayObject( pval );
			arrayobj_flag = 1;
			code_next();											// ')'を読み飛ばす
		}
	}
}



int code_get( void )
{
	//		parameter analysis
	//			result: 0=ok(PARAM_OK)  -1=end(PARAM_END)  -2=default(PARAM_DEFAULT)
	//					(エラー発生時は例外が発生します)
	//
	STMDATA *stm;
	PVal *argpv;
	HspVarProc *varproc;
	HSP3TYPEINFO *info;
	MPModVarData *var;
	FlexValue *fv;
	char *out;
	STRUCTPRM *prm;
	char *ptr;
	int tflag;
	int basesize;
	int tmpval;
	int stack_def;
	int resval;

	if (exflg&EXFLG_1) return PARAM_END;		// パラメーター終端
	if (exflg&EXFLG_2) {						// パラメーター区切り(デフォルト時)
		exflg^=EXFLG_2;
		return PARAM_DEFAULT;
	}
	if ( type == TYPE_MARK ) {
		if ( val == 63 ) {						// パラメーター省略時('?')
			code_next();
			exflg&=~EXFLG_2;
			return PARAM_DEFAULT;
		}
		if ( val == ')' ) {						// 関数内のパラメーター省略時
			exflg&=~EXFLG_2;
			return PARAM_ENDSPLIT;
		}
	}

	if ( csvalue & EXFLG_0 ) {					// 単一の項目(高速化)
		switch(type) {
		case TYPE_INUM:
			mpval = mpval_int;
			*(int *)mpval->pt =val;
			break;
		case TYPE_DNUM:
		case TYPE_STRING:
			varproc = HspVarCoreGetProc( type );
			mpval = HspVarCoreGetPVal( type );
			if ( mpval->mode == HSPVAR_MODE_NONE ) {					// 型に合わせたテンポラリ変数を初期化
				if ( varproc->flag == 0 ) {
					throw HSPERR_TYPE_INITALIZATION_FAILED;
				}
				HspVarCoreClearTemp( mpval, type );						// 最小サイズのメモリを確保
			}
			varproc->Set( mpval, (PDAT *)(mpval->pt), strp(val) );		// テンポラリ変数に初期値を設定
			break;
		default:
			throw HSPERR_UNKNOWN_CODE;
		}
		code_next();
		exflg&=~EXFLG_2;
		return 0;
	}

	resval = 0;
	stack_def = StackGetLevel;					// スタックのレベルを取得

	while(1) {
		//Alertf( "type%d val%d exflg%d",type,val,exflg );
		//printf( "type%d val%d exflg%d\n",type,val,exflg );

		switch(type) {
		case TYPE_MARK:
			if ( val == ')' ) {					// 引数の終了マーク
				if ( stack_def == StackGetLevel ) { throw HSPERR_WRONG_EXPRESSION; }
				resval = PARAM_SPLIT;
				exflg |= EXFLG_2;
				break;
			}
			code_calcop( val );
			code_next();
			break;
		case TYPE_VAR:
			argpv = &hspctx->mem_var[val];
			code_next();
			tflag = argpv->flag;
			if ( argpv->support & HSPVAR_SUPPORT_MISCTYPE ) {
				ptr = (char *)code_checkarray_obj( argpv, &tflag );
			} else {
				code_checkarray( argpv );
				ptr = (char *)HspVarCorePtr( argpv );
			}
			varproc = HspVarCoreGetProc( tflag );
			basesize = varproc->basesize;
			if ( basesize < 0 ) { basesize = varproc->GetSize( (PDAT *)ptr ); }
			StackPush( tflag, ptr, basesize );
			break;
		case TYPE_INUM:
			StackPushi( val );
			code_next();
			break;
		case TYPE_STRING:
			StackPush( type, strp(val) );
			code_next();
			break;
		case TYPE_DNUM:
			StackPush( type, strp(val), sizeof(double) );
			code_next();
			break;
		case TYPE_STRUCT:
			prm = &hspctx->mem_minfo[val];
			code_next();
			out = ((char *)hspctx->prmstack );
			if ( out == NULL ) throw HSPERR_INVALID_PARAMETER;
			if ( prm->subid != STRUCTPRM_SUBID_STACK ) {
				var = (MPModVarData *)out;
				if (( var->magic != MODVAR_MAGICCODE )||( var->subid != prm->subid )||(var->pval->flag != TYPE_STRUCT )) throw HSPERR_INVALID_STRUCT_SOURCE;
				fv = (FlexValue *)HspVarCorePtrAPTR( var->pval, var->aptr );
				if ( fv->type == FLEXVAL_TYPE_NONE ) throw HSPERR_INVALID_STRUCT_SOURCE;
				out = (char *)fv->ptr;
			}
			out += prm->offset;
			tflag = prm->mptype;
			ptr = (char *)code_get_proxyvar( out, &tflag );
			varproc = HspVarCoreGetProc( tflag );
			basesize = varproc->basesize;
			if ( basesize < 0 ) { basesize = varproc->GetSize( (PDAT *)ptr ); }
			StackPush( tflag, ptr, basesize );
			break;
		case TYPE_LABEL: {
			unsigned short *tmpval = hspctx->mem_mcs + hspctx->mem_ot[val];
			StackPush( HSPVAR_FLAG_LABEL, (char *)&tmpval, sizeof(unsigned short *)  );
			code_next();
			break;
		}
		default:
			//		リダイレクト(reffunc)使用チェック
			//
			info = GetTypeInfoPtr( type );
			if ( info->reffunc == NULL ) {
				throw HSPERR_INVALID_PARAMETER;
			}
			tmpval = val;
			code_next();
			ptr = (char *)info->reffunc( &tflag, tmpval );	// タイプごとの関数振り分け
			basesize = HspVarCoreGetProc( tflag )->GetSize( (PDAT *)ptr );
			StackPush( tflag, ptr, basesize );
			break;
		}

		if ( exflg ) {								// パラメーター終端チェック
			exflg&=~EXFLG_2;
			break;
		}
	}

	stm = StackPeek;
	tflag = stm->type;

	if ( tflag == HSPVAR_FLAG_INT ) {				// int型の場合は直接値を設定する(高速化)
		mpval = mpval_int;
		*(int *)mpval->pt = stm->ival;
	} else {
		varproc = HspVarCoreGetProc( tflag );
		mpval = HspVarCoreGetPVal( tflag );

		if ( mpval->mode == HSPVAR_MODE_NONE ) {					// 型に合わせたテンポラリ変数を初期化
			if ( varproc->flag == 0 ) {
				throw HSPERR_TYPE_INITALIZATION_FAILED;
			}
			HspVarCoreClearTemp( mpval, tflag );									// 最小サイズのメモリを確保
		}
		varproc->Set( mpval, (PDAT *)(mpval->pt), STM_GETPTR(stm) );				// テンポラリ変数に初期値を設定
	}

	StackPop();
	if ( stack_def != StackGetLevel )					{		// スタックが正常に復帰していない
		throw HSPERR_STACK_OVERFLOW;
	}

	return resval;
}


char *code_gets( void )
{
	//		文字列パラメーターを取得
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { throw HSPERR_NO_DEFAULT; }
	if ( mpval->flag != HSPVAR_FLAG_STR ) { throw HSPERR_TYPE_MISMATCH; }
	return (mpval->pt);
}


char *code_getds( const char *defval )
{
	//		文字列パラメーターを取得(デフォルト値あり)
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { return (char *)defval; }
	if ( mpval->flag != HSPVAR_FLAG_STR ) { throw HSPERR_TYPE_MISMATCH; }
	return (mpval->pt);
}

char *code_getas(void)
{
#ifdef HSPUTF8
	HSPCHAR *s;
	HSPAPICHAR *hactmp1 = 0;
	char *actmp1 = 0;
	s = code_gets();
	chartoapichar(s, &hactmp1);
	apichartoansichar(hactmp1, &actmp1);
	freehac(&hactmp1);
	sbCopy(&hspctx->stmp,actmp1,strlen(actmp1)+1);
	freeac(&actmp1);
	return hspctx->stmp;
#else
	return code_gets();
#endif
}

char *code_getads(const char *defval)
{
#ifdef HSPUTF8
	HSPCHAR *s;
	HSPAPICHAR *hactmp1 = 0;
	char *actmp1 = 0;
	s = code_getds(defval);
	if (s == defval){
		return s;
	}
	chartoapichar(s, &hactmp1);
	apichartoansichar(hactmp1, &actmp1);
	freehac(&hactmp1);
	sbCopy(&hspctx->stmp,actmp1,strlen(actmp1)+1);
	freeac(&actmp1);
	return hspctx->stmp;
#else
	return code_getds( defval );
#endif
}

char *code_getdsi( const char *defval )
{
	//		文字列パラメーターを取得(デフォルト値あり・数値も可)
	//
	int chk;
	char *ptr;
	chk = code_get();
	if ( chk<=PARAM_END ) { return (char *)defval; }

	ptr = mpval->pt;
	if ( mpval->flag != HSPVAR_FLAG_STR ) {			// 型が一致しない場合は変換
		//ptr = (char *)HspVarCoreCnv( mpval->flag, HSPVAR_FLAG_STR, ptr );
		ptr = (char *)HspVarCoreCnvPtr( mpval, HSPVAR_FLAG_STR );
	}
	return ptr;
}


int code_geti( void )
{
	//		数値パラメーターを取得
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { throw HSPERR_NO_DEFAULT; }
	if ( mpval->flag != HSPVAR_FLAG_INT ) {
		if ( mpval->flag != HSPVAR_FLAG_DOUBLE ) throw HSPERR_TYPE_MISMATCH;
		return (int)(*(double *)(mpval->pt));		// doubleの時はintに変換
	}
	return *(int *)(mpval->pt);
}


int code_getdi( const int defval )
{
	//		数値パラメーターを取得(デフォルト値あり)
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { return defval; }
	if ( mpval->flag != HSPVAR_FLAG_INT ) {
		if ( mpval->flag != HSPVAR_FLAG_DOUBLE ) throw HSPERR_TYPE_MISMATCH;
		return (int)(*(double *)(mpval->pt));		// doubleの時はintに変換
	}
	return *(int *)(mpval->pt);
}


double code_getd( void )
{
	//		数値(double)パラメーターを取得
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { throw HSPERR_NO_DEFAULT; }
	if ( mpval->flag != HSPVAR_FLAG_DOUBLE ) {
		if ( mpval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		return (double)(*(int *)(mpval->pt));		// intの時はdoubleに変換
	}
	return *(double *)(mpval->pt);
}


double code_getdd( const double defval )
{
	//		数値(double)パラメーターを取得(デフォルト値あり)
	//
	int chk;
	chk = code_get();
	if ( chk<=PARAM_END ) { return defval; }
	if ( mpval->flag != HSPVAR_FLAG_DOUBLE ) {
		if ( mpval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		return (double)(*(int *)(mpval->pt));		// intの時はdoubleに変換
	}
	return *(double *)(mpval->pt);
}


static APTR code_getv_sub( PVal **pval )
{
	//		pvalの実体を検索する(マルチパラメーターの場合があるため)
	//		( 返値が新しいPValポインタとなる )
	//
	PVal *getv_pval;
//	MPVarData *var;
	int mptype;

	mptype = MPTYPE_VAR;
	getv_pval = *pval;
	if ( getv_pval->support & HSPVAR_SUPPORT_MISCTYPE ) {		// 連想配列などの検索
		code_checkarray_obj2( getv_pval );
		return HspVarCoreGetAPTR( getv_pval );
	}
	code_checkarray2( *pval );									// 通常の配列検索(拡張あり)
	return HspVarCoreGetAPTR( getv_pval );
}

/*
static APTR code_getv_sub2( PVal **pval )
{
	//		pvalの実体を検索する(マルチパラメーターの場合があるため)
	//		( 返値が新しいPValポインタとなる )
	//
	PVal *getv_pval;
	MPVarData *var;
	int mptype;

	mptype = MPTYPE_VAR;
	getv_pval = *pval;
	if ( getv_pval->support & HSPVAR_SUPPORT_MISCTYPE ) {		// 連想配列などの検索
		var = (MPVarData *)code_checkarray_obj( getv_pval, &mptype );
		return code_getv_proxy( pval, var, mptype );
	}
	code_checkarray2( *pval );									// 通常の配列検索
	return HspVarCoreGetAPTR( getv_pval );
}
*/

APTR code_getv_proxy( PVal **pval, MPVarData *var, int mptype )
{
	PVal *getv_pval;
	APTR aptr;
	switch( mptype ) {										// マルチパラメーターを取得
	case MPTYPE_VAR:
		return HspVarCoreGetAPTR( *pval );
	case MPTYPE_SINGLEVAR:
		getv_pval = var->pval;
		aptr = var->aptr;
		if ( type == TYPE_MARK ) if ( val == '(' ) throw HSPERR_INVALID_ARRAY;
		break;
	case MPTYPE_ARRAYVAR:
		getv_pval = var->pval;
		aptr = code_getv_sub( &getv_pval );
		break;
	case MPTYPE_LOCALVAR:
		getv_pval = (PVal *)var;
		aptr = code_getv_sub( &getv_pval );
		break;
	default:
		throw HSPERR_VARIABLE_REQUIRED;
	}
	*pval = getv_pval;
	return aptr;
}


static inline APTR code_getva_struct( PVal **pval )
{
	//		置き換えパラメーターを変数の代わりに取得
	//
	MPModVarData *var;
	FlexValue *fv;
	STRUCTPRM *prm;
	APTR aptr;
	char *out;
	int i;

	i = val;
	code_next();
	out = ((char *)hspctx->prmstack );
	if ( out == NULL ) throw HSPERR_INVALID_PARAMETER;

	if ( i == STRUCTCODE_THISMOD ) {						// thismod
		var = (MPModVarData *)out;
		if ( var->magic != MODVAR_MAGICCODE ) throw HSPERR_INVALID_STRUCT_SOURCE;
		*pval = var->pval;
		exflg&=EXFLG_1;
		return var->aptr;
	}

	prm = &hspctx->mem_minfo[i];
	if ( prm->subid != STRUCTPRM_SUBID_STACK ) {
		var = (MPModVarData *)out;
		if (( var->magic != MODVAR_MAGICCODE )||( var->subid != prm->subid )||(var->pval->flag != TYPE_STRUCT )) throw HSPERR_INVALID_STRUCT_SOURCE;
		fv = (FlexValue *)HspVarCorePtrAPTR( var->pval, var->aptr );
		if ( fv->type == FLEXVAL_TYPE_NONE ) throw HSPERR_INVALID_STRUCT_SOURCE;
		out = (char *)fv->ptr;
	}
	out += prm->offset;
	aptr = code_getv_proxy( pval, (MPVarData *)out, prm->mptype );
	exflg&=EXFLG_1;											// for 2nd prm_get
	return aptr;
}


APTR code_getva( PVal **pval )
{
	//		変数パラメーターを取得(pval+APTR)
	//
	PVal *getv_pval;
	APTR aptr;
	if ( exflg ) { throw HSPERR_VARIABLE_REQUIRED; }			// パラメーターなし(デフォルト時)

	if ( type == TYPE_STRUCT ) {								// 置き換えパラメーター時
		return code_getva_struct( pval );
	}
	if ( type != TYPE_VAR ) { throw HSPERR_VARIABLE_REQUIRED; }

	getv_pval = &hspctx->mem_var[val];
	code_next();

	aptr = code_getv_sub( &getv_pval );

	exflg&=EXFLG_1;												// for 2nd prm_get
	*pval = getv_pval;
	return aptr;
}


PVal *code_getpval( void )
{
	//		変数パラメーターを取得(PVal)
	//
	PVal *getv_pval;
	APTR aptr;
	aptr = code_getva( &getv_pval );
	if ( aptr != 0 ) throw HSPERR_BAD_ARRAY_EXPRESSION;
	return getv_pval;
}



unsigned short *code_getlb( void )
{
	//		ラベルパラメーターを取得
	//
	if ( type != TYPE_LABEL ) {
		int chk;
		unsigned short *p;
		chk = code_get();
		if ( chk<=PARAM_END ) { throw HSPERR_LABEL_REQUIRED; }
		if ( mpval->flag != HSPVAR_FLAG_LABEL ) { throw HSPERR_LABEL_REQUIRED; }
		p = *(unsigned short **)mpval->pt;
		if ( p == NULL ) { // ラベル型変数の初期値はエラーに
			throw HSPERR_LABEL_REQUIRED;
		}
		mcs = mcsbak;
		return p;
	}
	return (unsigned short *)( hspctx->mem_mcs + (hspctx->mem_ot[val]) );
}


unsigned short *code_getlb2( void )
{
	unsigned short *s;
	s = code_getlb();
	code_next();
	exflg&=~EXFLG_2;
	return s;
}


STRUCTPRM *code_getstprm( void )
{
	//		構造体パラメーターを取得
	//
	STRUCTPRM *prm;
	if ( type != TYPE_STRUCT ) throw HSPERR_STRUCT_REQUIRED;
	prm = &hspctx->mem_minfo[ val ];
	code_next();
	exflg&=~EXFLG_2;
	return prm;
}


STRUCTDAT *code_getstruct( void )
{
	//		構造体パラメーターを取得
	//
	STRUCTDAT *st;
	STRUCTPRM *prm;
	prm = code_getstprm();
	if ( prm->mptype != MPTYPE_STRUCTTAG ) throw HSPERR_STRUCT_REQUIRED;
	st = &hspctx->mem_finfo[ prm->subid ];
	return st;
}


STRUCTDAT *code_getcomst( void )
{
	//		COM構造体パラメーターを取得
	//
	STRUCTDAT *st;
	if ( type != TYPE_DLLCTRL ) throw HSPERR_TYPE_MISMATCH;
	val -= TYPE_OFFSET_COMOBJ;
	if ( val < 0 ) throw HSPERR_TYPE_MISMATCH;
	st = &hspctx->mem_finfo[ val ];
	code_next();
	exflg&=~EXFLG_2;
	return st;
}


/*
void code_setv( PVal *pval, PDAT *dat, int type, void *ptr )
{
	//		変数にパラメーターを指定する
	//
	PDAT *p;
	HspVarProc *proc;

	p = dat;
	proc = HspVarCoreGetProc( type );
	if ( pval->flag != type ) {
		HspVarCoreReset( pval );
		p = HspVarCorePtr( pval );					// 要素0のPDATポインタを取得
		if ( p != dat ) throw HSPERR_INVALID_ARRAYSTORE;
		HspVarCoreClear( pval, type );				// 最小サイズのメモリを確保
		p = proc->GetPtr( pval );					// PDATポインタを取得
	}
	proc->Set( p, ptr );
}
*/

void code_setva( PVal *pval, APTR aptr, int type, const void *ptr )
{
	//		変数にパラメーターを指定する
	//
	HspVarProc *proc;
	pval->offset = aptr;
	proc = HspVarCoreGetProc( type );
	if ( pval->flag != type ) {
		if ( aptr != 0 ) throw HSPERR_INVALID_ARRAYSTORE;
		HspVarCoreClear( pval, type );				// 最小サイズのメモリを確保
	}
	proc->Set( pval, proc->GetPtr( pval ), ptr );
}


char *code_getvptr( PVal **pval, int *size )
{
	//		変数ポインタを得る
	//
	APTR aptr;
	aptr = code_getva( pval );
	return (char *)HspVarCoreGetBlockSize( *pval, HspVarCorePtrAPTR( *pval,aptr ), size );
}


/*------------------------------------------------------------*/
/*
		call-return process function
*/
/*------------------------------------------------------------*/

static void customstack_delete( STRUCTDAT *st, char *stackptr )
{
	//	custom command stack delete
	//
	int i;
	char *out;
	char *ss;
	STRUCTPRM *prm;
	prm = &hspctx->mem_minfo[ st->prmindex ];
	for(i=0;i<st->prmmax;i++) {							// パラメーターを取得
		if ( prm->mptype == MPTYPE_LOCALSTRING ) {
			out = stackptr + prm->offset;
			ss = *(char **)out;
			sbFree( ss );
		} else if ( prm->mptype == MPTYPE_LOCALVAR ) {
			HspVarCoreDispose( (PVal *)(stackptr + prm->offset) );
		}
		prm++;
	}
}

void cmdfunc_return( void )
{
	//		return execute
	//
	STMDATA *stm;
	HSPROUTINE *r;

	if ( StackGetLevel == 0 ) throw HSPERR_RETURN_WITHOUT_GOSUB;

	stm = StackPeek;
	r = (HSPROUTINE *)STM_GETPTR(stm);

	if ( stm->type == TYPE_EX_CUSTOMFUNC ) {
		customstack_delete( r->param, (char *)(r+1) );	// カスタム命令のローカルメモリを解放
	}

	mcs=r->mcsret;
	hspctx->prmstack = r->oldtack;						// 以前のスタックに戻す

	hspctx->sublev--;
	code_next();

	StackPop();
}

#ifdef HSPEMSCRIPTEN
static void cmdfunc_gosub( unsigned short *subr, unsigned short *retpc )
{
	//		gosub execute
	//
	HSPROUTINE r;
	r.mcsret = retpc;
	r.stacklev = hspctx->sublev++;
	r.oldtack = hspctx->prmstack;
	r.param = NULL;
	StackPush( TYPE_EX_SUBROUTINE, (char *)&r, sizeof(HSPROUTINE) );

	code_setpc( subr );
}
#else
static int cmdfunc_gosub( unsigned short *subr )
{
	//		gosub execute
	//
	HSPROUTINE r;
	r.mcsret = mcs;
	r.stacklev = hspctx->sublev++;
	r.oldtack = hspctx->prmstack;
	r.param = NULL;
	StackPush( TYPE_EX_SUBROUTINE, (char *)&r, sizeof(HSPROUTINE) );

	mcs = subr;
	code_next();

	//		gosub内で呼び出しを完結させる
	//
	while(1) {
#ifdef HSPDEBUG
		if ( dbgmode ) code_dbgtrace();					// トレースモード時の処理
#endif
		if ( GetTypeInfoPtr( type )->cmdfunc( val ) ) {	// タイプごとの関数振り分け
			if ( hspctx->runmode == RUNMODE_RETURN ) {
				cmdfunc_return();
				break;
			} else {
				hspctx->msgfunc( hspctx );
			}
			if ( hspctx->runmode == RUNMODE_END ) {
				return RUNMODE_END;
			}
		}
	}

	return RUNMODE_RUN;
}
#endif


static int code_callfunc( int cmd )
{
	//	ユーザー拡張命令を呼び出す
	//
	STRUCTDAT *st;
	HSPROUTINE *r;
	int size;
	char *p;

	st = &hspctx->mem_finfo[cmd]; 

	size = sizeof(HSPROUTINE) + st->size;
	r = (HSPROUTINE *)StackPushSize( TYPE_EX_CUSTOMFUNC, size );
	p = (char *)(r+1);
	code_expandstruct( p, st, CODE_EXPANDSTRUCT_OPT_NONE );			// スタックの内容を初期化

	r->oldtack = hspctx->prmstack;				// 以前のスタックを保存
	hspctx->prmstack = (void *)p;				// 新規スタックを設定

	r->mcsret = mcsbak;							// 戻り場所
	r->stacklev = hspctx->sublev++;				// ネストを進める
	r->param = st;

	mcs = (unsigned short *)( hspctx->mem_mcs + (hspctx->mem_ot[ st->otindex ]) );
	code_next();

	//		命令内で呼び出しを完結させる
	//
	while(1) {

#ifdef HSPDEBUG
		if ( dbgmode ) code_dbgtrace();					// トレースモード時の処理
#endif
		if ( GetTypeInfoPtr( type )->cmdfunc( val ) ) {	// タイプごとの関数振り分け
			if ( hspctx->runmode == RUNMODE_END ) {
				throw HSPERR_NONE;
			}
			if ( hspctx->runmode == RUNMODE_RETURN ) {
				cmdfunc_return();
				break;
			} else {
				hspctx->msgfunc( hspctx );
			}
		}
	}

	return RUNMODE_RUN;
}


/*------------------------------------------------------------*/
/*
		structure process function
*/
/*------------------------------------------------------------*/

APTR code_newstruct( PVal *pval )
{
	int i,max;
	APTR ofs;
	FlexValue *fv;
	ofs = 0;
	if ( pval->flag != TYPE_STRUCT ) return 0;
	fv = (FlexValue *)pval->pt;
	max = pval->len[1];
	for( i=0;i<max;i++ ) {
		if ( fv[i].type == FLEXVAL_TYPE_NONE ) return i;
	}
	HspVarCoreReDim( pval, 1, max+1 );				// 配列を拡張する
	return max;
}


FlexValue *code_setvs( PVal *pval, APTR aptr, void *ptr, int size, int subid )
{
	//		TYPE_STRUCTの変数を設定する
	//		(返値:構造体を収めるための情報ポインタ)
	//
	FlexValue fv;
	fv.customid = subid;
	fv.clonetype = 0;
	fv.size = size;
	fv.ptr = ptr;
	code_setva( pval, aptr, TYPE_STRUCT, &fv );
	return (FlexValue *)HspVarCorePtrAPTR( pval, aptr );
}


void code_expandstruct( char *p, STRUCTDAT *st, int option )
{
	//	構造体の項目にパラメーターを代入する
	//
	int i,chk;
	char *out;
	STRUCTPRM *prm;
	prm = &hspctx->mem_minfo[ st->prmindex ];

	for(i=0;i<st->prmmax;i++) {							// パラメーターを取得
		out = p + prm->offset;
		//Alertf("(%d)type%d index%d/%d offset%d", st->subid, prm->mptype, st->prmindex + i, st->prmmax, prm->offset);
		switch( prm->mptype ) {
		case MPTYPE_INUM:
			*(int *)out = code_getdi(0);
			break;
		case MPTYPE_MODULEVAR:
			{
			MPModVarData *var;
			PVal *refpv;
			APTR refap;
			var = (MPModVarData *)out;
			refap = code_getva( &refpv );
			var->magic = MODVAR_MAGICCODE;
			var->subid = prm->subid;
			var->pval = refpv;
			var->aptr = refap;
			break;
			}
		case MPTYPE_IMODULEVAR:
		case MPTYPE_TMODULEVAR:
			*(MPModVarData *)out = modvar_init;
			break;
		case MPTYPE_SINGLEVAR:
		case MPTYPE_ARRAYVAR:
			{
			MPVarData *var;
			PVal *refpv;
			APTR refap;
			var = (MPVarData *)out;
			refap = code_getva( &refpv );
			var->pval = refpv;
			var->aptr = refap;
			break;
			}
		case MPTYPE_LABEL:
			*(unsigned short **)out = code_getlb2();
			break;
		case MPTYPE_DNUM:
			{
			//*(double *)out = code_getd();
			double d = code_getd();
			memcpy(out, &d, sizeof(double));
			break;
			}
		case MPTYPE_LOCALSTRING:
			{
			char *str;
			char *ss;
			str = code_gets();
			ss = sbAlloc( (int)strlen(str)+1 );
			strcpy( ss, str );
			*(char **)out = ss;
			break;
			}
		case MPTYPE_LOCALVAR:
			{
			PVal *pval;
			PDAT *dst;
			HspVarProc *proc;
			pval = (PVal *)out;
			pval->mode = HSPVAR_MODE_NONE;
			if ( option & CODE_EXPANDSTRUCT_OPT_LOCALVAR ) {
				chk = code_get();							// パラメーター値を取得
				if ( chk == PARAM_OK ) {
					pval->flag = mpval->flag;					// 仮の型
					HspVarCoreClear( pval, mpval->flag );		// 最小サイズのメモリを確保
					proc = HspVarCoreGetProc( pval->flag );
					dst = proc->GetPtr( pval );					// PDATポインタを取得
					proc->Set( pval, dst, mpval->pt );
					break;
				}
			}
			pval->flag = HSPVAR_FLAG_INT;				// 仮の型
			HspVarCoreClear( pval, HSPVAR_FLAG_INT );	// グローバル変数を0にリセット
			break;
			}
		case MPTYPE_STRUCTTAG:
			break;
		default:
			throw HSPERR_INVALID_STRUCT_SOURCE;
		}
		prm++;
	}
}


void code_delstruct( PVal *in_pval, APTR in_aptr )
{
	//		モジュール変数を破棄する
	//
	int i;
	char *p;
	char *out;
	STRUCTPRM *prm;
	STRUCTDAT *st;
	PVal *pval;
	FlexValue *fv;

	fv = (FlexValue *)HspVarCorePtrAPTR( in_pval, in_aptr );

	if ( fv->type != FLEXVAL_TYPE_ALLOC ) return;

	prm = &hspctx->mem_minfo[ fv->customid ];
	st = &hspctx->mem_finfo[ prm->subid ];
	p = (char *)fv->ptr;

	if ( fv->clonetype == 0 ) {

		//Alertf( "del:%d",st->otindex );
		if ( st->otindex ) {								// デストラクタを起動
			modvar_init.magic = MODVAR_MAGICCODE;
			modvar_init.subid = prm->subid;
			modvar_init.pval = in_pval;
			modvar_init.aptr = in_aptr;
			code_callfunc( st->otindex );
		}

		for(i=0;i<st->prmmax;i++) {							// パラメーターを取得
			out = p + prm->offset;
			switch( prm->mptype ) {
			case MPTYPE_LOCALVAR:
				pval = (PVal *)out;
				HspVarCoreDispose( pval );
				break;
			}
			prm++;
		}
	}
		
	//Alertf("STRUCT:BYE");
	sbFree( fv->ptr );
	fv->type = FLEXVAL_TYPE_NONE;
}


void code_delstruct_all( PVal *pval )
{
	//		モジュール変数全体を破棄する
	//
	int i;
	if ( pval->mode == HSPVAR_MODE_MALLOC ) {
		for(i=0;i<pval->len[1];i++) {
			code_delstruct( pval, i );
		}
	}
}


char *code_stmp( int size )
{
	//		stmp(文字列一時バッファ)を指定サイズで初期化する
	//
	if ( size > 1024 ) { hspctx->stmp = sbExpand( hspctx->stmp, size ); }
	return hspctx->stmp;
}


char *code_stmpstr( char *src )
{
	//		stmp(文字列一時バッファ)にsrcをコピーする
	//
	char *p;
	p = code_stmp( (int)strlen( src ) + 1 );
	strcpy( p, src );
	return p;
}


char *code_getsptr( int *type )
{
	int fl;
	char *bp;
	if ( code_get() <= PARAM_END ) {
		fl = HSPVAR_FLAG_INT;
		sptr_res = 0;
		bp = (char *)&sptr_res;
	} else {
		fl = mpval->flag;
		bp = mpval->pt;
		if (( fl != HSPVAR_FLAG_INT )&&( fl != HSPVAR_FLAG_STR )) {
			throw HSPERR_TYPE_MISMATCH;
		}
	}
	*type = fl;
	return bp;
}

/*------------------------------------------------------------*/
/*
		type process function
*/
/*------------------------------------------------------------*/

static int reffunc_intfunc_ivalue;

/*
	rev 43
	mingw : warning : 有符号型と無符号型の比較
	に対処
*/
#define ETRLOOP ((int)0x80000000)

#define GETLOP(num) (&(hspctx->mem_loop[num]))

static int cmdfunc_default( int cmd )
{
	//		cmdfunc : default
	//
	int tmp;
	if ( exflg & EXFLG_1 ) {
		tmp = type;
		code_next();
		if (( tmp == TYPE_INTFUNC )||( tmp == TYPE_EXTSYSVAR )) throw HSPERR_FUNCTION_SYNTAX;
		throw HSPERR_WRONG_NAME;
	}
	throw HSPERR_TOO_MANY_PARAMETERS;
	return RUNMODE_ERROR;
}


static int cmdfunc_custom( int cmd )
{
	//	custom command execute
	//
	STRUCTDAT *st;
	code_next();

	st = &hspctx->mem_finfo[cmd];
	if ( st->index != STRUCTDAT_INDEX_FUNC ) throw HSPERR_SYNTAX;

	return code_callfunc( cmd );
}


static void *reffunc_custom( int *type_res, int arg )
{
	//	custom function execute
	//
	STRUCTDAT *st;
	void *ptr;
	int old_funcres;

	//		返値のタイプを設定する
	//
	st = &hspctx->mem_finfo[arg];
	if ( st->index != STRUCTDAT_INDEX_CFUNC ) throw HSPERR_SYNTAX;
	old_funcres = funcres;
	funcres = TYPE_ERROR;

	//			'('で始まるかを調べる
	//
	if ( type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( val != '(' ) throw HSPERR_INVALID_FUNCPARAM;

	code_next();
	code_callfunc( arg );

	*type_res = funcres;					// 返値のタイプを指定する
	switch( funcres ) {						// 返値のポインタを設定する
	case TYPE_STRING:
		ptr = hspctx->refstr;
		break;
	case TYPE_DNUM:
		ptr = &hspctx->refdval;
		break;
	case TYPE_INUM:
		ptr = &hspctx->stat;
		break;
	default:
		if ( hspctx->runmode == RUNMODE_END ) {
			throw HSPERR_NONE;
		}
		throw HSPERR_NORETVAL;
	}

	//			')'で終わるかを調べる
	//
	if ( type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	funcres = old_funcres;
	return ptr;
}


static int cmdfunc_var( int cmd )
{
	//		cmdfunc : TYPE_VAR
	//		(変数代入 : 変数名が先頭に来る場合)
	//
	PVal *pval;
	HspVarProc *proc;
	APTR aptr;
	void *ptr;
	PDAT *dst;
	int chk,exp,incval;
	int baseaptr;

#ifdef HSPDEBUG
	if (( exflg & EXFLG_1 ) == 0 ) throw HSPERR_TOO_MANY_PARAMETERS;
#endif

	exflg = 0;								// code_nextを使わない時に必要
	aptr = code_getva( &pval );				// 変数を取得

	if ( type != TYPE_MARK ) { mcs = mcsbak; throw HSPERR_SYNTAX; }
	exp = val;
	code_next();							// 次のコードへ

	if ( exflg ) {							// インクリメント(+)・デクリメント(-)
		proc = HspVarCoreGetProc( pval->flag );
		incval = 1;
		if ( pval->flag == HSPVAR_FLAG_INT ) { ptr = &incval; } else {
			ptr = (int *)proc->Cnv( &incval, HSPVAR_FLAG_INT );	// 型がINTでない場合は変換
		}
		dst = HspVarCorePtrAPTR( pval, aptr );
		switch( exp ) {
		case CALCCODE_ADD:
			proc->AddI( dst, ptr );
			break;
		case CALCCODE_SUB:
			proc->SubI( dst, ptr );
			break;
		default:
			throw HSPERR_SYNTAX;
		}
		return RUNMODE_RUN;
	}

	chk = code_get();									// パラメーター値を取得
	if ( chk != PARAM_OK ) { throw HSPERR_SYNTAX; }

	proc = HspVarCoreGetProc( pval->flag );
	dst = HspVarCorePtrAPTR( pval, aptr );
	ptr = mpval->pt;
	if ( exp == CALCCODE_EQ ) {							// '='による代入
		if ( pval->support & HSPVAR_SUPPORT_NOCONVERT ) {	// 型変換なしの場合
			if ( arrayobj_flag ) {
				proc->ObjectWrite( pval, ptr, mpval->flag );
				return RUNMODE_RUN;
			}
		}
		if ( pval->flag != mpval->flag ) {
			if ( aptr != 0 ) throw HSPERR_INVALID_ARRAYSTORE;	// 型変更の場合は配列要素0のみ
			HspVarCoreClear( pval, mpval->flag );		// 最小サイズのメモリを確保
			proc = HspVarCoreGetProc( pval->flag );
			dst = proc->GetPtr( pval );					// PDATポインタを取得
		}
		proc->Set( pval, dst, ptr );
		if ( exflg ) return RUNMODE_RUN;

		chk = pval->len[1];
		if ( chk == 0 ) baseaptr = aptr; else baseaptr = aptr % chk;
		aptr -= baseaptr;

		while(1) {
			if ( exflg ) break;
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
		}
		return RUNMODE_RUN;
	}

	//		変数+演算子による代入
	//
	if ( pval->flag != mpval->flag ) {					// 型が一致しない場合は変換
		ptr = HspVarCoreCnvPtr( mpval, pval->flag );
		//ptr = proc->Cnv( ptr, mpval->flag );
	}
	calcprm( proc, dst, exp, ptr );
	if ( proc->aftertype != pval->flag ) {				// 演算後に型が変わる場合
		throw HSPERR_TYPE_MISMATCH;
	}
	return RUNMODE_RUN;
}


static void cmdfunc_return_setval( void )
{
	//		引数をシステム変数にセットする(返値用)
	//
	if ( code_get() <= PARAM_END ) return;

	hspctx->retval_level = hspctx->sublev;
	funcres = mpval->flag;

	switch( funcres ) {
	case HSPVAR_FLAG_INT:
		hspctx->stat = *(int *)mpval->pt;
		break;
	case HSPVAR_FLAG_STR:
		sbStrCopy( &hspctx->refstr, mpval->pt );
		break;
	case HSPVAR_FLAG_DOUBLE:
		hspctx->refdval = *(double *)mpval->pt;
		break;
	default:
		throw HSPERR_TYPE_MISMATCH;
	}
}


static int cmdfunc_ifcmd( int cmd )
{
	//	'if' command execute
	//
	int i;
	unsigned short *mcstmp;
	i=(int)*mcs;mcs++;						// skip offset get
	mcstmp=mcs+i;
	if (val==0) {							// 'if'
		code_next();						// get first token
		if ( code_geti() ) return RUNMODE_RUN;			// if true
	}
	mcs=mcstmp;
	code_next();
	return RUNMODE_RUN;
}


static void cmdfunc_mref( PVal *pval, int prm )
{
	//		mref command
	//
	int t,size;
	char *out;
	HSPEXINFO *exinfo;

	if ( prm & 1024 ) throw HSPERR_UNSUPPORTED_FUNCTION;
	if ( prm >= 0x40 ) {
		exinfo = hspctx->exinfo2;
		if ( exinfo->HspFunc_mref != NULL ) exinfo->HspFunc_mref( pval, prm );
		return;
	}
	if (( prm & 0x30 )||( prm >= 8 )) throw HSPERR_UNSUPPORTED_FUNCTION;

	out = ((char *)hspctx->prmstack );
	if ( out == NULL ) throw HSPERR_ILLEGAL_FUNCTION;
	t = HSPVAR_FLAG_INT; size = sizeof(int);
	HspVarCoreDupPtr( pval, t, (out+(size*prm)), size );
}


static int cmdfunc_prog( int cmd )
{
	//		cmdfunc : TYPE_PROGCMD
	//

	int p1,p2,p3,p4,p5;

	code_next();							// 次のコードを取得(最初に必ず必要です)

	switch( cmd ) {							// サブコマンドごとの分岐

	case 0x00:								// goto
		mcs = code_getlb();
		code_next();
		break;

	case 0x01:								// gosub
#ifdef HSPEMSCRIPTEN
		{
		unsigned short *sbr;
		sbr = code_getlb();
		cmdfunc_gosub( sbr, mcs );
		break;
		}
#else
		{
		unsigned short *sbr;
		sbr = code_getlb();
		return cmdfunc_gosub( sbr );
		}
#endif
	case 0x02:								// return
#ifdef HSPEMSCRIPTEN
		if ( hspctx->prmstack != NULL ) cmdfunc_return_setval();
#else
		if ( exflg == 0 ) cmdfunc_return_setval();
#endif
		//return cmdfunc_return();
		hspctx->runmode = RUNMODE_RETURN;
		return RUNMODE_RETURN;

	case 0x03:								// break
		if (hspctx->looplev==0) throw HSPERR_LOOP_WITHOUT_REPEAT;
		hspctx->looplev--;
		mcs = code_getlb();
		code_next();
		break;

	case 0x04:								// repeat
		{
		LOOPDAT *lop;
		unsigned short *label;
		if (hspctx->looplev>=(HSP3_REPEAT_MAX-1)) throw HSPERR_TOO_MANY_NEST;
		label = code_getlb();
		code_next();
		p1 = code_getdi( ETRLOOP );
		p2 = code_getdi( 0 );
		if ( p1==0 ) {				// 0は即break
			mcs=label;
			code_next();
			break;
		}
		if ( p1<0 ) p1=ETRLOOP; else p1+=p2;
		hspctx->looplev++;
		lop=GETLOP(hspctx->looplev);
		lop->cnt = p2;
		lop->time = p1;
		lop->pt = mcsbak;
		break;
		}

	case 0x05:								// loop
		{
		LOOPDAT *lop;
		if (hspctx->looplev==0) throw HSPERR_LOOP_WITHOUT_REPEAT;
		lop=GETLOP(hspctx->looplev);
		lop->cnt++;
		if ( lop->time != ETRLOOP ) {		// not eternal loop
			if ( lop->cnt >= lop->time ) {
				hspctx->looplev--;
				break;
			}
		}
		mcs=lop->pt;
		code_next();
		break;
		}

	case 0x06:								// continue
		{
		LOOPDAT *lop;
		unsigned short *label;
		label = code_getlb();
		lop=GETLOP(hspctx->looplev);
		code_next();
		p2=lop->cnt + 1;
		p1 = code_getdi( p2 );
		lop->cnt = p1 - 1;
		mcs=label;
		val=0x05;type=TYPE_PROGCMD;exflg=0;	// set 'loop' code
		break;
		}

	case 0x07:								// wait
		hspctx->waitcount = code_getdi( 100 );
		hspctx->runmode = RUNMODE_WAIT;
		return RUNMODE_WAIT;

	case 0x08:								// await
		p1=code_getdi( 0 );
		//p2=code_getdi( -1 );
		hspctx->waitcount = p1;
		hspctx->waittick = -1;
		//if ( p2 > 0 ) hspctx->waitbase = p2;
		hspctx->runmode = RUNMODE_AWAIT;
		return RUNMODE_AWAIT;

	case 0x09:								// dim
	case 0x0a:								// sdim
	case 0x0d:								// dimtype
		{
		HspVarProc *proc;
		PVal *pval;
		int fl;
		pval = code_getpval();
		fl = TYPE_INUM;
		if ( cmd == 0x0d ) {
			fl = code_geti();
			proc = HspVarCoreGetProc( fl );
			if ( proc->flag == 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		}
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p4 = code_getdi( 0 );
		if ( cmd == 0x0a ) {
			p5 = code_getdi( 0 );
			HspVarCoreDimFlex( pval, TYPE_STRING, p1, p2, p3, p4, p5 );
			//HspVarCoreAllocBlock( pval, HspVarCorePtrAPTR( pval, 0 ), p1 );
			break;
		}
		HspVarCoreDim( pval, fl, p1, p2, p3, p4 );
		break;
		}

	case 0x0b:								// foreach
		{
		LOOPDAT *lop;
		unsigned short *label;
		if (hspctx->looplev>=(HSP3_REPEAT_MAX-1)) throw HSPERR_TOO_MANY_NEST;
		label = code_getlb();
		code_next();
		hspctx->looplev++;
		lop=GETLOP(hspctx->looplev);
		lop->cnt = 0;
		lop->time = ETRLOOP;
		lop->pt = mcsbak;
		break;
		}
	case 0x0c:								// (hidden)foreach check
		{
		int i;
		PVal *pval;
		LOOPDAT *lop;
		unsigned short *label;
		if (hspctx->looplev==0) throw HSPERR_LOOP_WITHOUT_REPEAT;
		label = code_getlb();
		code_next();
		lop=GETLOP(hspctx->looplev);

		pval = code_getpval();
		if ( lop->cnt >= pval->len[1] ) {		// ループ終了
			hspctx->looplev--;
			mcs = label;
			code_next();
			break;
		}
		if ( pval->support & HSPVAR_SUPPORT_VARUSE ) {
			i = HspVarCoreGetUsing( pval, HspVarCorePtrAPTR( pval, lop->cnt ) );
			if ( i == 0 ) {						// スキップ
				mcs=label;
				val=0x05;type=TYPE_PROGCMD;exflg=0;	// set 'loop' code
			}
		}
		break;
		}
	case 0x0e:								// dup
		{
		PVal *pval_m;
		PVal *pval;
		APTR aptr;
		pval_m = code_getpval();
		aptr = code_getva( &pval );
		HspVarCoreDup( pval_m, pval, aptr );
		break;
		}
	case 0x0f:								// dupptr
		{
		PVal *pval_m;
		pval_m = code_getpval();
		p1 = code_geti();
		p2 = code_geti();
		p3 = code_getdi( HSPVAR_FLAG_INT );
		if ( p2<=0 ) throw HSPERR_ILLEGAL_FUNCTION;
		if ( HspVarCoreGetProc(p3)->flag == 0 ) throw HSPERR_ILLEGAL_FUNCTION;
		HspVarCoreDupPtr( pval_m, p3, (void *)p1, p2 );
		break;
		}

	case 0x10:								// end
		hspctx->endcode = code_getdi(0);
		hspctx->runmode = RUNMODE_END;
		return RUNMODE_END;
	case 0x1b:								// assert
		p1 = code_getdi( 0 );
		if ( p1 ) break;
		hspctx->runmode = RUNMODE_ASSERT;
		return RUNMODE_ASSERT;
	case 0x11:								// stop
		hspctx->runmode = RUNMODE_STOP;
		return RUNMODE_STOP;

	case 0x12:								// newmod
	case 0x13:								// setmod
		{
		char *p;
		PVal *pval;
		APTR aptr;
		FlexValue *fv;
		STRUCTDAT *st;
		STRUCTPRM *prm;
		if ( cmd == 0x12 ) {
			pval = code_getpval();
			aptr = code_newstruct(pval);
		}
		else {
			aptr = code_getva( &pval );
		}
		st = code_getstruct();
		fv = code_setvs(pval, aptr, NULL, st->size, st->prmindex);
		fv->type = FLEXVAL_TYPE_ALLOC;
		p = sbAlloc( fv->size );
		fv->ptr = (void *)p;
		prm = &hspctx->mem_minfo[ st->prmindex ];
		if ( prm->mptype != MPTYPE_STRUCTTAG ) throw HSPERR_STRUCT_REQUIRED;
		code_expandstruct(p, st, CODE_EXPANDSTRUCT_OPT_NONE);
		if (prm->offset != -1) {
			modvar_init.magic = MODVAR_MAGICCODE;
			modvar_init.subid = prm->subid;
			modvar_init.pval = pval;
			modvar_init.aptr = aptr;
			return code_callfunc(prm->offset);
		}
		break;
		}
	case 0x14:								// delmod
		{
		PVal *pval;
		APTR aptr;
		aptr = code_getva( &pval );
		if ( pval->flag != TYPE_STRUCT ) throw HSPERR_TYPE_MISMATCH;
		code_delstruct( pval, aptr );
		break;
		}
/*
	case 0x15:								// alloc
		{
		PVal *pval;
		pval = code_getpval();
		p1 = code_getdi( 0 );
		if ( p1 <= 64 ) p1 = 64;
		HspVarCoreDim( pval, TYPE_STRING, 1, 0, 0, 0 );
		HspVarCoreAllocBlock( pval, HspVarCorePtrAPTR( pval, 0 ), p1 );
		break;
		}
*/
	case 0x16:								// mref
		{
		PVal *pval_m;
		pval_m = code_getpval();
		p1 = code_getdi(0);
		cmdfunc_mref( pval_m, p1 );
		break;
		}
	case 0x17:								// run
		sbStrCopy( &hspctx->refstr, code_gets() );
		code_stmpstr( code_getds("") );
		throw HSPERR_EXITRUN;

	case 0x18:								// exgoto
		{
		PVal *pval;
		APTR aptr;
		int *ival;
		int i;
		unsigned short *label;
		aptr = code_getva( &pval );
		if ( pval->flag != HSPVAR_FLAG_INT ) throw HSPERR_TYPE_MISMATCH;
		ival = (int *)HspVarCorePtrAPTR( pval, aptr );
		p1=code_getdi( 0 );
		p2=code_getdi( 0 );
		label = code_getlb2();
		i = 0;
		if ( p1 >= 0 ) {
			if ( (*ival) >= p2 ) i++;
		} else {
			if ( (*ival) <= p2 ) i++;
		}
		if ( i ) code_setpc( label);
		break;
		}

	case 0x19:								// on
		{
		unsigned short *label;
		unsigned short *otbak;
		p1=code_getdi( 0 );
		if ( type != TYPE_PROGCMD ) throw HSPERR_SYNTAX;
		if ( val >= 0x02 ) throw HSPERR_SYNTAX;	// goto/gosub以外はエラー
		p2 = 0; p3 = val; otbak = NULL;
		code_next();
		while( (exflg&EXFLG_1)==0 ) {
			label = code_getlb2();
			if ( p1 == p2 ) {
				if ( p3 ) {
					otbak = label;			// on～gosub
				} else {
					code_setpc( label);		// on～goto
					break;
				}
			}
			p2++;
		}
		if ( otbak != NULL ) {
			code_call( otbak );
			return hspctx->runmode;
		}
		break;
		}

	case 0x1a:								// mcall
		{
		PVal *pval;
		HspVarProc *varproc;
		pval = code_getpval();
		varproc = HspVarCoreGetProc( pval->flag );
		varproc->ObjectMethod( pval );
		break;
		}
	case 0x1c:								// logmes
		code_stmpstr( code_getdsi("") );
		hspctx->runmode = RUNMODE_LOGMES;
		return RUNMODE_LOGMES;

	case 0x1d:								// newlab
		{
		PVal *pval;
		APTR aptr;
		unsigned short *label;
		int i;
		aptr = code_getva( &pval );
		label = NULL;
		switch( type ) {
		case TYPE_INUM:
			i = code_geti();
			if ( i == 0 ) label = mcsbak;
			if ( i == 1 ) label = mcs;
			break;
		case TYPE_LABEL:
			label = code_getlb2();
			break;
		default:
			throw HSPERR_TYPE_MISMATCH;
		}
		code_setva( pval, aptr, HSPVAR_FLAG_LABEL, &label );
		break;
		}

	case 0x1e:								// resume
		break;

	case 0x1f:								// yield
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


static void *reffunc_sysvar( int *type_res, int arg )
{
	//		reffunc : TYPE_SYSVAR
	//		(内蔵システム変数)
	//
	void *ptr;

	//		返値のタイプを設定する
	//
	*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
	ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ

	switch( arg ) {

	//	int function
	case 0x000:								// system
		reffunc_intfunc_ivalue = 0;
		break;
	case 0x001:								// hspstat
		reffunc_intfunc_ivalue = hspctx->hspstat;
		break;
	case 0x002:								// hspver
		reffunc_intfunc_ivalue = vercode | mvscode;
		break;
	case 0x003:								// stat
		reffunc_intfunc_ivalue = hspctx->stat;
		break;
	case 0x004:								// cnt
		reffunc_intfunc_ivalue = hspctx->mem_loop[hspctx->looplev].cnt;
		break;
	case 0x005:								// err
		reffunc_intfunc_ivalue = hspctx->err;
		break;
	case 0x006:								// strsize
		reffunc_intfunc_ivalue = hspctx->strsize;
		break;
	case 0x007:								// looplev
		reffunc_intfunc_ivalue = hspctx->looplev;
		break;
	case 0x008:								// sublev
		reffunc_intfunc_ivalue = hspctx->sublev;
		break;

	case 0x009:								// iparam
		reffunc_intfunc_ivalue = hspctx->iparam;
		break;
	case 0x00a:								// wparam
		reffunc_intfunc_ivalue = hspctx->wparam;
		break;
	case 0x00b:								// lparam
		reffunc_intfunc_ivalue = hspctx->lparam;
		break;
	case 0x00c:								// refstr
		*type_res = HSPVAR_FLAG_STR;
		ptr = (void *)hspctx->refstr;
		break;
	case 0x00d:								// refdval
		*type_res = HSPVAR_FLAG_DOUBLE;
		ptr = (void *)&hspctx->refdval;
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return ptr;
}



/*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

static void hsp3typeinit_var( HSP3TYPEINFO *info )
{
	info->cmdfunc = cmdfunc_var;
}

static void hsp3typeinit_prog( HSP3TYPEINFO *info )
{
	info->cmdfunc = cmdfunc_prog;
}

static void hsp3typeinit_ifcmd( HSP3TYPEINFO *info )
{
	info->cmdfunc = cmdfunc_ifcmd;
}

static void hsp3typeinit_sysvar( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_sysvar;
}

static void hsp3typeinit_custom( HSP3TYPEINFO *info )
{
	info->cmdfunc = cmdfunc_custom;
	info->reffunc = reffunc_custom;
}

static void hsp3typeinit_default( int type )
{
	//		typeinfoの初期化
	HSP3TYPEINFO *info;
	info = GetTypeInfoPtr( type );
	info->type = type;
	info->option = 0;
	info->hspctx = hspctx;
	info->hspexinfo = hspctx->exinfo2;
	info->cmdfunc = cmdfunc_default;
	info->reffunc = NULL;
	info->termfunc = NULL;
	info->eventfunc = NULL;
}


HSP3TYPEINFO *code_gettypeinfo( int type )
{
	//		指定されたタイプのHSP3TYPEINFO構造体を取得します。
	//		( typeがマイナスの場合は、新規typeIDを発行 )
	//
	int id;
	HSP3TYPEINFO *info;
	id = type;
	if ( id < 0 ) {
		id = tinfo_cur++;
		hsp3tinfo = (HSP3TYPEINFO *)sbExpand( (char *)hsp3tinfo, sizeof(HSP3TYPEINFO) * tinfo_cur );
		hsp3typeinit_default( id );
	}
	info = GetTypeInfoPtr( id );
	return info;
}


void code_setctx( HSPCTX *ctx )
{
	//		HSPコンテキストを設定
	//
	hspctx = ctx;
}


void code_def_msgfunc(HSPCTX *ctx )
{
	//	デフォルトのHSPメッセージコールバック
	//
	ctx->runmode = RUNMODE_END;
}


void code_resetctx( HSPCTX *ctx )
{
	//		コンテキストのリセット(オブジェクトロード後の初期化)
	//
	mpval_int = HspVarCoreGetPVal(HSPVAR_FLAG_INT);
	HspVarCoreClearTemp( mpval_int, HSPVAR_FLAG_INT );	// int型のテンポラリを初期化

	ctx->err = HSPERR_NONE;
#ifdef HSPDEBUG
	ctx->hspstat = HSPSTAT_DEBUG;
#else
	ctx->hspstat = 0;
#endif
	ctx->waitbase = 5;
	ctx->lasttick = 0;
	ctx->looplev = 0;
	ctx->sublev = 0;
	ctx->stat = 0;
	ctx->strsize = 0;
	ctx->runmode = RUNMODE_RUN;
	ctx->prmstack = NULL;
	ctx->note_pval = NULL;
	ctx->notep_pval = NULL;
	ctx->msgfunc = code_def_msgfunc;
}

HSPCTX *code_getctx( void )
{
	return hspctx;
}

void code_enable_typeinfo( HSP3TYPEINFO *info )
{
	//		typeinfoを有効にする(オプションチェック)
	//
	hspevent_opt |= info->option;
}


static HspVarProc *HspFunc_getproc( int id )
{
	return (&hspvarproc[id]);
}


HSPERROR code_geterror( void )
{
	return hspctx->err;
}

void code_setpc( const unsigned short *pc )
{
	//		プログラムカウンタを設定
	//
	if ( hspctx->runmode == RUNMODE_END ) return;
	mcs = (unsigned short *)pc;
	code_next();
	hspctx->runmode = RUNMODE_RUN;
}

void code_setpci( const unsigned short *pc )
{
	//		プログラムカウンタを設定(interrput)
	//
	code_setpc( pc );
	hspctx->runmode = RUNMODE_INTJUMP;
}

void code_call( const unsigned short *pc )
{
	//		サブルーチンジャンプを行なう
	//
	mcs = mcsbak;
#ifdef HSPEMSCRIPTEN
	cmdfunc_gosub( (unsigned short *)pc, mcs );
#else
	cmdfunc_gosub( (unsigned short *)pc );
#endif
	if ( hspctx->runmode == RUNMODE_END ) return;
	hspctx->runmode = RUNMODE_RUN;
}

unsigned short *code_getpcbak( void )
{
	//mcsbakを返す
	return mcsbak;
}

//
//		Error report routines
//
int code_getdebug_line( void )
{
	return code_getdebug_line( mcsbak );
}
int code_getdebug_line( unsigned short* pt )
{
	//		Get current debug line info
	//		(最後に実行した場所を示す)
	//			result :  0=none  others=line#
	//
#ifdef HSPDEBUG
	unsigned char *mem_di;
	unsigned char ofs;
	int cl,a,tmp,curpt,debpt;

	mem_di = hspctx->mem_di;
	debpt=0;
	curpt = (int)( pt - hspctx->mem_mcs );
	if ( mem_di[0]==255) return -1;

	cl=0;a=0;
	while(1) {
		ofs=mem_di[a++];
		switch( ofs ) {
		case 252:
			debpt+=(mem_di[a+1]<<8)+mem_di[a];
			if (curpt<=debpt) return cl;
			cl++;
			a+=2;
			break;
		case 253:
			a+=5;
			break;
		case 254:
			tmp = (mem_di[a+2]<<16)+(mem_di[a+1]<<8)+mem_di[a];
			if ( tmp ) srcname = tmp;
			cl=(mem_di[a+4]<<8)+mem_di[a+3];
			a+=5;
			break;
		case 255:
			return -1;
		default:
			debpt+=ofs;
			if (curpt<=debpt) return cl;
			cl++;
			break;
		}
	}
	return cl;
#else
	return -1;
#endif
}


int code_debug_init( void )
{
	//		mem_di_valを更新
	//
	unsigned char ofs;
	unsigned char *mem_di;
	int cl,a;

	cl=0;a=0;
	mem_di_val = NULL;
	mem_di = hspctx->mem_di;
	if ( mem_di[0]==255) return -1;
	while(1) {
		ofs=mem_di[a++];
		switch( ofs ) {
		case 252:
			a+=2;
			break;
		case 253:
			mem_di_val=&mem_di[a-1];
			return 0;
		case 254:
			cl=(mem_di[a+4]<<8)+mem_di[a+3];
			a+=5;
			break;
		case 255:
			return -1;
		default:
			cl++;
			break;
		}
	}
	return cl;
}


char *code_getdebug_varname( int val_id )
{
	unsigned char *mm;
	int i;
	if ( mem_di_val == NULL ) return "";
	mm = mem_di_val + ( val_id * 6 );
	i = (mm[3]<<16)+(mm[2]<<8)+mm[1];
	return strp(i);
}


int code_getdebug_seekvar( const char *name )
{
	unsigned char *mm;
	int i,ofs;
	if ( mem_di_val != NULL ) {
		mm = mem_di_val;
		for(i=0;i<hspctx->hsphed->max_val;i++) {
			ofs = (mm[3]<<16)+(mm[2]<<8)+mm[1];
			if ( strcmp( strp( ofs ), name ) == 0 ) return i;
			mm += 6;
		}
	}
	return -1;
}


char *code_getdebug_name( void )
{
	return strp(srcname);
}


int code_exec_wait( int tick )
{
	//		時間待ち(wait)
	//		(awaitに変換します)
	//
	if ( hspctx->waitcount <= 0 ) {
		hspctx->runmode = RUNMODE_RUN;
		return RUNMODE_RUN;
	}
	hspctx->waittick = tick + ( hspctx->waitcount * 10 );
	return RUNMODE_AWAIT;
}


int code_exec_await( int tick )
{
	//		時間待ち(await)
	//
	if ( hspctx->waittick < 0 ) {
		if ( hspctx->lasttick == 0 ) hspctx->lasttick = tick;
		hspctx->waittick = hspctx->lasttick + hspctx->waitcount;
	}
	if ( tick >= hspctx->waittick ) {
		hspctx->lasttick = tick;
		hspctx->runmode = RUNMODE_RUN;
		return RUNMODE_RUN;
	}
	return RUNMODE_AWAIT;
}


/*------------------------------------------------------------*/
/*
		code main interface
*/
/*------------------------------------------------------------*/

static int error_dummy = 0;		// 2.61互換のためのダミー値

static int code_cnv_get( void )
{
	//		データを取得(プラグイン交換用)
	//
	hspctx->exinfo.mpval = &mpval;
	return code_get();
}

static void *code_cnv_getv( void )
{
	//		変数データアドレスを取得(2.61互換用)
	//
	char *ptr;
	int size;
	ptr = code_getvptr( &plugin_pval, &size );
	hspctx->exinfo.mpval = &plugin_pval;
	return (void *)ptr;
}

static int code_cnv_realloc( PVal *pv, int size, int mode )
{
	//		変数データバッファを拡張(2.61互換用)
	//
	PDAT *ptr;
	ptr = HspVarCorePtrAPTR( pv, 0 );
	HspVarCoreAllocBlock( pv, ptr, size );
	return 0;
}

void code_init( void )
{
	int i;
	HSPEXINFO *exinfo;

	sbInit();					// 可変メモリバッファ初期化
	StackInit();
	HspVarCoreInit();			// ストレージコア初期化
	mpval = HspVarCoreGetPVal(0);
	hspevent_opt = 0;			// イベントオプションを初期化

	//		exinfoの初期化
	//
	exinfo = &mem_exinfo;

	//		2.61互換フィールド
	exinfo->ver = vercode;
	exinfo->min = mvscode;
	exinfo->er = &error_dummy;
	exinfo->pstr = hspctx->stmp;
	exinfo->stmp = hspctx->refstr;
	exinfo->strsize = &hspctx->strsize;
	exinfo->refstr = hspctx->refstr;
	exinfo->HspFunc_prm_getv = code_cnv_getv;

	exinfo->HspFunc_val_realloc = code_cnv_realloc;
	exinfo->HspFunc_fread = dpm_read;
	exinfo->HspFunc_fsize = dpm_exist;

	//		共用フィールド
	exinfo->nptype = &type;
	exinfo->npval = &val;
	exinfo->mpval = &mpval;

	exinfo->HspFunc_prm_geti = code_geti;
	exinfo->HspFunc_prm_getdi = code_getdi;
	exinfo->HspFunc_prm_gets = code_getas;
	exinfo->HspFunc_prm_getds = code_getads;
	exinfo->HspFunc_getbmscr = NULL;
	exinfo->HspFunc_addobj = NULL;
	exinfo->HspFunc_setobj = NULL;
	exinfo->HspFunc_setobj = NULL;

	//		3.0拡張フィールド
	exinfo->hspctx = hspctx;
	exinfo->npexflg = &exflg;
	exinfo->HspFunc_setobj = NULL;

	exinfo->HspFunc_puterror = code_puterror;
	exinfo->HspFunc_getproc = HspFunc_getproc;
	exinfo->HspFunc_seekproc = HspVarCoreSeekProc;
	exinfo->HspFunc_prm_next = code_next;
	exinfo->HspFunc_prm_get = code_cnv_get;
	exinfo->HspFunc_prm_getlb = code_getlb2;
	exinfo->HspFunc_prm_getpval = code_getpval;
	exinfo->HspFunc_prm_getva = code_getva;
	exinfo->HspFunc_prm_setva = code_setva;
	exinfo->HspFunc_prm_getd = code_getd;
	exinfo->HspFunc_prm_getdd = code_getdd;

	exinfo->HspFunc_malloc = sbAlloc;
	exinfo->HspFunc_free = sbFree;
	exinfo->HspFunc_expand = sbExpand;
	exinfo->HspFunc_addirq = code_addirq;
	exinfo->HspFunc_hspevent = code_event;
	exinfo->HspFunc_registvar = HspVarCoreRegisterType;
	exinfo->HspFunc_setpc = code_setpc;
	exinfo->HspFunc_call = code_call;

	exinfo->HspFunc_dim = HspVarCoreDimFlex;
	exinfo->HspFunc_redim = HspVarCoreReDim;
	exinfo->HspFunc_array = HspVarCoreArray;

	//		3.1拡張フィールド
	exinfo->HspFunc_varname = code_getdebug_varname;
	exinfo->HspFunc_seekvar = code_getdebug_seekvar;

	exinfo->HspFunc_prm_getns = code_gets;
	exinfo->HspFunc_prm_getnds = code_getds;
	//		HSPCTXにコピーする
	//
	memcpy( &hspctx->exinfo, exinfo, sizeof(HSPEXINFO30) );
	hspctx->exinfo2 = exinfo;

	//		標準typefunc登録
	//
	hsp3tinfo = (HSP3TYPEINFO *)sbAlloc( sizeof(HSP3TYPEINFO) * HSP3_FUNC_MAX );
	tinfo_cur = HSP3_FUNC_MAX;
	for(i=0;i<tinfo_cur;i++) {
		hsp3typeinit_default( i );
	}

	//		内蔵タイプの登録
	//
	hsp3typeinit_var( GetTypeInfoPtr( TYPE_VAR ) );
	hsp3typeinit_var( GetTypeInfoPtr( TYPE_STRUCT ) );
	hsp3typeinit_prog( GetTypeInfoPtr( TYPE_PROGCMD ) );
	hsp3typeinit_ifcmd( GetTypeInfoPtr( TYPE_CMPCMD ) );
	hsp3typeinit_sysvar( GetTypeInfoPtr( TYPE_SYSVAR ) );
	hsp3typeinit_intcmd( GetTypeInfoPtr( TYPE_INTCMD ) );
	hsp3typeinit_intfunc( GetTypeInfoPtr( TYPE_INTFUNC ) );
	hsp3typeinit_intfunc( GetTypeInfoPtr( TYPE_INTFUNC ) );
	hsp3typeinit_custom( GetTypeInfoPtr( TYPE_MODCMD ) );

	//		割り込みの初期化
	//
	hspctx->mem_irq = NULL;
	hspctx->irqmax = 0;
	for(i=0;i<HSPIRQ_MAX;i++) { code_addirq(); }	// 標準割り込みを確保
	code_enableirq( HSPIRQ_USERDEF, 1 );			// カスタムタイプのみEnable

	//		プラグイン追加の準備
	//
	tinfo_cur = HSP3_TYPE_USER;

	//		文字列バッファの初期化
	//
	hspctx->refstr = sbAlloc( HSPCTX_REFSTR_MAX );
	hspctx->fnbuffer = sbAlloc( HSP_MAX_PATH );
	hspctx->stmp = sbAlloc( HSPCTX_REFSTR_MAX );
	hspctx->cmdline = sbAlloc( HSPCTX_CMDLINE_MAX );

#ifdef HSPDEBUG
	//		デバッグ情報の初期化
	//
	mem_di_val = NULL;
	dbgmode = HSPDEBUG_NONE;
	dbginfo.hspctx = hspctx;
	dbginfo.line = 0;
	dbginfo.fname = NULL;
	dbginfo.get_value = code_dbgvalue;
	dbginfo.get_varinf = code_dbgvarinf;
	dbginfo.dbg_close = code_dbgclose;
	dbginfo.dbg_curinf = code_dbgcurinf;
	dbginfo.dbg_set = code_dbgset;
	dbginfo.dbg_callstack = code_dbgcallstack;
#endif

}



void code_termfunc( void )
{
	//		コードの終了処理
	//
	int i;
	int prmmax;
	STRUCTDAT *st;
	HSP3TYPEINFO *info;
	PVal *pval;

	//		モジュール変数デストラクタ呼び出し
	//
#ifdef HSPERR_HANDLE
	try {
#endif
	prmmax = hspctx->hsphed->max_val;
	pval = hspctx->mem_var;
	for(i=0;i<prmmax;i++) {
		if ( pval->flag == HSPVAR_FLAG_STRUCT ) code_delstruct_all( pval );
		pval++;
	}
#ifdef HSPERR_HANDLE
	}
	catch( ... ) {
	}
#endif

	//		クリーンアップモジュールの呼び出し
	//
#ifdef HSPERR_HANDLE
	try {
#endif
	prmmax = hspctx->hsphed->max_finfo / sizeof(STRUCTDAT);
	i = prmmax;
	while(1) {
		i--; if ( i < 0 ) break;
		st = &hspctx->mem_finfo[ i ];
		if (( st->index == STRUCTDAT_INDEX_FUNC )&&( st->funcflag & STRUCTDAT_FUNCFLAG_CLEANUP )) {
			code_callfunc( i );
		}
	}
#ifdef HSPERR_HANDLE
	}
	catch( ... ) {
	}
#endif

	//		タイプの終了関数をすべて呼び出す
	//
	for(i=tinfo_cur-1;i>=0;i--) {
		info = GetTypeInfoPtr( i );
		if ( info->termfunc != NULL ) info->termfunc( 0 );
	}

}


void code_bye( void )
{
	//		コード実行を終了
	//
	HspVarCoreBye();

	//		コード用のメモリを解放する
	//
	if ( hspctx->mem_irq != NULL ) sbFree( hspctx->mem_irq );

	sbFree( hspctx->cmdline );
	sbFree( hspctx->stmp );
	sbFree( hspctx->fnbuffer );
	sbFree( hspctx->refstr );

	sbFree( hsp3tinfo );
	StackTerm();
	sbBye();
}


int code_execcmd( void )
{
	//		命令実行メイン
	//
	int i;
	hspctx->endcode = 0;

rerun:
	hspctx->looplev = 0;
	hspctx->sublev = 0;
	StackReset();

#ifdef HSPERR_HANDLE
	try {
#endif
#ifdef HSPEMSCRIPTEN
		{
#else
		while(1) {
#endif
			//Alertf( "#%d,%d line%d",type,val,code_getdebug_line() );
			//Alertf( "#%d,%d",type,val );
			//printf( "#%d,%d  line%d\n",type,val,code_getdebug_line() );
			//stack->Reset();
			//stack->StoreLevel();
			//stack->ResumeLevel();

#ifdef HSPDEBUG
			if ( dbgmode ) code_dbgtrace();					// トレースモード時の処理
#endif

			if ( GetTypeInfoPtr( type )->cmdfunc( val ) ) {	// タイプごとの関数振り分け
				if ( hspctx->runmode == RUNMODE_RETURN ) {
					cmdfunc_return();
				} else {
					hspctx->msgfunc( hspctx );
				}
				if ( hspctx->runmode == RUNMODE_END ) {
					return RUNMODE_END;
//					i = hspctx->runmode;
//					break;
				}
			}
#ifdef HSPEMSCRIPTEN
			return RUNMODE_RUN;
#else
#endif
		}
#ifdef HSPERR_HANDLE
	}

	catch( HSPERROR code ) {						// HSPエラー例外処理
		if ( code == HSPERR_NONE ) {
			i = RUNMODE_END;
		} else if ( code == HSPERR_INTJUMP ) {
			goto rerun;
		} else if ( code == HSPERR_EXITRUN ) {
			i = RUNMODE_EXITRUN;
		} else {
			i = RUNMODE_ERROR;
			hspctx->err = code;
			hspctx->runmode = i;
			if ( code_isirq( HSPIRQ_ONERROR ) ) {
				code_sendirq( HSPIRQ_ONERROR, 0, (int)code, code_getdebug_line() );
				if ( hspctx->runmode != i ) goto rerun;
				return i;
			}
		}
	}
#endif

#ifdef SYSERR_HANDLE
	catch( ... ) {									// その他の例外発生時
		hspctx->err = HSPERR_UNKNOWN_CODE;
		return RUNMODE_ERROR;
	}
#endif
	hspctx->runmode = i;
	return i;
}

#ifdef HSPEMSCRIPTEN
int code_execcmd_one( int& prev )
{
	//		命令実行メイン
	//
	int i;
	hspctx->endcode = 0;

rerun:
	if (prev == 0) {
		hspctx->looplev = 0;
		hspctx->sublev = 0;
		StackReset();
		prev = 1;
	}

#ifdef HSPERR_HANDLE
	try {
#endif
		{
			//Alertf( "#%d,%d line%d",type,val,code_getdebug_line() );
			//Alertf( "#%d,%d",type,val );
			//printf( "#%d,%d  line%d\n",type,val,code_getdebug_line() );
			//stack->Reset();
			//stack->StoreLevel();
			//stack->ResumeLevel();

#ifdef HSPDEBUG
			if ( dbgmode ) code_dbgtrace();					// トレースモード時の処理
#endif

			if ( GetTypeInfoPtr( type )->cmdfunc( val ) ) {	// タイプごとの関数振り分け
				if ( hspctx->runmode == RUNMODE_RETURN ) {
					cmdfunc_return();
				} else {
					hspctx->msgfunc( hspctx );
				}
				if ( hspctx->runmode == RUNMODE_END ) {
					return RUNMODE_END;
//					i = hspctx->runmode;
//					break;
				}
			}
			return RUNMODE_RUN;
		}
#ifdef HSPERR_HANDLE
	}

	catch( HSPERROR code ) {						// HSPエラー例外処理
		//printf( "#catch %d\n", code );
		if ( code == HSPERR_NONE ) {
			i = RUNMODE_END;
		} else if ( code == HSPERR_INTJUMP ) {
			goto rerun;
		} else if ( code == HSPERR_EXITRUN ) {
			i = RUNMODE_EXITRUN;
		} else {
			i = RUNMODE_ERROR;
			hspctx->err = code;
			hspctx->runmode = i;
			if ( code_isirq( HSPIRQ_ONERROR ) ) {
				code_sendirq( HSPIRQ_ONERROR, 0, (int)code, code_getdebug_line() );
				if ( hspctx->runmode != i ) {
					prev = 0;
					return RUNMODE_RUN;
				}
				return i;
			}
		}
	}
#endif

#ifdef SYSERR_HANDLE
	catch( ... ) {									// その他の例外発生時
		hspctx->err = HSPERR_UNKNOWN_CODE;
		return RUNMODE_ERROR;
	}
#endif
	hspctx->runmode = i;
	return i;
}
#endif

int code_execcmd2( void )
{
	//		部分的な実行を行なう(ENDSESSION用)
	//
	while(1) {
		if ( GetTypeInfoPtr( type )->cmdfunc( val ) ) {	// タイプごとの関数振り分け
			break;
		}
	}
	return hspctx->runmode;
}



/*------------------------------------------------------------*/
/*
		EVENT controller
*/
/*------------------------------------------------------------*/

static int call_eventfunc( int option, int event, int prm1, int prm2, void *prm3 )
{
	//		各タイプのイベントコールバックを呼び出す
	//
	int i,res;
	HSP3TYPEINFO *info;
	for( i=HSP3_TYPE_USER; i<tinfo_cur; i++) {
		info = GetTypeInfoPtr( i );
		if ( info->option & option ) {
			if ( info->eventfunc != NULL ) {
				res = info->eventfunc( event, prm1, prm2, prm3 );
				if ( res ) return res;
			}
		}
	}
	return 0;
}


static char *dirlist_target;
static short evcategory[]={
0,							// HSPEVENT_NONE
HSPEVENT_ENABLE_COMMAND,	// HSPEVENT_COMMAND
HSPEVENT_ENABLE_HSPIRQ,		// HSPEVENT_HSPIRQ
HSPEVENT_ENABLE_GETKEY,		// HSPEVENT_GETKEY
HSPEVENT_ENABLE_GETKEY,		// HSPEVENT_STICK
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FNAME
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FREAD
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FWRITE
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FEXIST
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FDELETE
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FMKDIR
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FCHDIR
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FCOPY
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FDIRLIST1
HSPEVENT_ENABLE_FILE,		// HSPEVENT_FDIRLIST2
HSPEVENT_ENABLE_PICLOAD,	// HSPEVENT_GETPICSIZE
HSPEVENT_ENABLE_PICLOAD,	// HSPEVENT_PICLOAD
};

int code_event( int event, int prm1, int prm2, void *prm3 )
{
	//		HSP内部イベント実行
	//		(result:0=Not care/1=Done)
	//
	int res;
	res = call_eventfunc( evcategory[event], event, prm1, prm2, prm3 ); 
	if ( res ) return res;

	switch( event ) {
	case HSPEVENT_COMMAND:
		// All commands (type,val,n/a)
		break;

	case HSPEVENT_HSPIRQ:
		// HSP Interrupt (IRQid,iparam,param_ptr)
		code_execirq( (IRQDAT *)prm3, prm1, prm2 );
		break;

	case HSPEVENT_GETKEY:
		// Key input (IDcode,option,resval ptr)
	case HSPEVENT_STICK:
		// Stick input (IDcode,option,resval ptr)
		break;

	case HSPEVENT_FNAME:
		// set FNAME (n/a,n/a,nameptr)
		strncpy( hspctx->fnbuffer, (char *)prm3, HSP_MAX_PATH-1 );
#ifdef HSP3IMP
		//	HSP3IMP用セキュリティ対応
		if ( SecurityCheck( hspctx->fnbuffer ) ) throw HSPERR_FILE_IO;
#endif
		break;
	case HSPEVENT_FREAD:
		// fread (fseek,size,loadptr)
		res = dpm_read( hspctx->fnbuffer, prm3, prm2, prm1 );
		if ( res < 0 ) throw HSPERR_FILE_IO;
		hspctx->strsize = res;
		break;
	case HSPEVENT_FWRITE:
		// fwrite (fseek,size,saveptr)
		res = mem_save( hspctx->fnbuffer, prm3, prm2, prm1 );
		if ( res < 0 ) throw HSPERR_FILE_IO;
		hspctx->strsize = res;
		break;
	case HSPEVENT_FEXIST:
		// exist (n/a,n/a,n/a)
		hspctx->strsize = dpm_exist( hspctx->fnbuffer );
		break;
	case HSPEVENT_FDELETE:
		// delete (n/a,n/a,n/a)
#ifdef HSP3IMP
		//	HSP3IMP用セキュリティ対応
		throw HSPERR_FILE_IO;
#endif
		if ( delfile( hspctx->fnbuffer ) == 0 ) throw HSPERR_FILE_IO;
		break;
	case HSPEVENT_FMKDIR:
		// mkdir (n/a,n/a,n/a)
		if ( makedir( hspctx->fnbuffer ) ) throw HSPERR_FILE_IO;
		break;
	case HSPEVENT_FCHDIR:
		// chdir (n/a,n/a,n/a)
		if ( changedir( hspctx->fnbuffer ) ) throw HSPERR_FILE_IO;
		break;
	case HSPEVENT_FCOPY:
		// bcopy (n/a,n/a,dst filename)
		if ( dpm_filecopy( hspctx->fnbuffer, (char *)prm3 ) ) throw HSPERR_FILE_IO;
		break;
	case HSPEVENT_FDIRLIST1:
		// dirlist1 (opt,n/a,result ptr**)
		{
		char **p;
		dirlist_target = sbAlloc( 0x1000 );
		hspctx->stat = dirlist( hspctx->fnbuffer, &dirlist_target, prm1 );
		p = (char **)prm3;
		*p = dirlist_target;
		break;
		}
	case HSPEVENT_FDIRLIST2:
		// dirlist2 (n/a,n/a,n/a)
		sbFree( dirlist_target );
		break;

	case HSPEVENT_GETPICSIZE:
		// getpicsize (n/a,n/a,resval ptr)
		break;
	case HSPEVENT_PICLOAD:
		// picload (n/a,n/a,HDC)
		break;

	}
	return 0;
}


void code_bload( char *fname, int ofs, int size, void *ptr )
{
	code_event( HSPEVENT_FNAME, 0, 0, fname );
	code_event( HSPEVENT_FREAD, ofs, size, ptr );
}


void code_bsave( char *fname, int ofs, int size, void *ptr )
{
	code_event( HSPEVENT_FNAME, 0, 0, fname );
	code_event( HSPEVENT_FWRITE, ofs, size, ptr );
}


/*------------------------------------------------------------*/
/*
		IRQ controller
*/
/*------------------------------------------------------------*/

IRQDAT *code_getirq( int id )
{
	return &hspctx->mem_irq[ id ];
}


void code_enableirq( int id, int sw )
{
	//		IRQの有効・無効切り替え
	//
	IRQDAT *irq;
	irq = code_getirq( id );
	if ( sw == 0 ) {
		irq->flag = IRQ_FLAG_DISABLE;
		return;
	}

	if ( id != HSPIRQ_USERDEF ) {
		if (( irq->opt == IRQ_OPT_CALLBACK && irq->callback == NULL)||
		    ( irq->opt != IRQ_OPT_CALLBACK && irq->ptr == NULL)) {
			 return;
		}
	}
	irq->flag = IRQ_FLAG_ENABLE;
}


void code_setirq( int id, int opt, int custom, unsigned short *ptr )
{
	//		IRQイベントを設定する
	//
	IRQDAT *irq;
	irq = code_getirq( id );
	irq->flag = IRQ_FLAG_ENABLE;
	irq->opt = opt;
	irq->ptr = ptr;
	irq->custom = custom;
}


int code_isirq( int id )
{
	//		指定したIRQイベントがENABLEかを調べる
	//
	if ( hspctx->mem_irq[ id ].flag != IRQ_FLAG_ENABLE ) return 0;
	return 1;
}


int code_sendirq( int id, int iparam, int wparam, int lparam )
{
	//		指定したIRQイベントを発生
	//
	IRQDAT *irq;
	irq = code_getirq( id );
	irq->iparam = iparam;
	code_event( HSPEVENT_HSPIRQ, wparam, lparam, irq );
	return hspctx->runmode;
}


int code_isuserirq( void )
{
	//		カスタム指定のIRQイベントがあるかどうか調べる
	//
	if ( hspctx->irqmax > HSPIRQ_USERDEF ) {
		if ( hspctx->mem_irq[ HSPIRQ_USERDEF ].flag == IRQ_FLAG_ENABLE ) return 1;
	}
	return 0;
}


int code_irqresult( int *value )
{
	//		IRQイベントの戻り値を取得する
	//
	*value = ( hspctx->stat );
	return ( hspctx->retval_level );
}


int code_checkirq( int id, int message, int wparam, int lparam )
{
	//		指定したメッセージに対応するイベントを発生
	//
	int i,cur;
	IRQDAT *irq;
	for( i=HSPIRQ_MAX; i<hspctx->irqmax; i++ ) {
		irq = &hspctx->mem_irq[ i ];
		if ( irq->custom == message ) {
			if ( irq->custom2 == id ) {
				if ( irq->flag == IRQ_FLAG_ENABLE ) {
					hspctx->intwnd_id = id;
					hspctx->retval_level = 0;
					cur = hspctx->sublev + 1;
					if ( irq->callback != NULL ) {
						irq->callback( irq, wparam, lparam );
					} else {
						code_sendirq( i, irq->custom, wparam, lparam );
						if ( hspctx->retval_level != cur ) return 0;		// returnの戻り値がなければ0を返す
					}
					return 1;
				}
			}
		}
	}
	return 0;
}


IRQDAT *code_seekirq( int actid, int custom )
{
	//		指定したcustomを持つIRQを検索する
	//
	int i;
	IRQDAT *irq;
	for( i=0; i<hspctx->irqmax; i++ ) {
		irq = code_getirq( i );
		if ( irq->flag != IRQ_FLAG_NONE ) {
			if (( irq->custom == custom )&&( irq->custom2 == actid )) {
				if ( irq->opt != IRQ_OPT_CALLBACK ) return irq;
			}
		}
	}
	return NULL;
}


IRQDAT *code_addirq( void )
{
	//		IRQを追加する
	//
	int id;
	IRQDAT *irq;
	id = hspctx->irqmax++;
	if ( hspctx->mem_irq == NULL) {
		hspctx->mem_irq = (IRQDAT *)sbAlloc( sizeof(IRQDAT) );
	} else {
		hspctx->mem_irq = (IRQDAT *)sbExpand( (char *)hspctx->mem_irq, sizeof(IRQDAT) * (hspctx->irqmax) );
	}
	irq = code_getirq( id );
	irq->flag = IRQ_FLAG_DISABLE;
	irq->opt = IRQ_OPT_GOTO;
	irq->custom = -1;
	irq->iparam = 0;
	irq->ptr = NULL;
	irq->callback = NULL;
	return irq;
}


void code_execirq( IRQDAT *irq, int wparam, int lparam )
{
	//		IRQを実行する
	//
	hspctx->iparam = irq->iparam;
	hspctx->wparam = wparam;
	hspctx->lparam = lparam;
	if ( irq->opt == IRQ_OPT_GOTO ) {
		code_setpci( irq->ptr );
	}
	if ( irq->opt == IRQ_OPT_GOSUB ) {
		mcs = mcsbak;
#ifdef HSPEMSCRIPTEN
		code_call( irq->ptr );
#else
		cmdfunc_gosub( (unsigned short *)irq->ptr );
		if ( hspctx->runmode != RUNMODE_END ) {
			hspctx->runmode = RUNMODE_RUN;
		}
#endif
	}
	//Alertf("sublev%d", hspctx->sublev );
}


/*------------------------------------------------------------*/
/*
		Debug support
*/
/*------------------------------------------------------------*/

#ifdef HSPDEBUG

int code_getdbgmode( void )
{
	return dbgmode;
}

static char *dbgbuf;

/*
	rev 49
	BT#190: return命令へ長い文字列を指定するとメモリアクセス違反が起こる
	に対処。

	実際はデバッグウィンドウで変数内容以外の長い文字列を表示するとバッファオーバーフローが起きていた。
*/

static void code_adddbg3( char const * s1, char const * sep, char const * s2 )
{
	char tmp[ 2048 ];
	strncpy( tmp, s1, 64 );
	strncat( tmp, sep, 8 );
	strncat( tmp, s2, 1973 );
	strcat( tmp, "\r\n" );
	sbStrAdd( &dbgbuf, tmp );
}


void code_adddbg( char * name, char * str )
{
	code_adddbg3( name, "\r\n", str );
}


void code_adddbg2( char * name, char * str )
{
	code_adddbg3( name, ":", str );
}


void code_adddbg( char *name, double val )
{
	char tmp[ 400 ];
	sprintf( tmp, "%-36.16f", val );
	code_adddbg( name, tmp );
}


void code_adddbg( char *name, int val )
{
	char tmp[32];
#ifdef HSPWIN
	_itoa( val, tmp, 10 );
#else
	sprintf( tmp, "%d", val);
#endif
	code_adddbg( name, tmp );
}


void code_adddbg2( char *name, int val )
{
	char tmp[32];
#ifdef HSPWIN
	_itoa( val, tmp, 10 );
#else
	sprintf( tmp, "%d", val);
#endif
	code_adddbg2( name, tmp );
}


char *code_inidbg( void )
{
	dbgbuf = sbAlloc( 0x4000 );
	return dbgbuf;
}


void code_dbg_global( void )
{
	HSPHED *hed;
	hed = hspctx->hsphed;
	code_adddbg( "axサイズ", hed->allsize );
	code_adddbg( "コードサイズ", hed->max_cs );
	code_adddbg( "データサイズ", hed->max_ds );
	code_adddbg( "変数予約", hed->max_val );
	code_adddbg( "実行モード", hspctx->runmode );
	code_adddbg( "stat", hspctx->stat );
	code_adddbg( "cnt", hspctx->mem_loop[hspctx->looplev].cnt );
	code_adddbg( "looplev", hspctx->looplev );
	code_adddbg( "sublev", hspctx->sublev );
	code_adddbg( "iparam", hspctx->iparam );
	code_adddbg( "wparam", hspctx->wparam );
	code_adddbg( "lparam", hspctx->lparam );
	code_adddbg( "refstr", hspctx->refstr );
	code_adddbg( "refdval", hspctx->refdval );
}


/*
	rev 53
	書き直し。
*/

static void code_dbgdump( char const * mem, int size )
{
	//		memory Hex dump
	//
	int adr = 0;
	char t[ 512 ];
	char tline[ 1024 ];
	while ( adr < size ) {
		sprintf( tline, "%04X", adr );
		for ( int i = 0; i < 8 && adr < size; ++i, ++adr ) {
			sprintf( t, " %02X", static_cast< unsigned char >( mem[ adr ] ) );
			strcat( tline, t );
		}
		strcat( tline, "\r\n" );
		sbStrAdd( &dbgbuf, tline );
	}
}


static void code_dbgvarinf_ext( PVal *pv, void *src, char *buf )
{
	//		特殊な変数の内容を取得
	//		(256bytes程度のバッファを確保しておいて下さい)
	//
	switch( pv->flag ) {
	case HSPVAR_FLAG_LABEL:
		sprintf( buf,"LABEL $%p", *(void **)src ); 
		break;
	case HSPVAR_FLAG_STRUCT:
		{
		FlexValue *fv;
		fv = (FlexValue *)src;
		if ( fv->type == FLEXVAL_TYPE_NONE ) {
			sprintf( buf,"STRUCT (Empty)" ); 
		} else {
			sprintf( buf,"STRUCT ID%d-%d PTR$%p SIZE%d(%d)", fv->myid, fv->customid, (void *)(fv->ptr), fv->size, fv->type ); 
		}
		break;
		}
#ifndef HSP_COM_UNSUPPORTED
	case HSPVAR_FLAG_COMSTRUCT:
		sprintf( buf,"COMPTR $%p", *(void **)src ); 
		break;
#endif
	default:
		strcpy( buf, "Unknown" );
		break;
	}
}


static void code_arraydump( PVal *pv )
{
	//		variable array dump
	//
	char t[512];
	PDAT *src;
	char *p;
	int ofs;
	int amax;
	int ok;

	amax = pv->len[1];
	if ( amax <= 1 ) return;
	if ( amax > 16 ) {
		sbStrAdd( &dbgbuf, "(配列の一部だけを表示)\r\n" );
		amax = 16;
	}

	for(ofs=0;ofs<amax;ofs++) {

		src = HspVarCorePtrAPTR( pv, ofs );
		ok = 1;
		try {
			p = (char *)HspVarCoreCnv( pv->flag, HSPVAR_FLAG_STR, src );
		} catch(...) {
			char tmpbuf[256];
			code_dbgvarinf_ext( pv, src, tmpbuf );
			sprintf( t,"(%d)=%s\r\n", ofs, tmpbuf ); 
			ok = 0;
		}
		if ( ok ) {
			if ( strlen(p) > 63 ) {
				strncpy( hspctx->stmp, p, 63 );
				hspctx->stmp[64]=0;
				p = hspctx->stmp;
			}
			sprintf( t,"(%d)=%s\r\n", ofs, p ); 
		}
		sbStrAdd( &dbgbuf, t );
	}
}


char *code_dbgvarinf( char *target, int option )
{
	//		変数情報取得
	//		option
	//			bit0 : sort ( 受け側で処理 )
	//			bit1 : module
	//			bit2 : array
	//			bit3 : dump
	//
	int i,id,max;
	char *name;
	HspVarProc *proc;
	PVal *pv;
	PDAT *src;
	char *p;
	char *padr;
	char tmp[256];
	int size;
	int orgsize;

	code_inidbg();
	max = hspctx->hsphed->max_val;

	if ( target == NULL ) {
		for(i=0;i<max;i++) {
			name = code_getdebug_varname( i );
			if ( strstr2( name, "@" ) != NULL ) {
				if (!( option & 2 )) name = NULL;
			}
			if ( name != NULL ) {
				sbStrAdd( &dbgbuf, name );
				sbStrAdd( &dbgbuf, "\r\n" );
			}
		}
		return dbgbuf;
	}

	id = 0;
	while(1) {
		if ( id >= max ) break;
		name = code_getdebug_varname(id);
		if ( strcmp( name, target ) == 0 ) break;
		id++;
	}

	pv = &hspctx->mem_var[id];
	proc = HspVarCoreGetProc(pv->flag);
	code_adddbg2( "変数名", name );
	code_adddbg2( "型", proc->vartype_name );
	sprintf( tmp, "(%d,%d,%d,%d)",pv->len[1],pv->len[2],pv->len[3],pv->len[4] );
	code_adddbg2( "配列", tmp );
	code_adddbg2( "モード", pv->mode );
	code_adddbg2( "使用サイズ", pv->size );

	HspVarCoreReset( pv );
	src = proc->GetPtr( pv );
	padr = (char *)proc->GetBlockSize( pv, src, &size );
	code_adddbg2( "バッファサイズ", size );

	switch( pv->flag ) {
	case HSPVAR_FLAG_STR:
	case HSPVAR_FLAG_DOUBLE:
	case HSPVAR_FLAG_INT:
		if ( pv->flag != HSPVAR_FLAG_STR ) {
			p = (char *)HspVarCoreCnv( pv->flag, HSPVAR_FLAG_STR, src );
		} else {
			p = padr;
		}
		orgsize = (int)strlen(p);
		if ( orgsize >= 1024 ) {
			strncpy( hspctx->stmp, p, 1023 );
			p = hspctx->stmp; p[1023] = 0;
			sprintf( tmp, "(内容%dbytesの一部を表示しています)\r\n",orgsize );
			sbStrAdd( &dbgbuf, tmp );
		}
		code_adddbg( "内容:", p );
		break;
	case HSPVAR_FLAG_LABEL:
	default:
		{
		char tmpbuf[256];
		code_dbgvarinf_ext( pv, src, tmpbuf );
		code_adddbg( "内容:", tmpbuf );
		break;
		}
	}

	if ( option & 4 ) {
		code_arraydump( pv );
	}
	if ( option & 8 ) {
		if ( size > 0x1000 ) size = 0x1000;
		code_dbgdump( padr, size );
	}


	return dbgbuf;
}


void code_dbgcurinf( void )
{
	unsigned short *bak;
	bak = mcsbak;
	mcsbak = mcs;
	dbginfo.line = code_getdebug_line();
	dbginfo.fname = code_getdebug_name();
	mcsbak = bak;
}


void code_dbgclose( char *buf )
{
	sbFree( dbgbuf );
}


HSP3DEBUG *code_getdbg( void )
{
	return &dbginfo;
}


char *code_dbgvalue( int type )
{
	//	ダミー用関数
	return code_inidbg();
}


int code_dbgset( int id )
{
	//	デバッグモード設定
	//
	switch( hspctx->runmode ) {
	case RUNMODE_STOP:
		if ( id != HSPDEBUG_STOP ) {
			hspctx->runmode = RUNMODE_RUN;
			if ( id == HSPDEBUG_RUN ) {
				dbgmode = HSPDEBUG_NONE;
			} else {
				dbgmode = id;
			}
			return 0;
		}
		break;
	case RUNMODE_WAIT:
	case RUNMODE_AWAIT:
		if ( id == HSPDEBUG_STOP ) {
			hspctx->runmode = RUNMODE_STOP;
			dbgmode = HSPDEBUG_NONE;
			return 0;
		}
		break;
	}
	return -1;
}

char *code_dbgcallstack( void )
{
	STMDATA* it;
	HSPROUTINE* routine;
	int  line;

	char tmp[HSP_MAX_PATH + 5 + 1];

	code_inidbg();

	for (it = mem_stm; it != stm_cur; it++)
	{
		if (it->type == TYPE_EX_SUBROUTINE ||
			it->type == TYPE_EX_CUSTOMFUNC)
		{
			routine = (HSPROUTINE *)STM_GETPTR(it);

			line = code_getdebug_line(routine->mcsret);
			sprintf(tmp, "%s:%4d\r\n", code_getdebug_name(), line);
			sbStrAdd(&dbgbuf, tmp);
		}
	}

	return dbgbuf;
}

void code_dbgtrace( void )
{
	//	トレース処理
	//
	int i;
	i = dbginfo.line;
	code_dbgcurinf();
	if ( i != dbginfo.line ) {
		hspctx->runmode = RUNMODE_STOP;
		hspctx->msgfunc( hspctx );
	}
}

#endif


