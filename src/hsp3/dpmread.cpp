//-----------------------------------------------------
//	Datafile Pack Manager service
//	( use DPM.EXE for pack files )
//			onion software 1996/6
//			Modified for win32 in 1997/8
//			Modified for HSP2.6 in 2000/7
//			Modified for HSP3.0 in 2004/11
//			Modified for HSP3.5 in 2016/7
//-----------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef HSPWIN
#include <windows.h>
#ifdef HSPWINDLL
extern HINSTANCE hDllInstance;
#endif
#endif

#include "hsp3config.h"
#include "dpmread.h"
#include "supio.h"

#include "filepack.h"

#ifdef HSPUTF8
#pragma execution_character_set("utf-8")
#endif



static FilePack filepack;

FILE *dpm_open( char *fname )
{
	FILE* fp = filepack.pack_fopen(fname);
	return fp;
}

void dpm_close( FILE *fp )
{
	filepack.pack_fclose(fp);
}


int dpm_fread( void *mem, int size, FILE *stream )
{
	return filepack.pack_fread(stream,mem,size);
}

void* dpm_stream(char* fname)
{
	DpmFile* dpm = new DpmFile;
	if (dpm == NULL) return NULL;
	bool res = dpm->open(&filepack, fname);
	if (res == false) {
		delete dpm;
		return NULL;
	}
	return dpm;
}


/*----------------------------------------------------------------------------------*/

int dpm_ini( char *fname, long dpmofs, int chksum, int deckey, int slot )
{
	//
	//		DPMファイル読み込みの初期化
	//
	char dpmfile[HSP_MAX_PATH];

	strcpy(dpmfile, fname);

#ifdef HSPWIN
#ifdef HSPUTF8
	WCHAR dpmfile_w[HSP_MAX_PATH];
#endif
	if ( *fname == 0 ) {
#ifndef HSPUTF8
#ifndef HSPWINDLL
		GetModuleFileName( NULL, dpmfile,_MAX_PATH );
#else
		GetModuleFileName(hDllInstance, dpmfile, _MAX_PATH);
#endif
#else
#ifndef HSPWINDLL
		GetModuleFileName(NULL, dpmfile_w, _MAX_PATH);
#else
		GetModuleFileName(hDllInstance, dpmfile_w, _MAX_PATH);
#endif
#endif

#ifdef HSPUTF8
		utf16_to_hsp3(dpmfile, (char *)dpmfile_w, _MAX_PATH);
#endif
	}
#endif

	int res = filepack.LoadPackFile(dpmfile, deckey, dpmofs, slot);
	if ( res < 0) {
		return -1;
	}
	return 0;
}


void dpm_bye( void )
{
}


int dpm_read( char *fname, void *readmem, int rlen, int seekofs )
{
	return filepack.pack_fread(fname, readmem, rlen, seekofs);
}


int dpm_exist( char *fname )
{
	return filepack.pack_flength(fname);
}


int dpm_filebase( char *fname )
{
	//		指定ファイルがどこにあるかを得る
	//		(-1:error/0=file/1=dpm/2=memory)
	//
	return filepack.pack_fbase(fname);
}


void dpm_getinf( char *inf )
{
	return filepack.pack_getinfstr(inf);
}


int dpm_filecopy( char *fname, char *sname )
{
	FILE *fp1;
	FILE *fp2;
	int fres;
	int flen;
	int xlen;
	int max=0x8000;
	char *mem;
#ifdef HSPWIN
#ifdef HSPUTF8
	HSPAPICHAR *hactmp1;
#endif
#endif

	flen= filepack.pack_flength(fname);
	if (flen<0) return 1;

#ifdef HSPWIN
#ifdef HSPUTF8
	fp2=_wfopen(chartoapichar(sname,&hactmp1),L"wb");
	freehac(&hactmp1);
	if (fp2==NULL) return 1;
#else
	fp2=fopen(sname,"wb");if (fp2==NULL) return 1;
#endif
#else
	fp2=fopen(sname,"wb");if (fp2==NULL) return 1;
#endif
	fp1 = filepack.pack_fopen(fname);
	if (fp1 == NULL) return -1;

		mem=(char *)mem_ini(max);
		while(1) {
			if (flen==0) break;
			if (flen<max) xlen=flen; else xlen=max;
			filepack.pack_fread(fp1, mem, xlen);
			fres = (int)fwrite( mem, 1, xlen, fp2 );
			if (fres<xlen) break;
			flen-=xlen;
		}

	filepack.pack_fclose(fp1);
	fclose(fp2);
	mem_bye(mem);

	if (flen!=0) return 1;
	return 0;
}


void dpm_memfile( void *mem, int size )
{
	filepack.pack_memfile(mem, size);
}


char *dpm_readalloc( char *fname )
{
	char *p;
	int len;
	len = filepack.pack_flength(fname);
	if ( len < 0 ) return NULL;
	p = mem_ini(len + 1);
	dpm_read(fname, p, len, 0);
	p[len] = 0;
	return p;
}

/*----------------------------------------------------------------------------------*/

void* dpm_getfilepack(void)
{
	return &filepack;
}


