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
2020/09/03
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
es_ini
System initialization
%group
Extended screen control command
%prm
p1,p2
p1 (512) = maximum number of sprites
p2 (1024) = maximum number of character definitions
p3 (64) = Circumference accuracy (64-4096)

%inst
Initializes the standard sprite system.
If the parameter is omitted, the maximum number of sprites is 512 and the maximum number of character definitions is 1024.
Initialize the sprite.
Since the es_ini command can be executed any number of times, it can also be used when you want to clear all sprites.
^
p3 specifies how many times to make the angle used in the es_adir command etc. around. finger
The numbers that can be determined are multiples of 64 and 256, and multiples of 360. Finger other numbers
If set, the closest value that is less than the specified value and meets the conditions is set.
^
If memory allocation fails for some reason, such as when there is not enough free memory,
A non-zero value is returned in stat.


%index
es_area
Sprite effective area setting
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 (0) = Upper left X coordinate of valid area
p2 (0) = Upper left Y coordinate of valid area
p3 (0) = lower right X coordinate of the valid area
p4 (0) = lower right Y coordinate of the effective area

%inst
Sets the effective area of the sprite.
If the sprite coordinates are not within the range (p1, p2)-(p3, p4), they will be automatically deleted.
^
When the sprite display range is initialized by the es_ini instruction, the sprite effective area is automatically reset.
%href
es_ini


%index
es_size
Character size specification
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = X size of character
p2 = Y size of character
p3 (100) = hit check area (%)
p4 ($ 3ff) = p4 = Specify special effect (EP value)

%inst
Specifies the size when defining the sprite character pattern.
(p1, p2) is the X and Y size of the character.
^
p3 specifies what percentage of the hit check area will be when making a hit judgment. If 100 is specified, the hit check will be performed to the full size specified in (p1, p2).
If you specify 0, the character has no collision detection.
p4 is the default setting for special effects (EP values). Normally, you can omit it.
The special effects (EP value) parameter specifies settings such as translucency and additive synthesis. See the Standard Sprite Programming Guide for more information.
^
There are no particular restrictions on the X and Y sizes of the character, but specifying a character pattern that extends beyond the offscreen buffer is invalid.
The setting of the es_size instruction is valid for all subsequent character definitions.
%href
es_pat


%index
es_pat
Character image definition
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 (-1) = Character No.
p2 = Upper left X coordinate of the character pattern
p3 = Upper left Y coordinate of the character pattern
p4 (0) = number of display frames (0)
p5 = window buffer ID
%inst
Defines the sprite's character pattern.
^
The character of the size set by es_size is registered in the character number specified by p1 from the coordinates of (p2, p3).
The target image here must be read into the window buffer in advance with the picload command or celload command.
In p5, if the window buffer ID is specified, it is cut out from the window buffer, and if p5 is omitted, a part of the image is cut out from the window buffer corresponding to the current operation destination and registered as a character pattern. I will.
^
If the p1 parameter is omitted or set to a negative value, a free character number is automatically assigned. After executing this instruction, the character number assigned to the system variable stat is assigned, so you can know which number was used.
^
The character pattern is registered in the specified character number with one es_pat command. If you need a lot of character patterns, you need to register that much.
The es_patanim command is also provided to register character patterns collectively.
^
The parameters of p4 are the settings for character animation.
If 0, there is no animation. If it is 1 or more, the character is displayed for the specified number of frames, and then the next character (character No. + 1) is displayed.
%href
es_link
es_patanim


%index
es_link
Animation settings
%group
Extended screen control command
%prm
p1,p2
p1 = Character No.
p2 = Character No. for loop animation

%inst
This is a command to set the loop animation of the character.
After the character number specified in p1 displays only the specified frame, in p2
Set to return to the specified character number.
%href
es_pat

%sample
es_size 32,32
es_pat 1,0,0,8
es_pat 2,32,0,8
es_pat 3,64,0,8
es_pat 4,96,0,8
es_link 4,1


%index
es_kill
Remove sprites
%group
Extended screen control command
%prm
p1
p1 = Sprite No.

%inst
Deletes the sprite number specified in p1.

%href
es_clear


%index
es_clear
Delete multiple sprites
%group
Extended screen control command
%prm
p1,p2
p1 = Sprite No.
p2 = number to be deleted

%inst
Sprites after the sprite number specified in p1 will be deleted and will not be registered.
You can specify the number of sprites that will be deleted by p2.
If you omit p1 and p2, all sprites will be deleted.
%href
es_kill


%index
es_exnew
Get new sprites
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = variable name to which the new No. is assigned
p2 = Start search Sprite No.
p3 = Sprite No. to end the search
p4 = incremental

%inst
This instruction searches for unused numbers in sprite numbers.
Assigns a number with a space between the numbers specified in p2 to p3 to the variable specified in p1.
If there is no space, -1 is assigned.
^
For p4, specify the increment of the sprite number to search. If omitted, it will be from p2 and p3.
Since it is set dynamically, it usually does not need to be specified.
^
* If you want to perform the same processing as the es_new command, specify -1 for p3.
%href
es_new


%index
es_new
Get new sprites
%group
Extended screen control command
%prm
p1,p2,p3
p1 = variable name to which the new No. is assigned
p2 = Start search Sprite No.
p3 = number of sprites to search

%inst
This instruction searches for unused numbers in sprite numbers.
Substitutes the number with space after the number specified in p2 to the variable specified in p1. Sky
If not, -1 is substituted.
^
This instruction is left for compatibility with previous versions.
For a more detailed search, be sure to use the es_exnew instruction.
%href
es_exnew



%index
es_get
Get sprite information
%group
Extended screen control command
%prm
p1,p2,p3
p1 = variable name to which information is assigned
p2 = Sprite No.
p3=info number

%inst
Substitutes the sprite number information of p2 for the variable name specified in p1.
The type of information is specified by the info number on p3. You can set the following values.
^p
Value macro content
----------------------------------------------------------
 0 ESI_FLAG flag value (function status setting)
 1 ESI_POSX X coordinates
 2 ESI_POSY Y coordinate
 3 ESI_SPDX X Move value
 4 ESI_SPDY Y Move value
 5 ESI_PRGCOUNT Progress count value ((unused)
 6 ESI_ANIMECOUNT animation counter
 7 ESI_CHRNO Character No.
 8 ESI_TYPE type value (user-configured attribute)
 9 ESI_ACCELX X Acceleration (for falling)
10 ESI_ACCELY Y Acceleration (for falling)
11 ESI_BOUNCEPOW Repulsive force (for falling)
12 ESI_BOUNCEFLAG Repulsion setting (for falling)
13 ESI_OPTION option value (depending on user settings)
14 ESI_PRIORITY Display priority
15 ESI_ALPHA Special effect (EP value)
16 ESI_FADEPRM Fade parameters
17 ESI_ZOOMX X Display magnification
18 ESI_ZOOMY Y Display magnification
19 ESI_ROTZ Rotation angle </ pre>
^p
All information is stored as a 32-bit integer. Please note that the coordinates, acceleration, magnification, etc. of the sprite are fixed decimal values, so the actual values are multiplied by 65536.
You can use es_setp to rewrite sprite information directly.

%href
es_setp


%index
es_setp
Sprite parameter settings
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No.
p2=info number
p3 = value to change
%inst
Change sprite information directly.
The parameter of p2 of the sprite No. specified by p1 is directly rewritten to the value of p3.
The type of information is specified by the info number on p3. You can set the following values.
^p
Value macro content
----------------------------------------------------------
 0 ESI_FLAG flag value (function status setting)
 1 ESI_POSX X coordinates
 2 ESI_POSY Y coordinate
 3 ESI_SPDX X Move value
 4 ESI_SPDY Y Move value
 5 ESI_PRGCOUNT Progress count value ((unused)
 6 ESI_ANIMECOUNT animation counter
 7 ESI_CHRNO Character No.
 8 ESI_TYPE type value (user-configured attribute)
 9 ESI_ACCELX X Acceleration (for falling)
10 ESI_ACCELY Y Acceleration (for falling)
11 ESI_BOUNCEPOW Repulsive force (for falling)
12 ESI_BOUNCEFLAG Repulsion setting (for falling)
13 ESI_OPTION option value (depending on user settings)
14 ESI_PRIORITY Display priority
15 ESI_ALPHA Special effect (EP value)
16 ESI_FADEPRM Fade parameters
17 ESI_ZOOMX X Display magnification
18 ESI_ZOOMY Y Display magnification
19 ESI_ROTZ Rotation angle </ pre>
^p
All information is stored as a 32-bit integer. Most parameters have separate instructions to set, so you do not need to use the es_setp instruction.
In addition, sprite coordinates, acceleration, magnification, etc. are fixed decimal values, so they may differ from the actual values. In that case, use the es_pos command.
You can use es_get to get sprite information directly.
%href
es_get
es_pos


%index
es_find
Sprite search
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 = variable name to which sprite number is assigned
p2 = type value to be searched
p3 = Start search Sprite No.
p4 = Sprite No. to end the search
p5 = incremental

%inst
Search for sprite No. with the specified type value among the registered sprites
To do.
Multiple types can be specified for the type value of p2 at the same time.
For example, if you want to target type1 and type4, specify 5 for 1 + 4.
^
Omit p2 or specify 0 to splice with all type values
Will be searched.
^
The search is performed in order from the sprite number specified by p3 to the number specified by p4.
If this is omitted, all sprites will be searched from No.0.
When the appropriate sprite is found, substitute the result for p1 and finish.
If the result of the search does not find the corresponding sprite, -1 is assigned.
^
p5 specifies the increment of the sprite number to be searched. If omitted, it is automatically set from p3 and p4, so it is not usually necessary to specify it.


%index
es_check
Collision detection acquisition
%group
Extended screen control command
%prm
p1,p2,p3
p1 = variable name to which the result is assigned
p2 = Sprite No. to be checked
p3 = type value to be searched

%inst
Collision detection between sprites is performed.
^
Detects sprites that collide (overlap) with the sprite number specified in p2
Search and assign the result to the variable specified in p1.
^
You can specify the type value that is the target of collision detection with p3.
If p3 is omitted or 0, it determines collisions with all sprites.
Multiple types can be specified for the type value of p3 at the same time.
For example, if you want to target type1 and type4, specify 5 for 1 + 4.
^
For collision detection, the hit check area specified when the character pattern was registered
The region (%) is used. Even if the images actually overlap, in the hit check area
It is not considered a collision unless the ranges overlap.
Note that the sprite's display magnification is reflected in the collision range, but the rectangular collision range does not change when rotated.
^
The result is assigned to the variable specified by p1. If there are any sprites colliding
If it is, -1 is used. If there is a sprite that is colliding, the sprite No.
Is substituted. If there are multiple sprites colliding, the sprites
The one with the smaller number has priority.
%href
es_type
es_size


%index
es_offset
Offset coordinate setting
%group
Extended screen control command
%prm
p1,p2
p1 = offset X value
p2 = offset Y value

%inst
The display of all sprites is staggered by the number specified by the offset value.
I will. The offset value set by this command is when drawing is performed by the es_draw command.
It will be reflected in.
%href
es_draw


%index
es_set
Sprite settings
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5,p6
p1 (-1) = Sprite No.
p2 = sprite X coordinate
p3 = sprite Y coordinate
p4 = Character No.
p5 (0) = option value
p6 (0) = priority

%inst
Register the sprite.
Specify the sprite number with p1, and (p2, p3) will be the X and Y coordinates of the sprite on the screen.
With this coordinate as the upper left, the character number specified in p4 is displayed.
Sprite coordinates can be set in the range of -32767 to 32767.
^
If p1 is omitted or set to a negative value, a free sprite number is automatically assigned.
In this case, the sprite number set after the instruction is executed is assigned to the system variable stat.
^
p5 sets an option value that the user can freely use.
(See the Standard Sprite Programming Guide for more information on option values) ^
Specify the sprite priority with p6. The smaller the value, the closer the priority is, and if the values are the same, the smaller sprite has priority.
You can also draw in ascending order of sprite number as before.
^
If you do not need it, you can omit the p5 and p6 parameters.

%href
es_pat
es_draw


%index
es_flag
flag value setting
%group
Extended screen control command
%prm
p1,p2
p1 = Sprite No.
p2 = flag value

%inst
Changes the flag value of the sprite specified in p1.
The flag value is automatically set to $ 100 (256) when sprites are enabled. Normally, you don't need to mess with it.
The values indicated by the flag values are as follows. The name in [] is the defined macro name.
^p
flag value =
  0: Sprite unregistered
  bit 0-6: 1 to 127 countdown timer
  bit 7: Countdown disappears SWITCH
  bit 8: $ 100 Sprite display ON [ESSPFLAG_STATIC]
  bit 9: $ 200 Sprite move ON [ESSPFLAG_MOVE]
  bit10: $ 400 Free fall movement ON [ESSPFLAG_GRAVITY]
  bit11: Move by linking with $ 800 BG (not implemented) [ESSPFLAG_BGLINK]
  bit12: $ 1000 Border erasure disabled ON [ESSPFLAG_NOWIPE]
  bit13: $ 2000 Inverted at border X SWITCH [ESSPFLAG_XBOUNCE]
  bit14: $ 4000 Inverted at border Y SWITCH [ESSPFLAG_YBOUNCE]
  bit15: $ 8000 Flashing at countdown timer SWITCH [ESSPFLAG_BLINK]
  bit16: $ 10000 Hidden SWITCH [ESSPFLAG_NODISP]
^p
%href
es_get


%index
es_chr
chr value setting
%group
Extended screen control command
%prm
p1,p2
p1 = Sprite No.
p2 = Character No.

%inst
Changes the character number of the sprite specified in p1.
The character number must be set in advance with the es_pat instruction.
%href
es_pat


%index
es_type
type value setting
%group
Extended screen control command
%prm
p1,p2
p1 = Sprite No.
p2 = type value

%inst
Changes the sprite type value specified in p1.
The sprite type value can be freely set by the user and can be effectively used when identifying an object in a game or the like.
The values that can be set are integer values such as 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768.
This is the value that indicates each bit in the binary number. You can use this for 32 bits, up to a total of 32 types.
Immediately after setting the sprite with the es_set instruction, the type value is 1.
%href
es_set


%index
es_pos
Sprite coordinate settings
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = Sprite No.
p2 = sprite X coordinate
p3 = sprite Y coordinate
p4 (0) = configuration option

%inst
Change information such as sprite coordinates.
Specify the sprite number with p1 and set the X and Y coordinates with (p2, p3).
The p4 configuration option allows you to set anything other than the sprite's X and Y coordinates.
^p
   p4 macro name Contents
--------------------------------------------------
      0 ESSPSET_POS Sprite X, Y coordinates
      1 ESSPSET_ADDPOS Sprite X, Y moving component
      2 ESSPSET_FALL Sprite X, Y Fall speed
      3 ESSPSET_BOUNCE Sprite bound coefficient
      4 ESSPSET_ZOOM Sprite X, Y Display magnification
 0x1000 ESSPSET_DIRECT Set 32bit value directly
 0x2000 ESSPSET_MASKBIT Hold fixed fractional part
^p
The coordinate value set by the es_pos instruction is internally a fixed 16-bit decimal number. Normally, the conversion is done automatically, but adding ESSPSET_DIRECT to the configuration option disables the conversion.
Also, by adding ESSPSET_MASKBIT, the decimal part up to that point will be retained when converting 16-bit fixed decimals.
^
The es_pos command is a command that changes only the coordinates of the set sprite. To set a new sprite, use the es_set instruction.

%href
es_set
es_getpos


%index
es_setrot
Sprite rotation angle / display magnification setting
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = Sprite No.
p2 (0) = angle value
p3 (-1) = Display magnification in the X direction (%)
p4 (-1) = Display magnification in the Y direction (%)

%inst
Changes the sprite rotation angle and display magnification specified in p1.
The sprite is rotated to the angle specified by p2. The value of p2 is an integer of 0 or more, and the larger the value, the more clockwise the rotation. The value that the angle goes around is specified by the es_ini instruction.
^
The display magnification in the X and Y directions is changed by the (p3, p4) parameter.
If the value of p3 or p4 is omitted or a negative value, the previous magnification is retained unchanged.
The display magnification is converted from 100% as 1x. If 250 is specified, it will be 2.5 times.
Immediately after setting the sprite with the es_set instruction, the angle is 0 and the magnification is 100%.

%href
es_set


%index
es_apos
Sprite movement settings
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = Sprite No.
p2 = 1 Number of dots moving in the X direction in the frame
p3 = 1 Number of dots moving in the Y direction in the frame
p4 = movement speed (%)

%inst
Make settings to move sprites automatically.
Specify the sprite number with p1 and the increment in the X and Y directions with p2 and p3.
^
Specify the movement speed in% with p4. If p4 is omitted or 100 is specified,
It becomes 100% speed and keeps moving in one frame by the increment specified by (p2, p3).
If the movement speed is set to 50 (%) and the increment is specified as (+4, -2), it will actually be an increment of (+2, -1).
The unit of increment moves accurately even if it is less than 1 dot. For example, even if you move (1,0) to 1 frame at 10%, it will be calculated to move 1 dot after 10 frames.
%href
es_adir
es_aim


%index
es_setgosub
Sprite display callback settings
%group
Extended screen control command
%prm
p1,p2
p1 = Sprite No.
p2 = Subroutine label to set

%inst
Sets the display callback for the sprite specified in p1.
Specify the subroutine label with p2. After that, every time the sprite is displayed, the subroutine is called like the gosub instruction.
^
When the subroutine is actually called, the sprite number is set in the system variable iparam.
The type value is assigned to wparam, and the character number is assigned to lparam.
^
The subroutine that makes the call acts as a callback routine.
Please note that some instructions such as waiting for time cannot be executed. For more information, see Callback routines.

%href
gosub


%index
es_adir
Sprite movement direction setting
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No.
p2 = moving direction (0 ~)
p3 = movement speed (%)

%inst
Make settings to move sprites automatically.
^
Specify the sprite number with p1 and the direction with p2. The direction is the numerical value specified by es_ini
Specify up to. 0 is directly below and increases counterclockwise.
^
Specify the movement speed in% with p3. When p3 is omitted or 100 (%) is specified
Moves a dot distance per frame in the specified direction.


%index
es_aim
Sprite reference coordinate setting
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = Sprite No.
p2 = X coordinate of destination
p3 = Y coordinate of destination
p4 = movement speed (%)

%inst
The es_aim instruction moves the sprite in the specified direction like the es_adir instruction.
Is for. In the es_aim command, the X and Y positions that are the targets to move instead of the direction
Specify the mark. The sprite will change the direction from the current coordinates to the target coordinates.
Calculate and start moving.
^
This command, for example, a missile launched by an enemy moves toward the player.
It is useful when you want to do it.
^
Specify the movement speed in% with p4. When p3 is omitted or 100 (%) is specified
Moves a dot distance per frame in the specified direction.
^
Executing this instruction returns the movement direction to the system variable stat.
You can use this to create 3-way bullets that aim at your ship.
%href
es_apos
es_adir


%index
es_draw
Sprite drawing
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 = Start drawing Sprite No.
p2 (-1) = number of sprites to draw
p3 (0) = Processing hold flag
p4 (-1) = priority start value
p5 (-1) = Priority end value

%inst
Draws the registered sprite on the screen.
Normally, you can draw all sprites by omitting the p1 and p2 parameters. Be sure to draw between redraw 0 and redraw 1.
^
If you want to draw only a part of the registered sprites, specify the start sprite No. in p1 and the number to draw in p2.
If p1 and p2 are omitted, all sprites will be drawn.
^
The p3 parameter allows you to suspend internal processing.
^p
 Value macro name content
------------------------------------------------------------------------
  0 ESDRAW_NORMAL Perform all processing
  1 ESDRAW_NOMOVE Hold move processing
  2 ESDRAW_NOANIM Hold animation
  4 Hold the ESDRAW_NOCALLBACK callback routine
  8 ESDRAW_NODISP Hold display
 16 ESDRAW_NOSORT Hold sort
^p
Normally, when an instruction is executed, sprite movement-> animation processing-> display processing-> callback processing is executed, but each processing can be put on hold.
The ESDRAW_NO * macros can be specified at the same time by adding them.
This allows you to temporarily stop moving or animating. Normally, you can safely omit the p3 parameter.
^
When the p4 and p5 parameters are specified, the priority specified by the es_set and es_setpri instructions is referenced, and the sprites are drawn by sorting in the order of the values. At that time, it is possible to draw only the range from p3 to p4.
If you specify the p4 and p5 parameters, the sprites are sorted according to their priority, and the one with the highest priority value is displayed in the foreground. (However, if ESDRAW_NOSORT is specified in the processing hold flag, sorting is disabled.)
If the p4 and p5 parameters are negative or omitted, the priority value is ignored, and the smaller the sprite number, the closer it is displayed.
^
%sample
*gmain
stick ky, $ 1f; Get key information
if ky & $ 80: goto * owari; [ESC] Suspension check
redraw 0; clear screen
es_draw; sprite drawing
redraw 1; screen refresh
await 1000/60; System idle
	goto *gmain
%href
es_set
es_setpri
es_setgosub


%index
es_gravity
Fall acceleration setting
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No. (-1 ~)
p2 = Acceleration in the X direction (-32767 to 32767)
p3 = Acceleration in the Y direction (-32767 to 32767)

%inst
Set the acceleration when free fall movement is turned on with the es_flag command.
^
p2 and p3 are the movement amounts that are added for each frame, and 256 is 1 dot.
You can set a default value by specifying -1 for p1. This value is
The value set when the es_set instruction is executed. The initial value is 0 for p2 and 256 for p3.
is.


%index
es_bound
Bounce ease setting
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No. (-1 ~)
p2 = Ease of momentum (0 to 256)
p3 = flag (1 = X direction 2 = Y direction 3 = both default = 3)

%inst
When free fall movement is turned on with the es_flag command, it bounces at the bottom (or top) of the screen.
Set the ease of momentum when returning. The larger this value, the less momentum it loses when it bounces.
I won't be able to.
^
p3 bounces or does not bounce when it hits the top and bottom edges of the screen, or the left and right edges
Can be set. However, this flag applies only in the direction of free fall.
Will be done.
^
You can set a default value by specifying -1 for p1. This value is
This value is set when the es_set instruction is executed, and the initial value is 128.


%index
es_effect
Sprite special effects setting
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No.
p2 ($ 3ff) = Effect parameter (EP value)
p3 (-1) = Light color RGB value (0 to $ ffffff) (not implemented)
%inst
Set the effect (special effect) when drawing a sprite.
The value specified by p2 is called the EP value and has the following meanings.
^p
  EP value content
-----------------------------------------------------------
  0 to 255 —¯ channel value (0 = transparent, 255 = opaque)
  + $ 300 Enables alpha channel synthesis (equivalent to gmode 3)
  + $ 500 Additive synthesis is performed on the original image (equivalent to gmode 5)
  + $ 600 Subtractive synthesis on the original image (equivalent to gmode 6)
^p
This value is the sum of the alpha channel value and the composite parameters of the gmode instruction.
The default value is set to $ 3ff (alpha channel = 255, equivalent to gmode3).
The p3 parameter allows you to multiply a specific light color when drawing a sprite. This parameter is currently unimplemented.


%index
es_fade
Sprite blinking / fade setting
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No.
p2 = blinking flag (1 to 3)
p3 = Flashing count initial value (-1 or 1 ~)

%inst
^p
Blinking flag = 1: No blinking, turn off sprites when count reaches 0
Destroy
Blinking flag = 2: Blinking, stop blinking when the count reaches 0
Blinking flag = 3: There is blinking, and the sprite is turned off when the count reaches 0.
Destroy
^p
Sets the sprite to blink. What was previously set with the es_flag command
It is an independent and extended instruction. The setting here is reset when the es_flag command is executed.
Will be done.
If you set p3 to -1, the count will be infinite.


%index
es_move
Sprite move
%group
Extended screen control command
%inst
This instruction is currently unimplemented.


%index
es_setpri
Sprite priority setting
%group
Extended screen control command
%prm
p1,p2
p1 (0) = Sprite No.
p2 (0) = priority (0 ~)
%inst
Change the sprite priority.
Sets the value specified in p2 to the sprite specified in p1.
When actually drawing according to the priority, it is necessary to specify the priority setting parameter of the sprite to be displayed by the es_draw command.
%href
es_set
es_draw


%index
es_put
Character screen display
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5,p6,p7
p1 = Upper left X coordinate to display
p2 = Upper left Y coordinate to display
p3 = Character No. to be displayed
p4 = EP value
p5 = Display X Magnification
p6 = Display Y magnification
p7 = display angle

%inst
The data registered as a character pattern for sprites is displayed at the specified coordinates.
This command is a command to display a character on the screen regardless of the sprite.


%index
es_ang
Angle acquisition
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = X coordinate of reference point
p2 = Y coordinate of the reference point
p3 = X coordinate of destination point
p4 = Y coordinate of destination point

%inst
The direction of (p3, p4) seen from (p1, p2) is returned to the system variable stat.
You can also get directions using es_aim, but you can use this command to sprite
You can get the direction without using.


%index
es_cos
Trigonometric function
%group
Extended screen control command
%prm
p1,p2
p1 = variable name to which the result is assigned
p2 = angle (0 to ‘¯ n)
* N = es_ini Accuracy set by the 3rd parameter

%inst
Substitute the cosine of the angle p2 for the variable p1.
The range of p2 is up to the value set in es_ini.
The value obtained is a fixed decimal with 10 bits in the decimal part. In other words, the value that should be obtained
Returns an integer multiplied by 1024.


%index
es_sin
Trigonometric function
%group
Extended screen control command
%prm
p1,p2
p1 = variable name to which the result is assigned
p2 = angle (0 to ‘¯ n)
* N = es_ini Accuracy set by the 3rd parameter

%inst
Assign the sine of the angle p2 to the variable p1.
The range of p2 is up to the value set in es_ini.
The value obtained is a fixed decimal with 10 bits in the decimal part. In other words, the value that should be obtained
Returns an integer multiplied by 1024.


%index
es_dist
Distance calculation between two points
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 = variable to which the result is assigned
p2, p3 = coordinates of point A
p4, p5 = coordinates of point B

%inst
Find the distance from (p2, p3) to (p4, p5) and assign it to the variable p1. The calculation result is a decimal
It is a fixed decimal number of 8 bits.


%index
es_bye
Sprite system release
%group
Extended screen control command
%inst
Detach the standard sprite system.
Frees all reserved memory. This is automatically called as a cleanup function at the end of HSP.
Normally, you do not need to write anything.


%index
es_opt
Setting sprite repulsion coordinates
%group
Extended screen control command
%prm
p1,p2
p1 = X coordinate setting
p2 = Y coordinate setting

%inst
Sets the coordinates of the ground that repels when gravity is applied to the sprite.
If you do not make this setting, it will repel at the bottom of the screen.


%index
es_patanim
Character image definition collectively
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 (-1) = Character No.
p2 = number to define
p3 = Upper left X coordinate of the character pattern
p4 = Upper left Y coordinate of the character pattern
p5 (0) = number of display frames (0)
p6 = window buffer ID
%inst
Defines sprite character patterns together.
^
After the character number specified in p1, the number of character patterns specified in p2 will be registered. Use the es_pat instruction to define a single character number.
The character pattern to be registered is the continuous side-by-side area in the image. The character is registered as if the patterns are lined up to the right with the size set by es_size from the coordinates of (p3, p4).
The target image here must be read into the window buffer in advance with the picload command or celload command.
In p5, if the window buffer ID is specified, it is cut out from the window buffer, and if p5 is omitted, a part of the image is cut out from the window buffer corresponding to the current operation destination and registered as a character pattern. I will.
^
If the p1 parameter is omitted or set to a negative value, a free character number is automatically assigned. After executing this instruction, the character number assigned to the system variable stat is assigned, so you can know which number was used.
^
The parameters of p5 are the settings for character animation.
If 0, there is no animation. If it is 1 or more, the character is displayed for the specified number of frames, and then the next character (character No. + 1) is displayed.
^
Normally, an animation is defined in which the number of patterns specified by the p2 parameter is repeated in the waiting frame of p5.
When ESSPPAT_1SHOT ($ 1000) is added to the p2 parameter, it defines a pattern that disappears after displaying multiple patterns in animation.
%href
es_link
es_pat


%index
es_getpos
Get sprite coordinates
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 = Sprite No.
variable name to which p2 = X coordinates are assigned
variable name to which the p3 = Y coordinate is assigned
p4 (0) = configuration option

%inst
Get X and Y information such as sprite coordinates and assign them to two variables.
Specify the sprite number with p1 and set the variables to be assigned with the p2 and p3 parameters.
The p4 configuration options allow you to specify what coordinates to get.
^p
   p4 macro name Contents
--------------------------------------------------
      0 ESSPSET_POS Sprite X, Y coordinates
      1 ESSPSET_ADDPOS Sprite X, Y moving component
      2 ESSPSET_FALL Sprite X, Y Fall speed
      3 ESSPSET_BOUNCE Sprite bound coefficient
      4 ESSPSET_ZOOM Sprite X, Y Display magnification
 0x1000 ESSPSET_DIRECT Get 32bit value directly
^p
The coordinate value acquired by the es_getpos instruction is internally a fixed 16-bit decimal number. Normally, the conversion is done automatically, but adding ESSPSET_DIRECT to the configuration option disables the conversion.

%href
es_set
es_pos


%index
es_bgmap
Initialize BG map
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1 = BGNo. (0 to 15)
p2 = variable name to store map data
p3 (16) = X size of the entire map
p4 (16) = Y size of the entire map
p5 (16) = X size of map display part
p6 (16) = X size of map display part
p7 (0) = Screen buffer ID to store map parts
p8 (0) = configuration option

%inst
Initialize a BG map with any size.
Specify the BG No. to be initialized with p1. BG No. can be specified from 0 to 15.
Specify the variable to store the map data in p2.
This variable must be initialized with the dim instruction in advance as an array variable that has the element "X size of the entire map x Y size of the entire map".
Specify the X and Y sizes of the entire map with (p3, p4).
Specify the X and Y sizes of the part where the map is displayed with (p5, p6).
All parameters from p3 to p6 are units of map parts. For example, if the overall size is 4x4, you would specify the variable initialized with "dim map, 4 * 4".
This variable stores the ID of the part as it is, such as 0 for the map part (cell ID) when it has the contents of the map data directly and the value is 0.
In p7, specify the screen buffer ID in which the image of the map part is loaded. Images must be loaded into this buffer in advance using the picload or celload command. (For the map part image, set the part size with the celdiv command.)
Set the behavior of the BG map with the setting options on p8. This item is for future expansion and is not currently implemented.
^
The es_bgmap instruction sets the map display. The actual map display is performed with the es_putbg instruction.
%href
es_putbg
dim
celload
celdiv


%index
es_putbg
Display BG map
%group
Extended screen control command
%prm
p1,p2,p3,p4,p5
p1 = BGNo. (0 to 15)
p2 (0) = Display start X coordinate
p3 (0) = Display start Y coordinate
p4 (0) = Display start map X position
p5 (0) = Display start map Y position

%inst
Displays the BG map of the specified BG No.
It is necessary to initialize the size and parts information of the BG map with the es_bgmap command in advance.
Specify the display start position (upper left) on the screen with the (p2, p3) parameter.
Specify the map display start position (upper left) with the (p4, p5) parameter.
Normally, the map is displayed from the (0,0) position in the entire map.
By specifying the map display start position, you can change the position of the map that corresponds to the upper left when a part of the entire map is displayed.
%href
es_bgmap


%index
es_bgmes
Write a string to the BG map
%group
Extended screen control command
%prm
p1,p2,p3,"str",p4
p1 (0) = BGNo. (0 to 15)
p2 (0) = map X position
p3 (0) = map Y position
"str" = write string
p4 (0) = character code offset value

%inst
Writes the code of the character string specified by "str" to the BG map of the specified BG No.
It is necessary to initialize the size and parts information of the BG map with the es_bgmap command in advance.
The es_bgmes instruction operates on the assumption that the character font in ASCII code order can be displayed as a BG map.
From the position of the map specified by (p2, p3), the character code included in the character string is written to the right.
^
Specify the offset value for the character code with the p4 parameter. Normally, it can be 0 or omitted. For example, the letter "A" has the character code 65 and writes the value 65 to the map.
If it contains a line feed code, move to the next line.

%href
es_bgmap


%index
es_setparent
Sprite parent settings
%group
Extended screen control command
%prm
p1,p2,p3
p1 = Sprite No.
p2 = Parent sprite No.
p3 = configuration options

%inst
Sets the sprite that is the parent of the specified sprite.
By setting the parent sprite number in the p2 parameter, the sprite will be displayed in coordinates relative to the parent's coordinates.
If you specify a negative value for p2, the parent sprite setting is canceled and the normal sprite display is restored.
^
Normally, specify 0 for the setting option of the p3 parameter.
By specifying 1 for p3, it becomes a sprite setting that displays the BG map as a parent.


%index
es_modaxis
Change sprite information at once
%group
Extended screen control command
%prm
p1,p2,p3
p1 (0) = Start sprite No.
p2 (-1) = End sprite No.
p3 (0) = type value option
p4 (0) = X value to change
p5 (0) = Y value to change
p6 (0) = configuration option

%inst
For sprites with numbers from p1 to p2, change the coordinates collectively.
If you omit the specification of p1 and p2, all sprites will be the target.
^
You can specify the type value with p3. If 4 is specified for p3, sprites with type value 4 will be targeted.
If you omit the p3 parameter or specify 0, all type values are included.
^
You can specify the value to be added to each of the sprite coordinate parameters X and Y with the (p4, p5) parameter. If you specify a negative value, it will be subtracted.
The value specified here will be added (subtracted) to all sprites that are subject to change.
^
The setting option on p6 allows you to set other than the X and Y coordinates of the sprite.
^p
   p4 macro name Contents
--------------------------------------------------
      0 ESSPSET_POS Sprite X, Y coordinates
      1 ESSPSET_ADDPOS Sprite X, Y moving component
      2 ESSPSET_FALL Sprite X, Y Fall speed
      3 ESSPSET_BOUNCE Sprite bound coefficient
      4 ESSPSET_ZOOM Sprite X, Y Display magnification
 0x1000 ESSPSET_DIRECT Set 32bit value directly
 0x2000 ESSPSET_MASKBIT Hold fixed fractional part
^p
You can use the same configuration options as the es_pos instruction.

%href
es_pos



