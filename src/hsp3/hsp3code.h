
//
//	hsp3code.cpp header
//
#ifndef HSP3CODE_H
#define HSP3CODE_H

#include "hsp3debug.h"
#include "hsp3struct.h"

extern PVal *mpval;

void code_init( void );
void code_bye( void );
void code_cleanup(void);
void code_termfunc( void );
void code_setctx( HSPCTX *ctx );
void code_resetctx( HSPCTX *ctx );
HSPCTX *code_getctx( void );
void code_setpc( const unsigned short *pc );
void code_setpci( const unsigned short *pc );
void code_call(const unsigned short *pc);
void code_callback(const unsigned short *pc);
void cmdfunc_return( void );
unsigned short *code_getpcbak( void );

int code_execcmd( void );
int code_execcmd2( void );
int code_exec_wait( int tick );
int code_exec_await( int tick );

HSPERROR code_geterror( void );
void code_puterror( HSPERROR error );

void code_next( void );
int code_get( void );
int code_getexflg( void );

char *code_gets( void );
char *code_getds( const char *defval );
char *code_getdsi( const char *defval );
int code_geti( void );
int code_getdi( const int defval );
double code_getd( void );
double code_getdd( const double defval );
PVal *code_getpval( void );
char *code_getvptr( PVal **pval, int *size );

APTR code_getva( PVal **pval );
void code_setva( PVal *pval, APTR aptr, int type, const void *ptr );
//PDAT *code_getv( PVal **pval );
//void code_setv( PVal *pval, PDAT *dat, int type, void *ptr );

unsigned short *code_getlb( void );
unsigned short *code_getlb2( void );

STRUCTDAT *code_getstruct( void );
STRUCTPRM *code_getstprm( void );
STRUCTDAT *code_getcomst( void );
char *code_get_proxyvar( char *ptr, int *mptype );
APTR code_getv_proxy( PVal **pval, MPVarData *var, int mptype );

#define CODE_EXPANDSTRUCT_OPT_NONE 0
#define CODE_EXPANDSTRUCT_OPT_LOCALVAR 1

void code_expandstruct( char *p, STRUCTDAT *st, int option );
void code_setvs( PVal *pval, APTR aptr, int type, void *ptr, int size, int subid );

char *code_stmpstr( char *src );
char *code_stmp( int size );
char *code_getsptr( int *type );

int code_debug_init( void );
int code_getdebug_line( void );
int code_getdebug_line( unsigned short *pt );
char *code_getdebug_name( void );
int code_getdebug_seekvar( const char *name );
char *code_getdebug_varname( int val_id );

int code_event( int event, int prm1, int prm2, void *prm3 );
void code_bload( char *fname, int ofs, int size, void *ptr );
void code_bsave( char *fname, int ofs, int size, void *ptr );

IRQDAT *code_getirq( int id );
IRQDAT *code_seekirq( int actid, int custom );
IRQDAT *code_addirq( void );
int code_isirq( int id );
int code_isuserirq( void );
int code_sendirq( int id, int iparam, int wparam, int lparam );
int code_checkirq( int id, int message, int wparam, int lparam );
void code_execirq( IRQDAT *irq, int wparam, int lparam );
void code_setirq( int id, int opt, int custom, unsigned short *ptr );
int code_irqresult( int *value );
void code_enableirq( int id, int sw );
int code_catcherror(HSPERROR code);

HSP3TYPEINFO *code_gettypeinfo( int type );
void code_enable_typeinfo( HSP3TYPEINFO *info );

int code_getdbgmode( void );
HSP3DEBUG *code_getdbg( void );
char *code_inidbg( void );
void code_adddbg( char *name, int val );
void code_adddbg( char *name, char *str );

void code_dbg_global( void );
char *code_dbgvalue( int type );
char *code_dbgvarinf( char *target, int option );
void code_dbgcurinf( void );
void code_dbgclose( char *buf );
int code_dbgset( int id );
char *code_dbgcallstack( void );
void code_dbgtrace( void );

void code_delstruct( PVal *in_pval, APTR in_aptr );
void code_delstruct_all( PVal *pval );

#define fpconv( fp )  ( reinterpret_cast< void * >(fp) )

char* code_strp(int dsptr);
int code_strexchange(char *fname, int option);

#endif
