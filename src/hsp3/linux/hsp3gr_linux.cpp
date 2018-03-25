
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

#include "hsp3gr_linux.h"

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
//					HSP system support
/*----------------------------------------------------------*/

static void ExecFile( char *stmp, char *ps, int mode )
{
	//	外部ファイル実行
	system(stmp);
}
		
static char *getdir( int id )
{
	//		dirinfo命令の内容をstmpに設定する
	//
	char *p;
	char *ss;
	char fname[HSP_MAX_PATH+1];
	p = ctx->stmp;

	*p = 0;

	switch( id ) {
	case 0:				//    カレント(現在の)ディレクトリ
		break;
	case 1:				//    HSPの実行ファイルがあるディレクトリ
		break;
	case 2:				//    Windowsディレクトリ
		break;
	case 3:				//    Windowsのシステムディレクトリ
		break;
	case 4:				//    コマンドライン文字列
		break;
	default:
		throw HSPERR_ILLEGAL_FUNCTION;
	}

	return p;
}


static int sysinfo( int p2 )
{
	//		System strings get
	//
	int fl;
	char *p1;

	fl = HSPVAR_FLAG_INT;
	p1 = ctx->stmp;
	*p1 = 0;

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


/*----------------------------------------------------------*/
//		GPIOデバイスコントロール関連
/*----------------------------------------------------------*/

#ifdef HSPRASPBIAN

#define GPIO_TYPE_NONE 0
#define GPIO_TYPE_OUT 1
#define GPIO_TYPE_IN 2
#define GPIO_MAX 32

#define GPIO_CLASS "/sys/class/gpio/"

static int gpio_type[GPIO_MAX];
static int gpio_value[GPIO_MAX];

static int echo_file( char *name, char *value )
{
	//	echo value > name を行なう
	//printf( "[%s]<-%s\n",name,value );
	int fd;
	fd = open( name, O_WRONLY );
	if (fd < 0) {
		return -1;
	}
	write( fd, value, strlen(value)+1 );
	close(fd);
	return 0;
}

static int echo_file2( char *name, int value )
{
	char vstr[64];
	sprintf( vstr, "%d", value );
	return echo_file( name, vstr );
}

static int gpio_delport( int port )
{
	if ((port<0)||(port>=GPIO_MAX)) return -1;

	if ( gpio_type[port]==GPIO_TYPE_NONE ) return 0;
	echo_file2( GPIO_CLASS "unexport", port );
	usleep(100000);		//0.1秒待つ(念のため)
	gpio_type[port]=GPIO_TYPE_NONE;
	return 0;
}

static int gpio_setport( int port, int type )
{
	if ((port<0)||(port>=GPIO_MAX)) return -1;

	if ( gpio_type[port]==GPIO_TYPE_NONE ) {
		echo_file2( GPIO_CLASS "export", port );
		usleep(100000);		//0.1秒待つ(念のため)
	}

	if ( gpio_type[port] == type ) return 0;

	int res = 0;
	char vstr[256];
	sprintf( vstr, GPIO_CLASS "gpio%d/direction", port );

	switch( type ) {
	case GPIO_TYPE_OUT:
		res = echo_file( vstr, "out" );
		break;
	case GPIO_TYPE_IN:
		res = echo_file( vstr, "in" );
		break;
	}

	if ( res ) {
		gpio_type[port] = GPIO_TYPE_NONE;
		return res;
	}

	gpio_type[port] = type;
	gpio_value[port] = 0;
	return 0;
}

static int gpio_out( int port, int value )
{
	if ((port<0)||(port>=GPIO_MAX)) return -1;
	if ( gpio_type[port]!=GPIO_TYPE_OUT ) {
		int res = gpio_setport( port, GPIO_TYPE_OUT );
		if ( res ) return res;
	}

	char vstr[256];
	sprintf( vstr, GPIO_CLASS "gpio%d/value", port );
	if ( value == 0 ) {
		gpio_value[port] = 0;
		return echo_file( vstr, "0" );
	}
	gpio_value[port] = 1;
	return echo_file( vstr, "1" );
}

static int gpio_in( int port, int *value )
{
	if ((port<0)||(port>=GPIO_MAX)) return -1;
	if ( gpio_type[port]!=GPIO_TYPE_IN ) {
		int res = gpio_setport( port, GPIO_TYPE_IN );
		if ( res ) return res;
	}

	int fd,rd,i;
	char vstr[256];
	char ev[256];
	char a1;
	sprintf( vstr, GPIO_CLASS "gpio%d/value", port );

	fd = open( vstr, O_RDONLY | O_NONBLOCK );
	if (fd < 0) {
		return -1;
	}
    rd = read(fd,ev,255);
    if(rd > 0) {
		i = 0;
		while(1) {
			if ( i >= rd ) break;
			a1 = ev[i++];
			if ( a1 == '0' ) gpio_value[port] = 0;
			if ( a1 == '1' ) gpio_value[port] = 1;
		}
	}
	close(fd);

	*value = gpio_value[port];
	return 0;
}

static void gpio_init( void )
{
	int i;
	for(i=0;i<GPIO_MAX;i++) {
		gpio_type[i] = GPIO_TYPE_NONE;
	}
}

static void gpio_bye( void )
{
	int i;
	for(i=0;i<GPIO_MAX;i++) {
		gpio_delport(i);
	}
}

//--------------------------------------------------------------

static int devprm( char *name, char *value )
{
	return echo_file( name, value );
}

static int devcontrol( char *cmd, int p1, int p2, int p3 )
{
	if (( strcmp( cmd, "gpio" )==0 )||( strcmp( cmd, "GPIO" )==0 )) {
		return gpio_out( p1, p2 );
	}
	if (( strcmp( cmd, "gpioin" )==0 )||( strcmp( cmd, "GPIOIN" )==0 )) {
		int res,val;
		res = gpio_in( p1, &val );
		if ( res == 0 ) return val;
		return res;
	}
	return -1;
}

#endif

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
		ExecFile( fname, ps, p1 );
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

#ifdef HSPRASPBIAN

	case 0x49:								// devprm
		{
		char *ps;
		char prmname[256];
		int p_res;
		strncpy( prmname, code_gets(), 255 );
		ps = code_gets();
		p_res = devprm( prmname, ps );
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
		p_res = devcontrol( cname, p1, p2, p3 );
		ctx->stat = p_res;
		break;
		}

#endif

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
		ptr = getdir( p1 );
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

#ifdef HSPRASPBIAN
	gpio_bye();
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
}

void hsp3typeinit_cl_extfunc( HSP3TYPEINFO *info )
{
	info->reffunc = reffunc_function;

#ifdef HSPRASPBIAN
	gpio_init();
#endif
}

