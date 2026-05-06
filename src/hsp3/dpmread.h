
//
//	dpmread.cpp header
//
#ifndef __dpmread_h
#define __dpmread_h

int dpm_ini( char *dpmfile, size_t dpmofs, int chksum, int deckey, int slot=0 );
void dpm_bye( void );

FILE *dpm_open( char *fname );
size_t dpm_fread( void *mem, size_t size, FILE *stream );
void dpm_close(FILE* fp);
size_t dpm_read( char *fname, void *readmem, size_t rlen, size_t seekofs );
size_t dpm_exist( char *fname );
void dpm_getinf( char *inf );
int dpm_filecopy( char *fname, char *sname );
int dpm_filebase( char *fname );
void dpm_memfile( void *mem, int size );
char *dpm_readalloc( char *fname );

void* dpm_stream(char* fname);				// DpmFileストリームを作成(packfile.cpp)


#endif
