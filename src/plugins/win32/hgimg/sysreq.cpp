
//
//		System request manager
//			onion software/onitama 2001/6
//
#include <stdio.h>
#include "sysreq.h"

static int	sysreq[SYSREQ_MAX];		// 初期設定データプール

void InitSysReq( void )
{
	int i;
	for(i=0;i<SYSREQ_MAX;i++) sysreq[i]=0;
	sysreq[ SYSREQ_MAXMODEL ] = 4096;
	sysreq[ SYSREQ_MAXOBJ ] = 512;
	sysreq[ SYSREQ_MAXTEX ] = 512;
	sysreq[ SYSREQ_MAXMOC ] = 4096;
	sysreq[ SYSREQ_PKTSIZE ] = 0x100000;
}


void SetSysReq( int reqid, int val )
{
	sysreq[ reqid ] = val;
}


int GetSysReq( int reqid )
{
	return sysreq[ reqid ];
}


