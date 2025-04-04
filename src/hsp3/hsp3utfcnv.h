
//
//	hsp3utfcnv.cpp header
//
#ifndef __hsp3utfcnv_h
#define __hsp3utfcnv_h

#include <stdio.h>

//------------------------------------------------------------//
/*
	windows UTF8,UTF-16 / File 関連のユーティリティ
*/
//------------------------------------------------------------//

#ifdef HSPWIN

//	Wide/Multibyte conversion
//
int hsp3_to_utf16(void* out, char* in, int bufsize);
int utf16_to_hsp3(char* out, void* in, int bufsize);


#ifdef HSPUTF8
#define HSPAPICHAR wchar_t
#define HSPCHAR char

//
//	unicode functions
//

HSPAPICHAR *chartoapichar( const char*,HSPAPICHAR** );
void freehac( HSPAPICHAR** );
HSPCHAR *apichartohspchar( const HSPAPICHAR*,HSPCHAR** );
void freehc( HSPCHAR** );
HSPAPICHAR *ansichartoapichar(const char*, HSPAPICHAR**);
char *apichartoansichar(const HSPAPICHAR*, char**);
void freeac(char**);

#else

#define HSPAPICHAR char
#define HSPCHAR char

//
//	ansi functions (dummy)
//

HSPAPICHAR *chartoapichar( const HSPCHAR*,HSPAPICHAR** );
void freehac( HSPAPICHAR** );
HSPCHAR *apichartohspchar( const HSPAPICHAR*,HSPCHAR** );
void freehc( HSPCHAR** );
HSPAPICHAR *ansichartoapichar(const char *, HSPAPICHAR **);
char *apichartoansichar(const HSPAPICHAR *, char **);
void freeac(char **);

#endif
#endif

//		String Service
//
int hsp3_to_utf8(void* out, char* in, int bufsize);
int utf8_to_hsp3(void* out, char* in, int bufsize);
int StrCopyLetter(char* source, char* dest);

//
//		File Service
//
FILE* hsp3_fopen(char* name, int offset=0);
FILE* hsp3_fopenwrite(char* name, int offset=-1);
void hsp3_fclose(FILE* ptr);
int hsp3_flength(char* name);
int hsp3_fread(FILE* ptr, void* mem, int size);
int hsp3_fseek(FILE* ptr, int offset, int whence);
int hsp3_binsave(char* fname8, void* mem, int msize, int seekofs=-1);
int hsp3_rawload(char* name, void* mem, int size, int seekofs);

#endif
