
//
//	HSP3 stack support
//	(汎用スタックマネージャー)
//	(int,double,stringなどの可変長データをpush,popできます)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsp3config.h"
#include "stack.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

int stm_max;
STMDATA *mem_stm;
STMDATA *stm_cur;
STMDATA *stm_maxptr;

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

void StackInit( void )
{
	int i;
	STMDATA *stm;

	stm_max = STM_MAX_DEFAULT;
	mem_stm = (STMDATA *)malloc( sizeof( STMDATA ) * stm_max );
	stm_maxptr = mem_stm + stm_max;
	stm_cur = mem_stm;
	stm = mem_stm;
	for(i=0;i<stm_max;i++) {
		stm->type = HSPVAR_FLAG_INT;
		stm->mode = STMMODE_SELF;
		stm->ptr = (char *)&(stm->ival);
		stm++;
	}
}

void StackTerm( void )
{
	StackReset();
	free( mem_stm );
}


static inline void StackAlloc( STMDATA *stm, int size )
{
	if ( size <= STM_STRSIZE_DEFAULT ) {
//		stm->mode = STMMODE_SELF;
//		stm->ptr = (char *)&(stm->ival);
		return;
	}
	stm->mode = STMMODE_ALLOC;
	stm->ptr = (char *)malloc( size );
}

void StackReset( void )
{
	while(1) {
		if ( stm_cur == mem_stm ) break;
		StackPop();
	}
}

void StackPush( int type, char *data, int size )
{
	STMDATA *stm;
//	double *dptr;
	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm = stm_cur;
	stm->type = type;
	switch( type ) {
	case HSPVAR_FLAG_LABEL:
	case HSPVAR_FLAG_INT:
//		stm->mode = STMMODE_SELF;
		stm->ival = *(int *)data;
//		stm->ptr = (char *)&(stm->ival);
		stm_cur++;
		return;
	case HSPVAR_FLAG_DOUBLE:
		//dptr = (double *)&stm->ival;
		//*dptr = *(double *)data;
		memcpy(&stm->ival, data, sizeof(double));
//		stm->mode = STMMODE_SELF;
//		stm->ptr = (char *)dptr;
		stm_cur++;
		return;
	default:
		break;
	}
	StackAlloc( stm, size );
	memcpy( stm->ptr, data, size );
	stm_cur++;
}

void StackPush( int type, char *str )
{
	StackPush( type, str, (int)STRLEN(str)+1 );
}

void *StackPushSize( int type, int size )
{
	STMDATA *stm;
	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm = stm_cur;
	stm->type = type;
	StackAlloc( stm, size );
	stm_cur++;
	return (void *)stm->ptr;
}

void StackPushStr( char *str )
{
	StackPush( HSPVAR_FLAG_STR, str, (int)strlen(str)+1 );
}

void StackPushTypeVal( int type, int val, int val2 )
{
	STMDATA *stm;
	int *iptr;
//	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm = stm_cur;
	stm->type = type;
//	stm->mode = STMMODE_SELF;
	stm->ival = val;
	iptr = (int *)stm->itemp;
	*iptr = val2;
	stm_cur++;
}

void StackPushVar( void *pval, int aptr )
{
    STMDATA *stm;
    //	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
    stm = stm_cur;
    stm->type = -1;         // HSPVAR_FLAG_VAR
    //	stm->mode = STMMODE_SELF;
    stm->pval = pval;
    stm->ival = aptr;
    stm_cur++;
}

void StackPushType( int type )
{
	StackPushTypeVal( type, 0, 0 );
}

void StackPopFree( void )
{
	free( stm_cur->ptr );
	stm_cur->mode = STMMODE_SELF;
	stm_cur->ptr = (char *)&(stm_cur->ival);
}
