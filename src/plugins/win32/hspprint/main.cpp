
//
//		HSP3.x plugin source
//		onion software/onitama 2010/4
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "../hpi3sample/hsp3plugin.h"

#include "DevPrinter.h"

/*----------------------------------------------------------------*/


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
	}
	return TRUE ;
}

 /*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI prnflags( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	プリンタを列挙するためのフラグを設定する
	//		prnflags flags
	//		
	//		  PRINTER_ENUM_DEFAULT = 1
	//		  PRINTER_ENUM_LOCAL = 2
	//		  PRINTER_ENUM_REMOTE = $10
	//		  PRINTER_ENUM_SHARED = $20
	//		  PRINTER_ENUM_NETWORK = $40
	//
	int ep1;
	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値
	DevSetPrinterFlags( ep1 );
	return 0;
}


EXPORT BOOL WINAPI enumprn( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	プリンタを列挙する
	//		enumprn 変数
	//
	char *ss;
	PVal *pv;
	APTR ap;
	int sz;
	int num;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数

	sz = DevGetEnumPrinterName( NULL );
	ss = (char *)malloc( sz+1 );
	DevGetEnumPrinterName( ss );

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	free( ss );

	num = DevGetNumPrinter();
	return -num;
}


EXPORT BOOL WINAPI getdefprn( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	規定のプリンタを取得する
	//		getdefprn 変数
	//
	char ss[512];
	PVal *pv;
	APTR ap;

	*ss = 0;
	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	DevGetDefaultPrinter( ss );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI propprn( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	プリンタ情報の取得
	//		propprn 変数,変数2,id,type
	//
	PVal *pv;
	APTR ap;
	PVal *pv2;
	APTR ap2;
	int ep1,ep2,x,y;
	DEVPRINTERDOC doc;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	ap2 = hei->HspFunc_prm_getva( &pv2 );		// パラメータ2:変数
	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ3:数値
	ep2 = hei->HspFunc_prm_getdi(0);			// パラメータ3:数値

	if ( DevGetPrinterInfo( &doc, ep1 ) == false ) return -1;

	x = y = 0;
	switch( ep2 ) {
	case 0:
		x = doc.HorzRes;
		y = doc.VertRes;
		break;
	case 1:
		x = doc.PhysicalWidth;
		y = doc.PhysicalHeight;
		break;
	case 2:
		x = doc.PhysicalOffsetX;
		y = doc.PhysicalOffsetY;
		break;
	case 3:
		x = doc.HorzSize;
		y = doc.VertSize;
		break;
	}

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &x );		// 変数に値を代入
	hei->HspFunc_prm_setva( pv2, ap2, HSPVAR_FLAG_INT, &y );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI execprn( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	プリンタ送信
	//		execprn プリンタID,x,y,sx,sy,bx,by,bsx,bsy,"name"
	//
	int ep1;
	int x,y,sx,sy,bx,by,bsx,bsy;
	char *docname;
	BMSCR *bm;

	bm = (BMSCR *)hei->HspFunc_getbmscr( *(hei->actscr) );

	ep1 = hei->HspFunc_prm_getdi(0);	// パラメータ1:数値
	x = hei->HspFunc_prm_getdi(0);
	y = hei->HspFunc_prm_getdi(0);
	sx = hei->HspFunc_prm_getdi( bm->sx );
	sy = hei->HspFunc_prm_getdi( bm->sy );
	bx = hei->HspFunc_prm_getdi(0);
	by = hei->HspFunc_prm_getdi(0);
	bsx = hei->HspFunc_prm_getdi( bm->sx );
	bsy = hei->HspFunc_prm_getdi( bm->sy );
	docname = hei->HspFunc_prm_getds( "hspdoc" );

	DevPrintBitmapPrinter( ep1, docname, x, y, sx, sy, (BITMAPINFO *)bm->pbi, bm->pBit, bx, by, bsx, bsy );

	return 0;
}


EXPORT BOOL WINAPI prndialog( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	プリンタの設定ダイアログを開く
	//		prndialog no
	//
	int ep1;
	DEVPRINTERDOC doc;
	ep1 = hei->HspFunc_prm_getdi(0);			// パラメータ1:数値
	DevGetPrinterProperty( &doc, ep1 );
	return 0;
}


/*------------------------------------------------------------------------------------*/
