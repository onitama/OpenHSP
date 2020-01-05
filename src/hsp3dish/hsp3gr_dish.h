
//
//	hsp3gr_dish.cpp header
//
#ifndef __hsp3gr_dish_h
#define __hsp3gr_dish_h

#include "../hsp3/hsp3struct.h"
#include "hspwnd_dish.h"

void hsp3typeinit_extcmd( HSP3TYPEINFO *info );
void hsp3typeinit_extfunc( HSP3TYPEINFO *info );
void hsp3notify_extcmd( void );

void hsp3gr_dbg_gui( void );
void hsp3extcmd_pause( void );
void hsp3extcmd_resume( void );

HSP3DEVINFO *hsp3extcmd_getdevinfo( void );
void hsp3extcmd_sysvars( int inst, int hwnd, int hdc );
void hsp3excmd_rebuild_window(void);

#endif
