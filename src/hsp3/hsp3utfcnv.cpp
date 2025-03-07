//
//	hsp3utfcnv.cpp functions
//
#include "hsp3config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* hsp3ext_getdir(int id);

#ifdef HSPWIN
#include <windows.h>
#include <tchar.h>
#endif

#include <ctype.h>

#ifdef HSPIOS
#include "iOSBridge.h"
#include "../hsp3dish/ios/appengine.h"
#endif

#ifdef HSPNDK
#include "../hsp3dish/hgio.h"
#include "../appengine.h"
#include "../javafunc.h"
#endif

#include "hsp3utfcnv.h"

#ifdef HSPWIN
#include "hsp3config.h"
#include "supio.h"
#include "hsp3ext.h"
#ifdef HSPUTF8
#pragma execution_character_set("utf-8")
#endif

//
//		変換用のテンポラリ
//
#define HSP3CNV_DEFSIZE 0x8000

static char* hsp3cnv_tmp = NULL;
static int hsp3cnv_tmpsize = 0;

static void hsp3cnv_cleartmp(void)
{
	if (hsp3cnv_tmp) {
		free(hsp3cnv_tmp);
		hsp3cnv_tmp = NULL;
	}
	hsp3cnv_tmpsize = 0;
}

static char* hsp3cnv_gettmp(int size)
{
	int newsize = size + 1;
	if (newsize < HSP3CNV_DEFSIZE) {
		newsize = HSP3CNV_DEFSIZE;
	}
	if (newsize > hsp3cnv_tmpsize) {
		hsp3cnv_cleartmp();
		hsp3cnv_tmpsize = newsize;
		hsp3cnv_tmp = (char *)malloc(hsp3cnv_tmpsize);
	}
	return hsp3cnv_tmp;
}


//
//		API用の文字エンコードへ変換
//

#ifdef HSPUTF8
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

#endif
#endif


//
//		basic File I/O support
//
FILE *hsp3_fopen(char*name, int offset)
{
	FILE* hsp3_fp = NULL;
#ifdef HSPWIN
#ifdef HSPUTF8
	HSPAPICHAR* hactmp1;
#endif

#ifdef HSPUTF8
	// Windows UTF
	hsp3_fp = _wfopen(chartoapichar(name, &hactmp1), L"rb");
	freehac(&hactmp1);
#else
	// Windows SJIS
	hsp3_fp = fopen(name, "rb");
#endif

	//	Read HSPTV resource
#ifdef HSPDEBUG

	if (hsp3_fp == NULL) {
		//	hsptvフォルダを検索する
		char fn[2048];
		TCHAR fporg[_MAX_PATH];
		TCHAR fporg_tmp[_MAX_PATH];
		char* resp8;
		GetModuleFileName(NULL, fporg, _MAX_PATH);
		getpathW(fporg, fporg_tmp, 32);
		apichartohspchar(fporg_tmp, &resp8);
		strcpy(fn, resp8);
		CutLastChr(fn, '\\');
		freehc(&resp8);
		strcat(fn, "\\hsptv\\");
		if ((strlen(name) + strlen(fn)) < 2047) {
			strcat(fn, name);

#ifdef HSPUTF8
			// Windows UTF
			hsp3_fp = _wfopen(chartoapichar(fn, &hactmp1), L"rb");
#else
			// Windows SJIS
			hsp3_fp = fopen(fn, "rb");
#endif
		}
	}

#endif

#else


#ifdef HSPNDK
	{
	char *fname = name;
	if ( *name == '*' ) {
		fname = hgio_getstorage(name+1);
	}
	hsp3_fp = hgio_android_fopen(fname,offset);
	if (hsp3_fp == NULL) return NULL;
	return hsp3_fp;
	}
#endif

	// Linux
	hsp3_fp = fopen(name, "rb");
#ifdef HSPDEBUG
	if (hsp3_fp == NULL) {
		//	hsptvフォルダを検索する
		char fn[2048];
		strcpy(fn, hsp3ext_getdir(5));		// tv folder
		strcat(fn, name);
		hsp3_fp = fopen(fn, "rb");
	}
#endif


#endif
	if (hsp3_fp == NULL) return NULL;
	if (offset > 0) {
		fseek(hsp3_fp, offset, SEEK_SET);
	}
	return hsp3_fp;
}


FILE* hsp3_fopenwrite(char* fname8, int offset)
{
	FILE* hsp3_fp = NULL;
#ifdef HSPWIN
#ifdef HSPUTF8
	// Windows UTF
	HSPAPICHAR* fnamew = 0;
	if (offset < 0) {
		hsp3_fp = _tfopen(chartoapichar(fname8, &fnamew), TEXT("wb"));
	}
	else {
		hsp3_fp = _tfopen(chartoapichar(fname8, &fnamew), TEXT("r+b"));
	}
	freehac(&fnamew);
#else
	// Windows SJIS
	if (offset < 0) {
		hsp3_fp = fopen(fname8, "w+b");
	}
	else {
		hsp3_fp = fopen(fname8, "r+b");
		if (hsp3_fp == NULL) return NULL;
		fseek(hsp3_fp, offset, SEEK_SET);
	}
#endif

#else

	char *fname;
	fname = fname8;
#ifdef HSPNDK
	if ( *fname != '/' ) {
		fname = hgio_getstorage(fname8);
	}
#endif
	// Linux
	if (offset < 0) {
		hsp3_fp = fopen(fname, "w+b");
	}
	else {
		hsp3_fp = fopen(fname, "r+b");
		if (hsp3_fp == NULL) return NULL;
		fseek(hsp3_fp, offset, SEEK_SET);
	}
#endif
	return hsp3_fp;
}


void hsp3_fclose(FILE* ptr)
{
#ifdef HSPNDK
	{
	return hgio_android_fclose(ptr);
	}
#endif

	fclose(ptr);
}


int hsp3_flength(char* name)
{
#ifdef HSPIOS
	{
	int length = gb_existdata( name );
	return length;
	}
#endif

#ifdef HSPNDK
	{
	int length = hgio_file_exist( name );
	if ( length>=0 ) return length;
	char *fname = name;
	if ( *fname != '/' ) {
		fname = hgio_getstorage(name);
	}
	FILE* fp = fopen(fname, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		int length = (int)ftell(fp);
		fclose(fp);
		return length;
	}
	}
#endif

	FILE* hsp3_fp = hsp3_fopen(name, 0);
	if (hsp3_fp ==NULL) {
		return -1;
	}
	fseek(hsp3_fp, 0, SEEK_END);
	int length = (int)ftell(hsp3_fp);			// normal file size
	hsp3_fclose(hsp3_fp);
	return length;
}


int hsp3_fread( FILE* ptr, void *mem, int size )
{
	if (ptr == NULL) return -1;
	if (mem == NULL) return -1;
	if (size <= 0) return 0;

#ifdef HSPNDK
	{
	return hgio_android_fread(ptr,mem,size);
	}
#endif

	int len = (int)fread(mem, 1, size, ptr);
	return len;
}


int hsp3_fseek(FILE* ptr, int offset, int whence)
{
	if (ptr == NULL) return -1;

#ifdef HSPNDK
	{
	return hgio_android_seek(ptr,offset,whence);
	}
#endif

	return fseek(ptr,offset,whence);
}


int hsp3_binsave( char *fname8, void *mem, int msize, int seekofs )
{
	FILE* hsp3_fp = hsp3_fopenwrite( fname8, seekofs );
	if (hsp3_fp == NULL) return -1;
	int flen = (int)fwrite( mem, 1, msize, hsp3_fp);

#ifdef HSPNDK
	fclose(hsp3_fp);
	return flen;
#endif

	hsp3_fclose(hsp3_fp);
#ifdef HSPWIN
	_fcloseall();
#endif
	return flen;
}


int hsp3_rawload(char* name, void* mem, int size, int seekofs)
{
#ifdef HSPNDK
	char *fname = name;
	if ( *fname != '/' ) {
		fname = hgio_getstorage(name);
	}
	FILE* fp = fopen(fname, "rb");
	if (fp==NULL) return -1;
	if (seekofs > 0) {
		fseek(fp, seekofs, SEEK_SET);
	}
	int len = (int)fread(mem, 1, size, fp);
	fclose(fp);
	return len;
#endif
	return -1;
}


//
//		UTF Conversion Service (Windows Only)
//
#ifdef HSPWIN
int hsp3_to_utf16(void* out, char* in, int bufsize)
{
	//	hspchar->UTF16 に変換
	//
#ifdef HSPUTF8 
	return MultiByteToWideChar(CP_UTF8, 0, in, -1, (LPWSTR)out, bufsize);
#else
	return MultiByteToWideChar(CP_ACP, 0, in, -1, (LPWSTR)out, bufsize);
#endif
}


int utf16_to_hsp3(char* out, void* in, int bufsize)
{
	//	UTF16->hspchar に変換
	//
#ifdef HSPUTF8 
	return WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, NULL);
#else
	return WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, NULL);
#endif
}


#endif


int hsp3_to_utf8(void* out, char* in, int bufsize)
{
	//	hspchar->UTF8 に変換
	//
#ifdef HSPWIN
#ifdef HSPUTF8 
	strncpy((char*)out, in, bufsize);
	return -1;
#else
	int reslen = MultiByteToWideChar(CP_ACP, 0, in, -1, (LPWSTR)NULL, 0);
	char* tmpbuf = hsp3cnv_gettmp(reslen);
	MultiByteToWideChar(CP_ACP, 0, in, -1, (LPWSTR)tmpbuf, hsp3cnv_tmpsize);
	return WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)tmpbuf, -1, (LPSTR)out, bufsize, NULL, NULL);
#endif
#else
	strncpy((char*)out, in, bufsize);
	return -1;
#endif
}


int utf8_to_hsp3(void* out, char* in, int bufsize)
{
	//	UTF8->hspchar に変換
	//
#ifdef HSPWIN
#ifdef HSPUTF8 
	strncpy((char*)out, in, bufsize);
	return -1;
#else
	int reslen = MultiByteToWideChar(CP_UTF8, 0, in, -1, (LPWSTR)NULL, 0);
	char* tmpbuf = hsp3cnv_gettmp(reslen);
	MultiByteToWideChar(CP_UTF8, 0, in, -1, (LPWSTR)tmpbuf, hsp3cnv_tmpsize);
	return WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)tmpbuf, -1, (LPSTR)out, bufsize, NULL, NULL);
#endif
#else
	strncpy((char*)out, in, bufsize);
	return -1;
#endif
}


int StrCopyLetter(char* source, char* dest)
{
	//		1文字をコピー(utf8/sjis対応版)
	//		(移動したbyte数を返します)
	//
	unsigned char a1;
	unsigned char* p = (unsigned char*)source;
	unsigned char* dst = (unsigned char*)dest;
	int i = 1;

	a1 = *p;
#ifdef HSPUTF8 
	if (a1 >= 128) {					// 多バイト文字チェック
		if (a1 >= 192) i++;
		if (a1 >= 224) i++;
		if (a1 >= 240) i++;
		if (a1 >= 248) i++;
		if (a1 >= 252) i++;
	}
#else
	if (a1 >= 129) {
		if ((a1 <= 159) || (a1 >= 224)) i++;
	}
#endif
	if (dst) {
		int j = 0;
		while (1) {
			if (j >= i) break;
			*dst = *p;
			dst++;
			p++;
			j++;
		}
	}
	return i;
}




