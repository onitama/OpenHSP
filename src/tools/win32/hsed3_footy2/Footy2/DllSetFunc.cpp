/*===================================================================
DllMetricsFunc.cpp
数値の取得に関する関数を記述します
===================================================================*/

/*----------------------------------------------------------------
Footy2SetFuncFocus
フォーカスを得たとかの関数
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFuncFocus(int nID,Footy2FuncFocus pFunc,void *pData){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*関数設定*/
	for (int i=0;i<4;i++){
		pFooty->m_cView[i].m_pFuncFocus = pFunc;
		pFooty->m_cView[i].m_pDataFocus = pData;
	}
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetFuncMoveCaret
キャレットが移動されたときのイベント
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFuncMoveCaret(int nID,Footy2FuncMoveCaret pFunc,void *pData){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*関数設定*/
	pFooty->m_cDoc.m_pFuncMoveCaret = pFunc;
	pFooty->m_cDoc.m_pDataMoveCaret = pData;
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetFuncTextModified
テキストが編集されたときのイベント
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFuncTextModified(int nID,Footy2FuncTextModified pFunc,void *pData){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*関数設定*/
	pFooty->m_cDoc.m_pFuncTextModified = pFunc;
	pFooty->m_cDoc.m_pDataTextModified = pData;
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetFuncTextModified
テキストが編集されたときのイベント
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFuncInsertModeChanged(int nID,Footy2FuncInsertModeChanged pFunc,void *pData){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*関数設定*/
	pFooty->m_cDoc.m_pFuncInsertMode = pFunc;
	pFooty->m_cDoc.m_pDataInsertModeChanged = pData;
	return FOOTY2ERR_NONE;
}

/*[EOF]*/
