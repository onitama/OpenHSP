
//
//	supio.cpp functions (linux)
//
#include "../../appengine.h"

#define HSPAPICHAR char
#define HSPCHAR char

HSPAPICHAR *chartoapichar( const HSPCHAR*,HSPAPICHAR** );
void freehac( HSPAPICHAR** );
HSPCHAR *apichartohspchar( const HSPAPICHAR*,HSPCHAR** );
void freehc( HSPCHAR** );
HSPAPICHAR *ansichartoapichar(const char *, HSPAPICHAR **);
char *apichartoansichar(const HSPAPICHAR *, char **);
void freeac(char **);

char *mem_ini( size_t size );
void mem_bye( void *ptr );
int mem_save( const char *fname, void *mem, int msize, int seekofs );
void strcase( char *str );
int strcpy2( char *str1, const char *str2 );
int strcat2( char *str1, const char *str2 );
char *strstr2( char *target, const char *src );
char *strchr2( char *target, char code );
int makedir( const char *name );
int changedir( const char *name );
int delfile( const char *name );
int gettime( int index );
void strsp_ini( void );
int strsp_getptr( void );
int strsp_get( const char *srcstr, char *dststr, char splitchr, int len );
int GetLimit( int num, int min, int max );
void CutLastChr( char *p, char code );
char *strsp_cmds( char *srcstr );
int htoi( const char *str );

int SecurityCheck( char *name );

char *strchr3( char *target, int code, int sw, char **findptr );
void TrimCode( char *p, int code );
void TrimCodeL( char *p, int code );
void TrimCodeR( char *p, int code );

void Alert( const char *mes );
#define AlertV LOGI
#define Alertf LOGI

void ReplaceSetMatch( char *src, char *match, char *result, int in_src, int in_match, int in_result );
char *ReplaceStr( char *repstr );
int ReplaceDone( void );
