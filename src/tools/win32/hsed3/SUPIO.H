
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
