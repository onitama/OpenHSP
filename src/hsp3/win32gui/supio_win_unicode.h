
#pragma execution_character_set ("utf-8")

#define HSPAPICHAR wchar_t
#define HSPCHAR char

//
//	supio.cpp functions
//

HSPAPICHAR *chartoapichar( const char*,HSPAPICHAR** );
void freehac( HSPAPICHAR** );
HSPCHAR *apichartohspchar( const HSPAPICHAR*,HSPCHAR** );
void freehc( HSPCHAR** );
HSPAPICHAR *ansichartoapichar(const char*, HSPAPICHAR**);
char *apichartoansichar(const HSPAPICHAR*, char**);
void freeac(char**);

char *mem_ini( int size );
void mem_bye( void *ptr );
char *mem_alloc( void *base, int newsize, int oldsize );
int mem_load( const char *fname, void *mem, int msize );
int mem_save( char *fname, void *mem, int msize, int seekofs );
int filecopy( const char *fname, const char *sname );

//void prtini( char *mes );
//void prt( char *mes );

size_t utf8strlen(const char *target);

int tstrcmp( const char *str1, const char *str2 );
void strcase( char *str );
void strcaseW( HSPAPICHAR *str );
void strcase2( char *str, char *str2 );
void addext( char *st, const char *exstr );
void cutext( char *st );
void cutlast( char *st );
void cutlast2( char *st );
void strcpy2( char *dest, const char *src, size_t size );
char *strchr2( char *target, char code );
int is_sjis_char_head( const unsigned char *str, int pos );
char *to_hsp_string_literal( const char *src );

void getpath( char *src, char *outbuf, int p2 );
void getpathW( HSPAPICHAR *src, HSPAPICHAR *outbuf, int p2 );

void ExecFile( char *stmp, char *ps, int mode );
void dirinfo( char *p, int id );

void Alert( const char *mes );
void AlertW( const HSPAPICHAR *mes );
void AlertV( const char *mes, int val );
void Alertf( const HSPAPICHAR *format, ... );

inline int issjisleadbyte( unsigned char c )
{
	return ( c >= 0x81 && c <= 0x9F ) || ( c >= 0xE0 && c <= 0xFC );
}

int makedir( char *name );
int changedir( char *name );
int changedirW( HSPAPICHAR *name );
int delfile( char *name );
int dirlist( char *fname, char **target, int p3 );
int gettime( int index );

int strcat2( char *str1, char *str2 );
char *strstr2( char *target, char *src );

void strsp_ini( void );
int strsp_getptr( void );
int strsp_get( char *srcstr, char *dststr, char splitchr, int len );
int GetLimit( int num, int min, int max );
void CutLastChr( char *p, char code );
char *strsp_cmds( char *srcstr );
wchar_t *strsp_cmdsW( wchar_t *srcstr );
int htoi( char *str );

int SecurityCheck( char *name );

char *strchr3( char *target, int code, int sw, char **findptr );
void TrimCode( char *p, int code );
void TrimCodeL( char *p, int code );
void TrimCodeR( char *p, int code );

void ReplaceSetMatch(char *src, char *match, char *result, int in_src, int in_match, int in_result);
char *ReplaceStr( char *repstr );
int ReplaceDone( void );

