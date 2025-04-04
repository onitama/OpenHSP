
//
//		HSP debug window support functions for HSP3
//				onion software/onitama 2005
//

#pragma comment(lib, "comctl32.lib")

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>

#include "supio.h"
#include "resource.h"
#include "hspdll.h"
#include "hsp3dbgwin.h"
#include "sortnote.h"

static HSP3DEBUG *g_debug;
static HWND hDlgWnd;
static HINSTANCE myinst;

#define IDU_TAB 100
#define ID_BTN1 1000
#define ID_BTN2 1001
#define ID_BTN3 1002
#define ID_BTN4 1003

#define TABDLGMAX 4
#define myClass TEXT("HSP3DEBUG")

#define DIALOG_X0 5
#define DIALOG_Y0 5
#define DIALOG_X1 366
#define DIALOG_Y1 406
#define DIALOG_Y2 23

#define WND_SX 380
#define WND_SY 480

static HWND g_hTabCtrl;
static HWND g_hGenList;
static HWND g_hTabSheet[TABDLGMAX];

static HWND g_hVarPage = NULL;
static HWND g_hVarList = NULL;
static HWND g_hVarEdit = NULL;

static HWND g_hBtn1;
static HWND g_hBtn2;
static HWND g_hBtn3;
static HWND g_hBtn4;
static HWND g_hSttCtrl;

static HWND g_hLogPage;
static HWND g_hLogEdit;

static HWND g_hCallstackPage;
static HWND g_hCallstackEdit;

typedef BOOL (CALLBACK *HSP3DBGFUNC)(HSP3DEBUG *,int,int,int);

//----------------------------------------------------------

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved) {
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		myinst = hInstance;
		hDlgWnd = NULL;
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		if ( hDlgWnd != NULL ) {
			DestroyWindow( hDlgWnd );
			hDlgWnd = NULL;
		}
	}
	return TRUE ;
}

//----------------------------------------------------------

static HWND GenerateObj( HWND parent, LPTSTR name, LPTSTR ttl, int x, int y, int sx, int sy, int menu, HFONT font )
{
	HWND h;
	h = CreateWindow( name, ttl,
				WS_CHILD|WS_VISIBLE,
				x, y, sx, sy,
				parent,
				(HMENU)menu,
				myinst,
				NULL );

	if ( font != NULL ) SendMessage( h, WM_SETFONT, (WPARAM)font, TRUE );
	return h;
}


static void TabGeneralInit( void )
{
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = 100;
	col.pszText = TEXT("項目");
	col.iSubItem = 0;
	ListView_InsertColumn( g_hGenList , 0 , &col);
	col.cx = 400;
	col.iSubItem = 1;
	col.pszText = TEXT("内容");
	ListView_InsertColumn( g_hGenList , 1 , &col);
}


static void TabGeneralReset( void )
{
	LV_ITEM item;
	int chk, tgmax;
	char *p0;
	LPTSTR p;
	TCHAR name[256];
	TCHAR val[512];
	HSPAPICHAR *hactmp1;

	ListView_DeleteAllItems( g_hGenList );
	tgmax = 0;

	p0 = g_debug->get_value(DEBUGINFO_GENERAL);
	p = chartoapichar(p0,&hactmp1);		// HSP側に問い合わせ
	strsp_ini();
	while(1) {
		chk = strsp_get( p, name, 0, 255 );
		if ( chk == 0 ) break;
		chk = strsp_get( p, val, 0, 511 );
		if ( chk == 0 ) break;

		item.mask = LVIF_TEXT;
		item.iItem = tgmax;
		item.iSubItem = 0;
		item.pszText = name;
		ListView_InsertItem( g_hGenList, &item );
		item.iSubItem = 1;
		item.pszText = val;
		ListView_SetItem( g_hGenList, &item );

		tgmax++;
	}
	g_debug->dbg_close( p0 );
	freehac(&hactmp1);
}


static void CurrnetUpdate( void )
{
	TCHAR tmp[512];
	LPTSTR fn;
	char *p0;
	HSPAPICHAR *hactmp1;

	g_debug->dbg_curinf();
	fn = chartoapichar(g_debug->fname,&hactmp1);
	if ( fn == NULL ) fn = TEXT("???");
	_stprintf( tmp,TEXT("%s\n( line:%d )"), fn, g_debug->line );
	SetWindowText( g_hSttCtrl, tmp );
	freehac(&hactmp1);

	p0 = g_debug->dbg_callstack();
	SetWindowText( g_hCallstackEdit, chartoapichar(p0,&hactmp1) );
	g_debug->dbg_close( p0 );
	freehac(&hactmp1);
}


static int GetTabVarsOption( void )
{
	int opt;
	opt = 0;
	if ( IsDlgButtonChecked( g_hVarPage, IDC_SORT ) ) opt|=1;
	if ( IsDlgButtonChecked( g_hVarPage, IDC_MODULE ) ) opt|=2;
	if ( IsDlgButtonChecked( g_hVarPage, IDC_ARRAY ) ) opt|=4;
	if ( IsDlgButtonChecked( g_hVarPage, IDC_DUMP ) ) opt|=8;
	return opt;
}


static void TabVarsReset( void )
{
	char *p0;
	LPTSTR p;
	TCHAR name[256];
	int chk;
	int opt;

	opt = GetTabVarsOption();

	if (g_hVarList == NULL) return;
/*
	long style;
	style = GetWindowLong( g_hVarList, GWL_STYLE );
	if ( opt & 1 ) { style |= LBS_SORT; } else { style &= ~LBS_SORT; }
	SetWindowLong( g_hVarList, GWL_STYLE, style );
*/
	SendMessage( g_hVarList, LB_RESETCONTENT, 0, 0L );
	p0 = g_debug->get_varinf( NULL, opt );					// HSP側に問い合わせ
	chartoapichar(p0, &p);
	if ( opt & 1 ) SortNote( p );

	strsp_ini();
	while(1) {
		chk = strsp_get( p, name, 0, 255 );
		if ( chk == 0 ) break;
		SendMessage(g_hVarList, LB_INSERTSTRING, 0, (LPARAM)name);
	}
	freehac(&p);
	g_debug->dbg_close( p0 );
}


static void TabVarsUpdate( void )
{
	TCHAR tmp[256];
	char *p;
	int i;
	HSPCHAR *hctmp1;
	HSPAPICHAR *hactmp1;
	i = (int)SendMessage( g_hVarList, LB_GETCURSEL,0,0L );
	if ( i < 0 ) return;
	SendMessage( g_hVarList, LB_GETTEXT, i, (LPARAM)tmp );
	p = g_debug->get_varinf( apichartohspchar(tmp,&hctmp1), GetTabVarsOption() );		// HSP側に問い合わせ
	freehc(&hctmp1);
	SetWindowText( g_hVarEdit, chartoapichar(p,&hactmp1) );
	freehac(&hactmp1);
	g_debug->dbg_close( p );
}


static void TabLogAdd( LPTSTR str )
{
	int size;
	size = Edit_GetTextLength( g_hLogEdit );
	Edit_SetSel( g_hLogEdit,size, size );
	Edit_ReplaceSel( g_hLogEdit, str );
	Edit_ScrollCaret( g_hLogEdit );
}


//----------------------------------------------------------

LRESULT CALLBACK TabGeneralProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	//		GENERALタブ
	//
	switch (msg) {
	case WM_INITDIALOG:
		g_hGenList = GetDlgItem( hDlg, IDC_LIST1 );
		TabGeneralInit();
		TabGeneralReset();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_BUTTON1:
			TabGeneralReset();
			break;
		}
		return FALSE;
	}
	return FALSE;
}


LRESULT CALLBACK TabVarsProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	//		Variablesタブ
	//
	switch (msg) {
	case WM_INITDIALOG:
		g_hVarPage = hDlg;
		g_hVarList = GetDlgItem( hDlg, IDC_LIST1 );
		g_hVarEdit = GetDlgItem( hDlg, IDC_EDIT1 );
		TabVarsReset();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDC_LIST1:
			TabVarsUpdate();
			break;
		case IDC_SORT:
		case IDC_MODULE:
		case IDC_ARRAY:
		case IDC_DUMP:
			TabVarsReset();
			TabVarsUpdate();
			break;
		}
		return FALSE;
	}
	return FALSE;
}


LRESULT CALLBACK TabLogProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	//		Logタブ
	//
	HFONT hf;
	switch (msg) {
	case WM_INITDIALOG:
		g_hLogPage = hDlg;
		g_hLogEdit = GetDlgItem( hDlg, IDC_EDIT1 );
		SendMessageA(g_hLogEdit, EM_LIMITTEXT, 0, 0);
		hf = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
		g_hBtn4 = GenerateObj( g_hLogPage, TEXT("button"), TEXT("ログ消去"), 266/*DIALOG_X0+168*/, 326/*DIALOG_Y1+4*/, 80, 24, ID_BTN4, hf );
	return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
			case ID_BTN4:
				SetWindowText( g_hLogEdit, TEXT("") );
			break;
		}
	return TRUE;
	}

	return FALSE;
}

LRESULT CALLBACK TabCallstackProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	//      Callstackタブ
	//
	switch (msg) {
	case WM_INITDIALOG:
		g_hCallstackPage = hDlg;
		g_hCallstackEdit = GetDlgItem( hDlg, IDC_EDIT1 );
		return TRUE;
	}
	return FALSE;
}


// 親ダイアログのコールバック関数
LRESULT CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	TCITEM	tc;
	RECT	rt;
	LPPOINT pt = (LPPOINT) &rt;
	HFONT hf;
	int		i;

	switch (msg) {

	// 初期化
	case WM_CREATE:

		hf = (HFONT)GetStockObject( DEFAULT_GUI_FONT );
		g_hTabCtrl = GenerateObj( hDlg, WC_TABCONTROL, TEXT(""), DIALOG_X0, DIALOG_Y0, DIALOG_X1, DIALOG_Y2, IDU_TAB, hf );
		g_hSttCtrl = GenerateObj( hDlg, TEXT("static"), TEXT(""), DIALOG_X0+180, DIALOG_Y1+4, DIALOG_X1-180, 48, 0, hf );
		g_hBtn1 = GenerateObj( hDlg, TEXT("button"), TEXT("実行"), DIALOG_X0+8, DIALOG_Y1+4, 80, 24, ID_BTN1, hf );
		g_hBtn2 = GenerateObj( hDlg, TEXT("button"), TEXT("次行"), DIALOG_X0+88, DIALOG_Y1+4, 40, 24, ID_BTN2, hf );
		g_hBtn3 = GenerateObj( hDlg, TEXT("button"), TEXT("停止"), DIALOG_X0+128, DIALOG_Y1+4, 40, 24, ID_BTN3, hf );

		tc.mask = TCIF_TEXT;
		tc.pszText = TEXT("全般");
		TabCtrl_InsertItem(g_hTabCtrl , 0, &tc);
		g_hTabSheet[0] = CreateDialog( myinst, TEXT("T_GENERAL"),
			hDlg, (DLGPROC) TabGeneralProc );

		tc.mask = TCIF_TEXT;
		tc.pszText = TEXT("変数");
		TabCtrl_InsertItem(g_hTabCtrl , 1, &tc);
		g_hTabSheet[1] = CreateDialog( myinst, TEXT("T_VAR"),
			hDlg, (DLGPROC) TabVarsProc );

		tc.mask = TCIF_TEXT;
		tc.pszText = TEXT("ログ");
		TabCtrl_InsertItem(g_hTabCtrl , 2, &tc);
		g_hTabSheet[2] = CreateDialog( myinst, TEXT("T_LOG"),
			hDlg, (DLGPROC) TabLogProc );

		tc.mask = TCIF_TEXT;
		tc.pszText = TEXT("コールスタック");
		TabCtrl_InsertItem(g_hTabCtrl , 3, &tc);
		g_hTabSheet[3] = CreateDialog( myinst, TEXT("T_CALL"),
			hDlg, (DLGPROC) TabCallstackProc );

		//GetClientRect(g_hTabCtrl, &rt);
		SetRect( &rt, 8, DIALOG_Y2+4, DIALOG_X1+8, DIALOG_Y1+4 );
		//TabCtrl_AdjustRect(g_hTabCtrl, FALSE, &rt);
		//MapWindowPoints(g_hTabCtrl, hDlg, pt, 2);

		// 生成した子ダイアログをタブシートの上に貼り付ける
		for (i=0;i<TABDLGMAX;i++) {
			MoveWindow( g_hTabSheet[i], rt.left, rt.top,
				rt.right - rt.left, rt.bottom - rt.top, FALSE);
		}

		// デフォルトで左側のタブを表示
		ShowWindow(g_hTabSheet[0], SW_SHOW);
		return TRUE;

	case WM_NOTIFY:
		{
		int cur;
		NMHDR *nm = (NMHDR *) lp;		// タブコントロールのシート切り替え通知
		cur = TabCtrl_GetCurSel(g_hTabCtrl);
		for (i=0;i<TABDLGMAX;i++) {
			if ( i == cur ) {
				ShowWindow(g_hTabSheet[i], SW_SHOW);
			} else {
				ShowWindow(g_hTabSheet[i], SW_HIDE);
			}
		}
		break;
		}
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case ID_BTN1:
			g_debug->dbg_set( HSPDEBUG_RUN );
			SetWindowText( g_hSttCtrl, TEXT("") );
			break;
		case ID_BTN2:
			g_debug->dbg_set( HSPDEBUG_STEPIN );
			SetWindowText( g_hSttCtrl, TEXT("") );
			break;
		case ID_BTN3:
			g_debug->dbg_set( HSPDEBUG_STOP );
			break;
		}
		break;

	case WM_CLOSE:
		return FALSE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hDlg,msg,wp,lp));
}

//----------------------------------------------------------

EXPORT BOOL WINAPI debugini ( HSP3DEBUG *p1, int p2, int p3, int p4 )
{
	//
	//		debugini ptr  (type1)
	//
	int dispx,dispy;
	HSPCTX *ctx;
	WNDCLASS wndclass;

	g_debug = p1;
	ctx = (HSPCTX *)p1->hspctx;
	dispx = GetSystemMetrics( SM_CXSCREEN );
	dispy = GetSystemMetrics( SM_CYSCREEN );

	wndclass.style=CS_HREDRAW|CS_VREDRAW;
	wndclass.lpfnWndProc=DlgProc;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance =myinst;
	wndclass.hIcon=NULL;
	wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszMenuName=NULL;
	wndclass.lpszClassName=myClass;
	RegisterClass(&wndclass);

	hDlgWnd = CreateWindow( myClass,
		TEXT("Debug Window"),
		WS_CAPTION | WS_OVERLAPPED | WS_BORDER | WS_VISIBLE,
		dispx-WND_SX, 0,
		WND_SX, WND_SY,
		NULL,
		NULL,
		myinst,
		NULL);

	//hDlgWnd = CreateDialog( myinst, "HSP3DEBUG", NULL, (DLGPROC)DlgProc );
	if ( hDlgWnd == NULL ) {
		MessageBox( NULL, TEXT("Debug window initalizing failed."), TEXT("Error"), 0 );
	}

	ShowWindow( hDlgWnd, SW_SHOW );
    UpdateWindow( hDlgWnd );

	return 0;
}


EXPORT BOOL WINAPI debug_notice ( HSP3DEBUG *p1, int p2, int p3, int p4 )
{
	//
	//		debug_notice ptr  (type1)
	//			p2: 0=stop event
	//			    1=send message
	//
	int cur;
	HSPCTX *ctx;
	HSPAPICHAR *hactmp1;

	if ( p2 == 1 ) {
		ctx = (HSPCTX *)p1->hspctx;
		strcat( ctx->stmp, "\r\n" );
		TabLogAdd( chartoapichar(ctx->stmp,&hactmp1) );
		freehac(&hactmp1);
		return 0;
	}

	CurrnetUpdate();
	cur = TabCtrl_GetCurSel(g_hTabCtrl);
	switch( cur ) {
	case 0:
		TabGeneralReset();
		break;
	case 1:
		TabVarsUpdate();
		break;
	}
	return 0;
}


//----------------------------------------------------------
