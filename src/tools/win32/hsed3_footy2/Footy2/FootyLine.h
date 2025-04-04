/**
 * @brief FootyLine.h
 * @brief Footyの行数を一行単位で管理するクラスです。
 * @author Shinji Watanabe
 * @date Nov.09.2008
 */

#pragma once

#include "StaticVector.h"
#include "EmphasisWord.h"

extern BOOL CustomMatchCase;// 大文字小文字の区別 by Tetr@pod
extern BOOL UseClickableLabel;// リンクラベルを使用する by Tetr@pod
extern BOOL UseSearchFunction;// ユーザー定義命令・関数を色分けする by Tetr@pod

extern BOOL ChangeColor_func;// #func、#cfunc、#cmd も色分けする
extern BOOL ChangeColor_define;// #define、#define ctype も色分けする


extern wchar_t *funclist;// 命令・関数のリスト by Tetr@pod
extern int *kindlist;// その種類 by Tetr@pod
extern int funcnum;// その数 by Tetr@pod
extern bool defined_funclist;// と、それが確保されたか by Tetr@podc
extern bool DrawAllUserFuncs;// すべての行を描画する？ by Tetr@pod
extern bool IsUFUpdateTiming;// 更新すべき時か？ by Tetr@pod

// URLごとの情報
class CUrlInfo
{
public:
	CUrlInfo()
	{
		m_nStartPos = 0;
		m_nEndPos = 0;
		m_nKind = 0;// 【ユーザー定義命令・関数用】 命令・関数の種類 by Tetr@pod
	}
	
	// ポジションが含まれるかチェックする
	inline bool IsInPosition(size_t nPosition)
	{
		return m_nStartPos <= nPosition &&
			nPosition < m_nEndPos;
	}
	inline size_t GetLength() const
	{
		return m_nEndPos - m_nStartPos;
	}


public:
	size_t m_nStartPos;					//!< URLの開始位置0ベース
	size_t m_nEndPos;					//!< URLの終了位置0ベース
	int m_nKind;						//!< 【ユーザー定義命令・関数用】 命令・関数の種類 by Tetr@pod
};

typedef std::vector<CUrlInfo>::iterator UrlIterator;

class CFootyLine
{
public:
	//! GUI部分に送るコマンド構造体
	struct EmpPos
	{
		size_t m_nPosition;				//!< コマンドの位置
		bool m_bIsStart;				//!< trueのとき、ここが色を変えるポイント
		COLORREF m_Color;				//!< 設定する色
	};
	
	//! 論理計測ルーチンの戻り
	struct EthicInfo
	{
		size_t m_nEthicLine;			//!< 論理行
		size_t m_nEthicColumn;			//!< 論理桁位置
	};
	
	//! 単語の情報
	struct WordInfo
	{
		size_t m_nBeginPos;				//!< 開始位置(0ベース)
		size_t m_nEndPos;				//!< 終了位置(0ベース)
	};
	
	// コンストラクタ
	CFootyLine();
	
	// 文字列操作終了後に呼び出す
	typedef CStaticVector<WordPt,sizeof(int)*8> TakeOver;
	bool SearchEmphasis(TakeOver *pBeforeBetween,LsWords *plsWords);
	bool FlushString(int m_nFootyID, size_t nTabLen,size_t nColumn,int nMode, bool *NeedRedrawAll/*全行再描画する必要があるか*/, bool ForceListUpdate/*改行を含むか by Tetr@pod*/);
	bool SetPrevLineInfo(std::list<CFootyLine>::iterator pPrevLine);
	inline void SetEmphasisChached(bool bChached){m_bEmphasisChached = bChached;}

	// 取得ルーチン
	const wchar_t *GetLineData() const {return m_strLineData.c_str();}
	size_t GetLineLength() const {return m_strLineData.size();}
	inline size_t GetEthicLine() const { return m_nEthicLine; }
	inline size_t GetOffset() const {return m_nLineOffset;}
	inline size_t GetRealLineNum() const {return m_nRealLineNum;}
	inline CStaticVector<WordPt,sizeof(int)*8> *GetBetweenNext()
		{return &m_vecLineBetweenAfter;}
	inline std::vector<EmpPos> *GetColorInfo(){return &m_vecColorInfo;}
	inline std::vector<CUrlInfo> *GetUrlInfo(){return &m_vecUrlInfo;}
	inline std::vector<CUrlInfo> *GetMailInfo(){return &m_vecMailInfo;}
	inline std::vector<CUrlInfo> *GetLabelInfo(){return &m_vecLabelInfo;}// by Tetr@pod
	inline std::vector<CUrlInfo> *GetFuncInfo(){return &m_vecFuncInfo;}// by Tetr@pod
	inline bool EmphasisChached() const {return m_bEmphasisChached;}
	inline int GetLineIcons() const {return m_nLineIcons;}
	inline void SetLineIcons(int nLineIcons){m_nLineIcons = nLineIcons;}

	// 文字情報を元に計算を行うルーチン
	EthicInfo CalcEthicLine(size_t nPos,size_t nColumn,size_t nTab,int nMode) const;
	size_t CalcRealPosition(size_t nEthicLine,size_t nEthicPos,size_t nColumn,size_t nTab,int nMode) const;
	bool IsGoNext(const wchar_t *pChar,size_t nPos,size_t nNowCol,size_t nColumns,int nMode) const;
	WordInfo GetWordInfo( size_t nPos, bool bIsBackMode = true ) const;
	size_t CalcAutoIndentPos( size_t nPos ) const;

	// 文字関連
	enum CharSets
	{
		CHARSETS_ANSI_ALPHABET,				//!< ANSI(アルファベット)
		CHARSETS_ANSI_NUMBER,				//!< ANSI(数字)
		CHARSETS_ANSI_SYMBOL,				//!< ANSI(アンダーバーを除く記号)
		CHARSETS_ANSI_UNDER,				//!< ANSIアンダーバー)
		CHARSETS_HIRAGANA,					//!< 日本語(ひらがな)
		CHARSETS_KATAKANA,					//!< 日本語(カタカナ)
		CHARSETS_KATAKANA_HALF,				//!< 日本語(半角カタカナ)
		CHARSETS_KANJI,						//!< 日本語(漢字)
		CHARSETS_FULL_ALPHABET,				//!< 全角アルファベット
		CHARSETS_FULL_NUMBER,				//!< 全角数字
		CHARSETS_AINU_EXTENSION,			//!< アイヌ語用拡張
		CHARSETS_IPA_EXTENSION,				//!< IPA拡張
		CHARSETS_SYMBOLS,					//!< 記号
		CHARSETS_JP_NOBASHI,				//!< 日本語のばし音
		CHARSETS_KOREA,						//!< 韓国語
		CHARSETS_LATIN,						//!< ラテン
		CHARSETS_BALTIC,					//!< バルト諸国
		CHARSETS_ARABIC,					//!< アラビア
		CHARSETS_HEBREW,					//!< ヘブライ
		CHARSETS_GREEK,						//!< ギリシャ
		CHARSETS_CYRILLIC,					//!< キリル文字
		CHARSETS_VIETTNAMESE,				//!< ベトナム語
		CHARSETS_THAI,						//!< タイ語
		
		CHARSETS_UNKNOWN,					//!< 不明
	};

	static CharSets GetCharSets(wchar_t w);
	static CharSets GetCharSets(wchar_t w1,wchar_t w2);
	static bool IsDualChar(CharSets c);		/*二倍の横幅の文字か取得する*/
	static inline bool IsDualChar(wchar_t w){return IsDualChar(GetCharSets(w));}
	static inline bool IsDualChar(wchar_t w1,wchar_t w2){return IsDualChar(GetCharSets(w1,w2));}
	static inline bool IsSurrogateLead(wchar_t w){return 0xD800 <= w && w <= 0xDBFF;}
	static inline bool IsSurrogateTail(wchar_t w){return 0xDC00 <= w && w <= 0xDFFF;}

private:
	// 内部ルーチン
	bool IsMatched(const wchar_t *pStr1,const wchar_t *pStr2,size_t nLen);
	
	// クリッカブル関連の処理
	bool FindMail(size_t start,size_t *pBegin,size_t *pEnd);
	bool FindURL(size_t start,size_t *pBegin,size_t *pEnd);
	bool FindLabel(size_t start,size_t *pBegin,size_t *pEnd);// by Tetr@pod
	bool FindFunc(size_t start,size_t *pBegin,size_t *pEnd, wchar_t *SearchWord, bool bIsFunction, bool bMatchCase);// by Tetr@pod
	bool IsMailChar(wchar_t wc);
	bool IsURLChar(wchar_t wc);
	bool IsLabelChar(wchar_t wc);// by Tetr@pod

public:
	std::wstring m_strLineData;				//!< 一行のデータ

private:
	std::vector<EmpPos> m_vecColorInfo;		//!< 色分け情報
	TakeOver m_vecLineBetweenAfter;			//!< この後に引き継ぐ複数行強調表示文字列
	std::vector<CUrlInfo> m_vecUrlInfo;		//!< URL情報
	std::vector<CUrlInfo> m_vecMailInfo;	//!< メール情報
	std::vector<CUrlInfo> m_vecLabelInfo;	//!< ラベル情報 by Tetr@pod
	std::vector<CUrlInfo> m_vecFuncInfo;	//!< 命令・関数情報 by Tetr@pod
	size_t m_nEthicLine;					//!< 論理何行分？
	size_t m_nLineOffset;					//!< 論理行オフセット位置
	size_t m_nRealLineNum;					//!< 本当の行番号
	bool m_bEmphasisChached;				//!< 強調文字列がキャッシュされている
	int m_nLineIcons;						//!< 行ごとのアイコン
};

// 長いタイプの宣言
typedef std::list<CFootyLine>::iterator LinePt;
typedef std::list<CFootyLine>::size_type LineSize;
typedef std::list<CFootyLine> LsLines;

/*[EOF]*/
