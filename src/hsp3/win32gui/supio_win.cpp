//
//	supio.cpp functions
//

#ifndef HSPUTF8

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

#include "supio_win.h"
#include "../dpmread.h"
#include "../strbuf.h"

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


void strcase( char *target )
{
	//		strをすべて小文字に(全角対応版)
	//
	unsigned char *p;
	unsigned char a1;
	p=(unsigned char *)target;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		*p=tolower(a1);
		p++;							// 検索位置を移動
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
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
	//		strstr関数の全角対応版
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
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
	}
	return NULL;
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


void getpath( char *stmp, char *outbuf, int p2 )
{
	char *p;
	char p_drive[_MAX_PATH];
	char p_dir[_MAX_DIR];
	char p_fname[_MAX_FNAME];
	char p_ext[_MAX_EXT];

	p = outbuf;
	if (p2&16) strcase( stmp );

	//新しいVC++で0x5cコードが正しく処理されないためSJIS版の_splitpathは使用せず
	//_splitpath( stmp, p_drive, p_dir, p_fname, p_ext );
	wchar_t wszBufPath[_MAX_PATH], wdrive[_MAX_DRIVE], wdir[_MAX_DIR], wfname[_MAX_FNAME], wext[_MAX_EXT];

	mbstowcs(wszBufPath, stmp, strlen(stmp) + 1);
	_wsplitpath(wszBufPath, wdrive, wdir, wfname, wext);

	wcstombs(p_drive, wdrive, _MAX_DRIVE);
	wcstombs(p_dir, wdir, _MAX_DIR);
	wcstombs(p_fname, wfname, _MAX_FNAME);
	wcstombs(p_ext, wext, _MAX_EXT);

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

void getpathW( HSPAPICHAR *stmp, HSPAPICHAR *outbuf, int p2 )
{
	getpath(stmp,outbuf,p2);
}

int makedir( char *name )
{
#ifdef HSPWIN
	return _mkdir( name );
#else
	return 0;
#endif
}


int changedir( char *name )
{
#ifdef HSPWIN
	return _chdir( name );
#else
	return 0;
#endif
}

int changedirW( HSPAPICHAR *name)
{
	return changedir(name);
}

int delfile( char *name )
{
#ifdef HSPWIN
	return DeleteFile( name );
#else
	return 0;
#endif
}


int dirlist( char *fname, char **target, int p3 )
{
#ifdef HSPWIN
	char *p;
	int fl;
	int stat_main;
	HANDLE sh;
	WIN32_FIND_DATA fd;
	DWORD fmask;
	BOOL ff;

	fmask=0;
	if (p3&1) fmask|=FILE_ATTRIBUTE_DIRECTORY;
	if (p3&2) fmask|=FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

	stat_main=0;

	sh=FindFirstFile( fname, &fd );
	if (sh==INVALID_HANDLE_VALUE) return 0;

	while(1) {
		ff=( fd.dwFileAttributes & fmask )>0;
		if ((p3&4)==0) ff=!ff;
		if (ff) {
			p = fd.cFileName; fl = 1;
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
	int sjflg;
	a=0;sjflg=0;
	while(1) {
		sjflg=0;
		a1=srcstr[splc];
		if (a1==0) break;
		splc++;
		if (a1>=0x81) if (a1<0xa0) sjflg++;
		if (a1>=0xe0) sjflg++;

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
		if (sjflg) {
			dststr[a++]=srcstr[splc++];
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

//
//		文字列置き換え
//		(入れ子になることがあるので、バッファの確保をhsp3int側で行なうように修正)
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


void ReplaceSetMatch( char *src, char *match, char *result, int in_src, int in_match, int in_result )
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
	int sjis_flag;

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

#ifndef HSPUTF8
		//	sjisチェック
		sjis_flag = 0;
		if ( a1 >= 129 ) {
			if ((a1<=159)||(a1>=224)) sjis_flag++;
		}
#endif

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
		if ( i >= len_result ) {
			while (1) {
				len_result += 0x8000;
				if (i < len_result) break;
			}
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
#ifndef HSPUTF8
			if ( sjis_flag ) {
				s_result[cursize++] = *p++;
			}
#endif
		}

	}
	s_result[cursize] = 0;
	return s_result;
}

int ReplaceDone( void )
{
	//		置き換えの後処理
	//		(呼び出し前に確保したメモリバッファは解放すること)
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
void Alert( const char *mes )
{
	MessageBox( NULL, mes, "error",MB_ICONINFORMATION | MB_OK );
}

void AlertV( const char *mes, int val )
{
	char ss[1024];
	sprintf( ss, "%s%d",mes,val );
	MessageBox( NULL, ss, "error",MB_ICONINFORMATION | MB_OK );
}

void Alertf( const char *format, ... )
{
	char textbf[4096];
	va_list args;
	va_start(args, format);
	vsprintf(textbf, format, args);
	va_end(args);
	MessageBox( NULL, textbf, "error",MB_ICONINFORMATION | MB_OK );
}

#endif
