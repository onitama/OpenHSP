/**
 * @file DllTextFunc.cpp
 * @brief 見栄えに関するDLL関数を記述します。
 * @author nabeshin
 * @date 08/24/2007
 * @version 0.9
 *
 * @note DllMain.cppから分離
 */

#include "ConvFactory.h"

#ifndef UNDER_CE
//-----------------------------------------------------------------------------
/**
 * @brief 全体のテキストをセットします。
 * @param nID FootyのID
 * @param pString 文字列
 * @return エラー
 */
FOOTYEXPORT(int) Footy2SetTextA(int nID,const char *pString)
{
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	// Unicode変換
	CConvFactory cConv;
	if (!cConv.GetConv()->ToUnicode(pString,(UINT)strlen(pString)))return FOOTY2ERR_MEMORY;
	// 文字列を挿入
	pFooty->SetText((wchar_t*)cConv.GetConv()->GetConvData());
	pFooty->SetCharSet(CSM_DEFAULT);
	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/

//-----------------------------------------------------------------------------
/**
 * @brief 全体のテキストをセットします。
 * @param nID FootyのID
 * @param pString 文字列
 * @return エラー
 */
FOOTYEXPORT(int) Footy2SetTextW(int nID,const wchar_t *pString)
{
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	// 文字列を挿入
	pFooty->SetText(pString);
	pFooty->SetCharSet(CSM_DEFAULT);
	return FOOTY2ERR_NONE;
}


#ifndef UNDER_CE
//-----------------------------------------------------------------------------
/**
 * Footy2SetSelTextA
 * @brief 選択文字列をセットします。
 * @param nID FootyのID
 * @param pString 文字列
 * @return エラー
 */
FOOTYEXPORT(int) Footy2SetSelTextA(int nID,const char *pString)
{
	if (!pString)return FOOTY2ERR_ARGUMENT;	// 文字列１は必須
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	// 変換してセット
	CConvFactory cConv;
	if (!cConv.GetConv()->ToUnicode(pString,(UINT)strlen(pString)))return FOOTY2ERR_MEMORY;
	return pFooty->SetSelText((wchar_t*)cConv.GetConv()->GetConvData(), true) ?
		FOOTY2ERR_NONE : FOOTY2ERR_ARGUMENT;
}
#endif

//-----------------------------------------------------------------------------
/**
 * @brief 選択文字列をセットします。
 * @param nID FootyのID
 * @param pString 文字列
 * @return エラー
 */
FOOTYEXPORT(int) Footy2SetSelTextW(int nID,const wchar_t *pString){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*強調表示文字列を登録する*/
	return pFooty->SetSelText(pString, true) ? FOOTY2ERR_NONE : FOOTY2ERR_ARGUMENT;
}

#ifndef UNDER_CE
//-----------------------------------------------------------------------------
/**
 * Footy2GetSelLengthA
 * @param nID FootyのID番号を指定します。
 * @param nLineMode 改行コードを何として取得するのか指定します。
 * @brief 選択中のテキストの文字数を返します。
 */
FOOTYEXPORT(int) Footy2GetSelLengthA(int nID, int nLineMode)
{
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	
	// 選択文字列を取得する
	std::wstring strSelText;
	if (!pFooty->m_cDoc.GetSelText(&strSelText))
		return FOOTY2ERR_MEMORY;

	// ANSI文字列に変換する
	CConvFactory cConv;
	cConv.GetConv()->ToMulti(strSelText.c_str(), (UINT)strSelText.size());

	// 値を返す
	return (int)cConv.GetConv()->GetConvTextSize() - sizeof(char);
}
#endif	/*UNDER_CE*/


//-----------------------------------------------------------------------------
/**
 * Footy2GetSelLengthW
 * @param nID FootyのID
 * @param nLineMode 改行コードをどのように扱うか
 */
FOOTYEXPORT(int) Footy2GetSelLengthW(int nID, int nLineMode)
{
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	// 強調表示文字列を登録する
	size_t nLength = pFooty->m_cDoc.GetSelLength(static_cast<LineMode>(nLineMode));
	if (nLength == 0)
		return FOOTY2ERR_NOTSELECTED;
	else
		return static_cast<int>(nLength);
}

#ifndef UNDER_CE
//-----------------------------------------------------------------------------
/**
 * Footy2GetTextLengthA
 * @brief テキストの文字数を取得します。
 */
FOOTYEXPORT(int) Footy2GetTextLengthA(int nID, int nLineMode)
{
	int nLineLen;
	size_t nRet = 0;
	LinePt iterLine,iterLastLine;
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*改行コードの文字数を指定する*/
	if (nLineMode == LM_AUTOMATIC)
		nLineMode = pFooty->m_cDoc.GetLineMode();
	nLineLen = nLineMode == LM_CRLF ? 2 : 1;
	/*ループさせて計算する*/
	for (iterLine = pFooty->m_cDoc.GetTopLine(),
		 iterLastLine = pFooty->m_cDoc.GetLastLine();;iterLine++)
	{
		 CConvFactory cConv;
		 cConv.GetConv()->ToMulti(iterLine->GetLineData(), (UINT)iterLine->GetLineLength());
		 nRet += cConv.GetConv()->GetConvTextSize() - sizeof(char);
		 if (iterLine == iterLastLine)break;
		 nRet += nLineLen;
	}
	/*値を返す*/
	return (int)nRet;
}
#endif	/*UNDER_CE*/

//-----------------------------------------------------------------------------
/**
 * @brief テキストの文字数を取得します。
 */
FOOTYEXPORT(int) Footy2GetTextLengthW(int nID,int nLineMode)
{
	int nLineLen;
	size_t nRet = 0;
	LinePt iterLine,iterLastLine;
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	// 改行コードの文字数を指定する
	if (nLineMode == LM_AUTOMATIC)
	{
		nLineMode = pFooty->m_cDoc.GetLineMode();
	}
	nLineLen = nLineMode == LM_CRLF ? 2 : 1;
	// ループさせて計算する
	for (iterLine = pFooty->m_cDoc.GetTopLine(),
		 iterLastLine = pFooty->m_cDoc.GetLastLine();
		 ;iterLine++)
	{
		nRet += iterLine->GetLineLength();
		if (iterLine == iterLastLine)break;
		nRet += nLineLen;
	}
	// 値を返す
	return (int)nRet;
}

#ifndef UNDER_CE

//-----------------------------------------------------------------------------
/**
 * @brief 文字列を取得します。
 * @param nID FootyのID番号
 * @param pString 格納先
 * @param nLineMode 改行コード
 * @param nSize pStringのバッファサイズ
 */
FOOTYEXPORT(int) Footy2GetTextA(int nID,char *pString,int nLineMode,int nSize)
{
	LinePt iterLine;
	LinePt iterLastLine;
	const char *pTextPos;
	size_t nLineLength;
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (!pString || nSize <= 0)return FOOTY2ERR_ARGUMENT;
	// 自動処理用に改行コードを変更する
	if (nLineMode == LM_AUTOMATIC)
		nLineMode = pFooty->m_cDoc.GetLineMode();
	// 文字列を挿入していく
	for (iterLine = pFooty->m_cDoc.GetTopLine(),
		 iterLastLine = pFooty->m_cDoc.GetLastLine();;
		 iterLine++)
	{
		// 行ごとに代入していく
		CConvFactory cConv;
		cConv.GetConv()->ToMulti(iterLine->GetLineData(), (UINT)iterLine->GetLineLength());
		pTextPos = cConv.GetConv()->GetConvData();
		nLineLength = cConv.GetConv()->GetConvTextSize() - sizeof(char);
		for (size_t i=0;i<nLineLength;i++)
		{
			if (nSize == 1)break;
			*pString = *pTextPos;
			pString ++;
			pTextPos ++;
			nSize --;
		}
		// 最後の行だったら抜ける
		if (iterLine == iterLastLine)
			break;
		// 改行コードを入れる
		if (nLineMode == LM_CRLF)
		{
			if (nSize < 3)break;
			*pString = '\r';
			pString++;
			*pString = '\n';
			pString++;
			nSize -= 2;
		}
		else if (nLineMode == LM_CR)
		{
			if (nSize < 2)break;
			*pString = '\r';
			pString++;
			nSize--;
		}
		else
		{
			if (nSize < 2)break;
			*pString = '\n';
			pString++;
			nSize--;
		 }
	}
	// 最後にNULLを入れる
	*pString = '\0';
	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/

//-----------------------------------------------------------------------------
/**
 * @brief 全文字列を取得します。
 * @param nID FootyのID番号
 * @param pString 格納先
 * @param nLineMode 改行コード
 * @param nSize pStringのバッファサイズ
 */
FOOTYEXPORT(int) Footy2GetTextW(int nID,wchar_t *pString,int nLineMode,int nSize)
{
	LinePt iterLine;
	LinePt iterLastLine;
	const wchar_t *pTextPos;
	size_t nLineLength;
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (!pString || nSize <= 0)return FOOTY2ERR_ARGUMENT;
	// 自動処理用に改行コードを変更する
	if (nLineMode == LM_AUTOMATIC)
	{
		nLineMode = pFooty->m_cDoc.GetLineMode();
	}
	// 文字列を挿入していく
	for (iterLine = pFooty->m_cDoc.GetTopLine(),
		 iterLastLine = pFooty->m_cDoc.GetLastLine();;
		 iterLine++)
	{
		// 行ごとに代入していく
		pTextPos = iterLine->GetLineData();
		nLineLength = iterLine->GetLineLength();
		for (size_t i=0;i<nLineLength;i++)
		{
		if (nSize == 1)break;
			*pString = *pTextPos;
			pString ++;
			pTextPos ++;
			nSize --;
		}
		// 最後の行だったら抜ける
		if (iterLine == iterLastLine)
			break;
		// 改行コードを入れる
		if (nLineMode == LM_CRLF)
		{
			if (nSize < 3)break;
			*pString = L'\r';
			pString++;
			*pString = L'\n';
			pString++;
			nSize -= 2;
		}
		else if (nLineMode == LM_CR)
		{
			if (nSize < 2)break;
			*pString = L'\r';
			pString++;
			nSize--;
		}
		else
		{
			if (nSize < 2)break;
			*pString = L'\n';
			pString++;
			nSize--;
		}
	}
	// 最後にNULLを入れる
	*pString = L'\0';
	return FOOTY2ERR_NONE;
}

#ifndef UNDER_CE
//-----------------------------------------------------------------------------
/**
 * @brief 文字列を取得します。
 * @param nID FootyのID番号
 * @param pString 格納先
 * @param nLineMode 改行コード
 * @param nSize pStringのバッファサイズ
 */
FOOTYEXPORT(int) Footy2GetSelTextA(int nID,char *pString,int nLineMode,int nSize){
	std::wstring strSelText;
	
	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (!pString || nSize <= 0)return FOOTY2ERR_ARGUMENT;
	
	// 選択文字列の取得
	if (!pFooty->m_cDoc.GetSelText(&strSelText, (LineMode)nLineMode))
		return FOOTY2ERR_NOTSELECTED;
	
	// 変換
	CConvFactory cConv;
	if (!cConv.GetConv()->ToMulti(strSelText.c_str(), (UINT)pFooty->m_cDoc.GetSelLength((LineMode)nLineMode)))
	{
		return FOOTY2ERR_MEMORY;
	}
	
	// コピー
	FOOTY2STRCPY(pString, cConv.GetConv()->GetConvData(), nSize);
	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/

//-----------------------------------------------------------------------------
/**
 * @brief 選択している文字列を取得します。
 * @param nID FootyのID番号
 * @param pString 格納先
 * @param nLineMode 改行コード
 * @param nSize pStringのバッファサイズ
 */
FOOTYEXPORT(int) Footy2GetSelTextW(int nID,wchar_t *pString,int nLineMode,int nSize){
	LinePt iterLine;
	LinePt iterEndLine;
	LinePt iterStartLine;
	const wchar_t *pTextPos;
	size_t nStartPos, nEndPos;

	// Footyを取得する
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (!pString || nSize <= 0)return FOOTY2ERR_ARGUMENT;

	// 自動処理用に改行コードを変更する
	if (nLineMode == LM_AUTOMATIC)
		nLineMode = pFooty->m_cDoc.GetLineMode();

	// 文字列を挿入していく
	for (iterStartLine = pFooty->m_cDoc.GetSelStart()->GetLinePointer(),
		 iterLine = iterStartLine,
		 iterEndLine = pFooty->m_cDoc.GetSelEnd()->GetLinePointer();;
		 iterLine++)
	{

		// 行ごとに代入していく
		nStartPos = iterLine == iterStartLine ? pFooty->m_cDoc.GetSelStart()->GetPosition() : 0;
		nEndPos = iterLine == iterEndLine ? pFooty->m_cDoc.GetSelEnd()->GetPosition() : iterLine->GetLineLength();

		pTextPos = &iterLine->GetLineData()[nStartPos];

		for (size_t i = nStartPos ; i < nEndPos; i++)
		{
			if (nSize == 1)break;
			*pString = *pTextPos;
			pString ++;
			pTextPos ++;
			nSize --;
		}
		 
		// 最後の行だったら抜ける
		if (iterLine == iterEndLine)
			break;

		// 改行コードを入れる
		if (nLineMode == LM_CRLF)
		{
			if (nSize < 3)break;
			*pString = L'\r';
			pString++;
			*pString = L'\n';
			pString++;
			nSize -= 2;
		}
		else if (nLineMode == LM_CR)
		{
			if (nSize < 2)break;
			*pString = L'\r';
			pString++;
			nSize--;
		}
		else
		{
			if (nSize < 2)break;
			*pString = L'\n';
			pString++;
			nSize--;
		}
	}
	// 最後にNULLを入れる
	*pString = L'\0';
	return FOOTY2ERR_NONE;
}

/*[EOF]*/
