/*------------------------------------------------------------------------------
	Header of HSED support routines
------------------------------------------------------------------------------*/

#ifndef __support_h
#define __support_h

int __cdecl msgboxf(HWND, LPCTSTR, LPCTSTR, UINT, ...);
void ShowLastError();
LPSTR *CommandLineToArgvA(LPSTR, int *);
size_t strlcpy(char *, char *, size_t);
ULONGLONG GetFileIndex(const char *);
int getStrLinesSize(const char *);
void GetDirName(char *dirName, const char *path);

class FileList{
private:
	char *buf, *buf2;
	char **index;
	int bufsize, idxsize, offset, idxoffset;

public:
	FileList();
	~FileList();
	void reset();
	void add(const char *);
	const char *get(int);
	void setlist(const char *);
	const char *getlist();
	size_t num();
};

#endif // #ifndef __support_h