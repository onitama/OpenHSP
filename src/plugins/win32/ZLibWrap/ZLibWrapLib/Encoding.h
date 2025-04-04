//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Encoding.h
//    Author:      Streamlet
//    Create Time: 2010-09-15
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __ENCODING_H_B23F48D1_9E5C_435B_9BDA_B6370F9BAD62_INCLUDED__
#define __ENCODING_H_B23F48D1_9E5C_435B_9BDA_B6370F9BAD62_INCLUDED__


#include <Windows.h>
#include <atlstr.h>

CStringW ANSIToUCS2(const CStringA &strANSI, UINT uCodePage = CP_ACP);
CStringA UCS2ToANSI(const CStringW &strUCS2, UINT uCodePage = CP_ACP);


#endif // #ifndef __ENCODING_H_B23F48D1_9E5C_435B_9BDA_B6370F9BAD62_INCLUDED__
