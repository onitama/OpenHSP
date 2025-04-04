
//
//	Basic I/O and Message buffer support
//	For both console and Windows application development
//	"supio.cpp"
//	onion software/onitama 1997
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
	flen = fread( mem, 1, msize, fp );
	fclose(fp);
	return flen;
}
int mem_save( char *fname, void *mem, int msize )
{
	FILE *fp;
	int flen;
	fp=fopen(fname,"wb");
	if (fp==NULL) return -1;
	flen = fwrite( mem, 1, msize, fp );
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
	while(1) {
		flen = fread( mem, 1, max, fp );
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
	while(1) {
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
	while(1) {
		a1=tolower(ss[a]);
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
	while(1) {
		as=str1[ap];
		if (as!=str2[ap]) return 0;
		if (as==0) break;
		ap++;
	}
	return -1;
}


