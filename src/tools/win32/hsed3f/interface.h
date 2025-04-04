/*------------------------------------------------------------------------------
	Header of interface routines
------------------------------------------------------------------------------*/

#ifndef __interface_h
#define __interface_h

//
// Functions
bool InitInterface(HINSTANCE hInstance);

//
// Messages
#define HSED_GETVER          (WM_APP + 0x000)
#define HSED_GETWND          (WM_APP + 0x100)
#define HSED_GETPATH         (WM_APP + 0x101)

#define HSED_GETTABCOUNT     (WM_APP + 0x200)
#define HSED_GETTABID        (WM_APP + 0x201)
#define HSED_GETFOOTYID      (WM_APP + 0x202)
#define HSED_GETACTTABID     (WM_APP + 0x203)
#define HSED_GETACTFOOTYID   (WM_APP + 0x204)

#define HSED_CANCOPY         (WM_APP + 0x300)
#define HSED_CANPASTE        (WM_APP + 0x301)
#define HSED_CANUNDO         (WM_APP + 0x302)
#define HSED_CANREDO         (WM_APP + 0x303)
#define HSED_GETMODIFY       (WM_APP + 0x304)

#define HSED_COPY            (WM_APP + 0x310)
#define HSED_CUT             (WM_APP + 0x311)
#define HSED_PASTE           (WM_APP + 0x312)
#define HSED_UNDO            (WM_APP + 0x313)
#define HSED_REDO            (WM_APP + 0x314)
#define HSED_INDENT          (WM_APP + 0x315)
#define HSED_UNINDENT        (WM_APP + 0x316)
#define HSED_SELECTALL       (WM_APP + 0x317)

#define HSED_SETTEXT         (WM_APP + 0x320)
#define HSED_GETTEXT         (WM_APP + 0x321)
#define HSED_GETTEXTLENGTH   (WM_APP + 0x322)
#define HSED_GETLINES        (WM_APP + 0x323)
#define HSED_SETSELTEXT      (WM_APP + 0x324)
#define HSED_GETSELTEXT      (WM_APP + 0x325)
#define HSED_GETLINETEXT     (WM_APP + 0x326)
#define HSED_GETLINELENGTH   (WM_APP + 0x327)
#define HSED_GETLINECODE     (WM_APP + 0x328)

#define HSED_SETSELA         (WM_APP + 0x330)
#define HSED_SETSELB         (WM_APP + 0x331)
#define HSED_GETSELA         (WM_APP + 0x332)
#define HSED_GETSELB         (WM_APP + 0x333)

#define HSED_GETCARETLINE    (WM_APP + 0x340)
#define HSED_GETCARETPOS     (WM_APP + 0x341)
#define HSED_GETCARETTHROUGH (WM_APP + 0x342)
#define HSED_GETCARETVPOS    (WM_APP + 0x343)
#define HSED_SETCARETLINE    (WM_APP + 0x344)
#define HSED_SETCARETPOS     (WM_APP + 0x345)
#define HSED_SETCARETTHROUGH (WM_APP + 0x346)

#define HSED_SETMARK         (WM_APP + 0x350)
#define HSED_GETMARK         (WM_APP + 0x351)
#define HSED_SETHIGHLIGHT    (WM_APP + 0x352)

//
// Constants for HSED_GETVER
#define HGV_PUBLICVER    0
#define HGV_PRIVATEVER   1
#define HGV_HSPCMPVER    2
#define HGV_FOOTYVER     3
#define HGV_FOOTYBETAVER 4

//
// Constants for HSED_GETWND
#define HGW_MAIN      0
#define HGW_CLIENT    1
#define HGW_TAB       2
#define HGW_FOOTY     3
#define HGW_TOOLBAR   4
#define HGW_STATUSBAR 5

#endif