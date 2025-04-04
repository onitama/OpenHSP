/**
 * @file FootyLineEmpSearch.cpp
 * @brief Footyの強調表示を検索するための処理です
 * @author Shinji Watanabe
 * @date Oct.30.2008
 */

#include "FootyLine.h"
#include "StaticStack.h"

#define MATCH_STR(s,n)	(pNowWord->IsNonCsWord() ? IsMatched(s,pNowLower,n) : IsMatched(s,pNowChar,n))

//-----------------------------------------------------------------------------
/**
 * @brief 強調表示文字列の検索
 * @param pBeforeBetween 前の行の持ち越し色分け
 * @param plsWords 強調表示文字列リスト
 * @return 変更に変化があった場合trueが返る
 */
bool CFootyLine::SearchEmphasis(TakeOver *pBeforeBetween, LsWords *plsWords)
{
	if (!plsWords)return false;
	// 宣言
	WordPt pNowWord;								//!< 現在走査中の強調表示文字列
	TakeOver::iterator pNowBefore;					//!< 前データセット用
	const wchar_t *pNowChar = GetLineData();		//!< 現在走査中の文字列
	size_t nStringLen = GetLineLength();			//!< 文字列の長さ
	CStaticStack<WordPt,sizeof(int)*8> cEmpStack;	//!< 現在色つけしているスタック
	EmpPos stInsert;								//!< 挿入する構造体
	
	// 初期化
	TakeOver vecBetweenBackup;
	vecBetweenBackup = m_vecLineBetweenAfter;
	m_vecColorInfo.clear();
	m_vecLineBetweenAfter.clear();

	// 前からのデータをセットする
	if (pBeforeBetween){									// データが存在するときのみ
		stInsert.m_nPosition = 0;
		stInsert.m_bIsStart = true;
		for (pNowBefore=pBeforeBetween->begin();pNowBefore!=pBeforeBetween->end();pNowBefore++){
			(*pNowBefore)->m_bDuplix = true;				// 重複チェック
			stInsert.m_Color = (*pNowBefore)->GetColor();	// コマンドをリストへ
			m_vecColorInfo.push_back(stInsert);
			cEmpStack.push(*pNowBefore);
		}
	}
	
	// 小文字化
	std::wstring strLower = GetLineData();
	const wchar_t *pNowLower = strLower.c_str();
	CEmphasisWord::SetLower(&strLower[0], nStringLen);

	// 検索していく
	for (size_t i=0;i<nStringLen;i++,pNowChar++,pNowLower++)
	{
		bool bWordSkipped = false;
		// 文字列を走査していく
		for (pNowWord=plsWords->begin();pNowWord!=plsWords->end();pNowWord++)
		{
			// この文字が無効であるかチェックする
			if (pNowWord->m_bDuplix)continue;						// 重複チェック
			if (i != 0)
			{
				if (pNowWord->IsOnlyHead())continue;				// 先頭に限る
				if (!pNowWord->CheckIndependence(pNowChar-1,false))	// 独立性チェック(前)
					continue;
			}
			if (cEmpStack.size() == 0)
			{
				if (!pNowWord->IsPermitted(0))continue;
			}
			else
			{
				if (!pNowWord->IsPermitted(cEmpStack.top()->GetLevel()))continue;
			}
			if (nStringLen - i < pNowWord->GetLen1())				// 文字の長さは十分か？
			{
				continue;
			}

			if (nStringLen - i != pNowWord->GetLen1())
			{
				if (!pNowWord->CheckIndependence
					(pNowChar+pNowWord->GetLen1(),true))			// 独立性チェック(後)
					continue;
			}
			
			// 文字が一致するかチェック
			if (!MATCH_STR(pNowWord->GetString1(),pNowWord->GetLen1()))
				continue;

			// 開始コマンドを挿入する
			stInsert.m_nPosition = i;
			stInsert.m_Color = pNowWord->GetColor();
			stInsert.m_bIsStart = true;
			m_vecColorInfo.push_back(stInsert);

			// インデックス番号を先へ移動させる
			i += pNowWord->GetLen1() - 1;
			pNowChar += pNowWord->GetLen1() - 1;
			pNowLower+= pNowWord->GetLen1() - 1;
			bWordSkipped = true;

			// 強調表示情報によって場合分け
			if (pNowWord->GetType() == EMP_WORD){
				// 単語の時は、それの終了コマンドを挿入する
				stInsert.m_nPosition = i + 1;
				stInsert.m_bIsStart = false;
				m_vecColorInfo.push_back(stInsert);
			}
			else{
				// スタックを積む
				pNowWord->m_bDuplix = true;
				cEmpStack.push(pNowWord);
			}
			// 次の文字のループへ移動する
			goto To_NextLoop;
		}
		
		//終了をチェック
		if (cEmpStack.size()){
			pNowWord = cEmpStack.top();
			// 二個目の単語当たりをチェック
			if ((pNowWord->GetType() == EMP_MULTI_BETWEEN || pNowWord->GetType() == EMP_LINE_BETWEEN) && 
				nStringLen - i >= pNowWord->GetLen2()){
				if (MATCH_STR(pNowWord->GetString2(),pNowWord->GetLen2())){
					// インデックス番号を先へ移動させる
					i += pNowWord->GetLen2() - 1;
					pNowChar  += pNowWord->GetLen2() - 1;
					pNowLower += pNowWord->GetLen2() - 1;
					bWordSkipped = true;
					// 終了コマンドを挿入する
					stInsert.m_nPosition = i + 1;
					stInsert.m_bIsStart = false;
					m_vecColorInfo.push_back(stInsert);
					// スタックを排除
					pNowWord->m_bDuplix = false;
					cEmpStack.pop();
				}
			}
		}
		// 次のループへ
To_NextLoop:;
		// サロゲートペアの１文字目で、強調表示が見つからなかったときはさらに一つ進めておく
		if (!bWordSkipped && IsSurrogateLead(*pNowChar)){
			pNowChar++;i++;
		}
	}
	
	// 次のベクトルへ
	for (;cEmpStack.size();){
		if (cEmpStack.top()->GetType() == EMP_MULTI_BETWEEN)
			m_vecLineBetweenAfter.push_back(cEmpStack.top());
		cEmpStack.top()->m_bDuplix = false;
		cEmpStack.pop();
	}

	m_bEmphasisChached = true;
	return !(vecBetweenBackup == m_vecLineBetweenAfter);
}

/*-------------------------------------------------------------------
CFootyLine::IsMatched
文字列pStr1とpStr2の文字がnLen個が一致しているかどうかの判断です。
-------------------------------------------------------------------*/
bool CFootyLine::IsMatched(const wchar_t *pStr1,const wchar_t *pStr2,size_t nLen){
#ifdef UNDER_CE
	/*ループして検査*/
	for (size_t i=0;i<nLen;i++,pStr1++,pStr2++){
		if ((*pStr1) != (*pStr2))return false;
	}
	return true;
#else	/*UNDER_CE*/
	/*宣言*/
	bool bIsOddNum = (nLen & 1) == 1;
	if (bIsOddNum && pStr1[nLen-1] != pStr2[nLen-1])return false;
	size_t nLoopNum = nLen >> 1;
	/*ループして文字列が一致しているかチェックする*/
	unsigned long* pStrLong1 = (unsigned long*)pStr1;
	unsigned long* pStrLong2 = (unsigned long*)pStr2;
	for (size_t i=0;i<nLoopNum;i++,pStrLong1++,pStrLong2++){
		if ((*pStrLong1) != (*pStrLong2))return false;
	}
	//if (bIsOddNum && pStr1[nLen-1] != pStr2[nLen-1])return false;
	return true;
#endif	/*UNDER_CE*/	
}

/*[EOF]*/
