/*===================================================================
CCharsetDetective_MultiBytesクラス
文字コードの判別処理を行うクラス
===================================================================*/

#ifndef _CARSET_DETECTIVE_MULTIBYTES_CLASS_H_
#define _CARSET_DETECTIVE_MULTIBYTES_CLASS_H_

#include "CharsetDetective.h"

//! マルチバイト文字列の基礎
class CCharsetDetective_MultiBytes : public CCharsetDetective{
public:
	CCharsetDetective_MultiBytes();

protected:
	inline void StoreChar(unsigned char cNew){
		if (m_nStoredChars != MAX_STORE_CHARS){
			m_cBeforeChars[m_nStoredChars] = cNew;
			m_nStoredChars ++;
		}
	}
	inline void ResetStores(){m_nStoredChars = 0;}


protected:
	enum fixedNum{
		MAX_STORE_CHARS = 6,
	};
	unsigned char m_cBeforeChars[MAX_STORE_CHARS];
	int m_nStoredChars;
	int m_nStatus;
};

//! ShiftJIS検出用
class CCharsetDetective_ShiftJIS2004 : public CCharsetDetective_MultiBytes{
public:
	CharSetMode GetCodepage(){return CSM_SHIFT_JIS_2004;}
protected:
	void NewCharInner(unsigned char cNewChar);
};

//! EUC-JP検出用
class CCharsetDetective_EUC_JIS_2004 : public CCharsetDetective_MultiBytes{
public:
	CharSetMode GetCodepage(){return CSM_EUC_JIS_2004;}
protected:
	void NewCharInner(unsigned char cNewChar);
private:
	enum EucJpMsStatus{
		EUC_JP_FIRSTBYTE,			//!< 最初のバイト
		EUC_JP_KATAKANA,			//!< JIS X 0201 半角かな
		EUC_JP_KANJI,				//!< JIS X 0208:1997 常用漢字
		EUC_JP_NEC,					//!< NEC特殊文字
		EUC_JP_USERS_FIRST,			//!< ユーザー定義 前半
		EUC_JP_SECONDBYTE,			//!< ２バイト目を選んでいる
		EUC_JP_EXTENDED_KANJI,		//!< JIS X 0212:1990 拡張漢字
		EUC_JP_IBM_EXTENDED,		//!< IBM拡張漢字 JIS X 0212以外
		EUC_JP_USERS_SECOND,		//!< ユーザー定義 後半
	};
};

//! Unicode用
class CCharsetDetective_Unicode : public CCharsetDetective_MultiBytes{
public:
	CCharsetDetective_Unicode();

protected:
	void NewCharInner(unsigned char cNewChar);
	bool HasByteOrderMark(){return m_nBomStatus == BOMSTATUS_HAS_BOM;}

	virtual const unsigned char *GetBOM() = 0;
	virtual void NewCharUnicode(unsigned char cNewChar) = 0;

	//! その文字がUnicodeかどうか取得する処理
	inline bool IsNotUnicode(unsigned long u){
		return		(0x0007C0 <= u && u < 0x000900) ||
					(0x0018B0 <= u && u < 0x001900) ||
					(0x001A20 <= u && u < 0x001D00) ||
					(0x002C60 <= u && u < 0x002C80) ||
					(0x002DE0 <= u && u < 0x002E00) ||
					(0x002FE0 <= u && u < 0x002FF0) ||
					(0x00A4D0 <= u && u < 0x00A700) ||
					(0x00A720 <= u && u < 0x00A800) ||
					(0x00A830 <= u && u < 0x00AC00) ||
					(0x00D7B0 <= u && u < 0x00D800) ||
					(0x010350 <= u && u < 0x010380) ||
					(0x0103E0 <= u && u < 0x010400) ||
					(0x0104B0 <= u && u < 0x010800) ||
					(0x010840 <= u && u < 0x010A00) ||
					(0x010A60 <= u && u < 0x01D000) ||
					(0x01D250 <= u && u < 0x01D300) ||
					(0x01D800 <= u && u < 0x020000) ||
					(0x02A6E0 <= u && u < 0x02F800) ||
					(0x02FA20 <= u && u < 0x0E0000) ||
					(0x0E0080 <= u && u < 0x0E0100) ||
					(0x0E01F0 <= u && u < 0x0F0000) ||
					(0x100000 <= u);
	}
	
	//! サロゲートペアを利用するか取得する
	bool IsSurrogateLead(wchar_t w){return 0xD800 <= w && w <= 0xDBFF;}

private:
	enum BomStatus{
		BOMSTATUS_DETECTING,		//!< 現在判定中
		BOMSTATUS_HAS_BOM,			//!< BOMを持っている
		BOMSTATUS_NO_BOM,			//!< BOMがない
	};
	
	BomStatus m_nBomStatus;			//!< 現在のBOMステータス
	int m_nCharNum;
};


//! UTF-8検出用
class CCharsetDetective_UTF8 : public CCharsetDetective_Unicode{
public:
	CharSetMode GetCodepage(){
		return HasByteOrderMark() ? CSM_UTF8_BOM : CSM_UTF8;
	}
protected:
	void NewCharUnicode(unsigned char cNewChar);
	const unsigned char *GetBOM();
};

//! UTF-16リトルエンディアン検出用
class CCharsetDetective_UTF16_LE : public CCharsetDetective_Unicode{
public:
	CharSetMode GetCodepage(){
		return HasByteOrderMark() ? CSM_UTF16_LE_BOM : CSM_UTF16_LE;
	}
protected:
	void NewCharUnicode(unsigned char cNewChar);
	const unsigned char *GetBOM();
};

//! UTF-16リトルエンディアン検出用
class CCharsetDetective_UTF16_BE : public CCharsetDetective_Unicode{
public:
	CharSetMode GetCodepage(){
		return HasByteOrderMark() ? CSM_UTF16_BE_BOM : CSM_UTF16_BE;
	}
protected:
	void NewCharUnicode(unsigned char cNewChar);
	const unsigned char *GetBOM();
};

//! UTF-32リトルエンディアン検出用
class CCharsetDetective_UTF32_LE : public CCharsetDetective_Unicode{
public:
	CharSetMode GetCodepage(){
		return HasByteOrderMark() ? CSM_UTF32_LE_BOM : CSM_UTF32_LE;
	}
protected:
	void NewCharUnicode(unsigned char cNewChar);
	const unsigned char *GetBOM();
};

//! UTF-32ビッグエンディアン検出用
class CCharsetDetective_UTF32_BE : public CCharsetDetective_Unicode{
public:
	CharSetMode GetCodepage(){
		return HasByteOrderMark() ? CSM_UTF32_BE_BOM : CSM_UTF32_BE;
	}
protected:
	void NewCharUnicode(unsigned char cNewChar);
	const unsigned char *GetBOM();
};

#endif	/*_CARSET_DETECTIVE_MULTIBYTES_CLASS_H_*/
 
 /*[EOF]*/
 