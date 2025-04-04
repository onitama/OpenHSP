%ver
3.7
%date
2023/08/04
%group
Input/Output Control Instructions

%type
User-Defined Instructions

%note
Include mod_vpad.as.

%author
onitama

%dll
mod_vpad

%port
Win
%index
hspvpad_init
Initialize the virtual pad
%prm
p1,p2,p3,p4
p1(0) : Buffer ID to store the pad image
p2(0) : Pad layout (1=Left PAD/2=Top PAD/4=Movement only)
p3(0) : Button layout offset (X)
p4(0) : Button layout offset (Y)
%inst
Initializes the virtual pad and makes it ready for use.
The virtual pad displays 4 directions and 4 buttons on the screen for touch input in games and other applications.
The p1 parameter specifies the buffer ID to store the pad image. The "vpad.png" file (HSPTV material) will be loaded into the buffer specified here.
If p1 is omitted or set to 0, an unused buffer ID will be used.
The p2 parameter specifies the position to display the virtual pad. You can specify the following values:
^p
   Value  : Action
 -----------------------------------------------------
   1   : Swap the position of movement and action buttons
   2   : Display the virtual pad at the top of the screen
   4   : Display only the movement controls of the virtual pad
^p
These values can be added together to specify multiple options simultaneously.
Also, the (p3, p4) parameters allow you to fine-tune the button placement with X, Y offsets.
After initialization is complete, you can use the virtual pad with the hspvpad_key and hspvpad_put commands.
Refer to the help for each command for more details.
%sample
	#include "mod_vpad.as"
	hspvpad_init
	repeat
		stick key,$ffff
		hspvpad_key key
		redraw 0
		color 0,0,0 : boxf : rgbcolor $ffffff
		pos 0,0 : mes "key = " + key
		hspvpad_put
		redraw 1 : await 30
	loop

%href
hspvpad_key
hspvpad_put
stick


%index
hspvpad_key
Virtual pad key processing
%prm
var
var : Variable name to which the value obtained by stick is assigned
%inst
Performs key processing for the virtual pad.
Be sure to initialize the module with the hspvpad_init command before using it.
Specify a variable with the var parameter when calling the command.
^p
	stick key,$ffff
	hspvpad_key key
^p
Specify the key content obtained with the stick command beforehand, as shown above.
This will assign the value touched on the virtual pad to the same variable.
The information obtained by the virtual pad uses the same values as the stick command, so you can process it in the same way as the normal stick command.

%href
hspvpad_init
hspvpad_put
stick



%index
hspvpad_put
Virtual pad display processing
%prm
p1
p1(0) : Key value to force ON
%inst
Performs display processing for the virtual pad.
Be sure to initialize the module with the hspvpad_init command before using it.
The p1 parameter allows you to specify the key information for buttons that should be in the pressed state.
The button value specified here will be displayed as pressed (darker icon).
If the p1 parameter is omitted or set to 0, the buttons received by the hspvpad_key command will be ON.

%href
hspvpad_init
hspvpad_key


%index
hspvpad_set
Adjust virtual pad button position
%prm
p1,p2,p3
p1(0) : Button ID (0=Up/1=Down/2=Left/3=Right/4=Check/5=Circle/6=Triangle/7=Cross)
p2(0) : X position (absolute coordinates) (If negative, no display)
p3(0) : Y position (absolute coordinates)
%inst
Adjusts the button positions of the virtual pad.
Be sure to initialize the module with the hspvpad_init command before using it.
The p1 parameter allows you to specify the button ID (button type).
The position of the button specified here will be replaced with the coordinates specified by (p2, p3).
If you specify a negative value for the X position, the corresponding button will no longer be processed, including its display.
%href
hspvpad_init
hspvpad_key


%index
hspvpad_stick
Virtual pad key replacement
%prm
p1,p2
p1(0) : Button ID (0=Up/1=Down/2=Left/3=Right/4=Check/5=Circle/6=Triangle/7=Cross)
p2(0) : Key value (Key information value of the stick command)
%inst
Replaces the key information when a button on the virtual pad is pressed.
Be sure to initialize the module with the hspvpad_init command before using it.
The p1 parameter allows you to specify the button ID (button type).
The key information for the button specified here will be replaced with the value specified by p2.
You can use the key values obtained by the stick command for the key information values.
%href
hspvpad_init
hspvpad_key
stick
