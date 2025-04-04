/**
 * @file FootyDocIndent.cpp
 * @brief インデント関連の処理を行うファイル
 * @version 1.0
 * @author Shinji Watanabe
 * @date Jan.09.2008
 */

#include "FootyDoc.h"

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::Indent
 * @brief インデント処理を行う
 */
CFootyDoc::RedrawType CFootyDoc::Indent()
{
	if (!IsSelecting())return REDRAW_NONE;
	// 宣言
	CEditPosition *pSelStart = GetSelStart();	//!< 選択開始位置
	CEditPosition *pSelEnd = GetSelEnd();		//!< 選択終了位置
	LinePt pStartLine = pSelStart->GetLinePointer();
	LinePt pEndLine = pSelEnd->GetLinePointer();
	if (pSelEnd->GetPosition() == 0)pEndLine--;

	// ループしてインデント後のバッファを作成する
	std::wstring strInsertBuf = L"";
	for (LinePt pNowLine = pStartLine;;pNowLine++)
	{
		strInsertBuf += L"\t";
		strInsertBuf += pNowLine->m_strLineData;
		if (pNowLine->GetRealLineNum() + 1 != GetLineNum())
			strInsertBuf += L"\r\n";
		if (pNowLine == pEndLine)break;
	}

	// 選択する
	m_cSelectBegin.SetPosition(pStartLine,0);
	if (pEndLine->GetRealLineNum() + 1 != GetLineNum())
	{
		pEndLine++;
		m_cSelectEnd.SetPosition(pEndLine,0);
	}
	else
	{
		m_cSelectEnd.SetPosition(pEndLine,pEndLine->GetLineLength());
	}
	m_nSelectType = SELECT_NORMAL;

	// いったん情報をバックアップ
	size_t nStartLine = pStartLine->GetRealLineNum();
	size_t nEndLine = pEndLine->GetRealLineNum();
	bool bIsLastPos = m_cSelectEnd.GetPosition() != 0;

	// バッファ情報を選択したところと置き換える(アンドゥ情報を含む)
	InsertString(strInsertBuf.c_str());

	// 置き換えた部分を再選択する
	m_cSelectBegin.SetPosition( GetLine( nStartLine ), 0 );
	m_cSelectEnd.SetPosition( GetLine( nEndLine ), bIsLastPos ? GetLine( nEndLine )->GetLineLength() : 0 );
	m_nSelectType = SELECT_NORMAL;

	// 編集通知
	SendTextModified(MODIFIED_CAUSE_INDENT);
	return REDRAW_ALL;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::Unindent
 * @brief 逆インデント処理を行う
 */
CFootyDoc::RedrawType CFootyDoc::Unindent()
{
	if (!IsSelecting())return REDRAW_NONE;

	// 宣言
	size_t i,nPos;
	CEditPosition *pSelStart = GetSelStart();	//!< 選択開始位置
	CEditPosition *pSelEnd = GetSelEnd();		//!< 選択終了位置
	LinePt pStartLine = pSelStart->GetLinePointer();
	LinePt pEndLine = pSelEnd->GetLinePointer();
	if (pSelEnd->GetPosition() == 0)pEndLine--;

	// ループしてインデント後のバッファを作成する
	std::wstring strInsertBuf = L"";
	for (LinePt pNowLine = pStartLine;;pNowLine++)
	{
		if (pNowLine->m_strLineData[0] == L'\t')
		{
			i = 1;
		}
		else
		{
			nPos = 0;
			for (i=0;i<pNowLine->GetLineLength();i++)
			{
				if (nPos >= m_nTabLen)break;
				if (pNowLine->m_strLineData[i] == L' ')
					nPos ++;
				else if (pNowLine->m_strLineData[i] == L'　')
					nPos += 2;
				else break;
			}
		}
		strInsertBuf += &pNowLine->m_strLineData[i];
		if (pNowLine->GetRealLineNum() + 1 != GetLineNum())
			strInsertBuf += L"\r\n";
		if (pNowLine == pEndLine)break;
	}

	// 選択する
	m_cSelectBegin.SetPosition(pStartLine,0);
	if (pEndLine->GetRealLineNum() + 1 != GetLineNum())
	{
		pEndLine++;
		m_cSelectEnd.SetPosition(pEndLine,0);
	}
	else
	{
		m_cSelectEnd.SetPosition(pEndLine,pEndLine->GetLineLength());
	}
	m_nSelectType = SELECT_NORMAL;

	// いったん情報をバックアップ
	size_t nStartLine = pStartLine->GetRealLineNum();
	size_t nEndLine = pEndLine->GetRealLineNum();
	bool bIsLastPos = m_cSelectEnd.GetPosition() != 0;

	// バッファ情報を選択したところと置き換える(アンドゥ情報を含む)
	InsertString(strInsertBuf.c_str());

	// 置き換えた部分を再選択する
	m_cSelectBegin.SetPosition( GetLine( nStartLine ), 0 );
	m_cSelectEnd.SetPosition( GetLine( nEndLine ), bIsLastPos ? GetLine( nEndLine )->GetLineLength() : 0 );
	m_nSelectType = SELECT_NORMAL;

	// 編集通知
	SendTextModified(MODIFIED_CAUSE_UNINDENT);
	return REDRAW_ALL;
}

/*[EOF]*/
