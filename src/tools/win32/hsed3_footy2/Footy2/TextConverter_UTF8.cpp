/**
 * @file TextConverter_UTF8.cpp
 * @brief テキストの変換処理が可能なクラスです。
 * @version 1.0
 * @author Shinji Watanabe
 */

#include "TextConverter_Unicode.h"

/**
 * CTextConverter_UTF8::ToUnicode
 * @brief UTF8からUnicodeに変換する
 * @param pChar 元のUTF-8文字列
 * @param nSize 元のUTF-8文字列の長さ(バイト)
 */
bool CTextConverter_UTF8::ToUnicode(const char* pChar,size_t nSize)
{
	size_t i,nStartPos;
	unsigned char c;
	int nAllocSize = 0;
	int nUnicodePos = 0;

	// BOMがあったら無視する
	if (strncmp(pChar,"\xEF\xBB\xBF",3) == 0)
	{
		m_bIncludeBOM = true;
		nStartPos=3;
	}
	else
	{
		m_bIncludeBOM = false;
		nStartPos=0;
	}
	
	// ステップ１：サイズを求める
	for (i=nStartPos;i<nSize;i++)
	{
		c = (unsigned char)pChar[i];
		if		((c & 0x80) == 0)		// この文字は1バイト
		{
			nAllocSize += sizeof(wchar_t);
		}
		else if (i + 1 < nSize &&
				 ((c & 0xDF) == c) &&
				 ((c & 0xC0) == 0xC0))	// この文字は2バイト
		{
			nAllocSize += sizeof(wchar_t);
			i++;
		}
		else if (i + 2 < nSize &&
				 ((c & 0xEF) == c) &&
				 ((c & 0xE0) == 0xE0))	// この文字は3バイト
		{
			nAllocSize += sizeof(wchar_t);
			i+=2;
		}
		else if (i + 3 < nSize && 
				 ((c & 0xF7) == c) &&
				 ((c & 0xF0) == 0xF0))	// この文字は4バイト
		{
			nAllocSize += sizeof(wchar_t) * 2;
			i+=3;
		}
		else if (i + 4 < nSize && 
				 ((c & 0xFB) == c) &&
				 ((c & 0xF8) == 0xF8))	// この文字は5バイト(Footy非対応)
		{
			i+=4;
		}
		else if (i + 5 < nSize && 
				 ((c & 0xFD) == c) &&
				 ((c & 0xFC) == 0xFC))	// この文字は5バイト(Footy非対応)
		{
			i+=5;
		}
	}

	// ステップ２：メモリを確保
	if (!Alloc(nAllocSize + sizeof(wchar_t)))
		return false;

	// ステップ３：変換
	for (i=nStartPos;i<nSize;i++){
		c = (unsigned char)pChar[i];
		if		((c & 0x80) == 0)		// この文字は1バイト
		{
			((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)c;
			nUnicodePos++;
		}
		else if (i + 1 < nSize &&
				 ((c & 0xDF) == c) &&
				 ((c & 0xC0) == 0xC0))	// この文字は2バイト
		{
			((wchar_t*)m_pOutputText)[nUnicodePos] = 
				(wchar_t)((c & 0x1F) << 6) | ((unsigned char)pChar[i+1] & 0x3F);
			nUnicodePos++;
			i++;
		}
		else if (i + 2 < nSize &&
				 ((c & 0xEF) == c) &&
				 ((c & 0xE0) == 0xE0))	// この文字は3バイト
		{
			((wchar_t*)m_pOutputText)[nUnicodePos] = 
				(wchar_t)(
					((c & 0x0F) << 12) | 
					(((unsigned char)pChar[i+1] & 0x3F) << 6) | 
					((unsigned char)pChar[i+2] & 0x3F)
				);
			nUnicodePos++;
			i+=2;
		}
		else if (i + 3 < nSize && 
				 ((c & 0xF7) == c) &&
				 ((c & 0xF0) == 0xF0))	// この文字は4バイト(サロゲートペアを利用する)
		{
			unsigned long ucs4 = 
				(unsigned long)(
					((c & 7) << 16) |
					((((unsigned char)pChar[i+1]) & 0x3F) << 12 )| 
					((((unsigned char)pChar[i+2]) & 0x3F) << 6) | 
					((((unsigned char)pChar[i+3]) & 0x3F))
				);
			ucs4 -= 0x10000;	
			((wchar_t*)m_pOutputText)[nUnicodePos + 0] = (wchar_t)(0xD800 | ((ucs4 >> 10) & 0x3FF));
			((wchar_t*)m_pOutputText)[nUnicodePos + 1] = (wchar_t)(0xDC00 | ((ucs4 >> 0 ) & 0x3FF));
			nUnicodePos += 2;
			i+=3;
		}
		else if (i + 4 < nSize && 
				 ((c & 0xFB) == c) &&
				 ((c & 0xF8) == 0xF8))	// この文字は5バイト(Footy非対応)
		{
			i+=4;
		}
		else if (i + 5 < nSize && 
				 ((c & 0xFD) == c) &&
				 ((c & 0xFC) == 0xFC))	// この文字は5バイト(Footy非対応)
		{
			i+=5;
		}
	}

	// ステップ４：NULL文字を書き込む
	((wchar_t*)m_pOutputText)[nUnicodePos] = L'\0';
	return true;
}

/**
 * CTextConverter_UTF8::ToMulti
 * @brief UTF8に変換する処理
 * @param pChar 変換文字列
 * @param nSize 変換する文字数(wchar_tの数)
 */
bool CTextConverter_UTF8::ToMulti(const wchar_t *pChar,size_t nSize)
{
	size_t i;
	wchar_t w;
	int nAllocSize = 0;
	int nUtf8Pos = 0;
	
	// ステップ１：サイズを求める
	if (m_bIncludeBOM)nAllocSize += sizeof(char)*3;
	for (i=0;i<nSize;i++)
	{
		w = pChar[i];
		if (0x0000 <= w && w <= 0x007F)
			nAllocSize += sizeof(char)*1;
		else if (0x0080 <= w && w <= 0x7FF)
			nAllocSize += sizeof(char)*2;
		else
			nAllocSize += sizeof(char)*3;
	}

	// ステップ２：メモリを確保
	if (!Alloc(nAllocSize + sizeof(char)))
		return false;

	// ステップ３：変換
	if (m_bIncludeBOM)
	{
		memcpy(m_pOutputText, "\xEF\xBB\xBF", 3);
		nUtf8Pos = 3;
	}
	for (i=0;i<nSize;i++)
	{
		w = pChar[i];
		if (0x0000 <= w && w <= 0x007F)
		{
			m_pOutputText[nUtf8Pos] = (char)(w & 0x00FF);
			nUtf8Pos ++;
		}
		else if (0x0080 <= w && w <= 0x7FF)
		{
			m_pOutputText[nUtf8Pos + 0] = (char)(0xC0 | ((w & 0x07C0) >> 6));
			m_pOutputText[nUtf8Pos + 1] = (char)(0x80 | ((w & 0x003F) >> 0));
			nUtf8Pos += 2;
		}
		else
		{
			m_pOutputText[nUtf8Pos + 0] = (char)(0xE0 | ((w & 0xF000) >> 12));
			m_pOutputText[nUtf8Pos + 1] = (char)(0x80 | ((w & 0x0FC0) >> 6));
			m_pOutputText[nUtf8Pos + 2] = (char)(0x80 | ((w & 0x003F) >> 0));
			nUtf8Pos += 3;
		}
	}

	// ステップ４：NULL文字を書き込む
	m_pOutputText[nUtf8Pos] = '\0';
	return true;
}

/*[EOF]*/
