
//
//	supio.cpp functions
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>
#include <tchar.h>
#include "supio.h"

//
//		API用の文字エンコードへ変換
//
#ifdef HSPUTF8
HSPAPICHAR *chartoapichar(const char *orig, HSPAPICHAR **pphac)
{

	int reslen;
	wchar_t *resw;
	reslen = MultiByteToWideChar(CP_UTF8, 0, orig, -1, (LPWSTR)NULL, 0);
	resw = (wchar_t*)calloc(reslen + 1, sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, orig, -1, resw, reslen);
	*pphac = resw;
	return resw;
}

void freehac(HSPAPICHAR **pphac)
{
	free(*pphac);
	*pphac = 0;
}

HSPCHAR *apichartohspchar(const HSPAPICHAR *orig, HSPCHAR **pphc)
{
	int plen;
	HSPCHAR *p;
	plen = WideCharToMultiByte(CP_UTF8, NULL, orig, -1, NULL, 0, NULL, NULL);
	p = (HSPCHAR *)calloc(plen + 1, sizeof(HSPCHAR*));
	WideCharToMultiByte(CP_UTF8, NULL, orig, -1, p, plen, NULL, NULL);
	*pphc = p;
	return p;
}

void freehc(HSPCHAR **pphc)
{
	free(*pphc);
	*pphc = 0;
}
#else
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
#endif



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


int mem_save( LPTSTR fname, void *mem, int msize, int seekofs )
{
	FILE *fp;
	int flen;

	if (seekofs<0) {
		fp=_tfopen(fname,TEXT("wb"));
	}
	else {
		fp=_tfopen(fname,TEXT("r+b"));
	}
	if (fp==NULL) return -1;
	if ( seekofs>=0 ) fseek( fp, seekofs, SEEK_SET );
	flen = (int)fwrite( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}


/*
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


int dpm_fread( void *mem, int size, FILE *stream )
{
	int len;
	len = (int)fread( mem, 1, size, stream );
	return len;
}


int dpm_exist( char *fname )
{
	FILE *ff;
	int length;

	ff=fopen( fname,"rb" );
	if (ff==NULL) return -1;
	fseek( ff,0,SEEK_END );
	length=(int)ftell( ff );			// normal file size
	fclose(ff);

	return length;
}


FILE *dpm_open( char *fname )
{
	fp=fopen( fname,"rb" );
	return fp;
}

void dpm_close()
{
	fclose(fp);
}


int dpm_filecopy( char *fname, char *sname )
{
	FILE *fp;
	FILE *fp2;
	int flen;
	int max=0x8000;
	int rval;
	char *mem;

	rval=1;
	mem=(char *)malloc(max);
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
	free(mem);
	return rval;
}


void dpm_memfile( void *mem, int size )
{
}


char *dpm_readalloc( char *fname )
{
	char *p;
	int len;
	len = dpm_exist( fname );
	if ( len < 0 ) return NULL;
	p = mem_ini( len );
	dpm_read( fname, p, len, 0 );
	return p;
}
*/

void strcase( LPTSTR target )
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
#ifndef HSPUTF8
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
#endif
	}
}


int strcpy2( LPTSTR str1, LPTSTR str2 )
{
	//	string copy (ret:length)
	//
	LPTSTR p;
	LPTSTR src;
	TCHAR a1;
	src = str2;
	p = str1;
	while(1) {
		a1=*src++;if (a1==0) break;
		*p++=a1;
	}
	*p++=0;
	return (int)(p-str1);
}


int strcat2( LPTSTR str1, LPTSTR str2 )
{
	//	string cat (ret:length)
	//
	TCHAR *src;
	TCHAR a1;
	int i;
	src = str1;
	while(1) {
		a1=*src;if (a1==0) break;
		src++;
	}
	i = (int)(src-str1);
	return (strcpy2(src,str2)+i);
}


char *strstr2( LPTSTR target, LPTSTR src )
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
#ifndef HSPUTF8
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
#endif
	}
	return NULL;
}


char *strchr2( LPTSTR target, TCHAR code )
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
#ifndef HSPUTF8
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
#endif
	}
	return res;
}


void getpath( LPTSTR stmp, LPTSTR outbuf, int p2 )
{
	LPTSTR p;
	TCHAR p_drive[_MAX_PATH];
	TCHAR p_dir[_MAX_DIR];
	TCHAR p_fname[_MAX_FNAME];
	TCHAR p_ext[_MAX_EXT];

	p = outbuf;
	if (p2&16) strcase( stmp );
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


int makedir( LPTSTR name )
{
	return _tmkdir( name );
}


int changedir( LPTSTR name )
{
	return _tchdir( name );
}


int delfile( LPTSTR name )
{
	return DeleteFile( name );
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
	SYSTEMTIME st;
	short *a;
	GetLocalTime( &st );
	a=(short *)&st;
	return (int)(a[index]);
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

int strsp_get( LPTSTR srcstr, LPTSTR dststr, TCHAR splitchr, int len )
{
	//		split string with parameters
	//
	TCHAR a1;
	TCHAR a2;
	int a;
	int sjflg;
	a=0;sjflg=0;
	while(1) {
		sjflg=0;
		a1=srcstr[splc];
		if (a1==0) break;
		splc++;
#ifndef HSPUTF8
		if (a1>=0x81) if (a1<0xa0) sjflg++;
		if (a1>=0xe0) sjflg++;
#endif

		if (a1==splitchr) break;
		if (a1==13) {
			a2=srcstr[splc];
			if (a2==10) splc++;
			break;
		}
		dststr[a++]=a1;
#ifndef HSPUTF8
		if (sjflg) {
			dststr[a++]=srcstr[splc++];
		}
#endif
		if ( a>=len ) break;
	}
	dststr[a]=0;
	return (int)a1;
}


int GetLimit( int num, int min, int max )
{
	if ( num > max ) return max;
	if ( num < min ) return min;
	return num;
}



//
//		windows debug support
//

void Alert( LPTSTR mes )
{
	MessageBox( NULL, mes, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}

void AlertV( LPTSTR *mes, int val )
{
	TCHAR ss[128];
	_stprintf( ss, TEXT("%s%d"),mes,val );
	MessageBox( NULL, ss, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}

void Alertf( LPTSTR format, ... )
{
	TCHAR textbf[1024];
	va_list args;
	va_start(args, format);
	_vstprintf(textbf, format, args);
	va_end(args);
	MessageBox( NULL, textbf, TEXT("error"),MB_ICONINFORMATION | MB_OK );
}



