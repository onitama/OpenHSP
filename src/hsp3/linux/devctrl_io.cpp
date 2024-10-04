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
#define HSPSPI_DEVNAME "/dev/spidev"

int spifd_ch[HSPSPI_CHMAX];
struct spi_ioc_transfer spi_tr_data = {
	.tx_buf = (uint64_t)NULL,
	.rx_buf = (uint64_t)NULL,
	.len = 0,
	.speed_hz = 0,
	.delay_usecs = 0,
	.bits_per_word = 0,		// use the device default of bits per word.
	.cs_change = 0,
	.tx_nbits = 0,			// SPI_TX_SINGLE
	.rx_nbits = 0,			// SPI_RX_SINGLE
	.word_delay_usecs = 0,
	.pad = 0,
};

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

int SPI_Open( int ch, int bus, int cs )
{
	int fd;
	char devname_suffix[128];
	char devname[128] = HSPSPI_DEVNAME;

	if(cs >= 10) return -2;		// FIXME: you need `itoa()`.
	if(bus >= 10) return -3;	// FIXME: you need `itoa()`.

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] ) SPI_Close( ch );

	devname_suffix[0] = bus + '0';
	devname_suffix[1] = '.';
	devname_suffix[2] = cs + '0';
	devname_suffix[3] = '\0';

	strcat(devname, devname_suffix);

	if((fd = open( devname, O_RDWR )) < 0){
		return 1;
	}

	spifd_ch[ch] = fd;
	return 0;
}

int SPI_ConfigureH(int speed_hz, int delay_usecs, int bits_per_word){
	spi_tr_data.speed_hz = speed_hz;
	spi_tr_data.delay_usecs = delay_usecs;
	spi_tr_data.bits_per_word = bits_per_word;
	return 0;
}

int SPI_ConfigureM(int cs_change, int tx_nbits, int rx_nbits){
	spi_tr_data.cs_change = cs_change;
	spi_tr_data.tx_nbits = tx_nbits;
	spi_tr_data.rx_nbits = rx_nbits;
	return 0;
}

int SPI_ConfigureL(int word_delay_usecs){
	spi_tr_data.word_delay_usecs = word_delay_usecs;
	return 0;
}

/*
 * SPI_Transceive
 *
 * Send and receive data simultaneously (full duplex communication)
 * with an SPI device. This function only supports little endian
 * environments.
 *
 * Parameters
 * ----------
 * ch: int
 *     index at spifd_ch.
 * val: int
 *     sending at most three values of uint8_t stored as int.
 *     Transfer order depends on the host byte order (endianness).
 * len: int
 *     the number of bytes to send/receive.
 *
 * Returns
 * -------
 * read_byte_int: int
 *     received values of uint8_t stored as int.
 *     Negative values indicates error occured in little endian
 *     environment.
 *     Transfer order depends on the host byte order (endianness).
 *
 * Note
 * ----
 * The allowed data length is limited to 3 bytes even for little endian
 * environments to indicate errors by negative values while 32-bit int
 * can store 4 bytes.
 *
 * In big endian environments this function potentially returns
 * unexpected negative values (e.g., receiving [0xff 0xff 0xff]
 * results in 0xffffff00 according to big endien). At the moment which
 * error ids fall within the range of -1 to -255, data and errors are
 * distinguishable. When it is -256, for example, data or errors are not
 * distinguishable from 0xffffff00.
 */
int SPI_Transceive( int ch, int val, int len ){
	uint8_t *write_bytes = (uint8_t *)&val;
	int read_bytes;
	int *leading_read_bytes = &read_bytes;
	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -2;
	if ( len > 3 ) return -3;
	if ( len == 0 ) len = 1;
	spi_tr_data.tx_buf = (uint64_t)(uint8_t *)write_bytes;
	spi_tr_data.rx_buf = (uint64_t)(uint8_t *)leading_read_bytes;
	spi_tr_data.len = len;
	if(ioctl(spifd_ch[ch], SPI_IOC_MESSAGE(1), &spi_tr_data) < 0){
		return -4;
	}
	return read_bytes;
}

int SPI_ReadByte( int ch )
{
	int res;
	unsigned char data;

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -2;

	res = read( spifd_ch[ch], &data, 1 );
	if ( res < 0 ) return -3;

	res = (int)data;
	return res;
}

int SPI_ReadWord( int ch )
{
	int res;
	unsigned char data[2];

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -1;
	if ( spifd_ch[ch] == 0 ) return -2;

	res = read( spifd_ch[ch], data, 2 );
	if ( res < 0 ) return -3;

	res = ((int)data[1]) << 8;
	res |= (int)data[0];
	return res;
}

int SPI_WriteByte( int ch, int value )
{
	int res;

	if ( ( ch<0 )||( ch>=HSPSPI_CHMAX ) ) return -5;
	if ( spifd_ch[ch] == 0 ) return -2;

	res = write( spifd_ch[ch], &value, 1 );
	if ( res < 0 ) return -4;

	return 0;
}

/*
 * Configure SPI options.
 *
 * Commands and arguments can be found on:
 * - https://codebrowser.dev/linux/linux/include/uapi/linux/spi/spidev.h.html
 * - https://codebrowser.dev/linux/linux/include/uapi/linux/spi/spi.h.html
 */
int SPI_WR_Options(int ch, int cmd, void *param){
	if (ioctl(spifd_ch[ch], cmd, param) != 0) {
		return 2;
	}
	return 0;
}

const size_t SPIMODE_LEN = 4;
uint8_t mode_list[SPIMODE_LEN] = {SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3};

int spi_check_spi_mode(uint32_t mode){
	for(int i = 0; i < (int)SPIMODE_LEN; i++){
		if(mode == (uint32_t)mode_list[i]){
			return i;
		}
	}
	return -1;
}

int spi_set_spi_mode(uint8_t *mode, int param){
	if(0 <= param && param < (int)SPIMODE_LEN){
		*mode = mode_list[param];
		return 0;
	}else{
		return -1;
	}
}

int SPI_RD_MODE(int ch){
	uint8_t mode;
	if(ioctl(spifd_ch[ch], SPI_IOC_RD_MODE, &mode) != 0){
		return -2;
	}
	return spi_check_spi_mode(mode);
}

int SPI_WR_MODE(int ch, int param){
	uint8_t mode;
	if(spi_set_spi_mode(&mode, param) != 0){
		return -1;
	}
	return SPI_WR_Options(ch, SPI_IOC_WR_MODE, &mode);
}

int SPI_RD_MODE32(int ch){
	uint32_t mode;
	if(ioctl(spifd_ch[ch], SPI_IOC_RD_MODE32, &mode) != 0){
		return -2;
	}
	return spi_check_spi_mode(mode);
}

int SPI_WR_MODE32(int ch, int param){
	uint8_t _mode;
	uint32_t mode;
	if(spi_set_spi_mode(&_mode, param) != 0){
		return -1;
	}
	mode = (uint32_t)_mode;
	return SPI_WR_Options(ch, SPI_IOC_WR_MODE32, &mode);
}

int SPI_RD_LSB_FIRST(int ch){
	uint8_t lsb_first = 1;			// Zero indicates MSB-first. https://www.kernel.org/doc/Documentation/spi/spidev
	if(ioctl(spifd_ch[ch], SPI_IOC_RD_LSB_FIRST, &lsb_first) != 0){
		return -2;
	}
	return lsb_first;
}

int SPI_WR_LSB_FIRST(int ch, int param){
	uint8_t lsb_first = (uint8_t) param;
	return SPI_WR_Options(ch, SPI_IOC_WR_LSB_FIRST, &lsb_first);
}

int SPI_RD_BITS_PER_WORD(int ch){
	uint8_t bits_per_word;
	if(ioctl(spifd_ch[ch], SPI_IOC_RD_BITS_PER_WORD, &bits_per_word) != 0){
		return -2;
	}
	return bits_per_word;
}

int SPI_WR_BITS_PER_WORD(int ch, int param){
	uint8_t bits_per_word = (uint8_t)param;
	return SPI_WR_Options(ch, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
}

int SPI_RD_MAX_SPEED_HZ(int ch) {
	uint32_t speed_hz;
	if(ioctl(spifd_ch[ch], SPI_IOC_RD_MAX_SPEED_HZ, &speed_hz) != 0){
		return -2;
	}
	return speed_hz;
}

int SPI_WR_MAX_SPEED_HZ(int ch, int param){
	uint32_t speed_hz = param;
	return SPI_WR_Options(ch, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz);
}

int SPI_Setting(int ch, int mode, int lsb_first){
	return SPI_WR_MODE(ch, mode) |
	SPI_WR_LSB_FIRST(ch, lsb_first);
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
		return SPI_WriteByte( p2, p1 );
	}
	if (( strcmp( cmd, "spiopen" )==0 )||( strcmp( cmd, "SPIOPEN" )==0 )) {
		return SPI_Open( p3, p1, p2 );
	}
	if (( strcmp( cmd, "spiclose" )==0 )||( strcmp( cmd, "SPICLOSE" )==0 )) {
		SPI_Close( p1 );
		return 0;
	}
	if (( strcmp( cmd, "spisetting" )==0 )||( strcmp( cmd, "SPISETTING" )==0 )) {
		return SPI_Setting(p3, p1, p2);
	}
	if (( strcmp( cmd, "spigetmode" )==0 )||( strcmp( cmd, "SPIGETMODE" )==0 )) {
		return SPI_RD_MODE(p1);
	}
	if (( strcmp( cmd, "spisetmode" )==0 )||( strcmp( cmd, "SPISETMODE" )==0 )) {
		return SPI_WR_MODE(p2, p1);
	}
	if (( strcmp( cmd, "spigetmode32" )==0 )||( strcmp( cmd, "SPIGETMODE32" )==0 )) {
		return SPI_RD_MODE32(p1);
	}
	if (( strcmp( cmd, "spisetmode32" )==0 )||( strcmp( cmd, "SPISETMODE32" )==0 )) {
		return SPI_WR_MODE32(p2, p1);
	}
	if (( strcmp( cmd, "spigetlsbfirst" )==0 )||( strcmp( cmd, "SPIGETLSBFIRST" )==0 )) {
		return SPI_RD_LSB_FIRST(p1);
	}
	if (( strcmp( cmd, "spisetlsbfirst" )==0 )||( strcmp( cmd, "SPISETLSBFIRST" )==0 )) {
		return SPI_WR_LSB_FIRST(p2, p1);
	}
	if (( strcmp( cmd, "spigetbitsperword" )==0 )||( strcmp( cmd, "SPIGETBITSPERWORD" )==0 )) {
		return SPI_RD_BITS_PER_WORD(p1);
	}
	if (( strcmp( cmd, "spisetbitsperword" )==0 )||( strcmp( cmd, "SPISETBITSPERWORD" )==0 )) {
		return SPI_WR_BITS_PER_WORD(p2, p1);
	}
	if (( strcmp( cmd, "spigetmaxspeedhz" )==0 )||( strcmp( cmd, "SPIGETMAXSPEEDHZ" )==0 )) {
		return SPI_RD_MAX_SPEED_HZ(p1);
	}
	if (( strcmp( cmd, "spisetmaxspeedhz" )==0 )||( strcmp( cmd, "SPISETMAXSPEEDHZ" )==0 )) {
		return SPI_WR_MAX_SPEED_HZ(p2, p1);
	}
	if (( strcmp( cmd, "spiconfigureh" )==0 )||( strcmp( cmd, "SPICONFIGUREH" )==0 )) {
		return SPI_ConfigureH(p1, p2, p3);
	}
	if (( strcmp( cmd, "spiconfigurem" )==0 )||( strcmp( cmd, "SPICONFIGUREM" )==0 )) {
		return SPI_ConfigureM(p1, p2, p3);
	}
	if (( strcmp( cmd, "spiconfigurel" )==0 )||( strcmp( cmd, "SPICONFIGUREL" )==0 )) {
		return SPI_ConfigureL(p1);
	}
	if (( strcmp( cmd, "spitransceive" )==0 )||( strcmp( cmd, "SPITRANSCEIVE" )==0 )) {
		return SPI_Transceive(p3, p1, p2);
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

