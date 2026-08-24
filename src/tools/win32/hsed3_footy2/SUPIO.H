
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
void getpath( char *stmp, char *outbuf, int p2 );

void strsp_ini(void);
int strsp_getptr(void);
int strsp_get(const char* srcstr, char* dststr, char splitchr, int len);

char* strstr2(char* target, const char* src);
