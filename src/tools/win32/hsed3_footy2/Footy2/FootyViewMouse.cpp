/**
 * @file FootyViewMouse.cpp
 * @brief Footyのビュー管理を行います。このファイルでマウスの処理を行います。
 */

#include "FootyView.h"
#include "KeyboardState.h"
#include "Cursor.h"
#ifndef WM_MOUSEWHEEL
#	include "zmouse.h"						/*マウスホイールメッセージ*/
#endif	/*WM_MOUSEWHEEL*/

//-----------------------------------------------------------------------------
/**
 * @brief ホイールマウスが動かされたときの処理
 * @param bIsForward 下方向へ回されたかどうか
 */
void CFootyView::OnMouseWheel(bool bIsForward)
{
	if (bIsForward)
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),WHEEL_MOUSE);
	else
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(WHEEL_MOUSE);
	Refresh();
}

//-----------------------------------------------------------------------------
/**
 * @brief 垂直スクロールタイマーのための自動実行処理
 */
void CFootyView::OnAutoVerticalTimer()
{
	if ( 0 < m_nAutoScrollNumVertical )
	{
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),(int)(m_nAutoScrollNumVertical));
	}
	else
	{
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack((int)(-m_nAutoScrollNumVertical));
	}
	OnMouseMove(m_nNowMousePosX,m_nNowMousePosY,true);
	if (m_ImgCtr->IsLoaded()){
		Refresh();// 背景画像が読まれている場合は、残像が残る為すべて再描画する
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 水平スクロールタイマーが実行されたときの処理
 */
void CFootyView::OnAutoHorizontalTimer()
{
	CEditPosition cNowPosition;
	int x = m_nNowMousePosX;
	int y = m_nNowMousePosY;
	if ( x < m_nLineCountWidth )	// 行番号表示領域より左
	{
		StartHorizontalScrollTimer((int)((x - m_nLineCountWidth) / PIXEL_PER_COLUMNSPEED) - 1);
		x = m_nLineCountWidth;
	}
	else if	( m_nWidth < x )		// 行番号表示領域より左
	{
		StartHorizontalScrollTimer((int)((x - m_nWidth) / PIXEL_PER_COLUMNSPEED) + 1);
		x = m_nWidth;
	}
	else
	{
		StopHorizontalScrollTimer();
	}
	
	// マウス位置におけるエディタ上の位置を算出します。ルーラー上だと計算できないので、最低でもルーラー上と仮定
	if (y < m_nRulerHeight) y = m_nRulerHeight;
	CalcInfoFromMouse( x, y, &cNowPosition );

	// 最初に見える桁数を調整します
	int nMax = max( 0, min
	(
		(int)m_pDocuments->GetLapelColumn(),
		(int)(cNowPosition.GetLinePointer()->GetLineLength() - m_nVisibleColumns / 2)
	));
	
	m_nFirstVisibleColumn += m_nAutoScrollNumHorizontal;
	if (m_nFirstVisibleColumn < 0)
	{
		m_nFirstVisibleColumn = 0;
	}
	if ( m_nFirstVisibleColumn > nMax )
	{
		m_nFirstVisibleColumn = nMax;
	}
	FOOTY2_PRINTF( L"OnAutoHorizontalTimer first visible column = %d", m_nFirstVisibleColumn );
	
	// 現在の位置をキャレットに
	m_pDocuments->SetSelectEndNormal(&cNowPosition);
	*m_pDocuments->GetCaretPosition() = cNowPosition;
	
	// 再描画
	Refresh();
}

//-----------------------------------------------------------------------------
/**
 * @brief マウスが押されたときの処理
 * @param x マウス座標x
 * @param y マウス座標y
 */
void CFootyView::OnMouseDown(int x,int y)
{
	/*宣言*/
	CEditPosition cNowPosition;
	CEditPosition cSelStart,cSelEnd;
	CKeyBoardState cKeyStates(m_bShiftLocked);
	bool bSelected = m_pDocuments->IsSelecting();
	
	/*フォーカスを合わせる*/
	::SetFocus(m_hWnd);

	if (y > m_nRulerHeight)
	{
		// 行番号表示領域
		if (x < m_nLineCountWidth)
		{
			CalcLineCount(y,&cSelStart,&cSelEnd);
			m_pDocuments->SetSelectStart(&cSelStart);
			m_pDocuments->SetSelectEndNormal(&cSelEnd);
			m_pDocuments->SetSelLineStart();
			*m_pDocuments->GetCaretPosition() = cSelEnd;
			m_pDocuments->SendMoveCaretCallBack();

			// ドラッグ情報を入れる
			m_nDragMode = DRAGMODE_LINE;
			SetCapture(m_hWnd);				// マウスキャプチャ開始

			// 再描画処理
			Refresh();
		}
		// 通常のエディタ領域
		else 
		{
			// キャレット位置を移動する
			CalcInfoFromMouse(x,y,&cNowPosition);
			if (cKeyStates.IsShiftPushed())	// Shiftキーが押されているときは選択する
			{
				if (m_pDocuments->IsSelecting())
					m_pDocuments->SetSelectEndNormal(&cNowPosition);
				else
				{
					m_pDocuments->SetSelectStart();
					m_pDocuments->SetSelectEndNormal(&cNowPosition);
				}
				bSelected = true;
			}
			else m_pDocuments->SetSelectStart(&cNowPosition);
			*m_pDocuments->GetCaretPosition() = cNowPosition;
			m_pDocuments->SendMoveCaretCallBack();

			// ドラッグ情報を入れる
			m_nDragMode = DRAGMODE_MOUSE;
			SetCapture(m_hWnd);				// マウスキャプチャ開始

			// 位置を再設定しておく
			bool bAdLine = AdjustVisibleLine();
			bool bAdPos = AdjustVisiblePos();

			/*再描画処理*/
			if (bSelected || bAdLine || bAdPos)Refresh();
			else CaretRefresh();
		}
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief マウスが動かされたときに呼び出されます。
 * @param x マウス座標x
 * @param y マウス座標y
 * @param bFromScrollTimer スクロールタイマーから呼ばれたらtrue、ウィンドウメッセージから呼ばれたらfalse
 */
void CFootyView::OnMouseMove(int x, int y, bool bFromScrollTimer)
{
	// 宣言
	CEditPosition cNowPosition;
	CEditPosition cSelStart,cSelEnd;
	
	// 現在のマウス位置を設定
	m_nNowMousePosX = x;
	m_nNowMousePosY = y;
	
	// ドラッグしている情報に応じて分岐
	switch(m_nDragMode)
	{
//ドラッグしていない
	case DRAGMODE_NONE:
		// マウスカーソルを変更する
		if (m_nRulerHeight < y)
		{
			if (m_nLineCountWidth < x)
			{
				if (CalcInfoFromMouse(x,y,&cNowPosition) &&
					IsOnUrl(&cNowPosition,NULL) != ONURL_NONE)
					CCursor::UseUrlCursor();	// URL上
				else CCursor::UseIBeam();		// Iビーム
			}
			else CCursor::UseLineCount();		// 行番号表示領域
		}
		else CCursor::UseArrow();
		break;

//行番号表示領域をドラッグ中
	case DRAGMODE_LINE:
		if (y < m_nRulerHeight)
		{
			StartVerticalScrollTimer( (int)((y - m_nRulerHeight) / PIXEL_PER_LINESPEED) - 1 );
		}
		else if (y > m_nHeight)
		{
			StartVerticalScrollTimer( (int)((y - m_nHeight) / PIXEL_PER_LINESPEED) + 1 );
		}
		else
		{
			StopVerticalScrollTimer();
		}
		
		if (CalcLineCount(y,&cSelStart,&cSelEnd))		// きちんと取得できたときのみ選択
		{
			if (cSelStart < *m_pDocuments->GetLineSelStartA())
			{
				m_pDocuments->SetSelectStart(&cSelStart);
				m_pDocuments->SetSelectEndNormal(m_pDocuments->GetLineSelStartB());
				*m_pDocuments->GetCaretPosition() = cSelStart;
			}
			else 
			{
				m_pDocuments->SetSelectStart(m_pDocuments->GetLineSelStartA());
				m_pDocuments->SetSelectEndNormal(&cSelEnd);
				*m_pDocuments->GetCaretPosition() = cSelEnd;
			}
		}
		m_pDocuments->SendMoveCaretCallBack();

		// 再描画処理
		Refresh();
		m_cCaret.Hide();
		break;

//マウスでエディタ上をドラッグ中
	case DRAGMODE_MOUSE:
		// 水平方向
		if ( x < m_nLineCountWidth )	// 行番号表示領域より左
		{
			StartHorizontalScrollTimer((int)((x - m_nLineCountWidth) / PIXEL_PER_COLUMNSPEED) - 1);
			x = m_nLineCountWidth;
		}
		else if	( m_nWidth < x )		// 行番号表示領域より左
		{
			StartHorizontalScrollTimer((int)((x - m_nWidth) / PIXEL_PER_COLUMNSPEED) + 1);
			x = m_nWidth;
		}
		else
		{
			StopHorizontalScrollTimer();
		}
		// 垂直方向
		if ( y < m_nRulerHeight )		// ルーラーより上
		{
			StartVerticalScrollTimer((int)((y - m_nRulerHeight) / PIXEL_PER_LINESPEED) - 1);
			y = m_nRulerHeight;
		}
		else if ( y > m_nHeight )		// 画面より下
		{
			StartVerticalScrollTimer((int)((y - m_nHeight) / PIXEL_PER_LINESPEED) + 1);
			y = m_nHeight;
		}
		else
		{
			StopVerticalScrollTimer();
		}
		
		CalcInfoFromMouse( x, y, &cNowPosition );
		
		// キャレット位置が移動したときに再描画する
		if (cNowPosition != *m_pDocuments->GetCaretPosition())
		{
			m_pDocuments->SetSelectEndNormal(&cNowPosition);
			
			// 現在の位置をキャレットに
			LinePt pRenderStart = m_pDocuments->GetCaretPosition()->GetLinePointer();
			*m_pDocuments->GetCaretPosition() = cNowPosition;

			// 再描画をかける
			if (bFromScrollTimer)
			{
				ScrollRefresh();
			}
			else
			{
				LineChangedRefresh(pRenderStart,cNowPosition.GetLinePointer());
			}

			// キャレットは非表示に
			m_cCaret.Hide();
			m_pDocuments->SendMoveCaretCallBack();
		}
		break;
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief マウスが離されたときに呼び出されます。
 */
void CFootyView::OnMouseUp(int x,int y)
{
	switch(m_nDragMode)
	{
//マウスでエディタ上をドラッグ中
	case DRAGMODE_MOUSE:
	case DRAGMODE_LINE:
		m_cCaret.Show();
		ReleaseCapture();

		// 自動スクロールをやめる
		StopVerticalScrollTimer();
		StopHorizontalScrollTimer();
		break;
	}
	
	// ドラッグ状態を元に戻す
	m_nDragMode = DRAGMODE_NONE;
}

//-----------------------------------------------------------------------------
/**
 * @brief 垂直スクロールを行うためのタイマーをスタートさせる処理です
 */
void CFootyView::StartVerticalScrollTimer( int nScrollNum )
{
	if ( !m_bAutoScrollVertical )
	{
		m_bAutoScrollVertical = true;
		SetTimer( m_hWnd, eTimer_AutoVertical, SCROLL_TIME, NULL );
	}
	m_nAutoScrollNumVertical = nScrollNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief 水平スクロールを行うためのタイマーをスタートさせる処理です
 */
void CFootyView::StartHorizontalScrollTimer( int nScrollNum )
{
	if ( !m_bAutoScrollHorizontal )
	{
		m_bAutoScrollHorizontal = true;
		SetTimer( m_hWnd, eTimer_AutoHorizontal, SCROLL_TIME, NULL );
	}
	m_nAutoScrollNumHorizontal = nScrollNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief 垂直スクロールタイマーを停止します。
 */
void CFootyView::StopVerticalScrollTimer()
{
	if ( m_bAutoScrollVertical )
	{
		m_bAutoScrollVertical = false;
		KillTimer( m_hWnd, eTimer_AutoVertical );
	}
	m_nAutoScrollNumVertical = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief 水平スクロールタイマーを停止します。
 */
void CFootyView::StopHorizontalScrollTimer()
{
	if ( m_bAutoScrollHorizontal )
	{
		m_bAutoScrollHorizontal = false;
		KillTimer( m_hWnd, eTimer_AutoHorizontal );
	}
	m_nAutoScrollNumHorizontal = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief ダブルクリックしたときに呼び出されます。
 */
void CFootyView::OnMouseDouble(int x,int y)
{
	// 宣言
	CEditPosition cNowPosition;
	CUrlInfo cUrlInfo;
	RetOnUrl nRetURL;
	std::wstring strShellCommand;

	// フォーカスを合わせる
	::SetFocus(m_hWnd);
	
	// 通常のエディタ領域
	if (y > m_nRulerHeight && x > m_nLineCountWidth)
	{
		// キャレット位置を移動する
		CalcInfoFromMouse(x,y,&cNowPosition);
		// クリッカブル？
		nRetURL = IsOnUrl(&cNowPosition,&cUrlInfo);
		if (nRetURL == ONURL_URL)				// URL
		{
			strShellCommand = cNowPosition.GetLinePointer()->
				m_strLineData.substr(cUrlInfo.m_nStartPos,cUrlInfo.GetLength());
			StartApplication(strShellCommand.c_str());
		}
		else if (nRetURL == ONURL_MAIL)			// メールアドレス
		{
			strShellCommand = L"mailto:";
			strShellCommand += cNowPosition.GetLinePointer()->
				m_strLineData.substr(cUrlInfo.m_nStartPos,cUrlInfo.GetLength());
			StartApplication(strShellCommand.c_str());
		}
		else if (nRetURL == ONURL_LABEL)			// ラベル
		{
			FOOTY2_PRINTF( L"ラベルダブルクリック\n");
			// ラベル探索
			wchar_t lname[256];
			wchar_t *buffer;
			wchar_t *wp;
			int len;
			int line;

			len = Footy2GetTextLengthW(m_nFootyID, LM_CRLF);
			if(len >= 0){
				buffer = (wchar_t *) calloc( len + 2 /* 文字列終端と番兵 */, sizeof(wchar_t) );
				Footy2GetTextW(m_nFootyID, buffer, LM_CRLF, len + 1);

				line = 1;
				wp = buffer;

				bool FoundLabel = false;// 目的のラベルが見つかったか

				for(; *wp; line++, wp++) {
					// 1行読み込み
					for(int LabelPosX = 0; *wp && 0x0d != *wp; wp++) {
						LabelPosX++;// by Tetr@pod
						// 先頭の空白を無視
						if( L' ' == *wp || L'\t' == *wp ) {
							continue;
						}
						// ラベル読み込み
						if( L'*' == *wp ) {
							wchar_t *pa = wp;
							int namelen;
							for(pa++;*pa &&
									(L':' > (unsigned wchar_t)*pa || L'>' < (unsigned wchar_t)*pa) &&
									L'/' < (unsigned wchar_t)*pa &&
									L' ' != *pa && '\t' != *pa && 
									0x0d != *pa; pa++) { }
							namelen = (int)(pa - wp);
							if( 1 < namelen) {// ラベル発見
								wcsncpy(lname, wp, namelen);
								lname[namelen] = '\0';
								// 比較
								if (_wcsicmp(cNowPosition.GetLinePointer()->m_strLineData.substr(cUrlInfo.m_nStartPos,cUrlInfo.GetLength()).c_str(), lname) == 0) {// by Tetr@pod
									// キャレット移動
									Footy2SetCaretPosition(m_nFootyID, line - 1, LabelPosX - 1, true);
									// Footy2Refresh(m_nFootyID);// いらないっぽい

									FoundLabel = true;// forの多重ループなのでフラグを使って抜ける

									break;// 本当はgotoで抜けたほうがいい？
								}
							}
							wp = pa;
							if( 0x0d == *wp ) {
								break;
							}
						}
						// コメント読み飛ばし
						if( L';' == *wp || (L'/' == *wp && L'/' == wp[1]) ) {
							// 行末まで読み飛ばし
							for(wp++; *wp && 0x0d != *wp ; wp++);
							break;
						}
						if( L'/' == *wp && L'*' == wp[1] ) {
							for(wp++; *wp ; wp++) {
								if( L'*' == *wp && L'/' == wp[1] ) {
									wp++;
									break;
								}
								if( 0x0d == *wp ) {
									line++;
								}
							}
							continue;
						}
						// 次のステートメントまで読み飛ばし
						for(bool bEscape = false;
							*wp && 0x0d != *wp && L':' != *wp &&
							L';' != *wp && (L'/' != *wp || L'*' != wp[1]) && (L'/' != *wp || L'/' != wp[1]); wp++)
						{
							if( L'\"' == *wp && !bEscape ) {
								bool bDameChk = false;
								// 文字列読み飛ばし
								if( L'{' == *(wp - 1) ) { // 複数行文字列
									wp += 2;
									for(bool bEscape = false; *wp && (L'\"' != *wp || L'}' != wp[1] || bEscape); ) {
										if( L'\\' == *wp ) {
											bEscape = !bEscape;
										}
										wp++;
									}
								} else {
									wp++;
									for(bool bEscape = false; *wp && (L'\"' != *wp || bEscape) && 0x0d != *wp; ) {
										if( L'\\' == *wp ) {
											bEscape = !bEscape;
										}
										wp++;
									}
								}
								wp--;
								continue;
							}
						}
						if( 0x0d == *wp ) {
							break;
						}
						if( L':' != *wp ) {
							wp--;
						}
					}

					if (FoundLabel) {// 目的のラベルが見つかったなら
						break;// 抜ける
					}

					if( 0x0d == *wp && 0x0a == wp[1] ) {
						wp++;
					}
				}
				free (buffer) ;
			}
		}
		else if (nRetURL == ONURL_FUNC)			// 関数
		{
			/* 未実装。というか、アイディアがない */
		}
		else									// 何もない(単語の選択処理)
		{
			m_pDocuments->SelectWord();
			Refresh();
		}
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief アプリケーションをShellExecuteExで実行する処理
 */
void CFootyView::StartApplication(const wchar_t *szFileName)
{
	SHELLEXECUTEINFO sei;
	memset(&sei,0,sizeof(SHELLEXECUTEINFO));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.nShow = SW_SHOWNORMAL;
	sei.lpVerb = L"open";
	sei.lpFile = szFileName;
	ShellExecuteEx(&sei);
}

//-----------------------------------------------------------------------------
/**
 * @brief エディタ上におけるポジションを割り出す関数です。
 * @param	x	[in] マウスのx座標
 * @param	y	[in] マウスのy座標
 * @return 範囲を超えているときはfalseが返りますが、xがエディタ内にありかつ
 *         yが最終行を超えているときはfalseを返しつつポジションも割り出されます。
 */
bool CFootyView::CalcInfoFromMouse(int x,int y,CEditPosition *pPos)
{
	FOOTY2_ASSERT( pPos );
	
	// 宣言
	size_t nEthicColumn = 0;
	size_t nPosition;
	size_t nLineFromTop;
	CEthicLine cMouseLine;

	// 適合範囲外
	if (x < m_nLineCountWidth) return false;
	if (y < m_nRulerHeight) return false;

	// 計算する
	nLineFromTop = (size_t)
		(y - m_nRulerHeight) / (m_pFonts->GetHeight() + m_nHeightMargin);
	nEthicColumn = (size_t)m_nFirstVisibleColumn + 
		(x - m_nLineCountWidth + (m_pFonts->GetWidth() + m_nWidthMargin) / 2)
		/ (m_pFonts->GetWidth() + m_nWidthMargin);

	// そこからポジションの割り出し
	cMouseLine = *m_pDocuments->GetFirstVisible(m_nViewID);
	if (cMouseLine.MoveEthicNext(m_pDocuments->GetLineList(),nLineFromTop))
	{
		nPosition = cMouseLine.GetLinePointer()->CalcRealPosition
			(cMouseLine.GetEthicNum(),nEthicColumn,
			 m_pDocuments->GetLapelColumn(),
			 m_pDocuments->GetTabLen(),
			 m_pDocuments->GetLapelMode());
		pPos->SetPosition(cMouseLine.GetLinePointer(),nPosition);
		return true;
	}
	else		// 最終行超えてる
	{
		LinePt pLast = m_pDocuments->GetLastLine();
		nPosition = pLast->CalcRealPosition
			(pLast->GetEthicLine() - 1,nEthicColumn,
			 m_pDocuments->GetLapelColumn(),
			 m_pDocuments->GetTabLen(),
			 m_pDocuments->GetLapelMode());
		pPos->SetPosition(pLast,nPosition);
		return false;
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 行番号表示領域をクリックされたと仮定してy座標から位置を割り出すルーチンです。
 */
bool CFootyView::CalcLineCount(int y,CEditPosition *pStart,CEditPosition *pEnd)
{
	// 宣言
	size_t nPosition;
	size_t nLineFromTop;
	CEthicLine cMouseLine;
	LinePt pLine;
	// 適合範囲外
	if (!pStart || !pEnd)return false;
	
	// 行位置の計算
	cMouseLine = *m_pDocuments->GetFirstVisible(m_nViewID);
	if (y < m_nRulerHeight)
	{
		nLineFromTop = (size_t)
			(m_nRulerHeight - y) / (m_pFonts->GetHeight() + m_nHeightMargin);
		if (!cMouseLine.MoveEthicBack(nLineFromTop))
			return false;
	}
	else
	{
		nLineFromTop = (size_t)
			(y - m_nRulerHeight) / (m_pFonts->GetHeight() + m_nHeightMargin);
		cMouseLine.MoveEthicNext(m_pDocuments->GetLineList(),nLineFromTop);
	}
	pLine = cMouseLine.GetLinePointer();

	// 開始位置割り出し
	nPosition = pLine->CalcRealPosition
		(cMouseLine.GetEthicNum(),0,
			m_pDocuments->GetLapelColumn(),
			m_pDocuments->GetTabLen(),
			m_pDocuments->GetLapelMode());
	pStart->SetPosition(pLine,nPosition);
	// 終了位置割り出し
	nPosition = pLine->CalcRealPosition
		(cMouseLine.GetEthicNum() + 1,0,
			m_pDocuments->GetLapelColumn(),
			m_pDocuments->GetTabLen(),
			m_pDocuments->GetLapelMode());
	//if (nPosition == pLine->GetLineLength())
	//{
	//	if (cMouseLine.MoveRealNext(m_pDocuments->GetLineList(),1))
	//		nPosition = 0;
	//}
	pEnd->SetPosition(cMouseLine.GetLinePointer(),nPosition);
	return true;
}


//-----------------------------------------------------------------------------
/**
 * @brief ポジションクラスがURL上にあるかどうかを判断します。pInfoには値が
 *        入ります。pInfoにはその情報が入ります(NULL可)
 */
CFootyView::RetOnUrl CFootyView::IsOnUrl(CEditPosition *pPos,CUrlInfo *pInfo)
{
	// 宣言
	UrlIterator pUrlIterator;
	UrlIterator pEndIterator;
	
	// URLの判断
	pUrlIterator = pPos->GetLinePointer()->GetUrlInfo()->begin();
	pEndIterator = pPos->GetLinePointer()->GetUrlInfo()->end();
	
	for (;pUrlIterator!=pEndIterator;pUrlIterator++)
	{
		if (pUrlIterator->IsInPosition(pPos->GetPosition()))
		{
			if (pInfo)*pInfo = *pUrlIterator;
			return ONURL_URL;
		}
	}
	
	/*メールの判断*/
	pUrlIterator = pPos->GetLinePointer()->GetMailInfo()->begin();
	pEndIterator = pPos->GetLinePointer()->GetMailInfo()->end();
	
	for (;pUrlIterator!=pEndIterator;pUrlIterator++)
	{
		if (pUrlIterator->IsInPosition(pPos->GetPosition()))
		{
			if (pInfo)*pInfo = *pUrlIterator;
			return ONURL_MAIL;
		}
	}

	/*ラベルの判断 by Tetr@pod*/
	FOOTY2_PRINTF(L"ラベルなのか\n");
	pUrlIterator = pPos->GetLinePointer()->GetLabelInfo()->begin();
	pEndIterator = pPos->GetLinePointer()->GetLabelInfo()->end();
	
	for (;pUrlIterator!=pEndIterator;pUrlIterator++)
	{
		if (pUrlIterator->IsInPosition(pPos->GetPosition()))
		{
			if (pInfo)*pInfo = *pUrlIterator;
			FOOTY2_PRINTF(L"ラベルだった\n");
			return ONURL_LABEL;
		}
	}

	/*命令・関数の判断 by Tetr@pod*/
	FOOTY2_PRINTF(L"命令・関数なのか\n");
	pUrlIterator = pPos->GetLinePointer()->GetFuncInfo()->begin();
	pEndIterator = pPos->GetLinePointer()->GetFuncInfo()->end();
	
	for (;pUrlIterator!=pEndIterator;pUrlIterator++)
	{
		if (pUrlIterator->IsInPosition(pPos->GetPosition()))
		{
			if (pInfo)*pInfo = *pUrlIterator;
			FOOTY2_PRINTF(L"命令・関数だった\n");
			return ONURL_NONE;// 関数は処理がないので、混乱させないようIビームで表示させる
			// return ONURL_FUNC;
		}
	}

	return ONURL_NONE;
}

/*[EOF]*/
