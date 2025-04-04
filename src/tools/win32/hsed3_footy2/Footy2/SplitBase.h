/**
 * @file SplitBase.h
 * @brief スプリットバーの基底クラスです。
 * @author Shinji Watanabe
 * @date Dec. 28, 2008
 */

#pragma once

#include "FootyView.h"

class CFooty;

#define SPLIT_WNDCLASSNAME	L"Splitter"
#define SPLIT_PROPNAME		L"SplitterClass"

class CSplitBase
{
public:
	CSplitBase();
	virtual ~CSplitBase();

	bool Create( HWND hWndParent, HINSTANCE hInstance );
	void DestroySplitBar();
	void OnBaseWindowMove(int x,int y,int nWidth,int nHeight);
	inline HWND GetWnd(){return m_hWnd;}
	inline int GetX() const { return m_x; }
	inline int GetY() const { return m_y; }

	virtual bool MoveWin(int x,int y,int nWidth,int nHeight){ return false; }// by Tetr@pod, return false;はダミー
	
	void SetViews(CFooty *pFooty, CFootyView *pTopView)
	{
		m_pFooty = pFooty;// by Tetr@pod, ダブルクリックによる分割解除のため
		m_pViews = pTopView;
		m_nMode = SPLIT_DUAL;
	}
	void SetViews(CFooty *pFooty, CFootyView *pTopView,CSplitBase *pSplit)
	{
		m_pFooty = pFooty;// by Tetr@pod
		m_pViews = pTopView;
		m_pOtherSplit = pSplit;
		m_nMode = SPLIT_QUAD;
	}

	void SetVisible(bool bVisible);

	enum fixed_num
	{
		SPLIT_SIZE = 4,

		// 中心の範囲 by Tetr@pod
		SPLIT_CENTER_RANGE1 = 32,// 左上
		SPLIT_CENTER_RANGE2 = 32,// 右下
	};

protected:

private:
	static LRESULT CALLBACK SplitWinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT MainProc(UINT msg,WPARAM wParam,LPARAM lParam);
	// virtual void OnMouseDown(int x,int y);
	virtual void OnMouseDown(int x,int y){}// 個別処理に変更
	virtual void OnMouseMove(int x,int y){}
	virtual void OnMouseUp(int x,int y);

	virtual void OnMouseDouble(int x,int y){}// by Tetr@pod


public:
protected:
	enum SplitMode
	{
		SPLIT_DUAL,						//!< デュアルビュー
		SPLIT_QUAD,						//!< クアッドビュー
	};

	bool m_bDrag;						//!< 現在ドラッグ中？
	int m_x,m_y;						//!< 現在の位置
	int m_nBaseX,m_nBaseY;				//!< ベースウィンドウの位置
	int m_nBaseWidth,m_nBaseHeight;		//!< ベースウィンドウの大きさ
	bool m_bIsCenter;					//!< 中央ドラッグ中？ by Tetr@pod

	HWND m_hWnd2;						//!< ウィンドウハンドル 個別処理のため
	
	CFootyView *m_pViews;				//!< Footyのビュー
	CFooty *m_pFooty;					//!< Footy by Tetr@pod
	CSplitBase *m_pOtherSplit;			//!< 他のスプリットバー
	SplitMode m_nMode;					//!< 現在のビューモード

private:
	HWND m_hWnd;						//!< ウィンドウハンドル
	bool m_bVisible;					//!< 表示状態か？
};

/*[EOF]*/
