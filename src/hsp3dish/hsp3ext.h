
//
//	hsp3ext.cpp header
//
#ifndef __hsp3ext_h
#define __hsp3ext_h

#ifdef HSPWIN
#include "../hsp3/win32gui/hsp3ext_win.h"
#endif

#ifdef HSPIOS
#include "ios/hsp3ext_ios.h"
#endif

#ifdef HSPNDK
#include "ndk/hsp3ext_ndk.h"
#endif

#ifdef HSPLINUX
#include "linux/hsp3ext_linux.h"
#endif


#endif
