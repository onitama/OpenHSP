
//
//	strbuf.cpp header
//
#ifndef __strbuf_h
#define __strbuf_h

#include "hsp3config.h"
#include "hsp3struct.h"

#define STRBUF_BLOCKSIZE 64
#define STRBUF_BLOCK_DEFAULT 0x400
#define STRBUF_SEGMENT_DEFAULT 0x1000

#define STRINF_FLAG_NONE 0
#define STRINF_FLAG_USEINT 1
#define STRINF_FLAG_USEEXT 2


//	STRBUF structure
//

typedef struct STRBUF STRBUF;

typedef struct
{
	//	String Data structure
	//
	short flag;						// 使用フラグ(0=none/other=busy)
	short exflag;					// 拡張フラグ(未使用)
	STRBUF *intptr;					// 自身のアドレス
	size_t size;						// 確保サイズ
	char *ptr;						// バッファポインタ
	STRBUF *extptr;					// 外部バッファポインタ(STRINF)
	void *opt;						// オプション(ユーザー定義用)
} STRINF;

struct STRBUF
{
	//	String Data structure
	//
	STRINF inf;						// バッファ情報
	char data[STRBUF_BLOCKSIZE] HSP_ALIGN_DOUBLE;	// 内部バッファ
};

void sbInit( void );
void sbBye( void );

char *sbAlloc(size_t size );
char *sbAllocClear(size_t size );
void sbFree( void *ptr );
char *sbExpand( char *ptr, size_t size );
STRINF *sbGetSTRINF( char *ptr );

void sbCopy( char **ptr, const char *data, size_t size );
void sbStrCopy( char **ptr, const char *str );
void sbAdd( char **ptr, const char *data, size_t size, size_t offset );
void sbStrAdd( char **ptr, const char *str );

void *sbGetOption( char *ptr );
void sbSetOption( char *ptr, void *option );
void sbInfo( char *ptr );

#endif
