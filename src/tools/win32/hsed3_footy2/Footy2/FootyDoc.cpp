/**
 * @file FootyDoc.cpp
 * @brief ドキュメント管理クラスの実装
 * @author ShinjiWatanabe
 * @version 1.0
 */

#include "FootyDoc.h"

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタです。
 */
CFootyDoc::CFootyDoc()	:
	m_nTabLen( 4 ),
	m_nLapelMode( 0 ),
	m_nLapelColumns( 1024 ),
	m_nSelectType( SELECT_NONE ),
	m_nLineMode( LM_CRLF ),
	m_bReadOnly( false ),
	m_bInsertMode( true ),
	m_pFuncMoveCaret( NULL ),
	m_pDataMoveCaret( NULL ),
	m_pFuncTextModified( NULL ),
	m_pDataTextModified( NULL ),
	m_pFuncInsertMode( NULL ),
	m_pDataInsertModeChanged( NULL )
{
	f_SpeedDraw = 0;
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタです。
 */
CFootyDoc::~CFootyDoc()
{
}

//-----------------------------------------------------------------------------
/**
 * @brief 文書の新規作成
 */
void CFootyDoc::CreateNew(int nGlobalID)
{
	// 今までの情報を全て破棄する
	m_lsLines.clear();
	m_lsUndoBuffer.clear();
	m_lsEmphasisWord.clear();
	m_pNowUndoPos = m_lsUndoBuffer.end();
	SetSavePoint();
	SetReadOnly( false );
	
	// 最初の１行を挿入する
	CFootyLine cFirstLine;
	cFirstLine.m_strLineData = L"";
	m_lsLines.push_back(cFirstLine);
	
	// エディタ上の位置を設定する
	m_cCaretPos.SetPosition(&m_lsLines,0,0);
	SendMoveCaretCallBack();
	for (int i=0;i<4;i++)
	{
		m_cFirstVisibleLine[i].SetPosition(m_lsLines.begin());
	}
	
	// 変数初期化
	m_nSelectType = SELECT_NONE;
	m_nLineMode = LM_CRLF;
	m_nGlobalID = nGlobalID;
}

//-----------------------------------------------------------------------------
/**
 * @brief 編集されているかどうかのセーブポイントとして登録する処理
 */
void CFootyDoc::SetSavePoint()
{
	m_pSavedPos = m_pNowUndoPos;
	m_bCannotReachSavedPos = false;
}

//-----------------------------------------------------------------------------
/**
 * @brief 編集されているか取得する処理
 */
bool CFootyDoc::IsEdited()
{
	if (m_bCannotReachSavedPos)		// セーブポイントにたどり着けないときは
		return true;				// 編集されている状態
	else							// たどりつけるときは
		return m_pNowUndoPos != m_pSavedPos;
}

//-----------------------------------------------------------------------------
/**
 * @brief やり直し回数を取得する処理
 */
int CFootyDoc::GetRedoRem()
{
	std::list<CUndoBuffer>::iterator p = m_pNowUndoPos;
	int nRedoNum;
	for(nRedoNum = 0; p != m_lsUndoBuffer.end() ; p++, nRedoNum++);
	return nRedoNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief 元に戻す回数を取得する処理
 */
int CFootyDoc::GetUndoRem()
{
	std::list<CUndoBuffer>::iterator p = m_pNowUndoPos;
	int nUndoNum = 0;
	if( p != m_lsUndoBuffer.end() || !m_lsUndoBuffer.empty() ) {
		for(; p != m_lsUndoBuffer.begin() ; p--, nUndoNum++);
	}
	return nUndoNum;
}

//-----------------------------------------------------------------------------
/**
 * @brief 全てのテキストをセットします
 */
void CFootyDoc::SetText( const wchar_t *pString )
{
	// データをセットする
	CreateNew(m_nGlobalID);
	InsertString( pString, false, false, true );
	
	// 変数の初期化
	m_cCaretPos.SetPosition(&m_lsLines,0,0);
	SendMoveCaretCallBack();
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定された行番号の行クラスへのイテレータを取得します(0ベース)。
 */
LinePt CFootyDoc::GetLine(size_t nLine)
{
	// 宣言
	size_t i;				// 番号走査用
	LinePt iterNowLine;		// 作業用のイテレータ
	// エラーチェック
	if (GetLineNum() <= nLine)return m_lsLines.end();
	// 走査開始
	iterNowLine=m_lsLines.begin();
	for (i=0;i<nLine;i++)iterNowLine++;
	// 値を返却する
	return iterNowLine;
}

//-----------------------------------------------------------------------------
/**
 * @brief 本当の最終行を取得するルーチンです。
 */
LinePt CFootyDoc::GetLastLine()
{
	LinePt pLast = m_lsLines.end();
	pLast -- ;
	return pLast;
}

//-----------------------------------------------------------------------------
/**
 * @brief 強調表示文字列を設定します。
 */
bool CFootyDoc::AddEmphasis(const wchar_t *pString1,const wchar_t *pString2,
	int nType,int nFlags,int nLevel,int nOnLevel,int nIndependence,COLORREF nColor)
{
	/*エラーチェック*/
	if (!pString1)return false;
	if (nLevel < 1 || sizeof(int)*8 - 1 < nLevel)return false;
	/*設定する*/
	CEmphasisWord cEmpWord;
	cEmpWord.SetValues(pString1,pString2,
		nType,nFlags,nLevel,nOnLevel,nIndependence,nColor);
	/*コンテナに格納する*/
	m_lsEmphasisWord.push_back(cEmpWord);
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 強調表示を確定します。
 */
void CFootyDoc::FlushEmphasis()
{
	for (LinePt pLine = m_lsLines.begin();pLine != m_lsLines.end();pLine++)
	{
		pLine->SetEmphasisChached(false);
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 強調表示文字列を登録されているのを全て削除する
 */
void CFootyDoc::ClearEmphasis()
{
	m_lsEmphasisWord.clear();
}

//-----------------------------------------------------------------------------
/** * @brief 挿入状態を変化させる処理です

 */
void CFootyDoc::SetInsertMode(bool bInsertMode)
{
	m_bInsertMode = bInsertMode;
	if (m_pFuncInsertMode)
	{
		m_pFuncInsertMode(m_nGlobalID,m_pDataInsertModeChanged,bInsertMode);
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief TAB文字の幅を設定します
 */
void CFootyDoc::SetTabLen(size_t nTabLen){
	if (0 < nTabLen){
		m_nTabLen = nTabLen;
	}
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::InsertString
 * @brief 現在のキャレット位置に文字を挿入します。
 * @param pString 挿入する文字列(NULL終端)
 * @param bRecUndo trueのときアンドゥを記録する
 * @param bOverwritable INSERTキーによる上書きチェックを有効にする
 * @param bMemLineMode 改行コードをこの文章のコードとして保存する
 */
CFootyDoc::RedrawType CFootyDoc::InsertString
		(const wchar_t *pString,bool bRecUndo,bool bOverwritable,bool bMemLineMode)
{
	FOOTY2_PRINTF( L"InsertString開始\n");
	// 宣言
	const wchar_t *pWork;			//!< 改行位置検索用
	std::wstring strRestLine;		//!< 挿入位置以降のデータ
	LinePt pLine,pBeginLine;
	size_t nNumLines = 0;
	size_t nPos;
	CUndoBuffer cUndo;

	// エラーチェック
	if ( !pString )return REDRAW_FAILED;
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 上書きの場合は
	if (bOverwritable && !m_bInsertMode && !IsSelecting())
	{
		// 長さを計測する
		size_t nStrLen = wcslen(pString);
		size_t nLineLeft = m_cCaretPos.GetLinePointer()->GetLineLength()
							- m_cCaretPos.GetPosition();
		// 選択処理
		SetSelectStart();
		m_cCaretPos.MoveColumnForward(&m_lsLines,min(nLineLeft,nStrLen));
		SetSelectEndNormal();
		// そして、再帰
		return InsertString(pString);
	}

	// 選択位置情報を削除してアンドゥ情報を代入
	if (bRecUndo)
	{
		if (IsSelecting())
		{
			DeleteSelected(&cUndo);
			cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_REPLACE;
		}
		else cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_INSERT;
		cUndo.m_cAfterStart = m_cCaretPos;
		cUndo.m_strAfter = pString;
	}
	else DeleteSelected(NULL);
	
	// キャレットの位置を取得する
	pLine = m_cCaretPos.GetLinePointer();
	pBeginLine = pLine;
	nPos = m_cCaretPos.GetPosition();

	// 文字を走査していく
	for (pWork=pString;;pWork++)
	{
		if (*pWork == L'\r')
		{
			if (*(pWork+1) == L'\n')	// CRLF
			{
				ReturnLine(&pLine,&pString,pWork,
							nPos,&strRestLine,2,nNumLines==0);
				nNumLines++;
				pWork++;
				if (bMemLineMode)m_nLineMode = LM_CRLF;
				FOOTY2_PRINTF( L"InsertString CRLF発見\n");
			}
			else						// キャリッジリターンのみ
			{
				ReturnLine(&pLine,&pString,pWork,
							nPos,&strRestLine,1,nNumLines==0);
				nNumLines++;
				if (bMemLineMode)m_nLineMode = LM_CR;
				FOOTY2_PRINTF( L"InsertString CR発見\n");
			}
		}
		else if (*pWork == L'\n')		// ラインフィード
		{
			ReturnLine(&pLine,&pString,pWork,
						nPos,&strRestLine,1,nNumLines==0);
			nNumLines++;
			if (bMemLineMode)m_nLineMode = LM_LF;
			FOOTY2_PRINTF( L"InsertString LF発見\n");
		}
		else if (*pWork == L'\0')		// 終端文字
		{
			FOOTY2_PRINTF( L"InsertString走査終了\n");
			if (!nNumLines)				// 今までに改行が無かったとき
			{
				FOOTY2_PRINTF( L"InsertString改行なし\n");
				// その位置に全ての文字列を挿入する
				pLine->m_strLineData.insert(nPos,pString);
				FOOTY2_PRINTF( L"InsertString変更確定しそう\n");
				SetLineInfo(pBeginLine, false/*改行を含むか by Tetr@pod*/);
				FOOTY2_PRINTF( L"InsertString変更確定\n");
				// キャレット位置を移動させる
				m_cCaretPos.MoveColumnForward(&m_lsLines,(size_t)(pWork-pString));
			}
			else						// 改行があったとき
			{
				FOOTY2_PRINTF( L"InsertString改行あり\n");
				// バックアップ文字列を代入する
				pLine->m_strLineData = pString + strRestLine;
				// 変更を確定する
				FOOTY2_PRINTF( L"InsertString変更確定しそう\n");
				SetLineInfo(pBeginLine,pLine, true/*改行を含むか by Tetr@pod*/);
				FOOTY2_PRINTF( L"InsertString変更確定\n");
				// キャレット位置を移動させる
				m_cCaretPos.MoveRealNext(&m_lsLines,nNumLines);
				m_cCaretPos.MoveColumnForward(&m_lsLines,(size_t)(pWork-pString));
			}
			FOOTY2_PRINTF( L"InsertStringアンドゥ情報追加\n");
			// アンドゥ情報を挿入する
			if (bRecUndo)
			{
				cUndo.m_cAfterEnd = m_cCaretPos;
				PushBackUndo(&cUndo);
			}
			break;
		}
	}
	FOOTY2_PRINTF( L"InsertString終了間際\n");
	SendMoveCaretCallBack();
	FOOTY2_PRINTF( L"InsertString終了\n");
	return REDRAW_ALL;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::ReturnLine
 * @brief InsertStringのサブルーチンで、改行処理を行います。
 */
void CFootyDoc::ReturnLine(LinePt *pNowLine,const wchar_t **pString,const wchar_t *pWork,
						   size_t nPos,std::wstring *pRestStr,int n,bool bFirst)
{
	// 宣言
	LinePt pLine = *pNowLine;
	CFootyLine cInsertLine;
	
	// データ挿入
	if (!bFirst)	// 最初でないときはコピーする
		pLine->m_strLineData = std::wstring(*pString,(size_t)(pWork - *pString));
	else			// 最初の位置の時はバックアップ
	{
		if (nPos == pLine->GetLineLength())
		{
			*pRestStr = L"";
		}
		else
		{
			*pRestStr = pLine->m_strLineData.substr(nPos);
			pLine->m_strLineData.erase(nPos,pLine->GetLineLength()-nPos);
		}
		pLine->m_strLineData += std::wstring(*pString,(size_t)(pWork - *pString));
	}
	//次の行を生成する
	pLine++;
	m_lsLines.insert(pLine,cInsertLine);
	// ポインタ操作
	*pString = pWork + n;
	(*pNowLine)++;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::InsertChar
 * @brief ワイド文字一つを挿入します
 * @param wChar 挿入する文字
 * @return 描画範囲
 */
CFootyDoc::RedrawType CFootyDoc::InsertChar(wchar_t wChar)
{
	// 宣言
	LinePt pLine;
	size_t nPos;
	CUndoBuffer cUndo;
	CEditPosition cEditPosition = m_cCaretPos;
	RedrawType nRetRedraw = REDRAW_LINE;
	
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 選択文字列を削除、アンドゥ情報を格納
	if ( IsSelecting() )
	{
		DeleteSelected(&cUndo);
		cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_REPLACE;
		nRetRedraw = REDRAW_ALL;
	}
	else if (m_bInsertMode || m_cCaretPos.GetPosition() == m_cCaretPos.GetLinePointer()->GetLineLength())
	{
		cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_INSERT;
	}
	else
	{
		// 上書きされるときは、アンドゥ情報を書き換える必要がある
		cUndo.m_cBeforeStart = cEditPosition;
		cEditPosition.MoveColumnForward(&m_lsLines,1);
		cUndo.m_cBeforeEnd = cEditPosition;
		cUndo.m_strBefore += m_cCaretPos.GetLinePointer()->GetLineData()
			[m_cCaretPos.GetPosition()];
		cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_REPLACE;
	}
	cUndo.m_strAfter += wChar;
	
	// 文字を入れる
	pLine = m_cCaretPos.GetLinePointer();
	nPos = m_cCaretPos.GetPosition();
	if (m_bInsertMode || nPos == pLine->GetLineLength())
		pLine->m_strLineData.insert(nPos,1,wChar);
	else
		pLine->m_strLineData[nPos] = wChar;
	
	// 倫理行何行になるかチェック
	size_t nBeforeEthic = pLine->GetEthicLine();
	if (SetLineInfo(pLine, false/*改行を含むか by Tetr@pod*/))
		nRetRedraw = REDRAW_ALL;
	if (nBeforeEthic != pLine->GetEthicLine())
		nRetRedraw = REDRAW_ALL;

	// 桁を増加させる
	cUndo.m_cAfterStart = m_cCaretPos;
	m_cCaretPos.MoveColumnForward(&m_lsLines,1);
	cUndo.m_cAfterEnd = m_cCaretPos;

	// アンドゥ情報を挿入
	PushBackUndo(&cUndo);
	SendMoveCaretCallBack();
	return nRetRedraw;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在のキャレット位置で改行処理を行います。
 * @param	bIndentMode [in] オートインデントを行うかどうか
 */
CFootyDoc::RedrawType CFootyDoc::InsertReturn( bool bIndentMode )
{
	// 宣言
	CUndoBuffer cUndo;
	CFootyLine cNewLine;
	LinePt pLine, pInsertPos;
	size_t nPos;
	
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 選択位置を削除します
	if (IsSelecting())
	{
		DeleteSelected(&cUndo);
		cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_REPLACE;
	}
	else
	{
		cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_INSERT;
	}
	cUndo.m_cAfterStart = m_cCaretPos;
	
	// 改行を挿入する位置を取得します
	pLine = m_cCaretPos.GetLinePointer();
	nPos = m_cCaretPos.GetPosition();

	// オートインデントとして挿入できる文字数をカウントします
	size_t nIndentChars = 0;
	if ( bIndentMode )
	{
		nIndentChars = pLine->CalcAutoIndentPos( nPos );
	}

	// 挿入後、となるテキストを作成します
	cUndo.m_strAfter = L"\r\n";
	if  ( nIndentChars != 0 )
	{
		cUndo.m_strAfter.append( pLine->GetLineData(), nIndentChars );
	}

	// 行末で改行してないときは次の行に回します
	if (pLine->GetLineLength() != nPos)
	{
		if ( nIndentChars == 0 )
		{
			cNewLine.m_strLineData = &pLine->GetLineData()[nPos];
		}
		else
		{
			cNewLine.m_strLineData.append( pLine->GetLineData(), nIndentChars );
			cNewLine.m_strLineData += &pLine->GetLineData()[nPos];
		}
		pLine->m_strLineData.erase(nPos);
	}
	else if  ( nIndentChars != 0 )
	{
		cNewLine.m_strLineData.append( pLine->GetLineData(), nIndentChars );
	}

	// 次の行をリストに追加
	pInsertPos = pLine;
	pInsertPos++;
	pInsertPos = m_lsLines.insert( pInsertPos, cNewLine );
	SetLineInfo( pLine, pInsertPos, false/*改行を含むか by Tetr@pod*/ );

	// キャレット位置を移動させる
	m_cCaretPos.SetPosition( pInsertPos, nIndentChars );

	// 再度更新
	SetLineInfo( pLine, pInsertPos, false/*改行を含むか by Tetr@pod*/ );

	// アンドゥー情報を格納
	cUndo.m_cAfterEnd = m_cCaretPos;
	PushBackUndo(&cUndo);
	SendMoveCaretCallBack();
	return REDRAW_ALL;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在のキャレット位置の一行上に空行を挿入します。
 * @param	bIndentMode [in] オートインデント処理を行うかどうか
 */
CFootyDoc::RedrawType CFootyDoc::InsertReturnUp( bool bIndentMode )
{
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 選択状態の解除
	m_nSelectType = SELECT_NONE;
	
	// 状況によって処理を分ける
	if (m_cCaretPos.GetLineNum() == 0)
	{
		// 一番上の行の時は、その位置の先頭で改行したことにする
		m_cCaretPos.SetPosition(m_cCaretPos.GetLinePointer(),0);
		InsertReturn( bIndentMode );
		m_cCaretPos.SetPosition(m_lsLines.begin(),0);
	}
	else
	{
		// それ以外の時は、その位置の一つ上で改行したことにする
		LinePt pLine = m_cCaretPos.GetLinePointer();
		pLine--;
		m_cCaretPos.SetPosition(pLine,pLine->GetLineLength());
		InsertReturn( bIndentMode );
	}
	return REDRAW_ALL;
}

//-----------------------------------------------------------------------------
/**
 * @brief 現在のキャレット位置の一行下に空行を挿入します。
 * @param bIndent [in] インデント処理を行うかどうか
 */
CFootyDoc::RedrawType CFootyDoc::InsertReturnDown( bool bIndent )
{
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 選択状態の解除
	m_nSelectType = SELECT_NONE;
	
	// それ以外の時は、その位置の一つ上で改行したことにする
	m_cCaretPos.SetPosition(m_cCaretPos.GetLinePointer(),
		m_cCaretPos.GetLinePointer()->GetLineLength());
	InsertReturn( bIndent );
	return REDRAW_ALL;
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::OnBackSpace
 * @brief BackSpaceキーが押されたときの処理を行います。
 * @return どのように再描画するのか
 * @note 適切な位置へキャレットが移動します。
 */
CFootyDoc::RedrawType CFootyDoc::OnBackSpace()
{
	// 宣言
	LinePt pLine = m_cCaretPos.GetLinePointer();
	size_t nPos = m_cCaretPos.GetPosition();
	CUndoBuffer cUndo;
	RedrawType nType;
	
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 場合分け
	if (IsSelecting())
	{
		nType = DeleteSelected(&cUndo);
		PushBackUndo(&cUndo);
		return nType;
	}
	else	// 選択していないとき
	{
		if (nPos != 0)			// 文字を削除
		{
			bool bIsSurrogatePair = CFootyLine::IsSurrogateTail(pLine->GetLineData()[nPos-1]);
			RedrawType nNeedRedraw = REDRAW_LINE;
			
			// アンドゥ情報を代入する
			cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_DELETE;
			if (bIsSurrogatePair)
				cUndo.m_strBefore = pLine->m_strLineData.substr(nPos-2,2);
			else	
				cUndo.m_strBefore = pLine->m_strLineData.substr(nPos-1,1);
			cUndo.m_cBeforeEnd = m_cCaretPos;
			
			// 文字列を削除する
			if (bIsSurrogatePair)
				pLine->m_strLineData.erase(nPos-2,2);
			else
				pLine->m_strLineData.erase(nPos-1,1);
			size_t nBeforeEthic = pLine->GetEthicLine();
			if (SetLineInfo(pLine, false/*改行を含むか by Tetr@pod*/))
				nNeedRedraw = REDRAW_ALL;
			if (nBeforeEthic != pLine->GetEthicLine())
				nNeedRedraw = REDRAW_ALL;
			
			// ポジションを移動して
			if (bIsSurrogatePair)
				m_cCaretPos.MoveColumnBackward(&m_lsLines,2);
			else
				m_cCaretPos.MoveColumnBackward(&m_lsLines,1);
			
			// アンドゥ情報を格納する
			cUndo.m_cBeforeStart = m_cCaretPos;
			PushBackUndo(&cUndo);
			SendMoveCaretCallBack();
			return nNeedRedraw;
		}
		else					// 改行を削除
		{
			if (pLine != m_lsLines.begin())
			{
				// 次の行ポインタ取得
				LinePt pPrevLine = pLine;
				pPrevLine--;

				// アンドゥ情報を代入しつつキャレット位置を移動する
				cUndo.m_nUndoType = CUndoBuffer::UNDOTYPE_DELETE;
				cUndo.m_strBefore = L"\r\n";
				cUndo.m_cBeforeEnd = m_cCaretPos;
				m_cCaretPos.MoveColumnBackward(&m_lsLines,1);
				cUndo.m_cBeforeStart = m_cCaretPos;
				PushBackUndo(&cUndo);

				// 行の結合
				pPrevLine->m_strLineData += pLine->GetLineData();

				// 現在の位置を削除する
				DeleteLine(pLine);

				// 情報を更新する
				LinePt pNextLine = pPrevLine;
				pNextLine++;      
				SetLineInfo(pPrevLine,pNextLine, false);  

				SendMoveCaretCallBack();
				return REDRAW_ALL;
			}
			else return REDRAW_NONE;
		}
	}
}

//-----------------------------------------------------------------------------
/**
 * CFootyDoc::OnDelete
 * @brief Deleteキーが押されたときの処理を行います。
 * @return どのように再描画するのか
 */
CFootyDoc::RedrawType CFootyDoc::OnDelete()
{
	// 宣言
	CUndoBuffer cUndo;
	RedrawType nType;
	
	if ( IsReadOnly() ) return REDRAW_FAILED;
	
	// 選択しているかどうかで場合分け
	if (IsSelecting()){
		nType = DeleteSelected(&cUndo);
		PushBackUndo(&cUndo);
		return nType;
	}
	else{
		// 前に一つ移動してからBackSpaceという形にする
		CEditPosition cBackUp = m_cCaretPos;
		m_cCaretPos.MoveColumnForward(&m_lsLines,1);
		if (cBackUp != m_cCaretPos)	// 違う
			return OnBackSpace();
		else						// 末尾
			return REDRAW_NONE;
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定した行以降のオフセット値と実際の行番号を設定するルーチン
 * @param pBegin 設定するための開始位置
 * @return 全ての再描画を要求するときtrue
 * @note pBeginは文字列が変更されているのでURLとメールアドレスを再検索。
 *       以降の行は強調表示文字列を全て検索します。
 */
bool CFootyDoc::SetLineInfo(LinePt pBegin, bool ForceListUpdate/*改行を含むか by Tetr@pod*/)
{
	bool bAllRedraw = false;
	bool bPrevLineInfoChanged = false;
	bool bEmphasisChanged = false;
	
	// 最初の行だけを確定させる
	bPrevLineInfoChanged = pBegin->FlushString(m_nGlobalID/*by Tetr@pod*/, m_nTabLen,m_nLapelColumns,m_nLapelMode, &bAllRedraw/*by Tetr@pod*/, ForceListUpdate/*改行を含むか by Tetr@pod*/);
	if (pBegin == m_lsLines.begin())
		bEmphasisChanged = pBegin->SearchEmphasis(NULL,&m_lsEmphasisWord);
	else
	{
		LinePt pPrevLine = pBegin;
		pPrevLine--;
		bEmphasisChanged = pBegin->SearchEmphasis(pPrevLine->GetBetweenNext(),&m_lsEmphasisWord);
	}
	bAllRedraw = bAllRedraw/*by Tetr@pod*/ || bEmphasisChanged || bPrevLineInfoChanged;

	// ループさせて収集(pBeginの次の行から最後までループ)
	pBegin++;
	for (LinePt pLine = pBegin; pLine != m_lsLines.end(); pLine++)
	{
		// 前の情報を入れておく
		if (bPrevLineInfoChanged && pLine != m_lsLines.begin())
		{
			LinePt pPrevLine = pLine;
			pPrevLine--;
			bPrevLineInfoChanged = pLine->SetPrevLineInfo(pPrevLine);
			bAllRedraw = bAllRedraw || bPrevLineInfoChanged;
		}

		// 強調が変わったときは全てフラグをおろしておく
		if (bEmphasisChanged)
			pLine->SetEmphasisChached(false);
	}

	return bAllRedraw;
}

//-----------------------------------------------------------------------------
/**
 * @brief 行の内容が変更されたとき、その情報を収集していく処理です。
 * @param pBegin 変更領域最初の行
 * @param pEnd 変更領域の最後の行
 * @return 全ての再描画を要求するときtrue
 */
bool CFootyDoc::SetLineInfo(LinePt pBegin, LinePt pEnd, bool ForceListUpdate/*改行を含むか by Tetr@pod*/)
{
	LinePt pLine;
	bool bAllRedraw = false;
	bool bPrevLineInfoChanged = false;
	bool bEmphasisChanged = false;

	// 変更された領域は常に見ておく
	for (pLine = pBegin; ;pLine++)
	{
		// それがすでに文書の最後の行のときは関数を終了
		if (pLine == m_lsLines.end()) {
			return bAllRedraw;
		}

		// 現在の行を調査する
		bPrevLineInfoChanged = pLine->FlushString(m_nGlobalID/*by Tetr@pod*/, m_nTabLen,m_nLapelColumns,m_nLapelMode, &bAllRedraw/*by Tetr@pod*/, ForceListUpdate/*改行を含むか by Tetr@pod*/);

		if (pLine == m_lsLines.begin())
			bEmphasisChanged = pLine->SearchEmphasis(NULL,&m_lsEmphasisWord);
		else
		{
			LinePt pPrevLine = pLine;
			pPrevLine--;
			bEmphasisChanged = pLine->SearchEmphasis(pPrevLine->GetBetweenNext(),&m_lsEmphasisWord);
			bPrevLineInfoChanged = pLine->SetPrevLineInfo(pPrevLine);
		}
		bAllRedraw = bAllRedraw/*by Tetr@pod*/ || bEmphasisChanged || bPrevLineInfoChanged;

		// 常に見る領域を抜けるかどうかの判定
		if (pLine == pEnd)break;
	}

	// ループさせて最後まで取っていく
	for (pLine++ ; pLine != m_lsLines.end(); pLine++)
	{
		if (pLine != m_lsLines.end()) {
			// 前の情報を入れておく
			if (bPrevLineInfoChanged && pLine != m_lsLines.begin())
			{
				LinePt pPrevLine = pLine;
				pPrevLine--;
				bPrevLineInfoChanged = pLine->SetPrevLineInfo(pPrevLine);
				bAllRedraw = bAllRedraw || bPrevLineInfoChanged;
			}
		}

		// 強調が変わったときは全てフラグをおろしておく
		if (bEmphasisChanged)
			pLine->SetEmphasisChached(false);
	}

	return bAllRedraw;
}

//-----------------------------------------------------------------------------
/**
 * @brief 強調表示のキャッシュを作成する処理
 * @param pLine キャッシュ計算を開始する位置
 */
void CFootyDoc::SetChacheCommand(LinePt pLine)
{
	// 表示位置を更新する
	//SYSTEMTIME st;
	if (pLine == m_lsLines.begin())
	{
		pLine->SearchEmphasis(NULL,&m_lsEmphasisWord);
//GetLocalTime(&st);
//FOOTY2_PRINTF( L"SearchEmphasis！ %d年%d月%d日%d時%d分%d秒%dms\n", st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
	}
	else
	{
		// キャッシュされている行を取得する(現在行からさかのぼって検索・マイナス方向)
		LinePt pStartLine;
		for (pStartLine = pLine;pStartLine != m_lsLines.begin();pStartLine--)
		{
			if (pStartLine->EmphasisChached()){
//GetLocalTime(&st);
//FOOTY2_PRINTF( L"キャッシュされている行がみつかった！ %d年%d月%d日%d時%d分%d秒%dms\n", st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
				break;
			}
//GetLocalTime(&st);
//FOOTY2_PRINTF( L"キャッシュ行検索ナウ！ %d行 : %d年%d月%d日%d時%d分%d秒%dms\n", pStartLine, st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
		}

		// 一行もキャッシュされていないとき
		if (pStartLine == m_lsLines.begin() && !pStartLine->EmphasisChached())
		{
			pStartLine->SearchEmphasis(NULL,&m_lsEmphasisWord);
//GetLocalTime(&st);
//FOOTY2_PRINTF( L"一行もキャッシュされていない %d年%d月%d日%d時%d分%d秒%dms\n", st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
		}

		//FOOTY2_PRINTF( L"キャッシュ開始行？？？ %d\n",pStartLine->GetRealLineNum());
		
		/*
		LinePt pNowLine;
		LinePt pPrevLine;
		int i = 0;
		for (pNowLine = pLine, i = 0;pNowLine != m_lsLines.begin();pNowLine--,pPrevLine--,i++)
		{
			if (i == 30)break;
			pNowLine->SearchEmphasis(pPrevLine->GetBetweenNext(),&m_lsEmphasisWord);

		}

		*/
		// 高速描画が有効な場合 by inovia
		if (f_SpeedDraw != 0){
			// キャッシュされている行を取得する(現在行からさかのぼって検索・マイナス方向)
			int i = 0;
			for (pStartLine = pLine;pStartLine != m_lsLines.begin();pStartLine--)
			{
				if (i == 30)break;
				if (pStartLine->EmphasisChached()){
					break;
				}
				i++;
			}
		}
		// 開始行から基準点までキャッシュする
		LinePt pPrevLine = pStartLine;//pStartLine;
		LinePt pNowLine = pStartLine;//pStartLine;

		for (pNowLine++;;pNowLine++,pPrevLine++)
		{
	//		FOOTY2_PRINTF( L"キャッシュカレント行 追加中 %d\n",pNowLine->GetRealLineNum());
	//GetLocalTime(&st);
	//FOOTY2_PRINTF( L"キャッシュ追加ナウ！ %d行%d行%d行 : %d年%d月%d日%d時%d分%d秒%dms\n", pPrevLine,pPrevLine,pLine, st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
			pNowLine->SearchEmphasis(pPrevLine->GetBetweenNext(),&m_lsEmphasisWord);
			if (pNowLine == pLine)break;
		}

		
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 一行だけ削除する処理です。 
 * @param pLine 削除する行
 */
void CFootyDoc::DeleteLine(LinePt pLine)
{
	if ( IsReadOnly() ) return;
	
	for (int i=0;i<4;i++)
	{
		if (m_cFirstVisibleLine[i].GetLineNum() == pLine->GetRealLineNum()){
			m_cFirstVisibleLine[i].MoveRealBack(1);
		}
	}
	m_lsLines.erase(pLine);
}

//-----------------------------------------------------------------------------
/**
 * @brief 指定された間の行を一気に削除する処理
 * @param pStart 開始行(これも削除される)
 * @param pEnd 終了行(これは削除されない)
 */
void CFootyDoc::DeleteLine(LinePt pStart,LinePt pEnd)
{
	if ( IsReadOnly() ) return;
	
	// 削除するデータの行番号を取得する
	size_t nRealStart = pStart->GetRealLineNum();
	LinePt pDelEnd = pEnd;
	pDelEnd--;
	size_t nRealEnd = pDelEnd->GetRealLineNum();

	// 最初に表示されている行と被っていたら調整してやる
	for (int i=0;i<4;i++)
	{
		if (nRealStart <= m_cFirstVisibleLine[i].GetLineNum() &&
			m_cFirstVisibleLine[i].GetLineNum() <= nRealEnd)
		{
			m_cFirstVisibleLine[i].SetPosition(pStart);
			m_cFirstVisibleLine[i].MoveRealBack(1);
		}
	}

	// 実際に削除する処理
	m_lsLines.erase(pStart,pEnd);
}

//-----------------------------------------------------------------------------
/**
 * @brief キャレット位置を単語すっ飛ばして前に
 */
void CFootyDoc::MoveWordForward()
{
	LinePt pLine = m_cCaretPos.GetLinePointer();
	size_t nPos = m_cCaretPos.GetPosition();
	
	if (m_cCaretPos.GetPosition() != pLine->GetLineLength())
	{
		CFootyLine::WordInfo wiWord = pLine->GetWordInfo(nPos,false);
		m_cCaretPos.MoveColumnForward(&m_lsLines,wiWord.m_nEndPos - nPos);
	}
	else m_cCaretPos.MoveColumnForward(&m_lsLines,1);
	SendMoveCaretCallBack();
}

//-----------------------------------------------------------------------------
/**
 * @brief キャレット位置を単語すっ飛ばして後に
 */
void CFootyDoc::MoveWordBack()
{
	LinePt pLine = m_cCaretPos.GetLinePointer();
	size_t nPos = m_cCaretPos.GetPosition();
	
	if (m_cCaretPos.GetPosition() != 0)
	{
		CFootyLine::WordInfo wiWord = pLine->GetWordInfo(nPos);
		m_cCaretPos.MoveColumnBackward(&m_lsLines,nPos - wiWord.m_nBeginPos);
	}
	else m_cCaretPos.MoveColumnBackward(&m_lsLines,1);
	SendMoveCaretCallBack();
}

//-----------------------------------------------------------------------------
/**
 * @brief 折り返し設定
 * @param nColumn 桁数
 * @param nMode 折り返しモード
 */
void CFootyDoc::SetLapel(size_t nColumn,int nMode)
{
	// 状態を設定する
	m_nLapelColumns = nColumn;
	m_nLapelMode = nMode;

	// ループさせて収集
	LinePt pLine = m_lsLines.begin();
	LinePt pPrevLine = m_lsLines.begin();
	pLine->FlushString(m_nGlobalID, m_nTabLen,m_nLapelColumns,m_nLapelMode, NULL, false/*改行を含むか by Tetr@pod*/);
	
	for (pLine++;pLine != m_lsLines.end();pLine++,pPrevLine++)
	{
		pLine->FlushString(m_nGlobalID, m_nTabLen,m_nLapelColumns,m_nLapelMode, NULL, false/*改行を含むか by Tetr@pod*/);
		pLine->SetPrevLineInfo(pPrevLine);
	}	
}
//-----------------------------------------------------------------------------
/**
 * @brief 高速描画 by inovia
 * @param	flag [in] フラグ
 */
void CFootyDoc::SetSpeedDraw( int flag )
{
	f_SpeedDraw = flag;
}

bool CFootyDoc::FlushString2(LinePt pLine)// by Tetr@pod
{
	return pLine->FlushString(m_nGlobalID/*by Tetr@pod*/, m_nTabLen,m_nLapelColumns,m_nLapelMode, NULL/*by Tetr@pod*/, false/*改行を含むか by Tetr@pod*/);
}

/*[EOF]*/
