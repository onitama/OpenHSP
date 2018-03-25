//	dpmread.c functions

int dpm_ini( char *dpmfile, long dpmofs, int chksum, int deckey );
void dpm_bye( void );

FILE *dpm_open( char *fname );
int dpm_fread( void *mem, int size, FILE *stream );
void dpm_close();
int dpm_read( char *fname, void *readmem, int rlen, int seekofs );
int dpm_exist( char *fname );
void dpm_getinf( char *inf );
int dpm_filecopy( char *fname, char *sname );
int dpm_filebase( char *fname );
void dpm_memfile( void *mem, int size );
char *dpm_readalloc( char *fname );

