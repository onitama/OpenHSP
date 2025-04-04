/**
 * @file TextConverter.cpp
 * @brief テキストの変換処理が可能なクラスの基底
 * @author Shinji Watanabe
 * @version 1.0
 * @date Jan.07.2008
 */

#include "TextConverter.h"

/*----------------------------------------------------------------
CTextConverter::CTextConverter
コンストラクタです。
----------------------------------------------------------------*/
CTextConverter::CTextConverter(){
	m_pOutputText = NULL;
	m_nTextSize = 0;
}

/*----------------------------------------------------------------
CTextConverter::~CTextConverter
デストラクタです。
----------------------------------------------------------------*/
CTextConverter::~CTextConverter(){
	Free();
}

/**
 * CTextConverter::Alloc
 * @brief メモリを割り当てます
 * @param nSize 確保するバイト数
 * @return 確保できたときtrue
 */
bool CTextConverter::Alloc(size_t nSize){
	Free();
	m_pOutputText = new char[nSize];
	if (!m_pOutputText)return false;
	m_nTextSize = nSize;
	return true;
}

/*----------------------------------------------------------------
CTextConverter::Free
メモリを解放します。
----------------------------------------------------------------*/
void CTextConverter::Free(){
	if (m_pOutputText){
		delete [] m_pOutputText;
		m_pOutputText = NULL;
	}
}

/*[EOF]*/
