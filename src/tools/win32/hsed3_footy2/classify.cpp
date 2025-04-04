/*------------------------------------------------------------------------------
	Tab management routines for HSP Script Editor
------------------------------------------------------------------------------*/

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "Footy2.h"
#include "tabmanager.h"
#include "classify.h"

#include "support.h"

// 色定義追加用 by inovia
#include "CustomColorKeywords.h"


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
	int Ind;
} DEF_CLASSIFY_TABLE;

DEF_CLASSIFY_TABLE DefClassifyTable[] = {

	// Grammer
	"\\\\",		"",		EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		0,	4,	PERMIT_LEVEL(1), EMP_IND_ALLOW_ALL,
	"\\\"",		"",		EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		0,	4,	PERMIT_LEVEL(1), EMP_IND_ALLOW_ALL,
	"\"",		"\"",	EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		0,	1,	PERMIT_LEVEL(0), EMP_IND_ALLOW_ALL,
	"'",		"'",	EMP_LINE_BETWEEN,	&(color.Character.String.Conf),		0,	1,	PERMIT_LEVEL(0), EMP_IND_ALLOW_ALL, // ※TODO:新たな色種別を追加する
	"{\"",		"\"}",	EMP_MULTI_BETWEEN,	&(color.Character.String.Conf),		0,	1,	PERMIT_LEVEL(0), EMP_IND_ALLOW_ALL,
//	":",		"",		EMP_LINE_AFTER,		&(color.Character.Default.Conf),	0,	2,	PERMIT_LEVEL(0), EMP_IND_ALLOW_ALL,
	"/*",		"*/",	EMP_MULTI_BETWEEN,	&(color.Character.Comment.Conf),	0,	3,	PERMIT_LEVEL(0)|PERMIT_LEVEL(5), EMP_IND_ALLOW_ALL,
	";",		"",		EMP_LINE_AFTER,		&(color.Character.Comment.Conf),	0,	3,	PERMIT_LEVEL(0)|PERMIT_LEVEL(5), EMP_IND_ALLOW_ALL,
	"//",		"",		EMP_LINE_AFTER,		&(color.Character.Comment.Conf),	0,	3,	PERMIT_LEVEL(0)|PERMIT_LEVEL(5), EMP_IND_ALLOW_ALL,

	// Label
	"*",		"",		EMP_LINE_AFTER,		&(color.Character.Label.Conf),	EMPFLAG_HEAD,	5,	PERMIT_LEVEL(0), EMP_IND_ASCII_LETTER|EMP_IND_UNDERBAR,

	// End of table	
	NULL
};

#define DefClassifyTableSize() ((sizeof(DefClassifyTable)/sizeof(*DefClassifyTable))-1)

// Table of type returned from hspcmp.dll 
static TYPE_TABLE TypeTable[] = {
	"sys|macro",	&(color.Character.Macro.Conf),			EMPFLAG_NON_CS,
	"sys|func",		&(color.Character.Function.Conf),		EMPFLAG_NON_CS,
	"sys|func|1",	&(color.Character.Function.Conf),		EMPFLAG_NON_CS,
	"sys|func|2",	&(color.Character.Function.Conf),		EMPFLAG_NON_CS,
	"pre|func",		&(color.Character.Preprocessor.Conf),	0,
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
	if ( ClassifyTable != NULL ) { free(ClassifyTable); }
	ClassifyTable = (CLASSIFY_TABLE *)malloc(tableCapacity * sizeof(CLASSIFY_TABLE));

	for(int i = 0; DefClassifyTable[i].Word1 != NULL; i++, nCTSize++){
		lstrcpy(ClassifyTable[i].Word1, DefClassifyTable[i].Word1);
		lstrcpy(ClassifyTable[i].Word2, DefClassifyTable[i].Word2);
		ClassifyTable[i].Type   = DefClassifyTable[i].Type;
		ClassifyTable[i].color  = DefClassifyTable[i].color;
		ClassifyTable[i].Status = DefClassifyTable[i].Status;
		ClassifyTable[i].Level  = DefClassifyTable[i].Level;
		ClassifyTable[i].pLevel = DefClassifyTable[i].pLevel;
		ClassifyTable[i].Ind    = DefClassifyTable[i].Ind;
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
					ClassifyTable[nCTSize].Status = /* | F_SE_INDEPENDENCE_B*/lpTT->Status;	// 2008-02-17 Shark++ 代替機能不明
					ClassifyTable[nCTSize].Level  = 1;
					ClassifyTable[nCTSize].pLevel = PERMIT_LEVEL(0);
					ClassifyTable[nCTSize].Ind    = EMP_IND_BLANKS|EMP_IND_ASCII_SIGN;
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


// 強調文字の設定
void SetClassify(int FootyID)
{

	// 2008-02-17 Shark++ 要動作確認
	for(CLASSIFY_TABLE *lpCT = ClassifyTable; lpCT->Word1[0] != '\0'; lpCT++) {
		Footy2AddEmphasis(FootyID, lpCT->Word1, *lpCT->Word2 ? lpCT->Word2 : NULL, lpCT->Type, 
			lpCT->Status, lpCT->Level, lpCT->pLevel, lpCT->Ind,
		//	lpCT->Status, lpCT->Level, PERMIT_LEVEL(lpCT->pLevel), EMP_IND_ALLOW_ALL,
			(lpCT->color != NULL ? *(lpCT->color) : RGB(255,0,0)));
	}

	// 追加の定義登録 by inovia CumtomColorKeywords.cpp で定義
	SetCumtomColorKeywords(FootyID, color);
	
	Footy2FlushEmphasis(FootyID);

//	Footy2Refresh(FootyID); // 2008-02-18 Shark++ 要らない？
	return;
}

void SetEditColor(int FootyID)
{
	Footy2SetColor(FootyID, CP_TAB, color.NonCharacter.Tab.Conf,  false);
	Footy2SetColor(FootyID, CP_HALFSPACE, color.NonCharacter.HalfSpace.Conf,  false);
	Footy2SetColor(FootyID, CP_NORMALSPACE, color.NonCharacter.FullSpace.Conf,  false);
	Footy2SetColor(FootyID, CP_UNDERLINE, color.Edit.CaretUnderLine.Conf,  false);
	Footy2SetColor(FootyID, CP_CRLF, color.NonCharacter.NewLine.Conf,  false);
	Footy2SetColor(FootyID, CP_EOF, color.NonCharacter.EndOfFile.Conf,  false);
	Footy2SetColor(FootyID, CP_TEXT, color.Character.Default.Conf,  false);
	Footy2SetColor(FootyID, CP_LINENUMTEXT, color.LineNumber.Number.Conf,  false);
	Footy2SetColor(FootyID, CP_LINENUMBORDER, color.Edit.BoundaryLineNumber.Conf,  false);
	Footy2SetColor(FootyID, CP_CARETLINE, color.LineNumber.CaretLine.Conf,  false);
	Footy2SetColor(FootyID, CP_RULERTEXT, color.Ruler.Number.Conf,  false);
	Footy2SetColor(FootyID, CP_RULERBACKGROUND, color.Ruler.Background.Conf,  false);
	Footy2SetColor(FootyID, CP_RULERLINE, color.Ruler.Division.Conf,  false);
	Footy2SetColor(FootyID, CP_CARETPOS, color.Ruler.Caret.Conf,  false);
	Footy2SetColor(FootyID, CP_BACKGROUND, color.Edit.Background.Conf,  false);

	Footy2SetColor(FootyID, CP_URLTEXT, color.Clickable.URL.Conf,  false);
	Footy2SetColor(FootyID, CP_URLUNDERLINE, color.Clickable.URLUnderLine.Conf,  false);
	Footy2SetColor(FootyID, CP_MAILTEXT, color.Clickable.Mail.Conf,  false);
	Footy2SetColor(FootyID, CP_MAILUNDERLINE, color.Clickable.MailUnderLine.Conf,  false);
	Footy2SetColor(FootyID, CP_LABELTEXT, color.Clickable.Label.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_LABELUNDERLINE, color.Clickable.LabelUnderLine.Conf,  false);// by Tetr@pod

	Footy2SetColor(FootyID, CP_FUNCTEXT1, color.UserFunction.UserFunction1.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT2, color.UserFunction.UserFunction2.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT3, color.UserFunction.UserFunction3.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT4, color.UserFunction.UserFunction4.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT5, color.UserFunction.UserFunction5.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT6, color.UserFunction.UserFunction6.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT7, color.UserFunction.UserFunction7.Conf,  false);// by Tetr@pod
	Footy2SetColor(FootyID, CP_FUNCTEXT8, color.UserFunction.UserFunction8.Conf,  false);// by Tetr@pod

	// 指定背景色で描画指定する
	Footy2SetBackgroundImageColor(FootyID, color.Edit.Background.Conf);
	
	Footy2Refresh(FootyID);
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
