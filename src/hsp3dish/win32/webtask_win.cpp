
//
//		WebTask : http access task source
//				  for Windows95/NT 32bit enviroment
//					onion software/onitama 2015/2
//

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "../../hsp3/hsp3config.h"
//#include "../../hsp3/supio.h"

#include "webtask_win.h"

#pragma comment( lib,"Wininet.lib" )

void WebTask::Terminate( void )
{
	//	Delete Session
	if ( hSession != NULL ) {
		InternetCloseHandle( hSession );
		hSession = NULL;
	}
	errstr[0] = 0;
	req_url[0] = 0;

	//	Clear headers
	if ( req_header != NULL ) { free( req_header ); req_header = NULL; }

	ClearVarData();
	ClearPostData();
}


void WebTask::Reset( void )
{
	// Initalize
	//
	char *agent;
	Terminate();

	agent = str_agent;
	if ( agent == NULL ) agent = "HSP3Dish(Windows)";

	if ( proxy_url[0] != 0 ) {
		char *local_prm = NULL;
		if ( proxy_local ) local_prm = "<local>";
		hSession = InternetOpen( agent, INTERNET_OPEN_TYPE_PROXY, proxy_url, local_prm, 0 );
	} else {
		hSession = InternetOpen( agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
	}
	if ( hSession == NULL ) {
		mode = CZHTTP_MODE_NONE;
		SetError( "初期化に失敗しました" ); return;
	}

	// Reset Value
	mode = CZHTTP_MODE_READY;
}


WebTask::WebTask( void )
{
	//	コンストラクタ
	//
	str_agent = NULL;
	mode = CZHTTP_MODE_NONE;
	hSession = NULL;
	proxy_local = 0;
	proxy_url[0] = 0;
	req_header = NULL;
	vardata = NULL;
	postdata = NULL;

	//	接続可能か?
	if( InternetAttemptConnect(0) ){
		SetError( "ネット接続が確認できませんでした" ); return;
	}

	//	初期化を行う
	Reset();
}


WebTask::~WebTask( void )
{
	//	デストラクタ
	//
	Terminate();
	if ( str_agent != NULL ) { free( str_agent ); str_agent = NULL; }
}


int	WebTask::Request( char *url, char *post )
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


int	WebTask::getStatus( int id )
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
	static char hdr[] = "Content-Type: application/x-www-form-urlencoded\r\n";
	BOOL res;

	switch( mode ) {
	case CZHTTP_MODE_VARREQUEST:

		// HTTPに接続
		hHttpSession = ::InternetConnectA( hSession, varserver, varport, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
		if ( hHttpSession == NULL ) {
			SetError( "無効なサーバーが指定されました" );
			break;
		}

		// HTTP要求の作成
		hHttpRequest = ::HttpOpenRequestA( hHttpSession, varstr, req_url, HTTP_VERSION, NULL, NULL, INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_UI, 0 );
		if ( hHttpSession == NULL ) {
			SetError( "無効なURLが指定されました" );
			break;
		}
		mode = CZHTTP_MODE_VARREQSEND;
		// FALL THROUGH
	case CZHTTP_MODE_VARREQSEND:

		// 作成したHTTP要求の発行
		if ( postdata != NULL ) {
			char *additional_header = req_header != NULL ? req_header : "";
			char *header = (char *)malloc( strlen(hdr) + strlen(additional_header) + 1 );
			strcpy(header, hdr);
			strcat(header, additional_header);
			res = ::HttpSendRequestA( hHttpRequest, header, -1L, postdata, (int)strlen(postdata) );
			free(header);
		} else {
			res = ::HttpSendRequestA( hHttpRequest, req_header, -1L, NULL, 0 );
		}
		if ( res == false ) {
			InternetCloseHandle( hHttpSession );
			SetError( "リクエストができませんでした" );
			break;
		}
		varsize = INETBUF_MAX;
		ClearVarData();
		vardata = (char *)malloc( varsize );
		size = 0;
		mode = CZHTTP_MODE_VARDATAWAIT;
		// FALL THROUGH
	case CZHTTP_MODE_VARDATAWAIT:
		{
		DWORD dwBytesRead;
		int needed_size = size + INETBUF_MAX;
		if ( needed_size > varsize ) {
			while ( needed_size > varsize ) {
				varsize *= 2;
			}
			vardata = (char *)realloc( vardata, varsize );
		}
		if ( InternetReadFile( hHttpRequest, vardata+size, INETBUF_MAX, &dwBytesRead ) == 0 ) {
			InternetCloseHandle( hHttpRequest );
			InternetCloseHandle( hHttpSession );
			SetError( "ダウンロード中にエラーが発生しました" );
			break;
		}
		size += dwBytesRead;
		if( dwBytesRead == 0 ) {
			mode = CZHTTP_MODE_VARDATAEND;
			vardata[size] = 0;
			break;
		}
		break;
		}
	case CZHTTP_MODE_VARDATAEND:
		InternetCloseHandle( hHttpRequest );
		InternetCloseHandle( hHttpSession );
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
	strncpy( req_url, url, 1024 );
}


void WebTask::SetProxy( char *url, int port, int local )
{
	// PROXYサーバーの設定
	//	(URLにNULLを指定するとPROXY無効となる)
	//
	if ( url == NULL ) {
		proxy_url[0] = 0;
	} else {
		sprintf( proxy_url, "%s:%d", url, port );
		proxy_local = local;
	}
	Reset();
}


void WebTask::SetAgent( char *agent )
{
	// エージェントの設定
	//	(URLにNULLを指定するとデフォルトになる)
	//
	if ( str_agent != NULL ) { free( str_agent ); str_agent = NULL; }
	if ( agent == NULL ) str_agent = agent; else {
		str_agent = (char *)malloc( strlen( agent ) + 1 );
		strcpy( str_agent, agent );
	}
	Reset();
}


void WebTask::SetHeader( char *header )
{
	// ヘッダ文字列の設定
	//
	if ( req_header != NULL ) { free( req_header ); req_header = NULL; }
	if ( header == NULL ) return;
	//
	req_header = (char *)malloc( strlen( header ) + 1 );
	strcpy( req_header, header );
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


void WebTask::SetVarServerFromURL( char *url )
{
	char *p;
	char *wr;
	char a1;
	int urllen;
	p = url;
	wr = varserver;
	*wr = 0;
	varport = INTERNET_DEFAULT_HTTP_PORT;

	while(1)				// '//'を探す
	{
		a1 = *p++;
		if ( a1 == 0 ) return;
		if ( a1 == '/' ) {
			if ( *p == '/' ) { p++; break; }
		}
	}
	while(1) {				// '/'までを取り出す
		a1 = *p;
		if ( a1 == 0 ) break;
		p++;
		if ( a1 == '/' ) break;
		if ( a1 == ':' ) {
			//	ポート番号を取り出す
			int i = 0;
			while(isdigit(p[i])) {
				i ++;
			}
			if (p[i] == '/' || p[i] == 0) {
				if (i != 0) varport = atoi(p);
				p += i;
				if (*p == '/') p ++;
				break;
			}
		}
		*wr++ = a1;
	}
	*wr = 0;

	wr = req_url;
	*wr++ = '/';
	urllen = 1;

	while(1) {				// 最後まで取り出す
		if ( urllen >= INETURL_MAX ) break;
		a1 = *p++;
		if ( a1 == 0 ) break;
		*wr++ = a1;
		urllen++;
	}
	*wr = 0;

}


void WebTask::SetVarRequestGet( char *path )
{
	// サーバーにファイルを要求(GET)
	//
	SetVarServerFromURL( path );
	strcpy( varstr, "GET" );
	postdata = NULL;
	mode = CZHTTP_MODE_VARREQUEST;
}


void WebTask::SetVarRequestPost( char *path, char *post )
{
	// サーバーにファイルを要求(POST)
	//
	SetVarServerFromURL( path );
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


