;
; HELP source file for HSP help manager
; (Lines starting with ; are treated as comments)
;

%type
Extended command
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
Extended screen control commands
%prm
"Filename"
"Filename": The BMP image filename from which to extract the palette.

%inst
Reads the palette information contained in an 8-bit BMP image file and retrieves it as an HSP palette.
Normally, the picload command in palette mode cannot read the palette information attached to the file.
This command can be used when file palette information is required, such as with the hspdx extended plugin.
Only 8-bit BMP files can be specified as files.
If any other file or an incorrect format is specified, nothing is performed.

%href
palette
getpal



%index
gettimestr
Get the current time as a string
%group
Extended input/output control commands
%prm
p1
p1: The variable name to store the string.

%inst
Retrieves the current time and assigns it to a variable as a string in "hh:mm:ss" format.

%href
getdatestr



%index
getdatestr
Get the current date as a string
%group
Extended input/output control commands
%prm
p1
p1: The variable name to store the string.

%inst
Retrieves the current date and assigns it to a variable as a string in "yyyy/mm/dd" format.

%href
gettimestr



%index
text
Set the waiting time for decorated text display
%group
Extended screen control commands
%prm
p1
p1(0): Display waiting time (ms)

%inst
Sets the waiting time for characters displayed by the emes command.
Specify the waiting time (in milliseconds) for each character with p1.
If p1 is set to a value less than or equal to 0, it will be displayed at once without waiting for each character.
The text command is treated as an extended command using a module instead of being standard from HSP3.
Please note that, unlike HSP2.x, the waiting time specified by the text command is reflected in the emes command.

%href
textmode
emes


%index
textmode
Set decorated text display
%group
Extended screen control commands
%prm
p1,p2
p1(0): Decoration character mode
p2(0): Adjustment parameter

%inst
Sets the decoration characters displayed by the emes command.
Specify the mode with p1. The mode values are as follows:
^p
Mode 0: Normal display
Mode 1: Shadowed display
Mode 2: Outlined display
^p
If mode 1 or 2 is specified, the color set at the time of executing the textmode command will be used for shadows or outlines.
Also, you can modify the shadow or outline distance by setting the adjustment parameter to p2.
If the adjustment parameter is 0, the minimum distance (1 dot) is used, and the distance increases as the value increases.

%href
text
emes



%index
emes
Display decorated characters
%group
Extended screen control commands
%prm
"strings"
"strings": Message or variable to display

%inst
Displays decorated characters.
The emes command displays strings on the screen in the same way as the mes command, but at that time, the characters can be decorated or displayed slowly one by one.
The decoration character settings can be made with the textmode command.
Also, the display speed of decoration characters can be specified with the text command.

%href
textmode
text




%index
gfade
Fade the screen
%group
Extended screen control commands
%prm
p1,p2,p3,p4,p5
p1(0): Fade level (0-256)
(p2,p3): X,Y coordinates of the upper left corner to fade
(p4,p5): X,Y size to fade

%inst
Fades a specified area of the full-color screen with a fixed color fade.
This command is for changing the fade level over time to perform fade-in, fade-out, etc.
Specify the fade level with p1. If it is 0, it has no effect on the screen.
When p1 is 256, it is completely filled with the specified color.
Specify the X,Y coordinates of the upper left corner to fade with (p2, p3). If omitted, (0,0) is used.
Specify the X,Y size to fade with (p4, p5). If omitted, the entire screen size is used.

%href




%index
statictext
Place static text
%group
Extended screen control commands
%prm
"strings",p1,p2
"strings": Static text string
p1(0): X size
p2(0): Y size

%inst
Generates static text as a placement object from the current current position.
The ID of the placed object is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
To change the contents of the placed static text, use the statictext_set command.
Note that the objprm command cannot be used.

%href
statictext_set



%index
statictext_set
Change static text
%group
Extended screen control commands
%prm
p1,"strings"
p1(0): Object ID
"strings": Static text string

%inst
Changes the contents of the static text placed with the statictext command.
You can specify the object ID in p1 and specify the string of the static text to be newly set in "strings".

%href
statictext




%index
scrollbar
Place a scroll bar
%group
Extended screen control commands
%prm
p1,p2
p1(0): X size
p2(0): Y size

%inst
Generates a scroll bar as a placement object from the current current position.
The ID of the placed object is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
This command supports generating scroll bars as placement objects.
Message transmission/reception to/from the placed scroll bar must be performed separately by the sendmsg command.
In that case, documentation on WIN32API scroll bar messages is required separately.
Also, note that the objprm command cannot be used for the placed object.

%href





%index
progbar
Place a progress bar
%group
Extended screen control commands
%prm
p1,p2
p1(0): X size
p2(0): Y size

%inst
Generates a progress bar as a placement object from the current current position.
The ID of the placed object is assigned to the system variable stat.
The size of the placed object can be specified by (p1, p2).
To advance the placed progress bar, use the progbar_set and progbar_step commands.
Also, note that the objprm command cannot be used for the placed object.

%href
progbar_set
progbar_step



%index
progbar_set
Set the progress bar step increment
%group
Extended screen control commands
%prm
p1,p2
p1(0): Object ID
p2(0): Increment value

%inst
By specifying the object ID in p1, you can set the increment for each step of the progress bar placed by the progbar command.
The progress bar has a value from 0 to 100 as a counter by default.
The increment is added to this for each step.
The default value when the increment is not specified is 10.

%href
progbar_step
progbar



%index
progbar_step
Advance the progress bar by one step
%group
Extended screen control commands
%prm
p1
p1(0): Object ID

%inst
By specifying the object ID in p1, you can advance the progress bar placed by the progbar command by one step.
The increment for each step can be specified by the progbar_set command.

%href
progbar_set
progbar




%index
note2array
Convert multi-line string to array
%group
Extended input/output control commands
%prm
p1,p2
p1: Variable to assign the converted array
p2: Variable in which the multi-line string is assigned

%inst
Divides a multi-line string into lines and assigns them to each element of the array variable.
The multi-line string specified in p2 is converted and assigned to the variable specified in p1.
The variable p1 is always a string-type one-dimensional array variable.

%href
array2note
arraysave
arrayload




%index
array2note
Convert array to multi-line string
%group
Extended input/output control commands
%prm
p1,p2
p1: Variable to assign the converted string
p2: Variable in which the string is assigned to the array

%inst
Converts a string-type one-dimensional array variable into a multi-line string.
The contents of the string-type one-dimensional array variable specified in p2 are converted and assigned to the variable specified in p1.

%href
note2array
arraysave
arrayload



%index
arraysave
Write string-type array variable to file
%group
Extended input/output control commands
%prm
"Filename",p1
"Filename": Filename to write to
p1: Variable to write from

%inst
The string-type one-dimensional array variable specified in p1 is converted to a multi-line string and written to the specified filename.
Only one-dimensional arrays can be handled.
The written file can be read by the arrayload command.

%href
note2array
array2note
arrayload



%index
arrayload
Load string-type array variable from file
%group
Extended input/output control commands
%prm
"Filename",p1
"Filename": Filename to read from
p1: Variable to read to

%inst
Reads the file written by the arraysave command into the variable specified by p1.
This command converts the lines of the multi-line string contained in the file into array elements and assigns them, and the created array is a one-dimensional array.


%href
note2array
array2note
arraysave
