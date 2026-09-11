
//
//	hsp3utfcnv.cpp header
//
#ifndef __hsp3utfcnv_h
#define __hsp3utfcnv_h

#include <stdio.h>

#include "hsp3config.h"
#include "hsp3struct.h"

//------------------------------------------------------------//
/*
	windows UTF8,UTF-16 / File 関連のユーティリティ
*/
//------------------------------------------------------------//

#ifdef HSPWIN

//	Wide/Multibyte conversion
//
int hsp3_to_utf16(void* out, const char* in, int bufsize);
int utf16_to_hsp3(char* out, const void* in, int bufsize);


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

//
//	RAII wrappers for character conversion
//
class HspToApiStr {
	HSPAPICHAR* hac = nullptr;
public:
	explicit HspToApiStr(const char* s) { chartoapichar(s, &hac); }
	~HspToApiStr() { freehac(&hac); }
	operator HSPAPICHAR*() const { return hac; }
	HspToApiStr(const HspToApiStr&) = delete;
	HspToApiStr& operator=(const HspToApiStr&) = delete;
};

class ApiToHspStr {
	HSPCHAR* hc = nullptr;
public:
	explicit ApiToHspStr(const HSPAPICHAR* s) { apichartohspchar(s, &hc); }
	~ApiToHspStr() { freehc(&hc); }
	operator HSPCHAR*() const { return hc; }
	ApiToHspStr(const ApiToHspStr&) = delete;
	ApiToHspStr& operator=(const ApiToHspStr&) = delete;
};

class ApiToAnsiStr {
	char* ac = nullptr;
public:
	explicit ApiToAnsiStr(const HSPAPICHAR* s) { apichartoansichar(s, &ac); }
	~ApiToAnsiStr() { freeac(&ac); }
	operator char*() const { return ac; }
	ApiToAnsiStr(const ApiToAnsiStr&) = delete;
	ApiToAnsiStr& operator=(const ApiToAnsiStr&) = delete;
};

class AnsiToApiStr {
	HSPAPICHAR* hac = nullptr;
public:
	explicit AnsiToApiStr(const char* s) { ansichartoapichar(s, &hac); }
	~AnsiToApiStr() { freehac(&hac); }
	operator HSPAPICHAR*() const { return hac; }
	AnsiToApiStr(const AnsiToApiStr&) = delete;
	AnsiToApiStr& operator=(const AnsiToApiStr&) = delete;
};

#endif

// Explicit UTF-8/UTF-16 conversion for path and other OS boundaries.
// These functions do not depend on HSPUTF8 or the target's HSP string encoding.
#if defined(HSPWIN) || defined(_WIN32)
int utf8_to_utf16_strict(void* out, const char* in, int bufsize);
int utf16_to_utf8_strict(char* out, const void* in, int bufsize);
int ansi_to_utf16_strict(void* out, const char* in, int bufsize);
int utf16_to_ansi_strict(char* out, const void* in, int bufsize);
#endif

//		String Service
//
int StrCopyLetter(const char* source, char* dest);

//
//		File Service
//
FILE* hsp3_fopen(const char* name, HSPPTRINT offset=0);
FILE* hsp3_fopenwrite(const char* name, HSPPTRINT offset=-1);
void hsp3_fclose(FILE* ptr);
HSPPTRINT hsp3_flength(const char* name);
HSPPTRINT hsp3_fread(FILE* ptr, void* mem, size_t size);
int hsp3_fseek(FILE* ptr, size_t offset, int whence);
HSPPTRINT hsp3_binsave(const char* fname8, void* mem, size_t msize, HSPPTRINT seekofs=-1);
HSPPTRINT hsp3_rawload(const char* name, void* mem, size_t size, HSPPTRINT seekofs);

#endif
