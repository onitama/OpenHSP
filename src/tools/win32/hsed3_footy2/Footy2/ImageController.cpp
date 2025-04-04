/*------------------------------------------------------------------------------
	背景画像読み込み＆描画 for HSP Script Editor by inovia
	作成 2015.01.09
	修正 2015.04.29
------------------------------------------------------------------------------*/

#include <windows.h>
#include <gdiplus.h>
#include <olectl.h>
#pragma once
#pragma	comment(lib, "Gdiplus.lib")
#include "ImageController.h"

// コンストラクタ
CImageController::CImageController(HWND hwnd){

	m_Load = FALSE;
	m_Bmp = NULL;
	m_Color = 0x00000000;
	m_g = NULL;

	HDC hdc;
	hdc = GetDC(hwnd);
	m_hWnd = hwnd;

	/* GDI+初期化 */
	Gdiplus::GdiplusStartup(&m_lpToken, &m_pSI, NULL);

	// オフスクリーンDC(デスクトップのサイズで作成します)
	m_hDrawDC = CreateCompatibleDC(hdc);
	GetClientRect(GetDesktopWindow(), &m_DesktopRect);

	HBITMAP hDrawBmp;
	hDrawBmp = CreateCompatibleBitmap(hdc, m_DesktopRect.right, m_DesktopRect.bottom);
	SelectObject(m_hDrawDC, hDrawBmp);
	DeleteObject(hDrawBmp);

}
	
// デストラクタ
CImageController::~CImageController(void){

	Destroy();

	// オフスクリーンDC破棄
	if(m_hDrawDC){
		DeleteDC(m_hDrawDC);
		m_hDrawDC = NULL;
	}

	/* 破棄 */
	Gdiplus::GdiplusShutdown(m_lpToken);
}

BOOL CImageController::Load(const WCHAR *fileName){

	this->Destroy();

	/* ファイルパスの文字列からBitmap作成 */
	m_Bmp = NULL;
	m_Bmp = new Gdiplus::Bitmap(fileName);
	if(m_Bmp != NULL && m_Bmp->GetLastStatus() == Gdiplus::Ok){
		m_g = new Gdiplus::Graphics(m_hDrawDC);
		this->Update();
		m_Load = TRUE;
		return TRUE;
	}
	this->UnLoad();

	return FALSE;
}

// 明示的に破棄する場合
void CImageController::Destroy(void){

	if (m_Bmp != NULL && m_Bmp->GetLastStatus() == Gdiplus::Ok){
		delete(m_Bmp);
		m_Bmp = NULL;
		if (m_g != NULL){
			delete(m_g);
			m_g = NULL;
		}
	}

}

void CImageController::Update(void){
	// 右下基準で配置する（ただしマイナスになった場合は0にする）
	LONG posX = m_DesktopRect.right - m_Bmp->GetWidth();
	LONG posY = m_DesktopRect.bottom - m_Bmp->GetHeight();
	if (posX < 0)
		posX = 0;
	if (posY < 0)
		posY = 0;

	// 背景をクリア
	this->Clear();

	// オフスクリーンDCに描画
	if(Gdiplus::Ok != m_g->DrawImage(m_Bmp, posX, posY)){
		FOOTY2_PRINTF(L"CImageController::Update() GDI+描画に失敗しました");
	}
}

void CImageController::Clear(COLORREF color){
	// 背景を消す
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	
	hPen = CreatePen(PS_SOLID, 1, color);
	hOldPen = (HPEN)SelectObject(m_hDrawDC, hPen);
	hBrush = CreateSolidBrush(color);
	hOldBrush = (HBRUSH)SelectObject(m_hDrawDC, hBrush);

	if (FALSE == Rectangle(m_hDrawDC, 0, 0, m_DesktopRect.right, m_DesktopRect.bottom)){
		FOOTY2_PRINTF(L"CImageController::Clear() GDI描画に失敗しました");
	}

	SelectObject(m_hDrawDC, hOldPen);
	SelectObject(m_hDrawDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

void CImageController::Clear(void){
	// 背景を消す
	this->Clear(m_Color);
}

void CImageController::UnLoad(void){
	this->Clear();
	this->Destroy();
	m_Load = FALSE;
}

void CImageController::Draw(HDC targetHDC, int x1, int y1, int x2, int y2, int maxWidth, int maxHeight){
	// 描画サイズ
	int width = x2 - x1;
	int height = y2 - y1;

	int drawWidth =  m_DesktopRect.right  - (maxWidth - x2) - width;
	int drawHeight = m_DesktopRect.bottom - (maxHeight - y2) - height;

	if (FALSE == BitBlt(targetHDC, x1, y1, width, height, m_hDrawDC, drawWidth, drawHeight, SRCCOPY)){
		FOOTY2_PRINTF(L"CImageController::Draw() GDI描画に失敗しました");
	}
}

void CImageController::Draw(HDC targetHDC, int x1, int y1, int x2, int y2){
	this->Draw(targetHDC, x1, y1 ,x2, y2, 0, 0);
}