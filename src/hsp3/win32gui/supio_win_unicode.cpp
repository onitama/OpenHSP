//
//	supio.cpp functions
//

#ifdef HSPUTF8

#include "../hsp3config.h"

#ifdef HSPWIN
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <ctype.h>
#include <tchar.h>

#include "supio_win_unicode.h"
#include "../dpmread.h"
#include "../strbuf.h"

#ifdef HSPUTF8
#pragma execution_character_set("utf-8")
#endif

//
//		API用の文字エンコードへ変換
//
HSPAPICHAR *chartoapichar( const char *orig,HSPAPICHAR **pphac)
{
	
	int reslen;
	wchar_t *resw;
	if (orig == 0) {
		*pphac = 0;
		return 0;
	}
	reslen = MultiByteToWideChar(CP_UTF8,0,orig,-1,(LPWSTR)NULL,0);
	resw = (wchar_t*)calloc(reslen+1,sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8,0,orig,-1,resw,reslen);
	*pphac = resw;
	return resw;
}

void freehac(HSPAPICHAR **pphac)
{
	free(*pphac);
	*pphac = 0;
}

HSPCHAR *apichartohspchar( const HSPAPICHAR *orig,HSPCHAR **pphc)
{
	int plen;
	HSPCHAR *p = 0;
	if (orig == 0) {
		*pphc = 0;
		return 0;
	}
	plen=WideCharToMultiByte(CP_UTF8,NULL,orig,-1,NULL,0,NULL,NULL);
	p = (HSPCHAR *)calloc(plen+1,sizeof(HSPCHAR*));
	WideCharToMultiByte(CP_UTF8,NULL,orig,-1,p,plen,NULL,NULL);
	*pphc = p;
	return p;
}

void freehc(HSPCHAR **pphc)
{
	free(*pphc);
	*pphc = 0;
}

HSPAPICHAR *ansichartoapichar(const char *orig, HSPAPICHAR **pphac)
{

	int reslen;
	wchar_t *resw;
	if (orig == 0) {
		*pphac = 0;
		return 0;
	}
	reslen = MultiByteToWideChar(CP_ACP, 0, orig, -1, (LPWSTR)NULL, 0);
	resw = (wchar_t*)calloc(reslen + 1, sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, orig, -1, resw, reslen);
	*pphac = resw;
	return resw;
}

char *apichartoansichar(const HSPAPICHAR *orig, char **ppac)
{
	int plen;
	HSPCHAR *p = 0;
	if (orig == 0) {
		*ppac = 0;
		return 0;
	}
	plen = WideCharToMultiByte(CP_ACP, NULL, orig, -1, NULL, 0, NULL, NULL);
	p = (char *)calloc(plen + 1, sizeof(char*));
	WideCharToMultiByte(CP_ACP,NULL, orig, -1, p, plen, NULL, NULL);
	*ppac = p;
	return p;
}

void freeac(char **ppac)
{
	free(*ppac);
	*ppac = 0;
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


int mem_save( char *fname8, void *mem, int msize, int seekofs )
{
	FILE *fp;
	int flen;
	HSPAPICHAR *fnamew = 0;

	if (seekofs<0) {
		fp=_tfopen(chartoapichar(fname8,&fnamew),TEXT("wb"));
	}
	else {
		fp=_tfopen(chartoapichar(fname8,&fnamew),TEXT("r+b"));
	}
	freehac(&fnamew);
	if (fp==NULL) return -1;
	if ( seekofs>=0 ) fseek( fp, seekofs, SEEK_SET );
	flen = (int)fwrite( mem, 1, msize, fp );
	fclose(fp);
	free(fnamew);
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

void strcaseW( HSPAPICHAR *target )
{
	//		strをすべて小文字に(API用)
	//
	HSPAPICHAR *p = 0;
	HSPAPICHAR a1;
	p=(HSPAPICHAR *)target;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		*p=tolower(a1);
		p++;							// 検索位置を移動
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


void getpathW( HSPAPICHAR *p_stmp, HSPAPICHAR *outbuf, int p2 )
{
	HSPAPICHAR *p = 0;
	HSPAPICHAR *stmp = p_stmp;
	HSPAPICHAR p_drive[_MAX_PATH];
	HSPAPICHAR p_dir[_MAX_DIR];
	HSPAPICHAR p_fname[_MAX_FNAME];
	HSPAPICHAR p_ext[_MAX_EXT];

	p = outbuf;
	if (p2 & 16) {
		stmp = _wcslwr(p_stmp);
		//strcaseW(stmp);
	}
	_tsplitpath( stmp, p_drive, p_dir, p_fname, p_ext );
	_tcscat( p_drive, p_dir );
	if ( p2&8 ) {
		_tcscpy( stmp, p_fname ); _tcscat( stmp, p_ext );
	} else if ( p2&32 ) {
		_tcscpy( stmp, p_drive );
	}
	switch( p2&7 ) {
	case 1:			// Name only ( without ext )
		stmp[ _tcslen(stmp)-_tcslen(p_ext) ] = 0;
		_tcscpy( p, stmp );
		break;
	case 2:			// Ext only
		_tcscpy( p, p_ext );
		break;
	default:		// Direct Copy
		_tcscpy( p, stmp );
		break;
	}
}

int makedir( char *name8 )
{
#ifdef HSPWIN
	HSPAPICHAR *namew = 0;
	int res;

	res = _tmkdir( chartoapichar(name8,&namew) );
	freehac( &namew );
	return res;
#else
	return 0;
#endif
}


int changedir( char *name8 )
{
#ifdef HSPWIN
	HSPAPICHAR *namew = 0;
	int res;
	res = _tchdir( chartoapichar(name8,&namew) );
	freehac( &namew );
	return res;
#else
	return 0;
#endif
}

int changedirW( HSPAPICHAR *name )
{
#ifdef HSPWIN
	int res;
	res = _tchdir( name );
	return res;
#else
	return 0;
#endif
}


int delfile( char *name8 )
{
#ifdef HSPWIN
	HSPAPICHAR *namew = 0;
	int res;

	res = DeleteFile( chartoapichar(name8,&namew) );
	freehac( &namew );
	return res;
#else
	return 0;
#endif
}


int dirlist( char *fname8, char **target, int p3 )
{
#ifdef HSPWIN
	TCHAR *pw;
	char *p;
	int fl;
	int stat_main;
	HANDLE sh;
	WIN32_FIND_DATA fd;
	DWORD fmask;
	BOOL ff;
	HSPAPICHAR *fnamew = 0;

	chartoapichar(fname8,&fnamew);

	fmask=0;
	if (p3&1) fmask|=FILE_ATTRIBUTE_DIRECTORY;
	if (p3&2) fmask|=FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

	stat_main=0;

	sh=FindFirstFile( chartoapichar(fname8,&fnamew), &fd );
	free(fnamew);
	if (sh==INVALID_HANDLE_VALUE) return 0;

	while(1) {
		ff=( fd.dwFileAttributes & fmask )>0;
		if ((p3&4)==0) ff=!ff;
		if (ff) {
			pw = fd.cFileName; fl = 1;
			apichartohspchar(pw,&p);
			if ( *p==0 ) fl=0;			// 空行を除外
			if ( *p=='.') {				// '.','..'を除外
				if ( p[1]==0 ) fl=0;
				if ((p[1]=='.')&&(p[2]==0)) fl=0;
			}
			if (fl) {
				stat_main++;
				sbStrAdd( target, p );
				sbStrAdd( target,"\r\n" );
			}
			freehc(&p);
		}
		if (!FindNextFile(sh, &fd)) {
			int res = GetLastError();
			if (res == ERROR_NO_MORE_FILES) break;
		}
	}
	FindClose(sh);
	return stat_main;
#else
	return 0;
#endif
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
*/
#ifdef HSPWIN
	SYSTEMTIME st;
	short *a;
	GetLocalTime( &st );
	a=(short *)&st;
	return (int)(a[index]);
#else
	return 0;
#endif
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
			if ((a1 >= 192) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 224) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 240) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 248) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
			if ((a1 >= 252) && (srcstr[splc + utf8cnt] != 0)) utf8cnt++;
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
		if ( a>=len-5 ) break;
	}
	for (int i = a; i < a+6; i++){
		dststr[i] = 0;
	}
	return (int)a1;
}

int strsp_getW( HSPAPICHAR *srcstr, HSPAPICHAR *dststr, HSPAPICHAR splitchr, int len )
{
	//		split string with parameters
	//

/*
	rev 44
	mingw : warning : 比較は常に偽
	に対処
*/
	HSPAPICHAR a1;
	HSPAPICHAR a2;
	int a;
	a=0;
	while(1) {
		a1=srcstr[splc];
		if (a1==0) break;
		splc++;

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
		if (a1 == 32) if (spmode == 0) {
			while (1) {
				a1 = *cmdchk;
				if (a1 == 0) break;
				if (a1 != 32) break;
				cmdchk++;
			}
			break;
		}
		if (a1==0x22) spmode^=1;
	}
	return cmdchk;
}

wchar_t *strsp_cmdsW( wchar_t *srcstr )
{
	//		Skip 1parameter from command line
	//
	int spmode;
	wchar_t a1;
	wchar_t *cmdchk;
	cmdchk = srcstr;
	spmode=0;
	while(1) {
		a1=*cmdchk;
		if (a1==0) break;
		cmdchk++;
		if (a1 == 32) if (spmode == 0) {
			while (1) {
				a1 = *cmdchk;
				if (a1 == 0) break;
				if (a1 != 32) break;
				cmdchk++;
			}
			break;
		}
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
		i = cursize + csize + len_buffer + 1;	// 置き換え後に十分なサイズを確保する
		if (i >= len_result) {
			while (1) {
				len_result += 0x8000;
				if (i < len_result) break;
			}
			s_result = sbExpand(s_result, len_result);
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
//		windows security support
//

#ifdef HSP3IMP
//
//	HSP3IMP用セキュリティ対応
//
int SecurityCheck( char *name )
{
	char *p;
	char fname[_MAX_PATH+1];
	GetWindowsDirectory( fname, _MAX_PATH );
	strcase( fname );
	CutLastChr( fname, '\\' );
	strcase( name );
	p = strstr( name, fname+2 );
	if ( p == NULL ) return 0;
	return -1;
}

#endif


//
//		windows debug support
//
//
void Alert( const char *mes8 )
{
	HSPAPICHAR *mesw = 0;

	MessageBox( NULL, chartoapichar(mes8,&mesw), TEXT("error"),MB_ICONINFORMATION | MB_OK );
	free(mesw);
}
void AlertW( const HSPAPICHAR *mes )
{
	MessageBox( NULL, mes, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}

void AlertV( const char *mes8, int val )
{
	wchar_t ss[1024];
	HSPAPICHAR *mesw = 0;

	wsprintf( ss, TEXT("%s%d"),chartoapichar(mes8,&mesw),val );
	MessageBoxW( NULL, ss, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}

void Alertf( const HSPAPICHAR *format, ... )
{
	TCHAR textbf[4096];
	va_list args;
	va_start(args, format);
	_vstprintf(textbf, format, args);
	va_end(args);
	MessageBox( NULL, textbf, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}

#endif
