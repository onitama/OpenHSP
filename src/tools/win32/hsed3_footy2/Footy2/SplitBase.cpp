/**
 * @file SplitBase.cpp
 * @brief スプリットバーの基底クラスです。
 * @author Shinji Watanabe
 * @date Dec. 28, 2008
 */

#include "SplitBase.h"

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
CSplitBase::CSplitBase()
{
	m_hWnd = NULL;
	m_hWnd2 = NULL;
	m_nMode = SPLIT_DUAL;
	m_pViews = NULL;
	m_bDrag = false;
	m_bVisible = false;
	m_pOtherSplit = NULL;
}


//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 *
 */
CSplitBase::~CSplitBase()
{
	DestroySplitBar();
}

//-----------------------------------------------------------------------------
/**
 * @brief 構築する
 */
 bool CSplitBase::Create(HWND hWndParent, HINSTANCE hInstance)
{
	if (!hWndParent || !hInstance)return false;
	
	// すでに生成済みの場合はいったん破棄
	if ( m_hWnd )
	{
		DestroySplitBar();
	}
	
	// クラスを登録する
	WNDCLASS wc;
	
#ifdef UNDER_CE
	wc.style			= 0;
#else	/*UNDER_CE*/
	// wc.style			= CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	wc.style			= CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW | CS_DBLCLKS;// by Tetr@pod
#endif	/*UNDER_CE*/
	wc.lpfnWndProc		= SplitWinProc;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;						// 状態によって適宜変更する
	wc.hbrBackground	= (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= SPLIT_WNDCLASSNAME;
	wc.cbClsExtra		= 0;						// 使用しない。0を指定
	wc.cbWndExtra		= 0;						// 使用しない。0を指定
	RegisterClass(&wc);								// ウィンドウクラスの登録

	/*ウィンドウの作成*/
	m_hWnd = CreateWindowEx(0,SPLIT_WNDCLASSNAME,SPLIT_WNDCLASSNAME,
							WS_CHILD,0,0,0,0,hWndParent,NULL,hInstance,NULL);
	if (!m_hWnd)return false;
	m_hWnd2 = m_hWnd;
	
	/*プロパティの設定*/
	SetProp(m_hWnd,SPLIT_PROPNAME,(HANDLE)this);
	return true;
}

//-----------------------------------------------------------------------------
/**
 * @brief スプリットバーを削除します
 */
void CSplitBase::DestroySplitBar()
{
	if (m_hWnd)
	{
		RemoveProp( m_hWnd, SPLIT_PROPNAME );
		DestroyWindow( m_hWnd );
		m_hWnd = NULL;
		m_hWnd2 = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief ウィンドウプロシージャです。
 */
LRESULT CALLBACK CSplitBase::SplitWinProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	CSplitBase *pSplitClass=NULL;	// そのクラスへのポインタ
	pSplitClass=(CSplitBase*)GetProp(hWnd,SPLIT_PROPNAME);
	if (pSplitClass)
		return pSplitClass->MainProc(msg,wParam,lParam);
	else
		return DefWindowProc(hWnd,msg,wParam,lParam);
}


//-----------------------------------------------------------------------------
/**
 * @brief ウィンドウに関連づけられてるクラスインスタンスが生成できた場合に行う
 *		  実際のウィンドウプロシージャ処理です
 */
LRESULT CSplitBase::MainProc(UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		//FOOTY2_PRINTF( L"Double Click A!\n" );
		OnMouseDown((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		OnMouseMove((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		OnMouseUp((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;

	case WM_LBUTTONDBLCLK:// by Tetr@pod
		FOOTY2_PRINTF( L"Double Click A!\n" );
		OnMouseDouble((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;

	default:
		return DefWindowProc(m_hWnd,msg,wParam,lParam);
	}
	return false;
}

//-----------------------------------------------------------------------------
/**
 * @brief マウスが動いたときの処理です
 * @param	x	[in] マウスx座標
 * @param	y	[in] マウスy座標
 */
/*void CSplitBase::OnMouseDown(int x,int y)
{
	if (m_bDrag == false) {// by Tetr@pod
		SetCapture(m_hWnd);
		m_bDrag = true;
	}
}*/
// 個別処理に変更

//-----------------------------------------------------------------------------
/**
 * @brief マウスの左ボタンがあがったときの処理
 */
void CSplitBase::OnMouseUp(int x,int y)
{
	if (m_bDrag == true) {// by Tetr@pod
		ReleaseCapture();

		if (m_nMode == SPLIT_QUAD && m_pOtherSplit) {// 補正する by Tetr@pod
			m_pOtherSplit->MoveWin(m_pOtherSplit->GetX(),m_pOtherSplit->GetY(),m_nBaseWidth,m_nBaseHeight);
			MoveWin(m_x,m_y,m_nBaseWidth,m_nBaseHeight);
		}

		m_bDrag = false;
	}
}

//-----------------------------------------------------------------------------
/**
 * @brief 表示状態を設定できます
 */
void CSplitBase::SetVisible(bool bVisible)
{
	ShowWindow(m_hWnd,bVisible ? SW_SHOWNORMAL : SW_HIDE);
}


//-----------------------------------------------------------------------------
/**
 * @brief 親ウィンドウが動いたときに呼び出してください
 */
void CSplitBase::OnBaseWindowMove(int x,int y,int nWidth,int nHeight)
{
	m_nBaseX = x;
	m_nBaseY = y;
	m_nBaseWidth = nWidth;
	m_nBaseHeight = nHeight;
}

/*[EOF]*/
