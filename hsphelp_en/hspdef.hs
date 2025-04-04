%type
System-defined macro
%ver
3.7
%date
2025/01/22
%note
This macro is defined in hspdef.as.
%group
Standard Definition Macro
%author
onitama
%port
Win
Mac
Cli

%index
gmode_sub
Color Subtraction Composition Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to color subtraction composition copy mode.

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
Color Addition Composition Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to color addition composition copy mode.

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
Normal Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to the normal copy mode.
The same result can be obtained by omitting it, so you can omit it if you wish.

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
Transparent Color Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to transparent color copy mode.

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
Memory-to-Memory Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to memory-to-memory copy mode.

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
Semi-Transparent Composition Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to semi-transparent composition copy mode.

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
Transparent Color Semi-Transparent Composition Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to transparent color semi-transparent composition copy mode.

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
Pixel Alpha Blend Copy Mode

%inst
Specifying this for the first argument of the gmode command sets the screen copy mode to pixel alpha blend copy mode.

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
p1=0〜 : Window object ID

%inst
Returns the mode and option data of the specified window object.

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
Get the pointer to the BMSCR structure where the object is placed

%prm
(p1)
p1=0〜 : Window object ID

%inst
Returns the pointer to the BMSCR structure where the specified window object is placed.

%sample
	button goto "sample", *dummy
	p_bmscr = objinfo_bmscr( stat )
	mes "Pointer to the BMSCR structure where the object is placed : " + p_bmscr
	mref bmscr, 67
	mes "Equivalent to the value obtained by mref (" + varptr( bmscr ) + ")"

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_hwnd
%index
objinfo_hwnd
Get the handle of the window object

%prm
(p1)
p1=0〜 : Window object ID

%inst
Returns the handle of the specified window object.

%sample
	button goto "sample", *dummy
	obj_hwnd = objinfo_hwnd( stat )
	mes "Window object handle : " + obj_hwnd

*dummy
	stop

%href
objinfo
objinfo_mode
objinfo_bmscr
%index
screen_normal
Create a normal window

%inst
Specifying this for the 4th argument of the screen command creates a normal window.
The same result can be obtained by omitting it, so you can omit it if you wish.

%sample
// Create a normal window with window ID 0
	screen 0, 640, 480, screen_normal

// The same result can be obtained by omitting it
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
Specifying this for the 4th argument of the screen command creates a window in palette mode.
When specifying in combination with other macros, specify the sum or logical sum of them.

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
Create a hidden window

%inst
Specifying this for the 4th argument of the screen command creates a hidden window.
When specifying in combination with other macros, specify the sum or logical sum of them.

%sample
// Create a window with window ID 0 in hidden mode
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
Create a fixed-size window

%inst
Specifying this for the 4th argument of the screen command creates a fixed-size window.
When specifying in combination with other macros, specify the sum or logical sum of them.

%sample
// Create a window with window ID 0 in fixed size
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
Create a tool window

%inst
Specifying this for the 4th argument of the screen command creates a tool window.
When specifying in combination with other macros, specify the sum or logical sum of them.

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
Create a window with a deep edge

%inst
Specifying this for the 4th argument of the screen command creates a window with a deep edge.
When specifying in combination with other macros, specify the sum or logical sum of them.

%sample
// Create a window with a deep edge with window ID
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
Set font with normal style

%inst
Specifying this for the 3rd argument of the font command sets the font with a normal style.
The same result can be obtained by omitting it, so you can omit it if you wish.

%sample
// Set MS Gothic with size 12
	font msgothic, 24, font_normal
	mes "MS Gothic with size 24 (normal style)"

// The same result can be obtained by omitting it
	font msgothic, 24
	mes "MS Gothic with size 24 (normal style)"
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
Set font with bold style

%inst
Specifying this for the 3rd argument of the font command sets the font with a bold style.
When specifying in combination with other macros, specify the sum or logical sum of them.

%sample
// Set MS Gothic with size 24
	font msgothic, 24, font_normal
	mes "MS Gothic with size 24 (normal style)"

// Set bold MS Gothic with size 24
	font msgothic, 24, font_bold
	mes "MS Gothic with size 24 (bold)"
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
Set font with italic style

%inst
Specifying this for the 3rd argument of the font command sets the font with an italic style.
When specifying in combination with other macros, specify the sum or logical sum of them.

%sample
// Set MS Gothic with size 24
	font msgothic, 24, font_normal
mes "MS Gothic, size 24 (regular style)"

// Set MS Gothic, size 24, in italics
	font msgothic, 24, font_italic
	mes "MS Gothic, size 24 (italic)"
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
Set font with underline

%inst
You can set an underlined font by specifying it as the third argument of the font command. If you specify it in combination with other macros, specify the sum or logical OR of them.

%sample
// Set MS Gothic, size 24
	font msgothic, 24, font_normal
	mes "MS Gothic, size 24 (regular style)"

// Set underlined MS Gothic, size 24
	font msgothic, 24, font_underline
	mes "MS Gothic, size 24 (underlined)"
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
Set font with strikethrough

%inst
You can set a font with a strikethrough by specifying it as the third argument of the font command. If you specify it in combination with other macros, specify the sum or logical OR of them.

%sample
// Set MS Gothic, size 24
	font msgothic, 24, font_normal
	mes "MS Gothic, size 24 (regular style)"

// Set MS Gothic, size 24, with strikethrough
	font msgothic, 24, font_strikeout
	mes "MS Gothic, size 24 (strikethrough)"
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
Set font with anti-aliasing

%inst
You can set an anti-aliased font by specifying it as the third argument of the font command. If you specify it in combination with other macros, specify the sum or logical OR of them.

%sample
// Set MS Gothic, size 24
	font msgothic, 24, font_normal
	mes "MS Gothic, size 24 (regular style)"

// Set anti-aliased MS Gothic, size 24
	font msgothic, 24, font_antialias
	mes "MS Gothic, size 24 (anti-aliased)"
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
By specifying it as the first argument of the objmode command, you can set the font used by object control commands to the HSP standard font. You can omit it and get the same result.

%sample
	s = "Sample of font used in object control commands"

// Set the font used in object control commands to the HSP standard font
	objmode objmode_normal
	mesbox s, ginfo_winx, ginfo_winy / 2

// Omitting it yields the same result
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
By specifying it as the first argument of the objmode command, you can set the font used by object control commands to the default GUI font.

%sample
	s = "Sample of font used in object control commands"

// Set the font used in object control commands to the default GUI font
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
Set font selected by font command

%inst
By specifying it as the first argument of the objmode command, you can set the font used by object control commands to the font selected by the font command.

%sample
	s = "Sample of font used in object control commands"

// Set the font used in object control commands to the font selected by the font command
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
Set color selected by objcolor command

%inst
By specifying it as the first argument of the objmode command, you can set the color used by object control commands to the color specified by the color command or the objcolor command.

%href
objmode
objmode_normal
objmode_guifont
objmode_usefont



%index
msgothic
MS Gothic font

%group
System variable

%inst
This is a keyword indicating the MS Gothic font. It can be used as the font name specified in the font command.

%href
msmincho

;---------------------------------------------------------------------
%index
msmincho
MS Mincho font

%group
System variable

%inst
This is a keyword indicating the MS Mincho font. It can be used as the font name specified in the font command.

%href
msgothic

;---------------------------------------------------------------------
%index
and
Logical AND (operator)

%group
Standard defined macro

%inst
This is a macro that can be used in the same way as the operator "&" that indicates logical AND.

%href
or
xor
not

;---------------------------------------------------------------------
%index
or
Logical OR (operator)

%group
Standard defined macro

%inst
This is a macro that can be used in the same way as the operator "|" that indicates logical OR.

%href
and
xor
not

;---------------------------------------------------------------------
%index
xor
Exclusive OR (operator)

%group
Standard defined macro

%inst
This is a macro that can be used in the same way as the operator "^" that indicates exclusive OR.

%href
and
or
not

;---------------------------------------------------------------------
%index
not
Negation (operator)

%group
Standard defined macro

%inst
This is a macro that can be used in the same way as the operator "!" that indicates negation.

%href
and
or
xor

;---------------------------------------------------------------------
%index
M_PI
Pi

%inst
This is a constant representing pi. 3.14159265358979323846 is defined.

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
p1 : Angle to convert to degrees (radians)

%inst
Converts the unit of angle from radians to degrees. It can also be said that it converts an angle expressed in radians to an angle in degrees.

%sample
	tmp = M_PI
	mes str(tmp) + " radians is " + rad2deg(tmp) + "属."
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
p1 : Angle to convert to radians (degrees)

%inst
Converts the unit of angle from degrees to radians. It can also be said that it converts an angle expressed in degrees to an angle in radians.

%sample
	tmp = 90
	mes str(tmp) + "属 is " + deg2rad(tmp) + " radians."
	stop

%href
M_PI
rad2deg
