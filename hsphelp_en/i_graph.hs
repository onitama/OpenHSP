;
;HELP source file for HSP help manager
;(Lines beginning with; are treated as comments)
;

%type
Built-in instructions
%ver
3.6
%note
ver3.6 standard instruction
%date
2019/04/09
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
Screen control instructions
%prm
p1
p1 = 0 to 4 (0): Color when clearing

%inst
Clears all information in the window.
^
You can specify 5 kinds of colors to clear with p1.
^p
Color specification value:
 (0 = white / 1 = light gray / 2 = gray / 3 = dark gray / 4 = black)
^p
If you clear the screen with the cls command, the objects such as buttons and input boxes on the screen, fonts and color settings will be returned to the initial state.
%port+
Let



%index
mes
Message display
%group
Screen control instructions
%prm
"strings",sw
"strings": Messages or variables to display
sw (0): Optional
%inst
Displays the specified message in the window.
The message is displayed from a virtual cursor position on the window called the current position. (The current position can be set by the pos command.)
The option value allows you to specify the behavior when displaying a message.
^p
   Value: Macro name: Behavior
 -----------------------------------------------------
   1: mesopt_nocr: No line break at the end
   2: mesopt_shadow: Draw shadowed characters
   4: mesopt_outline: Draw border characters
   8: mesopt_light: Simple drawing of shadows / borders
   16: mesopt_gmode: Reflect the gmode setting (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position moves to the right of the displayed characters.
If this option is not specified, the current position will automatically move to the next line.
If 2 or mesopt_shadow is specified for the option, the shadowed characters will be drawn.
If 4 or mesopt_outline is specified for the option, the bordered characters will be drawn.
For shadows and borders, the color specified by the objcolor command is used.
Also, the width of the shadow and border can be specified with the font command.
When 16 or mesopt_gmode is specified as an option, drawing that reflects the settings such as translucency and color addition specified in gmode is performed.
(This option is only valid for HSP3Dish and HGIMG4, ignored by normal HSP3 runtime)
^
If the width of the shadow and border is made larger than 1, the load of display processing will increase.
If you want to reduce the load with simple drawing, specify 8 or mesopt_light as an option.
^
Multiple option values can be specified at the same time by adding them.
If "mesopt_nocr + mesopt_shadow" is specified, the display will be shaded and will not break.
^
The font of the displayed message can be specified with the font command.
If the message to be displayed contains a line feed code, the line will be broken and the display will continue from the next line.

%href
print
font
pos
objcolor
%port+
Let
%sample
mes "The mes instruction prints a string"

mes {"Multi-line strings
Can be displayed "}

	font msmincho, 20, font_bold
mes "You can change the font size etc. by the font command"

%index
print
Message display
%group
Screen control instructions
%prm
"strings",sw
"strings": Messages or variables to display
sw (0): Optional

%inst
Displays the specified message in the window.
The message is displayed from a virtual cursor position on the window called the current position. (The current position can be set by the pos command.)
The option value allows you to specify the behavior when displaying a message.
^p
   Value: Macro name: Behavior
 -----------------------------------------------------
   1: mesopt_nocr: No line break at the end
   2: mesopt_shadow: Draw shadowed characters
   4: mesopt_outline: Draw border characters
   8: mesopt_light: Simple drawing of shadows / borders
   16: mesopt_gmode: Reflect the gmode setting (HSP3Dish only)
^p
If 1 or mesopt_nocr is specified, the current position moves to the right of the displayed characters.
If this option is not specified, the current position will automatically move to the next line.
If 2 or mesopt_shadow is specified for the option, the shadowed characters will be drawn.
If 4 or mesopt_outline is specified for the option, the bordered characters will be drawn.
For shadows and borders, the color specified by the objcolor command is used.
Also, the width of the shadow and border can be specified with the font command.
When 16 or mesopt_gmode is specified as an option, drawing that reflects the settings such as translucency and color addition specified in gmode is performed.
(This option is only valid for HSP3Dish and HGIMG4, ignored by normal HSP3 runtime)
^
If the width of the shadow and border is made larger than 1, the load of display processing will increase.
If you want to reduce the load with simple drawing, specify 8 or mesopt_light as an option.
^
Multiple option values can be specified at the same time by adding them.
If "mesopt_nocr + mesopt_shadow" is specified, the display will be shaded and will not break.
^
The font of the displayed message can be specified with the font command.
If the message to be displayed contains a line feed code, the line will be broken and the display will continue from the next line.
^
The print instruction is another name for the mes instruction, and both have the same meaning.
HSP recommends using the mes instruction.

%href
mes
font
pos
objcolor
%port+
Let



%index
title
Title bar settings
%group
Screen control instructions
%prm
"strings"
"strings": Strings to specify

%inst
Set the title bar caption of the window to the contents of "strings".
(The title bar caption is the part that is usually displayed as "Hot Soup Processor ver3.x")

%href
objprm
%port+
Let
%portinfo
For the HSPLet applet, the text in the status bar changes.

%sample
// Display the word "Window Title" in the window title bar
	title "Window Title"

%index
dialog
Open dialog
%group
Screen control instructions
%prm
"message",p1,"option"
p1 = 0 to (0): Dialog type setting

%inst
Displays various standard Windows dialog boxes on the screen.
The dialog displayed by the dialog command pops up separately from the HSP window.
^
The continuation of the HSP script will not be executed until the dialog is closed by the user's operation.
^
The type of dialog depends on the p1 setting.
^p
   Type: Content
 -----------------------------------------------------
     0: Standard message box + [OK] button
     1: Warning message box + [OK] button
     2: Standard message box + [Yes] [No] button
     3: Warning message box + [Yes] [No] button
    16: File OPEN dialog
    17: File SAVE dialog
    32: Color selection dialog (fixed color)
    33: Color selection dialog (freely select RGB)
    64 ~: Extended dialog
^p
For types 0 to 3, a message box appears that displays the content specified by "message". You can also specify the title bar string of the message box with "option". (If omitted, nothing will be displayed in the title)
When the message box is closed, the information of the pressed button is assigned to the system variable stat. At this time, the content of the variable stat is
^p
  1: The "OK" button was pressed
  6: "Yes" button was pressed
  7: "No" button was pressed
^p
It looks like.
^
For types 16-17, a file selection dialog will appear for you to select from the file list. At this time, you can specify the extension (up to 3 characters) of the file to be loaded (or saved) in "message" so that only that extension is displayed. (If you specify "*", all files will be displayed.)
Also, "option" will display the extension details, for example "txt", a supplementary description such as "text file" will be displayed in the "Files of type" dialog.
"option" can be omitted.
^p
example :
dialog "txt", 16, "text file"
dialog "STAT =" + stat + "\\ nNAME =" + refstr, 0, "Result"
	stop
^p
When the file selection is complete, the result is assigned to the system variable stat.
If the variable stat is 1, it means that it was selected successfully. If the variable stat is 0, it means that it was canceled or an error occurred.
Multiple file types can be specified by separating them with the "|" symbol.
^p
example :
dialog "txt | log", 16, "text file | log file"
dialog "STAT =" + stat + "\\ nNAME =" + refstr, 0, "Result"
	stop
^p
For types 32 to 33, a color selection dialog is displayed. When the color selection is completed, the RGB data of the selected color is assigned to the system variables ginfo_r, ginfo_g, ginfo_b. Also, if the variable stat is 0, it means that it was canceled or an error occurred. If it is 1, it means that it was selected successfully.
^p
example :
	dialog "",33
dialog "R =" + ginfo_r + "/ G =" + ginfo_g + "/ B =" + ginfo_b, 0, "Result"
	stop
^p
Values of type 64 and above are reserved for extended dialogs provided by each runtime. If an unsupported type is specified, nothing will be done.
%port+
Let



%index
bgscr
Initialize a window without a frame
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1 = 0 to (0): Window ID
p2 = 1 to (640): Screen size to be initialized X (1 dot unit)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to (0): Screen mode to initialize
p5 = 0 to (-1): Window layout X (1 dot unit)
p6 = 0 to (-1): Window layout Y (1 dot unit)
p7 = 0 ~: Window size X (in 1 dot unit)
p8 = 0 ~: Window size Y (in 1 dot unit)

%inst
Initialize the window ID to create a frameless window.
This window is used for special purposes such as creating a screen saver.
The p2 and p3 parameters specify the screen size to be initialized.
The screen mode to initialize p4 can be selected from the following.
^p
   1: Create in palette mode (256 out of 16.7 million colors)
   2: Create a hidden window
^p
If "+1" (created in palette mode) is specified for the p4 parameter, the screen will be created in 256-color mode with palette. If this mode is not specified, a full color (24bit color) screen will be created.
^
If "+2" (creates a hidden window) is specified for the p4 parameter, the window will not be displayed at runtime, but it can be displayed with "gsel ID, 1".
^
The p5 and p6 parameters allow you to specify the position of the window displayed on the desktop screen.
If p5 and p6 are omitted or set to a negative value, the system-specified coordinates will be used.
If you want to display only a part of the screen size specified by the p2 and p3 parameters in the window,
You can specify the client size (the size that is actually displayed) of the window with the parameters of p7 and p8.
In that case, the groll command can be used to set the position displayed in the window.
Normally, you can omit the p7 and p8 parameters. In that case, the client size is the same as the screen size specified by p2 and p3.
^
After initializing the screen with the bgscr instruction, the window ID becomes the drawing destination of the screen control instruction.

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
mes "Please close this window to exit the program"

// Create a window without a frame
	bgscr 2, 320, 240
	boxf

%index
bmpsave
Screen image save
%group
Screen control instructions
%prm
"filename"
"filename": File name to save

%inst
Save the current screen image as it is as an image file in BMP format.
A file will be created with the name specified in "filename". You need to specify the file name including the extension.
^
The saved image size will be the size at which the target window is initialized. If you want to change the size, please copy the contents to a window of another size and then bmp save.




%index
boxf
Fill the rectangle
%group
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to (0): Upper left X coordinate of the rectangle
p2 = 0 to (0): Upper left Y coordinate of the rectangle
p3 = 0 ~: Lower right X coordinate of the rectangle
p4 = 0 ~: Lower right Y coordinate of the rectangle

%inst
On the screen, (p1, p2) and (p3, p4) are the upper left and lower right points, and the current drawing color is a rectangle.
Fill (square).
If (p1, p2) is omitted, the upper left (0,0) of the screen is set.
If (p3, p4) is omitted, the lower right corner of the screen (up to the full drawing size) is set.

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
// If all parameters are omitted, the whole will be filled
	boxf

	color 255, 128, 0
	boxf 80, 80, 150, 130

%index
buffer
Initialize virtual screen
%group
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to (0): Window ID
p2 = 1 to (640): Screen size to be initialized X (1 dot unit)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to (0): Screen mode to initialize

%inst
Like the screen instruction, it initializes the specified window ID so that it can be used. The screen size to be initialized and the screen mode are the same as the screen command, but the buffer command only creates a virtual screen in memory and does not display it on the actual screen.
The virtual screen created with this can use the print command and picload command in the same way as a normal screen. It can be used as a stock location for copying to another screen with the gcopy command, or as a storage location for temporarily keeping the current screen.
^
The window ID initialized by screen cannot be reinitialized by the buffer instruction.
^
After initializing the screen with the buffer command, the window ID becomes the drawing destination of the screen control command.

%href
screen
bgscr
screen_normal
screen_palette
%port+
Let
%portinfo
Palette mode cannot be used when using HSPLet.



%index
chgdisp
Change the image resolution
%group
Screen control instructions
%prm
p1,p2,p3
p1 = 0 to 2 (0): Mode setting
p2 = 0 to (640): Setting the image resolution in the X direction
p3 = 0 to (480): Setting the image resolution in the Y direction

%inst
Forces the current display resolution to change.
Set the X and Y resolutions with (p2, p3) and set the change mode with p1.
The mode is 1 for full color mode (32bit) and 2 for palette mode (8bit). If the change mode is 0, it returns to the initial state (before change).
^p
example :
chgdisp 1,640,480; 640x480 Set to full color
^p
If the parameter specification of (p2, p3) is omitted, it will be (640,480).
^
When the chgdisp instruction is executed, the result is reflected in the system variable stat.
The contents of the system variable stat are as follows.
^p
  stat = 0: The resolution was set normally.
  stat = 1: I couldn't change the color mode,
           The resolution change was successful.
  stat = 2: The resolution could not be changed.
^p



%index
color
Color setting
%group
Screen control instructions
%prm
p1,p2,p3
p1, p2, p3 = 0 to 255 (0): Color code (brightness of R, G, B)

%inst
Set the color of message display, drawing, etc. to the specified value.
p1, p2, and p3 are the brightness of R, G, and B, respectively.
^
0 is the darkest and 255 is the brightest.
color 0,0,0 is black and color 255,255,255 is white.
On the palette mode screen, the palette closest to the specified color is selected.

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
Screen control instructions
%prm
"fontname",p1,p2,p3
"fontname": font name
p1 = 1 ~ (12): Font size
p2 = 0 ~ (0): Font style
p3 = 1 ~ (1): Width of font modification

%inst
Set the text typeface to be displayed with the mes and print commands.
Specify the name of the font with "fontname".
^
For the font name, specify the font name such as "MS Mincho" or "MS Gothic" directly.
The font size is specified by the logical size proportional to the number of dots.
The larger the number, the larger the letters. The font style is
^p
  Style 1: Bold
  Style 2: Italics
  Style 4: Underline
  Style 8: Strikethrough
  Style 16: Antialiasing
^p
It will be. It is also possible to specify multiple styles at the same time by summing the numbers.
If you omit p2, it will be 0 (normal style).
Style 16 enables character antialiasing. (This works differently depending on the OS environment)
^
The p3 parameter specifies the width used when displaying qualified fonts such as shading and edging. If you omit the parameter, the default value (1) is used.
Font modification can be set with the option value of the mes or print instruction.
^
Use the sysfont instruction to return to the default or system-specified font. If you want to change the font of the object, you need to specify the mode with the objmode instruction.
^
If the specified font cannot be found, a substitute font will be searched automatically. If the search for a substitute font also fails, the system variable stat is assigned -1. If the font is set successfully, the system variable stat is assigned 0.

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
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 to (0): Window ID
p2 = 0 to (0): Upper left X coordinate of copy source
p3 = 0 to (0): Upper left Y coordinate of copy source
p4 = 0 ~: Copy size X (dot unit)
p5 = 0 ~: Copy size Y (dot unit)

%inst
The gcopy command copies a part of the screen with the specified window ID to the current drawing destination current position.
The copy source window ID must be the same as your own window ID or the window ID initialized by the screen or buffer instruction.
^
When copying with the gcopy instruction, you can select several modes to copy with the gmode instruction.
^
Be careful when copying the image on the palette mode screen with the gcopy command.
It is necessary to prepare an image that does not cause any problem even if the palettes of the two windows to be copied are the same.
Also, make sure that the user sets the correct palette in palette mode. (When using an image file, the palette cannot be set only with the picload command.)

%href
gmode
gzoom
%port+
Let




%index
gmode
Screen copy mode setting
%group
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to 6 (0): Screen copy mode
p2 = 0 to (32): Copy size X (dot unit)
p3 = 0 to (32): Copy size Y (dot unit)
p4 = 0 to 256 (0): Blend rate during translucent composition
%inst
Set the mode used by gcopy, size, blend ratio, etc.
The operation of each copy mode is as follows.
^
ÊèÉ Mode 0 = normal copy
^p
Copy the image using the Windows system API (BitBlt function).
This is the most standard mode.
It is often faster than other modes.
^p
ÊèÉ Mode 1 = memory-to-memory copy
^p
Makes a memory copy between image buffers.
The copy source and copy destination must be in the same image mode (palette mode / full color mode).
The operation is the same as mode 0, but it may be slightly faster than mode 0 when copying a very small size (16 * 16 dots or less, etc.).
^p
ÊèÉ Mode 2 = transparent colored copy
^p
Copying is done in the same way as in mode 1, but dots with all 0 RGB (complete black) are considered transparent and are not copied.
^p
„Éª Mode 3 = Semi-transparent composite copy
^p
It can be used only in full color mode.
Performs a semi-transparent composite copy with the blend ratio specified by the 4th parameter of gmode. The blend ratio can range from 0 to 256, with 256 being a perfect copy.
If the blend ratio is 0, no copy will be executed.
Also, if the blend ratio is 256 or more, memory-to-memory copy is executed.
^p
„Éª Mode 4 = Translucent composite copy with transparent color
^p
Copying is done in the same way as in mode 3, but dots with RGB values that are the same as the color set by the color command are considered transparent and are not copied.
If the blend ratio is 0, no copy will be executed.
^p
„Éª Mode 5 = Color Additive Synthesis Copy
^p
It can be used only in full color mode.
Performs color additive synthesis copying with the blend ratio specified by the 4th parameter of gmode. In color addition, the RGB value of the copy source is added to the RGB value of the copy destination.
If the brightness exceeds 255 at the time of addition, it will be clamped to 255.
^p
„Éª Mode 6 = Color subtractive composite copy
^p
It can be used only in full color mode.
Performs color subtractive composite copy with the blend ratio specified by the 4th parameter of gmode. In color subtraction, the RGB value of the copy source is subtracted from the RGB value of the copy destination. If the brightness is less than 0 when subtracting, it will be clamped to 0.
^p
ÊèÉ Mode 7 = Pixel Alpha Blend Copy
^p
It can be used only in full color mode.
Prepare an image of the size specified by gmode on the right side of the original image, and make a background composite copy using the image on the right side as an alpha blend component.
If the alpha blend component is 255, the pixels of the original image are copied as they are, and if it is smaller than 255, it is combined with the background at that blend ratio.
Pixels are referenced in RGB units, so you can set different blend rates for RGB.
Alpha-blended images are usually represented in grayscale from (0,0,0) to (255,255,255).
^p
These modes are not used in the gzoom instruction.
Please note that this is only when using the gcopy command.
In addition, gmode changes the mode of the window ID currently selected by the gsel instruction. Note that the setting will not be reflected even if the gcopy command is executed with a different window ID after setting gmode.
^
(p2, p3) sets the default copy size. This value is the size used by default when the value of the size to be copied by the gcopy and gzoom instructions is omitted.

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
Mode 7 is not supported during HSPLet.



%index
gsel
Specify drawing destination, front of window, hide setting
%group
Screen control instructions
%prm
p1,p2
p1 = 0 to (0): Window ID
p2 = 0 to 2 (0): Window active switch

%inst
Changes the drawing destination of the screen control command to the screen with the specified window ID.
After that, commands related to screen drawing such as the mes command and picload command will be executed for the screen with the window ID specified in p1.
^
An optional switch can be specified with p2. The value of p2 is
^p
  -1: Hide the specified window
   0: No particular effect on the specified window
   1: The specified window becomes active
   2: The specified window becomes active and always comes to the foreground
^p
You can change the state of the window like this.
If you omit the specification of p2, there is no particular effect.
^
If you hide it with p2 = -1, the specified window disappears, but it is not completely erased. If you activate it again with p2 = 1 or 2, it will return.
%port+
Let
%portinfo
Mode 2 is not supported during HSPLet and is not always active in mode 1.




%index
gzoom
Scale and copy screen
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1 = 0 ~: Size when copying to the screen X (dot unit)
p2 = 0 ~: Size Y (dot unit) when copying to the screen
p3 = 0 to (0): Window ID
p4 = 0 to (0): Upper left X coordinate of copy source
p5 = 0 to (0): Upper left Y coordinate of copy source
p6 = 0 ~: Copy size X (dot unit)
p7 = 0 ~: Copy size Y (dot unit)
p8 = 0 to 1 (0): Zoom mode

%inst
The gzoom instruction copies a part of the screen with the specified window ID to the current drawing destination current position by scaling it to an arbitrary size.
The copy source window ID must be the same as your own window ID or the window ID initialized by the screen or buffer instruction.
^
You can specify the zoom mode with p8.
If you specify 1 for p8, a high-quality image using halftone is generated when scaling. (It doesn't work on Windows 9X.)
^
If p8 is 0 or omitted, halftone is not used, but image processing can be performed faster.

%href
gcopy
%port+
Let




%index
palcolor
Drawing palette settings
%group
Screen control instructions
%prm
p1
p1 = 0 to 255 (0): Palette code

%inst
Set the color of message display, drawing, etc. to the specified value.
p1 is the palette code. The palette code indicates the number of the 256 colors (colors set by the palette command) prepared for the palette mode screen.
Normally, you do not need to use it as long as you are using full color mode.

%href
color
hsvcolor
rgbcolor
palette




%index
palette
Palette settings
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 to 255 (0): Palette code
p2, p3, p4 = 0 to 255 (0): Color code (brightness of R, G, B)
p5 = 0 to 1 (0): Update switch

%inst
Changes the color data of the palette set on the current screen.
Set the RGB color of the brightness specified by p2, p3, p4 to the palette code of p1.
If the value of p1 is negative, the setting will be ignored.
The value of p5 allows you to specify an update on the screen.
When p5 is 0 or omitted, the value is set in the palette, but it is not immediately reflected on the screen.
When p5 is 1, the entire set palette is reflected on the screen.
It takes time for the palette to be reflected on the screen, so if you want to update the palettes all at once, set the update switch to 1 at the very end.

%href
palcolor




%index
pget
Get 1dot points
%group
Screen control instructions
%prm
p1,p2
p1 = 0 ~: X coordinate on the screen
p2 = 0 ~: Y coordinate on the screen

%inst
Reads the 1-dot color data on the screen.
^
The color data of the dots at the coordinates specified by p1 and p2 is set as the selected color. (It will be in the same state as specified by the color command)
If p1 and p2 are omitted, the coordinates of the current position set by the pos instruction will be used.
The acquired color information can be referenced by the ginfo function or the ginfo_r, ginfo_g, ginfo_b macros.

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
Screen control instructions
%prm
"filename",p1
"filename": Filename to load
p1 = 0 to 1 (0): Image load mode

%inst
Load the image file. Normally,
^p
picload "test.bmp"
^p
Just do, the image file specified in "test.bmp" will be loaded on the current screen and the window size will be automatically adjusted to the size of the picture.
^
The image load mode of p1 is
^p
  Mode 0: Initialize and load the window
  Mode 1: Load on top of current screen
  Mode 2: Initialize and load the window in black
^p
By omitting or setting the load mode of p1 to 0, it will be loaded in the window initialized with the size of the image.
^
By specifying load mode 1 for p1, you can load from the specified coordinates without changing the size of the current window at all. In this case, the coordinates specified by the pos instruction will be the upper left.
^
The file formats that can be loaded now are as follows.
^p
BMP format: Extension BMP: Windows standard 4,8,24 bit data.
                        It can also handle RLE-compressed data.
GIF format: Extension GIF: GIF format data.
                        Animation format cannot be handled.
JPEG format: Extension JPG: JFIF standard JPEG data.
                        Grayscale data is also OK.
ICO format: Extension ICO: Windows standard ICO format icon data.
PNG format: Extension PNG: Portable Network Graphics data.
PSD format: Extension PSD: Photoshop format data. (Composite layer only)
TGA format: Extension TGA: Data in TARGA format.
^p
The picload instruction can be executed on the screen initialized by the screen, buffer, and bgscr instructions.
* Multi-icon format ICO files are not supported as standard.
* Interlaced PNG files are not supported as standard. If you want to load all formats supported by PNG, please use the imgload or hspcv plugin.

%href
screen
buffer
bgscr
imgload
%port+
Let
%portinfo
At HSPLet, it supports reading BMP, JPEG, GIF and PNG. Other reads are not supported.

%index
pos
Current position setting
%group
Screen control instructions
%prm
p1,p2
p1 = 0 ~: X coordinate of current position
p2 = 0 ~: Y coordinate of the current position

%inst
Specify the coordinates of the current position, which is the basic coordinates for displaying messages and objects.
^
X is 0 on the far left and 0 is on the top, which is specified in 1-dot units.
If you omit the parameter, the current value is used.

%href
mes
gcopy
line
%port+
Let



%index
pset
Display 1 dot dots
%group
Screen control instructions
%prm
p1,p2
p1 = 0 ~: X coordinate on the screen
p2 = 0 ~: Y coordinate on the screen

%inst
Draws a 1-dot dot on the screen with the currently set drawing color.
If p1 and p2 are omitted, the coordinates of the current position set by the pos instruction will be used.

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
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 to 3 (1): Drawing mode setting
p2 = 0 to (0): Upper left X coordinate to redraw
p3 = 0 ~ (0): Upper left Y coordinate to be redrawn
p4 = 0 to (0): Redraw size X (dot unit)
p5 = 0 to (0): Redraw size Y (dot unit)

%inst
Specifies the screen drawing mode. What is drawing mode?
^
Drawing mode 0:
^p
Even if screen control commands such as mes, print, gcopy, and gzoom are executed, they only rewrite the virtual screen and are not reflected on the actual screen.
^p
Drawing mode 1:
^p
When the screen control command is executed, it is reflected on the actual screen.
^p
It has become. Normally, it is drawing mode 1.
You can copy an image on the screen in drawing mode 0, display a message, and finally set it to mode 1 to eliminate the flicker of screen rewriting and make it look smooth.
^
If mode 1 is specified, the screen will be updated immediately.
^
If you specify the mode value plus 2, only the drawing mode is changed and the screen is not updated.
^
In addition, it is possible to redraw only a part of the screen by using the parameters from p2 to p5. Normally, if omitted, the full screen will be updated.
%port+
Let
%sample
// Sample redraw instruction
// You can see the flicker by turning off the redraw command.

*main_loop
	redraw 0

	color 255, 255, 255
	boxf

	pos 0, 0
	color 0, 0, 0
mes "Characters do not flicker even after repeated drawing and erasing"

	redraw 1

	wait 1
	goto *main_loop


%index
screen
Initialize window
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1 = 0 to (0): Window ID
p2 = 1 to (640): Screen size to be initialized X (1 dot unit)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to (0): Screen mode to initialize
p5 = 0 to (-1): Window layout X (1 dot unit)
p6 = 0 to (-1): Window layout Y (1 dot unit)
p7 = 0 ~: Window size X (in 1 dot unit)
p8 = 0 ~: Window size Y (in 1 dot unit)
%inst
Initializes the specified window ID so that it can be used.
In the initial state of HSP, only the screen with window ID0 is used, but you can create a new window by specifying window ID1 or higher.
Windows with ID1 and above will have a style that can be freely resized as standard.
^
The screen mode to initialize p4 can be selected from the following.
^p
   0 (screen_normal): Create in full color mode
  +1 (screen_palette): Create in palette mode (256 out of 16.7 million colors)
  +2 (screen_hide): Create a hidden window
  +4 (screen_fixedsize): Fixed size window
  +8 (screen_tool): Tool window
 +16 (screen_frame): Window with deep edges
^p
If "+1" (created in palette mode) is specified for the p4 parameter, the screen will be created in 256-color mode with palette. If 0 is specified, a full-color (24-bit color) screen will be created.
^
If "+2" (creates a hidden window) is specified for the p4 parameter, the window will not be displayed at runtime, but it can be displayed with "gsel ID, 1".
^
Of the p4 parameters, "+4", "+8", and "+16" are for setting the window style, respectively.
A "fixed size window" is created in a style that cannot be resized even for windows with ID1 or later, similar to windows with ID0.
The "Tool Window" creates a window in which the title bar is reduced in size and the window name is not displayed on the taskbar.
^
If the p4 parameter is omitted, the current Windows screen mode will be created in palette mode if it has a palette (256 colors), otherwise it will be created as a full color screen.
^
You can set multiple screen mode values by summing the values.
for example,
^p
	screen 2,320,240,4+8
^p
Is created with a fixed size + tool window style.
^
The screen instruction can also reinitialize a window ID that has already been initialized with a different setting.
^p
example :
	screen 0,640,480,1
^p
In the above example, window ID 0, that is, the main window, is reinitialized in palette mode with 640x480 dots.
^
The p5 and p6 parameters allow you to specify the position of the window displayed on the desktop screen.
If p5 and p6 are omitted or set to a negative value, the system-speciî0¯¬«’