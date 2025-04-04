
//
//		System request manager
//			onion software/onitama 2001/6
//
#include <stdio.h>
#include <stdarg.h>
#include "sysreq.h"

static float sysreq[SYSREQ_MAX];		// 初期設定データプール
static char dbgmsg[512];				// デバッグ用メッセージプール

void InitSysReq( void )
{
	int i;
	for(i=0;i<SYSREQ_MAX;i++) sysreq[i]=0;
	dbgmsg[0] = 0;
}


void SetSysReq( int reqid, int val )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return;
	sysreq[ reqid ] = (float)val;
}


void SetSysReq( int reqid, float val )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return;
	sysreq[ reqid ] = val;
}


float GetSysReq( int reqid )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return 0.0f;
	return sysreq[ reqid ];
}


int GetSysReqInt( int reqid )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return 0;
	return (int)sysreq[ reqid ];
}


void DebugMsg( char *format, ... )
{
	va_list args;
	va_start(args, format);
	vsprintf(dbgmsg, format, args);
	va_end(args);
}

char *GetDebug( void )
{
	return dbgmsg;
}

