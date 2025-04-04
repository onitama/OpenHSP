/*------------------------------------------------------------------------------
	Header of tab management routines
------------------------------------------------------------------------------*/

#ifndef __tabmanager_h
#define __tabmanager_h

// Tab Information structure
typedef struct tagTabInfo {
	char FileName[_MAX_PATH];
	char TitleName[_MAX_FNAME + _MAX_EXT];
    char DirName[_MAX_PATH];
	ULONGLONG FileIndex;
	int LatestUndoNum;
	BOOL NeedSave;
	int FootyID;
	struct {
		struct tagTabInfo *prev;
		struct tagTabInfo *next;
	} ZOrder;
} TABINFO;

// Functions
void CreateTab(int, const char *, const char *, const char *);
void DeleteTab(int);
TABINFO *GetTabInfo(int);
int SearchTab(const char *, const char *, const char *, ULONGLONG);
void SetTabInfo(int, const char *, const char *, const char *, BOOL);
void ActivateTab(int, int);
void ChangeZOrder(int, int);
int GetTabID(int);

#endif // #ifndef __tabmanager_h