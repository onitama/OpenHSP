
//
//		Advanced Function Plugin for HSP
//				onion software/onitama 1998
//

#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <shlobj.h>
#include <objbase.h>

#include "../hpi3sample/hsp3plugin.h"

/*------------------------------------------------------------*/

static int hsrc,hdst;
static OFSTRUCT srcinfo, dstinfo;

static char dstdir[MAX_PATH];


EXPORT BOOL WINAPI lzdist( BMSCR *bm, char *p2, int p3, int p4 )
{
	//		LZDIST (type6)
	//			lzdist dist-dir
	//
	strcpy(dstdir,p2);
	return 0;
}


EXPORT BOOL WINAPI lzcopy( BMSCR *bm, char *p2, int p3, char *p4 )
{
	//		LZCOPY (type$16)
	//			lzcopy fname
	//
	int a;
	char fname[MAX_PATH];
	char sname[MAX_PATH];
	char dname[MAX_PATH];

	strcpy( fname,p2 );
	a=GetExpandedName( fname,sname );
	if (a!=1) return -2;
	strcpy( p4,sname );
	//
	strcpy( dname,dstdir );
	strcat( dname,sname );

	hsrc=LZOpenFile( fname,&srcinfo,OF_READ );
	if (hsrc<0) return -1;
	hdst=LZOpenFile( dname,&dstinfo,OF_CREATE );
	if (hdst<0) return -1;
	LZCopy( hsrc,hdst );
	LZClose( hdst );
	LZClose( hsrc );
	return 0;
}


/*------------------------------------------------------------*/

//	ショートカットを作る
//
static HRESULT CreateLink(LPCSTR lpszPathObj, LPSTR lpszPathLink)
{
    HRESULT hres;
    IShellLink* psl;

    CoInitialize(NULL);

    hres = CoCreateInstance(::CLSID_ShellLink, NULL,
            CLSCTX_INPROC_SERVER, ::IID_IShellLink, (LPVOID FAR*)&psl);

    if (SUCCEEDED(hres)) {
        ::IPersistFile* ppf;
        psl->SetPath(lpszPathObj);
        hres = psl->QueryInterface(::IID_IPersistFile, (LPVOID FAR*)&ppf);
        if(SUCCEEDED(hres))  {
            wchar_t wsz[MAX_PATH + 1];
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    CoUninitialize();
    return hres;
}


EXPORT BOOL WINAPI fxlink( char *p1, char *p2, int p3, int p4 )
{
	//		make shortcut file (type5)
	//			fxlink a,"src file path"
	//
	char sname[MAX_PATH];
	strcpy(sname,p1);
	strcat(sname,".lnk");
    if (SUCCEEDED(CreateLink(p2,sname))) return 0;
	return -1;
}


/*------------------------------------------------------------*/
