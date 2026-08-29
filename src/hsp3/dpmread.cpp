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
#include <string>

#ifdef HSPWIN
#include <windows.h>
#ifdef HSPWINDLL
extern HINSTANCE hDllInstance;
#endif
#endif

#include "hsp3config.h"
#include "hsp3struct.h"
#include "dpmread.h"
#include "supio.h"
#include "hsp3pathio.h"

#include "filepack.h"


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


ptrdiff_t dpm_fread( void *mem, ptrdiff_t size, FILE *stream )
{
	return (ptrdiff_t)filepack.pack_fread(stream,mem,size);
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

int dpm_ini( char *fname, ptrdiff_t dpmofs, int chksum, int deckey, int slot )
{
	//
	//		DPMファイル読み込みの初期化
	//
	char dpmfile[HSP_MAX_PATH];
	if (fname == NULL || strlen(fname) >= sizeof(dpmfile)) return -1;
	strcpy(dpmfile, fname);

#ifdef HSPWIN
	if ( *fname == 0 ) {
		std::string module_filename;
#ifdef HSPWINDLL
		if (hsp_path_get_module_filename(module_filename, (void*)hDllInstance) != 0) return -1;
#else
		if (hsp_path_get_module_filename(module_filename) != 0) return -1;
#endif
		if (module_filename.size() >= sizeof(dpmfile)) return -1;
		memcpy(dpmfile, module_filename.c_str(), module_filename.size() + 1);
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


ptrdiff_t dpm_read( char *fname, void *readmem, ptrdiff_t rlen, ptrdiff_t seekofs )
{
	return (ptrdiff_t)filepack.pack_fread(fname, readmem, rlen, seekofs);
}


ptrdiff_t dpm_exist( char *fname )
{
	return (ptrdiff_t)filepack.pack_flength(fname);
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
	size_t fres;
	size_t flen;
	size_t xlen;
	size_t max=0x8000;
	char *mem;
	flen= (size_t)filepack.pack_flength(fname);
	if (flen<0) return 1;

	fp2=hsp_path_fopen(hsp_path::path_view(sname), "wb");
	if (fp2==NULL) return 1;
	fp1 = filepack.pack_fopen(fname);
	if (fp1 == NULL) {
		fclose(fp2);
		return -1;
	}

	mem=(char *)mem_ini(max);
	while(1) {
		if (flen==0) break;
		if (flen<max) xlen=flen; else xlen=max;
		filepack.pack_fread(fp1, mem, xlen);
		fres = fwrite( mem, 1, xlen, fp2 );
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
	int64_t len;
	len = filepack.pack_flength(fname);
	if ( len < 0 ) return NULL;
	p = mem_ini((size_t)len + 1);
	dpm_read(fname, p, (ptrdiff_t)len, 0);
	p[len] = 0;
	return p;
}

/*----------------------------------------------------------------------------------*/

void* dpm_getfilepack(void)
{
	return &filepack;
}
