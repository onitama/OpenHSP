
//
//	supio.cpp functions (for Linux)
//	Linux用のsupio.cppを別ファイルとして作成しました。
//
//	Special thanks to Charlotte at HSP開発wiki
//	http://hspdev-wiki.net/?OpenHSP%2FLinux%2Fhsp3
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <emscripten.h>

// gettime
#include <sys/time.h>
#include <time.h>
// mkdir stat
#include <sys/stat.h>
#include <sys/types.h>
// changedir delfile get_current_dir_name stat
#include <unistd.h>
// dirlist
#include <dirent.h>

#include "supio_emscripten.h"

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


//
//		Internal function support (without Windows API)
//
void _splitpath( const char *path, char *p_drive, char *dir, char *fname, char *ext )
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


//
//		basic C I/O support
//
char *mem_ini( int size ) {
	return (char *)calloc(size,1);
}

void mem_bye( void *ptr ) {
	free(ptr);
}

int mem_load( const char *fname, void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"rb");
	if (fp==NULL) return -1;
	flen = fread( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}

int mem_save( const char *fname, const void *mem, int msize, int seekofs )
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


void strcase( char *str )
{
	//	string case to lower
	//
	unsigned char a1;
	unsigned char *ss;
	ss=(unsigned char *)str;
	while(1) {
		a1=*ss;
		if (a1==0) break;
		if (a1>=0x80) {
			*ss++;
			a1=*ss++;
			if (a1==0) break;
		}
		else {
			*ss++=tolower(a1);
		}
	}
}


void strcase2( char *str, char *str2 )
{
	//	string case to lower and copy
	//
	unsigned char a1;
	unsigned char *ss;
	unsigned char *ss2;
	ss=(unsigned char *)str;
	ss2=(unsigned char *)str2;
	while(1) {
		a1=*ss;
		if (a1==0) break;
		if (a1>=0x80) {
			*ss++;
			*ss2++=a1;
			a1=*ss++;
			if (a1==0) break;
			*ss2++=a1;
		}
		else {
			a1=tolower(a1);
			*ss++=a1;
			*ss2++=a1;
		}
	}
	*ss2=0;
}


int tstrcmp( const char *str1, const char *str2 )
{
	//	string compare (0=not same/-1=same)
	//
	int ap;
	char as;
	ap=0;
	while(1) {
		as=str1[ap];
		if (as!=str2[ap]) return 0;
		if (as==0) break;
		ap++;
	}
	return -1;
}


void getpath( const char *src, char *outbuf, int p2 )
{
	char *p;
	char stmp[_MAX_PATH];
	char p_drive[_MAX_PATH];
	char p_dir[_MAX_DIR];
	char p_fname[_MAX_FNAME];
	char p_ext[_MAX_EXT];

	p = outbuf;
	strcpy( stmp, src );
	if (p2&16) strcase( stmp );
	_splitpath( stmp, p_drive, p_dir, p_fname, p_ext );
	strcat( p_drive, p_dir );
	if ( p2&8 ) {
		strcpy( stmp, p_fname ); strcat( stmp, p_ext );
	} else if ( p2&32 ) {
		strcpy( stmp, p_drive );
	}
	switch( p2&7 ) {
	case 1:			// Name only ( without ext )
		stmp[ strlen(stmp)-strlen(p_ext) ] = 0;
		strcpy( p, stmp );
		break;
	case 2:			// Ext only
		strcpy( p, p_ext );
		break;
	default:		// Direct Copy
		strcpy( p, stmp );
		break;
	}
}


void strcpy2( char *dest, const char *src, size_t size )
{
	if(size == 0) {
		return;
	}
	char *d = dest;
	const char *s = src;
	size_t n = size;
	while (--n) {
		if((*d++ = *s++) == '\0') {
			return;
		}
	}
	*d = '\0';
	return;
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

int makedir( char *name )
{
	return mkdir( name, 0755 );
}


int changedir( char *name )
{
	return chdir( name );
}


int delfile( char *name )
{
	return unlink( name );
	//return remove( name );		// ディレクトリにもファイルにも対応
}

/*----------------------------------------------------------*/

void addext( char *st, const char *exstr )
{
	//	add extension of filename

	int a1;
	char c1;
	a1=0;while(1) {
		c1=st[a1];if (c1==0) break;
		if (c1=='.') return;
		a1++;
	}
	st[a1]='.'; st[a1+1]=0;
	strcat(st,exstr);
}

void cutext( char *st )
{
	//	cut extension of filename

	int a1;
	char c1;
	a1=0;while(1) {
		c1=st[a1];if (c1==0) break;
		if (c1=='.') break;
		a1++;
	}
	st[a1]=0;
}

void cutlast( char *st )
{
	//	cut last characters

	int a1;
	unsigned char c1;
	a1=0;while(1) {
		c1=st[a1];if (c1<33) break;
		st[a1]=tolower(c1);
		a1++;
	}
	st[a1]=0;
}


void cutlast2( char *st )
{
	//	cut last characters

	int a1;
	char c1;
	char ts[256];

	strcpy(ts,st);
	a1=0;
	while(1) {
		c1=ts[a1];if (c1<33) break;
		ts[a1]=tolower(c1);
		a1++;
	}
	ts[a1]=0;

	while(1) {
		a1--;c1=ts[a1];
		if (c1==0x5c) { a1++;break; }
		if (a1==0) break;
	}
	strcpy(st,ts+a1);
}


char *strchr2( char *target, char code )
{
	//		str中最後のcode位置を探す(全角対応版)
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
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
	}
	return res;
}

int is_sjis_char_head( const unsigned char *str, int pos )
{
	//		Shift_JIS文字列のposバイト目が文字の先頭バイトであるか
	//		マルチバイト文字の後続バイトなら0、それ以外なら1を返す
	int result = 1;
	while(pos != 0 && issjisleadbyte(str[--pos])) {
		result = ! result;
	}
	return result;
}

char *to_hsp_string_literal( const char *src ) {
	//		文字列をHSPの文字列リテラル形式に
	//		戻り値のメモリは呼び出し側がfreeする必要がある。
	//		HSPの文字列リテラルで表せない文字は
	//		そのまま出力されるので注意。（'\n'など）
	//
	size_t length = 2;
	const unsigned char *s = (unsigned char *)src;
	while (1) {
		unsigned char c = *s;
		if ( c == '\0' ) break;
		switch (c) {
		case '\r':
			if ( *(s+1) == '\n' ) {
				s ++;
			}
			// FALL THROUGH
		case '\t':
		case '"':
		case '\\':
			length += 2;
			break;
		default:
			length ++;
		}
		if ( issjisleadbyte(c) && *(s+1) != '\0' ) {
			length ++;
			s += 2;
		} else {
			s ++;
		}
	}
	char *dest = (char *)malloc(length + 1);
	if ( dest == NULL ) return dest;
	s = (unsigned char *)src;
	unsigned char *d = (unsigned char *)dest;
	*d++ = '"';
	while (1) {
		unsigned char c = *s;
		if ( c == '\0' ) break;
		switch (c) {
		case '\t':
			*d++ = '\\';
			*d++ = 't';
			break;
		case '\r':
			*d++ = '\\';
			if ( *(s+1) == '\n') {
				*d++ = 'n';
				s ++;
			} else {
				*d++ = 'r';
			}
			break;
		case '"':
			*d++ = '\\';
			*d++ = '"';
			break;
		case '\\':
			*d++ = '\\';
			*d++ = '\\';
			break;
		default:
			*d++ = c;
			if( issjisleadbyte(c) && *(s+1) != '\0' ) {
				*d++ = *++s;
			}
		}
		s ++;
	}
	*d++ = '"';
	*d = '\0';
	return dest;
}

int atoi_allow_overflow( const char *s )
{
	//		オーバーフローチェックをしないatoi
	//
	int result = 0;
	while (isdigit(*s)) {
		result = result * 10 + (*s - '0');
		s ++;
	}
	return result;
}

void CutLastChr( char *p, char code )
{
	//		最後の'/'を取り除く
	//
	char *ss;
	char *ss2;
	int i;
	ss = strchr2( p, '/' );
	if ( ss != NULL ) {
		i = (int)strlen( p ); ss2 = p + i -1;
		if (( i > 3 )&&( ss == ss2 )) *ss = 0;
	}
}



/*----------------------------------------------------------*/
//					HSP string trim support
/*----------------------------------------------------------*/

char *strchr3( char *target, int code, int sw, char **findptr )
{
	//		文字列中のcode位置を探す(2バイトコード、全角対応版)
	//		sw = 0 : findptr = 最後に見つかったcode位置
	//		sw = 1 : findptr = 最初に見つかったcode位置
	//		sw = 2 : findptr = 最初に見つかったcode位置(最初の文字のみ検索)
	//		戻り値 : 次の文字にあたる位置
	//
	unsigned char *p;
	unsigned char a1;
	unsigned char code1;
	unsigned char code2;
	char *res;
	char *pres;

	p=(unsigned char *)target;
	code1 = (unsigned char)(code&0xff);
	code2 = (unsigned char)(code>>8);

	res = NULL;
	pres = NULL;
	*findptr = NULL;

	while(1) {
		a1=*p;if ( a1==0 ) break;
		if ( a1==code1 ) {
			if ( a1 <129 ) {
				res=(char *)p;
			} else {
				if ((a1<=159)||(a1>=224)) {
					if ( p[1]==code2 ) {
						res=(char *)p;
					}
				} else {
					res=(char *)p;
				}
			}
		}
		p++;							// 検索位置を移動
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
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


void TrimCodeR( char *p, int code )
{
	//		最後のcodeを取り除く
	//
	char *ss;
	char *ss2;
	char *sslast;
	int i;
	while(1) {
		i = (int)strlen( p );
		sslast = p + i;
		ss = strchr3( p, code, 0, &ss2 );
		if ( ss2 == NULL ) break;
		if ( ss != sslast ) break;
		*ss2 = 0;
	}
}


void TrimCode( char *p, int code )
{
	//		すべてのcodeを取り除く
	//
	char *ss;
	char *ss2;
	while(1) {
		ss = strchr3( p, code, 1, &ss2 );
		if ( ss2 == NULL ) break;
		strcpy( ss2, ss );
	}
}


void TrimCodeL( char *p, int code )
{
	//		最初のcodeを取り除く
	//
	char *ss;
	char *ss2;
	while(1) {
		ss = strchr3( p, code, 2, &ss2 );
		if ( ss2 == NULL ) break;
		strcpy( ss2, ss );
	}
}



/*----------------------------------------------------------*/
//					HSP system support
/*----------------------------------------------------------*/

void dirinfo( char *p, int id )
{
	//		dirinfo命令の内容をstmpに設定する
	//
	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
	case 1:				//    実行ファイルがあるディレクトリ
		getcwd( p, _MAX_PATH);
		break;
	case 2:				//    Windowsディレクトリ
	case 3:				//    Windowsのシステムディレクトリ
	default:
		*p = 0;
		return;
	}

	//		最後の'/'を取り除く
	//
	CutLastChr( p, '/' );
}


/*----------------------------------------------------------*/

//
//		Memory Manager
//
char *mem_alloc( void *base, int newsize, int oldsize )
{
	char *p;
	if ( base == NULL ) {
		p = (char *)calloc( newsize, 1 );
		return p;
	}
	if ( newsize <= oldsize ) return (char *)base;
	p = (char *)calloc( newsize, 1 );
	memcpy( p, base, oldsize );
	free( base );
	return p;
}


//
//		emscripten debug support
//
void Alert( const char *mes )
{
	EM_ASM_ARGS( {
			console.log(Pointer_stringify($0));
			//alert(Pointer_stringify($0));
		}, mes );
}

void AlertV( const char *mes, int val )
{
	char ss[1024];
	sprintf( ss, "%s%d",mes,val );
	EM_ASM_ARGS( {
			console.log(Pointer_stringify($0));
			//alert(Pointer_stringify($0));
		}, ss );
}

void Alertf( const char *format, ... )
{
	char textbf[4096];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	EM_ASM_ARGS( {
			console.log(Pointer_stringify($0));
			//alert(Pointer_stringify($0));
		}, textbf );
}
