;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Extension instructions
%ver
3.4
%note
Include obaq.as.

%date
2014/04/24
%author
onitama
%dll
obaq
%url
http://hsp.tv/
%port
Win
%group
Extended I / O control instructions


%index
qreset
OBAQ initialization
%inst
All the information that OBAQ has is initialized.
When the qreset command is used for initialization, the wall (frame) that surrounds the entire screen is automatically set.
If you want to change the size or position of the wall, reset it with the qborder command.
The window that was set as the drawing target of HSP when qreset is executed is also the drawing target of OBAQ.
If you want to change the window to be drawn, use the qview instruction.
%href
qview
qborder
qterm


%index
qterm
OBAQ termination processing
%inst
Discards all information that OBAQ has.
It is called internally when the program ends.
Normally you do not need to call it.
%href
qreset


%index
qexec
OBAQ frame processing
%inst
Performs frame-by-frame physics.
This will update all object coordinates.
In the main loop, it is necessary to move and update the object by using the qexec instruction.
Also, when using internal drawing, it is necessary to update the screen with the qdraw command.
%href
qdraw


%index
qdraw
OBAQ drawing process
%prm
mode
mode (0): Debug display mode setting (0 = ON / 1 = OFF)
%inst
Performs object drawing processing by OBAQ.
Appropriate display is performed based on the material information set for each object.
You can set ON / OFF of the debug display by setting the mode.
During debug display, you can check the shape, center position, group, etc. of the object by wireframe.
Use the qview command to change the display destination window and set the display magnification and display position offset.
Drawing by the qdraw command uses the Windows system API (GDI), so
It is also necessary to clear the screen and update with the redraw command.
When drawing with another system (DirectX, etc.), only update with the qexec command, and then update.
You need to get the display position of each object by yourself and draw it.
(Qcnvaxis and qgetaxis instructions are provided for converting display coordinates.)

%href
qview
qexec
qcnvaxis
qgetaxis


%index
qview
View settings
%prm
zoomx,zoomy,ofsx,ofsy
zoomx (4.0): Zoom magnification in the X direction (real number)
zoomy (4.0): Zoom magnification in the Y direction (real number)
ofsx (0.0): Display offset in the X direction (real number)
ofsy (0.0): Display offset in the Y direction (real number)
%inst
Set the display when drawing with the qdraw command.
Set the display magnification (magnification when reflecting the internal coordinates of OBAQ on the screen) with zoomx and zoomy.
The display position offset can be set by ofsx, ofsy.
Also, when qview is executed, the window that was set as the drawing target of HSP will be reset as the drawing target.

%href
qdraw
qreset


%index
qsetreq
System request settings
%prm
reqid,val
reqid (0): Request ID
val (0.0): Set value (real number)
%inst
Make various system settings for OBAQ.
The following symbols can be specified by the reqid value.
^p
Symbol name Contents
------------------------------------------------------------------------
REQ_PHYSICS_RATE Number of physics calculations per frame
REQ_MAXOBJ Maximum number of objects (within 512) (*)
REQ_MAXLOG Maximum number of collision logs (*)
REQ_DEFAULT_WEIGHT Default weight
REQ_DEFAULT_MOMENT Default moment
REQ_DEFAULT_FRICTION Default friction

The contents of the (*) symbol are not reflected until the qreset instruction is executed.
^p

%href
qgetreq


%index
qgetreq
Get system request
%prm
var,reqid
var: Variable to get the contents
reqid (0): Request ID
%inst
Gets the OBAQ system settings.
Reads the contents into the variable specified by var.
Variables in var are automatically set to real type.
The following symbols can be specified by the reqid value.
^p
Symbol name Initial value Contents
------------------------------------------------------------------------------
REQ_PHYSICS_RATE 4 Number of physics calculations per frame
REQ_MAXOBJ 512 Maximum number of objects (within 512) (*)
REQ_MAXLOG 256 Maximum number of collision logs (*)
REQ_DEFAULT_WEIGHT 6.0 Default weight
REQ_DEFAULT_MOMENT 1200.0 Default moment
REQ_DEFAULT_FRICTION 1.0 Default friction

The contents of the (*) symbol are not reflected until the qreset instruction is executed.
^p

%href
qsetreq


%index
qborder
Set the outer wall
%prm
x1,y1,x2,y2
x1 (-100): Upper left X coordinate (real number)
y1 (-100): Upper left Y coordinate (real number)
x2 (100): Lower right X coordinate (real number)
y2 (100): Lower right Y coordinate (real number)
%inst
Resets the standard exterior wall size.
Specify the upper left and lower right coordinates (OBAQ internal coordinates) with the center of the screen as (0,0).
The outer wall is when an object is placed on the screen without any wall.
This is to prevent it from falling off the screen.
%href
qreset


%index
qgravity
Set gravity
%prm
gx,gy
gx (0.0): Gravity in the X direction (real number)
gy (0.005): Gravity in the Y direction (real number)
%inst
Sets the gravity of space.
The initial value is set to (0,0.005).

%href
qreset


%index
qcnvaxis
Convert X, Y coordinate values
%prm
var_x,var_y,x,y,opt
var_x: Variable to get X
var_y: Variable to get Y
x (0.0): X coordinate of conversion source (real number)
y (0.0): Y coordinate of conversion source (real number)
opt (0): Conversion mode
%inst
Converts the X and Y coordinate values according to the specified method.
If the conversion source coordinates are specified by (x, y), the converted X and Y coordinate values are assigned to var_x and var_y, respectively.
The following values can be selected for the conversion mode specified by opt.
^p
Conversion mode contents
------------------------------------------------------------------------
0 Convert internal coordinates to on-screen coordinates
1 Convert on-screen coordinates to internal coordinates
^p
Conversion mode 0 (or default) converts the internal coordinates to the on-screen coordinates displayed by the qdraw instruction. (var_x and var_y are set to integer type.)
Conversion mode 1 converts the on-screen coordinates displayed by the qdraw command to internal coordinates. (var_x and var_y are set to real type.)

%href
qgetaxis


%index
qgetaxis
Get internal coordinates
%prm
num,var_x,var_y,type
num (0): Object ID
var_x: Variable to get X
var_y: Variable to get Y
type (0): acquisition type
%inst
Read the internal coordinates and display parameters of the OBAQ object into variables.
var_x and var_y are automatically set as integer types.
The contents that can be specified by type are as follows.
^p
Acquisition type Contents
------------------------------------------------------------------------
0 Upper left coordinate of the object
1 Lower right coordinate of the object
2 Object X, Y size (*)

For the (*) symbol, the size (number of dots) on the display screen is returned.
^p

%href
qcnvaxis


%index
qdel
Delete object
%prm
num
num (0): Object ID
%inst
Deletes the specified object.
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qaddpoly
qaddmodel


%index
qaddpoly
Add polygon object
%prm
var,shape,x,y,r,sx,sy,colsw,mygroup,exgroup,loggroup
var: Variable to get the object ID
shape (3): Number of vertices
x (0.0): Placement X coordinate (real number)
y (0.0): Placement Y coordinate (real number)
r (0.0): Placement angle (real number) (unit is radian)
sx (10.0): Placement X size (real number)
sy (10.0): Placement Y size (real number)
colsw (1): Placement check mode
mygroup (1): Collision group to which you belong
exgroup (0): Group to exclude collisions
loggroup (0): Group that creates collision logs
%inst
Add a polygonal object to the OBAQ.
An object with the vertices specified by shape will be created.
(The number of vertices in the shape must be 3 or more.)
Specify the coordinates of the placement with (x, y). (Set the internal coordinates of OBAQ.)
Specify the placement angle (+ direction is clockwise) with r. The unit is radians.
Use (sx, sy) to specify the size at the time of placement. By default, the size (10,10) is used.
mygroup, exgroup, loggroup are the group values set for collision detection.
The group value is indicated by a value in which one of the 32 bits is 1 (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768Åc).
You can select the mode of placement check with colsw.
^p
Mode value Contents
------------------------------------------------------------------------
colsw_none 0 No collision check when appearing
colsw_active 1 Avoid contact with active rocks when spawning
colsw_all 2 Avoid contact with rocks other than reserve when spawning
^p
When the object is added successfully, the object ID value is assigned to the variable specified by var. (Variables are automatically integer type.)
If no object is added due to the registration limit, -1 is assigned to the variable specified by var.
If no object is added due to collision detection, -2 is assigned to the variable specified by var.

%href
qaddmodel


%index
qaddmodel
Free setting object added
%prm
var,shape_var,nvertex,x,y,r,sx,sy,colsw,mygroup,exgroup,loggroup
var: Variable to get the object ID
shape_var: Array variable to which shape data is assigned
nvertex (-1): Number of shape data
x (0.0): Placement X coordinate (real number)
y (0.0): Placement Y coordinate (real number)
r (0.0): Placement angle (real number) (unit is radian)
sx (10.0): Placement X size (real number)
sy (10.0): Placement Y size (real number)
colsw (1): Placement check mode
mygroup (1): Collision group to which you belong
exgroup (0): Group to exclude collisions
loggroup (0): Group that creates collision logs
%inst
Add an object with a freely set shape to OBAQ.
Common polygons can be generated with the qaddpoly command.
With the qaddmodel command, you can register more free forms by specifying the vertices individually.
It is necessary to specify the array variable to which the shape data is assigned to shape_var.
Store the coordinates in the array variable in the order of x1, y1, x2, y2 ...
The coordinates must be specified counterclockwise and must be convex.
It is also possible to include multiple convex bodies in one object.
In that case, after making the end of the coordinates specified counterclockwise the same as the first coordinates and making it a closed form,
Be sure to specify a new, different form of coordinates.
^
Specify the number of data (not the number of vertices) with nvertex.
If nvertex is negative or omitted, the number of elements set in the array will be used.
Either real type or integer type can be used for the array variable that stores the shape data.
Please note that it is not possible to handle data that exceeds the specified number of vertices. See the OBAQ.DLL documentation for more information.
^p
Added free-form model
	;
	model=-1.0,-1.0, -1.0,2.0, 2.0,1.0, 1.0,-1.0
	qaddmodel i,model,-1, 30,64,0
^p
Specify the coordinates of the placement with (x, y). (Set the internal coordinates of OBAQ.)
Specify the placement angle (+ direction is clockwise) with r. The unit is radians.
Use (sx, sy) to specify the size at the time of placement. By default, the size (10,10) is used.
mygroup, exgroup, loggroup are the group values set for collision detection.
The group value is indicated by a value in which one of the 32 bits is 1 (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768Åc).
You can select the mode of placement check with colsw.
^p
Mode value Contents
------------------------------------------------------------------------
colsw_none 0 No collision check when appearing
colsw_active 1 Avoid contact with active rocks when spawning
colsw_all 2 Avoid contact with rocks other than reserve when spawning
^p
When the object is added successfully, the object ID value is assigned to the variable specified by var. (Variables are automatically integer type.)
If no object is added due to the registration limit, -1 is assigned to the variable specified by var.
If no object is added due to collision detection, -2 is assigned to the variable specified by var.

%href
qaddpoly


%index
qtype
Set type parameter
%prm
num,type,option
num (0): Object ID
type (0): set value
option (0): Configuration option
%inst
Sets the type parameter for the specified object.
The following contents can be specified for the type parameter.
^p
Macro name value Content
-----------------------------------------------------------
type_normal 0 Normal object
type_inner 1 Object with inverted contact judgment
type_bindX 0x40 Fixed with X axis in the background
type_bindY 0x80 Fixed with Y axis in the background
type_bindR 0x100 Fixed rotation
type_bind 0x1c0 Completely fixed to the background (no physical behavior)
type_autowipe 0x100000 Automatically erase when the border range is exceeded
^p
type_inner is used when creating the outer wall.
Used for the default wall (the shape created by the qborder instruction),
Be careful not to use it for any other purpose.
type_autowipe is a flag to automatically delete objects that are outside the border area (frame) set by the qborder command.
^
With option, you can specify how to set the type value.
^p
option content
------------------------------------------------------------------------
0 type Set the value (set)
1 Add type value (or)
2 Exclude type value (not)
^p
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgettype
qborder


%index
qstat
Set stat parameter
%prm
num,stat,sleepcount
num (0): Object ID
stat (0): set value
sleepcount (0): sleep count value
%inst
Sets the stat parameter and sleep count value for the specified object.
The following contents can be specified for the stat parameter.
^p
Macro name value Content
-------------------------------------------------------------------
stat_reserve 0 unused object
stat_sleep 1 Waiting object
stat_active 2 Appearing objects
^p
With option, you can specify how to set the type value.
The sleep count value is the value that is referenced when stat_sleep is specified for stat.
Objects that are in stat_sleep will blink as many times as the sleep count value.
Will appear. (If the sleep count value is 0, it will not appear and will remain ready.)
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetstat


%index
qpos
Set position and angle parameters
%prm
num,x,y,angle
num (0): Object ID
x (0.0): X coordinate setting value (real number)
y (0.0): Y coordinate setting value (real number)
angle (0.0): Angle setting value (real number) (radian)
%inst
Sets the position and angle parameters of the specified object.
Specify the reference position (center coordinates) of the object with (x, y).
Specify the object rotation angle (+ direction is clockwise) with angle. The unit is radians.
In OBAQ, even when you move an object, you can directly change the coordinates of the object.
Apply force in the direction you want to move instead of changing it, or by the qgetspeed command
Please realize by setting the acceleration.
If you forcibly change the coordinates, the physical behavior may not be correct.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetpos


%index
qspeed
Set speed parameters
%prm
num,px,py,pr,option
num (0): Object ID
px (0.0): X direction setting value (real number)
py (0.0): Y direction setting value (real number)
pr (0.0): Rotation speed parameter (real number)
option (0): Configuration option
%inst
Sets the velocity parameter for the specified object.
(px, py) specifies the moving speed of the object in the X and Y directions.
Specify the speed for rotation with pr.
You can specify the setting method with option.
^p
option content
------------------------------------------------------------------------
0 Add velocity value (add)
1 Overwrite speed value x 2 (set)
2 Overwrite speed value (set)
^p
If execution fails, a non-zero value is assigned to the system variable stat.
If the option value is 2, the specified value is set.
Note that if the option value is 1, it will be set to twice the specified value.

%href
qgetspeed


%index
qweight
Set parameters such as weight
%prm
num,weight,moment
num (0): Object ID
weight (6.0): Weight value (real number)
moment (1200.0): Moment value (real number)
%inst
Sets parameters such as weight, moment, and friction for the specified object.
weight is the standard specific gravity for a circle with radius r
^p
	(r ^ 2) / 32.0
^p
Please set as a guide.
Think of moment as a parameter related to ease of rotation.
^p
Disk with radius r: weight * (r ^ 2) / 2.0
Rectangle with side lengths a and b: weight * ((a ^ 2) + (b ^ 2)) / 12.0
^p
Set a value like this as a guide.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetweight


%index
qdamper
Set parameters such as vibration absorption
%prm
num,damper,friction
num (0): Object ID
damper (0.7): Vibration absorption parameter value (real number)
friction (1.0): Friction resistance value (real number)
%inst
Sets parameters such as vibration damping, inertia, and gravitational constant for the specified object.
Damper indicates the ability to suppress shaking when in contact.
Be sure to set a value between 0.0 and 1.0. (No range check)
Friction is the coefficient of force that decays when objects come into contact with each other.
The value multiplied by the friction of the contacting object is referenced and calculated.
Be sure to set a value between 0.0 and 1.0. (No range check)
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetdamper


%index
qinertia
Set parameters such as inertia
%prm
num,inertia,gravity
num (0): Object ID
inertia (0.999): Inertia parameter value (real number)
gravity (1.0): Gravity value (real number) of the object
%inst
Sets parameters such as the inertia of the specified object and the gravitational constant.
Inertia becomes a sticky movement due to resistance generated as it approaches 0.0.
Become. Be sure to set a value between 0.0 and 1.0. (No range check)
gravity (object gravity) is multiplied by the gravity set in space.
It is reflected only in the object.
It is used to apply different gravity only to a specific object.
It is effective for character control such as games because it gives weightlessness and antigravity only to specific objects in a form that is impossible in reality.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetinertia


%index
qgroup
Set group parameters
%prm
num,mygroup,exgroup,loggroup
num (0): Object ID
mygroup (1): Collision group value to which you belong
exgroup (0): Group value to exclude collisions
loggroup (0): Group value that creates the collision log
%inst
Sets the group parameters for the specified object.
Group parameters are used to identify conflicting objects, such as when identifying them.
The group value is indicated by a value in which one of the 32 bits is 1 (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768Åc).
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetgroup
qfind
qcollision


%index
qmat
Set material parameters
%prm
num,type,id,subid
num (0): Object ID
type (0): Material type value
id (0): Reference id value
subid (0): Reference subid value
%inst
Sets the material parameters for the specified object.
Material parameters are reflected when drawing with the qdraw command.
The material type value allows you to choose how to display it.
^p
Type Value Content
-------------------------------------------------------------------
mat_none 0 Do nothing (hidden)
mat_spr 1 sprite
mat_spr2 2 Sprite (with size setting)
mat_wire 3 wireframe
mat_wire2 4 Wireframe (with contour)
mat_delay 0x1000 Draw after
^p
Sprites are pasted with a rectangle that covers the entire shape.
In the case of wireframe, connect each vertex with a line.
If mat_spr or mat_spr2 is selected, it will be displayed as a sprite in id.
Specify the split image number in the window ID and subid of the image.
The window ID and split image number are the parameters used in the celput instruction.
Similarly, you can refer to the image that is the material divided by the celdiv command.
If mat_wire or mat_wire2 is selected, the wire color code with id
(24bit value packed with RGB), set the outline color code with subid.
The object obtained by adding mat_delay (0x1000) to the material type value is
It will be drawn later.
This can be used for display using translucency.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetmat
qmat2
qmat3


%index
qmat2
Set material detail parameters
%prm
num,offsetx,offsety,zoomx,zoomy
num (0): Object ID
offsetx (0): X-direction display offset
offsety (0): Y direction display offset
zoomx (1.0): X-direction display magnification (real number)
zoomy (1.0): Y-direction display magnification (real number)
%inst
Sets the material detail parameters for the specified object.
The material detail parameters are reflected when drawing with the qdraw command.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetmat2
qmat
qmat3


%index
qmat3
Set material display parameters
%prm
num,gmode,rate
num (0): Object ID
gmode (0): copy mode
rate (256): translucent rate
%inst
Sets the material display parameters for the specified object.
The material display parameters are reflected when drawing with the qdraw command.
gmode specifies the copy mode when drawing the sprite.
This is the same as the mode value specified by the HSP gmode instruction.
Also, rate (semi-transparent rate) has the same result as when specified by the gmode instruction.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
gmode
qgetmat3
qmat
qmat2


%index
quser
Set user-defined data
%prm
num,user1,user2,user3
num (0): Object ID
user1 (0): User-defined data 1
user2 (0): User-defined data 2
user3 (0): User-defined data 3
%inst
Sets the user-defined data for the specified object.
User-defined data is a value that can be saved for each object, and the user can freely decide what to use it for.
User-defined data 1 to 3 can store integer type values.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetuser
quser2


%index
quser2
Set user-defined data 2
%prm
num,user4,user5,user6
num (0): Object ID
user4 (0.0): User-defined data 4 (real number)
user5 (0.0): User-defined data 5 (real number)
user6 (0.0): User-defined data 6 (real number)
%inst
Sets the user-defined data for the specified object.
User-defined data is a value that can be saved for each object, and the user can freely decide what to use it for.
User-defined data 4 to 6 can store real-type values.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetuser2
quser


%index
qfind
Object search
%prm
group,stat
group (-1): Group value to search
stat (0): stat value specification
%inst
Search for registered objects with arbitrary conditions.
First, you need to specify the condition with the qfind instruction and then receive the result with the qnext instruction.
Specify the collision group value to search by group.
If the group specification is omitted or -1, all groups are targeted.
(However, objects with a group value of 0 are excluded)
When specifying multiple groups, specify the value obtained by adding the values of each group.
By specifying stat, it is possible to search only a specific state.
If stat is 0, all states are covered.
If you specify stat_sleep (1) or stat_active (2) for stat, only objects with the same value are targeted.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qnext


%index
qnext
Object search result acquisition
%prm
var
var: Variable to get search results
%inst
Get the result of the search by the qfind instruction.
First, you need to specify the condition with the qfind instruction and then receive the result with the qnext instruction.
The object ID value is assigned to the variable specified by var. (Variables are automatically integer type.)
You can retrieve the search results each time you execute the qnext instruction.
If there are no more search results, -1 will be substituted.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind


%index
qcollision
Collision acquisition started
%prm
num, target
num (0): Object ID
target (-1): Object ID of the target to get the collision
%inst
Starts getting information about collisions between objects.
First, you need to specify the object with the qcollision instruction and then receive the result with the qgetcol instruction.
In target, the object ID for which you want to get a collision.
If target is -1 or omitted, all information related to the object specified by num is retrieved.
When acquiring a collision, it is necessary to set the collision group.
Please note that only the parameters of the object specified by num that are in the collision log creation group will be acquired.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgroup
qgetcol
qgetcol2
qgetcol3


%index
qgetcol
Collision acquisition
%prm
var_id,var_x,var_y
var_id: Variable to get the object ID
var_x: Variable to get the X coordinate
var_y: Variable to get the Y coordinate
%inst
Acquires the result of the search by the qcollision command.
First, you need to specify the condition with the qcollision instruction and then receive the result with the qgetcol instruction.
Furthermore, if necessary, additional information can be obtained with the qgetcol2 and qgetcol3 instructions.
The ID value of the object in which the collision was detected is assigned to the variable specified by var_id. (Variables are automatically integer type.)
The variable specified by var_x is assigned the X coordinate where the collision was detected. (Variables are automatically real type.)
The Y coordinate where the collision was detected is assigned to the variable specified by var_y. (Variables are automatically real type.)
You can retrieve the search results each time you execute the qgetcol instruction.
If there are no more search results, -1 will be assigned as the object ID.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol2
qgetcol3


%index
qgetcol2
Collision detailed information acquisition
%prm
var_depth,var_nx,var_ny
var_depth: Variable to get the depth of collision
var_nx: Variable to get the X normal of the collision
var_ny: Variable to get the Y normal of the collision
%inst
Acquires the result of the search by the qcollision command.
Be sure to get the information after receiving the result with the qgetcol instruction.
The collision depth (distance sunk into the contact surface) is assigned to the variable specified by var_depth. (Variables are automatically real type.)
The normal (x, y) at the time of collision is assigned to the variable specified by var_nx, var_ny. (Variables are automatically real type.)
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol
qgetcol3


%index
qgetcol3
Collision detailed information acquisition 2
%prm
var_bound,var_slide
var_bound: Variable to get the vertical velocity
var_slide: Variable to get the horizontal velocity
%inst
Acquires the result of the search by the qcollision command.
Be sure to get the information after receiving the result with the qgetcol instruction.
The variable specified by var_bound is assigned the velocity in the direction perpendicular to the contact surface (bounce) (the plus side is the sinking direction). (Variables are automatically real type.)
The variable specified by var_slide is assigned the velocity in the horizontal (dragging) direction with the contact surface. (Variables are automatically real type.)
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol
qgetcol2


%index
qgettype
Get type parameter
%prm
num,var_type
num (0): Object ID
var_type: variable to get the type parameter
%inst
Gets the parameters of the specified object in a variable.
The type parameter value is assigned to the variable specified by var_type. (Variables are automatically integer type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qtype


%index
qgetstat
Get stat parameter
%prm
num,var_stat,var_sleepcount
num (0): Object ID
var_stat: stat Variable to get the setting value
var_sleepcount: Variable to get the sleep count value
%inst
Gets the parameters of the specified object in a variable.
The stat parameter value is assigned to the variable specified by var_stat. (Variables are automatically integer type.)
The sleep count value is assigned to the variable specified by var_sleepcount. (Variables are automatically integer type.)
Please note that the sleep count value is not the value set by qstat, but the value according to the number of internal physical calculation executions.
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qstat


%index
qgetpos
Get position and angle parameters
%prm
num,var_x,var_y,var_angle
num (0): Object ID
var_x: Variable to get the X coordinate setting value
var_y: Variable to get the Y coordinate setting value
var_angle: Variable to get the angle setting value
%inst
Gets the parameters of the specified object in a variable.
The X coordinate setting value is assigned to the variable specified by var_x. (Variables are automatically real type.)
The Y coordinate setting value is assigned to the variable specified by var_y. (Variables are automatically real type.)
The angle setting value is assigned to the variable specified by var_angle. (Variables are automatically real type.)
Please note that the acquired X and Y coordinates are internal coordinates used by OBAQ, not on-screen coordinates.
Use the qcnvaxis instruction to convert internal coordinate values to on-screen coordinates.
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qpos
qcnvaxis


%index
qgetspeed
Get speed parameters
%prm
num,var_px,var_py,var_pr
num (0): Object ID
var_px: Variable to get the velocity value in the X direction
var_py: Variable to get the velocity value in the Y direction
var_pr: Variable to get the rotation speed value
%inst
Gets the parameters of the specified object in a variable.
The X-direction velocity value is assigned to the variable specified by var_px. (Variables are automatically real type.)
The Y-direction velocity value is assigned to the variable specified by var_py. (Variables are automatically real type.)
The rotation speed value is assigned to the variable specified by var_pr. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qspeed


%index
qgetweight
Get parameters such as weight
%prm
num,var_weight,var_moment
num (0): Object ID
var_weight: Variable to get the weight value
var_moment: Variable to get the moment value
%inst
Gets the parameters of the specified object in a variable.
The weight value is assigned to the variable specified by var_weight. (Variables are automatically real type.)
The moment value is assigned to the variable specified by var_moment. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qweight


%index
qgetdamper
Get parameters such as vibration absorption
%prm
num,var_damper,var_friction
num (0): Object ID
var_damper: Variable to get the vibration absorption parameter value
var_friction: Variable to get the friction resistance value
%inst
Gets the parameters of the specified object in a variable.
The vibration absorption parameter value is assigned to the variable specified by var_damper. (Variables are automatically real type.)
The frictional resistance value is assigned to the variable specified by var_friction. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qdamper


%index
qgetinertia
Get parameters such as inertia
%prm
num,var_inertia,var_gravity
num (0): Object ID
var_inertia: Variable to get the inertia parameter value
var_gravity: Variable to get the gravity value of the object
%inst
Gets the parameters of the specified object in a variable.
The inertia parameter value is assigned to the variable specified by var_inertia. (Variables are automatically real type.)
The gravity value of the object is assigned to the variable specified by var_gravity. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qinertia


%index
qgetgroup
Get group parameters
%prm
num,var_mygroup,var_exgroup,var_loggroup
num (0): Object ID
var_mygroup: Variable to get the collision group value to which you belong
var_exgroup: Variable to get the group value to exclude collisions
var_loggroup: Variable to get the group value that creates the collision log
%inst
Gets the parameters of the specified object in a variable.
The collision group value to which you belong is assigned to the variable specified by var_mygroup. (Variables are automatically integer type.)
The variable specified by var_exgroup is assigned the group value that excludes collisions. (Variables are automatically integer type.)
The variable specified by var_loggroup is assigned the group value that creates the collision log. (Variables are automatically integer type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qgroup
qfind
qcollision


%index
qgetmat
Get material parameters
%prm
num,var_type,var_id,var_subid
num (0): Object ID
var_type: Variable to get the material type value
var_id: Variable to get the reference id value
var_subid: Variable to get the reference subid value
%inst
Gets the parameters of the specified object in a variable.
The material type value is assigned to the variable specified by var_type. (Variables are automatically integer type.)
The reference id value is assigned to the variable specified by var_id. (Variables are automatically integer type.)
The reference subid value is assigned to the variable specified by var_subid. (Variables are automatically integer type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qmat
qgetmat2
qgetmat3


%index
qgetmat2
Get material detail parameters
%prm
num,var_offsetx,var_offsety,var_zoomx,var_zoomy
num (0): Object ID
var_offsetx: Variable to get the X-direction display offset value
var_offsety: Variable to get the Y-direction display offset value
var_zoomx: Variable to get the X-direction display magnification value
var_zoomy: Variable to get the Y-direction display magnification value
%inst
Gets the parameters of the specified object in a variable.
The X-direction display offset value is assigned to the variable specified by var_offsetx. (Variables are automatically integer type.)
Gets the parameters of the specified object in a variable.
The Y-direction display offset value is assigned to the variable specified by var_offsety. (Variables are automatically integer type.)
Gets the parameters of the specified object in a variable.
The X-direction display magnification value is assigned to the variable specified by var_zoomx. (Variables are automatically real type.)
Gets the parameters of the specified object in a variable.
The Y-direction display magnification value is assigned to the variable specified by var_zoomy. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qmat2
qgetmat
qgetmat3


%index
qgetmat3
Get material display parameters
%prm
num,var_gmode,var_rate
num (0): Object ID
var_gmode: Variable to get the copy mode value
var_rate: Variable to get the translucent rate value
%inst
Gets the parameters of the specified object in a variable.
The copy mode value is assigned to the variable specified by var_gmode. (Variables are automatically integer type.)
The translucent rate value is assigned to the variable specified by var_rate. (Variables are automatically integer type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qmat3
qgetmat
qgetmat2


%index
qgetuser
Get user-defined data
%prm
num,var_user1,var_user2,var_user3
num (0): Object ID
var_user1: Variable to get the value of user-defined data 1.
var_user2: Variable to get the value of user-defined data2
var_user3: Variable to get the value of user-defined data 3
%inst
Gets the parameters of the specified object in a variable.
The value of user-defined data 1 is assigned to the variable specified by var_user1. (Variables are automatically integer type.)
The value of user-defined data 2 is assigned to the variable specified by var_user2. (Variables are automatically integer type.)
The value of user-defined data 3 is assigned to the variable specified by var_user3. (Variables are automatically integer type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
quser
qgetuser2


%index
qgetuser2
Get user-defined data 2
%prm
num,var_user4,var_user5,var_user6
num (0): Object ID
var_user4: Variable to get the value of user-defined data 4
var_user5: Variable to get the value of user-defined data 5
var_user6: Variable to get the value of user-defined data 6.
%inst
Gets the parameters of the specified object in a variable.
The value of user-defined data 4 is assigned to the variable specified by var_user4. (Variables are automatically real type.)
The value of user-defined data 5 is assigned to the variable specified by var_user5. (Variables are automatically real type.)
The value of user-defined data 6 is assigned to the variable specified by var_user6. (Variables are automatically real type.)
If execution fails, a non-zero value is assigned to the system variable stat.

%href
quser2
qgetuser


%index
qpush
Empower anywhere
%prm
num,xw,yw,ax,ay,sw
num (0): Object ID
xw (0.0): X coordinate (real number)
yw (0.0): Y coordinate (real number)
ax (0.0): Force in the X direction (real number)
ay (0.0): Force in Y direction (real number)
sw (1): configuration options
%inst
Apply force to the specified object from the position of (xw, yw) with the strength of (ax, ay).
The posture and speed of the object change according to the applied force.
You can specify the setting method with sw.
^p
sw content
--------------------------------------------------
0 Not reflected in the speed of the object
1 Reflect in the speed of the object
^p
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qblast


%index
qblast
Radially apply force from anywhere
%prm
xw,yw,power,near,far
xw (0.0): X coordinate (real number)
yw (0.0): Y coordinate (real number)
power (1.0): magnitude of force (real number)
near (1.0): closest distance (real number)
far (9999.0): Farthest distance (real number)
%inst
Radially apply force to all objects from anywhere.
A force that is inversely proportional to the distance is applied around (xw, yw).
power is the magnitude of the force applied at a distance of 1.0.
Positions closer to near are not inversely proportional and have the same force as near.
No force is applied if it is farther than far.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
qpush


%index
qgetversion
Get version information
%prm
var
var: Variable to get version information
%inst
Get OBAQ version information.
Version information is assigned as a character string to the variable specified by var.

%href


%index
qinner
Get if the coordinates are in the object
%prm
var,x,y,num
var: Variable to get the result
x (0.0): X coordinate (real number)
y (0.0): Y coordinate (real number)
num (-1): Object ID
%inst
Checks if the coordinates specified by (x, y) are inside the object shape.
Specify the ID of the object you want to check with num.
If num is negative or omitted, it spans all valid objects
Check if the coordinates are inside.
If it is inside, the object ID value is assigned to the variable specified by var.
If it is not inside, -1 is assigned to the variable specified by var.
The variable specified by var is automatically an integer type.
The coordinates specified by (x, y) are the internal coordinate values used by OBAQ, not the coordinates on the screen.
If you want to get the result from the coordinates on the screen, convert it to the internal coordinate value with the cnvaxis command etc.
Please note that you need to do it.
^
If execution fails, a non-zero value is assigned to the system variable stat.

%href
cnvaxis


%index
qgetptr
Get system pointer value
%prm
var,option
var: Variable to get the contents
option (0): option value
%inst
Gets the OBAQ system pointer value.
This system pointer value is used to access OBAQ's internal data from external applications, plugins, etc.
For normal use, no system pointer value is needed.
You can set the option value with the option parameter.
Currently, the option parameter has no non-zero value.

%href
hgobaq


