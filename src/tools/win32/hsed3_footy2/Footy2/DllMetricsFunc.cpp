/*===================================================================
DllMetricsFunc.cpp
数値の取得に関する関数を記述します
===================================================================*/

/*----------------------------------------------------------------
Footy2SetMetrics
数値設定
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetMetrics(int nID,int nObject,int nValue,bool bRefresh)
{
	// 宣言
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	
	switch(nObject)
	{
	// for (int i=0;i<3;i++)をfor (int i=0;i<4;i++)に修正 by Tetr@pod
	case SM_MARK_VISIBLE:
		for (int i=0;i<4;i++)
		{
			pFooty->m_cView[i].SetMarkVisible(nValue);
		}
		break;
	case SM_LINENUM_WIDTH:
		for (int i=0;i<4;i++)
		{
			pFooty->m_cView[i].SetLineWidth(nValue);
		}
		break;
	case SM_RULER_HEIGHT:
		for (int i=0;i<4;i++)
		{
			pFooty->m_cView[i].SetRulerHeight(nValue);
		}
		break;
	case SM_UNDERLINE_VISIBLE:
		for (int i=0;i<4;i++)
		{
			pFooty->m_cView[i].SetUnderlineVisible(nValue);
		}
		break;
	case SM_MARGIN_HEIGHT:
		for (int i=0;i<4;i++)
		{
			pFooty->m_cView[i].SetMarginHeight(nValue);
		}
		break;
	case SM_TAB_WIDTH:
		pFooty->m_cDoc.SetTabLen(nValue);
		break;
	default:
		return FOOTY2ERR_ARGUMENT;
	}

	/*再描画させる*/
	if (bRefresh)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2GetMetrics
数値取得
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetMetrics(int nID,int nObject,int *pValue)
{
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (!pValue)return FOOTY2ERR_ARGUMENT;

	// 返す
	switch(nObject)
	{
	case SM_LAPEL_COLUMN:
		*pValue = (int)pFooty->m_cDoc.GetLapelColumn();
		break;
	case SM_LAPEL_MODE:
		*pValue = (int)pFooty->m_cDoc.GetLapelMode();
		break;
	case SM_MARK_VISIBLE:
		*pValue = pFooty->m_cView[0].GetMarkVisible();
		break;
	case SM_LINENUM_WIDTH:
		*pValue = pFooty->m_cView[0].GetLineCountWidth();
		break;
	case SM_RULER_HEIGHT:
		*pValue = pFooty->m_cView[0].GetRulerHeight();
		break;
	case SM_UNDERLINE_VISIBLE:
		*pValue = pFooty->m_cView[0].GetUnderlineVisible();
		break;
	case SM_TAB_WIDTH:
		*pValue = pFooty->m_cDoc.GetTabLen();
		break;
	case SM_REDOREM:
		*pValue = pFooty->m_cDoc.GetRedoRem();
		break;
	case SM_UNDOREM:
		*pValue = pFooty->m_cDoc.GetUndoRem();
		break;
	default:
		return FOOTY2ERR_ARGUMENT;
	}
	return FOOTY2ERR_NONE;
}

/*[EOF]*/
