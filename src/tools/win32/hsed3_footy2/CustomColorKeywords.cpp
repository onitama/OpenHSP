/*------------------------------------------------------------------------------
	色定義追加 for HSP Script Editor by inovia
	作成 2011.12.20
	修正 2011.12.26
------------------------------------------------------------------------------*/

// ↓いらないのがあるかも
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Footy2.h"
#include "hsed_config.h"
#include "poppad.h"
#include "config.h"
#include "classify.h"
#include "support.h"
#include "exttool.h"

// PathIsDirectoryで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#include "CustomColorKeywords.h"

#pragma warning( disable : 4996 )

extern BOOL bCustomColor;

extern BOOL CustomMatchCase;// by Tetr@pod


// classify.cpp	の SetClassify()から呼び出されます
void SetCumtomColorKeywords(int FootyID, MYCOLOR color)
{
	// 設定ファイルで使用しない設定の場合は色付けしない
	if (bCustomColor == FALSE)
		return;
	// 色定義(デフォの色)
	int FuncColor = RGB(  0, 255, 255);
	int OwnFuncColor = RGB(  0, 255, 255);
	int CFuncColor = RGB(  0, 255, 255);
	int OwnCFuncColor = RGB(  0, 255, 255);
	int Win32APIFuncColor = RGB(  0, 255, 255);
	int Win32APICFuncColor = RGB(  0, 255, 255);
	int MacroColor = RGB(192, 255, 255);
	int SysVarColor = RGB(  0, 255, 255);
	/*int FuncParam = EMP_IND_BLANKS;
	int CFuncParam = EMP_IND_BLANKS | EMP_IND_PARENTHESIS; 
	int VarParam = EMP_IND_BLANKS | EMP_IND_PARENTHESIS | EMP_IND_OTHER_ASCII_SIGN;*/
	// 大小比較記号、OR記号、演算子、ifの{}、演算子の() などを許可 by Tetr@pod
	int FuncParam = EMP_IND_BLANKS | EMP_IND_BRACE | EMP_IND_OPERATORS | EMP_IND_OTHER_ASCII_SIGN | EMP_IND_ANGLE_BRACKET | EMP_IND_PARENTHESIS;
	int CFuncParam = EMP_IND_BLANKS | EMP_IND_PARENTHESIS | EMP_IND_BRACE | EMP_IND_OPERATORS | EMP_IND_OTHER_ASCII_SIGN | EMP_IND_ANGLE_BRACKET; 
	int VarParam = EMP_IND_BLANKS | EMP_IND_PARENTHESIS | EMP_IND_OTHER_ASCII_SIGN | EMP_IND_BRACE | EMP_IND_OPERATORS | EMP_IND_ANGLE_BRACKET;

	// エディタから設定読み出し
	FuncColor = color.Character.Function.Conf;
	CFuncColor = color.Character.Function.Conf;
	OwnFuncColor = color.Character.Function.Conf;
	OwnCFuncColor = color.Character.Function.Conf;
	Win32APIFuncColor = color.Character.Function.Conf;
	Win32APICFuncColor = color.Character.Function.Conf;
	SysVarColor = color.Character.Function.Conf;
	MacroColor = color.Character.Macro.Conf;

	// EXEのあるディレクトリ
	char exepath[MAX_PATH+1];
	GetModuleFileNameA(0, exepath, MAX_PATH);
	PathRemoveFileSpec(exepath);

	// カスタム色定義ファイル読み込み
	char cumtompath[MAX_PATH+1];
	strcpy(cumtompath, exepath);
	strcat(cumtompath, "\\keywords\\color.ini");

	FILE *fpini = fopen(cumtompath,"r");
	if (fpini != NULL){
		char tmp[1024+1];
		while ((fgets(tmp, 1024, fpini)) != NULL) {
			// 改行コードだった場合
			if (tmp[strlen(tmp)-1] == 0x0A){
				tmp[strlen(tmp)-1] = '\0';	// 余計な改行コードをNULLで埋めて削除
			}
			// 分割数が２の場合
			if (GetSplitNum(tmp, " ") == 2){
				char* p = (char*)calloc(1024+1, sizeof(char));
				char* c = (char*)calloc(1024+1, sizeof(char));
				GetSplitString(tmp, " ", 0, p); // スペースで分離して０番目の要素を取得
				GetSplitString(tmp, " ", 1, c);	// スペースで分離して１番目の要素を取得
				// キーワードチェック
				if (strcmp(p,"FUNC") == 0){
					FuncColor = atoi(c);
				}else if(strcmp(p,"OWNFUNC") == 0){
					OwnFuncColor = atoi(c);
				}else if (strcmp(p,"CFUNC") == 0){
					CFuncColor = atoi(c);
				}else if(strcmp(p,"OWNCFUNC") == 0){
					OwnCFuncColor = atoi(c);
				}else if(strcmp(p,"MACRO") == 0){
					MacroColor = atoi(c);
				}else if(strcmp(p,"SYSVAR") == 0){
					SysVarColor = atoi(c);
				}else if((strcmp(p,"W32APIFUNC") == 0) || (strcmp(p,"WIN32APIFUNC") == 0)){
					Win32APIFuncColor = atoi(c);
				}else if((strcmp(p,"W32APICFUNC") == 0) || (strcmp(p,"WIN32APICFUNC") == 0)){
					Win32APICFuncColor = atoi(c);
				}
				free(p);
				free(c);
			}
		}
		fclose(fpini);
	}


	// キーワードフォルダをチェック
	char crDir[MAX_PATH + 1];
	//GetCurrentDirectoryA(MAX_PATH+1, crDir);
	//strcat(crDir, "\\keywords\\");
	strcpy(crDir, exepath);
	strcat(crDir, "\\keywords\\");

	// キーワードフォルダが存在しているかどうか
	if (PathIsDirectoryA(crDir)){
		// ある場合(最後に\\がついていること)
		// フォルダ内のファイルの数を取得
		int fnum = GetFileListNum(crDir);
		// (MAX_PATH＋2)×fnum bytes分の領域を確保(改行コードも含む)
		char* flist = (char*)calloc(fnum*(MAX_PATH+2)+1,sizeof(char));
		// ファイルの一覧を取得(\nで分けられた形式)
		GetFileList(crDir, flist);
		// ファイルの数だけ繰り返す
		int i = 0;
		for(i = 0; i<GetSplitNum(flist, "\n"); i++){
			// ファイル名を格納する変数を確保
			char* fname = (char*)calloc(MAX_PATH+1, sizeof(char));
			strcpy(fname, exepath);
			strcat(fname, "\\keywords\\");// フォルダ名を先に付加
			// ファイル名を取得する
			GetSplitString(flist, "\n", i, fname);
			// ファイルをオープンする
			FILE *fp = fopen(fname,"r");
			if (fp != NULL){
				// 1行分のデータ保存用（こんなにいらないと思うけど）
				char* line = (char*)calloc(1024+1, sizeof(char));
				// 1行ずつ読み取り
				while ((fgets(line, 1024, fp)) != NULL) {
					// 改行コードだった場合
					if (line[strlen(line)-1] == 0x0A){
						line[strlen(line)-1] = '\0';	// 余計な改行コード削除
					}
					// 分割数が３の場合は処理する
					if (GetSplitNum(line, " ") == 3){
						int footy2col = 0;
						int footy2flg = 0;
						char* param = (char*)calloc(1024+1, sizeof(char));
						char* flg = (char*)calloc(1024+1, sizeof(char));
						char* col = (char*)calloc(1024+1, sizeof(char));

						GetSplitString(line, " ", 0, param);
						GetSplitString(line, " ", 1, flg);
						GetSplitString(line, " ", 2, col);

						// 色のキーワードチェック
						if (strcmp(col,"FUNC") == 0){
							footy2col = FuncColor;
						}else if(strcmp(col,"OWNFUNC") == 0){
							footy2col = OwnFuncColor;
						}else if (strcmp(col,"CFUNC") == 0){
							footy2col = CFuncColor;
						}else if(strcmp(col,"OWNCFUNC") == 0){
							footy2col = OwnCFuncColor;
						}else if(strcmp(col,"MACRO") == 0){
							footy2col = MacroColor;
						}else if(strcmp(col,"SYSVAR") == 0){
							footy2col = SysVarColor;
						}else if((strcmp(col,"W32APIFUNC") == 0) || (strcmp(col,"WIN32APIFUNC") == 0)){
							footy2col = Win32APIFuncColor;
						}else if((strcmp(col,"W32APICFUNC") == 0) || (strcmp(col,"WIN32APICFUNC") == 0)){
							footy2col = Win32APICFuncColor;
						}else{
							// それ以外の場合は数値とみなす。
							int customcolor = atoi(col);
							footy2col = customcolor;
						}
						// 独立性の設定
						if (strcmp(flg,"FUNC") == 0){
							footy2flg = FuncParam;
						}else if(strcmp(flg,"OWNFUNC") == 0){
							footy2flg = FuncParam;
						}else if (strcmp(flg,"CFUNC") == 0){
							footy2flg = CFuncParam;
						}else if(strcmp(flg,"OWNCFUNC") == 0){
							footy2flg = CFuncParam;
						}else if(strcmp(flg,"MACRO") == 0){
							footy2flg = VarParam;
						}else if((strcmp(flg,"W32APIFUNC") == 0) || (strcmp(flg,"WIN32APIFUNC") == 0)){
							footy2flg = FuncParam;
						}else if((strcmp(flg,"W32APICFUNC") == 0) || (strcmp(flg,"WIN32APICFUNC") == 0)){
							footy2flg = CFuncParam;
						}else if(strcmp(flg,"SYSVAR") == 0){
							footy2flg = VarParam;
						}else{
							// それ以外の場合は数値とみなす。
							footy2flg = atoi(flg);
						}
						// Footy2側に適用
						
						if (CustomMatchCase) {// by Tetr@pod
							int res= Footy2AddEmphasis(FootyID, param, 0, EMP_WORD, 0, 1, 1, footy2flg, footy2col);
						} else {
							int res= Footy2AddEmphasis(FootyID, param, 0, EMP_WORD, 2, 1, 1, footy2flg, footy2col);
						}

						free(param);
						free(flg);
						free(col);
					}
				}
				// 1行を格納した変数を解放
				free(line);
				// ファイルをクローズ
				fclose(fp);
			}
			// ファイル名を格納した変数を解放
			free(fname);
		}
		
		// 解放
		free(flist);
		flist = NULL;

	}else{
		// フォルダがない
		//MessageBoxA(0, "ない", "ない", 0);
	}

}

// 内部的に使用している関数

// 戻り値はテキストファイルの数
int GetFileListNum(char *folder){
	// 1回目はデータのサイズを取得するため
	char temp[_MAX_PATH];
	WIN32_FIND_DATAA lp;
	HANDLE hFile;

	// パスのコピー＆*.txtの付加
	strcpy(temp, folder);
	strcat(temp, "*.txt");
	// 検索する
	hFile = FindFirstFileA(temp, &lp);

	if (hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}

	int num = 0;
	do{      
		if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY){
			num++;
		}
	}while(FindNextFileA(hFile, &lp));
	// クローズ
	FindClose(hFile);

	return num;
}

// 戻り値はテキストファイルの数
int GetFileList(char *folder,char *flist){
	// 2回目はデータを取得するため
	char temp[_MAX_PATH];
	WIN32_FIND_DATAA lp;
	HANDLE hFile;

	// パスのコピー＆*.txtの付加
	strcpy(temp, folder);
	strcat(temp, "*.txt");
	// 検索する
	hFile = FindFirstFileA(temp, &lp);

	if (hFile == INVALID_HANDLE_VALUE) {
		return 0;
	}

	int num = 0;
	do{      
		if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=FILE_ATTRIBUTE_DIRECTORY){
			strcat(flist, lp.cFileName);
			strcat(flist, "\n");
			num++;
		}
	}while(FindNextFileA(hFile, &lp));
	// クローズ
	FindClose(hFile);

	// 最後に\nを削除
	flist[strlen(flist)-1] = '\0';

	return num;
}

// 区切られた数を取得
int GetSplitNum(char *_s1,const char *s2){
	char *tok;
	int num = 0;

	// コピーして使用するため確保(かならず+1する)
	char* s1 = (char*)calloc(strlen(_s1)+1,sizeof(char));
	strcpy(s1, _s1);

	tok = strtok( s1, s2 );
	while( tok != NULL ){
		num++;
		tok = strtok( NULL, s2 );  /* 2回目以降 */
	}
	
	free(s1);
	return num;
}

// 指定された行の文字列を取得
int GetSplitString(char *_s1, const char *s2, int num, char *res){

	char *tok;

	// コピーして使用するため確保(かならず+1する)
	char* s1 = (char*)calloc(strlen(_s1)+1,sizeof(char));
	strcpy(s1, _s1);

	int i = 0;
	tok = strtok( s1, s2 );
	while( tok != NULL ){
		if (num == i){
			strcat(res, tok);
			free(s1);	// 解放忘れてたので追加
			return 1;
		}
		i++;
		tok = strtok( NULL, s2 );  /* 2回目以降 */
	}
	
	free(s1);
	return 0;
}