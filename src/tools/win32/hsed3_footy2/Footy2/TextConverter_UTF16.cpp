/**
 * @file TextConverter_UTF16.cpp
 * @brief UTF16への変換クラス
 * @version 1.0
 * @author ShinjiWatanabe
 */

#include "TextConverter_Unicode.h"

/**
 * CTextConverter_UTF16LE::ToUnicode
 * @brief UTF16からUnicodeに変換する
 * @param pChar 変換元文字列
 * @param nSize 文字サイズ(バイト)
 * @return 変換出来たらtrue
 */
bool CTextConverter_UTF16LE::ToUnicode(const char* pChar,size_t nSize){
	// 宣言
	size_t i,nStartPos;
	
	// BOMがあったら無視する
	if (strncmp(pChar,"\xFF\xFE",2) == 0){
		m_bIncludeBOM = true;
		nStartPos=2;
	}
	else{
		m_bIncludeBOM = false;
		nStartPos=0;
	}

	// メモリを確保
	if ((nSize - nStartPos) % 2 != 0)return false;
	if (!Alloc(nSize - nStartPos + sizeof(wchar_t)))
		return false;

	// メモリをコピーする
	for (i=0;i<nSize - nStartPos;i++){
		m_pOutputText[i] = pChar[i + nStartPos];
	}

	// 最後にNULL文字を
	m_pOutputText[nSize - nStartPos + 0] = 0;
	m_pOutputText[nSize - nStartPos + 1] = 0;
	return true;
}

/**
 * CTextConverter_UTF16LE::ToMulti
 * @brief UTF16に変換する処理
 * @param pChar 変換元文字列
 * @param nSize 文字サイズ(バイト)
 * @return 変換出来たらtrue
 */
bool CTextConverter_UTF16LE::ToMulti(const wchar_t *pChar,size_t nSize){
	// 宣言
	size_t i,nStartPos;

	// メモリを確保
	if (m_bIncludeBOM)nStartPos = 2;
	else nStartPos = 0;
	if (!Alloc(nSize * sizeof(wchar_t) + nStartPos + sizeof(char)))
		return false;
	
	// BOMがあったら書き込み
	if (m_bIncludeBOM)
	{
		memcpy(m_pOutputText, "\xFF\xFE", 2);
	}

	// メモリをコピーする
	for (i=0;i<nSize;i++){
		((wchar_t*)(&m_pOutputText[nStartPos]))[i] = pChar[i];
	}

	// 最後にNULL文字を
	m_pOutputText[nSize * sizeof(wchar_t) + nStartPos] = 0;
	return true;
}

/**
 * CTextConverter_UTF16BE::ToUnicode
 * @brief UTF16からUnicodeに変換する
 * @param pChar 変換元文字列
 * @param nSize 変換サイズ
 * @return 成功したときtrue
 */
bool CTextConverter_UTF16BE::ToUnicode(const char* pChar,size_t nSize){
	// 宣言
	size_t i,nStartPos;

	// BOMがあったら無視する
	if (strncmp(pChar,"\xFE\xFF",2) == 0){
		m_bIncludeBOM  = true;
		nStartPos=2;
	}
	else{
		m_bIncludeBOM = false;
		nStartPos=0;
	}

	// メモリを確保
	if ((nSize - nStartPos) % 2 != 0)return false;
	if (!Alloc(nSize - nStartPos + sizeof(wchar_t)))
		return false;

	// メモリをコピーする
	for (i=0;i<nSize - nStartPos;i++){
		if (i % 2 == 0)
			m_pOutputText[i] = pChar[i + nStartPos + 1];
		else
			m_pOutputText[i] = pChar[i + nStartPos - 1];
	}

	// 最後にNULL文字を
	m_pOutputText[nSize - nStartPos + 0] = 0;
	m_pOutputText[nSize - nStartPos + 1] = 0;
	return true;
}

/**
 * CTextConverter_UTF16BE::ToMulti
 * @brief UTF16に変換する処理
 * @param pChar 変換元文字列
 * @param nSize 変換サイズ
 */
bool CTextConverter_UTF16BE::ToMulti(const wchar_t *pChar,size_t nSize){
	// 宣言
	size_t i,nStartPos;
	
	// メモリを確保
	if (m_bIncludeBOM)nStartPos = 2;
	else nStartPos = 0;
	if (!Alloc(nSize * sizeof(wchar_t) + nStartPos + sizeof(char)))
		return false;
	
	// BOMがあったら書き込み
	if (m_bIncludeBOM)
	{
		memcpy(m_pOutputText, "\xFE\xFF", 2);
	}

	// メモリをコピーする
	for (i=0;i<nSize;i++){
		((wchar_t*)(&m_pOutputText[nStartPos]))[i] =
			((pChar[i] & 0xFF00) >> 8) | ((pChar[i] & 0x00FF) << 8);
	}

	// 最後にNULL文字を
	m_pOutputText[nSize * sizeof(wchar_t) + nStartPos] = 0;
	return true;
}

/*[EOF]*/
