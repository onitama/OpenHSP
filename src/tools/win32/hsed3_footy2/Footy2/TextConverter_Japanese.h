/*===================================================================
CTextConverter_Japaneseクラス
UNICODEを基点にしたテキストの変換処理が可能なベースクラスです。
これは日本語の基底クラス
===================================================================*/

#ifndef _TEXT_CONVERTER_CLASS_JAPANESE_H_
#define _TEXT_CONVERTER_CLASS_JAPANESE_H_

#include "TextConverter.h"

//! 日本語のコンバーター基底
class  CTextConverter_Japanese : public CTextConverter{
protected:
	inline bool IsAscii(unsigned char c1){return (0x00 <= c1 && c1 <= 0x7F);}
	inline void ToSurrogate(unsigned long ucs4,int &nUnicodePos){
		if (ucs4 >= 0x10000){
			ucs4 -= 0x10000;	
			((wchar_t*)m_pOutputText)[nUnicodePos + 0] = (wchar_t)(0xD800 | ((ucs4 >> 10) & 0x3FF));
			((wchar_t*)m_pOutputText)[nUnicodePos + 1] = (wchar_t)(0xDC00 | ((ucs4 >> 0 ) & 0x3FF));
			nUnicodePos += 2;
		}
		else{
			((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)ucs4;
			nUnicodePos ++;
		}
	}
	inline unsigned long ToUCS4(const wchar_t *pChar){
		return (((pChar[0] & 0x3FF) << 10) | ((pChar[1] & 0x3FF) <<  0)) + 0x10000;
	}
};

//! ShiftJISのJIS X 0213:2004拡張
class  CTextConverter_ShiftJIS2004 : public CTextConverter_Japanese{
public:
	bool ToUnicode(const char*,size_t nSize);
	bool ToMulti(const wchar_t*,size_t nSize);
private:
	inline bool IsPlane1Lead(unsigned char c1){return (0x81 <= c1 && c1 <= 0x9F) || (0xE0 <= c1 && c1 <= 0xEF);}
	inline bool IsPlane2Lead(unsigned char c1){return (0xF0 <= c1 && c1 <= 0xFC);}
	inline bool IsTail(unsigned char c2){return (0x40 <= c2 && c2 <= 0x7E) || (0x80 <= c2 && c2 <= 0xFC);}
	inline bool IsKana(unsigned char c1){return (0xA1 <= c1 && c1 <= 0xDF);}
	unsigned short ToJisCode(unsigned char c1,unsigned char c2);
	unsigned short ToSjis(unsigned short jis);
};

//! JIS X 0213:2004のEUC-JP実装
class CTextConverter_EucJis2004 : public CTextConverter_Japanese{
public:
	bool ToUnicode(const char*,size_t nSize);
	bool ToMulti(const wchar_t*,size_t nSize);
private:
	//inline bool IsJISX0208_Lead(unsigned char c1){return (0xA1 <= c1 && c1 <= 0xA8) || (0xB0 <= c1 && c1 <= 0xF4);}
	//inline bool IsNEC_Lead(unsigned char c1){return (c1 == 0xAD);}
	//inline bool IsJISX0212_Second(unsigned char c2){return (0xA1 <= c2 && c2 <= 0xAB) || (0xB0 <= c2 && c2 <= 0xED);}
	//inline bool IsIBM_Second(unsigned char c2){return (0xF3 <= c2 && c2 <= 0xF4);}
	inline bool IsThreeBytes_Lead(unsigned char c1){return c1 == 0x8F;}
	inline bool IsKana_Lead(unsigned char c1){return c1 == 0x8E;}
	inline bool IsKana_Tail(unsigned char c2){return (0xA1 <= c2 && c2 <= 0xDF);}
	inline bool IsTwoBytes(unsigned char c){return (0xA1 <= c && c <= 0xFE);}

	//unsigned short ToSjis(unsigned char c1,unsigned char c2);
	//unsigned short ToEUC(unsigned char c1,unsigned char c2);
};

#endif	/*not defined _TEXT_CONVERTER_CLASS_JAPANESE_H_*/

/*[EOF]*/
