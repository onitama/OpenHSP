;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Built-in instructions
%ver
3.6
%note
ver3.6 standard instruction
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
Button display
%group
Object control instructions
%prm
goto/gosub "name",*label
"name": The name of the button
* label: Label name to jump when pressed

%inst
Places a pushbutton as an object at the current position.
The character string specified by "name" is written on the button, and when the button is clicked with the mouse, the control of the program is transferred to the location specified by * label.
^
If you write button goto, jump to the label. If you write button gosub, a subroutine jump is performed on the label. If the goto and gosub keywords are omitted, the operation is the same as goto.
^p
Example:
button gosub "button", * aaa; Create a button to call * aaa
^p
^
The size of the object can be specified with the objsize command. When you place the button, the current position automatically moves to the next line.
^
In the program that jumps and executes when the button is pressed, the object ID is assigned as the initial value of the system variable stat.
^
Normally, the button shape provided by the Windows system is used.
However, if the custom button using the image is set by the objimage command, you can make it look free.
See the objimage instruction section for custom button settings.


%href
objsize
objimage



%index
chkbox
Check box display
%group
Object control instructions
%prm
"strings",p1
"strings": Checkbox content display string
p1 = variable: variable that holds the state of the checkbox

%inst
Places a checkbox as an object at the current position.
The check box is an object with a switch on the left side of the character string specified by "strings" that can be switched ON / OFF with the cursor.
^
The size of the checkbox is the size specified by objsize and the background is gray.
^
If the content of the numeric variable specified by p1 is 0, the check is OFF, and if it is 1, it is ON. When the ON / OFF of the check is changed, the contents of the variable also change at the same time.

%href
objsize




%index
clrobj
Clear the object
%group
Object control instructions
%prm
p1,p2
p1 = 0 to (0): Object ID to be erased (start)
p2 = 0 to (-1): Object ID to be erased (end) (If -1, the final ID is specified
Will be)

%inst
Deletes the object issued by the button command or mesbox command.
^
If you omit p1 and p2 and execute only clrobj, all the objects on the screen will be deleted. If you want to delete only some objects, specify the first ID in p1 and the last ID in p2, and only the objects from p1 to p2 will be deleted. The specified object ID is the same as the ID specified by the objprm instruction.
^
The ID of the deleted object will be reused when placing a new object.



%index
combox
Combo box display
%group
Object control instructions
%prm
p1,p2,p3
p1 = variable: A numeric variable that holds the state of the combo box
p2 = 0 ~ (100): Extended Y size
p3 = "strings": Strings indicating the contents of the combo box

%inst
Place the combo box as an object at the current position.
A combo box is an input object that allows you to select one of multiple string elements.
The size of the object will be the size set by the objsize command. However, the p2 parameter must specify the Y size for list display when selected. (Usually 100-150 is suitable)
^
You can set the element to be selected by specifying the character string separated by "\\ n" with p3.
For example, if you specify the character string "APPLE \\ nORANGE \\ nGRAPE", it becomes a combo box to select one from "APPLE", "ORANGE", and "GRAPE".
Each element has an index number starting from 0. In the previous example, "APPLE" is numbered with index 0, "ORANGE" is numbered with index 1, "GRAPE" is numbered with index 2, and so on.
^
The data format separated by "\\ n" is the same as the multi-line text data handled by the memory notepad instruction. The data created by the memory notepad instruction can be used as it is for the combox instruction.
^
When the user makes a selection, the index number is assigned to the numeric variable specified in p1.
When the combo box is placed for the first time, the index indicated by the variable set in p1 will be selected. (When the index number is -1, it will be in the non-selected state)

%href
objsize

%sample
	a=0:objsize 120,24
	combox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
input
Input box display
%group
Object control instructions
%prm
p1,p2,p3,p4
p1 = variable: variable for input
p2, p3: Message box size (in dots)
p4 = 0 ~: Maximum number of characters that can be entered

%inst
Place the input box as an object at the current position. Specify the size with p2 and p3, and the location will be from the current position. If p2 and p3 are omitted, the size specified by objsize will be used.
^
The input box is a small window that allows you to type directly from the keyboard. You will be able to enter parameters from the keyboard with the cursor raised by clicking the input box with the mouse.
The entered value is assigned to the variable specified by p1. If the variable of p1 is a character string type, the entered character string is reflected in the variable, and if it is a numeric type, the entered value is reflected in the variable as it is.
^
In the initial state of the input box, the value assigned to the variable specified by p1 is displayed in the box.
^
You can specify the maximum number of characters that can be entered with p4. If p4 is omitted, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned. (If the variable is not a string type, it defaults to 32 characters.)
If p4 is specified as 0, the maximum number of characters that can be handled by that version of Windows can be entered.
^
When you place the input box, the current position automatically moves to the next line.

%href
mesbox
objsize




%index
listbox
List box display
%group
Object control instructions
%prm
p1,p2,p3
p1 = variable: a numeric variable that holds the state of the list box
p2 = 0 ~ (100): Extended Y size
p3 = "strings": Strings indicating the contents of the list box

%inst
Place the list box as an object at the current position.
A list box is an input object that allows you to select one of multiple string elements.
The size of the object will be the size set by the objsize command. However, the p2 parameter must specify the Y size for list display when selected. (Usually 100-150 is suitable)
^
You can set the element to be selected by specifying the character string separated by "\\ n" with p3.
For example, if you specify the character string "APPLE \\ nORANGE \\ nGRAPE", it becomes a list box to select one from "APPLE", "ORANGE", and "GRAPE".
Each element has an index number starting from 0. In the previous example, "APPLE" is numbered with index 0, "ORANGE" is numbered with index 1, "GRAPE" is numbered with index 2, and so on.
^
The data format separated by "\\ n" is the same as the multi-line text data handled by the memory notepad instruction. The data created by the memory notepad instruction can be used as it is for the listbox instruction.
^
When the user makes a selection, the index number is assigned to the numeric variable specified in p1.
When the list box is placed for the first time, the index indicated by the variable set in p1 will be selected. (When the index number is -1, it will be in the non-selected state)

%href
objsize

%sample
	a=0:objsize 120,24
	listbox a,120,"APPLE\nORANGE\nGRAPE"
	stop




%index
mesbox
Message box display
%group
Object control instructions
%prm
p1,p2,p3,p4,p5
p1 = variable: String variable to which the display message is assigned
p2, p3: Message box size (in dots)
p4 = 0 to (1): Message box style
p5 = 0 to (-1): Maximum number of characters that can be entered

%inst
Place a message box (window for displaying messages) as an object on the window. Specify the size with p2 and p3, and the location will be from the current position.
The value (p4) to be set for the message box style is as follows.
^p
   Value: Corresponding key
 ---------------------------------------------------------------
     0: Scrollable edit box (non-rewritable)
    +1: Scrollable edit box (rewritable)
    +4: Add a horizontal scroll bar
    +8: Disable automatic wrap (wrap)
^p
Creating a rewritable edit box makes it a simple text editor where users can type whatever they want. For +4 (with horizontal scroll bar) and +8 (disable automatic wrap), you can specify multiple values at the same time by adding each value to p4.
^
You can specify the maximum number of characters that can be entered with p5.
If p5 is 0, it is the maximum number of characters that can be handled by that version of Windows.
If p5 is omitted or has a negative value, the maximum number of characters that can be stored in the variable specified by p1 is automatically assigned.

%href
input




%index
objprm
Change the contents of the object
%group
Object control instructions
%prm
p1,p2
p1 = 0 to (0): Object ID specification
p2: Contents of the parameter to be changed

%inst
Use the button command, input, mesbox command, etc. to change the contents and parameters of the object placed on the screen.
^
For p1, specify the object ID. The object ID is a number assigned to each object displayed on the screen in order from 0.
The object ID is assigned to the system variable stat after the instruction to place the object is executed (usually, the numbers 0, 1, 2 ... are assigned in the order in which they are placed).
^
Changes the object specified here with the parameters specified in p2.
The parameters specified in p2 differ depending on the object type. Depending on the object, there are those that specify a character string and those that specify a numerical value. See the table below for details.
^p
     Object: p2 specification
 ----------------------------------------------------------
  Button: Change button string (string)
  Input box (numerical value): Change input contents (numerical value)
  Input box (character string): Change input contents (character string)
  Check box: Check ON / OFF (numerical value)
  Combo box: Change the contents of the combo box (character string)
  List box: Change the contents of the list box (character string)
^p
For example, if you specify an ID that indicates a check box and then specify 1 for the parameter of p2, the check box is forcibly changed to the ON state. In this case, the value of the variable that holds the contents of the checkbox is also automatically rewritten.
^
If you change the contents of the input box, the input focus is automatically set in the input box and the cursor is displayed in the box.

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
Object size setting
%group
Object control instructions
%prm
p1,p2,p3
p1 = 0 to (64): The size of the object in the X direction (in dots)
p2 = 0 to (24): Object size in Y direction (in dots)
p3 = 0 to (0): Minimum reserved line size in Y direction (in dots)

%inst
Set the size of the object when arranging buttons, input boxes, etc.
^
You can specify the minimum amount that the current position will move after the button or message is placed on p3. This will allow space of the same size when the buttons and messages are placed consecutively.
^
When the screen is cleared, the object size will automatically return to the default.

%href
button
chkbox
combox
input
listbox




%index
objsel
Set input focus on object
%group
Object control instructions
%prm
p1
p1 = 0 to (0): Object ID specification

%inst
Focuses the input on the object ID specified by p1.
By focusing the input, you can put the input cursor (caret) in the input box placed by the mesbox command or the input command.
This command is for performing processing such as when you want to focus the input on any location in multiple input boxes, or when you move to the next input box by key input.
If you specify -1 for p1, the object ID that is currently in focus is assigned to the system variable stat.



%index
objmode
Object mode setting
%group
Object control instructions
%prm
p1,p2
p1 = 0 to (0): Object font setting mode specification
p2 = 0 to 1: Focus movement key specification (0 = OFF / 1 = ON)
%inst
This command is used to set the style used in object control commands such as button, input, and mesbox.
You can specify font settings and style-related modes with p1.
This will change the style settings when executing subsequent object placement instructions.
The mode values and contents are as follows.
^p
   p1: Macro name: Mode
 ------------------------------------------------------------
    0: objmode_normal Use HSP standard font
    1: objmode_guifont Use default GUI font
    2: Use the font selected by the objmode_usefont font command
    4: Use the color of the objmode_usecolor color instruction / objcolor instruction
^p
Immediately after the window is initialized, it is set to mode 1.
^
If you change to mode 2, the font specified by the font instruction will be used. This uses the set font when the object control instruction is executed. Note that it is not the font at the time the objmode instruction was executed.
^
Mode 4 (objmode_usecolor) can be used together by adding it to other modes. If "objmode_usefont + objmode_usecolor" is specified, both modes are enabled.
^
Use p2 to turn on / off the focus movement mode of the object with the [TAB] key. When p2 is specified as 1, the input focus of the displayed object can be moved with the [TAB] key. If the mode specification of p2 is omitted, the previous mode is inherited.
^p
  p2: mode
 --------------------------------------------------------------
  0: Disable [TAB]
  1: The focus of the object can be moved by pressing the [TAB] key (standard)
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
Object color settings
%group
Object control instructions
%prm
p1,p2,p3
p1, p2, p3 = 0 to 255 (0): Color code (brightness of R, G, B)

%inst
Sets the color used by the object.
p1, p2, and p3 are the brightness of R, G, and B, respectively. The values you specify are 0 for the darkest and 255 for the brightest. (0,0,0 is black and 255,255,255 is white.)
^
The color specified by the objcolor instruction is valid only when the objmode_usecolor option of the objmode instruction is specified.
With the objmode_usecolor option, it is possible to specify the text color and background color when arranging objects.

%href
objmode
color
objmode_usecolor




%index
objenable
Enable / disable the object
%group
Object control instructions
%prm
p1,p2
p1 = 0 to (0): Object ID specification
p2 = 0 to (1): Invalid if 0, valid if other than 0

%inst
Changes the state of the object ID specified in p1.
If the value specified in p2 is 0, the object is invalidated.
The invalidated object will be present on the screen, but will change color and become inoperable.
If you specify a value other than 0 for the value specified in p2, the object will be valid as usual.
You can enable / disable all objects such as input boxes and buttons.
%href
objgray

%port-
Let


%index
objskip
Set the focus movement mode of the object
%group
Object control instructions
%prm
p1,p2
p1 = 0 to (0): Object ID specification
p2 = 1 ~ (2): Focus movement mode

%inst
Sets the focus movement mode for the object ID specified in p1.
The focus movement mode specifies the behavior when moving an object placed with the [TAB] key for each object.
Specify the mode value with p2. The details of the mode value are as follows.
^p
  p2: Focus movement mode
 --------------------------------------------------------------
   1: You can move the focus to the next object with the [TAB] key (standard)
   2: It is not possible to move the focus to the next object by pressing the [TAB] key.
   3: Do not move the focus to the object with the [TAB] key (skip)
  +4: Select all text when moving focus (input box only)
^p
Normally, the optimum mode is set when the object is placed, so there is no need to reset the focus movement mode.
Use this when you change the focus movement mode only for objects that play a special role, or when you add a new system-defined object using the winobj instruction.
Note that the focus movement is not performed by the [TAB] key when the focus movement mode function is turned off by the objmode command.

%href
winobj
objmode

%port-
Let


%index
objimage
Custom button settings
%group
Object control instructions
%prm
id,x1,y1,x2,y2,x3,y3
id: Custom button reference buffer ID
x1, y1: Custom button reference coordinates 1 (normal time)
x2, y2: Reference coordinate 2 of custom button (when pressed down)
x3, y3: Custom button reference coordinates 3 (when mouse over)

%inst
Make settings for placing custom buttons.
Custom buttons can replace the object appearance of pushbuttons created by the button command with any image.
In order to create a custom button, you need to prepare an image to be displayed as a button in advance.
^
The id parameter specifies the screen buffer ID where the custom button image is stored.
If you omit the id parameter or specify -1, the custom button setting is disabled and the button provided by a normal Windows system is used.
In (x1, y1), specify the upper left coordinates of the image to be displayed as a normal button. (If the parameter is omitted, (0,0) is specified.)
(x2, y2) specifies the upper left coordinates of the image to be displayed on the button when the button is pressed. (If the parameter is omitted, (0,0) is specified.)
In (x3, y3), specify the upper left coordinates of the image to be displayed on the button when the mouse hovers over the button (mouse over). (If the parameter is omitted, the same value as x1 and y1 is used.)
^
After the custom button is set by the objimage command, the setting is applied to all the buttons placed by the button command.
Custom buttons change their appearance by copying the specified image to the location where the button is located.
The range to be copied will be the same size as the button (set by the objsize command).
The behavior other than the appearance is the same as a normal button. Character display on the button, focus movement, etc. are also supported.
Custom button settings are cleared when the screen is initialized (when the cls command or screen command is executed).

%href
button

%port-
Let


%index
layerobj
Add layer object
%group
Object control instructions
%prm
p1,p2,p3,*label,p4
p1, p2; XY size of layer object (in dots)
p3 (0): Layer ID
* label: Layer object processing subroutine
p4 = 0 to (0): Option value

%inst
Place the layer object at the current position.
Layer objects are user-defined placement objects.
By registering a subroutine that draws on the screen in advance, drawing can be executed at the specified timing (layer).
In order to notify the layer object of the drawing area, it is necessary to specify the X and Y sizes in dots in p1 and p2. If p1 and p2 are omitted, the size of the entire screen will be used.
Now, the current positions X and Y are treated as the upper left coordinates, and the size specified by p1 and p2 is treated as a layer object.
The p3 parameter specifies the layer to draw.
^p
  p3: Layer to draw
 --------------------------------------------------------------
    0: Do not draw (objlayer_min)
    1: Background layer (objlayer_bg)
    2: Normal drawing layer (objlayer_normal)
    3: GUI layer (objlayer_posteff)
    4: Foreground drawing layer (objlayer_max)
 +256: Allow duplicate registration of layer objects (objlayer_multi)
^p
The * label parameter specifies the processing subroutine for the layer object.
This subroutine must be provided by the user.
The p4 parameter can be set to any integer value set on the layer object.
^
It's all up to the user-created subroutines to do what the layer object does.
The subroutine is called with the system variables iparam, wparam, lparam set. Based on this information, the user should describe the process, including drawing the layer object.
In the layer object processing subroutine, it is necessary to perform only drawing and immediately end the processing with the return instruction. You cannot write an instruction to stop a task such as wait / await / redraw.
For details on how to use layer objects, refer to the programming manual.


%href
objprm

%port-
Let


