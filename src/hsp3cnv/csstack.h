
//
//	csstack.cpp structures
//
#ifndef __csstack_h
#define __csstack_h

#include "membuf.h"

//	計算式スタック用構造体
//
typedef struct OPSTACK {
	int		type;
	int		val;
	char	*opt;
} OPSTACK;

#define OPSTACK_MAX 256		// 計算式スタックの最大数

//  CsStack manager class
class CsStack {
public:
	CsStack();
	~CsStack();
	void Reset( void );
	OPSTACK *Push( int type, int val );
	void Pop( void );
	OPSTACK *Peek( void );
	OPSTACK *Peek2( void );
	void SetExStr( OPSTACK *opstack, char *exstr );
	int GetLevel( void ) { return oplev; };

private:
	//	data
	int oplev;
	OPSTACK opstack[OPSTACK_MAX];
	CMemBuf *buf;
};


#endif
