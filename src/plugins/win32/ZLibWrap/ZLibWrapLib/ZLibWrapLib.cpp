//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   ZLibWrapLib.cpp
//    Author:      Streamlet
//    Create Time: 2010-09-16
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "stdafx.h"
#include "ZLibWrapLib.h"
#include "Encoding.h"
#include <loki/ScopeGuard.h>
#include <minizip/zip.h>
#include <minizip/unzip.h>
#include <atlstr.h>

#define ZIP_GPBF_LANGUAGE_ENCODING_FLAG 0x800

static 

BOOL ZipAddFile(zipFile zf, LPCTSTR lpszFileNameInZip, LPCTSTR lpszFilePath, bool bUtf8 = false)
{
    DWORD dwFileAttr = GetFileAttributes(lpszFilePath);

    if (dwFileAttr == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    DWORD dwOpenAttr = (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0 ? FILE_FLAG_BACKUP_SEMANTICS : 0;
    HANDLE hFile = CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwOpenAttr, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);
    
    FILETIME ftUTC, ftLocal;

    GetFileTime(hFile, NULL, NULL, &ftUTC);
    FileTimeToLocalFileTime(&ftUTC, &ftLocal);

    WORD wDate, wTime;
    FileTimeToDosDateTime(&ftLocal, &wDate, &wTime);

    zip_fileinfo FileInfo;
    ZeroMemory(&FileInfo, sizeof(FileInfo));

    FileInfo.dosDate = ((((DWORD)wDate) << 16) | (DWORD)wTime);
    FileInfo.external_fa |= dwFileAttr;

    if (bUtf8)
    {
        CStringA strFileNameInZipA = UCS2ToANSI(lpszFileNameInZip, CP_UTF8);

        if (zipOpenNewFileInZip4(zf, strFileNameInZipA, &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9,
                                 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 0, ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != ZIP_OK)
        {
            return FALSE;
        }
    }
    else
    {
        CStringA strFileNameInZipA = UCS2ToANSI(lpszFileNameInZip);

        if (zipOpenNewFileInZip(zf, strFileNameInZipA, &FileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9) != ZIP_OK)
        {
            return FALSE;
        }
    }

    LOKI_ON_BLOCK_EXIT(zipCloseFileInZip, zf);

    if ((dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) != 0)
    {
        return TRUE;
    }

    const DWORD BUFFER_SIZE = 4096;
    BYTE byBuffer[BUFFER_SIZE];

    LARGE_INTEGER li = {};

    if (!GetFileSizeEx(hFile, &li))
    {
        return FALSE;
    }

    while (li.QuadPart > 0)
    {
        DWORD dwSizeToRead = li.QuadPart > (LONGLONG)BUFFER_SIZE ? BUFFER_SIZE : (DWORD)li.LowPart;
        DWORD dwRead = 0;

        if (!ReadFile(hFile, byBuffer, dwSizeToRead, &dwRead, NULL))
        {
            return FALSE;
        }

        if (zipWriteInFileInZip(zf, byBuffer, dwRead) < 0)
        {
            return FALSE;
        }

        li.QuadPart -= (LONGLONG)dwRead;
    }

    return TRUE;
}

BOOL ZipAddFiles(zipFile zf, LPCTSTR lpszFileNameInZip, LPCTSTR lpszFiles, bool bUtf8 = false)
{
    WIN32_FIND_DATA wfd;
    ZeroMemory(&wfd, sizeof(wfd));

    HANDLE hFind = FindFirstFile(lpszFiles, &wfd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(FindClose, hFind);

    CString strFilePath = lpszFiles;
    int nPos = strFilePath.ReverseFind('\\');

    if (nPos != -1)
    {
        strFilePath = strFilePath.Left(nPos + 1);
    }
    else
    {
        strFilePath.Empty();
    }

    CString strFileNameInZip = lpszFileNameInZip;
    
    do 
    {
        CString strFileName = wfd.cFileName;

        if (strFileName == _T(".") || strFileName == _T(".."))
        {
            continue;
        }

        if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            if (!ZipAddFile(zf, strFileNameInZip + strFileName + _T("/"), strFilePath + strFileName, bUtf8))
            {
                return FALSE;
            }

            if (!ZipAddFiles(zf, strFileNameInZip + strFileName + _T("/"), strFilePath + strFileName + _T("\\*"), bUtf8))
            {
                return FALSE;
            }
        }
        else
        {
            if (!ZipAddFile(zf, strFileNameInZip + strFileName, strFilePath + strFileName, bUtf8))
            {
                return FALSE;
            }
        }

    } while (FindNextFile(hFind, &wfd));

    return TRUE;
}

BOOL ZipCompress(LPCTSTR lpszSourceFiles, LPCTSTR lpszDestFile, bool bUtf8 /*= false*/)
{
    CStringA strDestFile = UCS2ToANSI(lpszDestFile);

    zipFile zf = zipOpen64(strDestFile, 0);

    if (zf == NULL)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(zipClose, zf, (const char *)NULL);

    if (!ZipAddFiles(zf, _T(""), lpszSourceFiles, bUtf8))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ZipExtractCurrentFile(unzFile uf, LPCTSTR lpszDestFolder)
{
    char szFilePathA[MAX_PATH];
    unz_file_info64 FileInfo;

    if (unzGetCurrentFileInfo64(uf, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK)
    {
        return FALSE;
    }

    if (unzOpenCurrentFile(uf) != UNZ_OK)
    {
        return FALSE;
    }
    
    LOKI_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);

    CString strDestPath = lpszDestFolder;
    CString strFileName;

    if ((FileInfo.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != 0)
    {
        strFileName = ANSIToUCS2(szFilePathA, CP_UTF8);
    }
    else
    {
        strFileName = ANSIToUCS2(szFilePathA);
    }

    int nLength = strFileName.GetLength();
    LPTSTR lpszFileName = strFileName.GetBuffer();
    LPTSTR lpszCurrentFile = lpszFileName;
    LOKI_ON_BLOCK_EXIT_OBJ(strFileName, &CString::ReleaseBuffer, -1);

    for (int i = 0; i <= nLength; ++i)
    {
        if (lpszFileName[i] == _T('\0'))
        {
            strDestPath += lpszCurrentFile;
            break;
        }

        if (lpszFileName[i] == '\\' || lpszFileName[i] == '/')
        {
            lpszFileName[i] = '\0';

            strDestPath += lpszCurrentFile;
            strDestPath += _T("\\");

            CreateDirectory(strDestPath, NULL);
            
            lpszCurrentFile = lpszFileName + i + 1;
        }
    }

    if (lpszCurrentFile[0] == _T('\0'))
    {
        return TRUE;
    }

    HANDLE hFile = CreateFile(strDestPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
         return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(CloseHandle, hFile);
    
    const DWORD BUFFER_SIZE = 4096;
    BYTE byBuffer[BUFFER_SIZE];

    while (true)
    {
        int nSize = unzReadCurrentFile(uf, byBuffer, BUFFER_SIZE);

        if (nSize < 0)
        {
            return FALSE;
        }
        else if (nSize == 0)
        {
            break;
        }
        else
        {
            DWORD dwWritten = 0;

            if (!WriteFile(hFile, byBuffer, (DWORD)nSize, &dwWritten, NULL) || dwWritten != (DWORD)nSize)
            {
                return FALSE;
            }
        }
    }

    FILETIME ftLocal, ftUTC;

    DosDateTimeToFileTime((WORD)(FileInfo.dosDate>>16), (WORD)FileInfo.dosDate, &ftLocal);
    LocalFileTimeToFileTime(&ftLocal, &ftUTC);
    SetFileTime(hFile, &ftUTC, &ftUTC, &ftUTC);
    
    return TRUE;
}

BOOL ZipExtract(LPCTSTR lpszSourceFile, LPCTSTR lpszDestFolder)
{
    CStringA strSourceFileA = UCS2ToANSI(lpszSourceFile);

    unzFile uf = unzOpen64(strSourceFileA);

    if (uf == NULL)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(unzClose, uf);

    unz_global_info64 gi;

    if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
    {
        return FALSE;
    }

    CString strDestFolder = lpszDestFolder;

    CreateDirectory(lpszDestFolder, NULL);

    if (!strDestFolder.IsEmpty() && strDestFolder[strDestFolder.GetLength() - 1] != _T('\\'))
    {
        strDestFolder += _T("\\");
    }
    
    for (int i = 0; i < gi.number_entry; ++i)
    {
        if (!ZipExtractCurrentFile(uf, strDestFolder))
        {
            return FALSE;
        }
        
        if (i < gi.number_entry - 1)
        {
            if (unzGoToNextFile(uf) != UNZ_OK)
            {
                return FALSE;
            }
        }
    }
  
    return TRUE;
}

BOOL ZipExtractCurrentFileList(unzFile uf, HANDLE hFile)
{
    char szFilePathA[MAX_PATH];
    unz_file_info64 FileInfo;

    if (unzGetCurrentFileInfo64(uf, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0) != UNZ_OK)
    {
        return FALSE;
    }

    if (unzOpenCurrentFile(uf) != UNZ_OK)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);

    CString strDestPath;
    CString strFileName;

    if ((FileInfo.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != 0)
    {
        strFileName = ANSIToUCS2(szFilePathA, CP_UTF8);
    }
    else
    {
        strFileName = ANSIToUCS2(szFilePathA);
    }

    int nLength = strFileName.GetLength();
    LPTSTR lpszFileName = strFileName.GetBuffer();
    LPTSTR lpszCurrentFile = lpszFileName;
    LOKI_ON_BLOCK_EXIT_OBJ(strFileName, &CString::ReleaseBuffer, -1);

    for (int i = 0; i <= nLength; ++i)
    {
        if (lpszFileName[i] == _T('\0'))
        {
            strDestPath += lpszCurrentFile;
            break;
        }

        if (lpszFileName[i] == '\\' || lpszFileName[i] == '/')
        {
            lpszFileName[i] = '\0';

            strDestPath += lpszCurrentFile;
            strDestPath += _T("\\");

            //CreateDirectory(strDestPath, NULL);

            lpszCurrentFile = lpszFileName + i + 1;
        }
    }

    if (lpszCurrentFile[0] == _T('\0'))
    {
        return TRUE;
    }

    DWORD dwWritten = 0;
    int nSize = strlen(szFilePathA);

    if (!WriteFile(hFile, szFilePathA, (DWORD)nSize, &dwWritten, NULL) || dwWritten != (DWORD)nSize)
    {
        return FALSE;
    }

    char crcode[4];
    crcode[0] = 13;
    crcode[1] = 10;
    crcode[2] = 0;
    nSize = 2;

    if (!WriteFile(hFile, crcode, (DWORD)nSize, &dwWritten, NULL) || dwWritten != (DWORD)nSize)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL ZipExtractList(LPCTSTR lpszSourceFile, LPCTSTR lpszDestFile)
{
    CStringA strSourceFileA = UCS2ToANSI(lpszSourceFile);

    unzFile uf = unzOpen64(strSourceFileA);

    if (uf == NULL)
    {
        return FALSE;
    }

    LOKI_ON_BLOCK_EXIT(unzClose, uf);

    unz_global_info64 gi;

    if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK)
    {
        return FALSE;
    }

    CString strDestFolder = "";
    CString strDestFile = lpszDestFile;

    HANDLE hFile = CreateFile(strDestFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    if (!strDestFolder.IsEmpty() && strDestFolder[strDestFolder.GetLength() - 1] != _T('\\'))
    {
        strDestFolder += _T("\\");
    }

    for (int i = 0; i < gi.number_entry; ++i)
    {
        if (!ZipExtractCurrentFileList(uf, hFile))
        {
            return FALSE;
        }

        if (i < gi.number_entry - 1)
        {
            if (unzGoToNextFile(uf) != UNZ_OK)
            {
                return FALSE;
            }
        }
    }

    CloseHandle( hFile );
    return TRUE;
}
