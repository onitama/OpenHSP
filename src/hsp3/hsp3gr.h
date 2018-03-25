
//
//	hsp3gr.cpp header
//
#ifndef __hsp3gr_h
#define __hsp3gr_h

#ifdef HSPWIN
#ifdef HSPWINGUI
#include "win32gui/hsp3gr_wingui.h"
#else
#ifdef HSPDISH
#include "win32dish/hsp3gr_dish.h"
#else
#include "win32/hsp3gr_win.h"
#endif
#endif
#endif

#ifdef HSPIOS
#include "ios/hsp3gr_ios.h"
#endif

#ifdef HSPNDK
#include "ndk/hsp3gr_ndk.h"
#endif

#ifdef HSPLINUX
#include "linux/hsp3gr_linux.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "emscripten/hsp3gr_emscripten.h"
#endif


#endif
