/*=============================================================================
Footy2.h
Footyテキストエディタコントロール２

Footyの関数を使用するための定義を追加するヘッダファイルです。
# Windows Mobile用にコンパイルするときはUNDER_CEをプリプロセス宣言します。

(C)2004-2009 ShinjiWatanabe
=============================================================================*/

#ifndef _FOOTY2_DLL_H_
#define _FOOTY2_DLL_H_

//必要なファイルのインクルード、エクスポート宣言///////////////////////////////
#include <windows.h>

#undef FOOTYEXPORT
#define FOOTYEXPORT(x)	extern "C" __declspec(dllexport) x __stdcall

//Unicode用の宣言//////////////////////////////////////////////////////////////
#ifdef _UNICODE
#	define Footy2AddEmphasis	Footy2AddEmphasisW
#	define Footy2SetText		Footy2SetTextW
#	define Footy2SetSelText		Footy2SetSelTextW
#	define Footy2GetTextLength	Footy2GetTextLengthW
#	define Footy2GetLine		Footy2GetLineW
#	define Footy2SetLine		Footy2SetLineW
#	define Footy2GetLineLength	Footy2GetLineLengthW
#	define Footy2TextFromFile	Footy2TextFromFileW
#	define Footy2SaveToFile		Footy2SaveToFileW
#	define Footy2SetFontFace	Footy2SetFontFaceW
#	define Footy2Search			Footy2SearchW
#	define Footy2GetText		Footy2GetTextW
#	define Footy2GetSelText		Footy2GetSelTextW
#	define Footy2GetSelLength	Footy2GetSelLengthW
#	define Footy2SetBackgroundImage	Footy2SetBackgroundImageW
#else	/*! _UNICODE*/
#	define Footy2AddEmphasis	Footy2AddEmphasisA
#	define Footy2SetText		Footy2SetTextA
#	define Footy2SetSelText		Footy2SetSelTextA
#	define Footy2GetTextLength	Footy2GetTextLengthA
#	define Footy2GetLine		Footy2GetLineA
#	define Footy2SetLine		Footy2SetLineA
#	define Footy2GetLineLength	Footy2GetLineLengthA
#	define Footy2TextFromFile	Footy2TextFromFileA
#	define Footy2SaveToFile		Footy2SaveToFileA
#	define Footy2SetFontFace	Footy2SetFontFaceA
#	define Footy2Search			Footy2SearchA
#	define Footy2GetText		Footy2GetTextA
#	define Footy2GetSelText		Footy2GetSelTextA
#	define Footy2GetSelLength	Footy2GetSelLengthA
#	define Footy2SetBackgroundImage	Footy2SetBackgroundImageA
#endif	/*_UNICODE*/

//関数ポインタとコールバック設定///////////////////////////////////////////////
typedef void (__stdcall *Footy2FuncFocus)(int nID,void* pData,int nViewID,bool bFocused);
typedef void (__stdcall *Footy2FuncMoveCaret)(int nID,void* pData,size_t nCaretLine,size_t nCaretColumn);
typedef void (__stdcall *Footy2FuncTextModified)(int nID,void* pData,int nCause);
typedef void (__stdcall *Footy2FuncInsertModeChanged)(int nID,void* pData,bool bInsertMode);
FOOTYEXPORT(int) Footy2SetFuncFocus(int nID,Footy2FuncFocus pFunc,void *pData);
FOOTYEXPORT(int) Footy2SetFuncMoveCaret(int nID,Footy2FuncMoveCaret pFunc,void *pData);
FOOTYEXPORT(int) Footy2SetFuncTextModified(int nID,Footy2FuncTextModified pFunc,void *pData);
FOOTYEXPORT(int) Footy2SetFuncInsertModeChanged(int nID,Footy2FuncInsertModeChanged pFunc,void *pData);

//便利なマクロ/////////////////////////////////////////////////////////////////
#define PERMIT_LEVEL(x)		(1 << (x))

//定数の宣言///////////////////////////////////////////////////////////////////

enum UrlType									//!< URLタイプ
{
	URLTYPE_HTTP,								//!< http
	URLTYPE_HTTPS,								//!< https
	URLTYPE_FTP,								//!< ftp
	URLTYPE_MAIL,								//!< メールアドレス
	URLTYPE_LABEL,								//!< ラベル by Tetr@pod
	URLTYPE_FUNC,								//!< 命令・関数 by Tetr@pod
};

enum ViewMode
{
	VIEWMODE_NORMAL,							//!< シングルモード
	VIEWMODE_VERTICAL,							//!< 縦分割
	VIEWMODE_HORIZONTAL,						//!< 横分割
	VIEWMODE_QUAD,								//!< 四分割
	VIEWMODE_INVISIBLE,							//!< 非表示
};

enum EmpMode
{
	EMP_INVALIDITY,								//!< 無効
	EMP_WORD,									//!< 単語を強調
	EMP_LINE_AFTER,								//!< それ以降～行末
	EMP_LINE_BETWEEN,							//!< 二つの文字の間（同一行に限る・二つの文字列を指定）
	EMP_MULTI_BETWEEN,							//!< 二つの文字の間（複数行にわたる・二つの文字列を指定）
};

enum ColorPos
{
	CP_TEXT,									//!< 通常の文字
	CP_BACKGROUND,								//!< 背景色
	CP_CRLF,									//!< 改行マーク
	CP_HALFSPACE,								//!< 半角スペース
	CP_NORMALSPACE,								//!< 全角スペース
	CP_TAB,										//!< タブ文字
	CP_EOF,										//!< [EOF]
	CP_UNDERLINE,								//!< キャレットの下のアンダーライン
	CP_LINENUMBORDER,							//!< 行番号とテキストエディタの境界線
	CP_LINENUMTEXT,								//!< 行番号テキスト
	CP_CARETLINE,								//!< 行番号領域におけるキャレット位置強調
	CP_RULERBACKGROUND,							//!< ルーラーの背景色
	CP_RULERTEXT,								//!< ルーラーのテキスト
	CP_RULERLINE,								//!< ルーラー上の線
	CP_CARETPOS,								//!< ルーラーにおけるキャレット位置強調
	CP_URLTEXT,									//!< URL文字
	CP_URLUNDERLINE,							//!< URL下に表示されるアンダーライン
	CP_MAILTEXT,								//!< メールアドレス文字
	CP_MAILUNDERLINE,							//!< メール下に表示されるアンダーライン
	CP_LABELTEXT,								//!< ラベル文字 by Tetr@pod
	CP_LABELUNDERLINE,							//!< ラベル下に表示されるアンダーライン by Tetr@pod

	CP_FUNCTEXT1,								//!< 命令・関数文字1 by Tetr@pod
	CP_FUNCTEXT2,								//!< 命令・関数文字2 by Tetr@pod
	CP_FUNCTEXT3,								//!< 命令・関数文字3 by Tetr@pod
	CP_FUNCTEXT4,								//!< 命令・関数文字4 by Tetr@pod
	CP_FUNCTEXT5,								//!< 命令・関数文字5 by Tetr@pod
	CP_FUNCTEXT6,								//!< 命令・関数文字6 by Tetr@pod
	CP_FUNCTEXT7,								//!< 命令・関数文字7 by Tetr@pod
	CP_FUNCTEXT8,								//!< 命令・関数文字8 by Tetr@pod

	CP_HIGHLIGHTTEXT,							//!< ハイライトテキスト
	CP_HIGHLIGHTBACKGROUND,						//!< ハイライト背景色
};

//! 改行コード
enum LineMode
{
	LM_AUTOMATIC,								//!< 自動
	LM_CRLF,									//!< CrLfコード
	LM_CR,										//!< Crコード
	LM_LF,										//!< Lfコード
	LM_ERROR,									//!< エラー識別用：戻り値専用です
};

//! キャラクタセット
enum CharSetMode
{
	CSM_AUTOMATIC,								//!< 自動モード(通常はこれを使用する)
#ifndef UNDER_CE
	CSM_PLATFORM,								//!< プラットフォーム依存
#endif	/*not defined UNDER_CE*/

	// 日本語
	CSM_SHIFT_JIS_2004,							//!< ShiftJISのJIS X 0213:2004拡張(WindowsVista標準)
	CSM_EUC_JIS_2004,							//!< EUC-JPのJIS X 0213:2004拡張
	CSM_ISO_2022_JP_2004,						//!< JISコードのJIS X 0213:2004拡張

	// ISO 8859
	CSM_ISO8859_1,								//!< 西ヨーロッパ(Latin1)
	CSM_ISO8859_2,								//!< 東ヨーロッパ(Latin2)
	CSM_ISO8859_3,								//!< エスペラント語他(Latin3)
	CSM_ISO8859_4,								//!< 北ヨーロッパ(Latin4)
	CSM_ISO8859_5,								//!< キリル
	CSM_ISO8859_6,								//!< アラビア
	CSM_ISO8859_7,								//!< ギリシャ
	CSM_ISO8859_8,								//!< ヘブライ
	CSM_ISO8859_9,								//!< トルコ(Latin5)
	CSM_ISO8859_10,								//!< 北欧(Latin6)
	CSM_ISO8859_11,								//!< タイ
	/*ISO8859-12は1997年に破棄されました*/
	CSM_ISO8859_13,								//!< バルト諸国の言語
	CSM_ISO8859_14,								//!< ケルト語
	CSM_ISO8859_15,								//!< ISO 8859-1の変形版
	CSM_ISO8859_16,								//!< 東南ヨーロッパ

	// Unicode
	CSM_UTF8,									//!< BOM無しUTF8
	CSM_UTF8_BOM,								//!< BOM付きUTF8
	CSM_UTF16_LE,								//!< BOM無しUTF16リトルエンディアン
	CSM_UTF16_LE_BOM,							//!< BOM付きUTF16リトルエンディアン
	CSM_UTF16_BE,								//!< BOM無しUTF16ビッグエンディアン
	CSM_UTF16_BE_BOM,							//!< BOM付きUTF16ビッグエンディアン
	CSM_UTF32_LE,								//!< BOM無しUTF32リトルエンディアン
	CSM_UTF32_LE_BOM,							//!< BOM付きUTF32リトルエンディアン
	CSM_UTF32_BE,								//!< BOM無しUTF32ビッグエンディアン
	CSM_UTF32_BE_BOM,							//!< BOM付きUTF32ビッグエンディアン
	
	// 内部処理用(使用しないでください)
	CSM_ERROR,									//!< エラー
};

//! フォント
enum FontMode
{
	FFM_ANSI_CHARSET,							//!< ANSI文字
	FFM_BALTIC_CHARSET,							//!< バルト文字
	FFM_BIG5_CHARSET,							//!< 繁体字中国語文字
	FFM_EASTEUROPE_CHARSET,						//!< 東ヨーロッパ文字
	FFM_GB2312_CHARSET,							//!< 簡体中国語文字
	FFM_GREEK_CHARSET,							//!< ギリシャ文字
	FFM_HANGUL_CHARSET,							//!< ハングル文字
	FFM_RUSSIAN_CHARSET,						//!< キリル文字
	FFM_SHIFTJIS_CHARSET,						//!< 日本語
	FFM_TURKISH_CHARSET,						//!< トルコ語
	FFM_VIETNAMESE_CHARSET,						//!< ベトナム語
	FFM_ARABIC_CHARSET,							//!< アラビア語
	FFM_HEBREW_CHARSET,							//!< ヘブライ語
	FFM_THAI_CHARSET,							//!< タイ語
	FFM_DEFAULT_CHARSET,						//!< デフォルト(追加した) by inovia							
	//!< タイ語
	// 内部処理用(使用しないでください)
	FFM_NUM_FONTS								//!< フォントの数
};

//! 行アイコン
enum LineIcons
{
	LINEICON_NONE				=	0x00000000,

	LINEICON_ATTACH				=	0x00000001,
	LINEICON_BACK				=	0x00000002,
	LINEICON_CHECKED			=	0x00000004,
	LINEICON_UNCHECKED			=	0x00000008,
	LINEICON_CANCEL				=	0x00000010,
	LINEICON_CLOCK				=	0x00000020,
	LINEICON_CONTENTS			=	0x00000040,
	LINEICON_DB_CANCEL			=	0x00000080,
	LINEICON_DB_DELETE			=	0x00000100,
	LINEICON_DB_FIRST			=	0x00000200,
	LINEICON_DB_EDIT			=	0x00000400,
	LINEICON_DB_INSERT			=	0x00000800,
	LINEICON_DB_LAST			=	0x00001000,
	LINEICON_DB_NEXT			=	0x00002000,
	LINEICON_DB_POST			=	0x00004000,
	LINEICON_DB_PREVIOUS		=	0x00008000,
	LINEICON_DB_REFRESH			=	0x00010000,
	LINEICON_DELETE				=	0x00020000,
	LINEICON_EXECUTE			=	0x00040000,
	LINEICON_FAVORITE			=	0x00080000,
	LINEICON_BLUE				=	0x00100000,
	LINEICON_GREEN				=	0x00200000,
	LINEICON_RED				=	0x00400000,
	LINEICON_FORWARD			=	0x00800000,
	LINEICON_HELP				=	0x01000000,
	LINEICON_INFORMATION		=	0x02000000,
	LINEICON_KEY				=	0x04000000,
	LINEICON_LOCK				=	0x08000000,
	LINEICON_RECORD				=	0x10000000,
	LINEICON_TICK				=	0x20000000,
	LINEICON_TIPS				=	0x40000000,
	LINEICON_WARNING			=	0x80000000,
};


//! 強調表示モード
enum EmpType
{
	EMPFLAG_BOLD				=	0x00000001,	//!< 太字にする
	EMPFLAG_NON_CS				=	0x00000002,	//!< 大文字と小文字を区別しない
	EMPFLAG_HEAD				=	0x00000004,	//!< 頭にあるときのみ
};

//! エディタマーク表示、非表示の設定
enum EditorMarks
{
	EDM_HALF_SPACE				=	0x00000001,	//!< 半角スペースを表示するか
	EDM_FULL_SPACE				=	0x00000002,	//!< 全角スペースを表示するか
	EDM_TAB						=	0x00000004,	//!< タブマークを表示するか
	EDM_LINE					=	0x00000008,	//!< 改行マークを表示するか
	EDM_EOF						=	0x00000010,	//!< [EOF]マークを表示するか
	
	EDM_SHOW_ALL				=	0xFFFFFFFF,	//!< 全て表示する
	EDM_SHOW_NONE				=	0x00000000,	//!< 何も表示しない
};


//! 検索フラグ
enum SearchFlags
{
	SEARCH_FROMCURSOR			=	0x00000001,	//!< 現在のカーソル位置から検索する
	SEARCH_BACK					=	0x00000002,	//!< 後ろ方向に検索処理を実行する
	SEARCH_REGEXP				=	0x00000004,	//!< 正規表現を利用する(未実装)
	SEARCH_NOT_REFRESH			=	0x00000008,	//!< 検索結果を再描画しない
	SEARCH_BEEP_ON_404			=	0x00000010,	//!< 見つからなかったときにビープ音をならす
	SEARCH_NOT_ADJUST_VIEW		=	0x00000020,	//!< 見つかったときにキャレット位置へスクロールバーを追随させない
	SEARCH_HIGHLIGHT			=	0x00000040,	//!< 見つかった文字列をハイライト表示します(未実装)
	SEARCH_IGNORECASE			=	0x00000040,	//!< 大文字小文字を区別しない
};

//! 独立レベル
enum IndependentFlags
{
	// ASCII記号用フラグ
	EMP_IND_PARENTHESIS			=	0x00000001,	//!< 前後に丸括弧()があることを許可する
	EMP_IND_BRACE				=	0x00000002,	//!< 前後に中括弧{}があることを許可する
	EMP_IND_ANGLE_BRACKET		=	0x00000004,	//!< 前後に山形括弧<>があることを許可する
	EMP_IND_SQUARE_BRACKET		=	0x00000008,	//!< 前後に各括弧[]があることを許可する
	EMP_IND_QUOTATION			=	0x00000010,	//!< 前後にコーテーション'"があることを許可する
	EMP_IND_UNDERBAR			=	0x00000020,	//!< 前後にアンダーバー_があることを許可する
	EMP_IND_OPERATORS			=	0x00000040,	//!< 前後に演算子 + - * / % ^ = があることを許可する
	EMP_IND_OTHER_ASCII_SIGN	=	0x00000080,	//!< 前述のものを除く全てのASCII記号 # ! $ & | \ @ ? , . : ; ~
	
	// ASCII文字列を指定するフラグ
	EMP_IND_NUMBER				=	0x00000100,	//!< 前後に数字を許可する
	EMP_IND_CAPITAL_ALPHABET	=	0x00000200,	//!< 前後に大文字アルファベットを許可する
	EMP_IND_SMALL_ALPHABET		=	0x00000400,	//!< 前後に小文字アルファベットを許可する
	
	// 空白を指定するフラグ
	EMP_IND_SPACE				=	0x00001000,	//!< 前後に半角スペースを許可する
	EMP_IND_FULL_SPACE			=	0x00002000,	//!< 前後に全角スペースを許可する
	EMP_IND_TAB					=	0x00004000,	//!< 前後にタブを許可する

	// そのほかの文字列
	EMP_IND_JAPANESE			=	0x00010000,	//!< 日本語
	EMP_IND_KOREAN				=	0x00020000,	//!< 韓国語
	EMP_IND_EASTUROPE			=	0x00040000,	//!< 東ヨーロッパ

	EMP_IND_OTHERS				=	0x80000000,	//!< 上記以外

	// 省略形(主にこれらを使用すると指定が楽です)
	EMP_IND_ASCII_SIGN			=	0x000000FF,	//!< 全てのASCII記号列を許可する
	EMP_IND_ASCII_LETTER		=	0x00000F00,	//!< 全てのASCII文字を許可する(数字とアルファベット)
	EMP_IND_BLANKS				=	0x0000F000,	//!< 全ての空白文字列を許可する
	EMP_IND_OTHER_CHARSETS		=	0xFFFF0000,	//!< 全てのキャラクタセットを許可する
	EMP_IND_ALLOW_ALL			=	0xFFFFFFFF	//!< 何でもOK
};

//! 折り返しモード
enum LapelMode
{
	LAPELFLAG_ALL				=	0xFFFFFFFF,	//!< 以下のフラグ全てを選択する
	LAPELFLAG_NONE				=	0x00000000,	//!< 何も有効にしない
	LAPELFLAG_WORDBREAK			=	0x00000001,	//!< 英文ワードラップ
	LAPELFLAG_JPN_PERIOD		=	0x00000002,	//!< 日本語の句読点に関する禁則処理
	LAPELFLAG_JPN_QUOTATION		=	0x00000004,	//!< 日本語のカギ括弧に関する禁則処理
};

//! 数値取得
enum SetMetricsCode
{
	SM_LAPEL_COLUMN,							//!< 折り返し位置(桁数)
	SM_LAPEL_MODE,								//!< 折り返しモード
	SM_MARK_VISIBLE,							//!< マークの表示状態
	SM_LINENUM_WIDTH,							//!< 左端の行番号の幅(ピクセル、-1でデフォルト)
	SM_RULER_HEIGHT,							//!< 上のルーラーの高さ(ピクセル、-1でデフォルト)
	SM_UNDERLINE_VISIBLE,						//!< 行下線の表示状態
	SM_TAB_WIDTH,								//!< タブの横幅
	SM_REDOREM,									//!< REDO可能な回数(取得のみ)
	SM_UNDOREM,									//!< UNDO可能な回数(取得のみ)
	SM_MARGIN_HEIGHT,							//!< 行マージン(onitama追加)
};

//! テキストが編集された原因
enum TextModifiedCause
{
	MODIFIED_CAUSE_CHAR,						//!< 文字が入力された(IMEオフ)
	MODIFIED_CAUSE_IME,							//!< 文字が入力された(IMEオン)
	MODIFIED_CAUSE_DELETE,						//!< Deleteキーが押下された
	MODIFIED_CAUSE_BACKSPACE,					//!< BackSpaceが押下された
	MODIFIED_CAUSE_ENTER,						//!< Enterキーが押下された
	MODIFIED_CAUSE_UNDO,						//!< 元に戻す処理が実行された
	MODIFIED_CAUSE_REDO,						//!< やり直し処理が実行された
	MODIFIED_CAUSE_CUT,							//!< 切り取り処理が実行された
	MODIFIED_CAUSE_PASTE,						//!< 貼り付け処理が実行された
	MODIFIED_CAUSE_INDENT,						//!< インデントされた
	MODIFIED_CAUSE_UNINDENT,					//!< 逆インデントされた
	MODIFIED_CAUSE_TAB,							//!< タブキーが押されて、タブ文字が挿入された
	MODIFIED_CAUSE_SETSELTEXT,					//!< Footy2SetSelText()が実行された
};

//! エラーコード
enum ErrCode
{
	FOOTY2ERR_NONE				=	0,			//!< 関数が成功した
	FOOTY2ERR_ARGUMENT			=	-1,			//!< 引数おかしい
	FOOTY2ERR_NOID				=	-2,			//!< IDが見つからない
	FOOTY2ERR_MEMORY			=	-3,			//!< メモリー不足
	FOOTY2ERR_NOUNDO			=	-4,			//!< アンドゥ情報がこれ以前に見つからない
	FOOTY2ERR_NOTSELECTED		=	-5,			//!< 選択されていない
	FOOTY2ERR_UNKNOWN			=	-6,			//!< 不明なエラー
	FOOTY2ERR_NOTYET			=	-7,			//!< 未実装(ごめんなさい)
	FOOTY2ERR_404				=	-8,			//!< ファイルが見つからない、検索文字列が見つからない
	FOOTY2ERR_NOACTIVE			=	-9,			//!< アクティブなビューは存在しません
	FOOTY2ERR_ENCODER			=	-10,		//!< 文字コードのエンコーダが見つかりません(ファイルのエンコード形式が不正です、バイナリとか)
};


//関数の宣言///////////////////////////////////////////////////////////////////

/*開始・終了関数(DLLを使用しているときは呼び出す必要はありません！)*/
#ifdef FOOTYSTATIC
	FOOTYEXPORT(void) Footy2Start(HINSTANCE hInstance);
	FOOTYEXPORT(void) Footy2End();
#endif	/*FOOTYSTATIC*/

//バージョン関連
FOOTYEXPORT(int) GetFooty2Ver();								//!< Footyのバージョンを取得します。
FOOTYEXPORT(int) GetFooty2BetaVer();							//!< Footyのβバージョンを取得します。

//ウィンドウ関連
FOOTYEXPORT(int) Footy2Create(HWND hWnd,int x,int y,
							  int dx,int dy,int nMode);			//!< Footyを作成します。
FOOTYEXPORT(int) Footy2ChangeParent(int nID, HWND hWnd,
							  int x,int y, int dx,int dy);		//!< 親ウィンドウを変更します
FOOTYEXPORT(int) Footy2Delete(int nID);							//!< 確保されたIDのデータを消去してメモリ領域を確保します。
FOOTYEXPORT(int) Footy2Move(int nID,int x,int y,int dx,int dy);	//!< Footyウィンドウを移動します。
FOOTYEXPORT(int) Footy2ChangeView(int nID,int nView);			//!< ビューモードの変更
FOOTYEXPORT(int) Footy2SetFocus(int nID,int nViewID);			//!< フォーカス設定
FOOTYEXPORT(HWND) Footy2GetWnd(int nID,int nViewID);			//!< Footyのウィンドウハンドルを取得します
FOOTYEXPORT(HWND) Footy2GetWndVSplit(int nID);					//!< 縦分割用バーのウィンドウハンドルを取得
FOOTYEXPORT(HWND) Footy2GetWndHSplit(int nID);					//!< 横分割用バーのウィンドウハンドルを取得
FOOTYEXPORT(int) Footy2GetActiveView(int nID);					//!< アクティブなビューを取得
FOOTYEXPORT(int) Footy2Refresh(int nID);						//!< 再描画処理

//編集系
FOOTYEXPORT(int) Footy2Copy(int nID);							//!< クリップボードへコピー
FOOTYEXPORT(int) Footy2Cut(int nID);							//!< クリップボードへカット
FOOTYEXPORT(int) Footy2Paste(int nID);							//!< クリップボードからペースト
FOOTYEXPORT(int) Footy2Undo(int nID);							//!< アンドゥ
FOOTYEXPORT(int) Footy2Redo(int nID);							//!< リドゥ
FOOTYEXPORT(bool) Footy2IsEdited(int nID);						//!< 選択されているかどうかを取得します
FOOTYEXPORT(int) Footy2SelectAll(int nID,bool bRefresh = true);	//!< 全て選択
FOOTYEXPORT(int) Footy2ShiftLock(int nID,bool bLocked);			//!< シフトロックを行うかどうか
FOOTYEXPORT(bool) Footy2IsShiftLocked(int nID);					//!< シフトロックが行われているかどうか取得する
FOOTYEXPORT(int) Footy2SetReadOnly( int nID, bool bReadOnly );	//!< 編集禁止状態にします
FOOTYEXPORT(bool) Footy2IsReadOnly( int nID );					//!< 編集禁止状態なのかどうかを取得します

//検索系
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SearchA(int nID,const char *szText,int nFlags);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SearchW(int nID,const wchar_t* szText,int nFlags);

// ファイル編集
FOOTYEXPORT(int) Footy2CreateNew(int nID);
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2TextFromFileA(int nID,const char* szPath,int nCharSets);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2TextFromFileW(int nID,const wchar_t* szPath,int nCharSets);
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SaveToFileA(int nID,const char* szPath,int nCharSets,int nLineMode);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SaveToFileW(int nID,const wchar_t* szPath,int nCharSets,int nLineMode);
FOOTYEXPORT(int) Footy2GetCharSet(int nID);
FOOTYEXPORT(int) Footy2GetLineCode(int nID);

//文字列処理
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SetSelTextA(int nID,const char *pString);//!< 選択文字列をセット
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SetSelTextW(int nID,const wchar_t* pString);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetSelTextA(int nID, char *pString, int nLineMode, int nSize);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2GetSelTextW(int nID, wchar_t* pString, int nLineMode, int nSize);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SetTextA(int nID,const char *pString);	//!< 文字列セット
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SetTextW(int nID,const wchar_t* pString);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetTextLengthA(int nID,int nLineMode);	//!< 文字列長さ取得
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2GetTextLengthW(int nID,int nLineMode);


#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetSelLengthA(int nID, int nLineMode);	//!< 選択文字列長さを取得
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2GetSelLengthW(int nID, int nLineMode);		//!< 選択文字列長さを取得


#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetTextA(int nID,char *pString,int nLineMode,int nSize);/*文字列取得*/
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2GetTextW(int nID,wchar_t* pString,int nLineMode,int nSize);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetLineA(int nID,char *pString,size_t nLine,int nSize);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(const wchar_t*) Footy2GetLineW(int nID,size_t nLine);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SetLineA(int nID,size_t nLine,const char *pString,bool bRedraw);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SetLineW(int nID,size_t nLine,const wchar_t* pString,bool bRedraw);

#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2GetLineLengthA(int nID,size_t nLine);	/*行ごとの長さ取得*/
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2GetLineLengthW(int nID,size_t nLine);

FOOTYEXPORT(int) Footy2GetLines(int nID);

//キャレット・選択系
FOOTYEXPORT(int) Footy2GetCaretPosition(int nID,size_t *pCaretLine,size_t *pCaretPos);
FOOTYEXPORT(int) Footy2SetCaretPosition(int nID,size_t nCaretLine,size_t nCaretPos,bool bRefresh = true);
FOOTYEXPORT(int) Footy2GetSel(int nID,size_t *pStartLine,size_t *pStartPos,size_t *pEndLine,size_t *pEndPos);
FOOTYEXPORT(int) Footy2SetSel(int nID,size_t nStartLine,size_t nStartPos,size_t nEndLine,size_t nEndPos,bool bRefresh = true);

//表示
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2AddEmphasisA(int nID,const char *pString1,const char *pString2,
		int nType,int nFlags,int nLevel,int nPermission,int nIndependence,COLORREF nColor);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2AddEmphasisW(int nID,const wchar_t* pString1,const wchar_t* pString2,
	int nType,int nFlags,int nLevel,int nPermission,int nIndependence,COLORREF nColor);
FOOTYEXPORT(int) Footy2FlushEmphasis(int nID);
FOOTYEXPORT(int) Footy2ClearEmphasis(int nID);
FOOTYEXPORT(int) Footy2SetFontSize(int nID,int nPoint,bool bRefresh = true);
#ifndef UNDER_CE
	FOOTYEXPORT(int) Footy2SetFontFaceA(int nID,int nFontMode,const char *pString,bool bRefresh = true);
#endif	/*not defined UNDER_CE*/
FOOTYEXPORT(int) Footy2SetFontFaceW(int nID,int nFontMode,const wchar_t* pString,bool bRefresh = true);
FOOTYEXPORT(int) Footy2SetLineIcon(int nID,size_t nLine,int nIcons,bool bRefresh = true);
FOOTYEXPORT(int) Footy2GetLineIcon(int nID,size_t nLine,int *pIcons);
FOOTYEXPORT(int) Footy2SetForceFont(int nID, int flag);	// フォントの強制使用 by inovia
FOOTYEXPORT(int) Footy2SetSpeedDraw(int nID, int flag);	// 高速描画をONにする by inovia
//設定関連
FOOTYEXPORT(int) Footy2SetLapel(int nID,int nColumns,int nMode,bool bRefresh = true);
FOOTYEXPORT(int) Footy2SetMetrics(int nID,int nObject,int nValue,bool bRefresh = true);
FOOTYEXPORT(int) Footy2GetMetrics(int nID,int nObject,int *pValue);
FOOTYEXPORT(int) Footy2SetColor(int nID,int nPosition,COLORREF nColor,bool bRefresh = true);
FOOTYEXPORT(int) Footy2GetVisibleColumns(int nID,int nViewID);	//!< 現在ビューで表示している桁数を取得
FOOTYEXPORT(int) Footy2GetVisibleLines(int nID,int nViewID);	//!< 現在ビューで表示している行数を取得

FOOTYEXPORT(int) Footy2SetUnderlineDraw(int nID, bool bFlag);	// フォーカスが外れたときもアンダーバーを描画するかどうか？

FOOTYEXPORT(int) Footy2SetBackgroundImageA(int nID, const char *pPath, bool bRefresh);
FOOTYEXPORT(int) Footy2SetBackgroundImageW(int nID, const wchar_t *pPath, bool bRefresh);
FOOTYEXPORT(int) Footy2ClearBackgroundImage(int nID, bool bRefresh);
FOOTYEXPORT(int) Footy2SetBackgroundImageColor(int nID, COLORREF color);
#endif	/*_FOOTY2_DLL_H_*/

/*[EOF]*/
