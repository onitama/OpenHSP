
//
//	HSP3 debug support
//	(エラー処理およびデバッグ支援)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsp3config.h"
#include "hsp3debug.h"
#include "supio.h"

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/


/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

#ifdef HSPDEBUG
#ifdef JPNMSG
static char *err[]={
	"",												// 0
	"システムエラーが発生しました",					// 1
	"文法が間違っています",							// 2
	"パラメータの値が異常です",						// 3
	"計算式でエラーが発生しました",					// 4
	"パラメータの省略はできません",					// 5
	"パラメータの型が違います",						// 6
	"配列の要素が無効です",							// 7
	"有効なラベルが指定されていません",				// 8
	"サブルーチンやループのネストが深すぎます",		// 9
	"サブルーチン外のreturnは無効です",				// 10
	"repeat外でのloopは無効です",					// 11
	"ファイルが見つからないか無効な名前です",		// 12
	"画像ファイルがありません",						// 13
	"外部ファイル呼び出し中のエラーです",			// 14
	"計算式でカッコの記述が違います",				// 15
	"パラメータの数が多すぎます",					// 16
	"文字列式で扱える文字数を越えました",			// 17
	"代入できない変数名を指定しています",			// 18
	"0で除算しました",								// 19
	"バッファオーバーフローが発生しました",			// 20
	"サポートされない機能を選択しました",			// 21
	"計算式のカッコが深すぎます",					// 22
	"変数名が指定されていません",					// 23
	"整数以外が指定されています",					// 24
	"配列の要素書式が間違っています",				// 25
	"メモリの確保ができませんでした",				// 26
	"タイプの初期化に失敗しました",					// 27
	"関数に引数が設定されていません",				// 28
	"スタック領域のオーバーフローです",				// 29
	"無効な名前がパラメーターに指定されています",	// 30
	"異なる型を持つ配列変数に代入しました",			// 31
	"関数のパラメーター記述が不正です",				// 32
	"オブジェクト数が多すぎます",					// 33
	"配列・関数として使用できない型です",			// 34
	"モジュール変数が指定されていません",			// 35
	"モジュール変数の指定が無効です",				// 36
	"変数型の変換に失敗しました",					// 37
	"外部DLLの呼び出しに失敗しました",				// 38
	"外部オブジェクトの呼び出しに失敗しました",		// 39
	"関数の戻り値が設定されていません。",			// 40
	"関数を命令として記述しています。\n(HSP2から関数化された名前を使用している可能性があります)",			// 41
	"*"
};
#else
static char *err[]={
	"",									// 0
	"Unknown system error",				// 1
	"Syntax error",						// 2
	"Illegal function call",			// 3
	"Wrong expression",					// 4
	"Default parameter not allowed",	// 5
	"Type mismatch",					// 6
	"Array overflow",					// 7
	"Not a label object",				// 8
	"Too many nesting",					// 9
	"Return without gosub",				// 10
	"Loop without repeat",				// 11
	"File I/O error",					// 12
	"Picture file missing",				// 13
	"External execute error",			// 14
	"Priority error",					// 15
	"Too many parameters",				// 16
	"Temporary buffer overflow",		// 17
	"Wrong valiable name",				// 18
	"Divided by zero",					// 19
	"Buffer Overflow",					// 20
	"Unsupported function",				// 21
	"Expression too complex",			// 22
	"Variable required",				// 23
	"Integer value required",			// 24
	"Bad array expression",				// 25
	"Out of memory",					// 26
	"Type initalization failed",		// 27
	"No Function parameters",			// 28
	"Stack overflow",					// 29
	"Invalid parameter name",			// 30
	"Invalid type of array",			// 31
	"Invalid function parameter",		// 32
	"Too many window objects",			// 33
	"Invalid Array/Function",			// 34
	"Structure required",				// 35
	"Illegal structure call",			// 36
	"Invalid type of variable",			// 37
	"DLL call failed",					// 38
	"External COM call failed",			// 39
	"No function result",				// 40
	"Invalid syntax for function",		// 41
	"*"
};
#endif

char *hspd_geterror( HSPERROR error )
{
	if ((error<0)||(error>=HSPERR_MAX)) return err[0];
	return err[error];
}

#else

static char errmsg[256];

char *hspd_geterror( HSPERROR error )
{
#ifdef JPNMSG
	sprintf( errmsg, "内部エラーが発生しました(%d)", (int)error );
#else
	sprintf( errmsg, "Internal Error(%d)", (int)error );
#endif
	return errmsg;
}

#endif



