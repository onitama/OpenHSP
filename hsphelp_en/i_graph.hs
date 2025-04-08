;
;	HSP help manager用 HELPソースファイル
;	(先頭が;の行はコメントとして処理されます)
;
;	HELP source file for HSP help manager
;	(Lines starting with ; are treated as comments)
;

%type
Built-in command
%ver
3.7
%note
ver3.7 standard command
%date
2022/10/17
%author
onitama
%url
http://hsp.tv/
%port
Win



%index
cls
Clear screen
%group
Screen control commands
%prm
p1
p1=0〜4(0) : Color to clear with (0)

%inst
Clears all information in the window.
^
You can specify 5 types of colors to clear with using p1.
^p
Color values :
 ( 0=White / 1=Light Gray / 2=Gray / 3=Dark Gray / 4=Black )
^p
When the screen is cleared with the cls command, objects such as buttons and input boxes, as well as font and color settings, are reset to their initial state.
%port+
Let



%index
mes
Display message
%group
Screen control commands
%prm
"strings",sw
"strings" : Message or variable to display
sw(0)     : Option
%inst
Displays the specified message in the window.
The message is displayed from the current position, which is a virtual cursor position on the window. (The current position can be set with the pos command.)
The behavior when displaying a message can be specified by the option value.
^p
   Value  : Macro name       : Behavior
 -----------------------------------------------------
   1   : mesopt_nocr    : No newline at the end
   2   : mesopt_shadow  : Draw shadowed characters
   4   : mesopt_outline : Draw outlined characters
   8   : mesopt_light   : Draw simplified shadow/outline
   16  : mesopt_gmode   : Reflect gmode settings (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position is moved to the right of the displayed characters.
If this option is not specified, the current position is automatically moved to the next line.
If 2 or mesopt_shadow is specified in the option, the characters with shadow are drawn.
If 4 or mesopt_outline is specified in the option, the characters with outline are drawn.
The colors specified by the objcolor command are used for the shadow and outline.
The width of the shadow and outline can be specified with the font command.
If 16 or mesopt_gmode is specified in the option, drawing will reflect settings such as transparency and color addition specified in gmode.
(This option is only valid for HSP3Dish and HGIMG4. It is ignored by the normal HSP3 runtime.)
^
If the width of the shadow and outline is greater than 1, the processing load will increase.
If you want to reduce the load with simplified drawing, specify 8 or mesopt_light in the option.
^
Multiple option values can be specified simultaneously by adding them.
If "mesopt_nocr+mesopt_shadow" is specified, it will be displayed with shadow and no newline.
^
The font of the displayed message can be specified by the font command.
If the message to be displayed contains a line feed code, it will be line-fed and the display will continue from the next line.

%href
print
font
pos
objcolor
%port+
Let
%sample
	mes "The mes command displays a string"

	mes {"Multiline strings can also
be displayed"}

	font msmincho, 20, font_bold
	mes "The size of characters can be changed by the font command"

%index
print
Display message
%group
Screen control commands
%prm
"strings",sw
"strings" : Message or variable to display
sw(0)     : Option

%inst
Displays the specified message in the window.
The message is displayed from the current position, which is a virtual cursor position on the window. (The current position can be set with the pos command.)
The behavior when displaying a message can be specified by the option value.
^p
   Value  : Macro name       : Behavior
 -----------------------------------------------------
   1   : mesopt_nocr    : No newline at the end
   2   : mesopt_shadow  : Draw shadowed characters
   4   : mesopt_outline : Draw outlined characters
   8   : mesopt_light   : Draw simplified shadow/outline
   16  : mesopt_gmode   : Reflect gmode settings (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position is moved to the right of the displayed characters.
If this option is not specified, the current position is automatically moved to the next line.
If 2 or mesopt_shadow is specified in the option, the characters with shadow are drawn.
If 4 or mesopt_outline is specified in the option, the characters with outline are drawn.
The colors specified by the objcolor command are used for the shadow and outline.
The width of the shadow and outline can be specified with the font command.
If 16 or mesopt_gmode is specified in the option, drawing will reflect settings such as transparency and color addition specified in gmode.
(This option is only valid for HSP3Dish and HGIMG4. It is ignored by the normal HSP3 runtime.)
^
If the width of the shadow and outline is greater than 1, the processing load will increase.
If you want to reduce the load with simplified drawing, specify 8 or mesopt_light in the option.
^
Multiple option values can be specified simultaneously by adding them.
If "mesopt_nocr+mesopt_shadow" is specified, it will be displayed with shadow and no newline.
^
The font of the displayed message can be specified by the font command.
If the message to be displayed contains a line feed code, it will be line-fed and the display will continue from the next line.
^
The print command is an alias for the mes command, and both have the same meaning.
In HSP, it is recommended to use the mes command.

%href
mes
font
pos
objcolor
%port+
Let



%index
title
Set title bar
%group
Screen control commands
%prm
"strings"
"strings" : String to specify

%inst
Sets the title bar caption of the window to the contents of "strings".
(The title bar caption is usually the part that says "Hot Soup Processor ver3.x".)

%href
objprm
%port+
Let
%portinfo
In the case of HSPLet applet, the text in the status bar changes.

%sample
	// Display the text "Window Title" in the window's title bar
	title "Window Title"

%index
dialog
Open dialog
%group
Screen control commands
%prm
"message",p1,"option"
p1=0〜(0) : Dialog type setting

%inst
Displays various standard Windows dialog boxes on the screen.
Dialogs displayed by the dialog command appear as pop-ups separate from the HSP window.
^
The HSP script will not continue execution until the dialog is closed by the user's operation.
^
The type of dialog changes depending on the setting of p1.
^p
   Type : Content
 -----------------------------------------------------
     0    : Standard message box + [OK] button
     1    : Warning message box + [OK] button
     2    : Standard message box + [Yes] [No] buttons
     3    : Warning message box + [Yes] [No] buttons
    16    : File OPEN dialog
    17    : File SAVE dialog
    32    : Color selection dialog (fixed colors)
    33    : Color selection dialog (RGB can be freely selected)
    64〜  : Extended dialog
^p
For types 0 to 3, a message box displaying the contents specified by "message" will appear. You can also specify the title bar string of the message box with "option". (If omitted, nothing will be displayed in the title.)
When the message box is closed, the information of the button pressed is substituted into the system variable stat. At this time, the contents of the variable stat are,
^p
  1 : "OK" button was pressed
  6 : "Yes" button was pressed
  7 : "No" button was pressed
^p
will be returned.
^
For types 16 to 17, a file selection dialog for selecting from a list of files will appear. At this time, you can specify the extension (up to 3 characters) of the file to load (or save) in "message" so that only that extension is displayed. (Specify "*" to display all files)
Also, with "option", you can add auxiliary explanations of the extension, such as "Text file" for "txt", which will be displayed in the "File type" in the dialog.
"option" can also be omitted.
^p
Example:
	dialog "txt",16,"Text file"
	dialog "STAT="+stat+"\nNAME="+refstr,0,"Result"
	stop
^p
When file selection is complete, the result is substituted into the system variable stat.
If the variable stat is 1, it means that it was selected successfully. If the variable stat is 0, it means that it was canceled or an error occurred.
Multiple file types can be specified by separating them with the "|" symbol.
^p
Example:
	dialog "txt|log",16,"Text file|Log file"
	dialog "STAT="+stat+"\nNAME="+refstr,0,"Result"
	stop
^p
For types 32 to 33, a color selection dialog is displayed. When the color selection is complete, the RGB data of the selected color is substituted into the system variables ginfo_r, ginfo_g, ginfo_b. Also, if the variable stat is 0, it means that it was canceled or an error occurred. 1 means that it was selected successfully.
^p
Example:
	dialog "",33
	dialog "R="+ginfo_r+"/G="+ginfo_g+"/B="+ginfo_b,0,"Result"
	stop
^p
Values of type 64 and above are reserved for extended dialogs prepared for each runtime. If an unsupported type is specified, nothing is executed.
%port+
Let



%index
bgscr
Initialize a window without a frame
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜(0)  : Window ID
p2=1〜(640): Initial screen size X (in dots)
p3=1〜(480): Initial screen size Y (in dots)
p4=0〜(0) : Initial screen mode
p5=0〜(-1) : Window placement X (in dots)
p6=0〜(-1) : Window placement Y (in dots)
p7=0〜     : Window size X (in dots)
p8=0〜     : Window size Y (in dots)

%inst
Initializes the window ID and creates a frameless window.
This window is used for special purposes such as creating screen savers.
The initial screen size is specified by the p2 and p3 parameters.
The initial screen mode for p4 can be selected from the following.
^p
   1 : Create in palette mode (256 colors out of 16.7 million colors)
   2 : Create a hidden window
 256 : Full screen (HSP3Dish/HGIMG4 only)
^p
If "+1" (create in palette mode) is specified for the p4 parameter, the screen will be created in a 256-color mode with a palette. If this mode is not specified, the screen will be created in full color (24-bit color).
^
If "+2" (create a hidden window) is specified for the p4 parameter, the window will not be displayed at runtime, but can be displayed with "gsel ID,1".
^
The p5 and p6 parameters allow you to specify the position of the window on the desktop screen.
If p5 and p6 are omitted or set to a negative value, the system's default coordinates will be used.
If you want to display only a portion of the screen size specified by the p2 and p3 parameters in the window,
you can specify the client size of the window (the actual size displayed) with the p7 and p8 parameters.
In that case, you can set the position displayed within the window using the groll command.
Normally, the p7 and p8 parameters can be omitted. In that case, the display will be the same client size as the screen size specified by p2 and p3.
^
After initializing the screen with the bgscr command, the window ID becomes the drawing destination for screen control commands.

%href
screen
buffer
groll
screen_hide
screen_normal
screen_palette
%port+
Let
%portinfo
Palette mode cannot be used when using HSPLet.
%sample
	mes "Close this window to exit the program"

	// Create a window without a frame
	bgscr 2, 320, 240
	boxf

%index
bmpsave
Save screen image
%group
Screen control commands
%prm
"filename"
"filename" : Filename to save

%inst
Saves the current screen image as a BMP format image file.
A file is created with the name specified by "filename". You must specify the filename including the extension.
^
The size of the saved image is the size at which the target window was initialized. If you want to change the size, copy the contents to a window of a different size first, and then bmpsave.

%index
boxf
Fill rectangle
%group
Screen control commands
%prm
p1,p2,p3,p4
p1=0〜(0)  : X coordinate of the upper left corner of the rectangle
p2=0〜(0)  : Y coordinate of the upper left corner of the rectangle
p3=0〜     : X coordinate of the lower right corner of the rectangle
p4=0〜     : Y coordinate of the lower right corner of the rectangle

%inst
Fills a rectangle (quadrangle) on the screen with the current drawing color, using (p1, p2) and (p3, p4) as the upper left and lower right points.
If (p1, p2) is omitted, the upper left corner of the screen (0,0) is set.
If (p3, p4) is omitted, the lower right corner of the screen (full drawing size) is set.

%href
color
hsvcolor
rgbcolor
palcolor
line
circle
gradf
%port+
Let
%sample
	// Fill the entire area if all parameters are omitted
	boxf

	color 255, 128, 0
	boxf 80, 80, 150, 130

%index
buffer
Initialize virtual screen
%group
Screen control commands
%prm
p1,p2,p3,p4
p1=0〜 (0) : Window ID
p2=1〜(640): Screen size X to initialize (in dots)
p3=1〜(480): Screen size Y to initialize (in dots)
p4=0〜(0) : Screen mode to initialize

%inst
Like the screen command, initializes the specified window ID so that it can be used. Like the screen command, it specifies the screen size to initialize, but with the buffer command, only a virtual screen is created in memory, and it is not displayed on the actual screen.
The virtual screen created in this way can use the print command or picload command in the same way as a normal screen. It can be used as a storage location for copying to another screen using the gcopy command, or as a storage location for temporarily leaving the current screen.
^
You can specify the following values for the screen mode of the p4 parameter.
^p
  Screen mode    Macro name             Content
---------------------------------------------------------------------------------------------------
   0            screen_normal        Initialize in full color mode (24-bit color) (default)
   1            screen_palette       Initialize in palette mode (256 colors out of 16.7 million colors) (Windows version only)
   32           screen_offscreen     Initialize as drawing destination (HSP3Dish/HGIMG4 only)
   64           screen_usergcopy     Assign drawing shader (HGIMG4 only)
^p
With HSP3 for Windows, it is possible to initialize in palette mode (256 colors out of 16.7 million colors).

With HSP3Dish and HGIMG4 runtimes, it is possible to specify screen_offscreen.
In HSP3Dish, the celbitmap command allows you to access the drawing destination buffer as array data.
In HGIMG4, it is initialized as a drawing destination to which normal drawing commands are applied.
Without this option, you cannot use drawing commands when using a virtual screen on the HSP3Dish and HGIMG4 runtimes (you can load images).
^
When using the HGIMG4 runtime, you can apply your own shader by specifying screen_usergcopy. See the HGIMG4 Programming Guide for more information.
^
Please note that you cannot re-initialize a window ID initialized with the screen command with the buffer command.
After initializing the screen with the buffer command, the window ID becomes the drawing destination for screen control commands.

%href
screen
bgscr
screen_normal
screen_palette
celbitmap
%port+
Let
%portinfo
Palette mode cannot be used when using HSPLet.

%index
chgdisp
Change image resolution
%group
Screen control commands
%prm
p1,p2,p3
p1=0〜2(0)  : Mode setting
p2=0〜(640) : X direction image resolution setting
p3=0〜(480) : Y direction image resolution setting

%inst
Forcibly changes the current display resolution.
Sets the X and Y resolutions with (p2, p3), and sets the change mode with p1.
The modes are 1 for full color mode (32-bit) and 2 for palette mode (8-bit). If the change mode is 0, it returns to the initial state (before the change).
^p
Example :
	chgdisp 1,640,480	; Set to 640x480 full color
^p
If the (p2, p3) parameters are omitted, (640,480) will be used.
^
When the chgdisp command is executed, the result is reflected in the system variable stat.
The contents of the system variable stat are as follows.
^p
  stat=0 : The resolution was set successfully.
  stat=1 : The color mode could not be changed,
           but the resolution was changed successfully.
  stat=2 : The resolution could not be changed.
^p

%index
color
Color setting
%group
Screen control commands
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : Color code (R, G, B brightness)

%inst
Sets the color for message display, drawing, etc. to the specified value.
p1, p2, and p3 are the brightness of R, G, and B, respectively.
^
0 is the darkest and 255 is the brightest.
color 0,0,0 is black and color 255,255,255 is white.
In palette mode screens, the closest palette to the specified color is selected.

%href
palcolor
hsvcolor
rgbcolor
%port+
Let

%index
font
Font settings
%group
Screen control commands
%prm
"fontname",p1,p2,p3
"fontname"   : Font name
p1=1〜(12)   : Font size
p2=0〜(0)    : Font style
p3=1〜(1)    : Font decoration width

%inst
Sets the text font for display with the mes and print commands.
Specify the font name with "fontname".
^
For the font name, specify the font name directly, such as "MS Mincho" or "MS Gothic".
The font size is specified in logical size proportional to the number of dots.
The larger the number, the larger the characters. The font styles are:
^p
  Style 1  : Bold
  Style 2  : Italic
  Style 4  : Underline
  Style 8  : Strikethrough
  Style 16  : Anti-aliasing
^p
Multiple styles can be specified at the same time by adding the numbers together.
If p2 is omitted, 0 (normal style) is used.
Specifying style 16 enables anti-aliasing for characters. (This behavior differs depending on the OS environment.)
^
The p3 parameter specifies the width used when displaying decorated fonts such as shadowed or bordered fonts. If the parameter is omitted, the default value (1) is used.
Font decoration can be set with the option value of the mes or print command.
^
To return to the initial settings or system-specified fonts, use the sysfont command. To change the font of an object, you need to specify the mode with the objmode command.
^
If the specified font is not found, a substitute font is automatically searched for. If the search for a substitute font also fails, -1 is assigned to the system variable stat. If the font is set successfully, 0 is assigned to the system variable stat.

%href
sysfont
objmode
font_antialias
font_bold
font_italic
font_normal
font_strikeout
font_underline
%port+
Let

%index
gcopy
Screen copy
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1=0〜(0) : Window ID
p2=0〜(0) : X coordinate of the upper left corner of the copy source
p3=0〜(0) : Y coordinate of the upper left corner of the copy source
p4=0〜    : Size X to copy (in dots)
p5=0〜    : Size Y to copy (in dots)

%inst
The gcopy command copies a part of the screen of the specified window ID to the current drawing destination current position.
The window ID of the copy source must be the same as your window ID or a window ID initialized with the screen or buffer command.
^
When copying with the gcopy command, you can select one of several copy modes with the gmode command.
^
Care must be taken when copying images from a palette mode screen with the gcopy command.
It is necessary to prepare images where there are no problems even if the palettes of the two windows to be copied are the same.
Also, be sure to set the correct palette on the user side when in palette mode. (The palette is not set by the picload command alone when using image files.)

%href
gmode
gzoom
%port+
Let

%index
gmode
Screen copy mode setting
%group
Screen control commands
%prm
p1,p2,p3,p4
p1=0〜6(0)   : Screen copy mode
p2=0〜(32)   : Size X to copy (in dots)
p3=0〜(32)   : Size Y to copy (in dots)
p4=0〜256(0) : Blend ratio during translucent compositing
%inst
Sets the screen copy mode.
%index
gcopy
Modes, sizes, blend rates, etc. used by gcopy can be set.
The copy mode operations are as follows:
^
・Mode 0 = Normal Copy
^p
Performs image copying using the Windows system API (BitBlt function).
This is the most standard mode.
In many cases, it processes faster than other modes.
^p
・Mode 1 = Memory-to-Memory Copy
^p
Performs memory copy between image buffers.
The copy source and copy destination must be in the same image mode (palette mode or full-color mode).
The operation is the same as Mode 0, but copying very small sizes (16*16 dots or less, etc.) may be slightly faster than Mode 0.
^p
・Mode 2 = Transparent Color Copy
^p
Copies in the same way as Mode 1, but dots with all RGB values equal to 0 (pure black) are treated as transparent and are not copied.
^p
・Mode 3 = Semitransparent Composite Copy
^p
Can only be used in full-color mode.
Performs semitransparent composite copying using the blend rate specified in the fourth parameter of gmode. The blend rate is a value from 0 to 256, with 256 being a complete copy.
If the blend rate is 0, copying is not performed.
If the blend rate is 256 or higher, memory-to-memory copying is performed.
^p
・Mode 4 = Transparent Color Semitransparent Composite Copy
^p
Copies in the same way as Mode 3, but dots with RGB values equal to the color set by the color command are treated as transparent and are not copied.
If the blend rate is 0, copying is not performed.
^p
・Mode 5 = Color Addition Composite Copy
^p
Can only be used in full-color mode.
Performs color addition composite copying using the blend rate specified in the fourth parameter of gmode. In color addition, the RGB values of the copy source are added to the RGB values of the copy destination.
If the luminance exceeds 255 during addition, it is clamped to 255.
^p
・Mode 6 = Color Subtraction Composite Copy
^p
Can only be used in full-color mode.
Performs color subtraction composite copying using the blend rate specified in the fourth parameter of gmode. In color subtraction, the RGB values of the copy source are subtracted from the RGB values of the copy destination. If the luminance falls below 0 during subtraction, it is clamped to 0.
^p
・Mode 7 = Pixel Alpha Blend Copy
^p
Can only be used in full-color mode.
Prepares an image of the size specified by gmode to the right of the original image, and performs background composite copying using the right image as the alpha blend component.
If the alpha blend component is 255, the pixels of the original image are copied as is. If it is less than 255, it is composited with the background at that blend rate.
Because pixels are referenced in RGB units, different blend rates can be set for RGB.
The alpha blend image should usually be expressed in grayscale from (0,0,0) to (255,255,255).
^p
These modes are not used by the gzoom command.
Please note that this applies only when using the gcopy command.
Also, gmode changes the mode of the window ID currently selected by the gsel command. Note that even if you execute the gcopy command with a different window ID after setting gmode, the setting will not be reflected.
^
(p2, p3) sets the default copy size. This value is the default size used when the size value to be copied is omitted in the gcopy and gzoom commands.

%href
gcopy
gmode_add
gmode_alpha
gmode_gdi
gmode_mem
gmode_pixela
gmode_rgb0
gmode_rgb0alpha
gmode_sub
%port+
Let
%portinfo
In HSPLet, Mode 7 is not supported.

%index
gsel
Specify drawing destination, window to the front, hide settings
%group
Screen control command
%prm
p1,p2
p1=0〜(0)  : Window ID
p2=0〜2(0) : Window active switch

%inst
Changes the drawing destination to the screen of the specified window ID.
From now on, screen drawing commands such as mes command and picload command will be executed for the screen of the window ID specified by p1.
^
You can specify an option switch in p2. The value of p2 is,
^p
  -1 : Hides the specified window
   0 : No particular effect on the specified window
   1 : The specified window becomes active
   2 : The specified window becomes active and always on top
^p
You can change the window state like this.
If you omit the specification of p2, there will be no particular effect.
^
If you hide it with p2 = -1, the specified window disappears, but it is not completely deleted. It will return when activated again with p2 = 1 or 2.
%port+
Let
%portinfo
In HSPLet, mode 2 is not supported, and mode 1 does not necessarily become active.

%index
gzoom
Scale and copy the screen
%group
Screen control command
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜     : Size X (in dots) when copying to the screen
p2=0〜     : Size Y (in dots) when copying to the screen
p3=0〜(0)  : Window ID
p4=0〜(0)  : X coordinate of the upper left of the copy source
p5=0〜(0)  : Y coordinate of the upper left of the copy source
p6=0〜     : Size X to copy (in dots)
p7=0〜     : Size Y to copy (in dots)
p8=0〜1(0) : Zoom mode

%inst
The gzoom command copies a part of the screen of the specified window ID to the current drawing destination current position, scaling it to an arbitrary size.
The window ID of the copy source must be the same as your window ID or a window ID initialized with the screen command or buffer command.
^
You can specify the zoom mode with p8.
If you specify 1 for p8, a high-quality image using halftone is generated during scaling. (Does not work on Windows 9X.)
^
If p8 is 0 or omitted, halftone is not used, but image processing can be performed faster.

%href
gcopy
%port+
Let

%index
palcolor
Drawing palette setting
%group
Screen control command
%prm
p1
p1=0〜255(0) : Palette code

%inst
Sets the color of message display, drawing, etc. to the specified value.
p1 is the palette code. The palette code indicates the number of the 256 colors (the colors set by the palette command) prepared for the palette mode screen.
Normally, it is not necessary to use this as long as you are using full-color mode.

%href
color
hsvcolor
rgbcolor
palette

%index
palette
Palette setting
%group
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0〜255(0)       : Palette code
p2,p3,p4=0〜255(0) : Color code (R, G, B luminance)
p5=0〜1(0)         : Update switch

%inst
Changes the color data of the palette set on the current screen.
Sets the RGB color with the luminance specified by p2, p3, p4 to the palette code of p1.
If the value of p1 is negative, the setting is ignored.
You can specify the update on the screen depending on the value of p5.
When p5 is 0 or omitted, the value is set in the palette, but it is not immediately reflected on the screen.
When p5 is 1, the entire set palette is reflected on the screen.
Since it takes time to reflect on the screen, if you want to update the palettes together, be sure to set the update switch to 1 last.

%href
palcolor

%index
pget
Get 1 dot point
%group
Screen control command
%prm
p1,p2
p1=0〜 : X coordinate on the screen
p2=0〜 : Y coordinate on the screen

%inst
Reads the color data of 1 dot on the screen.
^
The color data of the dot at the coordinates specified by p1 and p2 is set as the selected color (the same state as specified by the color command).
If the specification of p1 and p2 is omitted, the coordinates of the current position set by the pos command are used.
The acquired color information can be referred to by the ginfo function or the ginfo_r, ginfo_g, ginfo_b macro.

%href
ginfo
ginfo_r
ginfo_g
ginfo_b
%port+
Let

%index
picload
Load image file
%group
Screen control command
%prm
"filename",p1
"filename" : File name to load
p1=0〜1(0) : Image load mode

%inst
Loads an image file. Usually,
^p
picload "test.bmp"
^p
By doing so, the image file specified by "test.bmp" is loaded into the current screen, and the window size is automatically adjusted to the image size.
^
The image load mode of p1 is,
^p
  Mode 0: Initialize the window and load
  Mode 1: Load on the current screen
  Mode 2: Initialize the window with black and load
^p
By omitting the load mode of p1 or setting it to 0, it is loaded into a window initialized with the image size.
^
By specifying load mode 1 for p1, you can load from the specified coordinates without changing the size of the current window. In this case, the coordinates specified by the pos command are the upper left.
^
The file formats that can be loaded currently are as follows.
^p
BMP format: Extension BMP: Windows standard 4, 8, 24bit data.
                        RLE compressed data can also be handled.
GIF format: Extension GIF: GIF format data.
                        Animation formats cannot be handled.
JPEG format: Extension JPG: JFIF standard JPEG data.
                        Grayscale data is also OK.
ICO format: Extension ICO: Windows standard ICO format icon data.
PNG format: Extension PNG: Portable Network Graphics data.
PSD format: Extension PSD: Photoshop format data. (Synthesis layer only)
TGA format: Extension TGA: TARGA format data.
^p
The picload command can be executed for screens initialized with the screen, buffer, and bgscr commands.
* Multi-icon format ICO files are not supported by default.
* Interlaced PNG files are not supported by default. To load all formats supported by PNG, please use the imgload or hspcv plugin.

%href
screen
buffer
bgscr
imgload
%port+
Let
%portinfo
HSPLet supports BMP, JPEG, GIF and PNG loading. Other loadings are not supported.

%index
pos
Current position setting
%group
Screen control command
%prm
p1,p2
p1=0〜     : X coordinate of the current position
p2=0〜     : Y coordinate of the current position
%inst
Specifies the coordinates of the current position, which is the base coordinate for displaying messages, objects, etc.
^
X is 0 at the leftmost position, and Y is 0 at the top position, with specification in 1-dot units.
If parameters are omitted, the current values are used.

%href
mes
gcopy
line
%port+
Let



%index
pset
Displays a 1-dot point
%group
Screen control command
%prm
p1,p2
p1=0〜 : X coordinate on the screen
p2=0〜 : Y coordinate on the screen

%inst
Draws a 1-dot point on the screen with the currently set drawing color.
If p1 and p2 are omitted, the coordinates of the current position set by the pos command are used.

%href
pget
color
palcolor
hsvcolor
%port+
Let





%index
redraw
Redraw setting
%group
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0〜3(1) : Drawing mode setting
p2=0〜(0)  : X coordinate of the upper left corner to be redrawn
p3=0〜(0)  : Y coordinate of the upper left corner to be redrawn
p4=0〜(0)  : Size X (in dots) to be redrawn
p5=0〜(0)  : Size Y (in dots) to be redrawn

%inst
Specifies the drawing mode of the screen. The drawing mode is:
^
Drawing mode 0:
^p
Even if screen control commands such as mes, print, gcopy, and gzoom are executed, only the virtual screen is rewritten, and it is not reflected on the actual screen.
^p
Drawing mode 1:
^p
When a screen control command is executed, it is also reflected on the actual screen.
^p
Normally, the drawing mode is 1.
By copying images or displaying messages in drawing mode 0 and then setting to mode 1 at the end, it is possible to eliminate flickering when rewriting the screen and make it look smoother.
^
If you specify mode 1, the screen is updated immediately.
^
If you specify a value that is the mode value plus 2, only the drawing mode is changed, and the screen is not updated.
^
Also, you can redraw only a part of the screen using the parameters p2 to p5. Normally, if omitted, the entire screen is updated.
%port+
Let
%sample
// Sample of redraw command
// You can confirm flickering by deleting the redraw command

*main_loop
	redraw 0

	color 255, 255, 255
	boxf

	pos 0, 0
	color 0, 0, 0
	mes "Characters do not flicker even if drawing and erasing are repeated"

	redraw 1

	wait 1
	goto *main_loop


%index
screen
Initialize window
%group
Screen control command
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜(0)  : Window ID
p2=1〜(640): Screen size X to initialize (in 1-dot units)
p3=1〜(480): Screen size Y to initialize (in 1-dot units)
p4=0〜(0)  : Screen mode to initialize
p5=0〜(-1) : Window placement X (in 1-dot units)
p6=0〜(-1) : Window placement Y (in 1-dot units)
p7=0〜     : Window size X (in 1-dot units)
p8=0〜     : Window size Y (in 1-dot units)
%inst
Initializes the specified window ID and makes it available.
In the initial state of HSP, only the screen with window ID 0 is used, but by specifying window ID 1 or higher, you can create a new window.
Windows with ID 1 or higher have a style that allows you to freely change the size by default.
^
The screen mode to initialize for p4 can be selected from the following:
^p
   0(screen_normal)    : Create in full-color mode
  +1(screen_palette)   : Create in palette mode (256 colors out of 16.7 million colors)
  +2(screen_hide)      : Create a hidden window
  +4(screen_fixedsize) : Fixed-size window
  +8(screen_tool)      : Tool window
 +16(screen_frame)     : Window with a deep frame
 +256                  : Make it full screen (HSP3Dish/HGIMG4 only)
^p
If you specify "+1" (create in palette mode) for the p4 parameter, the screen will be created in a palette-based 256-color mode. If you specify 0, a full-color (24-bit color) screen will be created.
^
If you specify "+2" (create a hidden window) for the p4 parameter, the window will not be displayed at runtime, but it can be displayed with "gsel ID,1".
^
The parameters "+4", "+8", and "+16" among the p4 parameters are for setting the window style.
A "fixed-size window" is created with a style in which the size cannot be changed, similar to the window with ID 0, even for windows with ID 1 or higher.
A "tool window" is created with a smaller title bar size and the window name is not displayed in the taskbar.
^
If the p4 parameter is omitted, it is created in palette mode if the current Windows screen mode is palette-based (256 colors), otherwise it is created as a full-color screen.
^
You can set multiple settings by adding the values of the screen mode.
For example,
^p
	screen 2,320,240,4+8
^p
creates an object with a fixed size + tool window style.
^
The screen command can also re-initialize a window ID that has already been initialized with different settings.
^p
Example:
	screen 0,640,480,1
^p
The above example re-initializes the window ID 0, that is, the main window, to 640x480 dots, palette mode.
^
You can specify the position of the window displayed on the desktop screen using the p5 and p6 parameters.
If p5 and p6 are omitted or set to negative values, the system-defined coordinates are used.
If you want to display only a part of the screen size specified by the p2 and p3 parameters in the window,
You can specify the client size of the window (the size that is actually displayed) using the p7 and p8 parameters.
In that case, you will be able to set the position displayed within the window using the groll command.
Normally, you can omit the specification of the p7 and p8 parameters. In that case, it will be displayed with the same client size as the screen size specified by p2 and p3.
^
After initializing the screen with the screen command, the window ID becomes the drawing destination for screen control commands.
^
Note that on HSP3Dish and HGIMG4 runtimes, all image buffers loaded so far are lost when the screen is initialized with the screen command.

%href
bgscr
buffer
groll
screen_fixedsize
screen_frame
screen_hide
screen_normal
screen_palette
screen_tool
%port+
Let
%portinfo
In HSPLet, palette mode, tool window, and window with a deep frame cannot be used.



%index
width
Window size setting
%group
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0〜(-1) : Client area size X (in 1-dot units)
p2=0〜(-1) : Client area size Y (in 1-dot units)
p3=0〜(-1) : Window X coordinate on the display (in 1-dot units)
p4=0〜(-1) : Window Y coordinate on the display (in 1-dot units)
p5=0〜1(0) : Window coordinate setting option
%inst
Changes the client size (the size that is actually displayed) of the window and the window display position.
The client size cannot be larger than the screen size initialized with the screen command, buffer, or bgscr command.
^
If the p1 and p2 parameters are omitted or are negative values, the current settings are used.
You can set the window coordinates on the display using the p3 and p4 parameters.
If the value of p5 is the default value or 0, the current settings are used when the p3 and p4 parameters are negative values.
If the value of p5 is 1, the coordinates are set even if the p3 and p4 parameters are negative values. This is used when it is necessary to handle negative coordinate values in a multi-monitor environment, etc.
%href
screen
buffer
bgscr
%port+
Let



%index
sysfont
Select system font
%group
Screen control command
%prm
p1
p1=0〜 : Font type specification

%inst
Select the standard system font.
Specify the font type with p1. Select this from the following:
^p
   p1 : font set
 --------------------------------------------------------------------
    0 : HSP standard system font
   10 : Fixed-width font with OEM character set
   11 : Fixed-width system font with Windows character set
   12 : Variable-width system font with Windows character set
   13 : Standard system font
   17 : Default GUI font
        (Used for menus and dialog boxes in a standard environment)
^p
If p1 is omitted, the standard system font is selected.


%href
font
%port+
Let





%index
line
Draw a straight line
%group
Screen control command
%prm
p1,p2,p3,p4
p1=0〜(0)  : X coordinate of the end point of the line
p2=0〜(0)  : Y coordinate of the end point of the line
p3=0〜     : X coordinate of the starting point of the line
p4=0〜     : Y coordinate of the starting point of the line

%inst
Draws a straight line connecting (p1, p2) and (p3, p4) on the screen.
If (p3, p4) is omitted, a straight line is drawn from the current position to (p1, p2) in the currently set color.
^
After executing the line command, the coordinates of (p1, p2) become the current position.
This makes it possible to draw continuous straight lines.

%href
pos
color
palcolor
boxf
circle
%port+
Let



%index
circle
Draw a circle
%group
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0〜(0)  : X coordinate of the upper left corner of the rectangle
p2=0〜(0)  : Y coordinate of the upper left corner of the rectangle
p3=0〜     : X coordinate of the lower right corner of the rectangle
p4=0〜     : Y coordinate of the lower right corner of the rectangle
p5=0〜1(1) : Drawing mode (0=line,1=fill)

%inst
Draws a circle on the screen that fits within a rectangle with (p1, p2) and (p3, p4) as the upper left and lower right points.
The drawing color is the one set by the color command, etc.
^
If 0 is specified for p5, only the outline is drawn with a line.
If p5 is 1 or omitted, the inside of the circle is also filled.

%href
boxf
line
color
palcolor
hsvcolor
%port+
Let



%index
syscolor
Set system color
%group
Screen control command
%prm
p1
p1 : System color index to set

%inst
Sets the system color index specified by p1 as the selected color.
(The same state as specified by the color command)
The system color index that can be specified by p1 is the following value.
^p
 0 : Scroll bar axis color
 1 : Desktop color
 2 : Active window title bar color (left color of the active window title bar in the case of a gradient)
 3 : Inactive window title bar text color (left color of the inactive window title bar in the case of a gradient)
 4 : Menu background color
 5 : Window background color
 6 : Window frame color
 7 : Text color in the menu
 8 : Text color in the window
 9 : Active window title bar text color
10 : Active window border color
11 : Inactive window border color
12 : MDI application background color
13 : Color of the selected item in the control
14 : Text color of the selected item in the control
15 : Surface color of 3D object
16 : Shadow color of 3D object (for the edge opposite the light source)
17 : Light color (invalid state) text color
18 : Push button text color
19 : Inactive window title bar text color
20 : Brightest color of 3D object (for the edge in the direction of the light source)
21 : Dark shadow color of 3D object
22 : Bright color of 3D object (for the edge in the direction of the light source)
23 : Tooltip control text color
24 : Tooltip control background color
26 : Hot track item color
27 : Right side color of the active window title bar in case of gradient
28 : Right side color of the inactive window title bar in case of gradient
29 : (Windows XP or later) The color used to highlight menu items when the menu is displayed as a flat menu
30 : (Windows XP or later) The background color of the menu bar when the menu is displayed as a flat menu
^p
%href
color
palcolor
hsvcolor
%port+
Let

%index
hsvcolor
Sets the color in HSV format
%group
Screen control commands
%prm
p1,p2,p3
p1=0〜191(0) : HSV format H value
p2=0〜255(0) : HSV format S value
p3=0〜255(0) : HSV format V value

%inst
Specifies the color in HSV format.
Like the color command, the set color will be used for subsequent drawing commands.

%href
color
palcolor
%port+
Let


%index
rgbcolor
Sets the color in RGB format
%group
Screen control commands
%prm
p1
p1=$000000〜$ffffff(0) : RGB format color code value

%inst
Specifies the color in RGB format ($rrggbb).
Specify the value obtained by converting the brightness of each of RGB to an 8-bit value in p1.
You can use "$" to specify hexadecimal numbers, such as "$112233".
In this case, "R=$11, G=$22, B=$33" will be specified.
The function of the command is equivalent to the color command and the hsvcolor command, and the set color is used in subsequent drawing commands.
The RGB format color code value specified by rgbcolor is compatible with the 24-bit color code specified in html, etc.

%href
color
palcolor
hsvcolor



%index
ginfo
Get window information
%group
Basic input/output functions
%prm
(p1)
p1=0〜 : Type to get

%inst
Returns the window-related information value of the type specified by p1.
The types that can be obtained are as follows:
^p
  0 : X coordinate of the mouse cursor on the screen
  1 : Y coordinate of the mouse cursor on the screen
      The screen coordinate system is not the coordinates within the window used by the pos command, etc., but the coordinates seen from the entire desktop screen.

  2 : Active window ID

      Returns the ID of the currently active window.
      If the active window is other than HSP, it will be -1.

  3 : Operation destination window ID

      The operation destination window ID of the screen specified by the gsel command is assigned.

  4 : X coordinate of the upper left of the window
  5 : Y coordinate of the upper left of the window

    Returns the upper left coordinates of the current window in screen coordinates.
    The screen coordinate system is not the coordinates within the window used by the pos command, etc., but the coordinates seen from the entire desktop screen.

  6 : X coordinate of the lower right of the window
  7 : Y coordinate of the lower right of the window

      Returns the lower right coordinates of the current window in screen coordinates.
      The screen coordinate system is not the coordinates within the window used by the pos command, etc., but the coordinates seen from the entire desktop screen.

  8 : X coordinate of the drawing origin of the window
  9 : Y coordinate of the drawing origin of the window

      Returns the drawing origin coordinates of the current operation destination window.
      This is a value that indicates how much the drawing content is scrolled in a scrollable window (the display size is smaller than the initialization size).
      In a normal state where scrolling is not performed, it will be (0,0).
      Use the groll command to set the drawing origin.

 10 : X size of the entire window
 11 : Y size of the entire window

      The width and height of the current operation destination window are returned.
      This includes the size of the window frame and title bar.

 12 : X size of the client area
 13 : Y size of the client area

      Returns the client area size of the current operation destination window.
      The client area refers to the drawable part displayed in the window.

 14 : X size of message output
 15 : Y size of message output

      The size of the message output by the last mes, print command is returned.
      The size indicates how many dots were drawn in the X and Y coordinates.
      Note that this is not an absolute coordinate on the screen.
      Also, if you output a multi-line string, the size of the last line will be acquired.

 16 : Currently set color code (R)
 17 : Currently set color code (G)
 18 : Currently set color code (B)

      The color code specified by the color command, etc. is returned.

 19 : Desktop color mode

      Returns the current desktop color mode (color mode).
      0 is returned for full color mode, and 1 is returned for palette mode.

 20 : X size of the entire desktop
 21 : Y size of the entire desktop

      The size of the entire desktop (screen resolution) is returned.

 22 : X coordinate of the current position
 23 : Y coordinate of the current position

      The X and Y coordinates of the current position set by the pos command are returned.

 24 : Window ID during message interrupt

      Returns the window ID during message interrupt set by the oncmd command.

 25 : Unused window ID

      Returns an unused window ID that has not been initialized by the screen command or buffer command.

 26 : X size of screen initialization
 27 : Y size of screen initialization

      Returns the initialization size of the current operation destination window.
      This is the screen size initially initialized with the screen, bgscr, and buffer commands.

 256 : Acceleration sensor value (X)
 257 : Acceleration sensor value (Y)
 258 : Acceleration sensor value (Z)

      Acquires the value of the sensor in an environment where the acceleration sensor is available.
      The acceleration of each coordinate axis is acquired as a real number (m/s^2). (A value including the gravitational acceleration of 9.8 m/s^2 is acquired.)
      If the acceleration sensor is not available, 0.0 is acquired.

 259 : Gyro sensor value (X)
 260 : Gyro sensor value (Y)
 261 : Gyro sensor value (Z)

      Acquires the value of the sensor in an environment where the gyro sensor is available.
      The tilt angle (Degree) for each coordinate axis is acquired.
      If the gyro sensor is not available, 0.0 is acquired.
^p

%href
objinfo
%port+
Let
%portinfo
In HSPLet, type 19, the screen color mode always returns 0.
In HSPLet, type 24, the interrupt window ID of oncmd is not supported.
In HSPLet, type 25 is not supported.


%index
grect
Fills with a rotating rectangle
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6
p1=0〜(0)   : X coordinate of the center of the rectangle
p2=0〜(0)   : Y coordinate of the center of the rectangle
p3=0.0〜(0.0) : Rotation angle (unit is radians)
p4=0〜(?)   : X size
p5=0〜(?)   : Y size

%inst
Draws a rectangle (rectangle) of the size specified by (p4, p5) with the currently set color, with the coordinates specified by (p1, p2) as the center.
You can specify the rotation angle as a real number with p3.
The unit of angle is radians (starting from 0, one revolution at 2π).
The grect command reflects the copy mode specified by gmode.
^
If gmode is 0 or 1, normal filling.
If gmode is 3, it becomes semi-transparent at the specified rate.
If gmode is 5 or 6, color addition or color subtraction processing is performed, respectively.
Also, if the size specification of (p4, p5) is omitted, the copy size set by the gmode command is used.
^
 In palette mode, filling is performed with the specified drawing color, but semi-transparent processing is not performed.

%href
color
hsvcolor
palcolor
grotate
gsquare
gmode
%port+
Let


%index
grotate
Rotates and copies a rectangular image
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6
p1=0〜(0)   : Copy source window ID
p2=0〜(0)   : X coordinate of the upper left of the copy source
p3=0〜(0)   : Y coordinate of the upper left of the copy source
p4=0.0〜(0.0) : Rotation angle (unit is radians)
p5=0〜(?)   : X size
p6=0〜(?)   : Y size

%inst
The grotate command performs processing that includes rotation in the gcopy command.
Specify the window ID of the copy source with p1, and the coordinates corresponding to the original image to be copied with (p2, p3). (Same as the gcopy command)
The copy destination is the window ID specified as the current drawing destination, and the copy is performed to the coordinates centered on the location set by the pos command. At that time, you can specify the rotation angle as a real number with p4.
The unit of angle is radians (starting from 0, one revolution at 2π).
Specify the X and Y sizes after being copied with (p5, p6).
Also, the X and Y sizes of the copy source use the default copy size set by the gmode command.
In other words, if you specify a size larger than the size specified by the gmode command with (p5, p6), it will be enlarged.
If (p5, p6) are omitted, the copy is performed at the same size as the copy source, that is, at the same magnification.
^
The grotate command reflects the copy mode specified by gmode. (Pixel alpha blend copy in mode 7 is not applied.)
Transparency color settings and semi-transparency settings all function in the same way as when copied with the gcopy command. (See the reference for the gmode command for details)
^
Semi-transparent processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full-color mode) results in an error.

%href
grect
gsquare
gmode
%port+
Let


%index
gsquare
Draws an arbitrary quadrangle
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1=0〜(0) : Copy source window ID (filled if negative value)
p2=variable name : Numerical array variable name in which the copy destination X coordinates are stored
p3=variable name : Numerical array variable name in which the copy destination Y coordinates are stored
p4=variable name : Numerical array variable name in which the copy source X coordinates are stored
p5=variable name : Numerical array variable name in which the copy source Y coordinates are stored

%inst
Copies or fills a quadrangle with arbitrary four points.
With p1, specify the window ID of the image copy source.
^
Specify variables in which the four coordinates of the copy destination are stored in an array in p2 and p3.
Specify variables in which the four coordinates of the copy source are stored in an array in p4 and p5.
The array variable must contain the coordinates in the order of upper left (0), upper right (1), lower right (2), and lower left (3).
^
The gsquare command reflects the copy mode specified by gmode. (Pixel alpha blend copy in mode 7 is not applied.)
Transparency color settings and semi-transparency settings all function in the same way as when copied with the gcopy command. (See the reference for the gmode command for details)
^
Semi-transparent processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full-color mode) results in an error.
p1 can be specified with a negative value such as -1, in which case, instead of copying an image, filling is performed.
You can select either solid color filling or gradient filling.
If p1 is specified with a value from -1 to -256, solid color filling will be performed.
When performing solid color filling, there is no need to specify the p4 and p5 parameters.
In that case, in full-color mode, filling will be done with the drawing color.
In palette mode, filling will be done using the value specified in p1 minus 1 as the palette code. (-1 to -256 corresponds to palette codes 0 to 255).
^
If p1 is specified as -257 or gsquare_grad (a macro definition of -257), gradient filling will be executed.
When performing gradient filling, specify an array variable containing the RGB codes indicating the colors of the vertices in the p4 parameter.
(The RGB codes are the same as the color codes specified by the gradf command.)
Note that in gradient filling, the copy mode setting specified by gmode is not reflected, and drawing is always opaque.


%href
grect
grotate
gmode
color
hsvcolor
palcolor
gradf
%port+
Let



%index
objinfo
Get window object information
%group
Basic input/output functions
%prm
(p1,p2)
p1=0〜 : Window object ID
p2=0〜 : Type to get

%inst
Returns information values related to a specific window object placed in the currently active window.
By specifying the window object ID in p1 and the acquisition type in p2, the corresponding information data is returned.
The types that can be obtained are as follows:
^p
     0  : Mode (lower 16 bits) and option data (upper 16 bits)
     1  : Pointer to the BMSCR structure where the object is placed
     2  : Window object handle (HWND)
     3  : owid (internal general-purpose data)
     4  : owsize (internal general-purpose data)
  5〜8  : Information of assignment variables
 9〜11  : Window object callback information
    12  : Background brush handle (HBRUSH)
    13  : Background color information (RGB)
    14  : Text color information (RGB)
15〜16  : Object-specific additional information (exinfo1, exinfo2)
^p
Window object information is not normally required for general use.
It can be used for special purposes, such as passing the window object handle (HWND) to an external API (DLL).

%href
ginfo
objinfo_bmscr
objinfo_hwnd
objinfo_mode
%port+
Let
%portinfo
During HSPLet, only the window handle (type==2) is available.


%index
axobj
Place ActiveX control
%group
Screen control commands
%prm
p1,"IID",p2,p3
p1    : Variable name to which the COM object is assigned
"IID" : Class ID or Program ID of the COM object
p2=0〜: X size of the object
p3=0〜: Y size of the object


%inst
Places an ActiveX control (OLE control) with the class ID or Program ID (ProgID) specified by "IID" as a window object.
Specify the X and Y sizes of the object to be placed with p2 and p3.
If the size specification is omitted or 0 or less, the entire window size is used.
If placement is successful, the object ID is assigned to the system variable stat, and the variable in p1 is initialized as a COM object type variable for the control.
If the ActiveX control fails to be placed, -1 is assigned to the system variable stat and the process terminates.
^p
Example :
	axobj ie, "Shell.Explorer.2",640,480
	ie->"Navigate" "www.onionsoft.net"
^p

%href
newcom
delcom
querycom
%portinfo
"atl.dll" is used to place ActiveX controls.
If "atl.dll" is not installed, an "Unsupported feature selected" error will be displayed.



%index
winobj
Place window object
%group
Screen control commands
%prm
"Class","Title",p1,p2,p3,p4,p5,p6
"Class" : Window class name
"Title" : Window name
p1(0)   : Extended window style
p2(0)   : Window style
p3(0)   : X size
p4(0)   : Y size
p5(0)   : Menu handle or child window ID
p6(0)   : Window creation data

%inst
winobj places a window supported by the Windows system as an object.
Specify the X and Y sizes to be placed with p3 and p4. If the size specification is omitted or 0 or less, the value set by objsize is used.
^
When the specified window object is created, the object ID is assigned to the system variable stat.
This object ID can be used with object manipulation commands such as the clrobj command.
(The created window is registered as an object managed by the currently active window.)
If the window object is not created normally due to an internal error, -1 is assigned to the system variable stat.
^
The objprm command cannot be used for objects placed by winobj.
^
This command is for placing window objects that are not supported by HSP commands through the Windows API, and receives the same parameters as CreateWindowEx in WIN32API. It is used for interacting with external DLLs and creating modules, and you usually don't need to remember it.



%href
pos
objsize
clrobj



%index
sendmsg
Send window message
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1(0) : Window handle
p2(0) : Message ID
p3(0) : wParam value
p4(0) : lParam value

%inst
Sends the message of p2 and the parameters of p3, p4 to the window handle specified in p1.
^
The value of the window handle specified in p1 can be obtained using the system variable hwnd if it is an HSP window, or using the objinfo function if it is an HSP window object.
^
The window message is sent with p3 as wParam and p4 as lParam information.
The p3 parameter can be specified as an integer value.
The p4 parameter can be specified as either an integer value or a string.
If it is an integer value, the value itself is treated as lParam.
If it is a string, the string pointer value is treated as lParam.
^
This command is for sending Windows messages to window objects that are not supported by HSP commands.
It is used for interacting with external DLLs and creating modules, and you usually don't need to remember it.

%href
hwnd
objinfo
winobj
%port+
Let
%portinfo
During HSPLet, only the following messages are implemented:
0x00B0:EM_GETSEL
0x00B1:EM_SETSEL
0x00C6:EM_CANUNDO
0x00C7:EM_UNDO
0x00CC:EM_SETPASSWORDCHAR
0x0300:WM_CUT
0x0301:WM_COPY
0x0302:WM_PASTE
0x0303:WM_CLEAR

%index
groll
Set window drawing origin and scale
%group
Screen control commands
%prm
p1,p2,p3,p4
p1=0〜(0)  : X coordinate that is the origin for drawing on the window
p2=0〜(0)  : Y coordinate that is the origin for drawing on the window

%inst
Sets the origin coordinate for drawing the graphic surface on the window.
The origin coordinate sets from which coordinates in the drawn graphic surface to display in the window.
By changing the coordinates, you can display from any position on the graphic surface.
If (0,0) is set as the origin coordinate, it will be displayed in the window in a form that matches the drawn coordinates.
The currently set origin coordinates can be obtained by ginfo_vx, ginfo_vy.

%href
ginfo_vx
ginfo_vy



%index
gradf
Fill a rectangle with a gradient
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7
p1=0〜(0)  : X coordinate of the upper left corner of the rectangle
p2=0〜(0)  : Y coordinate of the upper left corner of the rectangle
p3=0〜     : X size of the rectangle
p4=0〜     : Y size of the rectangle
p5=0〜(0)  : Gradient mode
p6=0〜     : Fill color 1 (RGB color code)
p7=0〜     : Fill color 2 (RGB color code)

%inst
Fills a rectangle (quadrangle) with a gradient, with (p1, p2) as the upper left point and (p3, p4) as the size.
The fill color is the RGB color code specified by p6, p7.
The RGB color code is a 24-bit number that collectively represents the luminance of each of R, G, and B specified by the color command.
When writing in a script, it is convenient to use a hexadecimal representation with "$" at the beginning.
If you write like "$112233", the R luminance will be $11, the G luminance will be $22, and the B luminance will be $33. In this case, the luminance will be from $00 to $ff (corresponding to 0 to 255 in decimal).
You can specify the gradient mode with p5.
^p
   p5 : Gradient mode
 --------------------------------------------------------------------
    0 : Horizontal gradient
    1 : Vertical gradient
^p
Gradient filling is performed with the upper left as the color specified by p6 and the lower right as the color specified by p7.
If the specification of p6 and p7 is omitted, the current drawing color is used.
If (p1, p2) is omitted, the upper left of the screen (0,0) is set.
If (p3, p4) is omitted, the drawing size of the screen is set.
^p
Example :
	; Fill a rectangle with a gradient
	gradf 120,180,400,100, 1, $ff00ff, $ffffff
^p
Use the boxf command to fill a rectangle with a solid color.
Also, to perform gradient drawing with the color of each vertex of the rectangle specified, use the gsquare command.

%href
boxf
gsquare



%index
celload
Load image file into buffer
%group
Screen control commands
%prm
"filename",p1,p2
"filename" : File name to load
p1=1〜(-2) : Destination window ID
p2=0〜1(0) : Screen mode to initialize

%inst
Loads an image file into the specified virtual screen.
The celload command is mainly used to load image materials for copying with the celput command or gcopy command
into a virtual screen (hidden window).
It is basically the same as initializing a virtual screen with the buffer command and loading an image file with the picload command.
^
Specify the image file name to load in the "filename" parameter.
The image file formats that can be used are the same as those of the picload command.
You can specify the window ID of the loading destination with p1.
If p1 is omitted or is a negative value, an unused window ID is automatically used.
^p
Value : Macro Name : Content
------------------------------------------------------------------------
-1 : celid_auto : Assigns an unused window ID
-2 : celid_reuse : Reuses an already loaded image and assigns its ID
^p
If -2 (celid_reuse) is specified in p1, or if the specification is omitted, the window ID of an already loaded image will be reused when loading an image file that is identical to an already loaded image.
If -1 (celid_auto) is specified in p1, reuse will not be performed, and loading will always be performed using an unused window ID.
In either case, the window ID that was loaded is assigned to the system variable stat after the celload command is executed, allowing you to know the loaded ID.
^
With p2, you can specify the initialization mode of the virtual screen.
If p2 is omitted or is 0, full-color mode is selected. If it is 1, palette mode is selected.
^p
p2 : Initialization Mode
--------------------------------------------------------------------
0 : Full-color mode
1 : Palette mode (invalid in HSP3Dish)
^p
The celload command allows you to efficiently load and manage image assets.
For details, please refer to the CEL related command section in the programming manual (hspprog.htm).

%href
picload
buffer
celdiv
celput

%index
celdiv
Sets the split size of the image asset
%group
Screen Control Command
%prm
p1,p2,p3,p4,p5
p1=0〜(1) : Window ID
p2=1〜(0) : Horizontal split size
p3=1〜(0) : Vertical split size
p4=0〜(0) : Horizontal center coordinate
p5=0〜(0) : Vertical center coordinate

%inst
Sets the split size of the image asset held by the specified window ID.
The divided image assets are referenced when drawing images using the celput command.
For example, if a 256 x 256 dot image is set to be divided into 128 x 64 dots, it can be treated as eight 128 x 64 dot images by the celput command.
^
With p1, specify the window ID that has the image asset.
With (p2, p3), specify the horizontal (X) and vertical (Y) split sizes (the number of dots per divided area).
If the split size is omitted, or if it is a value of 0 or less, the size of the asset will be used as is.
^
With (p4, p5), you can set the center coordinates of the drawing.
This will be the base point for drawing when drawing with the celput command.
For example, if (0, 0) is specified as the center coordinates, (0, 0) of the image asset will be placed at the position drawn by the celput command (the coordinates specified by the pos command).
It also serves as the center of rotation when drawing rotated images.
Normally, the (0, 0) position, or the upper left corner, is the base point. This is the same as the base point in the gcopy command.

^
The split setting of the image asset is saved for each window ID, and when initialized by the cls command, the setting is no division (specifying the entire screen as the size).
Be sure to make the split setting after the image asset has been loaded into the specified window ID.
If you load the image asset after making the split setting, the split setting will be reset.
For details, please refer to the CEL related command section in the programming manual (hspprog.htm).

%href
celload
celput

%index
celput
Draws an image asset
%group
Screen Control Command
%prm
id,no,zoomx,zoomy,angle
id=0〜(1) : Window ID with the image asset
no=0〜(0) : Divided image No.
zoomx=0.0〜(1.0) : Horizontal display magnification (real number)
zoomy=0.0〜(1.0) : Vertical display magnification (real number)
angle=0.0〜(0.0) : Rotation angle (unit is radians)

%inst
Draws the image asset loaded into the virtual screen on the current operation target window.
With id, specify the window ID that has the image asset.
With no, specify the divided image No. The divided image No. is the number that identifies the divided image within the window ID that has the image asset.
With zoomx, zoomy, you can specify the horizontal (X) and vertical (Y) magnification of the image to be drawn.
The zoomx and zoomy specifications can be specified as real numbers.
You can also specify the rotation angle of the image with angle.
The value specified for angle is a real number and the unit is radians (starting from 0, one rotation is 2π). (Same value as grotate and grect commands)
^
The celput command copies and draws the image of the specified window ID in the same way as the gcopy command.
Similar to the gcopy command, the copy mode and blend rate specified by the gmode command are reflected.
The drawing position is based on the current current position (the value specified by the pos command).
The size of the image to be drawn is usually the same as the original image (in the specified window ID).
The size of the image to be drawn and the center position of drawing and rotation can be changed by the celdiv command.
^
If the zoomx, zoomy, and angle specifications are omitted, or if the magnification is set to equal (1, 1, 0), a high-speed copy is automatically executed internally. Conversely, if drawing is not at equal magnification, the same drawing process as the grotate command is executed.
After the drawing is completed, the current position is moved to the right according to the drawn size.
(However, the angle is not taken into account. It only moves to the right by the horizontal size of the drawing)
^
The celput command and its related commands allow you to efficiently manage image assets.
For details, please refer to the CEL related command section in the programming manual (hspprog.htm).

%href
celload
celdiv
gcopy
grotate
