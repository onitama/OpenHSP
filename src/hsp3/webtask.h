
//
//	Web Access Task Class
//
#ifndef __webtask_h
#define __webtask_h

#ifdef HSPWIN
#include "win32/webtask_win.h"
#endif

#ifdef HSPIOS
#include "ios/webtask_ios.h"
#endif

#ifdef HSPNDK
#include "ndk/webtask_ndk.h"
#endif

#ifdef HSPLINUX
#include "linux/webtask_linux.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "emscripten/webtask_emscripten.h"
#endif

#endif
