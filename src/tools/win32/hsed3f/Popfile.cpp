/*------------------------------------------
   POPFILE.C -- Popup Editor File Functions
  ------------------------------------------*/

#include <windows.h>
#include <direct.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include "poppad.h"
#include "FootyDLL.h"
#include "tabmanager.h"

static OPENFILENAME ofn, ofn2 ;
static char szMyDir[_MAX_PATH];

extern int	startflag;
extern char startdir[_MAX_PATH];

extern int activeFootyID ;
extern int activeID ;

void PopFileInitialize (HWND hwnd)
     {
     static char szFilter[] = 
	                          "HSP Source (*.hsp)\0*.hsp\0"  \
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
     ofn.lpstrDefExt       = "txt" ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;

	 static char szFilter2[] = 
	                          "Excutable Files (*.EXE,*.COM,*.BAT)\0*.exe;*.com;*.bat\0"  \
                              "All Files (*.*)\0*.*\0\0" ;

     ofn2.lStructSize       = sizeof (OPENFILENAME) ;
     ofn2.hwndOwner         = hwnd ;
     ofn2.hInstance         = NULL ;
     ofn2.lpstrFilter       = szFilter2 ;
     ofn2.lpstrCustomFilter = NULL ;
     ofn2.nMaxCustFilter    = 0 ;
     ofn2.nFilterIndex      = 0 ;
     ofn2.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn2.nMaxFile          = _MAX_PATH ;
     ofn2.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn2.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofn2.lpstrInitialDir   = NULL ;
     ofn2.lpstrTitle        = NULL ;
     ofn2.Flags             = 0 ;             // Set in Open and Close functions
     ofn2.nFileOffset       = 0 ;
     ofn2.nFileExtension    = 0 ;
     ofn2.lpstrDefExt       = "exe" ;
     ofn2.lCustData         = 0L ;
     ofn2.lpfnHook          = NULL ;
     ofn2.lpTemplateName    = NULL ;
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

	 return GetOpenFileName (&ofn);
     }

BOOL PopFileOpenDlg2 (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
     {
 
	 ofn2.hwndOwner         = hwnd ;
     ofn2.lpstrFile         = pstrFileName ;
     ofn2.lpstrFileTitle    = pstrTitleName ;
     ofn2.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;

	 if ( startflag == STARTDIR_NONE ) {
		 ofn2.lpstrInitialDir   = NULL;
	 } else {
		 _getcwd( szMyDir, _MAX_PATH );
		 ofn2.lpstrInitialDir   = szMyDir ;
	 }

	 return GetOpenFileName (&ofn2);
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

BOOL PopFileRead (int nFootyID, PSTR pstrFileName)
     {
     FILE    *file ;
     int      iLength ;
     PSTR     pstrBuffer ;

	 if (NULL == (file = fopen (pstrFileName, "rb")))
		 return FALSE ;

     iLength = PopFileLength (file) ;

	 if (NULL == (pstrBuffer = (PSTR) malloc (iLength+4))){
		 fclose (file) ;
		 return FALSE ;
	 }

     fread (pstrBuffer, 1, iLength, file) ;
     fclose (file) ;
	 pstrBuffer[iLength] = '\0' ;

//     SetWindowText (hwndEdit, pstrBuffer) ;
	 FootySetText(nFootyID, pstrBuffer);
     free (pstrBuffer) ;

     return TRUE ;
     }

BOOL PopFileWrite (int FootyID, PSTR pstrFileName)
     {
     FILE  *file ;
     int    iLength ;
     PSTR   pstrBuffer ;

     if (NULL == (file = fopen (pstrFileName, "wb"))){
		return FALSE ;
	 }

//     iLength = GetWindowTextLength (hwndEdit) ;
	 iLength = FootyGetTextLength(FootyID);

     if (NULL == (pstrBuffer = (PSTR) malloc (iLength + 1)))
          {
          fclose (file) ;
          return FALSE ;
          }

//     GetWindowText (hwndEdit, pstrBuffer, iLength + 1) ;
	 FootyGetText(FootyID, pstrBuffer, RETLINE_CRLF);

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
