/*------------------------------------------
   POPFILE.C -- Popup Editor File Functions
  ------------------------------------------*/

#include <windows.h>
#include <direct.h>
#include <commdlg.h>
#include <stdlib.h>
#include <stdio.h>
#include "poppad.h"
#include "Footy2.h"
#include "tabmanager.h"
#include "classify.h"

static OPENFILENAME ofn, ofn2, ofnImg ;
static char szMyDir[_MAX_PATH];

extern int	startflag;
extern char startdir[_MAX_PATH];

extern int activeFootyID ;
extern int activeID ;

// PathFileExistsで使用
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

#pragma warning( disable : 4996 )
#pragma warning( disable : 4006 )
void GetImagesFullPath(char *imgpath){
	GetModuleFileNameA(0, imgpath, _MAX_PATH);
	PathRemoveFileSpec(imgpath);	// ディレクトリのみにする
	strcat(imgpath, "\\images\\");
}

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

	 static char szFilterImg[] = 
	                          "Image Files (*.bmp;*.jpg;*.gif;*.png;*.tif;*.ico;*.emf;*.wmf)\0*.bmp;*.jpg;*.gif;*.png;*.tif;*.ico;*.emf;*.wmf\0"  \
                              "All Files (*.*)\0*.*\0\0" ;

     ofnImg.lStructSize       = sizeof (OPENFILENAME) ;
     ofnImg.hwndOwner         = hwnd ;
     ofnImg.hInstance         = NULL ;
     ofnImg.lpstrFilter       = szFilterImg ;
     ofnImg.lpstrCustomFilter = NULL ;
     ofnImg.nMaxCustFilter    = 0 ;
     ofnImg.nFilterIndex      = 0 ;
     ofnImg.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofnImg.nMaxFile          = _MAX_PATH ;
     ofnImg.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofnImg.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
     ofnImg.lpstrInitialDir   = NULL ;
     ofnImg.lpstrTitle        = NULL ;
     ofnImg.Flags             = 0 ;             // Set in Open and Close functions
     ofnImg.nFileOffset       = 0 ;
     ofnImg.nFileExtension    = 0 ;
     ofnImg.lpstrDefExt       = NULL ;
     ofnImg.lCustData         = 0L ;
     ofnImg.lpfnHook          = NULL ;
     ofnImg.lpTemplateName    = NULL ;
     }

BOOL PopFileOpenDlg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
     {
 
	 ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST/* | OFN_CREATEPROMPT*/;

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
     ofn2.Flags             = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST/* | OFN_CREATEPROMPT*/ ;

	 if ( startflag == STARTDIR_NONE ) {
		 ofn2.lpstrInitialDir   = NULL;
	 } else {
		 _getcwd( szMyDir, _MAX_PATH );
		 ofn2.lpstrInitialDir   = szMyDir ;
	 }

	 return GetOpenFileName (&ofn2);
     }

BOOL PopFileOpenDlgImg (HWND hwnd, PSTR pstrFileName, PSTR pstrTitleName)
     {
 
	 ofnImg.hwndOwner         = hwnd ;
     ofnImg.lpstrFile         = pstrFileName ;
     ofnImg.lpstrFileTitle    = pstrTitleName ;
     ofnImg.Flags             = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	 char newCd[_MAX_PATH + 1] = "";

     GetImagesFullPath(newCd);
	 ofnImg.lpstrInitialDir = newCd;
	 BOOL res = GetOpenFileName (&ofnImg);

	 return res;
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

//static long PopFileLength (FILE *file)
//     {
//     int iCurrentPos, iFileLength ;
//
//     iCurrentPos = ftell (file) ;
//
//     fseek (file, 0, SEEK_END) ;
//
//     iFileLength = ftell (file) ;
//
//     fseek (file, iCurrentPos, SEEK_SET) ;
//
//     return iFileLength ;
//     }

BOOL PopFileRead (int nFootyID, PSTR pstrFileName)
     {
//     FILE    *file ;
//     int      iLength ;
//     PSTR     pstrBuffer ;
//
//	 if (NULL == (file = fopen (pstrFileName, "rb")))
//		 return FALSE ;
//
//     iLength = PopFileLength (file) ;
//
//	 if (NULL == (pstrBuffer = (PSTR) malloc (iLength+4))){
//		 fclose (file) ;
//		 return FALSE ;
//	 }
//
//     fread (pstrBuffer, 1, iLength, file) ;
//     fclose (file) ;
//	 pstrBuffer[iLength] = '\0' ;
//
////     SetWindowText (hwndEdit, pstrBuffer) ;
////	 Footy2SetText(nFootyID, pstrBuffer); // 2008-02-21 Shark++ こっちだと強調文字の設定が破棄される
//     Footy2SelectAll(nFootyID, false);
//     Footy2SetSelText(nFootyID, pstrBuffer);
//	 Footy2SetCaretPosition(nFootyID, 0, 0);
//     free (pstrBuffer) ;
//
//     return TRUE ;
		int nRet;
		nRet = Footy2TextFromFile(nFootyID, pstrFileName, CSM_PLATFORM);
		if( FOOTY2ERR_NONE != nRet ) {
			return FALSE;
		}
		// 強調文字列を再設定
		SetClassify(nFootyID);
		return TRUE;
     }

BOOL PopFileWrite (int FootyID, PSTR pstrFileName)
     {
//     FILE  *file ;
//     int    iLength ;
//     PSTR   pstrBuffer ;
//
//     if (NULL == (file = fopen (pstrFileName, "wb"))){
//		return FALSE ;
//	 }
//
////     iLength = GetWindowTextLength (hwndEdit) ;
//	 iLength = Footy2GetTextLength(FootyID, LM_CRLF);
//
//     if (NULL == (pstrBuffer = (PSTR) malloc (iLength + 1)))
//          {
//          fclose (file) ;
//          return FALSE ;
//          }
//
////     GetWindowText (hwndEdit, pstrBuffer, iLength + 1) ;
//	 Footy2GetText(FootyID, pstrBuffer, LM_CRLF, iLength);
//
//     if (iLength != (int) fwrite (pstrBuffer, 1, iLength, file))
//          {
//          fclose (file) ;
//          free (pstrBuffer) ;
//          return FALSE ;
//          }
//
//     fclose (file) ;
//     free (pstrBuffer) ;
//
//     return TRUE ;
		int nRet;
		nRet = Footy2SaveToFile(FootyID, pstrFileName, CSM_AUTOMATIC, LM_AUTOMATIC);
		if( FOOTY2ERR_NONE != nRet ) {
			return FALSE;
		}
		return TRUE;
     }
