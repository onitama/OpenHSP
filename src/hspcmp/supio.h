
//
//	supio.cpp functions
//

#ifdef HSPWIN
#include "win32/supio_win.h"
#endif
#ifdef HSPLINUX
#include "linux/supio_linux.h"
#endif
#ifdef HSPEMSCRIPTEN
#include "emscripten/supio_emscripten.h"
#endif

