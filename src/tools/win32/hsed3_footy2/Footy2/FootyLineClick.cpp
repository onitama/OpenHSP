/**
 * @file FootyLineClick.cpp
 * @brief クリッカブル系の処理を行います。
 * @author Shinji Watanabe
 * @version 1.0
 * @date Jan.08.2008
 */

#include "FootyLine.h"

/**
 * CFootyLine::FindURL
 * @brief URL文字列を検索します。
 * @param start [in] 開始位置
 * @param pBegin [out] 見つかった開始位置
 * @param pEnd [out] 見つかった終了位置
 */
bool CFootyLine::FindURL(size_t start,size_t *pBegin,size_t *pEnd)
{
	// 宣言
	const wchar_t *pLineData = GetLineData() + start;
	size_t nTextLen = GetLineLength();
	size_t len = -1;
	// 検索開始
	for (size_t i=start;i<nTextLen;i++,pLineData++)
	{
		// 比較
		if (5 <= nTextLen-i && IsMatched(pLineData,L"http:",5))len=5;
		if (6 <= nTextLen-i && IsMatched(pLineData,L"https:",6))len=6;
		if (4 <= nTextLen-i && IsMatched(pLineData,L"ftp:",4))len=4;
		
		// 一致したとき
		if (len!=-1)
		{
			*pBegin = i;
			for (pLineData+=len,i+=len;i<nTextLen;i++,pLineData++)
			{
				if (!IsURLChar(*pLineData))	// URLの文字の適合性をチェック
				{
					*pEnd = i;
					return true;
				}
			}
			*pEnd = nTextLen;
			return true;
		}
	}
	*pBegin = *pEnd = -1;
	return false;
}

/**
 * CFootyLine::FindMail
 * @brief メールアドレス文字列を検索します。
 * @param start [in] 開始位置
 * @param pBegin [out] 見つかった開始位置
 * @param pEnd [out] 見つかった終了位置
 */
bool CFootyLine::FindMail(size_t start,size_t *pBegin,size_t *pEnd)
{
	// 宣言
	int work=-1;										//!< 検出用
	bool bDotFound;										//!< ドットが見つかったか
	size_t i,j;
	const wchar_t *pLineData = GetLineData() + start;	//!< 高速化用
	const wchar_t *pWork;
	size_t nTextLen = GetLineLength();

	// 検索
	for (i = start;i<nTextLen;i++,pLineData++)			// 最後から一つ前まで
	{
		bDotFound = false;
		if (*pLineData == L'@')							// ＠マーク発見時
		{
			if (i == 0)continue;						// それ以前が無い場合は次へ
			for (j=i-1,pWork=pLineData-1;;pWork--,j--)	// それ以前を検索していく
			{
				if (j == -1)
				{
					*pBegin=0;
					break;
				}
				if (!IsMailChar(*pWork))				// メール文字列として不適切な場合
				{
					if (j == i - 1)goto NextI;			// ＠マーク以前になかったら次へ
					*pBegin = j + 1;
					break;
				}
			}
			for (j=i+1,pWork=pLineData+1;;pWork++,j++)	// ＠マーク以降を検索
			{
				if (j == nTextLen)
				{
					*pEnd = nTextLen;
					if (bDotFound)
						return true;
					goto NextI;
				}
				if (!IsMailChar(*pWork))
				{
					if (i == j+1)goto NextI;			// それ以降の文字列が見つからない
					*pEnd = j;
					if (bDotFound)
						return true;
					goto NextI;
				}
				if (*pWork == L'.')						// @以降に.が見つかった
					bDotFound = true;
			}
		}
NextI:;
	}
	*pBegin = *pEnd = -1;
	return false;
}

/**
 * CFootyLine::FindLabel by Tetr@pod
 * @brief ラベル文字列を検索します。
 * @param start [in] 開始位置
 * @param pBegin [out] 見つかった開始位置
 * @param pEnd [out] 見つかった終了位置
 */
bool CFootyLine::FindLabel(size_t start,size_t *pBegin,size_t *pEnd)
{
	FOOTY2_PRINTF(L"FindLabel Start\n");
	// 宣言
	const wchar_t *pLineData = GetLineData() + start;
	size_t nTextLen = GetLineLength();
	bool IsLabelDefination = true;// ラベル本体(gotoとかのじゃない、定義部)か？
	// 検索開始
	for (size_t i=start;i<nTextLen;i++,pLineData++)
	{
		if (IsLabelDefination && *pLineData != L'*' && ( *pLineData != L'\t' && *pLineData != L' ' && *pLineData != L':' && *pLineData != L'{' && *pLineData != L'}' )) {
			// ラベル本体ではない
			IsLabelDefination = false;
		}
		if (*pLineData == L'*')							// ＊マーク発見時
		{
			if (IsLabelDefination || i == 0) {// ラベル本体だった
				// 見つからなかった
				*pBegin = i - 1;// もっと良い抜け方があるはずだけど…
				break;
			}
			FOOTY2_PRINTF(L"FindLabel Find '*'\n");
			*pBegin = i;
			const wchar_t *pLabelStart = pLineData - 1;
			for (pLineData++,i++;i<nTextLen;i++,pLineData++)
			{
				if (!IsLabelChar(*pLineData))	// ラベルの文字の適合性をチェック
				{
					if (*pBegin == i - 1) {// *だけのものはラベルではない
						break;
					}
					// 直前の文字が ":"、" "、"\t"、"\n"、"//"、"/ *"、"* /"、";"、","、"{"、"}" 以外のものはラベルではない
					if (*pLabelStart == L':' || *pLabelStart == L' ' || 
						*pLabelStart == L'\t' || *pLabelStart == L'\n' || 
						( *pBegin > 1 && ( ( *(pLabelStart-1) == L'/' && 
						( *pLabelStart == L'/' || *pLabelStart == L'*' ) ) ||
						( *(pLabelStart-1) == L'*' && *pLabelStart == L'/' ) ) ) ||
						*pLabelStart == L';' || *pLabelStart == L',' ||
						*pLineData == L'{' || *pLineData == L'}') {

							// 直後の文字が ":"、" "、"\t"、"\r"、"//"、"/ *"、"* /"、";"、","、"{"、"}" 以外のものはラベルではない
							if (*pLineData == L':' || *pLineData == L' ' || 
								*pLineData == L'\t' || *pLineData == L'\r' || 
								( i + 1 < nTextLen && ( ( *pLineData == L'/' && 
								( *(pLineData+1) == L'/' || *(pLineData+1) == L'*' ) ) ||
								( *pLineData == L'*' && *(pLineData+1) == L'/' ) ) ) ||
								*pLineData == L';' || *pLineData == L',' ||
								*pLineData == L'{' || *pLineData == L'}') {
									*pEnd = i;
									return true;
							}
					}

					// 見つからなかった
					*pBegin = i - 1;// もっと良い抜け方があるはずだけど…
					break;
				}
			}

			// 見つからなかった
			if (*pBegin == i - 1) {
				break;
			}

			*pEnd = nTextLen;
			return true;
		}
	}
	*pBegin = *pEnd = -1;
	FOOTY2_PRINTF(L"FindLabel Finish on false\n");
	return false;
}

/**
 * CFootyLine::FindFunc by Tetr@pod
 * @brief ユーザー定義命令・関数などを検索します。
 *        ただし、やっていることはただの検索です。
 *        頻繁に呼び出されます。
 *        高速化できる方はしてください＞＜
 *        ちなみにアルゴリズムはBM法のはず、です。
 * @param start [in] 開始位置
 * @param pBegin [out] 見つかった開始位置
 * @param pEnd [out] 見つかった終了位置
 * @param SearchWord [in] 検索する命令・関数名
 * @param bIsFunction [in] それは関数か(trueで関数)
 * @param bMatchCase [in] 大文字小文字を区別するか
 */
bool CFootyLine::FindFunc(size_t start,size_t *pBegin,size_t *pEnd, wchar_t *SearchWord, bool bIsFunction, bool bMatchCase)
{
	FOOTY2_PRINTF(L"FindFunc Start\n");

#if 0
	// BM法で検索

	// 宣言
	const wchar_t *pLineData = GetLineData() + start;
	size_t nTextLen = GetLineLength();
	
	int skip[65536];
	int i, j;
	int pat_len = wcslen(SearchWord);
	
	for(i = 0; i < 65536; i++) {
		skip[i] = pat_len;
	}
	for(i = 0; i < pat_len - 1; i++) {
		skip[*(SearchWord + i)] = pat_len - i - 1;
	}

	int count=0;
	i = start + pat_len - 1;
	
	while(i < nTextLen){
		j = pat_len - 1;
		count++;
		while(true){
			if (bMatchCase) {
				if (*(pLineData + i) != *(SearchWord + j)) {
					break;
				}
			}
			else {
				if (towlower(*(pLineData + i)) != towlower(*(SearchWord + j))) {
					break;
				}
			}
			if (j == 0) {
# if 0
				// 前後の文字をチェックしない場合
				*pBegin = i;
				*pEnd = i + pat_len;
				return true;
# else
				// 前の文字が :;+-/\*=( ^|&<>,? 以外のものはだめ
				if ( i == 0 || (
					*(pLineData + i - 1) == L':'  || *(pLineData + i - 1) == 0x20 || 
					*(pLineData + i - 1) == L'+'  || *(pLineData + i - 1) == 0x09 || 
					*(pLineData + i - 1) == L'-'  || *(pLineData + i - 1) == 0x0a || 
					*(pLineData + i - 1) == L'*'  || *(pLineData + i - 1) == L'/' || 
					*(pLineData + i - 1) == L'\\' || *(pLineData + i - 1) == L'=' || 
					*(pLineData + i - 1) == L'('  || *(pLineData + i - 1) == L'^' || 
					*(pLineData + i - 1) == L'|'  || *(pLineData + i - 1) == L'&' || 
					*(pLineData + i - 1) == L'<'  || *(pLineData + i - 1) == L'>' || 
					*(pLineData + i - 1) == L'?'  || *(pLineData + i - 1) == L',' ||
					*(pLineData + i - 1) == L';') ) {
	
						if (bIsFunction) {
							// 関数なら、次の文字が @() ;//  / *  * /  \t\r 以外のものはだめ
							if (i + pat_len >= nTextLen || (
								*(pLineData + i + pat_len) == L'@' || *(pLineData + i + pat_len) == 0x20 || 
								*(pLineData + i + pat_len) == 0x09 || *(pLineData + i + pat_len) == 0x0d || 
								( i + pat_len + 1 < nTextLen && ( ( *(pLineData + i + pat_len) == L'/' && 
								( *(pLineData + i + pat_len + 1) == L'/' || *(pLineData + i + pat_len + 1) == L'*' ) ) ||
								( *(pLineData + i + pat_len) == L'*' && *(pLineData + i + pat_len + 1) == L'/' ) ) ) ||
								*(pLineData + i + pat_len) == L'(' || *(pLineData + i + pat_len) == L')' || 
								*(pLineData + i + pat_len) == L';')) {
									*pBegin = i;
									*pEnd = i + pat_len;
									return true;
							}
						}
						else {
							// 命令なら、次の文字が @$):} ;//  / *  * /  \t\r 以外のものはだめ
							if (i + pat_len >= nTextLen || (
								*(pLineData + i + pat_len) == L'@' || *(pLineData + i + pat_len) == 0x20 || 
								*(pLineData + i + pat_len) == 0x09 || *(pLineData + i + pat_len) == 0x0d || 
								( i + pat_len + 1 < nTextLen && ( ( *(pLineData + i + pat_len) == L'/' && 
								( *(pLineData + i + pat_len + 1) == L'/' || *(pLineData + i + pat_len + 1) == L'*' ) ) ||
								( *(pLineData + i + pat_len) == L'*' && *(pLineData + i + pat_len + 1) == L'/' ) ) ) ||
								*(pLineData + i + pat_len) == L';' || *(pLineData + i + pat_len) == L')' || 
								*(pLineData + i + pat_len) == L'$' || *(pLineData + i + pat_len) == L':' || 
								*(pLineData + i + pat_len) == L'}')) {
									*pBegin = i;
									*pEnd = i + pat_len;
									return true;
							}
						}

				}
				break;
# endif
			}
			i--;
			j--;
			count++;
		}
		i += max(skip[*(pLineData + i)], pat_len - j);
	}


#else
	// 標準関数(wcsstr)で検索

	// 宣言
	const wchar_t *pLineData = GetLineData() + start;
	size_t nTextLen = GetLineLength();
	int pat_len = wcslen(SearchWord);
	if (nTextLen < 1 || pat_len < 1) {
		FOOTY2_PRINTF(L"FindFunc invalid length.");
		return false;
	}
	
	// 検索用文字列の宣言(大文字小文字の変換とかやるため)
	wchar_t *pLineData_ForSearch;// 検索用検索元文字列
	wchar_t *SearchWord_ForSearch;// 検索用検出文字列
	if (bMatchCase) {
		// 大文字小文字の比較をするならそのままポインタをコピーする
		pLineData_ForSearch  = (wchar_t *) pLineData;// あまりよくない。
		SearchWord_ForSearch = SearchWord;
	}
	else {
		// 大文字小文字の比較をしないなら文字列をコピーし、小文字化する

		// 確保して
		pLineData_ForSearch  = (wchar_t *)malloc(sizeof(pLineData_ForSearch ));
		SearchWord_ForSearch = (wchar_t *)malloc(sizeof(SearchWord_ForSearch));

		// エラー処理して
		if (pLineData_ForSearch == NULL | SearchWord_ForSearch == NULL ) {
			if (pLineData_ForSearch  != NULL) {
				free(pLineData_ForSearch );
			}
			if (SearchWord_ForSearch != NULL) {// ちゃんと書くこと。あり得るから。
				free(SearchWord_ForSearch);
			}

			FOOTY2_PRINTF(L"!! FindFunc allocation error!");

			return false;
		}

		// コピーして
		memcpy(pLineData_ForSearch , pLineData , sizeof(pLineData ));
		memcpy(SearchWord_ForSearch, SearchWord, sizeof(SearchWord));

		// 小文字化する
		for(int i=0; i<nTextLen; i++) {
			if (pLineData_ForSearch[i] >= L'A' && pLineData_ForSearch[i] <= L'Z') {
				pLineData_ForSearch[i] += 32;// 'a' - 'A'
			}
		}
		for(int i=0; i<pat_len; i++) {
			if (SearchWord_ForSearch[i] >= L'A' && SearchWord_ForSearch[i] <= L'Z') {
				SearchWord_ForSearch[i] += 32;// 'a' - 'A'
			}
		}
	}

	const wchar_t *ptr = pLineData_ForSearch;
	for ( ptr = wcsstr(ptr, SearchWord_ForSearch); ptr != NULL; ptr = wcsstr(ptr, SearchWord_ForSearch)) {

# if 0
		// 前後の文字をチェックしない場合
		*pBegin = ptr - pLineData_ForSearch;
		*pEnd = ptr - pLineData_ForSearch + pat_len;
		return true;
# else
		// 前の文字が :;+-/\*=( ^|&<>,? 以外のものはだめ
		/*if ( ptr - pLineData_ForSearch + start == 0 || (
			*(ptr - 1) == L':'  || *(ptr - 1) == 0x20 || 
			*(ptr - 1) == L'+'  || *(ptr - 1) == 0x09 || 
			*(ptr - 1) == L'-'  || *(ptr - 1) == 0x0a || 
			*(ptr - 1) == L'*'  || *(ptr - 1) == L'/' || 
			*(ptr - 1) == L'\\' || *(ptr - 1) == L'=' || 
			*(ptr - 1) == L'('  || *(ptr - 1) == L'^' || 
			*(ptr - 1) == L'|'  || *(ptr - 1) == L'&' || 
			*(ptr - 1) == L'<'  || *(ptr - 1) == L'>' || 
			*(ptr - 1) == L'?'  || *(ptr - 1) == L',' ||
			*(ptr - 1) == L';') ) {*/

		if (ptr - pLineData_ForSearch + start == 0 || (// 0-9 A-Z a-z " ' # _ 以外ならOKにする
			( *(ptr - 1) <  L'0'  || *(ptr - 1) >  L'9'  ) && 
			( *(ptr - 1) <  L'A'  || *(ptr - 1) >  L'Z'  ) && 
			( *(ptr - 1) <  L'a'  || *(ptr - 1) >  L'z'  ) &&
			  *(ptr - 1) != L'\"' && *(ptr - 1) != L'\''   &&
			  *(ptr - 1) != L'#'  && *(ptr - 1) != L'_'    ))
			{
				/*bool FunctionFound = false;// 命令・関数は見つかった？

				if (bIsFunction) {// 関数の時の文字チェック
					// 関数なら、次の文字が @() ;//  / *  * /  \t\r 以外のものはだめ
					if (ptr - pLineData_ForSearch + start + pat_len >= nTextLen || (
						*(ptr + pat_len) == L'(' || *(ptr + pat_len) == L')' || 
						*(ptr + pat_len) == L';' || 
						*(ptr + pat_len) == L'@' || *(ptr + pat_len) == 0x20 || 
						*(ptr + pat_len) == 0x09 || *(ptr + pat_len) == 0x0d || 
						( ptr - pLineData_ForSearch + start + pat_len + 1 < nTextLen && ( ( *(ptr + pat_len) == L'/' && 
						( *(ptr + pat_len + 1) == L'/' || *(ptr + pat_len + 1) == L'*' ) ) ||
						( *(ptr + pat_len) == L'*' && *(ptr + pat_len + 1) == L'/' ) ) ) ) )
						{
							FunctionFound = true;
					}
				}
				else {// 命令の時の文字チェック
					// 命令なら、次の文字が @$):} ;//  / *  * /  \t\r 以外のものはだめ
					if (ptr - pLineData_ForSearch + start + pat_len >= nTextLen || (
						*(ptr + pat_len) == L'@' || *(ptr + pat_len) == 0x20 || 
						*(ptr + pat_len) == 0x09 || *(ptr + pat_len) == 0x0d || 
						( ptr - pLineData_ForSearch + start + pat_len + 1 < nTextLen && ( ( *(ptr + pat_len) == L'/' && 
						( *(ptr + pat_len + 1) == L'/' || *(ptr + pat_len + 1) == L'*' ) ) ||
						( *(ptr + pat_len) == L'*' && *(ptr + pat_len + 1) == L'/' ) ) ) ||
						*(ptr + pat_len) == L';' || *(ptr + pat_len) == L')' || 
						*(ptr + pat_len) == L'$' || *(ptr + pat_len) == L':' || 
						*(ptr + pat_len) == L'}')) {
							FOOTY2_PRINTF(L"FindFunc Finish on true. It's NOT function, %d >= %d\n", ptr - pLineData_ForSearch + start + pat_len, nTextLen);
							FunctionFound = true;
					}
				}

				if (FunctionFound) {// 見つかったなら、*/

				if (ptr - pLineData_ForSearch + start + pat_len >= nTextLen || (// 0-9 A-Z a-z " ' # _ 以外ならOKにする
					( *(ptr + pat_len) <  L'0'  || *(ptr + pat_len) >  L'9'  ) && 
					( *(ptr + pat_len) <  L'A'  || *(ptr + pat_len) >  L'Z'  ) && 
					( *(ptr + pat_len) <  L'a'  || *(ptr + pat_len) >  L'z'  ) && 
					  *(ptr + pat_len) != L'\"' && *(ptr + pat_len) != L'\''   &&
					  *(ptr + pat_len) != L'#'  && *(ptr + pat_len) != L'_'    ))
					{
						*pBegin = ptr - pLineData_ForSearch + start;
						*pEnd = ptr - pLineData_ForSearch + pat_len + start;
						return true;
				}

		}
# endif
		if (ptr - pLineData_ForSearch + start + pat_len >= nTextLen - 1) {
			// もう見つかることはないだろう
			break;
		}
		ptr += 2;
	}
#endif

	*pBegin = *pEnd = -1;
	FOOTY2_PRINTF(L"FindFunc Finish on false\n");
	return false;
}


/**
 * CFootyLine::IsMailChar
 * @brief メールアドレスの文字として適正か判定
 * @param c 調べる文字
 * @return 適切な場合trueを返します。
 */
bool CFootyLine::IsMailChar(wchar_t c)
{
	if (c==L'~'  || (97 <=c && c <= 122) || // a～z
		c==L'_'  || (65 <=c && c <= 90)  || // ?,A～Z
		c==L'\\' || (45 <=c && c <= 58)  || // 数字、ピリオド、ハイフン、スラッシュ、コロン
		c==L'='  || (36 <=c && c <= 38)  ||
		c==L'&'  || c==L'%' || c==L'#' || c==L'+')
		return true;
	else
		return false;
}

/**
 * CFootyLine::IsURLChar
 * @brief URLの文字として適正のあるものか調べます。
 * @param c 調べる文字
 * @return 適切な場合trueを返します。
 */
bool CFootyLine::IsURLChar(wchar_t c)
{
	if (c==L'~'  || (97 <=c && c <= 122) ||
		c==L'_'  || (65 <=c && c <= 90)  ||
		c==L'!'  || (44 <=c && c <= 59)  ||
		c==L'='  || (36 <=c && c <= 38)  ||
		c==L'*'  || c==L'?' || c==L'#')
		return true;
	else
		return false;
}

/**
 * CFootyLine::IsLabelChar by Tetr@pod
 * @brief ラベルの文字として適正のあるものか調べます。
 * @param c 調べる文字
 * @return 適切な場合trueを返します。
 */
bool CFootyLine::IsLabelChar(wchar_t c)
{
	if ((97 <=c && c <= 122) || // a-z
		(65 <=c && c <= 90)  || // A-Z
		(48 <=c && c <= 57)  || // 0-9
		(c  == L'_'))           // "_"
		return true;
	else
		return false;
}

/*[EOF]*/
