/*================================================================
Footyテキストエディタコントロール用ヘッダ

このヘッダファイルを使用するファイルでインクルードしてください。
#include "FootyDll.h"

※注意※
このファイルを編集すると想像も出来ないことが起こってしまう可能性が
あったりなかったりしないでもないので、おとなしく使ってください。

Copyright(C)2003-2004 ShinjiWatanabe
================================================================*/

#ifndef _FOOTY_DLL_H_
#define _FOOTY_DLL_H_

//エクスポート
#ifndef FOOTYEXPORT
	#define FOOTYEXPORT	extern "C" __declspec(dllexport)
#endif

//必要なファイルのインクルード
#include <windows.h>
#include <tchar.h>

//UNICODE版は現在整備中です。
#ifdef _UNICODE
	#undef _UNICODE
#endif
#ifdef UNICODE
	#undef UNICODE
#endif

//静的リンクライブラリ専用関数
#ifdef FOOTYSTATIC
	FOOTYEXPORT bool FootyStart(HINSTANCE hInstance);
	FOOTYEXPORT void FootySetCursor(int OnLine,int OnUrl);
	FOOTYEXPORT void FootyEnd();
#endif

//便利なマクロ//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ONLYLEVEL(x)		(1 << (23 + (x)))									/*FootyAddEmphasis用*/

//メッセージ用の定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	FE_CHANGE			WM_USER+1000										/*変更があったときに送られます。*/
#define		FECH_SB			0													/*シングルバイト文字が挿入されたとき*/
#define		FECH_IME		1													/*ＩＭＥによる文字列*/
#define		FECH_RETURN		2													/*改行*/
#define		FECH_BS			3													/*バックスペース*/
#define		FECH_DEL		4													/*Deleteキー*/
#define		FECH_PASTE		5													/*貼り付け*/
#define		FECH_CUT		6													/*カット*/
#define		FECH_UNDO		7													/*アンドゥ*/
#define		FECH_REDO		8													/*リドゥ*/
#define		FECH_TAB		9													/*タブが押された*/
#define		FECH_SETSEL		10													/*FootySetSelText命令*/
#define		FECH_SETTEXT	11													/*FootySetText命令*/
#define		FECH_INDENT		12													/*インデント処理が行われたとき*/
#define		FECH_REINDENT	13													/*逆インデント処理が行われたとき*/
#define		FECH_OLE		14													/*OLE DnDが行われたとき*/

#define	FE_MOUSEMOVE		WM_USER+1001										/*マウスが動かされたときに発生します。*/
#define		FEMM_NONE		0													/*通常の状態です。*/
#define		FEMM_ONLN		1													/*行番号上にあります。*/
#define		FEMM_ONRL		2													/*ルーラ上にあります。*/
#define		FEMM_DRAG		3													/*ドラッグ中です。*/
#define		FEMM_DRAGLN		4													/*行番号をドラッグ中です。*/
#define		FEMM_ON_URL		5													/*ＵＲＬ上にあります。*/
#define		FEMM_ON_MAIL	6													/*メールアドレス上にあります。*/

#define FE_CONTEXTMENU		WM_USER+1002										/*コンテキストメニューの呼び出し要求*/

#define FE_CARETMOVE		WM_USER+1003										/*カーソルが動かされた*/
#define		FECM_COMMAND	0													/*コマンドによって動かされた*/
#define		FECM_KEY		1													/*キーの入力*/
#define		FECM_MOUSE		2													/*マウスの入力*/
#define		FECM_OLE		3													/*OLE関連*/

#define FE_SELCHANGE		WM_USER+1004										/*選択状態が変更された*/
#define FE_INSCHANGE		WM_USER+1005

//マーク用の定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_MARK_NONE			0													/*何もなし*/
#define F_MARK_BB			1													/*青い棒です。*/
#define F_MARK_RB			2													/*赤い棒です。*/
#define F_MARK_GB			3													/*緑の棒です。*/

//強調単語の定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EMP_INVALIDITY		0													/*無効*/
#define EMP_WORD			1													/*単語を強調*/
#define EMP_LINE_AFTER		2													/*それ以降～行末*/
#define EMP_LINE_BETWEEN	3													/*二つの文字の間（同一行に限る・二つの文字列を指定）*/
#define EMP_MULTI_BETWEEN	4													/*二つの文字の間（複数行にわたる・二つの文字列を指定・最大二組まで）*/

//マーク定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_SC_TAB			0													/*タブ文字*/
#define F_SC_HALFSPACE		1													/*半角スペース*/
#define F_SC_NORMALSPACE	2													/*全角スペース*/
#define F_SC_UNDERLINE		3													/*カーソル下アンダーライン*/
#define F_SC_CRLF			4													/*CRLF改行*/
#define F_SC_EOF			5													/*[EOF]*/

//色指定用定数定数/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_SC_DEFAULT		6													/*通常文字列*/
#define F_SC_LINENUM		7													/*行番号表示文字列*/
#define F_SC_LINENUMBORDER	8													/*行番号とエディタの境界線*/
#define F_SC_CARETLINE		9													/*行番号に表示するキャレット位置の色*/
#define F_SC_RULER_TXT		10													/*ルーラーの文字色*/
#define F_SC_RULER_BK		11													/*ルーラーの背景色*/
#define F_SC_RULER_LINE		12													/*ルーラーの線の色*/
#define F_SC_CARETPOS		13													/*ルーラーに表示するキャレット位置の色*/
#define F_SC_URL			14													/*クリッカブルURLの色*/
#define F_SC_MAIL			15													/*クリッカブルメールアドレスの色*/
#define F_SC_HIGHLIGHT_TXT	16													/*強調表示文字列の文字色*/
#define F_SC_HIGHLIGHT_BK	17													/*強調表示文字列の背景色*/
#define F_SC_BACKGROUND		18													/*背景色*/
#define F_SC_URL_UNDER		19													/*URL下線*/
#define F_SC_MAIL_UNDER		20													/*メール下線*/

//強調表示用定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_SE_BOLD			1													/*太字にする*/
#define F_SE_NON_BS			2													/*大文字と小文字を区別しない*/
#define F_SE_INDEPENDENCE_A	4													/*独立した文字列のみ*/
#define F_SE_INDEPENDENCE_B	8													/*独立した文字列のみ*/
#define F_SE_INDEPENDENCE_C	16													/*独立した文字列のみ*/
#define F_SE_INDEPENDENCE_D	32													/*独立した文字列のみ*/
#define F_SE_HEAD			64													/*行頭にあるときのみ*/

//生成時表示・非表示切り替え///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CS_SHOW			0													/*表示*/
#define F_CS_HIDE			1													/*非表示*/

//数値設定用定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_SM_TAB			0													/*タブの文字数*/
#define F_SM_RULER			1													/*ルーラーの高さ*/
#define F_SM_LINENUM		2													/*行番号表示領域*/
#define F_SM_ROP2			3													/*SetROP2で使用する値*/
#define F_SM_FN_WEIGHT		4													/*強調表示の太字のfnWeightの値*/
#define F_SM_WHEELSCROLL	5													/*ホイールマウスのスクロール行*/
#define F_SM_LINESPACE		6													/*行間スペース*/
#define F_SM_CLICKABLE_MODE	7													/*クリッカブルモード*/
#define F_SM_OLEMODE		8													/*OLEモード*/
#define F_SM_URLLINEMODE	9													/*URL下線の描画方法*/
#define F_SM_MAILLINEMODE	10													/*メールアドレス下線の描画方法*/
#define F_SM_BORDERLINEMODE	11													/*境界線の描画方法*/
#define F_SM_INSERTMODE		12													/*挿入状態*/
#define F_SM_LINEMARGIN		13													/*行とのマージン*/
#define F_SM_OWNDRAWMODE	14													/*オーナードローモード*/
#define F_SM_CREATESHOW		15													/*生成時の表示・非表示状態の切り替え*/
#define F_SM_UNDONUM		16													/*アンドゥの使用可能回数*/
#define F_SM_SELGETMODE		17													/*選択モード*/
#define F_SM_SETSELMODE		18													/*ダブルクリックしたときの選択方法*/

//数値取得用定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_GM_FONTWIDTH		0													/*フォント幅(半角)ピクセル*/
#define F_GM_FONTHEIGHT		1													/*フォント高さピクセル*/
#define F_GM_UNDOREM		2													/*アンドゥの残り使用可能回数*/
#define F_GM_REDOREM		3													/*リドゥの残り使用回数*/
#define F_GM_CARETPOS_X		4													/*キャレットの位置x座標*/
#define F_GM_CARETPOS_Y		5													/*キャレットの位置y座標*/

//改行コード定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define RETLINE_CRLF		1													/*キャリッジリターン＋ラインフィード*/
#define RETLINE_CR			2													/*キャリッジリターンのみ*/
#define RETLINE_LF			3													/*ラインフィードのみ*/
#define RETLINE_AUTO		4													/*自動出力*/

//数値取得用定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SELMODE_NORMAL		0													/*通常モード*/
#define SELMODE_NUMALPHA	1													/*アルファベットと数字を同時に選択*/

//クリッカブルモード定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_CM_ALLMODE		-1													/*オールモード(全機能を有効に)*/
#define F_CM_MAIL			1													/*メールアドレス*/
#define F_CM_HTTP			2													/*http://*/
#define F_CM_HTTPS			4													/*https://*/
#define F_CM_FTP			8													/*ftp://*/

//OLEモード定数////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_OLE_CUT			1													/*カットモード*/
#define F_OLE_COPY			2													/*コピーモード*/
#define F_OLE_N_IN			4													/*受け入れ拒否*/
#define F_OLE_N_OUT			8													/*出力拒否*/
#define F_OLE_CTRL			16													/*コントロールモード*/

//禁則処理定数/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_JPN_NONE			0													/*なし*/
#define F_JPN_SOFT			1													/*甘い*/
#define F_JPN_NORMAL		2													/*通常*/
#define F_JPN_HARD			3													/*厳しい*/

//オーナードロー用の定数///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_OD_NONE			0													/*無し*/
#define F_OD_ALL			-1													/*全部*/
#define F_OD_HALFSPACE		0x01												/*半角スペース*/
#define F_OD_NORMALSPACE	0x02												/*全角スペース*/
#define F_OD_TAB			0x04												/*タブ文字*/
#define F_OD_CRLF			0x08												/*改行*/
#define F_OD_EOF			0x10												/*[EOF]*/

#define ODT_FOOTY			1000												/*これがFootyであることを示す*/
#define ODS_HALFSPACE		0													/*半角スペース*/
#define ODS_NORMALSPACE		1													/*全角スペース*/
#define ODS_TAB				2													/*タブ*/
#define ODS_CRLF			3													/*改行*/
#define ODS_EOF				4													/*[EOF]*/

//戻り値の定数/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_RET_OK			-1													/*成功*/
#define F_RET_FAILED		-2													/*何らかの原因でDLL内部で処理が失敗した*/
#define F_RET_MEMORY		-3													/*メモリ不足による作業失敗*/
#define F_RET_PROGRAM		-4													/*プログラムから渡された引数が不正（IDをのぞく）*/
#define F_RET_OUTID			-5													/*プログラムから渡されたIDが不正*/
#define F_RET_CANCELED		-6													/*キャンセルされた*/
#define F_RET_CANNOTWRITE	-7													/*書き込み禁止状態になっている*/
#define F_RET_OVER_MAX		-8													/*強調単語を指定しすぎ*/
#define F_RET_OVER_LINE		-9													/*複数行にわたる単語を指定しすぎ*/
#define F_RET_OVER_INTE		-10													/*割り込み許可に単語を指定しすぎ*/
#define F_RET_ALREADY		-11													/*すでに生成されている*/
#define F_RET_NSELECTING	-12													/*選択していません。*/

//関数ポインタ/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef void (__stdcall *pFuncCaretMove)(int,void*,int);
typedef void (__stdcall *pFuncSelChange)(int,void*);
typedef void (__stdcall *pFuncInsChange)(int,void*,int);
typedef void (__stdcall *pFuncChange)(int,void*,int);
typedef void (__stdcall *pFuncContext)(int,void*);
typedef void (__stdcall *pFuncMouseMove)(int,void*,int);

//関数/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//バージョン関連
FOOTYEXPORT int __stdcall GetFootyVer();										/*Footyのバージョンを取得します。*/
FOOTYEXPORT int __stdcall GetFootyBetaVer();									/*Footyのβバージョンを取得します。*/

//FootyのID管理
FOOTYEXPORT int __stdcall FootyRedim(int num);									/*FootyのIDを確保したり確保しなおしたりします。*/
FOOTYEXPORT int __stdcall FootyDelete(int id);									/*確保されたIDのデータを消去してメモリ領域を確保します。*/
FOOTYEXPORT int __stdcall FootyCreate(int id,HWND hWnd,int x,
									  int y,int dx,int dy);						/*Footyを作成します。*/

//ウィンドウ関連
FOOTYEXPORT int __stdcall FootySetFocus(int id);								/*Footyにフォーカスをあわせます。*/
FOOTYEXPORT int __stdcall FootySetPos(int id,int x,int y,int dx,int dy);		/*Footyの位置、大きさを変更します。*/
FOOTYEXPORT int __stdcall FootyRefresh(int id);									/*Footyを再描写します。*/
FOOTYEXPORT int __stdcall FootySetVisible(int id,bool v);						/*Footyの表示・非表示を切り替えます。*/
FOOTYEXPORT HWND __stdcall FootyGetWnd(int id);									/*Footyのウィンドウハンドルを取得します。*/

//編集関連
FOOTYEXPORT int __stdcall FootyCopy(int id);									/*現在選択している文字をクリップボードへコピーします。*/
FOOTYEXPORT int __stdcall FootyCut(int id);										/*現在選択している文字をクリップボードへコピーして削除します。*/
FOOTYEXPORT int __stdcall FootyPaste(int id);									/*クリップボードから現在のキャレットの位置に貼り付けます。*/
FOOTYEXPORT int __stdcall FootyUndo(int id);									/*アンドゥ処理を行います。*/
FOOTYEXPORT int __stdcall FootyRedo(int id);									/*リドゥ処理を行います。*/
FOOTYEXPORT int __stdcall FootyIndent(int id);									/*インデント処理を行います。*/
FOOTYEXPORT int __stdcall FootyUnIndent(int id);								/*逆インデント処理を行います。*/
FOOTYEXPORT int __stdcall FootySelectAll(int id);								/*全て選択処理を行います。*/

//文書データの取得・設定
FOOTYEXPORT int __stdcall FootySetText(int id,const _TCHAR *pszText);			/*Footyのテキスト全体を設定します。*/
FOOTYEXPORT int __stdcall FootyGetTextLength(int id);							/*現在編集中のテキストのバイト数を得ます。*/
FOOTYEXPORT int __stdcall FootyGetText(int id,_TCHAR *pOut,int ret=RETLINE_AUTO);/*現在編集中のテキストを取得します。*/
FOOTYEXPORT int __stdcall FootyGetLines(int id);								/*現在の総行数を取得します。*/
FOOTYEXPORT int __stdcall FootySetSelText(int id,const _TCHAR *pszText);		/*選択文字列の設定*/
FOOTYEXPORT int __stdcall FootyGetSelText(int id,_TCHAR *pOut);					/*選択文字列の取得*/
FOOTYEXPORT const _TCHAR* __stdcall FootyGetLineData(int id,int line);			/*指定した行の内容を取得*/
FOOTYEXPORT int __stdcall FootyGetLineLen(int id,int line);						/*指定した行の長さを取得*/
FOOTYEXPORT int __stdcall FootyGetLineCode(int id);								/*改行コードの種類を取得*/

//選択関連
FOOTYEXPORT int __stdcall FootySetSelA(int id,int sLine,int sPos,
									   int eLine,int ePos);						/*選択状態を設定します。*/
FOOTYEXPORT int __stdcall FootyGetSelA(int id,int *psLine,int *psPos,
									   int *peLine,int *pePos);					/*選択状態の取得*/
FOOTYEXPORT int __stdcall FootySetSelB(int id,int sPos,int ePos);				/*選択状態を設定します。*/
FOOTYEXPORT int __stdcall FootyGetSelB(int id,int *pStart,int *pEnd);			/*選択状態の取得*/

//カーソル関連
FOOTYEXPORT int __stdcall FootyGetCaretLine(int id);							/*キャレット行を取得します。*/
FOOTYEXPORT int __stdcall FootyGetCaretPos(int id);								/*キャレット桁を取得します。(Tabも一文字と換算)*/
FOOTYEXPORT int __stdcall FootySetCaretLine(int id,int line);					/*キャレット行を設定します。*/
FOOTYEXPORT int __stdcall FootySetCaretPos(int id,int pos);						/*キャレット桁を設定します。*/
FOOTYEXPORT int __stdcall FootyGetCaretThrough(int id);							/*キャレット位置を通し番号で取得します。*/
FOOTYEXPORT int __stdcall FootySetCaretThrough(int id,int pos);					/*キャレット位置を等し番号で設定します。*/
FOOTYEXPORT int __stdcall FootyGetCaretVPos(int id);							/*キャレット桁を実際の位置で取得します。*/

//マーク・強調
FOOTYEXPORT int __stdcall FootySetMark(int id,int line,int mark,
									   bool bRedraw=true);						/*マークをセットします。*/
FOOTYEXPORT int __stdcall FootyGetMark(int id,int line);						/*マークを取得します。*/
FOOTYEXPORT int __stdcall FootySetHighlight(int id,const _TCHAR *szString,
											bool bBS,bool bRedraw=true);		/*ハイライト文字を設定します。*/

FOOTYEXPORT int __stdcall FootyAddEmphasis(int id,const _TCHAR *szWord1,const _TCHAR *szWord2,int nType,
										   COLORREF color,int nStatus,int nLevel,
										   int nPLevel=0);						/*強調文字を設定します。*/
FOOTYEXPORT int __stdcall FootyResetEmphasis(int id,bool bRedraw=true);			/*強調表示を全て解除*/


//設定関連
FOOTYEXPORT int __stdcall FootyFontDialog(int id,_TCHAR *szFontName=NULL,
										  bool bRedraw=true);					/*フォントダイアログを表示し、その内容に表示を変更します。*/
FOOTYEXPORT int __stdcall FootySetFont(int id,const _TCHAR *szFontName,
									   int point,bool bRedraw=true);			/*フォントを設定します。*/
FOOTYEXPORT int __stdcall FootySetColor(int id,int obj,COLORREF color,
										bool bRedraw=true);						/*色を変更します（マーク用定数および色指定定数を使用可能）*/
FOOTYEXPORT int __stdcall FootySetMarkVisible(int id,int obj,bool bVisible,
										      bool bRedraw=true);				/*マークの表示設定をします。*/
FOOTYEXPORT int __stdcall FootySetMetrics(int id,int object,
										  int num,bool bRedraw=true);			/*数値を設定します。*/
FOOTYEXPORT int __stdcall FootyGetMetrics(int id,int object);					/*数値を取得します。*/
FOOTYEXPORT int __stdcall FootyWriteEnable(int id,bool bEnable);				/*書き換えが可能であるかどうか設定します。*/
FOOTYEXPORT int __stdcall FootySetLapel(int id,bool bLapel,
										int JPNhyphenation,bool bRedraw=true);	/*折り返し処理です。*/


//関数ポインタ登録
FOOTYEXPORT int __stdcall FootyEventCaretMove(int id,pFuncCaretMove pFunc,void *pParam);
FOOTYEXPORT int __stdcall FootyEventSelChange(int id,pFuncSelChange pFunc,void *pParam);
FOOTYEXPORT int __stdcall FootyEventInsChange(int id,pFuncInsChange pFunc,void *pParam);
FOOTYEXPORT int __stdcall FootyEventChange(int id,pFuncChange pFunc,void *pParam);
FOOTYEXPORT int __stdcall FootyEventContextMenu(int id,pFuncContext pFunc,void *pParam);
FOOTYEXPORT int __stdcall FootyEventMouseMove(int id,pFuncMouseMove pFunc,void *pParam);


#endif

/*
 * FootyEditorControl for Win32 JapaneseEdition
 * Copyright(C)2002-2004 ShinjiWatanabe
 */