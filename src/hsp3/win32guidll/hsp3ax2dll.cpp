
#include <windows.h>

#include "hsp3ax2dll.h"

#include "../hsp3code.h"
#include "../stack.h"
#include "../strbuf.h"
#include "../hspvar_core.h"
#include "../win32gui/hsp3win.h"

extern int termcmd;

static HSPCTX *ctx;
static HSPEXINFO *exinfo;
static int *type;
static int *val;

static int cmdfunc_ax2dllcmd(int);
static void hsp3typeinit_ax2dllcmd(HSP3TYPEINFO*);

HINSTANCE hDllInstance;
BOOL IsInitialized = FALSE;

BOOL WINAPI DllMain( HINSTANCE hInstance,
					 DWORD fdwReason,
					 LPVOID lpvReserved )
{
	hDllInstance = hInstance;
	if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (IsInitialized)
		{
			if (termcmd != -1)
			{
				callretvoid(termcmd,0,(void**)0,(int*)0);//
			}
			hsp3win_bye();
		}
	}
	return TRUE;
}

static BOOL LoadHSP(void)
{
	int res;
#ifdef HSPDEBUG
	res = hsp3ax2dll_init( hDllInstance, "obj" );
#else
	res = hsp3ax2dll_init( hDllInstance, NULL );
#endif
	if (res)
	{
		return FALSE;
	}
	dll_init();
	return TRUE;
}

static BOOL Initialize(void)
{
	BOOL bRet;
	if (IsInitialized == FALSE)
	{
		bRet = LoadHSP();
		if (bRet)
		{
			IsInitialized = TRUE;
			return TRUE;
		}else{
			return FALSE;
		}
	}
	return TRUE;
}

int hsp3ax2dll_init(HINSTANCE hInstance,char *startfile)
{
	int res;
	res = hsp3win_init(hInstance,startfile);
	hsp3typeinit_ax2dllcmd(code_gettypeinfo(TYPE_USERDEF));
	return res;
}

static int cmdfunc_ax2dllcmd(int cmd)
{
	code_next();
	switch ( cmd )
	{
	case 0x000:
		code_gets();
		code_gets();
		code_gets();
		code_next();
		break;
	case 0x001:
		code_gets();
		break;
	default:
		throw HSPERR_UNSUPPORTED_FUNCTION;
	}
	return 0;
}

void hsp3typeinit_ax2dllcmd(HSP3TYPEINFO *info)
{
	ctx = info->hspctx;
	exinfo = info->hspexinfo;
	type = exinfo->nptype;
	val = exinfo->npval;
	
	info->cmdfunc = cmdfunc_ax2dllcmd;
}

void dll_init(void)
{
	code_call(ctx->mem_mcs);
}

int runproc( void )
{
	while(1) {

#ifdef HSPDEBUG
		if ( code_getdbgmode() ) code_dbgtrace();					// トレースモード時の処理
#endif
		if ( code_gettypeinfo( *type )->cmdfunc( *val ) ) {	// タイプごとの関数振り分け
			if ( ctx->runmode == RUNMODE_END ) {
				throw HSPERR_NONE;
			}
			if ( ctx->runmode == RUNMODE_RETURN ) {
				cmdfunc_return();
				break;
			} else {
				ctx->msgfunc( ctx );
			}
		}
	}

	return RUNMODE_RUN;//
}


void execdllfunc(HSPCTX * hspctx,int cmd,int num,void **args,int *argtypes)
{
  STRUCTDAT *st;
  int size;
  HSPROUTINE *r;
  char *p;
  STRUCTPRM *prm;
  char *out;
  st = &hspctx->mem_finfo[cmd];
  size = sizeof(HSPROUTINE) + st->size;
  r = (HSPROUTINE *)StackPushSize( TYPE_EX_CUSTOMFUNC, size );
  p = (char *)(r+1);
  prm = &hspctx->mem_minfo[ st->prmindex ];
  for(int i=0;i<num;i++)
  {
    out = p + prm->offset;
    switch(argtypes[i])
    {
      case MPTYPE_STRING://2
      {
        char *ss;
        ss = sbAlloc( (int)strlen(((char*)args[i])+1 ));
        strcpy( ss, (char*)args[i] );
        *(char **)out = ss;
        break;
      }
      case MPTYPE_DNUM://3
      {
        memcpy(out, args[i], sizeof(double));
        break;
      }
      case MPTYPE_INUM://4
      {
        *(int *)out = (int)args[i];
        break;
      }
    }
    prm++;
  }
  r->oldtack = hspctx->prmstack;
  hspctx->prmstack = (void *)p;
  r->mcsret = code_getpcbak();
  r->stacklev = hspctx->sublev++;
  r->param = st;
  code_setpc((unsigned short *)( hspctx->mem_mcs + (hspctx->mem_ot[ st->otindex ]) ));
  runproc();
}
extern "C" void callretvoid(int cmd,int num,void **args,int *argtypes)
{
  HSPCTX *hspctx;
  if (IsInitialized == FALSE){Initialize();}
  hspctx = code_getctx();
  execdllfunc(hspctx,cmd,num,args,argtypes);
  return;
}
extern "C" int callretint(int cmd,int num,void **args,int *argtypes)
{
  HSPCTX *hspctx;
  int stat;
  if (IsInitialized == FALSE){Initialize();}
  hspctx = code_getctx();
  execdllfunc(hspctx,cmd,num,args,argtypes);
  stat = hspctx->stat;
  return stat;
}
extern "C" char *callretstr(int cmd,int num,void **args,int *argtypes)
{
  HSPCTX *hspctx;
  char *refstr;
  if (IsInitialized == FALSE){Initialize();}
  hspctx = code_getctx();
  execdllfunc(hspctx,cmd,num,args,argtypes);
  refstr = hspctx->refstr;
  return refstr;
}
extern "C" double callretdouble(int cmd,int num,void **args,int *argtypes)
{
  HSPCTX *hspctx;
  double refdval;
  if (IsInitialized == FALSE){Initialize();}
  hspctx = code_getctx();
  execdllfunc(hspctx,cmd,num,args,argtypes);
  refdval = hspctx->refdval;
  return refdval;
}
