
//
//	dpmread.cpp header
//
#ifndef __dpmread_h
#define __dpmread_h

int dpm_ini( char *dpmfile, ptrdiff_t dpmofs, int chksum, int deckey, int slot=0 );
void dpm_bye( void );

FILE *dpm_open( char *fname );
ptrdiff_t dpm_fread( void *mem, ptrdiff_t size, FILE *stream );
void dpm_close(FILE* fp);
ptrdiff_t dpm_read( char *fname, void *readmem, ptrdiff_t rlen, ptrdiff_t seekofs );
ptrdiff_t dpm_exist( char *fname );
void dpm_getinf( char *inf );
int dpm_filecopy( char *fname, char *sname );
int dpm_filebase( char *fname );
void dpm_memfile( void *mem, int size );
char *dpm_readalloc( char *fname );

void* dpm_stream(char* fname);				// DpmFileストリームを作成(packfile.cpp)


#endif
