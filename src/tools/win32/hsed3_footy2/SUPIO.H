
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

void strsp_ini(void);
int strsp_getptr(void);
int strsp_get(char* srcstr, char* dststr, char splitchr, int len);

char* strstr2(char* target, char* src);
