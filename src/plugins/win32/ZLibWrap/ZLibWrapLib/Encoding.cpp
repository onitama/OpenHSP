//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Encoding.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-15
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "Encoding.h"


CStringW ANSIToUCS2(const CStringA &strANSI, UINT uCodePage /*= CP_ACP*/)
{
    int size = MultiByteToWideChar(uCodePage, 0, strANSI, -1, NULL, 0);

    if (size == 0)
    {
        return L"";
    }

    WCHAR *szUCS2 = new WCHAR[size];

    if (MultiByteToWideChar(uCodePage, 0, strANSI, -1, szUCS2, size) == 0)
    {
        delete[] szUCS2;
        return L"";
    }

    CStringW ret = szUCS2;
    delete[] szUCS2;

    return ret;
}

CStringA UCS2ToANSI(const CStringW &strUCS2, UINT uCodePage /*= CP_ACP*/)
{
    int size = WideCharToMultiByte(uCodePage, 0, strUCS2, -1, NULL, 0, NULL, NULL);

    if (size == 0)
    {
        return "";
    }

    CHAR *szANSI = new CHAR[size];

    if (WideCharToMultiByte(uCodePage, 0, strUCS2, -1, szANSI, size, NULL, NULL) == 0)
    {
        delete[] szANSI;
        return "";
    }

    CStringA ret = szANSI;
    delete[] szANSI;

    return ret;
}
