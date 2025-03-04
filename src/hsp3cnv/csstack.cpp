
//
//		CS Stack Manager class
//			onion software/onitama 2008/5
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "supio.h"
#include "csstack.h"

//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CsStack::CsStack( void )
{
	buf = new CMemBuf;
	Reset();
}


void CsStack::Reset( void )
{
	oplev = 0;
}


CsStack::~CsStack( void )
{
	delete buf;
}


OPSTACK *CsStack::Push( int type, int val )
{
	OPSTACK *op;
	op = &opstack[oplev];
	op->type = type;
	op->val = val;
	op->opt = NULL;
	oplev++;
	if ( oplev >= OPSTACK_MAX ) {
		Alert( "Stack Overflow!" ); return op;
	}
	return op;
}


void CsStack::Pop( void )
{
	if ( oplev < 1 ) {
		Alert( "CsStack:Pop error" );
		return;
	}
	oplev--;
}


void CsStack::SetExStr( OPSTACK *opstack, char *exstr )
{
	//		スタックに拡張文字列を積む
	//
	char *p;
	p = buf->GetBuffer() + buf->GetSize();
	buf->PutStr( exstr );
	opstack->opt = p;
}


OPSTACK *CsStack::Peek( void )
{
	if ( oplev < 1 ) {
		Alert( "CsStack:Peek error" );
		return NULL;
	}
	return &opstack[oplev-1];
}


OPSTACK *CsStack::Peek2( void )
{
	if ( oplev < 2 ) {
		Alert( "CsStack:Peek2 error" );
		return NULL;
	}
	return &opstack[oplev-2];
}

