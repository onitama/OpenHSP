/**
 * @file TextConverter_UTF32.cpp
 * @brief テキストの変換処理が可能なクラスです。
 * @author Shinji Watanabe
 * @version 1.0
 */

#include "TextConverter_Unicode.h"

/*----------------------------------------------------------------
CTextConverter_UTF32LE::ToUnicode
UTF16からUnicodeに変換する
----------------------------------------------------------------*/
bool CTextConverter_UTF32LE::ToUnicode(const char* pChar,size_t nSize){
	/*宣言*/
	size_t i,nStartPos;
	size_t nAllocSize = 0;
	size_t nUnicodePos = 0;
	
	/*BOMがあったら無視する*/
	if (memcmp(pChar,"\xFF\xFE\x00\x00",4) == 0){
		m_bIncludeBOM = true;
		nStartPos=4;
	}
	else{
		m_bIncludeBOM = false;
		nStartPos=0;
	}

	/*必要なメモリ領域を計算*/
	for (i=nStartPos;i<nSize;i += 4){
		unsigned long u = *((unsigned long*)(&pChar[i]));
		if		(0x00000000 <= u && u <= 0x0000FFFF){	/*サロゲートペア無し*/
			nAllocSize += sizeof(wchar_t);
		}
		else if (0x00010000 <= u && u <= 0x000FFFFF){	/*サロゲートペアあり*/
			nAllocSize += sizeof(wchar_t) * 2;
		}
	}
	if (!Alloc(nAllocSize + sizeof(wchar_t)))
		return false;
	
	/*変換する*/
	for (i=nStartPos;i<nSize;i += 4){
		unsigned long u = *((unsigned long*)(&pChar[i]));
		if		(0x00000000 <= u && u <= 0x0000FFFF){	/*サロゲートペア無し*/
			((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)(u & 0xFFFF);
			nUnicodePos ++;
		}
		else if (0x00010000 <= u && u <= 0x000FFFFF){	/*サロゲートペアあり*/
			u -= 0x10000;	
			((wchar_t*)m_pOutputText)[nUnicodePos + 0] = (wchar_t)(0xD800 | ((u >> 10) & 0x3FF));
			((wchar_t*)m_pOutputText)[nUnicodePos + 1] = (wchar_t)(0xDC00 | ((u >> 0 ) & 0x3FF));
			nUnicodePos += 2;
		}
	}
	
	/*NULL文字を挿入する*/
	((wchar_t*)m_pOutputText)[nUnicodePos] = L'\0';
	return true;
}

/*----------------------------------------------------------------
CTextConverter_UTF32LE::ToMulti
UTF16に変換する処理
----------------------------------------------------------------*/
bool CTextConverter_UTF32LE::ToMulti(const wchar_t *pChar,size_t nSize){
	/*宣言*/
	size_t i;
	size_t nAllocSize = 0;
	size_t nUnicodePos = 0;
	
	/*ステップ１：サイズを求める*/
	if (m_bIncludeBOM)nAllocSize = 4;
	for (i=0;i<nSize;i++){
		nAllocSize += 4;
		if (IsSurrogateLead(pChar[i]))i++;
	}

	/*ステップ２：メモリを確保*/
	if (!Alloc(nAllocSize + sizeof(char)))
		return false;

	/*ステップ３：変換処理*/
	if (m_bIncludeBOM){
		memcpy(m_pOutputText,"\xFF\xFE\x00\x00",4);
		nUnicodePos = 4;
	}
	for (i=0;i<nSize;i++){
		wchar_t w = pChar[i];
		if (IsSurrogateLead(w)){
			unsigned long ucs4 = (
				(((unsigned long)(pChar[i + 0] & 0x3FF)) << 10) |
				(((unsigned long)(pChar[i + 1] & 0x3FF)) <<  0)
			);
			ucs4 += 0x10000;
			*(unsigned long*)(&m_pOutputText[nUnicodePos]) = ucs4;
			i++;
		}
		else
			*(unsigned long*)(&m_pOutputText[nUnicodePos]) = (unsigned long)w;
		nUnicodePos += 4;
	}
	
	/*ステップ４：NULL文字を書き込む*/
	m_pOutputText[nUnicodePos] = '\0';
	return true;
}

/*----------------------------------------------------------------
CTextConverter_UTF32BE::ToUnicode
UTF16からUnicodeに変換する
----------------------------------------------------------------*/
bool CTextConverter_UTF32BE::ToUnicode(const char* pChar,size_t nSize){
	/*宣言*/
	size_t i,nStartPos;
	size_t nAllocSize = 0;
	size_t nUnicodePos = 0;
	
	/*BOMがあったら無視する*/
	if (memcmp(pChar,"\x00\x00\xFE\xFF",4) == 0){
		m_bIncludeBOM = true;
		nStartPos=4;
	}
	else{
		m_bIncludeBOM = false;
		nStartPos=0;
	}

	/*必要なメモリ領域を計算*/
	for (i=nStartPos;i<nSize;i += 4){
		unsigned long u = *((unsigned long*)(&pChar[i]));
		u = ChangeByteOrder(u);
		if		(0x00000000 <= u && u <= 0x0000FFFF){	/*サロゲートペア無し*/
			nAllocSize += sizeof(wchar_t);
		}
		else if (0x00010000 <= u && u <= 0x000FFFFF){	/*サロゲートペアあり*/
			nAllocSize += sizeof(wchar_t) * 2;
		}
	}
	if (!Alloc(nAllocSize + sizeof(wchar_t)))
		return false;
	
	/*変換する*/
	for (i=nStartPos;i<nSize;i += 4){
		unsigned long u = *((unsigned long*)(&pChar[i]));
		u = ChangeByteOrder(u);
		if		(0x00000000 <= u && u <= 0x0000FFFF){	/*サロゲートペア無し*/
			((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)(u & 0xFFFF);
			nUnicodePos ++;
		}
		else if (0x00010000 <= u && u <= 0x000FFFFF){	/*サロゲートペアあり*/
			u -= 0x10000;	
			((wchar_t*)m_pOutputText)[nUnicodePos + 0] = (wchar_t)(0xD800 | ((u >> 10) & 0x3FF));
			((wchar_t*)m_pOutputText)[nUnicodePos + 1] = (wchar_t)(0xDC00 | ((u >> 0 ) & 0x3FF));
			nUnicodePos += 2;
		}
	}
	
	/*NULL文字を挿入する*/
	((wchar_t*)m_pOutputText)[nUnicodePos] = L'\0';
	return true;
}

/*----------------------------------------------------------------
CTextConverter_UTF32BE::ToMulti
UTF16に変換する処理
----------------------------------------------------------------*/
bool CTextConverter_UTF32BE::ToMulti(const wchar_t *pChar,size_t nSize){
	/*宣言*/
	size_t i;
	size_t nAllocSize = 0;
	size_t nUnicodePos = 0;
	
	/*ステップ１：サイズを求める*/
	if (m_bIncludeBOM)nAllocSize = 4;
	for (i=0;i<nSize;i++){
		nAllocSize += 4;
		if (IsSurrogateLead(pChar[i]))i++;
	}

	/*ステップ２：メモリを確保*/
	if (!Alloc(nAllocSize + sizeof(char)))
		return false;

	/*ステップ３：変換処理*/
	if (m_bIncludeBOM){
		memcpy(m_pOutputText,"\x00\x00\xFE\xFF",4);
		nUnicodePos = 4;
	}
	for (i=0;i<nSize;i++){
		wchar_t w = pChar[i];
		if (IsSurrogateLead(w)){
			unsigned long ucs4 = (
				(((unsigned long)(pChar[i + 0] & 0x3FF)) << 10) |
				(((unsigned long)(pChar[i + 1] & 0x3FF)) <<  0)
			);
			ucs4 += 0x10000;
			*(unsigned long*)(&m_pOutputText[nUnicodePos]) = ChangeByteOrder(ucs4);
			i++;
		}
		else
			*(unsigned long*)(&m_pOutputText[nUnicodePos]) = ChangeByteOrder((unsigned long)w);
		nUnicodePos += 4;
	}
	
	/*ステップ４：NULL文字を書き込む*/
	m_pOutputText[nUnicodePos] = '\0';
	return true;
}

/*[EOF]*/
