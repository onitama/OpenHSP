/*===================================================================
CCursorクラス
カーソル管理を行います。全てstaticで構成されます。
===================================================================*/

#pragma once

class CCursor{
public:
	/*構築*/
	static bool LoadCursors(HINSTANCE hInstance);/*カーソルをロードします。*/
	static void DestroyCursors();				/*カーソルを破棄します。*/

	/*カーソルの使用*/
	static void UseArrow();						/*アローカーソルを使用します。*/
	static void UseIBeam();						/*Iビームカーソルを使用します。*/
	static void UseUrlCursor();					/*URLに乗ったときのカーソルを使用します。*/
	static void UseLineCount();					/*行番号表示領域のカーソルを使用します。*/
	static void UseUpDown();					/*上下カーソルを使用します。*/
	static void UseRightLeft();					/*左右カーソルを使用します。*/
	static void UseCross();					/*十字カーソルを使用します。*/

	
//private:
	static HCURSOR m_hArrowCursor;				/*アローカーソル*/
	static HCURSOR m_hIBeam;					/*Iビーム*/
	static HCURSOR m_hLineCountCursor;			/*行番号表示領域のカーソル*/
	static HCURSOR m_hOnUrlCursor;				/*URLに乗ったときのカーソル*/
	static HCURSOR m_hUpDownCursor;				/*上下カーソル*/
	static HCURSOR m_hRightLeftCursor;			/*左右カーソル*/
	static HCURSOR m_hCrossCursor;				/*十字カーソル*/
};

/*[EOF]*/
