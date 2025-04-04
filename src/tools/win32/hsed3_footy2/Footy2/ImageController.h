/*------------------------------------------------------------------------------
	îwåiâÊëúì«Ç›çûÇ›Åïï`âÊ for HSP Script Editor by inovia
	çÏê¨ 2015.01.09
	èCê≥ 2015.01.24
------------------------------------------------------------------------------*/

#include <windows.h>
#include <gdiplus.h>
#include <olectl.h>
#pragma once
#pragma	comment(lib, "Gdiplus.lib")


class CImageController{
private:
	Gdiplus::Bitmap * m_Bmp;
	Gdiplus::GdiplusStartupInput m_pSI;
	ULONG_PTR m_lpToken;
	Gdiplus::Graphics * m_g;
	HDC m_hDrawDC;
	RECT m_DesktopRect;
	BOOL m_Load;
	HWND m_hWnd;
	COLORREF m_Color;
public:
	CImageController(HWND hwnd);
	~CImageController(void);
	BOOL Load(const WCHAR *fileName);
	void Destroy(void);
	void Update(void);
	void Draw(HDC targetHDC, int x1, int y1, int x2, int y2);
	void Draw(HDC targetHDC, int x1, int y1, int x2, int y2, int maxWidth, int maxHeight);
	void Clear(void);
	void Clear(COLORREF color);
	void UnLoad(void);
	BOOL IsLoaded(void){
		return m_Load;
	}
	void SetBgColor(COLORREF color){
		m_Color = color;
	}
	COLORREF GetBgColor(void){
		return m_Color;
	}
};
