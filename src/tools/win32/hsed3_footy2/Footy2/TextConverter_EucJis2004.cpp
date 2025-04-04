/*===================================================================
CTextConverter_eucJP_msクラス
テキストの変換処理が可能なクラスです。
===================================================================*/

#include "TextConverter_Japanese.h"
#include "UnicodeTable.h"

/*----------------------------------------------------------------
CTextConverter_EucJis2004::ToUnicode
Unicodeに変換する処理
----------------------------------------------------------------*/
bool CTextConverter_EucJis2004::ToUnicode(const char* pChar,size_t nSize){
	size_t i;
	unsigned char c1,c2,c3;
	int nAllocSize = 0;
	int nUnicodePos = 0;
	CUnicodeTable_JIS2004_1PLANE TableP1;
	CUnicodeTable_JIS2004_2PLANE TableP2;
	
	/*ステップ１：サイズを求める*/
	for (i=0;i<nSize;i++){
		c1 = (unsigned char)pChar[i];
		/*1バイト文字か*/
		if		(IsAscii(c1)){
			nAllocSize += sizeof(wchar_t);
		}
		/*2バイト文字(半角カナ)*/
		else if (i + 1 < nSize && IsKana_Lead(c1)){
			c2 = (unsigned char)pChar[i+1];
			if (IsKana_Tail(c2)){
				i++;
				nAllocSize += sizeof(wchar_t);
			}
		}
		/*2バイト文字*/
		else if ((i + 1 < nSize) && IsTwoBytes(c1)){
			c2 = (unsigned char)pChar[i+1];
			if (IsTwoBytes(c2)){
				i++;
				nAllocSize += sizeof(wchar_t) * 2;
			}
		}
		/*3バイト文字か*/
		else if (i + 2 < nSize && IsThreeBytes_Lead(c1)){
			c2 = (unsigned char)pChar[i+1];
			c3 = (unsigned char)pChar[i+2];
			if (IsTwoBytes(c2) && IsTwoBytes(c3)){ 
			   i += 2;
			   nAllocSize += sizeof(wchar_t)*2;
			}
		}
	}
	
	/*ステップ２：メモリを確保*/
	if (!Alloc(nAllocSize + sizeof(wchar_t)))
		return false;
	
	/*ステップ３：変換*/
	for (i=0;i<nSize;i++){
		c1 = (unsigned char)pChar[i];
		/*JIS X 0201 ラテン文字*/
		if		(IsAscii(c1)){
			((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)c1;
			nUnicodePos++;
		}
		/*2バイト文字(半角カナ)*/
		else if (i + 1 < nSize && IsKana_Lead(c1)){
			c2 = (unsigned char)pChar[i+1];
			if (IsKana_Tail(c2)){
				((wchar_t*)m_pOutputText)[nUnicodePos] = (wchar_t)(0xFF61 + c1 - 0xA1);
				nUnicodePos++;
				i++;
			}
		}
		/*2バイト文字(JIS X 2028またはNEC特殊文字)*/
		else if ((i + 1 < nSize) && IsTwoBytes(c1)){
			c2 = (unsigned char)pChar[i+1];
			if (IsTwoBytes(c2)){
				unsigned short s = (c1 << 8) | c2;
				s -= 0x8080;
				unsigned ucs4 = TableP1.ToUnicode(s);
				if (ucs4 != 0){
					ToSurrogate(ucs4,nUnicodePos);
					i++;
				}
			}
		}
		/*3バイト文字か*/
		else if (i + 2 < nSize && IsThreeBytes_Lead(c1)){
			c2 = (unsigned char)pChar[i+1];
			c3 = (unsigned char)pChar[i+2];
			if (IsTwoBytes(c2) && IsTwoBytes(c3)){
				unsigned short s = (c2 << 8) | c3;
				s -= 0x8080;
				unsigned ucs4 = TableP1.ToUnicode(s);
				if (ucs4 != 0){
					ToSurrogate(ucs4,nUnicodePos);
					i += 2;
				}
			}
		}
	}
	
	/*ステップ４：NULL文字を書き込む*/
	((wchar_t*)m_pOutputText)[nUnicodePos] = L'\0';

	/*テキストのサイズを適当に求めたので、最後に帳尻合わせ*/
	m_nTextSize = nUnicodePos + sizeof(wchar_t);
	return true;
}
/*----------------------------------------------------------------
CTextConverter_EucJis2004::ToMulti
eucJP-msに変換する処理
----------------------------------------------------------------*/
bool CTextConverter_EucJis2004::ToMulti(const wchar_t *pChar,size_t nSize){
	size_t i;
	wchar_t w;
	int nAllocSize = 0;
	int nEucPos = 0;

	CUnicodeTable_JIS2004_1PLANE TableP1;
	CUnicodeTable_JIS2004_2PLANE TableP2;

	/*ステップ１：サイズを求める*/
	for (i=0;i<nSize;i++){
		w = pChar[i];
		/*ASCII文字*/
		if		(0x0000 <= w && w <= 0x007F){
			nAllocSize += sizeof(char);
		}
		/*半角カナ*/
		else if (0xFF61 <= w && w <= 0xFF9F){
			nAllocSize += sizeof(char)*2;
		}
		/*それ以外はとりあえず3バイト確保しておく*/
		else {
			nAllocSize += sizeof(char)*3;
			if (IsSurrogateLead(w))i++;
		}
	}
	
	/*ステップ２：メモリを確保*/
	if (!Alloc(nAllocSize + sizeof(char)))
		return false;
	
	/*ステップ３：変換処理*/
	for (i=0;i<nSize;i++){
		w = pChar[i];
		/*JIS X 0201 ラテン文字*/
		if		(0x0000 <= w && w <= 0x007F){
			m_pOutputText[nEucPos] = (char)w;
			nEucPos++;
		}
		/*JIS X 0201 半角カタカナ*/
		else if (0xFF61 <= w && w <= 0xFF9F){
			m_pOutputText[nEucPos + 0] = (char)0x8E;
			m_pOutputText[nEucPos + 1] = (char)(w - 0xFF61 + 0xA1);
			nEucPos += 2;
		}
		/*検索して探す*/
		else{
			/*１文字分を取得する*/
			unsigned long ucs4;
			if (IsSurrogateLead(w) && i + 1 < nSize){
				ucs4 = ToUCS4(&pChar[i]);
				i++;
			}
			else ucs4 = (unsigned long)w;
			/*検索する*/
			unsigned short euc;
			if ((euc = TableP1.ToCode(ucs4)) != 0){			/*JIS X 0213:2004一面*/
				euc += 0x8080;
				m_pOutputText[nEucPos + 0] = (char)(euc >> 8);
				m_pOutputText[nEucPos + 1] = (char)(euc & 0xFF);
				nEucPos += 2;
			}
			else if ((euc = TableP2.ToCode(ucs4)) != 0){	/*JIS X 0213:2004二面*/
				euc += 0x8080;
				m_pOutputText[nEucPos + 0] = (char)0x8F;
				m_pOutputText[nEucPos + 1] = (char)(euc >> 8);
				m_pOutputText[nEucPos + 2] = (char)(euc & 0xFF);
				nEucPos += 3;
			}
		}
	}
	
	/*ステップ４：NULL文字を書き込む*/
	m_pOutputText[nEucPos] = '\0';
	
	/*テキストのサイズを適当に求めたので、最後に帳尻合わせ*/
	m_nTextSize = nEucPos + sizeof(char);
	return true;
}


/*[EOF]*/
