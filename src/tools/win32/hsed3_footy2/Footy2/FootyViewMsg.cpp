/*===================================================================
CFootyViewクラス
Footyのビュー管理を行います。
このファイルはメッセージ管理を行います。
===================================================================*/

#include "FootyView.h"

/*-------------------------------------------------------------------
CFootyView::MainProc
メインプロシージャ
-------------------------------------------------------------------*/
LRESULT CFootyView::MainProc(UINT msg,WPARAM wParam,LPARAM lParam){
	/*ホイールメッセージかどうか取得する*/
#ifndef UNDER_CE
	if (msg == m_uWheelMsg)msg = WM_MOUSEWHEEL;
#endif	/*UNDER_CE*/

	/*メッセージにより場合分け*/
	switch(msg){
//キー関連
	case WM_CHAR:
		OnChar((wchar_t)wParam);
		break;
	case WM_IME_COMPOSITION:
		OnImeComposition(lParam);
		return DefWindowProc(m_hWnd,msg,wParam,lParam);
	case WM_IME_REQUEST:
		return OnImeRequest(wParam,lParam);
	case WM_KEYDOWN:
		OnKeyDown((int)wParam);
		break;
	case WM_GETDLGCODE:
		if (m_bIsFocused)	/*フォーカスがあるときのみ全部取得*/
			return DLGC_WANTALLKEYS;
		else				/*それ以外はデフォルトに任せる*/
			return DefWindowProc(m_hWnd,msg,wParam,lParam);
//マウス関連
	case WM_LBUTTONDOWN:
		OnMouseDown((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		OnMouseMove((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_LBUTTONUP:
		OnMouseUp((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_LBUTTONDBLCLK:
		OnMouseDouble((int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam));
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel((short)HIWORD(wParam) < 0);
		break;
//スクロール
    case WM_VSCROLL:
		OnVScroll(wParam);
        break;
	case WM_HSCROLL:
		OnHScroll(wParam);
		break;
//タイマーイベント
	case WM_TIMER:
		switch(wParam)
		{
		case eTimer_AutoVertical:
			FOOTY2_PRINTF( L"On Auto Vertical Scroll Timer\n" );
			OnAutoVerticalTimer();
			break;
		case eTimer_AutoHorizontal:
			FOOTY2_PRINTF( L"On Auto Horizontal Scroll Timer\n" );
			OnAutoHorizontalTimer();
			break;
		}
		break;
//フォーカス関連
	case WM_SETFOCUS:
		OnGotFocus();
		FOOTY2_PRINTF(L"On got focus\n");
		break;
	case WM_KILLFOCUS:
		OnLostFocus();
		FOOTY2_PRINTF(L"On lost focus\n");
		break;
	case WM_PAINT:
		
		PAINTSTRUCT ps;
		BeginPaint(m_hWnd,&ps);
		//FOOTY2_PRINTF( L"BeginPaint %d\n", m_nFootyID );
		Refresh(ps.hdc,&ps.rcPaint);
		EndPaint(m_hWnd,&ps);
		//FOOTY2_PRINTF( L"EndPaint %d\n", m_nFootyID );
		
		break;
	default:
		return DefWindowProc(m_hWnd,msg,wParam,lParam);
	}
	return false;
}

/*----------------------------------------------------------------
CFootyView::OnLostFocus
フォーカスを失ったときに発生します。
----------------------------------------------------------------*/
void CFootyView::OnLostFocus(){


	m_bIsFocused = false;
	m_cCaret.Hide();		/*キャレットを非表示にして*/
	if ((m_bUnderlineDraw == false)){
		m_cCaret.Destroy();		/*破壊。*/
		CaretRefresh();
	}
	/*イベント出力*/
	if (m_pFuncFocus)
		m_pFuncFocus(m_nFootyID,m_pDataFocus,m_nViewID,false);
	
//	m_cCaret.Hide();		/*キャレットを非表示にして*/
//	// 4つのビューがフォーカスされてないときは破棄しない
//	// １つでもビューがフォーカスされている場合は破棄

//	if ( ( m_pView[0].IsFocused() == false ) && ( m_pView[1].IsFocused() == false ) && ( m_pView[2].IsFocused() == false ) && ( m_pView[3].IsFocused() == false ) ){
//		FOOTY2_PRINTF( L"Test_OnLostFocus %d\n", m_nFootyID );
//	}else{
//		if ((m_bUnderlineDraw == false)){
//			m_cCaret.Destroy();		/*破壊。*/
//			CaretRefresh();
//		}
//	}


//	m_bIsFocused = false;
	
	/*イベント出力*/
//	if (m_pFuncFocus)
//		m_pFuncFocus(m_nFootyID,m_pDataFocus,m_nViewID,false);
	
}

/*----------------------------------------------------------------
CFootyView::OnGotFocus
フォーカスを得たときに発生します。
----------------------------------------------------------------*/
void CFootyView::OnGotFocus(){

	// アンダーラインの表示バグ(仕様？)修正 by Tetr@pod
	if (m_bUnderlineDraw){// 非フォーカス時のアンダーラインの描画が有効なときで、
		for(int i=0; i<4; i++){// 他のビューの
			if (m_pView[i].IsFocused() == false){// フォーカスがないときは
				m_pView[i].m_cCaret.Destroy();// そのビューのアンダーラインを破棄
				m_pView[i].CaretRefresh();// そして更新
			}
		}
	}

	if (!m_bIsFocused){				/*フォーカスを失っていたときは*/
		m_bIsFocused = true;		/*フォーカスフラグを立てて*/
		Refresh(false);				/*再描画させる*/
	}
	CaretMove();
	/*イベント出力*/
	if (m_pFuncFocus)
		m_pFuncFocus(m_nFootyID,m_pDataFocus,m_nViewID,true);
}
