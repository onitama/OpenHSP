
//
//	HSP3 COM support
//	onion software/onitama 2005/4
//

#include "../hsp3/hsp3config.h"

#ifdef HSPWIN
#define USE_WINDOWS_COM		// WindowsCOM APIを使用する
#endif

#ifdef USE_WINDOWS_COM
#include <windows.h>
#include <ocidl.h>
#include <objbase.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comutil.h"

#ifdef USE_WINDOWS_COM
/*------------------------------------------------------------*/
/*
		Windowsの場合は以下を使用してください。
*/
/*------------------------------------------------------------*/

static void cnvwstr( void *out, char *in, int bufsize )
{
	//	sjis->unicode に変換
	//
	MultiByteToWideChar( CP_ACP, 0, in, -1, (LPWSTR)out, bufsize );
}

static int GetIIDFromString( IID *iid, char *ps, bool fClsid = false )
{
	//		SJIS文字列 IID から IID 構造体を得る
	//
	//		fClsid が真のとき、ProgID からの CLSID 取得を試みる
	//		ProgID の変換に失敗した場合は 1 を返す
	//		(それ以外のエラーは例外を throw)
	//
	char stmp[1024];
	HRESULT hr;
	cnvwstr( stmp, ps, 1024 );
	if ( *ps == '{' ) {
		// GUID 文字列 → GUID 構造体
		hr = IIDFromString( (LPOLESTR)stmp, iid );
		if ( SUCCEEDED(hr) ) return 0;
	}
	if ( fClsid ) {
		// ProgID 文字列 → GUID 構造体
		hr = CLSIDFromProgID( (LPOLESTR)stmp, iid );
		if ( SUCCEEDED(hr) ) return 0;
		return 1;
	}
	return -1;
}

int ConvertIID( COM_GUID *guid, char *name )
{
	return GetIIDFromString( (IID *)guid, name );
}

#else

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int ConvertIID( COM_GUID *guid, char *name )
{
	//		SJIS文字列 IID から IID 構造体を得る
	//		(COMサポート場合は変換が必要)
	return 0;
}

#endif
