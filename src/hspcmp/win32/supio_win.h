
//
//	supio.cpp functions
//

char *mem_ini( int size );
void mem_bye( void *ptr );
char *mem_alloc( void *base, int newsize, int oldsize );
int mem_load( const char *fname, void *mem, int msize );
int mem_save( const char *fname, const void *mem, int msize );
int filecopy( const char *fname, const char *sname );

//void prtini( char *mes );
//void prt( char *mes );

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
void CutLastChr( char *p, char code );

void ExecFile( char *stmp, char *ps, int mode );
void dirinfo( char *p, int id );

char *strchr3( char *target, int code, int sw, char **findptr );
void TrimCode( char *p, int code );
void TrimCodeL( char *p, int code );
void TrimCodeR( char *p, int code );

void Alert( const char *mes );
void AlertV( const char *mes, int val );
void Alertf( const char *format, ... );

int ConvSJis2Utf8( char *pSource, char *pDist, int buffersize );


