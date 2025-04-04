/*===================================================================
CVSplitter
垂直スクロールバーの管理クラスです。
===================================================================*/

#include "VSplitter.h"
#include "Cursor.h"

/*-------------------------------------------------------------------
CVSplitter::CVSplitter
コンストラクタ
-------------------------------------------------------------------*/
CVSplitter::CVSplitter(){
}

/*-------------------------------------------------------------------
CVSplitter::MoveWin
スプリッターを移動させる
-------------------------------------------------------------------*/
bool CVSplitter::MoveWin(int x,int y,int nWidth,int nHeight){
	FOOTY2_PRINTF( L"MoveWin y=%d, nHeight=%d\n", y, nHeight );
	/*エラーチェック*/
	if (x <= 0 || m_nBaseWidth < x)return false;
	if (nHeight < y)return false;
	/*アタッチされたビューを移動させる*/
	if (m_pViews){
		FOOTY2_PRINTF( L"MoveWinOK y=%d, nHeight=%d\n", y, nHeight );
		if (m_nMode == SPLIT_DUAL){
			m_pViews[0].MoveWin(m_nBaseX,m_nBaseY,x-m_nBaseX,nHeight);
			m_pViews[1].MoveWin(x+SPLIT_SIZE,m_nBaseY,nWidth-(x-m_nBaseX)-SPLIT_SIZE,nHeight);
			FOOTY2_PRINTF( L"MoveWinD y=%d, nHeight=%d\n", y, nHeight );
		}
		else if (m_pOtherSplit){
			m_pViews[0].MoveWin(m_nBaseX,y,x-m_nBaseX,m_pOtherSplit->GetY()-y);
			m_pViews[1].MoveWin(x+SPLIT_SIZE,y,nWidth-x-SPLIT_SIZE,m_pOtherSplit->GetY()-y);
			m_pViews[2].MoveWin(m_nBaseX,m_pOtherSplit->GetY()+SPLIT_SIZE,
				x-m_nBaseX,nHeight-(m_pOtherSplit->GetY()-y)-SPLIT_SIZE);
			m_pViews[3].MoveWin(x+SPLIT_SIZE,m_pOtherSplit->GetY()+SPLIT_SIZE,
				nWidth-x-SPLIT_SIZE,nHeight-(m_pOtherSplit->GetY()-y)-SPLIT_SIZE);
			FOOTY2_PRINTF( L"MoveWinQ y=%d, nHeight=%d\n", y, nHeight );
		}
	}
	/*スプリットバーを移動させる*/
	MoveWindow(GetWnd(),x,y,SPLIT_SIZE,nHeight,true);
	/*メンバ変数を代入*/
	m_x = x;
	m_y = y;
	return true;
}


/*-------------------------------------------------------------------
CVSplitter::OnMouseMove
マウスが動かされたときの処理
-------------------------------------------------------------------*/
void CVSplitter::OnMouseMove(int x,int y){
	/*if (m_bDrag)
		MoveWin(m_x+x,m_y,m_nBaseWidth,m_nBaseHeight);
	CCursor::UseRightLeft();*/
	// 中央でのドラッグ処理追加 by Tetr@pod
	if (m_bDrag) {
		if (m_nMode == SPLIT_QUAD && m_pOtherSplit && m_bIsCenter) {// 中心か？
			FOOTY2_PRINTF( L"Center dragging! x=%d, y=%d, mx=%d, my=%d, omx=%d, omy=%d\n", x, y, m_x, m_y, m_pOtherSplit->GetX(), m_pOtherSplit->GetY() );
			MoveWin(m_x+x,m_y,m_nBaseWidth,m_nBaseHeight);
			m_pOtherSplit->OnBaseWindowMove(m_nBaseX,m_nBaseY,m_nBaseWidth,m_nBaseHeight);
			m_pOtherSplit->MoveWin(m_pOtherSplit->GetX(),y,m_nBaseWidth,m_nBaseHeight);
			CCursor::UseCross();
		}
		else {
			MoveWin(m_x+x,m_y,m_nBaseWidth,m_nBaseHeight);
			CCursor::UseRightLeft();
		}
	}
	else {
		if (m_nMode == SPLIT_QUAD && m_pOtherSplit && y >= m_pOtherSplit->GetY() - SPLIT_CENTER_RANGE1 && y < m_pOtherSplit->GetY() + SPLIT_CENTER_RANGE2) {// 中心か？
			CCursor::UseCross();
		}
		else {
			CCursor::UseRightLeft();
		}
	}
}

void CVSplitter::OnMouseDouble(int x,int y){// by Tetr@pod
	//FOOTY2_PRINTF( L"Double Click! y=%d, oy=%d, by=%d\n", y, m_pOtherSplit->GetY(), m_nBaseX );
	if (m_nMode == SPLIT_DUAL){
		m_pFooty->ChangeView(VIEWMODE_NORMAL, true);
	}
	else if (m_pOtherSplit) {
		if (y >= m_pOtherSplit->GetY() - SPLIT_CENTER_RANGE1 && y < m_pOtherSplit->GetY() + SPLIT_CENTER_RANGE2) {// 中心か？
			m_pFooty->ChangeView(VIEWMODE_NORMAL, true);
		}
		else {
			m_pFooty->ChangeView(VIEWMODE_VERTICAL, true);
		}
	}
	if (m_bDrag) {
		ReleaseCapture();
		m_bDrag = false;
	}
}

void CVSplitter::OnMouseDown(int x,int y)
{
	if (m_bDrag == false) {// by Tetr@pod
		SetCapture(m_hWnd2);
		m_bDrag = true;
		if (m_pOtherSplit) {
			m_bIsCenter = (y >= m_pOtherSplit->GetY() - SPLIT_CENTER_RANGE1 && y < m_pOtherSplit->GetY() + SPLIT_CENTER_RANGE2);// 中心か？ by Tetr@pod
		}
	}
}

/*[EOF]*/
