/*------------------------------------------------------------------------------
	Header for executing extension tool routines
------------------------------------------------------------------------------*/

#ifndef __exttool_h
#define __exttool_h

#include <stdlib.h>

#define SIZE_OF_TOOLNAME 256
#define SIZE_OF_FILENAME (_MAX_PATH + 1)
#define SIZE_OF_CMDLINE  1025
#define SIZE_OF_WORKDIR  (_MAX_PATH + 1)

typedef struct tagExtensionToolInformation{
	bool Used;

	char ToolName[SIZE_OF_TOOLNAME];
	char FileName[SIZE_OF_FILENAME];
	char CmdLine[SIZE_OF_CMDLINE];
	char WorkDir[SIZE_OF_WORKDIR];
	bool ShowOnMainMenu;
	bool ShowOnPopupMenu;
	bool ExecOnStartup;
	bool ExecWithOverwrite;
} EXTTOOLINFO;

bool InitExtTool(int nNewSize);
void ReleaseExtTool();
void SetMenuExtTool();
bool AddExtTool(unsigned int nToolID, EXTTOOLINFO *lpAddExtToolInfo);
bool ExecExtTool(HWND hWnd, unsigned int nToolID, bool bStartup);
void ExecStartupTools(HWND hWnd);
int GetExtToolSize();
EXTTOOLINFO *GetExtTool(unsigned int nToolID);

#endif