
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
#include "../hsp3/win32gui/supio_win_unicode.h"
#else
#include "../hsp3/win32gui/supio_win.h"
#endif
#endif

#ifdef HSPIOS
#include "../hsp3/ios/supio_ios.h"
#endif

#ifdef HSPNDK
#include "../hsp3/ndk/supio_ndk.h"
#endif

#ifdef HSPLINUX
#include "../hsp3/linux/supio_linux.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "../hsp3/emscripten/supio_emscripten.h"
#endif


#endif

