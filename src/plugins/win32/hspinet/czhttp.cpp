
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include <tchar.h>

#include "czhttp.h"

#pragma comment( lib,"Wininet.lib" )

void CzHttp::Terminate( void )
{
	//	Delete Session
	if ( pt != NULL ) {
		free( pt );	pt = NULL;
	}
	if ( hSession != NULL ) {
		InternetCloseHandle( hSession );
		hSession = NULL;
	}
	errstr.clear();
	req_url.clear();
	req_path.clear();
	down_path.clear();
	req_header.clear();

	ClearVarData();
}


void CzHttp::Reset( void )
{
	// Initalize
	//
	char *agent;
	Terminate();

	agent = str_agent;
	if ( agent == NULL ) agent = "HSPInet(HSP3.6; Windows)";

	if ( proxy_url != "" ) {
		char *local_prm = NULL;
		if ( proxy_local ) local_prm = "<local>";
		hSession = InternetOpen( agent, INTERNET_OPEN_TYPE_PROXY, proxy_url.c_str(), local_prm, 0 );
	} else {
		hSession = InternetOpen( agent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
	}
	if ( hSession == NULL ) {
		mode = CZHTTP_MODE_NONE;
		SetError( "初期化に失敗しました" ); return;
	}

	username = "anonymous";
	userpass = "aaa@aaa.com";
	ftp_port = INTERNET_DEFAULT_FTP_PORT;
	ftp_flag = 0;

	// Reset Value
	mode = CZHTTP_MODE_READY;
}


CzHttp::CzHttp( void )
{
	//	コンストラクタ
	//
	str_agent = NULL;
	pt = NULL;
	mode = CZHTTP_MODE_NONE;
	hSession = NULL;
	proxy_local = 0;
	proxy_url.clear();
	req_header.clear();
	vardata = NULL;

	//	接続可能か?
	if( InternetAttemptConnect(0) ){
		SetError( "ネット接続が確認できませんでした" ); return;
	}

	//	初期化を行う
	Reset();
}


CzHttp::~CzHttp( void )
{
	//	デストラクタ
	//
	Terminate();
	if ( str_agent != NULL ) { free( str_agent ); str_agent = NULL; }
}


//--------------------------------------------------------------//
//				External functions
//--------------------------------------------------------------//

int CzHttp::Exec( void )
{
	//	毎フレーム実行
	//
	static char hdr[] = "Content-Type: application/x-www-form-urlencoded\r\n";
	std::string req_name;
	char *name;
	BOOL res;
	int flagmode = 0;

	switch( mode ) {
	case CZHTTP_MODE_REQUEST:			// httpに接続
		req_name = req_url;
		req_name += req_path;
		hService = InternetOpenUrl( hSession, req_name.c_str(), req_header.c_str(), -1L, 0, INTERNET_FLAG_RELOAD );
		if ( hService == NULL ) {
			SetError( "無効なURLが指定されました" );
			break;
		}
		mode = CZHTTP_MODE_REQSEND;
	case CZHTTP_MODE_REQSEND:
		name = (char *)down_path.c_str();
		if ( name[0] == 0 ) name = (char *)req_path.c_str();
		fp = fopen( name, "wb");
		if ( fp == NULL ) {
			SetError( "ダウンロードファイルが作成できません" );
			break;
		}
		size = 0;
		mode = CZHTTP_MODE_DATAWAIT;
	case CZHTTP_MODE_DATAWAIT:
		{
		DWORD dwBytesRead = INETBUF_MAX;
		if ( InternetReadFile( hService, buf, INETBUF_MAX, &dwBytesRead ) == 0 ) {
			fclose( fp );
			InternetCloseHandle( hService );
			SetError( "ダウンロード中にエラーが発生しました" );
			break;
		}
		if( dwBytesRead == 0 ) {
			mode = CZHTTP_MODE_DATAEND;
			break;
		}
		fwrite( buf, dwBytesRead, 1, fp );
		size += dwBytesRead;
		break;
		}
	case CZHTTP_MODE_DATAEND:
		fclose( fp );
		InternetCloseHandle( hService );
		mode = CZHTTP_MODE_READY;
		break;



	case CZHTTP_MODE_VARREQUEST:
		
		if (strncmp(req_url.c_str(), "https://", strlen("https://")) == 0){
			varport = INTERNET_DEFAULT_HTTPS_PORT;
			flagmode= INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_UI|INTERNET_FLAG_SECURE;
		}else{
			flagmode = INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_UI;
		}

		req_name = req_url2;
		req_name += req_path;

		// HTTPに接続
		hHttpSession = ::InternetConnectA( hSession, varserver.c_str(), varport, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
		if ( hHttpSession == NULL ) {
			SetError( "無効なサーバーが指定されました" );
			break;
		}

		// HTTP要求の作成
		hHttpRequest = ::HttpOpenRequestA( hHttpSession, varstr.c_str(), req_name.c_str(), HTTP_VERSION, NULL, NULL, flagmode, 0 );
		if ( hHttpSession == NULL ) {
			SetError( "無効なURLが指定されました" );
			break;
		}
		mode = CZHTTP_MODE_VARREQSEND;
		// FALL THROUGH
	case CZHTTP_MODE_VARREQSEND:

		// 作成したHTTP要求の発行
		if ( postdata != NULL ) {
			std::string header;
			header = hdr;
			header += req_header;
			res = ::HttpSendRequestA( hHttpRequest, header.c_str(), -1L, postdata, (int)postsize/*strlen(postdata)*/ );
		} else {
			res = ::HttpSendRequestA( hHttpRequest, req_header.c_str(), -1L, NULL, 0 );
		}
		if ( res == false ) {
			InternetCloseHandle( hHttpSession );
			SetError( "リクエストができませんでした" );
			break;
		}
		// HTTPヘッダ
		resphead = resphead_buf;
		resphead_size = HEADBUF_DEFAULT;
		resphead_index = 0;
		resphead_size_ptr = &resphead_size;
		HttpQueryInfo( hHttpRequest, HTTP_QUERY_RAW_HEADERS_CRLF, resphead, resphead_size_ptr, &resphead_index );
		varsize = 0x40000;
		ClearVarData();
		vardata = (char *)malloc( varsize );
		size = 0;
		mode = CZHTTP_MODE_VARDATAWAIT;
		// FALL THROUGH
	case CZHTTP_MODE_VARDATAWAIT:
		{
		DWORD dwBytesRead;
		int needed_size = size + INETBUF_MAX + 1;
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



	case CZHTTP_MODE_INFOREQ:
		req_name = req_url;
		req_name += req_path;
		hService = InternetOpenUrl( hSession, req_name.c_str(), req_header.c_str(), -1L, 0, 0 );
		if ( hService == NULL ) {
			SetError( "無効なURLが指定されました" );
			break;
		}
		mode = CZHTTP_MODE_INFORECV;
	case CZHTTP_MODE_INFORECV:
		{
		DWORD dwSize = INETBUF_MAX;
		buf[0] = 0;
		HttpQueryInfo( hService, HTTP_QUERY_RAW_HEADERS_CRLF, buf, &dwSize, 0 );
		InternetCloseHandle( hService );
		mode = CZHTTP_MODE_READY;
		break;
		}

	case CZHTTP_MODE_FTPREADY:
		return 2;

	case CZHTTP_MODE_FTPDIR:
		{
		char tx[512];
		char dname[64];
		char *fname;
		SYSTEMTIME t;

		fname = finddata.cFileName;
		if( finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			strcpy( dname, "<DIR>" );
        }
        else{
			sprintf( dname, "%d", finddata.nFileSizeLow );
        }
		FileTimeToSystemTime( &finddata.ftLastWriteTime,&t );
		sprintf( tx, "\"%s\",%s,%4d/%02d/%02d,%02d:%02d:%02d\r\n", fname, dname, t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond );
		AddFlexBuf( tx, (int)strlen(tx) );
		if ( !InternetFindNextFile( hFtpEnum, &finddata ) ) {
			InternetCloseHandle( hFtpEnum );
			mode = CZHTTP_MODE_FTPREADY;
		}
		break;
		}

	case CZHTTP_MODE_FTPREAD:
		break;
	case CZHTTP_MODE_FTPWRITE:
		break;
	case CZHTTP_MODE_FTPCMD:
		{
		char tx[1024];
	    DWORD dwBytesRead;
		if( InternetReadFile( hResponse, buf, 1024, &dwBytesRead ) ){
			AddFlexBuf( tx, dwBytesRead );
		}
		if ( dwBytesRead == 0 ) {
			InternetCloseHandle( hResponse );
			mode = CZHTTP_MODE_FTPREADY;
		}
		break;
		}
	case CZHTTP_MODE_FTPRESULT:
		GetFtpResponse();
		break;

	case CZHTTP_MODE_ERROR:
		return -1;
	default:
		return 1;
	}
	return 0;
}


void CzHttp::ClearVarData( void )
{
	if ( vardata == NULL ) return;
	free( vardata );
	vardata = NULL;
}


char *CzHttp::GetError( void )
{
	// エラー文字列のポインタを取得
	//
	return (char *)errstr.c_str();
}


void CzHttp::Resume(void)
{
	// エラーから復帰する
	//
	if (mode == CZHTTP_MODE_ERROR) {
		if (hSession == NULL) {
			Reset();
		}
		else {
			mode = CZHTTP_MODE_READY;
		}
	}
}


int CzHttp::RequestFile( char *path )
{
	// サーバーにファイルを要求
	//
	Resume();
	if ( mode != CZHTTP_MODE_READY ) {
		return -1;
	}
	req_path = path;
	mode = CZHTTP_MODE_REQUEST;
	return 0;
}

int CzHttp::GetRespHead( char *buff, LPDWORD size ){
	resphead = buff;
	resphead_size_ptr = size;
	return 0;
}

char *CzHttp::RequestFileInfo( char *path )
{
	// サーバーにファイル情報を要求
	//
	std::string req_name;
	DWORD dwSize = INETBUF_MAX;

	Resume();
	if ( mode != CZHTTP_MODE_READY ) {
		return NULL;
	}
	req_name = req_url;
	req_name += path;

	hService = InternetOpenUrl( hSession, req_name.c_str(), req_header.c_str(), -1L, 0, 0 );
	if ( hService == NULL ) return NULL;

	buf[0] = 0;
	resphead_index = 0;
	HttpQueryInfo( hService, HTTP_QUERY_RAW_HEADERS_CRLF, buf, &dwSize, &resphead_index);
	InternetCloseHandle( hService );
	return buf;
}


void CzHttp::SetURL( char *url )
{
	// サーバーのURLを設定
	//
	req_url = url;
}


void CzHttp::SetLocalName( char *name )
{
	// サーバーのURLを設定
	//
	down_path = name;
}


int CzHttp::GetSize( void )
{
	// 取得ファイルのサイズを返す
	//
	if ( mode != CZHTTP_MODE_READY ) return 0;
	return size;
}


char *CzHttp::GetData( void )
{
	// 取得ファイルデータへのポインタを返す
	//
	if ( mode != CZHTTP_MODE_READY ) return NULL;
	return pt;
}


void CzHttp::SetProxy( char *url, int port, int local )
{
	// PROXYサーバーの設定
	//	(URLにNULLを指定するとPROXY無効となる)
	//
	if ( url == NULL ) {
		proxy_url.clear();
	} else {
		char tmp[128];
		sprintf( tmp, "%s:%d", url, port );
		proxy_local = local;
		proxy_url = tmp;
	}
	Reset();
}


void CzHttp::SetAgent( char *agent )
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


void CzHttp::SetHeader( char *header )
{
	// ヘッダ文字列の設定
	//
	req_header = header;
}


void CzHttp::SetUserName( char *name )
{
	// ユーザー名の設定
	//
	username = name;
}


void CzHttp::SetUserPassword( char *pass )
{
	// パスワードの設定
	//
	userpass = pass;
}


void CzHttp::SetFtpPort( int port )
{
	// ポートの設定
	//
	ftp_port = port;
}


void CzHttp::SetFtpFlag( int flag )
{
	// パッシブモードの設定
	//
	if ( flag ) {
		ftp_flag = INTERNET_FLAG_PASSIVE;
	} else {
		ftp_flag = 0;
	}
}


void CzHttp::ResetFlexBuf( int defsize )
{
	if ( pt != NULL ) {	free( pt );	}
	pt = (char *)malloc( defsize );
	pt_size = defsize;
	pt_cur = 0;
}


void CzHttp::AllocFlexBuf( int size )
{
	char *p;
	if ( pt_size >= size ) return;
	p = (char *)malloc( size );
	memcpy( p, pt, pt_size );
	free( pt );
	pt_size = size;
	pt = p;
}


void CzHttp::AddFlexBuf( char *data, int size )
{
	int i;
	int req_size;
	i = pt_cur + size + 1;
	if ( i > pt_size ) {
		req_size = ( i + 0x0fff ) & 0x7ffff000;
		AllocFlexBuf( req_size );
	}
	memcpy( pt + pt_cur, data, size );
	pt_cur += size;
	pt[ pt_cur ] = 0;									// Terminate
}



void CzHttp::SetVarServerFromURL( void )
{
	char *p;
	char a1;
	char tmp[8];
	p = (char *)req_url.c_str();
	varserver.clear();
	tmp[1] = 0;
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
		tmp[0] = a1;
		varserver += tmp;
	}

	req_url2 = "/";

	while(1) {				// 最後まで取り出す
		a1 = *p++;
		if ( a1 == 0 ) break;
		tmp[0] = a1;
		req_url2 += tmp;
	}

}


void CzHttp::SetVarRequestGet( char *path )
{
	// サーバーにファイルを要求(GET)
	//
	Resume();
	if ( mode != CZHTTP_MODE_READY ) {
		return;
	}
	SetVarServerFromURL();
	varstr = "GET";
	req_path = path;
	postdata = NULL;
	mode = CZHTTP_MODE_VARREQUEST;
}


void CzHttp::SetVarRequestPost( char *path, char *post )
{
	// サーバーにファイルを要求(POST)
	//
	Resume();
	if ( mode != CZHTTP_MODE_READY ) {
		return;
	}
	SetVarServerFromURL();
	varstr = "POST";
	req_path = path;
	postdata = post;
	postsize = (int)strlen(postdata);
	mode = CZHTTP_MODE_VARREQUEST;
}

void CzHttp::SetVarRequestPost2( char *path, char *post, int size )
{
	// サーバーにファイルを要求(POST)
	//
	Resume();
	if ( mode != CZHTTP_MODE_READY ) {
		return;
	}
	SetVarServerFromURL();
	varstr = "POST";
	req_path = path;
	postdata = post;
	postsize = size;
	mode = CZHTTP_MODE_VARREQUEST;
}

void CzHttp::SetVarRequestPut(char *path, char *post)
{
	// サーバーにファイルを要求(PUT)
	//
	Resume();
	if (mode != CZHTTP_MODE_READY) {
		return;
	}
	SetVarServerFromURL();
	varstr = "PUT";
	req_path = path;
	postdata = post;
	postsize = (int)strlen(postdata);
	mode = CZHTTP_MODE_VARREQUEST;
}

void CzHttp::SetVarRequestPut2(char *path, char *post, int size)
{
	// サーバーにファイルを要求(PUT)
	//
	Resume();
	if (mode != CZHTTP_MODE_READY) {
		return;
	}
	SetVarServerFromURL();
	varstr = "PUT";
	req_path = path;
	postdata = post;
	postsize = size;
	mode = CZHTTP_MODE_VARREQUEST;
}

void CzHttp::SetVarRequestDelete(char *path)
{
	// サーバーにファイルを要求(DELETE)
	//
	Resume();
	if (mode != CZHTTP_MODE_READY) {
		return;
	}
	SetVarServerFromURL();
	varstr = "DELETE";
	req_path = path;
	postdata = NULL;
	mode = CZHTTP_MODE_VARREQUEST;
}

//--------------------------------------------------------------//
//				FTP functions
//--------------------------------------------------------------//

int CzHttp::FtpConnect( void )
{
	// FTP接続
	//
	*buf = 0;
	if ( mode != CZHTTP_MODE_READY ) {
		return -1;
	}

	hService = InternetConnect( hSession,
                                req_url.c_str(),
                                ftp_port,
                                username.c_str(),
                                userpass.c_str(),
                                INTERNET_SERVICE_FTP,
                                ftp_flag,
                                0 );


	GetFtpResponse();
	if ( mode != CZHTTP_MODE_FTPREADY ) return -1;
	return 0;
}


void CzHttp::FtpDisconnect( void )
{
	// FTP切断
	//
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return;
	}
	InternetCloseHandle( hService );
	mode = CZHTTP_MODE_READY;
}


char *CzHttp::GetFtpResponse( void )
{
	// FTPレスポンスへのポインタを返す
	//
	DWORD dwSize = INETBUF_MAX;
	DWORD dwError;
	if ( InternetGetLastResponseInfo( &dwError, buf, &dwSize ) ) {
		mode = CZHTTP_MODE_FTPREADY;
	} else {
		InternetCloseHandle( hService );
		mode = CZHTTP_MODE_ERROR;
	}
	return buf;
}


char *CzHttp::GetFtpCurrentDir( void )
{
	// FTPカレントディレクトリを取得
	//
	DWORD dwSize = INETBUF_MAX;

	if ( mode != CZHTTP_MODE_FTPREADY ) {
		*buf = 0;
		return buf;
	}
	FtpGetCurrentDirectory( hService, buf, &dwSize );
	GetFtpResponse();	// 順番入れ替え
	return buf;
}


void CzHttp::SetFtpDir( char *name )
{
	// FTPカレントディレクトリを変更
	//
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return;
	}
	FtpSetCurrentDirectory( hService, name );
	GetFtpResponse();
}


void CzHttp::GetFtpDirList( void )
{
	// FTPカレントディレクトリ内容を取得
	//
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return;
	}
	ResetFlexBuf( 1024 );
    hFtpEnum = FtpFindFirstFile( hService, NULL, &finddata, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0 );
	if ( hFtpEnum != NULL ) mode = CZHTTP_MODE_FTPDIR;
}


char *CzHttp::GetTempBuffer( void )
{
	// 内部バッファへのポインタを返す
	//
	return buf;
}


int CzHttp::GetFtpFile( char *name, char *downname, int tmode )
{
	// FTPファイルを取得
	//
	int i;
	DWORD type;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	type = FTP_TRANSFER_TYPE_BINARY;
	if ( tmode ) type = FTP_TRANSFER_TYPE_ASCII;
	i = FtpGetFile( hService, name, downname, FALSE, FILE_ATTRIBUTE_ARCHIVE, type, 0 );
	GetFtpResponse();	// 順番入れ替え
	if ( i == 0 ) return -1;
	return 0;
}


int CzHttp::PutFtpFile( char *name, char *downname, int tmode )
{
	// FTPファイルを送信
	//
	int i;
	DWORD type;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	type = FTP_TRANSFER_TYPE_BINARY;
	if ( tmode ) type = FTP_TRANSFER_TYPE_ASCII;
	i = FtpPutFile( hService, downname, name, type, 0 );
	GetFtpResponse();	// 順番入れ替え
	if ( i == 0 ) return -1;
	return 0;
}


int CzHttp::RenameFtpFile( char *name, char *newname )
{
	// FTPファイルをリネーム
	//
	int i;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	if ( newname == NULL ) {
		i = FtpDeleteFile( hService, name );
	} else {
		i = FtpRenameFile( hService, name, newname );
	}
	GetFtpResponse();	// 順番入れ替え
	if ( i == 0 ) return -1;
	return 0;
}


int CzHttp::MakeFtpDir( char *name )
{
	// FTPディレクトリ作成
	//
	int i;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	i = FtpCreateDirectory( hService, name );
	GetFtpResponse();	// 順番入れ替え
	if ( i == 0 ) return -1;
	return 0;
}


int CzHttp::KillFtpDir( char *name )
{
	// FTPディレクトリ削除
	//
	int i;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	i = FtpRemoveDirectory( hService, name );
	GetFtpResponse();	// 順番入れ替え
	if ( i == 0 ) return -1;
	return 0;
}


int CzHttp::FtpSendCommand( char *cmd )
{
	// FTPコマンド送信
	//
	int i;
	if ( mode != CZHTTP_MODE_FTPREADY ) {
		return -1;
	}
	ResetFlexBuf( 1024 );
	i = FtpCommand( hService, TRUE, FTP_TRANSFER_TYPE_ASCII, cmd, 0, &hResponse );
	if ( i ) {
		mode = CZHTTP_MODE_FTPCMD;
		return 0;
	}
	return -1;
}


//--------------------------------------------------------------//
//				Utility functions
//--------------------------------------------------------------//

int CzHttp::UrlEncode( char *dst, int dstsize, char *src )
{
	//	URLエンコード
	//
	int len;
	unsigned char *p;
	unsigned char *wrt;
	int a1;
	p = (unsigned char *)src;
	wrt = (unsigned char *)dst;
	len =0;
	while(1) {
		if ( len >= (dstsize-1) ) break;
		a1 = *p++;
		if ( a1 == 0 ) break;
		if(isalnum(a1) || a1 == ',' ||a1 == '-' || a1 == '_') {
			wrt[len++] = a1;
		} else if (a1 == ' ') {
			wrt[len++] = '+';
		} else {
			wrt[len++] = '%';
			sprintf( (char *)&wrt[len], "%02X",a1 );
			len+=2;
		}
	}
	wrt[len] = 0;
	return len;
}


int CzHttp::UrlDecode( char *dst, int dstsize, char *src )
{
	//	URLデコード
	//
	int len,val1,val2;
	unsigned char *p;
	unsigned char *wrt;
	int a1,a2;
	p = (unsigned char *)src;
	wrt = (unsigned char *)dst;
	len =0;
	while(1) {
		if ( len >= (dstsize-1) ) break;
		a1 = *p++;
		if ( a1 == 0 ) break;
		if( a1 == '%' ) {
			a1 = *p++;
			a2 = *p++;
			val1 = CheckHexCode( a1 );
			val2 = CheckHexCode( a2 );
			if (( val1 >= 0 )&&( val2 >= 0 )) {
				wrt[len++] = (val1<<4)+val2;
			} else {
				wrt[len++] = '%';
				wrt[len++] = a1;
				wrt[len++] = a2;
			}
		} else if (a1 == '+') {
			wrt[len++] = ' ';
		} else {
			wrt[len++] = a1;
		}
	}
	wrt[len] = 0;
	return len;
}


//--------------------------------------------------------------//
//				Internal functions
//--------------------------------------------------------------//

int CzHttp::CheckHexCode( int code )
{
	if (( code >= '0' )&&( code <='9' )) {
		return (code-'0');
	}
	if (( code >= 'a' )&&( code <='f' )) {
		return (code-'a'+10);
	}
	if (( code >= 'A' )&&( code <='F' )) {
		return (code-'A'+10);
	}
	return -1;
}


void CzHttp::SetError( char *mes )
{
	//	エラー文字列を設定
	//
	mode = CZHTTP_MODE_ERROR;
	errstr = mes;
}


