
//
//	HSPVAR core module
//	onion software/onitama 2003/4
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hspvar_core.h"
#include "hsp3debug.h"
#include "strbuf.h"

/*------------------------------------------------------------*/
/*
		HSPVAR core interface (double)
*/
/*------------------------------------------------------------*/

#define GetPtr(pval) ((double *)pval)

static double conv;
static short *aftertype;

// Core
static PDAT *HspVarDouble_GetPtr( PVal *pval )
{
	return (PDAT *)(( (double *)(pval->pt))+pval->offset);
}

static void *HspVarDouble_Cnv( const void *buffer, int flag )
{
	//		リクエストされた型 -> 自分の型への変換を行なう
	//		(組み込み型にのみ対応でOK)
	//		(参照元のデータを破壊しないこと)
	//
	switch( flag ) {
	case HSPVAR_FLAG_STR:
		conv = (double)atof( (char *)buffer );
		return &conv;
	case HSPVAR_FLAG_INT:
		conv = (double)( *(int *)buffer );
		return &conv;
	case HSPVAR_FLAG_DOUBLE:
		break;
	default:
		throw HSPVAR_ERROR_TYPEMISS;
	}
	return (void *)buffer;
}

/*
static void *HspVarDouble_CnvCustom( const void *buffer, int flag )
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
	return HspVarCoreCountElems(pval) * sizeof(double);
}


static void HspVarDouble_Free( PVal *pval )
{
	//		PVALポインタの変数メモリを解放する
	//
	if ( pval->mode == HSPVAR_MODE_MALLOC ) { sbFree( pval->pt ); }
	pval->pt = NULL;
	pval->mode = HSPVAR_MODE_NONE;
}


static void HspVarDouble_Alloc( PVal *pval, const PVal *pval2 )
{
	HspVarCoreAllocPODArray(pval, pval2, sizeof(double));
}

/*
static void *HspVarDouble_ArrayObject( PVal *pval, int *mptype )
{
	//		配列要素の指定 (文字列/連想配列用)
	//
	throw HSPERR_UNSUPPORTED_FUNCTION;
	return NULL;
}
*/

// Size
static int HspVarDouble_GetSize( const PDAT *pval )
{
	return sizeof(double);
}

// Set
static void HspVarDouble_Set( PVal *pval, PDAT *pdat, const void *in )
{
	//*GetPtr(pdat) = *((double *)(in));
	memcpy(pdat, in, sizeof(double));
}

// Add
static void HspVarDouble_AddI( PDAT *pval, const void *val )
{
	*GetPtr(pval) += *((double *)(val));
	*aftertype = HSPVAR_FLAG_DOUBLE;
}

// Sub
static void HspVarDouble_SubI( PDAT *pval, const void *val )
{
	*GetPtr(pval) -= *((double *)(val));
	*aftertype = HSPVAR_FLAG_DOUBLE;
}

// Mul
static void HspVarDouble_MulI( PDAT *pval, const void *val )
{
	*GetPtr(pval) *= *((double *)(val));
	*aftertype = HSPVAR_FLAG_DOUBLE;
}

// Div
static void HspVarDouble_DivI( PDAT *pval, const void *val )
{
	double p = *((double *)(val));
	if ( p == 0.0 ) throw( HSPVAR_ERROR_DIVZERO );
	*GetPtr(pval) /= p;
	*aftertype = HSPVAR_FLAG_DOUBLE;
}

// Mod
static void HspVarDouble_ModI( PDAT *pval, const void *val )
{
	double p = *((double *)(val));
	double dval;
	if ( p == 0.0 ) throw( HSPVAR_ERROR_DIVZERO );
	dval = *GetPtr(pval);
	*GetPtr(pval) = fmod( dval, p );
	*aftertype = HSPVAR_FLAG_DOUBLE;
}


// Eq
static void HspVarDouble_EqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) == *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Ne
static void HspVarDouble_NeI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) != *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Gt
static void HspVarDouble_GtI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) > *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// Lt
static void HspVarDouble_LtI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) < *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// GtEq
static void HspVarDouble_GtEqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) >= *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

// LtEq
static void HspVarDouble_LtEqI( PDAT *pval, const void *val )
{
	*((int *)pval) = ( *GetPtr(pval) <= *((double *)(val)) );
	*aftertype = HSPVAR_FLAG_INT;
}

/*
// INVALID
static void HspVarDouble_Invalid( PDAT *pval, const void *val )
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

void HspVarDouble_Init( HspVarProc *p )
{
	aftertype = &p->aftertype;

	p->Set = HspVarDouble_Set;
	p->Cnv = HspVarDouble_Cnv;
	p->GetPtr = HspVarDouble_GetPtr;
//	p->CnvCustom = HspVarDouble_CnvCustom;
	p->GetSize = HspVarDouble_GetSize;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

//	p->ArrayObject = HspVarDouble_ArrayObject;
	p->Alloc = HspVarDouble_Alloc;
	p->Free = HspVarDouble_Free;

	p->AddI = HspVarDouble_AddI;
	p->SubI = HspVarDouble_SubI;
	p->MulI = HspVarDouble_MulI;
	p->DivI = HspVarDouble_DivI;
	p->ModI = HspVarDouble_ModI;

//	p->AndI = HspVarDouble_Invalid;
//	p->OrI  = HspVarDouble_Invalid;
//	p->XorI = HspVarDouble_Invalid;

	p->EqI = HspVarDouble_EqI;
	p->NeI = HspVarDouble_NeI;
	p->GtI = HspVarDouble_GtI;
	p->LtI = HspVarDouble_LtI;
	p->GtEqI = HspVarDouble_GtEqI;
	p->LtEqI = HspVarDouble_LtEqI;

//	p->RrI = HspVarDouble_Invalid;
//	p->LrI = HspVarDouble_Invalid;

	p->vartype_name = "double";				// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_STORAGE|HSPVAR_SUPPORT_FLEXARRAY;
										// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = sizeof(double);		// １つのデータが使用するサイズ(byte) / 可変長の時は-1
}

/*------------------------------------------------------------*/

