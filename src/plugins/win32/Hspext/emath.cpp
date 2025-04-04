
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>
#include <io.h>
#include <string.h>
#include <math.h>
#include <winbase.h>
#include <winuser.h>
#include <shlobj.h>

#include "../hpi3sample/hsp3plugin.h"

/*------------------------------------------------------------*/
/*
		easy mathmatics routines
*/
/*------------------------------------------------------------*/

static	int	em_base=256;
static	int	em_sft=8;
static	double	emd_base=256;
static	double	pi = 3.1415926535;
static	double	parg = 40.74366543;

EXPORT BOOL WINAPI emath ( int p1, int p2, int p3, int p4 )
{
	//		ez-math initalize (type0)
	//			emath fp-base
	//
	int a;
	a=p1;
	if (a==0) a=8;
	if ((a<1)||(a>30)) return 3;
	em_sft=a;em_base=1<<a;
	emd_base=(double)em_base;
	parg=emd_base/(pi*2);
	return 0;
}


EXPORT BOOL WINAPI emstr ( char *p1, int p2, int p3, int p4 )
{
	//		ez-math -> strings convert (type1)
	//			emstr str_val, emval, length
	//
	int dig;
	double a;
	int i;
	dig=p3;if (dig==0) dig=10;
	a=(double)p2;a/=emd_base;
	_gcvt( a,dig,p1 );
	i=strlen(p1)-1;
	if (p1[i]=='.') p1[i]=0;
	return 0;
}


EXPORT BOOL WINAPI emcnv ( int *p1, char *p2, int p3, int p4 )
{
	//		strings -> ez-math convert (type5)
	//			emstr val, "1.123..."
	//
	double a;
	a=strtod( p2,NULL );
	a*=emd_base;*p1=(int)a;
	return 0;
}


EXPORT BOOL WINAPI emint ( int *p1, int p2, int p3, int p4 )
{
	//		strings -> ez-math convert (type1)
	//			emint val, em_val
	//
	*p1=p2>>em_sft;
	return 0;
}


EXPORT BOOL WINAPI emsin ( int *p1, int p2, int p3, int p4 )
{
	//		ez-math sin (type1)
	//			emsin val,arg
	//
	double a,b;
	a=(double)(p2&(em_base-1));
	b=sin(a/parg);
	b*=emd_base;*p1=(int)b;
	return 0;
}


EXPORT BOOL WINAPI emcos ( int *p1, int p2, int p3, int p4 )
{
	//		ez-math cos (type1)
	//			emcos val,arg
	//
	double a,b;
	a=(double)(p2&(em_base-1));
	b=cos(a/parg-pi);
	b*=emd_base;*p1=(int)b;
	return 0;
}


EXPORT BOOL WINAPI ematan ( int *p1, int p2, int p3, int p4 )
{
	//		ez-math atan (type1)
	//			ematan val,x,y
	//
	double a;
	a=atan2( (double)-p2,(double)p3 );
	a=(a+pi)*parg;*p1=(int)a;
	return 0;
}


EXPORT BOOL WINAPI emsqr ( int *p1, int p2, int p3, int p4 )
{
	//		ez-math square root (type1)
	//			emsqr val,emval
	//
	double a,b;
	a=(double)p2;
	b=sqrt(a/emd_base);
	b*=emd_base;*p1=(int)b;
	return 0;
}

