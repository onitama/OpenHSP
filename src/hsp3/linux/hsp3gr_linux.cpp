
//
//	HSP3 graphics command
//	(GUI関連コマンド・関数処理)
//	onion software/onitama 2004/6
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
#include "../hsp3ext.h"

#include "hsp3gr_linux.h"

#include "../../hsp3dish/hgio.h"
#include "../../hsp3dish/supio.h"

#define DEVCTRL_IO
#include "devctrl_io.h"

static HSP3DEVINFO mem_devinfo;
static HSP3DEVINFO *devinfo;

/*------------------------------------------------------------*/
/*
		system data
*/
/*------------------------------------------------------------*/

static HSPCTX *ctx;
static int *type;
static int *val;
static int cur_window;
static int p1,p2,p3,p4,p5,p6;
static int ckey,cklast,cktrg;
static int msact;
static int dispflg;

extern int resY0, resY1;

/*----------------------------------------------------------*/
//					TCP/IP support
/*----------------------------------------------------------*/
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

static int sockclose(int);
static int sockget(char*, int, int);
static int sockgetm(char*, int, int, int);
static int sockclose(int);
static int sockreadbyte();

#define SOCKMAX 32
static	int sockf=0;
static	int soc[SOCKMAX];		/* Soket Descriptor */
// static	SOCKET socsv[SOCKMAX];		/* Soket Descriptor (server) */
static	int svstat[SOCKMAX];		/* Soket Thread Status (server) */
// static	HANDLE svth[SOCKMAX];		/* Soket Thread Handle (server) */

static int sockopen( int p1, char *p2, int p3){
  struct sockaddr_in addr;
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
  close(soc[p1]);
  return 0;
}

static int sockget(char* buf, int size, int socid){
  return sockgetm(buf, size, socid, 0);
}

static int sockgetm(char* buf, int size, int socid, int flag){
  int recv_len;
  memset(buf, 0, size);
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

/*----------------------------------------------------------*/
//					HSP system support
/*----------------------------------------------------------*/

static int sysinfo(int p2)
{
	//		System strings get
	//
	int fl;
	char* p1;

	p1 = hsp3ext_sysinfo(p2, &fl, ctx->stmp);
	if (p1 == NULL) {
		p1 = ctx->stmp;
		*p1 = 0;
		return HSPVAR_FLAG_INT;
	}
	return fl;
}

void *ex_getbmscr( int wid )
{
	return NULL;
}

void ex_mref( PVal *pval, int prm )
{
	int t,size;
	void *ptr;
	const int GETBM=0x60;
	t = HSPVAR_FLAG_INT;
	size = 4;
	if ( prm >= GETBM ) {
		throw HSPERR_UNSUPPORTED_FUNCTION;
	} else {
		switch( prm ) {
		case 0x40:
			ptr = &ctx->stat;
			break;
		case 0x41:
			ptr = ctx->refstr;
			t = HSPVAR_FLAG_STR;
			size = 1024;
			break;
		case 0x44:
			ptr = ctx; size = sizeof(HSPCTX);
			break;
		default:
			throw HSPERR_UNSUPPORTED_FUNCTION;
		}
	}
	HspVarCoreDupPtr( pval, t, ptr, size );
}

/*------------------------------------------------------------*/
/*
		TCP/IP
*/
/*------------------------------------------------------------*/

static int printmsggo(int p1){
  printf("Number: %d\n", p1);
  return 0;
}

/*------------------------------------------------------------*/
/*
		interface
*/
/*------------------------------------------------------------*/

static void cmdfunc_dialog( void )
{
	// dialog
	int i;
	char *ptr;
	char *ps;
	char stmp[0x4000];
	ptr = code_getdsi( "" );
	strncpy( stmp, ptr, 0x4000-1 );
	p1 = code_getdi( 0 );
	ps = code_getds("");

}


static int cmdfunc_extcmd( int cmd )
{
	//		cmdfunc : TYPE_EXTCMD
	//		(内蔵GUIコマンド)
	//
	code_next();							// 次のコードを取得(最初に必ず必要です)
	switch( cmd ) {							// サブコマンドごとの分岐

	case 0x02:								// exec
		{
		char *ps;
		char fname[HSP_MAX_PATH];
		strncpy( fname, code_gets(), HSP_MAX_PATH-1 );
		p1 = code_getdi( 0 );
		ps = code_getds( "" );
		hsp3ext_execfile( fname, ps, p1 );
		break;
		}

	case 0x03:								// dialog
		cmdfunc_dialog();
		break;

	case 0x0f:								// mes,print
		{
		//char stmp[1024];
		char *ptr;
		int chk;
		chk = code_get();
		if ( chk<=PARAM_END ) {
			printf( "\n" );
			break;
		}
		ptr = (char *)(HspVarCorePtr(mpval));
		if ( mpval->flag != HSPVAR_FLAG_STR ) {
			ptr = (char *)HspVarCoreCnv( mpval->flag, HSPVAR_FLAG_STR, ptr );	// 型が一致しない場合は変換
		}
		printf( "%s\n",ptr );
		//strsp_ini();
		//while(1) {
		//	chk = strsp_get( ptr, stmp, 0, 1022 );
		//	printf( "%s\n",stmp );
		//	if ( chk == 0 ) break;
		//}
		break;
		}

	case 0x27:								// input (console)
		{
		PVal *pval;
		APTR aptr;
		char *pp2;
		char *vptr;
		int strsize;
		int a;
		strsize = 0;
		aptr = code_getva( &pval );
		//pp2 = code_getvptr( &pval, &size );
		p2 = code_getdi( 0x4000 );
		p3 = code_getdi( 0 );

		if ( p2 < 64 ) p2 = 64;
		pp2 = code_stmp( p2+1 );

		switch( p3 & 15 ) {
		case 0:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a==EOF ) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 1:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		case 2:
			while(1) {
				if ( p2<=0 ) break;
				a = getchar();
				if ( a == '\r' ) {
					int c = getchar();
					if( c != '\n' ) {
						ungetc(c, stdin);
					}
					break;
				}
				if (( a==EOF )||( a=='\n' )) break;
				*pp2++ = a;
				p2--;
				strsize++;
			}
			break;
		}

		*pp2 = 0;
		ctx->strsize = strsize + 1;

		if ( p3 & 16 ) {
			if (( pval->support & HSPVAR_SUPPORT_FLEXSTORAGE ) == 0 ) throw HSPERR_TYPE_MISMATCH;
			//HspVarCoreAllocBlock( pval, (PDAT *)vptr, strsize );
			vptr = (char *)HspVarCorePtrAPTR( pval, aptr );
			memcpy( vptr, ctx->stmp, strsize );
		} else {
			code_setva( pval, aptr, TYPE_STRING, ctx->stmp );
		}
		break;
		}


	case 0x49:								// devprm
		{
		char *ps;
		char prmname[256];
		int p_res;
		strncpy( prmname, code_gets(), 255 );
		ps = code_gets();
		p_res = devinfo->devprm( prmname, ps );
		ctx->stat = p_res;
		break;
		}
	case 0x4a:								// devcontrol
		{
		char *cname;
		int p_res;
		cname = code_stmpstr( code_gets() );
		p1 = code_getdi( 0 );
		p2 = code_getdi( 0 );
		p3 = code_getdi( 0 );
		p_res = devinfo->devcontrol( cname, p1, p2, p3 );
		ctx->stat = p_res;
		break;
		}

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
	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return RUNMODE_RUN;
}


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

	case 0x002:								// dirinfo
		p1 = code_geti();
		ptr = hsp3ext_getdir( p1 );
		*type_res = HSPVAR_FLAG_STR;
		break;

	case 0x003:								// sysinfo
		p1 = code_geti();
		*type_res = sysinfo( p1 );
		ptr = ctx->stmp;
		break;

	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}

	//			')'で終わるかを調べる
	//
	if ( *type != TYPE_MARK ) throw HSPERR_INVALID_FUNCPARAM;
	if ( *val != ')' ) throw HSPERR_INVALID_FUNCPARAM;
	code_next();

	return ptr;
}


static int termfunc_extcmd( int option )
{
	//		termfunc : TYPE_EXTCMD
	//		(内蔵GUI)
	//
#ifdef DEVCTRL_IO
	hsp3dish_termdevinfo_io();
#endif
	return 0;
}

void hsp3typeinit_cl_extcmd( HSP3TYPEINFO *info )
{
	HSPEXINFO *exinfo;								// Info for Plugins

	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;

	//		function register
	//
	info->cmdfunc = cmdfunc_extcmd;
	info->termfunc = termfunc_extcmd;

	//		HSPEXINFOに関数を登録する
	//
	exinfo->actscr = &cur_window;					// Active Window ID
	exinfo->HspFunc_getbmscr = ex_getbmscr;
	exinfo->HspFunc_mref = ex_mref;

	//		バイナリモードを設定
	//
	//_setmode( _fileno(stdin),  _O_BINARY );

	//		Initalize DEVINFO
#ifdef DEVCTRL_IO
	devinfo = &mem_devinfo;
	hsp3dish_setdevinfo_io( devinfo );
#endif
}

void hsp3typeinit_cl_extfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_function;
}
