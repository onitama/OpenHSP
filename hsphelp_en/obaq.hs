;
;	HELP source file for HSP help manager
;	(Lines starting with ";" are treated as comments)
;

%type
Extended command
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
Extended Input/Output Control Command


%index
qreset
OBAQ Initialization
%inst
All information held by OBAQ is initialized.
When initialized by the qreset command, a wall (frame) enclosing the entire screen is automatically set.
If you want to change the size or position of the wall, reset it with the qborder command.
When qreset is executed, the window set as the drawing target of HSP is also set as the drawing target of OBAQ.
To change the window to be drawn, use the qview command.
%href
qview
qborder
qterm


%index
qterm
OBAQ Termination Process
%inst
Discards all information held by OBAQ.
It is called internally when the program ends, etc.
Normally, it is not necessary to call this.
%href
qreset


%index
qexec
OBAQ Frame Processing
%inst
Executes physics calculations for each frame.
This updates the coordinates of all objects.
In the main loop, it is essential to update the object's movement by using the qexec command.
Also, if you are using internal drawing, you need to update the screen with the qdraw command.
%href
qdraw


%index
qdraw
OBAQ Drawing Process
%prm
mode
mode(0) : Debug display mode setting (0=ON/1=OFF)
%inst
Performs drawing processing of objects by OBAQ.
Displays objects appropriately based on the material information set for each object.
You can set the ON/OFF of debug display with the mode setting.
During debug display, you can check the shape of the object by wireframe, the center position, and the group.
To change the display window, magnification, and display position offset, use the qview command.
Drawing by the qdraw command uses the Windows system API (GDI), so you need to clear the screen and update it with the redraw command.
When drawing with other systems (DirectX, etc.), update only with the qexec command,
and then get the display position of each object and draw it yourself.
(The qcnvaxis command and qgetaxis command are prepared for converting display coordinates.)

%href
qview
qexec
qcnvaxis
qgetaxis


%index
qview
View Settings
%prm
zoomx,zoomy,ofsx,ofsy
zoomx(4.0) : Zoom magnification in the X direction (real number)
zoomy(4.0) : Zoom magnification in the Y direction (real number)
ofsx(0.0)  : Display offset in the X direction (real number)
ofsy(0.0)  : Display offset in the Y direction (real number)
%inst
Sets the display settings when drawing with the qdraw command.
Set the display magnification (the magnification when reflecting OBAQ internal coordinates on the screen) with zoomx and zoomy.
You can set the display position offset with ofsx and ofsy.
Also, when qview is executed, the window set as the drawing target of HSP is reset as the drawing target.

%href
qdraw
qreset


%index
qsetreq
System Request Settings
%prm
reqid,val
reqid(0) : Request ID
val(0.0) : Setting value (real number)
%inst
Performs various system settings for OBAQ.
The following symbols can be specified by the reqid value.
^p
	Symbol name        	Content
------------------------------------------------------------------------
	REQ_PHYSICS_RATE	Number of physics calculations per frame
	REQ_MAXOBJ		Maximum number of objects (up to 512) (*)
	REQ_MAXLOG		Maximum number of collision logs (*)
	REQ_DEFAULT_WEIGHT	Default weight
	REQ_DEFAULT_MOMENT	Default moment
	REQ_DEFAULT_FRICTION	Default friction

(*) symbols are not reflected until the qreset command is executed.
^p

%href
qgetreq


%index
qgetreq
Get System Request
%prm
var,reqid
var      : Variable to get the content
reqid(0) : Request ID
%inst
Gets the system settings of OBAQ.
Reads the content into the variable specified by var.
The var variable is automatically set to a real number type.
The following symbols can be specified by the reqid value.
^p
	Symbol name            Initial value	Content
------------------------------------------------------------------------------
	REQ_PHYSICS_RATE	4	Number of physics calculations per frame
	REQ_MAXOBJ		512	Maximum number of objects (up to 512) (*)
	REQ_MAXLOG		256	Maximum number of collision logs (*)
	REQ_DEFAULT_WEIGHT	6.0	Default weight
	REQ_DEFAULT_MOMENT	1200.0	Default moment
	REQ_DEFAULT_FRICTION	1.0	Default friction

(*) symbols are not reflected until the qreset command is executed.
^p

%href
qsetreq


%index
qborder
Set Outer Wall
%prm
x1,y1,x2,y2
x1(-100) : Upper left X coordinate (real number)
y1(-100) : Upper left Y coordinate (real number)
x2(100)  : Lower right X coordinate (real number)
y2(100)  : Lower right Y coordinate (real number)
%inst
Resets the size of the standard outer wall.
Specify the coordinates (OBAQ internal coordinates) of the upper left and lower right, with the center of the screen as (0,0).
The outer wall is used to prevent objects from falling out of the screen when placing objects on the screen without any walls.
%href
qreset


%index
qgravity
Set Gravity
%prm
gx,gy
gx(0.0)   : Gravity in the X direction (real number)
gy(0.005) : Gravity in the Y direction (real number)
%inst
Sets the gravity of space.
The initial value is (0,0.005).

%href
qreset


%index
qcnvaxis
Convert X, Y Coordinates
%prm
var_x,var_y,x,y,opt
var_x  : Variable to get X
var_y  : Variable to get Y
x(0.0) : X coordinate of the conversion source (real number)
y(0.0) : Y coordinate of the conversion source (real number)
opt(0) : Conversion mode
%inst
Converts X, Y coordinates according to the specified method.
If you specify the coordinates of the conversion source as (x, y), the converted X, Y coordinate values are assigned to var_x, var_y, respectively.
You can select the following values for the conversion mode specified by opt.
^p
	Conversion mode        	Content
------------------------------------------------------------------------
	0                       Convert internal coordinates to screen coordinates
	1                       Convert screen coordinates to internal coordinates
^p
Conversion mode 0 (or when omitted) converts internal coordinates to screen coordinates displayed by the qdraw command. (var_x, var_y are set to integer type.)
Conversion mode 1 converts screen coordinates displayed by the qdraw command to internal coordinates. (var_x, var_y are set to real number type.)

%href
qgetaxis


%index
qgetaxis
Get Internal Coordinates
%prm
num,var_x,var_y,type
num(0) : Object ID
var_x  : Variable to get X
var_y  : Variable to get Y
type(0): Acquisition type
%inst
Reads the internal coordinates and display parameters of the OBAQ object into a variable.
var_x and var_y are automatically set as integer types.
The following contents can be specified by type.
^p
	Acquisition type        	Content
------------------------------------------------------------------------
	0                       Upper left coordinates of the object
	1                       Lower right coordinates of the object
	2			X, Y size of the object (*)

(*) The size (number of dots) on the display screen is returned
^p

%href
qcnvaxis


%index
qdel
Delete Object
%prm
num
num(0) : Object ID
%inst
Deletes the specified object.
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qaddpoly
qaddmodel


%index
qaddpoly
Add Polygon Object
%prm
var,shape,x,y,r,sx,sy,colsw,mygroup,exgroup,loggroup
var      : Variable to get the object ID
shape(3) : Number of vertices
x(0.0)   : X coordinate of placement (real number)
y(0.0)   : Y coordinate of placement (real number)
r(0.0)   : Placement angle (real number) (unit is radian)
sx(10.0) : X size of placement (real number)
sy(10.0) : Y size of placement (real number)
colsw(1) : Placement check mode
mygroup(1)  : Collision group to which you belong
exgroup(0)  : Collision exclusion group
loggroup(0) : Collision log creation group
%inst
Adds a polygon object to OBAQ.
An object with the number of vertices specified by shape is generated.
(The number of vertices of shape must be 3 or more.)
Specify the placement coordinates with (x, y). (Set the OBAQ internal coordinates.)
Specify the placement angle with r (+ direction is clockwise). The unit is radians.
Specify the size at the time of placement with (sx, sy). By default, a size of (10, 10) is used.
mygroup, exgroup, and loggroup are group values set for collision detection.
The group value is indicated by a value (1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768...) where one of the 32 bits is 1.
You can select the placement check mode with colsw.
^p
	Mode        Value	Content
------------------------------------------------------------------------
	colsw_none     0        No collision check at appearance
	colsw_active   1        Avoid contact with active rocks at appearance
	colsw_all      2        Avoid contact with rocks other than the reserve state at appearance
^p
When the object is added successfully, the object ID value is assigned to the variable specified by var. (The variable is automatically set to integer type.)
If the object is not added due to the registration limit, -1 is assigned to the variable specified by var.
If the object is not added for collision detection, -2 is assigned to the variable specified by var.

%href
qaddmodel


%index
qaddmodel
Add Freely Configured Object
%prm
var,shape_var,nvertex,x,y,r,sx,sy,colsw,mygroup,exgroup,loggroup
var      : Variable to get the object ID
shape_var: Array variable in which shape data is assigned
nvertex(-1) : Number of shape data
x(0.0)   : X coordinate of placement (real number)
y(0.0)   : Y coordinate of placement (real number)
r(0.0)   : Placement angle (real number) (unit is radian)
sx(10.0) : X size of placement (real number)
sy(10.0) : Y size of placement (real number)
colsw(1) : Placement check mode
mygroup(1)  : Collision group to which you belong
exgroup(0)  : Collision exclusion group
loggroup(0) : Collision log creation group
%inst
Adds an object with a freely configured shape to OBAQ.
General polygons can be generated by the qaddpoly command.
With the qaddmodel command, you can register more free shapes by individually specifying the vertices.
You need to specify an array variable in which shape data is assigned to shape_var.
Store the coordinates in the array variable in the order of x1, y1, x2, y2...
The coordinates must be specified counterclockwise and must be a convex body.
It is also possible to include multiple convex bodies in one object.
In that case, close the shape by making the last of the counterclockwise coordinates the same as the first coordinate, and then
specify the coordinates of a new, different shape.
^

nvertex specifies the number of data points (not the number of vertices).
If nvertex is a negative value or omitted, the number of elements set in the array will be used.
Either real or integer type can be used for the array variable that stores shape data.
Please note that you cannot handle data exceeding the specified number of vertices. For details, please refer to the OBAQ.DLL manual.
^p
	;	Add a model with a free shape
	;
	model=-1.0,-1.0, -1.0,2.0, 2.0,1.0, 1.0,-1.0
	qaddmodel i,model,-1, 30,64,0
^p
Specify the placement coordinates with (x, y). (Sets the internal coordinates of OBAQ.)
Specify the placement angle with r (the + direction is clockwise). The unit is radians.
Specify the size at the time of placement with (sx, sy). The default size of (10, 10) is used.
mygroup, exgroup, and loggroup are group values that are set for collision detection.
The group value is indicated by a value (1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768...) in which any of the 32 bits is 1.
You can select the placement check mode with colsw.
^p
	Mode        Value	Content
------------------------------------------------------------------------
	colsw_none     0        No collision check on appearance
	colsw_active   1        Avoid contact with active rocks on appearance
	colsw_all      2        Avoid contact with rocks other than in the reserve state on appearance
^p
When an object is successfully added, the object ID value will be assigned to the variable specified by var. (The variable automatically becomes an integer type.)
If the object cannot be added due to the registration limit, -1 is assigned to the variable specified by var.
If the object cannot be added due to collision detection, -2 is assigned to the variable specified by var.

%href
qaddpoly


%index
qtype
Set the type parameter
%prm
num,type,option
num(0)    : Object ID
type(0)   : Setting value
option(0) : Setting option
%inst
Sets the type parameter of the specified object.
You can specify the following contents for the type parameter.
^p
	Macro Name    Value	Content
-----------------------------------------------------------
	type_normal	0	Normal object
	type_inner	1	Object with inverted contact judgment
	type_bindX	0x40	X-axis fixed to the background
	type_bindY	0x80	Y-axis fixed to the background
	type_bindR	0x100	Rotation fixed
	type_bind	0x1c0	Completely fixed to the background (no physical behavior)
	type_autowipe	0x100000  Automatically delete when crossing the border range
^p
type_inner is used to create outer walls.
It is used for the default walls (shapes created with the qborder command), but be careful not to use it for any other purpose.
type_autowipe is a flag to automatically erase objects that go beyond the border area (frame) set by the qborder command.
^
You can specify the setting method of the type value with option.
^p
	option       	Content
------------------------------------------------------------------------
	0               Set the type value (set)
	1               Add the type value (or)
	2               Exclude the type value (not)
^p
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgettype
qborder


%index
qstat
Set the stat parameter
%prm
num,stat,sleepcount
num(0)        : Object ID
stat(0)       : Setting value
sleepcount(0) : Sleep count value
%inst
Sets the stat parameter and sleep count value of the specified object.
You can specify the following contents for the stat parameter.
^p
	Macro Name    Value	Content
-------------------------------------------------------------------
	stat_reserve 	0	Unused object
	stat_sleep 	1	Object waiting to appear
	stat_active 	2	Object appearing
^p
You can specify the setting method of the type value with option.
The sleep count value is a value that is referred to when stat_sleep is specified for stat.
Objects that are in the stat_sleep state will blink for the number of times of the sleep count value and appear. (If the sleep count value is 0, it will not appear and will remain in the standby state.)
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetstat


%index
qpos
Set position and angle parameters
%prm
num,x,y,angle
num(0)     : Object ID
x(0.0)     : X coordinate setting value (real number)
y(0.0)     : Y coordinate setting value (real number)
angle(0.0) : Angle setting value (real number) (radians)
%inst
Sets the position and angle parameters of the specified object.
(x, y) specifies the reference position (center coordinates) of the object.
angle specifies the object rotation angle (the + direction is clockwise). The unit is radians.
In OBAQ, even when moving an object, basically, do not directly change the coordinates of the object, but add force in the direction you want to move or use the qgetspeed command to set the acceleration.
If you forcibly change the coordinates, the physical behavior may not be correct.
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetpos


%index
qspeed
Set speed parameters
%prm
num,px,py,pr,option
num(0)     : Object ID
px(0.0) : X direction setting value (real number)
py(0.0) : Y direction setting value (real number)
pr(0.0) : Rotation speed parameter (real number)
option(0) : Setting option
%inst
Sets the speed parameters of the specified object.
(px, py) specifies the moving speed of the object in the X and Y directions.
pr specifies the speed for rotation.
You can specify the setting method with option.
^p
	option       	Content
------------------------------------------------------------------------
	0               Add speed values (add)
	1               Overwrite speed values x 2 (set)
	2               Overwrite speed values (set)
^p
If the execution fails, a value other than 0 will be assigned to the system variable stat.
If the option (setting option) value is 2, the specified value will be set.
If the option (setting option) value is 1, please note that twice the specified value will be set.

%href
qgetspeed


%index
qweight
Set parameters such as weight
%prm
num,weight,moment
num(0) : Object ID
weight(6.0)    : Weight value (real number)
moment(1200.0) : Moment value (real number)
%inst
Sets parameters such as weight, moment, and friction of the specified object.
The guideline for weight is: for a circle with radius r and standard specific gravity:
^p
	(r ^ 2) / 32.0
^p
Please think of moment as a parameter related to the ease of rotation.
^p
	Disk with radius r: weight * (r ^ 2) / 2.0
	Rectangle with sides of length a, b: weight * ((a ^ 2) + (b ^ 2)) / 12.0
^p
Please use values like the above as a guideline.
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetweight


%index
qdamper
Set parameters such as vibration absorption
%prm
num,damper,friction
num(0) : Object ID
damper(0.7)    : Vibration absorption parameter value (real number)
friction(1.0)  : Friction resistance value (real number)
%inst
Sets parameters such as vibration absorption, inertia, and gravity coefficient of the specified object.
damper (vibration absorption) indicates the force that suppresses shaking upon contact.
Be sure to set a value from 0.0 to 1.0. (Range checking is not performed)
friction (friction) is the coefficient of force that attenuates when objects come into contact with each other.
The value obtained by multiplying it by the friction of the object that comes into contact is referred to and calculated.
Be sure to set a value from 0.0 to 1.0. (Range checking is not performed)
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetdamper


%index
qinertia
Set parameters such as inertia
%prm
num,inertia,gravity
num(0) : Object ID
inertia(0.999) : Inertia parameter value (real number)
gravity(1.0)   : Object's gravity value (real number)
%inst
Sets parameters such as inertia and gravity coefficient of the specified object.
The closer inertia (inertia) is to 0.0, the more resistance occurs, resulting in a sticky movement.
Be sure to set a value from 0.0 to 1.0. (Range checking is not performed)
gravity (object's gravity) is reflected only on the object in a form that is multiplied by the gravity set in the space.
It is used to give different gravity only to specific objects.
It is effective for character control in games, etc., because it gives zero gravity or anti-gravity only to specific objects in a way that is impossible in reality.
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetinertia


%index
qgroup
Set group parameters
%prm
num,mygroup,exgroup,loggroup
num(0)      : Object ID
mygroup(1)  : Collision group value to which you belong
exgroup(0)  : Group value to exclude from collisions
loggroup(0) : Group value to create a collision log
%inst
Sets the group parameters of the specified object.
Group parameters are used for identification, such as when specifying a colliding partner object.
The group value is indicated by a value (1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768...) in which any of the 32 bits is 1.
^
If the execution fails, a value other than 0 will be assigned to the system variable stat.

%href
qgetgroup
qfind
qcollision


%index
qmat
Set material parameters
%prm
num,type,id,subid
num(0)   : Object ID
type(0)  : Material type value
id(0)    : Reference id value
subid(0) : Reference subid value
%inst
Sets the material parameters of the specified object.
Material parameters are reflected when drawing with the qdraw command.
You can select the display method according to the material type value.
^p
	Type          Value	Content
-------------------------------------------------------------------
	mat_none	0	Do nothing (hide)
	mat_spr		1	Sprite
	mat_spr2	2	Sprite (with size setting)
	mat_wire	3	Wireframe
	mat_wire2	4	Wireframe (with outline)
	mat_delay	0x1000	Draw later
^p
A rectangle is pasted in a form that covers the entire shape for sprites.
In the case of a wireframe, each vertex is connected by a line.
If you select mat_spr or mat_spr2, specify the window ID of the image to display as a sprite in id, and the divided image No. in subid.
The window ID and divided image No. are the same as the parameters used in the celput command, and can refer to images that have been divided using the celdiv command.
If you select mat_wire or mat_wire2, set the wire color code (24-bit value packed with RGB) in id and the outline color code in subid.
Objects with mat_delay (0x1000) added to the material type value will be drawn later.
This can be used for display using transparency, etc.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetmat
qmat2
qmat3


%index
qmat2
Set material detail parameters
%prm
num,offsetx,offsety,zoomx,zoomy
num(0)   : Object ID
offsetx(0) : X-direction display offset
offsety(0) : Y-direction display offset
zoomx(1.0) : X-direction display magnification (real number)
zoomy(1.0) : Y-direction display magnification (real number)
%inst
Sets the material detail parameters of the specified object.
Material detail parameters are reflected when drawing with the qdraw command.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetmat2
qmat
qmat3


%index
qmat3
Set material display parameters
%prm
num,gmode,rate
num(0)   : Object ID
gmode(0) : Copy mode
rate(256): Transparency rate
%inst
Sets the material display parameters of the specified object.
Material display parameters are reflected when drawing with the qdraw command.
gmode specifies the copy mode when drawing a sprite.
This is the same as the mode value specified by the HSP gmode command.
Also, rate (transparency rate) will have the same result as when specified with the gmode command.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

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
num(0)   : Object ID
user1(0) : User-defined data 1
user2(0) : User-defined data 2
user3(0) : User-defined data 3
%inst
Sets the user-defined data for the specified object.
User-defined data is a value that can be stored for each object, and the user is free to decide what to use it for.
User-defined data 1 to 3 can store integer values.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetuser
quser2


%index
quser2
Set user-defined data 2
%prm
num,user4,user5,user6
num(0)     : Object ID
user4(0.0) : User-defined data 4 (real number)
user5(0.0) : User-defined data 5 (real number)
user6(0.0) : User-defined data 6 (real number)
%inst
Sets the user-defined data for the specified object.
User-defined data is a value that can be stored for each object, and the user is free to decide what to use it for.
User-defined data 4 to 6 can store real number values.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qgetuser2
quser


%index
qfind
Object search
%prm
group,stat
group(-1) : Group value to search for
stat(0)   : stat value specification
%inst
Searches registered objects according to arbitrary conditions.
First, you must specify the conditions with the qfind command, and then receive the results with the qnext command.
Specify the collision group value to search for with group.
If the group is omitted or -1, all groups are targeted.
(However, objects with a group value of 0 are excluded)
If you want to specify multiple groups, specify the value that is the sum of each group value.
By specifying stat, you can search for only specific states.
If stat is 0, all states are targeted.
If stat_sleep(1) or stat_active(2) is specified for stat, only objects with the same value are targeted.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qnext


%index
qnext
Get object search result
%prm
var
var : Variable to get the search result
%inst
Gets the result of the search by the qfind command.
First, you must specify the conditions with the qfind command, and then receive the results with the qnext command.
The object ID value is assigned to the variable specified by var. (The variable automatically becomes an integer type.)
You can retrieve search results each time you execute the qnext command.
If there are no more search results, -1 is assigned.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind


%index
qcollision
Start collision acquisition
%prm
num, target
num(0)     : Object ID
target(-1) : Object ID of the object to acquire collisions for
%inst
Starts acquiring information about collisions between objects.
First, you must specify the object with the qcollision command, and then receive the results with the qgetcol command.
target specifies the object ID of the object for which to get the collision.
If target is -1 or omitted, all information related to the object specified by num is acquired.
When performing collision acquisition, you must always set the collision group.
Note that only objects whose parameters specified by num are in the collision log creation group will be acquired.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

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
var_id : Variable to get the object ID
var_x  : Variable to get the X coordinate
var_y  : Variable to get the Y coordinate
%inst
Gets the result of the search by the qcollision command.
First, you must specify the conditions with the qcollision command, and then receive the results with the qgetcol command.
Furthermore, you can acquire additional information with the qgetcol2 command and qgetcol3 command as needed.
The ID value of the object where a collision was detected is assigned to the variable specified by var_id. (The variable automatically becomes an integer type.)
The X coordinate where a collision was detected is assigned to the variable specified by var_x. (The variable automatically becomes a real number type.)
The Y coordinate where a collision was detected is assigned to the variable specified by var_y. (The variable automatically becomes a real number type.)
You can retrieve search results each time you execute the qgetcol command.
If there are no more search results, -1 is assigned as the object ID.
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol2
qgetcol3


%index
qgetcol2
Collision detail information acquisition
%prm
var_depth,var_nx,var_ny
var_depth : Variable to get the depth of the collision
var_nx    : Variable to get the X normal of the collision
var_ny    : Variable to get the Y normal of the collision
%inst
Gets the result of the search by the qcollision command.
Be sure to acquire information after receiving the results with the qgetcol command.
The depth of the collision (distance of intrusion into the contact surface) is assigned to the variable specified by var_depth. (The variable automatically becomes a real number type.)
The normals (x, y) at the time of collision are assigned to the variables specified by var_nx, var_ny. (The variables automatically become real number types.)
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol
qgetcol3


%index
qgetcol3
Collision detail information acquisition 2
%prm
var_bound,var_slide
var_bound : Variable to get the vertical velocity
var_slide : Variable to get the horizontal velocity
%inst
Gets the result of the search by the qcollision command.
Be sure to acquire information after receiving the results with the qgetcol command.
The velocity perpendicular (bouncing direction) to the contact surface (the positive side is the intrusion direction) is assigned to the variable specified by var_bound. (The variable automatically becomes a real number type.)
The velocity horizontal (dragging direction) to the contact surface is assigned to the variable specified by var_slide. (The variable automatically becomes a real number type.)
^
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qfind
qgetcol
qgetcol2


%index
qgettype
Get the type parameter
%prm
num,var_type
num(0)    : Object ID
var_type  : Variable to get the type parameter
%inst
Gets the parameters of the specified object into a variable.
The type parameter value is assigned to the variable specified by var_type. (The variable automatically becomes an integer type.)
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qtype


%index
qgetstat
Get the stat parameter
%prm
num,var_stat,var_sleepcount
num(0)        : Object ID
var_stat      : Variable to get the stat setting value
var_sleepcount: Variable to get the sleep count value
%inst
Gets the parameters of the specified object into a variable.
The stat parameter value is assigned to the variable specified by var_stat. (The variable automatically becomes an integer type.)
Note that the sleep count value is not the value set by qstat, but the value corresponding to the internal number of physical calculation executions.
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qstat


%index
qgetpos
Get position and angle parameters
%prm
num,var_x,var_y,var_angle
num(0)     : Object ID
var_x      : Variable to get the X coordinate setting value
var_y      : Variable to get the Y coordinate setting value
var_angle  : Variable to get the angle setting value
%inst
Gets the parameters of the specified object into a variable.
The X coordinate setting value is assigned to the variable specified by var_x. (The variable automatically becomes a real number type.)
The Y coordinate setting value is assigned to the variable specified by var_y. (The variable automatically becomes a real number type.)
The angle setting value is assigned to the variable specified by var_angle. (The variable automatically becomes a real number type.)
Note that the acquired X and Y coordinates are not the coordinates on the screen, but the internal coordinate values used by OBAQ.
To convert internal coordinate values to screen coordinates, use the qcnvaxis command.
If the execution fails, a non-zero value is assigned to the system variable stat.

%href
qpos
qcnvaxis


%index
qgetspeed
Get the speed parameters
%prm
num,var_px,var_py,var_pr
num(0) : Object ID
var_px : Variable to get the X-direction speed value
var_py : Variable to get the Y-direction speed value
var_pr : Variable to get the rotational speed value
%inst
Gets the parameters of the specified object into a variable.
The X-direction speed value is assigned to the variable specified by var_px. (The variable automatically becomes a real number type.)
The Y-direction speed value is assigned to the variable specified by var_py. (The variable automatically becomes a real number type.)
The rotational speed value is assigned to the variable specified by var_pr. (The variable automatically becomes a real number type.)
If the execution fails, a non-zero value is assigned to the system variable stat.
%href
qspeed


%index
qgetweight
Get weight and other parameters.
%prm
num,var_weight,var_moment
num(0)     : Object ID
var_weight : Variable to store the weight value
var_moment : Variable to store the moment value
%inst
Retrieves the parameters of the specified object into variables.
The weight value is assigned to the variable specified by var_weight. (The variable is automatically converted to a real number type.)
The moment value is assigned to the variable specified by var_moment. (The variable is automatically converted to a real number type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qweight


%index
qgetdamper
Get vibration absorption and other parameters.
%prm
num,var_damper,var_friction
num(0) : Object ID
var_damper   : Variable to store the vibration absorption parameter value
var_friction : Variable to store the frictional resistance value
%inst
Retrieves the parameters of the specified object into variables.
The vibration absorption parameter value is assigned to the variable specified by var_damper. (The variable is automatically converted to a real number type.)
The frictional resistance value is assigned to the variable specified by var_friction. (The variable is automatically converted to a real number type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qdamper


%index
qgetinertia
Get inertia and other parameters.
%prm
num,var_inertia,var_gravity
num(0) : Object ID
var_inertia : Variable to store the inertia parameter value
var_gravity : Variable to store the object's gravity value
%inst
Retrieves the parameters of the specified object into variables.
The inertia parameter value is assigned to the variable specified by var_inertia. (The variable is automatically converted to a real number type.)
The object's gravity value is assigned to the variable specified by var_gravity. (The variable is automatically converted to a real number type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qinertia


%index
qgetgroup
Get group parameters.
%prm
num,var_mygroup,var_exgroup,var_loggroup
num(0)      : Object ID
var_mygroup : Variable to store the collision group value to which the object belongs
var_exgroup : Variable to store the group value to exclude from collisions
var_loggroup: Variable to store the group value for creating collision logs
%inst
Retrieves the parameters of the specified object into variables.
The collision group value to which the object belongs is assigned to the variable specified by var_mygroup. (The variable is automatically converted to an integer type.)
The group value to exclude from collisions is assigned to the variable specified by var_exgroup. (The variable is automatically converted to an integer type.)
The group value for creating collision logs is assigned to the variable specified by var_loggroup. (The variable is automatically converted to an integer type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qgroup
qfind
qcollision


%index
qgetmat
Get material parameters.
%prm
num,var_type,var_id,var_subid
num(0)    : Object ID
var_type  : Variable to store the material type value
var_id    : Variable to store the reference ID value
var_subid : Variable to store the reference sub ID value
%inst
Retrieves the parameters of the specified object into variables.
The material type value is assigned to the variable specified by var_type. (The variable is automatically converted to an integer type.)
The reference ID value is assigned to the variable specified by var_id. (The variable is automatically converted to an integer type.)
The reference sub ID value is assigned to the variable specified by var_subid. (The variable is automatically converted to an integer type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qmat
qgetmat2
qgetmat3


%index
qgetmat2
Get detailed material parameters.
%prm
num,var_offsetx,var_offsety,var_zoomx,var_zoomy
num(0)   : Object ID
var_offsetx : Variable to store the X-direction display offset value
var_offsety : Variable to store the Y-direction display offset value
var_zoomx   : Variable to store the X-direction display magnification value
var_zoomy   : Variable to store the Y-direction display magnification value
%inst
Retrieves the parameters of the specified object into variables.
The X-direction display offset value is assigned to the variable specified by var_offsetx. (The variable is automatically converted to an integer type.)
Retrieves the parameters of the specified object into variables.
The Y-direction display offset value is assigned to the variable specified by var_offsety. (The variable is automatically converted to an integer type.)
Retrieves the parameters of the specified object into variables.
The X-direction display magnification value is assigned to the variable specified by var_zoomx. (The variable is automatically converted to a real number type.)
Retrieves the parameters of the specified object into variables.
The Y-direction display magnification value is assigned to the variable specified by var_zoomy. (The variable is automatically converted to a real number type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qmat2
qgetmat
qgetmat3


%index
qgetmat3
Get material display parameters.
%prm
num,var_gmode,var_rate
num(0)   : Object ID
var_gmode : Variable to store the copy mode value
var_rate  : Variable to store the transparency rate value
%inst
Retrieves the parameters of the specified object into variables.
The copy mode value is assigned to the variable specified by var_gmode. (The variable is automatically converted to an integer type.)
The transparency rate value is assigned to the variable specified by var_rate. (The variable is automatically converted to an integer type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qmat3
qgetmat
qgetmat2


%index
qgetuser
Get user-defined data.
%prm
num,var_user1,var_user2,var_user3
num(0)   : Object ID
var_user1 : Variable to store the value of user-defined data 1
var_user2 : Variable to store the value of user-defined data 2
var_user3 : Variable to store the value of user-defined data 3
%inst
Retrieves the parameters of the specified object into variables.
The value of user-defined data 1 is assigned to the variable specified by var_user1. (The variable is automatically converted to an integer type.)
The value of user-defined data 2 is assigned to the variable specified by var_user2. (The variable is automatically converted to an integer type.)
The value of user-defined data 3 is assigned to the variable specified by var_user3. (The variable is automatically converted to an integer type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
quser
qgetuser2


%index
qgetuser2
Get user-defined data 2.
%prm
num,var_user4,var_user5,var_user6
num(0)     : Object ID
var_user4 : Variable to store the value of user-defined data 4
var_user5 : Variable to store the value of user-defined data 5
var_user6 : Variable to store the value of user-defined data 6
%inst
Retrieves the parameters of the specified object into variables.
The value of user-defined data 4 is assigned to the variable specified by var_user4. (The variable is automatically converted to a real number type.)
The value of user-defined data 5 is assigned to the variable specified by var_user5. (The variable is automatically converted to a real number type.)
The value of user-defined data 6 is assigned to the variable specified by var_user6. (The variable is automatically converted to a real number type.)
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
quser2
qgetuser


%index
qpush
Apply force to an arbitrary location.
%prm
num,xw,yw,ax,ay,sw
num(0)     : Object ID
xw(0.0) : X coordinate (real number)
yw(0.0) : Y coordinate (real number)
ax(0.0) : Force in the X direction (real number)
ay(0.0) : Force in the Y direction (real number)
sw(1)   : Setting option
%inst
Applies a force of strength (ax, ay) from the position (xw, yw) to the specified object.
The object's attitude and speed will change according to the applied force.
sw can be used to specify the setting method.
^p
	sw     	Contents
--------------------------------------------------
	0       Do not reflect in the object's speed
	1       Reflect in the object's speed
^p
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qblast


%index
qblast
Apply force radially from an arbitrary location.
%prm
xw,yw,power,near,far
xw(0.0)    : X coordinate (real number)
yw(0.0)    : Y coordinate (real number)
power(1.0) : Force magnitude (real number)
near(1.0)  : Closest distance (real number)
far(9999.0): Furthest distance (real number)
%inst
Applies force radially from an arbitrary location to all objects.
A force that is inversely proportional to the distance is applied with (xw, yw) as the center.
power is the magnitude of the force applied at a distance of 1.0.
The force at positions closer than near does not decrease proportionally, but remains the same as the force at near.
No force is applied if the object is further than far.
^
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
qpush


%index
qgetversion
Get version information.
%prm
var
var : Variable to store version information.
%inst
Gets OBAQ version information.
The version information is assigned as a string to the variable specified by var.

%href


%index
qinner
Get whether a coordinate is inside an object.
%prm
var,x,y,num
var     : Variable to store the result
x(0.0)  : X coordinate (real number)
y(0.0)  : Y coordinate (real number)
num(-1) : Object ID
%inst
Checks whether the coordinate specified by (x, y) is inside the object shape.
Specify the ID of the object you want to check with num.
If num is a negative value or is omitted, it checks all valid objects to see
if the coordinate is inside.
If it is inside, the object ID value is assigned to the variable specified by var.
If it is not inside, -1 is assigned to the variable specified by var.
The variable specified by var is automatically converted to an integer type.
Note that the coordinates specified by (x, y) are not the coordinates on the screen, but the internal coordinate values used by OBAQ.
To get the result from the coordinates on the screen, you need to convert them to internal coordinate values using the cnvaxis command, etc.
^
If the execution fails, a value other than 0 is assigned to the system variable stat.

%href
cnvaxis


%index
qgetptr
Get system pointer value
%prm
var,option
var       : Variable to store the content
option(0) : Option value
%inst
Gets the OBAQ system pointer value.
This system pointer value is used to access OBAQ's internal data from external applications or plugins, etc.
In normal use, the system pointer value is not necessary.
You can set the option value with the option parameter.
Currently, option parameters other than 0 are invalid.
Please provide more context. "hgobaq" alone doesn't translate directly into English. Without knowing what it *means* in the context of HSP3, I can only offer guesses. Here are some possibilities, depending on the context:

*   **If it's a variable name or a function name:**  There's no direct translation. It remains "hgobaq". In programming, names are often arbitrary.  I would need to see more code to infer its *purpose* and describe that in English (e.g., "This variable stores the number of objects," rather than translating the variable's name).
*   **If it's an error message or part of an error message:** The translation depends on what the error *means*.  Again, context is crucial.  I would need the full error message or surrounding code to understand the error.
*   **If it's a misspelling or abbreviation:** I would need to guess the intended word and translate *that*.

**Therefore, please provide the surrounding code or sentence(s) where "hgobaq" appears.  This will allow me to provide an accurate and meaningful translation.**
