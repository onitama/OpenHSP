/**
 * @file RegExpElement.cpp
 * @brief 正規表現の要素クラス
 * @author Shinji Watanabe
 * @date Jan.22.2008
 */

#include "RegExpElement.h"

/**
 * CRegExpElement::CRegExpElement
 * @brief コンストラクタ
 */
CRegExpElement::CRegExpElement()
{
	m_nLoopType = LOOPTYPE_NONE;
}

/**
 * CRegExpElement::~CRegExpElement
 * @brief デストラクタ
 */
CRegExpElement::~CRegExpElement()
{
}


/**
 * CRegExpElement::Search
 * @brief この中を検索する処理
 */
bool CRegExpElement::Search(LinePt pStartLine,size_t nStartPos,CEditPosition *pEnd,LinePt pEndLine)
{
	// ループ設定情報に応じてループをする
	switch (m_nLoopType)
	{
	case LOOPTYPE_NONE:
		return SearchInner(pStartLine,nStartPos,pEnd,pEndLine);
	default:
		return false;
	}
}


/*[EOF]*/
