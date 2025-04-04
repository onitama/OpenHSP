/*===================================================================
CCharsetDetectiveクラス
文字コードの判別処理を行うクラス
===================================================================*/

#include "CharsetDetective.h"

/*----------------------------------------------------------------
CCharsetDetective::CCharsetDetective
コンストラクタ
----------------------------------------------------------------*/
CCharsetDetective::CCharsetDetective(){
	m_bIsImpossible = false;
	m_nLetterCount = 0;
	m_nCommonLetter = 0;
	m_nRareLetter = 0;
}

/*[EOF]*/
