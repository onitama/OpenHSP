
//
//		WebTask : http access task source
//				  for iOS enviroment
//					onion software/onitama 2015/5
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../hsp3/hsp3config.h"
#include "../../hsp3/supio.h"

#include "webtask_ios.h"

void WebTask::Terminate( void )
{
}


void WebTask::Reset( void )
{
	// Initalize
	//
	Terminate();
}


WebTask::WebTask( void )
{
	//	コンストラクタ
	//
	//	初期化を行う
	Reset();
}


WebTask::~WebTask( void )
{
	//	デストラクタ
	//
	Terminate();
}


int	WebTask::Request( char *url, char *post )
{
	//	HTTPリクエスト発行
	//	( url:リクエストするURL )
	//	( post:NULLの場合はGET、文字列の場合はPOSTで渡す )
	//
	return 0;
}


int	WebTask::getStatus( int id )
{
	// ステータス値の取得
	//
	return 0;
}


char *WebTask::getData( int id )
{
	// データ文字列の取得
	//
	return "";
}


void WebTask::setData( int id, char *str )
{
	// データ文字列の取得
	//
	return;
}


//--------------------------------------------------------------//
//				External functions
//--------------------------------------------------------------//

int WebTask::Exec( void )
{
	//	毎フレーム実行
	//
	return 0;
}


//--------------------------------------------------------------//
//				Internal functions
//--------------------------------------------------------------//

