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
The message is displayed from a virtual cursor position on the window called the current position. (The current position can be set by the pos instruction.)
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
(This option is only valid for HSP3Dish and HGIM G4, ignored by normal HSP3 runtime)
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
The message is displayed from a virtual cursor position on the window called the current position. (The current position can be set by the pos instruction.)
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
(This option is only valid for HSP3Dish and HGIM G4, ignored by normal HSP3 runtime)
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
If p1 is omitted, the window title bar caption is set to the contents of "strings".
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
The continuation of the HSP script will not be executed until the dialog is closed by the user.
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
Example:
dialog "txt", 16, "text file"
dialog "STAT =" + stat + "\\ nNAME =" + refstr, 0, "Result"
	stop
^p
When the file selection is complete, the result is assigned to the system variable stat.
If the variable stat is 1, it means that it was selected successfully. If the variable stat is 0, it means that it was canceled or an error occurred.
Multiple file types can be specified by separating them with the "|" symbol.
^p
Example:
dialog "txt | log", 16, "text file | log file"
dialog "STAT =" + stat + "\\ nNAME =" + refstr, 0, "Result"
	stop
^p
For types 32 to 33, a color selection dialog is displayed. When the color selection is completed, the RGB data of the selected color is assigned to the system variables ginfo_r, ginfo_g, ginfo_b. Also, if the variable stat is 0, it means that it was canceled or an error occurred. If it is 1, it means that it was selected successfully.
^p
Example:
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
p2 = 1 to (640): Screen size X to be initialized (in 1-dot units)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to 1 (0): Screen mode to initialize
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
"filename": Filename to save

%inst
Save the current screen image as it is as an image file in BMP format.
A file will be created with the name specified in "filename". You need to specify the file name including the extension.
^
The saved image size will be the size that the target window is initialized. If you want to change the size, please copy the contents to a window of another size and then bmp save.




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
p2 = 1 to (640): Screen size X to be initialized (in 1-dot units)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to 1 (0): Screen mode to initialize

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
Use (p2, p3) to set the X and Y resolutions, and p1 to set the change mode.
The mode is 1 for full color mode (32bit) and 2 for palette mode (8bit). If the change mode is 0, it returns to the initial state (before change).
^p
Example:
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
The window ID of the copy source must be the same as your own window ID or the window ID initialized by the screen or buffer instruction.
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
揃 Mode 0 = normal copy
^p
Copy the image using the Windows system API (BitBlt function).
This is the most standard mode.
It is often faster than other modes.
^p
揃 Mode 1 = memory-to-memory copy
^p
Makes a memory copy between image buffers.
The copy source and copy destination must be in the same image mode (palette mode / full color mode).
The operation is the same as mode 0, but it may be slightly faster than mode 0 when copying a very small size (16 * 16 dots or less, etc.).
^p
揃 Mode 2 = transparent colored copy
^p
Copying is done in the same way as in mode 1, but dots with all 0 RGB (complete black) are considered transparent and are not copied.
^p
・ Mode 3 = Semi-transparent composite copy
^p
It can be used only in full color mode.
Performs a translucent composite copy with the blend ratio specified by the 4th parameter of gmode. The blend ratio can range from 0 to 256, with 256 being a perfect copy.
If the blend ratio is 0, no copy will be executed.
Also, if the blend ratio is 256 or more, memory-to-memory copy is executed.
^p
・ Mode 4 = Translucent composite copy with transparent color
^p
Copying is done in the same way as in mode 3, but dots with RGB values that are the same as the color set by the color command are considered transparent and are not copied.
If the blend ratio is 0, no copy will be executed.
^p
・ Mode 5 = Color Additive Synthesis Copy
^p
It can be used only in full color mode.
Performs color additive synthesis copying with the blend ratio specified by the 4th parameter of gmode. In color addition, the RGB value of the copy source is added to the RGB value of the copy destination.
If the brightness exceeds 255 at the time of addition, it will be clamped to 255.
^p
繝サ Mode 6 = Color subtractive composite copy
^p
It can be used only in full color mode.
Performs color subtractive composite copy with the blend ratio specified by the 4th parameter of gmode. In color subtraction, the RGB value of the copy source is subtracted from the RGB value of the copy destination. If the brightness is less than 0 when subtracting, it will be clamped to 0.
^p
ツキ Mode 7 = Pixel Alpha Blend Copy
^p
It can be used only in full color mode.
Prepare an image of the size specified by gmode on the right side of the original image, and perform background compositing copying using the image on the right side as an alpha blend component.
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
The window ID of the copy source must be the same as your own window ID or the window ID initialized by the screen or buffer instruction.
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
PNG format: Extension PNG: Portable Network Graphics data. (*)
PSD format: Extension PSD: Photoshop format data. (Composite layer only) (*)
TGA format: Extension TGA: Data in TARGA format. (*)
^p
The picload instruction can be executed on the screen initialized by the screen, buffer, and bgscr instructions.
* Multi-icon format ICO files are not supported as standard.
* Interlaced PNG files are not supported as standard. If you want to load all formats supported by PNG, please use the imgload or hspcv plugin.
* Please note that the formats marked with (*) cannot be used with the compact runtime (hsp3c) and HSPLet.

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
p1 = 0 ~: X coordinate of the current position
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
It has become. Normally it is drawing mode 1.
You can copy an image on the screen in drawing mode 0, display a message, and finally set it to mode 1 to eliminate the flicker of screen rewriting and make it look smooth.
^
If mode 1 is specified, the screen will be updated immediately.
^
If you specify the mode value plus 2, only the drawing mode is changed and the screen is not updated.
^
In addition, it is possible to redraw only a part of the screen by the parameters from p2 to p5. Normally, if omitted, the full screen will be updated.
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
p2 = 1 to (640): Screen size X to be initialized (in 1-dot units)
p3 = 1 to (480): Screen size Y to initialize (1 dot unit)
p4 = 0 to 1 (0): Screen mode to initialize
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
The "Tool Window" creates a window in which the title bar is reduced in size and the window name is not displayed in the taskbar.
^
If the p4 parameter is omitted, the current Windows screen mode will be created in palette mode if it has a palette (256 colors), otherwise it will be created as a full color screen.
^
You can set multiple screen mode values by summing the values.
For example
^p
	screen 2,320,240,4+8
^p
Is created with a fixed size + tool window style.
^
The screen instruction can also reinitialize a window ID that has already been initialized with a different setting.
^p
Example:
	screen 0,640,480,1
^p
In the above example, window ID 0, that is, the main window, is reinitialized in palette mode with 640x480 dots.
^
The p5 and p6 parameters allow you to specify the position of the window displayed on the desktop screen.
If p5 and p6 are omitted or set to a negative value, the system-specified coordinates will be used.
If you want to display only a part of the screen size specified by the p2 and p3 parameters in the window,
You can specify the client size (the size that is actually displayed) of the window with the parameters of p7 and p8.
In that case, the groll command can be used to set the position displayed in the window.
Normally, you can omit the p7 and p8 parameters. In that case, the client size is the same as the screen size specified by p2 and p3.
^
After initializing the screen with the screen command, the window ID becomes the drawing destination of the screen control command.

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
At HSPLet, palette mode, tool window, and window with deep edge cannot be used.



%index
width
Window size setting
%group
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to (-1): Client area size X (1 dot unit)
p2 = 0 to (-1): Client area size Y (1 dot unit)
p3 = 0 to (-1): Window X coordinates on the display (in 1-dot units)
p4 = 0 to (-1): Window Y coordinate on the display (in 1 dot unit)

%inst
Change the client size (actually displayed size) of the window and the window display position.
The client size cannot be larger than the screen size initialized by the screen instruction, buffer, or bgscr instruction.
^
If the p1, p2 and p3, p4 parameters are omitted or have negative values, the current settings are used.

%href
screen
buffer
bgscr
%port+
Let



%index
sysfont
System font selection
%group
Screen control instructions
%prm
p1
p1 = 0 ~: Font type specification

%inst
Select a system standard font.
Use p1 to specify the font type. Choose from the following:
^p
   p1 : font set
 --------------------------------------------------------------------
    0: HSP standard system font
   10: Fixed width font for OEM character set
   11: Fixed width system font for Windows character set
   12: Variable width system font for Windows character set
   13: Standard system font
   17: Default GUI font
        (Used for menus and dialog boxes in the standard environment)
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
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to (0): Line end point X coordinate
p2 = 0 to (0): Line end point Y coordinate
p3 = 0 ~: Line start point X coordinate
p4 = 0 ~: Line start point Y coordinate

%inst
Draw a straight line connecting (p1, p2) and (p3, p4) on the screen.
If (p3, p4) is omitted, a straight line will be drawn from the current position to (p1, p2) in the currently set color.
^
After executing the line instruction, the coordinates of (p1, p2) become the current position.
This makes it possible to draw a continuous straight line.

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
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 to (0): Upper left X coordinate of the rectangle
p2 = 0 to (0): Upper left Y coordinate of the rectangle
p3 = 0 ~: Lower right X coordinate of the rectangle
p4 = 0 ~: Lower right Y coordinate of the rectangle
p5 = 0 to 1 (1): Drawing mode (0 = line, 1 = fill)

%inst
Draw a circle on the screen that fits in a rectangle (square) with (p1, p2) and (p3, p4) as the upper left and lower right points.
The drawing color will be the one set by the color command, etc.
^
If 0 is specified for p5, only the outline will be drawn as a line.
If p5 is 1 or omitted, it also fills the inside of the circle.

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
Screen control instructions
%prm
p1
p1: System color index to set

%inst
Sets the system color index specified in p1 as the selected color.
(It will be in the same state as specified by the color command)
The system color index that can be specified with p1 is as follows.
^p
 0: Scroll bar axis color
 1: Desktop color
 2: The color of the title bar of the active window (the color on the left side of the title bar of the active window for gradients)
 3: Color of text in the title bar of the inactive window (color to the left of the title bar of the inactive window for gradients)
 4: Menu background color
 5: Window background color
 6: Window frame color
 7: Text color in menu
 8: Text color in the window
 9: Text color in the title bar of the active window
10: Active window border color
11: Border color of inactive window
12: Background color of MDI application
13: Color of the selected item in the control
14: Text color of the selected item in the control
15: Surface color of 3D object
16: Shadow color of 3D object (for the opposite edge of the light source)
17: Light-colored (disabled) text color
18: Push button text color
19: Text color in the title bar of the inactive window
20: Brightest color of 3D object (for edges in the direction of the light source)
21: Dark shadow color of 3D object
22: Bright colors of 3D objects (for edges in the direction of the light source)
23: Tooltip control text color
24: Tooltip control background color
26: Hot track item color
27: Color on the right side of the title bar of the active window for gradients
28: Color to the right of the title bar of the inactive window for gradients
29: (Windows XP and above) The color used to highlight menu items when the menu is displayed as a flat menu
30: (Windows XP and above) Menu bar background color when the menu is displayed as a flat menu
^p

%href
color
palcolor
hsvcolor
%port+
Let



%index
hsvcolor
Set colors in HSV format
%group
Screen control instructions
%prm
p1,p2,p3
p1 = 0 to 191 (0): HSV format H value
p2 = 0 to 255 (0): HSV format S value
p3 = 0 to 255 (0): HSV format V value

%inst
Specify the color in HSV format.
As with the color command, the color set in the subsequent drawing commands is used.

%href
color
palcolor
%port+
Let


%index
rgbcolor
Set color in RGB format
%group
Screen control instructions
%prm
p1
p1 = $ 000000 ~ $ ffffff (0): RGB format color code value

%inst
Specify the color in RGB format ($ rrggbb).
Specify the value obtained by converting the brightness of each RGB with an 8-bit value in p1.
You can write something like "$ 112233" using the "$" that specifies the hexadecimal number.
In this case, "R = $ 11, G = $ 22, B = $ 33" will be specified.
The function of the command is the same as the color command and hsvcolor command, and the color set by the subsequent drawing commands is used.
The RGB format color code value specified by rgbcolor is compatible with the 24-bit color code specified by html etc.

%href
color
palcolor
hsvcolor



%index
ginfo
Get window information
%group
Basic input / output functions
%prm
(p1)
p1 = 0 ~: Type to get

%inst
Returns the window-related information value of the type specified in p1.
The types that can be obtained are as follows.
^p
  0: Mouse cursor X coordinates on the screen
  1: Mouse cursor Y coordinate on the screen
      The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.

  2: Active window ID

      Returns the currently active window ID.
      If the active window is other than HSP, it will be -1.

  3: Operation destination window ID

      The operation destination window ID of the screen specified by the gsel instruction is assigned.

  4: Upper left X coordinate of the window
  5: Upper left Y coordinate of the window

    Returns the upper left coordinate of the current window in the screen coordinate system.
    The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.

  6: Lower right X coordinate of the window
  7: Lower right Y coordinate of the window

      Returns the lower right coordinates of the current window in the screen coordinate system.
      The screen coordinate system is the coordinates seen from the entire desktop screen, not the coordinates in the window used by the pos command.

  8: Window drawing base X coordinate
  9: Window drawing base Y coordinate

      Returns the drawing base coordinate of the current destination window.
      This is a value that indicates how much the drawing content is scrolled in a scrollable window (display size is smaller than the initialization size).
      In the normal state without scrolling, it will be (0,0).
      To set the drawing base point, use the groll instruction.

 10: X size of the entire window
 11: Y size of the entire window

      The width and height of the current operation window are returned.
      This is the size including the window frame, title bar, etc.

 12: Client area X size
 13: Client area Y size

      Returns the client area size of the current destination window.
      The client area refers to the drawable part displayed in the window.

 14: Message output X size
 15: Message output Y size

      Finally, the size of the message output by the mes and print instructions is returned.
      The size indicates how many dots were drawn on the X and Y coordinates.
      Please note that it is not the absolute coordinates on the screen.
      Also, if you output a character string with multiple lines, the size corresponding to the last line will be acquired.

 16: Currently set color code (R)
 17: Currently set color code (G)
 18: Currently set color code (B)

      The color code specified by the color command etc. is returned.

 19: Desktop color mode

      Returns the current desktop color mode (color mode).
      Returns 0 for full color mode and 1 for palette mode.

 20: X size of the entire desktop
 21: Y size of the entire desktop

      Returns the size of the entire desktop (screen resolution).

 22: X coordinate of the current position
 23: Y coordinate of the current position

      The X and Y coordinates of the current position set by the pos instruction are returned.

 24: Window ID at the time of message interrupt

      The window ID at the time of message interrupt set by the oncmd instruction is returned.

 25: Unused window ID

      An unused window ID that has not been initialized by the screen instruction or buffer instruction is returned.

 26: Screen initialization X size
 27: Screen initialization Y size

      Returns the initialization size of the current destination window.
      First, the screen size is initialized by the screen, bgscr, and buffer instructions.

 256: Accelerometer value (X)
 257: Accelerometer value (Y)
 258: Accelerometer value (Z)

      Get the sensor value in an environment where the accelerometer is available.
      The acceleration of each coordinate axis is obtained as a real number (m / s ^ 2).
      In an environment where the accelerometer cannot be used, 0.0 is obtained.

 259: Gyro sensor value (X)
 260: Gyro sensor value (Y)
 261: Gyro sensor value (Z)

      Obtain the sensor value in an environment where the gyro sensor can be used.
      The slope of each coordinate axis is obtained as a real number (rad / s).
      In an environment where the gyro sensor cannot be used, 0.0 is obtained.
^p

%href
objinfo
%port+
Let
%portinfo
At HSPLet, type 19, screen color mode always returns 0.
At HSPLet, type 24, oncmd interrupt window IDs are not supported.
Type 25 is not supported when HSPLet.


%index
grect
Fill with a rotating rectangle
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6
p1 = 0 to (0): Rectangle center X coordinate
p2 = 0 to (0): Y coordinate of the center of the rectangle
p3 = 0 to (0.0): Rotation angle (unit is radian)
p4 = 0 ~ (?): X size
p5 = 0 ~ (?): Y size

%inst
Draws a rectangle (rectangle) of the size specified in (p4, p5) with the currently set color centered on the coordinates specified in (p1, p2).
You can specify the rotation angle as a real number with p3.
The unit of angle is radians (starting from 0 and going around at 2π).
The grect instruction reflects the copy mode specification set in gmode.
^
If gmode is 0,1, normal fill.
If gmode is 3, it will be translucent at the specified rate.
When gmode is 5 or 6, color addition and color subtraction are performed respectively.
If the size specification of (p4, p5) is omitted, the copy size set by the gmode instruction is used.
^
 In palette mode, the specified drawing color is used for filling, but translucency processing is not performed.

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
Rotate and copy rectangular image
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6
p1 = 0 to (0): Copy source window ID
p2 = 0 to (0): Upper left X coordinate of copy source
p3 = 0 to (0): Upper left Y coordinate of copy source
p4 = 0 to (0.0): Rotation angle (unit is radian)
p5 = 0 ~ (?): X size
p6 = 0 ~ (?): Y size

%inst
The grotate instruction performs processing including rotation in the gcopy instruction.
Specify the window ID of the copy source with p1, and specify the coordinates corresponding to the original image to be copied with (p2, p3). (Similar to the gcopy instruction)
The copy destination is the window ID specified as the current drawing destination, and the copy is performed to the coordinates centered on the location set by the pos command. At that time, you can specify the rotation angle with a real number with p4.
The unit of angle is radians (starting from 0 and going around at 2π).
In (p5, p6), specify the X and Y sizes after copying.
Also, the default copy size set by the gmode instruction is used for the X and Y sizes of the copy source.
In other words, if you specify a size larger than the size specified by the gmode instruction with (p5, p6), it will be expanded.
If (p5, p6) is omitted, the copy will be performed in the same size as the copy source, that is, at the same size.
^
The grotate instruction reflects the copy mode specification set in gmode. (Mode 7 pixel alpha blend copy does not apply.)
The transparent color setting and the translucent setting all work in the same way as when copying with the gcopy command. (See the gmode instruction reference for details)
^
Semi-transparency processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full color mode) will result in an error.

%href
grect
gsquare
gmode
%port+
Let


%index
gsquare
Draw any rectangle
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 to (0): Copy source window ID (filled if negative value)
p2 = variable name: Numeric array variable name where the copy destination X coordinates are stored
p3 = variable name: Numeric array variable name where the copy destination Y coordinates are stored
p4 = variable name: Numeric array variable name where the copy source X coordinates are stored
p5 = variable name: Numeric array variable name where the copy source Y coordinate is stored

%inst
Copy or fill a rectangle with any 4 points.
In p1, specify the window ID of the image copy source.
^
For p2 and p3, specify the variables that store the four coordinates of the copy destination in the array.
For p4 and p5, specify the variables that store the four coordinates of the copy source in the array.
It is necessary to put the coordinates in the order of upper left (0), upper right (1), lower right (2), lower left (3) in the array variable.
^
The gsquare instruction reflects the copy mode specification set in gmode. (Mode 7 pixel alpha blend copy does not apply.)
The transparent color setting and the translucent setting all work in the same way as when copying with the gcopy command. (See the gmode instruction reference for details)
^
Semi-transparency processing is not executed in palette mode.
Also, copying between different screen modes (palette mode and full color mode) will result in an error.
^
If you specify a negative value such as -1 for p1, the image will be filled instead of being copied.
For the fill, you can select a single color fill or a gradation fill.
If you specify a value from -1 to -256 for p1, it will be filled with a single color.
It is not necessary to specify the p4 and p5 parameters when performing a single color fill.
At that time, in full color mode, it will be filled with the drawing color.
In palette mode, the value -1 specified in p1 is used as the palette code to fill. (-1 to -256 correspond to palette codes 0 to 255)
^
If you specify -257 or gsquare_grad (macro definition of -257) for p1, the gradient fill is executed.
When performing a gradation fill, specify an array variable that stores the RGB code indicating the color of the vertices in the p4 parameter.
(The RGB code will be the same as the color code specified by the gradf instruction.)
Please note that the gradation fill does not reflect the copy mode specification set in gmode and always draws opaquely.


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
Basic input / output functions
%prm
(p1,p2)
p1 = 0 ~: Window object ID
p2 = 0 ~: Type to get

%inst
Returns the information value associated with a particular window object that is placed in the currently active window.
By specifying the window object ID in p1 and the acquisition type in p2, the corresponding information data is returned.
The types that can be obtained are as follows.
^p
     0: Mode (lower 16 bits) and optional data (upper 16 bits)
     1: Pointer to the BMSCR structure where the object is located
     2: Window object handle (HWND)
     3: owid (internal general purpose data)
     4: owsize (internal general purpose data)
  5-8: Substitute variable information
 9-11: Window object callback information
    12: Background brush handle (HBRUSH)
    13: Background color information (RGB)
    14: Character color information (RGB)
15-16: Additional information specific to the object (exinfo1, exinfo2)
^p
Window object information does not need to be used in normal use.
It can be used for special purposes such as passing the handle (HWND) of a window object to an external API (DLL).

%href
ginfo
objinfo_bmscr
objinfo_hwnd
objinfo_mode
%port+
Let
%portinfo
At HSPLet, only the window handle (type == 2) is available.


%index
axobj
Placement of ActiveX controls
%group
Screen control instructions
%prm
p1,"IID",p2,p3
p1: Variable name to which the COM object is assigned
"IID": COM object class ID or program ID
p2 = 0 ~: X size of object
p3 = 0 ~: Y size of object


%inst
Places an ActiveX control (OLE control) with the class ID specified by "IID" or the program ID (ProgID) as a window object.
Specifies the X and Y sizes of the objects placed on p2 and p3.
If the size specification is omitted or less than or equal to 0, the size of the entire window is used.
If the placement is successful, the object ID is assigned to the system variable stat and the variable p1 is initialized as a COM object type variable for control.
If the placement of the ActiveX control fails, the system variable stat is assigned -1 and exits.
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
Use "atl.dll" to deploy the ActiveX control.
In an environment where "atl.dll" is not installed, the "Selected unsupported feature" error is displayed.



%index
winobj
Arrangement of window objects
%group
Screen control instructions
%prm
"Class","Title",p1,p2,p3,p4,p5,p6
"Class": window class name
"Title": window name
p1 (0): Extended window style
p2 (0): Window style
p3 (0): X size
p4 (0): Y size
p5 (0): Menu handle or child window ID
p6 (0): Window creation data

%inst
winobj arranges windows supported by the Windows system as objects.
Specify the X and Y sizes to be placed in p3 and p4. If the size specification is omitted or less than or equal to 0, the value set in objsize is used.
^
When the specified window object is created, the object ID is assigned to the system variable stat.
This object ID can be used in object manipulation instructions such as the clrobj instruction.
(The generated window is registered as an object managed by the currently active window.)
If the window object is not created normally due to an internal error, -1 is assigned to the system variable stat.
^
You cannot use the objprm instruction on objects placed by winobj.
^
This instruction is for arranging window objects that are not supported by HSP instructions through the Windows API, and receives the same parameters as CreateWindowEx of WIN32API. It is used for interacting with external DLLs and creating modules, and usually does not need to be remembered.



%href
pos
objsize
clrobj



%index
sendmsg
Send window message
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 (0): Window handle
p2 (0): Message ID
p3 (0): wParam value
p4 (0): lParam value

%inst
Sends the message of p2 and the parameters of p3 and p4 to the window handle specified by p1.
^
The value of the window handle specified by p1 can be obtained by using the system variable hwnd for HSP windows and the objinfo function for HSP window objects.
^
A window message is sent with p3 as wParam and p4 as lParam information.
The p3 parameter can be an integer value.
The p4 parameter can be either an integer value or a character string.
For integer values, treat the value itself as lParam.
For strings, treat the string pointer value as lParam.
^
This instruction is for sending Windows messages to window objects that HSP instructions do not support.
It is used for interacting with external DLLs and creating modules, and usually does not need to be remembered.

%href
hwnd
objinfo
winobj
%port+
Let
%portinfo
At the time of HSPLet, only the following messages have been implemented.
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
Set the drawing base and scale of the window
%group
Screen control instructions
%prm
p1,p2,p3,p4
p1 = 0 to (0): X coordinate that is the base point to draw in the window
p2 = 0 to (0): Y coordinate that is the base point to draw in the window

%inst
Sets the base point coordinates for drawing the graphic surface in the window.
The base point coordinates set which coordinates are displayed in the window on the drawn graphic surface.
By changing the coordinates, it can be displayed from any position on the graphic surface.
If you set the base point coordinates to (0,0), they will be displayed in the window in a form that matches the drawn coordinates.
The currently set base point coordinates can be obtained by ginfo_vx, ginfo_vy.

%href
ginfo_vx
ginfo_vy



%index
gradf
Fill the rectangle with a gradient
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5,p6,p7
p1 = 0 to (0): Upper left X coordinate of the rectangle
p2 = 0 to (0): Upper left Y coordinate of the rectangle
p3 = 0 ~: Rectangle X size
p4 = 0 ~: Y size of rectangle
p5 = 0 to (0): Gradation mode
p6 = 0 ~: Fill color 1 (RGB color code)
p7 = 0 ~: Fill color 2 (RGB color code)

%inst
With (p1, p2) as the upper left point, fill a rectangle (square) with a gradation of the size of (p3, p4).
The fill color will be the RGB color code specified by p6 and p7.
The RGB color code is a 24-bit numerical value that collectively expresses the brightness of each of R, G, and B specified by the color command.
When writing in a script, it is convenient to use the hexadecimal representation with "$" at the beginning.
If you write something like "$ 112233", the R brightness will be $ 11, the G brightness will be $ 22, and the B brightness will be $ 33. In this case, the brightness is from $ 00 to $ ff. (Corresponds to decimal numbers 0 to 255)
You can specify the gradation mode with p5.
^p
   p5: Gradient mode
 --------------------------------------------------------------------
    0: Horizontal gradation
    1: Vertical gradation
^p
Gradient filling is done with the color specified by p6 in the upper left and p7 in the lower right.
If p6 and p7 are omitted, the current drawing color will be used.
If (p1, p2) is omitted, the upper left (0,0) of the screen is set.
If (p3, p4) is omitted, the screen drawing size will be set.
^p
Example:
; Fill the rectangle with a gradient
	gradf 120,180,400,100, 1, $ff00ff, $ffffff
^p
If you want to fill the rectangle with a single color, use the boxf instruction.
Also, if you want to draw a gradation by specifying the color of each vertex of the rectangle, use the gsquare command.

%href
boxf
gsquare



%index
celload
Load image file into buffer
%group
Screen control instructions
%prm
"filename",p1,p2
"filename": Filename to load
p1 = 1 ~ (-1): Read destination window ID
p2 = 0 to 1 (0): Screen mode to initialize

%inst
Loads the image file into the specified virtual screen.
The celload command mainly uses the celput command and gcopy command to copy image material.
It is for loading into a virtual screen (hidden window).
Initialize the virtual screen with the buffer command and load the image file with the picload command.
Is basically the same as the operation.
^
In the "filename" parameter, specify the name of the image file to read.
The format of the image file that can be used is the same as the picload command.
You can specify the window ID of the read destination with p1.
If p1 is omitted or a negative value, an unused window ID is automatically used.
(After executing the instruction, the window ID read in the system variable stat is assigned.)
You can specify the initialization mode of the virtual screen with p2.
Full color mode if p2 is omitted or 0. If 1, palette mode is selected.
^
Image materials can be efficiently read and managed by the celload command.
For more information, see About CEL-related instructions in the programming manual (hspprog.htm).

%href
picload
buffer
celdiv
celput


%index
celdiv
Set the division size of the image material
%group
Screen control instructions
%prm
p1,p2,p3,p4,p5
p1 = 0 ~ (1): Window ID
p2 = 1 ~ (0): Horizontal division size
p3 = 1 ~ (0): Vertical division size
p4 = 0 to (0): Horizontal center coordinates
p5 = 0 to (0): Vertical center coordinates

%inst
Sets the division size of the image material that the specified window ID has.
The divided image material is referenced when drawing the image with the celput command.
For example, if you set an image of 256 x 256 dots to be divided by 128 x 64 dots,
The celput command makes it possible to handle eight 128 x 64 dot images.
^
In p1, specify the window ID that holds the image material.
In (p2, p3), specify the division size (number of dots per divided area) in the horizontal (X) and vertical (Y) directions.
If you omit the split size or set it to a value less than or equal to 0, the size of the material is used as is.
^
You can set the center coordinates of drawing with (p4, p5).
This will be the base point position for drawing when drawing with the celput command.
For example, if (0,0) is specified as the center coordinate, the image material (0,0) will be referenced at the position drawn by the celput command (coordinates specified by the pos command).
It also becomes the center of rotation when drawing a rotated image.
Normally, the position at (0,0), that is, the upper left is the base point position. This is similar to the base point position in the gcopy instruction.

^
The image material division settings are saved for each window ID and initialized by the cls command, etc.
At this point, it is set to no split (specify the entire screen as the size).
Be sure to set the division with the image material loaded in the specified window ID.
If you load the image material after setting the division, the division setting will be reset.
For more information, see About CEL-related instructions in the programming manual (hspprog.htm).

%href
celload
celput


%index
celput
Draw image material
%group
Screen control instructions
%prm
id,no,zoomx,zoomy,angle
id = 0 ~ (1): Window ID with image material
no = 0 ~ (0): Divided image No.
zoomx = 0.0 ~ (1.0): Horizontal display magnification (real number)
zoomy = 0.0 ~ (1.0): Vertical display magnification (real number)
angle = 0.0 ~ (0.0): Rotation angle (unit is radian)

%inst
The image material loaded on the virtual screen is drawn in the current operation destination window.
In id, specify the window ID that has the image material.
Specify the split image number with no. The split image No. is the window ID that has the image material.
It is a number that identifies the divided image.
With zoomx and zoomy, you can specify the horizontal (X) and vertical (Y) direction magnifications of the drawn image.
You can specify zoomx and zoomy with real numbers.
It is also possible to specify the rotation angle of the image by angle.
The value specified for angle is a real number and the unit is radians (starting from 0 and going around in 2π). (It will be the same value as the grottate and grect instructions)
^
The celput command copies and draws the image with the specified window ID in the same way as the gcopy command.
As with the gcopy instruction, the copy mode and blend ratio specified by the gmode instruction are reflected.
The drawing position is based on the current current position (value specified by the pos instruction).
The size of the rendered image is usually the same as the original image (at the specified window ID).
The size of the image to be drawn and the center position of drawing and rotation can be changed by the celdiv command.
^
If zoomx, zoomy, angle are omitted or the same size is set (1,1,0),
A fast copy is automatically performed internally. On the contrary, when drawing that is not the same size,
The same drawing process as the grotate command is executed.
After drawing, move the current position to the right according to the drawn size.
(However, the angle is not taken into consideration. The horizontal size of the drawing is moved to the right.)
^
Image materials can be managed efficiently by the celput command and related commands.
For more information, see About CEL-related instructions in the programming manual (hspprog.htm).

%href
celload
celdiv
gcopy
grotate




