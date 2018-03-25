
//
//	HSP3 debug support
//	(エラーメッセージ)
//	onion software/onitama 2004/6
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../hsp3/hsp3config.h"
#include "hsc3.h"
#include "errormsg.h"

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

#ifdef JPNMSG
static char *err[]={
	"",														// 0
	"解釈できない文字コードです",							// 1
	"文法が間違っています",									// 2
	"小数の記述が間違っています",							// 3
	"パラメーター式の記述が無効です",						// 4
	"カッコが閉じられていません",							// 5
	"配列の書式が間違っています",							// 6
	"ラベル名はすでに使われています",						// 7
	"ラベル名は指定できません",								// 8
	"repeatのネストが深すぎます",							// 9
	"repeatループ外でbreakが使用されました",				// 10
	"repeatループ外でcontinueが使用されました",				// 11
	"repeatループでないのにloopが使用されました",			// 12
	"repeatループが閉じられていません",						// 13
	"elseの前にifが見当たりません",							// 14
	"{が閉じられていません",								// 15
	"if命令以外で{～}が使われています",						// 16
	"else命令の位置が不正です",								// 17
	"if命令の階層が深すぎます",								// 18
	"致命的なエラーです",									// 19
	"プリプロセッサ命令が解釈できません",					// 20
	"コマンド登録中のエラーです",							// 21
	"プリプロセッサは文字列のみ受け付けます",				// 22
	"パラメーター引数の指定が間違っています",				// 23
	"ライブラリ名が指定されていません",						// 24
	"命令として定義できない名前です",						// 25
	"パラメーター引数名は使用されています",					// 26
	"モジュール変数の参照元が無効です",						// 27
	"モジュール変数の指定が無効です",						// 28
	"外部関数のインポート名が無効です",						// 29
	"拡張命令の名前はすでに使用されています",				// 30
	"互換性のない拡張命令タイプを使用しています",			// 31
	"コンストラクタは既に登録されています",					// 32
	"デストラクタは既に登録されています",					// 33
	"複数行文字列の終端ではありません",						// 34
	"タグ名はすでに使用されています",						// 35
	"インターフェース名が指定されていません",				// 36
	"インポートするインデックスが指定されていません",		// 37
	"インポートするIID名が指定されていません",				// 38
	"未初期化変数を使用しようとしました",					// 39
	"指定できない変数名です",								// 40
	"*"
};
#else
static char *err[]={
	"",														// 0
	"Unknown code",											// 1
	"Syntax error",											// 2
	"Wrong expression (float)",								// 3
	"Wrong expression (parameter)",							// 4
	"Parameter not closed",									// 5
	"Wrong array expression",								// 6
	"Label name is already in use",							// 7
	"Reserved name for label",								// 8
	"Too many repeat level",								// 9
	"break without repeat command",							// 10
	"continue without repeat command",						// 11
	"loop without repeat command",							// 12
	"loop not found for repeat command",					// 13
	"Not match if and else pair",							// 14
	"Not match { and } pair",								// 15
	"Not allow { and } in here",							// 16
	"Illegal use of else command",							// 17
	"Too many If command level",							// 18
	"Fatal error occurs",									// 19
	"Preprocessor syntax error",							// 20
	"Error during register command",						// 21
	"Only strings are acceptable",							// 22
	"Wrong parameter name expression",						// 23
	"No library name to bind",								// 24
	"custom name already in use",							// 25
	"parameter name already in use",						// 26
	"Bad structure source expression",						// 27
	"Bad structure expression",								// 28
	"Bad import name",										// 29
	"External func name in use",							// 30
	"Incompatible type of import",							// 31
	"Initalizer already exists",							// 32
	"Terminator already exists",							// 33
	"Wrong multi line string expression",					// 34
	"Tag name already in use",								// 35
	"No interface name to bind",							// 36
	"No import index to bind",								// 37
	"No import IID to bind",								// 38
	"Uninitalized variable detected",						// 39
	"Wrong name for variable",								// 40
	"*"
};
#endif

char *cg_geterror( CGERROR error )
{
	if ((error<0)||(error>=CGERROR_MAX)) return err[0];
	return err[error];
}

