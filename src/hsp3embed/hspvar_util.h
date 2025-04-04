
//
//	hspvar_util.cpp header
//
#ifndef __hspvar_util_h
#define __hspvar_util_h

//	CHSP3 Task callback function
//
typedef void (* CHSP3_TASK) (void);

//	CHSP3 original type (for stack)
//
#define HSPVAR_FLAG_MARK 0
#define HSPVAR_FLAG_VAR -1
#define HSPVAR_FLAG_DEFAULT -2

//	Utility
//
void VarUtilInit( void );
void VarUtilTerm( void );

void TaskExec( void );
void TaskSwitch( int label );
void HspPostExec( void );
int GetTaskID( void );

void DebugStackPeek( void );
void DebugMsg( char *msg );

//  HSPVAR support functions
//
extern PVal *mem_var;
extern int	prmstacks;

//void PushInt( int val );
//void PushDouble( double val );
//void PushLabel( int val );
#define PushInt StackPushi
#define PushDouble StackPushd
#define PushLabel StackPushl

void PushStr( char *st );
void PushVar( PVal *pv, int aval );
void PushVAP( PVal *pval, int aval );
void PushVarFromVAP( PVal *pval, int aval );
void PushDefault( void );
void PushFuncEnd( void );

void PushFuncPrm( int num );
void PushFuncPrmI( int num );
void PushFuncPrmD( int num );
void PushFuncPrm( int num, int aval );
void PushFuncPAP( int num, int aval );
PVal *FuncPrm( int num );
PVal *FuncPrmVA( int num );
PVal *LocalPrm( int num );
void PushVarOffset( PVal *pval );

void PushExtvar( int val, int pnum );
void PushIntfunc( int val, int pnum );
void PushSysvar( int val, int pnum );
void PushDllfunc( int val, int pnum );
void PushDllctrl( int val, int pnum );
void PushModcmd( int val, int pnum );
void PushUsrfunc( int val, int pnum );

void CalcAddI( void );
void CalcSubI( void );
void CalcMulI( void );
void CalcDivI( void );
void CalcModI( void );
void CalcAndI( void );
void CalcOrI( void );
void CalcXorI( void );
void CalcEqI( void );
void CalcNeI( void );
void CalcGtI( void );
void CalcLtI( void );
void CalcGtEqI( void );
void CalcLtEqI( void );
void CalcRrI( void );
void CalcLrI( void );

void VarSet( PVal *pv, int aval, int pnum );
void VarSet( PVal *pv, int aval );
void VarSet2( PVal *pval );
void VarInc( PVal *pv, int aval );
void VarDec( PVal *pv, int aval );
void VarCalc( PVal *pv, int aval, int op );

//  command support functions
//
bool HspIf( void );
void Extcmd( int cmd, int pnum );
void Modcmd( int cmd, int pnum );
void Dllfunc( int cmd, int pnum );
void Dllctrl( int cmd, int pnum );
void Prgcmd( int cmd, int pnum );
void Intcmd( int cmd, int pnum );
void Usrfunc( int cmd, int pnum );

#endif
