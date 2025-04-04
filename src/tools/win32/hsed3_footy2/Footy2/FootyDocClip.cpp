/**
 * @file FootyDocClip.cpp
 * @brief ドキュメント管理クラス。このファイルでクリップボード関連の処理を行います
 * @author Shinji Watanabe
 * @date 2009/01/11
 */

#include "FootyDoc.h"

//-----------------------------------------------------------------------------
/**
 * @brief クリップボードへコピー処理を行います。
 */
bool CFootyDoc::ClipCopy(HWND hWnd)
{
	// 宣言
	std::wstring strSelText;
	HGLOBAL hMem;
	
	// ロックして、コピー文字列を生成
	if (!GetSelText(&strSelText))return false;
	hMem = GlobalAlloc(GHND, strSelText.size()+sizeof(wchar_t));
    memcpy(GlobalLock(hMem),strSelText.c_str(),strSelText.size()+sizeof(wchar_t));
    GlobalUnlock(hMem);
	
	// クリップボードをオープンして、コピー
	if (!OpenClipboard(hWnd))
	{
		GlobalFree(hMem);
		return false;
	}
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief クリップボードから貼り付け処理を行います。
 */
CFootyDoc::RedrawType CFootyDoc::ClipPaste(HWND hWnd)
{
	// 宣言
	HGLOBAL hMem;
	wchar_t* lpStr;
	
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// クリップボード情報を取得する
	if(!IsClipboardFormatAvailable(CF_UNICODETEXT)) // テキスト情報が利用できない
        return REDRAW_FAILED;
	if (!OpenClipboard(hWnd))
		return REDRAW_FAILED;
	hMem = (HGLOBAL*)GetClipboardData(CF_UNICODETEXT);
	if (!hMem)
	{
		CloseClipboard();
		return REDRAW_FAILED;
	}
	lpStr=(wchar_t*)GlobalLock(hMem);
	if (lpStr[0] != L'\0')				// 以下は文字列があるときのみ
		InsertString(lpStr,true);		// 挿入
	GlobalUnlock(hMem);					// グローバルメモリの解放
	CloseClipboard();					// クリップボードを閉じる
	return REDRAW_ALL;
}

/*[EOF]*/
