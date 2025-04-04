/*===================================================================
CCharsetDetective_MultiBytesクラス
文字コードの判別処理を行うクラス
===================================================================*/

#include "CharsetDetective_MultiBytes.h"

/*----------------------------------------------------------------
CCharsetDetective_MultiBytes::CCharsetDetective_MultiBytes
コンストラクタ
----------------------------------------------------------------*/
CCharsetDetective_MultiBytes::CCharsetDetective_MultiBytes(){
	m_nStoredChars = 0;
	m_nStatus = 0;
}

/*----------------------------------------------------------------
CCharsetDetective_ShiftJIS2004::NewCharacterInner
新しいキャラクタ
----------------------------------------------------------------*/
void CCharsetDetective_ShiftJIS2004::NewCharInner(unsigned char cNewChar){
	switch(m_nStoredChars){
	case 0:					/*一番目の文字のとき*/
		/*ShiftJISの開始文字か？*/
		if ((0x81 <= cNewChar && cNewChar <= 0x9F) ||
			(0xE0 <= cNewChar && cNewChar <= 0xFC)){
			StoreChar(cNewChar);
		}
		/*半角かな*/
		else if (0xA1 <= cNewChar && cNewChar <= 0xDF){
			IncRare();
		}
		/*ASCII文字列*/
		else if (IsAsciiChar(cNewChar)){
			IncLetter();
		}
		else SetImpossible();
		break;
	case 1:					/*二番目のバイトのとき*/
		if (0x40 <= cNewChar && cNewChar <= 0xFC && cNewChar != 0x7F){
			unsigned short s = (cNewChar << 8) | m_cBeforeChars[0];
			/*ひらがな*/
			if (0x82A0 <= s && s <= 0x82F1)
				IncCommon();
			/*かたかな*/
			else if (0x8340 <= s && s <= 0x8396)
				IncCommon();
			/*それ以外*/
			else
				IncLetter();
			ResetStores();
		}
		else SetImpossible();
		break;
	}
}

/*[EOF]*/
