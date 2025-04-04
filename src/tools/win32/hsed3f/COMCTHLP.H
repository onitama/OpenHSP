/*---------------------------------------------------
   COMCTHLP.H -- Helper macros for common controls
                 (c) Paul Yao, 1996

   Portions Copyright (c) 1992-1996, Microsoft Corp.
  ---------------------------------------------------*/

//-------------------------------------------------------------------
// Hot-Key Helper Macros
//-------------------------------------------------------------------
#define HotKey_SetHotKey(hwnd, bVKHotKey, bfMods) \
    (void)SendMessage((hwnd), HKM_SETHOTKEY, MAKEWORD(bVKHotKey, bfMods), 0L)

#define HotKey_GetHotKey(hwnd) \
    (WORD)SendMessage((hwnd), HKM_GETHOTKEY, 0, 0L)

#define HotKey_SetRules(hwnd, fwCombInv, fwModInv) \
    (void)SendMessage((hwnd), HKM_SETRULES, (WPARAM) fwCombInv, MAKELPARAM(fwModInv, 0))

//-------------------------------------------------------------------
// Progress Bar Helper Macros
//-------------------------------------------------------------------
#define Progress_SetRange(hwnd, nMinRange, nMaxRange) \
    (DWORD)SendMessage((hwnd), PBM_SETRANGE, 0, MAKELPARAM(nMinRange, nMaxRange))

#define Progress_SetPos(hwnd, nNewPos) \
    (int)SendMessage((hwnd), PBM_SETPOS, (WPARAM) nNewPos, 0L)

#define Progress_DeltaPos(hwnd, nIncrement) \
    (int)SendMessage((hwnd), PBM_DELTAPOS, (WPARAM) nIncrement, 0L)

#define Progress_SetStep(hwnd, nStepInc) \
    (int)SendMessage((hwnd), PBM_SETSTEP, (WPARAM) nStepInc, 0L)

#define Progress_StepIt(hwnd) \
    (int)SendMessage((hwnd), PBM_STEPIT, 0, 0L)

//-------------------------------------------------------------------
// Rich Edit Control Helper Macros
//-------------------------------------------------------------------

//---------------- Begin Macros Copied from windowsx.h--------------- 
#define RichEdit_Enable(hwndCtl, fEnable) \
    (BOOL)EnableWindow((hwndCtl), (fEnable))

#define RichEdit_GetText(hwndCtl, lpch, cchMax) \
    (int)GetWindowText((hwndCtl), (lpch), (cchMax))

#define RichEdit_GetTextLength(hwndCtl) \
    (int)GetWindowTextLength(hwndCtl)

#define RichEdit_SetText(hwndCtl, lpsz)  \
    (BOOL)SetWindowText((hwndCtl), (lpsz))

#define RichEdit_LimitText(hwndCtl, cchMax) \
    ((void)SendMessage((hwndCtl), EM_LIMITTEXT, (WPARAM)(cchMax), 0L))

#define RichEdit_GetLineCount(hwndCtl) \
    ((int)(DWORD)SendMessage((hwndCtl), EM_GETLINECOUNT, 0L, 0L))

#define RichEdit_GetLine(hwndCtl, line, lpch, cchMax) \
    ((*((int *)(lpch)) = (cchMax)), ((int)(DWORD)SendMessage((hwndCtl), EM_GETLINE, (WPARAM)(int)(line), (LPARAM)(LPTSTR)(lpch))))

#define RichEdit_GetRect(hwndCtl, lprc) \
    ((void)SendMessage((hwndCtl), EM_GETRECT, 0L, (LPARAM)(RECT *)(lprc)))

#define RichEdit_SetRect(hwndCtl, lprc) \
    ((void)SendMessage((hwndCtl), EM_SETRECT, 0L, (LPARAM)(const RECT *)(lprc)))

#define RichEdit_GetSel(hwndCtl) \
    ((DWORD)SendMessage((hwndCtl), EM_GETSEL, 0L, 0L))

#define RichEdit_SetSel(hwndCtl, ichStart, ichEnd) \
    ((void)SendMessage((hwndCtl), EM_SETSEL, (ichStart), (ichEnd)))

#define RichEdit_ReplaceSel(hwndCtl, lpszReplace) \
    ((void)SendMessage((hwndCtl), EM_REPLACESEL, 0L, (LPARAM)(LPCTSTR)(lpszReplace)))

#define RichEdit_GetModify(hwndCtl) \
    ((BOOL)(DWORD)SendMessage((hwndCtl), EM_GETMODIFY, 0L, 0L))

#define RichEdit_SetModify(hwndCtl, fModified) \
    ((void)SendMessage((hwndCtl), EM_SETMODIFY, (WPARAM)(UINT)(fModified), 0L))

#define RichEdit_ScrollCaret(hwndCtl) \
    ((BOOL)(DWORD)SendMessage((hwndCtl), EM_SCROLLCARET, 0, 0L))

#define RichEdit_LineFromChar(hwndCtl, ich) \
    ((int)(DWORD)SendMessage((hwndCtl), EM_LINEFROMCHAR, (WPARAM)(int)(ich), 0L))

#define RichEdit_LineIndex(hwndCtl, line) \
    ((int)(DWORD)SendMessage((hwndCtl), EM_LINEINDEX, (WPARAM)(int)(line), 0L))

#define RichEdit_LineLength(hwndCtl, line) \
    ((int)(DWORD)SendMessage((hwndCtl), EM_LINELENGTH, (WPARAM)(int)(line), 0L))

#define RichEdit_Scroll(hwndCtl, dv, dh) \
    ((void)SendMessage((hwndCtl), EM_LINESCROLL, (WPARAM)(dh), (LPARAM)(dv)))

#define RichEdit_CanUndo(hwndCtl)  \
    ((BOOL)(DWORD)SendMessage((hwndCtl), EM_CANUNDO, 0L, 0L))

#define RichEdit_Undo(hwndCtl)  \
    ((BOOL)(DWORD)SendMessage((hwndCtl), EM_UNDO, 0L, 0L))

#define RichEdit_EmptyUndoBuffer(hwndCtl) \
    ((void)SendMessage((hwndCtl), EM_EMPTYUNDOBUFFER, 0L, 0L))

#define RichEdit_GetFirstVisibleLine(hwndCtl) \
    ((int)(DWORD)SendMessage((hwndCtl), EM_GETFIRSTVISIBLELINE, 0L, 0L))

#define RichEdit_SetReadOnly(hwndCtl, fReadOnly) \
    ((BOOL)(DWORD)SendMessage((hwndCtl), EM_SETREADONLY, (WPARAM)(BOOL)(fReadOnly), 0L))

#define RichEdit_SetWordBreakProc(hwndCtl, lpfnWordBreak) \
    ((void)SendMessage((hwndCtl), EM_SETWORDBREAKPROC, 0L, (LPARAM)(EDITWORDBREAKPROC)(lpfnWordBreak)))

#define RichEdit_GetWordBreakProc(hwndCtl) \
    ((EDITWORDBREAKPROC)SendMessage((hwndCtl), EM_GETWORDBREAKPROC, 0L, 0L))

#define RichEdit_CanPaste(hwnd, uFormat) \
    (BOOL)SendMessage((hwnd), EM_CANPASTE, (WPARAM) (UINT) uFormat, 0L) 

#define RichEdit_CharFromPos(hwnd, x, y) \
    (DWORD)SendMessage((hwnd), EM_CHARFROMPOS, 0, MAKELPARAM(x, y))

#define RichEdit_DisplayBand(hwnd, lprc) \
    (BOOL)SendMessage((hwnd), EM_DISPLAYBAND, 0, (LPARAM) (LPRECT) lprc)

#define RichEdit_ExGetSel(hwnd, lpchr) \
    (void)SendMessage((hwnd), EM_EXGETSEL, 0, (LPARAM) (CHARRANGE FAR *) lpchr)

#define RichEdit_ExLimitText(hwnd, cchTextMax) \
    (void)SendMessage((hwnd), EM_EXLIMITTEXT, 0, (LPARAM) (DWORD) cchTextMax)

#define RichEdit_ExLineFromChar(hwnd, ichCharPos) \
    (int)SendMessage((hwnd), EM_EXLINEFROMCHAR, 0, (LPARAM) (DWORD) ichCharPos)

#define RichEdit_ExSetSel(hwnd, ichCharRange) \
    (int)SendMessage((hwnd), EM_EXSETSEL, 0, (LPARAM) (CHARRANGE FAR *) ichCharRange)

#define RichEdit_FindText(hwnd, fuFlags, lpFindText) \
    (int)SendMessage((hwnd), EM_FINDTEXT, (WPARAM) (UINT) fuFlags, (LPARAM) (FINDTEXT FAR *) lpFindText)

#define RichEdit_FindTextEx(hwnd, fuFlags, lpFindText) \
    (int)SendMessage((hwnd), EM_FINDTEXTEX, (WPARAM) (UINT) fuFlags, (LPARAM) (FINDTEXT FAR *) lpFindText)

#define RichEdit_FindWordBreak(hwnd, code, ichStart) \
    (int)SendMessage((hwnd), EM_FINDWORDBREAK, (WPARAM) (UINT) code, (LPARAM) (DWORD) ichStart)

#define RichEdit_FormatRange(hwnd, fRender, lpFmt) \
    (int)SendMessage((hwnd), EM_FORMATRANGE, (WPARAM) (BOOL) fRender, (LPARAM) (FORMATRANGE FAR *) lpFmt)

#define RichEdit_GetCharFormat(hwnd, fSelection, lpFmt) \
    (DWORD)SendMessage((hwnd), EM_GETCHARFORMAT, (WPARAM) (BOOL) fSelection, (LPARAM) (CHARFORMAT FAR *) lpFmt)

#define RichEdit_GetEventMask(hwnd) \
    (DWORD)SendMessage((hwnd), EM_GETEVENTMASK, 0, 0L)

#define RichEdit_GetLimitText(hwnd) \
    (int)SendMessage((hwnd), EM_GETLIMITTEXT, 0, 0L)

#define RichEdit_GetOleInterface(hwnd, ppObject) \
    (BOOL)SendMessage((hwnd), EM_GETOLEINTERFACE, 0, (LPARAM) (LPVOID FAR *) ppObject)

#define RichEdit_GetOptions(hwnd) \
    (UINT)SendMessage((hwnd), EM_GETOPTIONS, 0, 0L)

#define RichEdit_GetParaFormat(hwnd, lpFmt) \
    (DWORD)SendMessage((hwnd), EM_GETPARAFORMAT, 0, (LPARAM) (PARAFORMAT FAR *) lpFmt)

#define RichEdit_GetSelText(hwnd, lpBuf) \
    (int)SendMessage((hwnd), EM_GETSELTEXT, 0, (LPARAM) (LPSTR) lpBuf)

#define RichEdit_GetTextRange(hwnd, lpRange) \
    (int)SendMessage((hwnd), EM_GETTEXTRANGE, 0, (LPARAM) (TEXTRANGE FAR *) lpRange)

#define RichEdit_GetWordBreakProcEx(hwnd) \
    (EDITWORDBREAKPROCEX *)SendMessage((hwnd), EM_GETWORDBREAKPROCEX, 0, 0L)
//----------------- End Macros Copied from windowsx.h----------------

#define RichEdit_HideSelection(hwnd, fHide, fChangeStyle) \
    (void)SendMessage((hwnd), EM_HIDESELECTION, (WPARAM) (BOOL) fHide, (LPARAM) (BOOL) fChangeStyle)

#define RichEdit_PasteSpecial(hwnd, uFormat) \
    (void)SendMessage((hwnd), EM_PASTESPECIAL, (WPARAM) (UINT) uFormat, 0L)

#define RichEdit_PosFromChar(hwnd, wCharIndex) \
    (DWORD)SendMessage((hwnd), EM_POSFROMCHAR, (WPARAM)wCharIndex, 0L)

#define RichEdit_RequestResize(hwnd) \
    (void)SendMessage((hwnd), EM_REQUESTRESIZE, 0, 0L)

#define RichEdit_SelectionType(hwnd) \
    (int)SendMessage((hwnd), EM_SELECTIONTYPE, 0, 0L)

#define RichEdit_SetBkgndColor(hwnd, fUseSysColor, clr) \
    (COLORREF)SendMessage((hwnd), EM_SETBKGNDCOLOR, (WPARAM) (BOOL) fUseSysColor, (LPARAM) (COLORREF) clr)

#define RichEdit_SetCharFormat(hwnd, uFlags, lpFmt) \
    (BOOL)SendMessage((hwnd), EM_SETCHARFORMAT, (WPARAM) (UINT) uFlags, (LPARAM) (CHARFORMAT FAR *) lpFmt)

#define RichEdit_SetEventMask(hwnd, dwMask) \
    (DWORD)SendMessage((hwnd), EM_SETEVENTMASK, 0, (LPARAM) (DWORD) dwMask)

#define RichEdit_SetOleCallback(hwnd, lpObj) \
    (BOOL)SendMessage((hwnd), EM_SETOLECALLBACK, 0, (LPARAM) (IRichEditOleCallback FAR *) lpObj)

#define RichEdit_SetOptions(hwnd, fOperation, fOptions) \
    (UINT)SendMessage((hwnd), EM_SETOPTIONS, (WPARAM) (UINT) fOperation, (LPARAM) (UINT) fOptions)

#define RichEdit_SetParaFormat(hwnd, lpFmt) \
    (BOOL)SendMessage((hwnd), EM_SETPARAFORMAT, 0, (LPARAM) (PARAFORMAT FAR *) lpFmt)

#define RichEdit_SetTargetDevice(hwnd, hdcTarget, cxLineWidth) \
    (BOOL)SendMessage((hwnd), EM_SETTARGETDEVICE, (WPARAM) (HDC) hdcTarget, (LPARAM) (int) cxLineWidth)

#define RichEdit_SetWordBreakProcEx(hwnd, pfnWordBreakProcEx) \
    (EDITWORDBREAKPROCEX *)SendMessage((hwnd), EM_SETWORDBREAKPROCEX, 0, (LPARAM) (EDITWORDBREAKPROCEX *)pfnWordBreakProcEx)

#define RichEdit_StreamIn(hwnd, uFormat, lpStream) \
    (int)SendMessage((hwnd), EM_STREAMIN, (WPARAM) (UINT) uFormat, (LPARAM) (EDITSTREAM FAR *) lpStream)

#define RichEdit_StreamOut(hwnd, uFormat, lpStream) \
    (int)SendMessage((hwnd), EM_STREAMOUT, (WPARAM) (UINT) uFormat, (LPARAM) (EDITSTREAM FAR *) lpStream)

//-------------------------------------------------------------------
// Status Bar Helper Macros
//-------------------------------------------------------------------
#define Status_GetBorders(hwnd, aBorders) \
    (BOOL)SendMessage((hwnd), SB_GETBORDERS, 0, (LPARAM) (LPINT) aBorders)

#define Status_GetParts(hwnd, nParts, aRightCoord) \
    (int)SendMessage((hwnd), SB_GETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aRightCoord)

#define Status_GetRect(hwnd, iPart, lprc) \
    (BOOL)SendMessage((hwnd), SB_GETRECT, (WPARAM) iPart, (LPARAM) (LPRECT) lprc)

#define Status_GetText(hwnd, iPart, szText) \
    (DWORD)SendMessage((hwnd), SB_GETTEXT, (WPARAM) iPart, (LPARAM) (LPSTR) szText)

#define Status_GetTextLength(hwnd, iPart) \
    (DWORD)SendMessage((hwnd), SB_GETTEXTLENGTH, (WPARAM) iPart, 0L)

#define Status_SetMinHeight(hwnd, minHeight) \
    (void)SendMessage((hwnd), SB_SETMINHEIGHT, (WPARAM) minHeight, 0L)

#define Status_SetParts(hwnd, nParts, aWidths) \
    (BOOL)SendMessage((hwnd), SB_SETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aWidths)

#define Status_SetText(hwnd, iPart, uType, szText) \
    (BOOL)SendMessage((hwnd), SB_SETTEXT, (WPARAM) (iPart | uType), (LPARAM) (LPSTR) szText)

#define Status_Simple(hwnd, fSimple) \
    (BOOL)SendMessage((hwnd), SB_SIMPLE, (WPARAM) (BOOL) fSimple, 0L)

//-------------------------------------------------------------------
// Tool Bar Helper Macros
//-------------------------------------------------------------------

#define ToolBar_AddBitmap(hwnd, nButtons, lptbab) \
    (int)SendMessage((hwnd), TB_ADDBITMAP, (WPARAM)nButtons, (LPARAM)(LPTBADDBITMAP) lptbab)

#define ToolBar_AddButtons(hwnd, uNumButtons, lpButtons) \
    (BOOL)SendMessage((hwnd), TB_ADDBUTTONS, (WPARAM)(UINT)uNumButtons, (LPARAM)(LPTBBUTTON)lpButtons)

#define ToolBar_AddString(hwnd, hinst, idString) \
    (int)SendMessage((hwnd), TB_ADDSTRING, (WPARAM)(HINSTANCE)hinst, (LPARAM)idString)

#define ToolBar_AutoSize(hwnd) \
    (void)SendMessage((hwnd), TB_AUTOSIZE, 0, 0L)

#define ToolBar_ButtonCount(hwnd) \
    (int)SendMessage((hwnd), TB_BUTTONCOUNT, 0, 0L)

#define ToolBar_ButtonStructSize(hwnd) \
    (void)SendMessage((hwnd), TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0L)

#define ToolBar_ChangeBitmap(hwnd, idButton, iBitmap) \
    (BOOL)SendMessage((hwnd), TB_CHANGEBITMAP, (WPARAM) idButton, (LPARAM)iBitmap);

#define ToolBar_CheckButton(hwnd, idButton, fCheck ) \
    (BOOL)SendMessage((hwnd), TB_CHECKBUTTON, (WPARAM) idButton, (LPARAM) MAKELONG(fCheck, 0))

#define ToolBar_CommandToIndex(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_COMMANDTOINDEX, (WPARAM) idButton, 0L)

#define ToolBar_Customize(hwnd) \
    (void)SendMessage((hwnd), TB_CUSTOMIZE, 0, 0L)

#define ToolBar_DeleteButton(hwnd, idButton) \
    (BOOL)SendMessage((hwnd), TB_DELETEBUTTON, (WPARAM) idButton, 0L)

#define ToolBar_EnableButton(hwnd, idButton, fEnable ) \
    (BOOL)SendMessage((hwnd), TB_ENABLEBUTTON, (WPARAM) idButton, (LPARAM) MAKELONG(fEnable, 0))

#define ToolBar_GetBitmap(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_GETBITMAP, (WPARAM) idButton, 0L)

#define ToolBar_GetBitmapFlags(hwnd) \
    (int)SendMessage((hwnd), TB_GETBITMAPFLAGS, 0, 0L)

#define ToolBar_GetButton(hwnd, idButton, lpButton) \
    (BOOL)SendMessage((hwnd), TB_GETBUTTON, (WPARAM)idButton, (LPARAM)(LPTBBUTTON) lpButton)

#define ToolBar_GetButtonText(hwnd, idButton, lpszText) \
    (int)SendMessage((hwnd), TB_GETBUTTONTEXT, (WPARAM) idButton, (LPARAM)(LPSTR)lpszText)

#define ToolBar_GetItemRect(hwnd, idButton, lprc) \
    (BOOL)SendMessage((hwnd), TB_GETITEMRECT, (WPARAM)idButton, (LPARAM)(LPRECT)lprc)

#define ToolBar_GetRows(hwnd) \
    (int)SendMessage((hwnd), TB_GETROWS, 0, 0L)

#define ToolBar_GetState(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_GETSTATE, (WPARAM) idButton, 0L)

#define ToolBar_GetToolTips(hwnd) \
    (HWND)SendMessage((hwnd), TB_GETTOOLTIPS, 0, 0L)

#define ToolBar_HideButton(hwnd, idButton, fShow) \
    (BOOL)SendMessage((hwnd), TB_HIDEBUTTON, (WPARAM)idButton, (LPARAM)MAKELONG(fShow, 0))

#define ToolBar_Indeterminate(hwnd, idButton, fIndeterminate) \
    (BOOL)SendMessage((hwnd), TB_INDETERMINATE, (WPARAM)idButton, (LPARAM) MAKELONG(fIndeterminate, 0))

#define ToolBar_InsertButton(hwnd, idButton, lpButton) \
    (BOOL)SendMessage((hwnd), TB_INSERTBUTTON, (WPARAM)idButton, (LPARAM)(LPTBBUTTON)lpButton)

#define ToolBar_IsButtonChecked(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_ISBUTTONCHECKED, (WPARAM)idButton, 0L)

#define ToolBar_IsButtonEnabled(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_ISBUTTONENABLED, (WPARAM) idButton, 0L)

#define ToolBar_IsButtonHidden(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_ISBUTTONHIDDEN, (WPARAM) idButton, 0L)

#define ToolBar_IsButtonIndeterminate(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_ISBUTTONINDETERMINATE, (WPARAM) idButton, 0L)

#define ToolBar_IsButtonPressed(hwnd, idButton) \
    (int)SendMessage((hwnd), TB_ISBUTTONPRESSED, (WPARAM) idButton, 0L

#define ToolBar_PressButton(hwnd, idButton, fPress) \
    (BOOL)SendMessage((hwnd), TB_PRESSBUTTON, (WPARAM)idButton, (LPARAM)MAKELONG(fPress, 0))

#define ToolBar_SaveRestore(hwnd, fSave, ptbsp) \
    (void)SendMessage((hwnd), TB_SAVERESTORE, (WPARAM)(BOOL)fSave, (LPARAM)(TBSAVEPARAMS *)ptbsp)

#define ToolBar_SetBitmapSize(hwnd, dxBitmap, dyBitmap) \
    (BOOL)SendMessage((hwnd), TB_SETBITMAPSIZE, 0, (LPARAM)MAKELONG(dxBitmap, dyBitmap))

#define ToolBar_SetButtonSize(hwnd, dxBitmap, dyBitmap) \
    (BOOL)SendMessage((hwnd), TB_SETBUTTONSIZE, 0, (LPARAM)MAKELONG(dxBitmap, dyBitmap))

#define ToolBar_SetCmdID(hwnd, index, cmdId) \
    (BOOL)SendMessage((hwnd), TB_SETCMDID, (WPARAM)(UINT)index, (WPARAM)(UINT)cmdId)

#define ToolBar_SetParent(hwnd, hwndParent) \
    (void)SendMessage((hwnd), TB_SETPARENT, (WPARAM) (HWND) hwndParent, 0L)

#define ToolBar_SetRows(hwnd, cRows, fLarger, lprc) \
    (void)SendMessage((hwnd), TB_SETROWS, (WPARAM)MAKEWPARAM(cRows, fLarger),(LPARAM)(LPRECT)lprc)

#define ToolBar_SetState(hwnd, idButton, fState) \
    (BOOL)SendMessage((hwnd), TB_SETSTATE, (WPARAM)idButton, (LPARAM)MAKELONG(fState, 0))

#define ToolBar_SetToolTips(hwnd) \
    (void)SendMessage((hwnd), TB_SETTOOLTIPS, (WPARAM)(HWND) hwndToolTip, 0L)

//-------------------------------------------------------------------
// Tool Tip Helper Macros
//-------------------------------------------------------------------
#define ToolTip_Activate(hwnd, fActivate) \
    (void)SendMessage((hwnd), TTM_ACTIVATE, (WPARAM) (BOOL) fActivate, 0L)

#define ToolTip_AddTool(hwnd, lpti) \
    (BOOL)SendMessage((hwnd), TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_DelTool(hwnd, lpti) \
    (void)SendMessage((hwnd), TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_EnumTools(hwnd, iTool, lpti) \
    (BOOL)SendMessage((hwnd), TTM_ENUMTOOLS, (WPARAM) (UINT) iTool, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_GetCurrentTool(hwnd, lpti) \
    (BOOL)SendMessage((hwnd), TTM_GETCURRENTTOOL, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_GetText(hwnd, lpti) \
    (void)SendMessage((hwnd), TTM_GETTEXT, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_GetToolCount(hwnd) \
    (int)SendMessage((hwnd), TTM_GETTOOLCOUNT, 0, 0L)

#define ToolTip_GetToolInfo(hwnd, lpti) \
    (BOOL)SendMessage((hwnd), TTM_GETTOOLINFO, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_HitText(hwnd, lphti) \
    (BOOL)SendMessage((hwnd), TTM_HITTEST, 0, (LPARAM) (LPHITTESTINFO) lphti)

#define ToolTip_NewToolRect(hwnd, lpti) \
    (void)SendMessage((hwnd), TTM_NEWTOOLRECT, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_RelayEvent(hwnd, lpmsg) \
    (void)SendMessage((hwnd), TTM_RELAYEVENT, 0, (LPARAM) (LPMSG) lpmsg)

#define ToolTip_SetDelayTime(hwnd, uFlag, iDelay) \
    (void)SendMessage((hwnd), TTM_SETDELAYTIME, (WPARAM) uFlag, (LPARAM) (int) iDelay)

#define ToolTip_SetToolInfo(hwnd, lpti) \
    (void)SendMessage((hwnd), TTM_SETTOOLINFO, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_UpdateTipText(hwnd, lpti) \
    (void)SendMessage((hwnd), TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) lpti)

#define ToolTip_WindowFromPoint(hwnd, lppt) \
    (HWND)SendMessage((hwnd), TTM_WINDOWFROMPOINT, 0, (POINT FAR *) lppt)

//-------------------------------------------------------------------
// Track Bar Helper Macros
//-------------------------------------------------------------------
#define TrackBar_ClearSel(hwnd, fRedraw) \
    (void)SendMessage((hwnd), TBM_CLEARSEL, (WPARAM) (BOOL) fRedraw, 0L)

#define TrackBar_ClearTics(hwnd, fRedraw) \
    (void)SendMessage((hwnd), TBM_CLEARTICS, (WPARAM) (BOOL) fRedraw, 0L)

#define TrackBar_GetChannelRect(hwnd, lprc) \
    (void)SendMessage((hwnd), TBM_GETCHANNELRECT, 0, (LPARAM) (LPRECT) lprc)

#define TrackBar_GetLineSize(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETLINESIZE, 0, 0L)

#define TrackBar_GetNumTics(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETNUMTICS, 0, 0L)

#define TrackBar_GetPageSize(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETPAGESIZE, 0, 0L)

#define TrackBar_GetPos(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETPOS, 0, 0L)

#define TrackBar_GetPTics(hwnd) \
    (LPLONG)SendMessage((hwnd), TBM_GETPTICS, 0, 0L)

#define TrackBar_GetRangeMax(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETRANGEMAX, 0, 0L)

#define TrackBar_GetRangeMin(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETRANGEMIN, 0, 0L)

#define TrackBar_GetSelEnd(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETSELEND, 0, 0L)

#define TrackBar_GetSelStart(hwnd) \
    (LONG)SendMessage((hwnd), TBM_GETSELSTART, 0, 0L)

#define TrackBar_GetThumbLength(hwnd) \
    (UINT)SendMessage((hwnd), TBM_GETTHUMBLENGTH, 0, 0L)

#define TrackBar_GetThumbRect(hwnd, lprc) \
    (void)SendMessage((hwnd), TBM_GETTHUMBRECT, 0, (LPARAM) (LPRECT) lprc)

#define TrackBar_GetTic(hwnd, iTic) \
    (LONG)SendMessage((hwnd), TBM_GETTIC, (WPARAM) (WORD) iTic, 0L)

#define TrackBar_GetTicPos(hwnd, iTic) \
    (LONG)SendMessage((hwnd), TBM_GETTICPOS, (WPARAM) (WORD) iTic, 0L)

#define TrackBar_SetLineSize(hwnd, lLineSize) \
    (LONG)SendMessage((hwnd), TBM_SETLINESIZE, 0, (LONG) lLineSize)

#define TrackBar_SetPageSize(hwnd, lPageSize) \
    (LONG)SendMessage((hwnd), TBM_SETPAGESIZE, 0, (LONG) lPageSize)

#define TrackBar_SetPos(hwnd, bPosition, lPosition) \
    (void)SendMessage((hwnd), TBM_SETPOS, (WPARAM) (BOOL) bPosition, (LPARAM) (LONG) lPosition)

#define TrackBar_SetRange(hwnd, bRedraw, lMinimum, lMaximum) \
    (void)SendMessage((hwnd), TBM_SETRANGE, (WPARAM) (BOOL) bRedraw, (LPARAM) MAKELONG(lMinimum, lMaximum))

#define TrackBar_SetRangeMax(hwnd, bRedraw, lMaximum) \
    (void)SendMessage((hwnd), TBM_SETRANGEMAX, (WPARAM) bRedraw, (LPARAM) lMaximum)

#define TrackBar_SetRangeMin(hwnd, bRedraw, lMinimum) \
    (void)SendMessage((hwnd), TBM_SETRANGEMIN, (WPARAM) bRedraw, (LPARAM) lMinimum)

#define TrackBar_SetSel(hwnd, bRedraw, lMinimum, lMaximum) \
    (void)SendMessage((hwnd), TBM_SETSEL, (WPARAM) (BOOL) bRedraw, (LPARAM) MAKELONG(lMinimum, lMaximum))

#define TrackBar_SetSelEnd(hwnd, bRedraw, lEnd) \
    (void)SendMessage((hwnd), TBM_SETSELEND, (WPARAM) (BOOL) bRedraw, (LPARAM) (LONG) lEnd)

#define TrackBar_SetSelStart(hwnd, bRedraw, lStart) \
    (void)SendMessage((hwnd), TBM_SETSELSTART, (WPARAM) (BOOL) bRedraw, (LPARAM) (LONG) lStart)

#define TrackBar_SetThumbLength(hwnd, iLength) \
    (void)SendMessage((hwnd), TBM_SETTHUMBLENGTH, (WPARAM) (UINT) iLength, 0L)

#define TrackBar_SetTic(hwnd, lPosition) \
    (BOOL)SendMessage((hwnd), TBM_SETTIC, 0, (LPARAM) (LONG) lPosition)

#define TrackBar_SetTicFreq(hwnd, wFreq, lPosition) \
    (void)SendMessage((hwnd), TBM_SETTICFREQ, (WPARAM) wFreq, (LPARAM) (LONG) lPosition)

//-------------------------------------------------------------------
// Up / Down Control Helper Macros
//-------------------------------------------------------------------
#define UpDown_GetAccel(hwnd, cAccels, paAccels) \
    (int)SendMessage((hwnd), UDM_GETACCEL, (WPARAM) cAccels, (LPARAM) (LPUDACCEL) paAccels)

#define UpDown_GetBase(hwnd) \
    (int)SendMessage((hwnd), UDM_GETBASE, 0, 0L)

#define UpDown_GetBuddy(hwnd) \
    (HWND)SendMessage((hwnd), UDM_GETBUDDY, 0, 0L)

#define UpDown_GetPos(hwnd) \
    (DWORD)SendMessage((hwnd), UDM_GETPOS, 0, 0L)

#define UpDown_GetRange(hwnd) \
    (DWORD)SendMessage((hwnd), UDM_GETRANGE, 0, 0L)

#define UpDown_SetAccel(hwnd, nAccels, aAccels) \
    (BOOL)SendMessage((hwnd), UDM_SETACCEL, (WPARAM) nAccels, (LPARAM) (LPUDACCEL) aAccels)

#define UpDown_SetBase(hwnd, nBase) \
    (int)SendMessage((hwnd), UDM_SETBASE, (WPARAM) nBase, 0L)

#define UpDown_SetBuddy(hwnd, hwndBuddy) \
    (HWND)SendMessage((hwnd), UDM_SETBUDDY, (WPARAM) (HWND) hwndBuddy, 0L)

#define UpDown_SetPos(hwnd, nPos) \
    (short)SendMessage((hwnd), UDM_SETPOS, 0, (LPARAM) MAKELONG((short) nPos, 0))

#define UpDown_SetRange(hwnd, nUpper, nLower) \
    (void)SendMessage((hwnd), UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) nUpper, (short) nLower))
