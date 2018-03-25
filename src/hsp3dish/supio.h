
//
//	supio.cpp functions
//
#ifndef __supio_h
#define __supio_h

#define STRLEN strlen

#ifdef HSPWIN
#include "../hsp3/win32gui/supio_win.h"
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

