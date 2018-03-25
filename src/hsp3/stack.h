
//
//	stack.cpp header
//
#ifndef __stack_h
#define __stack_h

#include "hsp3config.h"

#define STM_MAX_DEFAULT 512
#define STM_STRSIZE_DEFAULT 64

#define STMMODE_SELF 0
#define STMMODE_ALLOC 1

#include "hspvar_core.h"
#include "hsp3debug.h"

//	STMDATA structure
//
typedef struct
{
	//	Memory Data structure
	//
	short type;
	short mode;
	char *ptr;
	void *pval;
	int ival HSP_ALIGN_DOUBLE;
	char itemp[STM_STRSIZE_DEFAULT-4];		// data area padding
} STMDATA;

void StackInit( void );
void StackTerm( void );
void StackReset( void );
void StackPush( int type, char *data, int size );
void StackPush( int type, char *str );
void *StackPushSize( int type, int size );
void StackPushi( int val );
void StackPushStr( char *str );
void StackPushType( int type );
void StackPushTypeVal( int type, int val, int val2 );
void StackPushVar( void *pval, int aptr );
void StackPop( void );
void StackPopFree( void );

extern int stm_max;
extern STMDATA *mem_stm;
extern STMDATA *stm_cur;
extern STMDATA *stm_maxptr;

#define STM_GETPTR( pp ) ( pp->ptr ) 

#define StackPeek (stm_cur-1)
#define StackPeek2 (stm_cur-2)
#define PeekPtr ((void *)(stm_cur-1)->ptr)

#define StackGetLevel (stm_cur-mem_stm)
#define StackDecLevel stm_cur--

inline void StackPushi( int val )
{
//	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm_cur->type = HSPVAR_FLAG_INT;
	stm_cur->ival = val;
	stm_cur++;
}

inline void StackPushl( int val )
{
//	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm_cur->type = HSPVAR_FLAG_LABEL;
	stm_cur->ival = val;
	stm_cur++;
}

inline void StackPushd( double val )
{
	double *dptr;
//	if ( stm_cur >= stm_maxptr ) throw HSPERR_STACK_OVERFLOW;
	stm_cur->type = HSPVAR_FLAG_DOUBLE;
	dptr = (double *)&stm_cur->ival;
	*dptr = val;
	stm_cur++;
}

inline void StackPop( void )
{
//	if ( stm_cur <= mem_stm ) throw HSPERR_UNKNOWN_CODE;
	stm_cur--;
	if ( stm_cur->mode ) {
		StackPopFree();
	}
}

#endif
