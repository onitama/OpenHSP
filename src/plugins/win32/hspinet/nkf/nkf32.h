#ifndef NKF32_H
#ifndef CLASS_DECLSPEC
/* dll __declspec(dllexport) */
/* app __declspec(dllimport) */
#define CLASS_DECLSPEC
#endif

#ifndef LPSTR
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __BORLANDC__
#pragma argsused
#endif /*__BORLANDC__*/
/* uminchu nkf32103a.lzh 1.00 */
void CALLBACK CLASS_DECLSPEC GetNkfVersion(LPSTR verStr);
int CALLBACK CLASS_DECLSPEC SetNkfOption(LPSTR optStr);
void CALLBACK CLASS_DECLSPEC NkfConvert(LPSTR outStr, LPCSTR inStr);
/* uminchu nkf32103a.lzh 1.02 */
void CALLBACK CLASS_DECLSPEC ToHankaku(LPSTR inStr);
void CALLBACK CLASS_DECLSPEC ToZenkakuKana(LPSTR outStr ,LPCSTR inStr);
/* uminchu nkf32103a.lzh 1.03 */
void CALLBACK CLASS_DECLSPEC EncodeSubject(LPSTR outStr ,LPCSTR inStr);
/* tkaneto nkf32204.zip 2.0.4.0 */
#ifdef TOMIME
void CALLBACK CLASS_DECLSPEC ToMime(LPSTR outStr ,LPCSTR inStr);
#endif /*TOMIME*/
#ifdef GETKANJICODE
int CALLBACK CLASS_DECLSPEC NkfGetKanjiCode(VOID);
#endif /*GETKANJICODE*/
#ifdef FILECONVERT1
void CALLBACK CLASS_DECLSPEC NkfFileConvert1(LPCSTR fName);
#endif /*FILECONVERT1*/
#ifdef FILECONVERT2
void CALLBACK CLASS_DECLSPEC NkfFileConvert2(LPCSTR fInName,LPCSTR fOutName);
#endif /*FILECONVERT2*/
#ifndef NKF32103A
/* safe */
BOOL WINAPI CLASS_DECLSPEC GetNkfVersionSafeA(LPSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC ToZenkakuKanaSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC ToHankakuSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC EncodeSubjectSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert1SafeA(LPCSTR fName,DWORD nBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert2SafeA(LPCSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC GetNkfGuessA(LPSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);

BOOL WINAPI CLASS_DECLSPEC GetNkfVersionSafeW(LPWSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert1SafeW(LPCWSTR fName,DWORD nBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC NkfFileConvert2SafeW(LPCWSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCWSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC GetNkfGuessW(LPWSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/);
BOOL WINAPI CLASS_DECLSPEC GetNkfSupportFunctions(void *outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/);
BOOL WINAPI CLASS_DECLSPEC NkfUsage(LPSTR outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/);

#ifdef UNICODE
#define GetNkfVersionSafe   GetNkfVersionSafeW
#define GetNkfGuess         GetNkfGuessW
#define NkfFileConvert1Safe NkfFileConvert1SafeW
#define NkfFileConvert2Safe NkfFileConvert2SafeW
#else /*UNICODE*/
#define GetNkfVersionSafe   GetNkfVersionSafeA
#define GetNkfGuess         GetNkfGuessA
#define NkfFileConvert1Safe NkfFileConvert1SafeA
#define NkfFileConvert2Safe NkfFileConvert2SafeA
#endif /*UNICODE*/

struct NKFSUPPORTFUNCTIONS {
DWORD size;
LPCSTR copyrightA;
LPCSTR versionA;
LPCSTR dateA;
DWORD functions;
};


void reinitdll(void);
#endif /*!defined(NKF32103A)*/

#ifdef __cplusplus
}       // Balance extern "C" above
#endif /*__cplusplus*/
#endif
