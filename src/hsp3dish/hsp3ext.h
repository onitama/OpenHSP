
//
//	hsp3ext.cpp header
//
#ifndef __hsp3ext_h
#define __hsp3ext_h

#ifdef HSPWIN
#include "../hsp3/win32gui/hsp3ext_win.h"
#endif

#ifdef HSPIOS
#include "../hsp3/ios/hsp3ext_ios.h"
#endif

#ifdef HSPNDK
#include "../hsp3/ndk/hsp3ext_ndk.h"
#endif

#ifdef HSPLINUX
#include "../hsp3/linux/hsp3ext_linux.h"
#endif

#ifdef HSPEMSCRIPTEN
#include "../hsp3/emscripten/hsp3ext_emscripten.h"
#endif

#endif
