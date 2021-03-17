;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
HSP system variables
%ver
3.6
%note
ver3.6 standard
%date
2017/09/16
%author
onitama
%url
http://www.onionsoft.net/
%port
Win
Cli
Let


;---------------------------------------------------------------------
%index
hspstat
HSP runtime information

%group
System variables

%inst
HSP runtime information is assigned as an integer value. The following information is the sum of all the following information.
 Debug mode = 1
 When the screen saver is started = 2
 Console mode = 16
 Macintosh version HSP = $ 80
 HSP3Dish = $100
 Linux version HSP = $ 1000
 UTF8 string runtime = $ 20000
 64-bit runtime = $ 40000

%href
_debug

;---------------------------------------------------------------------
%index
hspver
HSP version number

%group
System variables

%inst
The HSP version number is assigned as an integer value. It is the value of version code + minor version code. (3.0 becomes $ 3 ???)

%href
__hspver__

;---------------------------------------------------------------------
%index
cnt
Loop counter

%group
System variables

%inst
The repeat to loop loop counter is assigned as an integer value.

%href
repeat
foreach
loop

;---------------------------------------------------------------------
%index
err
HSP error code

%group
System variables

%inst
HSP error code. Please refer to the separate manual (error.htm) for the correspondence between error codes and messages.


%href
onerror


;---------------------------------------------------------------------
%index
stat
Status values for various instructions

%group
System variables

%inst
The result (status) of executing various instructions is assigned as an integer value.

%href
return
dialog
mref

;---------------------------------------------------------------------
%index
mousex
X coordinate of mouse cursor

%group
System variables

%inst
The X coordinate of the mouse cursor is assigned as an integer value.

%sample
Draw a line along the movement of the mouse pointer
	repeat
		line mousex, mousey
		wait 1
		loop
%href
mouse
mousey
mousew
ginfo_mx

;---------------------------------------------------------------------
%index
mousey
Y coordinate of mouse cursor

%group
System variables

%inst
The Y coordinate of the mouse cursor is assigned as an integer value.

%sample
Draw a line along the movement of the mouse pointer
	repeat
		line mousex, mousey
		wait 1
		loop

%href
mouse
mousex
mousew
ginfo_my

;---------------------------------------------------------------------
%index
mousew
Mouse cursor wheel value

%group
System variables

%inst
The amount of movement of the mouse with a wheel is assigned as an integer value.

%href
mousex
mousey
%port-
Let

;---------------------------------------------------------------------
%index
strsize
Number of bytes to read

%group
System variables

%inst
The number of bytes read by the getstr, exist, bload instruction, etc. is assigned.

%href
bload
exist
getstr

;---------------------------------------------------------------------
%index
refstr
Status strings for various instructions


%group
System variables

%inst
The character string as a result of executing various instructions is assigned.

%href
return
dialog
mref

;---------------------------------------------------------------------
%index
looplev
Nested level of repeat

%group
System variables

%inst
The nesting level of repeat ~ loop is assigned.

%href
repeat
foreach
loop

;---------------------------------------------------------------------
%index
sublev
Subroutine nesting level

%group
System variables

%inst
The nesting level of the subroutine (or user-defined instruction, function) is assigned.

%href
gosub
return

;---------------------------------------------------------------------
%index
iparam
A value that indicates the interrupt factor

%group
System variables

%inst
A value indicating the interrupt factor is assigned.
When interrupted by the oncmd instruction, the interrupt message ID is assigned.
At the end interrupt by the onexit instruction, 0 is assigned if the end is due to normal window closing, and 1 is assigned if the end is due to system notification such as shutdown.


%href
onkey
onclick
onexit
lparam
wparam
onerror
oncmd

%sample
	onkey goto *lab
	stop
*lab
mes "key code:" + wparam + ", character code:" + iparam
	stop
%port-
Let

;---------------------------------------------------------------------
%index
wparam
WParam on interrupt

%group
System variables

%inst
The Windows system value (wParam) saved on interrupt is assigned.

%href
onkey
onclick
onerror
iparam
lparam
onerror
oncmd

%sample
	onkey goto *lab
	stop
*lab
	a = lparam>>24&1
mes "key code:" + wparam + ", extended key flag:" + a
	stop
%port-
Let

;---------------------------------------------------------------------
%index
lparam
LParam on interrupt

%group
System variables

%inst
The Windows system value (lParam) that is saved on interrupt is assigned.

%href
onkey
onclick
onerror
iparam
wparam
onerror
oncmd

%sample
	onkey goto *lab
	stop
*lab
	a = lparam>>24&1
mes "key code:" + wparam + ", extended key flag:" + a
	stop
%port-
Let

;---------------------------------------------------------------------
%index
hwnd
Current window handle

%group
System variables

%inst
The handle (pointer) of the currently selected window is assigned.
It is mainly referred to when it is necessary for external calls such as DLLs.

%href
mref

;---------------------------------------------------------------------
%index
hdc
Current device context

%group
System variables

%inst
The device context (pointer) of the currently selected window is assigned.
It is mainly referred to when it is necessary for external calls such as DLLs.

%href
mref

;---------------------------------------------------------------------
%index
hinstance
Current instance handle

%group
System variables

%inst
The instance handle (pointer) of the currently running application is assigned.
It is mainly referred to when it is necessary for external calls such as DLLs.

%href
mref
%port-
Let

;---------------------------------------------------------------------
%index
refdval
Status real value of various instructions

%group
System variables

%inst
The real value of the result of executing various instructions is assigned.

%href
return

;---------------------------------------------------------------------
%index
thismod
Currently valid module variables

%group
System variables

%inst
In user-defined instructions / functions for module variable processing
It can be used as a name to indicate the passed module variable itself.
It can be used when calling another instruction / function from the routine defined by #modfunc.

%href
newmod
delmod
%port-
Let

;---------------------------------------------------------------------
%index
notemax
Number of lines in memory notepad

%group
System variables

%inst
It can be used as a system variable to indicate the total number of rows in the currently selected memory notepad.
Similar information can be obtained with the noteinfo function.
^
* This system variable is defined in hspdef.as as a macro.

%href
noteinfo

;---------------------------------------------------------------------
%index
notesize
Number of characters in memory notepad

%group
System variables

%inst
Indicates the total number of characters (bytes) of the currently selected memory notepad
It can be used as a system variable.
Similar information can be obtained with the noteinfo function.
^
* This system variable is defined in hspdef.as as a macro.

%href
noteinfo

;---------------------------------------------------------------------
%index
ginfo_mx
Mouse cursor X coordinates on screen

%group
System variables

%inst
The X coordinate of the mouse cursor on the screen has been assigned.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo
ginfo_my

;---------------------------------------------------------------------
%index
ginfo_my
Mouse cursor Y coordinate on screen

%group
System variables

%inst
The Y coordinate of the mouse cursor on the screen has been assigned.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo
ginfo_mx

;---------------------------------------------------------------------
%index
ginfo_act
Active window ID

%group
System variables

%inst
The currently active window ID has been assigned.
If the active window is other than HSP, it will be -1.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_sel
Operation destination window ID

%group
System variables

%inst
The operation destination window ID of the screen specified by the gsel instruction is assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
gsel
ginfo

;---------------------------------------------------------------------
%index
ginfo_wx1
Top left X coordinate of the window

%group
System variables

%inst
The upper left X coordinate of the current window is assigned in the screen coordinate system.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_wy1
ginfo_wx2
ginfo_wy2
ginfo

;---------------------------------------------------------------------
%index
ginfo_wy1
Upper left Y coordinate of the window

%group
System variables

%inst
The upper left Y coordinate of the current window is assigned in the screen coordinate system.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_wx1
ginfo_wx2
ginfo_wy2
ginfo

;---------------------------------------------------------------------
%index
ginfo_wx2
Lower right X coordinate of the window

%group
System variables

%inst
The lower right X coordinate of the current window is assigned in the screen coordinate system.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_wy2
ginfo_wx1
ginfo_wy1
ginfo

;---------------------------------------------------------------------
%index
ginfo_wy2
Lower right Y coordinate of the window

%group
System variables

%inst
The lower right Y coordinate of the current window is assigned in the screen coordinate system.
The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_wx2
ginfo_wx1
ginfo_wy1
ginfo

;---------------------------------------------------------------------
%index
ginfo_vx
Window scroll X coordinate

%group
System variables

%inst
The scroll X coordinate of the current destination window has been assigned.
This is a value that indicates how much the coordinates displayed in the upper left are scrolled when the window size is smaller than the initialization size.
In the normal state without scrolling, it will be (0,0).
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_vy
ginfo

;---------------------------------------------------------------------
%index
ginfo_vy
Window scroll Y coordinate

%group
System variables

%inst
The scroll Y coordinate of the current destination window has been assigned.
This is a value that indicates how much the coordinates displayed in the upper left are scrolled when the window size is smaller than the initialization size.
In the normal state without scrolling, it will be (0,0).
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_vx
ginfo

;---------------------------------------------------------------------
%index
ginfo_sizex
X size of the entire window

%group
System variables

%inst
The width of the current operation window is assigned.
This is the size including the window frame, title bar, etc.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_sizey
ginfo

;---------------------------------------------------------------------
%index
ginfo_sizey
Y size of the entire window

%group
System variables

%inst
The vertical width of the current operation window is assigned.
This is the size including the window frame, title bar, etc.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_sizex
ginfo

;---------------------------------------------------------------------
%index
ginfo_winx
Screen drawing area X size

%group
System variables

%inst
The width size of the current window drawing area has been assigned.
(If the screen is created by the buffer command, the screen size will be initialized.)
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_winy
ginfo

;---------------------------------------------------------------------
%index
ginfo_winy
Screen drawing area Y size

%group
System variables

%inst
The vertical size of the current window drawing area has been assigned.
(If the screen is created by the buffer command, the screen size will be initialized.)
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_winx
ginfo

;---------------------------------------------------------------------
%index
ginfo_sx
Screen initialization X size

%group
System variables

%inst
The initialization X size of the current destination window has been assigned.
(The screen size is initially initialized by the screen, bgscr, and buffer instructions.)
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_sy
ginfo

;---------------------------------------------------------------------
%index
ginfo_sy
Screen initialization Y size

%group
System variables

%inst
The initialization Y size of the current destination window has been assigned.
(The screen size is initially initialized by the screen, bgscr, and buffer instructions.)
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_sx
ginfo

;---------------------------------------------------------------------
%index
ginfo_mesx
Message output X size

%group
System variables

%inst
Finally, the X size of the message output by the mes and print instructions is assigned.
The size indicates how many dots were drawn in the X coordinate.
Please note that it is not the absolute coordinates on the screen.
Also, if you output a character string with multiple lines, the size corresponding to the last line will be acquired.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_mesy
ginfo

;---------------------------------------------------------------------
%index
ginfo_mesy
Message output Y size

%group
System variables

%inst
Finally, the Y size of the message output by the mes and print instructions is assigned.
The size indicates how many dots were drawn in the Y coordinate.
Please note that it is not the absolute coordinates on the screen.
Also, if you output a character string with multiple lines, the size corresponding to the last line will be acquired.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_mesx
ginfo

;---------------------------------------------------------------------
%index
ginfo_r
Currently set color code R

%group
System variables

%inst
The red color code (luminance) specified by the color command etc. is substituted.
The color code can be an integer value from 0 to 255.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_g
ginfo_b
ginfo

;---------------------------------------------------------------------
%index
ginfo_g
Currently set color code G

%group
System variables

%inst
The green color code (luminance) specified by the color command etc. is substituted.
The color code can be an integer value from 0 to 255.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_r
ginfo_b
ginfo

;---------------------------------------------------------------------
%index
ginfo_b
Currently set color code B

%group
System variables

%inst
The blue color code (luminance) specified by the color command etc. is substituted.
The color code can be an integer value from 0 to 255.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_r
ginfo_g
ginfo

;---------------------------------------------------------------------
%index
ginfo_paluse
Desktop color mode

%group
System variables

%inst
The current desktop color mode (color mode) has been assigned.
Returns 0 for full color mode and 1 for palette mode.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_dispx
X size of the entire desktop

%group
System variables

%inst
The size of the entire desktop (screen resolution) is substituted.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_dispy
ginfo

;---------------------------------------------------------------------
%index
ginfo_dispy
Y size of the entire desktop

%group
System variables

%inst
The size of the entire desktop (screen resolution) has been assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_dispx
ginfo

;---------------------------------------------------------------------
%index
ginfo_cx
X coordinate of current position

%group
System variables

%inst
The X coordinate of the current position set by the pos instruction is assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_cy
ginfo

;---------------------------------------------------------------------
%index
ginfo_cy
Y coordinate of the current position

%group
System variables

%inst
The Y coordinate of the current position set by the pos instruction is assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo_cx
ginfo

;---------------------------------------------------------------------
%index
ginfo_intid
Window ID at the time of message interrupt

%group
System variables

%inst
The window ID at the time of message interrupt set by the oncmd instruction is assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
oncmd
ginfo

;---------------------------------------------------------------------
%index
ginfo_newid
Unused window ID

%group
System variables

%inst
An unused window ID that has not been initialized by the screen instruction or buffer instruction is returned.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_accx
Accelerometer value (X)

%group
System variables

%inst
Get the sensor value in an environment where the accelerometer is available.
The acceleration with respect to the X axis is obtained in the range of -1.0 to +1.0.
In an environment where the accelerometer cannot be used, 0.0 is obtained.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo
ginfo_accy
ginfo_accz

;---------------------------------------------------------------------
%index
ginfo_accy
Accelerometer value (Y)

%group
System variables

%inst
Get the sensor value in an environment where the accelerometer is available.
The acceleration with respect to the Y axis is obtained in the range of -1.0 to +1.0.
In an environment where the accelerometer cannot be used, 0.0 is obtained.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo
ginfo_accx
ginfo_accz

;---------------------------------------------------------------------
%index
ginfo_accz
Accelerometer value (Z)

%group
System variables

%inst
Get the sensor value in an environment where the accelerometer is available.
The acceleration with respect to the Z axis is obtained in the range of -1.0 to +1.0.
In an environment where the accelerometer cannot be used, 0.0 is obtained.
^
* This system variable is defined in hspdef.as as a macro.

%href
ginfo
ginfo_accy
ginfo_accx

;---------------------------------------------------------------------
%index
dir_cur
Current directory (folder)

%group
System variables

%inst
The path of the current current directory (folder) is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_exe
Directory (folder) containing the executable file

%group
System variables

%inst
The path of the directory (folder) where the runtime executable file is located is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_win
Windows directory (folder)

%group
System variables

%inst
The path of the directory (folder) where Windows is installed is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_sys
Windows system directory (folder)

%group
System variables

%inst
The path of the Windows system directory (folder) is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_cmdline
Command line string

%group
System variables

%inst
The command line string passed at run time is assigned.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_desktop
Desktop directory (folder)

%group
System variables

%inst
The path of the desktop directory (folder) is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_mydoc
My Documents Directory (Folder)

%group
System variables

%inst
The path of the My Documents directory (folder) is assigned as a character string.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_tv
HSPTV directory (folder)

%group
System variables

%inst
The path of the HSPTV directory (folder) is assigned as a character string.
It can be used to refer to the resources provided as standard in the HSPTV directory.
dir_tv is retrieved only during debug execution and returns an empty string ("") if it is running as an executable (.exe) file. Note that when creating an executable file for software that uses files in the HSPTV directory, it is necessary to specify the required files with the #pack instruction.
^
* This system variable is defined in hspdef.as as a macro.

%href
dirinfo

;---------------------------------------------------------------------
