/**
 * @file DllMain.cpp
 * @brief DLL関数の実装
 * @author Shinji Watanabe
 * @date 2009/01/10
 */

#include "Cursor.h"
#include "CharsetDetector.h"
#include "ConvFactory.h"

#ifndef UNDER_CE
#	pragma comment (lib,"imm32.lib")		//!< IME関連のライブラリ
#endif

#define FOOTY_VER			2017			//!< Footyのバージョン×1000
#define FOOTY_BETA			0				//!< Footyのβバージョン番号

/*グローバル変数の定義*/
static CRITICAL_SECTION g_CriticalSection;	//!< クリティカルセクション
static std::list<CFooty*> *g_plsFooty = NULL;
static HINSTANCE g_hInstance = NULL;

//-----------------------------------------------------------------------------
/**
 * @brief 指定されたIDのFootyコントロールポインタを取得します。
 */
CFooty *GetFooty(int nID)
{
	EnterCriticalSection(&g_CriticalSection);
	{
		if (!g_plsFooty)
		{
			LeaveCriticalSection(&g_CriticalSection);
			return NULL;
		}
		// 回していく
		for (FootyPt pRetFooty = g_plsFooty->begin();
			pRetFooty != g_plsFooty->end();
			pRetFooty++)
		{
			if ((*pRetFooty)->GetID() == nID)
			{
				LeaveCriticalSection(&g_CriticalSection);
				return *pRetFooty;
			}
		}
	}
	LeaveCriticalSection(&g_CriticalSection);
	return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief 情報を出力します
 */
#ifdef PRINT_ENABLED
void FormatPrint( const wchar_t* filename, int nLine, const wchar_t* fmt, ... )
{
	const int MaxLogBuffer = 1024;
	wchar_t szText[ MaxLogBuffer + 1 ];
	
	swprintf( szText, MaxLogBuffer, L"%s(%d):", filename, nLine );
	OutputDebugStringW( szText );

	va_list vaList;
	va_start( vaList, fmt );
	vswprintf_s( szText, MaxLogBuffer, fmt, vaList );
	va_end( vaList );

	OutputDebugStringW( szText );
}
#endif


//-----------------------------------------------------------------------------
/**
 * @brief 開始時に呼び出します。Footy2ライブラリを初期化します
 */
FOOTYEXPORT(void) Footy2Start(HINSTANCE hInstance)
{
	InitializeCriticalSection(&g_CriticalSection);
	EnterCriticalSection(&g_CriticalSection);
	{
		g_hInstance = hInstance;
		g_plsFooty = new std::list<CFooty*>;
	}
	LeaveCriticalSection(&g_CriticalSection);
	CCursor::LoadCursors(hInstance);
	CFootyView::LoadLineIcons(hInstance);
}

//-----------------------------------------------------------------------------
/**
 * @brief 終了時に呼び出します
 */
FOOTYEXPORT(void) Footy2End()
{
	FootyPt iterFooty;
	// Footyのポインタを削除していく
	EnterCriticalSection(&g_CriticalSection);
	if (g_plsFooty)
	{
		for (iterFooty = g_plsFooty->begin();iterFooty != g_plsFooty->end();iterFooty++)
		{
			if (*iterFooty)
			{
				delete (*iterFooty);
				*iterFooty = NULL;
			}
		}
		delete g_plsFooty;
		g_plsFooty = NULL;
	}
	LeaveCriticalSection(&g_CriticalSection);
	// カーソルを破棄
	CCursor::DestroyCursors();
	CFootyView::ReleaseLineIcons();
}

#if !defined FOOTYSTATIC
//-----------------------------------------------------------------------------
/**
 * @brief DLLのメイン関数です。
 */
BOOL APIENTRY DllMain(HANDLE hInst, DWORD  fdwReason, LPVOID lpReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:		// プロセスにアタッチされた。
		Footy2Start((HINSTANCE)hInst);
		break;
	case DLL_PROCESS_DETACH:		// プロセスにデタッチされた。
		Footy2End();
#if defined _DEBUG && !defined UNDER_CE
	_CrtDumpMemoryLeaks();			// メモリリーク検出
#endif	/*_DEBUG*/
		break;
	}
	return true;
}
#endif	/*!defined FOOTYSTATIC*/

//-----------------------------------------------------------------------------
/**
 * @brief バージョン情報を取得します
 */
FOOTYEXPORT(int) GetFooty2Ver()
{
	return FOOTY_VER;
}

//-----------------------------------------------------------------------------
/**
 * @brief βバージョンを取得します
 */
FOOTYEXPORT(int) GetFooty2BetaVer()
{
	return FOOTY_BETA;
}

//-----------------------------------------------------------------------------
/**
 * @brief Footyを構築します
 */
FOOTYEXPORT(int) Footy2Create(HWND hWnd,int x,int y,int dx,int dy,int nMode)
{
	// エラーチェック
	if (!hWnd || dx < 0 || dy < 0)
	{
		return FOOTY2ERR_ARGUMENT;
	}

	// Footyを確保する
	int nID;
	// リストへ代入
	EnterCriticalSection(&g_CriticalSection);
		FootyPt iterFooty = g_plsFooty->begin();
		for (nID = 0;iterFooty != g_plsFooty->end();iterFooty++,nID++)
		{
			if ((*iterFooty)->GetID() != nID)		//途中へ
			{
				break;
			}
		}
		// Footyオブジェクトを構築します
		CFooty *pFooty = new CFooty( nID, g_hInstance, nMode );
		if (!pFooty)
		{
			LeaveCriticalSection(&g_CriticalSection);
			return FOOTY2ERR_MEMORY;
		}
		g_plsFooty->insert(iterFooty,pFooty);
	LeaveCriticalSection(&g_CriticalSection);
	
	// 構築する
	pFooty->Create( hWnd, x, y, dx, dy );
	
	// IDを返す
	return nID;
}

//-----------------------------------------------------------------------------
/**
 * @brief 親ウィンドウを変更します
 */
FOOTYEXPORT(int) Footy2ChangeParent(int nID, HWND hWnd, int x,int y, int dx,int dy)
{
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	pFooty->Create( hWnd, x, y, dx, dy );
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2ChangeView
Footyのビューモードの変更です。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2ChangeView(int nID,int nView){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	pFooty->ChangeView(nView);
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2SetFocus
Footyのビューモードの変更です。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetFocus(int nID,int nViewID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (nViewID < 0 || 4 < nViewID)return FOOTY2ERR_ARGUMENT;
	pFooty->m_cView[nViewID].SetFocus();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2GetWnd
Footyのウィンドウハンドルを取得します
----------------------------------------------------------------*/
FOOTYEXPORT(HWND) Footy2GetWnd(int nID,int nViewID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return NULL;
	if (nViewID < 0 || 4 < nViewID)return NULL;
	return pFooty->m_cView[nViewID].GetWnd();
}

/*----------------------------------------------------------------
Footy2GetWndVSplit
FootyのVSplitのウィンドウハンドルを取得します
----------------------------------------------------------------*/
FOOTYEXPORT(HWND) Footy2GetWndVSplit(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return NULL;
	return pFooty->m_cVSplitter.GetWnd();
}

/*----------------------------------------------------------------
Footy2GetWndHSplit
FootyのHSplitのウィンドウハンドルを取得します
----------------------------------------------------------------*/
FOOTYEXPORT(HWND) Footy2GetWndHSplit(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return NULL;
	return pFooty->m_cHSplitter.GetWnd();
}

/*----------------------------------------------------------------
Footy2GetWndHSplit
FootyのHSplitのウィンドウハンドルを取得します
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetActiveView(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*取得する*/
	int nRet = FOOTY2ERR_NOACTIVE;
	for (int i=0;i<4;i++){
		if (pFooty->m_cView[i].IsFocused())
			nRet = i;
	}
	return nRet;
}

/*----------------------------------------------------------------
Footy2Delete
Footyを削除します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2Delete(int id){
	CFooty *pFooty = GetFooty(id);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*リストから削除する*/
	g_plsFooty->remove(pFooty);
	/*ポインタを削除する*/
	if (pFooty){
		delete pFooty;
		pFooty = NULL;
	}
	return FOOTY2ERR_NONE;
}


/*----------------------------------------------------------------
Footy2Move
Footy2ウィンドウを移動するための関数です。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2Move(int nID,int x,int y,int dx,int dy){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	return pFooty->Move(x,y,dx,dy) ? FOOTY2ERR_NONE : FOOTY2ERR_ARGUMENT;
}

/*----------------------------------------------------------------
Footy2Refresh
Footy2ウィンドウを再描画する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2Refresh(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2CreateNew
新規作成
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2CreateNew(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*新規作成*/
	pFooty->m_cDoc.CreateNew(nID);
	pFooty->SetCharSet(CSM_DEFAULT);
	return FOOTY2ERR_NONE;
}

/*----------------------------------------------------------------
Footy2GetCharSet
文字コードセットを取得する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetCharSet(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return CSM_ERROR;
	return pFooty->GetCharSet();
}

/*----------------------------------------------------------------
Footy2GetLineCode
新規作成
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetLineCode(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return LM_ERROR;
	return pFooty->m_cDoc.GetLineMode();
}

/*----------------------------------------------------------------
Footy2GetLines
全行数を取得します。
----------------------------------------------------------------*/
FOOTYEXPORT(int)Footy2GetLines(int nID){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*行数を取得する*/
	return (int)pFooty->m_cDoc.GetLineNum();
}


#ifndef UNDER_CE
/**
 * Footy2GetLineA
 * @brief 指定行のテキストを取得します。
 * @param nID FootyのID番号
 * @param pString 出力先
 * @param nLine 出力する行
 * @param nSize バッファサイズ
 */
FOOTYEXPORT(int) Footy2GetLineA(int nID,char *pString,size_t nLine,int nSize){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*該当行を取得する*/
	if (nLine < 0 || pFooty->m_cDoc.GetLineNum() <= nLine)return FOOTY2ERR_ARGUMENT;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*データを返す*/
	CConvFactory cConv;
	cConv.GetConv()->ToMulti(pLine->GetLineData(), (UINT)pLine->GetLineLength());
	FOOTY2STRCPY(pString,cConv.GetConv()->GetConvData(),nSize);
	return FOOTY2ERR_NONE;
}
#endif	/*UNDER_CE*/

/*----------------------------------------------------------------
Footy2GetLineW
指定行のテキストを取得します。
----------------------------------------------------------------*/
FOOTYEXPORT(const wchar_t*) Footy2GetLineW(int nID,size_t nLine){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return NULL;
	/*該当行を取得する*/
	if (nLine < 0 || pFooty->m_cDoc.GetLineNum() <= nLine)return NULL;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*データを返す*/
	return pLine->GetLineData();
}


#ifndef UNDER_CE
/*----------------------------------------------------------------
Footy2SetLineA
指定行のテキストを設定します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetLineA(int nID,size_t nLine,const char *pString,bool bRedraw){
	CConvFactory cConv;
	cConv.GetConv()->ToUnicode(pString,(UINT)strlen(pString));
	return Footy2SetLineW(nID,nLine,(wchar_t*)cConv.GetConv()->GetConvData(),bRedraw);
}
#endif

/*----------------------------------------------------------------
Footy2SetLineW
指定行のテキストを設定します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetLineW(int nID,size_t nLine,const wchar_t *pString,bool bRedraw){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return NULL;
	if (!pString)return FOOTY2ERR_ARGUMENT;
	/*選択する*/
	CFootyDoc *pDoc = &pFooty->m_cDoc;
	if (!pDoc->GetCaretPosition()->SetPosition(pDoc->GetLineList(),nLine,0))
		return FOOTY2ERR_ARGUMENT;
	LinePt pLine = pDoc->GetCaretPosition()->GetLinePointer();
	pDoc->SetSelectStart();
	pDoc->GetCaretPosition()->SetPosition(pLine,pLine->GetLineLength());
	pDoc->SetSelectEndNormal();
	/*設定する*/
	pDoc->InsertString(pString);
	/*キャレットから位置を再設定*/
	for (int i=0;i<4;i++){
		pFooty->m_cView[i].AdjustVisibleLine();
		pFooty->m_cView[i].AdjustVisiblePos();
	}
	/*再描画処理*/
	if (bRedraw)pFooty->m_cView[0].Refresh();
	return FOOTY2ERR_NONE;
}

#ifndef UNDER_CE
/*----------------------------------------------------------------
Footy2GetLineSizeA
指定行の長さを取得します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetLineLengthA(int nID,size_t nLine){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return -1;
	/*該当行を取得する*/
	if (pFooty->m_cDoc.GetLineNum() <= nLine)return -1;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*データを返す*/
	CConvFactory cConv;
	cConv.GetConv()->ToMulti(pLine->GetLineData(), (UINT)pLine->GetLineLength());
	return (int)cConv.GetConv()->GetConvTextSize() - sizeof(char);
}
#endif

/*----------------------------------------------------------------
Footy2GetLineSizeW
指定行の長さを取得します。
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetLineLengthW(int nID,size_t nLine){
	/*Footyを取得する*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	/*該当行を取得する*/
	if (pFooty->m_cDoc.GetLineNum() <= nLine)return FOOTY2ERR_ARGUMENT;
	LinePt pLine = pFooty->m_cDoc.GetLine(nLine);
	/*データを返す*/
	return (int)pLine->GetLineLength();
}

/*----------------------------------------------------------------
Footy2SetLapel
折り返し設定
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2SetLapel(int nID,int nColumns,int nMode,bool bRedraw){
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	return pFooty->SetLapel(nColumns,nMode,bRedraw) ? FOOTY2ERR_NONE : FOOTY2ERR_ARGUMENT;
}

/*----------------------------------------------------------------
Footy2GetVisibleColumns
表示可能な桁数を取得する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetVisibleColumns(int nID,int nViewID){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (nViewID < 0 || 4 <= nViewID)return FOOTY2ERR_ARGUMENT;
	return (int)pFooty->m_cView[nViewID].GetVisibleColumns();
}

/*----------------------------------------------------------------
Footy2GetVisibleLines
表示可能な行数を取得する処理
----------------------------------------------------------------*/
FOOTYEXPORT(int) Footy2GetVisibleLines(int nID,int nViewID){
	/*宣言*/
	CFooty *pFooty = GetFooty(nID);
	if (!pFooty)return FOOTY2ERR_NOID;
	if (nViewID < 0 || 4 <= nViewID)return FOOTY2ERR_ARGUMENT;
	return (int)pFooty->m_cView[nViewID].GetVisibleLines();
}

/*[EOF]*/
