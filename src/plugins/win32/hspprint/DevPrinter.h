/**
 *	@brief
 *	@file	DevPrinter.h
 *	@date
 *	@author	
 */
#ifndef	_DEVPRINTER_H_
#define	_DEVPRINTER_H_
#ifdef	WIN32

/**
 *	@brief	プリンタドキュメント構造体
 */
typedef struct _DEVPRINTERDOC{
	void*	pDevMode;
	HDC		hDC;
	int		HorzRes;			//	ピクセル単位の画面の幅
	int		VertRes;			//	ピクセル単位の高さ
	int		HorzSize;			//	mm単位の幅
	int		VertSize;			//	mm単位の高さ
	int		PhysicalWidth;		//	印刷デバイス用です。ページの物理的な幅
	int		PhysicalHeight;		//	印刷デバイス用です。ページの物理的な高さ
	int		PhysicalOffsetX;	//	印刷デバイス用です。物理的なページの左端から印刷可能領域の左端までの距離をデバイス単位
	int		PhysicalOffsetY;	//	印刷デバイス用です。物理的なページの上端から印刷可能領域の上端までの距離をデバイス単位
}DEVPRINTERDOC, *LPDEVPRINTERDOC;

extern int DevGetNumPrinter(void);
extern bool DevGetEnumPrinterNameByNo( int No, char* pName );
extern int DevGetEnumPrinterName( char* pName );
extern int DevGetEnumPrinterDriverName( char* pName );
extern bool DevGetPrinterInfo( DEVPRINTERDOC* pPrinterDoc, const int PrinterNo );
extern bool DevStartDocPrinter( DEVPRINTERDOC* pPrinterDoc, const int PrinterNo, const char* pDocName );
extern bool DevStartPagePrinter( DEVPRINTERDOC* pPrinterDoc );	
extern bool DevEndPagePrinter( DEVPRINTERDOC* pPrinterDoc );
extern bool DevEndDocPrinter( DEVPRINTERDOC* pPrinterDoc );
extern void DevAbortDocPrinter( DEVPRINTERDOC* pPrinterDoc );
extern bool DevPrintBitmapPrinter( const int PrinterNo, const char* pDocName, int DstX, int DstY, int DstW, int DstH, const BITMAPINFO* pBitmapInfo, const void* pBits, int SrcX, int SrcY, int SrcW, int SrcH );
extern void DevSetPrinterFlags(int printe_prm);
extern bool DevGetDefaultPrinter( char *pName );
extern bool DevGetPrinterProperty( DEVPRINTERDOC* pPrinterDoc, const int PrinterNo );


#endif	//	WIN32
#endif
