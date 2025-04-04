/**
 * @file FootyViewKey.cpp
 * @brief このファイルでキーボードメッセージからの処理を行います。
 * @author Shinji Watanabe
 * @date Nov.08.2008
 */

#include "FootyView.h"
#include "KeyboardState.h"

//-----------------------------------------------------------------------------
/**
 * CFootyView::OnChar
 * @brief 文字が打たれたとき、ドキュメントを更新して再描画する
 * @param c 入力された文字一文字
 * @note WM_CHARから呼ばれます。
 */
void CFootyView::OnChar(wchar_t c)
{
	// WM_IME_COMPOSITIONの分だけ、キャンセル
	if (m_nIgnoreKey > 0){
		m_nIgnoreKey -= sizeof(wchar_t);
		return;
	}

	// 正当な文字列かチェック
	if ((0x20 <= c && c <= 0x7e) ||
		(0xa1 <= c && c <= 0xdf)){

		// 一文字挿入するルーチンを呼び出す
		CFootyDoc::RedrawType nNeedRedraw = m_pDocuments->InsertChar(c);

		// キャレット位置から表示位置を設定
		size_t nBeforeFirstVisibleColumn = m_nFirstVisibleColumn;
		CEthicLine cBeforeFirstVisibleLine = *m_pDocuments->GetFirstVisible(m_nViewID);

		AdjustVisibleLine();
		AdjustVisiblePos();

		// フラグを調整する
		if (nBeforeFirstVisibleColumn != m_nFirstVisibleColumn ||
			*m_pDocuments->GetFirstVisible(m_nViewID) != cBeforeFirstVisibleLine)
			nNeedRedraw = CFootyDoc::REDRAW_ALL;

		// 再描画
		if (nNeedRedraw == CFootyDoc::REDRAW_ALL)
			Refresh();
		else
			LineChangedRefresh();

		// イベントを通知する
		m_pDocuments->SendTextModified(MODIFIED_CAUSE_CHAR);
	}
	
}

//-----------------------------------------------------------------------------
/**
 * CFootyView::OnImeComposition
 * @brief IMEのメッセージが送られてきたときの処理
 * @param lParam メッセージのLPARAMをそのまま引き継ぐ
 */
void CFootyView::OnImeComposition(LPARAM lParam)
{
	if (lParam & GCS_RESULTSTR) 										// 変換候補が確定されたとき
	{
		// 宣言
		unsigned long dwSize;											// IMEの変換文字列長さ
		HIMC hImc = ImmGetContext(m_hWnd);								// IMEのハンドル
		
		// IME変換文字列を取得する
		if (!hImc)return;
		dwSize = ImmGetCompositionStringW(hImc,GCS_RESULTSTR,NULL,0);	// 確定文字列の長さを取得
		std::wstring strImeString;
		strImeString.resize(dwSize/sizeof(wchar_t));
		ImmGetCompositionStringW(hImc,GCS_RESULTSTR,(LPVOID)strImeString.c_str(),dwSize);
		
		// 取得した文字列を挿入する
		CFootyDoc::RedrawType nNeedRedraw = m_pDocuments->InsertString(strImeString.c_str(),true,true);

		// キャンセル量を代入
		m_nIgnoreKey += dwSize;											// OnCharでキャンセルする量

		// キャレット位置から表示位置を設定
		size_t nBeforeFirstVisibleColumn = m_nFirstVisibleColumn;
		CEthicLine cBeforeFirstVisibleLine = *m_pDocuments->GetFirstVisible(m_nViewID);

		AdjustVisibleLine();
		AdjustVisiblePos();

		// フラグを調整する
		if (nBeforeFirstVisibleColumn != m_nFirstVisibleColumn ||
			*m_pDocuments->GetFirstVisible(m_nViewID) != cBeforeFirstVisibleLine)
			nNeedRedraw = CFootyDoc::REDRAW_ALL;

		// 再描画
		if (nNeedRedraw == CFootyDoc::REDRAW_ALL)
			Refresh();
		else
			LineChangedRefresh();

		// イベントを通知する
		m_pDocuments->SendTextModified(MODIFIED_CAUSE_IME);
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief IMEリクエストがあったときの処理
 * @param wParam メッセージのWPARAMを引き継ぐ
 * @param lParam メッセージのLPARAMを引き継ぐ
 * @return メッセージが返すべきLRESULT値
 */
LRESULT CFootyView::OnImeRequest(WPARAM wParam,LPARAM lParam)
{
	LRESULT lReturn = false;
	static int nTextLength = 0;
	if (wParam == IMR_RECONVERTSTRING && m_pDocuments->IsSelecting()){		// 再変換要求
		RECONVERTSTRING* pReConvStr = (RECONVERTSTRING*)lParam;
		std::wstring wstrSelText;
		m_pDocuments->GetSelText(&wstrSelText);
		if (!pReConvStr){													// 初回呼び出し
			nTextLength = WideCharToMultiByte(CP_ACP,0,wstrSelText.c_str(),
				-1,NULL,0,NULL,NULL) - 1;									// NULL文字バッファも含むので引く
		}
		else{																// 二回目の呼び出し
			// RECONVERTSTRING構造体を埋める
			LPSTR lpsz = (LPSTR)pReConvStr + sizeof(RECONVERTSTRING);
			WideCharToMultiByte(CP_ACP,0,wstrSelText.c_str(),
				-1,lpsz,nTextLength,NULL,NULL);
			pReConvStr->dwStrLen = nTextLength;
			pReConvStr->dwStrOffset = sizeof(RECONVERTSTRING);
			pReConvStr->dwTargetStrOffset = 0;
			pReConvStr->dwTargetStrLen = nTextLength;
			pReConvStr->dwCompStrOffset = 0;
			pReConvStr->dwCompStrLen = nTextLength;
		}
		lReturn = sizeof(RECONVERTSTRING) + nTextLength;
	}
	return lReturn;
}



#define START_MOVEKEY	nNeedRedraw = CFootyDoc::REDRAW_CURMOVE;			\
						if (cKeyStates.IsShiftPushed()){					\
							if (!m_pDocuments->IsSelecting())				\
								m_pDocuments->SetSelectStart();				\
						}													\
						else{												\
							if (m_pDocuments->IsSelecting()){				\
								m_pDocuments->UnSelect();					\
								nNeedRedraw = CFootyDoc::REDRAW_ALL;		\
							}												\
						}

#define END_MOVEKEY		if (cKeyStates.IsShiftPushed()){					\
							m_pDocuments->SetSelectEndNormal();				\
							nNeedRedraw = CFootyDoc::REDRAW_SELCHANGED;		\
						}													\
						m_pDocuments->SendMoveCaretCallBack();

	

//-----------------------------------------------------------------------------
/**
 * @brief キーが押されたときに呼ばれます。
 * @param nKeyCode メッセージから呼び出されたときのキーコードが入ります
 * @return この関数が処理を行うときにはtrueが、そうでないときはfalseが返ります
 */
bool CFootyView::OnKeyDown(int nKeyCode)
{
	// 宣言
	size_t i;
	bool bDontAdjustLine = false;
	CFootyDoc::RedrawType nNeedRedraw = CFootyDoc::REDRAW_FAILED;
	CFootyLine::EthicInfo stEthicInfo;
	size_t nPosition;
	CKeyBoardState cKeyStates(m_bShiftLocked);
	LinePt pLineBefore = m_pDocuments->GetCaretPosition()->GetLinePointer();
	size_t nPosBefore = m_pDocuments->GetCaretPosition()->GetPosition();
	size_t nBeforeFirstVisibleColumn = m_nFirstVisibleColumn;
	CEthicLine cBeforeFirstVisibleLine = *m_pDocuments->GetFirstVisible(m_nViewID);
	
	// キーによって場合わけ
	switch(nKeyCode)
	{

//上のキー
	case VK_UP:
		if (cKeyStates.IsControlPushed()){
			m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(1);
			bDontAdjustLine = true;
		}
		else
		{
			START_MOVEKEY
			// 表示位置を更新する
			if (*m_pDocuments->GetFirstVisible(m_nViewID) == *m_pDocuments->GetCaretPosition())
			{
				stEthicInfo = m_pDocuments->GetCaretPosition()->GetLinePointer()->CalcEthicLine
					(m_pDocuments->GetCaretPosition()->GetPosition(),m_pDocuments->GetLapelColumn(),
					m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode());
				m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(1);
			}
			// キャレットを移動させる
			m_pDocuments->GetCaretPosition()->MoveLineBackwardEthic(m_pDocuments->GetLineList(),1,
				m_pDocuments->GetLapelColumn(),
				m_pDocuments->GetTabLen(),
				m_pDocuments->GetLapelMode());
			END_MOVEKEY
				if (m_ImgCtr->IsLoaded()){
					nNeedRedraw = CFootyDoc::REDRAW_ALL;// 背景画像が読まれている場合は、残像が残る為すべて再描画する
				}
		}
		break;

//下のキー
	case VK_DOWN:
		if (cKeyStates.IsControlPushed()){
			m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),1);
			bDontAdjustLine = true;
		}
		else{
			START_MOVEKEY
			// キャレットを移動させる
			m_pDocuments->GetCaretPosition()->MoveLineForwardEthic(m_pDocuments->GetLineList(),1,
				m_pDocuments->GetLapelColumn(),
				m_pDocuments->GetTabLen(),
				m_pDocuments->GetLapelMode());
			// 表示位置を更新する
			stEthicInfo = m_pDocuments->GetCaretPosition()->GetLinePointer()->CalcEthicLine
				(m_pDocuments->GetCaretPosition()->GetPosition(),m_pDocuments->GetLapelColumn(),
				m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode());
			if (m_pDocuments->GetFirstVisible(m_nViewID)->GetLinePointer()->GetOffset() + 
				m_pDocuments->GetFirstVisible(m_nViewID)->GetEthicNum() + m_nVisibleLines ==
				m_pDocuments->GetCaretPosition()->GetLinePointer()->GetOffset() + stEthicInfo.m_nEthicLine + 1)
				m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),1);
			END_MOVEKEY
				if (m_ImgCtr->IsLoaded()){
					nNeedRedraw = CFootyDoc::REDRAW_ALL;// 背景画像が読まれている場合は、残像が残る為すべて再描画する
				}
		}
		break;

//右のキー
	case VK_RIGHT:
		START_MOVEKEY
		// 位置を移動させる
		if (cKeyStates.IsControlPushed())
			m_pDocuments->MoveWordForward();
		else{
			// サロゲートペアかどうかチェックする
			CEditPosition *pCaretPos = m_pDocuments->GetCaretPosition();
			if (CFootyLine::IsSurrogateLead(pCaretPos->GetLinePointer()->GetLineData()[pCaretPos->GetPosition()]))
				pCaretPos->MoveColumnForward(m_pDocuments->GetLineList(),2);
			else
				pCaretPos->MoveColumnForward(m_pDocuments->GetLineList(),1);
		}
		// 表示位置を更新する
		stEthicInfo = m_pDocuments->GetCaretPosition()->GetLinePointer()->CalcEthicLine
			(m_pDocuments->GetCaretPosition()->GetPosition(),m_pDocuments->GetLapelColumn(),
			m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode());
		if (stEthicInfo.m_nEthicColumn == m_nFirstVisibleColumn + m_nVisibleColumns)
			m_nFirstVisibleColumn ++;
		if (m_pDocuments->GetFirstVisible(m_nViewID)->GetLinePointer()->GetOffset() + 
			m_pDocuments->GetFirstVisible(m_nViewID)->GetEthicNum() + m_nVisibleLines ==
			m_pDocuments->GetCaretPosition()->GetLinePointer()->GetOffset() + stEthicInfo.m_nEthicLine + 1)
			m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),1);
		END_MOVEKEY
		if (m_ImgCtr->IsLoaded()){
			nNeedRedraw = CFootyDoc::REDRAW_ALL;// 背景画像が読まれている場合は、残像が残る為すべて再描画する
		}
		break;

//左のキー
	case VK_LEFT:
		START_MOVEKEY
		// 表示位置を更新する
		stEthicInfo = m_pDocuments->GetCaretPosition()->GetLinePointer()->CalcEthicLine
			(m_pDocuments->GetCaretPosition()->GetPosition(),m_pDocuments->GetLapelColumn(),
			m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode());
		if (stEthicInfo.m_nEthicColumn != 0 &&
			stEthicInfo.m_nEthicColumn == m_nFirstVisibleColumn)
			m_nFirstVisibleColumn --;
		// 位置を移動させる
		if (cKeyStates.IsControlPushed())
			m_pDocuments->MoveWordBack();
		else{
			// サロゲートペアかどうかチェックする
			CEditPosition *pCaretPos = m_pDocuments->GetCaretPosition();
			if (CFootyLine::IsSurrogateTail(pCaretPos->GetLinePointer()->GetLineData()[pCaretPos->GetPosition()-1]))
				pCaretPos->MoveColumnBackward(m_pDocuments->GetLineList(),2);
			else
				pCaretPos->MoveColumnBackward(m_pDocuments->GetLineList(),1);
		}
		END_MOVEKEY
		if (m_ImgCtr->IsLoaded()){
			nNeedRedraw = CFootyDoc::REDRAW_ALL;// 背景画像が読まれている場合は、残像が残る為すべて再描画する
		}
		break;

//ページダウンキー
	case VK_NEXT:
		START_MOVEKEY
		// 表示位置を更新する
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),m_nVisibleLines);
		// カーソルも移動する
		for (i=0;i<m_nVisibleLines;i++){
			m_pDocuments->GetCaretPosition()->MoveLineForwardEthic(m_pDocuments->GetLineList(),1,
				m_pDocuments->GetLapelColumn(),
				m_pDocuments->GetTabLen(),
				m_pDocuments->GetLapelMode());
		}
		END_MOVEKEY
		nNeedRedraw = CFootyDoc::REDRAW_ALL;
		break;

//ページアップキー
	case VK_PRIOR:
		START_MOVEKEY
		// 表示位置を更新する
		m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicBack(m_nVisibleLines);
		// カーソルも移動する
		for (i=0;i<m_nVisibleLines;i++){
			m_pDocuments->GetCaretPosition()->MoveLineBackwardEthic(m_pDocuments->GetLineList(),1,
				m_pDocuments->GetLapelColumn(),
				m_pDocuments->GetTabLen(),
				m_pDocuments->GetLapelMode());
		}
		END_MOVEKEY
		nNeedRedraw = CFootyDoc::REDRAW_ALL;
		break;

//Homeキーが押されたとき
	case VK_HOME:
		START_MOVEKEY
		if (cKeyStates.IsControlPushed())	// Ctrlキー押しているので最初へ
		{
			m_pDocuments->GetCaretPosition()->SetPosition(m_pDocuments->GetLineList(), 0, 0 );
		}
		else								// Ctrlキーを押していない
		{
			stEthicInfo = pLineBefore->CalcEthicLine( nPosBefore, m_pDocuments->GetLapelColumn(), m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode() );

			if ( m_bAutoIndentMode && stEthicInfo.m_nEthicLine == 0)
			{
				size_t nIndentPos = pLineBefore->CalcAutoIndentPos( pLineBefore->GetLineLength() );
				if ( nIndentPos == nPosBefore )
				{
					nPosition = 0;
				}
				else
				{
					nPosition = nIndentPos;
				}
			}
			else
			{
				nPosition = pLineBefore->CalcRealPosition(stEthicInfo.m_nEthicLine,0,m_pDocuments->GetLapelColumn(), m_pDocuments->GetTabLen(), m_pDocuments->GetLapelMode() );
			}

			m_pDocuments->GetCaretPosition()->SetPosition( pLineBefore, nPosition);
		}
		END_MOVEKEY
		break;

//Endキーが押されたとき
	case VK_END:
		START_MOVEKEY
		if (cKeyStates.IsControlPushed())	// Ctrlキーを押しているので最後へ
		{
			LinePt pLastLine = m_pDocuments->GetLastLine();
			m_pDocuments->GetCaretPosition()->SetPosition( pLastLine, pLastLine->GetLineLength() );
		}
		else								// Ctrlキーを押していない
		{
			stEthicInfo = pLineBefore->CalcEthicLine( nPosBefore, m_pDocuments->GetLapelColumn(), m_pDocuments->GetTabLen(),m_pDocuments->GetLapelMode() );
			nPosition = pLineBefore->CalcRealPosition( stEthicInfo.m_nEthicLine, m_pDocuments->GetLapelColumn(), m_pDocuments->GetLapelColumn(), m_pDocuments->GetTabLen(), m_pDocuments->GetLapelMode() );
			m_pDocuments->GetCaretPosition()->SetPosition( pLineBefore, nPosition );
		}
		END_MOVEKEY
		break;

//タブキー
	case VK_TAB:
		if (m_pDocuments->IsSelecting() &&
			m_pDocuments->GetSelStart()->GetLinePointer() != 
			m_pDocuments->GetSelEnd()->GetLinePointer()){
			if (cKeyStates.IsShiftPushed())
				m_pDocuments->Unindent();
			else
				m_pDocuments->Indent();
			nNeedRedraw = CFootyDoc::REDRAW_ALL;
		}
		else{
			nNeedRedraw = m_pDocuments->InsertChar(L'\t');
			m_pDocuments->SendTextModified(MODIFIED_CAUSE_TAB);
		}
		
		break;

//バックスペース
	case VK_BACK:
		nNeedRedraw = m_pDocuments->OnBackSpace();
		m_pDocuments->SendTextModified(MODIFIED_CAUSE_BACKSPACE);		
		break;

//Delキー
	case VK_DELETE:
		nNeedRedraw = m_pDocuments->OnDelete();
		m_pDocuments->SendTextModified(MODIFIED_CAUSE_DELETE);
		break;

//Insertキー
	case VK_INSERT:
		/*キャレットは変えるが再描画はしない*/
		m_pDocuments->SetInsertMode(!m_pDocuments->IsInsertMode());
		CaretMove();
		break;

//Enterキーが押されたとき
	case VK_RETURN:
		if (cKeyStates.IsControlPushed())
		{
			if (cKeyStates.IsShiftPushed())
			{
				m_pDocuments->InsertReturnDown( m_bAutoIndentMode );
			}
			else
			{
				m_pDocuments->InsertReturnUp( m_bAutoIndentMode );
			}
		}
		else m_pDocuments->InsertReturn( m_bAutoIndentMode );
		// 表示位置を更新する
		if (m_pDocuments->GetFirstVisible(m_nViewID)->GetLinePointer()->GetOffset() + m_nVisibleLines ==
			m_pDocuments->GetCaretPosition()->GetLinePointer()->GetOffset())
			m_pDocuments->GetFirstVisible(m_nViewID)->MoveEthicNext(m_pDocuments->GetLineList(),1);
		nNeedRedraw = CFootyDoc::REDRAW_ALL;
		// イベントの通知
		m_pDocuments->SendTextModified(MODIFIED_CAUSE_ENTER);		
		break;

// ESCキーで選択をキャンセルします
	case VK_ESCAPE:
		m_pDocuments->UnSelect();
		break;

//それ以外のとき
	default:
		return false;
	}

	// キャレット位置の最終調節
	if (!bDontAdjustLine)AdjustVisibleLine();
	AdjustVisiblePos();

	// 再描画フラグを整理
	if (nNeedRedraw != CFootyDoc::REDRAW_ALL){
		if (nBeforeFirstVisibleColumn != m_nFirstVisibleColumn)
			nNeedRedraw = CFootyDoc::REDRAW_ALL;
		else if (*m_pDocuments->GetFirstVisible(m_nViewID) != cBeforeFirstVisibleLine){
			if (nNeedRedraw == CFootyDoc::REDRAW_CURMOVE)
				nNeedRedraw = CFootyDoc::REDRAW_SCROLLED;
			else
				nNeedRedraw = CFootyDoc::REDRAW_ALL;
		}
	}

	// 再描画処理
	switch(nNeedRedraw){
	case CFootyDoc::REDRAW_SCROLLED:		// スクロールされた
		ScrollRefresh();
		FOOTY2_PRINTF( L"Scroll redraw\n" );
		break;
	case CFootyDoc::REDRAW_CURMOVE:			// キャレット移動のみ
		CaretRefresh();
		FOOTY2_PRINTF( L"cursor redraw\n" );
		break;
	case CFootyDoc::REDRAW_SELCHANGED:
		LineChangedRefresh(pLineBefore, m_pDocuments->GetCaretPosition()->GetLinePointer());
		FOOTY2_PRINTF( L"sel redraw\n" );
		break;
	case CFootyDoc::REDRAW_LINE:			// 一行のみ再描画
		LineChangedRefresh();
		FOOTY2_PRINTF( L"line redraw\n" );
		break;
	case CFootyDoc::REDRAW_ALL:				// 完全再描画
		Refresh();
		FOOTY2_PRINTF( L"all redraw\n" );
		break;
	}

	return true;
}

/*[EOF]*/
