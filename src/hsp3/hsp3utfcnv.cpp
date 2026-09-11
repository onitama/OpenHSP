//
//	hsp3utfcnv.cpp functions
//
#include "hsp3utfcnv.h"
#include "hsp3pathio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#if defined(HSPDEBUG) && !defined(HSPCMP)
char* hsp3ext_getdir(int id);
#endif

#if defined(HSPWIN) || defined(_WIN32)
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
FILE *hsp3_fopen(const char*name, HSPPTRINT offset)
{
	FILE* hsp3_fp = NULL;
#ifdef HSPNDK
	{
		const char *fname = name;
		if ( *name == '*' ) {
			fname = hgio_getstorage(name+1);
		}
		hsp3_fp = hgio_android_fopen(fname,offset);
		if (hsp3_fp == NULL) return NULL;
		return hsp3_fp;
	}
#endif

#ifdef HSPIOS
	{
		char* path = gb_filepath(name);
		//printf("Load %s", path);
		hsp3_fp = fopen(path, "rb");
		if (hsp3_fp == NULL) return NULL;
		return  hsp3_fp;
	}
#endif

	hsp3_fp = hsp_path_fopen(hsp_path::path_view(name), "rb");
#if defined(HSPDEBUG) && !defined(HSPCMP)
	if (hsp3_fp == NULL) {
		//	hsptvフォルダを検索する
		std::string fn;
#if defined(HSPWIN)
		if (hsp_path_get_hsptv_path(fn, hsp_path::path_view(name)) == 0) {
			hsp3_fp = hsp_path_fopen(hsp_path::path_view(fn.c_str()), "rb");
		}
#else
		if (hsp_path_get_hsptv_path_utf8(fn,
			hsp_path::utf8_view(hsp3ext_getdir(5)), hsp_path::utf8_view(name)) == 0) {
			hsp3_fp = hsp_path_fopen_utf8(hsp_path::utf8_view(fn.c_str()), "rb");
		}
#endif
	}
#endif
	if (hsp3_fp == NULL) return NULL;
	if (offset > 0) {
		hsp3_fseek(hsp3_fp, offset, SEEK_SET);
	}
	return hsp3_fp;
}


FILE* hsp3_fopenwrite(const char* fname8, HSPPTRINT offset)
{
	const char* mode = offset < 0 ? "w+b" : "r+b";
	FILE* hsp3_fp;
#ifdef HSPNDK
	char *fname = fname8;
	if ( *fname != '/' ) {
		fname = hgio_getstorage(fname8);
	}
	hsp3_fp = fopen(fname, mode);
#else
	hsp3_fp = hsp_path_fopen(hsp_path::path_view(fname8), mode);
#endif
	if (hsp3_fp == NULL) return NULL;
	if (offset >= 0) {
#ifdef HSPNDK
		fseek(hsp3_fp, offset, SEEK_SET);
#else
		hsp3_fseek(hsp3_fp, offset, SEEK_SET);
#endif
	}
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


HSPPTRINT hsp3_flength(const char* name)
{
#ifdef HSPIOS
    {
        int length = gb_existdata( name );
        if (length >= 0)
            return length;
    }
#endif

#ifdef HSPNDK
	{
	int length = hgio_file_exist( name );
	if ( length>=0 ) return length;
	const char *fname = name;
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
	HSPPTRINT length;
	FILE* hsp3_fp = hsp3_fopen(name, 0);
	if (hsp3_fp ==NULL) {
		return -1;
	}
	hsp3_fseek(hsp3_fp, 0, SEEK_END);
#if defined(HSPWIN)&&defined(HSP64)
	length = (HSPPTRINT)_ftelli64(hsp3_fp);		// normal file size
#else
	length = (HSPPTRINT)ftell(hsp3_fp);			// normal file size
#endif
	hsp3_fclose(hsp3_fp);
	return length;
}


HSPPTRINT hsp3_fread( FILE* ptr, void *mem, size_t size )
{
	if (ptr == NULL) return -1;
	if (mem == NULL) return -1;
	if (size <= 0) return 0;

#ifdef HSPNDK
	{
	return hgio_android_fread(ptr,mem,size);
	}
#endif

	HSPPTRINT len = (HSPPTRINT)fread(mem, 1, size, ptr);
	return len;
}


int hsp3_fseek(FILE* ptr, size_t offset, int whence)
{
	if (ptr == NULL) return -1;

#ifdef HSPNDK
	{
	return hgio_android_seek(ptr,offset,whence);
	}
#endif
#if defined(HSPWIN)&&defined(HSP64)
	return _fseeki64(ptr, offset, whence);
#else
	return fseek(ptr, offset, whence);
#endif
}


HSPPTRINT hsp3_binsave( const char *fname8, void *mem, size_t msize, HSPPTRINT seekofs )
{
#ifdef HSPIOS
    gb_savedata( fname8, (char *)mem, msize, seekofs );
    return msize;
#endif

	FILE* hsp3_fp = hsp3_fopenwrite( fname8, seekofs );
	if (hsp3_fp == NULL) return -1;
	size_t flen = fwrite( mem, 1, msize, hsp3_fp);
	if (flen < msize) flen = 0;

#ifdef HSPNDK
	fclose(hsp3_fp);
	return flen;
#endif

	hsp3_fclose(hsp3_fp);
#ifdef HSPWIN
	_fcloseall();
#endif
	return (HSPPTRINT)flen;
}


HSPPTRINT hsp3_rawload(const char* name, void* mem, size_t size, HSPPTRINT seekofs)
{
#ifdef HSPIOS
    int filesize = gb_existdata( name );
    if (size>=0) filesize=size;
    if (filesize>0) {
        gb_loaddata( name, (char *)mem, filesize, seekofs);
        return filesize;
    }
#endif
#ifdef HSPNDK
	const char *fname = name;
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
int hsp3_to_utf16(void* out, const char* in, int bufsize)
{
	//	hspchar->UTF16 に変換
	//
#ifdef HSPUTF8 
	return MultiByteToWideChar(CP_UTF8, 0, in, -1, (LPWSTR)out, bufsize);
#else
	return MultiByteToWideChar(CP_ACP, 0, in, -1, (LPWSTR)out, bufsize);
#endif
}


int utf16_to_hsp3(char* out, const void* in, int bufsize)
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

#if defined(HSPWIN) || defined(_WIN32)
int utf8_to_utf16_strict(void* out, const char* in, int bufsize)
{
	if (in == NULL || bufsize < 0) return 0;
	return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, in, -1,
		(LPWSTR)out, bufsize);
}


int utf16_to_utf8_strict(char* out, const void* in, int bufsize)
{
	if (in == NULL || bufsize < 0) return 0;
	return WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
		(LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, NULL);
}


int ansi_to_utf16_strict(void* out, const char* in, int bufsize)
{
	if (in == NULL || bufsize < 0) return 0;
	return MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, in, -1,
		(LPWSTR)out, bufsize);
}


int utf16_to_ansi_strict(char* out, const void* in, int bufsize)
{
	if (in == NULL || bufsize < 0) return 0;

	BOOL used_default = FALSE;
	int length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		(LPCWSTR)in, -1, NULL, 0, NULL, &used_default);
	if (length <= 0 || used_default) return 0;
	if (out == NULL || bufsize == 0) return length;

	used_default = FALSE;
	int result = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		(LPCWSTR)in, -1, (LPSTR)out, bufsize, NULL, &used_default);
	return result == 0 || used_default ? 0 : result;
}
#endif


int StrCopyLetter(const char* source, char* dest)
{
	//		1文字をコピー(utf8/sjis対応版)
	//		(移動したbyte数を返します)
	//
	unsigned char a1;
	unsigned char* p = (unsigned char*)source;
	unsigned char* dst = (unsigned char*)dest;
	int i = 1;

	a1 = *p;
#ifdef HSP_PATHIO_UTF8
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
