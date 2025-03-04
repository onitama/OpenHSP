
//
//	supio.cpp functions
//

char *mem_ini( int size );
void mem_bye( void *ptr );
char *mem_alloc( void *base, int newsize, int oldsize );

int dpm_exist( char *fname );
int dpm_read( char *fname, void *readmem, int rlen, int seekofs );
char *dpm_readalloc( char *fname );
int dpm_save( char *fname, void *mem, int msize, int seekofs );

void strsp_ini( void );
int strsp_getptr( void );
int strsp_get( char *srcstr, char *dststr, char splitchr, int len );

char *strstr2( char *target, char *src );
char *strchr2( char *target, char code );

int tstrcmp( const char *str1, const char *str2 );
void strcase( char *str );
void strcase2( char *str, char *str2 );
void addext( char *st, const char *exstr );
void cutext( char *st );
void cutlast( char *st );
void cutlast2( char *st );
void strcpy2( char *dest, const char *src, size_t size );
char *strchr2( char *target, char code );
int is_sjis_char_head( const unsigned char *str, int pos );
char *to_hsp_string_literal( const char *src );
int atoi_allow_overflow( const char *s );

void getpath( const char *src, char *outbuf, int p2 );

void ExecFile( char *stmp, char *ps, int mode );
void dirinfo( char *p, int id );

char *strchr3( char *target, int code, int sw, char **findptr );
void TrimCode( char *p, int code );
void TrimCodeL( char *p, int code );
void TrimCodeR( char *p, int code );

char *gethsperror( int error );

