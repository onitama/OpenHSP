/**
 * @file	FontObjects.cpp
 * @brief	フォントのオブジェクト管理クラスです。
 * @author	Shinji Watanabe
 * @date	2009/03/16
 */

#include "FontObjects.h"
#pragma warning (disable : 4996)

const int CFontObjects::m_nCharSets[FFM_NUM_FONTS] = 
{
	ANSI_CHARSET,
	BALTIC_CHARSET,
	CHINESEBIG5_CHARSET,
	EASTEUROPE_CHARSET,
	GB2312_CHARSET,
	GREEK_CHARSET,
	HANGUL_CHARSET,
	RUSSIAN_CHARSET,
	SHIFTJIS_CHARSET,
	TURKISH_CHARSET,
	VIETNAMESE_CHARSET,
	ARABIC_CHARSET,
	HEBREW_CHARSET,
	THAI_CHARSET,
};

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタです
 */
CFontObjects::CFontObjects()
{
	// 全てNULL化
	for (int i=0;i<FFM_NUM_FONTS;i++)
	{
		m_hFont[i] = NULL;
	}
	m_hRulerFont = NULL;
	m_fnWeight = FW_BOLD;
	m_nFontHeight = 0;
	m_nFontWidth = 0;
	m_nRulerHeight = 0;
	m_fForceFont = 0;		// 強制させるか？ by inovia
	
	// デフォルトフォント名を入れておく
	m_strFace[FFM_ANSI_CHARSET]			= L"Courier New";
	m_strFace[FFM_BALTIC_CHARSET]		= L"Courier New";
	m_strFace[FFM_BIG5_CHARSET]			= L"MingLiU";
	m_strFace[FFM_EASTEUROPE_CHARSET]	= L"Courier New";
	m_strFace[FFM_GB2312_CHARSET]		= L"SimSun";
	m_strFace[FFM_GREEK_CHARSET]		= L"Courier New";
	m_strFace[FFM_HANGUL_CHARSET]		= L"GulimChe";
	m_strFace[FFM_RUSSIAN_CHARSET]		= L"Courier New";
	m_strFace[FFM_SHIFTJIS_CHARSET]		= L"MS Gothic";
	m_strFace[FFM_TURKISH_CHARSET]		= L"Courier New";
	m_strFace[FFM_VIETNAMESE_CHARSET]	= L"Courier New";
	m_strFace[FFM_ARABIC_CHARSET]		= L"GulimChe";
	m_strFace[FFM_HEBREW_CHARSET]		= L"Courier New";
	m_strFace[FFM_THAI_CHARSET]			= L"Tahoma";	
	m_nFontPoint	=	FONTNORMAL_DEFAULT;
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタです。
 */
CFontObjects::~CFontObjects()
{
	for (int i=0;i<FFM_NUM_FONTS;i++)
	{
		if (m_hFont[i])
		{
			DeleteObject(m_hFont[i]);
			m_hFont[i] = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 一つ分のフォントを作り直す処理
 */
bool CFontObjects::CreateFontObject(int nType,HDC hDC)
{
	if( m_hFont[nType] )
	{
		DeleteObject(m_hFont[nType]);
	}
	m_hFont[nType] = GetFontHandle(m_strFace[nType].c_str(),
			HeightFromPoint(hDC,m_nFontPoint),
			false,m_nCharSets[nType]);
	if (!m_hFont[nType])return false;
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief 全てのフォントを作り直す処理です
 */
bool CFontObjects::CreateAll( HDC hDC )
{
	for (int i=0;i<FFM_NUM_FONTS;i++)
	{
		if (!CreateFontObject(i,hDC))return false;
	}
	SetFontPixels(hDC);
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief フォントをセットする処理
 */
bool CFontObjects::SetFontFace(int nType,const wchar_t *pFontFace,HDC hDC)
{
	m_strFace[nType] = pFontFace;
	if (!CreateFontObject(nType,hDC))return false;
	SetFontPixels(hDC);
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief フォントの大きさ調べてセットする処理
 * @param	hDC	[in] 調べる対象のデバイスコンテキスト
 */
void CFontObjects::SetFontPixels( HDC hDC )
{
	SIZE wSize;						// 描画したサイズ
	
	m_nFontWidth = 0;
	m_nFontHeight = 0;
	
	for ( int i = 0; i < FFM_NUM_FONTS ; i++ )
	{
		SelectObject( hDC, m_hFont[ i ] );
		GetTextExtentPoint32W(hDC,L"abcdefghijklmnopqrstuvwxyz",26,&wSize);
		m_nFontWidth = max( wSize.cx / 26, m_nFontWidth );
		m_nFontHeight = max( wSize.cy, m_nFontHeight );
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief ルーラー用フォントの作成
 */
bool CFontObjects::SetRuler(HDC hDC,int nRulerHeight)
{
	// 宣言
	HFONT hTemp;
	// フォントを作成
	hTemp = GetFontHandle(m_strFace[FFM_ANSI_CHARSET].c_str(),nRulerHeight,false,ANSI_CHARSET);
	if (!hTemp)return false;
	m_hRulerFont = hTemp;
	m_nRulerHeight = nRulerHeight;
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief フォントを作成して、それを返します。
 * @param	szFontName	フォントの名前
 * @param	nPoint		フォントの大きさ
 * @param	hDC			デバイスコンテキストハンドル
 */
HFONT CFontObjects::GetFontHandle(const wchar_t *szFontName,int nHeight,bool bBold,int nCharSets)
{
	LOGFONTW fontStruct;
	memset(&fontStruct,0,sizeof(LOGFONTW));
	fontStruct.lfHeight = -nHeight;
	fontStruct.lfWeight = (bBold ? m_fnWeight : FW_NORMAL);
	if (m_fForceFont == 0){// 条件追加 by inovia
		fontStruct.lfCharSet = nCharSets;
	}else{
		fontStruct.lfCharSet = DEFAULT_CHARSET;
	}
	fontStruct.lfOutPrecision = OUT_DEFAULT_PRECIS;
	fontStruct.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	fontStruct.lfQuality = DEFAULT_QUALITY;
	fontStruct.lfPitchAndFamily = FF_MODERN | FIXED_PITCH;
	wcsncpy(fontStruct.lfFaceName,szFontName,LF_FACESIZE);
	return CreateFontIndirectW(&fontStruct);
}

//-----------------------------------------------------------------------------
/**
 * @brief nNumber * nNumerator / nDenominator を四捨五入して返す
 */
int CFontObjects::GetMulDiv(int nNumber,int nNumerator,int nDenominator)
{
	long long nTemp = nNumber * nNumerator;
	double fReturn = ((double)nTemp / nDenominator);
	fReturn += 0.5f;
	return (int)floor(fReturn);
}

//-----------------------------------------------------------------------------
/**
 * @brief フォントの強制 by inovia
 * @param	flag [in] フラグ
 */
void CFontObjects::SetForceFont( int flag )
{
	m_fForceFont = flag;
}
/*[EOF]*/
