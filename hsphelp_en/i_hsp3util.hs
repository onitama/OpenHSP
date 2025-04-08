;
;	HELP source file for HSP help manager
;	(Lines starting with ; are treated as comments)
;

%type
Extended Command
%ver
3.6
%note
Include hsp3util.as.
%date
2009/08/01
%author
onitama
%dll
hsp3util
%url
http://hsp.tv/
%port
Win


%index
bmppalette
Read BMP image palette information
%group
Extended Screen Control Command
%prm
"filename"
"filename" : BMP image file name to extract the palette from

%inst
Reads the palette information contained in an 8-bit BMP image file and extracts it as an HSP palette.
Normally, the picload command in palette mode cannot read the palette information attached to the file.
This can be used when palette information in the file is required, such as with the hspdx extension plugin.
Only 8-bit BMP files can be specified as files.
Nothing will be done if other files or incorrect formats are specified.

%href
palette
getpal



%index
gettimestr
Get the current time as a string
%group
Extended Input/Output Control Command
%prm
p1
p1 : Variable name to get the string

%inst
Gets the current time and assigns it to a variable as a string in "hh:mm:ss" format.

%href
getdatestr



%index
getdatestr
Get the current date as a string
%group
Extended Input/Output Control Command
%prm
p1
p1 : Variable name to get the string

%inst
Gets the current date and assigns it to a variable as a string in "yyyy/mm/dd" format.

%href
gettimestr



%index
text
Set the waiting time for modified character display
%group
Extended Screen Control Command
%prm
p1
p1(0) : Display waiting time (ms)

%inst
Sets the waiting time for characters displayed by the emes command.
Specify the waiting time (in milliseconds) for each character with p1.
If a value of 0 or less is specified for p1, it will be displayed all at once without waiting for each character.
The text command is treated as an extended command using a module, not standard from HSP3.
Please note that unlike HSP2.x, the waiting time specified by the text command is reflected in the emes command.

%href
textmode
emes


%index
textmode
Set up modified character display
%group
Extended Screen Control Command
%prm
p1,p2
p1(0) : Modified character mode
p2(0) : Adjustment parameter

%inst
Sets the settings for modified characters displayed by the emes command.
Specify the mode with p1. The mode values are as follows:
^p
	Mode 0 : Normal display
	Mode 1 : Shadowed display
	Mode 2 : Outlined display
^p
If mode 1 or 2 is specified, the color set at the time the textmode command is executed will be used for the shadow or outline.
Also, you can correct the shadow or outline distance by setting an adjustment parameter to p2.
If the adjustment parameter is 0, it will be the minimum distance (1 dot), and the distance will increase as the value increases.

%href
text
emes



%index
emes
Display modified characters
%group
Extended Screen Control Command
%prm
"strings"
"strings" : Message or variable to display

%inst
Displays modified characters.
The emes command displays strings on the screen in the same way as the mes command, but at that time, the characters can be modified or displayed slowly one by one.
The settings for modified characters can be set with the textmode command.
Also, the display speed of modified characters can be specified with the text command.

%href
textmode
text




%index
gfade
Fades the screen
%group
Extended Screen Control Command
%prm
p1,p2,p3,p4,p5
p1(0) : Fade level (0-256)
(p2,p3) : X, Y coordinates of the upper left to fade
(p4,p5) : X, Y size to fade

%inst
Fades the specified area of the full-color screen with a fixed color.
This command is intended to change the fade level over time to perform fade-in, fade-out, etc.
Specify the fade level with p1. If it is 0, it will not affect the screen.
When p1 is 256, it will be completely filled with the specified color.
Specify the upper left X, Y coordinates to fade with (p2, p3). If omitted, (0,0) will be used.
Specify the X, Y size to fade with (p4, p5). If omitted, the entire screen size is used.

%href




%index
statictext
Place static text
%group
Extended Screen Control Command
%prm
"strings",p1,p2
"strings" : Static text string
p1(0) : X size
p2(0) : Y size

%inst
Generates static text as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
To change the content of the placed static text, use the statictext_set command.
Note that the objprm command cannot be used.

%href
statictext_set



%index
statictext_set
Change static text
%group
Extended Screen Control Command
%prm
p1,"strings"
p1(0) : Object ID
"strings" : Static text string

%inst
Changes the content of the static text placed by the statictext command.
You can specify the object ID in p1 and specify the string of the static text to be newly set in "strings".

%href
statictext




%index
scrollbar
Place a scroll bar
%group
Extended Screen Control Command
%prm
p1,p2
p1(0) : X size
p2(0) : Y size

%inst
Generates a scroll bar as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
This command supports generating a scroll bar as a placement object.
Messages to and from the placed scroll bar must be sent and received separately using the sendmsg command.
In that case, materials related to scroll bar messages of WIN32API are required separately.
Also, note that the objprm command cannot be used for the placed object.

%href





%index
progbar
Place a progress bar
%group
Extended Screen Control Command
%prm
p1,p2
p1(0) : X size
p2(0) : Y size

%inst
Generates a progress bar as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
To advance the placed progress bar, use the progbar_set and progbar_step commands.
Also, note that the objprm command cannot be used for the placed object.

%href
progbar_set
progbar_step



%index
progbar_set
Set the step increment of the progress bar
%group
Extended Screen Control Command
%prm
p1,p2
p1(0) : Object ID
p2(0) : Increment value

%inst
By specifying the object ID in p1, you can set the increment for each step of the progress bar placed by the progbar command.
The progress bar has a counter with values from 0 to 100 by default.
The increment is added to this for each step.
The default value is 10 if no increment is specified.

%href
progbar_step
progbar



%index
progbar_step
Advance the progress bar by one step
%group
Extended Screen Control Command
%prm
p1
p1(0) : Object ID

%inst
By specifying the object ID in p1, the progress bar placed by the progbar command advances by one step.
The increment for each step can be specified by the progbar_set command.

%href
progbar_set
progbar




%index
note2array
Convert multi-line string to array
%group
Extended Input/Output Control Command
%prm
p1,p2
p1 : Variable to assign the converted array
p2 : Variable in which the multi-line string is assigned

%inst
Divides a multi-line string into lines and assigns them to each element of the array variable.
Converts the multi-line string specified by p2 and assigns it to the variable specified by p1.
The variable p1 must be a string type one-dimensional array variable.

%href
array2note
arraysave
arrayload




%index
array2note
Convert array to multi-line string
%group
Extended Input/Output Control Command
%prm
p1,p2
p1 : Variable to assign the converted string
p2 : Variable in which the string is assigned to the array

%inst
Converts a string-type one-dimensional array variable into a multi-line string.
Converts the contents of the string-type one-dimensional array variable specified by p2 and assigns it to the variable specified by p1.

%href
note2array
arraysave
arrayload



%index
arraysave
Write string-type array variable to file
%group
Extended Input/Output Control Command
%prm
"filename",p1
"filename" : File name to write to
p1 : Variable to write from

%inst
Converts the string-type one-dimensional array variable specified by p1 into a multi-line string and writes it with the specified file name.
Only one-dimensional arrays can be handled.
The written file can be read using the arrayload command.

%href
note2array
array2note
arrayload



%index
arrayload
Load string-type array variable from file
%group
Extended Input/Output Control Command
%prm
"filename",p1
"filename" : File name to load from
p1 : Variable to load to

%inst
Loads the file written by the arraysave command into the variable specified by p1.
This command converts the lines of the multi-line string contained in the file into array elements and assigns them, and the array created is a one-dimensional array.


%href
note2array
array2note
arraysave
