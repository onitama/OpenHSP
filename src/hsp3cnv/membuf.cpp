
//
//		Memory buffer class
//			onion software/onitama 2002/2
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "membuf.h"

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
	name[0] = 0;
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


char *CMemBuf::PreparePtr( int sz )
{
	//	バッファ拡張チェック
	//	(szサイズを書き込み可能なバッファを返す)
	//		(return:もとのバッファ先頭ptr)
	//
	int i;
	char *p;
	if ( (cur+sz) < size ) {
		p = mem_buf + cur;
		cur += sz;
		return p;
	}
	//	expand buffer (VCのreallocは怖いので使わない)
	i = size;
	while( i<=(cur+sz) ) i+=limit_size;
	p = (char *)malloc( i );
	memcpy( p, mem_buf, size );
	free( mem_buf );
	size = i;
	mem_buf = p;
	p = mem_buf + cur;
	cur += sz;
	return p;
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


char *CMemBuf::SlideDataBuffer( int addsize )
{
	//	データエリアの先頭からsize分を後ろに詰める
	//
	int i,total;
	char *p;
	char *p2;
	total = GetSize();
	p = PreparePtr( addsize );
	p2 = mem_buf + cur;

	for(i=0;i<total;i++) {
		p2--;
		p--;
		*p2 = *p;
	}
	return mem_buf;
}


void CMemBuf::Index( void )
{
	RegistIndex( cur );
}


void CMemBuf::Put( int data )
{
	int *p;
	p = (int *)PreparePtr( sizeof(int) );
	*p = data;
}


void CMemBuf::Put( short data )
{
	short *p;
	p = (short *)PreparePtr( sizeof(short) );
	*p = data;
}


void CMemBuf::Put( char data )
{
	char *p;
	p = PreparePtr( 1 );
	*p = data;
}


void CMemBuf::Put( float data )
{
	float *p;
	p = (float *)PreparePtr( sizeof(float) );
	*p = data;
}


void CMemBuf::Put( double data )
{
	double *p;
	p = (double *)PreparePtr( sizeof(double) );
	*p = data;
}


void CMemBuf::PutStr( char *data )
{
	char *p;
	p = PreparePtr( (int)strlen(data) );
	strcpy( p, data );
}


void CMemBuf::PutCR( void )
{
	char *p;
	p = PreparePtr( 2 );
	*p++ = 13; *p++ = 10;
}


void CMemBuf::PutData( void *data, int sz )
{
	char *p;
	p = PreparePtr( sz );
	memcpy( p, (char *)data, sz );
}


int CMemBuf::PutFile( char *fname )
{
	//		バッファに指定ファイルの内容を追加
	//		(return:ファイルサイズ(-1=error))
	//
	char *p;
	int length;
	FILE *ff;

	ff=fopen( fname,"rb" );
	if (ff==NULL) return -1;
	fseek( ff,0,SEEK_END );
	length=(int)ftell( ff );			// normal file size
	fclose(ff);
	if (length < 0) return -1;

	p = PreparePtr( length+1 );
	ff=fopen( fname,"rb" );
	fread( p, 1, length, ff );
	fclose(ff);
	p[length]=0;
	
	strcpy( name,fname );
	return length;
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


int CMemBuf::GetSize( void )
{
	return cur;
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
	if ( idxflag==0 ) return -1;
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
	int j;
	if ( idxflag==0 ) return -1;
	j = -1;
	for(i=0;i<cur;i++) {
		if ( idxbuf[i] == val ) j=i;
	}
	return j;
}


int CMemBuf::SaveFile( char *fname )
{
	//		バッファをファイルにセーブ
	//		(return:ファイルサイズ(-1=error))
	//
	FILE *fp;
	int flen;
	fp=fopen(fname,"wb");
	if (fp==NULL) return -1;
	flen = (int)fwrite( mem_buf, 1, cur, fp );
	fclose(fp);
	strcpy( name,fname );
	return flen;
}


char *CMemBuf::GetFileName( void )
{
	//		ファイル名を取得
	//
	return name;
}


void CMemBuf::MoveCursor( int value )
{
	//		書き込みカーソルを相対移動
	//
	cur += value;
}

