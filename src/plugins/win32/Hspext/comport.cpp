
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>
#include <io.h>
#include <string.h>
#include <winbase.h>
#include <winuser.h>
#include <shlobj.h>
#include <stdio.h>

#include "../hpi3sample/hsp3plugin.h"


/*------------------------------------------------------------*/
/*
		Serial Port I/O routines
*/
/*------------------------------------------------------------*/

static	HANDLE comHandle = NULL;

static void *Hsp3GetBlockSize( HSPEXINFO *hei, PVal *pv, APTR ap, int *size )
{
	//		(HSP3用)
	//		pv,apからメモリブロックを取得する
	//
	PDAT *pd;
	HspVarProc *proc;
	proc = hei->HspFunc_getproc( pv->flag );
	pv->offset = ap;
	pd =  proc->GetPtr( pv );
	return proc->GetBlockSize( pv,pd,size );
}


EXPORT BOOL WINAPI comopen( int p1, char *p2, int p3, int p4 )
{
	//
	//		open communication port (type4)
	//			comopen	port, "DCB strings"
	//
	DCB dcb;
	BOOL success;
	COMMTIMEOUTS timeouts;
	char ppstr[16];

	// ポートを開く
	//
	if (p1>0) {
		sprintf(ppstr,"\\\\.\\COM%d", p1);
	}
	else strcpy(ppstr,"LPT1");

	comHandle = CreateFile( ppstr, 
		GENERIC_READ|GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (comHandle == INVALID_HANDLE_VALUE) return -1;

	success = GetCommState(comHandle, &dcb);
	if (!success) return -1;

	if (p2[0]!=0) {
		success = BuildCommDCB( p2, &dcb );
		if (!success) return 3;
	}

   // Test data
	//dcb.BaudRate = 57600;
	//dcb.ByteSize = 8;
	//dcb.Parity = NOPARITY;
	//dcb.StopBits = ONESTOPBIT;

	// 新しい設定
	success = SetCommState(comHandle, &dcb);
	if (!success) return -1;

	// Timeout設定
	timeouts.ReadIntervalTimeout = MAXDWORD; 
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts( comHandle, &timeouts );

	// DTR設定
	EscapeCommFunction(comHandle, SETDTR);
	return 0;
}


EXPORT BOOL WINAPI comclose( int p1, int p2, int p3, int p4 )
{
	//
	//		close communication port (type0)
	//			comclose
	//
	EscapeCommFunction(comHandle, CLRDTR);
	CloseHandle(comHandle);  
	comHandle=NULL;
	return 0;
}


EXPORT BOOL WINAPI comput( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		send communication (type6)
	//			comput "strings"
	//
	DWORD numWrite;
	BOOL success;

	// データ送信
	// (\nではなく\rを改行にすること)
	success = WriteFile(comHandle, p1, strlen(p1), &numWrite, 0);
	if (!success) return 0;
	return -(int)numWrite;
}


EXPORT BOOL WINAPI computc( int p1, int p2, int p3, int p4 )
{
	//
	//		send communication (type0)
	//			computc val
	//
	char sd[4];
	DWORD numWrite;
	BOOL success;

	sd[0] = p1;
	sd[1] = 0;
	success = WriteFile(comHandle, sd, 1, &numWrite, 0);
	if (!success) return 0;
	return -(int)numWrite;
}


EXPORT BOOL WINAPI computb( HSPEXINFO *hei, int p1, int p2, int p3 )
{
	//
	//		send binary (type$202)
	//			computb var, size
	//
	DWORD numWrite;
	BOOL success;
	PVal *pv;
	APTR ap;
	char *sd;
	int size;
	int _p2;

	ap = hei->HspFunc_prm_getva( &pv );		// パラメータ1:変数
	sd = (char *)Hsp3GetBlockSize( hei, pv, ap, &size );
	_p2 = hei->HspFunc_prm_getdi(size-1);	// パラメータ2:整数値
	if ( _p2 > size ) _p2=size;

	success = WriteFile(comHandle, sd, _p2, &numWrite, 0);
	if (!success) return 0;
	return -(int)numWrite;
}


EXPORT BOOL WINAPI comget( char *p1, int p2, int p3, int p4 )
{
	//
	//		get communication (type1)
	//			comget val, size
	//
	DWORD numRead;
	BOOL success;
	int	GetSize;

	GetSize=p2;if (GetSize==0) GetSize=64;
	success = ReadFile(comHandle, p1, GetSize, &numRead, 0);
	if (!success) {
		*p1=0;return 0;
	}
	p1[numRead]=0;
	return -(int)numRead;
}


EXPORT BOOL WINAPI comgetc( unsigned int *p1, int p2, int p3, int p4 )
{
	//
	//		get communication text (type1)
	//			comtext val
	//
	DWORD numRead;
	BOOL success;
	unsigned char str[10];

	success = ReadFile(comHandle, str, 1, &numRead, 0);
	*p1=(unsigned int)str[0];
	return -(int)numRead;
}


EXPORT BOOL WINAPI comcontrol( int p1, int p2, int p3, int p4 )
{
	//
	//		put communication control (type0)
	//			comcontrol val
	//
	//	#define SETXOFF             1       // Simulate XOFF received
	//	#define SETXON              2       // Simulate XON received
	//	#define SETRTS              3       // Set RTS high
	//	#define CLRRTS              4       // Set RTS low
	//	#define SETDTR              5       // Set DTR high
	//	#define CLRDTR              6       // Set DTR low
	//	#define RESETDEV            7       // Reset device if possible
	//	#define SETBREAK            8       // Set the device break line.
	//	#define CLRBREAK            9       // Clear the device break line.
	int success;
	success = EscapeCommFunction( comHandle, p1 );
	if ( success == 0 ) return -1;
	return 0;
}


EXPORT BOOL WINAPI comstat( int *p1, int p2, int p3, int p4 )
{
	//
	//		get communication status (type1)
	//			comstat val
	//
	int success;
	success = ClearCommError( comHandle, (DWORD *)p1, (COMSTAT *)(p1+1) );
	if ( success == 0 ) return -1;
	return 0;
}


