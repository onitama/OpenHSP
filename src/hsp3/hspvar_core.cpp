
//
//	HSPVAR manager
//	onion software/onitama 2003/4
//
#include <stdio.h>
#include <stdlib.h>
#include "hspvar_core.h"
#include "hspvar_label.h"
#include "hsp3debug.h"

/*
	rev 43
	たぶんシステムヘッダ。それともカスタムヘッダを使うのか？ (naznyark)
*/
#include "string.h"
#include "strbuf.h"
#include "supio.h"

PVal *mem_pval;

extern void HspVarInt_Init( HspVarProc *p );
extern void HspVarStr_Init( HspVarProc *p );
extern void HspVarDouble_Init( HspVarProc *p );
extern void HspVarStruct_Init( HspVarProc *p );

/*------------------------------------------------------------*/
/*
		master pointer
*/
/*------------------------------------------------------------*/

HspVarProc *hspvarproc;
int hspvartype_max;
int hspvartype_limit;

void HspVarCoreInit( void )
{
	int i;
	hspvarproc = (HspVarProc *)sbAlloc( sizeof(HspVarProc) * HSPVAR_FLAG_MAX );
	hspvartype_max = HSPVAR_FLAG_MAX;
	for(i=0;i<HSPVAR_FLAG_MAX;i++) {
		hspvarproc[i].flag = 0;
	}

	//		mpval(テンポラリ変数)を初期化します
	//		(実態の初期化は、変数使用時に行なわれます)
	PVal *pval;
	mem_pval = (PVal *)sbAlloc( sizeof(PVal) * HSPVAR_FLAG_MAX );
	for(i=0;i<HSPVAR_FLAG_MAX;i++) {
		pval = &mem_pval[i];
		pval->mode = HSPVAR_MODE_NONE;
		pval->flag = HSPVAR_FLAG_INT;				// 仮の型
	}
}

void HspVarCoreBye( void )
{
	int i;
	for(i=0;i<hspvartype_max;i++) {
		if ( mem_pval[i].mode == HSPVAR_MODE_MALLOC ) {
			HspVarCoreDispose( &mem_pval[i] );
		}
	}
	sbFree( mem_pval );
	sbFree( hspvarproc );
}


void HspVarCoreResetVartype( int expand )
{
	//		VARTYPEを初期化する(HspVarCoreInitの後で呼ぶ)
	//		(expandに拡張するVARTYPEの数を指定する)
	//
	hspvartype_limit = hspvartype_max + expand;
	if ( expand >= 0 ) {
		hspvarproc = (HspVarProc *)sbExpand( (char *)hspvarproc, sizeof(HspVarProc) * hspvartype_limit );
		mem_pval = (PVal *)sbExpand( (char *)mem_pval, sizeof(PVal) * hspvartype_limit );
	}

	//		標準の型を登録する
	//
	HspVarCoreRegisterType( HSPVAR_FLAG_INT, (HSPVAR_COREFUNC)HspVarInt_Init );
	HspVarCoreRegisterType( HSPVAR_FLAG_STR, (HSPVAR_COREFUNC)HspVarStr_Init );
	HspVarCoreRegisterType( HSPVAR_FLAG_DOUBLE, (HSPVAR_COREFUNC)HspVarDouble_Init );
	HspVarCoreRegisterType( HSPVAR_FLAG_STRUCT, (HSPVAR_COREFUNC)HspVarStruct_Init );
	HspVarCoreRegisterType( HSPVAR_FLAG_LABEL, (HSPVAR_COREFUNC)HspVarLabel_Init );		// ラベル型(3.1)
}


int HspVarCoreAddType()
{
	int id;
	PVal *pval;
	if ( hspvartype_max >= hspvartype_limit ) return -1;
	id = hspvartype_max++;
	//hspvarproc = (HspVarProc *)sbExpand( (char *)hspvarproc, sizeof(HspVarProc) * hspvartype_max );
	hspvarproc[id].flag = 0;
	//mem_pval = (PVal *)sbExpand( (char *)mem_pval, sizeof(PVal) * hspvartype_max );
	pval = &mem_pval[id];
	pval->mode = HSPVAR_MODE_NONE;
	pval->flag = HSPVAR_FLAG_INT;					// 仮の型
	return id;
}


static void PutInvalid( void )
{
	throw( HSPERR_UNSUPPORTED_FUNCTION );
}


void HspVarCoreRegisterType( int flag, HSPVAR_COREFUNC func )
{
	int id;
	void **procs;
	HspVarProc *p;

	id = flag;
	if ( id < 0 ) {
		id = HspVarCoreAddType();
		if ( id < 0 ) return;
	}
	p = &hspvarproc[ id ];
	p->flag = p->aftertype = id;

	procs = (void **)(&p->Cnv);
	while(1) {
		*procs = (void *)(PutInvalid);
		if ( procs == (void **)(&p->LrI) ) break;
		procs++;
	}

	//	初期化関数の呼び出し

	func( p );
}


/*------------------------------------------------------------*/

void HspVarCoreDupPtr( PVal *pval, int flag, void *ptr, int size )
{
	//		指定されたポインタからのクローンになる
	//
	PDAT *buf;
	HspVarProc *p;
	p = &hspvarproc[ flag ];
	buf = (PDAT *)ptr;

	HspVarCoreDispose( pval );
	pval->pt = (char *)buf;
	pval->flag = flag;
	pval->size = size;
	pval->mode = HSPVAR_MODE_CLONE;
	pval->len[0] = 1;

	if ( p->basesize < 0 ) {
		pval->len[1] = 1;
	} else {
		pval->len[1] = size / p->basesize;
	}
	pval->len[2] = 0;
	pval->len[3] = 0;
	pval->len[4] = 0;
	pval->offset = 0;
	pval->arraycnt = 0;
	pval->support = HSPVAR_SUPPORT_STORAGE;
}


void HspVarCoreDup( PVal *pval, PVal *arg, APTR aptr )
{
	//		指定された変数のクローンになる
	//
	int size;
	PDAT *buf;
	HspVarProc *p;
	p = &hspvarproc[ arg->flag ];
	buf = HspVarCorePtrAPTR( arg, aptr );
	HspVarCoreGetBlockSize( arg, buf, &size );
	HspVarCoreDupPtr( pval, arg->flag, buf, size );
}


void HspVarCoreDim( PVal *pval, int flag, int len1, int len2, int len3, int len4 )
{
	//		配列を確保する
	//		(len1～len4は、4byte単位なので注意)
	//
	HspVarProc *p;
	p = &hspvarproc[ flag ];
	if ((len1<0)||(len2<0)||(len3<0)||(len4<0)) throw HSPVAR_ERROR_ILLEGALPRM;

	HspVarCoreDispose( pval );

	pval->flag = flag;
	pval->len[0] = 1;
	pval->offset = 0;
	pval->arraycnt = 0;
	pval->support = p->support;
	pval->len[1] = len1;
	pval->len[2] = len2;
	pval->len[3] = len3;
	pval->len[4] = len4;
	p->Alloc( pval, NULL );
}


void HspVarCoreDimFlex( PVal *pval, int flag, int len0, int len1, int len2, int len3, int len4 )
{
	//		配列を確保する(可変長配列用)
	//		(len1～len4は、4byte単位なので注意)
	//
	HspVarProc *p;
	p = &hspvarproc[ flag ];
	if ((len1<0)||(len2<0)||(len3<0)||(len4<0)) throw HSPVAR_ERROR_ILLEGALPRM;
	HspVarCoreDispose( pval );
	pval->flag = flag;
	pval->len[0] = len0;
	pval->offset = 0;
	pval->arraycnt = 0;
	pval->support = p->support;
	pval->len[1] = len1;
	pval->len[2] = len2;
	pval->len[3] = len3;
	pval->len[4] = len4;
	p->Alloc( pval, NULL );
	pval->len[0] = 1;
}


void HspVarCoreReDim( PVal *pval, int lenid, int len )
{
	//		配列を拡張する
	//
	HspVarProc *p;
	p = &hspvarproc[ pval->flag ];
	pval->len[lenid] = len;
	p->Alloc( pval, pval );
}


void HspVarCoreClear( PVal *pval, int flag )
{
	//		指定タイプの変数を最小メモリで初期化する
	//
	HspVarCoreDim( pval, flag, 1, 0, 0, 0 );	// 最小サイズのメモリを確保
}


void HspVarCoreClearTemp( PVal *pval, int flag )
{
	//		指定タイプの変数を最小メモリで初期化する(テンポラリ用)
	//
	HspVarCoreDim( pval, flag, 1, 0, 0, 0 );	// 最小サイズのメモリを確保
	pval->support |= HSPVAR_SUPPORT_TEMPVAR;
}


void *HspVarCoreCnvPtr( PVal *pval, int flag )
{
	//		指定されたtypeフラグに変換された値のポインタを得る
	//
	if ( pval->flag == flag ) {
		return hspvarproc[ flag ].GetPtr( pval );
	}
	//		型変換をする
	void *buf;
	buf = hspvarproc[ pval->flag ].GetPtr( pval );
	if ( pval->flag >= HSPVAR_FLAG_USERDEF ) {
		return ( hspvarproc[ pval->flag ].CnvCustom( buf, flag ) );
	}
	return ( hspvarproc[ flag ].Cnv( buf, pval->flag ) );
}


#if 0
PDAT *HspVarCorePtrAPTR( PVal *pv, APTR ofs )
{
	//		変数データの実態ポインタを得る
	//		(APTRとpvalから実態を求める)
	//
	pv->offset=ofs;
	return hspvarproc[(pv)->flag].GetPtr(pv);
}
#endif


HspVarProc *HspVarCoreSeekProc( const char *name )
{
	int i;
	HspVarProc *p;
	for(i=0;i<hspvartype_max;i++) {
		p = &hspvarproc[ i ];
		if ( p->flag ) {
			if ( strcmp( p->vartype_name, name )==0 ) {
				return p;
			}
		}
	}
	return NULL;
}


void HspVarCoreArray( PVal *pval, int offset )
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
		throw HSPVAR_ERROR_ARRAYOVER;
	}
	pval->offset += offset * pval->arraymul;
}


int HspVarCoreCountElems( PVal *pval )
{
	int k = pval->len[1];
	if ( pval->len[2] ) k *= pval->len[2];
	if ( pval->len[3] ) k *= pval->len[3];
	if ( pval->len[4] ) k *= pval->len[4];
	return k;
}


void HspVarCoreAllocPODArray( PVal *pval, const PVal *pval2, int basesize )
{
	//		pval変数が必要とするサイズを確保する。
	//		(pvalがすでに確保されているメモリ解放は呼び出し側が行なう)
	//		(flagの設定は呼び出し側が行なう)
	//		(pval2がNULLの場合は、新規データ)
	//		(pval2が指定されている場合は、pval2の内容を継承して再確保)
	//

	// 配列を最低1は確保する
	if ( pval->len[1] < 1 ) pval->len[1] = 1;

	int size = HspVarCoreCountElems(pval) * basesize;
	int old_size = (pval2 ? pval2->size : 0);

	char *pt;
	if ( pval == pval2 ) {
		if ( size > STRBUF_BLOCKSIZE ) {
			size = (size > old_size ? size + size / 8 : old_size);
		}
		pt = sbExpand(pval->pt, size);
	} else {
		pt = sbAlloc(size);

		if ( pval2 != NULL ) {
			memcpy(pt, pval2->pt, pval2->size);
			sbFree(pval->pt);
		}
	}

	// 新規要素を0埋め
	if ( size > old_size ) {
		memset(pt + old_size, 0, (size - old_size));
	}

	pval->pt = pt;
	pval->size = size;
	pval->mode = HSPVAR_MODE_MALLOC;
}

/*------------------------------------------------------------*/


