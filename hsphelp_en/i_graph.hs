;
;	HSP help manager用 HELPソースファイル
;	(先頭が;の行はコメントとして処理されます)
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
p1=0〜4(0) : Color to clear with

%inst
Clears all information in the window.
^
You can specify 5 types of colors to clear with using p1.
^p
Color values:
 ( 0=White / 1=Light Gray / 2=Gray / 3=Dark Gray / 4=Black )
^p
When the screen is cleared with the cls command, objects such as buttons and input boxes, font and color settings on the screen will be reset to their initial state.
%port+
Let



%index
mes
Display message
%group
Screen control commands
%prm
"strings",sw
"strings" : Message to display, or variable
sw(0)     : Option
%inst
Displays the specified message in the window.
The message is displayed from the current position, which is a virtual cursor position on the window. (The current position can be set with the pos command.)
The option value can be used to specify the behavior when displaying the message.
^p
   Value  : Macro name       : Action
 -----------------------------------------------------
   1   : mesopt_nocr    : Does not add a line feed at the end
   2   : mesopt_shadow  : Draws shadowed characters
   4   : mesopt_outline : Draws outlined characters
   8   : mesopt_light   : Draws simple shadows/outlines
   16  : mesopt_gmode   : Reflects gmode settings (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position will move to the right of the displayed characters.
If this option is not specified, the current position will automatically move to the next line.
If 2 or mesopt_shadow is specified as an option, characters with shadows will be drawn.
If 4 or mesopt_outline is specified as an option, outlined characters will be drawn.
The color specified by the objcolor command is used for the shadow and outline.
Also, the width of the shadow and outline can be specified with the font command.
If 16 or mesopt_gmode is specified as an option, drawing will be performed reflecting the settings of transparency, color addition, etc. specified by gmode.
(This option is only valid for HSP3Dish and HGIMG4. It is ignored in the regular HSP3 runtime.)
^
If the width of the shadow and outline is greater than 1, the processing load will increase.
If you want to reduce the load with simple drawing, specify 8 or mesopt_light as an option.
^
Option values can be added to specify multiple options at the same time.
If "mesopt_nocr+mesopt_shadow" is specified, the display will be shadowed and will not be line-fed.
^
The font of the displayed message can be specified with the font command.
If the message to be displayed contains a line feed code, it will be line-fed and the display will continue from the next line.

%href
print
font
pos
objcolor
%port+
Let
%sample
	mes "mes command displays a string"

	mes {"Multi-line strings can also
be displayed"}

	font msmincho, 20, font_bold
	mes "The font command can change the size and other attributes of the characters"

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
The option value can be used to specify the behavior when displaying the message.
^p
   Value  : Macro name       : Action
 -----------------------------------------------------
   1   : mesopt_nocr    : Does not add a line feed at the end
   2   : mesopt_shadow  : Draws shadowed characters
   4   : mesopt_outline : Draws outlined characters
   8   : mesopt_light   : Draws simple shadows/outlines
   16  : mesopt_gmode   : Reflects gmode settings (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position will move to the right of the displayed characters.
If this option is not specified, the current position will automatically move to the next line.
If 2 or mesopt_shadow is specified as an option, characters with shadows will be drawn.
If 4 or mesopt_outline is specified as an option, outlined characters will be drawn.
The color specified by the objcolor command is used for the shadow and outline.
Also, the width of the shadow and outline can be specified with the font command.
If 16 or mesopt_gmode is specified as an option, drawing will be performed reflecting the settings of transparency, color addition, etc. specified by gmode.
(This option is only valid for HSP3Dish and HGIMG4. It is ignored in the regular HSP3 runtime.)
^
If the width of the shadow and outline is greater than 1, the processing load will increase.
If you want to reduce the load with simple drawing, specify 8 or mesopt_light as an option.
^
Option values can be added to specify multiple options at the same time.
If "mesopt_nocr+mesopt_shadow" is specified, the display will be shadowed and will not be line-fed.
^
The font of the displayed message can be specified with the font command.
If the message to be displayed contains a line feed code, it will be line-fed and the display will continue from the next line.
^
The print command is an alias of the mes command, and both have the same meaning.
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
(The title bar caption is the part that usually displays "Hot Soup Processor ver3.x".)

%href
objprm
%port+
Let
%portinfo
In the case of HSPLet applet, the text in the status bar changes.

%sample
	// Display the characters "Window Title" in the title bar of the window
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
The dialog boxes displayed by the dialog command pop up separately from the HSP window.
^
The HSP script will not continue execution until the dialog is closed by the user's operation.
^
The type of dialog changes depending on the setting of p1.
^p
   Type : Content
 -----------------------------------------------------
     0    : Standard message box + [OK] button
     1    : Warning message box + [OK] button
     2    : Standard message box + [Yes][No] button
     3    : Warning message box + [Yes][No] button
    16    : File OPEN dialog
    17    : File SAVE dialog
    32    : Color selection dialog (fixed colors)
    33    : Color selection dialog (RGB can be freely selected)
    64〜  : Extended dialog
^p
For types 0 to 3, a message box displaying the contents specified by "message" will appear. Also, you can specify the title bar string of the message box in "option". (If omitted, nothing will be displayed in the title.)
When the message box is closed, the information of the button pressed is assigned to the system variable stat. At this time, the content of the variable stat is,
^p
  1 : "OK" button was pressed
  6 : "Yes" button was pressed
  7 : "No" button was pressed
^p
like this.
^
For types 16 to 17, a file selection dialog box will appear for selecting from a list of files. At this time, you can specify the extension of the file to be loaded (or saved) in "message" (up to 3 characters), so that only that extension is displayed. (Specify "*" to display all files)
Also, an auxiliary explanation of the extension, such as "Text file" for "txt", will be displayed in "File type" in the dialog box in "option".
"option" can also be omitted.
^p
Example :
	dialog "txt",16,"Text file"
	dialog "STAT="+stat+"\nNAME="+refstr,0,"Result"
	stop
^p
When the file selection is completed, the result is assigned to the system variable stat.
If the variable stat is 1, it means that the selection was successful. If the variable stat is 0, it means that it was canceled or an error occurred.
Multiple file types can be specified by separating them with the "|" symbol.
^p
Example :
	dialog "txt|log",16,"Text file|Log file"
	dialog "STAT="+stat+"\nNAME="+refstr,0,"Result"
	stop
^p
For types 32 to 33, a color selection dialog is displayed. When the color selection is completed, the RGB data of the selected color is assigned to the system variables ginfo_r, ginfo_g, ginfo_b. Also, if the variable stat is 0, it means that it was canceled or an error occurred. If it is 1, it means that the selection was successful.
^p
Example :
	dialog "",33
	dialog "R="+ginfo_r+"/G="+ginfo_g+"/B="+ginfo_b,0,"Result"
	stop
^p
Values of type 64 and later are reserved for extended dialogs prepared for each runtime. If an unsupported type is specified, nothing will be executed.
%port+
Let



%index
bgscr
Initialize borderless window
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜(0)  : Window ID
p2=1〜(640): Initial screen size X (in pixels)
p3=1〜(480): Initial screen size Y (in pixels)
p4=0〜(0) : Initial screen mode
p5=0〜(-1) : Window placement X (in pixels)
p6=0〜(-1) : Window placement Y (in pixels)
p7=0〜     : Window size X (in pixels)
p8=0〜     : Window size Y (in pixels)

%inst
Initializes the window ID and creates a borderless window.
This window is used for special purposes such as creating screen savers.
Specify the screen size to be initialized with p2 and p3 parameters.
The screen mode to be initialized by p4 can be selected from the following:
^p
1 : Create in Palette Mode (256 colors out of 16.7 million)
2 : Create a hidden window
 256 : Full screen (HSP3Dish/HGIMG4 only)
^p
If you specify "+1" as the p4 parameter (create in palette mode), the screen will be created in 256-color mode with a palette. If you don't specify this mode, the screen will be created in full color (24-bit color).
^
If you specify "+2" as the p4 parameter (create a hidden window), the window will not be displayed at startup, but you can display it using "gsel ID,1".
^
The p5 and p6 parameters allow you to specify the position of the window displayed on the desktop.
If p5 and p6 are omitted or set to negative values, the system-defined coordinates will be used.
If you want to display only a portion of the screen size specified by the p2 and p3 parameters in the window,
you can specify the client size of the window (the actual size displayed) with the p7 and p8 parameters.
In that case, you will be able to set the position displayed within the window using the groll command.
Normally, you can omit the specification of the p7 and p8 parameters. In that case, the client size will be the same as the screen size specified by p2 and p3.
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

	// Create a window without a border
	bgscr 2, 320, 240
	boxf

%index
bmpsave
Save screen image
%group
Screen control command
%prm
"filename"
"filename" : Filename to save

%inst
Saves the current screen image as a BMP image file.
A file is created with the name specified by "filename". You must specify the filename including the extension.
^
The saved image size will be the size at which the target window was initialized. To change the size, copy the content to a window of a different size and then save it with bmpsave.

%index
boxf
Fill rectangle
%group
Screen control command
%prm
p1,p2,p3,p4
p1=0〜(0)  : X coordinate of the top-left corner of the rectangle
p2=0〜(0)  : Y coordinate of the top-left corner of the rectangle
p3=0〜     : X coordinate of the bottom-right corner of the rectangle
p4=0〜     : Y coordinate of the bottom-right corner of the rectangle

%inst
Fills a rectangle (quadrilateral) on the screen with the current drawing color, using (p1,p2) and (p3,p4) as the top-left and bottom-right points.
If (p1,p2) are omitted, the top-left corner of the screen (0,0) is set.
If (p3,p4) are omitted, the bottom-right corner of the screen (up to the maximum drawing size) is set.

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
Screen control command
%prm
p1,p2,p3,p4
p1=0〜 (0) : Window ID
p2=1〜(640): Screen size X to initialize (in pixels)
p3=1〜(480): Screen size Y to initialize (in pixels)
p4=0〜(0) : Screen mode to initialize

%inst
Similar to the screen command, initializes the specified window ID for use. Similar to the screen command, specifies the screen size to initialize, but the buffer command only creates a virtual screen in memory and does not display it on the actual screen.
The virtual screen created in this way can use the print and picload commands in the same way as a normal screen. It can be used as a storage location for copying to another screen using the gcopy command, or as a storage location for temporarily saving the current screen.
^
You can specify the following values for the screen mode of the p4 parameter:
^p
  Screen Mode    Macro Name             Description
---------------------------------------------------------------------------------------------------
   0            screen_normal        Initialize in full-color mode (24-bit color) (default)
   1            screen_palette       Initialize in palette mode (256 colors out of 16.7 million colors) (Windows version only)
   32           screen_offscreen     Initialize as a drawing destination (HSP3Dish/HGIMG4 only)
   64           screen_usergcopy     Assign a drawing shader (HGIMG4 only)
^p
In HSP3 for Windows, it is possible to initialize in palette mode (256 colors out of 16.7 million colors).

In HSP3Dish and HGIMG4 runtimes, it is possible to specify screen_offscreen.
In HSP3Dish, you can access the drawing destination buffer as array data using the celbitmap command.
In HGIMG4, it is initialized as a drawing destination to which normal drawing commands are applied.
Without this option, you cannot use drawing commands (you can load images) when using a virtual screen on the HSP3Dish and HGIMG4 runtimes.
^
When using the HGIMG4 runtime, you can apply your own shader by specifying screen_usergcopy. For more information, refer to the HGIMG4 programming guide.
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
Screen control command
%prm
p1,p2,p3
p1=0〜2(0)  : Mode setting
p2=0〜(640) : X-direction image resolution setting
p3=0〜(480) : Y-direction image resolution setting

%inst
Forces a change to the current display resolution.
Sets the X and Y resolutions with (p2, p3) and sets the change mode with p1.
The modes are 1 for full-color mode (32-bit) and 2 for palette mode (8-bit). If the change mode is 0, the system returns to the initial state (before the change).
^p
Example:
	chgdisp 1,640,480	; Set to 640x480 full color
^p
If the specification of the (p2, p3) parameters is omitted, (640,480) will be used.
^
When the chgdisp command is executed, the result is reflected in the system variable stat.
The contents of the system variable stat are as follows:
^p
  stat=0 : Resolution was set successfully.
  stat=1 : Color mode could not be changed, but
           the resolution was changed successfully.
  stat=2 : Resolution could not be changed.
^p

%index
color
Color settings
%group
Screen control command
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : Color code (R, G, B brightness)

%inst
Sets the color for message display, drawing, etc., to the specified value.
p1, p2, and p3 are the brightness of R, G, and B, respectively.
^
0 is the darkest and 255 is the brightest.
color 0,0,0 is black, and color 255,255,255 is white.
In palette mode screens, the palette closest to the specified color is selected.

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
Screen control command
%prm
"fontname",p1,p2,p3
"fontname"   : Font name
p1=1〜(12)   : Font size
p2=0〜(0)    : Font style
p3=1〜(1)    : Font modification width

%inst
Sets the font style for text displayed by the mes and print commands.
Specify the font name with "fontname".
^
For the font name, specify the font name directly, such as "MS Mincho" or "MS Gothic".
The font size is specified by the logical size proportional to the number of dots.
The larger the number, the larger the characters. The font styles are:
^p
  Style 1  : Bold
  Style 2  : Italic
  Style 4  : Underline
  Style 8  : Strikeout
  Style 16  : Anti-aliasing
^p
You can also specify multiple styles simultaneously by adding the numbers.
If p2 is omitted, 0 (normal style) will be used.
Specifying style 16 enables anti-aliasing of the characters (this behavior depends on the OS environment).
^
The p3 parameter specifies the width used when displaying fonts with decorations such as shadows and borders. If the parameter is omitted, the default value (1) is used.
Font decorations can be set with the option values of the mes or print commands.
^
To return to the initial settings or the system-specified font, use the sysfont command. To change the font of an object, you need to specify the mode with the objmode command.
^
If the specified font cannot be found, a substitute font is automatically searched for. If the search for a substitute font also fails, -1 is assigned to the system variable stat. If the font is set normally, 0 is assigned to the system variable stat.

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
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0〜(0) : Window ID
p2=0〜(0) : X coordinate of the top-left corner of the copy source
p3=0〜(0) : Y coordinate of the top-left corner of the copy source
p4=0〜    : X size to copy (in pixels)
p5=0〜    : Y size to copy (in pixels)

%inst
The gcopy command copies a part of the screen of the specified window ID to the current drawing destination and current position.
The window ID of the copy source must be the same as your own window ID, or a window ID initialized with the screen or buffer command.
^
When copying with the gcopy command, you can select several modes for copying with the gmode command.
^
When copying an image from a palette mode screen with the gcopy command, caution is required.
It is necessary to prepare an image that has no problems even if the palettes of the two windows being copied are the same.
Also, for palettes in palette mode, be sure to set the correct ones yourself. (Palette settings are not performed by the picload command alone when using image files.)

%href
gmode
gzoom
%port+
Let

%index
gmode
Screen copy mode setting
%group
Screen control command
%prm
p1,p2,p3,p4
p1=0〜6(0)   : Screen copy mode
p2=0〜(32)   : X size to copy (in pixels)
p3=0〜(32)   : Y size to copy (in pixels)
p4=0〜256(0) : Blend rate for semi-transparent composition

gcopy Modes and Settings (Size, Blend Rate, etc.)

The following copy modes are available for use with `gcopy`, each with different behaviors:

^
*   Mode 0 = Normal Copy

^p
Performs image copying using the Windows system API (BitBlt function).
This is the most standard mode.
In most cases, it processes faster than other modes.

^p
*   Mode 1 = Memory-to-Memory Copy

^p
Performs a memory copy between image buffers.
The source and destination must be in the same image mode (palette or full-color).
The behavior is the same as Mode 0, but it may be slightly faster than Mode 0 when copying very small sizes (e.g., 16*16 dots or smaller).

^p
*   Mode 2 = Transparent Color Copy

^p
Copies in the same way as Mode 1, but any pixel with RGB values of all 0s (pure black) is treated as transparent and is not copied.

^p
*   Mode 3 = Semitransparent Blending Copy

^p
Can only be used in full-color mode.
Performs semitransparent blending copy using the blend rate specified in the 4th parameter of `gmode`. The blend rate ranges from 0 to 255, where 255 is a complete copy.
If the blend rate is 0, no copy is performed.
If the blend rate is 256 or higher, a memory-to-memory copy is performed.

^p
*   Mode 4 = Transparent Color Semitransparent Blending Copy

^p
Copies in the same way as Mode 3, but any pixel with RGB values equal to the color set by the `color` command is treated as transparent and is not copied.
If the blend rate is 0, no copy is performed.

^p
*   Mode 5 = Color Addition Blending Copy

^p
Can only be used in full-color mode.
Performs color addition blending copy using the blend rate specified in the 4th parameter of `gmode`. With color addition, the RGB values of the source are added to the RGB values of the destination.
If the luminance exceeds 255 during addition, it is clamped to 255.

^p
*   Mode 6 = Color Subtraction Blending Copy

^p
Can only be used in full-color mode.
Performs color subtraction blending copy using the blend rate specified in the 4th parameter of `gmode`. With color subtraction, the RGB values of the source are subtracted from the RGB values of the destination. If the luminance falls below 0 during subtraction, it is clamped to 0.

^p
*   Mode 7 = Pixel Alpha Blending Copy

^p
Can only be used in full-color mode.
Prepares an image of the size specified by `gmode` to the right of the original image and performs background composition copy using the right image as the alpha blend component.
If the alpha blend component is 255, the pixel from the original image is copied as-is. If it is less than 255, it is composited with the background at that blend rate.
Because pixels are referenced per RGB unit, it's possible to set different blend rates for each RGB.
The alpha blend image should typically be expressed in grayscale from (0,0,0) to (255,255,255).

^p
These modes are not used with the `gzoom` command.
Note that they only apply to the `gcopy` command.
Also, `gmode` changes the mode of the window ID currently selected with the `gsel` command.
Be aware that settings will not be reflected if you execute the `gcopy` command with a different window ID after setting `gmode`.

^
(p2, p3) set the default copy size. This value is used by default when omitting the size values when using the `gcopy` and `gzoom` commands.

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
Mode 7 is not supported in HSPLet.

%index
gsel
Specify Drawing Destination, Window to Front, Hide Settings
%group
Screen Control Commands
%prm
p1,p2
p1=0〜(0)  : Window ID
p2=0〜2(0) : Window Active Switch

%inst
Changes the screen for screen control commands to the specified window ID.
After this, screen drawing related commands such as mes command and picload command will be executed for the screen of the window ID specified by p1.
^
You can specify an optional switch with p2. The value of p2 can change the state of the window as follows:

^p
-1: Hides the specified window
 0: Does not particularly affect the specified window
 1: The specified window becomes active
 2: The specified window becomes active and always becomes the topmost
^p
If you omit the specification of p2, there will be no particular effect.
^
If you hide with p2=-1, the specified window disappears, but it is not completely erased. It will return if you activate it again with p2=1 or 2.
%port+
Let
%portinfo
In HSPLet, mode 2 is not supported, and mode 1 does not necessarily become active.

%index
gzoom
Scale and Copy Screen
%group
Screen Control Commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜     : Size to copy to the screen X (in dots)
p2=0〜     : Size to copy to the screen Y (in dots)
p3=0〜(0)  : Window ID
p4=0〜(0)  : X coordinate of the upper left of the copy source
p5=0〜(0)  : Y coordinate of the upper left of the copy source
p6=0〜     : Size to copy X (in dots)
p7=0〜     : Size to copy Y (in dots)
p8=0〜1(0) : Zoom mode

%inst
The `gzoom` command copies a portion of the screen of the specified window ID to the current drawing destination current position by scaling it to an arbitrary size.
The window ID of the copy source must be the same as your window ID, or a window ID initialized with the screen command or buffer command.
^
You can specify the zoom mode with p8.
If you specify 1 for p8, a high-quality image using halftone is generated during zooming. (Does not work on Windows 9X.)
^
If p8 is 0 or omitted, halftone is not used, but image processing can be performed faster.

%href
gcopy
%port+
Let

%index
palcolor
Drawing Palette Setting
%group
Screen Control Commands
%prm
p1
p1=0〜255(0) : Palette Code

%inst
Sets the color for message display, drawing, etc. to the specified value.
p1 becomes the palette code. The palette code indicates the number of the color among the 256 colors prepared for the palette mode screen (the color set by the palette command).
Normally, it is not necessary to use it as long as you are using full color mode.

%href
color
hsvcolor
rgbcolor
palette

%index
palette
Palette Setting
%group
Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1=0〜255(0)       : Palette Code
p2,p3,p4=0〜255(0) : Color Code (R, G, B luminance)
p5=0〜1(0)         : Update Switch

%inst
Changes the color data of the palette set on the current screen.
Sets the RGB color of the luminance specified by p2, p3, p4 to the palette code of p1.
If the value of p1 is negative, the setting is ignored.
You can specify the update on the screen by the value of p5.
When p5 is 0 or omitted, the value is set to the palette, but it is not immediately reflected on the screen.
When p5 is 1, the entire set palette is reflected on the screen.
It takes time for it to be reflected on the screen, so if you update the palette together, be sure to set the update switch to 1 at the end.

%href
palcolor

%index
pget
Get 1 dot point
%group
Screen Control Commands
%prm
p1,p2
p1=0〜 : X coordinate on the screen
p2=0〜 : Y coordinate on the screen

%inst
Reads the color data of 1 dot on the screen.
^
The color data of the dot at the coordinates specified by p1 and p2 is set as the selected color. (It will be the same state as specified by the color command)
If the specification of p1 and p2 is omitted, the coordinates of the current position set by the pos command are used.
The acquired color information can be referred to by the ginfo function or ginfo_r, ginfo_g, ginfo_b macros.

%href
ginfo
ginfo_r
ginfo_g
ginfo_b
%port+
Let

%index
picload
Load Image File
%group
Screen Control Commands
%prm
"filename",p1
"filename" : File name to load
p1=0〜1(0) : Image Load Mode

%inst
Loads an image file. Normally,
^p
picload "test.bmp"
^p
Simply doing this will load the image file specified by "test.bmp" onto the current screen, and the window size will automatically match the image size.
^
The image load mode of p1 is
^p
Mode 0: Initialize window and load
Mode 1: Load on top of current screen
Mode 2: Initialize window with black and load
^p
By omitting the load mode of p1 or setting it to 0, it is loaded into the window initialized with the image size.
^
By specifying load mode 1 for p1, you can load it from the specified coordinates without changing the current window size or anything else. In this case, the coordinates specified by the pos command are the upper left.
^
The file formats that can currently be loaded are as follows.
^p
BMP format: Extension BMP: Windows standard 4, 8, 24 bit data.
                            RLE compressed data can also be handled.
GIF format: Extension GIF: GIF format data.
                            Animation format is not supported.
JPEG format: Extension JPG: JFIF standard JPEG data.
                            Grayscale data is also OK.
ICO format: Extension ICO: Windows standard ICO format icon data.
PNG format: Extension PNG: Portable Network Graphics data.
PSD format: Extension PSD: Photoshop format data. (Only composite layers)
TGA format: Extension TGA: TARGA format data.
^p
The picload command can be executed on screens initialized with the screen, buffer, and bgscr commands.
*Multi-icon format ICO files are not supported by default.
*Interlaced PNG files are not supported by default. If you want to load all formats supported by PNG, please use the imgload or hspcv plugin.

%href
screen
buffer
bgscr
imgload
%port+
Let
%portinfo
HSPLet supports BMP, JPEG, GIF, and PNG loading. Other loads are not supported.

%index
pos
Current Position Setting
%group
Screen Control Commands
%prm
p1,p2
p1=0〜     : X coordinate of the current position
p2=0〜     : Y coordinate of the current position
%inst
Specifies the coordinates of the current position, which serves as the basic coordinates for displaying messages, objects, etc.
^
X is 0 at the far left, Y is 0 at the top, and the units are in single dots.
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
Screen control commands
%prm
p1,p2
p1=0〜 : X coordinate on the screen
p2=0〜 : Y coordinate on the screen

%inst
Draws a 1-dot point on the screen with the currently set drawing color.
If p1 and p2 are omitted, the coordinates set by the pos command are used.

%href
pget
color
palcolor
hsvcolor
%port+
Let





%index
redraw
Redraw settings
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1=0〜3(1) : Drawing mode setting
p2=0〜(0)  : X coordinate of the upper left corner to redraw
p3=0〜(0)  : Y coordinate of the upper left corner to redraw
p4=0〜(0)  : Width X to redraw (in dots)
p5=0〜(0)  : Width Y to redraw (in dots)

%inst
Specifies the drawing mode of the screen.  The drawing mode is:
^
Drawing mode 0:
^p
Screen control commands such as mes, print, gcopy, and gzoom only rewrite the virtual screen and are not reflected on the actual screen.
^p
Drawing mode 1:
^p
When a screen control command is executed, it is also reflected on the actual screen.
^p
This is the normal mode.
By copying images or displaying messages within the screen in drawing mode 0, and then switching to mode 1 at the end, you can eliminate screen flickering and make the display smoother.
^
Specifying mode 1 will immediately update the screen.
^
If you specify a value that is the mode value plus 2, only the drawing mode will be changed, and the screen will not be updated.
^
Also, the p2 to p5 parameters allow you to redraw only a portion of the screen. If omitted, the entire screen will be updated.
%port+
Let
%sample
// Sample of redraw command
// You can see the flickering if you delete the redraw command

*main_loop
	redraw 0

	color 255, 255, 255
	boxf

	pos 0, 0
	color 0, 0, 0
	mes "Characters do not flicker even when drawing and erasing repeatedly"

	redraw 1

	wait 1
	goto *main_loop


%index
screen
Initializes the window
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1=0〜(0)  : Window ID
p2=1〜(640): Initial screen size X (in dots)
p3=1〜(480): Initial screen size Y (in dots)
p4=0〜(0)  : Initial screen mode
p5=0〜(-1) : Window position X (in dots)
p6=0〜(-1) : Window position Y (in dots)
p7=0〜     : Window size X (in dots)
p8=0〜     : Window size Y (in dots)
%inst
Initializes the specified window ID so that it can be used.
In the initial state of HSP, only the screen with window ID 0 is used, but by specifying window ID 1 or higher, you can create a new window.
Windows with ID 1 or higher have a style that allows the size to be freely changed by default.
^
The initial screen mode of p4 can be selected from the following:
^p
   0(screen_normal)    : Create in full-color mode
  +1(screen_palette)   : Create in palette mode (256 colors out of 16.7 million colors)
  +2(screen_hide)      : Create a hidden window
  +4(screen_fixedsize) : Fixed-size window
  +8(screen_tool)      : Tool window
 +16(screen_frame)     : Window with a deep edge
 +256                  : Full screen (HSP3Dish/HGIMG4 only)
^p
If you specify "+1" (create in palette mode) for the p4 parameter, the screen will be created in palette mode with 256 colors. If you specify 0, a full-color (24-bit color) screen will be created.
^
If you specify "+2" (create a hidden window) for the p4 parameter, the window will not be displayed at runtime, but it can be displayed with "gsel ID,1".
^
Among the p4 parameters, "+4", "+8", and "+16" are for setting the window style.
"Fixed-size window" is created with the same style as the window of ID0, where the size cannot be changed even for windows of ID1 or later.
"Tool window" creates a window with a smaller title bar and the window name is not displayed in the taskbar.
^
If the p4 parameter is omitted, it will be created in palette mode if the current Windows screen mode has a palette (256 colors), otherwise it will be created as a full-color screen.
^
Multiple settings can be made for the screen mode value by summing the values.
For example,
^p
	screen 2,320,240,4+8
^p
creates a window with the style of fixed size + tool window.
^
The screen command can also re-initialize a window ID that has already been initialized with a different setting.
^p
Example:
	screen 0,640,480,1
^p
The above example re-initializes window ID 0, that is, the main window, to 640x480 dots in palette mode.
^
The p5 and p6 parameters allow you to specify the position of the window displayed on the desktop screen.
If p5 and p6 are omitted or set to negative values, the system's default coordinates are used.
If you want to display only a portion of the screen size specified by the p2 and p3 parameters in the window,
you can specify the client size of the window (the actual size displayed) with the p7 and p8 parameters.
In that case, the position displayed within the window can be set with the groll command.
Normally, you can omit the specification of the p7 and p8 parameters. In that case, it will be displayed with the same client size as the screen size specified by p2 and p3.
^
After initializing the screen with the screen command, that window ID becomes the drawing destination for screen control commands.
^
On the HSP3Dish and HGIMG4 runtime, all image buffers loaded up to that point will be lost if you initialize the screen with the screen command.

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
HSPLet cannot use palette mode, tool windows, or windows with deep edges.



%index
width
Window size setting
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1=0〜(-1) : Client area size X (in dots)
p2=0〜(-1) : Client area size Y (in dots)
p3=0〜(-1) : Window X coordinate on the display (in dots)
p4=0〜(-1) : Window Y coordinate on the display (in dots)
p5=0〜1(0) : Window coordinate setting option
%inst
Changes the client size of the window (the actual size displayed) and the window display position.
The client size cannot be larger than the screen size initialized by the screen command or the buffer/bgscr commands.
^
If the p1 and p2 parameters are omitted or set to negative values, the current settings are used.
The p3 and p4 parameters allow you to set the window coordinates on the display.
If the value of p5 is the default value or 0, the current settings are used when the p3 and p4 parameters are negative values.
If the value of p5 is 1, the coordinates are set even when the p3 and p4 parameters are negative values. This is used when it is necessary to handle negative coordinate values in a multi-monitor environment, etc.
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
Screen control commands
%prm
p1
p1=0〜 : Font type specification

%inst
Selects a system standard font.
Specify the font type with p1. Select from the following:
^p
   p1 : font set
 --------------------------------------------------------------------
    0 : HSP standard system font
   10 : Fixed-width font for OEM character set
   11 : Fixed-width system font for Windows character set
   12 : Variable-width system font for Windows character set
   13 : Standard system font
   17 : Default GUI font
        (Used for menus and dialog boxes in standard environments)
^p
If p1 is omitted, the standard system font is selected.


%href
font
%port+
Let





%index
line
Draws a straight line
%group
Screen control commands
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
This allows you to draw continuous straight lines.

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
Screen control commands
%prm
p1,p2,p3,p4,p5
p1=0〜(0)  : X coordinate of the upper left corner of the rectangle
p2=0〜(0)  : Y coordinate of the upper left corner of the rectangle
p3=0〜     : X coordinate of the lower right corner of the rectangle
p4=0〜     : Y coordinate of the lower right corner of the rectangle
p5=0〜1(1) : Drawing mode (0=line, 1=fill)

%inst
Draws a circle on the screen with a size that fits within a rectangle with (p1, p2) as the upper left point and (p3, p4) as the lower right point.
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
Screen control commands
%prm
p1
p1 : System color index to set

%inst
Sets the system color index specified by p1 as the selected color.
(This is the same state as specified by the color command).
The system color indices that can be specified by p1 are the following values:
^p
 0 : Scroll bar axis color
 1 : Desktop color
 2 : Active window title bar color (left side color of active window title bar in case of gradient)
 3 : Text color of inactive window title bar (left side color of inactive window title bar in case of gradient)
 4 : Menu background color
 5 : Window background color
 6 : Window frame color
 7 : Menu text color
 8 : Window text color
 9 : Active window title bar text color
10 : Active window border color
11 : Inactive window border color
12 : MDI application background color
13 : Color of selected item within the control
14 : Text color of selected item within the control
15 : Surface color of 3D object
16 : Shadow color of 3D object (for edge opposite the light source)
17 : Light color (disabled state) text color
18 : Push button text color
19 : Inactive window title bar text color
20 : Brightest color of 3D object (for edge towards light source)
21 : Dark shadow color of 3D object
22 : Bright color of 3D object (for edge towards light source)
23 : Tooltip control text color
24 : Tooltip control background color
26 : Hot track item color
27 : Right side color of active window title bar in case of gradient
28 : Right side color of inactive window title bar in case of gradient
29 : (Windows XP or later) Color used to highlight menu items when the menu is displayed as a flat menu
30 : (Windows XP or later) Menu bar background color when the menu is displayed as a flat menu
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
Screen control command
%prm
p1,p2,p3
p1=0-191(0) : HSV format H value
p2=0-255(0) : HSV format S value
p3=0-255(0) : HSV format V value

%inst
Specifies the color in HSV format.
Like the color command, the set color will be used in subsequent drawing commands.

%href
color
palcolor
%port+
Let


%index
rgbcolor
Sets the color in RGB format
%group
Screen control command
%prm
p1
p1=$000000-$ffffff(0) : RGB format color code value

%inst
Specifies the color in RGB format ($rrggbb).
Specify the value obtained by converting the luminance of each of RGB into an 8-bit value in p1.
You can write like "$112233" using "$" to specify a hexadecimal number.
In this case, it will specify "R=$11, G=$22, B=$33".
The function of the command is equivalent to the color command and the hsvcolor command, and the set color will be used in the subsequent drawing commands.
The RGB format color code value specified by rgbcolor is compatible with the 24-bit color code specified in HTML, etc.

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
p1=0- : Type to get

%inst
Returns the window-related information value of the type specified by p1.
The obtainable types are as follows:
^p
  0 : X coordinate of the mouse cursor on the screen
  1 : Y coordinate of the mouse cursor on the screen
      Screen coordinate system is not the coordinate within the window used in pos command etc., but the coordinate viewed from the entire desktop screen.

  2 : Active window ID

      Returns the currently active window ID.
      If the active window is other than HSP, it will be -1.

  3 : Operation target window ID

      The operation target window ID of the screen specified by the gsel command is assigned.

  4 : X coordinate of the upper left of the window
  5 : Y coordinate of the upper left of the window

    Returns the upper left coordinates of the current window in the screen coordinate system.
    Screen coordinate system is not the coordinate within the window used in pos command etc., but the coordinate viewed from the entire desktop screen.

  6 : X coordinate of the lower right of the window
  7 : Y coordinate of the lower right of the window

      Returns the lower right coordinates of the current window in the screen coordinate system.
      Screen coordinate system is not the coordinate within the window used in pos command etc., but the coordinate viewed from the entire desktop screen.

  8 : X coordinate of the drawing origin of the window
  9 : Y coordinate of the drawing origin of the window

      Returns the drawing origin coordinates of the current operation target window.
      This is the value indicating how much it is scrolled in the scrollable window (the display size is smaller than the initialization size).
      In the normal state where it is not scrolled, it will be (0,0).
      Use the groll command to set the drawing origin.

 10 : X size of the entire window
 11 : Y size of the entire window

      The width and height of the current operation target window are returned.
      This is the size including the frame and title bar of the window.

 12 : Client area X size
 13 : Client area Y size

      The client area size of the current operation target window is returned.
      The client area refers to the drawable part displayed in the window.

 14 : Message output X size
 15 : Message output Y size

      The size of the message output by the last mes, print command is returned.
      The size shows how many dots are drawn in the X and Y coordinates.
      Please note that it is not an absolute coordinate on the screen.
      Also, when a multi-line string is output, the size corresponding to the last line is acquired.

 16 : Currently set color code (R)
 17 : Currently set color code (G)
 18 : Currently set color code (B)

      The color code specified by the color command etc. is returned.

 19 : Desktop color mode

      The current desktop color mode is returned.
      0 is returned for full color mode, and 1 is returned for palette mode.

 20 : X size of the entire desktop
 21 : Y size of the entire desktop

      The size of the entire desktop (screen resolution) is returned.

 22 : X coordinate of the current position
 23 : Y coordinate of the current position

      The X, Y coordinates of the current position set by the pos command are returned.

 24 : Window ID at the time of message interrupt

      Returns the window ID at the time of message interrupt set by the oncmd command.

 25 : Unused window ID

      Returns the unused window ID that is not initialized by the screen command or the buffer command.

 26 : Screen initialization X size
 27 : Screen initialization Y size

      The initialization size of the current operation target window is returned.
      This is the screen size initially initialized by the screen, bgscr, and buffer commands.

 256 : Acceleration sensor value (X)
 257 : Acceleration sensor value (Y)
 258 : Acceleration sensor value (Z)

      Acquires the sensor value in an environment where the acceleration sensor is available.
      The acceleration of each coordinate axis is acquired as a real number (m/s^2). (A numerical value including the gravitational acceleration of 9.8m/s^2 is acquired.)
      0.0 is acquired in an environment where the acceleration sensor cannot be used.

 259 : Gyro sensor value (X)
 260 : Gyro sensor value (Y)
 261 : Gyro sensor value (Z)

      Acquires the sensor value in an environment where the gyro sensor is available.
      The tilt angle (Degree) for each coordinate axis is acquired.
      0.0 is acquired in an environment where the gyro sensor cannot be used.
^p

%href
objinfo
%port+
Let
%portinfo
In HSPLet, type 19, the color mode of the screen always returns 0.
In HSPLet, type 24, the interrupt window ID of oncmd is not supported.
In HSPLet, type 25 is not supported.


%index
grect
Fill with a rotating rectangle
%group
Screen control command
%prm
p1,p2,p3,p4,p5,p6
p1=0-(0)   : X coordinate of the center of the rectangle
p2=0-(0)   : Y coordinate of the center of the rectangle
p3=0.0-(0.0) : Rotation angle (unit is radian)
p4=0-(?)   : X size
p5=0-(?)   : Y size

%inst
Draws a rectangle (rectangle) with the size specified by (p4, p5) in the currently set color with the coordinates specified by (p1, p2) as the center.
The rotation angle can be specified by a real number in p3.
The unit of angle is radian (starting from 0, one round at 2π).
The grect command reflects the copy mode specified by gmode.
^
If gmode is 0 or 1, normal filling.
If gmode is 3, it becomes semi-transparent at the specified rate.
If gmode is 5 or 6, the color addition and color subtraction processes are performed respectively.
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
Rotate and copy a rectangular image
%group
Screen control command
%prm
p1,p2,p3,p4,p5,p6
p1=0-(0)   : Copy source window ID
p2=0-(0)   : X coordinate of the upper left of the copy source
p3=0-(0)   : Y coordinate of the upper left of the copy source
p4=0.0-(0.0) : Rotation angle (unit is radian)
p5=0-(?)   : X size
p6=0-(?)   : Y size

%inst
The grotate command performs processing including rotation on the gcopy command.
Specify the window ID of the copy source in p1, and specify the coordinates corresponding to the original image to be copied in (p2, p3). (Same as the gcopy command)
The copy destination is the window ID specified as the current drawing destination, and the copy is performed to the coordinates centered on the location set by the pos command. At that time, the rotation angle can be specified by a real number in p4.
The unit of angle is radian (starting from 0, one round at 2π).
Specify the X, Y sizes after the copy in (p5, p6).
Also, the X, Y sizes of the copy source use the default copy size set by the gmode command.
In other words, if you specify a size larger than the size specified by the gmode command in (p5, p6), it will be enlarged.
If (p5, p6) is omitted, the copy will be performed at the same size as the copy source, that is, at the same magnification.
^
The grotate command reflects the copy mode setting specified by gmode. (Pixel alpha blend copy in mode 7 is not applied.)
Transparent color settings and semi-transparent settings all function in the same way as when copied with the gcopy command. (See the reference of the gmode command for details)
^
Semi-transparent processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full color mode) will result in an error.

%href
grect
gsquare
gmode
%port+
Let


%index
gsquare
Draw an arbitrary quadrilateral
%group
Screen control command
%prm
p1,p2,p3,p4,p5
p1=0-(0) : Copy source window ID (fill if negative value)
p2=variable name : Numeric array variable name storing copy destination X coordinates
p3=variable name : Numeric array variable name storing copy destination Y coordinates
p4=variable name : Numeric array variable name storing copy source X coordinates
p5=variable name : Numeric array variable name storing copy source Y coordinates

%inst
Copies or fills a quadrilateral with arbitrary 4 points.
Specify the image copy source window ID in p1.
^
Specify variables in which the four coordinates of the copy destination are stored in an array in p2 and p3.
Specify variables in which the four coordinates of the copy source are stored in an array in p4 and p5.
It is necessary to put the coordinates in the array variables in the order of upper left (0), upper right (1), lower right (2), lower left (3).
^
The gsquare command reflects the copy mode setting specified by gmode. (Pixel alpha blend copy in mode 7 is not applied.)
Transparent color settings and semi-transparent settings all function in the same way as when copied with the gcopy command. (See the reference of the gmode command for details)
^
Semi-transparent processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full color mode) will result in an error.
p1に-1などのマイナス値を指定した場合は、画像コピーではなく塗りつぶしが行われます。
塗りつぶしは、単色による塗りつぶしと、グラデーションによる塗りつぶしを選択することができます。
p1に、-1から-256の値を指定した場合は、単色による塗りつぶしとなります。
単色塗りつぶしを行なう場合には、p4,p5パラメーターを指定する必要はありません。
その際、フルカラーモード時には描画色による塗りつぶしとなります。
パレットモード時は、p1で指定した値-1をパレットコードとして塗りつぶしを行ないます。(-1〜-256がパレットコード0〜255に対応します)

If a negative value such as -1 is specified for p1, filling will be performed instead of image copying.
You can choose between solid color filling and gradient filling.
If a value from -1 to -256 is specified for p1, solid color filling will be performed.
When performing solid color filling, it is not necessary to specify the p4 and p5 parameters.
At that time, in full-color mode, filling will be done with the drawing color.
In palette mode, the value specified by p1 minus 1 will be used as the palette code for filling. (Values from -1 to -256 correspond to palette codes 0 to 255.)

p1に、-257またはgsquare_grad(-257のマクロ定義)を指定した場合は、グラデーションによる塗りつぶしが実行されます。
グラデーション塗りつぶしを行なう場合には、p4パラメーターに頂点の色を示すRGBコードを格納した配列変数を指定してください。
(RGBコードは、gradf命令で指定される色コードと同一のものになります。)
グラデーション塗りつぶしでは、gmodeで設定されたコピーモードの指定は反映されず、常に不透明の描画になるので注意してください。

If -257 or gsquare_grad (macro definition of -257) is specified for p1, gradient filling will be executed.
When performing gradient filling, specify an array variable containing the RGB codes indicating the vertex colors in the p4 parameter.
(The RGB codes are the same as the color codes specified by the gradf command.)
Note that in gradient filling, the copy mode specified by gmode is not reflected, and the drawing is always opaque.


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
Getting window object information
%group
Basic input/output functions
%prm
(p1,p2)
p1=0〜 : Window object ID
p2=0〜 : Type to get

%inst
Returns the information value related to a specific window object placed in the currently active window.
By specifying the window object ID in p1 and the acquisition type in p2, the corresponding information data is returned.
The following types can be obtained:
^p
     0  : Mode (lower 16 bits) and option data (upper 16 bits)
     1  : Pointer to the BMSCR structure where the object is placed
     2  : Window object handle (HWND)
     3  : owid (internal general-purpose data)
     4  : owsize (internal general-purpose data)
  5〜8  : Information of assigned variables
 9〜11  : Information of window object callback
    12  : Background brush handle (HBRUSH)
    13  : Background color information (RGB)
    14  : Text color information (RGB)
15〜16  : Additional information specific to the object (exinfo1, exinfo2)
^p
Window object information is usually not necessary for normal use.
It can be used for special purposes, such as passing the window object handle (HWND) to an external API (DLL).

%href
ginfo
objinfo_bmscr
objinfo_hwnd
objinfo_mode
%port+
Let
%portinfo
In HSPLet, only the window handle (type==2) can be used.


%index
axobj
Placing ActiveX controls
%group
Screen control commands
%prm
p1,"IID",p2,p3
p1    : Variable name to which the COM object is assigned
"IID" : Class ID or Program ID of the COM object
p2=0〜: X size of the object
p3=0〜: Y size of the object


%inst
Places the ActiveX control (OLE control) with the class ID or Program ID (ProgID) specified by "IID" as a window object.
Specify the X and Y sizes of the placed object with p2 and p3.
If the size specification is omitted or 0 or less, the entire window size is used.
If the placement is successful, the object ID is assigned to the system variable stat, and the variable p1 is initialized as a COM object type variable for the control.
If the ActiveX control fails to be placed, -1 is assigned to the system variable stat, and the process terminates.
^p
Example:
	axobj ie, "Shell.Explorer.2",640,480
	ie->"Navigate" "www.onionsoft.net"
^p

%href
newcom
delcom
querycom
%portinfo
"atl.dll" is used to place ActiveX controls.
The error "Selected an unsupported feature" will be displayed in an environment where "atl.dll" is not installed.



%index
winobj
Window object placement
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
This command is for placing window objects that are not supported by HSP commands through the Windows API, and receives the same parameters as WIN32API's CreateWindowEx. It is used for exchanging data with external DLLs and creating modules, and you usually do not need to remember it.



%href
pos
objsize
clrobj



%index
sendmsg
Sending window messages
%group
Screen control commands
%prm
p1,p2,p3,p4,p5
p1(0) : Window handle
p2(0) : Message ID
p3(0) : wParam value
p4(0) : lParam value

%inst
Sends the message in p2 and the parameters in p3 and p4 to the window handle specified in p1.
^
The value of the window handle specified in p1 can be obtained using the system variable hwnd for HSP windows, or the objinfo function for HSP window objects.
^
The window message is sent with p3 as wParam and p4 as lParam.
An integer value can be specified for the p3 parameter.
Either an integer value or a string can be specified for the p4 parameter.
In the case of an integer value, the value itself is treated as lParam.
In the case of a string, the string pointer value is treated as lParam.
^
This command is for sending Windows messages to window objects that are not supported by HSP commands.
It is used for exchanging data with external DLLs and creating modules, and you usually do not need to remember it.

%href
hwnd
objinfo
winobj
%port+
Let
%portinfo
In HSPLet, only the following messages have been implemented:
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
Set the drawing origin and scale of the window
%group
Screen control commands
%prm
p1,p2,p3,p4
p1=0〜(0)  : X coordinate that becomes the origin of drawing to the window
p2=0〜(0)  : Y coordinate that becomes the origin of drawing to the window

%inst
Sets the origin coordinate for drawing the graphic surface to the window.
The origin coordinate sets which coordinate in the drawn graphic surface to display in the window.
By changing the coordinates, you can display from any position on the graphic surface.
If (0,0) is set as the origin coordinate, it will be displayed in the window in a form that matches the drawn coordinate.
The currently set origin coordinates can be obtained by ginfo_vx and ginfo_vy.

%href
ginfo_vx
ginfo_vy



%index
gradf
Fills a rectangle with a gradient
%group
Screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7
p1=0〜(0)  : X coordinate of the upper left of the rectangle
p2=0〜(0)  : Y coordinate of the upper left of the rectangle
p3=0〜     : X size of the rectangle
p4=0〜     : Y size of the rectangle
p5=0〜(0)  : Gradient mode
p6=0〜     : Fill color 1 (RGB color code)
p7=0〜     : Fill color 2 (RGB color code)

%inst
Fills a rectangle (quadrilateral) with a gradient, using (p1, p2) as the upper left point and (p3, p4) as the size.
The fill color is the RGB color code specified in p6 and p7.
The RGB color code is a 24-bit number that combines the brightness of each of R, G, and B specified by the color command.
When writing in a script, it is convenient to use hexadecimal notation with a "$" at the beginning.
For example, if you write "$112233", the R brightness will be $11, the G brightness will be $22, and the B brightness will be $33. In this case, the brightness will be from $00 to $ff (corresponding to 0 to 255 in decimal).
You can specify the gradient mode with p5.
^p
   p5 : Gradient mode
 --------------------------------------------------------------------
    0 : Horizontal gradient
    1 : Vertical gradient
^p
Gradient filling is performed with the upper left as the color specified by p6 and the lower right as the color specified by p7.
If the specification of p6 and p7 is omitted, the current drawing color is used.
If (p1, p2) is omitted, the upper left of the screen (0, 0) is set.
If (p3, p4) is omitted, the drawing size of the screen is set.
^p
Example:
	; Fills a rectangle with a gradient
	gradf 120,180,400,100, 1, $ff00ff, $ffffff
^p
To fill a rectangle with a single color, use the boxf command.
Also, to perform gradient drawing with the colors of each vertex of a quadrilateral specified, use the gsquare command.

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
"filename" : Filename to load
p1=1〜(-2) : Window ID to load to
p2=0〜1(0) : Screen mode to initialize

%inst
Loads an image file into the specified virtual screen.
The celload command is mainly for loading image materials into a virtual screen (hidden window) for copying using the celput command or gcopy command.
It is basically the same as initializing a virtual screen with the buffer command and loading an image file with the picload command.
^
Specify the image filename to load in the "filename" parameter.
The available image file formats are the same as for the picload command.
You can specify the window ID to load to in p1.
If p1 is omitted or a negative value, an unused window ID will be automatically used.
^p
Value : Macro Name : Content
------------------------------------------------------------------------
-1 : celid_auto : Assigns an unused window ID.
-2 : celid_reuse : Reuses an already loaded image and assigns its ID.
^p
If -2 (celid_reuse) is specified in p1, or if the specification is omitted, the already loaded window ID will be reused when loading an image file that is identical to an already loaded image.
If -1 (celid_auto) is specified in p1, reuse will not be performed, and an unused window ID will always be used for loading.
In either case, the window ID that was loaded will be assigned to the system variable stat after the celload command is executed, allowing you to know the loaded ID.
^
p2 can be used to specify the initialization mode of the virtual screen.
If p2 is omitted, or if it is 0, full-color mode is selected. If it is 1, palette mode is selected.
^p
p2 : Initialization Mode
--------------------------------------------------------------------
0 : Full-color Mode
1 : Palette Mode (Invalid for HSP3Dish)
^p
The celload command allows you to efficiently load and manage image assets.
For more information, please refer to the CEL-related commands in the programming manual (hspprog.htm).

%href
picload
buffer
celdiv
celput


%index
celdiv
Sets the division size of the image material
%group
Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1=0〜(1) : Window ID
p2=1〜(0) : Horizontal division size
p3=1〜(0) : Vertical division size
p4=0〜(0) : Horizontal center coordinate
p5=0〜(0) : Vertical center coordinate

%inst
Sets the division size of the image material held by the specified window ID.
The divided image material is referenced when drawing images with the celput command.
For example, if you set a 256x256 dot image to be divided by 128x64 dots, you can handle it as 8 128x64 dot images with the celput command.
^
Specify the window ID that has the image material in p1.
Specify the horizontal (X) and vertical (Y) division sizes (the number of dots per divided area) in (p2, p3).
If the division size is omitted or set to a value of 0 or less, the size of the material will be used as is.
^
You can set the center coordinates for drawing in (p4, p5).
This will be the base point for drawing when drawing with the celput command.
For example, if you specify (0,0) as the center coordinates, the (0,0) of the image material will be placed at the position where the image is drawn by the celput command (the coordinates specified by the pos command).
It also serves as the center of rotation when drawing rotated images.
Normally, the (0,0) position, i.e., the upper left corner, is the base point. This is the same as the base point for the gcopy command.

^
The division settings of image material are saved for each window ID, and when initialized with the cls command, the setting is no division (the entire screen is specified as the size).
Be sure to make the division settings after the image material has been loaded into the specified window ID.
If you load the image material after making the division settings, the division settings will be reset.
For more information, please refer to the CEL-related commands in the programming manual (hspprog.htm).

%href
celload
celput


%index
celput
Draw image material
%group
Screen Control Commands
%prm
id,no,zoomx,zoomy,angle
id=0〜(1) : Window ID with the image material
no=0〜(0) : Divided image No.
zoomx=0.0〜(1.0) : Horizontal display magnification (real number)
zoomy=0.0〜(1.0) : Vertical display magnification (real number)
angle=0.0〜(0.0) : Rotation angle (unit is radians)

%inst
Draws the image material loaded into the virtual screen on the current operation destination window.
Specify the window ID that has the image material in id.
Specify the divided image No. in no. The divided image No. is a number that identifies the divided image within the window ID that has the image material.
You can specify the horizontal (X) and vertical (Y) magnification of the image to be drawn by zoomx, zoomy.
The zoomx and zoomy specifications can be specified as real numbers.
You can also specify the rotation angle of the image by angle.
The value specified in angle is a real number, and the unit is radians (starting from 0, 2π for one rotation). (Same values as the grote, grect commands)
^
The celput command copies and draws the image of the specified window ID in the same way as the gcopy command.
Similarly to the gcopy command, the copy mode and blend ratio specified by the gmode command are reflected.
The drawing position is based on the current current position (the value specified by the pos command).
The size of the image to be drawn is usually the same as the original image (in the specified window ID).
The size of the image to be drawn and the center position for drawing and rotation can be changed by the celdiv command.
^
If the zoomx, zoomy, and angle specifications are omitted, or if the magnification is set to equal magnification (1,1,0), a high-speed copy is automatically executed internally. Conversely, if you are drawing at a non-equal magnification, the same drawing process as the grote command is executed.
After the drawing is completed, the current position is moved to the right according to the drawn size.
(However, the angle is not taken into consideration. It only moves to the right by the horizontal size of the drawing.)
^
The celput command and its related commands allow you to efficiently manage image materials.
For more information, please refer to the CEL-related commands in the programming manual (hspprog.htm).

%href
celload
celdiv
gcopy
grotate
