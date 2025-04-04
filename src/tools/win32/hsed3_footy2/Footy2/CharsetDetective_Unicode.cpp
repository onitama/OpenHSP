/*===================================================================
CCharsetDetective_Unicodeクラス
文字コードの判別処理を行うクラス
===================================================================*/

#include "CharsetDetective_MultiBytes.h"

/*----------------------------------------------------------------
CCharsetDetective_Unicode::CCharsetDetective_Unicode
コンストラクタ
----------------------------------------------------------------*/
CCharsetDetective_Unicode::CCharsetDetective_Unicode(){
	m_nBomStatus = BOMSTATUS_DETECTING;
	m_nCharNum = 0;
}

/*----------------------------------------------------------------
BOMの宣言
----------------------------------------------------------------*/
const unsigned char *CCharsetDetective_UTF8::GetBOM(){return (const unsigned char*)"\xEF\xBB\xBF$";}
const unsigned char *CCharsetDetective_UTF16_LE::GetBOM(){return (const unsigned char*)"\xFF\xFE$";}
const unsigned char *CCharsetDetective_UTF16_BE::GetBOM(){return (const unsigned char*)"\xFE\xFF$";}
const unsigned char *CCharsetDetective_UTF32_LE::GetBOM(){return (const unsigned char*)"\xFF\xFE\x00\x00$";}
const unsigned char *CCharsetDetective_UTF32_BE::GetBOM(){return (const unsigned char*)"\x00\x00\xFE\xFF$";}

/*----------------------------------------------------------------
CCharsetDetective_Unicode::NewCharInner
UNICODEのBOM判定ルーチン
----------------------------------------------------------------*/
void CCharsetDetective_Unicode::NewCharInner(unsigned char cNewChar){
	if (m_nBomStatus == BOMSTATUS_DETECTING){
		unsigned char cBomChar;
		cBomChar = GetBOM()[m_nCharNum];
		if (cBomChar == '$'){		/*BOMの判定完了*/
			m_nBomStatus = BOMSTATUS_HAS_BOM;
			m_nStatus = 0;
		}
		else{
			if (cBomChar != cNewChar)
				m_nBomStatus = BOMSTATUS_NO_BOM;
		}
		m_nCharNum++;
	}
	
	NewCharUnicode(cNewChar);
}

/**
 * CCharsetDetective_UTF8::NewCharUnicode
 * @brief UTF8内部用判定ルーチン
 * @param cNewChar 新しい文字
 */
void CCharsetDetective_UTF8::NewCharUnicode(unsigned char cNewChar){
	if (m_nStatus == 0){			/*1バイト目のとき*/
		if		((cNewChar & 0x80) == 0){		/*この文字は1バイト*/
			ResetStores();
			IncLetter();
			m_nStatus = 0;
		}
		else if (((cNewChar & 0xDF) == cNewChar) &&
				 ((cNewChar & 0xC0) == 0xC0)){	/*この文字は2バイト*/
			StoreChar(cNewChar);
			m_nStatus = 1;
		}
		else if (((cNewChar & 0xEF) == cNewChar) &&
				 ((cNewChar & 0xE0) == 0xE0)){	/*この文字は3バイト*/
			StoreChar(cNewChar);
			m_nStatus = 2;
		}
		else if (((cNewChar & 0xF7) == cNewChar) &&
				 ((cNewChar & 0xF0) == 0xF0)){	/*この文字は4バイト*/
			StoreChar(cNewChar);
			m_nStatus = 3;
		}
		else if (((cNewChar & 0xFB) == cNewChar) &&
				 ((cNewChar & 0xF8) == 0xF8)){	/*この文字は5バイト*/
			StoreChar(cNewChar);
			m_nStatus = 4;
		}
		else if (((cNewChar & 0xFD) == cNewChar) &&
				 ((cNewChar & 0xFC) == 0xFC)){	/*この文字は5バイト*/
			StoreChar(cNewChar);
			m_nStatus = 5;
		}
		else SetImpossible();
	}
	else{							/*2バイト目以降*/
		if	(((cNewChar & 0xBF) == cNewChar) &&
			 ((cNewChar & 0x80) == 0x80)){
			m_nStatus--;
			if (m_nStatus == 0){
				ResetStores();
				IncLetter();
			}
		}
		else SetImpossible();
	}
}
/*----------------------------------------------------------------
CCharsetDetective_UTF16_LE::NewCharUnicode
UTF16LE内部用判定ルーチン
----------------------------------------------------------------*/
void CCharsetDetective_UTF16_LE::NewCharUnicode(unsigned char cNewChar){
	if (m_nStatus == 0){	/*1バイト目*/
		StoreChar(cNewChar);
		m_nStatus = 1;
	}
	else{					/*2バイト目*/
		/*入力された文字について検討する*/
		wchar_t w = (wchar_t)((cNewChar << 8) | m_cBeforeChars[0]);
		if (IsNotUnicode((unsigned long)w))SetImpossible();		/*Unicodeとして不適切な文字か*/
		else if (0x0000 == w)SetImpossible();					/*NULLはUTF32にもよく出る*/
		else if (0x0000 < w && w < 0x0080)IncCommon();			/*ASCII文字なら良く出るはずである*/
		else if (0x20 <= cNewChar && cNewChar <= 0x7E &&		/*両方ASCII文字なら他の方がよく出るはず*/
				 0x20 <= m_cBeforeChars[0] && m_cBeforeChars[0] <= 0x7E)IncRare();
		else if (IsSurrogateLead(w))IncRare();					/*サロゲートペア*/
		else IncLetter();										/*それ以外は出る可能性があるものとする*/
		/*1バイト目に戻る*/
		ResetStores();
		m_nStatus = 0;
	}
}

/*----------------------------------------------------------------
CCharsetDetective_UTF16_BE::NewCharUnicode
UTF16LE内部用判定ルーチン
----------------------------------------------------------------*/
void CCharsetDetective_UTF16_BE::NewCharUnicode(unsigned char cNewChar){
	if (m_nStatus == 0){	/*1バイト目*/
		StoreChar(cNewChar);
		m_nStatus = 1;
	}
	else{					/*2バイト目*/
		/*入力された文字について検討する*/
		wchar_t w = (wchar_t)((m_cBeforeChars[0] << 8) | cNewChar);
		if (IsNotUnicode((unsigned long)w))SetImpossible();		/*Unicodeとして不適切な文字か*/
		else if (0x0000 == w)SetImpossible();					/*NULLはUTF32にもよく出る*/
		else if (0x0000 < w && w < 0x0080)IncCommon();			/*ASCII文字なら良く出るはずである*/
		else if (0x20 <= cNewChar && cNewChar <= 0x7E &&		/*両方ASCII文字なら他の方がよく出るはず*/
				 0x20 <= m_cBeforeChars[0] && m_cBeforeChars[0] <= 0x7E)IncRare();
		else if (IsSurrogateLead(w))IncRare();					/*サロゲートペア*/
		else IncLetter();										/*それ以外は出る可能性があるものとする*/
		/*1バイト目に戻る*/
		ResetStores();
		m_nStatus = 0;
	}
}

/*----------------------------------------------------------------
CCharsetDetective_UTF32_LE::NewCharUnicode
UTF16LE内部用判定ルーチン
----------------------------------------------------------------*/
void CCharsetDetective_UTF32_LE::NewCharUnicode(unsigned char cNewChar){
	if (m_nStatus != 3){	/*1～3バイト目*/
		StoreChar(cNewChar);
		m_nStatus++;
	}
	else{					/*4バイト目*/
		/*入力された文字について検討する*/
		unsigned long u = (unsigned long)((cNewChar << 24) | (m_cBeforeChars[2] << 16) | ( (m_cBeforeChars[1] << 8) | m_cBeforeChars[0]));
		if (IsNotUnicode(u))SetImpossible();					/*Unicodeとして不適切な文字か*/
		else if (0x0000 <= u && u < 0x0080)IncCommon();			/*ASCII文字ならよく出るはず*/
		else IncLetter();										/*それ以外は出る可能性があるものとする*/
		/*1バイト目に戻る*/
		ResetStores();
		m_nStatus = 0;
	}
}

/*----------------------------------------------------------------
CCharsetDetective_UTF32_BE::NewCharUnicode
UTF16LE内部用判定ルーチン
----------------------------------------------------------------*/
void CCharsetDetective_UTF32_BE::NewCharUnicode(unsigned char cNewChar){
	if (m_nStatus != 3){	/*1～3バイト目*/
		StoreChar(cNewChar);
		m_nStatus++;
	}
	else{					/*4バイト目*/
		/*入力された文字について検討する*/
		unsigned long u = (unsigned long)(cNewChar | (m_cBeforeChars[2] << 8) | ( (m_cBeforeChars[1] << 16) | (m_cBeforeChars[0] << 24)));
		if (IsNotUnicode(u))SetImpossible();					/*Unicodeとして不適切な文字か*/
		else if (0x0000 <= u && u < 0x0080)IncCommon();			/*ASCII文字ならよく出るはず*/
		else IncLetter();										/*それ以外は出る可能性があるものとする*/
		/*1バイト目に戻る*/
		ResetStores();
		m_nStatus = 0;
	}
}

/*[EOF]*/
