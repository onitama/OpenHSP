;
;	HELP source file for HSP help manager
;	(Lines starting with ";" are treated as comments)
;

%type
HSP System Variables
%ver
3.7
%note
ver3.7 standard
%date
2025/01/22
%author
onitama
%url
https://www.onionsoft.net/
%port
Win
Cli
Let


;---------------------------------------------------------------------
%index
hspstat
HSP Runtime Information

%group
System Variables

%inst
Information about the HSP runtime is assigned as an integer value. This value is the sum of all the following information:
 Debug mode = 1
 Screen saver active = 2
 Console mode = 16
 Macintosh version of HSP = $80
 HSP3Dish = $100
 Linux version of HSP = $1000
 UTF8 string runtime = $20000
 64-bit runtime = $40000

%href
_debug

;---------------------------------------------------------------------
%index
hspver
HSP Version Number

%group
System Variables

%inst
The HSP version number is assigned as an integer value.  It is the value of the version code + minor version code (3.0 will be $3???).

%href
__hspver__

;---------------------------------------------------------------------
%index
cnt
Loop Counter

%group
System Variables

%inst
The counter for the repeatÅ`loop loop is assigned as an integer value.

%href
repeat
foreach
loop

;---------------------------------------------------------------------
%index
err
HSP Error Code

%group
System Variables

%inst
The HSP error code. See the separate manual (error.htm) for the correspondence between error codes and messages.


%href
onerror


;---------------------------------------------------------------------
%index
stat
Status Value of Various Commands

%group
System Variables

%inst
The result (status) of executing various commands is assigned as an integer value.

%href
return
dialog
mref

;---------------------------------------------------------------------
%index
mousex
Mouse Cursor X Coordinate

%group
System Variables

%inst
The X coordinate of the mouse cursor is assigned as an integer value.

%sample
	; Draw a line following the movement of the mouse pointer
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
Mouse Cursor Y Coordinate

%group
System Variables

%inst
The Y coordinate of the mouse cursor is assigned as an integer value.

%sample
	; Draw a line following the movement of the mouse pointer
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
Mouse Cursor Wheel Value

%group
System Variables

%inst
The amount of movement of a mouse with a wheel is assigned as an integer value.

%href
mousex
mousey
%port-
Let

;---------------------------------------------------------------------
%index
strsize
Number of Bytes Read

%group
System Variables

%inst
The number of bytes read by commands such as getstr, exist, and bload is assigned.

%href
bload
exist
getstr

;---------------------------------------------------------------------
%index
refstr
Status String of Various Commands


%group
System Variables

%inst
The string result of executing various commands is assigned.

%href
return
dialog
mref

;---------------------------------------------------------------------
%index
looplev
Repeat Nesting Level

%group
System Variables

%inst
The nesting level of repeatÅ`loop is assigned.

%href
repeat
foreach
loop

;---------------------------------------------------------------------
%index
sublev
Subroutine Nesting Level

%group
System Variables

%inst
The nesting level of subroutines (or user-defined commands and functions) is assigned.

%href
gosub
return

;---------------------------------------------------------------------
%index
iparam
Value Indicating Interrupt Cause

%group
System Variables

%inst
A value indicating the cause of the interrupt is assigned.
When interrupted by the oncmd command, the ID of the interrupt message is assigned.
When exiting with the onexit command, 0 is assigned if it is a normal window close, and 1 is assigned if it is a system notification such as shutdown.


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
	mes "Key Code : "+wparam+", Character Code : "+iparam
	stop
%port-
Let

;---------------------------------------------------------------------
%index
wparam
wParam at Interrupt Time

%group
System Variables

%inst
The Windows system value (wParam) saved at the time of the interrupt is assigned.

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
	mes "Key Code : "+wparam+", Extended Key Flag : "+a
	stop
%port-
Let

;---------------------------------------------------------------------
%index
lparam
lParam at Interrupt Time

%group
System Variables

%inst
The Windows system value (lParam) saved at the time of the interrupt is assigned.

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
	mes "Key Code : "+wparam+", Extended Key Flag : "+a
	stop
%port-
Let

;---------------------------------------------------------------------
%index
hwnd
Current Window Handle

%group
System Variables

%inst
The handle (pointer) of the currently selected window is assigned.
It is mainly referenced when necessary for external calls such as DLLs.

%href
mref

;---------------------------------------------------------------------
%index
hdc
Current Device Context

%group
System Variables

%inst
The device context (pointer) of the currently selected window is assigned.
It is mainly referenced when necessary for external calls such as DLLs.

%href
mref

;---------------------------------------------------------------------
%index
hinstance
Current Instance Handle

%group
System Variables

%inst
The instance handle (pointer) of the currently running application is assigned.
It is mainly referenced when necessary for external calls such as DLLs.

%href
mref
%port-
Let

;---------------------------------------------------------------------
%index
refdval
Status Real Value of Various Commands

%group
System Variables

%inst
The real number result of executing various commands is assigned.

%href
return

;---------------------------------------------------------------------
%index
thismod
Current Valid Module Variable

%group
System Variables

%inst
Within user-defined commands/functions for module variable processing,
it can be used as a name indicating the module variable itself that was passed.
It can be used when calling another command/function from a routine defined by #modfunc.

%href
newmod
delmod
%port-
Let

;---------------------------------------------------------------------
%index
notemax
Number of Lines in Memory Notepad

%group
System Variables

%inst
Can be used as a system variable that indicates the number of lines in the currently selected memory notepad.
Similar information can also be obtained with the noteinfo function.
^
*This system variable is defined as a macro within hspdef.as.

%href
noteinfo

;---------------------------------------------------------------------
%index
notesize
Number of Characters in Memory Notepad

%group
System Variables

%inst
Indicates the total number of characters (bytes) in the currently selected memory notepad.
Can be used as a system variable.
Similar information can also be obtained with the noteinfo function.
^
*This system variable is defined as a macro within hspdef.as.

%href
noteinfo

;---------------------------------------------------------------------
%index
ginfo_mx
Mouse Cursor X Coordinate on Screen

%group
System Variables

%inst
The X coordinate of the mouse cursor on the screen is assigned.
The screen coordinate system is not the coordinate within the window used by the pos command, but the coordinate seen from the entire desktop screen.
^
*This system variable is defined as a macro within hspdef.as.

%href
ginfo
ginfo_my

;---------------------------------------------------------------------
%index
ginfo_my
Mouse Cursor Y Coordinate on Screen

%group
System Variables

%inst
The Y coordinate of the mouse cursor on the screen is assigned.
The screen coordinate system is not the coordinate within the window used by the pos command, but the coordinate seen from the entire desktop screen.
^
*This system variable is defined as a macro within hspdef.as.

%href
ginfo
ginfo_mx

;---------------------------------------------------------------------
%index
ginfo_act
Active Window ID

%group
System Variables

%inst
The ID of the currently active window is assigned.
If the active window is other than HSP, it will be -1.
^
*This system variable is defined as a macro within hspdef.as.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_sel
Operation Target Window ID

%group
System Variables

%inst
The operation target window ID specified by the gsel command is assigned.
^
*This system variable is defined as a macro within hspdef.as.

%href
gsel
ginfo

;---------------------------------------------------------------------
%index
ginfo_wx1
X Coordinate of the Upper Left of the Window

%group
System Variables

%inst
The X coordinate of the upper left of the current window is assigned in the screen coordinate system.
The screen coordinate system is not the coordinate within the window used by the pos command, but the coordinate seen from the entire desktop screen.
^
*This system variable is defined as a macro within hspdef.as.
*This value cannot be obtained in systems such as html5, iOS, and android.

%href
ginfo_wy1
ginfo_wx2
ginfo_wy2
ginfo

;---------------------------------------------------------------------
%index
ginfo_wy1
Y Coordinate of the Upper Left of the Window

%group
System Variables

%inst
The Y coordinate of the upper left of the current window is assigned in the screen coordinate system.
The screen coordinate system is not the coordinate within the window used by the pos command, but the coordinate seen from the entire desktop screen.
^
*This system variable is defined as a macro within hspdef.as.
*This value cannot be obtained in systems such as html5, iOS, and android.
%href
ginfo_wx1
ginfo_wx2
ginfo_wy2
ginfo

;---------------------------------------------------------------------
%index
ginfo_wx2
Right-bottom X coordinate of the window

%group
System Variable

%inst
The X coordinate of the bottom right corner of the current window in screen coordinates is stored.
Screen coordinates are not the coordinates within the window used by the pos command, but the coordinates seen from the entire desktop screen.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_wy2
ginfo_wx1
ginfo_wy1
ginfo

;---------------------------------------------------------------------
%index
ginfo_wy2
Right-bottom Y coordinate of the window

%group
System Variable

%inst
The Y coordinate of the bottom right corner of the current window in screen coordinates is stored.
Screen coordinates are not the coordinates within the window used by the pos command, but the coordinates seen from the entire desktop screen.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

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
System Variable

%inst
The scroll X coordinate of the current operation target window is stored.
This value indicates how much the coordinates displayed in the upper left corner are scrolled when the window size is smaller than the initialization size.
In the normal state where it is not scrolled, it will be (0,0).
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_vy
ginfo

;---------------------------------------------------------------------
%index
ginfo_vy
Window scroll Y coordinate

%group
System Variable

%inst
The scroll Y coordinate of the current operation target window is stored.
This value indicates how much the coordinates displayed in the upper left corner are scrolled when the window size is smaller than the initialization size.
In the normal state where it is not scrolled, it will be (0,0).
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_vx
ginfo

;---------------------------------------------------------------------
%index
ginfo_sizex
Overall X size of the window

%group
System Variable

%inst
The width of the current operation target window is stored.
This is the size including the window frame and title bar.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_sizey
ginfo

;---------------------------------------------------------------------
%index
ginfo_sizey
Overall Y size of the window

%group
System Variable

%inst
The height of the current operation target window is stored.
This is the size including the window frame and title bar.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_sizex
ginfo

;---------------------------------------------------------------------
%index
ginfo_winx
Drawing area X size of the screen

%group
System Variable

%inst
The horizontal width of the current window drawing area is stored.
(In the case of a screen created with the buffer command, this will be the initialized screen size.)
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_winy
ginfo

;---------------------------------------------------------------------
%index
ginfo_winy
Drawing area Y size of the screen

%group
System Variable

%inst
The vertical width of the current window drawing area is stored.
(In the case of a screen created with the buffer command, this will be the initialized screen size.)
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_winx
ginfo

;---------------------------------------------------------------------
%index
ginfo_sx
Initialization X size of the screen

%group
System Variable

%inst
The initialization X size of the current operation target window is stored.
(This is the screen size initially initialized with the screen, bgscr, or buffer command.)
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_sy
ginfo

;---------------------------------------------------------------------
%index
ginfo_sy
Initialization Y size of the screen

%group
System Variable

%inst
The initialization Y size of the current operation target window is stored.
(This is the screen size initially initialized with the screen, bgscr, or buffer command.)
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_sx
ginfo

;---------------------------------------------------------------------
%index
ginfo_mesx
Message output X size

%group
System Variable

%inst
The X size of the message output by the last mes or print command is stored.
The size indicates how many dots were drawn in the X coordinate.
Note that this is not an absolute coordinate on the screen.
Also, if you output a multi-line string, the size of the last line will be acquired.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_mesy
ginfo

;---------------------------------------------------------------------
%index
ginfo_mesy
Message output Y size

%group
System Variable

%inst
The Y size of the message output by the last mes or print command is stored.
The size indicates how many dots were drawn in the Y coordinate.
Note that this is not an absolute coordinate on the screen.
Also, if you output a multi-line string, the size of the last line will be acquired.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_mesx
ginfo

;---------------------------------------------------------------------
%index
ginfo_r
Currently set color code R

%group
System Variable

%inst
The red color code (brightness) specified by the color command, etc., is stored.
The color code is an integer value from 0 to 255.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_g
ginfo_b
ginfo

;---------------------------------------------------------------------
%index
ginfo_g
Currently set color code G

%group
System Variable

%inst
The green color code (brightness) specified by the color command, etc., is stored.
The color code is an integer value from 0 to 255.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_r
ginfo_b
ginfo

;---------------------------------------------------------------------
%index
ginfo_b
Currently set color code B

%group
System Variable

%inst
The blue color code (brightness) specified by the color command, etc., is stored.
The color code is an integer value from 0 to 255.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_r
ginfo_g
ginfo

;---------------------------------------------------------------------
%index
ginfo_paluse
Desktop color mode

%group
System Variable

%inst
The current desktop color mode is stored.
0 is returned for full color mode, and 1 is returned for palette mode.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_dispx
Overall X size of the desktop

%group
System Variable

%inst
The overall size of the desktop (screen resolution) is stored.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_dispy
ginfo

;---------------------------------------------------------------------
%index
ginfo_dispy
Overall Y size of the desktop

%group
System Variable

%inst
The overall size of the desktop (screen resolution) is stored.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
ginfo_dispx
ginfo

;---------------------------------------------------------------------
%index
ginfo_cx
X coordinate of the current position

%group
System Variable

%inst
The X coordinate of the current position set by the pos command is stored.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_cy
ginfo

;---------------------------------------------------------------------
%index
ginfo_cy
Y coordinate of the current position

%group
System Variable

%inst
The Y coordinate of the current position set by the pos command is stored.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo_cx
ginfo

;---------------------------------------------------------------------
%index
ginfo_intid
Window ID during message interrupt

%group
System Variable

%inst
The window ID during message interrupt set by the oncmd command is stored.
^
* This system variable is defined as a macro in hspdef.as.
* This value cannot be obtained in systems such as html5, iOS, and Android.

%href
oncmd
ginfo

;---------------------------------------------------------------------
%index
ginfo_newid
Unused window ID

%group
System Variable

%inst
Returns an unused window ID that has not been initialized with the screen or buffer command.
^
* This system variable is defined as a macro in hspdef.as.

%href
ginfo

;---------------------------------------------------------------------
%index
ginfo_accelx
Acceleration sensor value (X)

%group
System Variable

%inst
Acquires the value from the acceleration sensor in an environment where it is available.
This is the acceleration with respect to the X axis, obtained as a real number (m/s^2). (The value includes the gravitational acceleration of 9.8 m/s^2.)
In environments where the acceleration sensor is not available, 0.0 is obtained.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device in HSP3Dish.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_accely
ginfo_accelz

;---------------------------------------------------------------------
%index
ginfo_accely
Accelerometer Value (Y)

%group
System Variables

%inst
Acquires the accelerometer value in environments where the accelerometer is available.
This is the acceleration with respect to the Y-axis, obtained as a real number (m/s^2). (The value includes the gravitational acceleration of 9.8 m/s^2.)
If the accelerometer is not available, 0.0 is returned.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_accelx
ginfo_accelz

;---------------------------------------------------------------------
%index
ginfo_accelz
Accelerometer Value (Z)

%group
System Variables

%inst
Acquires the accelerometer value in environments where the accelerometer is available.
This is the acceleration with respect to the Z-axis, obtained as a real number (m/s^2). (The value includes the gravitational acceleration of 9.8 m/s^2.)
If the accelerometer is not available, 0.0 is returned.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_accely
ginfo_accelx

;---------------------------------------------------------------------
%index
ginfo_gyrox
Tilt Sensor Value (X)

%group
System Variables

%inst
Acquires the tilt sensor value in environments where the tilt sensor is available.
This is the tilt angle with respect to the X-axis, obtained in degrees.
If the tilt sensor is not available, 0.0 is returned.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_gyroy
ginfo_gyroz

;---------------------------------------------------------------------
%index
ginfo_gyroy
Tilt Sensor Value (Y)

%group
System Variables

%inst
Acquires the tilt sensor value in environments where the tilt sensor is available.
This is the tilt angle with respect to the Y-axis, obtained in degrees.
If the tilt sensor is not available, 0.0 is returned.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_gyrox
ginfo_gyroz

;---------------------------------------------------------------------
%index
ginfo_gyroz
Tilt Sensor Value (Z)

%group
System Variables

%inst
Acquires the tilt sensor value in environments where the tilt sensor is available.
This is the tilt angle with respect to the Z-axis, obtained in degrees.
If the tilt sensor is not available, 0.0 is returned.
^
Å¶This system variable is defined as a macro within hspdef.as.
Å¶It may not be obtainable depending on the device.
Å¶For HTML5, an HTTPS connection is required. Please note that some browsers may not support this.

%href
ginfo
ginfo_gyroy
ginfo_gyrox

;---------------------------------------------------------------------
%index
dir_cur
Current Directory (Folder)

%group
System Variables

%inst
The path of the current directory (folder) is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_exe
Directory (Folder) where the Executable File is Located

%group
System Variables

%inst
The path of the directory (folder) where the runtime executable file is located is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_win
Windows Directory (Folder)

%group
System Variables

%inst
The path of the directory (folder) where Windows is installed is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_sys
Windows System Directory (Folder)

%group
System Variables

%inst
The path of the Windows system directory (folder) is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_cmdline
Command Line String

%group
System Variables

%inst
The command line string passed at execution time is assigned.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_desktop
Desktop Directory (Folder)

%group
System Variables

%inst
The path of the desktop directory (folder) is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_mydoc
My Documents Directory (Folder)

%group
System Variables

%inst
The path of the My Documents directory (folder) is assigned as a string.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo

;---------------------------------------------------------------------
%index
dir_tv
HSPTV Directory (Folder)

%group
System Variables

%inst
The path of the HSPTV directory (folder) is assigned as a string.
It can be used to refer to resources that are standardly prepared in the HSPTV directory.
dir_tv is only acquired during debug execution, and returns an empty string ("") when running as an executable (.exe) file. When creating an executable file for software that uses files in the HSPTV directory, be careful to specify the necessary files using the #pack command.
^
Å¶This system variable is defined as a macro within hspdef.as.

%href
dirinfo
