;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
Built-in command
%ver
3.6
%note
ver3.6 Standard command
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
Display Button
%group
Object Control Commands
%prm
goto/gosub "name",*label
"name" : Button name
*label : Label name to jump to when pressed

%inst
Places a button as an object in the current position.
The button displays the string specified by "name", and when the button is clicked with the mouse, the program control moves to the location specified by *label.
^
If you write button goto, it jumps to the label. If you write button gosub, it performs a subroutine jump to the label. If the goto or gosub keyword is omitted, it behaves the same as goto.
^p
Example :
	button gosub "Button",*aaa  ; Create a button that calls *aaa
^p
^
The size of the object can be specified with the objsize command.  After placing the button, the current position automatically moves to the next line.
^
In the program that jumps and executes when the button is pressed, the object ID is assigned as the initial value of the system variable stat.
^
Normally, the button shape is the one provided by the Windows system.
However, if a custom button setting using an image is performed by the objimage command, it can have a free appearance.
See the objimage command section for custom button settings.


%href
objsize
objimage



%index
chkbox
Display Checkbox
%group
Object Control Commands
%prm
"strings",p1
"strings" : Checkbox content display string
p1=variable   : Variable to hold the checkbox state

%inst
Places a checkbox as an object in the current position.
The checkbox is an object that has a switch that can be turned ON/OFF with the cursor on the left side of the string specified by "strings".
^
The size of the checkbox is the size specified by objsize, and the background is gray.
^
If the content of the numeric variable specified by p1 is 0, the check is OFF, and if it is 1, it is ON. When the check is turned ON/OFF, the content of the variable also changes.

%href
objsize




%index
clrobj
Clear object
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0)   : Object ID to delete (start)
p2=-1〜(-1) : Object ID to delete (end) (If -1, the final ID is specified)

%inst
Deletes objects displayed by button, mesbox, etc.
^
If you omit p1 and p2 and execute only clrobj, all objects on the screen will be deleted. If you want to delete only some objects, specify the first ID in p1 and the last ID in p2 to delete only the objects from p1 to p2. The object ID to specify is the same as the ID specified by the objprm command.
^
The ID of the deleted object is reused when placing a new object.



%index
combox
Display Combo Box
%group
Object Control Commands
%prm
p1,p2,p3
p1=variable      : Numeric variable holding the state of the combo box
p2=0〜(100)  : Extended Y size
p3="strings" : String indicating the content of the combo box

%inst
Places a combo box as an object in the current position.
A combo box is an input object that allows you to select one from multiple string elements.
The size of the object is the size set by the objsize command. However, you must specify the Y size for the list display at the time of selection with the p2 parameter. (Usually about 100 to 150 is appropriate)
^
By specifying a string delimited by "\n" in p3, you can set the elements to select.
For example, specifying the string "APPLE\nORANGE\nGRAPE" creates a combo box that allows you to select one of "APPLE", "ORANGE", and "GRAPE".
Each element is assigned an index number starting from 0. In the previous example, "APPLE" has index 0, "ORANGE" has index 1, and "GRAPE" has index 2.
^
This data format of delimiting with "\n" is the same as the multi-line text data handled by the memnote command. The data created by the memnote command can be used directly for the combox command.
^
When the user makes a selection, the index number is assigned to the numeric variable specified by p1.
When the combo box is first placed, the index indicated by the variable set by p1 is selected. (When the index number is -1, it is unselected)

%href
objsize

%sample
	a=0:objsize 120,24
	combox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
input
Display Input Box
%group
Object Control Commands
%prm
p1,p2,p3,p4
p1=variable : Variable for input
p2,p3   : Message box size (in dots)
p4=0〜  : Maximum number of characters that can be entered

%inst
Places an input box as an object in the current position. Specify the size with p2, p3, and the location is from the current position. If p2 and p3 are omitted, the size specified by objsize is used.
^
An input box is a small window that you can type directly from the keyboard. You can click on the input box with the mouse to bring up the cursor and enter parameters from the keyboard.
The value entered is assigned to the variable specified by p1. If the variable p1 is a string type, the entered string is reflected, and if it is a numeric type, the entered value is reflected as is in the variable.
^
In the initial state, the input box displays the value that was assigned to the variable specified by p1.
^
You can specify the maximum number of characters that can be entered with p4. If p4 is omitted, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned. (If the variable is not a string type, the default is up to 32 characters.)
If p4 is set to 0, the maximum number of characters that can be handled by that version of Windows can be entered.
^
After placing the input box, the current position automatically moves to the next line.

%href
mesbox
objsize




%index
listbox
Display List Box
%group
Object Control Commands
%prm
p1,p2,p3
p1=variable      : Numeric variable holding the state of the list box
p2=0〜(100)  : Extended Y size
p3="strings" : String indicating the content of the list box

%inst
Places a list box as an object in the current position.
A list box is an input object that allows you to select one from multiple string elements.
The size of the object is the size set by the objsize command. However, you must specify the Y size for the list display at the time of selection with the p2 parameter. (Usually about 100 to 150 is appropriate)
^
By specifying a string delimited by "\n" in p3, you can set the elements to select.
For example, specifying the string "APPLE\nORANGE\nGRAPE" creates a list box that allows you to select one of "APPLE", "ORANGE", and "GRAPE".
Each element is assigned an index number starting from 0. In the previous example, "APPLE" has index 0, "ORANGE" has index 1, and "GRAPE" has index 2.
^
This data format of delimiting with "\n" is the same as the multi-line text data handled by the memnote command. The data created by the memnote command can be used directly for the listbox command.
^
When the user makes a selection, the index number is assigned to the numeric variable specified by p1.
When the list box is first placed, the index indicated by the variable set by p1 is selected. (When the index number is -1, it is unselected)

%href
objsize

%sample
	a=0:objsize 120,24
	listbox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
mesbox
Display Message Box
%group
Object Control Commands
%prm
p1,p2,p3,p4,p5
p1=variable    : String variable with the display message assigned
p2,p3      : Message box size (in dots)
p4=0〜(1)  : Message box style
p5=0〜(-1) : Maximum number of characters that can be entered

%inst
Places a message box (window for displaying messages) as an object on the window. Specify the size with p2, p3, and the location is from the current position.
The values to set for the message box style (p4) are as follows.
^p
   Value : Corresponding key
 ---------------------------------------------------------------
     0  : Scrollable edit box (not writable)
    +1  : Scrollable edit box (writable)
    +4  : Add a horizontal scroll bar
    +8  : Disable auto-wrap (folding)
^p
Creating a writable edit box creates a simple text editor in which the user can freely enter characters. +4 (add a horizontal scroll bar) and +8 (disable auto-wrap) can be specified simultaneously by adding each value to p4.
^
You can specify the maximum number of characters that can be entered with p5.
If p5 is 0, the maximum number of characters that can be handled by that version of Windows will be used.
If p5 is omitted or a negative value, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned.

%href
input




%index
objprm
Change object content
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2        : Contents of the parameter to be changed

%inst
Changes the content and parameters of objects placed on the screen with the button, input, mesbox, etc. commands.
^
Specify the object ID in p1. The object ID is a number assigned to each object displayed on the screen, starting from 0.
The object ID is assigned to the system variable stat after executing the command to place the object (usually the numbers 0, 1, 2... are assigned in the order they are placed).
^
Here, modify the object specified here with the parameters specified in p2.
The parameters specified in p2 vary depending on the type of object. Some objects require a string, while others require a number. See the table below for details.
^p
     Object               : p2 specification
 ----------------------------------------------------------
  Button                 : Change button text (String)
  Input Box (Numeric)    : Change input content (Numeric)
  Input Box (String)     : Change input content (String)
  Checkbox               : Check ON/OFF (Numeric)
  Combobox               : Change combobox content (String)
  Listbox                : Change listbox content (String)
^p
For example, if you specify the ID of a checkbox and then specify 1 for the p2 parameter, the checkbox will be forcibly set to the ON state. In this case, the value of the variable holding the checkbox content is also automatically rewritten.
^
When you change the contents of an input box, the input box is automatically focused and the cursor is displayed inside the box.

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
Set object size
%group
Object Control Commands
%prm
p1,p2,p3
p1=0〜(64)  : Object width (in dots)
p2=0〜(24)  : Object height (in dots)
p3=0〜(0)   : Minimum number of lines to secure in the Y direction (in dots)

%inst
Sets the size of the object when placing buttons and input boxes.
^
p3 allows you to specify the minimum amount that the current position moves after a button or message is placed. This ensures that buttons and messages placed in succession have the same amount of space between them.
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
Set input focus to object
%group
Object Control Commands
%prm
p1
p1=0〜(0) : Object ID specification

%inst
Sets the input focus to the object ID specified by p1.
By setting the input focus, you can display an input cursor (caret) in input boxes placed with the mesbox command or input command.
This command is used to set the input focus to an arbitrary location in multiple input boxes, or to perform processing such as moving to the next input box with key input.
Also, if you specify -1 for p1, the object ID that currently has the input focus is assigned to the system variable stat.



%index
objmode
Set object mode
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object font setting mode specification
p2=0〜1   : Focus movement key specification (0=OFF/1=ON)
%inst
This command is used to set the styles, etc. used by object control commands such as button, input, and mesbox.
You can specify the font settings and style-related modes with p1.
This changes the style settings when subsequent object placement commands are executed.
The values and contents of the mode are as follows.
^p
   p1 : Macro Name          : Mode
 ------------------------------------------------------------
    0 : objmode_normal      : Use HSP standard font
    1 : objmode_guifont     : Use default GUI font
    2 : objmode_usefont     : Use the font selected by the font command
    4 : objmode_usecolor    : Use the color specified by the color command/objcolor command
^p
Immediately after the window is initialized, mode 1 is set.
^
If you change to mode 2, the font specified by the font command will be used. Note that the font that is set at the time the object control command is executed will be used. It is not the font at the time the objmode command was executed.
^
Mode 4 (objmode_usecolor) can be used in combination by adding it to other modes. If "objmode_usefont+objmode_usecolor" is specified, both modes will be enabled.
^
With p2, you can turn ON/OFF the object focus movement mode using the [TAB] key. If p2 is set to 1, the input focus of the displayed object can be moved with the [TAB] key. If the mode specification of p2 is omitted, the previous mode is inherited.
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
Set object color
%group
Object Control Commands
%prm
p1,p2,p3
p1,p2,p3=0〜255(0) : Color code (R,G,B brightness)

%inst
Sets the color used by the object.
p1, p2, and p3 are the brightness of R, G, and B, respectively. The value to specify is 0 for the darkest and 255 for the brightest. (0,0,0 is black, and 255,255,255 is white.)
^
The color specified by the objcolor command is valid only when the objmode_usecolor option of the objmode command is specified.
The objmode_usecolor option allows you to specify the text color and background color when placing objects.

%href
objmode
color
objmode_usecolor




%index
objenable
Set object enabled/disabled
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2=0〜(1) : 0 if disabled, other than 0 is enabled

%inst
Changes the state of the object ID specified by p1.
If the value specified by p2 is 0, the object is disabled.
Disabled objects exist on the screen, but their color is changed and they cannot be operated.
If the value specified by p2 is other than 0, the object is normally enabled.
You can enable or disable all objects, such as input boxes and buttons.
%href
objgray

%port-
Let


%index
objskip
Set object focus movement mode
%group
Object Control Commands
%prm
p1,p2
p1=0〜(0) : Object ID specification
p2=1〜(2) : Focus movement mode

%inst
Sets the focus movement mode of the object ID specified by p1.
The focus movement mode specifies the behavior for each object when moving the placed objects with the [TAB] key.
Specify the mode value with p2. The details of the mode value are as follows.
^p
  p2 : Focus movement mode
 --------------------------------------------------------------
   1 : Focus can be moved to the next object with the [TAB] key (standard)
   2 : Focus cannot be moved to the next object with the [TAB] key
   3 : Focus movement to objects with the [TAB] key is not performed (skip)
  +4 : Select all text when moving focus (input box only)
^p
Normally, the optimal mode is set when the object is placed, so there is no need to reset the focus movement mode.
Use it only when changing the focus movement mode for objects that play a special role, or when adding a system-defined object by the winobj command.
Note that if the focus movement mode function is turned OFF by the objmode command, focus movement by the [TAB] key will not be performed.

%href
winobj
objmode

%port-
Let


%index
objimage
Custom button settings
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
To create a custom button, you must prepare an image to display as a button in advance.
^
The id parameter specifies the screen buffer ID in which the custom button image is stored.
If you omit the id parameter or specify -1, the custom button setting is disabled, and the button provided by the normal Windows system is used.
Specify the upper left coordinates of the image to display as a normal button at (x1,y1). (If the parameter is omitted, (0,0) is specified.)
(x2,y2) specifies the upper left coordinates of the image to display on the button when the button is pressed. (If the parameter is omitted, (0,0) is specified.)
(x3,y3) specifies the upper left coordinates of the image to display on the button when the mouse is over the button (mouse over). (If the parameter is omitted, the same value as x1, y1 is used.)
^
After the custom button settings are made by the objimage command, the settings are applied to all buttons placed with the button command.
Custom buttons change their appearance by copying the specified image to the location where the button is placed.
The copied range is the same size as the button (set by the objsize command).
The behavior other than the appearance is the same as a normal button. Character display on the button, focus movement, etc. are also supported in the same way.
Custom button settings are cleared when the screen is initialized (when the cls command or screen command is executed).

%href
button

%port-
Let


%index
layerobj
Add layer object
%group
Object Control Commands
%prm
p1,p2,p3,*label,p4
p1,p2     ; Layer object XY size (in dots)
p3(0)     : Layer ID
*label    : Layer object processing subroutine
p4=0〜(0) : Option value

%inst
Place a layer object at the current position.
Layer objects are placement objects defined by the user.
By registering a subroutine that performs drawing on the screen in advance, you can execute drawing at a specified timing (layer).
To notify the layer object of the drawing area, you need to specify the X and Y sizes in dots with the p1 and p2 parameters. If you omit the p1 and p2 specifications, the size of the entire screen is used.
This treats the current position X, Y as the upper left coordinates and the size specified by p1, p2 as a layer object.
The p3 parameter specifies the layer to draw on.
^p
 p3 : Layer to draw on
 --------------------------------------------------------------
    0 : Do not draw (objlayer_min)
    1 : Background layer (objlayer_bg)
    2 : Normal drawing layer (objlayer_normal)
    3 : GUI layer (objlayer_posteff)
    4 : Foremost drawing layer (objlayer_max)
 +256 : Allow duplicate registration of layer objects (objlayer_multi)
^p
*The *label* parameter specifies the processing subroutine for the layer object.
You must provide this subroutine yourself.
The p4 parameter allows you to set an arbitrary integer value for the layer object.
^
What the layer object does is entirely up to the user-created subroutine.
The subroutine is called with the system variables iparam, wparam, and lparam set. Based on this information, the user must write the processing, including the drawing of the layer object.
Within the layer object processing subroutine, you must only draw and immediately terminate the processing with a return statement. Instructions that stop tasks, such as wait/await/redraw, cannot be written.
Refer to the programming manual for detailed information on using layer objects.


%href
objprm

%port-
Let
