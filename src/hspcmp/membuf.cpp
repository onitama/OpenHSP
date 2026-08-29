
//
//		Memory buffer class
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
#include "membuf.h"
#include "supio.h"

//-------------------------------------------------------------
//		Routines
//-------------------------------------------------------------

void CMemBuf::InitMemBuf( int sz )
{
	//	バッファ初期化
	size = sz;
	if ( size<0x1000 ) {
		size = 0x1000;
	} else if ( size<0x4000 ) {
		size = 0x4000;
	} else {
		size = 0x10000;
	}
	limit_size = size;
	mem_buf = (char *)malloc( limit_size );
	mem_buf[0] = 0;
	name.clear();
	cur = 0;
	//	Indexバッファ初期化
	idxflag = 0;
	idxmax = -1;
	curidx = 0;
	idxbuf = NULL;
}


void CMemBuf::InitIndexBuf( int sz )
{
	//	Indexバッファ初期化
	idxflag = 1;
	idxmax = sz;
	curidx = 0;
	idxbuf = (int *)malloc( sizeof(int)*sz );
}


char *CMemBuf::InitSubBuffer(int sz)
{
	sub_buf = (char*)malloc(sz);
	return sub_buf;
}


void CMemBuf::ExchangeSubToMainBuffer(int size)
{
	if (mem_buf != NULL) {
		free(mem_buf);
	}
	if (idxbuf != NULL) {
		free(idxbuf);
	}

	limit_size = size+1;
	mem_buf = (char*)malloc(limit_size);
	mem_buf[0] = 0;
	memcpy( mem_buf, sub_buf, limit_size);

	if (sub_buf != NULL) {
		free(sub_buf);
		sub_buf = NULL;
	}

	cur = size;

	//	Indexバッファ初期化
	idxflag = 0;
	idxmax = -1;
	curidx = 0;
	idxbuf = NULL;
}


bool CMemBuf::TryPreparePtr( int sz, char **result )
{
	//	バッファ拡張チェック
	//	(szサイズを書き込み可能なバッファを返す)
	//		(result:もとのバッファ先頭ptr)
	//
	if (result == NULL || sz < 0 || cur < 0 || size < 0 || limit_size <= 0) {
		return false;
	}
	size_t required = (size_t)cur + (size_t)sz;
	if (required > INT_MAX) return false;
	if (required < (size_t)size) {
		*result = mem_buf + cur;
		cur += sz;
		return true;
	}

	//	expand buffer (VCのreallocは怖いので使わない)
	int new_size = size;
	while ((size_t)new_size <= required) {
		if (new_size > INT_MAX - limit_size) return false;
		new_size += limit_size;
	}
	char *p = (char *)malloc((size_t)new_size);
	if (p == NULL) return false;
	memcpy(p, mem_buf, (size_t)size);
	free( mem_buf );
	size = new_size;
	mem_buf = p;
	*result = mem_buf + cur;
	cur += sz;
	return true;
}


char *CMemBuf::PreparePtr( int sz )
{
	char *result = NULL;
	if (!TryPreparePtr(sz, &result)) return NULL;
	return result;
}


void CMemBuf::RegistIndex( int val )
{
	//	インデックスを登録
	int *p;
	if ( idxflag==0 ) return;
	idxbuf[ curidx++ ]= val;
	if ( curidx >= idxmax ) {
		idxmax+=256;
		p = (int *)malloc( sizeof(int)*idxmax );
		memcpy( p, idxbuf, sizeof(int)*curidx );
		free( idxbuf );
		idxbuf = p;
	}
}


void CMemBuf::Index(void)
{
	RegistIndex(cur);
}


void CMemBuf::IndexExclusive(void)
{
	if (SearchIndexValue(cur) < 0) {
		RegistIndex(cur);
	}
}


bool CMemBuf::Put( int data )
{
	char *p;
	p = PreparePtr( sizeof(int) );
	if (p == NULL) return false;
	memcpy( p, &data, sizeof(int) );
	return true;
}


bool CMemBuf::Put( short data )
{
	char *p;
	p = PreparePtr( sizeof(short) );
	if (p == NULL) return false;
	memcpy( p, &data, sizeof(short) );
	return true;
}


bool CMemBuf::Put( char data )
{
	char *p;
	p = PreparePtr( 1 );
	if (p == NULL) return false;
	*p = data;
	return true;
}


bool CMemBuf::Put( unsigned char data )
{
	unsigned char *p;
	p = (unsigned char *) PreparePtr( 1 );
	if (p == NULL) return false;
	*p = data;
	return true;
}


bool CMemBuf::Put( float data )
{
	char *p;
	p = PreparePtr( sizeof(float) );
	if (p == NULL) return false;
	memcpy( p, &data, sizeof(float) );
	return true;
}


bool CMemBuf::Put( double data )
{
	char *p;
	p = PreparePtr( sizeof(double) );
	if (p == NULL) return false;
	memcpy( p, &data, sizeof(data) );
	return true;
}


bool CMemBuf::PutStr( const char *data )
{
	char *p;
	if (data == NULL) return false;
	p = PreparePtr( strlen(data) );
	if (p == NULL) return false;
	strcpy( p, data );
	return true;
}


bool CMemBuf::PutStrDQ( char *data )
{
	//		ダブルクォート内専用str
	//
	unsigned char *src;
	unsigned char *p;
	unsigned char a1;
	unsigned char a2;
	int fl;
	if (data == NULL) return false;
	src = (unsigned char *)data;

	while(1) {
		a1 = *src++;
		if ( a1 == 0 ) break;

		fl = 0;
		if ( a1 == '\\' ) {					// \を\\に
			p = (unsigned char *) PreparePtr( 1 );
			if (p == NULL) return false;
			*p = a1;
		}
		if ( a1 == 13 ) {					// CRを\nに
			fl = 1; a2 = 10;
			if ( *src == 10 ) src++;
		}

		if (a1>=129) {						// 全角文字チェック
			if (a1<=159) { fl = 1; a2 = *src++; }
			else if (a1>=224) {  fl = 1; a2 = *src++; }
			if ( a2 == 0 ) break;
		}
		if ( fl ) {
			p = (unsigned char *) PreparePtr( 2 );
			if (p == NULL) return false;
			p[0] = a1;
			p[1] = a2;
			continue;
		}
		p = (unsigned char *) PreparePtr( 1 );
		if (p == NULL) return false;
		*p = a1;
	}
	return true;
}


bool CMemBuf::PutStrBlock( char *data )
{
	char *p;
	if (data == NULL) return false;
	p = PreparePtr( strlen(data)+1 );
	if (p == NULL) return false;
	strcpy( p, data );
	return true;
}


bool CMemBuf::PutCR( void )
{
	char *p;
	p = PreparePtr( 2 );
	if (p == NULL) return false;
	*p++ = 13; *p++ = 10;
	return true;
}


bool CMemBuf::PutData( void *data, int sz )
{
	char *p;
	if (data == NULL || sz < 0) return false;
	p = PreparePtr( sz );
	if (p == NULL) return false;
	memcpy( p, (char *)data, sz );
	return true;
}


#if ( WIN32 || _WIN32 ) && ! __CYGWIN__
# define VSNPRINTF _vsnprintf
#else
# define VSNPRINTF vsnprintf
#endif

bool CMemBuf::PutStrf( char *format, ... )
{
	va_list args;
	int c = cur;
	int space = size - cur;
	if (format == NULL) return false;
	while(1) {
		char *p = PreparePtr(space - 1);
		if (p == NULL) return false;
		cur = c;
		space = size - cur;
		int n;
		va_start(args, format);
		n = VSNPRINTF(p, space, format, args);
		va_end(args);
		if ( 0 <= n && n < space ) {
			cur += n;
			return true;
		}
		if ( 0 <= n ) {
			space = n + 1;
		} else {
			space *= 2;
		}
	}
}


int CMemBuf::PutFile( const char *fname )
{
	//		バッファに指定ファイルの内容を追加
	//		(return:ファイルサイズ(-1=error))
	//
	char *p;
	int length;
	FILE *ff;

	int64_t file_size = hsp_path_filesize(hsp_path::path_view(fname));
	if (file_size < 0 || file_size >= INT_MAX) return -1;
	length = (int)file_size;
	ff = hsp_path_fopen(hsp_path::path_view(fname), "rb");
	if (ff == NULL) return -1;

	int original_size = cur;
	if (!TryPreparePtr(length + 1, &p)) {
		fclose(ff);
		return -1;
	}
	if (fread(p, 1, length, ff) != (size_t)length) {
		fclose(ff);
		ReduceSize(original_size);
		return -1;
	}
	fclose(ff);
	p[length]=0;
	
	name = fname;
	return length;
}


void CMemBuf::ReduceSize( int new_cur )
{
	assert( new_cur >= 0 && new_cur <= cur );
	cur = new_cur;
}


//-------------------------------------------------------------
//		Interfaces
//-------------------------------------------------------------

CMemBuf::CMemBuf( void )
{
	//		空のバッファを初期化(64K)
	//
	InitMemBuf( 0x10000 );
}


CMemBuf::CMemBuf( int sz )
{
	//		指定サイズのバッファを初期化(64K)
	//
	InitMemBuf( sz );
}


CMemBuf::~CMemBuf( void )
{
	if ( mem_buf != NULL ) {
		free( mem_buf );
		mem_buf = NULL;
	}
	if ( idxbuf != NULL ) {
		free( idxbuf );
		idxbuf = NULL;
	}
}


void CMemBuf::AddIndexBuffer( void )
{
	InitIndexBuf( 256 );
}


void CMemBuf::AddIndexBuffer( int sz )
{
	InitIndexBuf( sz );
}


char *CMemBuf::GetBuffer( void )
{
	return mem_buf;
}


int CMemBuf::GetBufferSize( void )
{
	return size;
}


int *CMemBuf::GetIndexBuffer( void )
{
	return idxbuf;
}


void CMemBuf::SetIndex( int idx, int val )
{
	if ( idxflag==0 ) return;
	idxbuf[idx] = val;
}


int CMemBuf::GetIndex( int idx )
{
	if ( idxflag==0 ) return 0;
	if ((idx < 0) || ( idx>=curidx)) return 0;
	return idxbuf[idx];
}


int CMemBuf::GetIndexBufferSize( void )
{
	if ( idxflag==0 ) return -1;
	return curidx;
}


int CMemBuf::SearchIndexValue( int val )
{
	int i;
	if ( idxflag==0 ) return -1;
	for(i=0;i< curidx;i++) {
		if ( idxbuf[i] == val ) return i;
	}
	return -1;
}


int CMemBuf::SearchIndexedData(char* data, int size)
{
	int i;
	int j;
	int sz = size;
	if (idxflag == 0) return -1;
	if (sz < 0) sz = (int)strlen(data)+1;
	if (sz == 0) return -1;

	j = -1;
	for (i = 0; i < curidx; i++) {
		char* p = mem_buf + idxbuf[i];
		if (memcmp( p, data, sz )==0) j = idxbuf[i];
	}
	return j;
}


int CMemBuf::SaveFile( const char *fname )
{
	//		バッファをファイルにセーブ
	//		(return:ファイルサイズ(-1=error))
	//
	FILE *fp;
	int flen;
	fp=hsp_path_fopen(hsp_path::path_view(fname), "w+b");
	if (fp==NULL) return -1;
	flen = fwrite( mem_buf, 1, cur, fp );
	fclose(fp);
	name = fname;
	return flen;
}


char *CMemBuf::GetFileName( void )
{
	//		ファイル名を取得
	//
	return const_cast<char*>(name.c_str());
}
