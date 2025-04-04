/*===================================================================
DllCaretFunc.cpp
キャレット関連の処理を行います
===================================================================*/

/*----------------------------------------------------------------
Footy2GetCaretPosition
キャレット位置を取得します
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetCaretPosition(int nID,size_t *pCaretLine,size_t *pCaretPos){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;

	/*キャレットの位置を取得*/
	CEditPosition *pPosition = pFooty->m_cDoc.GetCaretPosition();
	if (pCaretLine)
		*pCaretLine = pPosition->GetLineNum();
	if (pCaretPos)
		*pCaretPos = pPosition->GetPosition();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetCaretPosition
キャレット位置を変更します
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetCaretPosition(int nID,size_t nCaretLine,size_t nCaretPos,bool bRefresh){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;

	/*キャレット位置を更新*/
	CEditPosition *pPosition = pFooty->m_cDoc.GetCaretPosition();
	if (!pPosition->SetPosition(pFooty->m_cDoc.GetLineList(),nCaretLine,nCaretPos))
		return FOOTY2ERR_ARGUMENT;
	else{
		pFooty->m_cDoc.UnSelect();
		pFooty->m_cDoc.SendMoveCaretCallBack();
		if (bRefresh){
			pFooty->m_cView[0].AdjustVisiblePos();
			pFooty->m_cView[0].AdjustVisibleLine();
			pFooty->m_cView[0].Refresh();
		}
		return FOOTY2ERR_NONE;
	}
}

/**
 * Footy2GetSel
 * @brief 選択の開始位置を取得する
 * @param nID FootyのID
 * @param pStartLine [out]選択開始行
 * @param pStartPos [out]選択開始桁
 * @param pEndLine [out]選択終了行
 * @param pEndPos [out]選択終了桁
 */
FOOTYEXPORT(int) Footy2GetSel(int nID,size_t *pStartLine,size_t *pStartPos,size_t *pEndLine,size_t *pEndPos){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*選択位置を取得する*/
	if (pFooty->m_cDoc.IsSelecting()){
		CEditPosition *pStart = pFooty->m_cDoc.GetSelStart();
		CEditPosition *pEnd = pFooty->m_cDoc.GetSelEnd();
		/*ポインタに返す*/
		if (pStartLine)
			*pStartLine = pStart->GetLineNum();
		if (pStartPos)
			*pStartPos = pStart->GetPosition();
		if (pEndLine)
			*pEndLine = pEnd->GetLineNum();
		if (pEndPos)
			*pEndPos = pEnd->GetPosition();
		/*正常に終了した*/
		return FOOTY2ERR_NONE;
	}
	else
		return FOOTY2ERR_NOTSELECTED;
}

/*----------------------------------------------------------------
Footy2SetSel
選択を開始させる
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetSel(int nID,size_t nStartLine,size_t nStartPos,size_t nEndLine,size_t nEndPos,bool bRefresh){
	/*数値が正しいかどうかチェックする*/
	if (nStartLine > nEndLine)return FOOTY2ERR_ARGUMENT;
	else if (nStartLine == nEndLine){
		if (nStartPos > nEndPos)return FOOTY2ERR_ARGUMENT;
	}
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	
	/*選択する*/
	CEditPosition cSelStart,cSelEnd;
	if (!cSelStart.SetPosition(pFooty->m_cDoc.GetLineList(),nStartLine,nStartPos) ||
		!cSelEnd.SetPosition(pFooty->m_cDoc.GetLineList(),nEndLine,nEndPos) ||
		!pFooty->m_cDoc.SetSelectStart(&cSelStart) ||
		!pFooty->m_cDoc.SetSelectEndNormal(&cSelEnd))
		return FOOTY2ERR_ARGUMENT;
	*pFooty->m_cDoc.GetCaretPosition() = cSelEnd;
	
	/*再描画*/
	if (bRefresh){
		pFooty->m_cView[0].AdjustVisiblePos();
		pFooty->m_cView[0].AdjustVisibleLine();
		pFooty->m_cView[0].Refresh();
	}
	
	/*関数が正常に終了した*/
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2ResetSel
選択を強制解除する by inovia
----------------------------------------------------------------*/
//FOOTYEXPORT(int) Footy2ResetSel(int nID,bool bRefresh){
//	/*Footyを取得する*/
//	CFooty *pFooty = GetFooty(nID);
//	if (!pFooty)return FOOTY2ERR_NOID;
//	
//	/*選択する*/
//	CEditPosition cSelStart,cSelEnd;
//	if (!cSelStart.SetPosition(pFooty->m_cDoc.GetLineList(),nStartLine,nStartPos) ||
//		!cSelEnd.SetPosition(pFooty->m_cDoc.GetLineList(),nEndLine,nEndPos) ||
//		!pFooty->m_cDoc.SetSelectStart(&cSelStart) ||
//		!pFooty->m_cDoc.SetSelectEndNormal(&cSelEnd))
//		return FOOTY2ERR_ARGUMENT;
//	*pFooty->m_cDoc.GetCaretPosition() = cSelEnd;
//	
//	/*再描画*/
//	if (bRefresh){
//		pFooty->m_cView[0].AdjustVisiblePos();
//		pFooty->m_cView[0].AdjustVisibleLine();
//		pFooty->m_cView[0].Refresh();
//		pFooty->m_cView[0].Refresh();
//	}
//	
//	/*関数が正常に終了した*/
//	return FOOTY2ERR_NONE;
//}

/*----------------------------------------------------------------
Footy2SetUnderlineDraw
フォーカスされていないときにアンダーバーを描画するかどうか？ by inovia
デフォルト: false
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetUnderlineDraw(int nID, bool bFlag){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	pFooty->m_cView[0].SetUnderlineDraw(bFlag);
	pFooty->m_cView[1].SetUnderlineDraw(bFlag);
	pFooty->m_cView[2].SetUnderlineDraw(bFlag);
	pFooty->m_cView[3].SetUnderlineDraw(bFlag);
	return FOOTY2ERR_NONE;
}

/*[EOF]*/