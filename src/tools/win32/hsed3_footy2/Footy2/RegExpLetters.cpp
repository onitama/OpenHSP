/*===================================================================
CRegExpLettersクラス
正規表現の要素クラス。通常の文字を格納する最も基本的なクラス
===================================================================*/

#include "RegExpLetters.h"


/*----------------------------------------------------------------
CRegExpLetters::CRegExpLetters
コンストラクタ
----------------------------------------------------------------*/
CRegExpLetters::CRegExpLetters(const wchar_t *pString, bool bIgnoreCase){
	m_strLetters = pString;
	m_bIgnoreCase = bIgnoreCase;
}

/*----------------------------------------------------------------
CRegExpLetters::CRegExpLetters
コンストラクタ
----------------------------------------------------------------*/
CRegExpLetters::CRegExpLetters(std::wstring &strLetters, bool bIgnoreCase){
	m_strLetters = strLetters;
	m_bIgnoreCase = bIgnoreCase;
}

/*----------------------------------------------------------------
CRegExpLetters::~CRegExpLetters
デストラクタ
----------------------------------------------------------------*/
CRegExpLetters::~CRegExpLetters(){
}

/*----------------------------------------------------------------
CRegExpLetters::~CRegExpLetters
デストラクタ
----------------------------------------------------------------*/
bool CRegExpLetters::SearchInner(LinePt pLine,size_t nPosition,CEditPosition *pEnd,LinePt pEndLine){
	/*文字数によってチェックする*/
	if (m_strLetters.size() > pLine->GetLineLength() - nPosition)
		return false;

	/*ループ*/
//	for (size_t i=0;
//		 i != m_strLetters.size() && nPosition != pLine->GetLineLength();
//		 nPosition++,i++){
//		 if (pLine->GetLineData()[nPosition] != m_strLetters[i])return false;
//	}

	if( CSTR_EQUAL != CompareStringW(
							LOCALE_USER_DEFAULT, (m_bIgnoreCase ? NORM_IGNORECASE : 0),
							pLine->GetLineData() + nPosition, static_cast<int>(m_strLetters.size()),
							m_strLetters.c_str(), static_cast<int>(m_strLetters.size())) )
	{
		return false;
	}
	nPosition += m_strLetters.size();
	
	/*成功した*/
	pEnd->SetPosition(pLine,nPosition);
	return true;
}



/*[EOF]*/
