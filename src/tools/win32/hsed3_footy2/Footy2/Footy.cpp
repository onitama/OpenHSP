/**
 * @file Footy.cpp
 * @author Shinji Watanabe
 * @brief Footyの大元となるクラスです。
 * @date Dec. 28, 2008
 */

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタです。
 */
CFooty::CFooty( int nID, HINSTANCE hInstance, int nViewMode ) :
	m_nID( nID ),
	m_nViewMode( VIEWMODE_NORMAL ),
	m_nCharSet( CSM_DEFAULT ),
	m_hWndParent( NULL ),
	m_x( 0 ),
	m_y( 0 ),
	m_nWidth( 0 ),
	m_nHeight( 0 )
{
	// ドキュメント新規作成
	m_cDoc.CreateNew(m_nID);
	
	// ビュー初期化
	for ( int i = 0; i < 4; i++ )
	{
		m_cView[i].Initialize( &m_cFonts, m_cView, &m_cDoc, &m_cStatus, m_nID, i );
	}
	
	m_hInstance = hInstance;
	m_nViewMode = nViewMode;
}

//-----------------------------------------------------------------------------
/**
 * @brief Footyを作成します
 */
bool CFooty::Create( HWND hWnd, int x,int y,int nWidth, int nHeight )
{
	// エラーチェック
	if (!hWnd || nWidth < 0 || nHeight < 0)return false;
	m_hWndParent = hWnd;

	// フォントを作成する
	HDC hDC = GetDC(hWnd);
	FOOTY2_PRINTF( L"GetDC %d\n", m_nID );
	if (!m_cFonts.CreateAll(hDC))return false;
	if (!m_cFonts.SetRuler(hDC,m_cView[0].GetRulerHeight()))return false;
	ReleaseDC(hWnd,hDC);
	FOOTY2_PRINTF( L"ReleaseDC %d\n", m_nID );
	
	// メンバ変数にコピー
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_x = x;
	m_y = y;
	
	// ビュー作成
	for ( int i = 0; i < 4; i++ )
	{
		if (!m_cView[i].CreateFootyView( hWnd, m_hInstance ))
		{
			return false;
		}
	}
	if ( !m_cVSplitter.Create( hWnd, m_hInstance ) ||
		 !m_cHSplitter.Create( hWnd, m_hInstance ) )
	{
		return false;
	}

	// ビューモードを変更する
	ChangeView( m_nViewMode );
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief ビューモードの変更する
 */
void CFooty::ChangeView( int nViewMode, bool bRedraw )
{
	// もとの位置を保存 by Tetr@pod
	int oldSplitterPosX = m_x + m_nWidth / 2 - CSplitBase::SPLIT_SIZE / 2;
	int oldSplitterPosY = m_y + m_nHeight / 2 - CSplitBase::SPLIT_SIZE / 2;
	if (m_nViewMode == VIEWMODE_VERTICAL) {
		oldSplitterPosX = m_cVSplitter.GetX();
	}
	else if (m_nViewMode == VIEWMODE_HORIZONTAL) {
		oldSplitterPosY = m_cHSplitter.GetY();
	}
	else if (m_nViewMode == VIEWMODE_QUAD) {
		oldSplitterPosX = m_cVSplitter.GetX();
		oldSplitterPosY = m_cHSplitter.GetY();
	}

	m_nViewMode = nViewMode;

	// スプリッタを変更する
	m_cVSplitter.OnBaseWindowMove(m_x,m_y,m_nWidth,m_nHeight);
	m_cHSplitter.OnBaseWindowMove(m_x,m_y,m_nWidth,m_nHeight);

	// モードに合わせてビューを調節する
	switch(nViewMode)
	{
	case VIEWMODE_NORMAL:
		m_cView[0].MoveWin(m_x,m_y,m_nWidth,m_nHeight);
		m_cHSplitter.SetVisible(false);
		m_cVSplitter.SetVisible(false);
		m_cView[0].SetVisible(true);
		m_cView[1].SetVisible(false);
		m_cView[2].SetVisible(false);
		m_cView[3].SetVisible(false);
		break;
	case VIEWMODE_VERTICAL:
		// m_cVSplitter.SetViews(m_cView);
		m_cVSplitter.SetViews(this, m_cView);// by Tetr@pod
		// m_cVSplitter.MoveWin(m_x+m_nWidth / 2 - CSplitBase::SPLIT_SIZE / 2,m_y,m_nWidth,m_nHeight);
		m_cVSplitter.MoveWin(oldSplitterPosX,m_y,m_nWidth,m_nHeight);
		m_cVSplitter.SetVisible(true);
		m_cHSplitter.SetVisible(false);
		m_cView[0].SetVisible(true);
		m_cView[1].SetVisible(true);
		m_cView[2].SetVisible(false);
		m_cView[3].SetVisible(false);
		break;
	case VIEWMODE_HORIZONTAL:
		// m_cHSplitter.SetViews(m_cView);
		m_cHSplitter.SetViews(this, m_cView);// by Tetr@pod
		// m_cHSplitter.MoveWin(m_x,m_y+m_nHeight / 2 - CSplitBase::SPLIT_SIZE / 2,m_nWidth,m_nHeight);
		m_cHSplitter.MoveWin(m_x,oldSplitterPosY,m_nWidth,m_nHeight);
		m_cHSplitter.SetVisible(true);
		m_cVSplitter.SetVisible(false);
		m_cView[0].SetVisible(true);
		m_cView[1].SetVisible(true);
		m_cView[2].SetVisible(false);
		m_cView[3].SetVisible(false);
		break;
	case VIEWMODE_QUAD:
		// m_cHSplitter.SetViews(m_cView,&m_cVSplitter);
		m_cHSplitter.SetViews(this, m_cView,&m_cVSplitter);// by Tetr@pod
		// m_cVSplitter.SetViews(m_cView,&m_cHSplitter);
		m_cVSplitter.SetViews(this, m_cView,&m_cHSplitter);// by Tetr@pod
		// m_cHSplitter.MoveWin(m_x,m_y+m_nHeight / 2 - CSplitBase::SPLIT_SIZE / 2,m_nWidth,m_nHeight);
		m_cHSplitter.MoveWin(m_x,oldSplitterPosY,m_nWidth,m_nHeight);
		// m_cVSplitter.MoveWin(m_x+m_nWidth / 2 - CSplitBase::SPLIT_SIZE / 2,m_y,m_nWidth,m_nHeight);
		m_cVSplitter.MoveWin(oldSplitterPosX,m_y,m_nWidth,m_nHeight);
		m_cHSplitter.SetVisible(true);
		m_cVSplitter.SetVisible(true);
		m_cView[0].SetVisible(true);
		m_cView[1].SetVisible(true);
		m_cView[2].SetVisible(true);
		m_cView[3].SetVisible(true);
		break;
	case VIEWMODE_INVISIBLE:
		m_cHSplitter.SetVisible(false);
		m_cVSplitter.SetVisible(false);
		m_cView[0].SetVisible(false);
		m_cView[1].SetVisible(false);
		m_cView[2].SetVisible(false);
		m_cView[3].SetVisible(false);
		break;
	}

	// フォーカスを当てる(いずれかがフォーカスを持っているときのみ)
	if ( IsFocused() )
	{
		m_cView[ 0 ].SetFocus();
	}
	
	// 再描画
	if (bRedraw)
	{
		m_cView[ 0 ].Refresh();
	}
}

/*-------------------------------------------------------------------
CFooty::IsFocused
いずれかのビューがフォーカスを持っているか調査する処理
-------------------------------------------------------------------*/
bool CFooty::IsFocused(){
	// for (int i=0;i<3;i++)をfor (int i=0;i<4;i++)に修正 by Tetr@pod
	for (int i=0;i<4;i++){
		if (m_cView[i].IsFocused())return true;
	}
	return false;
}

/*-------------------------------------------------------------------
CFooty::Move
移動させるための関数です。
-------------------------------------------------------------------*/
bool CFooty::Move(int x,int y,int nWidth,int nHeight){
	if (nWidth < 0 || nHeight < 0)return false;
	
	/*元々の幅 by Tetr@pod*/
	int old_nWidth = m_nWidth;
	int old_nHeight = m_nHeight;

	/*メンバ変数へコピー*/
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_x = x;
	m_y = y;
	
	/*スプリットバーを調節*/
	m_cVSplitter.OnBaseWindowMove(m_x,m_y,m_nWidth,m_nHeight);
	m_cHSplitter.OnBaseWindowMove(m_x,m_y,m_nWidth,m_nHeight);
	
	// スプリットバーの位置を自動で調整するように変更 by Tetr@pod
	/*モードに応じてビューを設定する*/
	switch(m_nViewMode)
	{
	case VIEWMODE_NORMAL:
		m_cView[0].MoveWin(x,y,nWidth,nHeight);
		break;
	case VIEWMODE_VERTICAL:
		// m_cVSplitter.MoveWin(m_cVSplitter.GetX(),y,nWidth,nHeight);
		m_cVSplitter.MoveWin(m_cVSplitter.GetX() * nWidth / old_nWidth,y,nWidth,nHeight);// by Tetr@pod
		break;
	case VIEWMODE_HORIZONTAL:
		// m_cHSplitter.MoveWin(x,m_cHSplitter.GetY(),nWidth,nHeight);
		m_cHSplitter.MoveWin(x,m_cHSplitter.GetY() * nHeight / old_nHeight,nWidth,nHeight);// by Tetr@pod
		break;
	case VIEWMODE_QUAD:
		// m_cVSplitter.MoveWin(m_cVSplitter.GetX(),y,nWidth,nHeight);
		// m_cHSplitter.MoveWin(x,m_cHSplitter.GetY(),nWidth,nHeight);
		m_cVSplitter.MoveWin(m_cVSplitter.GetX() * nWidth / old_nWidth,y,nWidth,nHeight);// by Tetr@pod
		m_cHSplitter.MoveWin(x,m_cHSplitter.GetY() * nHeight / old_nHeight,nWidth,nHeight);// by Tetr@pod
		break;
	}
	
	/*全てのビューでキャレットを作成する*/
	for (int i=0;i<4;i++){
		m_cView[i].CaretMove();
	}
	return true;
}

/*-------------------------------------------------------------------
CFooty::Undo
アンドゥ処理を行います。
-------------------------------------------------------------------*/
bool CFooty::Undo(){
	if (!m_cDoc.Undo())
		return false;
	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++){
		if (m_cView[i].IsFocused()){
			m_cView[i].AdjustVisibleLine();
			m_cView[i].AdjustVisiblePos();
		}
	}
	m_cView[0].Refresh();
	/*イベントを通知する*/
	m_cDoc.SendTextModified(MODIFIED_CAUSE_UNDO);
	return true;
}

/*-------------------------------------------------------------------
CFooty::Redo
リドゥ処理を行います
-------------------------------------------------------------------*/
bool CFooty::Redo(){
	if (!m_cDoc.Redo())
		return false;
	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++)
	{
		if (m_cView[i].IsFocused())
		{
			m_cView[i].AdjustVisibleLine();
			m_cView[i].AdjustVisiblePos();
		}
	}
	/*再描画*/
	m_cView[0].Refresh();
	/*イベントを通知する*/
	m_cDoc.SendTextModified(MODIFIED_CAUSE_REDO);
	return true;
}

/*-------------------------------------------------------------------
CFooty::Copy
コピー処理を行います。
-------------------------------------------------------------------*/
bool CFooty::Copy()
{
	if (!m_cDoc.ClipCopy(m_cView[0].GetWnd()))
		return false;
	return true;
}

/*-------------------------------------------------------------------
CFooty::Cut
切り取り処理を行います。
-------------------------------------------------------------------*/
bool CFooty::Cut()
{
	if (!m_cDoc.ClipCopy(m_cView[0].GetWnd()))
		return false;
	m_cDoc.OnBackSpace();
	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++){
		if (m_cView[i].IsFocused()){
			m_cView[i].AdjustVisibleLine();
			m_cView[i].AdjustVisiblePos();
		}
	}
	/*再描画*/
	m_cView[0].Refresh();
	/*イベントを通知する*/
	m_cDoc.SendTextModified(MODIFIED_CAUSE_CUT);
	return true;
}


/*-------------------------------------------------------------------
CFooty::Paste
ペースト処理を行います。
-------------------------------------------------------------------*/
bool CFooty::Paste()
{
	if (!m_cDoc.ClipPaste(m_cView[0].GetWnd()))
		return false;

	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++)
	{
		if (m_cView[i].IsFocused())
		{
			m_cView[i].AdjustVisibleLine();
			m_cView[i].AdjustVisiblePos();
		}
	}
	/*再描画*/
	m_cView[0].Refresh();
	/*イベントを通知する*/
	m_cDoc.SendTextModified(MODIFIED_CAUSE_PASTE);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetSelText
選択文字列をセットします。
-------------------------------------------------------------------*/
bool CFooty::SetSelText(const wchar_t *pString, bool recUndo)
{
	if (!m_cDoc.InsertString(pString, recUndo))return false;
	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++)
	{
		if (m_cView[i].IsFocused())
		{
			m_cView[i].AdjustVisibleLine();
			m_cView[i].AdjustVisiblePos();
		}
	}
	/*再描画*/
	m_cView[0].Refresh();
	/*イベントを通知する*/
	m_cDoc.SendTextModified(MODIFIED_CAUSE_SETSELTEXT);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetText
文字列をセットします。
-------------------------------------------------------------------*/
void CFooty::SetText(const wchar_t *pString)
{
	m_cDoc.SetText(pString);
	m_cView[0].Refresh();
}


/*-------------------------------------------------------------------
CFooty::SetLapel
折り返し位置を設定します。
-------------------------------------------------------------------*/
bool CFooty::SetLapel(int nColumns,int nMode,bool bRedraw)
{
	/*カラムが正しいか調べる*/
	if (nColumns < 2)return false;

	/*セットする*/
	m_cDoc.SetLapel(nColumns,nMode);
	for (int i=0;i<4;i++)
	{
		m_cView[i].SetVisibleCols();
		m_cView[i].SetFirstColumn(0);
	}
	if (bRedraw)m_cView[0].Refresh(true);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetFontSize
フォントのサイズを設定します
-------------------------------------------------------------------*/
bool CFooty::SetFontSize(int nPoint,bool bRedraw)
{
	/*ポイントサイズが正しいかチェックする*/
	if (nPoint < 2)return false;
	
	/*セットする*/
	HDC hDC = GetDC(m_hWndParent);
	m_cFonts.SetFontSize(nPoint,hDC);
	ReleaseDC(m_hWndParent,hDC);
	
	for (int i=0;i<4;i++)
	{
		m_cView[i].SetVisibleCols();
		m_cView[i].SetFirstColumn(0);
	}
	if (bRedraw)m_cView[0].Refresh(true);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetFontSize
フォントのサイズを設定します
-------------------------------------------------------------------*/
bool CFooty::SetFontFace(int nType,const wchar_t *pFaceName,bool bRedraw)
{
	/*セットする*/
	HDC hDC = GetDC(m_hWndParent);
	m_cFonts.SetFontFace(nType,pFaceName,hDC);
	ReleaseDC(m_hWndParent,hDC);
	
	for (int i=0;i<4;i++)
	{
		m_cView[i].SetVisibleCols();
		m_cView[i].SetFirstColumn(0);
	}
	if (bRedraw)m_cView[0].Refresh(true);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetForceFont
指定フォントの強制使用 by inovia
-------------------------------------------------------------------*/
bool CFooty::SetForceFont(int flag)
{
	/*セットする*/
	m_cFonts.SetForceFont(flag);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetSpeedDraw
指定フォントの強制使用 by inovia
-------------------------------------------------------------------*/
bool CFooty::SetSpeedDraw(int flag)
{
	/*セットする*/
	m_cDoc.SetSpeedDraw(flag);
	return true;
}

/*-------------------------------------------------------------------
CFooty::SetBackgroundImage
背景画像を設定する処理
-------------------------------------------------------------------*/
bool CFooty::SetBackgroundImage(const wchar_t *pFilePath, bool bRedraw)
{
	/*セットする*/
	
	for (int i=0;i<4;i++)
	{
		m_cView[i].ImageLoad(pFilePath);
	}
	if (bRedraw)m_cView[0].Refresh(true);
	return true;
}

/*-------------------------------------------------------------------
CFooty::ClearBackgroundImage
背景画像を解除する処理
-------------------------------------------------------------------*/
bool CFooty::ClearBackgroundImage(bool bRedraw)
{
	/*セットする*/
	
	for (int i=0;i<4;i++)
	{
		m_cView[i].ImageClear();
	}
	if (bRedraw)m_cView[0].Refresh(true);
	return true;
}
/*-------------------------------------------------------------------
CFooty::SetBackgroundColor
背景色を設定する処理
-------------------------------------------------------------------*/
bool CFooty::SetBackgroundColor(COLORREF color)
{
	/*セットする*/
	
	for (int i=0;i<4;i++)
	{
		m_cView[i].SetColor(color);
	}
	return true;
}
/*[EOF]*/
