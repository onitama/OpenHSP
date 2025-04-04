/*===================================================================
DllViewFunc.cpp
見栄えに関するDLL関数を記述します。
===================================================================*/

#include "ConvFactory.h"

/*----------------------------------------------------------------
Footy2SetColor
色設定
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetColor(int nID,int nPosition,COLORREF nColor,bool bRedraw){
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*色を変更する*/
	switch(nPosition){
	case CP_TEXT:
		pFooty->m_cStatus.m_clDefaultLetter = nColor;
		break;
	case CP_BACKGROUND:
		pFooty->m_cStatus.m_clBackGround = nColor;
		break;
	case CP_CRLF:
		pFooty->m_cStatus.m_clCrlf = nColor;
		break;
	case CP_HALFSPACE:
		pFooty->m_cStatus.m_clHalfSpace = nColor;
		break;
	case CP_NORMALSPACE:
		pFooty->m_cStatus.m_clNormalSpace = nColor;
		break;
	case CP_TAB:
		pFooty->m_cStatus.m_clTab = nColor;
		break;
	case CP_EOF:
		pFooty->m_cStatus.m_clEOF = nColor;
		break;
	case CP_UNDERLINE:
		pFooty->m_cStatus.m_clUnderLine = nColor;
		break;
	case CP_LINENUMBORDER:
		pFooty->m_cStatus.m_clLineNumLine = nColor;
		break;
	case CP_LINENUMTEXT:
		pFooty->m_cStatus.m_clLineNum = nColor;
		break;
	case CP_CARETLINE:
		pFooty->m_cStatus.m_clCaretLine = nColor;
		break;
	case CP_RULERBACKGROUND:
		pFooty->m_cStatus.m_clRulerBk = nColor;
		break;
	case CP_RULERTEXT:
		pFooty->m_cStatus.m_clRulerText = nColor;
		break;
	case CP_RULERLINE:
		pFooty->m_cStatus.m_clRulerLine = nColor;
		break;
	case CP_CARETPOS:
		pFooty->m_cStatus.m_clCaretPos = nColor;
		break;
	case CP_URLTEXT:
		pFooty->m_cStatus.m_clUrl = nColor;
		break;
	case CP_URLUNDERLINE:
		pFooty->m_cStatus.m_clUrlUnder = nColor;
		break;
	case CP_MAILTEXT:
		pFooty->m_cStatus.m_clMail = nColor;
		break;
	case CP_MAILUNDERLINE:
		pFooty->m_cStatus.m_clMailUnder = nColor;
		break;
	case CP_LABELTEXT:// by Tetr@pod
		pFooty->m_cStatus.m_clLabel = nColor;
		break;
	case CP_LABELUNDERLINE:// by Tetr@pod
		pFooty->m_cStatus.m_clLabelUnder = nColor;
		break;

	case CP_FUNCTEXT1:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc1 = nColor;
		break;
	case CP_FUNCTEXT2:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc2 = nColor;
		break;
	case CP_FUNCTEXT3:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc3 = nColor;
		break;
	case CP_FUNCTEXT4:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc4 = nColor;
		break;
	case CP_FUNCTEXT5:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc5 = nColor;
		break;
	case CP_FUNCTEXT6:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc6 = nColor;
		break;
	case CP_FUNCTEXT7:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc7 = nColor;
		break;
	case CP_FUNCTEXT8:// by Tetr@pod
		pFooty->m_cStatus.m_clFunc8 = nColor;
		break;

	case CP_HIGHLIGHTTEXT:
		pFooty->m_cStatus.m_clHighlightText = nColor;
		break;
	case CP_HIGHLIGHTBACKGROUND:
		pFooty->m_cStatus.m_clHighlightBk = nColor;
		break;
	default:
		return FOOTY2ERR_ARGUMENT;
	}
	/*再描画処理*/
	if (bRedraw)
		pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}


#ifndef UNDER_CE
/*----------------------------------------------------------------
Footy2AddEmphasisA
強調表示文字列を追加します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2AddEmphasisA(int nID,const char *pString1,const char *pString2,
	int nType,int nFlags,int nLevel,int nOnLevel,int nIndependence,COLORREF nColor){
	
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);

	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	
	/*WIDE変換*/
	/*変換して登録*/
	CConvFactory cConv1;
	if (!cConv1.GetConv()->ToUnicode(pString1,(size_t)strlen(pString1)))
		return FOOTY2ERR_MEMORY;

	const wchar_t *pConvStr2 = NULL;
	CConvFactory cConv2;
	if (pString2){
		if (!cConv2.GetConv()->ToUnicode(pString2,(UINT)strlen(pString2)))
			return FOOTY2ERR_MEMORY;
		pConvStr2 = (const wchar_t*)cConv2.GetConv()->GetConvData();
	}

	/*強調表示文字列を登録する*/
	if (!pFooty->m_cDoc.AddEmphasis
		((const wchar_t*)cConv1.GetConv()->GetConvData(),
		 pConvStr2,nType,nFlags,nLevel,nOnLevel,nIndependence,nColor))
		return FOOTY2ERR_ARGUMENT;

	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/


/*----------------------------------------------------------------
Footy2AddEmphasisW
強調表示文字列を追加します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2AddEmphasisW(int nID,const wchar_t *pString1,const wchar_t *pString2,
	int nType,int nFlags,int nLevel,int nOnLevel,int nIndependence,COLORREF nColor){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*強調表示文字列を登録する*/
	if (pFooty->m_cDoc.AddEmphasis(pString1,pString2,
		nType,nFlags,nLevel,nOnLevel,nIndependence,nColor))
		return FOOTY2ERR_NONE;
	else
		return FOOTY2ERR_ARGUMENT;
}

/*----------------------------------------------------------------
Footy2FlushEmphasis
強調表示文字列をフラッシュする
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2FlushEmphasis(int nID){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	pFooty->m_cDoc.FlushEmphasis();
	/*全て再描画*/
	pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2ClearEmphasis
強調表示文字列を消去する
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2ClearEmphasis(int nID){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	pFooty->m_cDoc.ClearEmphasis();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetFontSize
フォントのサイズを設定する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFontSize(int nID,int nPoint,bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	if (!pFooty->SetFontSize(nPoint,bRefresh))
		return FOOTY2ERR_ARGUMENT;
	return FOOTY2ERR_NONE;
}

#ifndef UNDER_CE
/*----------------------------------------------------------------
Footy2SetFontFaceA
強調表示文字列を消去する
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFontFaceA(int nID,int nFontMode,const char *pString,bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	CConvFactory cConv;
	if (!cConv.GetConv()->ToUnicode(pString,strlen(pString)))
		return FOOTY2ERR_MEMORY;
	if (!pFooty->SetFontFace(nFontMode,(const wchar_t*)cConv.GetConv()->GetConvData(),bRefresh))
		return FOOTY2ERR_ARGUMENT;
	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/

/*----------------------------------------------------------------
Footy2SetFontFaceW
強調表示文字列を消去する
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFontFaceW(int nID,int nFontMode,const wchar_t *pString,bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	if (!pFooty->SetFontFace(nFontMode,pString,bRefresh))
		return FOOTY2ERR_ARGUMENT;
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetForceFont
指定フォントを使用を強制する by inovia
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetForceFont(int nID, int flag){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	if (!pFooty->SetForceFont(flag))
		return FOOTY2ERR_ARGUMENT;
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetSpeedDraw
高速描画をONにする？ by inovia
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetSpeedDraw(int nID, int flag){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	/*エラーチェック*/
	if (!pFooty)return FOOTY2ERR_NOID;
	/*フラッシュさせる*/
	if (!pFooty->SetSpeedDraw(flag))
		return FOOTY2ERR_ARGUMENT;
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetLineIcon
行アイコンの状態を設定する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetLineIcon(int nID,size_t nLine,int nIcons,bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*行を取得する*/
	if (pFooty->m_cDoc.GetLineNum() <= nLine)
		return FOOTY2ERR_ARGUMENT;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*設定する*/
	pLine->SetLineIcons(nIcons);
	/*再描画させる*/
	if (bRefresh)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2GetLineIcon
行アイコンの状態を取得する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetLineIcon(int nID,size_t nLine,int *pIcons){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*行を取得する*/
	if (pFooty->m_cDoc.GetLineNum() <= nLine)
		return FOOTY2ERR_ARGUMENT;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*取得する*/
	if (pIcons)*pIcons = pLine->GetLineIcons();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetBackgroundImageA
背景画像を設定する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetBackgroundImageA(int nID, const char *pPath, bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*画像を設定する*/
	CConvFactory cConv;
	if (!cConv.GetConv()->ToUnicode(pPath,strlen(pPath)))
		return FOOTY2ERR_MEMORY;
	if (!pFooty->SetBackgroundImage((const wchar_t*)cConv.GetConv()->GetConvData(), bRefresh)){
		return FOOTY2ERR_404;
	}
	/*再描画させる*/
	if (bRefresh)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetBackgroundImageW
背景画像を設定する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetBackgroundImageW(int nID, const wchar_t *pPath, bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*画像を設定する*/
	if (!pFooty->SetBackgroundImage(pPath, bRefresh)){
		return FOOTY2ERR_404;
	}
	/*再描画させる*/
	if (bRefresh)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2ClearBackgroundImage
背景画像を解除する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2ClearBackgroundImage(int nID, bool bRefresh){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*画像を設定する*/
	pFooty->ClearBackgroundImage(bRefresh);
	/*再描画させる*/
	if (bRefresh)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetBackgroundImageColor
背景画像の背景色を設定する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetBackgroundImageColor(int nID, COLORREF color){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*色を設定する*/
	if (!pFooty->SetBackgroundColor(color)){
		return FOOTY2ERR_ARGUMENT;
	}
	return FOOTY2ERR_NONE;
}
/*[EOF]*/
