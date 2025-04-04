
//
//		Advanced Communications Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>
#include <winsock.h>
#include <process.h>
#include "hspdll.h"


int WINAPI hspsock_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
     {
     return TRUE ;
     }


/*------------------------------------------------------------*/
/*
		TCP/IP socket routines
*/
/*------------------------------------------------------------*/

#define SOCKMAX 32
static	int sockf=0;
static	SOCKET soc[SOCKMAX];		/* Soket Descriptor */
static	SOCKET socsv[SOCKMAX];		/* Soket Descriptor (server) */
static	int svstat[SOCKMAX];		/* Soket Thread Status (server) */
static	HANDLE svth[SOCKMAX];		/* Soket Thread Handle (server) */


int sockprep( void )
{
	WORD wVersionRequested;
	int  a,err;
	WSADATA wsaData;

	/* WinSockの初期化を行う */
	wVersionRequested = MAKEWORD(1, 1);		/* バージョン 1.1 を要求する */
	err=WSAStartup(wVersionRequested, &wsaData);

	//if (atexit((void (*)(void))(WSACleanup))) {		/* 終了時にWinSockのリソースを解放するようにしておく */
	//	return 1;
	//}
	if ( err!=0 ) return -1;
	sockf++;
	for(a=0;a<SOCKMAX;a++) { soc[a]=-1;socsv[a]=-1;svstat[a]=0; }
	return 0;
}


EXPORT BOOL WINAPI sockopen ( int p1, char *p2, int p3, int p4 )
{
	unsigned long serveraddr;		/* サーバのIPアドレス */
	struct hostent *serverhostent;	/* サーバのホスト情報を指すポインタ */
	struct  sockaddr_in     serversockaddr;		/* サーバのアドレス */
	int port;

	port=p3;
	if (sockf==0) {
		if ( sockprep() ) return -1;
	}
	socsv[p1] = -1;
	soc[p1] = socket(PF_INET, SOCK_STREAM, 0);
	if (soc[p1] == INVALID_SOCKET) return -2;

	/* Nameにドットで区切った10進数のIPアドレスが入っている場合、serveraddrに32bit整数のIPアドレスが返ります */
	serveraddr = inet_addr(p2);
	if (serveraddr == -1) {
		/* サーバ名からサーバのホスト情報を取得します */
		serverhostent = gethostbyname(p2);
		if (serverhostent == NULL) {
			return -3;
		}else{
			/* サーバのホスト情報からIPアドレスをserveraddrにコピーします */
			serveraddr = *((unsigned long *)((serverhostent->h_addr_list)[0]));
		}
	}

	/* サーバのアドレスの構造体にサーバのIPアドレスとポート番号を設定します */
	serversockaddr.sin_family       = AF_INET;		/* インターネットの場合 */
	serversockaddr.sin_addr.s_addr  = serveraddr;	/* サーバのIPアドレス */
	serversockaddr.sin_port         = htons((unsigned short)port);		/* ポート番号 */
	memset(serversockaddr.sin_zero,(int)0,sizeof(serversockaddr.sin_zero));
	/* 指定のソケットでサーバへコネクトします */
	if(connect(soc[p1],(struct sockaddr *)&serversockaddr,sizeof(serversockaddr)) == SOCKET_ERROR)
	{
		return -4;
	}

	return 0;
}


DWORD WINAPI fnSockListen( LPVOID pVoid )
{
	//		listen待機用スレッド
	//
	int p1;
	p1=(int)pVoid;

	if ( listen(socsv[p1],5) ) {
		svstat[p1]=-1;
		return 0;
	}
	svstat[p1]++;
	return 0;
}


EXPORT BOOL WINAPI sockmake ( int p1, int p2, int p3, char *p4 )
{
	//		sockmake id,port ( type $10 )
	//
	//
	SOCKADDR_IN	addr;		/* サーバのアドレス */
	SOCKADDR_IN	from;		/* クライアントのアドレス */
	SOCKET mysoc;
	int port,len;
//	char s1[128];
//	char s2[128];
//	DWORD dwThreadId=0;
//	HANDLE hThread;
	
	port=p2;
	if (sockf==0) {
		if ( sockprep() ) return -1;
	}

	if ( socsv[p1] == -1 ) {
		mysoc = socket(PF_INET, SOCK_STREAM, 0);
		if (mysoc == INVALID_SOCKET) return -2;
		socsv[p1]=mysoc;

		len=sizeof(from);
		memset( &from,0,len );
		memset( &addr,0,len );

		addr.sin_family		 = AF_INET;						/* インターネットの場合 */
		addr.sin_port		 = htons((unsigned short)port);	/* ポート番号 */
		addr.sin_addr.s_addr = INADDR_ANY;					/* サーバのIPアドレス */

		if ( bind( mysoc,(LPSOCKADDR)&addr,sizeof(addr) ) ) return -3;

	} else {
		mysoc = socsv[p1];
	}

	svstat[p1]=1;
	fnSockListen( (LPVOID)p1 );
	//hThread = CreateThread( NULL, 0, fnSockListen, (LPVOID)p1, 0, &dwThreadId );
	//svth[ p1 ] = hThread;
/*
	if ( listen(mysoc,5) ) return -4;

	soc[p1]=accept(mysoc,(LPSOCKADDR)&from,&len);
	if (soc[p1] == INVALID_SOCKET) return -5;

	//(dllサイズ削減のため以下のコードに差し替え)
	//sprintf( s1,"%s:%d",inet_ntoa(from.sin_addr),ntohs(from.sin_port) );

	strcpy( s1, inet_ntoa(from.sin_addr) );
	strcat( s1,":" );
	itoa( ntohs(from.sin_port),s2,10 );
	strcat( s1,s2 );
	strcpy( p4,s1 );
*/  
	return 0;
}


EXPORT BOOL WINAPI sockwait ( int p1, int p2, int p3, char *p4 )
{
	//		sockwait id ( type $10 )
	//
	//
	int len;
    SOCKET soc2;
	SOCKADDR_IN	from;		/* クライアントのアドレス */
	char s1[128];
	char s2[128];
	int err;
	fd_set fdsetread,fdsetwrite,fdseterror;
	TIMEVAL TimeVal={0,10000};

	if (socsv[p1]==-1) return -2;
	if (svstat[p1]==-1) return -4;
	if (svstat[p1]!=2) return -3;

	FD_ZERO(&fdseterror);
	FD_ZERO(&fdsetread);
	FD_ZERO(&fdsetwrite);
	FD_SET( socsv[p1],&fdsetread );

	if((err=select(0,&fdsetread,&fdsetwrite,&fdseterror,&TimeVal))==0) {
		if ( err==SOCKET_ERROR ) return -4;
		return -1;
	}

	//CloseHandle( svth[ p1 ] );
	svstat[p1] = 0;

	//if ( listen(socsv[p1],5) ) return -4;

	len=sizeof(from);
	memset( &from,0,len );
	soc2 = accept( socsv[p1], (LPSOCKADDR)&from, &len );

	if ( soc2 == INVALID_SOCKET) {
		closesocket( socsv[p1] );
		socsv[p1]=-1;
		return -5;
	}

	//(dllサイズ削減のため以下のコードに差し替え)
	//sprintf( s1,"%s:%d",inet_ntoa(from.sin_addr),ntohs(from.sin_port) );

	if ( p2 == 0 ) {
		soc[p1]=soc2;
	} else {
		soc[p2]=soc2;
		socsv[p2]=-1;
	}

	strcpy( s1, inet_ntoa(from.sin_addr) );
	strcat( s1,":" );
	itoa( ntohs(from.sin_port),s2,10 );
	strcat( s1,s2 );
	strcpy( p4,s1 );

	return 0;
}


EXPORT BOOL WINAPI sockclose ( int p1, int p2, int p3, int p4 )
{
	/* シャットダウン */
	if (socsv[p1]!=-1) shutdown( socsv[p1], 2 );
	if (soc[p1]!=-1) shutdown(soc[p1], 2 );
	/* ソケットを破棄 */
	if (soc[p1]!=-1) closesocket(soc[p1]);
	if (socsv[p1]!=-1) closesocket( socsv[p1] );

	soc[p1]=-1;socsv[p1]=-1;svstat[p1]=0;
	return 0;
}


EXPORT BOOL WINAPI sockbye ( int p1, int p2, int p3, int p4 )
{
	int a;
	sockf=0;
	for(a=0;a<SOCKMAX;a++) { sockclose( a,0,0,0 ); }
	WSACleanup();						/* WinSockのリソースを解放する */
	return 0;
}


EXPORT BOOL WINAPI sockcheck( int p1, int p2, int p3, int p4 )
{
	//
	//		socket data recieve
	//			sockcheck sockid
	//
	int err;
	fd_set fdsetread,fdsetwrite,fdseterror;
	TIMEVAL TimeVal={0,10000};

	FD_ZERO(&fdseterror);
	FD_ZERO(&fdsetread);
	FD_ZERO(&fdsetwrite);
	FD_SET( soc[p1],&fdsetread );

	err=select(0,&fdsetread,&fdsetwrite,&fdseterror,&TimeVal);

	if ( err==SOCKET_ERROR ) return -2;			// some error
	if ( err==0) return -1;						// timeout
	return 0;
}




EXPORT BOOL WINAPI sockget ( char *p1, int p2, int p3, int p4 )
{
	//
	//		socket data recieve
	//			sockget recv_buff, size, sockid
	//
	int recvsize;
	int buf_len;		/* 受信したバイト数 */
	char *buf;

	buf=p1;recvsize=p2;
	if (recvsize==0) recvsize=64;

	/* ソケットから文字列を受信します */
	/* 受信した文字列は buf に入ります */
	/* 受信する文字列はサーバが送信したものです */
	buf_len = recv(soc[p3], buf, recvsize - 1, 0);
	if (buf_len == SOCKET_ERROR ){
		return -1;
	} else {
		buf[buf_len] = 0;	/* 受信したバッファの後ろにNULLを付加する */
	}
	return 0;
}


EXPORT BOOL WINAPI sockput ( BMSCR *bm, char *p2, int p3, int p4 )
{
	//
	//		socket data send (type6)
	//			sockput "recv_str", sockid
	//
	char *buf;

	buf=p2;

	/* 指定のソケットに文字列(buf)を送信します */
	/* 送信した文字列はサーバに届きます */
	if( send(soc[p3], buf, lstrlen(buf), 0) == SOCKET_ERROR ) {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI sockputc ( BMSCR *bm, int p2, int p3, int p4 )
{
	//
	//		socket data send (1byte) type$02
	//			sockputc send_chr, sockid
	//
	char buf[2];

	buf[0]=(char)p2;
	buf[1]=0;

	/* 指定のソケットにbufを送信 */
	if( send(soc[p3], buf, 1, 0) == SOCKET_ERROR ) {
		return -1;
	}
	return 0;
}


EXPORT BOOL WINAPI sockgetc ( int *p1, int p2, int p3, int p4 )
{
	//
	//		socket data recieve (1byte)
	//			sockgetc recv_val, sockid
	//
	int buf_len;		/* 受信したバイト数 */
	char buf[4];

	/* ソケットから文字列を受信します */
	/* 受信したdataは buf に入ります */
	/* 受信するdataはサーバが送信したものです */
	buf_len = recv(soc[p2], buf, 1, 0);
	if (buf_len == SOCKET_ERROR ) return -1;
	if (buf_len == 0 ) {
		*p1=-1;return 0;
	}
	*p1=(int)buf[0];
	return 0;
}


EXPORT BOOL WINAPI sockputb ( char *p1, int p2, int p3, int p4 )
{
	//
	//		socket data send (binary) type$01
	//			sockputb send_buf, start, length, sockid
	//
	int sendsize;
	int buf_len;		/* 送信したバイト数 */
	char *buf;

	buf=p1;buf+=p2;
	sendsize=p3;
	if (sendsize==0) sendsize=64;

	/* 指定のソケットにdata(buf)を送信します */
	/* 送信したdataはサーバに届きます */
	buf_len = send(soc[p4], buf, sendsize, 0);
	if ( buf_len == SOCKET_ERROR ) return 0;
	return -(buf_len);
}


EXPORT BOOL WINAPI sockgetb ( char *p1, int p2, int p3, int p4 )
{
	//
	//		socket data recieve (binary) type$01
	//			sockgetb recv_buf, start, length, sockid
	//
	int recvsize;
	int buf_len;		/* 受信したバイト数 */
	char *buf;

	buf=p1;buf+=p2;
	recvsize=p3;
	if (recvsize==0) recvsize=64;

	/* 受信したdataは buf に入ります */
	/* 受信するdataはサーバが送信したものです */
	buf_len = recv(soc[p4], buf, recvsize , 0);
	if (buf_len == SOCKET_ERROR ){
		return 0;
	}
	return -(buf_len);
}


EXPORT BOOL WINAPI ipget(int a,int b,int c,char *d)
{
	char sh[256];
	PHOSTENT hostent;
	IN_ADDR	in;

	if (sockf==0) {
		if ( sockprep() ) return -1;
	}

	if(0 == gethostname(sh,256))
		{
			hostent = gethostbyname(sh);
			if(WSANO_DATA == WSAGetLastError())
				{
					lstrcpy(d, "0");
					return -1;
				}
			memcpy(&in, hostent->h_addr, 4);
			lstrcpy(d, inet_ntoa(in));
		}
	return 0;
}


