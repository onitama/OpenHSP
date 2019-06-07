
//
//	hsp3dish socket拡張(linux)
//	(拡張コマンド・関数処理)
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include "../hsp3config.h"
#include "../hsp3code.h"
#include "../hsp3debug.h"
#include "../supio.h"
#include "../strbuf.h"

#ifndef MAX
  #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HSPCTX *ctx;		// Current Context
static HSPEXINFO *exinfo;
static int *type;
static int *val;
static int *exflg;
static int p1,p2,p3,p4,p5;

/*----------------------------------------------------------*/
//					TCP/IP support
/*----------------------------------------------------------*/
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

static int sockprep();
static int sockclose(int);
static int sockget(char*, int, int);
static int sockgetm(char*, int, int, int);
static int sockreadbyte();

#define SOCKMAX 32
static	int sockf=0;
static	int soc[SOCKMAX];		/* Soket Descriptor */
static int socsv[SOCKMAX];
static	int svstat[SOCKMAX];		/* Soket Thread Status (server) */

static int sockprep( void ){
  sockf++;
  for(int a = 0; a < SOCKMAX; a++){
    soc[a] = -1;
    socsv[a] = -1;
    svstat[a] = 0;
  }
  return 0;
}

static int sockopen( int p1, char *p2, int p3){
  struct sockaddr_in addr;

  if(sockf == 0){
    if(sockprep()) return -1;;
  }

  soc[p1] = socket(AF_INET, SOCK_STREAM, 0);
  if(soc[p1]<0){ return -2; }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr=inet_addr(p2);
  addr.sin_port=htons(p3);
  if(connect(soc[p1], (struct sockaddr*)&addr, sizeof(addr)) < 0){
    return -4;
  }
  return 0;
}

static int sockclose(int p1){
  if(socsv[p1]!=-1) close(socsv[p1]);
  if(soc[p1]!=-1) close(soc[p1]);

  soc[p1]=-1;socsv[p1]=-1;svstat[p1]=0;
  return 0;
}

static int sockget(char* buf, int size, int socid){
  return sockgetm(buf, size, socid, 0);
}

static int sockgetm(char* buf, int size, int socid, int flag){
  int recv_len;
  memset(buf, 0, sizeof(buf));
  recv_len = recv(soc[socid], buf, size, flag);
  if(recv_len == -1) return errno;
  return 0;
}

static int sockgetc(int* buf, int socid){
	int recv_len;
	char recv;
	recv_len = read(soc[socid], &recv, 1);
  if(recv_len < 0) return errno;
	buf[0] = (int)recv;
	return 0;
}

static int sockgetbm( char *org_buf, int offset, int size, int socid, int flag){
  int buf_len;
  char* buf;
  buf = org_buf + offset;
  if(size == 0) size = 64;

  buf_len = recv(soc[socid], buf, size, flag);
  if(buf_len == -1) return errno;
  return -(buf_len);
}

static int sockgetb( char *org_buf, int offset, int size, int socid){
  return sockgetbm(org_buf, offset, size, socid, 0);
}

static int sockputm(char* buf, int socid, int flag){
  if(send(soc[socid], buf, strlen(buf), flag) == -1){   // FIXME: is `strlen(buf)` must be like `lstrlen()` in windows?
    return errno;
  }
  return 0;
}

static int sockput(char* buf, int socid){
  return sockputm(buf, socid, 0);
}

static int sockputc(int c, int socid){
  char buf[2];
  buf[0] = (char)c;
  buf[1] = '\0';

  if(send(soc[p3], buf, 1, 0) == -1){
    return errno;
  }
  return 0;
}

static int sockputb(char* org_buf, int offset, int size, int socid){
  char *buf;
  int buf_len;
  buf = org_buf + offset;
  if(size == 0) size = 64;

  buf_len = send(soc[socid], buf, size, 0);
  if(buf_len == -1) return 0;
  return -(buf_len);
}

static int sockmake(int socid, int port){

  if(sockf == 0){
    if(sockprep()) return -1;
  }

  if(socsv[socid] == -1){
    struct sockaddr_in addr;
    int len = sizeof(struct sockaddr_in);

    socsv[socid] = socket(AF_INET, SOCK_STREAM, 0);
    if(socsv[socid] < 0){
      return -2;
    }

    bzero((char *)&addr, sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if(bind(socsv[socid], (struct sockaddr *)&addr, len) < 0){
      return -3;
    }

  }else{
  }

  svstat[socid] = 1;
  if(listen(socsv[socid], SOMAXCONN) < 0){
    svstat[socid] = 0;
  }else{
    svstat[socid]++;
  }
  return 0;
}


static int sockwait(int socid){
  static int maxfd = 0;
  int err;
  fd_set fdsetread, fdsetwrite, fdseterror;
  struct timeval TimeVal={0,10000};

  int soc2;
  struct sockaddr_in from;

  if(socsv[socid] == -1) return -2;
  if(svstat[socid] == -1) return -4;
  if(svstat[socid] != 2) return -3;

  FD_ZERO(&fdsetread);
  FD_ZERO(&fdsetwrite);
  FD_ZERO(&fdseterror);
  FD_SET(socsv[socid], &fdsetread);
  maxfd = MAX(maxfd, socsv[socid]);
  if((err = select(maxfd+1, &fdsetread, &fdsetwrite, &fdseterror, &TimeVal))==0){
    if(err == -1) return -4;
    return -1;
  }

  svstat[socid] = 0;

  unsigned int len = sizeof(from);
  memset(&from, 0, len);
  soc2 = accept(socsv[socid], (struct sockaddr *)&from, &len);

  if(soc2 == -1){
    close(socsv[socid]);
    socsv[socid] = -1;
    return -5;
  }

  soc[socid] = soc2;
  close(socsv[socid]);
  socsv[socid] = -1;

  char s1[128];
  char s2[128];
  sprintf(s1, "%s:%d", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
  return 0;
}


static int sockreadbyte(){
  // No arguments
  // Return read byte
  int buf_len;
  char buf[1];
  memset(buf, 0, sizeof(buf));

  buf_len = read(soc[p2], buf, sizeof(buf));
  if(buf_len < 0){
    return -2;
  }
  if(buf_len == 0){
    return -1;
  }
  return (int)buf[0];
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static int cmdfunc_extcmd( int cmd )
{
	//		cmdfunc : TYPE_EXTCMD
	//		(内蔵GUIコマンド)
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)
	switch( cmd ) {							// サブコマンドごとの分岐

	//	command part
	//
  case 0x60:              //sockopen
    {
    char *address;
    int p_res;
    p1 = code_getdi( 0 );
    address = code_stmpstr( code_gets() );
    p3 = code_getdi( 0 );
    p_res = sockopen(p1, address, p3);
    ctx->stat = p_res;
    break;
    }
  case 0x61:              //sockclose
    {
    char *cname;
    int p_res;
    p1 = code_geti();
    p_res = sockclose(p1);
    ctx->stat = p_res;
    break;
    }
  case 0x62:              //sockreadbyte
    {
    char *cname;
    int p_res;
    p_res = sockreadbyte();
    if(p_res > 0){
      printf("%c\n", p_res);
    }
    ctx->stat = p_res;
    break;
    }
	case 0x63:	//sockget
		{
			PVal *pval;
			char *ptr;
			int size;
			ptr = code_getvptr( &pval, &size );
			p2 = code_getdi( 0 );
			p3 = code_getdi( 0 );
			int p_res;
			p_res = sockget(pval->pt, p2, p3);
			ctx->stat = p_res;
			break;
		}
	case 0x64:	//sockgetc
		{
		}
	case 0x65:	//sockgetm
		{
			PVal *pval;
			char *ptr;
			int size;
			ptr = code_getvptr( &pval, &size );
			p2 = code_getdi( 0 );
			p3 = code_getdi( 0 );
			p4 = code_getdi( 0 );
			int p_res;
			p_res = sockgetm(pval->pt, p2, p3, p4);
			ctx->stat = p_res;
			break;
		}
  case 0x66:  //sockgetb
    {
      PVal *pval;
      char *ptr;
      int size;
      ptr = code_getvptr(&pval, &size);
      p2 = code_getdi( 0 );
      p3 = code_getdi( 0 );
      p4 = code_getdi( 0 );
      int p_res;
      p_res = sockgetb(pval->pt, p2, p3, p4);
      ctx->stat = p_res;
      break;
    }
  case 0x67:  //sockgetbm
    {
      PVal *pval;
      char *ptr;
      int size;
      ptr = code_getvptr(&pval, &size);
      p2 = code_getdi( 0 );
      p3 = code_getdi( 0 );
      p4 = code_getdi( 0 );
      p5 = code_getdi( 0 );
      int p_res;
      p_res = sockgetbm(pval->pt, p2, p3, p4, p5);
      ctx->stat = p_res;
      break;
    }
  case 0x68:  // sockput
    {
      PVal *pval;
      char *ptr;
      int size;
      ptr = code_getvptr(&pval, &size);
      p2 = code_getdi( 0 );
      int p_res;
      p_res = sockput(pval->pt, p2);
      ctx->stat = p_res;
      break;
    }
  case 0x69:  // sockputc
    {
      PVal *pval;
      char *ptr;
      int size;
      p1 = code_getdi( 0 );
      p2 = code_getdi( 0 );
      int p_res;
      p_res = sockputc(p1, p2);
      ctx->stat = p_res;
      break;
    }
  case 0x6a:  // sockputb
    {
      PVal *pval;
      char *ptr;
      int size;
      ptr = code_getvptr(&pval, &size);
      p2 = code_getdi( 0 );
      p3 = code_getdi( 0 );
      p4 = code_getdi( 0 );
      int p_res;
      p_res = sockputb(ptr, p2, p3, p4);
      ctx->stat = p_res;
      break;
    }
  case 0x6b:  // sockmake
    {
      p1 = code_getd();
      p2 = code_getd();
      int p_res = sockmake(p1, p2);
      ctx->stat = p_res;
      break;
    }
  case 0x6c:  // sockwait
    {
      p1 = code_getd();
      int p_res = sockwait(p1);
      ctx->stat = p_res;
      break;
    }

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


#if 0
static int reffunc_intfunc_ivalue;

static void *reffunc_function( int *type_res, int arg )
{
	void *ptr;

	//		返値のタイプを設定する
	//
	*type_res = HSPVAR_FLAG_INT;			// 返値のタイプを指定する
	ptr = &reffunc_intfunc_ivalue;			// 返値のポインタ

	//			'('で始まるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != '(' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	switch( arg & 0xff ) {

	//	int function

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}

	//			'('で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	return ptr;
}
#endif

static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//
	return 0;
}

void hsp3typeinit_sock_extcmd( HSP3TYPEINFO *info )
{
	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		Application initalize
	//
}

void hsp3typeinit_sock_extfunc( HSP3TYPEINFO *info )
{
}


