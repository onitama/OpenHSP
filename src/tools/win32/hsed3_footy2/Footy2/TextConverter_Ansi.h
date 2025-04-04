/*================================================================
ANSIに関連する文字コード変換クラス
WindowsのAPIを使用する
================================================================*/

#pragma once

#include "TextConverter.h"

#ifndef UNDER_CE

class CTextConverter_ANSI : public CTextConverter{
public:
	bool ToUnicode(const char* pRawText,size_t nSize);
	bool ToMulti(const wchar_t* pMulti,size_t nSize);
};

#endif	/*not defined UNDER_CE*/

/*[EOF]*/
