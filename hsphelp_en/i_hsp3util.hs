;
;HELP source file for HSP help manager
;(Lines beginning with; are treated as comments)
;

%type
Extension instructions
%ver
3.4
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
Read bmp image palette information
%group
Extended screen control command
%prm
"file name"
"File name": BMP image file name to extract the palette

%inst
Reads the palette information included in the 8-bit format bmp image file and retrieves it as an HSP palette.
Normally, when loading an image in palette mode using the picload command, the palette information attached to the file cannot be loaded.
It can be used when you need palette information in a file, such as the hspdx extension plugin.
Only 8BIT format BMP files can be specified as files.
If any other file or incorrect format is specified, nothing will be done.

%href
palette
getpal



%index
gettimestr
Get the current time as a string
%group
Extended I / O control instructions
%prm
p1
p1: Variable name to get the character string

%inst
Gets the current time and assigns it to the variable as a string format of "hh: mm: ss".

%href
getdatestr



%index
getdatestr
Get the current date as a string
%group
Extended I / O control instructions
%prm
p1
p1: Variable name to get the character string

%inst
Gets the current date and assigns it to a variable in the string format "yyyy / mm / dd".

%href
gettimestr



%index
text
Set the waiting time for displaying qualified characters
%group
Extended screen control command
%prm
p1
p1 (0): Display wait time (ms)

%inst
Sets the waiting time for characters displayed by the emes instruction.
Specify the waiting time (in milliseconds) for each character with p1.
If you specify a value of 0 or less for p1, it will be displayed all at once without waiting for each character.
The text instruction is treated as an extension instruction using a module instead of the standard from HSP3.
Note that unlike HSP2.x, the waiting time specified by the text instruction is reflected in the emes instruction.

%href
textmode
emes


%index
textmode
Set modifier character display
%group
Extended screen control command
%prm
p1,p2
p1 (0): Modifier mode
p2 (0): Adjustment parameters

%inst
Set the modifier characters displayed by the emes command.
Specify the mode with p1. The mode values are as follows.
^p
Mode 0: Normal display
Mode 1: Shadowed display
Mode 2: Contoured display
^p
When modes 1 and 2 are specified, the color set at the time of executing the textmode command is used for shadows and contours.
You can also modify the shadow and contour distances by setting adjustment parameters on p2.
If the adjustment parameter is 0, the minimum distance (1 dot) is set and the distance increases as the value increases.

%href
text
emes



%index
emes
Show modifier characters
%group
Extended screen control command
%prm
"strings"
"strings": Messages or variables to display

%inst
Displays modifier characters.
The emes command displays a character string on the screen in the same way as the mes command, but at that time, the characters can be modified or displayed slowly one character at a time.
You can use the textmode instruction to set modifier characters.
In addition, the display speed of modifier characters can be specified with the text instruction.

%href
textmode
text




%index
gfade
Fade the screen
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 (0): Fade level (0 to 256)
(p2, p3): Upper left X, Y coordinates to fade
(p4, p5): X, Y size to fade

%inst
Performs a fixed color fade on the specified area for a full-color screen.
This command is for fading in, fading out, etc. by changing the fade level every hour.
Specify the fade level with p1. If it is 0, there is no effect on the screen.
When p1 is 256, it will be completely filled with the specified color.
Specify the upper left X and Y coordinates to fade with (p2, p3). If omitted, it will be (0,0).
Specify the X and Y sizes to fade with (p4, p5). If omitted, the size of the entire screen is used.

%href




%index
statictext
Place static text
%group
Extended screen control command
%prm
"strings",p1,p2
"strings": Static text strings
p1 (0): X size
p2 (0): Y size

%inst
Generates static text as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
You can specify the size of the object to be placed with (p1, p2).
If you want to change the contents of the placed static text, use the statictext_set instruction.
Note that the objprm instruction cannot be used.

%href
statictext_set



%index
statictext_set
Change static text
%group
Extended screen control command
%prm
p1,"strings"
p1 (0): Object ID
"strings": Static text strings

%inst
Change the content of static text placed by the statictext instruction.
You can specify the object ID in p1 and specify the newly set static text string in "strings".

%href
statictext




%index
scrollbar
Place scroll bar
%group
Extended screen control command
%prm
p1,p2
p1 (0): X size
p2 (0): Y size

%inst
Generates a scroll bar as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
You can specify the size of the object to be placed with (p1, p2).
This instruction supports creating a scrollbar as a placement object.
Sending and receiving messages to the placed scroll bar must be performed separately with the sendmsg command.
In that case, a separate document related to the scroll bar message of WIN32API is required.
Also note that the objprm instruction cannot be used for placed objects.

%href





%index
progbar
Place a progress bar
%group
Extended screen control command
%prm
p1,p2
p1 (0): X size
p2 (0): Y size

%inst
Generates a progress bar as a placement object from the current current position.
The placed object ID is assigned to the system variable stat.
You can specify the size of the object to be placed with (p1, p2).
Use the progbar_set and progbar_step instructions to advance the placed progress bar.
Also note that the objprm instruction cannot be used for placed objects.

%href
progbar_set
progbar_step



%index
progbar_set
Set the progress bar step increment
%group
Extended screen control command
%prm
p1,p2
p1 (0): Object ID
p2 (0): Incremental value

%inst
By specifying the object ID in p1, you can set the step-by-step increment of the progress bar placed by the progbar instruction.
The progress bar has a value from 0 to 100 as a counter by default.
On the other hand, the increment is added step by step.
If you do not specify an increment, the default value is 10.

%href
progbar_step
progbar



%index
progbar_step
Take the progress bar one step
%group
Extended screen control command
%prm
p1
p1 (0): Object ID

%inst
By specifying the object ID in p1, the progress bar placed by the progbar instruction is advanced by one step.
The step-by-step increment can be specified with the progbar_set command.

%href
progbar_set
progbar




%index
note2array
Convert multi-line string to array
%group
Extended I / O control instructions
%prm
p1,p2
p1: Variable to assign the converted array
p2: Variable to which a multi-line character string is assigned

%inst
Divide a multi-line string into lines and assign them to each element of the array variable.
Converts the multi-line character string specified in p2 to the variable specified in p1 and assigns it.
The variable of p1 is always a character string type one-dimensional array variable.

%href
array2note
arraysave
arrayload




%index
array2note
Convert an array to a multi-line string
%group
Extended I / O control instructions
%prm
p1,p2
p1: Variable that substitutes the converted character string
p2: Variable whose string is assigned to the array

%inst
Converts a one-dimensional array variable of character string type to a multi-line character string.
Converts the contents of the character string type one-dimensional array variable specified by p2 and assigns it to the variable specified by p1.

%href
note2array
arraysave
arrayload



%index
arraysave
Write a string type array variable to a file
%group
Extended I / O control instructions
%prm
"Filename", p1
"File name": File name to export
p1: Original variable to export

%inst
Converts the character string type one-dimensional array variable specified in p1 to a multi-line character string, and then writes it out with the specified file name.
Only one-dimensional arrays can be handled.
The exported file can be read by the arrayload command.

%href
note2array
array2note
arrayload



%index
arrayload
Read a string type array variable from a file
%group
Extended I / O control instructions
%prm
"Filename", p1
"File name": The name of the file to be read
p1: Variable to read

%inst
The file written by the arraysave command is read into the variable specified by p1.
This instruction converts a line of a multi-line character string contained in a file into an array element and substitutes it, and the created array array becomes a one-dimensional array.


%href
note2array
array2note
arraysave




