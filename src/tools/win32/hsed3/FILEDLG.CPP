/*------------------------------------------
	File Load/Save common dialog routines
  ------------------------------------------*/
/*


#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static HWND hwbak;
static OPENFILENAME ofn ;
static char szFileName[_MAX_PATH] ;
static char szTitleName[_MAX_FNAME + _MAX_EXT] ;
static char szFilter[128];

void PopFileInitialize (HWND hwnd)
     {
     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.lpstrFile         = szFileName ;
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrFileTitle    = szTitleName ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = NULL ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
     }


void fd_ini( HWND hwnd )
{
	hwbak=hwnd;
	szFileName[0]=0;
	szTitleName[0]=0;
	PopFileInitialize(hwnd);
}

char *fd_getfname( void )
{
	return szFileName;
}

BOOL fd_load( int mode )
{
	if (mode==1) {
		strcpy(szFileName,"*.tim");
		strcpy( szFilter, "TIM file (*.TIM)\0*.tim\0\0" );
	}
	else {
		strcpy(szFileName,"*.dpm");
		strcpy( szFilter,
				"DPM file (*.DPM)\0*.dpm\0All Files (*.*)\0*.*\0\0" );
	}

	ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
	return GetOpenFileName (&ofn) ;
}


BOOL fd_save( void )
{
	strcpy(szFileName,"*.dpm");
	strcpy( szFilter,
			"DPM file (*.DPM)\0*.dpm\0All Files (*.*)\0*.*\0\0" );
	ofn.Flags             = OFN_OVERWRITEPROMPT ;
	return GetSaveFileName (&ofn) ;
}

*/