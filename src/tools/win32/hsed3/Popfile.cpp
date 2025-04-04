/*------------------------------------------
   POPFILE.C -- Popup Editor File Functions
  ------------------------------------------*/

#include <windows.h>
#include <direct.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include "poppad.h"

static OPENFILENAME ofn ;
static char szMyDir[_MAX_PATH];

extern int	startflag;
extern char startdir[_MAX_PATH];

void PopFileInitialize (HWND hwnd)
     {
     static char szFilter[] = "HSP Source (*.hsp)\0*.hsp\0"  \
							  "HSP Header (*.as)\0*.as\0"  \
                              "Text Files (*.txt)\0*.txt\0" \
                              "All Files (*.*)\0*.*\0\0" ;

     ofn.lStructSize       = sizeof (OPENFILENAME) ;
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter ;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = _MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = "hsp" ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
     }

BOOL PopFileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
     {
 
	 ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

	 if ( startflag == STARTDIR_NONE ) {
		 ofn.lpstrInitialDir   = NULL;
	 } else {
		 _getcwd( szMyDir, _MAX_PATH );
		 ofn.lpstrInitialDir   = szMyDir ;
	 }

	 return GetOpenFileName (&ofn) ;
     }

BOOL PopFileSaveDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
     {

	 ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_OVERWRITEPROMPT ;

	 if ( startflag == STARTDIR_NONE ) {
		 ofn.lpstrInitialDir   = NULL;
	 } else {
		 _getcwd( szMyDir, _MAX_PATH );
		 ofn.lpstrInitialDir   = szMyDir ;
	 }

     return GetSaveFileName (&ofn) ;
     }

static long PopFileLength (FILE *file)
     {
     int iCurrentPos, iFileLength ;

     iCurrentPos = ftell (file) ;

     fseek (file, 0, SEEK_END) ;

     iFileLength = ftell (file) ;

     fseek (file, iCurrentPos, SEEK_SET) ;

     return iFileLength ;
     }

BOOL PopFileRead (HWND hwndEdit, PSTR pstrFileName)
     {
     FILE  *file ;
     int    iLength ;
     PSTR   pstrBuffer ;

     if (NULL == (file = fopen (pstrFileName, "rb")))
          return FALSE ;

     iLength = PopFileLength (file) ;

     if (NULL == (pstrBuffer = (PSTR) malloc (iLength+4)))
          {
          fclose (file) ;
          return FALSE ;
          }

     fread (pstrBuffer, 1, iLength, file) ;
     fclose (file) ;
     pstrBuffer[iLength] = '\0' ;

     SetWindowText (hwndEdit, pstrBuffer) ;
     free (pstrBuffer) ;

     return TRUE ;
     }

BOOL PopFileWrite (HWND hwndEdit, PSTR pstrFileName)
     {
     FILE  *file ;
     int    iLength ;
     PSTR   pstrBuffer ;

     if (NULL == (file = fopen (pstrFileName, "wb"))){
		return FALSE ;
	 }

     iLength = GetWindowTextLength (hwndEdit) ;

     if (NULL == (pstrBuffer = (PSTR) malloc (iLength + 1)))
          {
          fclose (file) ;
          return FALSE ;
          }

     GetWindowText (hwndEdit, pstrBuffer, iLength + 1) ;

     if (iLength != (int) fwrite (pstrBuffer, 1, iLength, file))
          {
          fclose (file) ;
          free (pstrBuffer) ;
          return FALSE ;
          }

     fclose (file) ;
     free (pstrBuffer) ;

     return TRUE ;
     }
