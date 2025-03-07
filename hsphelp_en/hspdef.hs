%type
System definition macro
%ver
3.6
%date
2019/04/09
%note
This macro is defined in hspdef.as.
%group
Standard definition macro
%author
onitama
%port
Win
Mac
Cli


%index
gmode_sub
Color subtractive composite copy mode

%inst
The screen copy mode can be set to the color subtractive composite copy mode by specifying it in the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_sub, img_width, img_height, 256
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_add
Color additive synthesis copy mode

%inst
The screen copy mode can be set to the color additive synthesis copy mode by specifying it in the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_add, img_width, img_height, 256
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_sub
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_gdi
Normal copy mode

%inst
You can set the screen copy mode to normal copy mode by specifying it in the first argument of gmode.
You can omit it because the same result will be obtained.

%sample
	buffer 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_gdi
	gcopy 1, 0, 0, img_width, img_height
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_sub
gmode_rgb0alpha
gmode_pixela
%index
gmode_rgb0
Transparent colored copy mode

%inst
You can set the screen copy mode to transparent colored copy mode by specifying it in the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_rgb0, img_width, img_height
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_sub
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_mem
Memory-to-memory copy mode

%inst
You can set the screen copy mode to the memory-to-memory copy mode by specifying it in the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	gmode gmode_mem, img_width, img_height
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_sub
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_alpha
Semi-transparent composite copy mode

%inst
You can set the screen copy mode to semi-transparent composite copy mode by specifying it in the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color       : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255   : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color ,255  : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color ,,255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_alpha, img_width, img_height, 128
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_sub
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_pixela
%index
gmode_rgb0alpha
Translucent composite copy mode with transparent color

%inst
By specifying it as the first argument of gmode, you can set the screen copy mode to the translucent composite copy mode with transparent color.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	gsel 0
	color   0,   0,   0 : boxf img_width / 5, 0, img_width  * 2 / 5, ginfo_winy
	color 255,   0,   0 : boxf img_width * 2 / 5, 0, img_width * 3 / 5, ginfo_winy
	color   0, 255,   0 : boxf img_width * 3 / 5, 0, img_width * 4 / 5, ginfo_winy
	color   0,   0, 255 : boxf img_width * 4 / 5, 0, img_width, ginfo_winy
	gmode gmode_rgb0alpha, img_width, img_height, 128
	color 0, 0, 0
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_sub
gmode_pixela
%index
gmode_pixela
Pixel alpha blend copy mode

%inst
You can set the screen copy mode to pixel alpha blend copy mode by specifying it as the first argument of gmode.

%sample
	screen 1 : picload dir_exe + "/sample/demo/logop.bmp"
	img_width = ginfo_winx
	img_height = ginfo_winy
	screen 1, img_width * 2, img_height : picload dir_exe + "/sample/demo/logop.bmp", 1
	repeat img_width
		hsvcolor cnt * 192 / ( img_width  ), 255, 255
		line img_width + cnt, img_height, img_width + cnt, 0
	loop
	gsel 0
	gmode gmode_pixela, img_width, img_height, 128
	gcopy 1, 0, 0
	stop

%href
gmode
gmode_rgb0
gmode_mem
gmode_alpha
gmode_add
gmode_gdi
gmode_rgb0alpha
gmode_sub
%index
objinfo_mode
Get mode and option data

%prm
(p1)
p1 = 0 ~: Window object ID

%inst
Returns the mode and option data for the specified window object.

%sample
	button goto "sample", *dummy
	info = objinfo_mode( stat )
	mes "mode : " + ( info & 0xffff )
	mes "option : " + ( info >> 16 & 0xffff )

*dummy
	stop

%href
objinfo
objinfo_hwnd
objinfo_bmscr
%index
objinfo_bmscr
Get a pointer to the BMSCR structure where the object is located

%prm
(p1)
p1 = 0 ~: Window object ID

%inst
Returns a pointer to the BMSCR structure where the specified window object is located.

%sample
	button goto "sample", *dummy
	p_bmscr = objinfo_bmscr( stat )
mes "Pointer to the BMSCR structure where the object is located:" + p_bmscr
	mref bmscr, 67
mes "equivalent to the value obtained by mref (" + varptr (bmscr) + ")"

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_hwnd
%index
objinfo_hwnd
Get the handle of a window object

%prm
(p1)
p1 = 0 ~: Window object ID

%inst
Returns the handle of the specified window object.

%sample
	button goto "sample", *dummy
	obj_hwnd = objinfo_hwnd( stat )
mes "Window object handle:" + obj_hwnd

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_bmscr
%index
screen_normal
Create a regular window

%inst
You can create a normal window by specifying it in the 4th argument of the screen instruction.
You can omit it because the same result will be obtained.

%sample
// Create a regular window with window ID 0
	screen 0, 640, 480, screen_normal

// Same result can be obtained even if omitted
	screen 1, 320, 240
	stop

%href
screen
screen_palette
screen_hide
screen_fixedsize
screen_tool
screen_frame

%index
screen_palette
Create a window in palette mode

%inst
You can create a window in palette mode by specifying it in the 4th argument of the screen instruction.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Create a window with window ID 0 in palette mode
	screen 0, 640, 480, screen_palette
	stop

%href
screen
screen_normal
screen_hide
screen_fixedsize
screen_tool
screen_frame

%index
screen_hide
Create hidden window

%inst
You can create a hidden window by specifying it in the 4th argument of the screen instruction.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Create a hidden window with window ID 0
	screen 0, 640, 480, screen_hide
	stop

%href
screen
screen_normal
screen_palette
screen_fixedsize
screen_tool
screen_frame
%index
screen_fixedsize
Create fixed size window

%inst
You can create a fixed-size window by specifying it in the 4th argument of the screen instruction.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Create a window with window ID 0 with a fixed size
	screen 0, 640, 480, screen_fixedsize
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_tool
screen_frame
%index
screen_tool
Create tool window

%inst
You can create a tool window by specifying it in the 4th argument of the screen instruction.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Create a tool window with window ID 0
	screen 0, 640, 480, screen_tool
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_fixedsize
screen_frame

%index
screen_frame
Create a window with deep edges

%inst
You can create a window with a deep edge by specifying it in the 4th argument of the screen instruction.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Create a window with a deep edge of the window ID
	screen 0, 640, 480, screen_frame
	stop

%href
screen
screen_normal
screen_palette
screen_hide
screen_fixedsize
screen_tool

%index
font_normal
Font settings in normal style

%inst
By specifying it in the third argument of the font command, you can set the font in the normal style.
You can omit it because the same result will be obtained.

%sample
// Set size 12 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Same result can be obtained even if omitted
	font msgothic, 24
mes "MS Gothic of size 24 (normal style)"
	stop

%href
font
font_bold
font_italic
font_underline
font_strikeout
font_antialias
%index
font_bold
Bold font setting

%inst
You can set the bold font by specifying it in the third argument of the font command.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Set size 24 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Set size 24 and bold MS Gothic
	font msgothic, 24, font_bold
mes "MS Gothic (bold) of size 24"
	stop

%href
font
font_normal
font_italic
font_underline
font_strikeout
font_antialias
%index
font_italic
Font settings in italics

%inst
Italic font can be set by specifying it in the third argument of the font command.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Set size 24 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Set italic MSms Gothic at size 24
	font msgothic, 24, font_italic
mes "MS Gothic (italic) size 24"
	stop

%href
font
font_normal
font_bold
font_underline
font_strikeout
font_antialias
%index
font_underline
Underlined font settings

%inst
You can set the underlined font by specifying it in the third argument of the font command.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Set size 24 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Set underlined MS Gothic at size 24
	font msgothic, 24, font_underline
mes "MS Gothic of size 24 (underlined)"
	stop

%href
font
font_normal
font_bold
font_italic
font_strikeout
font_antialias
%index
font_strikeout
Font settings with strikethrough

%inst
You can set a font with strikethrough by specifying it in the third argument of the font command.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Set size 24 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Set MS Gothic with strikethrough at size 24
	font msgothic, 24, font_strikeout
mes "MS Gothic of size 24 (with strikethrough)"
	stop

%href
font
font_normal
font_bold
font_italic
font_underline
font_antialias
%index
font_antialias
Font setting with antialiasing

%inst
You can set the antialiasing font by specifying it in the third argument of the font command.
When specifying in combination with other macros, specify the sum or logical sum.

%sample
// Set size 24 MS Gothic
	font msgothic, 24, font_normal
mes "MS Gothic of size 24 (normal style)"

// Set antialiasing MS Gothic at size 24
	font msgothic, 24, font_antialias
mes "MS Gothic (antialiasing) of size 24"
	stop

%href
font
font_normal
font_bold
font_italic
font_underline
font_strikeout


%index
objmode_normal
Set HSP standard font

%inst
By specifying it in the first argument of the objmode instruction, the font used in the object control instruction can be set to the HSP standard font.
You can omit it because the same result will be obtained.

%sample
s = "Sample font used in object control instructions"

// Set the font used for object control instructions to HSP standard font
	objmode objmode_normal
	mesbox s, ginfo_winx, ginfo_winy / 2

// Same result can be obtained even if omitted
	objmode objmode_normal
	mesbox s, ginfo_winx, ginfo_winy / 2

	stop

%href
objmode
objmode_guifont
objmode_usefont
objmode_usecolor

%index
objmode_guifont
Set default GUI font

%inst
By specifying it in the first argument of the objmode instruction, the font used in the object control instruction can be set as the default GUI font.

%sample
s = "Sample font used in object control instructions"

// Set the font used in the object control instruction to the default GUI font
	objmode objmode_guifont
	mesbox s, ginfo_winx, ginfo_winy

	stop

%href
objmode
objmode_normal
objmode_usefont
objmode_usecolor


%index
objmode_usefont
Set the font selected by the font command

%inst
By specifying it in the first argument of the objmode instruction, the font used in the object control instruction can be set to the font selected by the font instruction.

%sample
s = "Sample font used in object control instructions"

// Set the font used in the object control instruction to the font selected in the font instruction
	objmode objmode_usefont

	font msmincho, 24
	mesbox s, ginfo_winx, ginfo_winy / 2

	font msgothic, 18, font_italic
	mesbox s, ginfo_winx, ginfo_winy / 2

	stop

%href
objmode
objmode_normal
objmode_guifont
objmode_usecolor


%index
objmode_usecolor
Set the color selected by the objcolor command

%inst
By specifying it in the first argument of the objmode instruction, the color used in the object control instruction can be set to the color specified in the color instruction and objcolor instruction.

%href
objmode
objmode_normal
objmode_guifont
objmode_usefont



%index
msgothic
MS gothic font

%group
System variables

%inst
A keyword that indicates the font that indicates MS Gothic.
It can be used as the font name specified by the font command.

%href
msmincho

;---------------------------------------------------------------------
%index
msmincho
MS Mincho font

%group
System variables

%inst
A keyword that indicates the font that indicates MS Mincho.
It can be used as the font name specified by the font command.

%href
msgothic

;---------------------------------------------------------------------
%index
and
Logical product (operator)

%group
Standard definition macro

%inst
A macro that can be used in the same way as the operator "&" that indicates the logical product.

%href
or
xor
not

;---------------------------------------------------------------------
%index
or
OR (operator)

%group
Standard definition macro

%inst
A macro that can be used in the same way as the operator "|" that indicates the logical sum.

%href
and
xor
not

;---------------------------------------------------------------------
%index
xor
Exclusive OR (operator)

%group
Standard definition macro

%inst
A macro that can be used in the same way as the operator "^" that indicates the exclusive OR.

%href
and
or
not

;---------------------------------------------------------------------
%index
not
Negation (operator)

%group
Standard definition macro

%inst
A macro that can be used in the same way as the negation operator "!".

%href
and
or
xor

;---------------------------------------------------------------------
%index
M_PI
Pi

%inst
A constant that represents pi. 3.14159265358979323846 is defined.

%group
Mathematical constant

%href
rad2deg
deg2rad


;---------------------------------------------------------------------
%index
rad2deg
Convert radians to degrees

%prm
(p1)
p1: Angle to convert to degrees (radians)

%inst
Converts the unit of angle from radians to degrees.
It can also be said to convert the angle expressed in the radian method to the angle in the degree method.

%sample
	tmp = M_PI
mes str (tmp) + "Radian is" + rad2deg (tmp) + "‘®."
	stop

%href
M_PI
deg2rad


;---------------------------------------------------------------------
%index
deg2rad
Convert degrees to radians

%prm
(p1)
p1: Angle to convert to radians (degrees)

%inst
Converts the unit of angle from degrees to radians.
It can also be said that the angle expressed in the degree method is converted into the angle in the radian method.

%sample
	tmp = 90
mes str (tmp) + "‘® is" + deg2rad (tmp) + "radians."
	stop

%href
M_PI
rad2deg


