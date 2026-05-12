
//
//	supio.cpp functions
//

char *mem_ini( int size );
void mem_bye( void *ptr );
int mem_load( char *fname, void *mem, int msize );
int mem_save( char *fname, void *mem, int msize );
int filecopy( char *fname, char *sname );

void prtini( char *mes );
void prt( char *mes );
int tstrcmp( char *str1, char *str2 );
void strcase( char *str );
void getpath( char *stmp, char *outbuf, int p2 );

void Alert( char *mes );
void AlertV( char *mes, int val );
void Alertf( char *format, ... );

