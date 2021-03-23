
//
//	hsp3cl.cpp header
//
#ifndef __hsp3cl_h
#define __hsp3cl_h

int hsp3cl_exec( void );
int hsp3cl_init( char *startfile );
void hsp3cl_option( int opt );
void hsp3cl_cmdline( char *cmdline );
void hsp3cl_modname( char *modname );
void hsp3win_dialog( char *mes );

#define HSP3CL_OPT1_RESOUT 1
#define HSP3CL_OPT1_ERRSTOP 2

#endif
