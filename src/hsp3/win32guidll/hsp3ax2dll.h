
#include <windows.h>

#include "../hsp3struct.h"

int hsp3ax2dll_init(HINSTANCE,char*);
void dll_init(void);
int runproc(void);
void execdllfunc(HSPCTX * hspctx,int cmd,int num,void **args,int *argtypes);
extern "C" void callretvoid(int cmd,int num,void **args,int *argtypes);
extern "C" int callretint(int cmd,int num,void **args,int *argtypes);
extern "C" char *callretstr(int cmd,int num,void **args,int *argtypes);
extern "C" double callretdouble(int cmd,int num,void **args,int *argtypes);
