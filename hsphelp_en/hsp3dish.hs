;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Extension instructions
%ver
3.6
%note
Include hsp3dish.as.

%date
2020/06/10
%author
onitama
%dll
HSP3Dish
%url
http://hsp.tv/
%port
Win
%group
Extended I / O control instructions


%index
setreq
System request settings
%prm
type,val
type (0): Setting type (SYSREQ_???)
val (0): Value to set
%inst
Make various system settings for the HSP3Dish runtime.
The following symbols can be specified by the setting type name.
^p
Setting type name Read / write (*) Contents
------------------------------------------------------------------------
SYSREQ_CLSMODE RW Background clear flag
SYSREQ_CLSCOLOR RW background clear color
SYSREQ_TIMER R Timer value in milliseconds
Platform running SYSREQ_PLATFORM R HSP3Dish

(*) Whether reading (R) or writing (W) is possible differs for each item.
^p

-SYSREQ_CLSMODE, SYSREQ_CLSCOLOR

Set whether to clear the screen when drawing by redraw 0 starts. If SYSREQ_CLSMODE is set to 1, it is cleared, and if it is 0, it is not cleared. If the screen is cleared in the script, it is better not to clear the background on the system side, which will improve the processing speed. You can set the color code for clearing with SYSREQ_CLSCOLOR. The color code is set with a single RGB 24bit ($ rrggbb).

ÅE SYSREQ_TIMER

Returns a timer value in milliseconds. The value returned will vary from platform to platform, but will be incremented by 1 per millisecond.

ÅE SYSREQ_PLATFORM

Returns the following HSP3Dish platform values.
^p
Macro name value Content
	-------------------------------------------
PLATFORM_WINDOWS 0 Windows version
PLATFORM_IOS 1 iOS version
PLATFORM_ANDROID 2 android version
PLATFORM_WEBGL 3 hsp3dish.js (WebGL / JavaScript) version
PLATFORM_LINUX 4 Linux version
PLATFORM_RASPBIAN 5 Raspberry Pi (raspbian) version
^p

%href
getreq


%index
getreq
Get system request
%prm
var,type
var: variable name to which the result is assigned
type: Setting type (SYSREQ_???)
%inst
Gets the system settings held by the HSP3Dish runtime and assigns them to variables.
The name that can be specified in the setting type name is the same as the setreq instruction.

%href
setreq


%index
gfilter
Texture interpolation settings
%prm
type
type: Setting type (FILTER_ ???)
%inst
Set the texture interpolation (filtering) when the HSP3Dish runtime draws.
The names that can be specified in the setting type name are as follows.
^p
Macro name value Content
	-------------------------------------------
FILTER_NONE 0 No interpolation (NEAREST)
FILTER_LINEAR 1 Linear interpolation (LINEAR)
FILTER_LINEAR2 2 Area interpolation (advanced interpolation processing)
^p
By setting the filtering, you can select the dot interpolation method when enlarging / reducing the image. By specifying FILTER_NONE, there is no interpolation (near rest). FILTER_LINEAR selects linear (linear) interpolation, and FILTER_LINEAR2 selects more advanced interpolation methods (such as bilinear). The settings of the gfilter instruction apply to all subsequent drawing instructions. Also, when the screen is cleared by "redraw 0", the filtering setting is reset to FILTER_NONE.

%href
gcopy
grotate
gsquare
celput


%index
mtlist
Get point ID list
%prm
var
var: variable name to which the result is assigned
%inst
Gets the point ID list with the information currently touched.
The point ID list returns multiple point IDs, and is information that includes multiple points by multi-touch.
When the mtlist instruction is executed, all the point IDs (integer values) pressed (touched) at that time are assigned as an array to the variable specified by var.
For example, if there are three pieces of information, ID0, ID2, and ID3, the values 0 for var (0), 2 for var (1), and 3 for var (2) will be assigned.
The number of assigned point IDs is returned in the system variable stat.
Using the point ID obtained by this command, detailed touch information can be obtained with the mtinfo command.
In order to get multi-touch information, you need to enable the multi-touch setting on each device.
Also, on Windows, a device that supports multi-touch on Windows 7 or later is required.

%href
mtinfo


%index
mtinfo
Get touch information
%prm
var,id
var: variable name to which the result is assigned
id (0): Point ID
%inst
Gets the touch information associated with the specified point ID.
The touch information is assigned as an array to the variable specified by var.
^p
Element value Content
	-------------------------------------------
0 Touch state (1 = ON / 0 = OFF)
1 Touched X coordinate
2 Touched Y coordinate
3 Touch identification ID
^p
That is, var (1) is assigned the touched X coordinate, and var (0) is assigned the touch state (if 1 is pressed).
The variable specified by var is always initialized as an integer type array variable.
The touch identification ID of var (3) is an integer value to distinguish it from other touches. This value varies from device to device.

%href
mtlist


%index
devinfo
Get device information string
%prm
var,"name"
var: variable name to which the result is assigned
"name": device information name
%inst
Acquires information that depends on a specific OS and device.
Assigns the information indicated by the character string "name" to the variable specified by var. (The information to be assigned is a character string.)
If the information is acquired correctly after execution, the system variable stat will be 0 or higher.
If the information does not exist or an error occurs, the system variable stat will have a negative value.

%href
devinfoi


%index
devinfoi
Get device information value
%prm
var,"name"
var: variable name to which the result is assigned
"name": device information name
%inst
Acquires information that depends on a specific OS and device.
Assigns the information indicated by the character string "name" to the variable specified by var. (The information to be assigned is an integer type array.)
If the information is acquired correctly after execution, the system variable stat will be 0 or higher.
If the information does not exist or an error occurs, the system variable stat will have a negative value.

%href
devinfo


%index
devprm
Parameter settings for device control
%prm
"name","value"
"name": A string indicating the parameter name
"value": string to set
%inst
Set the parameters (additional information) when controlling the device with the devcontrol command.
After execution, if the execution is successful, the system variable stat will be 0 or more.
If the parameter does not exist or an error occurs, the system variable stat will have a negative value.

%href
devcontrol


%index
devcontrol
Perform device control
%prm
"command",p1,p2,p3
"command": A string indicating the command name
p1 (0): Control parameter 1
p2 (0): Control parameter 2
p3 (0): Control parameter 3
%inst
Performs device control depending on the specific OS and device.
After execution, if the execution is successful, the system variable stat will be 0 or more.
If the parameter does not exist or an error occurs, the system variable stat will have a negative value.

%href
devprm


%index
setcls
Screen clear setting
%group
Extended multimedia control instructions
%prm
mode,color,tex
mode (0): Screen clear mode
color (0): Screen clear color (24bit RGB code)
%inst
Make settings for clearing the screen when the screen is initialized by redraw 0.
Set the mode for clearing the screen by setting the mode value.
If 1 is set, it will be cleared, and if it is 0, it will not be cleared.
If the screen is cleared in the script, it is better not to clear the background on the system side to improve the processing speed.
^p
Mode value Contents
	------------------------------------------------------
0 Do not clear the screen
1 Clear the screen
^p
You can set the color when clearing with color. The color is set by the numerical value of 24bit RGB code ($ rrggbb).


%href
redraw


%index
celputm
Draw multiple cells together
%group
Extended multimedia control instructions
%prm
vxpos,vypos,vcelid,bufid,count
vxpos: Integer array variable name containing the display X coordinates
vypos: Integer array variable name containing the display Y coordinates
vcelid: Integer array variable name containing the cell ID
bufid (0): Buffer ID where the cell is stored
count (1): Number of items to retrieve from the array variable
%inst
The same display as celput is executed for each array element based on the coordinates and cell ID assigned to the integer type array variable.
Multiple items stored in the array are processed at once, which leads to efficiency and speed.
Specify the number of items to be fetched from the array variable with count. Values are retrieved from the array variable in order from element 0.
vxpos must be assigned the X coordinate, vypos must be assigned the Y coordinate, and vcelid must be assigned the cell ID.
If the cell ID is a negative value, no drawing will be performed.
After executing the celputm instruction, the number of cells actually displayed is assigned to the system variable stat.

%href
celput


%index
httpload
http communication started
%group
Extended I / O control instructions
%prm
"url","postdata"
"url": File name to make the request
"postdata": Character string data when performing POST

%inst
Starts http communication for the specified URL.
This instruction only initiates a communication request and does not get the result immediately.
The communication result must be obtained after confirming that the data has been received correctly using the httpinfo instruction.
^
httpload normally starts HTTP communication by specifying a URL scheme (a character string such as "http://www.onionsoft.net/about.html") that makes a request by specifying "url". ..
^p
;Example:
; Specify the URL to start communication
httpload "http://www.onionsoft.net/about.html"
^p
When communicating in POST format to access CGI, WebAPI, etc., specify the parameter character string in "postdata".
(If "postdata" is omitted, HTTP communication will be performed in the normal GET format.)
^p
;Example:
; Specify the URL to start POST format communication
httpload "http://www.onionsoft.net/hsp/beta/betabbs.cgi","mode=find&cond=and&log=0&word=script"
^p
POST format communication is provided for those who are familiar with CGI and http requests regarding the format and specifications of "postdata".
After the httpload instruction is executed, the system variable stat is assigned a numerical value indicating whether the request has been processed.
If the system variable stat is 0, the request is completed correctly. If the system variable stat is non-zero, it means that the request could not be made due to an error.
After normally starting http communication with the httpload instruction, it is necessary to perform reception confirmation and data acquisition processing with the httpinfo instruction on the script side.
..
^p
;Example:
; Specify the URL to start communication
	httpload "http://www.onionsoft.net/about.html"
if stat: goto * bad; Could not request correctly
*main
Loop to wait for results
httpinfo res, HTTPINFO_MODE; Get current mode
if res = HTTPMODE_READY: goto * ok; Communication has ended
if res <= HTTPMODE_NONE: goto * bad; error occurred
await 50; wait for time
	goto *main
*bad
Error
httpinfo estr, HTTPINFO_ERROR; Get the error string
	dialog "ERROR "+estr
	stop
*ok
Done
httpinfo buf, HTTPINFO_DATA; Get result data
httpinfo size, HTTPINFO_SIZE; Get the data size
^p
The data acquired by http communication is stored in the memory of the variable.
Please note that if you get a large file, the memory will be consumed by that size.
Please understand that it is a simple communication mechanism that assumes the size of the range that can be acquired in the memory.
On Windows, the HSPINET plug-in enables more detailed HTTP communication.

%href
httpinfo


%index
httpinfo
Get http communication information
%group
Extended I / O control instructions
%prm
p1,p2
p1: Variable to which data is assigned
p2 (0): Data type ID

%inst
Get information about http communication.
You can get the result of http communication by the httpload command.
Assigns the content corresponding to the acquisition mode specified in p2 to the variable specified in p1.
The values of the acquisition mode are as follows.
^p
Type ID Contents
	------------------------------------------------------
HTTPINFO_MODE 0 Current communication mode
HTTPINFO_SIZE 1 data size
HTTPINFO_DATA 16 Acquired data (*)
HTTPINFO_ERROR 17 Error string (*)

Items in (*) are assigned as string type variables.
^p
Note that the contents of the received data will be cleared when the received data (HTTPINFO_DATA) is read.
If an invalid type ID is specified, 0 or "" (empty string) is returned.
^
The communication mode values are as follows.
^p
Communication mode value Contents
	------------------------------------------------------
HTTPMODE_NONE Communication initialization error
HTTPMODE_READY Communicable status
HTTPMODE_REQUEST Request communication preparation
HTTPMODE_SEND Request transmission
HTTPMODE_DATAWAIT Waiting for communication result
HTTPMODE_DATAEND Communication end
HTTPMODE_ERROR error occurred
^p

%href
httpload



%index
gmulcolor
Setting the texture multiplication value
%prm
p1,p2,p3
p1, p2, p3 = 0 to 255 (255): Color code (brightness of R, G, B)
%inst
Specifies the color multiplication value when copying an image (texture).
p1, p2, and p3 are the brightness of R, G, and B, respectively.
The specified R, G, B values are multiplied by the image copied by gcopy, gzoom, celput, grottate (when using the image).
The initial value is 255 for each of RGB. You can change the color of the original image by setting the texture multiplication value.
Note that when copying an image, if the copy mode specified by the gmode instruction is 0 or 1 (does not reflect the alpha channel), the texture multiplication value will be invalid.
The texture multiplication value is not reflected in the drawing of characters by the mes instruction and the gsquare instruction. Also, it is not reflected in single color drawing such as boxf and line commands.

%href
color
gcopy
gzoom
grotate
gsquare
celput




%index
viewcalc
Set coordinate transformation at the time of drawing
%prm
p1,p2,p3,p4,p5
p1 (0): Setting mode
p2 (0.0): Parameter 1 (real number)
p3 (0.0): Parameter 2 (real number)
p4 (0.0): Parameter 3 (real number)
p5 (0.0): Parameter 4 (real number)
%inst
Applies arbitrary calculations to the X and Y coordinates when drawing in 2D.
This allows you to scale, move, rotate, etc. the entire rendered display.
Specify the setting mode with the p1 parameter.
Subsequent settings will change depending on the setting mode.
^p
Macro name value Content
	-----------------------------------------------------
vptype_off 0 No coordinate transformation (default)
vptype_translate 1 Specify movement parameters
vptype_rotate 2 Specify rotation parameter
vptype_scale 3 Specify scale parameter
vptype_3dmatrix 4 Coordinate transformation by 4 Å~ 4 matrix
vptype_2d 5 Set 2D coordinate transformation
vptype_3d 6 Set 3D coordinate transformation
^p
vptype_2d applies a basic 2D coordinate transformation.
Parameters 1 to 3 allow you to set the X and Y scales (magnification) and rotation angle (in radians).
^p
viewcalc vptype_translate, move X, move Y
viewcalc vptype_2d, scale X, scale Y, rotation angle (radians)
^p
vptype_3d maps and applies drawing in 3D space.
You can specify the X, Y, and Z values for move, rotate, and scale respectively.
^p
viewcalc vptype_translate, move X, move Y, move Z
viewcalc vptype_rotate, rotate X, rotate Y, rotate Z
viewcalc vptype_scale, scale X, scale Y, scale Z
viewcalc vptype_3d, camera angle (FOV), NearZ value, FarZ value
^p
vptype_3dmatrix performs coordinate transformation by specifying a 4x4 matrix.
You can use it if you want to create your own matrix.
Specify the 4x4 parameters from m00 to m33 as follows.
^p
	viewcalc vptype_translate, m00, m01, m02, m03
	viewcalc vptype_rotate, m10, m11, m12, m13
	viewcalc vptype_scale, m20, m21, m22, m23
	viewcalc vptype_3dmatrix, m30, m31, m32, m33
^p
Coordinate transformation is applied to the drawing after setting the viewcalc command.
When the screen is initialized, no coordinate conversion (vptype_off) is set.
Please note that if the conversion is set by vptype_3d and vptype_3dmatrix, the mouse coordinates cannot be acquired correctly.

%href
pos
mes


%index
celbitmap
Apply variable buffer as image data
%prm
p1,var,p2
p1 (0): Window ID
var: Array variable to replace the image
p2 (0): Operation option
%inst
Replaces the contents of the variable buffer with the offscreen buffer specified as image data.
It is necessary to initialize the offscreen buffer created by the buffer instruction and the variable buffer to replace it in advance.
Create an offscreen buffer by specifying the screen_offscreen option with the buffer instruction.
^p
	buffer 2,256,256,screen_offscreen
^p
Then create a variable buffer to replace the buffer.
Initialize the one-dimensional array with the dim command and create it. The size to be created is X size x Y size of the offscreen buffer.
^p
	dim bitmap, 256*256
^p
Now you are ready to use the celbitmap instruction.
By specifying the ID of the offscreen buffer and the array variable with the celbitmap instruction, the contents of the variable buffer are applied as they are as an image.
The variable buffer is 1 dot per element. One dot is RGB format data in which 32-bit elements are stored in 8-bit units for each of R, G, B, and A.
For example, if you specify "$ ff204080" (hex number), A = $ ff (255), B = $ 20 (32), G = $ 40 (64), R = $ 80 (128). (Decimal numbers in parentheses)
Please note that replacing image data is a costly process and may reduce the frame rate when replacing large size images.
The p2 parameter allows you to specify the replacement format and behavior.
^p
 Value macro behavior
---------------------------------------------------------
  0 Hold data in celbitmap_rgb $ aabbggrr format
  1 Hold data in celbitmap_bgr $ aarrggbb format
  16 celbitmap_capture Read the contents of the drawing screen into a variable
^p

Normally, the format of celbitmap_rgb that conforms to the OpenGL standard is used. If celbitmap_bgr is specified as an option, the ARGB sequence will be in accordance with the DirectX standard.
When celbitmap_capture is specified, the data on the drawing destination screen is read into the array variable. (The read size will be the size of the offscreen buffer)
* The celbitmap_capture option is an experimental implementation. Currently, it can be used only on Windows (excluding DirectX version HGIMG4) and Linux version.


%sample
#include "hsp3dish.as"

	buffer 2,256,256,screen_offscreen
	gsel 0
	dim bitmap,256*256
	repeat 256*256
	bitmap(cnt)=$ff00ffff
	loop
*main
	redraw 0
	celbitmap 2,bitmap
	pos 0,0
	celput 2
	redraw 1
	await 1000/30
	goto *main
%href
buffer


