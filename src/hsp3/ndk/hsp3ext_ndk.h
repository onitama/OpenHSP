
//
//	hsp3ext.cpp header
//
#ifndef __hsp3ext_ndk_h
#define __hsp3ext_ndk_h

#include "../hsp3struct.h"

#ifdef __cplusplus
extern "C" {
#endif

void hsp3typeinit_dllcmd( HSP3TYPEINFO *info );
void hsp3typeinit_dllctrl( HSP3TYPEINFO *info );
void hsp3dish_setdevinfo( void );

char *hsp3ext_sysinfo(int p2, int* res, char* outbuf);
char* hsp3ext_getdir(int id);
void hsp3ext_execfile(char* stmp, char* ps, int mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
