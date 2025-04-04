/**
 * @file	FontObjects.cpp
 * @brief	フォントのオブジェクト管理クラスです。
 * @author	Shinji Watanabe
 * @date	2009/03/16
 */

#pragma once

class CFontObjects
{
public:
	// コンストラクタ
	CFontObjects();
	virtual ~CFontObjects();

	bool CreateAll( HDC hDC );
	

	// フォントを作成するためのルーチン
	bool SetRuler(HDC hDC,int nRulerHeight);

	inline HFONT UseFont(HDC hDC,FontMode nMode)
	{
		return (HFONT)SelectObject(hDC,m_hFont[nMode]);
	}
	inline HFONT UseKanjiFont(HDC hDC)
	{
		return (HFONT)SelectObject(hDC,GetKanjiFont());
	}
	inline HFONT GetKanjiFont()
	{
		return m_hFont[FFM_SHIFTJIS_CHARSET];
	}
	inline HFONT UseRulerFont(HDC hDC)
	{
		return (HFONT)SelectObject(hDC,m_hRulerFont);
	}

	// 取得
	inline int GetWidth()		const {return m_nFontWidth;}
	inline int GetHeight()		const { return m_nFontHeight; }
	inline int GetRulerHeight() const { return m_nRulerHeight; }
	inline int GetFontSize()	const { return m_nFontPoint; }
	
	/*設定*/
	void SetFontSize( int nPoint, HDC hDC )
	{
		m_nFontPoint = nPoint;
		CreateAll( hDC );
	}
	bool SetFontFace(int nType,const wchar_t *pFontFace,HDC hDC);

	// フォントの強制 by inovia
	void SetForceFont( int flag );
	// 高速描画 by inovia
	void SetSpeedDraw( int flag );

private:
	bool CreateFontObject(int nType,HDC hDC);
	HFONT GetFontHandle(const wchar_t *szFontName,int nHeight,bool bBold,int nCharSets);
	void SetFontPixels(HDC hDC);
	int GetMulDiv(int nNumber,int nNumerator,int nDenominator);
	inline int HeightFromPoint(HDC hDC,int nPoint){
		return -GetMulDiv(nPoint, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	}
	
private:
	enum fixed_num
	{
#ifdef UNDER_CE
		FONTNORMAL_DEFAULT = 9,
#else	/*UNDER_CE*/
		FONTNORMAL_DEFAULT = 11,
#endif	/*UNDER_CE*/
	}	/*enum fixed_num*/;
	
	HFONT m_hFont[FFM_NUM_FONTS];				//!< フォントリスト
	HFONT m_hRulerFont;							//!< ルーラーのフォント
	std::wstring m_strFace[FFM_NUM_FONTS];		//!< フォントの名前
	static const int m_nCharSets[FFM_NUM_FONTS];
	int m_fnWeight;								//!< ボールド体の時の太さ
	int m_nFontPoint;							//!< フォントのサイズ(ポイント)
	int m_nFontHeight;							//!< メインフォントの高さ(ピクセル)
	int m_nFontWidth;							//!< メインフォントの幅(ピクセル)
	int m_nRulerHeight;							//!< ルーラーフォントの高さ
	int m_fForceFont;							//!< フォント強制させるか？ by inovia
};

/*[EOF]*/
