/*------------------------------------------
	File Load/Save common dialog routines
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "../../hsp3/hsp3debug.h"
#include "../supio.h"

#ifndef min
#define min(prm0, prm1) (((prm0) > (prm1)) ? (prm1) : (prm0))
#endif

static HWND hwbak;
static OPENFILENAME ofn ;
static TCHAR szFileName[_MAX_PATH + 1] ;
static TCHAR szTitleName[_MAX_PATH + 1] ;
static char FileName8[(_MAX_PATH + 1)*6] ;
//static char szFilter[128];


void PopFileInitialize (HWND hwnd)
     {
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
//   ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 1 ;
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.lpstrFile         = szFileName ;
     ofn.nMaxFileTitle     = _MAX_PATH ;
     ofn.lpstrFileTitle    = szTitleName ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = NULL ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
     }

// SJISの1バイト目か調べる
#define is_sjis1(c)	 ( ( (unsigned char)(c) >= 0x81 && (unsigned char)(c) <= 0x9F ) || ( (unsigned char)(c) >= 0xE0 && (unsigned char)(c) <= 0xFC ) )

void fd_ini( HWND hwnd, char *extname, char *extinfo )
{
	// dialog p1,16,p3/dialog p1,17,p3 と OpenFileDialog/SaveFileDialogに渡すデータの
	// p1(extname)      p3(extinfo)                    フィルタ用データ 
	// "txt"            "テキストファイル"              "*.txt\0テキストファイル(*.txt)\0\0"
	// "txt;*.text"     "テキストファイル"              "*.txt;*.text\0テキストファイル(*.txt;*.text)\0\0"
	// "txt;*.text|log" "テキストファイル|ログファイル" "*.txt;*.text\0テキストファイル(*.txt;*.text)\0*.log\0ログファイル(*.log)\0\0"
	// ";a*.txt"        "テキストファイル"              "a*.txt\0テキストファイル(a*.txt)\0\0"

	// Shark++
	// ※ MSも全角を推奨していたし(メニュー文字列だったけど)もう半角捨ててもいいよね...
	// 　 ってことで"ﾌｧｲﾙ" は "ファイル" にしました。

#define realloc_filter_buffer()                        \
	pszFilterPtr = (LPTSTR)realloc(pszFilter, (nFilterLen + 1)*sizeof(TCHAR)); \
	if( NULL == pszFilterPtr ) goto out_of_memory;     \
	pszFilter = pszFilterPtr

	// 区切り文字
	// "|"区切り
	static const TCHAR DELIMITER_PIPE[]  = TEXT("|");
	static const int  DELIMITER_PIPE_LEN= 1;
	// "\n"区切り
	static const TCHAR DELIMITER_CR[]    = TEXT("\r\n");
	static const int  DELIMITER_CR_LEN  = 2;
	static const TCHAR DEFAULT_DESC[]    = TEXT("ファイル");
	static const TCHAR ALL_FILE_FILTER[] = TEXT("すべてのファイル (*.*)");

	LPTSTR pszFilter = NULL, pszFilterPtr;
	int nFilterLen;
	int nFilterSeek;
	LPTSTR fext_bak = NULL,fext = NULL, fext_next;
	LPTSTR finf_bak = NULL,finf = NULL, finf_next;
	int fext_len;
	int finf_len;
	bool no_aster;
	int nFilterIndex;

	szFileName[0]=0;
	szTitleName[0]=0;

	chartoapichar(extname,&fext_bak);
	chartoapichar(extinfo,&finf_bak);
	fext = fext_bak;
	finf = finf_bak;

	nFilterLen = 0;
	nFilterSeek = 0;

	for(nFilterIndex = 0;;
		fext = fext_next + (*DELIMITER_CR == *fext_next && DELIMITER_CR[1] == fext_next[1] ? DELIMITER_CR_LEN : DELIMITER_PIPE_LEN),
		finf = finf_next + (*DELIMITER_CR == *finf_next && DELIMITER_CR[1] == finf_next[1] ? DELIMITER_CR_LEN : DELIMITER_PIPE_LEN),
		nFilterIndex++)
	{
		// 区切り文字で分割
		for(fext_next = fext; *fext_next &&
			*DELIMITER_PIPE != *fext_next && *DELIMITER_CR != *fext_next;
			fext_next++) {
#ifndef HSPUTF8
			// SJISの1バイト目チェック＆2文字目を飛ばすときの'\0'チェック
			if( is_sjis1(*fext_next) && fext_next[1] )
				fext_next++;
#endif
		}
		for(finf_next = finf; *finf_next &&
			*DELIMITER_PIPE != *finf_next && *DELIMITER_CR != *finf_next;
			finf_next++) {
#ifndef HSPUTF8
			// SJISの1バイト目チェック＆2文字目を飛ばすときの'\0'チェック
			if( is_sjis1(*finf_next) && finf_next[1] )
				finf_next++;
#endif
		}
		if( fext_next == fext && finf_next == finf ) {
			break;
		}

		fext_len = (int)(fext_next - fext);
		finf_len = (int)(finf_next - finf);

		if( !*fext_next )
			fext_next -= DELIMITER_PIPE_LEN;
		if( !*finf_next )
			finf_next -= DELIMITER_PIPE_LEN;

		// 拡張子の先頭に';'があった場合は"*."を先頭につけないモードにする
		no_aster = (TEXT(';') == *fext);
		if( no_aster ) {
			fext++;
			fext_len--;
		}

		if( 0 == fext_len ||
			(TEXT('*') == *fext && 1 == fext_len) )
		{
			// 拡張子指定が空文字 or "*" の場合はフィルタに登録をしない
			continue;
		}

		// デフォルトファイル名指定
		if( 0 == nFilterIndex ) {
			if( !no_aster )
				_tcscat(szFileName, TEXT("*."));
			_tcsncat(szFileName, fext, min((size_t)fext_len, sizeof(szFileName)/sizeof(szFileName[0]) - 3/* strlen("*.")+sizeof('\0') */));
		}

		// finf + "(" + "*." + fext + ")" + "\0" + "*." + fext + "\0"
		nFilterSeek = nFilterLen;
		nFilterLen += finf_len + 1 + 2 + fext_len + 1 + 1 + 2 + fext_len + 1 + (no_aster ? -4 : 0);
		if( 0 == finf_len ) {
			// ファイルの説明が空文字の場合は拡張子+"ファイル"に
			nFilterLen += fext_len;
			nFilterLen += (int)_tcslen(DEFAULT_DESC); // ※
		}
		realloc_filter_buffer();

		pszFilterPtr = pszFilter + nFilterSeek;
		*pszFilterPtr = TEXT('\0');

		// フィルタ説明
		if( 0 == finf_len ) {
			_tcsncat(pszFilterPtr, fext, (size_t)fext_len);
			_tcscat(pszFilterPtr, DEFAULT_DESC); // ※
		} else {
			_tcsncat(pszFilterPtr, finf, (size_t)finf_len);
		}

		_tcscat(pszFilterPtr,  no_aster ? TEXT("(") : TEXT("(*."));
		_tcsncat(pszFilterPtr, fext, (size_t)fext_len);
		_tcscat(pszFilterPtr,  TEXT(")"));
		_tcscat(pszFilterPtr,  DELIMITER_PIPE);

		// フィルタ拡張子
		if( !no_aster )
			_tcscat(pszFilterPtr, TEXT("*."));
		_tcsncat(pszFilterPtr, fext, (size_t)fext_len);
		_tcscat(pszFilterPtr,  DELIMITER_PIPE);
	}

	// "すべてのファイル (*.*)" + "\0" + "*.*" + "\0" + "\0"
	nFilterSeek = nFilterLen;
	nFilterLen += (int)_tcslen(ALL_FILE_FILTER) + 1 + (int)_tcslen(TEXT("*.*")) + 1 + 1;
	realloc_filter_buffer();

	pszFilterPtr = pszFilter + nFilterSeek;
	*pszFilterPtr = TEXT('\0');

	// フィルタ説明
	_tcscat(pszFilterPtr, ALL_FILE_FILTER); // ※
	_tcscat(pszFilterPtr, DELIMITER_PIPE);

	// フィルタ拡張子
	_tcscat(pszFilterPtr, TEXT("*.*"));
	_tcscat(pszFilterPtr, DELIMITER_PIPE);
	_tcscat(pszFilterPtr, DELIMITER_PIPE);

//	for(int i = 0; i < nFilterLen-1; i++) if('\0'==pszFilter[i]) pszFilter[i] = '|';
//	MessageBox(NULL,pszFilter,"",0);

	// 区切り文字を'\0'に変換
	pszFilterPtr = pszFilter;
	for(nFilterSeek = 0; nFilterSeek < nFilterLen; pszFilterPtr++, nFilterSeek++) {
#ifndef HSPUTF8
		if( is_sjis1(*pszFilterPtr) )
			pszFilterPtr++, nFilterSeek++;
		else if( *DELIMITER_PIPE == *pszFilterPtr )
			*pszFilterPtr = TEXT('\0');
#else
		if (*DELIMITER_PIPE == *pszFilterPtr )
			*pszFilterPtr = TEXT('\0');
#endif
	}
	
	PopFileInitialize(hwnd);
	ofn.lpstrFilter = pszFilter;

	freehac(&fext_bak);
	freehac(&finf_bak);

#undef realloc_filter_buffer

	return;

out_of_memory:
	free(pszFilter);
	throw HSPERR_OUT_OF_MEMORY;
}

char *fd_getfname( void )
{
	HSPCHAR *hctmp1 = 0;
	int hclen;
	apichartohspchar(szFileName,&hctmp1);
	hclen = strlen(hctmp1);
	memcpy(FileName8,hctmp1,hclen);
	FileName8[hclen] = '\0';
	freehc(&hctmp1);
	return FileName8;
}

BOOL fd_dialog( HWND hwnd, int mode, char *fext, char *finf )
{
	BOOL bResult = FALSE;
	switch(mode) {
	case 0:
		fd_ini( hwnd, fext, finf );
		ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
		bResult = GetOpenFileName (&ofn) ;
		free((void*)ofn.lpstrFilter);
		ofn.lpstrFilter = NULL;
		break;
	case 1:
		fd_ini(hwnd, fext, finf);
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
		bResult = GetSaveFileName (&ofn) ;
		free((void*)ofn.lpstrFilter);
		ofn.lpstrFilter = NULL;
		break;
	}
	return bResult;
}


DWORD fd_selcolor( HWND hwnd, int mode )
     {
	 BOOL res;
     static CHOOSECOLOR cc ;
     static COLORREF    crCustColors[16] ;

     cc.lStructSize    = sizeof (CHOOSECOLOR) ;
     cc.hwndOwner      = hwnd ;
     cc.hInstance      = NULL ;
     cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
     cc.lpCustColors   = crCustColors ;

	 if (mode)
	     cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
	 else
		 cc.Flags          = CC_RGBINIT;

     cc.lCustData      = 0L ;
     cc.lpfnHook       = NULL ;
     cc.lpTemplateName = NULL ;

     res=ChooseColor(&cc) ;
	 if (res) {
		return (DWORD)cc.rgbResult;
	 }
/*
	rev 43
	mingw : warning : DWORD型の戻り値に-1を返している
	に対処
*/
	 	return static_cast< DWORD >( -1 );
     }
