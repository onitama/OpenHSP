/* nkf32.dll nfk32dll.c */
/* e-mail:tkaneto@nifty.com */
/* URL: http://www1.ttcn.ne.jp/~kaneto */

/*WIN32DLL*/
/* こちらのバージョンも更新してください。 */
#define NKF_VERSIONW L"2.1.0"
/* NKF_VERSION のワイド文字 */
#define DLL_VERSION   "2.1.0.0 0"
/* DLLが返す */
#define DLL_VERSIONW L"2.1.0.0 0"
/* DLLが返す DLL_VERSION のワイド文字 */

/* nkf32.dll main */
#include <windows.h>
#include <tchar.h>
#include <stdarg.h>

//	DLL EXPORT
#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif

#ifdef DLLDBG /* DLLDBG @@*/
#include "nkf.h"

void dumpn(unsigned char *buff,unsigned n)
{
    int i;

    i = 0;
    while ( n ) {
        if ( i == 0 ) {
            printf(":%x  ",buff);
        }
        printf("%02x ",*buff++);
        i++;
        if ( i == 16 ) {
            printf("\n");
            i = 0;
        }
        n--;
    }
    printf("\n");
}

void dumpf(char *f);
void mkfile(char *f,char *p);
#endif /* DLLDBG @@*/

#ifndef GUESS
#define GUESS 64
#endif /*GUESS*/

char *guessbuffA = NULL;
#ifdef UNICODESUPPORT
wchar_t *guessbuffW = NULL;
UINT guessCodePage = CP_OEMCP;
DWORD guessdwFlags = MB_PRECOMPOSED;

wchar_t *tounicode(const char *p)
{
static wchar_t buff[GUESS];
    int sts;

    sts = MultiByteToWideChar(guessCodePage,guessdwFlags,p,-1,buff,sizeof(buff) / sizeof(wchar_t));
    if ( sts ) {
        return buff;
    } else {
        return L"(NULL)";
    }
}
#endif /*UNICODESUPPORT*/

char *ubuff;
int ulen;
int uret;

int dllprintf(FILE *fp,char *fmt,...)
{
    va_list argp;
    int sts;

    if ( uret != FALSE && ulen >= 1 && fmt != NULL && *fmt != 0 ) {
        va_start(argp, fmt);
        sts = _vsnprintf(ubuff,ulen - 1,fmt,argp);
        va_end(argp);
        if ( sts >= 0 ) {
            ubuff += sts;
            ulen -= sts;
        } else {
            uret = FALSE;
        }
        return sts;
    } else return 0;
}

/** Network Kanji Filter. (PDS Version)
************************************************************************
** Copyright (C) 1987, Fujitsu LTD. (Itaru ICHIKAWA)
** 連絡先： （株）富士通研究所　ソフト３研　市川　至 
** （E-Mail Address: ichikawa@flab.fujitsu.co.jp）
** Copyright (C) 1996,1998
** Copyright (C) 2002
** 連絡先： 琉球大学情報工学科 河野 真治  mime/X0208 support
** （E-Mail Address: kono@ie.u-ryukyu.ac.jp）
** 連絡先： COW for DOS & Win16 & Win32 & OS/2
** （E-Mail Address: GHG00637@niftyserve.or.p）
**
**    このソースのいかなる複写，改変，修正も許諾します。ただし、
**    その際には、誰が貢献したを示すこの部分を残すこと。
**    再配布や雑誌の付録などの問い合わせも必要ありません。
**    営利利用も上記に反しない範囲で許可します。
**    バイナリの配布の際にはversion messageを保存することを条件とします。
**    このプログラムについては特に何の保証もしない、悪しからず。
**
**    Everyone is permitted to do anything on this program 
**    including copying, modifying, improving,
**    as long as you don't try to pretend that you wrote it.
**    i.e., the above copyright notice has to appear in all copies.  
**    Binary distribution requires original version messages.
**    You don't have to ask before copying, redistribution or publishing.
**    THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE.
***********************************************************************/

static const unsigned char *cin = NULL;
static int nin = -1;
static int ninmax = -1;
static int std_getc_mode = 1;

int 
std_getc(f)
FILE *f;
{
    if (!nkf_buf_empty_p(nkf_state->std_gc_buf)) {
        return nkf_buf_pop(nkf_state->std_gc_buf);
    } else {
        if ( std_getc_mode == 1 ) {
            return getc(f);
        }
        if ( std_getc_mode == 2 && cin != NULL ) {
            if ( ninmax >= 0 ) {
                if ( nin >= ninmax ) {
                    return EOF;
                } else {
                    nin++;
                    return *cin++;
                }
            } else {
                if ( *cin ) {
                    return *cin++;
                } else {
                    return EOF;
                }
            }
        }
    }
    return EOF;
}

static FILE *fout = NULL;
static unsigned char *cout = NULL;
static int nout = -1;
static int noutmax = -1;
static int std_putc_mode = 1;

void 
std_putc(c)
int c;
{
    if(c!=EOF)
    {
        if ( (std_putc_mode & 1) && fout != NULL ) {
            putc(c,fout);
        }
        if ( (std_putc_mode & 4) && nout != -1 ) {
            if ( noutmax >= 0 && nout >= noutmax ) std_putc_mode &= ~2;
            nout++;
        }
        if ( (std_putc_mode & 2) && cout != NULL ) {
            *cout++ = c;
        }
    }
}

void
print_guessed_code (filename)
    char *filename;
{
    const char *codename = get_guessed_code();
    if (filename != NULL) {
        guessbuffA = realloc(guessbuffA,(strlen(filename) + GUESS + 1) * sizeof (char) );
        sprintf(guessbuffA,"%s:%s", filename,codename);
    } else {
        guessbuffA = realloc(guessbuffA,(GUESS + 1) * sizeof (char) );
        sprintf(guessbuffA,"%s", codename);
    }
}

#ifdef UNICODESUPPORT
void
print_guessed_codeW (filename)
    wchar_t *filename;
{
    const char *codename = get_guessed_code();
    size_t size;
    if (filename != NULL) {
	size = (wcslen(filename) + GUESS + 1) * sizeof (wchar_t);
	guessbuffW = realloc(guessbuffW, size);
	_snwprintf(guessbuffW, size, L"%s:%s", filename, tounicode(codename));
    } else {
	size = (GUESS + 1) * sizeof (wchar_t);
	guessbuffW = realloc(guessbuffW, size);
	_snwprintf(guessbuffW, size, L"%s", tounicode(codename));
    }
}
#endif /*UNICODESUPPORT*/

/**
 ** パッチ制作者
 **  void@merope.pleiades.or.jp (Kusakabe Youichi)
 **  NIDE Naoyuki <nide@ics.nara-wu.ac.jp>
 **  ohta@src.ricoh.co.jp (Junn Ohta)
 **  inouet@strl.nhk.or.jp (Tomoyuki Inoue)
 **  kiri@pulser.win.or.jp (Tetsuaki Kiriyama)
 **  Kimihiko Sato <sato@sail.t.u-tokyo.ac.jp>
 **  a_kuroe@kuroe.aoba.yokohama.jp (Akihiko Kuroe)
 **  kono@ie.u-ryukyu.ac.jp (Shinji Kono)
 **  GHG00637@nifty-serve.or.jp (COW)
 **
 **/

void
reinitdll(void)
{
    cin = NULL;
    nin = -1;
    ninmax = -1;
    std_getc_mode = 1;
    fout = stdout;
    cout = NULL;
    nout = -1;
    noutmax = -1;
    std_putc_mode = 1;
    if ( guessbuffA ) {
        free(guessbuffA);
        guessbuffA = NULL;
    }
#ifdef UNICODESUPPORT
    if ( guessbuffW ) {
        free(guessbuffW);
        guessbuffW = NULL;
    }
#endif /*UNICODESUPPORT*/
}

#ifndef DLLDBG /* DLLDBG @@*/
#if 0
int WINAPI DllEntryPoint(HINSTANCE hinst,unsigned long reason,void* lpReserved)
{
        return 1;
}
#endif
#endif /* DLLDBG @@*/

static LPSTR nkfverA = NKF_VERSION;
static LPSTR dllverA = DLL_VERSION;
#ifdef UNICODESUPPORT
static LPWSTR nkfverW = NKF_VERSIONW;
static LPWSTR dllverW = DLL_VERSIONW;
#endif /*UNICODESUPPORT*/

BOOL scp(LPSTR s,LPSTR t,DWORD n)
{
    while ( n ) {
        if ( (*s = *t) == 0 ) return TRUE;
        if ( --n == 0 ) {
            *s = 0;
            break;
        }
        s++;
        t++;
    }
    return FALSE;
}

#ifdef UNICODESUPPORT
BOOL wscp(LPWSTR s,LPWSTR t,DWORD n)
{
    while ( n ) {
        if ( (*s = *t) == 0 ) return TRUE;
        if ( --n == 0 ) {
            *s = 0;
            break;
        }
        s++;
        t++;
    }
    return FALSE;
}
#endif /*UNICODESUPPORT*/

void CALLBACK GetNkfVersion(LPSTR verStr){
    strcpy(verStr,dllverA);
}

BOOL WINAPI GetNkfVersionSafeA(LPSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
    *lpTCHARsReturned = strlen(dllverA) + 1;
    if ( verStr == NULL || nBufferLength == 0 ) return FALSE;
    return scp(verStr,dllverA,nBufferLength);
}

BOOL WINAPI GetNkfVersionSafeW(LPWSTR verStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
#ifdef UNICODESUPPORT
    *lpTCHARsReturned = wcslen(dllverW) + 1;
    if ( verStr == NULL || nBufferLength == 0 ) return FALSE;
    wcsncpy(verStr,dllverW,nBufferLength);
    if ( wcslen(dllverW) >= nBufferLength )  {
        *(verStr + nBufferLength - 1) = 0;
        return FALSE;
    }
    return TRUE;
#else /*UNICODESUPPORT*/
    return FALSE;
#endif /*UNICODESUPPORT*/
}

static LPSTR optStr0 = NULL;

int CALLBACK SetNkfOption(LPSTR optStr)
{
    LPSTR p;
    int len;

    if ( *optStr == '-' ) {
        len = strlen(optStr) + 1;
        p = realloc(optStr0,len);
        strcpy(p,optStr);
    } else {
        len = strlen(optStr) + 2;
        p = realloc(optStr0,len);
        *p = '-';
        strcpy(p + 1,optStr);
    }
    optStr0 = p;
    return 0;
}

void options0(void)
{
    reinit();
    if ( optStr0 != NULL ) {
        options(optStr0);
    }
}

void CALLBACK NkfConvert(LPSTR outStr, LPCSTR inStr)
{
    options0();
    std_putc_mode = 2;
    cout = outStr;
    noutmax = -1;
    nout = -1;
    std_getc_mode = 2;
    cin = inStr;
    ninmax = -1;
    nin = -1;
    kanji_convert(NULL);
    *cout = 0;
}

BOOL WINAPI NkfConvertSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/, LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/){
    if ( inStr == NULL ) return FALSE;
    options0();
    std_putc_mode = 6;
    cout = outStr;
    noutmax = nOutBufferLength;
    nout = 0;
    std_getc_mode = 2;
    cin = inStr;
    ninmax = nInBufferLength;
    nin = 0;
    kanji_convert(NULL);
    *lpBytesReturned = nout;
    if ( nout < noutmax ) *cout = 0;
    return TRUE;
}

void CALLBACK ToHankaku(LPSTR inStr)
{
    unsigned char *p;
    int len;

    len = strlen(inStr) + 1;
    p = malloc(len);
    if ( p == NULL ) return;
    memcpy(p,inStr,len);
    options("-ZSs");
    NkfConvert(inStr,p);
    free(p);
}

BOOL WINAPI ToHankakuSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-ZSs");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

void CALLBACK ToZenkakuKana(LPSTR outStr, LPCSTR inStr)
{
    options("-Ss");
    NkfConvert(outStr, inStr);
}

BOOL WINAPI ToZenkakuKanaSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-Ss");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

void CALLBACK EncodeSubject(LPSTR outStr ,LPCSTR inStr){
    options("-jM");
    NkfConvert(outStr, inStr);
}

BOOL WINAPI EncodeSubjectSafe(LPSTR outStr,DWORD nOutBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/,LPCSTR inStr,DWORD nInBufferLength /*in Bytes*/)
{
    options("-jM");
    return NkfConvertSafe(outStr,nOutBufferLength,lpBytesReturned,inStr,nInBufferLength);
}

#ifdef TOMIME
void CALLBACK ToMime(LPSTR outStr ,LPCSTR inStr)
{
    EncodeSubject(outStr,inStr);
}
#endif /*TOMIME*/

#ifdef GETKANJICODE
int CALLBACK NkfGetKanjiCode(VOID)
{
    int iCode=0;
    /* if(iconv == s_iconv)iCode=0; */ /* 0:シフトJIS */
    if(iconv == w_iconv)iCode=3; /* UTF-8 */
    else if(iconv == w_iconv16){
        if(input_endian == ENDIAN_BIG)iCode=5; /* 5:UTF-16BE */
        else iCode=4; /* 4:UTF-16LE */
    }else if(iconv == e_iconv){
        if(estab_f == FALSE)iCode=2; /* 2:ISO-2022-JP */
        else iCode=1; /* 1:EUC */
    }
    return iCode;
}
#endif /*GETKANJICODE*/

#ifdef FILECONVERT1
void CALLBACK NkfFileConvert1(LPCSTR fName)
{
    FILE *fin;
    char *tempdname;
    char tempfname[MAX_PATH];
    char d[4];
    DWORD len;
    BOOL sts;

    len = GetTempPath(sizeof d,d);
    tempdname = malloc(len + 1);
    if ( tempdname == NULL ) return;
    len = GetTempPath(len + 1,tempdname);
    sts = GetTempFileName(tempdname,"NKF",0,tempfname);
    if ( sts != 0 )  {
        sts = CopyFileA(fName,tempfname,FALSE);
         if ( sts ) {
             if ((fin = fopen(tempfname, "rb")) != NULL) {
                 if ((fout = fopen(fName, "wb")) != NULL) {
                     cin = NULL;
                     nin = -1;
                     ninmax = -1;
                     std_getc_mode = 1;
                     cout = NULL;
                     nout = -1;
                     noutmax = -1;
                     std_putc_mode = 1;
                     kanji_convert(fin);
                     fclose(fin);
                 }
                 fclose(fout);
             }
        DeleteFile(tempfname);
        }
    }
    free(tempdname);
}
#endif /*FILECONVERT1*/

BOOL WINAPI NkfFileConvert1SafeA(LPCSTR fName,DWORD nBufferLength /*in TCHARs*/)
{
    FILE *fin;
    char *tempdname;
    char tempfname[MAX_PATH];
    char d[4];
    DWORD len;
    BOOL sts;
    BOOL ret;
    LPCSTR p;

    ret = FALSE;
    p = fName;
    for ( ;; ) {
        if ( nBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --nBufferLength;
    }
    if ( chmod(fName,_S_IREAD | _S_IWRITE) == -1 ) return ret;
    len = GetTempPath(sizeof d,d);
    tempdname = malloc(len + 1);
    if ( tempdname == NULL ) return FALSE;
    len = GetTempPath(len + 1,tempdname);
    sts = GetTempFileName(tempdname,"NKF",0,tempfname);
    if ( sts != 0 )  {
        sts = CopyFileA(fName,tempfname,FALSE);
        if ( sts ) {
            if ((fin = fopen(tempfname, "rb")) != NULL) {
                if ((fout = fopen(fName, "wb")) != NULL) {
                    cin = NULL;
                    nin = -1;
                    ninmax = -1;
                    std_getc_mode = 1;
                    cout = NULL;
                    nout = -1;
                    noutmax = -1;
                    std_putc_mode = 1;
                    kanji_convert(fin);
                    fclose(fin);
                    ret = TRUE;
                }
                fclose(fout);
            }
            DeleteFileA(tempfname);
        }
    }
    free(tempdname);
    return ret;
}

BOOL WINAPI NkfFileConvert1SafeW(LPCWSTR fName,DWORD nBufferLength /*in TCHARs*/)
{
#ifdef UNICODESUPPORT
    FILE *fin;
    wchar_t *tempdname;
    wchar_t tempfname[MAX_PATH];
    wchar_t d[2];
    DWORD len;
    BOOL sts;
    BOOL ret;
    LPCWSTR p;

    ret = FALSE;
    p = fName;
    for ( ;; ) {
        if ( nBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --nBufferLength;
    }
    if ( _wchmod(fName,_S_IREAD | _S_IWRITE) == -1 ) return ret;
    len = GetTempPathW(sizeof d / sizeof(WCHAR),d);
    tempdname = malloc((len + 1) * sizeof(WCHAR));
    if ( tempdname == NULL ) return FALSE;
    len = GetTempPathW(len + 1,tempdname);
    sts = GetTempFileNameW(tempdname,L"NKF",0,tempfname);
    if ( sts != 0 )  {
        sts = CopyFileW(fName,tempfname,FALSE);
        if ( sts ) {
            if ((fin = _wfopen(tempfname,L"rb")) != NULL) {
                if ((fout = _wfopen(fName,L"wb")) != NULL) {
                    cin = NULL;
                    nin = -1;
                    ninmax = -1;
                    std_getc_mode = 1;
                    cout = NULL;
                    nout = -1;
                    noutmax = -1;
                    std_putc_mode = 1;
                    kanji_convert(fin);
                    fclose(fin);
                    ret = TRUE;
                }
                fclose(fout);
            }
            DeleteFileW(tempfname);
        }
    }
    free(tempdname);
    return ret;
#else /*UNICODESUPPORT*/
    return FALSE;
#endif /*UNICODESUPPORT*/
}

#ifdef FILECONVERT2
void CALLBACK NkfFileConvert2(LPCSTR fInName,LPCSTR fOutName)
{
    FILE *fin;

    if ((fin = fopen(fInName, "rb")) == NULL) return;
    if((fout=fopen(fOutName, "wb")) == NULL) {
        fclose(fin);
        return;
    }
    cin = NULL;
    nin = -1;
    ninmax = -1;
    std_getc_mode = 1;
    cout = NULL;
    nout = -1;
    noutmax = -1;
    std_putc_mode = 1;
    kanji_convert(fin);
    fclose(fin);
    fclose(fout);
}
#endif /*FILECONVERT2*/

BOOL WINAPI NkfFileConvert2SafeA(LPCSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/)
{
    FILE *fin;
    BOOL sts;
    BOOL ret;
    LPCSTR p;

    ret = FALSE;
    p = fInName;
    for ( ;; ) {
        if ( fInBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fInBufferLength;
    }
    p = fOutName;
    for ( ;; ) {
        if ( fOutBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fOutBufferLength;
    }
    if ((fin = fopen(fInName, "rb")) != NULL) {
        if((fout=fopen(fOutName, "wb")) != NULL) {
            cin = NULL;
            nin = -1;
            ninmax = -1;
            std_getc_mode = 1;
            cout = NULL;
            nout = -1;
            noutmax = -1;
            std_putc_mode = 1;
            kanji_convert(fin);
            fclose(fin);
            ret = TRUE;
        }
        fclose(fout);
    }
    return ret;
}

BOOL WINAPI NkfFileConvert2SafeW(LPCWSTR fInName,DWORD fInBufferLength /*in TCHARs*/,LPCWSTR fOutName,DWORD fOutBufferLength /*in TCHARs*/)
{
#ifdef UNICODESUPPORT
    FILE *fin;
    BOOL sts;
    BOOL ret;
    LPCWSTR p;

    ret = FALSE;
    p = fInName;
    for ( ;; ) {
        if ( fInBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fInBufferLength;
    }
    p = fOutName;
    for ( ;; ) {
        if ( fOutBufferLength == 0 ) return ret;
        if ( *p == 0 ) break;
        p++;
        --fOutBufferLength;
    }
    if ( (fin = _wfopen(fInName,L"rb")) != NULL) {
        if( (fout = _wfopen(fOutName,L"wb")) != NULL) {
            cin = NULL;
            nin = -1;
            ninmax = -1;
            std_getc_mode = 1;
            cout = NULL;
            nout = -1;
            noutmax = -1;
            std_putc_mode = 1;
            kanji_convert(fin);
            fclose(fin);
            ret = TRUE;
        }
        fclose(fout);
    }
    return ret;
#else /*UNICODESUPPORT*/
    return FALSE;
#endif /*UNICODESUPPORT*/
}

BOOL WINAPI GetNkfGuessA(LPSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
    if ( outStr == NULL || nBufferLength == 0 ) return FALSE;
    print_guessed_code(NULL);
    *lpTCHARsReturned = strlen(guessbuffA) + 1;
    return scp(outStr,guessbuffA,nBufferLength);
}

BOOL WINAPI GetNkfGuessW(LPWSTR outStr,DWORD nBufferLength /*in TCHARs*/,LPDWORD lpTCHARsReturned /*in TCHARs*/)
{
#ifdef UNICODESUPPORT
    if ( outStr == NULL || nBufferLength == 0 ) return FALSE;
    print_guessed_codeW(NULL);
    *lpTCHARsReturned = wcslen(guessbuffW) + 1;
    return wscp(outStr,guessbuffW,nBufferLength);
#else /*UNICODESUPPORT*/
    return FALSE;
#endif /*UNICODESUPPORT*/
}

static struct {
DWORD size;
LPCSTR copyrightA;
LPCSTR versionA;
LPCSTR dateA;
DWORD functions;
} NkfSupportFunctions = {
sizeof(NkfSupportFunctions),
NULL,
NKF_VERSION,
NKF_RELEASE_DATE,
1 /* nkf32103a.lzh uminchu 1.03 */
/* | 2 */ /* nkf32dll.zip 0.91 */
#if defined(TOMIME) && defined(GETKANJICODE) && defined(FILECONVERT1) && defined(FILECONVERT2) 
| 4 /* nkf32204.zip Kaneto 2.0.4.0 */
#endif
| 8 /* this */
#ifdef UNICODESUPPORT
| 0x80000000
#endif /*UNICODESUPPORT*/
,
};

BOOL WINAPI GetNkfSupportFunctions(void *outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/)
{
    *lpBytesReturned = sizeof NkfSupportFunctions;
    if ( outStr == NULL || nBufferLength == 0 ) return FALSE;
    NkfSupportFunctions.copyrightA = COPY_RIGHT;
    memcpy(outStr,&NkfSupportFunctions,sizeof NkfSupportFunctions > nBufferLength ? nBufferLength : sizeof NkfSupportFunctions);
    return TRUE;
}

BOOL WINAPI NkfUsage(LPSTR outStr,DWORD nBufferLength /*in Bytes*/,LPDWORD lpBytesReturned /*in Bytes*/)
{
    ubuff = outStr;
    ulen = nBufferLength;
    uret = TRUE;
    usage();
    if ( uret == TRUE ) {
        *lpBytesReturned = nBufferLength - ulen;
    }
    return uret;
}

/* nkf32.dll main end */

#ifdef DLLDBG /* DLLDBG @@*/
/* dbg.exe */
unsigned char buff[65536];
unsigned char buff2[65536];
unsigned char buff3[65536];
unsigned char buff4[65536];
char *code[] = {"シフトJIS","EUC","ISO-2022-JP","UTF-8","UTF-16LE","UTF-16BE"};

    int n;
    BOOL sts;
    DWORD len;

void mimeencode(unsigned char *buff2)
{
    memset(buff,0,sizeof buff);
    EncodeSubject(buff,buff2);
    printf("EncodeSubject(%s)=%s\n",buff2,buff);
        memset(buff,0,sizeof buff);
        ToMime(buff,buff2);
        printf("ToMime(%s)=%s\n",buff2,buff);
        memset(buff,0,sizeof buff);
        sts = EncodeSubjectSafe(buff,sizeof buff,&len,buff2,strlen(buff2));
        printf("EncodeSubjectSafe(%s)=%d len=%d '%s'\n",buff,sts,len,buff);
        dumpn(buff2,strlen(buff2));
        dumpn(buff,len);
}

void convert(char *arg,unsigned char *buff2)
{
    sts = SetNkfOption(arg);
    printf("SetNkfOption(%s)=%d\n",arg,sts);
    memset(buff,0,sizeof buff);
    NkfConvert(buff,buff2);
    printf("NkfConvert(%s)=%s\n",buff2,buff);
    n = NkfGetKanjiCode();
    printf("NkfGetKanjiCode()=%d\n",n);
        sts = SetNkfOption(arg);
        printf("SetNkfOption(%s)=%d\n",arg,sts);
        memset(buff,0,sizeof buff);
        sts = NkfConvertSafe(buff,sizeof buff,&len,buff2,strlen(buff2));
        printf("NkfConvertSafe(%s)=%d len=%d '%s'\n",buff2,sts,len,buff);
        dumpn(buff2,strlen(buff2));
        dumpn(buff,len);
        n = NkfGetKanjiCode();
        printf("NkfGetKanjiCode()=%d\n",n);
}

void guess(unsigned char *buff2)
{
    char *g = "--guess";

        sts = SetNkfOption(g);
        printf("SetNkfOption(%s)=%d\n",g,sts);
        memset(buff,0,sizeof buff);
        NkfConvert(buff,buff2);
        printf("NkfConvert(%s)=%s\n",buff2,buff);
        dumpn(buff2,strlen(buff2));
        n = NkfGetKanjiCode();
        printf("NkfGetKanjiCode()=%d %s\n",n,code[n]);
        memset(buff,0,sizeof buff);
        sts = GetNkfGuessA(buff,sizeof buff,&len);
        printf("GetNkfGuessA()=%d len=%d '%s'\n",sts,len,buff);
        dumpn(buff,len);
        memset(buff,0,sizeof buff);
        sts = GetNkfGuessW((LPWSTR)buff,sizeof buff / sizeof(WCHAR),&len);
        printf("GetNkfGuessW()=%d len=%d\n",sts,len);
        dumpn(buff,len * sizeof(WCHAR));
}

void dumpf(char *f)
{
    FILE *fp;
    unsigned int n;

    fp = fopen(f,"rb");
    if ( fp == NULL ) return;
    n = fread(buff,1,sizeof buff,fp);
    fclose(fp);
    printf("dumpf(%s,%d)\n",f,n);
    dumpn(buff,n);
}

void mkfile(char *f,char *p)
{
    FILE *fp;

    fp = fopen(f,"w");
    if ( fp == NULL ) return;
    fputs(p,fp);
    fclose(fp);
    dumpf(f);
}

void file(char *arg2,char *arg3,unsigned char *buf)
{
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            NkfFileConvert1(arg3);
            printf("NkfFileConvert1(%s)\n",arg3);
            dumpf(arg3);
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            sts = NkfFileConvert1SafeA(arg3,strlen(arg3) + 1);
            printf("NkfFileConvert1SafeA(%s)=%d\n",arg3,sts);
            dumpf(arg3);
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            sts = MultiByteToWideChar(CP_OEMCP,MB_PRECOMPOSED,arg3,-1,(wchar_t *)buff,sizeof(buff) / sizeof(wchar_t));
            printf("MultiByteToWideChar(%s)=%d\n",arg3,sts);
            dumpn(buff,(wcslen((wchar_t *)buff) + 1) * sizeof(wchar_t));
            sts = NkfFileConvert1SafeW((wchar_t *)buff,sizeof buff / sizeof(wchar_t) /*wcslen((wchar_t *)buff) + 1*/);
            printf("NkfFileConvert1SafeW()=%d\n",sts);
            dumpf(arg3);
}

void file2(char *arg2,char *arg3,char *arg4,unsigned char *buf)
{
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            NkfFileConvert2(arg3,arg4);
            printf("NkfFileConvert1(%s,%s)\n",arg3,arg4);
            dumpf(arg3);
            dumpf(arg4);
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            sts = NkfFileConvert2SafeA(arg3,strlen(arg3) + 1,arg4,strlen(arg4) + 1);
            printf("NkfFileConvert2SafeA(%s,%s)=%d\n",arg3,arg4,sts);
            dumpf(arg3);
            dumpf(arg4);
            sts = SetNkfOption(arg2);
            printf("SetNkfOption(%s)=%d\n",arg2,sts);
            mkfile(arg3,buf);
            sts = MultiByteToWideChar(CP_OEMCP,MB_PRECOMPOSED,arg3,-1,(wchar_t *)buff,sizeof(buff) / sizeof(wchar_t));
            printf("MultiByteToWideChar(%s)=%d\n",arg3,sts);
            dumpn(buff,(wcslen((wchar_t *)buff) + 1) * sizeof(wchar_t));
            sts = MultiByteToWideChar(CP_OEMCP,MB_PRECOMPOSED,arg4,-1,(wchar_t *)buff4,sizeof(buff4) / sizeof(wchar_t));
            printf("MultiByteToWideChar(%s)=%d\n",arg4,sts);
            dumpn(buff4,(wcslen((wchar_t *)buff4) + 1) * sizeof(wchar_t));
            sts = NkfFileConvert2SafeW((wchar_t *)buff,sizeof buff / sizeof(wchar_t) ,(wchar_t *)buff4,sizeof buff4 / sizeof(wchar_t));
            printf("NkfFileConvert2SafeW()=%d\n",sts);
            dumpf(arg3);
            dumpf(arg4);
}

int main(int argc,char **argv)
{
    struct NKFSUPPORTFUNCTIONS fnc;

    if ( argc < 2 ) return 0;
    switch ( *argv[1] ) {
      case 'v':
        memset(buff,0,sizeof buff);
        GetNkfVersion(buff);
        printf("GetNkfVersion() '%s'\n",buff);
            sts = GetNkfVersionSafeA(buff,sizeof buff,&len);
            printf("GetNkfVersionSafeA()=%d len=%d '%s'\n",sts,len,buff);
            sts = GetNkfVersionSafeW((LPWSTR)buff,sizeof buff / sizeof(WCHAR),&len);
            printf("GetNkfVersionSafeW()=%d len=%d\n",sts,len);
            dumpn(buff,len * sizeof(WCHAR));
            sts = GetNkfSupportFunctions(&fnc,sizeof fnc,&len);
            printf("GetNkfSupportFunctions()=%d len=%d\n",sts,len);
            printf("size=%d\n",fnc.size);
            printf("copyrightA='%s'\n",fnc.copyrightA);
            printf("versionA='%s'\n",fnc.versionA);
            printf("dateA='%s'\n",fnc.dateA);
            printf("functions=%d %x\n",fnc.functions,fnc.functions);
        break;
      case 'm':
        if ( argc < 3 ) return 0;
        mimeencode(argv[2]);
        break;
      case 'M':
        if ( argc < 2 ) return 0;
        gets(buff2);
        mimeencode(buff2);
        break;
      case 'c':
        if ( argc < 4 ) return 0;
        convert(argv[2],argv[3]);
        break;
      case 'C':
        if ( argc < 3 ) return 0;
        gets(buff2);
        convert(argv[2],buff2);
        break;
      case 'g':
        if ( argc < 3 ) return 0;
        guess(argv[2]);
        break;
      case 'G':
        if ( argc < 2 ) return 0;
        gets(buff2);
        guess(buff2);
        break;
      case 'f':
        if ( argc < 5 ) return 0;
        file(argv[2],argv[3],argv[4]);
        break;
      case 'F':
        if ( argc < 4 ) return 0;
        gets(buff3);
        file(argv[2],argv[3],buff3);
        break;
      case '2':
        if ( argc < 6 ) return 0;
        file2(argv[2],argv[3],argv[4],argv[5]);
        break;
      case '#':
        if ( argc < 5 ) return 0;
        gets(buff3);
        file2(argv[2],argv[3],argv[4],buff3);
        break;
      case 'u':
        sts = NkfUsage(buff,sizeof buff,&len);
        printf("strlen(buff)=%d\n",strlen(buff));
        printf("NkfUsage()=%d len=%d \n%s",sts,len,buff);
        break;
    }
    return 0;
}
/* dbg.exe end */
#endif /* DLLDBG @@*/
/*WIN32DLL*/
