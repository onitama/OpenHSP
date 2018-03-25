
//
//	Basic I/O and Message buffer support
//	For both console and Windows application development
//	"supio.cpp"
//	onion software/onitama 1997
//
#include "../../hsp3/hsp3config.h"

#ifdef HSPWIN
#define USE_WINDOWS_API		// WINDOWS APIを使用する
#endif

#ifdef USE_WINDOWS_API
#include <windows.h>
#include <shlobj.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <ctype.h>

#include "supio_win.h"

//
//		basic C I/O support
//

char *mem_ini( int size ) { return (char *)malloc(size); }
void mem_bye( void *ptr ) { free(ptr); }
int mem_load( const char *fname, void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"rb");
	if (fp==NULL) return -1;
	flen = (int)fread( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}
int mem_save( const char *fname, const void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"wb");
	if (fp==NULL) return -1;
	flen = (int)fwrite( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}

int filecopy( const char *fname, const char *sname )
{
	FILE *fp;
	FILE *fp2;
	int flen, rval;
	int max=0x8000;
	char *mem;
	rval=1;
	mem=mem_ini(max);
	fp=fopen(fname,"rb");if (fp==NULL) goto jobov;
	fp2=fopen(sname,"wb");if (fp2==NULL) goto jobov;
	while(1) {
		flen = (int)fread( mem, 1, max, fp );
		if (flen==0) break;
		fwrite( mem, 1, flen, fp2 );
		if (flen<max) break;
	}
	fclose(fp2);fclose(fp);
	rval=0;
jobov:
	mem_bye(mem);
	return rval;
}


static inline int issjisleadbyte( unsigned char c )
{
	return ( c >= 0x81 && c <= 0x9F ) || ( c >= 0xE0 && c <= 0xFC );
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

#ifdef USE_WINDOWS_API
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
#else
	*outbuf = 0;
#endif
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



/*----------------------------------------------------------*/


/*
	rev 54
	とりあえず書き直し。
	sjis全角を含むパスに対応。
*/

static int findext( char const * st )
{
	//	拡張子をさがす。
	//
	int r = -1, f = 0;
	for ( int i = 0; st[ i ] != '\0'; ++i ) {
		if ( f ) {
			f = 0;
		} else {
			if ( st[ i ] == '.' ) {
				r = i;
			} else if ( st[ i ] == '\\' ) {
				r = -1;
			}
			f = issjisleadbyte( st[ i ] );
		}
	}
	return r;
}


void addext( char *st, const char *exstr )
{
	//	add extension of filename
	int i = findext( st );
	if ( i == -1 ) {
		strcat( st, "." );
		strcat( st, exstr );
	}
}


void cutext( char * st )
{
	//		拡張子を取り除く
	//
	int i = findext( st );
	if ( i >= 0 ) st[ i ] = '\0';
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
	//		最後の'\\'を取り除く
	//
	char *ss;
	char *ss2;
	int i;
	ss = strchr2( p, '\\' );
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
#ifdef USE_WINDOWS_API
	char fname[_MAX_PATH+1];

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
		_getcwd( p, _MAX_PATH );
		break;
	case 1:				//    実行ファイルがあるディレクトリ
		GetModuleFileName( NULL,fname,_MAX_PATH );
		getpath( fname, p, 32 );
		break;
	case 2:				//    Windowsディレクトリ
		GetWindowsDirectory( p, _MAX_PATH );
		break;
	case 3:				//    Windowsのシステムディレクトリ
		GetSystemDirectory( p, _MAX_PATH );
		break;
	default:
		if ( id & 0x10000 ) {
			SHGetSpecialFolderPath( NULL, p, id & 0xffff, FALSE );
			break;
		}
		*p = 0;
		return;
	}

	//		最後の'\\'を取り除く
	//
	CutLastChr( p, '\\' );
#else
	*p = 0;
#endif
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
//		windows debug support
//
#ifdef USE_WINDOWS_API

void Alert( const char *mes )
{
	MessageBox( NULL, mes, "error",MB_ICONINFORMATION | MB_OK );
}

void AlertV( const char *mes, int val )
{
	char ss[128];
	sprintf( ss, "%s%d",mes,val );
	MessageBox( NULL, ss, "error",MB_ICONINFORMATION | MB_OK );
}

void Alertf( const char *format, ... )
{
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	MessageBox( NULL, textbf, "error",MB_ICONINFORMATION | MB_OK );
}

#endif




/*----------------------------------------------------------*/

//
//		Character Code Convert
//

int ConvSJis2Utf8( char *pSource, char *pDist, int buffersize )
{
	int size = 0;
 
   //ShiftJISからUTF-16へ変換
   const int nSize = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pSource, -1, NULL, 0 );
 
   BYTE* buffUtf16 = new BYTE[ nSize * 2 + 2 ];
   ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize );
 
   //UTF-16からUTF-8へ変換
   const int nSizeUtf8 = ::WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL );
   if( pDist == NULL ){
       delete buffUtf16;
       return -1;
   }
 
   BYTE* buffUtf8 = new BYTE[ nSizeUtf8 * 2 ];
   ZeroMemory( buffUtf8, nSizeUtf8 * 2 );
   ::WideCharToMultiByte( CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)buffUtf8, nSizeUtf8, NULL, NULL );
 
   size = lstrlen( (char*)buffUtf8 ) + 1;
   if ( size > buffersize ) size = buffersize;

   memcpy( pDist, buffUtf8, size );
 
   delete buffUtf16;
   delete buffUtf8;
 
   return size;
}

