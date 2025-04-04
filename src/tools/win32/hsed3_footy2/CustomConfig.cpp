/*------------------------------------------------------------------------------
	カスタム設定ファイル読み込み for HSP Script Editor by inovia
	作成 2012.06.21
	修正 2012.06.21
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

#include "CustomConfig.h"
#include "PackIconResource.h"

// PathFileExistsで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#pragma warning( disable : 4996 )
#pragma warning( disable : 4006 )

// フルパスゲット
void GetINIFullPath(char *inipath){
	GetModuleFileNameA(0, inipath, _MAX_PATH);
	PathRemoveFileSpec(inipath);	// ディレクトリのみにする
	strcat(inipath, "\\custom.ini");
}

// 設定が増えそうなので汎用的に使えるように変更
void SetINICustomInt(int f, char *s){
	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	char tmp[64];
	sprintf(tmp,"%d",f);
	WritePrivateProfileString("HSED3", s, tmp, inipath);
}

void SetINICustomBOOL(BOOL f, char *s){
	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	char tmp[64];
	if (f == TRUE){
		sprintf(tmp, "1");
	}else{
		sprintf(tmp, "0");
	}
	WritePrivateProfileString("HSED3", s, tmp, inipath);
}

void SetINICustomString(char *key, char *value){
	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	WritePrivateProfileString("HSED3", key, value, inipath);
}

int GetINICustomInt(char *s){

	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	// パスがない場合は設定を0にして返却
	if (!_PathFileExists(inipath))
		return 0;

	// INIファイル読み込み(失敗したときは0)
	return GetPrivateProfileIntA("HSED3", s, 0, inipath);
}

// あらかじめ呼び出し元でバッファの確保&0埋めが必要
DWORD GetINICustomString(char *key, char *value, int maxLength){

	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	// パスがない場合は0にして返却
	if (!_PathFileExists(inipath))
		return 0;

	// INIファイル読み込み(失敗したときは0)
	return GetPrivateProfileStringA("HSED3", key, "", value, (DWORD)maxLength, inipath);
}

BOOL GetINICustomBOOL(char *s){

	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	// パスがない場合は設定を0にして返却
	if (!_PathFileExists(inipath))
		return FALSE;

	// INIファイル読み込み(失敗したときは0)
	return GetPrivateProfileIntA("HSED3", s, FALSE, inipath);
}

// ↑変更

void SetINIForceFont(BOOL f){
	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	char tmp[64];
	if (f == TRUE){
		sprintf(tmp, "1");
	}else{
		sprintf(tmp, "0");
	}
	WritePrivateProfileString("HSED3", "ForceFont", tmp, inipath);
}


void SetINICustomColor(int f){
	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	char tmp[64];
	sprintf(tmp,"%d",f);
	WritePrivateProfileString("HSED3", "CustomColor", tmp, inipath);
}

// INIファイルのフォント強制設定を読み込む
int GetINIForceFont(void){

	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	// パスがない場合は設定を0にして返却
	if (!_PathFileExists(inipath))
		return 0;

	// INIファイル読み込み(失敗したときは0)
	return GetPrivateProfileIntA("HSED3", "ForceFont", 0, inipath);
}

// INIファイルのカスタムカラー設定を読み込む
BOOL GetINICustomColor(void){

	// EXE(hsed3.exe)のあるディレクトリ
	char inipath[_MAX_PATH+1];
	GetINIFullPath(inipath);

	// パスがない場合は設定を0にして返却
	if (!_PathFileExists(inipath))
		return FALSE;

	// INIファイル読み込み(失敗したときは0)
	return GetPrivateProfileIntA("HSED3", "CustomColor", FALSE, inipath);
}

