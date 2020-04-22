
//
//		System request manager
//			onion software/onitama 2001/6
//
#include <stdio.h>
#include <stdarg.h>
#include "sysreq.h"

#include "hgio.h"
#include "supio.h"

static int	sysreq[SYSREQ_MAX];		// 初期設定データプール
static float sysreqf[SYSREQ_MAX];		// 初期設定データプール
static char dbgmsg[512];			// デバッグ用メッセージプール

void SetSysReq( int reqid, int val )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return;
	sysreq[ reqid ] = val;
	sysreqf[ reqid ] = (float)val;
}


int GetSysReq( int reqid )
{
	switch( reqid ) {
	case SYSREQ_TIMER:
		return hgio_gettick();
	default:
		break;
	}
	return sysreq[ reqid ];
}


void SetSysReqF( int reqid, float val )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return;
	sysreqf[ reqid ] = val;
	sysreq[ reqid ] = (int)val;
}


float GetSysReqF( int reqid )
{
	if (( reqid < 0 )||( reqid >= SYSREQ_MAX )) return 0.0f;
	return sysreqf[ reqid ];
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

void InitSysReq( void )
{
	int i;
	for(i=0;i<SYSREQ_MAX;i++) {
		sysreq[i]=0;
		sysreqf[i]=0.0f;
	}
	sysreq[ SYSREQ_MAXMODEL ] = 128;
	sysreq[ SYSREQ_MAXOBJ ] = 1024;
	sysreq[ SYSREQ_MAXTEX ] = 128;
	sysreq[ SYSREQ_MAXMOC ] = 4096;
	sysreq[ SYSREQ_PKTSIZE ] = 0x100000;
	sysreq[ SYSREQ_MAXEVENT ] = 1024;
	sysreq[ SYSREQ_MDLANIM ] = 16;
	sysreq[ SYSREQ_MAXEMITTER ] = 16;
	sysreq[ SYSREQ_OBAQMATBUF ] = 1024;

	sysreq[ SYSREQ_2DFILTER ] = 1;			// D3DTEXF_POINT
	sysreq[ SYSREQ_2DFILTER2 ] = 1;			// D3DTEXF_POINT
	sysreq[ SYSREQ_3DFILTER ] = 2;			// D3DTEXF_LINEAR

	sysreq[ SYSREQ_MAXMATERIAL ] = 128;

	SetSysReq( SYSREQ_MESCACHE_MAX, 256 );

	sysreq[SYSREQ_DLIGHT_MAX] = 1;			// number of Directional Light(HGIMG4)
	sysreq[SYSREQ_PLIGHT_MAX] = 0;			// number of Point Light(HGIMG4)
	sysreq[SYSREQ_SLIGHT_MAX] = 0;			// number of Spot Light(HGIMG4)

	sysreq[SYSREQ_LOGWRITE] = 1;			// HGIMG4 log write
	sysreq[SYSREQ_FIXEDFRAME] = -1;			// fixed frame rate


#ifdef HSPIOS
    sysreq[ SYSREQ_PLATFORM ] = PLATFORM_IOS;
#endif    
#ifdef HSPNDK
    sysreq[ SYSREQ_PLATFORM ] = PLATFORM_ANDROID;
#endif    
#ifdef HSPEMSCRIPTEN
	sysreq[ SYSREQ_PLATFORM ] = PLATFORM_EMSCRIPTEN;
#endif
#ifdef HSPLINUX
#ifdef HSPRASPBIAN
	sysreq[SYSREQ_PLATFORM] = PLATFORM_RASPBIAN;
#else
	sysreq[SYSREQ_PLATFORM] = PLATFORM_LINUX;
#endif
#endif

	dbgmsg[0] = 0;
}


