
//
//	HSPVAR core module
//	onion software/onitama 2003/4
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hspvar_core.h"
#include "hsp3debug.h"

#include "strbuf.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		HSPVAR core interface (str)
*/
/*------------------------------------------------------------*/

#define GetPtr(pval) ((char *)pval)

static char conv[400];
static HspVarProc *myproc;

static char **GetFlexBufPtr( PVal *pval, int num )
{
	//		可変長バッファのポインタを得る
	//
	char **pp;
	if ( num == 0 ) return &(pval->pt);		// ID#0は、ptがポインタとなる
	pp = (char **)(pval->master);
	return &pp[num];
}



// Core
static PDAT *HspVarStr_GetPtr( PVal *pval )
{
	char **pp;
	pp = GetFlexBufPtr( pval, pval->offset );
	return (PDAT *)( *pp );
}

static void *HspVarStr_Cnv( const void *buffer, int flag )
{
	//		リクエストされた型 -> 自分の型への変換を行なう
	//		(組み込み型にのみ対応でOK)
	//		(参照元のデータを破壊しないこと)
	//
	switch( flag ) {
	case HSPVAR_FLAG_INT:
#ifdef HSPWIN
		_itoa( *(int *)buffer, conv, 10 );
#else
		sprintf( conv, "%d", *(int*)buffer);
#endif
		return conv;
	case HSPVAR_FLAG_STR:
		break;
	case HSPVAR_FLAG_DOUBLE:
		//_gcvt( *(double *)buffer, 32, conv );
		sprintf( conv,"%f", *(double *)buffer );
		return conv;
	default:
		throw HSPVAR_ERROR_TYPEMISS;
	}
	return (void *)buffer;
}

/*
static void *HspVarStr_CnvCustom( const void *buffer, int flag )
{
	//		(カスタムタイプのみ)
	//		自分の型 -> リクエストされた型 への変換を行なう
	//		(組み込み型に対応させる)
	//		(参照元のデータを破壊しないこと)
	//
	return buffer;
}
*/

static int GetVarSize( PVal *pval )
{
	//		PVALポインタの変数が必要とするサイズを取得する
	//		(sizeフィールドに設定される)
	//
	int size;
	size = pval->len[1];
	if ( pval->len[2] ) size*=pval->len[2];
	if ( pval->len[3] ) size*=pval->len[3];
	if ( pval->len[4] ) size*=pval->len[4];
	size *= sizeof(char *);
	pval->size = size;
	return size;
}

static void HspVarStr_Free( PVal *pval )
{
	//		PVALポインタの変数メモリを解放する
	//
	char **pp;
	int i,size;
	if ( pval->mode == HSPVAR_MODE_MALLOC ) {
		size = GetVarSize( pval );
		for(i=0;i<(int)(size/sizeof(char *));i++) {
			pp = GetFlexBufPtr( pval, i );
			sbFree( *pp );
		}
		free( pval->master );
	}
	pval->mode = HSPVAR_MODE_NONE;
}


static void HspVarStr_Alloc( PVal *pval, const PVal *pval2 )
{
	//		pval変数が必要とするサイズを確保する。
	//		(pvalがすでに確保されているメモリ解放は呼び出し側が行なう)
	//		(pval2がNULLの場合は、新規データ。len[0]に確保バイト数が代入される)
	//		(pval2が指定されている場合は、pval2の内容を継承して再確保)
	//
	char **pp;
	int i, i2, size, bsize;
	PVal oldvar;
	if ( pval->len[1] < 1 ) pval->len[1] = 1;		// 配列を最低1は確保する
	if ( pval2 != NULL ) oldvar = *pval2;			// 拡張時は以前の情報を保存する

	size = GetVarSize( pval );
	pval->mode = HSPVAR_MODE_MALLOC;
	pval->master = (char *)calloc( size, 1 );
	if ( pval->master == NULL ) throw HSPERR_OUT_OF_MEMORY;

	if ( pval2 == NULL ) {							// 配列拡張なし
		bsize = pval->len[0];
		if ( bsize < STRBUF_BLOCKSIZE ) { bsize = STRBUF_BLOCKSIZE; }
		for(i=0;i<(int)(size/sizeof(char *));i++) {
			pp = GetFlexBufPtr( pval, i );
			*pp = sbAllocClear( bsize );
			sbSetOption( *pp, (void *)pp );
		}
		return;
	}

	i2 = oldvar.size / sizeof(char *);
	for(i=0;i<(int)(size/sizeof(char *));i++) {
		pp = GetFlexBufPtr( pval, i );
		if ( i>=i2 ) {
			*pp = sbAllocClear( 64 );				// 新規確保分
		} else {
			*pp = *GetFlexBufPtr( &oldvar, i );		// 確保済みバッファ
		}
		sbSetOption( *pp, (void *)pp );
	}
	free( oldvar.master );
}


/*
static void *HspVarStr_ArrayObject( PVal *pval, int *arg )
{
	//		配列要素の指定 (文字列/連想配列用)
	//		( Reset後に次元数だけ連続で呼ばれます )
	//
	throw HSPERR_UNSUPPORTED_FUNCTION;
}
*/

// Size
static int HspVarStr_GetSize( const PDAT *pval )
{
	return (int)(strlen( (char *)pval ) + 1 );
}

// Set
static void HspVarStr_Set( PVal *pval, PDAT *pdat, const void *in )
{
	char **pp;
	if ( pval->mode == HSPVAR_MODE_CLONE ) {
		strncpy( (char *)pdat, (char *)in, pval->size );
		return;
	}
	pp = (char **)sbGetOption( (char *)pdat );
	sbStrCopy( pp, (char *)in );
	//strcpy( GetPtr(pval), (char *)in );
}

// Add
static void HspVarStr_AddI( PDAT *pval, const void *val )
{
	char **pp;
	pp = (char **)sbGetOption( (char *)pval );
	sbStrAdd( pp, (char *)val );
	//strcat( GetPtr(pval), (char *)val );
	myproc->aftertype = HSPVAR_FLAG_STR;
}

// Eq
static void HspVarStr_EqI( PDAT *pdat, const void *val )
{
	if ( strcmp( (char *)pdat, (char *)val ) ) {
		*(int *)pdat = 0;
	} else {
		*(int *)pdat = 1;
	}
	myproc->aftertype = HSPVAR_FLAG_INT;
}

// Ne
static void HspVarStr_NeI( PDAT *pdat, const void *val )
{
	int i;
	i = strcmp( (char *)pdat, (char *)val );
	if (i<0) i=-1;
	if (i>0) i=1;
	*(int *)pdat = i;
	myproc->aftertype = HSPVAR_FLAG_INT;
}

/*
// INVALID
static void HspVarStr_Invalid( PDAT *pval, const void *val )
{
	throw( HSPERR_UNSUPPORTED_FUNCTION );
}
*/

static void *GetBlockSize( PVal *pval, PDAT *pdat, int *size )
{
	STRINF *inf;
	if ( pval->mode == HSPVAR_MODE_CLONE ) {
		*size = pval->size;
		return pdat;
	}
	inf = sbGetSTRINF( (char *)pdat );
	*size = inf->size;
	return pdat;
}

static void AllocBlock( PVal *pval, PDAT *pdat, int size )
{
	char **pp;
	if ( pval->mode == HSPVAR_MODE_CLONE ) return;
	pp = (char **)sbGetOption( (char *)pdat );
	*pp = sbExpand( *pp, size );
}


/*------------------------------------------------------------*/

void HspVarStr_Init( HspVarProc *p )
{
	myproc = p;

	p->Set = HspVarStr_Set;
	p->Cnv = HspVarStr_Cnv;
	p->GetPtr = HspVarStr_GetPtr;
//	p->CnvCustom = HspVarStr_CnvCustom;
	p->GetSize = HspVarStr_GetSize;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

//	p->ArrayObject = HspVarStr_ArrayObject;
	p->Alloc = HspVarStr_Alloc;
	p->Free = HspVarStr_Free;

	p->AddI = HspVarStr_AddI;
//	p->SubI = HspVarStr_Invalid;
//	p->MulI = HspVarStr_Invalid;
//	p->DivI = HspVarStr_Invalid;
//	p->ModI = HspVarStr_Invalid;

//	p->AndI = HspVarStr_Invalid;
//	p->OrI  = HspVarStr_Invalid;
//	p->XorI = HspVarStr_Invalid;

	p->EqI = HspVarStr_EqI;
	p->NeI = HspVarStr_NeI;
//	p->GtI = HspVarStr_Invalid;
//	p->LtI = HspVarStr_Invalid;
//	p->GtEqI = HspVarStr_Invalid;
//	p->LtEqI = HspVarStr_Invalid;

//	p->RrI = HspVarStr_Invalid;
//	p->LrI = HspVarStr_Invalid;

	p->vartype_name = "str";			// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_FLEXSTORAGE | HSPVAR_SUPPORT_FLEXARRAY;
										// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = -1;					// １つのデータが使用するサイズ(byte) / 可変長の時は-1
}

/*------------------------------------------------------------*/

