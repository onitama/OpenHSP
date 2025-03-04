
//
//		WebTask : http access task source
//				  for Linux enviroment
//					onion software/onitama 2015/2
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>

#include "webtask_linux.h"

void WebTask::Reset( void )
{
	// Reset Value
	Terminate();
	mode = CZHTTP_MODE_READY;

	const char* agent = str_agent;
	if ( agent == NULL ) agent = "HSP3Dish(Windows)";

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, agent);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

	if ( proxy_url[0] != 0 ) {
		curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url);
		if ( proxy_local ) {
			char *local_prm = NULL;
			local_prm = "<local>";
			curl_easy_setopt(curl, CURLOPT_NOPROXY, local_prm);
		}
	}
}


WebTask::WebTask( void )
{
	//	コンストラクタ
	//
	str_agent = NULL;
	mode = CZHTTP_MODE_NONE;
	vardata = NULL;
	postdata = NULL;
	proxy_local = 0;
	proxy_url[0] = 0;
	req_header = NULL;
	curl = NULL;

	mcurl = curl_multi_init();

	//	初期化を行う
	Reset();
}


WebTask::~WebTask( void )
{
	//	デストラクタ
	//
	curl_multi_cleanup(mcurl);
}

void WebTask::Terminate( void )
{
	//	Delete Session
	if ( curl != NULL ) {
		curl_multi_remove_handle(mcurl, curl);
		curl_easy_cleanup(curl);
		curl = NULL;

	}
	errstr[0] = 0;
	req_url[0] = 0;

	//	Clear headers
	if ( req_header != NULL ) { free( req_header ); req_header = NULL; }

	ClearVarData();
	ClearPostData();
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
	CURLMcode ret;
	int running = 1;

	switch( mode ) {
	case CZHTTP_MODE_VARREQUEST:
		//printf("url %s\n", req_url.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, req_url.c_str());
		if (postdata) {
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(postdata));
		}
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WebTask::OnReceive);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		varsize = INETBUF_MAX;
		ClearVarData();
		vardata = (char *)malloc( varsize );
		size = 0;
		mode = CZHTTP_MODE_VARREQSEND;
		// FALL THROUGH
	case CZHTTP_MODE_VARREQSEND:
		// HTTPに接続
#if 0
		//printf("perform curl\n");
		ret = curl_easy_perform(curl);
		if (ret == CURLE_OK) {
			//printf("curle ok\n");
			mode = CZHTTP_MODE_VARDATAEND;
		} else {
			//printf("curle error\n");
		        char buffer[256];
		        sprintf(buffer, "ダウンロード中にエラーが発生しました(%d:%s)", ret, curl_easy_strerror(ret));
			SetError(buffer);
		}
#else

		curl_multi_add_handle(mcurl, curl);
		mode = CZHTTP_MODE_VARDATAWAIT;
#endif
		// FALL THROUGH
	case CZHTTP_MODE_VARDATAWAIT:
		ret = curl_multi_perform(mcurl, &running);
		if (ret != CURLM_OK && ret != CURLM_CALL_MULTI_PERFORM) {
		        char buffer[256];
		        sprintf(buffer, "ダウンロード中にエラーが発生しました(%d:%s)", ret, curl_multi_strerror(ret));
			SetError(buffer);
			break;
		}
		if (running == 0) {
			struct CURLMsg *m;

			mode = CZHTTP_MODE_VARDATAEND;
			break;
		}
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

size_t WebTask::OnReceive(char* ptr, size_t size, size_t nmemb, void* stream) {
	//printf("curle receive %ld %ld\n", size, nmemb);
	WebTask* web = (WebTask*) stream;
	const size_t sizes = size * nmemb;

	size_t needed_size = size + sizes;
	if ( needed_size > web->varsize ) {
		while ( needed_size > web->varsize ) {
			web->varsize *= 2;
		}
		web->vardata = (char *)realloc( web->vardata, web->varsize );
		//printf("curle alloc %ld %ld\n", (size_t)web->vardata, web->varsize);
	}
	memcpy(web->vardata + web->size, ptr, sizes);
	web->size += sizes;
	//printf("curle received %ld\n", web->size);
	return sizes;
}
