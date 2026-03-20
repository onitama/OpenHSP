
//
//	HSPVAR core module
//	onion software/onitama 2003/4
//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "hspvar_core.h"
#include "hsp3debug.h"
#include "strbuf.h"

/*------------------------------------------------------------*/
/*
		HSPVAR core interface (int64_t)
*/
/*------------------------------------------------------------*/

#define GetPtr(pval) ((int64_t *)pval)

static int64_t conv;
static short *aftertype;

// Core
static PDAT *HspVarInt64_GetPtr( PVal *pval )
{
	return (PDAT *)(( (int64_t *)(pval->pt))+pval->offset);
}

static void *HspVarInt64_Cnv( const void *buffer, int flag )
{
	//		リクエストされた型 -> 自分の型への変換を行なう
	//		(組み込み型にのみ対応でOK)
	//		(参照元のデータを破壊しないこと)
	//
	switch( flag ) {
	case HSPVAR_FLAG_STR:
		conv = atoll( (char *)buffer );
		return &conv;
	case HSPVAR_FLAG_INT:
		conv = (int64_t)( *(int *)buffer );
		return &conv;
	case HSPVAR_FLAG_DOUBLE:
		conv = (int64_t)( *(double *)buffer );
		return &conv;
	case HSPVAR_FLAG_INT64:
		break;
	default:
		throw HSPVAR_ERROR_TYPEMISS;
	}
	return (void *)buffer;
}

/*
static void *HspVarInt64_CnvCustom( const void *buffer, int flag )
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
	return HspVarCoreCountElems(pval) * sizeof(int64_t);
}


static void HspVarInt64_Free( PVal *pval )
{
	//		PVALポインタの変数メモリを解放する
	//
	if ( pval->mode == HSPVAR_MODE_MALLOC ) { sbFree( pval->pt ); }
	pval->pt = NULL;
	pval->mode = HSPVAR_MODE_NONE;
}


static void HspVarInt64_Alloc( PVal *pval, const PVal *pval2 )
{
	HspVarCoreAllocPODArray(pval, pval2, sizeof(int64_t));
}

/*
static void *HspVarInt64_ArrayObject( PVal *pval, int *mptype )
{
	//		配列要素の指定 (文字列/連想配列用)
	//
	throw HSPERR_UNSUPPORTED_FUNCTION;
	return NULL;
}
*/

// Size
static int HspVarInt64_GetSize( const PDAT *pval )
{
	return sizeof(int64_t);
}

// Set
static void HspVarInt64_Set( PVal *pval, PDAT *pdat, const void *in )
{
	//*GetPtr(pdat) = *((int64_t *)(in));
	memcpy(pdat, in, sizeof(int64_t));
}

// Add
static void HspVarInt64_AddI( PDAT *pval, const void *val )
{
	*GetPtr(pval) += *((int64_t *)(val));
	*aftertype = HSPVAR_FLAG_INT64;
}

// Sub
static void HspVarInt64_SubI( PDAT *pval, const void *val )
{
	*GetPtr(pval) -= *((int64_t *)(val));
	*aftertype = HSPVAR_FLAG_INT64;
}

// Mul
static void HspVarInt64_MulI( PDAT *pval, const void *val )
{
	*GetPtr(pval) *= *((int64_t *)(val));
	*aftertype = HSPVAR_FLAG_INT64;
}

// Div
static void HspVarInt64_DivI( PDAT *pval, const void *val )
{
	int64_t p = *((int64_t *)(val));
	if ( p == 0 ) throw( HSPVAR_ERROR_DIVZERO );
	*GetPtr(pval) /= p;
	*aftertype = HSPVAR_FLAG_INT64;
}

// Mod
static void HspVarInt64_ModI( PDAT *pval, const void *val )
{
	int64_t p = *((int64_t *)(val));
	if (p == 0) throw(HSPVAR_ERROR_DIVZERO);
	*GetPtr(pval) %= p;
	*aftertype = HSPVAR_FLAG_INT64;
}


// Eq
static void HspVarInt64_EqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) == *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Ne
static void HspVarInt64_NeI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) != *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Gt
static void HspVarInt64_GtI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) > *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Lt
static void HspVarInt64_LtI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) < *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// GtEq
static void HspVarInt64_GtEqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) >= *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// LtEq
static void HspVarInt64_LtEqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) <= *((int64_t *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

/*
// INVALID
static void HspVarInt64_Invalid( PDAT *pval, const void *val )
{
	throw( HSPVAR_ERROR_INVALID );
}
*/

static void *GetBlockSize( PVal *pval, PDAT *pdat, int *size )
{
	*size = pval->size - ( ((char *)pdat) - pval->pt );
	return (pdat);
}

static void AllocBlock( PVal *pval, PDAT *pdat, int size )
{
}


/*------------------------------------------------------------*/

void HspVarInt64_Init( HspVarProc *p )
{
	aftertype = &p->aftertype;

	p->Set = HspVarInt64_Set;
	p->Cnv = HspVarInt64_Cnv;
	p->GetPtr = HspVarInt64_GetPtr;
//	p->CnvCustom = HspVarInt64_CnvCustom;
	p->GetSize = HspVarInt64_GetSize;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

//	p->ArrayObject = HspVarInt64_ArrayObject;
	p->Alloc = HspVarInt64_Alloc;
	p->Free = HspVarInt64_Free;

	p->AddI = HspVarInt64_AddI;
	p->SubI = HspVarInt64_SubI;
	p->MulI = HspVarInt64_MulI;
	p->DivI = HspVarInt64_DivI;
	p->ModI = HspVarInt64_ModI;

//	p->AndI = HspVarInt64_Invalid;
//	p->OrI  = HspVarInt64_Invalid;
//	p->XorI = HspVarInt64_Invalid;

	p->EqI = HspVarInt64_EqI;
	p->NeI = HspVarInt64_NeI;
	p->GtI = HspVarInt64_GtI;
	p->LtI = HspVarInt64_LtI;
	p->GtEqI = HspVarInt64_GtEqI;
	p->LtEqI = HspVarInt64_LtEqI;

//	p->RrI = HspVarInt64_Invalid;
//	p->LrI = HspVarInt64_Invalid;

	p->vartype_name = "int64";				// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_STORAGE|HSPVAR_SUPPORT_FLEXARRAY;
										// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = sizeof(int64_t);		// １つのデータが使用するサイズ(byte) / 可変長の時は-1
}

/*------------------------------------------------------------*/

