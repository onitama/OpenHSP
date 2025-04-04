/**
 * @file FootyDocSelect.cpp
 * @brief CFootyDocクラスの中で選択関連の処理を行います。
 * @author Shinji Watanabe
 * @version Jan.22.2008
 */

#include "FootyDoc.h"

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::SetSelectStart
 * @brief 選択開始位置を設定します。
 * @param pStart 設定する位置
 */
bool CFootyDoc::SetSelectStart(CEditPosition *pStart)
{
	if (!pStart)return false;
	m_nSelectType = SELECT_NONE;
	m_cSelectBegin = *pStart;
	return true;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::SetSelectStart
 * @brief 選択開始位置を設定します。
 * @param pStart 設定する位置
 */
bool CFootyDoc::SetSelectStart(CUndoBuffer::CPosInfo *pStart)
{
	if (!pStart)return false;
	m_nSelectType = SELECT_NONE;
	pStart->SetPosition(&m_cSelectBegin,&m_lsLines);
	return true;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::SetSelectEndNormal
 * @brief 選択終了位置を設定する
 * @param 設定する位置
 */
bool CFootyDoc::SetSelectEndNormal(CEditPosition *pEnd){
	if (!pEnd)return false;
	m_cSelectEnd = *pEnd;
	if (m_cSelectEnd == m_cSelectBegin)		// 開始位置と同じ
		m_nSelectType = SELECT_NONE;
	else if (m_cSelectEnd > m_cSelectBegin)	// 通常選択
		m_nSelectType = SELECT_NORMAL;
	else									// 逆位置選択
		m_nSelectType = SELECT_REV;
	return true;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::SetSelectEndNormal
 * @brief 選択終了位置を設定する
 */
bool CFootyDoc::SetSelectEndNormal(CUndoBuffer::CPosInfo *pEnd)
{
	if (!pEnd)return false;
	pEnd->SetPosition(&m_cSelectEnd,&m_lsLines);
	if (m_cSelectEnd == m_cSelectBegin)		// 開始位置と同じ
		m_nSelectType = SELECT_NONE;
	else if (m_cSelectEnd > m_cSelectBegin)	// 通常選択
		m_nSelectType = SELECT_NORMAL;
	else									// 逆位置選択
		m_nSelectType = SELECT_REV;
	return true;
}

/*-------------------------------------------------------------------
CFootyDoc::GetSelStart
選択開始位置を取得するルーチン
-------------------------------------------------------------------*/
CEditPosition *CFootyDoc::GetSelStart(){
	if (m_nSelectType == SELECT_REV ||
		m_nSelectType == SELECT_BOX_REV)
		return &m_cSelectEnd;
	else
		return &m_cSelectBegin;
}

/*-------------------------------------------------------------------
CFootyDoc::GetSelEnd
選択終了位置を取得するルーチン
-------------------------------------------------------------------*/
CEditPosition *CFootyDoc::GetSelEnd(){
	if (m_nSelectType == SELECT_REV ||
		m_nSelectType == SELECT_BOX_REV)
		return &m_cSelectBegin;
	else
		return &m_cSelectEnd;
}

/*-------------------------------------------------------------------
CFootyDoc::SelectAll
全て選択する
-------------------------------------------------------------------*/
void CFootyDoc::SelectAll(){
	m_nSelectType = SELECT_NORMAL;
	m_cSelectBegin.SetPosition(GetTopLine(),0);
	LinePt pLastLine = GetEndLine();pLastLine--;
	m_cSelectEnd.SetPosition(pLastLine,pLastLine->GetLineLength());
	m_cCaretPos = m_cSelectEnd;
}

/**
 * CFootyDoc::GetSelText
 * @brief 選択しているテキストを取得するルーチン
 * @param pString 出力先
 * @param nLineMode 
 */
bool CFootyDoc::GetSelText(std::wstring *pString, LineMode nLineMode){
	if (!pString || !IsSelecting())return false;
	// 宣言
	size_t nNowIndex = 0;						//!< 代入用、現在のインデックス番号
	size_t nReserve = GetSelLength(nLineMode);	//!< 確保量
	CEditPosition *pSelStart = GetSelStart();	//!< 選択開始位置
	CEditPosition *pSelEnd = GetSelEnd();		//!< 選択終了位置
	CEditPosition cNowPos;						//!< 現在の位置
	bool bCopyLine;

	// メモリをまとめて確保する
	pString->clear();
	pString->resize(nReserve);
	
	// 改行コードを設定
	if (nLineMode == LM_AUTOMATIC)
		nLineMode = (LineMode)GetLineMode();
	
	/*回しながらデータを入れていく*/
	cNowPos = *pSelStart;
	forever
	{
		bCopyLine = false;
		if (pSelStart->GetLineNum() == cNowPos.GetLineNum() &&
			pSelEnd->GetLineNum() == cNowPos.GetLineNum())
		{
			pString->insert(nNowIndex,
				&cNowPos.GetLinePointer()->m_strLineData[pSelStart->GetPosition()],
				pSelEnd->GetPosition() - pSelStart->GetPosition());
			break;
		}
		else if (pSelStart->GetLineNum() == cNowPos.GetLineNum())
		{
			pString->insert(nNowIndex,
				&pSelStart->GetLinePointer()->m_strLineData[pSelStart->GetPosition()]);
			nNowIndex += cNowPos.GetLinePointer()->GetLineLength() - pSelStart->GetPosition();
			bCopyLine = true;
		}
		else if (pSelEnd->GetLineNum() == cNowPos.GetLineNum())
		{
			pString->insert(nNowIndex,
				cNowPos.GetLinePointer()->m_strLineData.c_str(),pSelEnd->GetPosition());
			break;
		}
		else
		{
			pString->insert(nNowIndex,cNowPos.GetLinePointer()->m_strLineData);
			nNowIndex += cNowPos.GetLinePointer()->GetLineLength();
			bCopyLine = true;
		}

		// 改行コードのコピー
		if (bCopyLine)
		{
			switch((int)nLineMode)
			{
			case LM_CRLF:
				pString->insert(nNowIndex,L"\r\n");
				nNowIndex += 2;
				break;
			case LM_CR:
				pString->insert(nNowIndex,L"\r");
				nNowIndex += 1;
				break;
			case LM_LF:
				pString->insert(nNowIndex,L"\n");
				nNowIndex += 1;
				break;
			}
		}

		/*次の行へ*/
		cNowPos.MoveRealNext(&m_lsLines,1);
	}
	return true;
}

/**
 * CFootyDoc::DeleteSelected
 * @brief 選択しているテキストを削除します。
 * @param pUndoBuffer アンドゥに記録するためのバッファ。NULL可。
 * @note 削除した後は選択状態を解除してキャレット位置(pPos)を選択始点に移動します。
 */
CFootyDoc::RedrawType CFootyDoc::DeleteSelected(CUndoBuffer *pUndoBuffer)
{
	if (!IsSelecting())return REDRAW_FAILED;

	// 宣言
	CEditPosition *pSelStart = GetSelStart();	//!< 選択開始位置
	CEditPosition *pSelEnd = GetSelEnd();		//!< 選択終了位置
	LinePt pStartLine = pSelStart->GetLinePointer();
	LinePt pEndLine = pSelEnd->GetLinePointer();
	LinePt pStartLineBack = pStartLine;

	// 初期化
	m_cCaretPos = *pSelStart;
	if (pUndoBuffer)
	{
		pUndoBuffer->m_nUndoType = CUndoBuffer::UNDOTYPE_DELETE;
		pUndoBuffer->m_cBeforeStart = *pSelStart;
		pUndoBuffer->m_cBeforeEnd = *pSelEnd;
		GetSelText(&pUndoBuffer->m_strBefore);
	}

	// 処理
	if (pSelStart->GetLineNum() == pSelEnd->GetLineNum())
	{
		pStartLine->m_strLineData.erase(
			pSelStart->GetPosition(),pSelEnd->GetPosition()-pSelStart->GetPosition());
		SetLineInfo(pStartLine, false/*改行を含むか by Tetr@pod*/);
	}
	else
	{
		// 開始行と終了行の選択位置から削除する
		pStartLine->m_strLineData.erase(pSelStart->GetPosition());
		pStartLine->m_strLineData.insert(pSelStart->GetPosition(),
			&pEndLine->m_strLineData[pSelEnd->GetPosition()]);

		// その間の行を削除する
		pStartLine++;
		pEndLine++;
		DeleteLine(pStartLine,pEndLine);

		// データをフラッシュする
		SetLineInfo(pStartLineBack,pEndLine, true/*改行を含むか by Tetr@pod*/);
	}

	// 選択を解除する
	m_nSelectType = SELECT_NONE;
	SendMoveCaretCallBack();
	return REDRAW_ALL;
}

/**
 * CFootyDoc::ClearSelected by inovia
 * @brief 選択しているテキストを解除します。
 * @param pUndoBuffer アンドゥに記録するためのバッファ。NULL可。
 * @note 削除した後は選択状態を解除してキャレット位置(pPos)を選択始点に移動します。
 */
int CFootyDoc::ClearSelected()
{
	if (!IsSelecting())return REDRAW_FAILED;
	// 選択を解除する
	m_nSelectType = SELECT_NONE;
	SendMoveCaretCallBack();
	return REDRAW_ALL;
}

/*-------------------------------------------------------------------
CFootyDoc::GetSelLength
現在選択しているテキストの長さを取得します
-------------------------------------------------------------------*/
size_t CFootyDoc::GetSelLength(LineMode nLineMode){
	if (!IsSelecting())return 0;
	
	/*宣言*/
	size_t nSelSize = 0;						/*選択している大きさ*/
	size_t nLineCodeSize;
	CEditPosition cNowPos;						/*現在走査中の行*/
	CEditPosition *pSelStart = GetSelStart();	/*選択開始位置*/
	CEditPosition *pSelEnd = GetSelEnd();		/*選択終了位置*/
	
	/*ラインコードの長さを取得*/
	if (nLineMode == LM_AUTOMATIC)
		nLineMode = m_nLineMode;

	switch(static_cast<size_t>(nLineMode)){
	case LM_CRLF:
		nLineCodeSize = 2; break;
	case LM_CR: case LM_LF:
		nLineCodeSize = 1; break;
	}
	
	/*長さをループして計算させる*/
	cNowPos = *pSelStart;
	forever{
		/*とりあえずその行の長さ分追加する*/
		nSelSize += cNowPos.GetLinePointer()->GetLineLength();
		/*最初の選択行のときは、桁数までを引く*/
		if (pSelStart->GetLineNum() == cNowPos.GetLineNum())
			nSelSize -= pSelStart->GetPosition();
		/*最終の選択行のときは、その分を引いてループを抜ける*/
		if (pSelEnd->GetLineNum() == cNowPos.GetLineNum()){
			nSelSize -= cNowPos.GetLinePointer()->GetLineLength() - pSelEnd->GetPosition();
			break;
		}
		/*最終行でないときは、改行分プラス*/
		nSelSize += nLineCodeSize;
		cNowPos.MoveRealNext(&m_lsLines,1);
	}
	
	/*値を返却する*/
	return nSelSize;
}

/*-------------------------------------------------------------------
CFootyDoc::SelectWord
現在のキャレットの位置で単語選択処理を行います。
-------------------------------------------------------------------*/
void CFootyDoc::SelectWord(){
	/*現在の位置にある単語情報を取得します*/
	LinePt pLine = m_cCaretPos.GetLinePointer();
	CFootyLine::WordInfo wInfo = pLine->GetWordInfo(m_cCaretPos.GetPosition());
	/*単語の選択処理を行います。*/
	m_cCaretPos.SetPosition(pLine,wInfo.m_nBeginPos);
	SetSelectStart();
	m_cCaretPos.SetPosition(pLine,wInfo.m_nEndPos);
	SetSelectEndNormal();
	SendMoveCaretCallBack();
}

/*[EOF]*/
