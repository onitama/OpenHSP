/*--------------------------------------------------------
   POPFIND.C -- Popup Editor Search and Replace Functions
  --------------------------------------------------------*/

// bool selectは、発見時にその部分を選択するかです。
// trueで選択し、falseで選択しません。
// 従来の動作は、 true です。

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <comdef.h>
#include <string.h>
#include <mbstring.h>
#include <ctype.h>
#include <wctype.h>
#include <string>
#include "vbsregexp.h"
#include "Footy2.h"
#include "resource.h"

#define MAX_STRING_LEN   512

#define FR_UP            0

#define IDC_MATCHCASE    1041
#define IDC_ESCSEQ       1042
#define IDC_UP           1056
#define IDC_DOWN         1057

static char szFindText [MAX_STRING_LEN] ;
static char szReplText [MAX_STRING_LEN] ;
static int repl_flag;

extern int activeFootyID;
extern HINSTANCE hInst;

typedef struct tagFindReplaceString{
	char *ptr;
	int offset;
	int length;
	int working;
} FRSTRING;

typedef struct __FindRet{
	int offset;
	long length;
	bool success;
} FINDRET;

typedef struct tagFRCustomData{
    int Mode;
	bool EscSeq;
	int plength;
	DWORD Flags;
} FRCUSTDATA;
static FRCUSTDATA frcd = {0, false, 0, FR_DOWN};

_COM_SMARTPTR_TYPEDEF(IRegExp, __uuidof(IRegExp));

//-----------------------------------------------------------

/*
char *strfind( char *srcstr, char *findstr )
{
	char a1,a2,c;
	char *p1;

	a1=findstr[0];
	if (a1==0) return NULL;
	a2=findstr[1];
	if (a2!=0) {
		return strstr ( srcstr, findstr );
	}
	p1=srcstr;
	while(1) {
		c=*p1;
		if (c==0) return NULL;
		if ((c>=129)&&(c<=159)) { c=0;p1++; }
		if ((c>=224)&&(c<=252)) { c=0;p1++; }
		if (c==a1) break;
		p1++;
	}
	return p1;
}
*/

//-----------------------------------------------------------

// 検索と置換ダイアログのフック プロシージャ
// Hook procedure for find and replace dialog
int CALLBACK FRHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg){
		case WM_INITDIALOG:
		{
			// 検索ダイアログのウィンドウハンドルをオーナーへ通知
			const auto iMsgNotifyFindReplaceWindowHandle = 
				::RegisterWindowMessage("NotifyFindReplaceWindowHandle");
			const auto& findReplace = (LPFINDREPLACE)lParam;
			::SendMessage(
				findReplace->hwndOwner, iMsgNotifyFindReplaceWindowHandle, (WPARAM)hDlg, 0);

#ifdef JPNMSG
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 0, (LPARAM)"標準");
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 1, (LPARAM)"正規表現");
#else
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 0, (LPARAM)"Normal");
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 1, (LPARAM)"Regexp");
#endif
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_SETCURSEL, frcd.Mode, 0L);
			SendDlgItemMessage(hDlg, IDC_ESCSEQ, BM_SETCHECK, frcd.EscSeq, 0L);
			return TRUE;
		}

		case WM_COMMAND:
		{
			switch( GET_WM_COMMAND_ID(wParam, lParam) ){
				case IDC_ESCSEQ:
					frcd.EscSeq = (IsDlgButtonChecked(hDlg, IDC_ESCSEQ) == BST_CHECKED);
					break;
				case IDC_FINDMODE:
					if(GET_WM_COMMAND_CMD(wParam, lParam) == CBN_SELCHANGE){
						frcd.Mode = (int)SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_GETCURSEL, 0L, 0L);
						if(frcd.Mode == -1) frcd.Mode = 0;
						CheckDlgButton(hDlg, IDC_ESCSEQ, (frcd.Mode ? BST_CHECKED : frcd.EscSeq));
						EnableWindow(GetDlgItem(hDlg, IDC_ESCSEQ), !frcd.Mode);
					}
					break;
				case IDC_UP: case IDC_DOWN:
					if(IsDlgButtonChecked(hDlg, IDC_DOWN))
                        frcd.Flags |= FR_DOWN;
					else
						frcd.Flags &= ~FR_DOWN;
					break;
				case IDC_MATCHCASE:
					if(IsDlgButtonChecked(hDlg, IDC_MATCHCASE))
						frcd.Flags |= FR_MATCHCASE;
					else
						frcd.Flags &= ~FR_MATCHCASE;
					break;
			}
			return FALSE;
		}
	}
	return FALSE;
}

#define FR_MYDEFAULT (FR_HIDEWHOLEWORD | FR_ENABLETEMPLATE | FR_ENABLEHOOK | FR_SHOWHELP)
HWND PopFindFindDlg (HWND hwnd, int down)
	{
	static FINDREPLACE fr;       // must be static for modeless dialog!!!
	static bool init_flag = false;

	if(!init_flag){
		fr.lStructSize      = sizeof (FINDREPLACE) ;
		fr.hwndOwner        = hwnd ;
		fr.hInstance        = NULL ;
		fr.Flags            = FR_MYDEFAULT | (down == FALSE ? FR_UP : FR_DOWN);
		fr.lpstrFindWhat    = szFindText ;
		fr.lpstrReplaceWith = NULL ;
		fr.wFindWhatLen     = sizeof (szFindText) ;
		fr.wReplaceWithLen  = 0 ;
		fr.lCustData        = 0 ;
		fr.lpfnHook         = (LPFRHOOKPROC)FRHookProc ;
		fr.lpTemplateName   = "FINDDLG" ;

		init_flag = true;
	} else {
		fr.Flags = FR_MYDEFAULT | frcd.Flags;
		switch(down){
			case TRUE:
				fr.Flags |= FR_DOWN;
				break;

			case FALSE:
				fr.Flags &= ~FR_DOWN;
				break;
		}
	}

	int nSelLen;
	nSelLen = Footy2GetSelLength(activeFootyID, LM_CRLF);
	if( 0 < nSelLen ){
		int i, j = 0;
		char *szTempFindText = (char *)malloc(nSelLen + 1);
		if( szTempFindText ) {
			Footy2GetSelText(activeFootyID, szTempFindText, LM_CRLF, nSelLen + 1);
			if(frcd.EscSeq){
				for(i = 0; szTempFindText[i] != '\0' && j + 2 < sizeof(szFindText); i++, j++){
					switch(szTempFindText[i]){
						case 0x0D:
							if(Footy2GetLineCode(activeFootyID) == LM_CRLF)
								i++;
						case 0x0A:
							if(j + 3 < sizeof(szFindText)){
								szFindText[j++] = '\\';
								szFindText[j] = 'n';
							}
							break;
						case '\t':
							if(j + 3 < sizeof(szFindText)){
								szFindText[j++] = '\\';
								szFindText[j] = 't';
							}
							break;
						default:
							szFindText[j] = szTempFindText[i];
							break;
					}
				}
			} else {
				for(i = j = 0; szTempFindText[i] != '\0' && szTempFindText[i] != 0x0D &&
					szTempFindText[i] != 0x0A && j + 1 < sizeof(szFindText); i++, j++)
					szFindText[j] = szTempFindText[i];
			}
			free(szTempFindText);
		}
		szFindText[j] = '\0';
	}
	return FindText (&fr) ;
	}

HWND PopFindReplaceDlg (HWND hwnd)
	{
	static FINDREPLACE fr ;       // must be static for modeless dialog!!!
	static bool init_flag = false;

	if(!init_flag){
		fr.lStructSize      = sizeof (FINDREPLACE) ;
		fr.hwndOwner        = hwnd ;
		fr.hInstance        = NULL ;
		fr.Flags            = FR_MYDEFAULT;
		fr.lpstrFindWhat    = szFindText ;
		fr.lpstrReplaceWith = szReplText ;
		fr.wFindWhatLen     = sizeof (szFindText) ;
		fr.wReplaceWithLen  = sizeof (szReplText) ;
		fr.lCustData        = 0 ;
		fr.lpfnHook         = (LPFRHOOKPROC)FRHookProc ;
		fr.lpTemplateName   = "REPLACEDLG" ;

		init_flag = true;
	} else {
		fr.Flags = FR_MYDEFAULT | frcd.Flags;
	}


	repl_flag = 0;

	return ReplaceText (&fr) ;
	}

VOID PopFindDlgTerm(DWORD Flags){ frcd.Flags = Flags; }

//
// 大文字小文字の区別を無くす
void IgnoreCase(wchar_t *pwDest)
{
	do{
		if(iswupper(*pwDest)) *pwDest = towlower(*pwDest);
	} while(*++pwDest);
	return;
}

//
// Unicodeでのオフセットからマルチバイトでのオフセットに変換する
void ConvertOffset(char *pDest, int *pnOffset)
{
	for(int i = 0; i < *pnOffset; i++){
		if(_ismbblead(pDest[i])){
			(*pnOffset)++;
			i++;
		}
	}
	return;
}

// Footy2の先頭からの範囲取得
int Footy2GetSelB(int FootyID, size_t * StartOffset, size_t * EndOffset)
{
	const static size_t LineCodeLenTable[] = {
		0 /* LM_AUTOMATIC */, 2 /* LM_CRLF */, 1 /* LM_CR */, 1 /* LM_LF */, 0 /* LM_ERROR */, 
	};
	int LineCode = Footy2GetLineCode(FootyID);
	size_t LineCodeLen = LineCodeLenTable[LineCode % (sizeof(LineCodeLenTable) / sizeof(LineCodeLenTable[0]))];
	size_t CharCount, /*LineLength, */LineNum;
	size_t LineCount = Footy2GetLines(FootyID);
	size_t nsLine, nsPos, neLine, nePos;
	size_t nsOffset, neOffset;
	int ret;

	ret = Footy2GetSel(FootyID, &nsLine, &nsPos, &neLine, &nePos);
	if(FOOTY2ERR_NONE != ret && FOOTY2ERR_NOTSELECTED != ret){
		return ret;
	}
	if( FOOTY2ERR_NOTSELECTED == ret ) {
		Footy2GetCaretPosition(FootyID, &nsLine, &nsPos);
		neLine = nsLine;
		nePos  = nsPos;
	}

	nsOffset = neOffset = (size_t)-1;
	
	for(CharCount = 0, LineNum = 0;
		LineNum < LineCount &&
			( (size_t)-1 == nsOffset || (size_t)-1 == neOffset );
		LineNum++)
	{
		if((size_t)-1 == nsOffset &&
			nsLine <= LineNum)
		{
			nsOffset = CharCount + nsPos;
		}
		if((size_t)-1 == neOffset &&
			neLine <= LineNum)
		{
			neOffset = CharCount + nePos;
		}
		CharCount += Footy2GetLineLengthW(FootyID, LineNum) + LineCodeLen;
	}

	if(StartOffset) {
		*StartOffset = nsOffset;
	}
	if(EndOffset) {
		*EndOffset = neOffset;
	}

	return ret;
}

// Footy2の先頭からの範囲選択
int Footy2SetSelB(int FootyID, size_t StartOffset, size_t EndOffset, bool bRefresh = true)
{
	const static size_t LineCodeLenTable[] = {
		0 /* LM_AUTOMATIC */, 2 /* LM_CRLF */, 1 /* LM_CR */, 1 /* LM_LF */, 0 /* LM_ERROR */, 
	};
	int LineCode = Footy2GetLineCode(FootyID);
	size_t LineCodeLen = LineCodeLenTable[LineCode % (sizeof(LineCodeLenTable) / sizeof(LineCodeLenTable[0]))];
	size_t CharCount, LineLength, LineNum;
	size_t LineCount = Footy2GetLines(FootyID);
	size_t nsLine, nsPos, neLine, nePos;

	nsLine = neLine = nsPos = nePos = (size_t)-1;
	
	for(CharCount = 0, LineNum = 0;
		LineNum < LineCount &&
		(	(size_t)-1 == nsLine || (size_t)-1 == nsPos ||
			(size_t)-1 == neLine || (size_t)-1 == nePos )
			;
		LineNum++)
	{
		LineLength = Footy2GetLineLengthW(FootyID, LineNum) + LineCodeLen;
		if((size_t)-1 == nsLine &&
			StartOffset <= CharCount + LineLength)
		{
			nsLine = LineNum;
			nsPos  = StartOffset - CharCount;
		}
		if((size_t)-1 == neLine &&
			EndOffset <= CharCount + LineLength)
		{
			neLine = LineNum;
			nePos  = EndOffset - CharCount;
		}
		CharCount += LineLength;
	}

	return Footy2SetSel(FootyID, nsLine, nsPos, neLine, nePos, bRefresh);
}

//
// 標準の検索を行う
static void FindTextAsStandard(FRSTRING * dest, FRSTRING *pattern, bool down, bool matchcase, bool select/*by Tetr@pod*/, FINDRET *frReturn)
{
// #if 0
	if ( select ) {
		int nRet;
		nRet = Footy2Search(
					activeFootyID,
					pattern->ptr,
					SEARCH_FROMCURSOR
						| (matchcase ? 0 : SEARCH_IGNORECASE)
						| (down ? 0 : SEARCH_BACK)
					);
		frReturn->success = 0 <= nRet;
		// return;
// #else
	} else {
		int nDestSize, nPatternSize, nLength, nOffset;
		char *pcDest;
		wchar_t *pwDest, *pwPattern;
		std::wstring wsDest;

		//
		// Unicodeへの変換を行う

		pcDest = dest->ptr + (down ? dest->offset : 0);
		nLength = (down ? dest->length - dest->offset : dest->offset);

		nDestSize = MultiByteToWideChar(CP_ACP, 0, pcDest, nLength, NULL, 0);
		nPatternSize = MultiByteToWideChar(CP_ACP, 0, pattern->ptr, pattern->length, NULL, 0);

		pwDest = (wchar_t *)malloc((nDestSize + 1) * sizeof(wchar_t));
		pwPattern = (wchar_t *)malloc((nPatternSize + 1) * sizeof(wchar_t));

		MultiByteToWideChar(CP_ACP, 0, pcDest, nLength, pwDest, nDestSize + 1);
		MultiByteToWideChar(CP_ACP, 0, pattern->ptr, pattern->length, pwPattern, nPatternSize + 1);

		pwDest[nDestSize] = pwPattern[nPatternSize] = 0;

		//
		// 大文字、小文字の区別を無くす

		if(!matchcase) {
			IgnoreCase(pwDest);
			IgnoreCase(pwPattern);
		}

		wsDest = pwDest;

		//
		// 検索

		nOffset = (down ? wsDest.find(pwPattern) : wsDest.rfind(pwPattern));
		if(nOffset == wsDest.npos){
			frReturn->success = false;
			frReturn->offset = frReturn->length = 0;
		}
		else{
			ConvertOffset(pcDest, &nOffset);

			frReturn->success = true;
			frReturn->offset = nOffset + (down ? dest->offset : 0);
			frReturn->length = pattern->length;

			/*// ありえないので
			if (select == true) {
				Footy2SetSelB(activeFootyID, frReturn->offset, frReturn->offset + frReturn->length);
			}
			*/
		}

		//
		// 後処理
		free(pwDest);
		free(pwPattern);
	}
	return;
// #endif
}

// 正規表現を用いて検索を行う
// Find text with regular expression.
static void FindTextAsRegExp(FRSTRING *dest, FRSTRING *pattern, bool down, bool matchcase, bool select/*by Tetr@pod*/, FINDRET *frReturn)
{
	IRegExpPtr pRegExp;
	IDispatch *pDispatch = NULL, *pDispatch2 = NULL;
	IMatchCollection *pCollection = NULL;
	IMatch* pMatch = NULL;
	int nCount = 0, nIndex = 0, offset;
	_bstr_t bsDest, bsPattern;
	char *pDest;

	frReturn->success = false;
	frReturn->offset = frReturn->length = 0;

	if(FAILED(pRegExp.CreateInstance(CLSID_RegExp))) return;
	
	offset = dest->offset;
	ConvertOffset(dest->ptr, &offset);

	pDest = dest->ptr + (down ? offset/*dest->offset*/ : 0);
	if(!down) dest->ptr[offset/*dest->offset*/] = '\0';
	bsDest = pDest;
	
	bsPattern = pattern->ptr;
	
	pRegExp->put_Pattern(bsPattern);
	pRegExp->put_IgnoreCase(-!matchcase);
	pRegExp->put_Global(-!down);
//2008-03-17 Shark++ 置換時の検索の選択がおかしい
	if(SUCCEEDED(pRegExp->Execute(bsDest, &pDispatch))){
		if(SUCCEEDED(pDispatch->QueryInterface(IID_IMatchCollection, (void **)&pCollection))){
			pDispatch2 = pMatch = NULL;
			pCollection->get_Count((long *)&nCount);
			if(nCount > 0){			
				pCollection->get_Item((down ? 0 : nCount - 1), &pDispatch2);
				if(SUCCEEDED(pDispatch2->QueryInterface(IID_IMatch, (void **)&pMatch))){
					pMatch->get_FirstIndex((long *)&nIndex);
					pMatch->get_Length(&frReturn->length);
					pMatch->Release();

				//	ConvertOffset(pDest, &nIndex);
				//	ConvertOffset(pDest + nIndex, (int *)&(frReturn->length));

					frReturn->success = true;
					frReturn->offset = nIndex + (down ? dest->offset : 0);

					if (select == true) {
						Footy2SetSelB(activeFootyID, frReturn->offset, frReturn->offset + frReturn->length);
					}
				}
				pDispatch2->Release();

			}
			pCollection->Release();
		}
		pDispatch->Release();
	}
}
// エスケープシーケンスをバイナリに置き換える
// Replace escape-sequence with binary.
static void ReplaceEscSeq(char *nstr)
{
	char *sstr = nstr, *ostr = nstr;
	bool esqsw = false;
	int linecode = Footy2GetLineCode(activeFootyID);

	do{
		if(esqsw){
			switch(*sstr){
				case '\\':
					*nstr++ = '\\';
					break;
				case 'n': case 'N':
					if(linecode == LM_CRLF || linecode == LM_CR)
						*nstr++ = '\r';
					if(linecode == LM_CRLF || linecode == LM_LF)
						*nstr++ = '\n';
					break;
				case 't': case 'T':
					*nstr++ = '\t';
					break;
				default:
					*nstr++ = '\\';
					*nstr++ = *sstr;
					break;
			}
			esqsw = false;
		} else {
			if(*sstr == '\\' && (sstr == ostr || !_ismbblead(sstr[-1])))
                esqsw = true;
			else if(nstr != sstr) *nstr++ = *sstr;
			else nstr++;
		}
	} while(*sstr++ != '\0');
	return;
}

BOOL PopFindFindText (HWND /*hwndEdit*/, int iSearchOffset, LPFINDREPLACE pfr, bool select/*by Tetr@pod*/)
	{
	FRSTRING dest, pattern;
	FINDRET frReturn = { 0,0,0 };
    
		// Read in the edit document

	dest.length = Footy2GetTextLength(activeFootyID, LM_CRLF);
	dest.ptr = (char *)malloc(dest.length + 1);
	if(dest.ptr == NULL) return FALSE;
	dest.offset = iSearchOffset;
	Footy2GetText(activeFootyID, dest.ptr, LM_CRLF, dest.length);

		// Search the document for the find string

	pattern.ptr = (char *)malloc(lstrlen(pfr->lpstrFindWhat) + 1);
	if(pattern.ptr == NULL){
		free(dest.ptr);
		return FALSE;
	}
	lstrcpy(pattern.ptr, pfr->lpstrFindWhat);

	if(frcd.EscSeq && !frcd.Mode) ReplaceEscSeq(pattern.ptr);
	pattern.length = lstrlen(pattern.ptr);
	if(dest.length > 0 && pattern.length > 0){
		if(frcd.Mode)
			FindTextAsRegExp(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, select, &frReturn);
		else
			FindTextAsStandard(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, select, &frReturn);
	}
	else
		frReturn.success = false;
	free(dest.ptr);
	free(pattern.ptr);

	if (!frReturn.success) return FALSE;

		// Find the position in the document and the new start offset
//	FootySetSelB(activeFootyID, frReturn.offset + 1, frReturn.offset + frReturn.length + 1);	// 2008-02-17 Shark++ 代替機能未実装
	frcd.plength = frReturn.length;
	repl_flag = 1;
	return TRUE ;
	}

BOOL PopFindNextText (HWND hwndEdit, int iSearchOffset, bool down)
	{
	FINDREPLACE fr ;

	fr.lpstrFindWhat = szFindText ;
	fr.Flags = (down ? FR_DOWN : FR_UP) ;

	return PopFindFindText(hwndEdit, iSearchOffset, &fr, true) ;
	}

BOOL PopFindReplaceText (HWND hwndEdit, int iSearchOffset, LPFINDREPLACE pfr)
	{
	char *ReplaceWith;


	ReplaceWith = (char *)malloc(lstrlen(pfr->lpstrReplaceWith) + 1);
	if(ReplaceWith == NULL){
		return FALSE;
	}
    lstrcpy(ReplaceWith, pfr->lpstrReplaceWith);
	if(frcd.EscSeq) ReplaceEscSeq(ReplaceWith);

		// Find the text

	if ( repl_flag == 0 ) {
		if (!PopFindFindText(hwndEdit, iSearchOffset, pfr, true)) return FALSE ;
	}

		// Replace it

	Footy2SetSelText(activeFootyID, ReplaceWith);
	free(ReplaceWith);

//	iSearchOffset = Footy2GetCaretThrough(activeFootyID) - 1;
	iSearchOffset = 0;	// 2008-02-17 Shark++ 代替機能不明
	
	if (!PopFindFindText(hwndEdit, iSearchOffset, pfr, true)){// 次が見つからなかったら
		repl_flag = 0;
        return FALSE ;// FALSEを返す
	}
	//PopFindFindText (hwndEdit, piSearchOffset, pfr, true);
	return TRUE ;
	}

BOOL PopFindReplaceAllText(HWND hwndEdit, int iSearchOffset, LPFINDREPLACE pfr)// ReplaceAll by Tetr@pod すべて置換が遅すぎるため
	{
	char *ReplaceWith;

	ReplaceWith = (char *)malloc(lstrlen(pfr->lpstrReplaceWith) + 1);
	if (ReplaceWith == NULL) {
		return FALSE;
	}
    lstrcpy(ReplaceWith, pfr->lpstrReplaceWith);
	if (frcd.EscSeq) {
		ReplaceEscSeq(ReplaceWith);
	}

		// Find the text
	if ( repl_flag == 0 ) {
		if (!PopFindFindText(hwndEdit, iSearchOffset, pfr, false)) {
			return FALSE ;
		}
	}



	FRSTRING dest, pattern;
	FINDRET frReturn = { 0,0,0 };
    
		// Read in the edit document
	dest.length = Footy2GetTextLength(activeFootyID, LM_CRLF) + 1;// 【重要】なぜか+1が必要
	dest.ptr = (char *)malloc(dest.length + 1);
	if(dest.ptr == NULL) return FALSE;
	//memset(dest.ptr, 0, dest.length + 2);// 一応dest.ptrを0フィル
	dest.offset = iSearchOffset;
	Footy2GetText(activeFootyID, dest.ptr, LM_CRLF, dest.length + 1);

		// Search the document for the find string
	pattern.ptr = (char *)malloc(lstrlen(pfr->lpstrFindWhat) + 1);
	if(pattern.ptr == NULL){
		free(dest.ptr);
		return FALSE;
	}
	lstrcpy(pattern.ptr, pfr->lpstrFindWhat);

	if(frcd.EscSeq && !frcd.Mode) ReplaceEscSeq(pattern.ptr);
	pattern.length = lstrlen(pattern.ptr);

	/*// 何故か最後の文字が消えるので、追加する
	dest.ptr[dest.length] = 1;// 検索に引っかからない、かつ\0でない文字付加
	dest.ptr[dest.length + 1] = 0;// \0付加
	dest.length++;*/

	while (true) {// 無限ループで置換しつづける(両方のFindTextAs...に問題ないなら、おそらく大丈夫)
	//	iSearchOffset = Footy2GetCaretThrough(activeFootyID) - 1;
		iSearchOffset = 0;	// 2008-02-17 Shark++ 代替機能不明
		
		if(dest.length > 0 && pattern.length > 0){
			if(frcd.Mode) {
				FindTextAsRegExp(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, false, &frReturn);
			} else {
				FindTextAsStandard(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, false, &frReturn);
			}
		} else {
			frReturn.success = false;
		}

		if (frReturn.success == false) {// 見つからなかった
			break;
		}


		// 次に備える
		dest.length = dest.length + lstrlen(ReplaceWith) - pattern.length;// dest.length更新

		char *ratemp;// 置換後の文字を保持する一時的な変数
		ratemp = (char *)malloc(dest.length + 1);// ratemp作成
		if (ratemp == NULL) {// ratemp確保失敗
			free(dest.ptr);// dest.ptr解放
			dest.ptr = NULL;// dest.ptrでエラーか判断する(あまり良くないかも)
			break;
		}
		memset(ratemp, 0, dest.length + 1);// 【重要】ratempを0フィル

		// ratempに置換後の文字を作成
		strncpy(ratemp, dest.ptr, frReturn.offset);// 前の部分
		strcat(ratemp, ReplaceWith);// 置換部分
		strcat(ratemp, dest.ptr + frReturn.offset + pattern.length);// 後ろの部分
		ratemp[dest.length - 1] = dest.ptr[dest.length - 1];

		free(dest.ptr);// dest.ptr解放
		dest.ptr = (char *)malloc(dest.length + 1);// dest.ptr作成
		if (dest.ptr == NULL) {// dest.ptr確保失敗
			free(ratemp);// ratemp解放
			break;
		}
		// memset(dest.ptr, 0, dest.length + 1);// 一応dest.ptrを0フィル
		strcpy(dest.ptr, ratemp);// ratempをdest.ptrにコピー

		free(ratemp);// ratemp解放
	}// while終了

	if ( dest.ptr != NULL ) {// エラーはなかった(dest.ptrでエラーか判断するのはあまり良くないかも)
		// Footy2更新、Footy2SetTextはアンドゥバッファが消えるため使用しない
		Footy2SelectAll(activeFootyID, false);// 全て選択(再描画なし)
		Footy2SetSelText(activeFootyID, dest.ptr);// 上で選択した部分(つまり全て)を書き換え

		free(dest.ptr);// dest.ptr解放
	}
	// 解放処理
	free(pattern.ptr);
	free(ReplaceWith);
	return FALSE;
	}

BOOL PopFindValidFind (void)
	{
	return *szFindText != '\0' ;
	}



BOOL PopFindReplaceAll (HWND hwndEdit, int iSearchOffset, LPFINDREPLACE pfr)
{
	BOOL rval;
	int ofs;
	ofs=iSearchOffset;
	//if (!PopFindFindText (hwndEdit, &ofs, pfr, true)) return FALSE ;
	//SendMessage (hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs ) ;
	rval=PopFindReplaceText ( hwndEdit, ofs, pfr );
	return rval;
}