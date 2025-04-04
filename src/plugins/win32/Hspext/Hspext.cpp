
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>
#include <io.h>
#include <string.h>
#include <winbase.h>
#include <winuser.h>
#include <shlobj.h>

#include "../hpi3sample/hsp3plugin.h"

#include "fcpoly.h"

int WINAPI hspext_DllMain (HINSTANCE hInstance, DWORD fdwReason, PVOID pvReserved)
{
	if ( fdwReason==DLL_PROCESS_ATTACH ) {
		hgiof_init();
	}
	if ( fdwReason==DLL_PROCESS_DETACH ) {
		hgiof_term();
	}
	return TRUE ;
}


/*------------------------------------------------------------*/
/*
		Registry I/O routines
*/
/*------------------------------------------------------------*/

/*
REG_NONE                    ( 0 )   // No value type
REG_SZ                      ( 1 )   // Unicode nul terminated string
REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
REG_BINARY                  ( 3 )   // Free form binary
REG_DWORD                   ( 4 )   // 32-bit number
REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
REG_LINK                    ( 6 )   // Symbolic Link (unicode)
REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
REG_RESOURCE_REQUIREMENTS_LIST ( 10 )
*/

/*------------------------------------------------------------*/

static	HKEY	hKey=NULL;
static	char	section[128];
static	int		regstat=0;
static	DWORD	RKind[3]={ REG_DWORD, REG_SZ, REG_BINARY };

EXPORT BOOL WINAPI regdone( int p1, int p2, int p3, int p4 )
{
	//		close registry key (type0)
	//
	if (hKey!=NULL) {
		RegCloseKey(hKey);
		hKey=NULL;
	}
	return 0;
}


HKEY keybase( int p1 )
{
	switch(p1) {
	case 0:
		return HKEY_CURRENT_USER;
	case 1:
		return HKEY_LOCAL_MACHINE;
	case 2:
		return HKEY_USERS;
	case 3:
		return HKEY_CLASSES_ROOT;
	case 4:
		return HKEY_DYN_DATA;
	case 5:
		return HKEY_PERFORMANCE_DATA;
	}
	return NULL;
}


EXPORT BOOL WINAPI regkey( int p1, int *p2, int p3, int p4 )
{
	//		set registry key (type4)
	//			regkey  keygroup, keydir, read/write sw , 0
	//
	int a;
	HKEY k;

	regdone(0,0,0,0);
	k=keybase(p1);

	if (p3==0) {
		a=RegOpenKeyEx( k, (char *)p2, 0, KEY_ALL_ACCESS, &hKey);
	} else {
		a=RegCreateKeyEx( k, (char *)p2, 0, "",
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
			NULL, &hKey, NULL);
	}
	regstat=0;
	if ( a != ERROR_SUCCESS) return -1;
	regstat++;
	return 0;
}


EXPORT BOOL WINAPI regkill( int p1, char *p2, int p3, int p4 )
{
	//
	//		delete registry entry (type4)
	//			regkill	keygroup, keydir
	//
	int a;
	HKEY k;
	k=keybase(p1);
	a=RegDeleteKey( k,p2 );
	if ( a != ERROR_SUCCESS ) return -1;
	return 0;
}


EXPORT BOOL WINAPI getreg( void *p1, char *p2, int p3, int p4 )
{
	//
	//		get registry entry (type5)
	//			getreg	val, "section name", type, length
	//
	int a;
	unsigned long RVSize;

	if (regstat==0) return 3;

	RVSize=(unsigned long)p4;
	if (RVSize==0) RVSize=64;
	a=RegQueryValueEx( hKey, p2, NULL, &RKind[p3], (unsigned char *)p1, &RVSize );
	if ( a != ERROR_SUCCESS ) return -1;
	return 0;
}


EXPORT BOOL WINAPI setreg( void *p1, char *p2, int p3, int p4 )
{
	//
	//		set registry entry (type5)
	//			setreg	val, "section name", type, length
	//
	int a;
	unsigned long RVSize;

	if (regstat==0) return 3;

	if (p3==0) RVSize=4;
	else if (p3==1) RVSize=strlen((char *)p1);
	else RVSize=(unsigned long)p4;

	a=RegSetValueEx( hKey, p2, 0, RKind[p3], (unsigned char *)p1, RVSize );
	if ( a != ERROR_SUCCESS ) return -1;
	return 0;
}


EXPORT BOOL WINAPI reglist( char *p1, int p2, int p3, int p4 )
{
	//
	//		listing registry entry (type1)
	//			reglist	val,mode
	//
	int a,idx;
	char ntmp[256];
	DWORD rcvsize;
	FILETIME ft;

	if (regstat==0) return 3;

	idx=0;
	p1[0]=0;

	while(1) {
		rcvsize=256;*ntmp=0;
		if (p2==0) {
			a=RegEnumValue( hKey,idx,ntmp,&rcvsize,NULL,NULL,NULL,NULL );
		}
		else {
			a=RegEnumKeyEx( hKey,idx,ntmp,&rcvsize,NULL,NULL,NULL,&ft );
		}
		if ( a == ERROR_NO_MORE_ITEMS ) break;
		strcat( p1,ntmp );
		strcat( p1,"\r" );
		idx++;
	}
	return -idx;
}


/*------------------------------------------------------------*/
/*
		Windows status report routines
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI sysexit ( int p1, int p2, int p3, int p4 )
{
	//		System exit (type0)
	//			sysexit type
	//
	int	a;
	BOOL success;
	a=-1;
	if (p1==0) a=EWX_LOGOFF;
	if (p1==1) a=EWX_REBOOT;
	if (p1==2) a=EWX_SHUTDOWN;
	if (p1==3) a=EWX_POWEROFF;
	if (a==-1) return 3;
	success = ExitWindowsEx( a,0 );
	return 0;
}

/*
EXPORT BOOL WINAPI sysinfo ( char *p1, int p2, int p3, int p4 )
{
	//		System strings get (type1)
	//			sysinfo type
	//
	unsigned long len;
	char pp[128];
	BOOL success;
	DWORD version;
	DWORD *mss;
	SYSTEM_INFO si;
	MEMORYSTATUS ms;
	int *ires;

	len=(unsigned long)p3;if (len==0) len=64;
	ires=(int *)p1;

	if (p2&16) GetSystemInfo(&si);
	if (p2&32) {
		GlobalMemoryStatus(&ms);
		mss=(DWORD *)&ms;
		*ires=(int)mss[p2&15];
		return 0;
	}

	switch(p2) {
	case 0:
		strcpy(p1,"Windows");
		version = GetVersion();
		if ((version & 0x80000000) == 0) strcat(p1,"NT");
									else strcat(p1,"95");
		wsprintf(pp," ver%d.%d",version&0xff,(version&0xff00)>>8);
		strcat(p1,pp);
		break;
	case 1:
		success = GetUserName( p1,&len );
		break;
	case 2:
		success = GetComputerName(p1,&len );
		break;
	case 16:
		*ires=(int)si.dwProcessorType;
		break;
	}
	return 0;
}
*/

/*------------------------------------------------------------*/
/*
		clipboard function
*/
/*------------------------------------------------------------*/

EXPORT BOOL WINAPI clipsetg ( BMSCR *bm, int p1, int p2, int p3 )
{
	//
	//		screen->clipboard  (type2)
	//
	HDC hdc;
	HDC hdcMem;
	HBITMAP hBitmap;
	int sx,sy;

	hdc = GetWindowDC( bm->hwnd );
	hdcMem = CreateCompatibleDC( hdc );

	sx=bm->sx;sy=bm->sy;
	hBitmap = CreateCompatibleBitmap( hdc, sx, sy );
	SelectObject( hdcMem, hBitmap );
	BitBlt( hdcMem, 0, 0, sx,sy, hdc, 0, 0, SRCCOPY );

	OpenClipboard( bm->hwnd );
	EmptyClipboard();
	SetClipboardData( CF_BITMAP, hBitmap );
	CloseClipboard();

	DeleteDC( hdcMem );
	ReleaseDC( bm->hwnd, hdc );
	return 0;
}


EXPORT BOOL WINAPI clipset ( BMSCR *bm, char *p1, int p2, int p3 )
{
	//
	//		text->clipboard  (type6)
	//
	int i;
	int iLength;
	char *pMem;
	HANDLE hGlobal;

	iLength=strlen(p1)+1;
	hGlobal = GlobalAlloc(GHND, iLength + 1);
	pMem = (char *) GlobalLock(hGlobal);
	for(i = 0; i < iLength; i++) pMem[i] = p1[i];
	GlobalUnlock(hGlobal);

	OpenClipboard( bm->hwnd );
	EmptyClipboard();
	SetClipboardData( CF_OEMTEXT, hGlobal );
	CloseClipboard();
	return 0;
}


EXPORT BOOL WINAPI clipget ( char *p1, int p2, int p3, int p4 )
{
	//
	//		clipboard->text  (type1)
	//			clipget var, maxbyte
	//
	int len;
	int res;
	int a;
	char c1;
	char *pMem;
	HANDLE hGlobal;

	res=0;
	len=p2;
	if (len==0) len=64;

	OpenClipboard( NULL );

	if (IsClipboardFormatAvailable(CF_OEMTEXT)) {
		hGlobal=GetClipboardData( CF_OEMTEXT );
		pMem = (char *) GlobalLock(hGlobal);
		a=0;
		while(1) {
			if (a>=len) break;
			c1=pMem[a];
			p1[a++]=c1;
			if (c1==0) break;
		}
		GlobalUnlock(hGlobal);
	}
	else res=-1;

	CloseClipboard();
	return res;
}



