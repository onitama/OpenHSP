/**
 * @file DoubleBuffering.cpp
 * @brief ウィンドウのダブルバッファリング処理を行うクラスです。
 * @author Shinji Watanabe
 * @version 1.0
 * @date Jan.06.2008
 */

#include "DoubleBuffering.h"

/**
 * CDoubleBuffering::CDoubleBuffering
 * @brief コンストラクタ
 */
CDoubleBuffering::CDoubleBuffering(){
	m_MainSurface = NULL;
	m_MainSurfaceOld = NULL;
	m_MainSurfaceDC = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bRendered = false;
}

/**
 * CDoubleBuffering::~CDoubleBuffering
 * @brief デストラクタ
 */
CDoubleBuffering::~CDoubleBuffering(){
	/*サーフィス関連を解放する*/
	if (m_MainSurface){
		SelectObject(m_MainSurfaceDC,m_MainSurfaceOld);
		DeleteObject(m_MainSurface);
		m_MainSurface = NULL;
	}
	if (m_MainSurfaceDC){
		DeleteDC(m_MainSurfaceDC);
	}
}

/**
 * CDoubleBuffering::SetRendInfo
 * @brief 描画情報を保存する
 */
void CDoubleBuffering::SetRendInfo(CEthicLine *pFirstVisible){
	m_bRendered = true;
	m_cRenderedFirstVisible = *pFirstVisible;
}

/**
 * CDoubleBuffering::ChangeSize
 * @brief サイズの変更処理
 */
bool CDoubleBuffering::ChangeSize(HWND hWnd,int nWidth,int nHeight)
{
	// 今までと同じサイズなら確保しない
	if (m_nWidth == nWidth && m_nHeight == nHeight)
	{
		return true;
	}

	HDC hDC = ::GetDC(hWnd);
	//FOOTY2_PRINTF( L"GetDC\n" );
	if ( !hDC )
	{
		FOOTY2_PRINTF( L"But failed...\n" );
	}

	// 互換デバイスコンテキストがあるかどうか
	if (!m_MainSurfaceDC)
	{
		m_MainSurfaceDC = CreateCompatibleDC(hDC);
		if (!m_MainSurfaceDC)
		{
			FOOTY2_PRINTF( L"failed to create compatibleDC\n" );
			ReleaseDC(hWnd,hDC);
			FOOTY2_PRINTF( L"ReleaseDC\n" );
			return false;
		}
	}
	
	// ダブルバッファリングがあれば破棄する
	if (m_MainSurface)
	{
		DeleteObject(m_MainSurface);
		m_MainSurface = NULL;
	}
	
	// 新しくバッファを生成する
	m_MainSurface=CreateCompatibleBitmap(hDC,nWidth,nHeight);		// それに関連するビットマップを作成
	if (!m_MainSurface)
	{
		ReleaseDC( hWnd, hDC );
		FOOTY2_PRINTF( L"ReleaseDC\n" );
		return false;
	}
	if (!m_MainSurfaceOld)
	{
		m_MainSurfaceOld = (HBITMAP)SelectObject(m_MainSurfaceDC,m_MainSurface);	
	}
	else
	{
		SelectObject(m_MainSurfaceDC,m_MainSurface);
	}
	if (m_nWidth > nWidth || m_nHeight > nHeight)					// 小さくされたとき
	{
		BitBlt(m_MainSurfaceDC,0,0,nWidth,nHeight,hDC,0,0,SRCCOPY);	// コンバチブルDCにコピー
	}
	ReleaseDC(hWnd,hDC);											// デバイスコンテキスト開放
	//FOOTY2_PRINTF( L"ReleaseDC\n" );

	// メンバ変数に格納
	m_nHeight = nHeight;
	m_nWidth = nWidth;

	// 一度も描画されていない状態にする
	m_bRendered = false;
	return true;
}

/*[EOF]*/
