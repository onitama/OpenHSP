#ifndef		__SYSREQ_H__
#define		__SYSREQ_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/*---------------------------------------------------------------------------*/

// システム初期化コード
enum {
SYSREQ_NONE = 0,
SYSREQ_MAXMODEL,
SYSREQ_MAXOBJ,
SYSREQ_MAXTEX,
SYSREQ_MAXMOC,
SYSREQ_DXMODE,
SYSREQ_DXHWND,
SYSREQ_DXWIDTH,
SYSREQ_DXHEIGHT,
SYSREQ_COLORKEY,
SYSREQ_RESULT,
SYSREQ_RESVMODE,
SYSREQ_PKTSIZE,
SYSREQ_MAX
};

void InitSysReq( void );
void SetSysReq( int reqid, int val );
int GetSysReq( int reqid );

/*---------------------------------------------------------------------------*/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
