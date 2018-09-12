//		スクリプトエディタ
//		created by K-K (http://www.binzume.net/)
//		modified by onitama for OpenHSP
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "supio.h"

#ifndef _MAX_PATH
#define _MAX_PATH	256
#endif
#ifndef _MAX_DIR
#define _MAX_DIR	256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT	256
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME	256
#endif


/*--------------------------------------------------------------------------------*/

#define HSPDEBUG
#define JPNMSG

#include "../hsp3/hsp3debug.cpp"

char *gethsperror( int error )
{
	if ((error<0)||(error>=HSPERR_MAX)) return err[0];
	return err[error];
}

/*--------------------------------------------------------------------------------*/

char *mem_ini( int size ) {
	return (char *)calloc(size,1);
}

void mem_bye( void *ptr ) {
	free(ptr);
}


int dpm_read( char *fname, void *readmem, int rlen, int seekofs )
{
	char *lpRd;
	FILE *ff;
	int a1;
	int seeksize;

	seeksize=seekofs;
	if (seeksize<0) seeksize=0;

	lpRd=(char *)readmem;

	//	Read normal file

	ff = fopen( fname, "rb" );
	if ( ff == NULL ) return -1;
	if ( seekofs>=0 ) fseek( ff, seeksize, SEEK_SET );
	a1 = (int)fread( lpRd, 1, rlen, ff );
	fclose( ff );
	return a1;
}

int dpm_exist( char *fname )
{
	FILE *ff;
	int length;

	if ( *fname == 0 ) return -1;
	ff=fopen( fname,"rb" );
	if (ff==NULL) return -1;
	fseek( ff,0,SEEK_END );
	length=(int)ftell( ff );			// normal file size
	fclose(ff);

	return length;
}

char *dpm_readalloc( char *fname )
{
	char *p;
	int len;
	len = dpm_exist( fname );
	if ( len < 0 ) return NULL;
	p = mem_ini( len+1 );
	dpm_read( fname, p, len, 0 );
	p[len] = 0;
	return p;
}

int dpm_save( char *fname, void *mem, int msize, int seekofs )
{
	FILE *fp;
	int flen;

	if (seekofs<0) {
		fp=fopen(fname,"wb");
	}
	else {
		fp=fopen(fname,"r+b");
	}
	if (fp==NULL) return -1;
	if ( seekofs>=0 ) fseek( fp, seekofs, SEEK_SET );
	flen = (int)fwrite( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}

static	int splc;	// split pointer

void strsp_ini( void )
{
	splc=0;
}

int strsp_getptr( void )
{
	return splc;
}

int strsp_get( char *srcstr, char *dststr, char splitchr, int len )
{
	//		split string with parameters
	//
	unsigned char a1;
	unsigned char a2;
	int a;
	int utf8cnt;
	a=0;utf8cnt=0;
	while(1) {
		utf8cnt=0;
		a1=srcstr[splc];
		if (a1==0) break;
		splc++;
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) utf8cnt++;
			if (a1>=224) utf8cnt++;
			if (a1>=240) utf8cnt++;
			if (a1>=248) utf8cnt++;
			if (a1>=252) utf8cnt++;
		}

		if (a1==splitchr) break;
		if (a1==13) {
			a2=srcstr[splc];
			if (a2==10) splc++;
			break;
		}
		if (a1==10) {
			a2=srcstr[splc];
			break;
		}
		dststr[a++]=a1;
		if (utf8cnt>0) {
			while(utf8cnt>0){
				dststr[a++]=srcstr[splc++];
				utf8cnt--;
			}
		}
		if ( a>=len ) break;
	}
	dststr[a]=0;
	return (int)a1;
}

/*--------------------------------------------------------------------------------*/

//
//		Internal function support (without Windows API)
//
static void _splitpath( char *path, char *p_drive, char *dir, char *fname, char *ext )
{
	//		Linux用ファイルパス切り出し
	//
	char *p, pathtmp[256];

	p_drive[0] = 0;
	strcpy( pathtmp, path );

	p = strchr2( pathtmp, '.' );
	if ( p == NULL ) {
		ext[0] = 0;
	} else {
		strcpy( ext, p );
		*p = 0;
	}
	p = strchr2( pathtmp, '/' );
	if ( p == NULL ) {
		dir[0] = 0;
		strcpy( fname, pathtmp );
	} else {
		strcpy( fname, p+1 );
		p[1] = 0;
		strcpy( dir, pathtmp );
	}
}

static int wildcard( char *text, char *wc )
{
	//		textに対してワイルドカード処理を適応
	//		return value: yes 1, no 0
	//
	if ( wc[0]=='\0' && *text=='\0' ) {
		return 1;
	}
	if ( wc[0]=='*' ) {
		if ( *text=='\0' && wc[1]=='\0' ) {
			return 1;
		} else if ( *text=='\0' ) {
			return 0;
		}
		if ( wc[1]==*text | wc[1]=='*' ) {
			if (wildcard( text, wc+1 )) {
				return 1;
			}
		}
		if ( *text!='\0' ) {
			return wildcard( text+1, wc );
		}
	}
	if ( (*text!='\0')&&(wc[0]==*text) ) {
		return wildcard( text+1, wc+1 );
	}
	return 0;
}


/*----------------------------------------------------------*/
//					HSP string trim support
/*----------------------------------------------------------*/

size_t utf8strlen( const char *target )
{
	//		UTF8文字列の長さを得る
	//
	unsigned char *p;
	unsigned char *base;
	unsigned char a1;
	p = (unsigned char *)target;
	base = p;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		p++;							// 検索位置を移動
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) p++;
			if (a1>=224) p++;
			if (a1>=240) p++;
			if (a1>=248) p++;
			if (a1>=252) p++;
		}
	}
	return (size_t)(p-base);
}


void strcase( char *target )
{
	//		strをすべて小文字に(utf8対応版)
	//
	unsigned char *p;
	unsigned char a1;
	p=(unsigned char *)target;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		*p=tolower(a1);
		p++;							// 検索位置を移動
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) p++;
			if (a1>=224) p++;
			if (a1>=240) p++;
			if (a1>=248) p++;
			if (a1>=252) p++;
		}
	}
}

int strcpy2( char *str1, char *str2 )
{
	//	string copy (ret:length)
	//
	char *p;
	char *src;
	char a1;
	src = str2;
	p = str1;
	while(1) {
		a1=*src++;if (a1==0) break;
		*p++=a1;
	}
	*p++=0;
	return (int)(p-str1);
}


int strcat2( char *str1, char *str2 )
{
	//	string cat (ret:length)
	//
	char *src;
	char a1;
	int i;
	src = str1;
	while(1) {
		a1=*src;if (a1==0) break;
		src++;
	}
	i = (int)(src-str1);
	return (strcpy2(src,str2)+i);
}


char *strstr2( char *target, char *src )
{
	//		strstr関数のutf8対応版
	//
	unsigned char *p;
	unsigned char *s;
	unsigned char *p2;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	p=(unsigned char *)target;
	if (( *src==0 )||( *target==0 )) return NULL;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		p2 = p;
		s=(unsigned char *)src;
		while(1) {
			a2=*s++;if (a2==0) return (char *)p;
			a3=*p2++;if (a3==0) break;
			if (a2!=a3) break;
		}
		p++;							// 検索位置を移動
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) p++;
			if (a1>=224) p++;
			if (a1>=240) p++;
			if (a1>=248) p++;
			if (a1>=252) p++;
		}
	}
	return NULL;
}


char *strchr2( char *target, char code )
{
	//		str中最後のcode位置を探す(utf8対応版)
	//
	unsigned char *p;
	unsigned char a1;
	char *res;
	p=(unsigned char *)target;
	res = NULL;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		if ( a1==code ) res=(char *)p;
		p++;							// 検索位置を移動
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) p++;
			if (a1>=224) p++;
			if (a1>=240) p++;
			if (a1>=248) p++;
			if (a1>=252) p++;
		}
	}
	return res;
}


void getpath( char *stmp, char *outbuf, int p2 )
{
	char *p;
	char tmp[_MAX_PATH];
	char p_drive[_MAX_PATH];
	char p_dir[_MAX_DIR];
	char p_fname[_MAX_FNAME];
	char p_ext[_MAX_EXT];

	p = outbuf;
	if (p2&16) strcase( stmp );
	_splitpath( stmp, p_drive, p_dir, p_fname, p_ext );

	strcat( p_drive, p_dir );
	if ( p2&8 ) {
		strcpy( tmp, p_fname ); strcat( tmp, p_ext );
	} else if ( p2&32 ) {
		strcpy( tmp, p_drive );
	} else {
		strcpy( tmp, stmp );
	}
	switch( p2&7 ) {
	case 1:			// Name only ( without ext )
		stmp[ strlen(tmp)-strlen(p_ext) ] = 0;
		strcpy( p, tmp );
		break;
	case 2:			// Ext only
		strcpy( p, p_ext );
		break;
	default:		// Direct Copy
		strcpy( p, tmp );
		break;
	}
}

char *strchr3( char *target, int code, int sw, char **findptr )
{
	//		文字列中のcode位置を探す(2バイトコード、utf8-4バイト分対応版)
	//		sw = 0 : findptr = 最後に見つかったcode位置
	//		sw = 1 : findptr = 最初に見つかったcode位置
	//		sw = 2 : findptr = 最初に見つかったcode位置(最初の文字のみ検索)
	//		戻り値 : 次の文字にあたる位置
	//
	unsigned char *p;
	unsigned char a1;
	unsigned char code1;
	unsigned char code2;
	unsigned char code3;
	unsigned char code4;
	char *res;
	char *pres;

	p=(unsigned char *)target;
	code1 = (unsigned char)(code&0xff);
	code2 = (unsigned char)(code>>8);
	code3 = (unsigned char)(code>>16);
	code4 = (unsigned char)(code>>24);

	res = NULL;
	pres = NULL;
	*findptr = NULL;

	while(1) {
		a1=*p;if ( a1==0 ) break;
		if ( a1==code1 ) {
			if ( a1 <128 ) {
				res=(char *)p;
			} else {
				if (a1>=128) {
					if (a1>=192) {
						if ( p[1]==code2 ) {
							if (a1>=224) {
								if ( p[2]==code3 ) {
									if (a1>=240) {
										if ( p[3]==code4 ) {
											res=(char *)p;
										}
									}else{
										res=(char *)p;
									}
								}
							}else{
								res=(char *)p;
							}
						}
					}else{
						res=(char *)p;
					}
				} else {
					res=(char *)p;
				}
			}
		}
		p++;							// 検索位置を移動
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) p++;
			if (a1>=224) p++;
			if (a1>=240) p++;
			if (a1>=248) p++;
			if (a1>=252) p++;
		}
		if ( res != NULL ) { *findptr = res; pres = (char *)p; res = NULL; }

		switch( sw ) {
		case 1:
			if ( *findptr != NULL ) return (char *)p;
			break;
		case 2:
			return (char *)p;
		}
	}
	return pres;
}


