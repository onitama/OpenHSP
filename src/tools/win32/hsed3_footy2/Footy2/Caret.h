/*===================================================================
CCaretクラス
キャレットの作成、管理を行うクラスです。
===================================================================*/

#pragma once

class CCaret{
public:
	/*コンストラクタ*/
	CCaret();
	virtual ~CCaret();

	void SetParam(HWND hWnd){m_hWnd = hWnd;}

	/*キャレット命令*/
	inline void ReCreate(int nWidth,int nHeight);
	inline void Move(int nPosX,int nPosY);
	inline void Destroy();
	inline void Show();
	inline void Hide();
	
	/*取得命令*/
	inline int GetNowX(){return m_nCurrentCaretX;}
	inline int GetNowY(){return m_nCurrentCaretY;}
	inline bool IsShown(){return m_bCaretShown;}

private:
	/*このデータ*/
	bool m_bCaretShown;							/*キャレットが表示されているか*/
	int m_nCurrentCaretX;						/*キャレットの相対座標*/
	int m_nCurrentCaretY;						/*キャレットの相対座標*/

	/*システムデータ*/
	HWND m_hWnd;								/*ウィンドウハンドル*/
};

/*-------------------------------------------------------------------
CCaret::Show
キャレットを表示する
-------------------------------------------------------------------*/
void CCaret::Show(){
	if (!m_bCaretShown || m_hWnd){
		ShowCaret(m_hWnd);
		m_bCaretShown = true;
	}
}

/*-------------------------------------------------------------------
CCaret::Hide
キャレットを非表示にする
-------------------------------------------------------------------*/
void CCaret::Hide(){
	if (m_bCaretShown || m_hWnd){
		HideCaret(m_hWnd);
		m_bCaretShown = false;
	}
}

/*-------------------------------------------------------------------
CCaret::ReCreate
キャレットを再構築する
-------------------------------------------------------------------*/
void CCaret::ReCreate(int nWidth,int nHeight){
	if (m_hWnd){
		Hide();
		Destroy();
		CreateCaret(m_hWnd,NULL,nWidth,nHeight);
	}
}

/*-------------------------------------------------------------------
CCaret::Destroy
キャレットを破棄する
-------------------------------------------------------------------*/
void CCaret::Destroy(){
	DestroyCaret();
}

/*-------------------------------------------------------------------
CCaret::Move
キャレットを移動する
-------------------------------------------------------------------*/
void CCaret::Move(int nPosX,int nPosY){
	SetCaretPos(nPosX,nPosY);
	m_nCurrentCaretX = nPosX;
	m_nCurrentCaretY = nPosY;
}

/*[EOF]*/
