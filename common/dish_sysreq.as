;
; HSP3Dish SysReq define macros
;
#ifndef __dishsysreq__
#define __dishsysreq__

;	system request
;
#enum global SYSREQ_MAXMODEL=1
#enum global SYSREQ_MAXOBJ
#enum global SYSREQ_MAXTEX
#enum global SYSREQ_MAXMOC
#enum global SYSREQ_DXMODE
#enum global SYSREQ_DXHWND
#enum global SYSREQ_DXWIDTH
#enum global SYSREQ_DXHEIGHT
#enum global SYSREQ_COLORKEY
#enum global SYSREQ_RESULT
#enum global SYSREQ_RESVMODE
#enum global SYSREQ_PKTSIZE
#enum global SYSREQ_MAXEVENT
#enum global SYSREQ_PTRD3D
#enum global SYSREQ_PTRD3DDEV
#enum global SYSREQ_MDLANIM
#enum global SYSREQ_CALCNORMAL
#enum global SYSREQ_2DFILTER
#enum global SYSREQ_3DFILTER
#enum global SYSREQ_OLDCAM
#enum global SYSREQ_QUATALG
#enum global SYSREQ_DXVSYNC
#enum global SYSREQ_DEFTIMER
#enum global SYSREQ_NOMIPMAP
#enum global SYSREQ_DEVLOST
#enum global SYSREQ_MAXEMITTER
#enum global SYSREQ_THROUGHFLAG
#enum global SYSREQ_OBAQMATBUF
#enum global SYSREQ_2DFILTER2
#enum global SYSREQ_FPUPRESERVE
#enum global SYSREQ_CLSMODE
#enum global SYSREQ_CLSCOLOR
#enum global SYSREQ_CLSTEX
#enum global SYSREQ_TIMER
#enum global SYSREQ_PLATFORM
#enum global SYSREQ_FPS
#enum global SYSREQ_VSYNC
#enum global SYSREQ_MAXMATERIAL

#enum global SYSREQ_PHYSICS_RATE
#enum global SYSREQ_MAXOBAQ
#enum global SYSREQ_MAXLOG
#enum global SYSREQ_DEFAULT_WEIGHT
#enum global SYSREQ_DEFAULT_MOMENT
#enum global SYSREQ_DEFAULT_DAMPER
#enum global SYSREQ_DEFAULT_FRICTION

#enum global SYSREQ_MESCACHE_MAX
#enum global SYSREQ_DLIGHT_MAX
#enum global SYSREQ_PLIGHT_MAX
#enum global SYSREQ_SLIGHT_MAX
#enum global SYSREQ_LOGWRITE
#enum global SYSREQ_FIXEDFRAME

#enum global SYSREQ_DRAWNUMOBJ
#enum global SYSREQ_DRAWNUMPOLY
#enum global SYSREQ_USEGPBFONT
#enum global SYSREQ_FIXMESALPHA
#enum global SYSREQ_OLDBOXF

#const global PLATFORM_WINDOWS (0)
#const global PLATFORM_IOS (1)
#const global PLATFORM_ANDROID (2)
#const global PLATFORM_WEBGL (3)
#const global PLATFORM_LINUX (4)
#const global PLATFORM_RASPBIAN (5)

#define global SYSREQ_DEBUG $10000

#endif


