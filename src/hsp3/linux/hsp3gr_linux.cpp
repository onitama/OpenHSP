
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

#ifndef MCP3008TEST

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
//					Raspberry Pi I2C support
/*----------------------------------------------------------*/
#ifdef HSPRASPBIAN
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define HSPI2C_CHMAX 16
#define HSPI2C_DEVNAME "/dev/i2c-1"

static int i2cfd_ch[HSPI2C_CHMAX];

static void I2C_Init( void )
{
	int i;
	for(i=0;i<HSPI2C_CHMAX;i++) {
		i2cfd_ch[i] = 0;
	}
}

static void I2C_Close( int ch )
{
	if ( ( ch<0 )||( ch>=HSPI2C_CHMAX ) ) return;
	if ( i2cfd_ch[ch] == 0 ) return;

	close( i2cfd_ch[ch] );
	i2cfd_ch[ch] = 0;
}

static void I2C_Term( void )
{
	int i;
	for(i=0;i<HSPI2C_CHMAX;i++) {
		I2C_Close(i);
	}
}

static int I2C_Open( int ch, int adr )
{
	int fd;
	unsigned char i2cAddress;

	if ( ( ch<0 )||( ch>=HSPI2C_CHMAX ) ) return -1;
	if ( i2cfd_ch[ch] ) I2C_Close( ch );

	if((fd = open( HSPI2C_DEVNAME, O_RDWR )) < 0){
        return 1;
    }
    i2cAddress = (unsigned char)(adr & 0x7f);
    if (ioctl(fd, I2C_SLAVE, i2cAddress) < 0) {
		close( fd );
        return 2;
    }

	i2cfd_ch[ch] = fd;
	return 0;
}

static int I2C_ReadByte( int ch )
{
	int res;
	unsigned char data[8];

	if ( ( ch<0 )||( ch>=HSPI2C_CHMAX ) ) return -1;
	if ( i2cfd_ch[ch] == 0 ) return -1;

	res = read( i2cfd_ch[ch], data, 1 );
	if ( res < 0 ) return -1;

	res = (int)data[0];
	return res;
}

static int I2C_ReadWord( int ch )
{
	int res;
	unsigned char data[8];

	if ( ( ch<0 )||( ch>=HSPI2C_CHMAX ) ) return -1;
	if ( i2cfd_ch[ch] == 0 ) return -1;

	res = read( i2cfd_ch[ch], data, 2 );
	if ( res < 0 ) return -1;

	res = ((int)data[1]) << 8;
	res += (int)data[0];
	return res;
}

static int I2C_WriteByte( int ch, int value, int length )
{
	int res;
	int len;
	unsigned char *data;

	if ( ( ch<0 )||( ch>=HSPI2C_CHMAX ) ) return -1;
	if ( i2cfd_ch[ch] == 0 ) return -1;
	if ( ( length<0 )||( length>4 ) ) return -1;

	len = length;
	if ( len == 0 ) len = 1;
	data = (unsigned char *)(&value);
	res = write( i2cfd_ch[ch], data, len );
	if ( res < 0 ) return -1;

	return 0;
}

#endif

#endif // MCP3008TEST
/*----------------------------------------------------------*/
//					Raspberry Pi SPI support
/*----------------------------------------------------------*/
#ifdef HSPRASPBIAN
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/spi/spidev.h>

#define HSPSPI_CHMAX 16
#define HSPSPI_DEVNAME "/dev/spidev0."

int spifd_ch[HSPSPI_CHMAX];

void SPI_Init( void )
{
	int i;
	for(i=0;i<HSPSPI_CHMAX;i++) {
		spifd_ch[i] = 0;
	}
}

void SPI_Close( int ch )
{
	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return;
	if ( spifd_ch[ch] == 0 ) return;

	close( spifd_ch[ch] );
	spifd_ch[ch] = 0;
}

void SPI_Term( void )
{
	int i;
	for(i=0;i<HSPSPI_CHMAX;i++) {
		SPI_Close(i);
	}
}

int SPI_Open( int ch, int ss )
{
	int fd;
  char ss_char[2];
  char devname[128] = HSPSPI_DEVNAME;

  if(ss >= 10) return -2;   // FIXME: you need `itoa()`.

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] ) SPI_Close( ch );

  ss_char[0] = ss + '0';
  ss_char[1] = '\0';

  strcat(devname, ss_char);

	if((fd = open( devname, O_RDWR )) < 0){
        return 1;
    }

  uint8_t spimode = SPI_MODE_0;
  uint8_t msbfirst = 0;
  // Set read mode 0
  if (ioctl(fd, SPI_IOC_RD_MODE, &spimode) < 0) {
    close( fd );
    return 2;
  }

  // Set write mode 0
  if (ioctl(fd, SPI_IOC_WR_MODE, &spimode) < 0) {
    close( fd );
    return 2;
  }

  // Set MSB first
  if (ioctl(fd, SPI_IOC_RD_LSB_FIRST, &msbfirst) < 0) {
    close( fd );
    return 2;
  }
  if (ioctl(fd, SPI_IOC_WR_LSB_FIRST, &msbfirst) < 0) {
    close( fd );
    return 2;
  }

	spifd_ch[ch] = fd;
	return 0;
}

int SPI_ReadByte( int ch )
{
	int res;
	unsigned char data[8];

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -1;

	res = read( spifd_ch[ch], data, 1 );
	if ( res < 0 ) return -1;

	res = (int)data[0];
	return res;
}

int SPI_ReadWord( int ch )
{
	int res;
	unsigned char data[8];

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -1;

	res = read( spifd_ch[ch], data, 2 );
	if ( res < 0 ) return -1;

	res = ((int)data[1]) << 8;
	res += (int)data[0];
	return res;
}

int SPI_WriteByte( int ch, int value, int length )
{
	int res;
	int len;
	unsigned char *data;

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -1;
	if ( ( length<0 )||( length>4 ) ) return -1;

	len = length;
	if ( len == 0 ) len = 1;
	data = (unsigned char *)(&value);
	res = write( spifd_ch[ch], data, len );
	if ( res < 0 ) return -1;

	return 0;
}

int MCP3008_FullDuplex(int spich, int adcch){
  const int COMM_SIZE = 3;
  const uint8_t START_BIT = 0x01;
  const uint8_t SINGLE_ENDED = 0x80;
  const uint8_t CHANNEL = adcch << 4;
  int res;
  struct spi_ioc_transfer tr;
  uint8_t tx[COMM_SIZE] = {0, };
  uint8_t rx[COMM_SIZE] = {0, };

	if ( ( spich<0 )||( spich>=HSPSPI_CHMAX ) ) return -1;
  if(spifd_ch[spich] == 0) return -1;

  tx[0] = START_BIT;
  tx[1] = SINGLE_ENDED | CHANNEL;

  tr.tx_buf = (unsigned long)tx;
  tr.rx_buf = (unsigned long)rx;
  tr.len = COMM_SIZE;
  tr.delay_usecs = 0;
  tr.bits_per_word = 8;
  tr.cs_change = 0;
  tr.speed_hz = 5000;

  if(ioctl(spifd_ch[spich], SPI_IOC_MESSAGE(1), &tr) < 1){
    return -2;
  }

  res = (0x03 & rx[1]) << 8;
  res |= rx[2];

  return res;
}

#endif

#ifndef MCP3008TEST

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
	if (( strcmp( cmd, "i2creadw" )==0 )||( strcmp( cmd, "I2CREADW" )==0 )) {
		return I2C_ReadWord( p1 );
	}
	if (( strcmp( cmd, "i2cread" )==0 )||( strcmp( cmd, "I2CREAD" )==0 )) {
		return I2C_ReadByte( p1 );
	}
	if (( strcmp( cmd, "i2cwrite" )==0 )||( strcmp( cmd, "I2CWRITE" )==0 )) {
		return I2C_WriteByte( p3, p1, p2 );
	}
	if (( strcmp( cmd, "i2copen" )==0 )||( strcmp( cmd, "I2COPEN" )==0 )) {
		return I2C_Open( p2, p1 );
	}
	if (( strcmp( cmd, "i2close" )==0 )||( strcmp( cmd, "I2CCLOSE" )==0 )) {
		I2C_Close( p1 );
		return 0;
	}
	if (( strcmp( cmd, "spireadw" )==0 )||( strcmp( cmd, "SPIREADW" )==0 )) {
		return SPI_ReadWord( p1 );
	}
	if (( strcmp( cmd, "spiread" )==0 )||( strcmp( cmd, "SPIREAD" )==0 )) {
		return SPI_ReadByte( p1 );
	}
	if (( strcmp( cmd, "spiwrite" )==0 )||( strcmp( cmd, "SPIWRITE" )==0 )) {
		return SPI_WriteByte( p3, p1, p2 );
	}
	if (( strcmp( cmd, "spiopen" )==0 )||( strcmp( cmd, "SPIOPEN" )==0 )) {
		return SPI_Open( p2, p1 );
	}
	if (( strcmp( cmd, "spiclose" )==0 )||( strcmp( cmd, "SPICLOSE" )==0 )) {
		SPI_Close( p1 );
    return 0;
	}
	if (( strcmp( cmd, "readmcpduplex" )==0 )||( strcmp( cmd, "READMCPDUPLEX" )==0 )) {
    return MCP3008_FullDuplex(p2, p1);
	}
	return -1;
}

#endif

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
	I2C_Term();
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
	I2C_Init();
#endif
}
#endif // MCP3008TEST
