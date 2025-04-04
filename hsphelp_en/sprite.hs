;
; HSP help manageróp HELPÉ\Å[ÉXÉtÉ@ÉCÉã
; (Lines starting with ";" are treated as comments)
;

%type
Extended command
%ver
3.7
%note
Include hsp3dish.as.

%date
2025/02/04
%author
onitama
%dll
HSP3Dish
%url
https://hsp.tv/
%port
Win
%group
Extended Input/Output Control Commands

%index
es_ini
System initialization
%group
Extended Screen Control Commands
%prm
p1,p2
p1(512)=Maximum number of sprites
p2(1024)=Maximum number of character definitions
p3(64)=Circumference precision (64Å`4096)
p4(16)=Maximum number of map definitions
%inst
Initializes the standard sprite system.
If the parameters are omitted, sprites are initialized with a maximum of 512 sprites, 1024 character definitions, a circumference precision of 64, and 16 map definitions.
The es_ini command can be executed any number of times, so it can also be used when you want to clear all sprites.
^
p3 specifies how many degrees the angle used in commands such as es_adir will be for one revolution. The
Values that can be specified are 64, multiples of 256, and multiples of 360. If other values are
If specified, the closest value that meets the condition below the specified value is set.
^
If memory allocation fails for any reason, such as insufficient free memory,
stat returns a value other than 0.

%index
es_screen
Sprite drawing resolution setting
%group
Extended Screen Control Commands
%prm
p1,p2
p1(0)=Drawing resolution in the X direction
p2(0)=Drawing resolution in the Y direction
%inst
Resets the drawing resolution of sprites.
The size specified by the p1,p2 parameters is set as the screen drawing resolution.
If you specify 0 or a negative value for the p1,p2 parameters, the size of the current drawing destination screen is used.
Normally, the screen resolution is automatically set when initialized with the es_ini command, so there is no need to reset the resolution.
The es_screen command should be used to change the settings when drawing to a different buffer with a different resolution.
^
When the sprite drawing resolution is reset with the es_screen command, the sprite valid area is automatically reset as well.
%href
es_ini
es_area

%index
es_area
Sprite valid area setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=X coordinate of the upper left of the valid area
p2(0)=Y coordinate of the upper left of the valid area
p3(0)=X coordinate of the lower right of the valid area
p4(0)=Y coordinate of the lower right of the valid area

%inst
Sets the sprite valid area (border).
Sprites whose screen coordinates are not within the range of (p1, p2)-(p3, p4) are automatically deleted. (Sprites with the ESSPFLAG_NOWIPE flag specified are not deleted)
^
When the sprite display range is initialized with the es_ini command or the es_screen command, the sprite valid area is automatically reset as well.
%href
es_ini
es_screen
es_bound

%index
es_size
Character size specification
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1=X size of character
p2=Y size of character
p3(100)=Hit check area (%)
p4($3ff)=p4=Specification of special effect (EP value)

%inst
Specifies the size when defining a character pattern for a sprite.
(p1, p2) is the X, Y size of the character.
^
p3 specifies what percentage of the hit check area is used for collision detection. If you specify 100, hit checking is performed to the full size specified by (p1, p2). (The es_sizeex command is also available for setting a more detailed collision detection area.)
If you specify 0, the character will not have collision detection.
p4 is the default setting for special effects (EP value). Normally, you can omit this.
The special effects (EP value) parameter specifies settings such as semi-transparency and additive synthesis. See the Standard Sprite Programming Guide for details.
^
There are no particular restrictions on the X and Y sizes of the characters, but specifying a character pattern that protrudes from the off-screen buffer is invalid.
The settings of the es_size command remain valid for subsequent character definitions.
%href
es_sizeex
es_pat

%index
es_pat
Character image definition
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1(-1)=Character No.
p2=X coordinate of the upper left of the character pattern
p3=Y coordinate of the upper left of the character pattern
p4(0)=Number of display frames (0)
p5=Window buffer ID
%inst
Defines the character pattern of the sprite.
^
The character of the size set by es_size is registered from the coordinates (p2, p3) to the character No. specified by p1.
The target images must be loaded into the window buffer in advance with the picload command or celload command.
If the window buffer ID is specified by p5, a part of the image is cut out from that window buffer, and if p5 is omitted, from the window buffer that is currently being operated on, and registered as a character pattern.
^
If you omit the p1 parameter or set it to a negative value, an available character No. is automatically assigned. After executing this command, the character No. assigned is stored in the system variable stat, so you can know which No. was used by that.
^
One es_pat command registers a character pattern to the specified character No. If you need many character patterns, you need to register that many patterns.
The es_patanim command is also available for registering character patterns in batches.
^
The p4 parameter is a setting for character animation.
If it is 0, there is no animation. If it is 1 or more, the character is displayed for the specified number of frames, and then the next character (character No.+1) is displayed.
%href
es_link
es_patanim

%index
es_link
Animation settings
%group
Extended Screen Control Commands
%prm
p1,p2
p1=Character No.
p2=Character No. to loop animation to

%inst
This is a command for setting up a character's loop animation.
After the character No. specified by p1 is displayed for the specified number of frames, it is set to return to the character No. specified by p2.
If the execution fails for any reason, the system variable stat returns a value other than 0.
%href
es_pat
es_patanim
%sample
es_size 32,32
es_pat 1,0,0,8
es_pat 2,32,0,8
es_pat 3,64,0,8
es_pat 4,96,0,8
es_link 4,1

%index
es_kill
Sprite deletion
%group
Extended Screen Control Commands
%prm
p1
p1(0)=Sprite No.
%inst
Deletes the sprite No. specified by p1.
To delete multiple sprites at once, use the es_clear command.
%href
es_clear

%index
es_clear
Multiple sprite deletion
%group
Extended Screen Control Commands
%prm
p1,p2
p1(0)=Sprite No.
p2(-1)=Number to be deleted

%inst
The sprites after the sprite No. specified by p1 are deleted and become unregistered.
You can specify the number of sprites to be deleted with p2.
If you omit the specifications for p1 and p2, all sprites are deleted.
%href
es_kill

%index
es_exnew
Get new sprite
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1=Variable name to which the new No. is assigned
p2=Sprite No. to start searching from
p3=Sprite No. to end searching
p4=Increment

%inst
This command searches for an unused No. among the sprite Nos.
Assigns an available No. between the Nos. specified by p2 and p3 to the variable specified by p1.
If there are no vacancies, -1 is assigned.
^
p4 specifies the increment of the sprite No. to search. If omitted, it is automatically set from p2 and p3, so you usually do not need to specify it explicitly.
^
*If you want to perform the same process as the es_new command, specify -1 for p3.
%href
es_new

%index
es_new
Get new sprite
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1=Variable name to which the new No. is assigned
p2(0)=Sprite No. to start searching from
p3=Number of sprites to search
%inst
This command searches for an unused No. among the sprite Nos.
Assigns an available No. after the No. specified by p2 to the variable specified by p1. If there are no vacancies, -1 is assigned.
Specifies the number of searches with p3. If you omit the specification of p3, the number of sprites prepared is specified.
^
This command is left for compatibility with previous versions.
Use the es_exnew command to perform more detailed searches.
%href
es_exnew

%index
es_get
Get sprite information
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1=Variable name to which information is assigned
p2(0)=Sprite No.
p3(0)=info number

%inst
Assigns the information of sprite No. p2 to the variable name specified by p1.
The type of information is specified by the info number of p3. You can set the following values.
^p
Value Macro Content
----------------------------------------------------------
0 ESI_FLAG flag value (function status setting)
1 ESI_POSX X coordinate
2 ESI_POSY Y coordinate
3 ESI_SPDX X movement value
4 ESI_SPDY Y movement value
5 ESI_PRGCOUNT Progress count value (unused)
6 ESI_ANIMECOUNT Animation counter
7 ESI_CHRNO Character No.
8 ESI_TYPE type value (attribute by user setting)
9 ESI_ACCELX X acceleration (for falling)
10 ESI_ACCELY Y acceleration (for falling)
11 ESI_BOUNCEPOW Repulsion power (for falling)
12 ESI_BOUNCEFLAG Repulsion setting (for falling)
13 ESI_OPTION Option value (user setting)
14 ESI_PRIORITY Display priority
15 ESI_ALPHA Special effect (EP value)
16 ESI_FADEPRM Fade parameter
17 ESI_ZOOMX X magnification
18 ESI_ZOOMY Y magnification
19 ESI_ROTZ Rotation angle
20 ESI_SPLINK Link to sprite No. (-1=none)
21 ESI_TIMER Timer value
22 ESI_TIMERBASE Timer value (master)
23 ESI_PROTZ Rotation angle (addition value)
24 ESI_PZOOMX X magnification (addition value)
25 ESI_PZOOMY Y magnification (addition value)
26 ESI_MAPHIT Map collision detection flag (ESSPMAPHIT_*)
27 ESI_STICKSP Adsorption destination sprite No. (-1=none)
28 ESI_STICKX X distance to adsorption destination
29 ESI_STICKY Y distance to adsorption destination
30 ESI_MOVERES Map collision detection result during movement
31 ESI_XREVCHR X-direction inverted character offset value
32 ESI_YREVCHR Y-direction inverted character offset value
33 ESI_MULCOLOR Multiplication color setting (-1=none)
34 ESI_LIFE Life value (for user setting)
35 ESI_LIFEMAX Maximum life value (for user setting)
36 ESI_POWER Power value (for user setting)
^p
All information is stored as 32-bit integers. Sprite coordinates, acceleration, scaling factors, etc., are fixed-point numbers, so be aware that the actual values are multiplied by 65536.
You can use `es_setp` to directly rewrite sprite information.
If execution fails for any reason, a non-zero value will be returned in the system variable `stat`.

%href
es_setp

%index
es_setp
Set Sprite Parameters
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=Sprite No.
p2(0)=info number
p3(0)=Value to change
p4(0)=Change option
%inst
Directly changes the information that the sprite possesses.
Rewrites the parameter of p2 that the sprite No. specified by p1 has directly to the value of p3.
The type of information is specified by the info number of p3. The following values can be set:
^p
Value     Macro           Content
----------------------------------------------------------
 0     ESI_FLAG        flag value (function status setting)
 1     ESI_POSX        X coordinate
 2     ESI_POSY        Y coordinate
 3     ESI_SPDX        X movement value
 4     ESI_SPDY        Y movement value
 5     ESI_PRGCOUNT    Progress count value (unused)
 6     ESI_ANIMECOUNT  Animation counter
 7     ESI_CHRNO       Character No.
 8     ESI_TYPE        type value (user-defined attribute)
 9     ESI_ACCELX      X acceleration (for falling)
10     ESI_ACCELY      Y acceleration (for falling)
11     ESI_BOUNCEPOW   Rebound force (for falling)
12     ESI_BOUNCEFLAG  Rebound setting (for falling)
13     ESI_OPTION      Option value (user-defined)
14     ESI_PRIORITY    Display priority
15     ESI_ALPHA       Special effect (EP value)
16     ESI_FADEPRM     Fade parameter
17     ESI_ZOOMX       X display scale
18     ESI_ZOOMY       Y display scale
19     ESI_ROTZ        Rotation angle
20     ESI_SPLINK      Sprite No. to link to (-1=none)
21     ESI_TIMER       Timer value
22     ESI_TIMERBASE   Timer value (master)
23     ESI_PROTZ       Rotation angle (addition value)
24     ESI_PZOOMX      X display scale (addition value)
25     ESI_PZOOMY      Y display scale (addition value)
26     ESI_MAPHIT      Map collision flag (ESSPMAPHIT_*)
27     ESI_STICKSP     Sprite No. to stick to (-1=none)
28     ESI_STICKX      X distance to the sticking target
29     ESI_STICKY      Y distance to the sticking target
30     ESI_MOVERES     Map collision result during movement
31     ESI_XREVCHR     X direction reverse character offset value
32     ESI_YREVCHR     Y direction reverse character offset value
33     ESI_MULCOLOR    Multiplication color setting (-1=none)
34     ESI_LIFE        Life value (for user setting)
35     ESI_LIFEMAX     Maximum life value (for user setting)
36     ESI_POWER       Power value (for user setting)
^p
All information is stored as 32-bit integers. Most parameters have separate commands for setting them, so there is no need to use the `es_setp` command.
Also, sprite coordinates, acceleration, and scaling factors are fixed-point numbers, so they differ from the actual values. In that case, please use the `es_pos` command, etc.
You can specify change options with the p4 parameter. Option values behave as follows. It can be used for bitwise set/reset.
^p
 Value   Content
----------------------------------
  0   Set the specified value
  1   Add the bits of the specified value
  2   Delete the bits of the specified value
^p
You can use `es_get` to directly retrieve sprite information.
If execution fails for any reason, a non-zero value will be returned in the system variable `stat`.
%href
es_get
es_pos
es_flag

%index
es_find
Sprite Search
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1=Variable name to which the sprite No. is assigned
p2(0)=type value to be searched
p3(0)=Sprite No. to start searching from
p4(-1)=Sprite No. to end searching at
p5(0)=Increment

%inst
Searches for the sprite No. that has the specified type value among the registered sprites.
You can specify multiple types simultaneously for the type value of p2.
For example, to target type1 and type4, specify 5 as 1+4.
^
If you omit the specification of p2 or specify 0, all sprites with all type values will be targeted for the search.
^
The search proceeds sequentially from the sprite No. specified by p3 to the No. specified by p4.
If this is omitted, all sprites from No. 0 will be searched.
When a matching sprite is found, the result is assigned to p1 and the process ends.
^
If no matching sprite is found as a result of the search, -1 is assigned.
^
p5 specifies the increment of the sprite No. to search, and it is usually not necessary to specify it because it is automatically set from p3 and p4 if omitted.

%index
es_check
Collision Detection Acquisition
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5,p6
p1=Variable name to which the result is assigned
p2(0)=Sprite No. to be checked
p3(0)=type value to be searched
p4(0)=Judgment flag (0=do not consider rotation, 1=consider rotation) (not implemented)
p5(0)=Sprite No. to start searching from
p6(-1)=Sprite No. to end searching at

%inst
Performs sprite collision (hit) detection.
^
Searches for a sprite that is colliding with (overlapping) the sprite No. specified by p2,
and assigns the result to the variable specified by p1.
^
You can specify the type value to be targeted for collision detection with p3.
If p3 is omitted or 0, collision with all sprites is determined.
You can specify multiple types simultaneously for the type value of p3.
For example, to target type1 and type4, specify 5 as 1+4.
^
The hit check area (%) specified when registering the character pattern is used for collision detection.
Even if the images actually overlap, they are not considered to collide unless the hit check area ranges overlap.
The p4 parameter is prepared for future expansion and is not currently implemented. The display magnification of the sprite is reflected in the collision range, but please note that even when rotated, the rectangular collision range does not change.
The p5 and p6 parameters allow you to specify the range of sprite Nos. to search for collisions. Specify the sprite No. to start with p5 and the sprite No. to end with p6 (the No. specified by p6 is also included in the search). If you specify a negative value for p6, the end No. is set. If you omit the parameters, all sprites will be targeted for detection.
^
The result is assigned to the variable specified by p1. If there are no colliding sprites, -1 is assigned. If there are colliding sprites, the sprite No. of the smallest one is assigned. If there are multiple colliding sprites, the one with the smaller sprite No. is prioritized.
%href
es_type
es_size
es_sizeex
es_nearobj

%index
es_offset
Offset Coordinate Setting
%group
Extended Screen Control Commands
%prm
p1,p2
p1(0)=Offset X value
p2(0)=Offset Y value

%inst
Shifts the display of all sprites by the number specified by the offset value.
The offset value set by this command is reflected when drawing with the `es_draw` command.
To shift the display coordinates for each individual character, register with the `es_sizeex` command.
%href
es_draw
es_sizeex

%index
es_set
Sprite Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5,p6
p1(-1)=Sprite No.
p2(0)=Sprite X coordinate
p3(0)=Sprite Y coordinate
p4(0)=Character No.
p5(0)=Option value
p6(0)=Priority

%inst
Registers a sprite.
Specifies the sprite No. with p1, and (p2, p3) become the X and Y coordinates of the sprite on the screen.
The character No. specified by p4 is displayed with this coordinate as the upper left.
Sprite coordinates can be set in the range of -32767 to 32767.
^
If you omit p1 or set it to a negative value, a vacant sprite No. is automatically assigned.
^
p5 sets an option value that the user can freely use.
(The option value is a 32-bit integer value that the user can freely use. You can get the value by reading ESI_OPTION with the es_get command.)^
p6 specifies the priority of the sprite. This value is referenced when sorting priorities with the `es_draw` command is enabled. Normally, sprites with smaller sprite Nos. are drawn in front.
If it is not particularly necessary, the p5 and p6 parameters can be omitted.
^
The sprite No. value is returned to the system variable `stat` after execution. If an error occurs for any reason, a negative value is returned.
%href
es_pat
es_draw

%index
es_flag
flag value setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Sprite No.
p2(0)=flag value
p3(0)=Change option
%inst
Changes the flag value of the sprite specified by p1.
If the sprite is enabled, $100 (256) is automatically set as the flag value. Normally, there is no need to touch it.
The values indicated by the flag value are as follows. [ ] indicates the defined macro.
^p
flag value=
  0    :  Sprite unregistered
  bit 0-7 : 1Å`127 Count down timer (HSPDX compatible)
  bit 8 :  $100 Sprite display ON [ESSPFLAG_STATIC]
  bit 9 :  $200 Sprite movement ON [ESSPFLAG_MOVE]
  bit10 :  $400 Free fall movement ON [ESSPFLAG_GRAVITY] (For HSPDX compatibility)
  bit11 :  $800 Move linked to parent [ESSPFLAG_SPLINK]
  bit12 : $1000 Border erase invalid ON [ESSPFLAG_NOWIPE]
  bit13 : $2000 Border X reverse SWITCH [ESSPFLAG_XBOUNCE]
  bit14 : $4000 Border Y reverse SWITCH [ESSPFLAG_YBOUNCE]
  bit15 : $8000 Countdown timer time flashing SWITCH [ESSPFLAG_BLINK]
  bit16 : $10000 Hidden SWITCH [ESSPFLAG_NODISP]
  bit17 : $20000 Fade-in SWITCH during countdown timer [ESSPFLAG_FADEIN]
  bit18 : $40000 Fade-out SWITCH during countdown timer [ESSPFLAG_FADEOUT]
  bit19 : $80000 Disappearance SWITCH at end of countdown timer [ESSPFLAG_TIMERWIPE]
  bit20 : $100000 Disappearance SWITCH2 during countdown timer [ESSPFLAG_BLINK2]
  bit21 : $200000 Countdown timer last fade SWITCH [ESSPFLAG_EFADE]
  bit22 : $400000 Countdown timer last fade SWITCH2 [ESSPFLAG_EFADE2]
  bit23 : $800000 Enable addition of rotation/zoom values [ESSPFLAG_MOVEROT]
  bit24 : $1000000 Display as a decoration sprite on the frontmost [ESSPFLAG_DECORATE]
^p
You can specify change options with the p4 parameter. Option values behave as follows. It can be used for bitwise set/reset.
^p
Value | Content
------- | --------
0 | Set the specified value
1 | Add the bits of the specified value
2 | Remove the bits of the specified value

When directly obtaining sprite information, you can use es_get.
After execution, the system variable stat will return the flag value that was previously set.
If an error occurs for any reason, a negative value is returned.

%index
es_chr
Set character number value
%group
Extended screen control commands
%prm
p1,p2
p1=Sprite No.
p2=Character No.

%inst
Changes the character No. of the sprite specified by p1.
The sprite No. must be initialized in advance with the es_set command.
The character No. must be set in advance with the es_pat command.
After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_type
Set type value
%group
Extended screen control commands
%prm
p1,p2
p1=Sprite No.
p2=type value

%inst
Changes the type value of the sprite specified by p1.
The sprite's type value can be freely set by the user and can be effectively used for object identification in games, etc.
The values that can be set are integer values such as 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768...
These values represent each bit of a binary number. You can use up to 32 of these, for a total of 32 bits.
Immediately after setting a sprite with the es_set command, the type value is 1.
After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_pos
Set sprite coordinates
%group
Extended screen control commands
%prm
p1,p2,p3,p4
p1(0)=Sprite No.
p2(0)=Sprite X coordinate
p3(0)=Sprite Y coordinate
p4(0)=Setting option

%inst
Changes sprite coordinate and other information.
Specify the sprite No. with p1 and set the X, Y coordinates with (p2, p3).
You can set other sprite information besides the X, Y coordinates with the setting option p4.

| p4 | Macro Name | Content |
|---|---|---|
| 0 | ESSPSET_POS | Sprite X, Y coordinates |
| 1 | ESSPSET_ADDPOS | Sprite X, Y movement components |
| 2 | ESSPSET_FALL | Sprite X, Y fall speed |
| 3 | ESSPSET_BOUNCE | Sprite bounce coefficient |
| 4 | ESSPSET_ZOOM | Sprite X, Y display magnification |
| 8 | ESSPSET_ADDPOS2 | Sprite X, Y movement components (relative specification) |
| 9 | ESSPSET_POS2 | Sprite X, Y coordinates (relative specification) |
| 0x1000 | ESSPSET_DIRECT | Set the 32-bit value directly |
| 0x2000 | ESSPSET_MASKBIT | Retain the fixed-point decimal part |

The coordinate values set by the es_pos command are internally 16-bit fixed-point numbers. Normally, the values are automatically converted, but the conversion is disabled by adding ESSPSET_DIRECT to the setting option.
Also, by adding ESSPSET_MASKBIT, the previous decimal part will be retained when converting to a 16-bit fixed-point number.

The es_pos command is used to change only the coordinates of an already set sprite. To set a new sprite, use the es_set command.

After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_setrot
Set sprite rotation angle/display magnification
%group
Extended screen control commands
%prm
p1,p2,p3,p4,p5
p1=Sprite No.
p2(0)=Angle value
p3(-1)=X direction display magnification (%)
p4(-1)=Y direction display magnification (%)

%inst
Changes the rotation angle and display magnification of the sprite specified by p1.
The sprite is rotated to the angle specified by p2. The value of p2 is an integer greater than or equal to 0, and the larger the value, the more it rotates clockwise. The value for one full rotation is specified by the es_ini command.

The display magnification in the X and Y directions is changed by the (p3, p4) parameters.
If the value of p3 or p4 is omitted or is a negative value, the previous magnification is retained without being changed.
The display magnification is converted with 100% as 1x. If you specify 250, it will be 2.5x.
Immediately after setting a sprite with the es_set command, the angle is 0 and the magnification is 100%.

After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_apos
Set sprite movement
%group
Extended screen control commands
%prm
p1,p2,p3,p4
p1(0)=Sprite No.
p2(0)=Number of dots to move in the X direction per frame
p3(0)=Number of dots to move in the Y direction per frame
p4(100)=Movement speed (%)

%inst
Sets up the automatic movement of a sprite.
Specify the sprite No. with p1, and specify the increments in the X and Y directions with p2, p3.

Specify the movement speed in % with p4. If p4 is omitted or 100 is specified,
the speed will be 100%, and the sprite will continue to move by the increment specified by (p2, p3) per frame.
If the movement speed is set to 50 (%) and the increment is specified as (+4, -2), the actual increment will be (+2, -1).
The unit of increment is accurately moved even if it is less than 1 dot. For example, even if you move (1, 0) at 10% in one frame, it will be calculated to move 1 dot after 10 frames.

After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_setgosub
Set sprite display callback
%group
Extended screen control commands
%prm
p1,p2
p1(0)=Sprite No.
p2=Label of the subroutine to set

%inst
Sets the display callback of the sprite specified by p1.
Specify the label of the subroutine with p2. After that, the subroutine will be called in the same way as the gosub command each time the sprite is displayed.

When the subroutine is actually called, the sprite No. is assigned to the system variable iparam,
the type value to wparam, and the character No. to lparam.

The subroutine called is functions as a callback routine.
Please note that you cannot execute some commands, such as time wait. For details, please refer to callback routines.

%index
es_adir
Set sprite movement direction
%group
Extended screen control commands
%prm
p1,p2,p3
p1(0)=Sprite No.
p2(0)=Direction of movement (0~)
p3(100)=Movement speed (%)

%inst
Sets up the automatic movement of a sprite.

Specify the sprite No. with p1, and specify the direction with p2. The direction is specified up to the number specified by es_ini. 0 is directly below, and it increases counterclockwise.

Specify the movement speed in % with p3. If p3 is omitted or 100 (%) is specified,
it will move a distance of 1 dot per frame in the specified direction.

After execution, a value is returned to the system variable stat. A value of 0 or greater is returned if the command completes successfully; a negative value is returned if an error occurs for any reason.

%index
es_aim
Set sprite reference coordinates
%group
Extended screen control commands
%prm
p1,p2,p3,p4
p1(0)=Sprite No.
p2(0)=X coordinate of the destination
p3(0)=Y coordinate of the destination
p4(100)=Movement speed (%)

%inst
The es_aim command, like the es_adir command, is for moving a sprite in the specified direction. In the es_aim command, the target X, Y coordinates to move to are specified instead of the direction. The sprite calculates the direction from the current coordinates to the target coordinates and starts moving.

This command is useful, for example, when you want a missile fired from an enemy to move towards the player.

Specify the movement speed in % with p4. If p3 is omitted or 100 (%) is specified,
it will move a distance of 1 dot per frame in the specified direction.

When this command is executed, the movement direction is returned to the system variable stat.
This can be used to create a 3-way bullet that aims at the player.

%index
es_draw
Draw sprite
%group
Extended screen control commands
%prm
p1,p2,p3,p4,p5
p1(0)=Sprite No. to start drawing
p2(-1)=Number of sprites to draw
p3(0)=Processing hold flag
p4(-1)=Starting value of priority
p5(-1)=Ending value of priority

%inst
Draws the registered sprites on the screen.
Normally, if you omit the p1 and p2 parameters, you can draw all sprites. Be sure to draw between redraw 0 and redraw 1.

If you want to draw only a part of the registered sprites, specify the starting sprite No. in p1 and the number of sprites to draw in p2.
If the specification of p1 and p2 is omitted, all sprites will be drawn.

You can hold internal processing using the p3 parameter.

| Value | Macro Name | Content |
|---|---|---|
| 0 | ESDRAW_NORMAL | Execute all processing |
| 1 | ESDRAW_NOMOVE | Hold movement processing |
| 2 | ESDRAW_NOANIM | Hold animation processing |
| 4 | ESDRAW_NOCALLBACK | Hold callback routine |
| 8 | ESDRAW_NODISP | Hold display |
| 16 | ESDRAW_NOSORT | Hold sorting |

Normally, when a command is executed, sprite movement Å® animation processing Å® display processing Å® callback processing are executed, but each process can be held.
ESDRAW_NO* macros can be added to specify them at the same time.
This allows you to temporarily stop movement and animation. Normally, there is no problem omitting the p3 parameter.

If you specify the p4 and p5 parameters, the priority specified by the es_set command or the es_setpri command is referenced, and the sprites are sorted and drawn in the order of the values. At that time, it is possible to draw only the range from p3 to p4.
If the p4 and p5 parameters are specified, the sprites are sorted according to their priority, and the sprites with higher priority values are displayed in the foreground. (However, if ESDRAW_NOSORT is specified in the processing hold flag, the sort is disabled)
If the p4 and p5 parameters are negative or omitted, the priority values are ignored, and the smaller the sprite No., the more foreground it is displayed.
Here's the translation of the provided text into English, focusing on clarity and accuracy within the context of the HSP3 programming language:

This command returns the number of sprites recognized as drawing targets in the system variable `stat`.

%sample
*gmain
	stick ky,$1f				; Get key information
	if ky&$80 : goto *owari			; [ESC] interrupt check
	redraw 0				; Clear the screen
	es_draw 				; Draw sprites
	redraw 1 				; Update the screen
	await 1000/60 				; System idle
	goto *gmain
%href
es_set
es_setpri
es_setgosub
es_spropt


%index
es_gravity
Gravity Acceleration Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Sprite No. (-1~)
p2(0)=X-axis acceleration (-32767~32767)
p3(0)=Y-axis acceleration (-32767~32767)

%inst
Sets the acceleration for free-fall movement.
^
p2 and p3 are the amount of movement added per frame, with 256 equaling 1 pixel.
^
If -1 is specified for p1, the default value can be set. The default value is automatically set when the es_set command is executed, and the initial values are 0 for both the X and Y axis accelerations.
^
After execution, the system variable `stat` will return a value. If the command is completed successfully, a value of 0 or greater will be returned. If an error occurs for any reason, a negative value will be returned.
%href
es_set
es_pos


%index
es_bound
Bounciness Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Sprite No. (-1~)
p2(128)=Bounciness (0~256)
p3(3)=Flag (1=X-axis, 2=Y-axis, 3=Both, Default=3)
%inst
When gravity acceleration is set, sets the bounciness when a sprite bounces off the sprite display area (the display border on the screen set by the es_area command). The larger this value, the less momentum is lost during the bounce.
^
p3 can be used to set whether to bounce when hitting the top/bottom or left/right edges of the screen. However, this flag is only applied in the direction where free-fall occurs.
^
If -1 is specified for p1, the default value can be set. This value is set when the es_set command is executed, and the initial value is 128.
^
After execution, the system variable `stat` will return a value. If the command is completed successfully, a value of 0 or greater will be returned. If an error occurs for any reason, a negative value will be returned.
%href
es_area
es_set


%index
es_effect
Sprite Special Effect Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Sprite No.
p2($3ff)=Effect Parameter (EP Value)
p3(-1)=Light Color RGB Value (0~$ffffff) (Not implemented)
%inst
Sets the effect (special effect) when drawing sprites.
The value specified by p2 is called the EP value and has the following meanings:
^p
  EP Value     Content
-----------------------------------------------------------
  0~255   Alpha channel value (0=Transparent, 255=Opaque)
  +$300    Enables alpha channel compositing (equivalent to gmode3)
  +$500    Performs additive blending on the original image (equivalent to gmode5)
  +$600    Performs subtractive blending on the original image (equivalent to gmode6)
^p
This value is a combination of the alpha channel value and the blending parameters from the gmode command.
The default value is $3ff (alpha channel=255, equivalent to gmode3).
The p3 parameter allows you to multiply the sprite drawing by a specific color. If the parameter is omitted or -1 is specified, the normal white color ($ffffff) will be used.
%href
es_fade


%index
es_fade
Sprite Blinking/Fade Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Sprite No.
p2(1)=Fade Mode
p3(30)=Timer Initial Value

%inst
Sets a timer for the sprite specified by p1 to execute blinking or fading for a certain period of time.
Sets the fade mode with p2. If omitted, 1 (ESSPF_TIMEWIPE) is selected.
^p
 Fade Mode  Macro              Content
------------------------------------------------------------------------------------
    1            ESSPF_TIMEWIPE      Disappears when the timer expires
    2            ESSPF_BLINK         Blinks, ends when the timer expires
    3            ESSPF_BLINKWIPE     Blinks, disappears when the timer expires
    4            ESSPF_BLINK2        Blinks slowly, ends when the timer expires
    5            ESSPF_BLINKWIPE2    Blinks slowly, disappears when the timer expires
    6            ESSPF_FADEOUT       Fades out over the timer period
    7            ESSPF_FADEOUTWIPE   Fades out over the timer period, then disappears
    8            ESSPF_FADEIN        Fades in over the timer period
    9            ESSPF_FADEINWIPE    Fades in over the timer period, then disappears
   10            ESSPF_EFADE         Fades out at the end of the timer period
   11            ESSPF_EFADEWIPE     Fades out at the end of the timer period, then disappears
   12            ESSPF_EFADE2        Fades out slowly at the end of the timer period
   13            ESSPF_EFADEWIPE2    Fades out slowly at the end of the timer period, then disappears
^p
Specifies the timer duration (number of frames) in p3. The blinking or fading operation will end after this number of frames has elapsed.

ESSPF_FADEOUT/ESSPF_FADEIN perform the fade over the entire specified timer period, while ESSPF_EFADE and ESSPF_EFADEWIPE2 fade at a constant rate regardless of the timer period.

Setting p3 to -1 will make the count infinite.

Please note that fade-in and fade-out will take into account the alpha channel value (transparency) set for the sprite at the time of setting.

In the case of fade-out, the alpha channel value changes from the set value to completely transparent. In the case of fade-in, the change is from completely transparent to the set alpha channel value.

If you select a mode that does not disappear after fading out, be aware that a valid sprite will remain displayed even though it is not visible.
%href
es_effect


%index
es_setpri
Sprite Priority Setting
%group
Extended Screen Control Commands
%prm
p1,p2
p1(0)=Sprite No.
p2(0)=Priority (0~)
%inst
Changes the priority of a sprite.
Sets the value specified by p2 to the sprite specified by p1.
In order to actually draw according to the priority, it is necessary to specify the priority setting parameter of the sprite to be displayed with the es_draw command.
Sprites with a higher priority value are drawn in front.
%href
es_set
es_draw


%index
es_put
Character Screen Display
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5,p6,p7
p1=X coordinate of the upper left corner to display
p2=Y coordinate of the upper left corner to display
p3(0)=Character No. to display
p4(-1)=EP Value
p5(65536)=X Magnification
p6(65536)=Y Magnification
p7(0)=Display Angle
%inst
Displays data registered as a character pattern for sprites at the specified coordinates.
This command directly draws characters on the screen, independent of sprites.
Draws character No. p3 at the coordinates set by (p1, p2) with the EP value of p4.
You can give the X, Y direction magnification (65536 = 100%) in (p5, p6), and the angle value in p7.
%href
es_spropt
es_draw


%index
es_ang
Angle Acquisition
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=X coordinate of the reference point
p2(0)=Y coordinate of the reference point
p3(0)=X coordinate of the target point
p4(0)=Y coordinate of the target point

%inst
The system variable `stat` returns the direction of (p3, p4) as seen from (p1, p2).
You can also get the direction using es_aim, but with this command you can get the direction without using sprites.
%href
es_cos
es_sin
es_dist


%index
es_cos
Trigonometric Function
%group
Extended Screen Control Commands
%prm
p1
p1(0)=Angle (0~ëØn) *n=accuracy set in es_ini 3rd parameter
%inst
Assigns the cosine of angle p1 to the system variable `stat`.
The range of p1 is up to the value set in es_ini.
The value obtained is a fixed point number with 10 bits of decimal part. In other words, the integer obtained by multiplying the value that should have been obtained by 1024 is returned.
%href
es_ang
es_sin
es_dist


%index
es_sin
Trigonometric Function
%group
Extended Screen Control Commands
%prm
p1
p1(0)=Angle (0~ëØn) *n=accuracy set in es_ini 3rd parameter
%inst
Assigns the sine of angle p1 to the system variable `stat`.
The range of p1 is up to the value set in es_ini.
The value obtained is a fixed point number with 10 bits of decimal part. In other words, the integer obtained by multiplying the value that should have been obtained by 1024 is returned.
%href
es_cos
es_ang
es_dist


%index
es_dist
Distance Calculation between 2 Points
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1=Variable to assign the result to
p2(0),p3(0)=Coordinates of Point A
p4(0),p5(0)=Coordinates of Point B
%inst
Calculates the distance from (p2, p3) to (p4, p5) and assigns it to the variable p1. The calculation result is a fixed point number with 8 bits of decimal part.
%href
es_cos
es_sin
es_ang


%index
es_bye
Sprite System Release
%group
Extended Screen Control Commands
%inst
Detaches the standard sprite system.
Releases all the memory that has been allocated. This is automatically called as a cleanup function when HSP ends.
Normally, there is no need to write it specifically.


%index
es_opt
Setting Sprite Repulsion Coordinates
%group
Extended Screen Control Commands
%prm
p1,p2
p1=X coordinate setting
p2=Y coordinate setting

%inst
Sets the ground coordinates for repelling when gravity is applied to the sprite.
If this setting is not made, it will repel at the bottom of the screen.
This command is left for HSPDX compatibility. Please use the es_bound command normally.
%href
es_bound


%index
es_patanim
Batch Character Image Definition
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5
p1(-1)=Character No.
p2(1)=Number to Define
p3=X coordinate of the upper left corner of the character pattern
p4=Y coordinate of the upper left corner of the character pattern
p5(0)=Display frame number (0)
p6=Window buffer ID
%inst
Defines the character patterns of sprites in batches.
^
Character patterns are registered starting from the character No. specified by p1 for the number specified by p2. Use the es_pat command to define a single character No.
The character patterns to be registered are consecutive horizontal areas in the image. Characters are registered assuming that patterns are arranged to the right from the coordinates (p3, p4) with the size set by es_size.
The target image here must be read into the window buffer in advance using the picload command or the celload command.
If the character No. is a negative value, an empty No. is automatically secured and used. In that case, the character No. is assigned to the system variable `stat`.
Extracts a portion of an image and registers it as a character pattern. If a window buffer ID is specified in p5, the image is extracted from that window buffer. If p5 is omitted, the image is extracted from the window buffer that is currently being operated on.

If the p1 parameter is omitted or set to a negative value, an available character number is automatically assigned. After executing this command, the assigned character number is stored in the system variable stat, allowing you to determine which number was used.

The p5 parameter is used to configure character animation. If set to 0, there is no animation. If set to 1 or higher, the character is displayed for the specified number of frames, and then the next character (character number + 1) is displayed.

Normally, an animation is defined where the number of patterns specified in the p2 parameter is repeated with the wait frame of p5. If ESSPPAT_1SHOT($1000) is added to the p2 parameter, a pattern is defined that disappears after animating multiple patterns.

^
Retrieves the X and Y information, such as sprite coordinates, and assigns it to two variables.

Specify the sprite number in p1, and set the variables to be assigned in the p2 and p3 parameters.

You can specify what kind of coordinates to acquire with the setting options of p4.
^p
   p4    Macro Name        Content
--------------------------------------------------
      0  ESSPSET_POS     Sprite X, Y coordinates
      1  ESSPSET_ADDPOS  Sprite X, Y movement component
      2  ESSPSET_FALL    Sprite X, Y falling speed
      3  ESSPSET_BOUNCE  Sprite bounce coefficient
      4  ESSPSET_ZOOM    Sprite X, Y display magnification
      5  ESSPSET_ADDZOOM Sprite X, Y display magnification increment
      6  ESSPSET_CENTER  Sprite center coordinates X, Y
      7  ESSPSET_PUTPOS  Sprite display coordinates X, Y
 0x1000  ESSPSET_DIRECT  Acquire direct 32-bit value
^p
The coordinate values acquired by the es_getpos command are internally 16-bit fixed-point numbers. Usually, they are automatically converted, but conversion is disabled by adding ESSPSET_DIRECT to the setting options.

After execution, a value is returned to the system variable stat. If the command completes successfully, a value of 0 or greater is returned. If an error occurs for some reason, a negative value is returned.
^
Initializes the background map.
^
Initializes a BG map with an arbitrary size.

Specify the BG number to initialize in p1. BG numbers can be specified from 0 to 15.

Specify the variable to store the map data in p2. This variable must be initialized in advance as an array variable with "map overall X size x map overall Y size" elements using the dim command.

Specify the overall X and Y sizes of the map in (p3, p4).

Specify the X and Y sizes of the part of the map to display in (p5, p6).

Parameters p3 to p6 are all in map cell units. For example, if the overall size is 4x4, specify a variable initialized with "dim map, 4*4".

This variable directly holds the content of the map data, and if the value is 0, the map cell number is 0, storing the cell number as is. (The cell number is the same as the split image number specified by the celput command.)

Specify the screen buffer ID in p7 where the map part (cell) images are loaded. Images must be loaded into this buffer in advance using the picload or celload command. (Set the cell size for the map cell image using the celdiv command.)

Configure the behavior of the BG map with the setting options in p8.
^p
     p8  Macro Name          Content
-----------------------------------------------------------------------
      1  ESBGOPT_NOTRANS   Do not perform transparency when drawing cells
  0x100  ESBGOPT_USEMASK   Use the mask pattern of the cell image (not implemented)
^p
The es_bgmap command configures settings related to map display. Actual map display is performed by the es_putbg command.

After execution, a value is returned to the system variable stat. If the command completes successfully, a value of 0 or greater is returned. If an error occurs for some reason, a negative value is returned.
^
Displays the background map.
^
Displays the BG map of the specified BG number.

It is necessary to initialize the BG map size, cell information, etc., in advance with the es_bgmap command.

Specify the display start position (top left) on the screen with the (p2, p3) parameters.

Specify the map display start position (top left) in dot units with the (p4, p5) parameters.

Normally, the map is displayed from the (0, 0) position within the entire map.

By specifying the map display start position, you can change the position of the map that is at the top left when displaying a part of the entire map.

The map display start position (p4, p5) can be used when scrolling a large map.

After command execution, the number of event attributes included in the displayed map parts (cells) is returned to the system variable stat.

Event attribute information can be acquired with the es_getbghit command.

The acquired event attribute information will only be ESMAPHIT_NOTICE and ESMAPHIT_EVENT.
^
If an attribute group value (ESMAP_PRM_GROUP) is set by the es_bgparam command, only cell numbers with the specified group are displayed.

Also, if an animation flag is set in the attribute value, a value with the animation index value (ESMAP_PRM_ANIM) added to the cell number is displayed.
^
If map display fails for some reason, a negative value is returned to the system variable stat.
^
Writes a string to the BG map.
^
Writes the code of the string specified by "str" to the BG map of the specified BG number.

It is necessary to initialize the BG map size, cell information, etc., in advance with the es_bgmap command.

The es_bgmes command operates on the premise that the BG map is in a state where it can display character fonts in ASCII code order.

Writes the character codes included in the string to the right, starting from the map position specified by (p2, p3).
^
Specify the offset value for the character code in the p4 parameter. Normally, it can be 0 or omitted. For example, the character "A" has a character code of 65, and the value 65 is written to the map.

If a line feed code is included, it moves to the next line.
^
If map writing fails for some reason, a negative value is returned to the system variable stat.
^
Sets the parent sprite.
^
Sets the sprite or BG map that is the parent of the specified sprite.

By setting the parent sprite number in the p2 parameter, the sprite will be displayed at a relative coordinate from the parent's coordinate.

If you specify a negative value for p2, the parent sprite setting will be canceled and the sprite will return to normal sprite display.
^
Normally, specify 0 for the setting option in the p3 parameter.

By specifying 1 for the p3 parameter, the sprite is set to display with the BG map as the parent. In that case, you can specify the BG number in the p2 parameter.

By the es_bglink command, you can automatically set the BG map as the parent when setting a new sprite.

If a sprite with a parent sprite further has a parent, the coordinates are recursively searched.
^
After command execution, a negative value is returned to the system variable stat if setting fails for some reason.
^
Changes sprite information collectively.
^
Collectively changes the coordinates of sprites with numbers from p1 to p2.

If the specifications of p1 and p2 are omitted, all sprites will be targeted.
^
You can specify the type value with p3. If 4 is specified for p3, sprites with a type value of 4 are targeted.

If the p3 parameter is omitted or 0 is specified, all type values will be targeted.
^
You can specify the values to add to the sprite coordinate parameters X and Y in the (p4, p5) parameters, respectively. If you specify a negative value, it will be subtracted.

The values specified here will be added (subtracted) to all sprites that are the target of the change.
^
You can set settings other than the sprite's X and Y coordinates with the p6 setting option.
^p
   p4    Macro Name        Content
--------------------------------------------------
      0  ESSPSET_POS     Sprite X, Y coordinates
      1  ESSPSET_ADDPOS  Sprite X, Y movement component
      2  ESSPSET_FALL    Sprite X, Y falling speed
      4  ESSPSET_ZOOM    Sprite X, Y display magnification
 0x1000  ESSPSET_DIRECT  Set direct 32-bit value
^p
You can use the same content for the setting options as the es_pos command.
^
After command execution, a negative value is returned to the system variable stat if it fails for some reason.
es_pos

%index
es_arot
Sprite Auto Rotation Zoom Settings
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=Sprite No.
p2(0)=Rotation angle to add per frame
p3(0)=Magnification to add in the X direction per frame (1/100)
p4(0)=Magnification to add in the Y direction per frame (1/100)
%inst
Sets the settings to automatically add the rotation angle, X magnification, and Y magnification set for the sprite.
Specify the sprite No. with p1, the rotation angle to add per frame with p2, and the magnification to add in the X and Y directions with p3 and p4, respectively.
The values of p3 and p4 are calculated in units where 1x is 100, similar to the magnification setting of the es_setrot command.
^
The specified values will be added to the specified sprite every frame.
If all values of p2, p3, and p4 are set to 0, automatic addition will not be performed.
^
If the command fails for any reason after execution, a negative value is returned to the system variable stat.
%href
es_setrot
es_apos

%index
es_bgparam
BG Map Parameter Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=BG No.(0 to 15)
p2(0)=Setting value
p3(0)=Setting type
%inst
Sets various parameter values for the BG map.
Specify the BG No. with the p1 parameter. BG No. can be specified from 0 to 15.
Set the value of the p2 parameter to the setting type specified by the p3 parameter.
The following values can be specified for the setting type.
^p
  Setting Type Value Macro Name Content
------------------------------------------------------------
           0    ESMAP_PRM_GMODE    Effect Parameter (EP value)
           1    ESMAP_PRM_ANIM     Animation Index Value
           2    ESMAP_PRM_GROUP    Attribute Group Value
           3    ESMAP_PRM_NOTICE   Notification Item Inspection Mask Value
           4    ESMAP_PRM_WIPEDECO Decoration ID to be generated when a sprite is erased
           5    ESMAP_PRM_BLOCKBIT Block Bit Setting Value
           6    ESMAP_PRM_GRAVITY  Default Gravity Setting Value
          16    ESMAP_PRM_OPTION   BG Setting Option Value (ESBGOPT_*)
^p
If the setting fails for any reason, a value other than 0 is returned to the system variable stat.
%href
es_bgmap
es_putbg

%index
es_bgattr
Set BG Map Attribute Value
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=BG No.(0 to 15)
p2(0)=Cell No.(Start)
p3($ffff)=Cell No.(End)
p4(0)=Attribute Value(0 to 65535)
%inst
Sets the attribute value of the BG map.
The attribute value is auxiliary information set for each Cell No. used by the BG map.
Specify the BG No. with p1. BG No. can be specified from 0 to 15.
The value specified by p4 is set as the attribute value for the range of Cell Nos. specified by p2 and p3.
The attribute value can be set from 0 to 65535 (16bit). This information is used for collision detection with the BG by the es_bghit and es_bghitpos commands.
Macros are defined to indicate attribute values. They have the following meanings.
^p
      Value Macro Name Content
--------------------------------------------------
       0  ESMAP_ATTR_NONE   Enterable place (default)
      16  ESMAP_ATTR_NOTICE Enterable and collidable (Notification Item)
      32  ESMAP_ATTR_ANIM   Cell animation flag
      64  ESMAP_ATTR_EVENT  Enterable and collidable (Event Item)
     128  ESMAP_ATTR_HOLD   Enterable but becomes a foothold
     192  ESMAP_ATTR_WALL   Impassable wall
  0x1000  ESMAP_ATTR_MAX    Maximum Cell No.
^p
Bits 0 to 3 of the attribute value (values from 0 to 15) store group information.
It can be used as a bit to display or hide only a specific group.
^
If the setting fails for any reason, a value other than 0 is returned to the system variable stat.
%href
es_bgmap
es_putbg
es_getbgattr
es_bghit
es_bghitpos

%index
es_getbgattr
Get BG Map Attribute Value
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1=Variable name to which the attribute value is assigned
p2(0)=BG No.(0 to 15)
p3(0)=Cell No.
%inst
Acquires the attribute value of the BG map set by the es_bgattr command.
The attribute value is auxiliary information set for each Cell No. used by the BG map. Values from 0 to 65535 are set, and are used for collision detection with the BG by the es_bghit and es_bghitpos commands.
Specify the BG No. with p2. BG No. can be specified from 0 to 15.
The attribute value set for the Cell No. specified by p3 is assigned to the variable specified by p1. The variable of p1 is automatically set as an integer type.
If the setting fails for any reason, a value other than 0 is returned to the system variable stat.
%href
es_bgmap
es_putbg
es_bgattr
es_bghit
es_bghitpos

%index
es_bghit
Execute Collision Detection between BG Map and Sprite
%group
Extended Screen Control Commands
%prm
p1
p1(0)=Sprite No. to perform the determination
%inst
Executes collision detection with a sprite based on the attribute value set in the BG map.
It is necessary to set an appropriate attribute value in the map data in advance using the es_bgattr command.
The es_bghit command determines whether a sprite comes into contact with a character on the BG map when the sprite is moved, and creates a list of results. If you want to perform collision detection based on arbitrary coordinates, use the es_bghitpos command.
Specify the sprite No. with the p1 parameter. The sprite must have collision detection with the BG enabled in advance.
When moving a sprite, it is necessary to set the amount of movement in advance using the es_adir, es_apos commands, etc.
Even if the amount of movement is 0, the direction of movement is automatically set if there are settings such as gravity.

The collision detection result between the BG map and the sprite is created in the same way as the es_bghitpos command. This result can be obtained with the es_getbghit command.
In addition, the outline of the result is stored in the sprite parameter "ESI_MOVERES" that can be obtained with the es_get command. Bits 0 to 7 (0 to 255) of this value store the map attribute Opt value (bits 8 to 15) of the area overlapping with itself.
This value also has the following flags.
^p
      Value Macro Name Content
--------------------------------------------------
    0x100 ESSPRES_XBLOCK    Collision occurred in the X direction
    0x200 ESSPRES_YBLOCK    Collision occurred in the Y direction
    0x400 ESSPRES_GROUND    Touching the foothold in the direction of gravity
    0x800 ESSPRES_EVENT     There was an event attribute
^p
The movement processing of the sprite moved by the es_bghit command is canceled in the es_draw and es_move commands.
If multiple characters are hit simultaneously, multiple results are created. In that case, specify the index value of the result and acquire it with the es_getbghit command.
If execution fails for any reason, a value other than 0 is returned to the system variable stat.
%href
es_bgmap
es_putbg
es_bgattr
es_getbghit
es_bghitpos

%index
es_bghitpos
Execute Collision Detection with BG Map
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8
p1(0)=BG No.(0 to 15)
p2(0)=X coordinate on the BG to be determined (in dots)
p3(0)=Y coordinate on the BG to be determined (in dots)
p4(0)=X size to be determined (in dots)
p5(0)=Y size to be determined (in dots)
p6(0)=Amount of movement in the X direction
p7(0)=Amount of movement in the Y direction
p8(0)=Coordinate Mode Switch (0=16bit/1=32bit)
%inst
Executes collision detection from the specified coordinates based on the attribute value set in the BG map.
It is necessary to set an appropriate attribute value in the map data in advance using the es_bgattr command.
The es_bghitpos command determines whether a rectangular area comes into contact with a character on the BG map when the rectangular area is moved, and creates a list of results. If you want to perform collision detection based on sprite coordinates, use the es_bghit command.
The result can be obtained with the es_getbghit command.
Specify the BG No. with p1. BG No. can be specified from 0 to 15.
Specify the upper left coordinates of the rectangle with the (p2, p3) parameters. This specifies the coordinates on the BG map in dots.
Specify the X and Y sizes of the rectangle in dots with the (p4, p5) parameters.
Specify the amount of movement in the X and Y directions to be moved with the (p6, p7) parameters in dots.
After executing the command, the number of collision detection results created is assigned to the system variable stat. Usually, at least one result is created.
If the p8 parameter is 1, the coordinate values of (p2, p3) and (p6, p7) are treated as 32-bit values of the standard sprite (coordinates including the lower 16 bits) instead of dot units.
If multiple characters are hit simultaneously, multiple results are created. In that case, specify the index value of the result and acquire it with the es_getbghit command.
If execution fails for any reason, a value of 0 is returned to the system variable stat.
%href
es_bgmap
es_putbg
es_bgattr
es_getbghit

%index
es_getbghit
Get Collision Detection Result with BG Map
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1=Variable name to which the collision detection result is assigned
p2(0)=BG No.(0 to 15)
p3(0)=Judgment result index (0~)
%inst
Acquires the result of the BG map collision detection executed by the es_bghit command.
The BG map collision detection is executed based on the attribute value set for each character No. used by the BG map. The attribute value can be set with the es_bgattr command.
Specify the BG No. with the p2 parameter. BG No. can be specified from 0 to 15.
Specify the judgment result index with the p3 parameter. This is the index value when multiple judgment results are created, and starts from 0. (If the p3 parameter is specified as a negative value, the number of judgment results created is assigned to the variable of p1.)
After executing the es_getbghit command, information is assigned to the variable specified by the p1 parameter. The variable of p1 must be initialized in advance as an integer array variable. (In the case of variable result, "dim result,7", etc.)
After execution, the results are assigned to the array elements 0 to 6 of the variable. For example, in the case of the variable result, the results are assigned to result(0) to result(6).
^p
    Array Element Content
----------------------------------------------------------
         (0)  Judgment Result (ESMAPHIT_*)
         (1)  Character No. that came into contact
         (2)  Attribute Value of the Character No. that came into contact
         (3)  X coordinate of the map that came into contact
         (4)  Y coordinate of the map that came into contact
         (5)  Upper left X coordinate of the determined coordinates
         (6)  Upper left Y coordinate of the determined coordinates
^p
%index
Variable(0) contains the judgment result. This value is defined by the ESMAPHIT_* macro.
The following values are assigned based on the attribute value.
^p
      ValueÅ@Macro Name        Content
--------------------------------------------------
       0  ESMAPHIT_NONE   No contact (movable)
       1  ESMAPHIT_HITX   Contact during X-axis movement
       2  ESMAPHIT_HITY   Contact during Y-axis movement
       4  ESMAPHIT_EVENT  Contact information (event item) exists (movable)
       5  ESMAPHIT_NOTICE Contact information (notification item) exists (movable)
       6  ESMAPHIT_SPHIT  Contact with a sprite (one with ESSPMAPHIT_BGOBJ set)
^p
If you specify a negative value for the p3 parameter, the number of judgment result indexes generated in the variable specified by p1 will be assigned.
^
If the result is successfully obtained after executing the es_getbghit instruction, 0 is assigned to the system variable stat.
If the acquisition fails for some reason, a negative value is returned to the system variable stat.
%href
es_bgmap
es_putbg
es_bgattr
es_bghit
es_bghitpos


%index
es_move
Sprite drawing
%group
Extended screen control commands
%prm
p1,p2,p3,p4
p1(1)=Number of frames to move (1Å`)
p2(0)=Starting sprite No.
p3(-1)=Number of sprites to move
p4(0)=Animation control flag (0Å`1)
%inst
Performs sprite movement processing. This is the same as the es_draw instruction, but without the sprite drawing, and is mainly used for frame skipping when the processing load is too high.
The movement process is executed for the number of frames specified by the p1 parameter.
If you want to move only a portion of the registered sprites, specify the starting sprite number in p2 and the number of sprites to draw in p3.
If p2 and p3 are omitted, all sprites are moved.
^
The animation process can be suspended using the p4 parameter.
^p
 Value  Content
------------------------------------------------------------------------
  0  Perform animation
  1  Do not perform animation
^p
This instruction is provided for compatibility with older libraries (hspdxfix). Similar processing can be achieved with the es_draw instruction, so the es_draw instruction is usually used.
When this command is executed, the number of sprites recognized as movement targets is returned to the system variable stat.
%href
es_draw


%index
es_bglink
New sprite map reference settings
%group
Extended screen control commands
%prm
p1,p2
p1(0)=BGNo.(0Å`15)
p2(0)=MAPHIT setting option (ESSPMAPHIT_*)
%inst
Sets the BG map reference setting for sprites newly set with the es_set command.
This command makes it easy to register sprites that synchronize their coordinates with the scrolling BG map.
(This is the same as being linked to the BG map by the es_setparent command.)
The MAPHIT setting option is also added in the same way. The following values can be specified for the MAPHIT setting option.
^p
      ValueÅ@Macro Name             Content
------------------------------------------------------------------
   0x100  ESSPMAPHIT_BGHIT     Performs contact determination based on the attribute value of the map
   0x400  ESSPMAPHIT_BLOCKBIT  Determines attributes that match the block bit setting value as walls
   0x800  ESSPMAPHIT_HITWIPE   Automatically deletes the sprite when it comes into contact with the map
  0x1000  ESSPMAPHIT_WIPEEVENT Deletes the attribute when acquiring map event information
 0x10000  ESSPMAPHIT_BGOBJ     Adds the same contact determination as the map to the sprite
 0x20000  ESSPMAPHIT_STICKSP   Adsorbs to sprites that have contact determination as a map
^p
If the result is successfully obtained after executing the command, 0 is assigned to the system variable stat.
If the setting fails for some reason, a negative value is returned to the system variable stat.
%href
es_set
es_setparent


%index
es_stick
Adsorb to other sprites
%group
Extended screen control commands
%prm
p1,p2
p1(0)=Sprite No.
p2(0)=Sprite No. to absorb to
%inst
Sets the adsorption of sprites. The specified sprite will be adsorbed to another sprite and move.
A sprite with an adsorption setting will move together, maintaining its distance, when the adsorbed sprite moves.
It can be used when temporarily getting on or being pulled by another object.
Specifying a negative value for the p2 parameter cancels the adsorption setting.
^
If the result is successfully obtained after executing the command, 0 is assigned to the system variable stat.
If the execution fails for some reason, a negative value is returned to the system variable stat.
%href
es_setparent


%index
es_regdeco
Register a decoration sprite
%group
Extended screen control commands
%prm
p1,p2,p3,p4,p5,p6
p1(0)=Character No.
p2(0)=Setting option value
p3(-1)=Movement direction
p4(-1)=Movement speed (%)
p5(30)=Display frame count
p6(-1)=Decoration No.
%inst
Registers a decoration sprite.
Decoration sprites are used for temporary decoration purposes and disappear after a certain period of time. They can be used as effects such as explosions or smoke.
After registering the pattern in which the decoration sprite will be displayed using the es_regdeco instruction, it is displayed at arbitrary coordinates using the es_setdeco instruction.
Once registered, the same pattern can be displayed at any time with the es_regdeco instruction.
^
Specify the character No. to display with the p1 parameter.
Specify the setting option for the p2 parameter. The following values can be specified (can be set simultaneously by adding them):
^p
      ValueÅ@Macro Name             Content
------------------------------------------------------------------
       1  ESDECO_FRONT         Display on the frontmost layer
       2  ESDECO_MAPHIT        Enable map collision detection
       4  ESDECO_GRAVITY       Reflects the gravity setting if it is set
       8  ESDECO_ZOOM          Display while zooming in
      16  ESDECO_ROTATE        Display while rotating
      32  ESDECO_BOOST         Double the movement distance
      64  ESDECO_SCATTER       Set the initial rotation angle randomly
   0x100  ESDECO_MULTI4        Sprite x 4
   0x200  ESDECO_MULTI8        Sprite x 8
   0x400  ESDECO_MULTI16       Sprite x 16
  0x1000  ESDECO_CHR2          Character No. is a random value of 2 types
  0x2000  ESDECO_CHR4          Character No. is a random value of 4 types
  0x4000  ESDECO_EPADD         Perform additive composition during display
  0x8000  ESDECO_FADEOUT       Fade out when disappearing
^p
ESDECO_MULTI4, ESDECO_MULTI8, and ESDECO_MULTI16 are options for displaying multiple sprites at the same time. In this case, the rotation angle is assigned according to the number of sprites displayed at the same time.
^
Specify the movement direction with the p3 parameter. This value is the same as the movement direction parameter of the es_adir instruction.
If the specification is omitted or a negative value is specified, it will be randomly determined from all directions.
Specify the movement speed with the p4 parameter. This value is the same as the movement speed parameter of the es_adir instruction.
If the specification is omitted or a negative value is specified, it will be randomly determined from 50 to 200%.
Specify the number of display frames with the p5 parameter. The decoration sprite will be deleted after this number of frames has elapsed.
Specify the decoration No. to overwrite with the p6 parameter. This is used to overwrite an already registered decoration No. Normally, omit the specification or specify a negative value to assign a new decoration No.
There are no restrictions on the registration of decoration No., and decoration No. cannot be deleted.
^
After executing the command, the registered decoration No. is assigned to the system variable stat.
By calling the es_setdeco instruction based on this number, you can display the decoration sprite.
If the setting fails for some reason, a negative value is returned to the system variable stat.
%href
es_setdeco


%index
es_setdeco
Display a decoration sprite
%group
Extended screen control commands
%prm
p1,p2,p3
p1(0)=Sprite X coordinate
p2(0)=Sprite Y coordinate
p3(0)=Decoration No.
%inst
Displays the decoration sprite registered by the es_regdeco command.
The sprite is displayed at the specified coordinates according to the pattern registered in advance.
Decoration sprites are used for temporary decoration purposes and disappear after a certain period of time.
For details, refer to the help for the es_regdeco instruction.
If the execution fails for some reason, a negative value is returned to the system variable stat.
%href
es_regdeco


%index
es_sizeex
Character size detailed specification
%group
Extended screen control commands
%prm
p1,p2,p3,p4,p5,p6,p7,p8,p9
p1(16)=X size of character
p2(16)=Y size of character
p3($3ff)=p4=Special effect (EP value) specification
p4(0)=Collision detection X size
p5(0)=Collision detection Y size
p6(0)=Collision detection offset X
p7(0)=Collision detection offset Y
p8(0)=Display offset X
p9(0)=Display offset Y
%inst
Specifies the size when defining the character pattern of the sprite.
For simple size settings, use the es_size command. The es_sizeex command allows you to set a more detailed collision detection area.
The X and Y sizes of the character are specified by the (p1, p2) parameters.
There are no particular restrictions on the X and Y sizes of the character, but specifying a character pattern that extends beyond the off-screen buffer is invalid.
The settings of the es_size command remain valid for all subsequent character definitions.
^
p3 is the default setting for special effects (EP value). Normally, it can be omitted.
The special effect (EP value) parameter specifies settings such as semi-transparency and additive composition. For details, refer to the standard sprite programming guide.
^
The p4 to p8 parameters are settings related to the hit check area for collision detection (collision detection is used not only for detecting collisions between sprites, but also for the area that collides with map cells).
The area of collision detection is the size specified by (p4, p5) from the location offset (moved) by the coordinates (p6, p7) from the coordinates where the sprite is displayed.
The (p8, p9) parameters specify the offset (movement) value when actually displaying on the screen. By specifying this value, it is possible to shift only the display position while leaving the collision detection area unchanged.
%href
es_size
es_pat

%index
es_bgfetch
Get attribute values within the BG map area
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1(0)=BGNo.(0Å`15)
p2(1)=Search Target Position(0=Down, 1=Right, 2=Up, 3=Left, 4=All)
p3(1)=Search Cell Size
p4(0)=Event Type Specification(Not Implemented)
%inst
Retrieves event attribute information within the map display area shown with the es_putbg command, and in hidden areas above, below, to the left, and to the right.
This is used to acquire event attribute information set in map cells slightly ahead when a wide map is displayed with scrolling. For example, by setting enemy spawn information as an event attribute, you can detect it and trigger the spawn when the screen scroll approaches.
^
Specify the BGNo. with the p1 parameter, and the area with the p2 parameter. You can specify values from the displayed scroll position as 0=Down, 1=Right, 2=Up, 3=Left, 4=All.
Specify the area size of the cells to search with the p3 parameter. For example, if 1(Right) is specified, event attribute information is searched only for the size specified by p3 to the right of the area not displayed on the screen.
The event attribute information that is searched will be ESMAPHIT_NOTICE and ESMAPHIT_EVENT, as with the es_putbg command execution.
^
After the command is executed, the number of event attribute information that was set is assigned to the system variable stat.
Event attribute information can be retrieved with the es_getbghit command.
If execution fails for some reason, the system variable stat returns a negative value.
%href
es_putbg
es_getbghit

%index
es_spropt
Sprite Drawing Option Settings
%group
Extended Screen Control Commands
%prm
p1,p2
p1(0)=Drawing Offset Scaling Setting
p2(0)=Drawing Clipping Setting
%inst
Sets options for sprite drawing.
The p1 parameter sets the offset scaling during sprite drawing.
In standard sprites, when registering a character with the es_sizeex command, you can register a parameter called "Display Offset X,Y" to change the base point of the sprite's display coordinates.
For example, when registering a character with a size of 128 x 128 dots, the character is normally displayed with the specified coordinates as the upper left, but by setting the display offset as (-64, -64), the X and Y are shifted by 64 in the negative direction. That is, the center position of the character can be used as the base point for display.
By using this feature, you can change the center position of rotation and scaling (enlargement/reduction).
The offset scaling setting during drawing determines whether or not the size of this display offset itself is changed by scaling.
^
If the p1 parameter is 0, the display offset is scaled according to the sprite's magnification (set by the es_setrot command, etc.). This is the default setting.
If the p1 parameter is 1, the sprite's magnification does not affect the display offset. Drawing is always performed with a fixed display offset.
This setting can be used selectively at any time.
^
The drawing clipping setting determines whether or not the clipping process performed internally during sprite drawing is performed.
If the p2 parameter is 0, the drawing process is canceled when coordinates outside the screen are specified during sprite drawing. This avoids unnecessary drawing costs. This is the default setting.
If the p2 parameter is 1, the drawing process is performed even if coordinates outside the screen are specified. This makes it possible to execute the drawing process even when a different position than the coordinates originally drawn by the viewcalc command, etc. is specified.
Normally, the drawing clipping setting is fine in its default state, but it is provided for setting in special situations.
^
When initialized with the es_ini command, both the scaling setting and the clipping setting at drawing are initialized and set to 0.
%href
es_ini
es_setrot
es_sizeex
es_draw
es_put

%index
es_nearobj
Get nearby sprite
%group
Extended Screen Control Commands
%prm
p1,p2,p3,p4
p1=Variable name to store the result
p2(0)=Target Sprite No.
p3(0)=Type value to be searched
p4(0)=Search range
%inst
Searches for sprites close to the coordinates of the sprite No. specified by the p2 parameter and assigns the result to the variable of the p1 parameter.
^
You can specify the type value to be searched with the p3 parameter.
If the p3 parameter is omitted or 0, all sprites are targeted. (Multiple types can be specified simultaneously for the p3 type value. For example, to target type1 and type4, specify 1+4, which is 5.)
^
You can specify the search range with the p4 parameter. For example, if 100 is specified, sprites within a straight-line distance of 100 (dots) are targeted.
If the p3 parameter is omitted or 0, sprites in all ranges are targeted.
^
The result is assigned to the variable specified by p1. If there is no sprite that meets the conditions, -1 is assigned. If there are multiple sprites that meet the conditions, the sprite No. of the one with the closest straight-line distance is assigned.
Since it only acquires the proximity of coordinates, the hit check area (%) and size of the sprite are not considered. To detect collisions with other sprites, use the es_check command.
%href
es_check
es_type

%index
es_setlife
Sprite Life Value Setting
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)=Target Sprite No.
p2(0)=Setting Value
p3(0)=Setting Option
%inst
Sets the life value and power value that the sprite has.
The life value can hold an integer value that increases or decreases, such as hit points or energy, which the sprite has uniquely.
The power value can hold an integer value that is the amount of attack that the sprite has uniquely.
The life value and power value do not change the behavior of the sprite itself. They are merely held as unique information and users are free to decide their use.
All sprites have Life Value/Max Life Value = 100, and Power Value = 1 as initial values.
The es_setlife command provides a mechanism for handling the life value as hit points and the power value as attack power.
It provides assistance for arbitrarily increasing or decreasing the life value and performing processing according to the result.
You can specify setting options with the p3 parameter.
^p
 Value   Content
--------------------------------------------------
 -1   Ignore the setting value and only get the life value
  0   Set the setting value as the life value
  1   Set the setting value as the power value
  2   Set the setting value as the maximum life value
  4   Subtract (minus) the setting value (p2) from the life value of the target sprite
  5   Subtract (minus) the power value of the sprite specified by p2 from the life value of the target sprite
^p
After executing this command, the final life value of the target sprite is assigned to the system variable stat.
For example, if the No. 0 sprite has a life value of 100, executing "es_setlife 0, 20, 4" will set the life value to 100-20 = 80, and 80 will be assigned to the system variable stat.
The life value will never be less than 0. Also, the upper limit is aligned with the maximum life value.
Even if the life value becomes 0, the sprite itself will not disappear. It only holds information.
%href
es_get
es_setp
