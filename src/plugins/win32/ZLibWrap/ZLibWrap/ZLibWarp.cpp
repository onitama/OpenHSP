//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   ZLibWarp.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-22
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "ZLibWrap.h"
#include "../ZLibWrapLib/ZLibWrapLib.h"

BOOL ZWZipCompress(LPCTSTR lpszSourceFiles, LPCTSTR lpszDestFile, bool bUtf8 /*= false*/)
{
    return ZipCompress(lpszSourceFiles, lpszDestFile, bUtf8);
}

BOOL ZWZipExtract(LPCTSTR lpszSourceFile, LPCTSTR lpszDestFolder)
{
    return ZipExtract(lpszSourceFile, lpszDestFolder);
}

BOOL ZWZipExtractList(LPCTSTR lpszSourceFile, LPCTSTR lpszDestFile)
{
    return ZipExtractList(lpszSourceFile, lpszDestFile);
}

