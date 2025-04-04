/**
 * @file FootyViewScroll
 * @brief Footyのビュー管理を行います。このファイルでスクロール関係の処理を行います。
 * @author Shinji Watanabe
 * @version 1.0
 */

#include "FootyView.h"

/**
 * CFootyView::SetBars
 * @brief スクロールバーを現在のメンバ変数パラメータに従って設定します。
 */
void CFootyView::SetBars(){
	/*宣言*/
	size_t nEthicLines;
	LinePt pLastLine = m_pDocuments->GetLastLine();
	
	/*表示できる量を計算する*/
	nEthicLines = pLastLine->GetOffset();
	nEthicLines += pLastLine->GetEthicLine();
	
	/*垂直スクロールバー*/
	m_vInfo.cbSize = sizeof(SCROLLINFO);
	m_vInfo.fMask = SIF_ALL;
	if (nEthicLines == 1)		/*一行のときは、表示行数＋１の値*/
		m_vInfo.nMax = (int)m_nVisibleLines;
	else						/*それ以外のときは、最下行が一番上にこれるように*/
		m_vInfo.nMax = (int)(m_nVisibleLines + nEthicLines - 2);
	m_vInfo.nMin=0;
	m_vInfo.nPage = (unsigned)m_nVisibleLines;
	m_vInfo.nPos = (int)(m_pDocuments->GetFirstVisible(m_nViewID)->
		GetLinePointer()->GetOffset() +
		m_pDocuments->GetFirstVisible(m_nViewID)->GetEthicNum());
	m_vInfo.nTrackPos = 0;
	SetScrollInfo(m_hWnd,SB_VERT,&m_vInfo,true);

	if (m_nVisibleColumns <= m_pDocuments->GetLapelColumn()){
		/*水平スクロールバーを表示する*/
		m_hInfo.cbSize=sizeof(SCROLLINFO);
		m_hInfo.fMask=SIF_ALL;
		m_hInfo.nMax = (int)(m_pDocuments->GetLapelColumn() + m_nVisibleColumns - 2);
		m_hInfo.nMin = 0;
		m_hInfo.nPage = (int)m_nVisibleColumns;
		m_hInfo.nPos = (int)m_nFirstVisibleColumn;
		m_hInfo.nTrackPos = 0;
		SetScrollInfo(m_hWnd,SB_HORZ,&m_hInfo,true);
	}
	else{
		/*水平スクロールバーは非表示に*/
		m_hInfo.cbSize=sizeof(SCROLLINFO);
		m_hInfo.fMask=SIF_ALL;
		m_hInfo.nMax=0;
		m_hInfo.nMin=0;
		m_hInfo.nPage=0;
		m_hInfo.nPos=0;
		m_hInfo.nTrackPos=0;
		SetScrollInfo(m_hWnd,SB_HORZ,&m_hInfo,true);
	}
}

// この辺いじった
int g_dy;
/**
 * CFootyView::OnVScroll
 * @brief 垂直スクロールバーが操作されたときに呼ばれます。
 * @param wParam ウィンドウメッセージのWPARAMを引き継ぎます
 */

/*
void CFootyView::OnVScroll(WPARAM wParam){
	int dy;                   // スクロール量
	SCROLLINFO sbInfo;

	switch (LOWORD(wParam)) {
	case SB_LINEUP:
		g_dy = -1; 
		return;                // 三角ボタン上
	case SB_LINEDOWN:
		g_dy = 1;
		return;                // 三角ボタン下
	case SB_PAGEUP:           // ちょこっと間を上へ
		g_dy = -1*m_vInfo.nPage;
		return;
	case SB_PAGEDOWN:         // ちょこっと間を下へ
		g_dy = m_vInfo.nPage;
		return;
	case SB_THUMBPOSITION:    // スクロールをぐりぐり動かす
	case SB_THUMBTRACK:       // スクロールをぐりぐり動かす
		ZeroMemory(&sbInfo, sizeof(SCROLLINFO));
		sbInfo.cbSize = sizeof(SCROLLINFO);
		sbInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(m_hWnd,SB_VERT,&sbInfo);
		g_dy = sbInfo.nTrackPos - m_vInfo.nPos;
		return;
	case SB_ENDSCROLL:
		dy = g_dy;
		break;
    default:
		return;
    }

	// 最初に表示されている行を移動させる
	if (dy < 0)m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(-dy);
	else m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),dy);

	// 再描画させる
	ScrollRefresh();
}
*/
void CFootyView::OnVScroll(WPARAM wParam){
	/*宣言*/
	int dy;                   // スクロール量
	SCROLLINFO sbInfo;

	switch (LOWORD(wParam)) {
	case SB_LINEUP:
		dy = -1; 
		break;                // 三角ボタン上
	case SB_LINEDOWN:
		dy = 1;
		break;                // 三角ボタン下
	case SB_PAGEUP:           // ちょこっと間を上へ
		dy = -1*m_vInfo.nPage;
		break;
	case SB_PAGEDOWN:         // ちょこっと間を下へ
		dy = m_vInfo.nPage;
		break;
	case SB_THUMBPOSITION:    // スクロールをぐりぐり動かす
	case SB_THUMBTRACK:       // スクロールをぐりぐり動かす
		ZeroMemory(&sbInfo, sizeof(SCROLLINFO));
		sbInfo.cbSize = sizeof(SCROLLINFO);
		sbInfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(m_hWnd,SB_VERT,&sbInfo);
		dy = sbInfo.nTrackPos - m_vInfo.nPos;
		break;
    default:
		return;
    }

	// 最初に表示されている行を移動させる
	if (dy < 0)m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(-dy);
	else m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),dy);

	// 再描画させる
	if (m_ImgCtr->IsLoaded()){
		Refresh();// 背景画像が読まれている場合は、残像が残る為すべて再描画する
	}else{
		ScrollRefresh();
	}
}

/**
 * CFootyView::OnHScroll
 * @brief 水平スクロールが動かされたときの処理
 * @param wParam ウィンドウメッセージの値を引き継ぐ
 */
void CFootyView::OnHScroll(WPARAM wParam){
	int dx;                   /*スクロール量*/

	switch (LOWORD(wParam)) {
	case SB_LINEUP:
		dx=-1; 
		break;                /*三角ボタン上*/
	case SB_LINEDOWN:
		dx=1;
		break;                /*三角ボタン下*/
	case SB_PAGEUP:           /*ちょこっと間を上へ*/
		dx = -1 * m_hInfo.nPage;
		break;
	case SB_PAGEDOWN:         /*ちょこっと間を下へ*/
		dx = m_hInfo.nPage;
		break;
	case SB_THUMBPOSITION:    /*スクロールをぐりぐり動かす*/
	case SB_THUMBTRACK:       /*スクロールをぐりぐり動かす*/
		dx = HIWORD(wParam) - m_hInfo.nPos;
		break;
    default:
		return;
    }

	m_nFirstVisibleColumn += dx;
	if (m_nFirstVisibleColumn < 0)m_nFirstVisibleColumn = 0;
	if (m_nFirstVisibleColumn > (int)m_pDocuments->GetLapelColumn())
		m_nFirstVisibleColumn = (int)m_pDocuments->GetLapelColumn();
	
	Refresh();
}

/*[EOF]*/
