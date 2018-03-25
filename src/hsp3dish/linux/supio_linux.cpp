
//
//	supio.cpp functions (for Linux)
//	Linux用のsupio.cppを別ファイルとして作成しました。
//
//	Special thanks to Charlotte at HSP開発wiki
//	http://hspdev-wiki.net/?OpenHSP%2FLinux%2Fhsp3
//
//
#include "../../hsp3/hsp3config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

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

#include "supio_linux.h"

#include "../../hsp3/dpmread.h"
#include "../../hsp3/strbuf.h"

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

HSPAPICHAR *chartoapichar( const char *orig,HSPAPICHAR **pphac)
{
	*pphac = (HSPAPICHAR*)orig;
	return (HSPAPICHAR*)orig;
}

void freehac(HSPAPICHAR **pphac)
{
	*pphac = 0;
	return;
}

HSPCHAR *apichartohspchar( const HSPAPICHAR *orig,HSPCHAR **pphc)
{
	*pphc = (HSPAPICHAR*)orig;
	return (HSPCHAR*)orig;
}

void freehc(HSPCHAR **pphc)
{
	*pphc = 0;
	return;
}

HSPAPICHAR *ansichartoapichar(const char *orig, HSPAPICHAR **pphac)
{
	*pphac = (HSPAPICHAR*)orig;
	return (HSPAPICHAR*)orig;
}

char *apichartoansichar(const HSPAPICHAR *orig, char **ppc)
{
	*ppc = (char*)orig;
	return (char*)orig;
}

void freeac(char **ppc)
{
	*ppc = 0;
	return;
}

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


//
//		basic C I/O support
//
static FILE *fp;

char *mem_ini( int size ) {
	return (char *)calloc(size,1);
}

void mem_bye( void *ptr ) {
	free(ptr);
}


int mem_save( char *fname, void *mem, int msize, int seekofs )
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


int dirlist( char *fname, char **target, int p3 )
{
	//		Linux System
	//
	enum { MASK = 3 };			// mode 3までのビット反転用
	char *p;
	unsigned int fl;
	unsigned int stat_main;
	unsigned int fmask;
	DIR *sh;
	struct dirent *fd;
	struct stat st;
	char curdir[_MAX_PATH+1];

	stat_main=0;

	//sh = opendir( get_current_dir_name() );
	getcwd( curdir, _MAX_PATH );
	sh = opendir( curdir );	// get_current_dir_nameはMinGWで通らなかったのでとりあえず

	fd = readdir( sh );
	while( fd != NULL ) {
		p = fd->d_name; fl = 1;
		if ( *p==0 ) fl=0;			// 空行を除外
		if ( *p=='.') {				// '.','..'を除外
			if ( p[1]==0 ) fl=0;
			if ((p[1]=='.')&&(p[2]==0)) fl=0;
		}
		//		表示/非表示のマスク
		//		Linux用なのでシステム属性は考慮しない
		if (p3!=0 && fl==1) {
			stat( p, &st );
			fmask=0;
			if (p3&4) {				// 条件反転
				if (S_ISREG( st.st_mode )&&( *p!='.' )) {
					fl=0;
				} else {
					fmask=MASK;
				}
			}
			if ( fl==1 ) {
				if ((p3^fmask)&1 && S_ISDIR( st.st_mode )) fl=0;	//ディレクトリ
				if ((p3^fmask)&2 && ( *p=='.' )) fl=0;				//隠しファイル
			}
		}
		//		ワイルドカード処理
		//
		if (fl) {
			fl=wildcard( p, fname );
		}
		
		if (fl) {
			stat_main++;
			sbStrAdd( target, p );
			sbStrAdd( target, "\n" );
		}
		fd = readdir( sh );
	}
	closedir( sh );
	return stat_main;
}


int gettime( int index )
{
/*
	Get system time entries
	index :
	    0 wYear
	    1 wMonth
	    2 wDayOfWeek
	    3 wDay
	    4 wHour
	    5 wMinute
	    6 wSecond
	    7 wMilliseconds
	    8 wMicroseconds
*/
	struct timeval tv;
	struct tm *lt;

	gettimeofday( &tv, NULL );	// MinGWだとVerによって通りません
	lt = localtime( &tv.tv_sec );

	switch( index ) {
	case 0:
		return lt->tm_year+1900;
	case 1:
		return lt->tm_mon+1;
	case 2:
		return lt->tm_wday;
	case 3:
		return lt->tm_mday;
	case 4:
		return lt->tm_hour;
	case 5:
		return lt->tm_min;
	case 6:
		return lt->tm_sec;
	case 7:
		return (int)tv.tv_usec/10000;
	case 8:
		/*	一応マイクロ秒まで取れる	*/
		return (int)tv.tv_usec%10000;
	}
	return 0;
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

/*
	rev 44
	mingw : warning : 比較は常に偽
	に対処
*/
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
#ifdef HSPLINUX
		if (a1==10) {
			a2=srcstr[splc];
			break;
		}
#endif
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


char *strsp_cmds( char *srcstr )
{
	//		Skip 1parameter from command line
	//
	int spmode;
	char a1;
	char *cmdchk;
	cmdchk = srcstr;
	spmode=0;
	while(1) {
		a1=*cmdchk;
		if (a1==0) break;
		cmdchk++;
		if (a1==32) if (spmode==0) break;
		if (a1==0x22) spmode^=1;
	}
	return cmdchk;
}


int GetLimit( int num, int min, int max )
{
	if ( num > max ) return max;
	if ( num < min ) return min;
	return num;
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


static int htoi_sub( char hstr )
{
	//	exchange hex to int

	char a1;
	a1=tolower(hstr);
	if ((a1>='0')&&(a1<='9')) return a1-'0';
	if ((a1>='a')&&(a1<='f')) return a1-'a'+10;
	return 0;
}


int htoi( char *str )
{
	char a1;
	int d;
	int conv;
	conv = 0;
	d = 0;
	while(1) {
		a1=str[d++];if ( a1 == 0 ) break;
		conv=(conv<<4) + htoi_sub(a1);
	}
	return conv;
}


/*----------------------------------------------------------*/
//					HSP string trim support
/*----------------------------------------------------------*/

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

//
//		文字列置き換え
//
static	char *s_match;
static	int len_match;
static	char *s_rep;
static	int len_rep;
static	char *s_buffer;
static	int len_buffer;
static	char *s_result;
static	int len_result;
static	int reptime;

void ReplaceSetMatch(char *src, char *match, char *result, int in_src, int in_match, int in_result)
{
	//		置き換え元、置き換え対象のセット
	//		(あらかじめメモリバッファの確保が必要)
	//
	s_buffer = src;
	s_match = match;
	s_result = result;
	len_buffer = in_src;
	len_match = in_match;
	len_result = in_result;
}

char *ReplaceStr( char *repstr )
{
	//		置き換え実行
	//
	char *p;
	unsigned char a1;
	unsigned char a2;
	int psize, csize, cursize, i;
	int utf8cnt;

	s_rep = repstr;
	len_rep = (int)strlen( s_rep );
	reptime = 0;

	// replace
	//
	cursize = 0;
	p = s_buffer;
	a2 = (unsigned char)s_match[0];
	while(1) {
		a1 = (unsigned char)*p;
		if ( a1 == 0 ) break;

		utf8cnt=0;
		if (a1>=128) {					// 多バイト文字チェック
			if (a1>=192) utf8cnt++;
			if (a1>=224) utf8cnt++;
			if (a1>=240) utf8cnt++;
			if (a1>=248) utf8cnt++;
			if (a1>=252) utf8cnt++;
		}

		//	比較する
		psize = 0; csize = 1;
		if ( a1 == a2 ) {
			if ( memcmp( p, s_match, len_match ) == 0 ) {
				psize = len_match;
				csize = len_rep;
			}
		}

		//	バッファチェック
		i = cursize + csize;
		if ( i >= len_result ) {
			len_result += 0x8000;
			s_result = sbExpand( s_result, len_result );
		}

		if ( psize ) {				// 置き換え

			memcpy( s_result+cursize, s_rep, csize );
			p += psize;
			cursize += csize;
			reptime++;

		} else {					// 置き換えなし
			s_result[cursize++] = a1;
			p++;
			if ( utf8cnt>0 ) {
				while (utf8cnt>0){
					s_result[cursize++] = *p++;
					utf8cnt--;
				}
			}
		}

	}
	s_result[cursize] = 0;
	return s_result;
}

int ReplaceDone( void )
{
	//		置き換えの後処理
	//
	return reptime;
}




//
//		linux debug support
//
void Alert( const char *mes )
{
	printf( mes );
	printf( "\r\n" );
}

void AlertV( const char *mes, int val )
{
	printf( "%s%d\r\n",mes,val );
}

void Alertf( const char *format, ... )
{
	char textbf[4096];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	printf( textbf );
	printf( "\r\n" );
}
