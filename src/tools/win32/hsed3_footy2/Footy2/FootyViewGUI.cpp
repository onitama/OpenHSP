/**
 * @file CFootyViewクラス
 * @brief このファイルはGUI描画処理を担当します。
 * @author Shinji Watanabe
 * @version 1.0
 */

#include "FootyView.h"
#include "StaticStack.h"

#ifndef UNDER_CE
#	pragma comment (lib, "Msimg32.lib")
#endif	/*not defined UNDER_CE*/

/*-------------------------------------------------------------------
CFootyView::MoveWin
大きさの変更。bRefreshがfalseのとき、ただ単に領域計算のみを行う
-------------------------------------------------------------------*/
bool CFootyView::MoveWin(int x,int y,int nWidth,int nHeight,bool bRefresh){
	/*宣言*/
	int nDrawAreaWidth;			/*描画領域幅*/
	int nDrawAreaHeight;		/*描画領域高さ*/

	/*情報からメンバ変数を設定する*/
	m_x = x;					/*x座標*/
	m_y = y;					/*y座標*/
	m_nHeight = nHeight;		/*幅*/
	m_nWidth = nWidth;			/*高さ*/
	
	/*表示できる桁数*/
	nDrawAreaWidth = nWidth - m_nLineCountWidth - GetSystemMetrics(SM_CXVSCROLL);
	if (nDrawAreaWidth > 0)
		m_nVisibleColumns = nDrawAreaWidth / (m_pFonts->GetWidth() + m_nWidthMargin);
	else
		m_nVisibleColumns = 0;

	/*表示できる行数*/
	if (m_nVisibleColumns <= m_pDocuments->GetLapelColumn())/*水平スクロールを表示している*/
		nDrawAreaHeight = nHeight - m_nRulerHeight - GetSystemMetrics(SM_CYVSCROLL);
	else													/*水平スクロールを表示していない*/
		nDrawAreaHeight = nHeight - m_nRulerHeight;

	if (nDrawAreaHeight > 0)
		m_nVisibleLines = nDrawAreaHeight / (m_pFonts->GetHeight() + m_nHeightMargin);
	else
		m_nVisibleLines = 0;

	/*移動して再描画処理*/
	if (bRefresh){
		m_cDoubleBuffering.ChangeSize(m_hWnd,nWidth,nHeight);
		MoveWindow(m_hWnd,x,y,m_nWidth,m_nHeight,true);	/*ウィンドウの移動*/
		Refresh(false);
	}
	return true;
}

/*-------------------------------------------------------------------
CFootyView::SetVisible
表示・非表示の設定
-------------------------------------------------------------------*/
void CFootyView::SetVisible(bool bVisible){
	ShowWindow(m_hWnd,bVisible ? SW_SHOWNORMAL : SW_HIDE);
	m_bVisible = bVisible;
}

/*-------------------------------------------------------------------
CFootyView::SetLineWidth
行番号表示幅を設定する
-------------------------------------------------------------------*/
void CFootyView::SetLineWidth(int nWidth){
	if (nWidth < 0)
		m_nLineCountWidth = LINEWIDTH_DEFAULT;
	else
		m_nLineCountWidth = nWidth;
	MoveWin(m_x,m_y,m_nWidth,m_nHeight,false);
}

/*-------------------------------------------------------------------
CFootyView::SetRulerHeight
ルーラー高さを指定する
-------------------------------------------------------------------*/
void CFootyView::SetRulerHeight(int nHeight){
	if (nHeight < 0)
		m_nRulerHeight = RULERHEIGHT_DEFAULT;
	else
		m_nRulerHeight = nHeight;
	MoveWin(m_x,m_y,m_nWidth,m_nHeight,false);
}

/*-------------------------------------------------------------------
CFootyView::SetMarginHeight
マージン高さを指定する
-------------------------------------------------------------------*/
void CFootyView::SetMarginHeight(int nHeight){
	if (nHeight < 0) {
		m_nHeightMargin = 0 ;
		m_nLineCountMargin = 0;
	} else {
		m_nHeightMargin = nHeight;
		m_nLineCountMargin = nHeight;
	}
	MoveWin(m_x,m_y,m_nWidth,m_nHeight,false);
}

//-----------------------------------------------------------------------------
/**
 * @brief ウィンドウの強制再描画処理です
 * @param	bSendOther	[in] 異なるビューに対しても情報を送信するかどうかを選択します
 */
bool CFootyView::Refresh(bool bSendOther)
{
	if (m_bVisible)						// 再描画は表示しているときのみ
	{
		FOOTY2_PRINTF( L"超！リフレッシュ！ %d\n", m_nFootyID );

		// 雑多処理
		CaretMove();
		SetBars();

		/*// 色分け by Tetr@pod
		for (LinePt Line = m_pDocuments->GetTopLine(); Line != m_pDocuments->GetLastLine(); Line++) {
			m_pDocuments->SetLineInfo2(Line);
		}*/
		if (DrawAllUserFuncs) {
			for (LinePt Line = m_pDocuments->GetTopLine(); Line != m_pDocuments->GetLastLine(); ++Line) {
				m_pDocuments->FlushString2(Line);
				Line->SetEmphasisChached(false);
			}
			DrawAllUserFuncs = false;
			IsUFUpdateTiming = true;
		}

		// 再描画処理をかける
		HDC hDC = GetDC(m_hWnd);		// デバイスコンテキストを取得
		FOOTY2_PRINTF( L"GetDC %d\n", m_nFootyID );
		if ( !hDC )
		{
			FOOTY2_PRINTF( L"But failed...\n", m_nFootyID );
			return false;
		}

		RECT rc;
		GetClientRect( m_hWnd, &rc );

		//FOOTY2_PRINTF( L"RECT %d,%d,%d,%d \n", rc.left, rc.top, rc.right, rc.bottom );
		//FOOTY2_PRINTF( L"描画開始！\n");
		if (!Refresh(hDC,&rc))
		{
			ReleaseDC( m_hWnd, hDC );
			FOOTY2_PRINTF( L"ReleaseDC %d\n", m_nFootyID );
			return false;
		}
		//FOOTY2_PRINTF( L"描画終了！\n");
		ReleaseDC(m_hWnd,hDC);			// デバイスコンテキストを解放
		FOOTY2_PRINTF( L"ReleaseDC %d\n", m_nFootyID );

		// 他のビューへ送信する
		if (bSendOther)
		{
			for ( int i = 0; i < 4; i++ )
			{
				if (i != m_nViewID)
					m_pView[i].Refresh(false);
			}
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定されたデバイスコンテキストへの描画処理
 * @param hDC 描画先デバイスコンテキスト
 * @param rc 描画先矩形
 */
bool CFootyView::Refresh(HDC hDC,const RECT *rc){
	// 宣言
	int yNowPos;									//!< 現在描画中のy座標位置
	CEthicLine cNowLine;							//!< 現在描画中の行を表す
	LineSize nLines = m_pDocuments->GetLineNum();	//!< 編集中の文書に含まれる行の総数
	SelOldInfo stOldInfo;							//!< 選択領域反転のための旧デバイスコンテキスト情報
	HDC hDcBuffer = m_cDoubleBuffering.GetDC();

	// 初期化
	m_cCaret.Hide();
	DrawRectangle(hDcBuffer,0,0,rc->right,rc->bottom,m_pStatus->m_clBackGround);
	m_ImgCtr->Draw(hDcBuffer, 0, 0, rc->right, rc->bottom, m_cDoubleBuffering.GetWidth(), m_cDoubleBuffering.GetHeight() );

	SetBkMode(hDcBuffer,TRANSPARENT);

	// 現在の表示位置からレンダリングしていく
	cNowLine = *m_pDocuments->GetFirstVisible(m_nViewID);
	yNowPos = m_nRulerHeight - (int)(cNowLine.GetEthicNum() *
		(m_nHeightMargin + m_pFonts->GetHeight()));

	forever
	{
		// レンダリング
		RendLine(hDcBuffer,cNowLine.GetLinePointer(),
				 yNowPos,cNowLine.GetLineNum() + 1 == nLines);
		// y座標を変更する
		yNowPos += (int)((m_nHeightMargin + m_pFonts->GetHeight()) *
			(cNowLine.GetLinePointer()->GetEthicLine()));
		if (yNowPos >= rc->bottom)break;
		// 次の行へ
		if (!cNowLine.MoveRealNext(m_pDocuments->GetLineList(),1))
			break;
	}
	// 選択領域の反転
	if (m_pDocuments->IsSelecting()){
		// 情報の初期化
		cNowLine = *m_pDocuments->GetFirstVisible(m_nViewID);
		yNowPos = m_nRulerHeight - (int)(cNowLine.GetEthicNum() *
			(m_nHeightMargin + m_pFonts->GetHeight()));
		// ループの開始
		InitDcForSelect(hDcBuffer,&stOldInfo);
		forever{
			// 選択領域の反転
			RendSelected(hDcBuffer,&cNowLine,yNowPos);						 
			// ｙ座標を変更する
			yNowPos += (int)((m_nHeightMargin + m_pFonts->GetHeight()) *
				cNowLine.GetLinePointer()->GetEthicLine());
			if (yNowPos >= rc->bottom)break;
			// 次の行へ
			if (!cNowLine.MoveRealNext(m_pDocuments->GetLineList(),1))
				break;
		}
		ReleaseDcForSelect(hDcBuffer,&stOldInfo);
	}

	// そのほか付属パーツを描画
	RendLineCount(hDcBuffer,rc);
	RendRuler(hDcBuffer,rc);
	RendBorderLine(hDcBuffer,rc);

	// ワーキングサーフィスからコピーする
	BitBlt(hDC,0,0,m_nWidth,m_nHeight,hDcBuffer,0,0,SRCCOPY);
	m_cDoubleBuffering.SetRendInfo(m_pDocuments->GetFirstVisible(m_nViewID));

	// キャレット下アンダーラインは最後に引く
	RendUnderLine(hDC,rc);
	// 終了処理
	m_cCaret.Show();

	return true;
}

//-----------------------------------------------------------------------------
/** 
 * @brief キャレットが動かされただけのときに呼び出す再描画ルーチンです。
 * @return 成功したときtrue、失敗したときfalse
 * @note 他のビューは再描画しません。
 */
bool CFootyView::CaretRefresh()
{
	if (m_bVisible)					// 再描画は表示しているときのみ
	{
		if (!m_cDoubleBuffering.IsRendered())
		{
			return Refresh(false);
		}
		else
		{
			// 雑多処理
			CaretMove();
			
			// 宣言
			HDC hDC;
			HDC hDcBuffer = m_cDoubleBuffering.GetDC();
			RECT rc;
			
			// 初期化
			hDC = GetDC(m_hWnd);		// デバイスコンテキストを取得
			if (!hDC)return false;
			FOOTY2_PRINTF( L"GetDC %d\n", m_nFootyID );
			GetClientRect( m_hWnd,&rc );// 描画エリア取得
			m_cCaret.Hide();			// キャレット非表示
			
			// 描画開始
			RendLineCount(hDcBuffer,&rc);
			RendRuler(hDcBuffer,&rc);
			RendBorderLine(hDcBuffer,&rc);
			BitBlt(hDC,0,0,m_nWidth,m_nHeight,hDcBuffer,0,0,SRCCOPY);
			RendUnderLine(hDC,&rc);
			
			// デバイスコンテキストを解放
			m_cCaret.Show();			// キャレット表示
			ReleaseDC(m_hWnd,hDC);
			FOOTY2_PRINTF( L"ReleaseDC %d\n", m_nFootyID );
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 上下スクロールされたときに高速再描画を行います。
 * @return 成功したときtrue、失敗したときfalse
 * @note 他のビューは再描画しません
 */
bool CFootyView::ScrollRefresh()
{
	if (m_bVisible)					// 再描画は表示しているときのみ
	{
		// 高速化が不可能でないかチェックする
		if (!m_cDoubleBuffering.IsRendered())
			return Refresh(false);
		
		// どのくらいスクロールされたか調べる
		CEthicLine lnBeforeLast = *m_cDoubleBuffering.GetFirstVisible();
		lnBeforeLast.MoveEthicNext(m_pDocuments->GetLineList(),m_nVisibleLines);
		CEthicLine lnAfterLast = *m_pDocuments->GetFirstVisible(m_nViewID);
		lnAfterLast.MoveEthicNext(m_pDocuments->GetLineList(),m_nVisibleLines);

		int nScrolledNum = m_cDoubleBuffering.GetFirstVisible()->GetDifference
		(
			*m_pDocuments->GetFirstVisible(m_nViewID),
			m_pDocuments->GetLineList()
		);
		
		HDC hDC = m_cDoubleBuffering.GetDC();
		
		// 全くスクロールされてません
		if (nScrolledNum == 0)
		{
			FOOTY2_PRINTF( L"not scrolled\n" );
			return true;
		}
		// 下へスクロールされたとき
		else if (0 < nScrolledNum && nScrolledNum < (int)m_nVisibleLines)
		{
			FOOTY2_PRINTF( L"scroll down\n" );
			CaretMove();
			SetBars();
			BitBlt(hDC,0,0,m_nWidth,m_nHeight,hDC,0,nScrolledNum * (m_pFonts->GetHeight() + m_nHeightMargin),SRCCOPY);
			RenderLines(lnBeforeLast.GetLinePointer(),lnAfterLast.GetLinePointer());
		}
		// 上へスクロールされたとき
		else if ((-(int)m_nVisibleLines) < nScrolledNum && nScrolledNum < 0)
		{
			FOOTY2_PRINTF( L"scroll up\n" );
			CaretMove();
			SetBars();
			BitBlt(hDC,0,0,m_nWidth,m_nHeight,hDC,0,nScrolledNum * (m_pFonts->GetHeight() + m_nHeightMargin),SRCCOPY);
			RenderLines(m_pDocuments->GetFirstVisible(m_nViewID)->GetLinePointer(),m_cDoubleBuffering.GetFirstVisible()->GetLinePointer());
		}
		// 大量すぎる
		else
		{
			FOOTY2_PRINTF( L"scroll too much\n" );
			return Refresh(false);
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定された行間のみを再描画する処理。指定された行はどちらが上でどちらが下でもかまいません。
 * @param pLine1 行1
 * @param pLine2 行2
 * @note 他のビューも再描画します。
 */
bool CFootyView::LineChangedRefresh(LinePt pLine1, LinePt pLine2)
{
	// 開始行と終了行を調べる
	LinePt pStartLine = pLine1->GetRealLineNum() <  pLine2->GetRealLineNum() ? pLine1 : pLine2;
	LinePt pEndLine   = pLine1->GetRealLineNum() >= pLine2->GetRealLineNum() ? pLine1 : pLine2;

	// キャレット位置を適切なものに。スクロールバーは変化しない…はず。
	CaretMove();

	// 全て再描画
	for (int i=0;i<4;i++){
		if (m_pView[i].m_bVisible){
			m_pView[i].RenderLines(pStartLine, pEndLine);
		}
	}

	return true;
}


//-----------------------------------------------------------------------------
/**
 * @brief 指定された行間のみを再描画する処理
 * @param pStartLine 開始行
 * @param pEndLine 終了行
 * @note 他のビューは再描画しません。
 */
bool CFootyView::RenderLines(LinePt pStartLine,LinePt pEndLine)
{
	// 宣言
	int yNowPos;									//!< 現在描画中のy座標位置
	CEthicLine cNowLine;							//!< 現在描画中の行を表す
	LineSize nLines = m_pDocuments->GetLineNum();	//!< 編集中の文書に含まれる行の総数
	SelOldInfo stOldInfo;							//!< 選択領域反転のための旧デバイスコンテキスト情報
	HDC hDcBuffer = m_cDoubleBuffering.GetDC();

	// 初期化
	RECT rc;
	SetRect(&rc,0,0,m_nWidth,m_nHeight);
	m_cCaret.Hide();
	SetBkMode(hDcBuffer,TRANSPARENT);

	// 現在の表示位置からレンダリングしていく
	cNowLine = *m_pDocuments->GetFirstVisible(m_nViewID);
	yNowPos = m_nRulerHeight - (int)(cNowLine.GetEthicNum() *
		(m_nHeightMargin + m_pFonts->GetHeight()));
	forever{
		int yNext = yNowPos + (int)cNowLine.GetLinePointer()->GetEthicLine() * (m_nHeightMargin + m_pFonts->GetHeight());
	
		// レンダリング
		if (pStartLine->GetRealLineNum() <= cNowLine.GetLineNum() &&
			cNowLine.GetLineNum() <= pEndLine->GetRealLineNum()){
			
			DrawRectangle(hDcBuffer,0,yNowPos,rc.right,yNext,m_pStatus->m_clBackGround);
			m_ImgCtr->Draw(hDcBuffer, 0, yNowPos, rc.right, yNext, m_cDoubleBuffering.GetWidth(), m_cDoubleBuffering.GetHeight() );
			RendLine(hDcBuffer,cNowLine.GetLinePointer(),yNowPos,cNowLine.GetLineNum() + 1 == nLines);
		}

		// y座標を変更する
		yNowPos = yNext;
		if (yNowPos >= rc.bottom)break;

		// 次の行へ
		if (!cNowLine.MoveRealNext(m_pDocuments->GetLineList(),1))
			break;
	}
	DrawRectangle(hDcBuffer,0,yNowPos,rc.right,rc.bottom,m_pStatus->m_clBackGround);
	m_ImgCtr->Draw(hDcBuffer, 0, yNowPos, rc.right, rc.bottom, m_cDoubleBuffering.GetWidth(), m_cDoubleBuffering.GetHeight() );

	// 選択領域の反転
	if (m_pDocuments->IsSelecting()){
		// 情報の初期化
		cNowLine = *m_pDocuments->GetFirstVisible(m_nViewID);
		yNowPos = m_nRulerHeight - (int)(cNowLine.GetEthicNum() *
			(m_nHeightMargin + m_pFonts->GetHeight()));
		// ループの開始
		InitDcForSelect(hDcBuffer,&stOldInfo);
		forever{
			// 選択領域の反転
			if (pStartLine->GetRealLineNum() <= cNowLine.GetLineNum() &&
				cNowLine.GetLineNum() <= pEndLine->GetRealLineNum())
				RendSelected(hDcBuffer,&cNowLine,yNowPos);						 
			// ｙ座標を変更する
			yNowPos += (int)((m_nHeightMargin + m_pFonts->GetHeight()) *
				cNowLine.GetLinePointer()->GetEthicLine());
			if (yNowPos >= rc.bottom)break;
			// 次の行へ
			if (!cNowLine.MoveRealNext(m_pDocuments->GetLineList(),1))
				break;
		}
		ReleaseDcForSelect(hDcBuffer,&stOldInfo);
	}

	// そのほか付属パーツを描画
	RendLineCount(hDcBuffer,&rc);
	RendRuler(hDcBuffer,&rc);
	RendBorderLine(hDcBuffer,&rc);

	// ワーキングサーフィスからコピーする
	HDC hDC = GetDC( m_hWnd );
	FOOTY2_PRINTF( L"GetDC %d\n", m_nFootyID );
	BitBlt(hDC,0,0,m_nWidth,m_nHeight,hDcBuffer,0,0,SRCCOPY);
	m_cDoubleBuffering.SetRendInfo(m_pDocuments->GetFirstVisible(m_nViewID));
	RendUnderLine(hDC,&rc);
	ReleaseDC( m_hWnd, hDC );
	FOOTY2_PRINTF( L"ReleaseDC %d\n", m_nFootyID );

	// 終了処理
	m_cCaret.Show();
	return true;
}


//-----------------------------------------------------------------------------
/**
 * @brief 指定された一行分のデータをレンダリングします。
 * @param hDC 描画先デバイスコンテキスト
 * @param pLine 描画する行データ
 * @param x 描画先x座標
 * @param y 描画先y座標
 * @param bEnd この行が最終行であるか？
 */
bool CFootyView::RendLine(HDC hDC,LinePt pLine,int y,bool bEnd)
{
	if (!hDC)return false;

	//! 描画モード
	enum RendMode
	{
		RM_NORMAL,										//!< 通常文字列のレンダリング中
		RM_URL,											//!< URLのレンダリング中
		RM_MAIL,										//!< メールのレンダリング中
		RM_LABEL,										//!< ラベルのレンダリング中 by Tetr@pod
		RM_FUNC,										//!< 命令・関数のレンダリング中 by Tetr@pod
	};
	
	/*宣言*/
	size_t nSize = pLine->GetLineLength();				//!< 文字列の長さ
	const wchar_t *pNowChar = pLine->GetLineData();		//!< 現在描画している文字
	int nColumn = 0;									//!< 現在描画する位置
	CStaticStack<COLORREF,sizeof(int)*8+3> StackColor;	//!< 描画する色のスタック
	RendMode nRendMode = RM_NORMAL;						//!< 現在の描画モード(ノーマル、URL、メール、ラベル、命令・関数)
	CFootyLine::CharSets nCharSets;
	int nStartUrl;										//!< URL、メールアドレス、ラベル、命令・関数の開始x座標

	// 高速化用
	int nFirstVisCol = m_nFirstVisibleColumn;
	int nVisColms = (int)m_nVisibleColumns;
	int nNowEthicLine = 0;
	size_t nLapelColumn = m_pDocuments->GetLapelColumn();
	int nLapelMode = m_pDocuments->GetLapelMode();
	UrlIterator pNowUrl = pLine->GetUrlInfo()->begin();
	UrlIterator pEndUrl = pLine->GetUrlInfo()->end();
	UrlIterator pNowMail = pLine->GetMailInfo()->begin();
	UrlIterator pEndMail = pLine->GetMailInfo()->end();
	UrlIterator pNowLabel = pLine->GetLabelInfo()->begin();// by Tetr@pod
	UrlIterator pEndLabel = pLine->GetLabelInfo()->end();// by Tetr@pod
	UrlIterator pNowFunc = pLine->GetFuncInfo()->begin();// by Tetr@pod
	UrlIterator pEndFunc = pLine->GetFuncInfo()->end();// by Tetr@pod

	// レンダリングのための初期化
	SetTextColor(hDC,m_pStatus->m_clDefaultLetter);
	StackColor.push(m_pStatus->m_clDefaultLetter);
	
	// URL、メール、ラベル用のペンを作成する
	HPEN hPenURL,hPenMail,hPenLabel;// hPenLabel by Tetr@pod
#ifdef UNDER_CE
	hPenMail = CreatePen(PS_SOLID,1,m_pStatus->m_clMailUnder);
	hPenURL = CreatePen(PS_SOLID,1,m_pStatus->m_clUrlUnder);
	hPenLabel = CreatePen(PS_SOLID,1,m_pStatus->m_clLabelUnder);// by Tetr@pod
#else	/*UNDER_CE*/
	hPenMail = CreatePen(PS_DOT,1,m_pStatus->m_clMailUnder);
	hPenURL = CreatePen(PS_DOT,1,m_pStatus->m_clUrlUnder);
	hPenLabel = CreatePen(PS_DOT,1,m_pStatus->m_clLabelUnder);// by Tetr@pod
#endif	/*UNDER_CE*/

	// キャッシュがあるか確認しておく
	if (!pLine->EmphasisChached())
		m_pDocuments->SetChacheCommand(pLine);
	std::vector<CFootyLine::EmpPos>::iterator pNowCommand = pLine->GetColorInfo()->begin();
	std::vector<CFootyLine::EmpPos>::iterator pEndCommand = pLine->GetColorInfo()->end();

	// １行分のデータをレンダリングする
	for (size_t i=0;i<nSize;i++,pNowChar++){
		// 命令・関数？ by Tetr@pod
		if (pNowFunc != pEndFunc){
			if (pNowFunc->m_nStartPos == i){
				nRendMode = RM_FUNC;
				switch (pNowFunc->m_nKind){
				case 0:
					SetTextColor(hDC,m_pStatus->m_clFunc1);
					break;
				case 1:
					SetTextColor(hDC,m_pStatus->m_clFunc2);
					break;
				case 2:
					SetTextColor(hDC,m_pStatus->m_clFunc3);
					break;
				case 3:
					SetTextColor(hDC,m_pStatus->m_clFunc4);
					break;
				case 4:
					SetTextColor(hDC,m_pStatus->m_clFunc5);
					break;
				case 5:
					SetTextColor(hDC,m_pStatus->m_clFunc6);
					break;
				case 6:
					SetTextColor(hDC,m_pStatus->m_clFunc7);
					break;
				case 7:
					SetTextColor(hDC,m_pStatus->m_clFunc8);
					break;
				}
				nStartUrl = GetTextPosX(nColumn);
			}
			if (pNowFunc->m_nEndPos == i){
				nRendMode = RM_NORMAL;
				SetTextColor(hDC,StackColor.top());
				pNowFunc++;
			}
		}
		// ラベル？ by Tetr@pod
		if (pNowLabel != pEndLabel){
			if (nRendMode != RM_FUNC){									// とりあえず命令・関数優先、誤作動防止
				if (pNowLabel->m_nStartPos == i){
					nRendMode = RM_LABEL;
					SetTextColor(hDC,m_pStatus->m_clLabel);
					nStartUrl = GetTextPosX(nColumn);
				}
				if (pNowLabel->m_nEndPos == i){
					nRendMode = RM_NORMAL;
					SetTextColor(hDC,StackColor.top());
					pNowLabel++;
					// アンダーラインの描画
					SelectObject(hDC,hPenLabel);
					MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
					LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
				}
			}
		}
		// URL？
		if (pNowUrl != pEndUrl){
			if (nRendMode != RM_LABEL && nRendMode != RM_FUNC){									// URLよりラベル、命令・関数優先
				if (pNowUrl->m_nStartPos == i){
					nRendMode = RM_URL;
					SetTextColor(hDC,m_pStatus->m_clUrl);
					nStartUrl = GetTextPosX(nColumn);
				}
			}
			if (pNowUrl->m_nEndPos == i){
				nRendMode = RM_NORMAL;
				SetTextColor(hDC,StackColor.top());
				pNowUrl++;
				// アンダーラインの描画
				SelectObject(hDC,hPenURL);
				MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
				LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
			}
		}
		// メールアドレス？
		if (pNowMail != pEndMail){
			if (nRendMode != RM_URL && nRendMode != RM_LABEL && nRendMode != RM_FUNC){			// メールアドレスよりURL、ラベル、命令・関数優先
				if (pNowMail->m_nStartPos == i){
					nRendMode = RM_MAIL;
					SetTextColor(hDC,m_pStatus->m_clMail);
					nStartUrl = GetTextPosX(nColumn);
				}
			}
			if (pNowMail->m_nEndPos == i){
				nRendMode = RM_NORMAL;
				SetTextColor(hDC,StackColor.top());
				pNowMail++;
				// アンダーラインの描画
				SelectObject(hDC,hPenMail);
				MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
				LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
			}
		}
		
		// その位置のコマンドをスタックに積む
		for (;pNowCommand != pEndCommand;pNowCommand++){
			if (pNowCommand->m_nPosition == i){			// 同じ位置
				if (pNowCommand->m_bIsStart){
					if (nRendMode == RM_NORMAL)
						SetTextColor(hDC,pNowCommand->m_Color);
					StackColor.push(pNowCommand->m_Color);
				}
				else{
					StackColor.pop();
					if (nRendMode == RM_NORMAL)
						SetTextColor(hDC,StackColor.top());
				}
			}
			else break;									// 違う位置の時はループを抜ける
		}
		// 文字をレンダリング(描画エリアの中のみ)
		if (*pNowChar == L'\t'){						// タブ文字
			if (nFirstVisCol <= nColumn && nColumn <= nFirstVisCol + (int)nVisColms && ((m_nMarkVisible & EDM_TAB) != 0)){
				SetTextColor(hDC,m_pStatus->m_clTab);
				m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
				ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,L">",1,NULL);
				SetTextColor(hDC,StackColor.top());
			}
			for (nColumn++;;nColumn++){
				if (nColumn % m_pDocuments->GetTabLen() == 0)
					break;
			}
		}
		else{
			// この文字を取得する
			if (CFootyLine::IsSurrogateLead(*pNowChar))
				nCharSets = CFootyLine::GetCharSets(*pNowChar,*(pNowChar+1));
			else
				nCharSets = CFootyLine::GetCharSets(*pNowChar);
			// フォントのセット
			if (nFirstVisCol - 1 <= nColumn && nColumn <= nFirstVisCol + (int)nVisColms){
				switch (nCharSets){
				case CFootyLine::CHARSETS_ANSI_ALPHABET:		// ANSI(アルファベット)
				case CFootyLine::CHARSETS_ANSI_NUMBER:			// ANSI(数字)
				case CFootyLine::CHARSETS_ANSI_SYMBOL:			// ANSI(アンダーバーを除く記号)
				case CFootyLine::CHARSETS_ANSI_UNDER:			// ANSIアンダーバー)
					m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
					break;
				case CFootyLine::CHARSETS_HIRAGANA:				// 日本語(ひらがな)
				case CFootyLine::CHARSETS_KATAKANA:				// 日本語(カタカナ)
				case CFootyLine::CHARSETS_KATAKANA_HALF:		// 日本語(半角カタカナ)					
				case CFootyLine::CHARSETS_FULL_ALPHABET:		// 全角アルファベット
				case CFootyLine::CHARSETS_FULL_NUMBER:			// 全角数字
				case CFootyLine::CHARSETS_AINU_EXTENSION:		// アイヌ語用拡張
				case CFootyLine::CHARSETS_SYMBOLS:				// 記号
				case CFootyLine::CHARSETS_JP_NOBASHI:			// 日本語のばし音
				case CFootyLine::CHARSETS_IPA_EXTENSION:		// IPA拡張
					m_pFonts->UseFont(hDC,FFM_SHIFTJIS_CHARSET);
					break;
				case CFootyLine::CHARSETS_KANJI:				// 漢字
					m_pFonts->UseKanjiFont(hDC);
					break;
				case CFootyLine::CHARSETS_KOREA:				// 韓国語
					m_pFonts->UseFont(hDC,FFM_HANGUL_CHARSET);
					break;
				case CFootyLine::CHARSETS_LATIN:				// ラテン
					
					break;
				case CFootyLine::CHARSETS_BALTIC:				// バルト諸国
					m_pFonts->UseFont(hDC,FFM_BALTIC_CHARSET);
					break;
				case CFootyLine::CHARSETS_ARABIC:				// アラビア
					m_pFonts->UseFont(hDC,FFM_ARABIC_CHARSET);
					break;
				case CFootyLine::CHARSETS_HEBREW:				// ヘブライ
					m_pFonts->UseFont(hDC,FFM_HEBREW_CHARSET);
					break;
				case CFootyLine::CHARSETS_GREEK:				// ギリシャ
					m_pFonts->UseFont(hDC,FFM_GREEK_CHARSET);
					break;
				case CFootyLine::CHARSETS_CYRILLIC:				// キリル文字
					m_pFonts->UseFont(hDC,FFM_RUSSIAN_CHARSET);
					break;
				case CFootyLine::CHARSETS_VIETTNAMESE:			// ベトナム語
					m_pFonts->UseFont(hDC,FFM_VIETNAMESE_CHARSET);
					break;
				case CFootyLine::CHARSETS_THAI:					// タイ語
					m_pFonts->UseFont(hDC,FFM_THAI_CHARSET);
					break;
				default:
					m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
					break;
				}

				// 色セット
				if (*pNowChar == L'　'){
					if ((m_nMarkVisible & EDM_FULL_SPACE) != 0){
						SetTextColor(hDC,m_pStatus->m_clNormalSpace);
						m_pFonts->UseFont(hDC,FFM_SHIFTJIS_CHARSET);
						ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,L"□",1,NULL);
						SetTextColor(hDC,StackColor.top());
					}
				}
				else if (*pNowChar == L' '){
					if ((m_nMarkVisible & EDM_HALF_SPACE) != 0){
						SetTextColor(hDC,m_pStatus->m_clHalfSpace);
						m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
						ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,L"`",1,NULL);
						SetTextColor(hDC,StackColor.top());
					}
				}
				else{
					// サロゲートペアを利用した文字の描画処理
					if (CFootyLine::IsSurrogateLead(*pNowChar) && i != nSize - 1){
						ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,pNowChar,2,NULL);
						i++;pNowChar++;
					}
					// 通常の文字の描画処理
					else{
						//FOOTY2_PRINTF( L"Drawing %s\n", pNowChar );
						ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,pNowChar,1,NULL);
					}
				}
			}
			// もしも描画位置を過ぎていて、サロゲートペアのときはポインタだけ移動する
			else if (CFootyLine::IsSurrogateLead(*pNowChar)){
				i++;pNowChar++;
			}
			
			// x座標を増やす
			if (CFootyLine::IsDualChar(nCharSets))
				nColumn += 2;
			else
				nColumn ++;
		}

		// もうこの位置は描画する位置を終了している？
		if (pLine->GetEthicLine() == nNowEthicLine + 1 &&
			nFirstVisCol + (int)nVisColms + 1 < nColumn)
			break;

		// 次の行へ移動するか？
		if (pLine->IsGoNext(pNowChar,i,nColumn,nLapelColumn,nLapelMode)){
			// URLアンダーラインの後始末
			switch(nRendMode){
			case RM_URL:
				SelectObject(hDC,hPenURL);
				MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
				LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
				nStartUrl = GetTextPosX(0);
				break;
			case RM_MAIL:
				SelectObject(hDC,hPenMail);
				MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
				LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
				nStartUrl = GetTextPosX(0);
				break;
			case RM_LABEL:// by Tetr@pod
				SelectObject(hDC,hPenLabel);
				MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
				LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
				nStartUrl = GetTextPosX(0);
				break;
			}
			// 次の行へ
			nColumn = 0;
			nNowEthicLine++;
			y += m_pFonts->GetHeight() + m_nHeightMargin;
		}
	}

	// URLアンダーラインの後始末
	switch(nRendMode){
	case RM_URL:
		SelectObject(hDC,hPenURL);
		MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
		LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
		break;
	case RM_MAIL:
		SelectObject(hDC,hPenMail);
		MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
		LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
		break;
	case RM_LABEL:// by Tetr@pod
		SelectObject(hDC,hPenLabel);
		MoveToEx(hDC,nStartUrl,y+m_pFonts->GetHeight(),NULL);
		LineTo(hDC,GetTextPosX(nColumn),y+m_pFonts->GetHeight());
		break;
	}
	
	// 行末のマークを描画する
	if (bEnd && ((m_nMarkVisible & EDM_EOF) != 0)){
		m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
		SetTextColor(hDC,m_pStatus->m_clEOF);
		ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,L"[EOF]",5,NULL);
	}
	else if ((m_nMarkVisible & EDM_LINE) != 0){
		m_pFonts->UseFont(hDC,FFM_SHIFTJIS_CHARSET);
		SetTextColor(hDC,m_pStatus->m_clCrlf);
		ExtTextOutW(hDC,GetTextPosX(nColumn),y,0,NULL,L"↓",1,NULL);
	}
	
	// オブジェクト破棄
	DeleteObject(hPenURL);
	DeleteObject(hPenMail);
	DeleteObject(hPenLabel);// by Tetr@pod
	return true;
}

/*----------------------------------------------------------------
CFootyView::InitDcForSelect
選択領域の描画のためにデバイスコンテキスト初期化。
pSelInfoには元の情報を格納するための構造体へのポインタ
----------------------------------------------------------------*/
bool CFootyView::InitDcForSelect(HDC hDC,SelOldInfo *pSelInfo){
	if (!hDC || !pSelInfo)return false;
	/*GDIオブジェクトの作成*/
	pSelInfo->m_hPen = CreatePen(PS_SOLID,1,RGB(255,255,255));
	pSelInfo->m_hBrush = CreateSolidBrush(RGB(255,255,255));
	/*デバイスコンテキストに対して選択処理を行わせる*/
	pSelInfo->m_hPenOld = (HPEN)SelectObject(hDC,pSelInfo->m_hPen);
	pSelInfo->m_hBrushOld = (HBRUSH)SelectObject(hDC,pSelInfo->m_hBrush);
	pSelInfo->m_nRopOld = SetROP2(hDC,m_nRop2);
	return true;
}

/*----------------------------------------------------------------
CFootyView::ReleaseDcForSelect
選択領域描画が終わったら呼び出す、デバイスコンテキストを元に
戻すためのルーチン。pSelInfoには元に情報を入れた構造体へのポインタ
----------------------------------------------------------------*/
bool CFootyView::ReleaseDcForSelect(HDC hDC,CFootyView::SelOldInfo *pSelInfo){
	if (!hDC || !pSelInfo)return false;
	/*元の物を選択させる*/
	SelectObject(hDC,pSelInfo->m_hPenOld);
	SelectObject(hDC,pSelInfo->m_hBrushOld);
	SetROP2(hDC,pSelInfo->m_nRopOld);
	/*オブジェクトを破壊する*/
	DeleteObject(pSelInfo->m_hPen);
	DeleteObject(pSelInfo->m_hBrush);
	return true;
}

/*----------------------------------------------------------------
CFootyView::RendSelected
選択エリアを描画する処理
----------------------------------------------------------------*/
bool CFootyView::RendSelected(HDC hDC,CEthicLine *pLine,int y){
	if (!hDC || !pLine)return false;
	/*宣言*/
	size_t i;														//!< ループカウンタ
	bool bSelCrlf = false;											//!< 改行位置を反転させるかどうか
	CEditPosition *pSelStartPos = m_pDocuments->GetSelStart();		//!< 選択開始位置
	CEditPosition *pSelEndPos = m_pDocuments->GetSelEnd();			//!< 選択終了位置
	size_t nEthicLine = pLine->GetLinePointer()->GetEthicLine();	//!< この行の論理行の数
	size_t nSelStartPosition,nSelEndPosition;						//!< この行における選択桁区間
	CFootyLine::EthicInfo stSelStartInfo,stSelEndInfo;				//!< ↑の論理行情報
	int nRectangleStartX,nRectangleEndX;							//!< 計算用
	
	/*開始絶対位置を設定する*/
	if (*pLine > *pSelStartPos)
		nSelStartPosition = 0;
	else if (*pLine == *pSelStartPos)
		nSelStartPosition = pSelStartPos->GetPosition();
	else return true;
	
	if (*pLine == *pSelEndPos)
		nSelEndPosition = pSelEndPos->GetPosition();
	else if (*pLine < *pSelEndPos){
		nSelEndPosition = pLine->GetLinePointer()->GetLineLength();
		bSelCrlf = true;
	}
	else return true;

	/*ループ用のデータを生成*/
	stSelStartInfo = pLine->GetLinePointer()->CalcEthicLine(nSelStartPosition,
		m_pDocuments->GetLapelColumn(),
		m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode());
	stSelEndInfo = pLine->GetLinePointer()->CalcEthicLine(nSelEndPosition,
		m_pDocuments->GetLapelColumn(),
		m_pDocuments->GetTabLen(),
		m_pDocuments->GetLapelMode());
	nRectangleStartX = (int)(m_nLineCountWidth + 
		(stSelStartInfo.m_nEthicColumn - m_nFirstVisibleColumn) *
		(m_pFonts->GetWidth() + m_nWidthMargin));
	nRectangleEndX = (int)(m_nLineCountWidth +
		(stSelEndInfo.m_nEthicColumn - m_nFirstVisibleColumn) *
		(m_pFonts->GetWidth() + m_nWidthMargin));
	/*ループで回して選択領域を反転させる*/
	for (i=0;i<nEthicLine;i++){
		/*その論理行の状態によって分岐*/
		if (i == stSelStartInfo.m_nEthicLine && i == stSelEndInfo.m_nEthicLine){
			Rectangle(hDC,nRectangleStartX,y,
				nRectangleEndX+(bSelCrlf ? m_pFonts->GetWidth()*2 : 0),
				y+m_pFonts->GetHeight()+m_nHeightMargin);
			break;
		}
		else if (i == stSelStartInfo.m_nEthicLine)
			Rectangle(hDC,nRectangleStartX,y,m_nWidth,
				y+m_pFonts->GetHeight()+m_nHeightMargin);
		else if (i == stSelEndInfo.m_nEthicLine){
			Rectangle(hDC,m_nLineCountWidth,y,
				nRectangleEndX+(bSelCrlf ? m_pFonts->GetWidth()*2 : 0),
				y+m_pFonts->GetHeight()+m_nHeightMargin);
			break;
		}
		else if (stSelStartInfo.m_nEthicLine < i && i < stSelEndInfo.m_nEthicLine)
			Rectangle(hDC,m_nLineCountWidth,y,m_nWidth,
				y+m_pFonts->GetHeight()+m_nHeightMargin);
		/*y座標を調整する*/
		y += m_pFonts->GetHeight() + m_nHeightMargin;
	}
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行番号表示領域を描画する処理です。
 * @param hDC 描画先デバイスコンテキスト
 * @param rc 描画先矩形
 * @return 描画に成功したときtrue
 */
bool CFootyView::RendLineCount(HDC hDC,const RECT *rc){
	if (!hDC || !rc)return false;					// バイスコンテキストが不正
	if (!m_nLineCountWidth)return true;				// 描画する必要がない

	// 宣言
	const size_t TEMP_BUFFER = 20;					//!< バッファサイズ
	wchar_t szTempBuf[TEMP_BUFFER + 1];				//!< 描画する文字列の一時バッファ
	CEthicLine cNowLine = *m_pDocuments->GetFirstVisible(m_nViewID); //!< 描画している位置
	int nPosX,nPosY;								//!< テキスト描画位置
	int nTextLen;									//!< 描画するテキストの長さ
	SIZE sizTextRender;								//!< 描画する範囲
	HDC hDcBitmap = CreateCompatibleDC(hDC);

	// 描画のための初期化
	m_pFonts->UseFont(hDC,FFM_ANSI_CHARSET);
	SetTextColor(hDC,m_pStatus->m_clLineNum);
	
	// 描画エリアを初期化
	DrawRectangle(hDC,0,m_nRulerHeight,m_nLineCountWidth,
				  rc->bottom,m_pStatus->m_clBackGround);	// 行番号表示領域の初期化

	// 現在の行の位置を強調する
	if (m_bIsFocused){
		if (m_nRulerHeight <= m_cCaret.GetNowY() && m_cCaret.GetNowY() < m_nHeight)
			DrawRectangle(hDC,0,m_cCaret.GetNowY(),m_nLineCountWidth-m_nLineCountMargin,
				m_cCaret.GetNowY() + m_pFonts->GetHeight() + m_nHeightMargin,m_pStatus->m_clCaretLine);
	}

	// レンダリング開始
	for (nPosY = m_nRulerHeight;nPosY < rc->bottom;nPosY += m_pFonts->GetHeight() + m_nHeightMargin ){
		// レンダリング処理を行う
		if (!cNowLine.GetEthicNum()){				// 行番号は最初の論理行のときのみ表示
			// 行番号を表示させる
			FOOTY2SPRINTF(szTempBuf, TEMP_BUFFER, L"%d", cNowLine.GetLineNum()+1);
			nTextLen = (int)wcslen(szTempBuf);
			GetTextExtentPoint32W(hDC, szTempBuf, nTextLen, &sizTextRender);
			nPosX = m_nLineCountWidth - m_nLineCountMargin - 2 - sizTextRender.cx;
			ExtTextOutW(hDC,nPosX,nPosY,0,NULL,szTempBuf,nTextLen,NULL);

			// アイコンを表示する
			int nIconLeft = LINEICON_MARGIN;
			int nLineIcons = cNowLine.GetLinePointer()->GetLineIcons();
			for (int nIconIndex = 0;nLineIcons != 0 && nIconIndex < sizeof(int) * 8;nIconIndex++){
				if ((nLineIcons & (1 << nIconIndex)) != 0){
					int nIconTop = nPosY + ( m_pFonts->GetHeight() + m_nHeightMargin ) / 2 - ICON_HEIGHT / 2;
					SelectObject(hDcBitmap,m_bmpIcons[nIconIndex]);
					TransparentBlt(hDC,nIconLeft,nIconTop,ICON_WIDTH,ICON_HEIGHT,
						hDcBitmap,0,0,ICON_WIDTH,ICON_HEIGHT,RGB(255,0,255));
					nIconLeft += ICON_WIDTH + LINEICON_MARGIN;
				}
			}
		}
		// 次の行へ
		if (!cNowLine.MoveEthicNext(m_pDocuments->GetLineList(),1))
			break;									// 次の行に行けなかったときは抜ける
	}

	DeleteDC(hDcBitmap);
	return true;
}

/*----------------------------------------------------------------
CFootyView::RendBorderLine
行番号表示領域とエディタ部のボーダーラインを描画する処理
----------------------------------------------------------------*/
bool CFootyView::RendBorderLine(HDC hDC,const RECT *rc){
	if (!hDC || !rc)return false;
	DrawLine(hDC,m_nLineCountWidth-m_nLineCountMargin,0,
		m_nLineCountWidth-m_nLineCountMargin,rc->bottom,m_pStatus->m_clLineNumLine);
	return true;
}

/*----------------------------------------------------------------
CFootyView::RendUnderLine
キャレット下のアンダーラインを描画するルーチン
----------------------------------------------------------------*/
bool CFootyView::RendUnderLine(HDC hDC,const RECT *rc)
{
#ifndef UNDER_CE
	if (!hDC || !rc)return false;
	if (m_bUnderlineVisible && m_bIsFocused && !m_pDocuments->IsSelecting())
	{
		DrawLine(hDC,m_nLineCountWidth,m_cCaret.GetNowY() + m_pFonts->GetHeight() + 1,
				rc->right,m_cCaret.GetNowY() + m_pFonts->GetHeight() + 1,m_pStatus->m_clUnderLine);
	}
#endif	/*not defined UNDER_CE*/
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行番号表示領域を描画する処理です。
 * @param hDC 描画先デバイスコンテキスト
 * @param rc 描画先矩形
 * @return 成功したときtrueが返る
 */
bool CFootyView::RendRuler(HDC hDC,const RECT *rc)
{
	FOOTY2_ASSERT( hDC );
	FOOTY2_ASSERT( rc );
	
	// 宣言
	size_t i;
	const size_t TEMP_BUFFER = 20;
	wchar_t szWork[TEMP_BUFFER + 1];
	int nPosX;

	// ベースを描画
	DrawRectangle(hDC,0,0,rc->right,m_nRulerHeight,m_pStatus->m_clRulerBk);	// ルーラー背景を描写
	
	// ルーラーを書くためのオブジェクト生成
	HPEN hPen, hOldPen;
	hPen=CreatePen(PS_SOLID,1,m_pStatus->m_clRulerLine);	// ルーラーの罫線を引くためのペン
	SetTextColor(hDC,m_pStatus->m_clRulerText);				// ルーラーのテキストの色
	SetBkMode(hDC,TRANSPARENT);								// 背景を透けさせるよう文字描写
	m_pFonts->UseRulerFont(hDC);
	hOldPen=(HPEN)SelectObject(hDC,hPen);					// ペンをセット
    
	/*ルーラーのキャレットのある箇所を強調*/
	if (m_bIsFocused && m_nLineCountWidth <= m_cCaret.GetNowX() && m_cCaret.GetNowX() < m_nWidth)
		DrawRectangle(hDC,m_cCaret.GetNowX(),0,m_cCaret.GetNowX()+m_pFonts->GetWidth(),
			m_nRulerHeight,m_pStatus->m_clCaretPos);

	/*ラインの描写をする*/
	int nFontWidth = m_pFonts->GetWidth();				// 高速化用
	int nRulerHeight = m_nRulerHeight;					// 高速化用
	int nOtherStartY = nRulerHeight*8/9;				// 高速化用
	int nHalfStartY = nRulerHeight >> 1;				// 高速化用
	int nFirstVisibleCol = m_nFirstVisibleColumn;		// 高速化用
	size_t nLapelCol = m_pDocuments->GetLapelColumn();	// 高速化用
	size_t nVisibleCol = m_nVisibleColumns;				// 高速化用
	for (i=m_nFirstVisibleColumn,nPosX=m_nLineCountWidth;;i++,nPosX+=nFontWidth)
	{
		// ラインの描画
		if (i % 10 == 0)			// 10の倍数(大きなバー)
		{
			MoveToEx(hDC,nPosX,0,NULL);
			LineTo(hDC,nPosX,nRulerHeight);
			FOOTY2SPRINTF(szWork, TEMP_BUFFER, L"%d", i);
			ExtTextOutW(hDC,nPosX+2,nRulerHeight-
				m_pFonts->GetRulerHeight(),0,NULL,szWork,(int)wcslen(szWork),NULL);
		}
		else if (i % 5 == 0)		// 5の倍数(中ぐらいのバー)
		{
			MoveToEx(hDC,nPosX,nHalfStartY,NULL);
			LineTo(hDC,nPosX,nRulerHeight);
		}
		else						// 小さなバー
		{
			MoveToEx(hDC,nPosX,nOtherStartY,NULL);
			LineTo(hDC,nPosX,nRulerHeight);
		}
		// ループを抜ける
		if (i >= nFirstVisibleCol + nVisibleCol + 2)break;
		if (i == nLapelCol)break;
	}

	// ルーラー用オブジェクトの開放
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
	return true;
}


/*----------------------------------------------------------------
CFootyView::DrawRectangle
指定されたデバイスコンテキストハンドルに、長方形を描写します。
この長方形は、中身を塗りつぶします。
----------------------------------------------------------------*/
void CFootyView::DrawRectangle(HDC hDC,int x1,int y1,int x2,int y2,COLORREF color)
{
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	
	hPen = (HPEN)CreatePen(PS_SOLID, 1, color);
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	hBrush = (HBRUSH)CreateSolidBrush(color);
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, x1, y1, x2, y2);

	SelectObject(hDC,hOldPen);
	SelectObject(hDC,hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

/*----------------------------------------------------------------
CFooty::DrawLine
<引数>
hDC         描画先デバイスコンテキスト
x1,y1,x2,y2 描画座標
color       色
<解説>
指定されたデバイスコンテキストハンドルに、線を引きます。
線は実線になります。
----------------------------------------------------------------*/
void CFootyView::DrawLine(HDC hDC,int x1,int y1,int x2,int y2,COLORREF color,int penType)
{
	HPEN hPen,hOldPen;
	hPen = (HPEN)CreatePen(penType,1,color);
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	MoveToEx(hDC,x1,y1,NULL);
	LineTo(hDC,x2,y2);
	SelectObject(hDC,hOldPen);
	DeleteObject(hPen);
}

void CFootyView::SetUnderlineDraw(bool flag){
	m_bUnderlineDraw = flag;
	//m_bUnderlineDraw = true;
}

BOOL CFootyView::ImageLoad(const wchar_t *pFilePath){
	return m_ImgCtr->Load(pFilePath);
}
void CFootyView::ImageClear(void){
	m_ImgCtr->UnLoad();
}
void CFootyView::SetColor(COLORREF color){
	m_ImgCtr->SetBgColor(color);
}

/*[EOF]*/
