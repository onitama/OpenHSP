
//
//	supio.cpp functions
//

char *mem_ini( int size );
void mem_bye( void *ptr );
char *mem_alloc( void *base, int newsize, int oldsize );
int mem_load( const char *fname, void *mem, int msize );
int mem_save( const char *fname, void *mem, int msize );
int filecopy( const char *fname, const char *sname );

//void prtini( char *mes );
//void prt( char *mes );

int tstrcmp( const char *str1, const char *str2 );
void strcase( char *str );
void strcase2( char *str, char *str2 );
void addext( char *st, const char *exstr );
void cutlast( char *st );
void cutlast2( char *st );
void strcpy2( char *str, const char *str2, int max );

void getpath( char *stmp, char *outbuf, int p2 );

void ExecFile( const char *stmp, const char *ps, int mode );
void dirinfo( char *p, int id );
int GetLimit( int num, int min, int max );

void Alert( const char *mes );
void AlertV( const char *mes, int val );
void Alertf( const char *format, ... );
