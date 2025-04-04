/**
 * @file RegExpContainer.cpp
 * @brief 正規表現のクラス。子供の要素を複数持つことが可能。
 * @author Shinji Watanabe
 * @date Jan.22.2008
 */

#include "RegExpContainer.h"

/**
 * CRegExpContainer::CRegExpContainer
 * @brief コンストラクタ
 */
CRegExpContainer::CRegExpContainer()
{
}

/**
 * CRegExpContainer::~CRegExpContainer
 * @brief デストラクタ
 */
CRegExpContainer::~CRegExpContainer()
{
	for (ItElement iterElement = m_vecChildElements.begin();
		 iterElement != m_vecChildElements.end();iterElement ++)
	{
		delete (*iterElement);
		*iterElement = NULL;
	}
}

/**
 * CRegExpContainer::SearchInner
 * @brief 内部用の文字列追加処理
 */
bool CRegExpContainer::SearchInner(LinePt pStartLine,size_t nStartPos,CEditPosition *pEnd,LinePt pEndLine)
{
	// 子供の要素についてループ処理を行う
	for (ItElement iterNowElement = m_vecChildElements.begin();
		 iterNowElement != m_vecChildElements.end();iterNowElement++)
	{
		
		// そうでないときは手前からチェック
		bool bResult = (*iterNowElement)->Search(pStartLine,nStartPos,pEnd,pEndLine);
		
		// 戻り値によって制御する
		if (!bResult)return false;
	}
	return true;
}

/*[EOF]*/
