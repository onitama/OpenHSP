/*----------------------------------------------
   POPPRNT.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include "poppad.h"

BOOL bUserAbort ;
HWND hDlgPrint ;

BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
     {
     switch (msg)
          {
          case WM_INITDIALOG :
               EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE,
                                                            MF_GRAYED) ;
               return TRUE ;

          case WM_COMMAND :
               bUserAbort = TRUE ;
               EnableWindow (GetParent (hDlg), TRUE) ;
               DestroyWindow (hDlg) ;
               hDlgPrint = 0 ;
               return TRUE ;
          }
     return FALSE ;
     }          

BOOL CALLBACK AbortProc (HDC hPrinterDC, int iCode)
     {
     MSG msg ;

     while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
          if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          }
     return !bUserAbort ;
     }

BOOL PopPrntPrintFile (HINSTANCE hInst, HWND hwnd, HWND hwndEdit, 
                                                   LPSTR szTitleName)
     {
     static DOCINFO  di = { sizeof (DOCINFO), "", NULL } ;
     static PRINTDLG pd ;
     BOOL            bSuccess ;
     LPCTSTR         pstrBuffer ;
     int             yChar, iCharsPerLine, iLinesPerPage, iTotalLines,
                     iTotalPages, iPage, iLine, iLineNum ;
     TEXTMETRIC      tm ;
     WORD            iColCopy, iNoiColCopy ;

     pd.lStructSize         = sizeof (PRINTDLG) ;
     pd.hwndOwner           = hwnd ;
     pd.hDevMode            = NULL ;
     pd.hDevNames           = NULL ;
     pd.hDC                 = NULL ;
     pd.Flags               = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC ;
     pd.nFromPage           = 0 ;
     pd.nToPage             = 0 ;
     pd.nMinPage            = 0 ;
     pd.nMaxPage            = 0 ;
     pd.nCopies             = 1 ;
     pd.hInstance           = NULL ;
     pd.lCustData           = 0L ;
     pd.lpfnPrintHook       = NULL ;
     pd.lpfnSetupHook       = NULL ;
     pd.lpPrintTemplateName = NULL ;
     pd.lpSetupTemplateName = NULL ;
     pd.hPrintTemplate      = NULL ;
     pd.hSetupTemplate      = NULL ;

     if (!PrintDlg (&pd))
          return TRUE ;

     iTotalLines = (short) SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0L) ;

     if (iTotalLines == 0)
          return TRUE ;

     GetTextMetrics (pd.hDC, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

     iCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth ;
     iLinesPerPage = GetDeviceCaps (pd.hDC, VERTRES) / yChar ;
     iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage ;

     pstrBuffer = (LPCTSTR) HeapAlloc (GetProcessHeap (), 
		                               HEAP_NO_SERIALIZE, iCharsPerLine + 1) ;

     EnableWindow (hwnd, FALSE) ;

     bSuccess   = TRUE ;
     bUserAbort = FALSE ;

     hDlgPrint = CreateDialog (hInst, (LPCTSTR) "PrintDlgBox", hwnd, (DLGPROC)PrintDlgProc) ;
     SetDlgItemText (hDlgPrint, IDD_FNAME, szTitleName) ;

     SetAbortProc (pd.hDC, (ABORTPROC)AbortProc) ;

     GetWindowText (hwnd, (PTSTR) di.lpszDocName, sizeof (PTSTR)) ;

     if (StartDoc (pd.hDC, &di) > 0)
          {
          for (iColCopy = 0 ;
               iColCopy < ((WORD) pd.Flags & PD_COLLATE ? pd.nCopies : 1) ;
               iColCopy++)
               {
               for (iPage = 0 ; iPage < iTotalPages ; iPage++)
                    {
                    for (iNoiColCopy = 0 ;
                         iNoiColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies) ;
                         iNoiColCopy++)
                         {

                         if (StartPage (pd.hDC) < 0)
                              {
                              bSuccess = FALSE ;
                              break ;
                              }

                         for (iLine = 0 ; iLine < iLinesPerPage ; iLine++)
                              {
                              iLineNum = iLinesPerPage * iPage + iLine ;

                              if (iLineNum > iTotalLines)
                                   break ;

                              *(int *) pstrBuffer = iCharsPerLine ;

                              TextOut (pd.hDC, 0, yChar * iLine, pstrBuffer,
                                   (int) SendMessage (hwndEdit, EM_GETLINE,
                                   (WPARAM) iLineNum, (LPARAM) pstrBuffer)) ;
                              }

                         if (EndPage (pd.hDC) < 0)
                              {
                              bSuccess = FALSE ;
                              break ;
                              }

                         if (bUserAbort)
                              break ;
                         }

                    if (!bSuccess || bUserAbort)
                         break ;
                    }

               if (!bSuccess || bUserAbort)
                    break ;
               }
          }
     else
          bSuccess = FALSE ;

     if (bSuccess)
          EndDoc (pd.hDC) ;

     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;
          DestroyWindow (hDlgPrint) ;
          }

     HeapFree (GetProcessHeap (), 0, (LPVOID) pstrBuffer) ;
     DeleteDC (pd.hDC) ;

     return bSuccess && !bUserAbort ;
     }
