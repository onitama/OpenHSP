/*------------------------------------------------------------------------------
	Suport routines for HSP Script Editor
------------------------------------------------------------------------------*/

#include <windows.h>
#include <stdarg.h>
#include <stdlib.h>

#include "support.h"

#define BUF_BLOCK_SIZE (_MAX_PATH + 1) * 16
#define IDX_BLOCK_SIZE 16

int __cdecl msgboxf(HWND hWnd, LPCTSTR lpFormat, LPCTSTR lpCaption, UINT uType, ...)
{
	va_list al;
	char szBuf[1280];

	va_start(al, uType);
	wvsprintf(szBuf, lpFormat, al);
	va_end(al);

	return MessageBox(hWnd, szBuf, lpCaption, uType);
}

LPSTR *CommandLineToArgvA(LPSTR CmdLine, int *_argc)
{
	LPSTR *argv, _argv;
	unsigned long len, argc, i, j;
	char tmp;
	BOOL in_QM, in_TEXT, in_SPACE;

	len = lstrlen(CmdLine);
	i = (((len+2)/2)*sizeof(LPSTR) + sizeof(LPSTR));

	argv = (LPSTR *)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(char));
	_argv = (LPSTR)(((PUCHAR)argv)+i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while( tmp = CmdLine[i] ) {
		if(in_QM) {
			if(tmp == '\"') in_QM = FALSE;
			else {
				_argv[j] = tmp;
				j++;
			}
		} else {
			switch(tmp) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if(in_SPACE) {
						argv[argc] = _argv+j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if(in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if(in_SPACE) {
						argv[argc] = _argv+j;
						argc++;
					}
					_argv[j] = tmp;
					j++;
					in_SPACE = FALSE;
					break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	*_argc = argc;
	return argv;
}

size_t strlcpy(char *dest, char *source, size_t size)
{
	size_t i;

	if(size <= 0) return 0;
	size--;
	for(i = 0; source[i] != '\0' && i < size; i++)
		dest[i] = source[i];
	dest[i] = '\0';
	return i;
}

FileList::FileList()
{
	bufsize = BUF_BLOCK_SIZE;
	idxsize = IDX_BLOCK_SIZE;
	buf = (char *)malloc(bufsize);
	buf2 = NULL;
	index = (char **)malloc(idxsize * sizeof(char **));
	offset = idxoffset = 0;
}

FileList::~FileList()
{
	free(buf);
	free(buf2);
	free(index);
}

void FileList::add(const char *filename)
{
	int length = lstrlen(filename) + 1;
	if(offset + length >= bufsize)
		buf = (char *)realloc(buf, (bufsize += BUF_BLOCK_SIZE));
	if(idxoffset >= idxsize)
		index = (char **)realloc(index, ((idxsize += IDX_BLOCK_SIZE) * sizeof(char **)));

	index[idxoffset] = buf + offset;
	lstrcpy(index[idxoffset], filename);

	offset += length;
	idxoffset++;
	return;
}

void FileList::reset()
{
	offset = idxoffset = 0;
}

const char *FileList::get(int offset)
{
	return index[offset];
}

size_t FileList::num()
{
	return idxoffset;
}

void FileList::setlist(const char *filelist)
{
	int length = lstrlen(filelist), num = 1;
	const char *pfilelist;
	offset = idxoffset = 0;

	if(bufsize < length)
		buf = (char *)realloc(buf, (bufsize = length));
	lstrcpy(buf, filelist);


	for(pfilelist = filelist; *pfilelist != '\0'; pfilelist++)
		if(*pfilelist == '*')
			num++;

	if(num > idxsize)
		index = (char **)realloc(index, (idxsize = num));

	pfilelist = strtok(buf, "*");
	if(lstrlen(pfilelist) > 0)
		index[idxoffset++] = (char *)pfilelist;
	
	while((pfilelist = strtok(NULL, "*")) != NULL)
		if(lstrlen(pfilelist) > 0)
            index[idxoffset++] = (char *)pfilelist;

	return;
}

const char *FileList::getlist()
{
	char *pbuf, *pbuf2;

	pbuf2 = buf2 = (char *)realloc(buf2, bufsize);
	for(int i = 0; i < (int)num(); i++){
		pbuf = index[i];
		while(*pbuf != '\0')
			*pbuf2++ = *pbuf++;
		*pbuf2++ = '*';
	}
	if(num() > 0)
		pbuf2[-1] = '\0';
	else
		pbuf2[0] = '\0';
	return buf2;
}

void ShowLastError()
{
	char s[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, s, sizeof(s), NULL);
	MessageBox(0, s, "Error", MB_OK);
	return;
}

ULONGLONG GetFileIndex(const char *szFileName)
{
	BY_HANDLE_FILE_INFORMATION FileInformation;
	ULARGE_INTEGER uliFileIndex;
	HANDLE hFile;

	hFile = CreateFile(szFileName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != NULL){
		GetFileInformationByHandle(hFile, &FileInformation);
		CloseHandle(hFile);

		uliFileIndex.HighPart = FileInformation.nFileIndexHigh;
		uliFileIndex.LowPart  = FileInformation.nFileIndexLow;
		return uliFileIndex.QuadPart;
	} else {
		return 0;
	}
}

int getStrLinesSize(const char *s)
{
	int result = 0;
	while (*s != '\0') {
		while (*s != '\0') {
			char c = *s++;
			if(c == '\n') break;
		}
		result ++;
	}
	return result;
}

void GetDirName(char *dirName, const char *path)
{
	int i = 0;
	int pos = -1;
	while (1) {
		char c = path[i];
		if (c=='\0') break;
		if (c=='\\' || c=='/') pos = i;
		i += IsDBCSLeadByte(c) ? 2 : 1;
	}
	memcpy(dirName, path, pos + 1);
	dirName[pos + 1] = '\0';
}
