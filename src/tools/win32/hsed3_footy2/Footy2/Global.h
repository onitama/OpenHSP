/**
 * @brief Footy2全体から利用されるヘッダ
 * @note 外部公開用ではなくて、プリコンパイル用です。
 * @author Shinji Watanabe
 * @date Oct.26.2008
 */

#pragma once

//#if defined( _DEBUG ) && !defined ( UNDER_CE )/*Tp*/
#	define PRINT_ENABLED
//#endif/*Tp*/

// 標準ヘッダファイル
#include <windows.h>
#include <math.h>
#include <vector>
#include <list>
#include <vector>
#include <string>
#include <wchar.h>

#ifndef WM_MOUSEWHEEL
#	include <zmouse.h>						/*マウスホイールメッセージ*/
#endif	/* WM_MOUSEWHEEL */

#ifdef PRINT_ENABLED
#	include <assert.h>
#	include <stdarg.h>
#endif	/* PRINT_ENABLED */

// 内部ヘッダファイル
#include "Footy2.h"
#include "Footy.h"

// 便利なマクロ
#define WIDEN_REAL(x) L ## x
#define WIDEN(x) WIDEN_REAL(x)

// 環境ごとの定義
#ifdef UNDER_CE
#	define CSM_DEFAULT CSM_SHIFT_JIS_2004
#	define FOOTY2STRCPY(dest,src,size)	strncpy(dest, src, size)
#	define FOOTY2SPRINTF _snwprintf
#else	/*UNDER_CE*/
#	define CSM_DEFAULT	CSM_PLATFORM
#	define FOOTY2STRCPY(dest,src,size)	strcpy_s(dest, size, src)
#	define FOOTY2SPRINTF swprintf_s
#endif	/*UNDER_CE*/

// プリントなど
#ifdef PRINT_ENABLED
#define		FOOTY2_ASSERT( cond )		assert( cond );
#define		FOOTY2_PRINTF( fmt, ... )	FormatPrint( WIDEN( __FILE__ ) , __LINE__, fmt, __VA_ARGS__ );
#else	/*_DEBUG*/
#define		FOOTY2_ASSERT( cond )		( void ) 0;
#define		FOOTY2_PRINTF( fmt, ... )	( void ) 0;
#endif	/*_DEBUG*/

// 省略形
#define SAFE_DELETE(p)			if (p) {delete [] (p); (p) = NULL;}
#define SAFE_FREE(p)			if (p) {free((p)); (p) = NULL;}
#define SAFE_DELETEOBJECT(p)	if (p) {DeleteObject(p); (p) = NULL;}
#define	forever					for(;;)

// 別名宣言
typedef std::list<CFooty*>::iterator FootyPt;

// 関数のプロトタイプ宣言
CFooty *GetFooty( int nID );
void FormatPrint( const wchar_t* filename, int nLine, const wchar_t* fmt, ... );

/*[EOF]*/
