
//
//	hgiox.cpp structures
//
#ifndef __hgiox_h
#define __hgiox_h

#include "hginc.h"

int hgio_device_ready(void);
int hgio_device_restore( void );
void hgio_text_render( void );
int hgio_dialog_ex(HSPCTX* ctx, Bmscr* bmscr, int mode, char* str1, char* str2);
void hgio_resize_window(int x, int y);

#endif
