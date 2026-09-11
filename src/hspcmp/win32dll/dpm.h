
//		in DPM.CPP functions

#ifndef __dpm_h
#define __dpm_h

class CMemBuf;
void dpmc_ini( CMemBuf *mesbuf, const char *infile );
int dpmc_pack( int mode );
int dpmc_mkexe( int mode, const char *hspexe, int opt1, int opt2, int opt3 );
int dpmc_get( char *outfile );
int dpmc_view( void );
void dpmc_dpmkey( int key );

#endif
