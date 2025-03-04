
//
//		WebTask : http access task source
//				  for android enviroment
//					onion software/onitama 2015/5
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "../../hsp3/hsp3config.h"
//#include "../../hsp3/supio.h"

#include "webtask_ndk.h"
#include "../../javafunc.h"

void WebTask::Terminate( void )
{
}


void WebTask::Reset( void )
{
	// Initalize
	//
	Terminate();
	mode = CZHTTP_MODE_NONE;
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
	int res;

	res = j_httpRequest( url, post, 0 );
	if ( res ) {
		mode = CZHTTP_MODE_NONE;
		return res;
	}

	mode = CZHTTP_MODE_READY;
	return res;
}


int	WebTask::getStatus( int id )
{
	// ステータス値の取得
	//
	switch( id ) {
	case HTTPINFO_MODE:
		Exec();
		return getMode();
	case HTTPINFO_SIZE:
		return strlen( j_getHttpInfo() );
	default:
		break;
	}
	return 0;
}


char *WebTask::getData( int id )
{
	// データ文字列の取得
	//
	// データ文字列の取得
	//
	switch( id ) {
	case HTTPINFO_DATA:				// 結果データ
		{
		char *p = j_getHttpInfo();
		if ( p != NULL ) return p;
		break;
		}
	case HTTPINFO_ERROR:			// エラー文字列
//		return getError();
	default:
		break;
	}
	return "";
}


void WebTask::setData( int id, char *str )
{
	// データ文字列の設定
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

