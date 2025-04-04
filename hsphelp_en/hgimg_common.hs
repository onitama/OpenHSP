;
; HSP help manager HELP source file
; (Lines starting with ";" are treated as comments)
;
%type
Extended command
%ver
3.7
%note
Include either hgimg3.as/hgimg4.as/hgimg4dx.as.
%author
onitama
%dll
HGIMG3/HGIMG4 common
%date
2022/06/07
%author
onitama
%url
http://www.onionsoft.net/
%port
Win
%portinfo
HGIMG4DX operates in a DirectX9 environment, and HGIMG4 operates in an OpenGL3.1 environment.


%index
fvseti
Set vector from integer value
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Integer value
%inst
Assigns the integer values specified by (x,y,z) as a vector to the FV value.
%href
fvset
fvadd
fvsub
fvmul
fvdiv
fvdir
fvmin
fvmax
fvouter
fvinner
fvface
fvunit


%index
fvset
Set vector
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Calculated value (real number)
%inst
Assigns the decimal values (X,Y,Z) specified by (x,y,z) as a vector to the FV value.
%href
fvseti
fvadd
fvsub
fvmul
fvdiv
fvdir
fvmin
fvmax
fvouter
fvinner
fvface
fvunit


%index
fvadd
Vector addition
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Calculated value (real number)
%inst
Adds the decimal values (X,Y,Z) specified by (x,y,z) to the FV value.
%href
fvseti
fvset
fvsub
fvmul
fvdiv
fvmin
fvmax


%index
fvsub
Vector subtraction
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Calculated value (real number)
%inst
Subtracts the decimal values (X,Y,Z) specified by (x,y,z) from the FV value.
%href
fvseti
fvset
fvadd
fvmul
fvdiv
fvmin
fvmax


%index
fvmul
Vector multiplication
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Calculated value (real number)
%inst
Multiplies the FV value by the decimal values (X,Y,Z) specified by (x,y,z) in parallel.
%href
fvseti
fvset
fvadd
fvsub
fvdiv
fvmin
fvmax


%index
fvdiv
Vector division
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Calculated value (real number)
%inst
Divides the FV value by the decimal values (X,Y,Z) specified by (x,y,z) in parallel.
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvmin
fvmax


%index
fvdir
Get a calculated vector
%group
Extended screen control commands
%prm
fv,x,y,z,type
fv      = Variable name to which the FV value is assigned
(x,y,z) = Vector value (real number)
type(0) = Calculation type of the acquired vector
%inst
Based on the FV value (X,Y,Z) stored in the variable specified by fv, the result of performing a calculation on the vector (X,Y,Z) specified by real numbers is assigned as an FV value to the variable fv.
The values to specify in the type parameter are as follows:
^p
    Macro name              Value    Content
    ---------------------------------------------------------------------------------------------
	FVDIR_ROTORDER_ZYX     0    Rotates the vector value based on the FV value (angle) (Z->Y->X)
	FVDIR_ROTORDER_XYZ     1    Rotates the vector value based on the FV value (angle) (X->Y->Z)
	FVDIR_ROTORDER_YXZ     2    Rotates the vector value based on the FV value (angle) (Y->X->Z)
	FVDIR_HALFVECTOR       4    Finds the half vector of the FV value (vector) and the vector value
	FVDIR_REFRECTION       5    Finds the reflection vector based on the FV value (incident vector) and the vector value (normal)
	FVDIR_MIRROR           6    Finds the mirror vector based on the FV value (incident vector) and the vector value (normal)
^p
If the calculation type is 0 to 2, the rotation is applied to the vector value specified by the parameter using the (X,Y,Z) values preset in the FV value as angle values (radians). The rotation order can be changed depending on the calculation type. If the calculation type is omitted or 0 is specified, the normal rotation order (Z->Y->X) will be used.
If the calculation type is 4 or higher, the calculation is applied based on the vector value specified by the parameter, using the (X,Y,Z) values preset in the FV value as the original vector. In this case, the preset vector value is treated as normalized.
%href
fvset
fvdir
fvface


%index
fvface
Get angle from coordinates
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = X,Y,Z coordinate values (real number)
%inst
Calculates the X,Y,Z rotation angles for viewing the specified X,Y,Z coordinates in a straight line from the X,Y,Z coordinates based on the vector (FV value) stored in the variable specified by fv, and assigns them to the variable fv.
%href
fvset
fvdir


%index
fvmin
Truncate vector elements with minimum value
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Comparison value (real number)
%inst
Truncates each element of the FV value stored in the variable specified by fv so that the value specified by the parameter (X,Y,Z) becomes the minimum value.
Used to apply the minimum value to each element of the FV value.
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvdiv
fvmax


%index
fvmax
Truncate vector elements with maximum value
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Comparison value (real number)
%inst
Truncates each element of the FV value stored in the variable specified by fv so that the value specified by the parameter (X,Y,Z) becomes the maximum value.
Used to apply the maximum value to each element of the FV value.
%href
fvseti
fvset
fvadd
fvsub
fvmul
fvdiv
fvmin


%index
fvouter
Vector cross product
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Vector value to be calculated (real number)
%inst
Calculates and assigns the cross product of the FV value stored in the variable specified by fv and the vector specified by decimal values (X,Y,Z).
%href
fvseti
fvset
fvinner


%index
fvinner
Vector dot product
%group
Extended screen control commands
%prm
fv,x,y,z
fv      = Variable name to which the FV value is assigned
(x,y,z) = Vector value to be calculated (real number)
%inst
Calculates the dot product of the FV value stored in the variable specified by fv and the vector specified by decimal values (X,Y,Z), and assigns it to fv.0.
%href
fvseti
fvset
fvouter


%index
fvunit
Vector normalization
%group
Extended screen control commands
%prm
fv
fv      = Variable name to which the FV value is assigned
%inst
Normalizes the vector (FV value) stored in the variable specified by fv.
%href
fvseti
fvset


%index
fsin
Calculate sine
%group
Extended screen control commands
%prm
fval,frot
fval    = Variable name to which the real number is assigned
frot    = Rotation angle (radians)
%inst
Assigns the sine value of the angle specified by frot to the variable specified by fval.
The unit of angle is radians (2π=360 degrees).
%href
fcos
fsqr
froti



%index
fcos
Calculate cosine
%group
Extended screen control commands
%prm
fval,frot
fval    = Variable name to which the real number is assigned
frot    = Rotation angle (radians)
%inst
Assigns the cosine value of the angle specified by frot to the variable specified by fval.
The unit of angle is radians (2π=360 degrees).
%href
fsin
fsqr
froti



%index
fsqr
Calculate square root
%group
Extended screen control commands
%prm
fval,fprm
fval    = Variable name to which the real number is assigned
fprm    = Value used for calculation (real number)
%inst
Assigns the square root of the value specified by fprm to the variable specified by fval.
%href
fsin
fcos
froti



%index
froti
Convert integer angle to decimal value
%group
Extended screen control commands
%prm
fval,prm
fval    = Variable name to which the real number is assigned
prm(0)  = Angle value of 0-1023 (integer)
%inst
Converts the integer value (0 to 1023) specified by prm as a value indicating an angle, converts it to a radian angle value of -π to +π, and assigns it to fval.
%href
fsin
fcos
fsqr



%index
str2fv
Convert string to vector
%group
Extended screen control commands
%prm
fv,"x,y,z"
fv      = Variable name to which the FV value is assigned
"x,y,z" = String containing real values separated by ","
%inst
Reads the string information specified by "x,y,z" as X,Y,Z decimal values separated by ",", and stores it in the variable specified by fv.
If any item cannot be correctly recognized as a numerical value (invalid) string, 0.0 will be assigned to that and subsequent items.
%href
fv2str
str2f
f2str
f2i


%index
fv2str
Convert vector to string
%group
Extended screen control commands
%prm
fv
fv      = Variable name to which the FV value is assigned
%inst
Converts the vector (FV value) stored in the variable specified by fv into a string and returns the result to the system variable refstr.
%href
str2fv
str2f
f2str
f2i


%index
str2f
Convert string to decimal value
%group
Extended screen control commands
%prm
fval,"fval"
fval    = Variable name to which the real number is assigned
"fval"  = String containing real value
%inst
Reads the string information specified by "fval" as a decimal value and stores it in the variable specified by fval.
%href
fv2str
str2fv
f2str
f2i


%index
f2str
Convert decimal value to string
%group
Extended screen control commands
%prm
sval,fval
sval    = Variable name to which the string is assigned
fval    = Real value of conversion source
%inst
Converts the decimal value specified by fval into a string, and returns the result to the string type variable specified by val.
%href
fv2str
str2fv
str2f
f2i


%index
delobj
Delete object
%group
Extended screen control commands
%prm
ObjID
ObjID  : Object ID
%inst
Deletes the specified object.
%href
regobj


%index
setpos
Set pos group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)

%inst
Sets the parameters of the object.
Sets the value specified by (x,y,z) to the pos group (display coordinates).
(x,y,z) can be specified as a real number or an integer value.

%href
setang
setangr
setscale
setdir
setefx
setwork


%index
setang
Set ang group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)

%inst
Sets the parameters of the object.
Sets the value specified by (x,y,z) to the ang group (display angle).
(x,y,z) can be specified as a real number or an integer value.
The unit of angle is radians.
(The rotation order is X->Y->Z. setangy and setangz commands are available for rotating in other orders.)
The setangr command for setting the angle with an integer is also available.

%href
setpos
setangr
setscale
setdir
setefx
setwork


%index
setangr
Set ang group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)

%inst
Sets the parameters of the object.
Sets the value specified by (x,y,z) to the ang group (display angle).
(x,y,z) can be specified as a real number or an integer value.
The unit of angle is an integer with a value of 0 to 255 per revolution.
The setang command for setting the angle in radians is also available.

%href
setpos
setang
setscale
setdir
setefx
setwork


%index
setscale
Set scale group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to set (default=0)

%inst
Sets parameters for the object.
Sets the values specified by (x,y,z) to the scale group (display magnification).
(x,y,z) can be either real or integer values.

%href
setpos
setang
setangr
setdir
setefx
setwork


%index
setdir
Set dir group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to set (default=0)

%inst
Sets parameters for the object.
Sets the values specified by (x,y,z) to the dir group (movement vector).
(x,y,z) can be either real or integer values.
The values registered in the movement vector are referenced during the object's automatic movement mode (OBJ_MOVE).

%href
setpos
setang
setangr
setscale
setefx
setwork


%index
setwork
Set work group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to set (default=0)

%inst
Sets parameters for the object.
Sets the values specified by (x,y,z) to the work group (work value).
(x,y,z) can be either real or integer values.

%href
setpos
setang
setangr
setscale
setdir
setefx
setwork2


%index
addpos
Add to pos group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the pos group (display coordinates).
(x,y,z) can be either real or integer values.

%href
addang
addangr
addscale
adddir
addefx
addwork


%index
addang
Add to ang group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the ang group (display angle).
(x,y,z) can be either real or integer values.
The unit of angle is radians.
The setangr command is also available for setting angles using integers.

%href
addpos
addangr
addscale
adddir
addefx
addwork


%index
addangr
Add to ang group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the ang group (display angle).
(x,y,z) can be either real or integer values.
The unit of angle is an integer value that cycles from 0 to 255.
The setang command is also available for setting angles in radians.

%href
addpos
addang
addscale
adddir
addefx
addwork


%index
addscale
Add to scale group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the scale group (display magnification).
(x,y,z) can be either real or integer values.

%href
addpos
addang
addangr
adddir
addefx
addwork


%index
adddir
Add to dir group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the dir group (movement vector).
(x,y,z) can be either real or integer values.

%href
addpos
addang
addangr
addscale
addefx
addwork


%index
addwork
Add to work group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Values to add (default=0)

%inst
Sets parameters for the object.
Adds the values specified by (x,y,z) to the work group (work value).
(x,y,z) can be either real or integer values.

%href
addpos
addang
addangr
addscale
adddir
addefx


%index
getpos
Get pos group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the pos group (display coordinates) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as real-type variables.
By adding "i" to the end of the command, you can retrieve the value as an integer.

%href
getposi
getang
getangr
getscale
getdir
getefx
getwork


%index
getscale
Get scale group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the scale group (display magnification) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as real-type variables.
By adding "i" to the end of the command, you can retrieve the value as an integer.

%href
getscalei
getpos
getang
getangr
getdir
getefx
getwork


%index
getdir
Get dir group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the dir group (movement vector) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as real-type variables.
By adding "i" to the end of the command, you can retrieve the value as an integer.

%href
getdiri
getpos
getang
getangr
getscale
getefx
getwork


%index
getwork
Get work group information
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the work group (work value) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as real-type variables.
By adding "i" to the end of the command, you can retrieve the value as an integer.

%href
getworki
getpos
getang
getangr
getscale
getdir
getefx


%index
getposi
Get pos group information as integers
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the pos group (display coordinates) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as integer-type variables.

%href
getpos
getangi
getangri
getscalei
getdiri
getefxi
getworki


%index
getscalei
Get scale group information as integers
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the scale group (display magnification) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as integer-type variables.

%href
getscale
getposi
getangi
getangri
getdiri
getefxi
getworki


%index
getdiri
Get dir group information as integers
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the dir group (movement vector) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as integer-type variables.

%href
getdir
getposi
getangi
getangri
getscalei
getefxi
getworki


%index
getworki
Get work group information as integers
%group
Extended screen control command
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to store the values

%inst
Gets parameters for the object.
The contents of the work group (work value) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as integer-type variables.

%href
getwork
getposi
getangi
getangri
getscalei
getdiri
getefxi


%index
selpos
Set the movement coordinates to MOC information
%group
Extended screen control command
%prm
id
id     : Object ID
%inst
Sets the MOC group to be targeted by the MOC setting command to pos (coordinates)
id is the object ID.
%href
selmoc
selang
selscale
seldir
selefx
selcam
selcpos
selcang
selcint


%index
selang
Set the rotation angle to MOC information
%group
Extended screen control command
%prm
id
id     : Object ID
%inst
Sets the MOC group to be targeted by the MOC setting command to ang (rotation angle)
id is the object ID.
%href
selmoc
selpos
selscale
seldir
selefx
selcam
selcpos
selcang
selcint


%index
selscale
Set the scale to MOC information
%group
Extended screen control command
%prm
id
id     : Object ID
%inst
Sets the MOC group to be targeted by the MOC setting command to scale (scale)
id is the object ID.
%href
selmoc
selpos
selang
selefx
seldir
selcam
selcpos
selcang
selcint


%index
seldir
Set the amount of movement to MOC information
%group
Extended screen control command
%prm
id
id     : Object ID
%inst
Sets the MOC group to be targeted by the MOC setting command to dir (amount of movement)
id is the object ID.
%href
selmoc
selpos
selang
selscale
selefx
selcam
selcpos
selcang
selcint


%index
selwork
Set object work to MOC information
%group
Extended screen control command
%prm
id
id     : Object ID
%inst
Sets the MOC group to be targeted by the MOC setting command to work (work)
id is the object ID.
%href
selmoc
selpos
selang
selscale
selefx
selcam
selcpos
selcang
selcint


%index
objset3
Set MOC information
%group
Extended screen control command
%prm
x,y,z
x   : Value to set
y   : Value 2 to set
z   : Value 3 to set

%inst
Sets MOC information.
The first three parameters starting from offset number 0 are targeted.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objsetf3
Set MOC information
%group
Extended screen control command
%prm
fx,fy,fz
fx  : Value to set (real value)
fy  : Value 2 to set (real value)
fz  : Value 3 to set (real value)

%inst
Sets MOC information.
The first three parameters starting from offset number 0 are targeted.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3
Add to MOC information
%group
Extended screen control command
%prm
x,y,z
x   : Value to add
y   : Value 2 to add
z   : Value 3 to add

%inst
Adds x, y, and z to the values set in the MOC information.
The first three parameters starting from offset number 0 are targeted.
%href
objset3
objadd3r
objset3r
objsetf3
objaddf3


%index
objaddf3
Add MOC Information
%group
Extended Screen Control Commands
%prm
fx,fy,fz
fx  : Value to add (real number)
fy  : Value to add 2 (real number)
fz  : Value to add 3 (real number)

%inst
Adds fx, fy, and fz to the values set in MOC information.
The parameters from offset number 0 are targeted.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3r
Add MOC Information
%group
Extended Screen Control Commands
%prm
ofs,fx,fy,fz
ofs : MOC offset number
fx  : Value to add (integer angle value)
fy  : Value to add 2 (integer angle value)
fz  : Value to add 3 (integer angle value)
%inst
Adds fx, fy, and fz to the values set in MOC information.
However, it adds the parameters converted from integer values (1 rotation in 256) to radians.
Please note that normal values will not be obtained except for parameters that specify angles.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objset3r
Set MOC Information
%group
Extended Screen Control Commands
%prm
x,y,z
x   : Value to set
y   : Value to set 2
z   : Value to set 3

%inst
Sets angle information in MOC information.
The parameters from offset number 0 are targeted.
Writes parameters by converting integer values (1 rotation in 256) to radians.
Please note that normal values will not be obtained except for parameters that specify angles.
%href
objset3
objadd3
objset3r
objsetf3
objadd3f


%index
setobjmode
Set Object Mode
%group
Extended Screen Control Commands
%prm
ObjID,mode,sw
ObjID    : Object ID
mode     : Mode value
sw       : Setting switch
%inst
Changes the mode of the specified object.
The object's mode is pre-set according to the model type, so it is usually not necessary to set it specifically.
The mode can be selected from the following:
^p
	Label             |        Content
	--------------------------------------------------------------
	OBJ_HIDE             Hide (remove from the screen)
	OBJ_CLIP             Enable 3D clipping (HGIMG4 only)
	OBJ_XFRONT           Front-facing attribute (always faces the screen)
	OBJ_WIRE             Draw in wireframe (HGIMG4 only)
	OBJ_MOVE             Automatic movement (refers to XYZ movement amounts)
	OBJ_FLIP             Flip in border area
	OBJ_BORDER           Enable border area
	OBJ_2D               2D sprite
	OBJ_TIMER            Enable timer (HGIMG4 only)
	OBJ_LATE             Drawn later (for semi-transparent objects)

	OBJ_FIRST            Always drawn first (HGIMG3 only)
	OBJ_SORT             Automatically drawn from the back (for 3D objects) (HGIMG3 only)
	OBJ_LOOKAT           Always faces a specific object (HGIMG3 only)
	OBJ_LAND             Always keeps the Y coordinate constant (HGIMG3 only)
	OBJ_GROUND           Recognized as ground (HGIMG3 only)
	OBJ_STAND            Place on the ground (HGIMG3 only)
	OBJ_GRAVITY          Enable gravity calculation (HGIMG3 only)
	OBJ_STATIC           Recognized as an obstacle (HGIMG3 only)
	OBJ_BOUND            Repel on the ground (for mesh map collision) (HGIMG3 only)
	OBJ_ALIEN            Aim at target (for mesh map collision) (HGIMG3 only)
	OBJ_WALKCLIP         Subject to movement restrictions (for mesh map collision) (HGIMG3 only)
	OBJ_EMITTER          Become an emitter object (HGIMG3 only)

^p
In the case of HGIMG3, it is the same as the mode value specified by the regobj command.
The OBJ_2D mode is set automatically, so do not change it mid-stream.
To select multiple items simultaneously, specify them separated by "|", such as "OBJ_LATE|OBJ_MOVE". If you do not specify anything, you can set it to 0 or omit it.

sw operates as follows:
^p
	sw = 0 : Add the specified mode value
	sw = 1 : Delete the specified mode value
	sw = 2 : Set only the specified mode value
^p
%href


%index
setcoli
Set Object Collision
%group
Extended Screen Control Commands
%prm
id,mygroup,enegroup
id       : Object ID
mygroup  : Group value to which you belong
enegroup : Group value to detect collisions with
%inst
Sets collision group information for the object.
The collision group value can be selected from only one of the following: 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768.
%href
getcoli
findobj
nextobj


%index
getcoli
Object Collision Detection
%group
Extended Screen Control Commands
%prm
val,id,distance,startid,numid
val           : Variable name to which the result is assigned
id(0)         : Object ID
distance(1.0) : Range to detect collisions (real number)
startid(0)    : Object ID to start detection
numid(-1)     : Number of object IDs to be detected (-1 = all)
%inst
Based on the collision information of the specified object, this command checks the ID of another object that the object is colliding with.
distance specifies the collision range (radius) as a real number.
If a collision is detected, the object ID is assigned to the variable.
If no collision is detected, -1 is assigned.
^
You can specify the range of object IDs to detect collisions by using the startid and numid parameters.
Specify the object ID to start detection with startid, and specify the number of object IDs with numid. If numid is omitted or is a negative value, all object IDs after the start object ID are targeted.
For example, if startid is 100 and numid is 50, objects with object IDs in the range of 100 to 149 are targeted for judgment. Normally, omitting the parameters will target all objects for detection. Use this when you want to limit the detection to only the specified range of object IDs.
In HGIMG4, lights and cameras are also treated as object IDs, but please note that the range of IDs that can be set is limited to 3D models and 2D sprite objects only.
^
In HGIMG4, if you specify a negative value for distance, collision detection is performed based on the value obtained by multiplying the collision range (bounding box) of the 3D model by distance.
For example, if you specify -1.5, collision detection is performed with the collision range enlarged by 1.5 times. Also, the gppcontact command can be used to create more accurate collision information.
%href
setcoli
findobj
nextobj
gppcontact


%index
getobjcoli
Get Object Group
%group
Extended Screen Control Commands
%prm
var,id,group
var      : Variable name to which the result is assigned
id(0)    : Object ID
group(0) : Group ID
%inst
Gets the group value (such as collision group) to which the specified object belongs and assigns it to the variable specified by var.
The group values are as follows:
^p
Group ID   Content
---------------------------------------
  0          Collision Group (set by setcoli)
  1          Collision Target Group (set by setcoli)
  2          Rendering Group (set by setobjrender)
  3          Lighting Group (set by setobjrender)
  4          Get Bounding Sphere Size (HGIMG4 only) (*)

(*) items are assigned as double values
^p
If you specify 4 for the group ID, the radius size of the bounding sphere surrounding the corresponding 3D model is assigned. This is only valid in HGIMG4.

%href
setcoli
getcoli
setobjrender


%index
setobjrender
Set Object Rendering Group
%group
Extended Screen Control Commands
%prm
id,rendergroup,lightgroup
id(0)           : Object ID
rendergroup(1)  : Rendering group value
lightgroup(1)   : Lighting group value
%inst
Sets rendering group and lighting group information for the object.
The rendering group value is a value for setting the ON/OFF of display when rendering from the camera. If it is the same as the rendering group value of the camera, the display is enabled.
Normally, group 1 is set for both the object and the camera. It can be used to disable the display only from a specific camera.
The lighting group value is used to enable/disable specific lights. If it is different from the lighting group value of the light, the light is disabled.

Each group value can be specified by combining arbitrary bits from 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768.

%href
setcoli
getobjcoli


%index
findobj
Object Search
%group
Extended Screen Control Commands
%prm
exmode,group
exmode(0) : Mode to exclude from the search
group(0)  : Collision group value to search for
%inst
Lists valid objects.
If a collision group value is specified, only objects belonging to that specific collision group are searched.
If the collision group value is 0, all objects are searched.
First, execute findobj, and then use the nextobj command to search for the corresponding object.
Also, the mode specified by exmode (same as the mode value specified by regobj) is excluded from the search.
%href
setcoli
nextobj


%index
nextobj
Next Object Search
%group
Extended Screen Control Commands
%prm
val
val      : Variable name to which the result is assigned
%inst
Searches for objects based on the conditions specified by the findobj command.
When found, the object ID is assigned to the variable.
-1 is assigned when there are no more search targets.
%href
setcoli
findobj


%index
setborder
Set Object Effective Range
%group
Extended Screen Control Commands
%prm
fx,fy,fz,option
( fx,fy,fz ) : Border area setting value (real number)
option(0) : Setting option (0-2)
%inst
Sets the border area (object effective range).
The content set for ( fx,fy,fz ) changes depending on the option parameter.
If the option parameter is omitted or is 0,
A cube with ( fx,fy,fz ) size centered on ( 0,0,0 ) becomes the border area.
If the option parameter is 1, the coordinates of ( fx,fy,fz ) are set as the border area on the smaller value side.
If the option parameter is 2, the coordinates of ( fx,fy,fz ) are set as the border area on the larger value side.

%href
regobj
setobjmode

%index
selmoc
Set MOC information
%group
Extended Screen Control Commands
%prm
id, mocofs
id     : Object ID
mocofs : MOC group specification
%inst
Specifies the MOC group to be targeted by the MOC setting command.
id is the object ID.
Normally, use the selpos, selang, selscale, seldir commands.
%href
selpos
selang
selscale
seldir
selcam
selcpos
selcang
selcint

%index
objgetfv
Get MOC information
%group
Extended Screen Control Commands
%prm
fv
fv      = Variable name to which the FV value is assigned
%inst
Assigns the value set in the MOC to the variable fv.
%href
objsetfv
fvset
fvadd
fvsub
fvmul
fvdiv

%index
objsetfv
Set MOC information
%group
Extended Screen Control Commands
%prm
fv
fv      = Variable name to which the FV value is assigned
%inst
Sets the content of the variable fv to the MOC.
%href
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv

%index
objaddfv
Add MOC information
%group
Extended Screen Control Commands
%prm
fv
fv      = Variable name to which the FV value is assigned
%inst
Adds the content of the variable fv to the MOC.
%href
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv

%index
objexist
Check if object ID is valid
%group
Extended Screen Control Commands
%prm
p1
p1(0) : Object ID
%inst
Checks whether the object ID specified by p1 is valid.
If the object ID is valid (registered), 0 is assigned to the system variable stat.
If the object ID is invalid (not registered), -1 is assigned to the system variable stat.

%href
regobj
delobj

%index
event_wait
Add wait time event
%group
Extended Screen Control Commands
%prm
id,p1
id      : Event ID
p1(0)   : Wait time (frames)
%inst
Adds a wait time event to the event ID specified by id.
The wait time event pauses the advancement to the next event for the number of frames specified by p1.

%href
newevent
setevent

%index
event_jump
Add jump event
%group
Extended Screen Control Commands
%prm
id,p1,p2
id      : Event ID
p1(0)   : Event number of the jump destination
p2(0)   : Probability of ignoring the jump (%)
%inst
Adds a jump event to the event ID specified by id.
The jump event instructs to continue execution from the specified event number.
It is equivalent to the goto command in the event list.
The event number specified by p1 is counted as 0,1,2... in the order in which the events were added.
p2 can be used to set the probability of ignoring the jump (%).
If it is 0 or omitted, the jump is always performed (unconditionally).
Otherwise, the jump is performed with a probability of 1 to 100% based on a random number.
If the jump does not occur, the next event is advanced to.

%href
newevent
setevent

%index
event_prmset
Add parameter setting event
%group
Extended Screen Control Commands
%prm
id,p1,p2
id      : Event ID
p1(0)   : Parameter ID (PRMSET_*)
p2(0)   : Value to be set
%inst
Adds a parameter setting event to the event ID specified by id.
The parameter setting event sets the value of p2 to the parameter ID specified by p1.
(The previously set content will be deleted)
The following names can be used for the parameter ID.
^p
Parameter ID   Content
---------------------------------------
		PRMSET_FLAG            Object registration flag(*)
		PRMSET_MODE            Mode flag value
		PRMSET_ID              Object ID(*)(HGIMG4 only)
		PRMSET_ALPHA           Transparency (留 value) (HGIMG4 only)
		PRMSET_SHADE           Shading mode (HGIMG3 only)
		PRMSET_TIMER           Timer value
		PRMSET_MYGROUP         Own collision group
		PRMSET_COLGROUP        Collision detection collision group
		PRMSET_SHAPE           Shape ID(*)(HGIMG4 only)
		PRMSET_USEGPMAT        Material ID(HGIMG4 only)
		PRMSET_USEGPPHY        Physics setting ID(*)(HGIMG4 only)
		PRMSET_COLILOG         Collision log ID(*)(HGIMG4 only)
		PRMSET_FADE            Fade parameter(HGIMG4 only)
		PRMSET_SPRID           Source buffer ID (sprite only) (HGIMG4 only)
		PRMSET_SPRCELID        Source cell ID (sprite only) (HGIMG4 only)
		PRMSET_SPRGMODE        Copy mode (sprite only) (HGIMG4 only)

		(*) items are read-only
^p

%href
event_prmon
event_prmoff
event_prmadd
newevent
setevent

%index
event_prmon
Add parameter bit setting event
%group
Extended Screen Control Commands
%prm
id,p1,p2
id      : Event ID
p1(0)   : Parameter ID (PRMSET_*)
p2(0)   : Bit to be set
%inst
Adds a parameter bit setting event to the event ID specified by id.
The parameter bit setting event sets the bit of p2 to the parameter ID specified by p1.
(The previously set content is retained, and only the bits of the new value are enabled)
For details on the parameter ID, refer to the event_prmset command.

%href
event_prmset
event_prmoff
newevent
setevent

%index
event_prmoff
Add parameter bit clearing event
%group
Extended Screen Control Commands
%prm
id,p1,p2
id      : Event ID
p1(0)   : Parameter ID (PRMSET_*)
p2(0)   : Bit to be cleared
%inst
Adds a parameter bit clearing event to the event ID specified by id.
The parameter bit clearing event clears only the bits of p2 from the parameter ID specified by p1.
For details on the parameter ID, refer to the event_prmset command.

%href
event_prmset
event_prmon
newevent
setevent

%index
event_setpos
Add pos group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent

%index
event_setang
Add ang group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setpos
event_setangr
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent

%index
event_setangr
Add ang group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setpos
event_setang
event_setscale
event_setdir
event_setefx
event_setwork
newevent
setevent

%index
event_setscale
Add scale group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setpos
event_setang
event_setangr
event_setdir
event_setefx
event_setwork
newevent
setevent

%index
event_setdir
Add dir group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setefx
event_setwork
newevent
setevent

%index
event_setwork
Add work group setting event
%group
Extended Screen Control Commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to be set (lower limit)
(x2,y2,z2) : Value to be set (upper limit)
%inst
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters that the object has.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within their respective ranges are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

%href
event_setpos
event_setang
event_setangr
event_setscale
event_setdir
event_setefx
newevent
setevent

%index
event_pos
Add pos group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Value to be set
sw(1)      : Interpolation option
%inst
Adds a group change event to the event ID specified by id.
The group change event sets the time-based changes of the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The interpolation options for sw can be specified as follows:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute value spline is set.
If 16 is added to the value of sw, the value of the work group that the object has is used instead of the value set by (x1, y1, z1) to set the change.
%href
event_ang
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_ang
Add an ang group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Values to be set
sw(0)      : Interpolation option
%inst
Adds a group change event to the event ID specified by id.
A group change event sets the time-based changes to an object's parameters.
The values will become (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute linear interpolation is set.
If 16 is added to the value of sw, the values of the work group owned by the object are used to set the change instead of the values set by (x1, y1, z1).
(Angle specification is the same as the setang command, and the rotation order is X->Y->Z. The event_angy and event_angz commands are available for rotating in other orders.)


%href
event_pos
event_angr
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_angr
Add an ang group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Values to be set
%inst
Adds a group change event to the event ID specified by id.
A group change event sets the time-based changes to an object's parameters.
The values will become (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute linear interpolation is set.
If 16 is added to the value of sw, the values of the work group owned by the object are used to set the change instead of the values set by (x1, y1, z1).

%href
event_pos
event_ang
event_scale
event_dir
event_efx
event_work
newevent
setevent


%index
event_scale
Add a scale group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Values to be set
sw(0)      : Interpolation option
%inst
Adds a group change event to the event ID specified by id.
A group change event sets the time-based changes to an object's parameters.
The values will become (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute linear interpolation is set.
If 16 is added to the value of sw, the values of the work group owned by the object are used to set the change instead of the values set by (x1, y1, z1).

%href
event_pos
event_ang
event_angr
event_dir
event_efx
event_work
newevent
setevent


%index
event_dir
Add a dir group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Values to be set
sw(0)      : Interpolation option
%inst
Adds a group change event to the event ID specified by id.
A group change event sets the time-based changes to an object's parameters.
The values will become (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute linear interpolation is set.
If 16 is added to the value of sw, the values of the work group owned by the object are used to set the change instead of the values set by (x1, y1, z1).

%href
event_pos
event_ang
event_angr
event_scale
event_efx
event_work
newevent
setevent


%index
event_work
Add a work group change event
%group
Extended Screen Control Commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Values to be set
sw(0)      : Interpolation option
%inst
Adds a group change event to the event ID specified by id.
A group change event sets the time-based changes to an object's parameters.
The values will become (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option:
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, absolute linear interpolation is set.

%href
event_pos
event_ang
event_angr
event_scale
event_dir
event_efx
newevent
setevent


%index
event_addpos
Add a pos group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addang
Add an ang group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addpos
event_addangr
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addangr
Add an ang group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addpos
event_addang
event_addscale
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_addscale
Add a scale group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addpos
event_addang
event_addangr
event_adddir
event_addefx
event_addwork
newevent
setevent


%index
event_adddir
Add a dir group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addpos
event_addang
event_addangr
event_addscale
event_addefx
event_addwork
newevent
setevent


%index
event_addwork
Add a work group addition event
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Event ID
(x,y,z) : Values to be added
%inst
Adds a group addition event to the event ID specified by id.
A group addition event adds the (x,y,z) values to the object's parameters.

%href
event_addpos
event_addang
event_addangr
event_addscale
event_adddir
event_addefx
newevent
setevent


%index
setevent
Set an event to an object
%group
Extended Screen Control Commands
%prm
p1,p2,p3
p1(0)  : Object ID
p2(0)  : Event ID
p3(-1) : Event slot ID
%inst
Applies the event p2 to the object specified by p1.
It is necessary to prepare an event list in which a predetermined flow of processing (event) has been registered in advance.
^
Up to four events set by the setevent command can be applied to one object at the same time.
You can specify the event slot ID (0 to 3) for setting the event in p3.
If p3 is omitted or -1 is specified, the available event slot IDs are used in order from 0.
To delete an event set for an object, specify the event slot ID in p3 and set p2 to a negative value.
^
If the event is successfully set, the event slot ID set in the system variable stat is assigned.
If the event setting fails, -1 is assigned to the system variable stat.


%href
newevent


%index
delevent
Delete an event list
%group
Extended Screen Control Commands
%prm
p1
p1 : Event ID
%inst
Deletes the event list specified by p1.

%href
newevent


%index
newevent
Create an event list
%group
Extended Screen Control Commands
%prm
p1
p1 : Variable name to which the Event ID will be assigned
%inst
Acquires a new event ID and assigns it to the variable specified by p1.
^
When creating a new event, you must always acquire an event ID with the newevent command.
Next, you can register various operations using the event list addition commands starting with "event_".
Once acquired, the event ID is retained until the scene is reset (hgreset command) or the event list is deleted by the delevent command.
^
The event thus created can be applied to an object at any time using the setevent command.


%href
delevent
setevent


%index
getang
Get ang group information
%group
Extended Screen Control Commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variables to be obtained

%inst
Gets the parameters of an object.
The contents of the ang group (display angle) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as real number type variables.
By adding "i" to the end of the command, values can be retrieved as integer values.
%href
getangi
getpos
getangr
getscale
getdir
getefx
getwork


%index
getangr
Get ang group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variable to retrieve

%inst
Gets the parameters of the object.
The contents of the ang group (display angle) are assigned to the variables specified by (x,y,z).
(x,y,z) are set as integer variables.
The unit of angle is an integer, using a value that cycles from 0 to 255.

%href
getpos
getang
getscale
getdir
getefx
getwork


%index
event_delobj
Add object deletion event
%group
Extended screen control commands
%prm
id
id         : Event ID
%inst
Adds an object deletion event to the event ID specified by id.
The object deletion event is a command to delete the object that is currently executing the event itself.
%href
event_regobj
newevent
setevent


%index
event_uv
Add UV setting event
%group
Extended screen control commands
%prm
p1,p2,p3
p1    : Event ID
p2    : X offset
p3    : Y offset
%inst
Adds a UV setting event to the event ID specified by id. The specifications of this command differ between HGIMG3 and HGIMG4.
In HGIMG3, it dynamically changes the UVs of a single polygon in 2D and 3D (excluding x-format models).
This is for realizing texture animation. When the event is executed, the texture's UV coordinates are reset to the value (number of dots) specified by (p2, p3).
In HGIMG4, the cell ID of an object generated as a 2D sprite node is set to the value specified by the p2 parameter. It is not applied to 3D nodes.
%href
newevent
setevent


%index
getnearobj
Search for the nearest object
%group
Extended screen control commands
%prm
var,id,group,range
var      : Variable name to which the result is assigned
id(0)    : Object ID of the search source
group(0) : Collision group to be searched
range(10.0): Search distance (real number)
%inst
Searches for another object closest to the object ID of the search source.
Searches for the object located closest to the object specified by id and assigns the object ID to the variable specified by var.
During the search, it refers to the collision group specified by group and the distance specified by range.
Only objects that match the collision group specified by group are searched.
(The collision target groups set in advance with the setcoli command, etc., are not referenced.)
range specifies how far to search.
For 3D objects, the coordinate distance is used, and for 2D objects, the distance on the 2D coordinates (number of dots) is used.
^
In HGIMG4, if a negative value is specified for range, collision detection is performed based on the value obtained by multiplying the collision range (bounding box) of the 3D model by range.
For example, if -1.5 is specified, collision detection is performed with the collision range enlarged by 1.5 times. Also, the gppcontact command can be used to create more accurate collision information.

%href
getcoli
findobj
