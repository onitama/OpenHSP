
//
//		WebTask : http access task source
//				  for Linux enviroment
//					onion software/onitama 2015/2
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "webtask_linux.h"

void WebTask::Reset( void )
{
	// Reset Value
	mode = CZHTTP_MODE_READY;
}


WebTask::WebTask( void )
{
	//	コンストラクタ
	//
	//str_agent = NULL;
	mode = CZHTTP_MODE_NONE;
	vardata = NULL;
	postdata = NULL;

	//	初期化を行う
	Reset();
}


WebTask::~WebTask( void )
{
	//	デストラクタ
	//
}


int WebTask::Request( char *url, char *post )
{
	//	HTTPリクエスト発行
	//	( url:リクエストするURL )
	//	( post:NULLの場合はGET、文字列の場合はPOSTで渡す )
	//
	if ( mode != CZHTTP_MODE_READY ) {
		return -1;
	}
	if ( post == NULL ) {
		SetVarRequestGet( url );
	} else {
		SetPostData( post );
		SetVarRequestPost( url, postdata );
	}
	return 0;
}


int WebTask::getStatus( int id )
{
	// ステータス値の取得
	//
	switch( id ) {
	case HTTPINFO_MODE:
		Exec();
		return getMode();
	case HTTPINFO_SIZE:
		return getSize();
	default:
		break;
	}
	return 0;
}


char *WebTask::getData( int id )
{
	// データ文字列の取得
	//
	switch( id ) {
	case HTTPINFO_DATA:				// 結果データ
		{
		char *p = getVarData();
		if ( p != NULL ) return p;
		break;
		}
	case HTTPINFO_ERROR:			// エラー文字列
		return getError();
	default:
		break;
	}
	return "";
}


void WebTask::setData( int id, char *str )
{
	// データ文字列の取得
	//
	switch( id ) {
	case HTTPINFO_DATA:
		ClearVarData();				// 結果データをクリアする
		break;
	case HTTPINFO_ERROR:
		SetError( str );			// エラー文字列
		break;
	default:
		break;
	}
	return;
}


//--------------------------------------------------------------//
//				External functions
//--------------------------------------------------------------//

int WebTask::Exec( void )
{
	//	毎フレーム実行
	//
	//int handle;

	switch( mode ) {
	case CZHTTP_MODE_VARREQUEST:
	case CZHTTP_MODE_VARREQSEND:
		// HTTPに接続
//		handle = emscripten_async_wget2_data(req_url.c_str(), varstr,
//									postdata, this, true,
//									WebTask::onLoaded, WebTask::onError, WebTask::onProgress);
		mode = CZHTTP_MODE_VARDATAWAIT;
		// FALL THROUGH
	case CZHTTP_MODE_VARDATAWAIT:
		break;
	case CZHTTP_MODE_VARDATAEND:
		mode = CZHTTP_MODE_READY;
		break;

	case CZHTTP_MODE_ERROR:
		return -1;
	default:
		return 1;
	}
	return 0;
}


//--------------------------------------------------------------//
//				Internal functions
//--------------------------------------------------------------//

void WebTask::ClearVarData( void )
{
	if ( vardata == NULL ) return;
	free( vardata );
	vardata = NULL;
}


void WebTask::ClearPostData( void )
{
	if ( postdata == NULL ) return;
	free( postdata );
	postdata = NULL;
}


void WebTask::SetURL( char *url )
{
	// サーバーのURLを設定
	//
	req_url = url;
}


void WebTask::SetPostData( char *post )
{
	// ポスト文字列の設定
	//
	ClearPostData();
	if ( post == NULL ) return;
	//
	postdata = (char *)malloc( strlen( post ) + 1 );
	strcpy( postdata, post );
}


void WebTask::SetVarRequestGet( char *path )
{
	// サーバーにファイルを要求(GET)
	//
	req_url = path;
	strcpy( varstr, "GET" );
	postdata = NULL;
	mode = CZHTTP_MODE_VARREQUEST;
}


void WebTask::SetVarRequestPost( char *path, char *post )
{
	// サーバーにファイルを要求(POST)
	//
	req_url = path;
	strcpy( varstr, "POST" );
	postdata = post;
	mode = CZHTTP_MODE_VARREQUEST;
}

void WebTask::SetError( char *mes )
{
	//	エラー文字列を設定
	//
	mode = CZHTTP_MODE_ERROR;
	strcpy( errstr,mes );
}

