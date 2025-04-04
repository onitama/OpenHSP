/*================================================================
ISO8859に関連する文字コード変換クラス
================================================================*/

#pragma once

#include "TextConverter.h"
#include "UnicodeTable.h"

//! 変換テーブルを持つものの変換クラス。Tに変換テーブルクラスを指定
template<unsigned char C,wchar_t W>
class CTextConverter_ISO8859_Slide : public CTextConverter{
public:
	//! Unicodeへ変換する処理
	bool ToUnicode(const char* pRawText,size_t nSize){
		unsigned char c;
		/*メモリを確保する*/
		if (!Alloc((nSize+1)*sizeof(wchar_t)))
			return false;
		/*変換する*/
		size_t i;
		for (i=0;i<nSize;i++){
			c = pRawText[i];
			if (0x00 <= c && c < C)			/*ASCII文字*/
				((wchar_t*)m_pOutputText)[i] = (wchar_t)c;
			else							/*それ以外は変換テーブルに従う*/
				((wchar_t*)m_pOutputText)[i] = (wchar_t)c + W - 0xA0;
		}
		/*NULLの書き込み*/
		((wchar_t*)m_pOutputText)[i] = L'\0';
		return true;
	}
	
	//! バイトコードへ変換する処理
	bool ToMulti(const wchar_t* pMulti,size_t nSize){
		wchar_t w;
		/*メモリを確保する*/
		if (!Alloc((nSize+1)*sizeof(char)))
			return false;
		/*変換する*/
		size_t i;
		for (i=0;i<nSize;i++){
			w = pMulti[i];
			if (0x00 <= w && w < C)			/*ASCII文字*/
				m_pOutputText[i] = (char)(w & 0xFF);
			else							/*それ以外は変換テーブルに従う*/
				m_pOutputText[i] = (char)((w - W + 0xA0) & 0xFF);
		}
		/*NULLの書き込み*/
		m_pOutputText[i] = '\0';
		return true;
	}
};

//! 変換テーブルを持つものの変換クラス。Tに変換テーブルクラスを指定
template<class T>
class CTextConverter_ISO8859_Table : public CTextConverter{
public:
	//! Unicodeへ変換する処理
	bool ToUnicode(const char* pRawText,size_t nSize){
		int nUnicodePos = 0;
		unsigned char c;
		wchar_t w;
		/*メモリを確保する*/
		if (!Alloc((nSize+1)*sizeof(wchar_t)))
			return false;
		/*変換する*/
		for (size_t i=0;i<nSize;i++){
			c = pRawText[i];
			if (0x00 <= c && c <= 0x9F){	/*ASCII文字*/
				((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)c;
				nUnicodePos++;
			}
			else{							/*それ以外は変換テーブルに従う*/
				w = (wchar_t)m_cTable.ToUnicode((unsigned short)c);
				if (w != 0){
					((wchar_t*)m_pOutputText)[nUnicodePos] = w;
					nUnicodePos++;
				}
			}
		}
		/*NULLの書き込み*/
		((wchar_t*)m_pOutputText)[nUnicodePos] = L'\0';
		return true;
	}
	
	//! バイトコードへ変換する処理
	bool ToMulti(const wchar_t* pMulti,size_t nSize){
		int nAnsiPos = 0;
		unsigned char c;
		wchar_t w;
		/*メモリを確保する*/
		if (!Alloc((nSize+1)*sizeof(char)))
			return false;
		/*変換する*/
		for (size_t i=0;i<nSize;i++){
			w = pMulti[i];
			if (0x00 <= w && w <= 0x9F){	/*ASCII文字*/
				m_pOutputText[nAnsiPos] = (char)(w & 0xFF);
				nAnsiPos++;
			}
			else{							/*それ以外は変換テーブルに従う*/
				c = (char)(m_cTable.ToCode((unsigned short)w) & 0xFF);
				if (c != 0){
					m_pOutputText[nAnsiPos] = c;
					nAnsiPos++;
				}
			}
		}
		/*NULLの書き込み*/
		m_pOutputText[nAnsiPos] = '\0';
		return true;
	}
private:
	T m_cTable;
};

/*[EOF]*/
