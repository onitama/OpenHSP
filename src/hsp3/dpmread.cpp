//-----------------------------------------------------
//	Datafile Pack Manager service
//	( No encryption for OpenHSP )
//			onion software 1996/6
//			Modified for win32 in 1997/8
//			Modified for HSP2.6 in 2000/7
//			Modified for HSP3.0 in 2004/11
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

#if (defined HSPUTF8 && defined HSPWIN)
#pragma execution_character_set("utf-8")
#endif

static int dpm_flag = 0;			// 0=none/1=packed
static int memf_flag = 0;			// 0=none/1=on memory
static unsigned char *mem_dpm = NULL;
static unsigned char *nameptr;
static unsigned char buf[0x80];
#if !(defined HSPUTF8 && defined HSPWIN)
static char dpm_file[HSP_MAX_PATH];
#else
static wchar_t dpm_file[HSP_MAX_PATH];
#endif
static long dpm_ofs,optr,fs,fptr;
static int dpm_fenc, dpm_enc1, dpm_enc2;
static int dpm_opmode;
static int dent;
static int seed1, seed2;
static unsigned char dpm_lastchr;
static FILE *fp;

typedef struct MEMFILE {
	char	*pt;				// target ptr
	int		cur;				// current ptr
	int		size;				// size
} MEMFILE;
static	MEMFILE memfile = { NULL, 0, -1 };


#define	p_optr 0x04			// 実データ開始ポインタ
#define p_dent 0x08			// ディレクトリエントリ数
#define p_dnam 0x0c			// ネームスペース確保サイズ(ver2.6)

#define d_farc 0x08			// 圧縮flag(先頭が'\0'の時のみ有効)
#define d_fasz 0x0c			// 圧縮後size(先頭が'\0'の時のみ有効)
#define d_fnpt 0x10			// 名前格納ポインタ(先頭が'\0'の時のみ有効)
#define d_fenc 0x14			// 暗号化flag(先頭が'\0'の時のみ有効)
#define d_fent 0x18			// ファイル格納ポインタ
#define d_fsiz 0x1c			// ファイルsize

/*----------------------------------------------------------------------------------*/

#define getdw( ofs ) ( *(unsigned long *)(buf+ofs) )

/*----------------------------------------------------------------------------------*/

static int dpmchk( char *fname )
{
	int a1,a2;
	char c1;
	char f1[HSP_MAX_PATH];
	char *ss;
	unsigned char *uc;
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif

	dpm_opmode = 0;
	dpm_fenc = 0;
	dpm_enc1 = 0; dpm_enc2 = 0;

	a1=0;while(1) {
		c1=fname[a1];f1[a1]=tolower(c1);
		if (c1==0) break;
		a1++;
	}

	if ( mem_dpm == NULL ) return -1;
	a2 = -1;
	ss = (char *)(mem_dpm + 16);
	for(a1=0;a1<dent;a1++) {
   		if ( strcmp( ss, f1 )==0 ) {
			memcpy( buf,ss,32 );
			a2=a1;
			break;
		}
		ss += 32;
	}
	if (a2==-1) return -1;

	fs = getdw( d_fsiz );
	fptr = getdw( d_fent );
#if (defined HSPUTF8 && defined HSPWIN)
	fp=_wfopen( dpm_file,L"rb");freehac(&hactmp1);
	if (fp==NULL) return -1;
#else
	fp=fopen( dpm_file,"rb" );if (fp==NULL) return -1;
#endif

	fseek( fp,fptr+optr,SEEK_SET );
	dpm_fenc = getdw( d_fenc );
	dpm_lastchr = 0;
	dpm_opmode = 1;
	return 0;

}


static int dpm_chkmemf( char *fname )
{
	//		メモリストリームをチェック
	//
	int i;
	char *p;
	char tmp[HSP_MAX_PATH];
	memf_flag=0;
	i = *(int *)fname;
	if (( i == 0x3a4d5044 )||( i == 0x3a6d7064 )) {			// 'DPM:'をチェックする
		p = strchr2( fname+4 , ':' );
		dpm_bye();
		if ( p != NULL ) {
			*p=0;
			strcpy( tmp, p+1 );
			dpm_ini( fname+4, 0, -1, -1 );
			strcpy( fname, tmp );
		}
		return 0;
	}
	if (( i == 0x3a4d454d )||( i == 0x3a6d656d )) {			// 'MEM:'をチェックする
		memf_flag = 1;
		return 1;
	}
	return 0;
}


FILE *dpm_open( char *fname )
{
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif
	dpm_chkmemf( fname );
	if ( memf_flag ) {
		memfile.cur = 0;
		if ( memfile.size < 0 ) return NULL;
		return (FILE *)&memfile;
	}
	if ( dpm_flag ) {
		if ( dpmchk( fname )==0 ) {
			return fp;
		}
	}
#if (defined HSPUTF8 && defined HSPWIN)
	fp=_wfopen( chartoapichar(fname,&hactmp1),L"rb" );
	freehac(&hactmp1);//
#else
	fp=fopen( fname,"rb" );
#endif
	return fp;
}


void dpm_close()
{
	if ( memf_flag ) { return; }
	fclose(fp);
}


int dpm_fread( void *mem, int size, FILE *stream )
{
	int a;
	int len;
	unsigned char *p;
	unsigned char ch;
	int seed1x,seed2x;

	if ( memf_flag ) {							// メモリストリーム時
		len = size;
		if (( memfile.cur + size ) >= memfile.size ) len = memfile.size - memfile.cur;
		if ( len>0 ) {
			memcpy( mem, memfile.pt + memfile.cur, len );
			memfile.cur += len;
		}
		return len;
	}
	len = (int)fread( mem, 1, size, stream );
	return len;
}

/*----------------------------------------------------------------------------------*/

int dpm_ini( char *fname, long dpmofs, int chksum, int deckey )
{
	//
	//		DPMファイル読み込みの初期化
	//
	int hedsize;
	int namsize;
	int dirsize;
	int sum,sumseed,sumsize;
	int a1;
#if !(defined HSPUTF8 && defined HSPWIN)
	char dpmfile[HSP_MAX_PATH];
#else
	WCHAR dpmfile[HSP_MAX_PATH];
#endif
	unsigned char *dec;
	HSPAPICHAR *hactmp1 = 0;

	optr=0;
	dpm_flag=0;
	memf_flag=0;
	dpm_ofs=dpmofs;
	mem_dpm = NULL;

#ifdef HSPWIN
	if ( *fname == 0 ) {
#ifndef HSPWINDLL
		GetModuleFileName( NULL,dpmfile,_MAX_PATH );
#else
		GetModuleFileName( hDllInstance,dpmfile,_MAX_PATH );
#endif
	} else {
#if !(defined HSPUTF8 && defined HSPWIN)
		strcpy( dpmfile, fname );
#else
		wcscpy(dpmfile, chartoapichar(fname,&hactmp1));
		freehac(&hactmp1);
#endif
	}
#else
	strcpy( dpmfile, fname );
#endif

#if (defined HSPUTF8 && defined HSPWIN)
	fp=_wfopen( dpmfile,L"rb" );
#else
	fp=fopen( dpmfile,"rb" );
#endif
	if (fp==NULL) return -1;

	if (dpmofs>0) fseek( fp, dpmofs, SEEK_SET );

	fread( buf,16,1,fp );
	optr=getdw( p_optr )+dpmofs;
	dent=(int)getdw( p_dent );
	fclose(fp);

	buf[4]=0;if (strcmp((char *)buf,"DPMX")!=0) {
		return -1;
	}

	namsize = getdw( p_dnam );
	dirsize = 32 * dent;
	hedsize = 16 + namsize + dirsize;
	mem_dpm = (unsigned char *)malloc( hedsize );
	if ( mem_dpm==NULL ) return -1;

	//		内部バッファにDPMヘッダを読み込む
	//
#if (defined HSPUTF8 && defined HSPWIN)
	fp=_wfopen( dpmfile,L"rb" );

#else
	fp=fopen( dpmfile,"rb" );
#endif
	if (dpmofs>0) fseek(fp, dpmofs, SEEK_SET);
	fread(mem_dpm, hedsize, 1, fp);
	fclose(fp);

	//		DPMのチェックサムを検証する
	//
	sum = 0; sumsize = 0;
	sumseed = ((deckey>>24)&0xff)/7;
	if (chksum != -1) {
#if (defined HSPUTF8 && defined HSPWIN)
		fp=_wfopen( dpmfile,L"rb" );
#else
		fp=fopen( dpmfile,"rb" );
#endif
		if (dpmofs>0) fseek( fp, dpmofs, SEEK_SET );
		while(1) {
			a1=fgetc(fp);if (a1<0) break;
			sum+=a1+sumseed;sumsize++;
		}
		fclose(fp);
		sum &= 0xffff;				// lower 16bit sum
		if ( chksum != sum ) {
			return -2;
		}
		sumsize -= hedsize;
	}

	//		DPMモードにする
	//
	dpm_flag = 1;
#if !(defined HSPUTF8 && defined HSPWIN)
	strcpy(dpm_file,dpmfile);
#else
	wcscpy(dpm_file, dpmfile);
#endif

	return 0;
}


void dpm_bye( void )
{
	if ( mem_dpm!=NULL ) free( mem_dpm );
	dpm_flag=0;
}


int dpm_read( char *fname, void *readmem, int rlen, int seekofs )
{
	char *lpRd;
	FILE *ff;
	int a1;
	int seeksize;
	int filesize;
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif

	dpm_chkmemf( fname );
	seeksize=seekofs;
	if (seeksize<0) seeksize=0;

	lpRd=(char *)readmem;

	if ( memf_flag ) {							// メモリストリーム時
		dpm_open( fname );
		memfile.cur = seeksize;
		a1 = dpm_fread( lpRd, rlen, NULL );
		dpm_close();
		return a1;
	}
		
	if (dpm_flag) {
		if ( dpmchk( fname )==0 ) {
			filesize = fs;
			fclose(fp);

			//	Read DPM file

#if (defined HSPUTF8 && defined HSPWIN)
			ff = _wfopen( dpm_file,L"rb" );
#else
			ff = fopen( dpm_file,"rb" );
#endif
			if ( ff == NULL ) return -1;
			fseek( ff, optr+fptr+seeksize, SEEK_SET );
			if ( rlen < filesize ) filesize = rlen;
			a1 = dpm_fread( lpRd, filesize, ff );
			fclose( ff );
			return a1;
		}
	}

	//	Read normal file
#if (defined HSPUTF8 && defined HSPWIN)
	ff = _wfopen( chartoapichar(fname,&hactmp1),L"rb" );
	freehac(&hactmp1);
#else
	ff = fopen( fname, "rb" );
#endif
	if ( ff == NULL ) return -1;
	if ( seekofs>=0 ) fseek( ff, seeksize, SEEK_SET );
	a1 = (int)fread( lpRd, 1, rlen, ff );
	fclose( ff );
	return a1;
}


int dpm_exist( char *fname )
{
	FILE *ff;
	int length;
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif

	dpm_chkmemf( fname );
	if ( memf_flag ) {					// メモリストリーム時
		return memfile.size;
	}

	if (dpm_flag) {
		if ( dpmchk( fname )==0 ) {
			dpm_close();
			return fs;					// dpm file size
		}
	}
#if (defined HSPUTF8 && defined HSPWIN)
	ff=_wfopen( chartoapichar(fname,&hactmp1),L"rb" );
	freehac(&hactmp1);
#else
	ff=fopen( fname,"rb" );
#endif
	if (ff==NULL) return -1;
	fseek( ff,0,SEEK_END );
	length=(int)ftell( ff );			// normal file size
	fclose(ff);

	return length;
}


int dpm_filebase( char *fname )
{
	//		指定ファイルがどこにあるかを得る
	//		(-1:error/0=file/1=dpm/2=memory)
	//
	FILE *ff;
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif
	dpm_chkmemf( fname );
	if ( memf_flag ) {
		return 2;
	}
	if ( dpm_flag ) {
		if ( dpmchk( fname )==0 ) {
			dpm_close();
			return 1;
		}
	}
#if (defined HSPUTF8 && defined HSPWIN)
	ff=_wfopen( chartoapichar(fname,&hactmp1),L"rb" );
#else
	ff=fopen( fname,"rb" );
#endif
	if (ff==NULL) return -1;
	fclose(ff);
	return 0;
}


void dpm_getinf( char *inf )
{
	long a;
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif
	a=dpm_ofs;
	if (dpm_flag==0) a=-1;
/*
	rev 43
	mingw : warning : 仮引数:int 実引数:long
	に対処
*/
#if (defined HSPUTF8 && defined HSPWIN)
	swprintf( chartoapichar(inf,&hactmp1),L"%s%d",dpm_file,static_cast< int >( a ) );
#else
	sprintf(inf,"%s,%d",dpm_file, static_cast< int >( a ) );
#endif
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
#if (defined HSPUTF8 && defined HSPWIN)
	HSPAPICHAR *hactmp1 = 0;
#endif

	dpm_chkmemf( fname );
	flen=dpm_exist(fname);
	if (flen<0) return 1;

#if (defined HSPUTF8 && defined HSPWIN)
	fp2=_wfopen(chartoapichar(sname,&hactmp1),L"wb");
	freehac(&hactmp1);
	if (fp2==NULL) return 1;
#else
	fp2=fopen(sname,"wb");if (fp2==NULL) return 1;
#endif
	fp1=dpm_open(fname);

		mem=(char *)malloc(max);
		while(1) {
			if (flen==0) break;
			if (flen<max) xlen=flen; else xlen=max;
			dpm_fread( mem, xlen, fp1 );
			fres = (int)fwrite( mem, 1, xlen, fp2 );
			if (fres<xlen) break;
			flen-=xlen;
		}

	dpm_close();
	fclose(fp2);
	free(mem);

	if (flen!=0) return 1;
	return 0;
}


void dpm_memfile( void *mem, int size )
{
	memfile.pt = (char *)mem;
	memfile.cur = 0;
	memfile.size = size;
}


char *dpm_readalloc( char *fname )
{
	char *p;
	int len;
	len = dpm_exist( fname );
	if ( len < 0 ) return NULL;
	p = mem_ini(len + 1);
	dpm_read(fname, p, len, 0);
	p[len] = 0;
	return p;
}
/*----------------------------------------------------------------------------------*/
