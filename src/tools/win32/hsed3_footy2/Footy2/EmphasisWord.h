/*===================================================================
CEmphasisWordクラス
一種類分の強調表示文字列を保持するクラスです。
===================================================================*/

#pragma once

class CEmphasisWord
{
public:
	CEmphasisWord();

	bool SetValues(const wchar_t *pString1,const wchar_t *pString2,
		int nType,int nFlags,int nLevel,int nPermission,int nIndependence,COLORREF nColor);
	
	/*取得系のルーチン*/
	inline bool IsPermitted(int nLevel){return (m_nPermission & (1 << nLevel)) != 0;}
	bool CheckIndependence(const wchar_t *pChar,bool bCheckAfter);
	inline int GetLevel(){return m_nLevel;}
	inline const wchar_t *GetString1() const { return m_strWord1.c_str(); }
	inline const wchar_t *GetString2() const { return m_strWord2.c_str(); }
	inline size_t GetLen1() const { return m_strWord1.size(); }
	inline size_t GetLen2() const { return m_strWord2.size(); }
	inline COLORREF GetColor(){return m_nColor;}
	inline int GetType(){return m_nType;}
	inline bool IsOnlyHead(){return (m_nEmpFlag & EMPFLAG_HEAD) != 0;}
	inline bool IsNonCsWord(){return (m_nEmpFlag & EMPFLAG_NON_CS) != 0;}
	
	/*オペレータ*/
	bool operator < (CEmphasisWord &cEmpWord){return GetLen1() < cEmpWord.GetLen1();}
	bool operator > (CEmphasisWord &cEmpWord){return GetLen1() > cEmpWord.GetLen1();}

	/*文字列を小文字化する*/
	static void SetLower(wchar_t *szStr,size_t nLen);

private:
public:
	/*一時的な変数*/
	bool m_bDuplix;

private:
	std::wstring m_strWord1;			//!< 文字列1
	std::wstring m_strWord2;			//!< 文字列2

	int m_nPermission;					//!< 乗ってるレベル
	int m_nIndependence;				//!< 独立状態
	int m_nLevel;						//!< この文字列に設定されているレベル
	int m_nEmpFlag;						//!< 強調フラグ
	int m_nType;						//!< 強調表示タイプ
	COLORREF m_nColor;					//!< 強調表示色
};

typedef std::list<CEmphasisWord> LsWords;
typedef std::list<CEmphasisWord>::iterator WordPt;

/*[EOF]*/
