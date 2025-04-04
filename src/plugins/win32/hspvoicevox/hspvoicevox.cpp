
//
//		HSP DB support DLL ( for ver2.6 )
//				onion software/onitama 2003/3
//

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>

#include "hspdll.h"
#include "simple_tts.h"

static void Alertf(const char *format, ...)
{
	char textbf[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(textbf, format, args);
	va_end(args);
	MessageBox(NULL, textbf, "error", MB_ICONINFORMATION | MB_OK);
}

/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
	}
	return TRUE ;
}

char *getvptr( HSPEXINFO *hei, PVal **pval, int *size )
{
	//		変数ポインタを得る
	//
	APTR aptr;
	PDAT *pdat;
	HspVarProc *proc;
	aptr = hei->HspFunc_prm_getva( pval );
	proc = hei->HspFunc_getproc( (*pval)->flag );
	(*pval)->offset = aptr;
	pdat = proc->GetPtr( *pval );
	return (char *)proc->GetBlockSize( *pval, pdat, size );
}

/*------------------------------------------------------------*/

//
//		変換用のテンポラリ
//
#define HSP3CNV_DEFSIZE 0x8000

static char* hsp3cnv_tmp = NULL;
static int hsp3cnv_tmpsize = 0;

static void hsp3cnv_cleartmp(void)
{
	if (hsp3cnv_tmp) {
		free(hsp3cnv_tmp);
		hsp3cnv_tmp = NULL;
	}
	hsp3cnv_tmpsize = 0;
}

static char* hsp3cnv_gettmp(int size)
{
	int newsize = size + 1;
	if (newsize < HSP3CNV_DEFSIZE) {
		newsize = HSP3CNV_DEFSIZE;
	}
	if (newsize > hsp3cnv_tmpsize) {
		hsp3cnv_cleartmp();
		hsp3cnv_tmpsize = newsize;
		hsp3cnv_tmp = (char*)malloc(hsp3cnv_tmpsize);
	}
	return hsp3cnv_tmp;
}

char *ConvSJis2Utf8(char* pSource, int *ressize)
{
	int size = 0;

	//ShiftJISからUTF-16へ変換
	const int nSize = ::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSource, -1, NULL, 0);

	BYTE* buffUtf16 = new BYTE[nSize * 2 + 2];
	::MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pSource, -1, (LPWSTR)buffUtf16, nSize);

	//UTF-16からUTF-8へ変換
	size = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, NULL, 0, NULL, NULL);
	size *= 2;

	char* pDist = hsp3cnv_gettmp(size);
	ZeroMemory(pDist, size);
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)buffUtf16, -1, (LPSTR)pDist, size, NULL, NULL);

	size = lstrlen((char*)pDist);
	if (ressize) *ressize = size;

	delete[] buffUtf16;
	return pDist;
}


char *ConvUtf82SJis(char* pSource, int *ressize)
{
	int size = 0;

	// サイズを計算する
	int iLenUnicode = ::MultiByteToWideChar(CP_UTF8, 0, pSource, strlen(pSource) + 1, NULL, 0);
	BYTE* buffUtf16 = new BYTE[iLenUnicode * 2 + 2];

	::MultiByteToWideChar(CP_UTF8, 0, pSource, strlen(pSource) + 1, (LPWSTR)buffUtf16, iLenUnicode);

	size = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)buffUtf16, iLenUnicode, NULL, 0, NULL, NULL);

	char* pDist = hsp3cnv_gettmp(size+1);

	::WideCharToMultiByte(CP_ACP, 0,
		(LPCWSTR)buffUtf16, iLenUnicode,
		pDist, size,
		NULL, NULL);

	delete[] buffUtf16;
	pDist[size] = 0;
	if (ressize) *ressize = size;
	return pDist;
}

/*------------------------------------------------------------*/

static int voicesize = 0;
static char* voicedata = NULL;

EXPORT BOOL WINAPI voicevoxinit( int p1, int p2, int p3, int p4 )
{
	//	voicevoxinit (type$00)
	//
	return tts_init();
}


EXPORT BOOL WINAPI voicevoxbye(int p1, int p2, int p3, int p4)
{
	//	voicevoxbye (type$00)
	//
	hsp3cnv_cleartmp();
	tts_term();
	return 0;
}

EXPORT BOOL WINAPI voicevoxload(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	voicevoxload id (type$202)
	//
	int res;
	int ep1;
	res = 0;
	ep1 = hei->HspFunc_prm_getdi(0);	// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	res = tts_load(ep1);
	return res;
}

EXPORT BOOL WINAPI voicevoxexec(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	voicevoxexec "text",id (type$202)
	//
	int res;
	int ep2;
	char* ep1;
	char* utf8str;
	HSPCTX* ctx;
	res = 0;
	ep1 = hei->HspFunc_prm_gets();		// パラメータ1:文字列
	ep2 = hei->HspFunc_prm_getdi(0);	// パラメータ1:数値
	if (*hei->er) return *hei->er;		// エラーチェック

	utf8str = ConvSJis2Utf8(ep1,NULL);
	ctx = hei->hspctx;

	voicedata = tts_getwav(utf8str,ep2,&voicesize);
	if (voicedata == NULL) return -1;
	ctx->strsize = voicesize;
	return 0;
}

EXPORT BOOL WINAPI voicevoxgetdata(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	voicevoxgetdata var (type$202)
	//
	PVal* pv;
	int ep1,size;
	char* res;
	res = getvptr(hei, &pv, &size);
	ep1 = hei->HspFunc_prm_getdi(0);		// パラメータ2:数値

	if (voicedata == NULL) return -1;

	if (size < voicesize) return -2;

	memcpy( res, voicedata, voicesize);

	voicedata = NULL;
	tts_closewav();
	return 0;
}


EXPORT BOOL WINAPI voicevoxgeterror(HSPEXINFO* hei, int _p1, int _p2, int _p3)
{
	//
	//		voicevoxgeterror var  (type$202)
	//
	PVal* pv;
	APTR ap;
	char* res;
	char* cnvres;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	res = (char *)tts_geterror();
	cnvres = ConvUtf82SJis(res, NULL);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, cnvres);	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI voicevoxgetversion(HSPEXINFO* hei, int _p1, int _p2, int _p3)
{
	//
	//		voicevoxgetversion var  (type$202)
	//
	PVal* pv;
	APTR ap;
	char* res;
	char* cnvres;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	res = (char*)tts_getversion();
	cnvres = ConvUtf82SJis(res, NULL);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, cnvres);	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI voicevoxgetmetas(HSPEXINFO* hei, int _p1, int _p2, int _p3)
{
	//
	//		voicevoxgetmetas var  (type$202)
	//
	PVal* pv;
	APTR ap;
	char* res;
	char* cnvres;
	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	res = (char*)tts_getmetas();
	cnvres = ConvUtf82SJis(res, NULL);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, cnvres);	// 変数に値を代入
	return 0;
}

