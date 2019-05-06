extern void SPI_Init( void );
extern void SPI_Close( int ch );
extern void SPI_Term( void );
extern int SPI_Open( int ch, int ss );
extern int SPI_ReadByte( int ch );
extern int SPI_ReadWord( int ch );
extern int SPI_WriteByte( int ch, int value, int length );
extern int MCP3008_FullDuplex(int spich, int adcch);


int main(){
  SPI_Init();
  MCP3008_FullDuplex(0, 0);
  return 0;
}
