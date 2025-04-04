/*===================================================================
CHSplitter
水平スクロールバーの管理クラスです。
===================================================================*/

#include "HSplitter.h"
#include "Cursor.h"

/*-------------------------------------------------------------------
CHSplitter::CHSplitter
コンストラクタ
-------------------------------------------------------------------*/
CHSplitter::CHSplitter(){
}


/*-------------------------------------------------------------------
CHSplitter::MoveWin
スプリッターを移動させる
-------------------------------------------------------------------*/
bool CHSplitter::MoveWin(int x,int y,int nWidth,int nHeight){
	/*エラーチェック*/
	if (y <= 0 || m_nBaseHeight < y)return false;
	/*アタッチされたウィンドウを移動させる*/
	if (m_pViews){
		if (m_nMode == SPLIT_DUAL){
			m_pViews[0].MoveWin(x,m_nBaseY,nWidth,y-m_nBaseY);
			m_pViews[1].MoveWin(x,y+SPLIT_SIZE,nWidth,nHeight-(y-m_nBaseY)-SPLIT_SIZE);
		}
		else if (m_pOtherSplit){
			//m_pViews[0].MoveWin(x,m_nBaseY,m_pOtherSplit->GetX(),y-m_nBaseY);
			m_pViews[0].MoveWin(x,m_nBaseY,m_pOtherSplit->GetX()-x,y-m_nBaseY);// バグ修正 by Tetr@pod
			m_pViews[1].MoveWin(m_pOtherSplit->GetX()+SPLIT_SIZE,m_nBaseY,
				nWidth-m_pOtherSplit->GetX()-SPLIT_SIZE,y-m_nBaseY);
			//m_pViews[2].MoveWin(x,y+SPLIT_SIZE,m_pOtherSplit->GetX(),m_nBaseHeight-(y-m_nBaseY)-SPLIT_SIZE);
			m_pViews[2].MoveWin(x,y+SPLIT_SIZE,m_pOtherSplit->GetX()-x,m_nBaseHeight-(y-m_nBaseY)-SPLIT_SIZE);// バグ修正 by Tetr@pod
			m_pViews[3].MoveWin(m_pOtherSplit->GetX()+SPLIT_SIZE,y+SPLIT_SIZE,
				nWidth-m_pOtherSplit->GetX()-SPLIT_SIZE,m_nBaseHeight-(y-m_nBaseY)-SPLIT_SIZE);
		}
	}
	/*スプリットバーを移動させる*/
	MoveWindow(GetWnd(),x,y,nWidth,SPLIT_SIZE,true);
	/*メンバ変数を代入*/
	m_x = x;
	m_y = y;
	return true;
}


/*-------------------------------------------------------------------
CHSplitter::OnMouseMove
マウスが動かされたときの処理
-------------------------------------------------------------------*/
void CHSplitter::OnMouseMove(int x,int y){
	/*if (m_bDrag)
		MoveWin(m_x,m_y+y,m_nBaseWidth,m_nBaseHeight);
	CCursor::UseUpDown();*/
	// 中央でのドラッグ処理追加 by Tetr@pod
	if (m_bDrag) {
		if (m_nMode == SPLIT_QUAD && m_pOtherSplit && m_bIsCenter) {// 中心か？
			FOOTY2_PRINTF( L"Center dragging! x=%d, y=%d, mx=%d, my=%d, omx=%d, omy=%d\n", x, y, m_x, m_y, m_pOtherSplit->GetX(), m_pOtherSplit->GetY() );
			MoveWin(m_x,m_y+y,m_nBaseWidth,m_nBaseHeight);
			m_pOtherSplit->OnBaseWindowMove(m_nBaseX,m_nBaseY,m_nBaseWidth,m_nBaseHeight);
			m_pOtherSplit->MoveWin(x,m_pOtherSplit->GetY(),m_nBaseWidth,m_nBaseHeight);
			CCursor::UseCross();
		}
		else {
			MoveWin(m_x,m_y+y,m_nBaseWidth,m_nBaseHeight);
			CCursor::UseUpDown();
		}
	}
	else {
		if (m_nMode == SPLIT_QUAD && m_pOtherSplit && x >= m_pOtherSplit->GetX() - SPLIT_CENTER_RANGE1 && x < m_pOtherSplit->GetX() + SPLIT_CENTER_RANGE2) {// 中心か？
			CCursor::UseCross();
		}
		else {
			CCursor::UseUpDown();
		}
	}
}

void CHSplitter::OnMouseDouble(int x,int y){// by Tetr@pod
	//FOOTY2_PRINTF( L"Double Click! x=%d, ox=%d, bx=%d\n", x, m_pOtherSplit->GetX(), m_nBaseX );
	if (m_nMode == SPLIT_DUAL){
		m_pFooty->ChangeView(VIEWMODE_NORMAL, true);
	}
	else if (m_pOtherSplit) {
		if (m_nMode == SPLIT_QUAD && m_pOtherSplit && x >= m_pOtherSplit->GetX() - SPLIT_CENTER_RANGE1 && x < m_pOtherSplit->GetX() + SPLIT_CENTER_RANGE2) {// 中心か？
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

void CHSplitter::OnMouseDown(int x,int y)
{
	if (m_bDrag == false) {// by Tetr@pod
		SetCapture(m_hWnd2);
		m_bDrag = true;
		if (m_pOtherSplit) {
			m_bIsCenter = (x >= m_pOtherSplit->GetX() - SPLIT_CENTER_RANGE1 && x < m_pOtherSplit->GetX() + SPLIT_CENTER_RANGE2);// 中心か？ by Tetr@pod
		}
	}
}

/*[EOF]*/