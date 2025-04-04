/*----------------------------------------------
   POPPRNT.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include "poppad.h"
#include "resource.h"
#include "Footy2.h"

extern int activeFootyID;

BOOL bUserAbort ;
HWND hDlgPrint ;

BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT msg, WPARAM /*wParam*/, LPARAM /*lParam*/)
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

BOOL CALLBACK AbortProc (HDC /*hPrinterDC*/, int /*iCode*/)
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

BOOL PopPrntPrintFile (HINSTANCE hInst, HWND hwnd, HWND /*hwndEdit*/, 
                                                   LPSTR szTitleName)
     {
     static DOCINFO  di = { sizeof (DOCINFO), "", NULL } ;
     static PRINTDLG pd ;
     BOOL            bSuccess ;
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

//     iTotalLines = (short) SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0L) ;
	 iTotalLines = (short)Footy2GetLines(activeFootyID);
     
     if (iTotalLines == 0)
          return TRUE ;

	 TCHAR szLineNumber[64];
	 int nLineNumberCharWidth = 1;
	 _stprintf(szLineNumber, "%d:", iTotalLines);
	 nLineNumberCharWidth = lstrlen(szLineNumber) - 1;

//	 DEVMODE dm;
//	 GetDeviceCaps(

	 HFONT hFont = CreateFont(-MulDiv(10, GetDeviceCaps(pd.hDC, LOGPIXELSY), 72), 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");
	 HFONT hFoltOld = (HFONT)SelectObject(pd.hDC, hFont);

	 RECT rc = {0};
	 DrawText(pd.hDC, szLineNumber, -1, &rc, DT_CALCRECT);
	 int nLineNumberWidth = rc.right - rc.left;

	 DRAWTEXTPARAMS dtp = { sizeof(DRAWTEXTPARAMS), 4, };

     GetTextMetrics (pd.hDC, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

	 SIZE sizePage;
	 sizePage.cx = GetDeviceCaps (pd.hDC, HORZRES);
	 sizePage.cy = GetDeviceCaps (pd.hDC, VERTRES);
     iCharsPerLine = sizePage.cx / tm.tmAveCharWidth ;
     iLinesPerPage = sizePage.cy / yChar ;
     iTotalPages   = (iTotalLines + iLinesPerPage - 1) / iLinesPerPage ;

     EnableWindow (hwnd, FALSE) ;

     bSuccess   = TRUE ;
     bUserAbort = FALSE ;

     hDlgPrint = CreateDialog (hInst, (LPCTSTR) "PrintDlgBox", hwnd, (DLGPROC)PrintDlgProc) ;
     SetDlgItemText (hDlgPrint, IDD_FNAME, szTitleName) ;

     SetAbortProc (pd.hDC, (ABORTPROC)AbortProc) ;

	 di.lpszDocName = szTitleName;

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

                              if (iLineNum >= iTotalLines)
                                   break ;

//                              TextOut (pd.hDC, 0, yChar * iLine, pstrBuffer,
//                                   (int) SendMessage (hwndEdit, EM_GETLINE,
//                                   (WPARAM) iLineNum, (LPARAM) pstrBuffer)) ;
					//		  pstrBuffer = FootyGetLineData(activeFootyID, iLineNum+1);	// 2008-02-17 Shark++ 後回し
					//		  TextOut(pd.hDC, 0, yChar * iLine, pstrBuffer, 
					//			  FootyGetLineLen(activeFootyID, iLineNum+1));
							  // 行番号
							  _stprintf(szLineNumber, "%*d ", nLineNumberCharWidth, iLineNum + 1);
							  TextOut(pd.hDC, 0, yChar * iLine, szLineNumber, lstrlen(szLineNumber));
							  // 行内容
							  LPCWSTR pstrBufferW = Footy2GetLineW(activeFootyID, iLineNum);	// 2008-02-28 Shark++ 全角文字が腐る・折り返しが(元から)未実装
							  SetRect(&rc, nLineNumberWidth, yChar * iLine, nLineNumberWidth + sizePage.cx, yChar * iLine + yChar);
							  DrawTextExW(pd.hDC, (LPWSTR)pstrBufferW, 
								  Footy2GetLineLengthW(activeFootyID, iLineNum)
								  , &rc, DT_SINGLELINE|DT_EXPANDTABS|DT_NOPREFIX|DT_TABSTOP, &dtp);
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

	 DeleteObject( SelectObject(pd.hDC, hFoltOld) );

	 DeleteDC (pd.hDC) ;

     return bSuccess && !bUserAbort ;
     }
