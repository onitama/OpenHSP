
//
//	supio.cpp functions
//
#ifndef __supio_h
#define __supio_h

#ifdef HSPUTF8
#define STRLEN utf8strlen
#else
#define STRLEN strlen
#endif

#ifdef HSPWIN
#ifdef HSPUTF8
#include "win32gui/supio_win_unicode.h"
#else
#include "win32gui/supio_win.h"
#endif
#endif

#ifdef HSPIOS
#include "ios/supio_ios.h"
#endif

#ifdef HSPNDK
#include "ndk/supio_ndk.h"
#endif

#ifdef HSPLINUX
#include "linux/supio_linux.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "emscripten/supio_emscripten.h"
#endif


#endif

