/*===================================================================
CDoubleBufferingクラス
ウィンドウのダブルバッファリング処理を行うクラスです。
===================================================================*/

#pragma once

#include "EthicLine.h"

class CDoubleBuffering{
public:
	CDoubleBuffering();
	virtual ~CDoubleBuffering();

	//! サイズが変更されたときに呼ばれる処理
	bool ChangeSize(HWND hWnd,int nWidth,int nHeight);
	//! ダブルバッファリング用のコンパチブルデバイスコンテキストを取得する
	inline HDC GetDC(){return m_MainSurfaceDC;}
	//! 描画されたかどうかを設定する処理
	void SetRendInfo(CEthicLine *pFirstVisible);
	//! 一度でも描画されているかどうか取得する処理(高速化のために描画処理を回避できる可能性がある)
	inline bool IsRendered(){return m_bRendered;}
	//! 前に描画されたときのバッファリング状態を返す
	inline CEthicLine* const GetFirstVisible(){return &m_cRenderedFirstVisible;}
	inline int GetWidth(){return m_nWidth;};
	inline int GetHeight(){return m_nHeight;};

private:
	/*ダブルバッファリングのデータ*/
	HBITMAP m_MainSurface;					//!< ダブルバッファリング
	HBITMAP m_MainSurfaceOld;				//!< 前に使用していたもの
	HDC m_MainSurfaceDC;					//!< コンパチブルデバイスコンテキスト
	int m_nWidth,m_nHeight;					//!< 大きさ
	
	/*どのような状態で再描画されたのか*/
	bool m_bRendered;						//!< すでに一度以上描画されているか？
	CEthicLine m_cRenderedFirstVisible;		//!< 描画時最上位行
};

/*[EOF]*/
