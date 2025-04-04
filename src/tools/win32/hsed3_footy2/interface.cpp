/*------------------------------------------------------------------------------
	Interface routines for HSP Script Editor
------------------------------------------------------------------------------*/

#include <windows.h>
#include <commctrl.h>
#include "hsed_config.h"
#include "interface.h"
#include "tabmanager.h"
#include "footy2.h"

//
// 型の定義
typedef BOOL (CALLBACK *DLLFUNC)(int, int, int, int);

//
// グローバル変数
static const char *szInterfaceName = HSED_INTERFACE_NAME;

//
// 外部変数
extern HWND hWndMain;
extern HWND hwndTab;
extern HWND hwndClient;
extern HWND hwndToolBar;
extern HWND hwndStatusBar;
extern DLLFUNC hsc_ver;
extern int activeID;
extern int activeFootyID;

//
// 関数の宣言
static LRESULT InterfaceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static inline LRESULT GetHspcmpVer(HANDLE hPipe);
static inline LRESULT GetModify(int nFootyID);
static inline LRESULT GetFilePath(int nTabID, HANDLE hPipe);
static inline LRESULT GetFootyID(int nTabID);
static inline LRESULT SetText(int nFootyID, HANDLE hPipe);
static inline LRESULT SetSelText(int nFootyID, HANDLE hPipe);
static inline LRESULT GetText(int nFootyID, HANDLE hPipe);

//
// インターフェイスの初期化
bool InitInterface(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.lpszClassName = szInterfaceName;
	wcex.hInstance     = hInstance;
	wcex.lpfnWndProc   = (WNDPROC)InterfaceProc;
	wcex.hCursor       = NULL;
	wcex.hIcon         = NULL;
 	wcex.lpszMenuName  = NULL;
	wcex.hbrBackground = NULL;
	wcex.style         = 0;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hIconSm       = NULL;

	if(RegisterClassEx(&wcex) == NULL)
		return false;

	if(CreateWindow(szInterfaceName, szInterfaceName, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL) == NULL)
		return false;

	return true;
}

//
// インターフェイスのプロシージャ
static LRESULT InterfaceProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int footy_defid;
	int nRet;
	footy_defid = (int)wParam;
	if ( footy_defid < 0 ) footy_defid = activeFootyID;

	switch(uMsg){
		case HSED_GETVER:
			switch(wParam){
				case HGV_PUBLICVER:
					return HSED_PUBLIC_VER;

				case HGV_PRIVATEVER:
					return HSED_PRIVATE_VER;

				case HGV_HSPCMPVER:
					return GetHspcmpVer((HANDLE)lParam);

				case HGV_FOOTYVER:
					return GetFooty2Ver();

				case HGV_FOOTYBETAVER:
					return GetFooty2BetaVer();

				default:
					return -1;
			}

		case HSED_GETWND:
			switch(wParam){
				case HGW_MAIN:
					return (LRESULT)hWndMain;

				case HGW_CLIENT:
					return (LRESULT)hwndClient;

				case HGW_TAB:
					return (LRESULT)hwndTab;

				case HGW_FOOTY:
					return (LRESULT)Footy2GetWnd((int)lParam, 0);

				case HGW_TOOLBAR:
					return (LRESULT)hwndToolBar;

				case HGW_STATUSBAR:
					return (LRESULT)hwndStatusBar;

				default:
					return NULL;
			}

		case HSED_GETPATH:
			return GetFilePath((int)wParam, (HANDLE)lParam);

		case HSED_GETTABCOUNT:
			return TabCtrl_GetItemCount(hwndTab);

		case HSED_GETTABID:
			return GetTabID((int)wParam);

		case HSED_GETFOOTYID:
			return GetFootyID((int)wParam);

		case HSED_GETACTTABID:
			return activeID;

		case HSED_GETACTFOOTYID:
			return activeFootyID;

		case HSED_CANCOPY:
			nRet = Footy2GetSel((int)wParam, NULL, NULL, NULL, NULL);
			return (nRet == FOOTY2ERR_NOTSELECTED) ? -1 : (nRet == FOOTY2ERR_NONE);

		case HSED_CANPASTE:
			return IsClipboardFormatAvailable(CF_TEXT) != 0;

		case HSED_CANUNDO:
			nRet = 0;
			Footy2GetMetrics((int)wParam, SM_UNDOREM, &nRet);
//			return (nRet == F_RET_OUTID) ? -1 : (nRet > 0);
			return nRet;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_CANREDO:
			nRet = 0;
			Footy2GetMetrics((int)wParam, SM_REDOREM, &nRet);
//			return (nRet == F_RET_OUTID) ? -1 : (nRet > 0);
			return nRet;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_GETMODIFY:
			return GetModify((int)wParam);

		case HSED_COPY:
			return Footy2Copy(footy_defid);

		case HSED_CUT:
			return Footy2Cut(footy_defid);

		case HSED_PASTE:
			return Footy2Paste(footy_defid);

		case HSED_UNDO:
			return Footy2Undo(footy_defid);

		case HSED_REDO:
			return Footy2Redo(footy_defid);

		case HSED_INDENT:
//			return FootyIndent(footy_defid);
			return -1;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_UNINDENT:
//			return FootyUnIndent(footy_defid);
			return -1;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_SELECTALL:
			return Footy2SelectAll(footy_defid);

		case HSED_SETTEXT:
			return SetText((int)wParam, (HANDLE)lParam);

		case HSED_GETTEXT:
			return GetText((int)wParam, (HANDLE)lParam);

		case HSED_GETTEXTLENGTH:
			return Footy2GetTextLength(footy_defid, LM_CRLF);

		case HSED_GETLINES:
			return Footy2GetLines(footy_defid);

		case HSED_SETSELTEXT:
			return SetSelText((int)wParam, (HANDLE)lParam);
/*
		case HSED_GETSELTEXT:
			return ;

		case HSED_GETLINETEXT:
			return ;
*/
		case HSED_GETLINELENGTH:
			return Footy2GetLineLength(footy_defid, (int)lParam);

		case HSED_GETLINECODE:
			return Footy2GetLineCode(footy_defid);	// 2008-02-17 Shark++ SDKを見直す必要あり？
/*
		case HSED_SETSELA:
			return ;

		case HSED_GETSELA:
			return ;

		case HSED_SETSELB:
			return ;

		case HSED_GETSELB:
			return ;
*/
		case HSED_GETCARETLINE:
			Footy2GetCaretPosition(footy_defid, (size_t*)&nRet, NULL);
			return nRet+1;

		case HSED_GETCARETPOS:
			Footy2GetCaretPosition(footy_defid, NULL, (size_t*)&nRet);
			return nRet + 1;

		case HSED_SETCARETLINE:
			return Footy2SetCaretPosition(footy_defid, (int)lParam, 0);	// 2008-02-17 Shark++ SDKを見直す必要あり？SDKで使用されていない？

		case HSED_SETCARETPOS:
//			return FootySetCaretPos(footy_defid, (int)lParam);
			return 0;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_GETCARETTHROUGH:
//			return FootyGetCaretThrough(footy_defid);
			return 0;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_SETCARETTHROUGH:
//			return FootySetCaretThrough(footy_defid, (int)lParam);
			return 0;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_GETCARETVPOS:
//			return FootyGetCaretVPos(footy_defid);
			return 0;	// 2008-02-17 Shark++ 代替機能未実装

		case HSED_SETMARK:
		//	return FootySetMark(footy_defid);
			Footy2GetLineIcon(footy_defid, (size_t)lParam, &nRet);
			return Footy2SetLineIcon(footy_defid, (size_t)lParam, (nRet & ~LINEICON_BLUE) | (wParam ? LINEICON_BLUE : 0) );	// 2008-02-23 Shark++ SDKを見直す必要あり

		case HSED_GETMARK:
//			return FootyGetMark(footy_defid, (int)lParam);
			Footy2GetLineIcon(footy_defid, (size_t)lParam, &nRet);
			return (nRet & LINEICON_BLUE) ? 1 : 0;	// 2008-02-23 Shark++ 要動作確認
/*
		case HSED_SETHIGHLIGHT:
			return ;
*/


	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//
// hspcmp.dll のバージョンを取得
static inline LRESULT GetHspcmpVer(HANDLE hPipe)
{
	char szRefstr[4096];
	DWORD dwNumberOfBytesWritten;
	BOOL bRet;

	hsc_ver(0, 0, 0, (int)szRefstr);
	bRet = WriteFile(hPipe, szRefstr, lstrlen(szRefstr) + 1, &dwNumberOfBytesWritten, NULL);
	return bRet ? dwNumberOfBytesWritten : -1;
}

//
// ファイルの変更フラグを取得
static inline LRESULT GetModify(int nFootyID)
{
	int nTabID;
	TABINFO *lpTabInfo;

	nTabID = GetTabID(nFootyID);
	if(nTabID < 0) return -1;

	lpTabInfo = GetTabInfo(nTabID);
	if(lpTabInfo == NULL) return -1;

	return lpTabInfo->NeedSave != FALSE;
}

//
// ファイルのパスを取得
static inline LRESULT GetFilePath(int nTabID, HANDLE hPipe)
{
	TABINFO *lpTabInfo = GetTabInfo(nTabID);
	DWORD dwNumberOfBytesWritten;
	BOOL bRet;
	if(lpTabInfo == NULL) return -1;
	bRet = WriteFile(hPipe, lpTabInfo->FileName, lstrlen(lpTabInfo->FileName), &dwNumberOfBytesWritten, NULL);
	return bRet ? 0 : -1;
}

//
// Footy IDを取得
static inline LRESULT GetFootyID(int nTabID)
{
//	int nFootyID;
	TABINFO *lpTabInfo;

	lpTabInfo = GetTabInfo(nTabID);
	if(lpTabInfo == NULL) return -1;

	return lpTabInfo->FootyID;
}

static inline int ReadPipe(HANDLE hPipe, char **pbuffer)
{
	DWORD dwSize, dwNumberOfBytesRead;
	char *lpBuffer;

	if(!PeekNamedPipe(hPipe, NULL, 0, NULL, &dwSize, NULL)) {
		return 1;
	}
	lpBuffer = (char *)malloc(dwSize + 1);
	*pbuffer = lpBuffer;
	if(lpBuffer == NULL) return 1;
	if(dwSize > 0) {
		ReadFile(hPipe, lpBuffer, dwSize, &dwNumberOfBytesRead, NULL);
	}
	lpBuffer[dwSize] = '\0';
	return 0;
}

//
// Footyに文字列を書き込む
static inline LRESULT SetText(int nFootyID, HANDLE hPipe)
{
	int nRet;
	char *lpBuffer;

	if(ReadPipe(hPipe, &lpBuffer)){
		return -3;
	}
//	nRet = Footy2SetText(nFootyID, lpBuffer); // 2008-02-21 Shark++ こっちだと強調文字の設定が破棄される
	Footy2SelectAll(nFootyID, false);
	nRet = Footy2SetSelText(nFootyID, lpBuffer);
	free(lpBuffer);

	switch(nRet){
		case FOOTY2ERR_NONE:   return 0;
		case FOOTY2ERR_MEMORY: return -1;
		case FOOTY2ERR_NOID:   return -2;
		default:               return -4;
	}
}

//
// Footyに文字列を挿入する
static inline LRESULT SetSelText(int nFootyID, HANDLE hPipe)
{
	int nRet;
	char *lpBuffer;

	if(ReadPipe(hPipe, &lpBuffer)){
		return -3;
	}
	nRet = Footy2SetSelText(nFootyID, lpBuffer);
	free(lpBuffer);

	switch(nRet){
		case FOOTY2ERR_NONE:   return 0;
		case FOOTY2ERR_MEMORY: return -1;
		case FOOTY2ERR_NOID:   return -2;
		default:               return -4;
	}
}

//
// Footyから文字列を読み込む
static inline LRESULT GetText(int nFootyID, HANDLE hPipe)
{
	int nRet;
	DWORD dwSize, dwNumberOfBytesWritten;
	char *lpBuffer;

	dwSize = Footy2GetTextLength(nFootyID, LM_CRLF);
	lpBuffer = (char *)malloc(dwSize + 1);
	if(lpBuffer == NULL) return -1;
	nRet = Footy2GetText(nFootyID, lpBuffer, LM_CRLF, dwSize+1);
	if(nRet == FOOTY2ERR_NONE){
		if(!WriteFile(hPipe, lpBuffer, dwSize, &dwNumberOfBytesWritten, NULL)){
			free(lpBuffer);
			return -3;
		}
	} else {
		free(lpBuffer);
	}

	switch(nRet){
		case FOOTY2ERR_NONE:   return 0;
		case FOOTY2ERR_MEMORY: return -1;
		case FOOTY2ERR_NOID:   return -2;
		default:               return -4;
	}
}