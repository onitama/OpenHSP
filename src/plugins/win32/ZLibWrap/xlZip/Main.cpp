//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Main.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include "../ZLibWrapLib/ZLibWrapLib.h"

void ShowBanner()
{
    _tprintf(_T("xlZip By Streamlet\n"));
    _tprintf(_T("\n"));
}

void ShowHelp()
{
    _tprintf(_T("Usage:\n"));
    _tprintf(_T("    xlZip /z <SourceFiles> <ZipFile> [/utf8]\n"));
    _tprintf(_T("    xlZip /u <ZipFile> <DestFolder>\n"));
}

int _tmain(int argc, TCHAR *argv[])
{
    setlocale(LC_ALL, "");

    ShowBanner();

    if (argc < 4)
    {
        ShowHelp();
        return 0;
    }

    enum
    {
        ZIP,
        UNZIP

    } TODO;

    bool bUtf8 = false;

    if (_tcsicmp(argv[1], _T("/z")) == 0)
    {
        TODO = ZIP;

        if (argc >= 5 && _tcsicmp(argv[4], _T("/utf8")) == 0)
        {
            bUtf8 = true;
        }
    }
    else if (_tcsicmp(argv[1], _T("/u")) == 0)
    {
        TODO = UNZIP;
    }
    else
    {
        ShowHelp();
        return 0;
    }

    switch (TODO)
    {
    case ZIP:
        if (ZipCompress(argv[2], argv[3], bUtf8))
        {
            _tprintf(_T("Compressed %s to %s successfully.\n"), argv[2], argv[3]);
        }
        else
        {
            _tprintf(_T("Failed to compress %s to %s.\n"), argv[2], argv[3]);
        }
        break;
    case UNZIP:
        if (ZipExtract(argv[2], argv[3]))
        {
            _tprintf(_T("Extracted %s to %s successfully.\n"), argv[2], argv[3]);
        }
        else
        {
            _tprintf(_T("Failed to Extract %s to %s.\n"), argv[2], argv[3]);
        }
        break;
    default:
        break;
    }

    return 0;
}




