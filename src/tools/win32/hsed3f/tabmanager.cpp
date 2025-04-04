/*------------------------------------------------------------------------------
	Tab management routines for HSP Script Editor
------------------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <string.h>
#include "tabmanager.h"
#include "footyDll.h"
#include "classify.h"
#include "hsed_config.h"
#include "support.h"
#include "poppad.h"
#include "resource.h"

// poppad.cpp
LRESULT CALLBACK MyEditProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
void poppad_setedit( int );
void DoCaption ( char *szTitleName, int TabID );

void __stdcall OnFootyChange(int id, void *pParam, int nStatus);
void __stdcall OnFootyContextMenu(void *pParam, int id);


// main.cpp
//void ConfigEditColor( COLORREF fore, COLORREF back );

// popfont.cpp
void PopFontSetEditFont();

extern HWND     hwndClient;
extern HWND     hwndTab;
extern HWND     hwbak;
extern HWND     hwndDummy;
extern int      activeID;
extern int      activeFootyID;
extern WNDPROC	Org_EditProc;
extern BOOL     bNeedSave ;
extern char     szDirName[_MAX_PATH] ;
extern char     szFileName[_MAX_PATH] ;
extern char     szTitleName[_MAX_FNAME + _MAX_EXT] ;
extern char     szStartDir[_MAX_PATH];
static HMENU    hMenu = NULL;

// タブを作成
// Create a tab
//
// 引数(Parameters)
// nTabNumber:  挿入先のタブ番号(Number of target tab's insert point)
// szTitleName: キャプション(The caption)
// szFileName:  管理するファイルのファイル名(File name of file managed by target tab)
// szDirName:   管理するファイルのディレクトリ名(Directory name of file managed by target tab)
//
// 戻り値
// ありません(Nothing)
void CreateTab(int nTabNumber, const char *szNewTitleName, const char *szNewFileName, const char *szNewDirName)
{
	int i, j, ret;
	HWND hWnd;
	TABINFO *lpTabInfo, *lpTopTabInfo;
	TCITEM tc_item;
	RECT rect;
	char szCaption[_MAX_PATH+128];
	MENUITEMINFO mii;

	// Footyのウィンドウを作成
	// Create a Footy window
	GetClientRect(hwndTab, &rect);
	TabCtrl_AdjustRect(hwndTab, FALSE, &rect);
	for(i = 0; ; i++){
		ret = FootyCreate(i, hwndDummy, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
        if(ret == F_RET_OK)
			break;
		if(ret == F_RET_OUTID){
			FootyRedim(i+128);
			FootyCreate(i, hwndDummy, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
			break;
		}
	}
	FootySetVisible(i, false);
	SetParent(FootyGetWnd(i), hwndTab);

	FootySetMetrics(i, F_SM_CLICKABLE_MODE, 0);
	FootySetMetrics(i, F_SM_UNDONUM, -1);
	FootySetMetrics(i, F_SM_OLEMODE, F_OLE_CTRL);
	FootyEventChange(i, (pFuncChange)OnFootyChange, NULL);
	FootyEventContextMenu(i, (pFuncContext)OnFootyContextMenu, NULL);

	SetEditColor(i);
	poppad_setedit(i);		// 起動直後の設定反映(onitama:050218)
	SetClassify(i);

	hWnd = FootyGetWnd(i);
	Org_EditProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MyEditProc);
	DragAcceptFiles(hWnd, TRUE);
	PopFontSetEditFont();

	// タブ情報の構造体を作成する
	// Create a struct of new tab information
	lpTabInfo = (TABINFO *)malloc(sizeof(TABINFO));
	lstrcpy(lpTabInfo->TitleName, szNewTitleName);
	lstrcpy(lpTabInfo->FileName, szNewFileName);
	lstrcpy(lpTabInfo->DirName, szNewDirName);
	lpTabInfo->LatestUndoNum = 0;
	lpTabInfo->NeedSave      = FALSE ;
 	lpTabInfo->FootyID       = i;
	lpTabInfo->FileIndex     = 0;

	// Z オーダーのリストに加える
	// Include tab in Z-order list
	lpTopTabInfo = GetTabInfo(activeID);
	if(lpTopTabInfo != NULL){
		lpTabInfo->ZOrder.prev = lpTopTabInfo;
		lpTabInfo->ZOrder.next = lpTopTabInfo->ZOrder.next;
		(lpTopTabInfo->ZOrder.next)->ZOrder.prev = lpTabInfo;
		lpTopTabInfo->ZOrder.next = lpTabInfo;
	}
	else lpTabInfo->ZOrder.next = lpTabInfo->ZOrder.prev = lpTabInfo;

	// タブの名前を登録
	// Register tab name
	if(szNewTitleName[0] == '\0') strcpy(szCaption, TABUNTITLED);
	else strcpy(szCaption, szNewTitleName);

	// 新しいタブを挿入
	// Insert a new tab
	if(nTabNumber < 0) nTabNumber = TabCtrl_GetItemCount(hwndTab);
	tc_item.mask = TCIF_TEXT | TCIF_PARAM;
	tc_item.pszText = szCaption;
	tc_item.lParam = (LPARAM)lpTabInfo;
	TabCtrl_InsertItem(hwndTab, nTabNumber, &tc_item);
	TabCtrl_SetCurSel(hwndTab, nTabNumber);
	if(lpTopTabInfo != NULL)
		activeID = GetTabID(lpTopTabInfo->FootyID);
	else
		activeID = -1;

	//
	// メニューに項目を追加
	if(hMenu == NULL)
		hMenu =  GetSubMenu(GetMenu(hwbak), POS_WINDOW);

	mii.cbSize     = sizeof(MENUITEMINFO);
	mii.fMask      = MIIM_ID | MIIM_TYPE;
	mii.fType      = MFT_RADIOCHECK | MFT_STRING;
	mii.wID        = IDM_ACTIVATETAB + nTabNumber;
	mii.dwTypeData = (LPSTR)(szNewTitleName[0] == '\0' ? TABUNTITLED : szNewTitleName);
	InsertMenuItem(hMenu, POS_TABBASE + nTabNumber, TRUE, &mii);

	mii.fMask = MIIM_ID;
	for(j = nTabNumber + 1; j < GetMenuItemCount(hMenu); j++){
		mii.wID = IDM_ACTIVATETAB + j;
		SetMenuItemInfo(hMenu, POS_TABBASE + j, TRUE, &mii);
	}
	DrawMenuBar(hwbak);

	ActivateTab(activeID, nTabNumber);

	return;
}

// タブを削除する
// Delete a tab
//
// 引数(Parameters)
// nTabNumber:  タブ番号(Number of target tab)
//
// 戻り値(Result)
// ありません(Nothing)
void DeleteTab(int nTabNumber)
{
	TABINFO *lpTabInfo;
	MENUITEMINFO mii;
	int n = TabCtrl_GetItemCount(hwndTab), i, nFootyID;

	lpTabInfo = GetTabInfo(nTabNumber);
	if(lpTabInfo == NULL || (n == 1 && lpTabInfo->FileName[0] == '\0' && FootyGetMetrics(0, F_GM_UNDOREM) <= 0
		&& FootyGetMetrics(0, F_GM_REDOREM) <= 0))
		return;

	nFootyID = lpTabInfo->FootyID;
	(lpTabInfo->ZOrder.prev)->ZOrder.next = lpTabInfo->ZOrder.next;
	(lpTabInfo->ZOrder.next)->ZOrder.prev = lpTabInfo->ZOrder.prev;
	free(lpTabInfo);
	TabCtrl_DeleteItem(hwndTab, nTabNumber);
	DeleteMenu(hMenu, POS_TABBASE + nTabNumber, MF_BYPOSITION);

	// 新しい操作対象となるタブを取得
	// Get a new current tab
	if(--n > 0){
		if(activeID == nTabNumber) TabCtrl_SetCurSel(hwndTab, n <= activeID ? n - 1 : activeID);
		else if(activeID > nTabNumber) TabCtrl_SetCurSel(hwndTab, activeID-1);
		activeID = TabCtrl_GetCurSel(hwndTab);
		if(GetTabInfo(activeID) != NULL)
			ActivateTab(-1, activeID);
		else activeFootyID = -1;
	} else CreateTab(0, "", "", "");

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask  = MIIM_ID;
	for(i = nTabNumber; i < GetMenuItemCount(hMenu); i++){
		mii.wID = IDM_ACTIVATETAB + i;
		SetMenuItemInfo(hMenu, POS_TABBASE + i, TRUE, &mii);
	}

	FootyDelete(nFootyID);
	return;
}

// タブ情報を取得
// Get a tab information
//
// 引数(Parameters)
// nTabNumber:  タブ番号(Number of target tab)
//
// 戻り値(Result)
// タブ情報の構造体へのポインタを返します。
// Return a pointer to a target tab information
TABINFO *GetTabInfo(int nTabNumber)
{
	TCITEM tc_item;

	tc_item.mask = TCIF_PARAM;
	if(!TabCtrl_GetItem(hwndTab, nTabNumber, &tc_item))
		return NULL;
    return (TABINFO *)tc_item.lParam;
}

// タブ情報を設定
// Set a tab information
//
// 引数(Parameters)
// nTabNumber:  タブ番号(Number of target tab.)
// szTitleName: キャプション(The caption.)
// szFileName:  管理するファイルのファイル名(File name of file managed by target tab.)
// szDirName:   管理するファイルのディレクトリ名(Directory name of file managed by target tab.)
// bNeedSave:   保存が必要ならTRUE、それ以外ならFALSE
//              If this parameter is TRUE, the string target tab has need to save; otherwise, it doesn't.
//
// 戻り値(Result)
// ありません(Nothing)
//
// 備考(Remarks)
// szTitleName, szFileName, szDirNameにNULLを指定すると、そのパラメータは無視されます。
// If szTitleName, szFileName, szDirName is set NULL, the parameter is ignored.
void SetTabInfo(int nTabNumber, const char *szTitleName, const char *szFileName, const char *szDirName, BOOL bNeedSave)
{
	MENUITEMINFO mii;
	char *szMenuText;
	TABINFO *lpTabInfo = GetTabInfo(nTabNumber);
	if(lpTabInfo){
		if(szTitleName != NULL) lstrcpy(lpTabInfo->TitleName, szTitleName);
		if(szFileName  != NULL) lstrcpy(lpTabInfo->FileName,  szFileName);
		if(szDirName   != NULL) lstrcpy(lpTabInfo->DirName,   szDirName);
		if(bNeedSave >= 0) lpTabInfo->NeedSave = bNeedSave;

		szMenuText = (char *)malloc(max(lstrlen(lpTabInfo->TitleName), lstrlen(TABUNTITLED)) + 3);
		lstrcpy(szMenuText, lpTabInfo->TitleName[0] == '\0' ? TABUNTITLED : lpTabInfo->TitleName);
		if(lpTabInfo->NeedSave) lstrcat(szMenuText, " *");
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask  = MIIM_TYPE;
		mii.fType  = MFT_STRING | MFT_RADIOCHECK;
		mii.dwTypeData = szMenuText;
		SetMenuItemInfo(hMenu, POS_TABBASE + nTabNumber, TRUE, &mii);
		DrawMenuBar(hwbak);
		free(szMenuText);
	}

	return;
}

// 条件を満たすタブを検索
// Search a tab matching up to a conditional
//
// 引数(Parameters)
// szTitleName: キャプション(The caption)
// szFileName:  管理するファイルのファイル名(File name of file managed by target tab)
// szDirName:   管理するファイルのディレクトリ名(Directory name of file managed by target tab)
// ullFileIndex: ファイル インデックス
//
// 戻り値(Result)
// 見つかった場合は対象のタブ番号、見つからなければ-1を返します。
// If finder could find a target, return number of target, or else return -1
int SearchTab(const char *szTitleName, const char *szFileName, const char *szDirName, ULONGLONG ullFileIndex)
{
	TABINFO *lpTabInfo;
	int num = TabCtrl_GetItemCount(hwndTab);

	for(int i = 0; i < num; i++){
		lpTabInfo = GetTabInfo(i);

		if((szTitleName  && !strcmp(szTitleName, lpTabInfo->TitleName))
		|| (szFileName   && !strcmp(szFileName,  lpTabInfo->FileName))
		|| (szDirName    && !strcmp(szDirName,   lpTabInfo->DirName))
		|| (ullFileIndex && ullFileIndex == lpTabInfo->FileIndex))
			return i;
	}
	return -1;
}

// タブをアクティブにする
// Activate a tab
//
// 引数(Parameters)
// nTabNumber: タブ番号(Number of target tab)
//
// 戻り値(Result)
// ありません(Nothing)
#include "support.h"
void ActivateTab(int nTabNumber1, int nTabNumber2)
{
	TABINFO *lpTabInfo1, *lpTabInfo2;
	RECT rect;

	if(nTabNumber1 == nTabNumber2) return;

	lpTabInfo1 = GetTabInfo(nTabNumber1);
	lpTabInfo2 = GetTabInfo(nTabNumber2);
	if(lpTabInfo2 != NULL){
		activeID = nTabNumber2;
		activeFootyID = lpTabInfo2->FootyID;

		bNeedSave = lpTabInfo2->NeedSave;
		strcpy(szTitleName, lpTabInfo2->TitleName);
		strcpy(szFileName, lpTabInfo2->FileName);
		strcpy(szDirName, lpTabInfo2->DirName);
		DoCaption (szTitleName, activeID);

		SetCurrentDirectory(lpTabInfo2->DirName);
		TabCtrl_SetCurSel(hwndTab, nTabNumber2);
		TabCtrl_SetCurFocus(hwndTab, nTabNumber2);

		GetClientRect(hwndTab, &rect);
		TabCtrl_AdjustRect(hwndTab, FALSE, &rect);
		FootySetPos(activeFootyID, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
		FootySetVisible(activeFootyID, true);
		FootySetFocus(activeFootyID);
		poppad_setsb_current(activeFootyID);
		FootyRefresh(activeFootyID);

		CheckMenuRadioItem(hMenu, POS_TABBASE, GetMenuItemCount(hMenu) - 1, POS_TABBASE + nTabNumber2, MF_BYPOSITION);
	}
	else activeID = activeFootyID = -1;
	if(lpTabInfo1 != NULL)
		FootySetVisible(lpTabInfo1->FootyID, false);

	return;
}

// Zオーダーの順序を変更する
// Change Z-Order.
//
// 引数(Parameters)
// nTabNumber1: トップだったタブ番号(Number of tab which was top tab)
// nTabNumber2: トップにするタブ番号(Number of tab been top tab)
//
// 戻り値(Result)
// ありません(Nothing)
void ChangeZOrder(int nTabNumber1, int nTabNumber2)
{
	TABINFO *lpTabInfo1, *lpTabInfo2;
	lpTabInfo1 = GetTabInfo(nTabNumber1);
	lpTabInfo2 = GetTabInfo(nTabNumber2);
	if(lpTabInfo1 != NULL && lpTabInfo2 != NULL){
		(lpTabInfo2->ZOrder.prev)->ZOrder.next = lpTabInfo2->ZOrder.next;
		(lpTabInfo2->ZOrder.next)->ZOrder.prev = lpTabInfo2->ZOrder.prev;
		(lpTabInfo1->ZOrder.next)->ZOrder.prev = lpTabInfo2;
		lpTabInfo2->ZOrder.prev = lpTabInfo1;
		lpTabInfo2->ZOrder.next = lpTabInfo1->ZOrder.next;
		lpTabInfo1->ZOrder.next = lpTabInfo2;
	}

}


// FootyのIDからIDを取得する
// Get tab ID by Footy ID
//
// 引数(Parameters)
// lpTabInfo: FootyのID(ID of Footy)
//
// 戻り値(Result)
// 渡されたFootyIDを持つタブのID
// ID of tab which has Footy ID
int GetTabID(int FootyID)
{
	TABINFO *lpTabInfo;
	int num = TabCtrl_GetItemCount(hwndTab);

    for(int i = 0; i < num; i++){
		lpTabInfo = GetTabInfo(i);
		if(lpTabInfo != NULL){
			if(lpTabInfo->FootyID == FootyID)
				return i;
		}
	}
	return -1;
}