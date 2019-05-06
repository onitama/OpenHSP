extern void SPI_Init( void );
extern void SPI_Close( int ch );
extern void SPI_Term( void );
extern int SPI_Open( int ch, int ss );
extern int SPI_ReadByte( int ch );
extern int SPI_ReadWord( int ch );
extern int SPI_WriteByte( int ch, int value, int length );
extern int MCP3008_FullDuplex(int spich, int adcch);

#include <iostream>
using std::cout;
using std::endl;

int main(){
  SPI_Init();
  SPI_Open(0, 0);

  for(int loop = 0; loop < 100; loop++){
    cout << MCP3008_FullDuplex(0, 0) << endl;
  }
  SPI_Close(0);
  return 0;
}
