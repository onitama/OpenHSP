/*===================================================================
CCursorクラス
カーソル管理を行います。
===================================================================*/

#include "Cursor.h"
#include "resource.h"

HCURSOR CCursor::m_hOnUrlCursor = NULL;
HCURSOR CCursor::m_hLineCountCursor = NULL;
HCURSOR CCursor::m_hIBeam = NULL;
HCURSOR CCursor::m_hArrowCursor = NULL;
HCURSOR CCursor::m_hUpDownCursor = NULL;
HCURSOR CCursor::m_hRightLeftCursor = NULL;
HCURSOR CCursor::m_hCrossCursor = NULL;// スプリットバーの中央ドラッグ by Tetr@pod

/*-------------------------------------------------------------------
CCursor::LoadCursors
カーソルをロードします。
-------------------------------------------------------------------*/
bool CCursor::LoadCursors(HINSTANCE hInstance){
	if (!hInstance)return false;
	/*カーソルを破棄*/
	DestroyCursors();
	/*カーソルの読み込み*/
	m_hArrowCursor = LoadCursor(NULL,IDC_ARROW);
	m_hIBeam = LoadCursor(NULL,IDC_IBEAM);
	m_hUpDownCursor = LoadCursor(NULL,IDC_SIZENS);
	m_hRightLeftCursor = LoadCursor(NULL,IDC_SIZEWE);

	m_hCrossCursor = LoadCursor(NULL,IDC_SIZEALL);// スプリットバーの中央ドラッグby Tetr@pod

	m_hOnUrlCursor = LoadCursor(NULL,IDC_HAND);
	//m_hOnUrlCursor = LoadCursor(NULL,IDC_ARROW);
	//m_hOnUrlCursor = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_URL));
	if (!m_hOnUrlCursor)return false;

	m_hLineCountCursor = LoadCursor(NULL,IDC_ARROW);
	//m_hLineCountCursor = LoadCursor(hInstance,MAKEINTRESOURCE(IDC_LINE));
	if (!m_hLineCountCursor)return false;
	return true;
}

/*-------------------------------------------------------------------
CCursor::DestroyCursors
カーソルを破棄します。
-------------------------------------------------------------------*/
void CCursor::DestroyCursors(){
	/*自分でロードしたのは破壊しちゃうよ*/
	//SAFE_DELETEOBJECT(m_hOnUrlCursor);
	//SAFE_DELETEOBJECT(m_hLineCountCursor);
	/*共通オブジェクトは破壊しちゃまずいでしょ…*/
	m_hOnUrlCursor = NULL;
	m_hLineCountCursor = NULL;

	m_hIBeam = NULL;
	m_hArrowCursor = NULL;
	m_hUpDownCursor = NULL;
	m_hRightLeftCursor = NULL;
	m_hCrossCursor = NULL;// スプリットバーの中央ドラッグ by Tetr@pod
}

/*-------------------------------------------------------------------
CCursor::Use●●
各カーソルを使用するルーチンです。
----------------------------------------------------------c---------*/
void CCursor::UseArrow(){				/*アローカーソルを使用します。*/
	SetCursor(m_hArrowCursor);	
}
void CCursor::UseIBeam(){				/*Iビームカーソルを使用します。*/
	SetCursor(m_hIBeam);
}
void CCursor::UseUrlCursor(){			/*URLに乗ったときのカーソルを使用します。*/
	SetCursor(m_hOnUrlCursor);
}
void CCursor::UseLineCount(){			/*行番号表示領域のカーソルを使用します。*/
	SetCursor(m_hLineCountCursor);
}
void CCursor::UseUpDown(){			/*水平スプリットバーのカーソルを使用します。*/
	SetCursor(m_hUpDownCursor);
}
void CCursor::UseRightLeft(){			/*垂直スプリットバーのカーソルを使用します。*/
	SetCursor(m_hRightLeftCursor);
}

void CCursor::UseCross(){			/*水平・垂直スプリットバーのカーソルを使用します。 by Tetr@pod*/
	SetCursor(m_hCrossCursor);
}


/*[EOF]*/
