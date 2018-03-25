
//
//	HSPVAR core module
//	onion software/onitama 2007/1
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsp3code.h"
#include "hspvar_core.h"
#include "hsp3debug.h"
#include "hspvar_label.h"

#include "strbuf.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		HSPVAR core interface (label)
*/
/*------------------------------------------------------------*/

#define GetPtr(pval) ((HSPVAR_LABEL *)pval)

// Core
static PDAT *HspVarLabel_GetPtr( PVal *pval )
{
	return (PDAT *)(( (HSPVAR_LABEL *)(pval->pt))+pval->offset);
}


static int GetVarSize( PVal *pval )
{
	//		PVALポインタの変数が必要とするサイズを取得する
	//		(sizeフィールドに設定される)
	//
	return HspVarCoreCountElems(pval) * sizeof(HSPVAR_LABEL);
}


static void HspVarLabel_Free( PVal *pval )
{
	//		PVALポインタの変数メモリを解放する
	//
	if ( pval->mode == HSPVAR_MODE_MALLOC ) { sbFree( pval->pt ); }
	pval->pt = NULL;
	pval->mode = HSPVAR_MODE_NONE;
}


static void HspVarLabel_Alloc( PVal *pval, const PVal *pval2 )
{
	//		pval変数が必要とするサイズを確保する。
	//		(pvalがすでに確保されているメモリ解放は呼び出し側が行なう)
	//		(flagの設定は呼び出し側が行なう)
	//		(pval2がNULLの場合は、新規データ)
	//		(pval2が指定されている場合は、pval2の内容を継承して再確保)
	//
	HspVarCoreAllocPODArray(pval, pval2, sizeof(HSPVAR_LABEL));
}

// Size
static int HspVarLabel_GetSize( const PDAT *pval )
{
	return sizeof(HSPVAR_LABEL);
}

// Using
static int HspVarLabel_GetUsing( const PDAT *pdat )
{
	//		(実態のポインタが渡されます)
	return ( *pdat != NULL );
}

// Set
static void HspVarLabel_Set( PVal *pval, PDAT *pdat, const void *in )
{
	*GetPtr(pdat) = *((HSPVAR_LABEL *)(in));
}

static void *GetBlockSize( PVal *pval, PDAT *pdat, int *size )
{
	*size = pval->size - ( ((char *)pdat) - pval->pt );
	return (pdat);
}

static void AllocBlock( PVal *pval, PDAT *pdat, int size )
{
}


/*------------------------------------------------------------*/

void HspVarLabel_Init( HspVarProc *p )
{
	p->Set = HspVarLabel_Set;
	p->GetPtr = HspVarLabel_GetPtr;
	p->GetSize = HspVarLabel_GetSize;
	p->GetUsing = HspVarLabel_GetUsing;
	p->GetBlockSize = GetBlockSize;
	p->AllocBlock = AllocBlock;

	p->Alloc = HspVarLabel_Alloc;
	p->Free = HspVarLabel_Free;

	p->vartype_name = "label";				// タイプ名
	p->version = 0x001;					// 型タイプランタイムバージョン(0x100 = 1.0)
	p->support = HSPVAR_SUPPORT_STORAGE | HSPVAR_SUPPORT_FLEXARRAY | HSPVAR_SUPPORT_VARUSE;
										// サポート状況フラグ(HSPVAR_SUPPORT_*)
	p->basesize = sizeof(HSPVAR_LABEL);	// １つのデータが使用するサイズ(byte) / 可変長の時は-1
}

/*------------------------------------------------------------*/

