/*------------------------------------------------------------------------------
	アイコンとリソース埋め込み for HSP Script Editor by inovia
	作成 2012.02.24
	修正 2012.04.08
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

#include "PackIconResource.h"
#include "CustomColorKeywords.h"

// PathFileExistsで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#pragma warning( disable : 4996 )
#pragma warning( disable : 4006 )

void RunIconChange(int activeFootyID){
	/* inovia */
	char fpath[1024+1] = {0};	// EXE
	char ipath[1024+1] = {0};	// ico
	char rpath[1024+1] = {0};	// バージョンリソース
	char mpath[1024+1] = {0};	// manifest by Tetr@pod
	char extension[1024+1] = {0};	// 拡張子（オプション）
	int upx = 0;	// UPX（オプション）
	int lang = -1;	// 言語ID（オプション）
	char cmd[8192+1] = {0};	// コマンドライン

	GetIconResourcePath(fpath, ipath, rpath, mpath/*manifest by Tetr@pod*/, activeFootyID, &upx, &lang, extension);

	// #packopt name "hogehoge" がない場合
	if (strcmp(fpath, "") == 0){
		strcpy(fpath, "hsptmp");
	}

	// EXE(hsed3.exe)のあるディレクトリ
	char exepath[_MAX_PATH+1];
	GetModuleFileNameA(0, exepath, _MAX_PATH);
	_PathRemoveFileSpec(exepath);	// ディレクトリのみにする
	strcat(exepath, "\\iconins.exe");

	// スクリプトソースのフォルダ(*.hsp)
	char tmpfn[_MAX_PATH+1] = {0};
	GetCurrentDirectory( _MAX_PATH, tmpfn );

	// ↓コマンドラインに渡す文字列を代入していく
	strcat(cmd, "-e\"");
	strcat(cmd, tmpfn);
	strcat(cmd, "\\");
	strcat(cmd, fpath);
	// 拡張子を変更するオプション（指定なしの場合は.exe）
	if (strcmp(extension, "") == 0){
		strcat(cmd, ".exe\"");
	}else{
		strcat(cmd, extension);
		strcat(cmd, "\"");
	}
	// アイコンパス
	if (strcmp(ipath, "") != 0){
		strcat(cmd, " -i\"");
		strcat(cmd, ipath);
		strcat(cmd, "\"");
	}
	// リソース（バージョン）パス
	if (strcmp(rpath, "") != 0){
		strcat(cmd, " -v\"");
		strcat(cmd, rpath);
		strcat(cmd, "\"");
	}
	// リソース（manifest）パス by Tetr@pod
	if (strcmp(mpath, "") != 0){
		strcat(cmd, " -m\"");
		strcat(cmd, mpath);
		strcat(cmd, "\"");
	}

	
	if (upx == 1){
		// UPXを使う場合
		strcat(cmd, " -u\"1\"");
	}
	if (lang != -1){
		// 言語ID
		char tmp[1024+1] = {0};
		sprintf(tmp, " -l\"%d\"", lang);
		strcat(cmd, tmp);
	}
	//ShellExecute( NULL, "explore", tmpfn, NULL, NULL, SW_SHOWNORMAL );

	// どちらかにパスが入っている場合は実行する
	if ((strcmp(ipath, ""))||(strcmp(rpath, ""))||(upx == 1)){
		if (_PathFileExists(exepath)){
			//char runpath[4096+1];memset( fpath, '\0', sizeof(runpath) );	// パス
			//memset( runpath, '\0', sizeof(runpath) );
			//sprintf( runpath, "\"%s\" %s", exepath, cmd );
			//WinExec( runpath, SW_SHOW );
			
			// 終了まで待機したいのでこちらを使用
			SHELLEXECUTEINFO sei = {sizeof(sei)};
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.hwnd = 0;
			sei.nShow = SW_SHOWNORMAL;
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			sei.lpFile = exepath;
			sei.lpParameters = cmd;
			ShellExecuteEx(&sei);
			// 終了待ち
			WaitForSingleObject(sei.hProcess, INFINITE) ;
			
		}else{
			//
			MessageBoxA(GetActiveWindow(), "iconins.exe が見つかりません。アイコンの埋め込みは失敗しました。", "エラー", MB_OK | MB_ICONERROR);
		}
	}
	/* inovia */
}

BOOL _PathFileExists(LPCTSTR pszPath)
{
	return PathFileExists(pszPath);
}

BOOL _PathRemoveFileSpec(LPTSTR pszPath)
{
	return PathRemoveFileSpec(pszPath);
}

// メインスクリプトからパスを検索
int GetIconResourcePath(char *exepath, char *iconpath, char *respath, char *manifestpath/*manifest by Tetr@pod*/, int _activeFootyID, int *upx, int *lang, char *extension){
	
	char *buffer, *dummy;
	// Footy2から１行ずつ取得
	int lines = Footy2GetLines(_activeFootyID);
	if (lines == FOOTY2ERR_NOID) return 0;	// そのIDが見つからない場合はそのままリターン
	int i = 0;
	int flag2 = 0;
	// 全行調べる
	for (i = 0;i <= lines; i++){
		// １行のサイズを取得
		int linesize = Footy2GetLineLength(_activeFootyID, i);
		// 問題があった場合は中止
		if ( (linesize == FOOTY2ERR_ARGUMENT) || (linesize == FOOTY2ERR_NOID) ) break;
		// １行の文字数が0の場合（空行の時に参照すると落ちる）
		if (linesize == 0) continue;
		// 動的確保
		buffer = (char *) calloc( linesize * 2 + 2 , sizeof(char) );
		// １行取得(Footy2GetLineAのマニュアルの引数が間違っているので注意)
		int res = Footy2GetLine(_activeFootyID, buffer, i, linesize * 2);
		// いじくってしまうほうのバッファーをbufferからdummyへコピー
		dummy = (char *) calloc( linesize * 2 + 2 , sizeof(char) );
		memcpy(dummy, buffer, linesize * 2);
	
		// 問題があった場合は中止
		if ( (res == FOOTY2ERR_ARGUMENT) || (res == FOOTY2ERR_NOID) ){
			free(dummy);dummy = 0;
			free(buffer);buffer = 0;
			break;
		}
		
		// dummyからタブと空白を取り除く
		/*
			0 #pack
			1 #packopt
			2  #packopt
			3 // #packopt 0 

			のような場合、#packopt の前にスペースやタブが入っていても動作するようにするため
		*/
		StrReplaceALL(dummy, "\t", "");
		StrReplaceALL(dummy, " ", "");

		// #packopt があるか調べる
		char *pos = strstr(dummy, "#packopt");
		if ((pos != NULL) && (pos-dummy == 0)){
			/*
			char test[128];
			sprintf(test,"%d",pos-dummy);
			MessageBoxA(0,test,"",0);
			*/
			// タブを空白に変換する(統一)
			StrReplaceALL(buffer, "\t", " ");
			
			// 本当に #packopt か調べる。
			// #packopt2 とか #packopts などのケースに対応
			int splits = GetSplitNum(buffer, " ");
			int flag = 0;	// 一致した場合のフラグ
			// 分割数だけ調べる
			int j;
			for (j = 0;j <= splits; j++){
				// 確保
				char* tmp = (char*)calloc(linesize * 2 + 2, sizeof(char));
				// スペースで分割した結果をtmpに代入
				GetSplitString(buffer, " ", j, tmp);
				// 一致した場合はフラグを立てる
				if (strcmp(tmp, "#packopt") == 0) flag = 1;
				// 解放
				free(tmp);tmp = 0;
			}
			// #packoptが一致した場合のみ続行
			if (flag == 1){
				// キーワードを取得というか比較してフラグを立てる
				// #packopt name "test"
				//           ↑ コレ
				splits = GetSplitNum(buffer, " ");
				int j;
				for (j = 0;j <= splits; j++){
					// 確保
					char* tmp = (char*)calloc(linesize * 2 + 2, sizeof(char));
					// スペースで分割した結果をtmpに代入
					GetSplitString(buffer, " ", j, tmp);
					// 一致した場合はフラグを立てる
					if (strcmp(tmp, "name") == 0){ 
						//MessageBoxA(0,tmp,tmp,0);
						flag2 = 1;
					}
					if (strcmp(tmp, "icon") == 0) flag2 = 2;
					if (strcmp(tmp, "version") == 0) flag2 = 3;
					if (strcmp(tmp, "upx") == 0) flag2 = 4;
					if (strcmp(tmp, "lang") == 0) flag2 = 5;
					if (strcmp(tmp, "ext") == 0) flag2 = 6;
					if (strcmp(tmp, "last") == 0) flag2 = 7;
					if (strcmp(tmp, "manifest") == 0) flag2 = 8;// manifest by Tetr@pod
					// 解放
					free(tmp);tmp = 0;
				}
				// 例 #packopt name "hogehoge" のかっこの間（範囲）を取得
				char *sp, *ep;
				char* outpath = (char*)calloc(linesize * 2 + 2, sizeof(char));
				sp = strstr(buffer, "\"");
				if (sp != NULL){
					ep = strstr(sp+1, "\"");
					if (ep != NULL){
						strncpy(outpath, sp+1, ep-sp-1);
						//MessageBox(0,outpath,"",0);
					}
				}

				// 文字列が長すぎる場合はエラー文字を代入する
				if (strlen(outpath) > 512){
					strcpy(outpath, "::ERROR::");
				}

				// 条件分岐
				switch (flag2) {
					case 1 : 
						// name
						strcpy(exepath, outpath);
						
						//MessageBoxA(0,exepath,exepath,0);
						break;
					case 2 : 
						// icon
						strcpy(iconpath, outpath);
						break;
					case 3 : 
						// version
						strcpy(respath, outpath);
						break;
					case 4 : 
						// upx
						char use_upx[1024+1];memset( use_upx, '\0', sizeof(use_upx) );
						strcpy(use_upx, outpath);
						*upx = atoi(use_upx);
						break;
					case 5 : 
						// lang(言語ID)
						char langid[1024+1];memset( langid, '\0', sizeof(langid) );
						strcpy(langid, outpath);
						*lang = atoi(langid);
						break;
					case 6 : 
						// extension(拡張子)
						strcpy(extension, outpath);
						break;
					case 8 : 
						// manifest by Tetr@pod
						strcpy(manifestpath, outpath);
						break;
					default : 
						// それ以外。何もしない。
						break;
				}
				// 解放
				free(outpath);outpath = 0;flag2 = 0;

			}

		}

		// 必ず解放
		free(dummy);dummy = 0;
		free(buffer);buffer = 0;

		// 7 の場合強制離脱
		if (flag2 == 7)
			break;
	} 
	return 0;
}

// １つのみ置き換え
int StrReplace(char *buf, char *mae, char *ato)
{
    char *pos;
    size_t maelen, atolen;
    
    maelen = strlen(mae);
    atolen = strlen(ato);
    if (maelen == 0 || (pos = strstr(buf, mae)) == 0) return 0;
    memmove(pos + atolen, pos + maelen, strlen(buf) - (pos + maelen - buf ) + 1);
    memcpy(pos, ato, atolen);
    return 1;
}

// 対象文字列をすべて置き換え
int StrReplaceALL(char *buf, char *mae, char *ato)
{
	int i = 0;
	while (StrReplace(buf, mae, ato)){
		i++;
	}
	return i;
}

