/*------------------------------------------------------------------------------
	Tab management routines for HSP Script Editor
------------------------------------------------------------------------------*/

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "FootyDLL.h"
#include "tabmanager.h"
#include "classify.h"

#include "support.h"

// Type table
typedef struct tagTypeTable{
	char *Type;
	COLORREF *color;
	int Status;
} TYPE_TABLE;

// Color variable
MYCOLOR color;

typedef struct tagDefaultClassifyTable{
	const char *Word1;
	const char *Word2;
	int Type;
	COLORREF *color;
	int Status;
	int Level;
	int pLevel;
} DEF_CLASSIFY_TABLE;

DEF_CLASSIFY_TABLE DefClassifyTable[] = {

	// Grammer
	"\\\"",		"\\\"",	EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		NULL,	4,	0,
	"\"",		"\"",	EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		NULL,	3,	4,
	"{\"",		"\"}",	EMP_MULTI_BETWEEN,	&(color.Character.String.Conf),		NULL,	3,	4,
	":",		"",		EMP_LINE_AFTER,		&(color.Character.Default.Conf),	NULL,	2,	1,
	"/*",		"*/",	EMP_MULTI_BETWEEN,	&(color.Character.Comment.Conf),	NULL,	3,	0,
	";",		"",		EMP_LINE_AFTER,		&(color.Character.Comment.Conf),	NULL,	3,	0,
	"//",		"",		EMP_LINE_AFTER,		&(color.Character.Comment.Conf),	NULL,	3,	0,

	// Label
	"*",		"",		EMP_LINE_AFTER,		&(color.Character.Label.Conf),	F_SE_HEAD,	0,	2,

	// End of table	
	NULL
};

#define DefClassifyTableSize() ((sizeof(DefClassifyTable)/sizeof(*DefClassifyTable))-1)

// Table of type returned from hspcmp.dll 
static TYPE_TABLE TypeTable[] = {
	"sys|macro",	&(color.Character.Macro.Conf),			NULL,
	"sys|func",		&(color.Character.Function.Conf),		NULL,
	"sys|func|1",	&(color.Character.Function.Conf),		NULL,
	"sys|func|2",	&(color.Character.Function.Conf),		NULL,
	"pre|func",		&(color.Character.Preprocessor.Conf),	NULL,
	NULL
};

// Function pointers in hspcmp.dll
typedef BOOL (CALLBACK *DLLFUNC)(int,int,int,int);
extern DLLFUNC hsc_ini;
extern DLLFUNC hsc_refname;
extern DLLFUNC hsc_objname;
extern DLLFUNC hsc_comp;
extern DLLFUNC hsc_getmes;
extern DLLFUNC hsc_clrmes;
extern DLLFUNC hsc_ver;
extern DLLFUNC hsc_bye;
extern DLLFUNC pack_ini;
extern DLLFUNC pack_make;
extern DLLFUNC pack_exe;
extern DLLFUNC pack_opt;
extern DLLFUNC pack_rt;
extern DLLFUNC hsc3_getsym;
extern DLLFUNC hsc3_make;
extern DLLFUNC hsc3_messize;

extern char szTitleName[_MAX_FNAME + _MAX_EXT] ;
extern HWND hwndTab;
extern int hsp_extmacro;

char *filebuf;

FileList filelist;

static CLASSIFY_TABLE *ClassifyTable = NULL;

int TableCompare(const void *pTable1, const void *pTable2)
{
	return lstrlen(((CLASSIFY_TABLE *)pTable2)->Word1)
		- lstrlen(((CLASSIFY_TABLE *)pTable1)->Word1);
}

void InitClassify()
{
	int bufsize;
	char *buf, *line, name[256], type[256];

	int nCTSize = 0;
/*
	FILE *fp = fopen("hsptmp", "w");
	char last;
	for(int i = 0; i < filelist.num(); i++){
		fputs("#addition \"", fp);
		
		last = '\0';
		for(const char *filename = filelist.get(i); *filename != '\0'; filename++){
            if(*filename == '\\' && !_ismbblead(last))
				fputc('\\', fp);
			fputc(*filename, fp);
		}
		fputs("\"\n", fp);
	}
	fclose(fp);
*/
	hsc_ini( 0,(int)"hsptmp", 0,0 );
	hsc_refname( 0,(int)(szTitleName[0] == '\0' ? "???" : szTitleName), 0,0 );
	hsc_objname( 0,(int)"obj", 0,0 );
	//hsc_comp( 1,1,0,0 );
	hsc3_getsym(0, 0, 0, 0);
	hsc3_messize((int)&bufsize, 0, 0, 0);
	buf = (char *)malloc(bufsize+1);
	hsc_getmes((int)buf, 0, 0, 0);

	int tableCapacity = DefClassifyTableSize() + getStrLinesSize(buf) + 1;
	free(ClassifyTable);
	ClassifyTable = (CLASSIFY_TABLE *)malloc(tableCapacity * sizeof(CLASSIFY_TABLE));

	for(int i = 0; DefClassifyTable[i].Word1 != NULL; i++, nCTSize++){
		lstrcpy(ClassifyTable[i].Word1, DefClassifyTable[i].Word1);
		lstrcpy(ClassifyTable[i].Word2, DefClassifyTable[i].Word2);
		ClassifyTable[i].Type   = DefClassifyTable[i].Type;
		ClassifyTable[i].color  = DefClassifyTable[i].color;
		ClassifyTable[i].Status = DefClassifyTable[i].Status;
		ClassifyTable[i].Level  = DefClassifyTable[i].Level;
		ClassifyTable[i].pLevel = DefClassifyTable[i].pLevel;
	}
    
	line = buf;
	for(;;){
		if(sscanf(line, "%s\t,%s", name, type) == 2){
			if(nCTSize >= tableCapacity){
				msgboxf(0, "色分けの個数が制限に達しました。\n\"%s\"以降は色分けされません。"
					, "エラー", MB_OK | MB_ICONERROR, name);
				break;
			}
			for(TYPE_TABLE *lpTT = TypeTable; lpTT->Type; lpTT++){
				if(!strcmp(type, lpTT->Type)){
					lstrcpy(ClassifyTable[nCTSize].Word1, name);
					lstrcpy(ClassifyTable[nCTSize].Word2, "");
					ClassifyTable[nCTSize].Type   = EMP_WORD;
					ClassifyTable[nCTSize].color  = lpTT->color;
					ClassifyTable[nCTSize].Status = F_SE_NON_BS | F_SE_INDEPENDENCE_B | lpTT->Status;
					ClassifyTable[nCTSize].Level  = 1;
					ClassifyTable[nCTSize].pLevel = 0;
					nCTSize++;
					break;
				}
			}
		}

		while(*line != '\0' && *line != '\n') line++;
		if(*line == '\0') break;
		line++;
	}
	free(buf);

	qsort(ClassifyTable, nCTSize, sizeof(CLASSIFY_TABLE), TableCompare);
	lstrcpy(ClassifyTable[nCTSize].Word1, "");
}


void SetClassify(int FootyID)
{
	FootyResetEmphasis(FootyID, false);
	for(CLASSIFY_TABLE *lpCT = ClassifyTable; lpCT->Word1[0] != '\0'; lpCT++) {
		FootyAddEmphasis(FootyID, lpCT->Word1, *lpCT->Word2 ? lpCT->Word2 : NULL, lpCT->Type, (lpCT->color != NULL ? *(lpCT->color) : NULL),
			lpCT->Status, lpCT->Level, lpCT->pLevel);
	}	

	FootyRefresh(FootyID);
	return;
}

void SetEditColor(int FootyID)
{
	FootySetColor(FootyID, F_SC_TAB, color.NonCharacter.Tab.Conf,  false);
	FootySetColor(FootyID, F_SC_HALFSPACE, color.NonCharacter.HalfSpace.Conf,  false);
	FootySetColor(FootyID, F_SC_NORMALSPACE, color.NonCharacter.FullSpace.Conf,  false);
	FootySetColor(FootyID, F_SC_UNDERLINE, color.Edit.CaretUnderLine.Conf,  false);
	FootySetColor(FootyID, F_SC_CRLF, color.NonCharacter.NewLine.Conf,  false);
	FootySetColor(FootyID, F_SC_EOF, color.NonCharacter.EndOfFile.Conf,  false);
	FootySetColor(FootyID, F_SC_DEFAULT, color.Character.Default.Conf,  false);
	FootySetColor(FootyID, F_SC_LINENUM, color.LineNumber.Number.Conf,  false);
	FootySetColor(FootyID, F_SC_LINENUMBORDER, color.Edit.BoundaryLineNumber.Conf,  false);
	FootySetColor(FootyID, F_SC_CARETLINE, color.LineNumber.CaretLine.Conf,  false);
	FootySetColor(FootyID, F_SC_RULER_TXT, color.Ruler.Number.Conf,  false);
	FootySetColor(FootyID, F_SC_RULER_BK, color.Ruler.Background.Conf,  false);
	FootySetColor(FootyID, F_SC_RULER_LINE, color.Ruler.Division.Conf,  false);
	FootySetColor(FootyID, F_SC_CARETPOS, color.Ruler.Caret.Conf,  false);
	FootySetColor(FootyID, F_SC_BACKGROUND, color.Edit.Background.Conf,  false);
	FootyRefresh(FootyID);
}

void SetAllEditColor()
{
	int num = TabCtrl_GetItemCount(hwndTab);
	for(int i = 0; i < num; i++)
		SetEditColor(i);
	return;
}

void ResetClassify()
{
	int i;
	TABINFO *lpTabInfo;

	for(i = 0; (lpTabInfo = GetTabInfo(i)) != NULL; i++)
		SetClassify(lpTabInfo->FootyID);
	return;
}

void ByeClassify()
{
	free(ClassifyTable);
	ClassifyTable = NULL;
}
