;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
Built-in command
%ver
3.6
%note
ver3.6 standard command
%date
2020/06/04
%author
onitama
%url
http://hsp.tv/
%port
Win
Let


%index
button
Display button
%group
Object control command
%prm
goto/gosub "name",*label
"name" : Button name
*label : Label to jump to when pressed

%inst
Places a push button as an object at the current position.
The button is labeled with the string specified by "name", and when the button is clicked with the mouse, program control is transferred to the location specified by *label.
^
If you write button goto, it will jump to the label. If you write button gosub, it will perform a subroutine jump to the label. If the goto or gosub keyword is omitted, it will behave the same as goto.
^p
Example:
	button gosub "Button",*aaa  ; Create a button that calls *aaa
^p
^
The size of the object can be specified with the objsize command. When a button is placed, the current position automatically moves to the next line.
^
In the program that jumps and executes when the button is pressed, the object ID is assigned as the initial value of the system variable stat.
^
Normally, the button shape provided by the Windows system is used.
However, if custom button settings have been made using the objimage command, you can have a free appearance.
For custom button settings, refer to the objimage command section.


%href
objsize
objimage



%index
chkbox
Display checkbox
%group
Object control command
%prm
"strings",p1
"strings" : Checkbox content display string
p1=variable   : Variable that holds the state of the checkbox

%inst
Places a checkbox as an object at the current position.
The checkbox is an object with a switch that can be turned ON/OFF with the cursor to the left of the string specified by "strings".
^
The size of the checkbox is the size specified by objsize, and the background is gray.
^
If the content of the numerical variable specified by p1 is 0, the check is OFF, and if it is 1, it is ON. When the ON/OFF of the check is changed, the content of the variable also changes simultaneously.

%href
objsize




%index
clrobj
Clear object
%group
Object control command
%prm
p1,p2
p1=0〜(0)   : Object ID to erase (start)
p2=-1〜(-1) : Object ID to erase (end) (If -1, the final ID is specified)

%inst
Erases objects that were created with the button command or the mesbox command.
^
If p1 and p2 are omitted and only clrobj is executed, all objects on the screen will be erased. If you want to erase only some objects, specify the first ID in p1 and the last ID in p2 to erase only the objects from p1 to p2. The object ID to specify is the same as the ID specified by the objprm command, etc.
^
The deleted object ID is reused when a new object is placed.



%index
combox
Display combo box
%group
Object control command
%prm
p1,p2,p3
p1=variable      : Numerical variable that holds the state of the combo box
p2=0〜(100)  : Extended Y size
p3="strings" : String indicating the contents of the combo box

%inst
Places a combo box as an object at the current position.
A combo box is an input object that allows you to select one from multiple string elements.
The size of the object is the size set by the objsize command. However, you must specify the Y size for the list display when selecting with the p2 parameter. (Usually, 100 to 150 is appropriate)
^
By specifying a string separated by "\n" in p3, you can set the elements to be selected.
For example, if you specify the string "APPLE\nORANGE\nGRAPE", it will be a combo box that allows you to select one from "APPLE", "ORANGE", and "GRAPE".
Each element has an index number assigned sequentially from 0. In the previous example, "APPLE" is index 0, "ORANGE" is index 1, and "GRAPE" is index 2.
^
This data format, which is separated by "\n", is the same as the multi-line text data handled by the memnote command. The data created with the memnote command can be used as is for the combox command.
^
When the user makes a selection, the index number is assigned to the numerical variable specified by p1.
When the combo box is initially placed, the index indicated by the variable set in p1 is selected. (When the index number is -1, it is in the unselected state)

%href
objsize

%sample
	a=0:objsize 120,24
	combox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
input
Display input box
%group
Object control command
%prm
p1,p2,p3,p4
p1=variable : Variable for input
p2,p3   : Message box size (in dots)
p4=0〜  : Maximum number of characters that can be entered

%inst
Places an input box as an object at the current position. Specify the size with p2, p3, and the location will be from the current position. If p2 and p3 are omitted, the size specified by objsize is used.
^
An input box is a small window where you can directly enter data from the keyboard. After clicking the input box with the mouse to display the cursor, you can enter parameters from the keyboard.
The entered value is assigned to the variable specified by p1. If the variable of p1 is a string type, the entered string will be reflected as is, and if it is a numerical type, the entered value will be reflected as is in the variable.
^
In the initial state, the input box displays the value assigned to the variable specified by p1.
^
You can specify the maximum number of characters that can be entered with p4. If p4 is omitted, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned. (If the variable is not a string type, the default is up to 32 characters.)
If p4 is set to 0, the maximum number of characters that can be handled by that version of Windows can be entered.
^
When an input box is placed, the current position automatically moves to the next line.

%href
mesbox
objsize




%index
listbox
Display list box
%group
Object control command
%prm
p1,p2,p3
p1=variable      : Numerical variable that holds the state of the list box
p2=0〜(100)  : Extended Y size
p3="strings" : String indicating the contents of the list box

%inst
Places a list box as an object at the current position.
A list box is an input object that allows you to select one from multiple string elements.
The size of the object is the size set by the objsize command. However, you must specify the Y size for the list display when selecting with the p2 parameter. (Usually, 100 to 150 is appropriate)
^
By specifying a string separated by "\n" in p3, you can set the elements to be selected.
For example, if you specify the string "APPLE\nORANGE\nGRAPE", it will be a list box that allows you to select one from "APPLE", "ORANGE", and "GRAPE".
Each element has an index number assigned sequentially from 0. In the previous example, "APPLE" is index 0, "ORANGE" is index 1, and "GRAPE" is index 2.
^
This data format, which is separated by "\n", is the same as the multi-line text data handled by the memnote command. The data created with the memnote command can be used as is for the listbox command.
^
When the user makes a selection, the index number is assigned to the numerical variable specified by p1.
When the list box is initially placed, the index indicated by the variable set in p1 is selected. (When the index number is -1, it is in the unselected state)

%href
objsize

%sample
	a=0:objsize 120,24
	listbox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
mesbox
Display message box
%group
Object control command
%prm
p1,p2,p3,p4,p5
p1=variable    : String variable with display message assigned
p2,p3      : Message box size (in dots)
p4=0〜(1)  : Message box style
p5=0〜(-1) : Maximum number of characters that can be entered

%inst
Places a message box (window for displaying messages) as an object on the window. Specify the size with p2, p3, and the location will be from the current position.
The values to set for the message box style (p4) are as follows:
^p
   Value : Corresponding key
 ---------------------------------------------------------------
     0  : Scrollable edit box (unwritable)
    +1  : Scrollable edit box (writable)
    +4  : Add a horizontal scroll bar
    +8  : Disable auto wrap (folding)
^p
Creating a writable edit box results in a simple text editor where the user can freely enter characters. +4 (add horizontal scroll bar), +8 (disable auto wrap) can be specified simultaneously by adding each value to p4.
^
You can specify the maximum number of characters that can be entered with p5.
If p5 is 0, it will be the maximum number of characters that can be handled by that version of Windows.
If p5 is omitted or a negative value, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned.

%href
input




%index
objprm
Change object contents
%group
Object control command
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2        : Parameter content to change

%inst
Changes the contents and parameters of objects placed on the screen with the button command, input, mesbox commands, etc.
^
Specify the object ID in p1. The object ID is a number assigned to each object displayed on the screen sequentially from 0.
The object ID is assigned to the system variable stat after executing the command that places the object (usually the numbers 0, 1, 2... are assigned in the order they are placed).
^
Here, modify the specified object with the parameters specified in p2.
The parameters specified in p2 vary depending on the type of object. Some objects require a string, while others require a number. See the table below for details.
^p
     Object               : p2 Specification
 ----------------------------------------------------------
  Button                 : Change button text (string)
  Input Box (Numeric)    : Change input content (numeric)
  Input Box (String)     : Change input content (string)
  Checkbox               : Check ON/OFF (numeric)
  Combo Box              : Change combo box content (string)
  List Box               : Change list box content (string)
^p
For example, if you specify the ID indicating a checkbox and then specify 1 for the p2 parameter, the checkbox will be forcibly changed to the ON state. In this case, the value of the variable holding the checkbox content is also automatically rewritten.
^
When the content of an input box is changed, the input box automatically receives input focus, and a cursor appears in the box.

%href
button
input
mesbox
chkbox
listbox
combox
layerobj


%index
objsize
Set Object Size
%group
Object Control Commands
%prm
p1,p2,p3
p1=0〜(64)  : Object width (in dots)
p2=0〜(24)  : Object height (in dots)
p3=0〜(0)   : Minimum line size to ensure in the Y direction (in dots)

%inst
Sets the size of objects such as buttons and input boxes when they are placed.
^
p3 can specify the minimum amount the current position moves after a button or message is placed. This allows buttons and messages to be placed consecutively with the same amount of space.
^
When the screen is cleared, the object size automatically returns to the default.

%href
button
chkbox
combox
input
listbox




%index
objsel
Set Input Focus to Object
%group
Object Control Commands
%prm
p1
p1=0〜(0) : Object ID specification

%inst
Sets the input focus to the object ID specified by p1.
By setting the input focus, you can display the input cursor (caret) in the input box placed with the mesbox or input commands.
This command is used when you want to set the input focus to an arbitrary location in multiple input boxes, or to perform processing such as moving to the next input box using key input.
Also, if you specify -1 for p1, the object ID that currently has input focus will be assigned to the system variable stat.



%index
objmode
Set Object Mode
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object font setting mode specification
p2=0〜1   : Focus movement key specification (0=OFF/1=ON)
%inst
This command is used to set styles, etc., used by object control commands such as button, input, and mesbox.
p1 can specify the font setting and style-related mode.
This changes the style settings when subsequent object placement commands are executed.
The values and contents of the modes are as follows:
^p
   p1 : Macro Name          : Mode
 ------------------------------------------------------------
    0 : objmode_normal      : Use the HSP standard font
    1 : objmode_guifont     : Use the default GUI font
    2 : objmode_usefont     : Use the font selected by the font command
    4 : objmode_usecolor    : Use the color specified by the color/objcolor commands
^p
Immediately after the window is initialized, mode 1 is set.
^
If you change to mode 2, the font specified by the font command will be used. Note that the font used is the one set at the time the object control command is executed, not the font at the time the objmode command was executed.
^
Mode 4 (objmode_usecolor) can be used in combination with other modes by adding them. If "objmode_usefont+objmode_usecolor" is specified, both modes will be enabled.
^
With p2, you can turn ON/OFF the object focus movement mode using the [TAB] key. If p2 is set to 1, you can move the input focus of the displayed object with the [TAB] key. If the mode specification for p2 is omitted, the previous mode will be inherited.
^p
  p2 : Mode
 --------------------------------------------------------------
  0  : Disable [TAB]
  1  : [TAB] key allows object focus movement (standard)
^p
%href
button
chkbox
combox
input
listbox
objmode_guifont
objmode_normal
objmode_usefont
objmode_usecolor
objcolor



%index
objcolor
Set Object Color
%group
Object Control Commands
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : Color code (R,G,B brightness)

%inst
Sets the color used by the object.
p1, p2, and p3 are the brightness of R, G, and B, respectively. The specified value is 0 for the darkest and 255 for the brightest. (0,0,0 is black, and 255,255,255 is white.)
^
The color specified by the objcolor command is valid only when the objmode_usecolor option of the objmode command is specified.
The objmode_usecolor option allows you to specify the text color and background color when placing objects.

%href
objmode
color
objmode_usecolor




%index
objenable
Set Object Enable/Disable
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2=0〜(1) : 0 to disable, any other value to enable

%inst
Changes the state of the object ID specified by p1.
If the value specified by p2 is 0, the object is disabled.
A disabled object exists on the screen but its color is changed and it cannot be operated.
If a value other than 0 is specified for p2, the object is enabled as usual.
You can enable or disable all objects, such as input boxes and buttons.
%href
objgray

%port-
Let


%index
objskip
Set Object Focus Movement Mode
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2=1〜(2) : Focus movement mode

%inst
Sets the focus movement mode of the object ID specified by p1.
The focus movement mode specifies the behavior when moving placed objects with the [TAB] key for each object.
Specify the mode value with p2. The details of the mode values are as follows:
^p
  p2 : Focus Movement Mode
 --------------------------------------------------------------
   1 : [TAB] key allows focus to move to the next object (standard)
   2 : [TAB] key does not allow focus to move to the next object
   3 : Do not move focus to the object with the [TAB] key (skip)
  +4 : Select all text when moving focus (input box only)
^p
Normally, the optimal mode is set when the object is placed, so there is no need to reset the focus movement mode.
Use this only when changing the focus movement mode for objects that play a special role, or when adding a new system-defined object with the winobj command.
Note that if the focus movement mode function is turned OFF by the objmode command, focus movement using the [TAB] key will not be performed.

%href
winobj
objmode

%port-
Let


%index
objimage
Set Custom Button
%group
Object Control Commands
%prm
id,x1,y1,x2,y2,x3,y3
id    : Custom button reference buffer ID
x1,y1 : Custom button reference coordinates 1 (normal)
x2,y2 : Custom button reference coordinates 2 (pressed)
x3,y3 : Custom button reference coordinates 3 (mouse over)

%inst
Performs settings for placing custom buttons.
Custom buttons can replace the appearance of push button objects created with the button command with arbitrary images.
To create a custom button, you must first prepare an image to be displayed as a button.
^
The id parameter specifies the screen buffer ID where the custom button image is stored.
If the id parameter is omitted, or if -1 is specified, the custom button setting is disabled, and the button provided by the normal Windows system is used.
(x1,y1) specifies the upper left coordinates of the image to be displayed as a normal button. (If the parameter is omitted, (0,0) is specified.)
(x2,y2) specifies the upper left coordinates of the image to be displayed on the button when the button is pressed. (If the parameter is omitted, (0,0) is specified.)
(x3,y3) specifies the upper left coordinates of the image to be displayed on the button when the mouse is over the button (mouse over). (If the parameter is omitted, the same value as x1, y1 is used.)
^
After the custom button setting is performed by the objimage command, the setting is applied to all buttons placed with the button command.
Custom buttons change their appearance by copying the specified image to the location where the button is placed.
The copied range is the same size as the button (set by the objsize command).
The behavior other than the appearance is the same as that of a normal button. Character display on the button, focus movement, etc. are also supported.
Custom button settings are cleared when the screen is initialized (when the cls command or screen command is executed).

%href
button

%port-
Let


%index
layerobj
Add Layer Object
%group
Object Control Commands
%prm
p1,p2,p3,*label,p4
p1,p2     ; Layer object XY size (in dots)
p3(0)     : Layer ID
*label    : Layer object processing subroutine
p4=0〜(0) : Option value

%inst
Places a layer object at the current position.
Layer objects are placement objects defined by the user.
By registering a subroutine to draw on the screen in advance, you can execute drawing at a specified timing (layer).
To notify the layer object of the drawing area, you need to specify the X and Y sizes in dots with the p1 and p2 parameters. If the specification of p1 and p2 is omitted, the size of the entire screen is used.
Now, treat the current position X, Y as the upper left coordinates, and up to the size specified by p1, p2 as a layer object.
The p3 parameter specifies the layer to draw on.
^p
 p3 : Layer to draw on
 --------------------------------------------------------------
    0 : Do not draw (objlayer_min)
    1 : Background layer (objlayer_bg)
    2 : Normal drawing layer (objlayer_normal)
    3 : GUI layer (objlayer_posteff)
    4 : Topmost drawing layer (objlayer_max)
 +256 : Allow duplicate registration of layer objects (objlayer_multi)
^p
*The *label* parameter specifies the processing subroutine for the layer object.
You must prepare this subroutine yourself.
The *p4* parameter allows you to set an arbitrary integer value for the layer object.
^
Everything the layer object does is left to the user-created subroutine.
The subroutine is called with system variables *iparam*, *wparam*, and *lparam* set.  Based on this information, the user must write the processing, including drawing the layer object.
Within the layer object processing subroutine, you must only draw and immediately terminate the processing with the *return* command.  You cannot write commands that stop tasks, such as *wait/await/redraw*.
Refer to the programming manual for detailed information on layer objects.

%href
objprm

%port-
Let
