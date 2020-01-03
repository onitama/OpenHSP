/*--------------------------------------------------------
	I/O control main (raspberry pi/raspbian/Linux)
  --------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <termios.h>

#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <linux/input.h>
#include <stdbool.h>


#if defined( __GNUC__ )
#include <ctype.h>
#endif

#include "../hsp3config.h"
#include "../hsp3struct.h"
#include "../../hsp3dish/hgio.h"
#include "../../hsp3dish/supio.h"

extern char *hsp_mainpath;

/*----------------------------------------------------------*/
//					Raspberry Pi I2C support
/*----------------------------------------------------------*/

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

/*----------------------------------------------------------*/
//					Raspberry Pi SPI support
/*----------------------------------------------------------*/
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
  memset(&tr, 0, sizeof(struct spi_ioc_transfer));
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


/*----------------------------------------------------------*/
//		GPIOデバイスコントロール関連
/*----------------------------------------------------------*/

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
	//echo_file2( GPIO_CLASS "unexport", port );
	//usleep(100000);		//0.1秒待つ(念のため)
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

static int hsp3dish_devprm( char *name, char *value )
{
	return echo_file( name, value );
}

static int hsp3dish_devcontrol( char *cmd, int p1, int p2, int p3 )
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

/*----------------------------------------------------------*/
//		デバイスコントロール関連
/*----------------------------------------------------------*/
static HSP3DEVINFO *mem_devinfo;
static int devinfo_dummy;

static int *hsp3dish_devinfoi( char *name, int *size )
{
	devinfo_dummy = 0;
	*size = -1;
	return NULL;
//	return &devinfo_dummy;
}

static char *hsp3dish_devinfo( char *name )
{
	if ( strcmp( name, "name" )==0 ) {
		return mem_devinfo->devname;
	}
	if ( strcmp( name, "error" )==0 ) {
		return mem_devinfo->error;
	}
	return NULL;
}

void hsp3dish_setdevinfo_io( HSP3DEVINFO *devinfo )
{
	//		Initalize DEVINFO
	mem_devinfo = devinfo;
#ifdef HSPRASPBIAN
	devinfo->devname = "RaspberryPi";
#else
	devinfo->devname = "linux";
#endif
	devinfo->error = "";
	devinfo->devprm = hsp3dish_devprm;
	devinfo->devcontrol = hsp3dish_devcontrol;
	devinfo->devinfo = hsp3dish_devinfo;
	devinfo->devinfoi = hsp3dish_devinfoi;

	gpio_init();
	I2C_Init();
	SPI_Init();
}

void hsp3dish_termdevinfo_io( void )
{
	SPI_Term();
	I2C_Term();
	gpio_bye();
}


/*----------------------------------------------------------*/

