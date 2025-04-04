
//	BASE64/RC4 class
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "czbase64.h"

/*------------------------------------------------------------------------------------------*/

static char base64_map[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned char base64_offset[256] =
{
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

/*------------------------------------------------------------------------------------------*/

CzBase64::CzBase64( void )
{
	//	コンストラクタ
	//
}


CzBase64::~CzBase64( void )
{
	//	デストラクタ
	//
}


int CzBase64::Encode( char *dstptr, char *srcptr, int basesize )
{
	//	エンコード
	//
	int i,size;
	char a1,a2,a3;
	char *dst = dstptr;
	char *src = srcptr;

	size = basesize;
	for (i = 0; i < size - 2; i += 3) {
		a1 = *(src+i);
		a2 = *(src+i+1);
		a3 = *(src+i+2);
		*dst++ = *(base64_map + ((a1>>2)&0x3f));
		*dst++ = *(base64_map + ((a1<<4)&0x30 | (a2>>4)&0x0f));
		*dst++ = *(base64_map + ((a2<<2)&0x3c | (a3>>6)&0x03));
		*dst++ = *(base64_map + (a3&0x3f));
	}

	src += i;
	size -= i;

	if(size & 0x02 )	{
		a1 = *src;
		a2 = *(src+1);
		*dst++ = *(base64_map + ((a1>>2)&0x3f));
		*dst++ = *(base64_map + ((a1<<4)&0x30 | (a2>>4)&0x0f));
		*dst++ = *(base64_map + ((a2<<2)&0x3c));
		*dst++ = '=';
	}
	else if( size & 0x01 ) {
		a1 = *src;
		*dst++ = *(base64_map + ((a1>>2)&0x3f));
		*dst++ = *(base64_map + ((a1<<4)&0x30));
		*dst++ = '=';
		*dst++ = '=';
	}
	*dst = 0;
	return i;
}


int CzBase64::Decode( char *dstptr, char *srcptr, int size )
{
	//	デコード
	//
	int i;
	char a1,a2,a3,a4;
	char *dst = dstptr;
	char *src = srcptr;

	for (i = 0; i < size; i += 4) {
		a1 = *(src+i);
		a2 = *(src+i+1);
		a3 = *(src+i+2);
		a4 = *(src+i+3);
		*dst++ = (base64_offset[a1]<<2 | base64_offset[a2]>>4);
		*dst++ = (base64_offset[a2]<<4 | base64_offset[a3]>>2);
		*dst++ = (base64_offset[a3]<<6 | base64_offset[a4] );
	}
	src += i;
	if(*(src-2) == '=') {
		*(dst--) = 0;
		*(dst--) = 0;
	}
	else if(*(src-1) == '=') {
		*(dst--) = 0;
	}
	*dst = 0;
	return i;
}


int CzBase64::GetSize( int len )
{
	//		エンコードに必要なサイズを得る
	//
	int npad = len%3;
	int size = (npad > 0)? (len +3-npad ) : len;
	return  (size*8)/6;
}


/*------------------------------------------------------------------------------------------*/

#define SWAP(a, b) (a)^=(b); (b)^=(a); (a)^=(b)

CzRC4::CzRC4( void )
{
	//	コンストラクタ
	//
}


CzRC4::~CzRC4( void )
{
	//	デストラクタ
	//
}

void CzRC4::Swap(unsigned char *a, unsigned char *b)
{
	unsigned char swapTemp;
	swapTemp = *a;
	*a = *b;
	*b = swapTemp;
}

void CzRC4::SetKey( char *szkey )
{
	//	キーワードをセット
	//
	unsigned char index1;
	unsigned char index2;
	unsigned char *kp;
	int i;
	int keylen;

	kp = (unsigned char *)szkey;
	keylen = (int)strlen( szkey );
	for(i = 0; i < 256; i++) {
		state[i] = (unsigned char)i;
	}
	xs = 0;
    ys = 0;
	index1 = 0;
	index2 = 0;
	for(i = 0; i < 256; i++) {
		index2 = (kp[index1] + state[i] + index2) & 255;
		Swap(&state[i], &state[index2]);
		index1 = (index1 + 1) % keylen;
	}
}


int CzRC4::Encode( char *ptr, int size )
{
	unsigned char xorIndex;
	unsigned char *buffer_ptr;
	int buflen;
	int i;

	buflen = size;
	if ( buflen < 0 ) buflen = (int)strlen(ptr);
	buffer_ptr = (unsigned char *)ptr;

	for(i = 0; i < buflen; i++) {
		xs = (xs + 1) & 255;
		ys = (state[xs] + ys) & 255;
		Swap( &state[xs], &state[ys] );
		xorIndex = (state[xs] + state[ys]) & 255;
		*buffer_ptr++ ^= state[xorIndex];
	}
	return buflen;
}

/*------------------------------------------------------------------------------------------*/

