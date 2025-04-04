;
;	HSP help manager用 HELPソースファイル
;	(Lines starting with ";" are treated as comments)
;

%type
Extended Command
%ver
3.7
%note
hsp3dish.as must be included.

%date
2022/06/02
%author
onitama
%dll
HSP3Dish
%url
http://hsp.tv/
%port
Win
%group
Extended Input/Output Control Commands


%index
setreq
Set System Request
%prm
type,val
type(0) : Setting type (SYSREQ_???)
val(0)  : Value to set
%inst
Sets various system configurations for the HSP3Dish runtime.
The following symbols can be specified for the setting type name.
^p
	Setting Type Name         Read/Write(*)   Content
---------------------------------------------------------------------------
	SYSREQ_MAXMODEL          RW            Maximum number of HGIMG3 models
	SYSREQ_MAXOBJ            RW            Maximum number of HGIMG3/4 objects
	SYSREQ_MAXTEX            RW            Maximum number of HGIMG3/4 textures
	SYSREQ_MAXMOC            RW            Maximum number of HGIMG3 MOCs
	SYSREQ_DXMODE            RW            DirectX mode (Windows only)
	SYSREQ_DXHWND            R             DirectX window handle (Windows only)
	SYSREQ_DXWIDTH           RW            DirectX full-screen X size (Windows only)
	SYSREQ_DXHEIGHT          RW            DirectX full-screen Y size (Windows only)
	SYSREQ_COLORKEY          RW            DirectX color key (Windows only)
	SYSREQ_RESULT            R             DirectX initialization result (Windows only)
	SYSREQ_RESVMODE          R             DirectX video initialization result (Windows only)
	SYSREQ_PKTSIZE           RW            Packet size (unused)
	SYSREQ_MAXEVENT          RW            Maximum number of HGIMG3/4 events
	SYSREQ_PTRD3D            R             Direct3D class pointer (Windows/HGIMG3 only)
	SYSREQ_PTRD3DDEV         R             Direct3DDevice class pointer (Windows/HGIMG3 only)
	SYSREQ_MDLANIM           RW            Maximum number of animations per model (HGIMG3)
	SYSREQ_CALCNORMAL        RW            Switch to recalculate X-file model normals (HGIMG3)
	SYSREQ_2DFILTER          RW            Texture interpolation mode for 2D drawing (HGIMG3)
	SYSREQ_3DFILTER          RW            Texture interpolation mode for 3D drawing (HGIMG3)
	SYSREQ_OLDCAM            RW            Camera focus mode operation (HGIMG3)
	SYSREQ_QUATALG           RW            X-file model animation interpolation mode (HGIMG3)
	SYSREQ_DXVSYNC           RW            VSYNC wait mode in full-screen mode (HGIMG3)
	SYSREQ_DEFTIMER          RW            hgsync time wait mode (HGIMG3)
	SYSREQ_NOMIPMAP          RW            Texture MIPMAP generation mode (HGIMG3)
	SYSREQ_DEVLOST           RW            DirectX device existence flag
	SYSREQ_MAXEMITTER        RW            Maximum number of HGIMG3 emitters
	SYSREQ_THROUGHFLAG       RW            X-direction border processing flag (HGIMG3)
	SYSREQ_OBAQMATBUF        RW            Number of material buffers for OBAQ (HGIMG3)
	SYSREQ_2DFILTER2         RW            Texture interpolation mode for 2D direct drawing (HGIMG3)
	SYSREQ_FPUPRESERVE       RW            FPU calculation precision setting option (HGIMG3)
	SYSREQ_CLSMODE           RW            Background clear flag
	SYSREQ_CLSCOLOR          RW            Background clear color
	SYSREQ_CLSTEX            RW            Background clear texture ID (HGIMG3)
	SYSREQ_TIMER             R             Timer value in milliseconds
	SYSREQ_PLATFORM          R             Platform running HSP3Dish
	SYSREQ_FPS               R             Frame rate (HGIMG4)
	SYSREQ_VSYNC             RW            VSync wait mode (HGIMG4)
	SYSREQ_MAXMATERIAL       RW            Maximum number of HGIMG4 materials
	SYSREQ_PHYSICS_RATE      RW            Physics parameter for OBAQ (OBAQ)
	SYSREQ_MAXOBAQ           RW            Maximum number of objects for OBAQ (OBAQ)
	SYSREQ_MAXLOG            RW            Maximum number of collision logs for OBAQ (OBAQ)
	SYSREQ_DEFAULT_WEIGHT    RW            Default Weight value for OBAQ (OBAQ)
	SYSREQ_DEFAULT_MOMENT    RW            Default Moment value for OBAQ (OBAQ)
	SYSREQ_DEFAULT_DAMPER    RW            Default Damper value for OBAQ (OBAQ)
	SYSREQ_DEFAULT_FRICTION  RW            Default Friction value for OBAQ (OBAQ)
	SYSREQ_MESCACHE_MAX      RW            Maximum number of message caches (HSP3Dish)
	SYSREQ_DLIGHT_MAX        RW            Maximum number of directional lights (HGIMG4)
	SYSREQ_PLIGHT_MAX        RW            Maximum number of point lights (HGIMG4)
	SYSREQ_SLIGHT_MAX        RW            Maximum number of spot lights (HGIMG4)
	SYSREQ_LOGWRITE          RW            Log output switch at exit (HGIMG4)
	SYSREQ_FIXEDFRAME        RW            Fixed frame rate switch (HGIMG4)
	SYSREQ_DRAWNUMOBJ        R             Number of objects drawn (HGIMG4)
	SYSREQ_DRAWNUMPOLY       R             Number of polygons drawn (HGIMG4)
	SYSREQ_USEGPBFONT        RW            gpb font usage switch (HGIMG4)
	SYSREQ_FIXMESALPHA       RW            Character drawing alpha value fixed mode (HSP3Dish)
	SYSREQ_OLDBOXF           RW            Traditional boxf drawing mode (HSP3Dish)

	(*) The availability of read(R) and write(W) differs for each item.
^p

・SYSREQ_CLSMODE, SYSREQ_CLSCOLOR

Sets whether to clear the screen when drawing starts with redraw 0. If SYSREQ_CLSMODE is set to 1, it will be cleared; if it is set to 0, it will not be cleared. If the screen is being cleared in the script, the processing speed will be improved if the system does not clear the background. The color code for clearing can be set with SYSREQ_CLSCOLOR. The color code should be set as a single numerical value combining RGB 24bit ($rrggbb).

・SYSREQ_TIMER

Returns the timer value in milliseconds. The value returned differs for each platform, but the value is incremented by 1 per millisecond.

・SYSREQ_PLATFORM

Returns the following HSP3Dish platform values.
^p
	Macro Name            Value  Content
	-------------------------------------------
	PLATFORM_WINDOWS    0   Windows version
	PLATFORM_IOS        1   iOS version
	PLATFORM_ANDROID    2   android version
	PLATFORM_WEBGL      3   hsp3dish.js (WebGL/JavaScript) version
	PLATFORM_LINUX      4   Linux version
	PLATFORM_RASPBIAN   5   Raspberry Pi (raspbian) version
^p

%href
getreq


%index
getreq
Get System Request
%prm
var,type
var  : Variable name to which the result is assigned
type : Setting type (SYSREQ_???)
%inst
Gets the system settings held by the HSP3Dish runtime and assigns them to a variable.
The names that can be specified for the setting type name are the same as for the setreq command.

%href
setreq


%index
gfilter
Texture Interpolation Settings
%prm
type
type : Setting type (FILTER_???)
%inst
Sets the texture interpolation (filtering) settings when the HSP3Dish runtime performs drawing.
The names that can be specified for the setting type name are as follows.
^p
	Macro Name        Value      Content
	-------------------------------------------
	FILTER_NONE     0       No interpolation (NEAREST)
	FILTER_LINEAR   1       Linear interpolation (LINEAR)
	FILTER_LINEAR2  2       Area interpolation (advanced interpolation processing)
^p
You can choose the dot interpolation method when enlarging or reducing images by setting the filtering. By specifying FILTER_NONE, it will be in a state of no interpolation (nearest). FILTER_LINEAR selects linear interpolation, and FILTER_LINEAR2 selects a more advanced interpolation method (such as bilinear). The settings of the gfilter command apply to all subsequent drawing commands. Also, the filtering setting is reset to FILTER_NONE when the screen is cleared with "redraw 0".

%href
gcopy
grotate
gsquare
celput


%index
mtlist
Get Point ID List
%prm
var
var  : Variable name to which the result is assigned
%inst
Gets a list of point IDs that have the currently touched information.
The point ID list returns multiple point IDs and contains information including multiple points due to multi-touch.
When the mtlist command is executed, all point IDs (integer values) that are currently pressed (touched) are assigned as an array to the variable specified by var.
For example, if there are three pieces of information: ID0, ID2, and ID3, the values 0, 2, and 3 will be assigned to var(0), var(1), and var(2), respectively.
The number of assigned point IDs is returned to the system variable stat.
You can get detailed touch information with the mtinfo command using the point ID obtained by this command.
In order to get multi-touch information, it is necessary to enable multi-touch settings on each device.
Also, on Windows, a device that supports multi-touch after Windows 7 is required.

%href
mtinfo


%index
mtinfo
Get Touch Information
%prm
var,id
var  : Variable name to which the result is assigned
id(0): Point ID
%inst
Gets the touch information associated with the specified point ID.
Touch information is assigned as an array to the variable specified by var.
^p
	Element Value  Content
	-------------------------------------------
	  0     Touch state (1=ON/0=OFF)
	  1     X coordinate of the touch
	  2     Y coordinate of the touch
	  3     Touch identification ID
^p
In other words, the X coordinate touched is assigned to var(1), and the touch state (1 if pressed) is assigned to var(0).
The variable specified by var is always initialized as an integer array variable.
The touch identification ID in var(3) is an integer value for identifying touches from each other. This value differs for each device.

%href
mtlist


%index
devinfo
Get Device Information String
%prm
var,"name"
var    : Variable name to which the result is assigned
"name" : Device information name
%inst
Gets information dependent on a specific OS and device.
The information indicated by the string "name" is assigned to the variable specified by var. (The information assigned is a string.)
After execution, the system variable stat will be 0 or more if the information is obtained correctly.
If the information does not exist or an error occurs, the system variable stat will be a negative value.

%href
devinfoi


%index
devinfoi
Get Device Information Value
%prm
var,"name"
var    : Variable name to which the result is assigned
"name" : Device information name
%inst
Gets information dependent on a specific OS and device.
The information indicated by the string "name" is assigned to the variable specified by var. (The information assigned is an integer array.)
After execution, the system variable stat will be 0 or more if the information is obtained correctly.
If the information does not exist or an error occurs, the system variable stat will be a negative value.

%href
devinfo


%index
devprm
Set Device Control Parameters
%prm
"name","value"
"name" : String indicating the parameter name
"value" : String to set
%inst
Sets parameters (additional information) when controlling the device with the devcontrol command.
After execution, the system variable stat will be a value of 0 or more if the execution is performed correctly.
If the parameter does not exist or an error occurs, the system variable stat will be a negative value.
%href
devcontrol

%index
devcontrol
Executes device control
%prm
"command",p1,p2,p3
"command" : A string indicating the command name
p1(0)     : Control parameter 1
p2(0)     : Control parameter 2
p3(0)     : Control parameter 3
%inst
Executes device control that depends on the specific OS and device.
After execution, the system variable stat will have a value of 0 or greater if the execution was successful.
If a parameter does not exist or an error occurs, the system variable stat will have a negative value.

%href
devprm

%index
setcls
Screen clear settings
%group
Extended multimedia control commands
%prm
mode,color
mode(0)  : Screen clear mode
color(0) : Screen clear color (24bit RGB code)
%inst
Sets up the screen clear that is performed during screen initialization with redraw 0.
The mode value setting sets the mode for clearing the screen.
If CLSMODE_SOLID or 1 is set, clearing is performed; if CLSMODE_NONE or 0 is set, clearing is not performed.
If the screen is being cleared within the script, processing speed can be improved by not clearing the background on the system side.
^p
	Mode Value  Definition Macro Name         Content
	-----------------------------------------------------------------
	    0     CLSMODE_NONE         Do not clear the screen
	    1     CLSMODE_SOLID        Clear the screen with the clear color
^p
The color used for clearing can be set with color. The color is set with a numerical value of 24bit RGB code ($rrggbb), similar to the parameters of the rgbcolor command.

%href
redraw
rgbcolor

%index
celputm
Draws multiple cells together
%group
Extended multimedia control commands
%prm
vxpos,vypos,vcelid,bufid,count
vxpos    : Integer array variable name containing display X coordinates
vypos    : Integer array variable name containing display Y coordinates
vcelid   : Integer array variable name containing cell IDs
bufid(0) : Buffer ID containing cells
count(1) : Number of items to retrieve from the array variable
%inst
Performs the same display as celput for each array element based on the coordinates and cell IDs assigned to integer array variables.
Because multiple items stored in an array are processed at once, this leads to efficiency and speed.
Specify the number of items to retrieve from the array variable with count. Values are retrieved sequentially from element 0 of the array variable.
vxpos must contain the X coordinates, vypos must contain the Y coordinates, and vcelid must contain the cell IDs.
If the cell ID is a negative value, drawing will not be performed.
After the celputm command is executed, the number of cells actually displayed is assigned to the system variable stat.

%href
celput

%index
httpload
Start http communication
%group
Extended input/output control commands
%prm
"url","postdata"
"url" : Filename to request
"postdata" : String data to POST

%inst
Starts http communication to the specified URL.
This command only starts the communication request and does not immediately retrieve the results.
The results of the communication must be obtained after confirming that the data has been received correctly using the httpinfo command.
^
httpload usually starts HTTP communication by specifying a URL scheme (a string like "http://www.onionsoft.net/about.html") with the "url" specification, which is the URL to make the request to.
^p
;Example:
;	Start communication by specifying a URL
httpload "http://www.onionsoft.net/about.html"
^p
To access CGI or WebAPI, specify the parameter string in "postdata" when communicating in POST format.
(If "postdata" is omitted, normal GET format HTTP communication is performed.)
^p
;Example:
;	Start communication in POST format by specifying a URL
httpload "http://www.onionsoft.net/hsp/beta/betabbs.cgi","mode=find&cond=and&log=0&word=script"
^p
POST format communication is intended for those with knowledge of CGI and http requests regarding the format and specifications of "postdata".
After the httpload command is executed, the system variable stat is assigned a numerical value indicating whether the request process was performed.
If the system variable stat is 0, the request has been completed correctly. If the system variable stat is other than 0, this indicates that the request could not be made due to an error.
After successfully starting http communication with the httpload command, it is necessary to perform reception confirmation and data acquisition processing by the script side using the httpinfo command.
.
^p
;Example:
	;	Start communication by specifying a URL
	httpload "http://www.onionsoft.net/about.html"
	if stat : goto *bad	; The request could not be made correctly
*main
	;	Loop for waiting for results
	httpinfo res,HTTPINFO_MODE		; Get current mode
	if res = HTTPMODE_READY : goto *ok	; Communication has ended
	if res <= HTTPMODE_NONE : goto *bad	; An error has occurred
	await 50				; Wait for a while
	goto *main
*bad
	;	Error
	httpinfo estr,HTTPINFO_ERROR		; Get the error string
	dialog "ERROR "+estr
	stop
*ok
	;	Completed
	httpinfo buf,HTTPINFO_DATA		; Get the result data
	httpinfo size,HTTPINFO_SIZE		; Get the data size
^p
The data obtained by http communication is stored in the memory of the variable.
Be careful because the memory is consumed by the size of the file when a large file is obtained.
Please understand that this is a simple communication mechanism that assumes a size that can be obtained on memory.
On Windows, more detailed HTTP communication is possible using the HSPINET plugin.

%href
httpinfo

%index
httpinfo
Get http communication information
%group
Extended input/output control commands
%prm
p1,p2
p1    : Variable to assign the data to
p2(0) : Data type ID

%inst
Gets information about http communication.
You can get the results of http communication using the httpload command.
Assign the content corresponding to the acquisition mode specified by p2 to the variable specified by p1.
The values for the acquisition mode are as follows:
^p
	  Type ID              Content
	------------------------------------------------------
	HTTPINFO_MODE 0       Current communication mode
	HTTPINFO_SIZE 1       Data size
	HTTPINFO_DATA 16      Acquired data (*)
	HTTPINFO_ERROR 17     Error string (*)

(*) items are assigned as string type variables.
^p
Please note that the content of the received data is cleared when reading the received data (HTTPINFO_DATA).
If an invalid type ID is specified, 0 or "" (empty string) is returned.
^
The values for the communication mode are as follows:
^p
	  Communication Mode Value        Content
	------------------------------------------------------
	HTTPMODE_NONE         Communication initialization error
	HTTPMODE_READY        Communication is possible
	HTTPMODE_REQUEST      Request communication preparation
	HTTPMODE_SEND         Request transmission
	HTTPMODE_DATAWAIT     Waiting for communication results
	HTTPMODE_DATAEND      Communication ended
	HTTPMODE_ERROR        Error occurred
^p

%href
httpload

%index
gmulcolor
Set texture multiplication value
%prm
p1,p2,p3
p1,p2,p3=0～255(255) : Color code (R, G, B brightness)
%inst
Specifies the color multiplication value when copying an image (texture).
p1, p2, and p3 are the brightness of R, G, and B, respectively.
The specified R, G, B values are multiplied for images copied by gcopy, gzoom, celput, grote(when using images), etc.
The initial values are RGB 255 respectively. By setting the texture multiplication value, you can change the color of the original image.
Note that the texture multiplication value is invalid if the copy mode specified by the gmode command is 0 or 1 (does not reflect the alpha channel).
Texture multiplication values are not reflected in text drawing with the mes command and the gsquare command. Also, it is not reflected in single color drawing such as boxf and line commands.

%href
color
gcopy
gzoom
grotate
gsquare
celput

%index
viewcalc
Set coordinate transformation for drawing
%prm
p1,p2,p3,p4,p5
p1(0)   : Setting mode
p2(0.0) : Parameter 1 (real number)
p3(0.0) : Parameter 2 (real number)
p4(0.0) : Parameter 3 (real number)
p5(0.0) : Parameter 4 (real number)
%inst
Applies arbitrary calculations to the X and Y coordinates during 2D drawing.
This allows you to perform scaling, movement, rotation, etc. of the entire drawn display.
Specify the setting mode with the p1 parameter.
The content of the subsequent settings will change depending on the setting mode.
^p
	Macro Name            Value   Content
	-----------------------------------------------------
	vptype_off           0   No coordinate transformation (default)
	vptype_translate     1   Specify movement parameters
	vptype_rotate        2   Specify rotation parameters
	vptype_scale         3   Specify scale parameters
	vptype_3dmatrix      4   Coordinate transformation by 4x4 matrix
	vptype_2d            5   Set 2D coordinate transformation
	vptype_3d            6   Set 3D coordinate transformation
^p
vptype_2d applies basic 2D coordinate transformation.
Parameters 1 to 3 can be used to set the X and Y scale (magnification) and rotation angle (in radians).
^p
	viewcalc vptype_translate, MoveX, MoveY
	viewcalc vptype_2d, ScaleX, ScaleY, Rotation Angle (radians)
^p
vptype_3d maps and applies drawing onto 3D space.
You can specify the X, Y, and Z values for each of movement, rotation, and scale.
^p
	viewcalc vptype_translate, MoveX, MoveY, MoveZ
	viewcalc vptype_rotate, RotateX, RotateY, RotateZ
	viewcalc vptype_scale, ScaleX, ScaleY, ScaleZ
	viewcalc vptype_3d, Camera Angle (FOV), NearZ value, FarZ value
^p
vptype_3dmatrix performs coordinate transformation by specifying a 4x4 matrix.
This can be used when creating the matrix yourself.
Specify parameters m00 to m33 of 4x4 as follows.
^p
	viewcalc vptype_translate, m00, m01, m02, m03
	viewcalc vptype_rotate, m10, m11, m12, m13
	viewcalc vptype_scale, m20, m21, m22, m23
	viewcalc vptype_3dmatrix, m30, m31, m32, m33
^p
Coordinate transformation is applied to the drawing after the viewcalc command is set.
When the screen is initialized, no coordinate transformation (vptype_off) is set.
Please note that when the transformation is set by vptype_3d and vptype_3dmatrix, it becomes impossible to correctly obtain the coordinates of the mouse.

%href
pos
mes

%index
celbitmap
Apply a variable buffer as image data
%prm
p1,var,p2
p1(0)   : Window ID
var     : Array variable to replace the image
p2(0)   : Operation options
%inst
Replaces the specified off-screen buffer with image data from a variable buffer.

You must initialize an off-screen buffer created with the buffer command and a variable buffer to replace it beforehand. The off-screen buffer must be created with the `screen_offscreen` option in the `buffer` command.

^p
	buffer 2,256,256,screen_offscreen
^p

Next, create a variable buffer to replace the buffer. Initialize and create a one-dimensional array using the `dim` command. The size to create should be the X size × Y size of the off-screen buffer.

^p
	dim bitmap, 256*256
^p

You are now ready to use the `celbitmap` command.

By specifying the ID of the off-screen buffer and the array variable with the `celbitmap` command, the contents of the variable buffer are applied as is as an image.

The variable buffer has one element per dot. One dot is RGB format data in which 32-bit elements are stored in 8-bit units for R, G, B, and A respectively.

For example, if you specify `$ff204080` (hexadecimal), then A=$ff(255), B=$20(32), G=$40(64), and R=$80(128) (the values in parentheses are decimal).

Replacing image data is a costly process, so be aware that the frame rate may drop if you replace a large image.

The p2 parameter can be used to specify the replacement format and behavior.

^p
 Value  Macro             Action
---------------------------------------------------------
  0  celbitmap_rgb      Holds data in $aabbggrr format
  1  celbitmap_bgr      Holds data in $aarrggbb format
  16 celbitmap_capture  Reads the contents of the drawing screen into a variable
^p

Normally, the `celbitmap_rgb` format, which conforms to the OpenGL standard, is used. If you specify the `celbitmap_bgr` option, it will be in the ARGB order, which conforms to the DirectX standard.

If `celbitmap_capture` is specified, the data on the drawing destination (ID 0) screen is read into the array variable. (The reading size will be the size of the off-screen buffer.)

*Note: The `celbitmap_capture` option is an experimental implementation. Currently, it can only be used on Windows (excluding DirectX version HGIMG4) and Linux versions.*

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
