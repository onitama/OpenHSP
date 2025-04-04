/*===================================================================
FootyStatusクラス
CFootyViewクラスの見え方を管理するクラスです。
===================================================================*/

#include "FootyStatus.h"

/*-------------------------------------------------------------------
CFootyStatus::CFootyStatus
コンストラクタです。デフォルト情報を代入します。
-------------------------------------------------------------------*/
CFootyStatus::CFootyStatus(){
	SetDefaultColor();
}

/*-------------------------------------------------------------------
CFootyStatus::SetDefaultColor
デフォルトの色設定を代入します。
-------------------------------------------------------------------*/
void CFootyStatus::SetDefaultColor(){
	m_clDefaultLetter = RGB(0,0,0);
	m_clBackGround = RGB(255,255,255);
	m_clCrlf = RGB(136,82,203);
	m_clEOF = RGB(78,207,191);
	m_clTab = RGB(200,181,43);
	m_clUnderLine = RGB(192,48,95);
	m_clLineNumLine = RGB(41,177,133);
	m_clLineNum = RGB(48,116,184);
	m_clCaretLine = RGB(200,250,200);
	m_clRulerBk = RGB(236,233,216);
	m_clRulerText = RGB(0,0,0);
	m_clRulerLine = RGB(0,0,0);
	m_clCaretPos = RGB(0,200,200);
	m_clUrl = RGB(0,0,255);
	m_clUrlUnder = RGB(0,0,255);
	m_clMail = RGB(170,43,94);
	m_clMailUnder = RGB(170,43,94);
	m_clLabel = RGB(0,0,255);// by Tetr@pod
	m_clLabelUnder = RGB(0,0,255);// by Tetr@pod

	m_clFunc1 = RGB(0,0,255);
	m_clFunc2 = RGB(0,0,255);
	m_clFunc3 = RGB(0,0,255);
	m_clFunc4 = RGB(0,0,255);
	m_clFunc5 = RGB(0,0,255);
	m_clFunc6 = RGB(0,0,255);
	m_clFunc7 = RGB(0,0,255);
	m_clFunc8 = RGB(0,0,255);


	m_clNormalSpace = RGB(255,150,150);
	m_clHalfSpace = RGB(255,150,150);
	m_clHighlightBk = RGB(255,178,0);
	m_clHighlightText = RGB(0,0,255);
}

/*[EOF]*/
