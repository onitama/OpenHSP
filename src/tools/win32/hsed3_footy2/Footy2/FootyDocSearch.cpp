/**
 * @file FootyDocSearch.cpp
 * @brief ドキュメント管理クラス。このファイルで検索系の実装をします。
 * @author Shinji Watanabe
 * @version 1.0
 * @date Jan.22.2008
 */

#include "FootyDoc.h"
#include "RegExpLetters.h"

#define FLAG_ON(x)		((nFlags & x) != 0)


/**
 * CFootyDoc::Search
 * @brief 検索処理を実行する処理
 * @param szText 検索する語句
 * @param nFlags 検索するためのフラグ(Footy2.hに定義)
 */
bool CFootyDoc::Search(const wchar_t *szText, int nFlags)
{
	CRegExpContainer cSearchText;
	
	// 正規表現にする
	if (FLAG_ON(SEARCH_REGEXP))
	{
	}
	else
	{
		cSearchText.AddRegExp(new CRegExpLetters(szText, FLAG_ON(SEARCH_IGNORECASE)));
	}
	
	// 検索処理の実行
	CEditPosition cStartPos;
	if (FLAG_ON(SEARCH_FROMCURSOR))
	{
		if ( IsSelecting() )
		{
			cStartPos = FLAG_ON(SEARCH_BACK) ? *GetSelStart() : *GetSelEnd();
		}
		else
		{
			cStartPos = m_cCaretPos;
		}
	}
	else
	{
		cStartPos.SetPosition(m_lsLines.begin(), 0);
	}
	return SearchText(&cStartPos, &cSearchText, FLAG_ON(SEARCH_BACK));
}

/**
 * CFootyDoc::SearchText
 * @brief 検索処理を実行する処理
 * @param pStart 検索開始位置
 * @param pRegElement 正規表現要素
 * @param bBackward 後方検索
 * @return 見つかったときにtrueが返る
 */
bool CFootyDoc::SearchText(CEditPosition *pStart, CRegExpElement *pRegElement, bool bBackward)
{
	// 宣言
	CEditPosition cEndPos;							//!< 選択終了位置
	LinePt pNowLine = pStart->GetLinePointer();		//!< 検索中の行位置
	size_t nNowPosition = pStart->GetPosition();	//!< 検索中の桁
	
	// 正方向へ検索する処理の場合
	if (!bBackward)
	{
		// 行ごとにループする
		for (;pNowLine != m_lsLines.end();)
		{
			// 桁ごとにループする
			for (;nNowPosition != pNowLine->GetLineLength();nNowPosition++)
			{
				if (pRegElement->Search(pNowLine,nNowPosition,&cEndPos,m_lsLines.end()))
				{
					SelectSearchedWord(pNowLine,nNowPosition,&cEndPos);
					return true;
				}
			}

			// 次の行へ移動する
			pNowLine++;
			nNowPosition = 0;
		}
	}
	else
	{
		// 行ごとにループする
		for (;;)
		{
			// 桁ごとにループする
			if ( nNowPosition != 0 )
			{
				for (nNowPosition-- ; ; nNowPosition--)
				{
					if (pRegElement->Search(pNowLine,nNowPosition,&cEndPos,m_lsLines.end()))
					{
						SelectSearchedWord(pNowLine,nNowPosition,&cEndPos);
						return true;
					}
					if (nNowPosition == 0)break;
				}
			}

			// 次の行へ移動する
			if (pNowLine == m_lsLines.begin())break;
			pNowLine--;
			nNowPosition = pNowLine->GetLineLength();
		}
	}
	return false;
}


/**
 * CFootyDoc::SelectSearchedWord
 * @brief 検索の結果見つかった言葉を選択する
 */
void CFootyDoc::SelectSearchedWord(LinePt pNowLine,size_t nPosition,CEditPosition *pEndPos)
{
	CEditPosition cStartSelect;			//!< 見つかったとき、選択開始位置とする位置

	cStartSelect.SetPosition(pNowLine,nPosition);
	*GetCaretPosition() = *pEndPos;
	SetSelectStart(&cStartSelect);
	SetSelectEndNormal();
}

/**
 * CFootyDoc::ParseRegulaerExpression
 * @brief 正規表現として文字列をパースする処理
 */
void CFootyDoc::ParseRegulaerExpression(const wchar_t *szString,CRegExpContainer *pRegExp)
{
}


/*[EOF]*/
