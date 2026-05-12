
//
//	supio.cpp functions
//

#ifdef HSPUTF8
#define HSPAPICHAR wchar_t
#define HSPCHAR char
#else
#define HSPAPICHAR char
#define HSPCHAR char
#endif

HSPAPICHAR *chartoapichar(const char*, HSPAPICHAR**);
void freehac(HSPAPICHAR**);
HSPCHAR *apichartohspchar(const HSPAPICHAR*, HSPCHAR**);
void freehc(HSPCHAR**);

char *mem_ini( int size );
void mem_bye( void *ptr );
int mem_save( LPTSTR fname, void *mem, int msize, int seekofs );
void strcase( LPTSTR str );
int strcpy2( LPTSTR str1, LPTSTR str2 );
int strcat2( LPTSTR str1, LPTSTR str2 );
char *strstr2( LPTSTR target, LPTSTR src );
char *strchr2( LPTSTR target, TCHAR code );
void getpath( LPTSTR stmp, LPTSTR outbuf, int p2 );
int makedir( LPTSTR name );
int changedir( LPTSTR name );
int delfile( LPTSTR name );
int dirlist( LPTSTR fname, LPTSTR *target, int p3 );
int gettime( int index );
void strsp_ini( void );
int strsp_getptr( void );
int strsp_get( LPTSTR srcstr, LPTSTR dststr, TCHAR splitchr, int len );
int GetLimit( int num, int min, int max );

void Alert( LPTSTR mes );
void AlertV( LPTSTR mes, int val );
void Alertf( LPTSTR format, ... );

