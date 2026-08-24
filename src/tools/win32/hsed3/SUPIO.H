
//
//	supio.cpp functions
//

char *mem_ini( int size );
void mem_bye( void *ptr );
int mem_load( const char *fname, void *mem, int msize );
int mem_save( const char *fname, void *mem, int msize );
int filecopy( const char *fname, const char *sname );

void prtini( char *mes );
void prt( const char *mes );
int tstrcmp( const char *str1, const char *str2 );
void strcase( char *str );
