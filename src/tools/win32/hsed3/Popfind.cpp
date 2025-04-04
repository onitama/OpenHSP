/*--------------------------------------------------------
   POPFIND.C -- Popup Editor Search and Replace Functions
  --------------------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <mbstring.h>

/*
		XP support routines
*/
extern int  flag_xpstyle;
int getUnicodeOffset2( char *text, int offset );

#define MAX_STRING_LEN   512

static char szFindText [MAX_STRING_LEN] ;
static char szReplText [MAX_STRING_LEN] ;
static int repl_flag;


//-----------------------------------------------------------

/*
char *strfind( char *srcstr, char *findstr )
{
	char a1,a2,c;
	char *p1;

	a1=findstr[0];
	if (a1==0) return NULL;
	a2=findstr[1];
	if (a2!=0) {
		return strstr ( srcstr, findstr );
	}
	p1=srcstr;
	while(1) {
		c=*p1;
		if (c==0) return NULL;
		if ((c>=129)&&(c<=159)) { c=0;p1++; }
		if ((c>=224)&&(c<=252)) { c=0;p1++; }
		if (c==a1) break;
		p1++;
	}
	return p1;
}
*/


//-----------------------------------------------------------

HWND PopFindFindDlg (HWND hwnd)
     {
     static FINDREPLACE fr ;       // must be static for modeless dialog!!!

     fr.lStructSize      = sizeof (FINDREPLACE) ;
     fr.hwndOwner        = hwnd ;
     fr.hInstance        = NULL ;
     fr.Flags            = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD ;
     fr.lpstrFindWhat    = szFindText ;
     fr.lpstrReplaceWith = NULL ;
     fr.wFindWhatLen     = sizeof (szFindText) ;
     fr.wReplaceWithLen  = 0 ;
     fr.lCustData        = 0 ;
     fr.lpfnHook         = NULL ;
     fr.lpTemplateName   = NULL ;

     return FindText (&fr) ;
     }

HWND PopFindReplaceDlg (HWND hwnd)
     {
     static FINDREPLACE fr ;       // must be static for modeless dialog!!!

     fr.lStructSize      = sizeof (FINDREPLACE) ;
     fr.hwndOwner        = hwnd ;
     fr.hInstance        = NULL ;
     fr.Flags            = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD ;
     fr.lpstrFindWhat    = szFindText ;
     fr.lpstrReplaceWith = szReplText ;
     fr.wFindWhatLen     = sizeof (szFindText) ;
     fr.wReplaceWithLen  = sizeof (szReplText) ;
     fr.lCustData        = 0 ;
     fr.lpfnHook         = NULL ;
     fr.lpTemplateName   = NULL ;

	 repl_flag = 0;

     return ReplaceText (&fr) ;
     }

BOOL PopFindFindText (HWND hwndEdit, int *piSearchOffset, LPFINDREPLACE pfr)
     {
     int   iLength, iPos ;
     PSTR  pstrDoc, pstrPos ;

               // Read in the edit document

	iLength = GetWindowTextLength (hwndEdit) ;

     if (NULL == (pstrDoc = (PSTR) malloc (iLength + 1)))
          return FALSE ;

     GetWindowText (hwndEdit, pstrDoc, iLength + 1) ;

               // Search the document for the find string

     pstrPos = (char *)_mbsstr( (BYTE *)(pstrDoc + *piSearchOffset), (BYTE *)pfr->lpstrFindWhat );

               // Return an error code if the string cannot be found

	 if (pstrPos == NULL) {
	     free (pstrDoc) ;
          return FALSE ;
	 }

               // Find the position in the document and the new start offset

     iPos = pstrPos - pstrDoc ;
     *piSearchOffset = iPos + strlen (pfr->lpstrFindWhat) ;

               // Select the found text

	 if ( flag_xpstyle ) {
		 SendMessage (hwndEdit, EM_SETSEL, getUnicodeOffset2( pstrDoc, iPos), getUnicodeOffset2( pstrDoc, *piSearchOffset ) );
	 } else {
	     SendMessage (hwndEdit, EM_SETSEL, iPos, *piSearchOffset );
	 }
     SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;

	 repl_flag = 1;
     free (pstrDoc) ;

	 return TRUE ;
     }

BOOL PopFindNextText (HWND hwndEdit, int *piSearchOffset)
     {
     FINDREPLACE fr ;

     fr.lpstrFindWhat = szFindText ;

     return PopFindFindText (hwndEdit, piSearchOffset, &fr) ;
     }

BOOL PopFindReplaceText (HWND hwndEdit, int *piSearchOffset, LPFINDREPLACE pfr)
     {

	 int slen;
	 int slen2;
	 int ofs;

     // Find the text

	 if ( repl_flag == 0 ) {
		if (!PopFindFindText (hwndEdit, piSearchOffset, pfr)) return FALSE ;
	 }

     // Replace it

    SendMessage (hwndEdit, EM_REPLACESEL, 0, (LPARAM) pfr->lpstrReplaceWith) ;

    slen = strlen( pfr->lpstrReplaceWith );
    slen2 = strlen (pfr->lpstrFindWhat) ;

	*piSearchOffset += slen - slen2;

	ofs = *piSearchOffset;
//	if ( flag_xpstyle ) { ofs = getUnicodeOffset( pstrDoc, ofs ); }

	SendMessage (hwndEdit, EM_SETSEL, ofs, ofs );
	
	//SendMessage (hwndEdit, EM_SETSEL, *piSearchOffset, *piSearchOffset) ;
	//SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;

     //SendMessage (hwndEdit, EM_SETSEL, *piSearchOffset-1, *piSearchOffset+slen-1) ;
     //SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;
	 //*piSearchOffset+=slen-1;

     if (!PopFindFindText (hwndEdit, piSearchOffset, pfr))
          return FALSE ;
     //PopFindFindText (hwndEdit, piSearchOffset, pfr);
     return TRUE ;
     }

BOOL PopFindValidFind (void)
     {
     return *szFindText != '\0' ;
     }



BOOL PopFindReplaceAll (HWND hwndEdit, int *piSearchOffset, LPFINDREPLACE pfr)
{
	 BOOL rval;
	 int ofs;
	 ofs=*piSearchOffset;
     //if (!PopFindFindText (hwndEdit, &ofs, pfr)) return FALSE ;
     //SendMessage (hwndEdit, EM_GETSEL, 0,(LPARAM) &ofs ) ;
	 rval=PopFindReplaceText ( hwndEdit, &ofs, pfr );
	 return rval;
}

