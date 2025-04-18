
//
//	Basic I/O and Message buffer support
//	For both console and Windows application development
//	"supio.cpp"
//	onion software/onitama 1997
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <direct.h>


//
//		basic C I/O support
//

char *mem_ini( int size ) { return (char *)malloc(size); }
void mem_bye( void *ptr ) { free(ptr); }
int mem_load( char *fname, void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"rb");
	if (fp==NULL) return -1;
	flen = (int)fread( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}
int mem_save( char *fname, void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"wb");
	if (fp==NULL) return -1;
	flen = (int)fwrite( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}

int filecopy( char *fname, char *sname )
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
	for(;;) {
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

static	char *ermes;			// info message buffer
static	int mespt;				// info message buffer (index)
#define	maxmes 2048				// message buffer max

void prtini( char *mes )
{
	ermes=mes;
	mespt=0;
}

void prt( char *mes )
{
	//		message buffer send
	//
	char a1;
	int a;
	a=0;
	for(;;) {
		if (mespt>=maxmes) break;
		a1=mes[a++];
		if (a1==10) ermes[mespt++]=13;
		ermes[mespt]=a1;
		if (a1==0) break;
		mespt++;
	}
}

void strcase( char *str )
{
	//	string case to lower
	//
	int a;
	char a1;
	unsigned char *ss;
	ss=(unsigned char *)str;
	a=0;
	for(;;) {
		a1=(char)tolower(ss[a]);
		if (a1==0) break;
		ss[a++]=a1;
	}
}

int tstrcmp( char *str1, char *str2 )
{
	//	string compare (0=not same/-1=same)
	//
	int ap;
	char as;
	ap=0;
	for(;;) {
		as=str1[ap];
		if (as!=str2[ap]) return 0;
		if (as==0) break;
		ap++;
	}
	return -1;
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
}


static	int splc;	// split pointer

void strsp_ini(void)
{
	splc = 0;
}

int strsp_getptr(void)
{
	return splc;
}

int strsp_get(char* srcstr, char* dststr, char splitchr, int len)
{
	//		split string with parameters
	//
	unsigned char a1;
	unsigned char a2;
	int a;
	int sjflg;
	a = 0; sjflg = 0;
	while (1) {
		sjflg = 0;
		a1 = srcstr[splc];
		if (a1 == 0) break;
		splc++;
		if (a1 >= 0x81) if (a1 < 0xa0) sjflg++;
		if (a1 >= 0xe0) sjflg++;

		if (a1 == splitchr) break;
		if (a1 == 13) {
			a2 = srcstr[splc];
			if (a2 == 10) splc++;
			break;
		}
		dststr[a++] = a1;
		if (sjflg) {
			dststr[a++] = srcstr[splc++];
		}
		if (a >= len) break;
	}
	dststr[a] = 0;
	return (int)a1;
}


char* strstr2(char* target, char* src)
{
	//		strstr関数の全角対応版
	//
	unsigned char* p;
	unsigned char* s;
	unsigned char* p2;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	p = (unsigned char*)target;
	if ((*src == 0) || (*target == 0)) return NULL;
	while (1) {
		a1 = *p; if (a1 == 0) break;
		p2 = p;
		s = (unsigned char*)src;
		while (1) {
			a2 = *s++; if (a2 == 0) return (char*)p;
			a3 = *p2++; if (a3 == 0) break;
			if (a2 != a3) break;
		}
		p++;							// 検索位置を移動
		if (a1 >= 129) {					// 全角文字チェック
			if ((a1 <= 159) || (a1 >= 224)) p++;
		}
	}
	return NULL;
}


