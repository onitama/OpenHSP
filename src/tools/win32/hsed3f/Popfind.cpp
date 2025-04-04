/*--------------------------------------------------------
   POPFIND.C -- Popup Editor Search and Replace Functions
  --------------------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <comdef.h>
#include <string.h>
#include <mbstring.h>
#include <ctype.h>
#include <wctype.h>
#include <string>
#include "vbsregexp.h"
#include "FootyDLL.h"
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
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 0, (LPARAM)"標準");
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_INSERTSTRING, 1, (LPARAM)"正規表現");
			SendDlgItemMessage(hDlg, IDC_FINDMODE, CB_SETCURSEL, frcd.Mode, 0L);
			SendDlgItemMessage(hDlg, IDC_ESCSEQ, BM_SETCHECK, frcd.EscSeq, 0L);
			return TRUE;
		}

		case WM_COMMAND:
		{
			switch(LOWORD(wParam)){
				case IDC_ESCSEQ:
					frcd.EscSeq = (IsDlgButtonChecked(hDlg, IDC_ESCSEQ) == BST_CHECKED);
					break;
				case IDC_FINDMODE:
					if(HIWORD(wParam) == CBN_SELCHANGE){
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

	int nStart, nEnd;
	FootyGetSelB(activeFootyID, &nStart, &nEnd);
	if(nEnd - nStart > 0){
		int i, j;
		char *szTempFindText = (char *)malloc(nEnd - nStart + 1);
		FootyGetSelText(activeFootyID, szTempFindText);
		if(frcd.EscSeq){
			for(i = j = 0; szTempFindText[i] != '\0' && j + 2 < sizeof(szFindText); i++, j++){
				switch(szTempFindText[i]){
					case 0x0D:
						if(FootyGetLineCode(activeFootyID) == RETLINE_CRLF)
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
		szFindText[j] = '\0';
		free(szTempFindText);
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


//
// 標準の検索を行う
static void FindTextAsStandard(FRSTRING *dest, FRSTRING *pattern, bool down, bool matchcase, FINDRET *frReturn)
{
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
	}

	//
	// 後処理
	free(pwDest);
	free(pwPattern);

	return;
	
}

// 正規表現を用いて検索を行う
// Find text with regular expression.
static void FindTextAsRegExp(FRSTRING *dest, FRSTRING *pattern, bool down, bool matchcase, FINDRET *frReturn)
{
	IRegExpPtr pRegExp;
	IDispatch *pDispatch = NULL, *pDispatch2 = NULL;
	IMatchCollection *pCollection = NULL;
	IMatch* pMatch = NULL;
	int nCount = 0, nIndex = 0;
	_bstr_t bsDest, bsPattern;
	char *pDest;

	frReturn->success = false;
	frReturn->offset = frReturn->length = 0;

	if(FAILED(pRegExp.CreateInstance(CLSID_RegExp))) return;

	pDest = dest->ptr + (down ? dest->offset : 0);
	if(!down) dest->ptr[dest->offset] = '\0';
	bsDest = pDest;
	
	bsPattern = pattern->ptr;
	
	pRegExp->put_Pattern(bsPattern);
	pRegExp->put_IgnoreCase(-!matchcase);
	pRegExp->put_Global(-!down);

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

					ConvertOffset(pDest, &nIndex);
					ConvertOffset(pDest + nIndex, (int *)&(frReturn->length));

					frReturn->success = true;
					frReturn->offset = nIndex + (down ? dest->offset : 0);
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
	int linecode = FootyGetLineCode(activeFootyID);

	do{
		if(esqsw){
			switch(*sstr){
				case '\\':
					*nstr++ = '\\';
					break;
				case 'n': case 'N':
					if(linecode == RETLINE_CRLF || linecode == RETLINE_CR)
						*nstr++ = '\r';
					if(linecode == RETLINE_CRLF || linecode == RETLINE_LF)
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

BOOL PopFindFindText (HWND hwndEdit, int iSearchOffset, LPFINDREPLACE pfr)
	{
	FRSTRING dest, pattern;
	FINDRET frReturn;
    
		// Read in the edit document

	dest.length = FootyGetTextLength(activeFootyID);
	dest.ptr = (char *)malloc(dest.length + 1);
	if(dest.ptr == NULL) return FALSE;
	dest.offset = iSearchOffset;
	FootyGetText(activeFootyID, dest.ptr, RETLINE_AUTO);

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
			FindTextAsRegExp(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, &frReturn);
		else
			FindTextAsStandard(&dest, &pattern, (pfr->Flags & FR_DOWN) != FALSE, (pfr->Flags & FR_MATCHCASE) != FALSE, &frReturn);
	}
	else
		frReturn.success = false;
	free(dest.ptr);
	free(pattern.ptr);

	if (!frReturn.success) return FALSE;

		// Find the position in the document and the new start offset
	FootySetSelB(activeFootyID, frReturn.offset + 1, frReturn.offset + frReturn.length + 1);
	frcd.plength = frReturn.length;
	repl_flag = 1;
	return TRUE ;
	}

BOOL PopFindNextText (HWND hwndEdit, int iSearchOffset, bool down)
	{
	FINDREPLACE fr ;

	fr.lpstrFindWhat = szFindText ;
	fr.Flags = (down ? FR_DOWN : FR_UP) ;

	return PopFindFindText (hwndEdit, iSearchOffset, &fr) ;
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
		if (!PopFindFindText (hwndEdit, iSearchOffset, pfr)) return FALSE ;
	}

		// Replace it

	FootySetSelText(activeFootyID, ReplaceWith);
	free(ReplaceWith);

	iSearchOffset = FootyGetCaretThrough(activeFootyID) - 1;
	
	if (!PopFindFindText (hwndEdit, iSearchOffset, pfr)){
		repl_flag = 0;
        return FALSE ;
	}
	//PopFindFindText (hwndEdit, piSearchOffset, pfr);
	return TRUE ;
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
	//if (!PopFindFindText (hwndEdit, &ofs, pfr)) return FALSE ;
	//SendMessage (hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs ) ;
	rval=PopFindReplaceText ( hwndEdit, ofs, pfr );
	return rval;
}