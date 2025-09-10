
//
//		HSP3.0 plugin sample
//		onion software/onitama 2005/5
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "czhttp.h"
#include "czcrypt.h"

#include "../hpi3sample/hsp3plugin.h"

#include "cJSON.h"
#include "nkf/nkf32.h"

/*----------------------------------------------------------------*/

static CzHttp *http;

#define NKFBUF_DEFAULTSIZE 0x8000
#define URLENCODE_BUFFERSIZE 0x40000

static	char *nkfbuf = NULL;	// nkf変換バッファ
static	int	nkfsize;			// nkf変換バッファサイズ
static	char json_nkfopt[128];	// 取得時のnkf変換
static	char json_nkfopt2[128];	// 設定時のnkf変換

/*------------------------------------------------------------*/
/*
		nkf related interface
*/
/*------------------------------------------------------------*/

static void termNkfBuf( void )
{
	if ( nkfbuf != NULL ) {
		free( nkfbuf );
		nkfbuf = NULL;
		nkfsize = 0;
	}
}

static void initNkfBuf( int size )
{
	termNkfBuf();
	nkfbuf = (char *)malloc( size );
	nkfsize = size;
}

static int cnvNkf( char *srcbuf, int insize, int outsize, char *opt )
{
	int size, bufsize, ressize;
	bufsize = outsize;
	if ( bufsize <= 0 ) bufsize = NKFBUF_DEFAULTSIZE;
	if ( bufsize > nkfsize ) {
		initNkfBuf( bufsize );
	}
	SetNkfOption( opt );
	size = insize;
	if ( size < 0 ) size = strlen( srcbuf );
	NkfConvertSafe( nkfbuf, bufsize, (LPDWORD)&ressize, srcbuf, size );
	return ressize;
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

int WINAPI DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		http = NULL;
		json_nkfopt[0] = 0;
		json_nkfopt2[0] = 0;
		initNkfBuf( NKFBUF_DEFAULTSIZE );
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		termNkfBuf();
		if ( http != NULL ) { delete http; http = NULL; }
	}
	return TRUE ;
}

 /*------------------------------------------------------------*/
/*
		controller
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI netinit( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	http = new CzHttp;
	if ( http->GetMode() != CZHTTP_MODE_READY ) {
		delete http; http = NULL;
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI netterm( int p1, int p2, int p3, int p4 )
{
	//	(type$00)
	if ( http != NULL ) { delete http; http = NULL; }
	return 0;
}


EXPORT BOOL WINAPI netexec( int *p1, int p2, int p3, int p4 )
{
	//	(type$01)
	*p1 = -1;
	if ( http == NULL ) return -1;
	*p1 = http->Exec();
	return 0;
}

#if 0
EXPORT BOOL WINAPI netexec( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	PVal *pv;
	APTR ap;
	int res;

	res = -1;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数

	if ( http != NULL ) {
		res = http->Exec();
	}

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &res );	// 変数に値を代入
	return 0;
}
#endif


EXPORT BOOL WINAPI netmode( int *p1, int p2, int p3, int p4 )
{
	//	(type$01)
	*p1 = -1;
	if ( http == NULL ) return -1;
	*p1 = http->GetMode();
	return 0;
}


EXPORT BOOL WINAPI netsize( int *p1, int p2, int p3, int p4 )
{
	//	(type$01)
	*p1 = -1;
	if ( http == NULL ) return -1;
	*p1 = http->GetSize();
	return 0;
}


EXPORT BOOL WINAPI neturl( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->SetURL( p2 );
	return 0;
}


EXPORT BOOL WINAPI netdlname( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->SetLocalName( p2 );
	return 0;
}


EXPORT BOOL WINAPI netproxy( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->SetProxy( p2, p3, p4 );
	return 0;
}


EXPORT BOOL WINAPI netagent( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->SetAgent( p2 );
	return 0;
}


EXPORT BOOL WINAPI netheader( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->SetHeader( p2 );
	return 0;
}


EXPORT BOOL WINAPI netrequest( BMSCR *p1, char *p2, int p3, int p4 )
{
	//	(type$06)
	if ( http == NULL ) return -1;
	http->RequestFile( p2 );
	return 0;
}


EXPORT BOOL WINAPI netfileinfo( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	エラー文字列を得る
	//	(変数にエラー文字列を代入)
	//		neterror 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	char *res;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列

	if ( http == NULL ) return -1;
	res = http->RequestFileInfo( ss );
	if ( res == NULL ) return -1;

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, res );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI neterror( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	エラー文字列を得る
	//	(変数にエラー文字列を代入)
	//		neterror 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	if ( http == NULL ) return -1;
	ss = http->GetError();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI filecrc( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	ファイルのCRCを求める
	//	(変数にCRC32、strsizeにファイルサイズを代入)
	//		filecrc 変数, ファイル名
	//
	PVal *pv;
	APTR ap;
	char fname[_MAX_PATH];
	char *ss;
	int i;
	int num;
	CzCrypt crypt;
	HSPCTX *ctx;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy( fname, ss, _MAX_PATH );
	i = crypt.DataLoad( fname );
	if ( i ) return -1;

	ctx = hei->hspctx;

	num = crypt.GetCRC32();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &num );	// 変数に値を代入
	ctx->strsize = crypt.GetSize();

	return 0;
}


EXPORT BOOL WINAPI filemd5( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	ファイルのMD5を求める
	//	(変数にMD5文字列、strsizeにファイルサイズを代入)
	//		filemd5 変数, ファイル名
	//
	PVal *pv;
	APTR ap;
	char fname[_MAX_PATH];
	char *ss;
	int i;
	char md5str[ 128 ];
	CzCrypt crypt;
	HSPCTX *ctx;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy( fname, ss, _MAX_PATH );
	i = crypt.DataLoad( fname );
	if ( i ) return -1;

	ctx = hei->hspctx;

	crypt.GetMD5( md5str );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, md5str );	// 変数に値を代入
	ctx->strsize = crypt.GetSize();

	return 0;
}


EXPORT BOOL WINAPI filesha256(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	ファイルのSHA256を求める
	//	(変数にSHA256文字列、strsizeにファイルサイズを代入)
	//		filesha256 変数, ファイル名
	//
	PVal* pv;
	APTR ap;
	char fname[_MAX_PATH];
	char* ss;
	int i;
	char sha256str[128];
	CzCrypt crypt;
	HSPCTX* ctx;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy(fname, ss, _MAX_PATH);
	i = crypt.DataLoad(fname);
	if (i) return -1;

	ctx = hei->hspctx;

	crypt.GetSHA256(sha256str);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, sha256str);	// 変数に値を代入
	ctx->strsize = crypt.GetSize();

	return 0;
}

/*----------------------------------------------------------------*/


EXPORT BOOL WINAPI ftpresult( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	FTPサーバー返信文字列を得る
	//	(変数に文字列を代入)
	//		ftpresult 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	if ( http == NULL ) return -1;
	ss = http->GetTempBuffer();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI ftpopen( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTP接続
	//		ftpopen "アドレス","ユーザー","パスワード",port,passive
	//
	int i;
	char *ss;
	int _p1,_p2;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	http->SetURL( ss );
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	http->SetUserName( ss );
	ss = hei->HspFunc_prm_gets();			// パラメータ3:文字列
	http->SetUserPassword( ss );
	_p1 = hei->HspFunc_prm_getdi(INTERNET_DEFAULT_FTP_PORT);		// パラメータ4:整数値
	_p2 = hei->HspFunc_prm_getdi(0);		// パラメータ5:整数値

	http->SetFtpPort( _p1 );
	http->SetFtpFlag( _p2 );
	i = http->FtpConnect();
	return i;
}


EXPORT BOOL WINAPI ftpclose( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTP切断
	//		ftpclose
	//
	http->FtpDisconnect();
	return 0;
}


EXPORT BOOL WINAPI ftpdir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	FTPサーバーディレクトリを得る
	//	(変数に文字列を代入)
	//		ftpdir 変数,変更dir
	//
	PVal *pv;
	APTR ap;
	char *ss;
	char *n;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	n = hei->HspFunc_prm_getds("");		// パラメータ2:文字列
	if ( http == NULL ) return -1;

	if ( *n != 0 ) {
		http->SetFtpDir( n );
	}
	ss = http->GetFtpCurrentDir();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI ftpdirlist( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイルリスト
	//		ftpdirlist
	//
	http->GetFtpDirList();
	return 0;
}


EXPORT BOOL WINAPI ftpdirlist2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイルリスト取得
	//		ftpdirlist2 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	int i;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	if ( http == NULL ) return -1;

	i = http->GetMode();
	if ( i == CZHTTP_MODE_FTPDIR ) return -3;
	if ( i != CZHTTP_MODE_FTPREADY ) return -2;

	ss = http->GetFlexBuffer();
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI ftpcmd( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPコマンド(結果はftpdirlist2で取得する)
	//		ftpcmd "command"
	//
	char *ss;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	return http->FtpSendCommand( ss );
}


EXPORT BOOL WINAPI ftprmdir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPディリクトリ削除
	//		ftprmdir "name"
	//
	char *ss;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	return http->KillFtpDir( ss );
}


EXPORT BOOL WINAPI ftpmkdir( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPディレクトリ作成
	//		ftpmkdir "name"
	//
	char *ss;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	return http->MakeFtpDir( ss );
}


EXPORT BOOL WINAPI ftpget( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイル取得
	//		ftpget "name","localname",mode
	//
	char name[256];
	char *ss;
	int _p1;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	strncpy( name, ss, 255 );
	ss = hei->HspFunc_prm_getds(name);		// パラメータ2:文字列
	_p1 = hei->HspFunc_prm_getdi(0);		// パラメータ3:整数値
	return http->GetFtpFile( name, ss, _p1 );
}


EXPORT BOOL WINAPI ftpput( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイル送信
	//		ftpput "name","localname",mode
	//
	char name[256];
	char *ss;
	int _p1;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	strncpy( name, ss, 255 );
	ss = hei->HspFunc_prm_getds(name);		// パラメータ2:文字列
	_p1 = hei->HspFunc_prm_getdi(0);		// パラメータ3:整数値
	return http->PutFtpFile( name, ss, _p1 );
}


EXPORT BOOL WINAPI ftprename( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイルリネーム
	//		ftpput "name","newname"
	//
	char name[256];
	char *ss;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	strncpy( name, ss, 255 );
	ss = hei->HspFunc_prm_getds(name);		// パラメータ2:文字列
	return http->RenameFtpFile( name, ss );
}


EXPORT BOOL WINAPI ftpdelete( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		FTPファイル削除
	//		ftpdelete "name"
	//
	char *ss;
	ss = hei->HspFunc_prm_gets();			// パラメータ1:文字列
	return http->RenameFtpFile( ss, NULL );
}


EXPORT BOOL WINAPI fencode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ファイル暗号化
	//		fencode "srcfile","newfile",seed1,seed2
	//
	int i;
	CzCrypt crypt;
	HSPCTX *ctx;
	char *ss;
	char fname1[_MAX_PATH];
	char fname2[_MAX_PATH];
	int seed1,seed2;

	ss = hei->HspFunc_prm_gets();				// パラメータ1:文字列
	strncpy( fname1, ss, _MAX_PATH -1 );
	ss = hei->HspFunc_prm_gets();				// パラメータ2:文字列
	strncpy( fname2, ss, _MAX_PATH -1 );
	seed1 = hei->HspFunc_prm_getdi(0);			// パラメータ3:整数値
	seed2 = hei->HspFunc_prm_getdi(0);			// パラメータ4:整数値

	ctx = hei->hspctx;
	i = crypt.DataLoad( fname1 );
	if ( i ) return -1;

	crypt.SetSeed( seed1, seed2 );
	crypt.Encrypt();
	i = crypt.DataSave( fname2 );
	if ( i ) return -1;

	ctx->strsize = crypt.GetSize();
	return 0;
}


EXPORT BOOL WINAPI fdecode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ファイル復号化
	//		fdecode "srcfile","newfile",seed1,seed2
	//
	int i;
	CzCrypt crypt;
	HSPCTX *ctx;
	char *ss;
	char fname1[_MAX_PATH];
	char fname2[_MAX_PATH];
	int seed1,seed2;

	ss = hei->HspFunc_prm_gets();				// パラメータ1:文字列
	strncpy( fname1, ss, _MAX_PATH -1 );
	ss = hei->HspFunc_prm_gets();				// パラメータ2:文字列
	strncpy( fname2, ss, _MAX_PATH -1 );
	seed1 = hei->HspFunc_prm_getdi(0);			// パラメータ3:整数値
	seed2 = hei->HspFunc_prm_getdi(0);			// パラメータ4:整数値

	ctx = hei->hspctx;
	i = crypt.DataLoad( fname1 );
	if ( i ) return -1;

	crypt.SetSeed( seed1, seed2 );
	crypt.Decrypt();
	i = crypt.DataSave( fname2 );
	if ( i ) return -1;

	ctx->strsize = crypt.GetSize();
	return 0;
}


/*------------------------------------------------------------------------------------*/


EXPORT BOOL WINAPI netgetv( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		取得データを文字列として変数に代入する
	//		netgetv 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	int size;
	char *varbase;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数

	if ( http == NULL ) return -1;

	//http->SetVarRequestGet( ss );
	ss = http->getVarData();
	size = http->getVarSize();
	if ( size < 63 ) size = 63;
	hei->HspFunc_dim( pv, TYPE_STRING, size+1, 0, 0, 0, 0 );
	varbase = (char *)pv->pt;
	memcpy( varbase, ss, size );								// 変数に値を代入
	//hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return -size;
}

EXPORT BOOL WINAPI netgetv4( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		取得データを文字列として変数に代入する
	//		netgetv 変数
	//
	PVal *pv;
	APTR ap;
	char *ss;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	//char *ep1;
	//ep1 = (char *)hei->HspFunc_prm_getv();

	if ( http == NULL ) return -1;

	//http->SetVarRequestGet( ss );
	ss = http->getVarData();
	hei->HspFunc_val_realloc( pv, http->getVarSize()+1, 0 );
	ap = (APTR)ss;
	//hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入
	return -http->getVarSize();
}

EXPORT int WINAPI netgetv_data(char *res)
{
	if ( http == NULL ) return -1;
	//MessageBoxA(0,http->getVarData(),"!",0);
	memcpy(res,http->getVarData(),http->getVarSize());
	//res = http->getVarData();
	return http->getVarSize();
}

EXPORT char * WINAPI netgetv_ptr()
{
	if (http == NULL) return NULL;
	return http->getVarData();
}

EXPORT int WINAPI netgetv_size()
{
	if (http == NULL) return -1;
	return http->getVarSize();
}

EXPORT int WINAPI netgetv_requestsize()
{
	if (http == NULL) return -1;
	return http->GetSize();
}

// レスポンスヘッダを取得
EXPORT int WINAPI netget_resphead(char *buff, LPDWORD size)
{
	if (http == NULL) return -1;
	return http->GetRespHead( buff, size );
}


EXPORT BOOL WINAPI netrequest_get( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_get "path"
	//
	char *ss;

	ss = hei->HspFunc_prm_gets();				// パラメータ1:文字列

	if ( http == NULL ) return -1;

	http->SetVarRequestGet( ss );
	return 0;
}


EXPORT BOOL WINAPI netrequest_post( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_post "path",var
	//
	char *ss;
	char *ap;
	ss = hei->HspFunc_prm_gets();					// パラメータ1:文字列
	ap = (char *)hei->HspFunc_prm_getv();			// パラメータ2:変数

	if ( http == NULL ) return -1;

	http->SetVarRequestPost( ss, ap );
	return 0;
}

EXPORT BOOL WINAPI netrequest_post2( char *path, char *data, int size )
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_post2 "path", var, varsize
	//

	if (http == NULL) return -1;

	http->SetVarRequestPost2( path, data, size );
	return 0;
}

EXPORT BOOL WINAPI netrequest_put(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_put "path",var
	//
	char *ss;
	char *ap;
	ss = hei->HspFunc_prm_gets();					// パラメータ1:文字列
	ap = (char *)hei->HspFunc_prm_getv();			// パラメータ2:変数

	if (http == NULL) return -1;

	http->SetVarRequestPut(ss, ap);
	return 0;
}

EXPORT BOOL WINAPI netrequest_put2(char *path, char *data, int size)
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_put2 "path", var, varsize
	//
	
	if (http == NULL) return -1;

	http->SetVarRequestPut2(path, data, size);
	return 0;
}

EXPORT BOOL WINAPI netrequest_delete(HSPEXINFO *hei, int p1, int p2, int p3)
{
	//	(type$202)
	//		ファイルデータをメモリに取得する(netgetvで取得)
	//		netrequest_delete "path"
	//
	char *ss;

	ss = hei->HspFunc_prm_gets();				// パラメータ1:文字列

	if (http == NULL) return -1;

	http->SetVarRequestDelete(ss);
	return 0;
}


/*------------------------------------------------------------------------------------*/

EXPORT BOOL WINAPI varmd5( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	変数バッファの内容からMD5を求める
	//	(変数にMD5文字列を代入)
	//		varmd5 変数, バッファ変数, サイズ
	//
	PVal *pv;
	APTR ap;
	int size;
	char md5str[ 128 ];
	CzCrypt crypt;
	char *vptr;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	vptr = (char *)hei->HspFunc_prm_getv();		// パラメータ2:変数
	size = hei->HspFunc_prm_getdi(0);			// パラメータ3:整数値

	crypt.GetMD5ext( md5str, vptr, size );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, md5str );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI varsha256(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	変数バッファの内容からSHA256を求める
	//	(変数にSHA256文字列を代入)
	//		varmd5 変数, バッファ変数, サイズ
	//
	PVal* pv;
	APTR ap;
	int size;
	char sha256str[128];
	CzCrypt crypt;
	char* vptr;

	ap = hei->HspFunc_prm_getva(&pv);			// パラメータ1:変数
	vptr = (char*)hei->HspFunc_prm_getv();		// パラメータ2:変数
	size = hei->HspFunc_prm_getdi(0);			// パラメータ3:整数値

	crypt.GetSHA256ext(sha256str, vptr, size);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_STR, sha256str);	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI b64encode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	変数バッファの内容をBASE64にエンコードする
	//	(変数にBASE64文字列を代入)
	//		b64encode 変数, バッファ変数, サイズ
	//
	PVal *pv;
	APTR ap;
	int size;
	CzCrypt crypt;
	char *dst;
	char *vptr;

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	vptr = (char *)hei->HspFunc_prm_getv();		// パラメータ2:変数
	size = hei->HspFunc_prm_getdi(-1);			// パラメータ3:整数値

	if ( size < 0 ) size = (int)strlen(vptr);
	dst = (char*)malloc( crypt.GetBASE64Size( size ) + 1 );
	crypt.EncodeBASE64( dst, vptr, size );

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, dst );	// 変数に値を代入
	free( dst );

	return 0;
}


EXPORT BOOL WINAPI b64decode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	変数バッファのBASE64文字列をデコードする
	//	(変数に変換後文字列を代入)
	//		b64decode 変数, バッファ変数, サイズ
	//
	int size;
	CzCrypt crypt;
	char *vptr;
	char *dstptr;

	dstptr = (char *)hei->HspFunc_prm_getv();	// パラメータ1:変数
	vptr = (char *)hei->HspFunc_prm_getv();		// パラメータ2:変数
	size = hei->HspFunc_prm_getdi(-1);			// パラメータ3:整数値

	if ( size < 0 ) size = (int)strlen(vptr);
	crypt.DecodeBASE64( dstptr, vptr, size );
	return 0;
}


EXPORT BOOL WINAPI rc4encode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	変数バッファのBASE64文字列をデコードする
	//	(変数に変換後文字列を代入)
	//		rc4encode バッファ変数, キー
	//
	CzCrypt crypt;
	char *dstptr;
	char *ss;
	int size;

	dstptr = (char *)hei->HspFunc_prm_getv();	// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();				// パラメータ2:文字列
	size = hei->HspFunc_prm_getdi(-1);			// パラメータ3:整数値
	crypt.EncodeRC4( dstptr, ss, size );
	return 0;
}


EXPORT BOOL WINAPI urlencode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	文字列をURLエンコードする
	//	(変数に変換後文字列を代入)
	//		urlencode 変数, "文字列"
	//
	char *ss;
	PVal *pv;
	APTR ap;
	char urltmp[URLENCODE_BUFFERSIZE];

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();				// パラメータ2:文字列
	http->UrlEncode( urltmp, URLENCODE_BUFFERSIZE, ss );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, urltmp );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI urldecode( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	文字列をURLデコードする
	//	(変数に変換後文字列を代入)
	//		urldecode 変数, "文字列"
	//
	char *ss;
	PVal *pv;
	APTR ap;
	char urltmp[4096];

	ap = hei->HspFunc_prm_getva( &pv );			// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();				// パラメータ2:文字列
	http->UrlDecode( urltmp, 4096, ss );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, urltmp );	// 変数に値を代入
	return 0;
}


/*------------------------------------------------------------------------------------*/

static	cJSON *json = NULL;

EXPORT BOOL WINAPI jsonopen( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		JSON形式を開く
	//		jsonopen 変数,バッファ変数
	//
	char *ptr;
	PVal *pv;
	APTR ap;
	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数

	if ( json != NULL ) {
		cJSON_Delete( json );
	}

	json = cJSON_Parse(ptr);
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &json );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI jsonclose( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		JSON形式を閉じる
	//		jsonclose
	//
	cJSON_Delete( json );
	json = NULL;
	return 0;
}


EXPORT BOOL WINAPI jsonout( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//		JSON形式データを変数に出力する
	//		jsonout 変数, JSONポインタ
	//
	PVal *pv;
	APTR ap;
	char *out;
	int _p3;
	cJSON *root;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ2:整数値

	if ( _p3 == 0 ) return -1;
	if ( _p3 < 0 ) {
		root = json;
	} else {
		root = (cJSON *)_p3;
	}
	out = cJSON_Print(root);
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, out );	// 変数に値を代入
	free(out);	
	return 0;
}


EXPORT BOOL WINAPI jsongetobj(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトを得る
	//		jsongetobj 変数, "名称", JSONポインタ
	//
	PVal* pv;
	APTR ap;
	char* ss;
	int _p3;
	char name[1024];
	cJSON* root;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if (_p3 == 0) return -1;
	if (_p3 < 0) {
		root = json;
	}
	else {
		root = (cJSON*)_p3;
	}
	cJSON* cjobj = cJSON_GetObjectItem(root, name);
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &cjobj);	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI jsongetarrayobj(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSON配列オブジェクト内のオブジェクトを得る
	//		jsongetarray 変数, JSONポインタ, 要素
	//
	PVal* pv;
	APTR ap;
	int _p3;
	int _p4;
	int size;
	cJSON* root;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ2:整数値
	_p4 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if (_p3 == 0) return -1;
	if (_p3 < 0) {
		root = json;
	}
	else {
		root = (cJSON*)_p3;
	}
	if (_p4 < 0) {
		size = cJSON_GetArraySize(root);
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &size);	// 変数に値を代入
	}
	else {
		cJSON* cjobj = cJSON_GetArrayItem(root, _p4);
		hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &cjobj);	// 変数に値を代入
	}
	return 0;
}


EXPORT BOOL WINAPI jsongetarrayobjbyname(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSON配列オブジェクト内のオブジェクトを得る
	//		jsongetarrayname 変数, "名称", JSONポインタ
	//
	PVal* pv;
	APTR ap;
	char* ss;
	int _p3;
	char name[1024];
	int size,i;
	cJSON* root;
	cJSON* arrayroot;
	cJSON* cjobj;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ss = hei->HspFunc_prm_getds("");		// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if (_p3 == 0) return -1;
	if (_p3 < 0) {
		root = json;
	}
	else {
		root = (cJSON*)_p3;
	}
	size = cJSON_GetArraySize(root);
	if (size <= 0) return -2;

	for (i = 0; i < size; i++) {
		arrayroot = cJSON_GetArrayItem(root, i);
		if (arrayroot) {
			cjobj = cJSON_GetObjectItem(arrayroot, name);
			if (cjobj) {
				hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &cjobj);	// 変数に値を代入
				return 0;
			}
		}
	}
	return -3;
}


EXPORT BOOL WINAPI jsonnext( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONオブジェクトを得る
	//		jsonnext 変数, JSONポインタ, option
	//
	PVal *pv;
	APTR ap;
	int _p3;
	int _p4;
	cJSON *root;
	cJSON *cjobj;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ2:整数値
	_p4 = hei->HspFunc_prm_getdi(0);		// パラメータ3:整数値

	if ( _p3 == 0 ) return -1;
	if ( _p3 < 0 ) {
		root = json;
	} else {
		root = (cJSON *)_p3;
	}

	switch( _p4 ) {
	case 1:
		cjobj = root->prev;
		break;
	case 2:
		cjobj = root->child;
		break;
	case 3:
		hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &root->type );	// 変数に値を代入
		return 0;
	default:
		cjobj = root->next;
		break;
	}
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &cjobj );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI jsongets( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONオブジェクトから文字列を得る
	//		jsongets 変数, "名称", JSONポインタ
	//
	PVal *pv;
	APTR ap;
	char *ss;
	int _p3;
	char name[1024];
	cJSON *root;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ss = hei->HspFunc_prm_getds( "" );		// パラメータ2:文字列
	strncpy( name, ss, 1024 );
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if ( _p3 == 0 ) return -1;
	if ( _p3 < 0 ) {
		root = json;
	} else {
		root = (cJSON *)_p3;
	}
	if ( *ss != 0 ) {
		root = cJSON_GetObjectItem( root, name );
		if ( root == NULL ) return -1;
	}

	char* mystr = NULL;
	switch (root->type)
	{
	case cJSON_String:
		mystr = root->valuestring;
		if (*json_nkfopt != 0) {
			cnvNkf(mystr, -1, -1, json_nkfopt);
			mystr = nkfbuf;
		}
		break;
	case cJSON_True:
		mystr = "True";
		break;
	case cJSON_False:
		mystr = "False";
		break;
	case cJSON_Number:
		mystr = "Number";
		break;
	case cJSON_Array:
		mystr = "Array";
		break;
	default:
		break;
	}
	if (mystr == NULL) return -1;
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, mystr);	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI jsongeti(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトから整数値を得る
	//		jsongeti 変数, "名称", JSONポインタ
	//
	PVal* pv;
	APTR ap;
	char* ss;
	int _p3;
	char name[1024];
	cJSON* root;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ss = hei->HspFunc_prm_getds("");		// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if (_p3 == 0) return -1;
	if (_p3 < 0) {
		root = json;
	}
	else {
		root = (cJSON*)_p3;
	}
	if (*ss != 0) {
		root = cJSON_GetObjectItem(root, name);
	}
	if (root == NULL) return -1;
	if (root->type != cJSON_Number) {
		return -1;
	}
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_INT, &root->valueint);	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI jsongetd(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトから実数値を得る
	//		jsongetd 変数, "名称", JSONポインタ
	//
	PVal* pv;
	APTR ap;
	char* ss;
	int _p3;
	char name[1024];
	cJSON* root;

	ap = hei->HspFunc_prm_getva(&pv);		// パラメータ1:変数
	ss = hei->HspFunc_prm_getds("");		// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ3:整数値

	if (_p3 == 0) return -1;
	if (_p3 < 0) {
		root = json;
	}
	else {
		root = (cJSON*)_p3;
	}
	if (*ss != 0) {
		root = cJSON_GetObjectItem(root, name);
	}
	if (root == NULL) return -1;
	if (root->type != cJSON_Number) {
		return -1;
	}
	hei->HspFunc_prm_setva(pv, ap, HSPVAR_FLAG_DOUBLE, &root->valuedouble);	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI jsonnewobj( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONオブジェクトを新規作成
	//		jsonnewobj 変数, JSONポインタ, "名称"
	//
	PVal *pv;
	APTR ap;
	char *ss;
	int _p3;
	char name[1024];
	cJSON *root;
	cJSON *cjobj;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	_p3 = hei->HspFunc_prm_getdi(-1);		// パラメータ2:整数値
	ss = hei->HspFunc_prm_getds( "" );		// パラメータ3:文字列
	strncpy( name, ss, 1024 );

	root = cJSON_CreateObject();
	if ( _p3 == 0 ) return -1;
	if ( _p3 < 0 ) {
		cjobj = root;
	} else {
		cjobj = (cJSON *)_p3;
		cJSON_AddItemToObject( cjobj, name, root );
	}

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_INT, &cjobj );	// 変数に値を代入
	return 0;
}


EXPORT BOOL WINAPI jsonputs( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONオブジェクトの文字列エントリ更新
	//		jsonputs JSONポインタ, "名称", "文字列"
	//
	char *ss;
	int _p3;
	char name[1024];
	cJSON *cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy( name, ss, 1024 );
	ss = hei->HspFunc_prm_gets();			// パラメータ3:文字列

	if ( _p3 <= 0 ) return -1;

	cjobj = (cJSON *)_p3;

	if ( *json_nkfopt2 != 0 ) {
		cnvNkf( ss, -1, -1, json_nkfopt2 );
		cJSON_AddStringToObject( cjobj, name, nkfbuf );
	} else {
		cJSON_AddStringToObject( cjobj, name, ss );
	}

	return 0;
}


EXPORT BOOL WINAPI jsonputi(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトの数値エントリ更新
	//		jsonputi JSONポインタ, "名称", 整数値, モード
	//
	char* ss;
	int _p3;
	int _p4;
	int _p5;
	char name[1024];
	cJSON* cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p4 = hei->HspFunc_prm_getdi(0);		// パラメータ3:整数値
	_p5 = hei->HspFunc_prm_getdi(0);		// パラメータ4:整数値

	if (_p3 <= 0) return -1;

	cjobj = (cJSON*)_p3;
	switch (_p5) {
	case 1:
		if (_p4) {
			cJSON_AddTrueToObject(cjobj, name);
		}
		else {
			cJSON_AddFalseToObject(cjobj, name);
		}
		break;
	default:
		cJSON_AddNumberToObject(cjobj, name, _p4);
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI jsonputd(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトの数値エントリ更新
	//		jsonputd JSONポインタ, "名称", 実数値
	//
	char* ss;
	int _p3;
	double _p4;
	char name[1024];
	cJSON* cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値
	ss = hei->HspFunc_prm_gets();			// パラメータ2:文字列
	strncpy(name, ss, 1024);
	_p4 = hei->HspFunc_prm_getdd(0.0);		// パラメータ3:実数値

	if (_p3 <= 0) return -1;

	cjobj = (cJSON*)_p3;
	cJSON_AddNumberToObject(cjobj, name, _p4);
	return 0;
}


EXPORT BOOL WINAPI jsonsetprm(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトのパラメーター設定
	//		jsonsetprm JSONポインタ, 設定値, モード
	//
	int _p3;
	int _p4;
	int _p5;
	cJSON* cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値
	_p4 = hei->HspFunc_prm_getdi(0);		// パラメータ2:整数値
	_p5 = hei->HspFunc_prm_getdi(0);		// パラメータ3:整数値

	if (_p3 <= 0) return -1;

	cjobj = (cJSON*)_p3;
	switch (_p5) {
	case 1:
		cjobj->prev = (cJSON*)_p4;
		break;
	case 2:
		cjobj->child = (cJSON*)_p4;
		break;
	case 3:
		cjobj->type = _p4;
		break;
	case 4:
		cjobj->valueint = _p4;
		cjobj->valuedouble = (double)_p4;
		cjobj->type = cJSON_Number;
		break;
	default:
		cjobj->next = (cJSON*)_p4;
		break;
	}
	return 0;
}


EXPORT BOOL WINAPI jsonsetprmd(HSPEXINFO* hei, int p1, int p2, int p3)
{
	//	(type$202)
	//	JSONオブジェクトのパラメーター設定
	//		jsonsetprmd JSONポインタ, 設定値(実数)
	//
	int _p3;
	double _p4;
	cJSON* cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値
	_p4 = hei->HspFunc_prm_getdd(0.0);		// パラメータ2:実数値

	if (_p3 <= 0) return -1;

	cjobj = (cJSON*)_p3;
	cjobj->valuedouble = _p4;
	cjobj->valueint = (int)_p4;
	cjobj->type = cJSON_Number;
	return 0;
}


EXPORT BOOL WINAPI jsondelobj( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONオブジェクトを削除
	//		jsondelobj JSONポインタ
	//
	int _p3;
	cJSON *cjobj;

	_p3 = hei->HspFunc_prm_getdi(0);		// パラメータ1:整数値

	if ( _p3 <= 0 ) return -1;
	cjobj = (cJSON *)_p3;
	cJSON_Delete( cjobj );
	return 0;
}


EXPORT BOOL WINAPI jsonnkf( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	JSONデータ変換オプション指定
	//		jsonnkf "取得時変換オプション", "設定時変換オプション"
	//
	char *ss;

	ss = hei->HspFunc_prm_getds( "" );		// パラメータ1:文字列
	strncpy( json_nkfopt, ss, 127 );
	ss = hei->HspFunc_prm_getds( "" );		// パラメータ2:文字列
	strncpy( json_nkfopt2, ss, 127 );

	return 0;
}


/*------------------------------------------------------------------------------------*/

EXPORT BOOL WINAPI nkfcnv( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	NKFによる文字コード変換を行なう
	//		nkfcnv 変数, 変数, "オプション", insize, outsize
	//
	PVal *pv;
	APTR ap;
	char *ss;
	char *vptr;
	int outsize, insize;
	char opt[1024];
//	cJSON *root;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	vptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数
	ss = hei->HspFunc_prm_getds( "s" );		// パラメータ3:文字列
	strncpy( opt, ss, 1024 );
	insize = hei->HspFunc_prm_getdi(-1);	// パラメータ3:整数値
	outsize = hei->HspFunc_prm_getdi(-1);	// パラメータ3:整数値

	cnvNkf( vptr, insize, outsize, opt );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, nkfbuf );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI nkfguess( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	NKFによる文字コード認識結果を取得する
	//		nkfguess 変数
	//
	PVal *pv;
	APTR ap;
	int ressize;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	GetNkfGuessA( nkfbuf, nkfsize, (LPDWORD)&ressize );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, nkfbuf );	// 変数に値を代入

	return 0;
}




/*------------------------------------------------------------------------------------*/

static char *strstr2( char *target, char *src )
{
	//		strstr関数の全角対応版
	//
	unsigned char *p;
	unsigned char *s;
	unsigned char *p2;
	unsigned char a1;
	unsigned char a2;
	unsigned char a3;
	p=(unsigned char *)target;
	if (( *src==0 )||( *target==0 )) return NULL;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		p2 = p;
		s=(unsigned char *)src;
		while(1) {
			a2=*s++;if (a2==0) return (char *)p;
			a3=*p2++;if (a3==0) break;
			if (a2!=a3) break;
		}
		p++;							// 検索位置を移動
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
	}
	return NULL;
}

static char *strchr2( char *target, char code )
{
	//		str中最後のcode位置を探す(全角対応版)
	//
	unsigned char *p;
	unsigned char a1;
	char *res;
	p=(unsigned char *)target;
	res = NULL;
	while(1) {
		a1=*p;if ( a1==0 ) break;
		if ( a1==code ) res=(char *)p;
		p++;							// 検索位置を移動
		if (a1>=129) {					// 全角文字チェック
			if ((a1<=159)||(a1>=224)) p++;
		}
	}
	return res;
}

static void CutLastChr( char *p, char code )
{
	//		最後のcodeを取り除く
	//
	char *ss;
	char *ss2;
	int i;
	ss = strchr2( p, code );
	if ( ss != NULL ) {
		i = (int)strlen( p ); ss2 = p + i -1;
		if ( ss == ss2 ) *ss = 0;
	}
}

static	int splc;	// split pointer

static void strsp_ini( void )
{
	splc=0;
}

static int strsp_getptr( void )
{
	return splc;
}

static int strsp_get( char *srcstr, char *dststr, char splitchr, int len )
{
	//		split string with parameters
	//
	unsigned char a1;
	unsigned char a2;
	int a;
	int sjflg;
	a=0;sjflg=0;
	while(1) {
		sjflg=0;
		a1=srcstr[splc];
		if (a1==0) break;
		splc++;
		if (a1>=0x81) if (a1<0xa0) sjflg++;
		if (a1>=0xe0) sjflg++;

		if (a1==splitchr) break;
		if (a1==13) {
			a2=srcstr[splc];
			if (a2==10) splc++;
			break;
		}
		dststr[a++]=a1;
		if (sjflg) {
			dststr[a++]=srcstr[splc++];
		}
		if ( a>=len ) break;
	}
	dststr[a]=0;
	if ( a1 == splitchr ) {
		while(1) {
			sjflg=0;
			a1=srcstr[splc];
			if (a1==0) break;
			if (a1==13) break;
			if (a1>=0x81) if (a1<0xa0) sjflg++;
			if (a1>=0xe0) sjflg++;

			if (( sjflg == 0 )&&( a1 != splitchr )) break;
			if ( sjflg ) splc++;
			splc++;
		}
	}
	return (int)a1;
}




EXPORT BOOL WINAPI getenv2( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	システム環境変数を取得します
	//		getenv2 変数, "環境変数名"
	//
	PVal *pv;
	APTR ap;
	char *ss;
	char buf[0x8000];

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	ss = hei->HspFunc_prm_gets();		// パラメータ2:文字列

	*buf = 0;
	GetEnvironmentVariable( ss, buf, 0x7fff );
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, buf );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI getctime( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	システム環境変数を取得します
	//		getctime 変数, mode
	//
	PVal *pv;
	APTR ap;
	char *ss;
	char buf[128];
	time_t timer;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	p1 = hei->HspFunc_prm_getdi(0);			// パラメータ2:数値

	switch( p1 ) {
	case 0:
		time( &timer );
		ss = ctime( &timer );
		CutLastChr( ss, 10 );
		break;
	case 1:
		time( &timer );
		ss = asctime( gmtime( &timer ) );
		CutLastChr( ss, 10 );
		break;
	case 2:
		{
		char tm1[32];
		char tm2[32];
		char tm3[32];
		char tm4[32];
		char tm5[32];
		int tm3_i;
		time( &timer );
		ss = asctime( gmtime( &timer ) );
		CutLastChr( ss, 10 );

		strsp_ini();
		strsp_get( ss, tm1, ' ', 31 );
		strsp_get( ss, tm2, ' ', 31 );
		strsp_get( ss, tm3, ' ', 31 );
		strsp_get( ss, tm4, ' ', 31 );
		strsp_get( ss, tm5, ' ', 31 );
		tm3_i = atoi( (const char *)tm3 );

		sprintf( buf,"%s, %02d %s %s %s GMT", tm1, tm3_i, tm2, tm5, tm4 );
		ss = buf;
		break;
		}
	default:
		ss = "";
		break;
	}
	//asctime
	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, ss );	// 変数に値を代入

	return 0;
}


EXPORT BOOL WINAPI getenvprm( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//	(type$202)
	//	パラメーターを取得します
	//		getenvprm 変数, データ変数, "要素名", 区切り記号
	//
	PVal *pv;
	APTR ap;
	char *vptr;
	char *ss;
	char *res;
	char src[256];
	char buf[0x8000];
	int slen;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	vptr = (char *)hei->HspFunc_prm_getv();	// パラメータ2:変数
	ss = hei->HspFunc_prm_gets();			// パラメータ3:文字列
	strncpy( src, ss, 255 ); 
	slen = (int)strlen( src );
	p1 = hei->HspFunc_prm_getdi('&');			// パラメータ4:数値

	*buf = 0;
	res = strstr2( vptr, src );
	if ( res != NULL ) {
		strsp_ini();
		strsp_get( res+slen, buf, p1, 0x7fff );
	}

	hei->HspFunc_prm_setva( pv, ap, HSPVAR_FLAG_STR, buf );	// 変数に値を代入

	return 0;
}

