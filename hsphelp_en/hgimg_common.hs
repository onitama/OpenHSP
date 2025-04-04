;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;
%type
Extension instructions
%ver
3.6
%note
Include hgimg4dx.as or hgimg4.as.
%author
onitama
%dll
hgimg4
%date
2021/01/18
%author
onitama
%url
http://www.onionsoft.net/
%port
Win
%portinfo
HGIMG4DX works in DirectX9, HGIMG4 works in OpenGL 3.1 environment


%index
fvseti
Vector setting from integer value
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = integer value
%inst
Substitute the integer value specified by (x, y, z) as a vector into the FV value.
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
Vector setting
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = calculated value (real value)
%inst
Substitute the decimal value (X, Y, Z) specified by (x, y, z) as a vector into the FV value.
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
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = calculated value (real value)
%inst
Adds the decimal value (X, Y, Z) specified by (x, y, z) to the FV value.
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
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = calculated value (real value)
%inst
Subtract the decimal value (X, Y, Z) specified by (x, y, z) from the FV value.
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
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = calculated value (real value)
%inst
Multiplies the FV value in parallel by the decimal value (X, Y, Z) specified by (x, y, z).
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
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = calculated value (real value)
%inst
Divide the decimal value (X, Y, Z) specified by (x, y, z) in parallel to the FV value.
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
Vector rotation
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = rotation angle (real value)
%inst
The FV value stored in the variable specified by fv is used as the X, Y, Z angle.
The result of rotating the vector specified by the decimal value (X, Y, Z) is assigned to the variable fv.
%href
fvset
fvdir
fvface


%index
fvface
Get the angle from the coordinates
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = X, Y, Z coordinate values (real values)
%inst
X, Y, Z rotation angle for viewing the specified X, Y, Z coordinates in a straight line from the X, Y, Z coordinates based on the vector (FV value) stored in the variable specified by fv. And assign it to the variable fv.
%href
fvset
fvdir


%index
fvmin
Truncate the elements of the vector to the minimum value
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = comparison value (real value)
%inst
Truncate each element of the FV value stored in the variable specified by fv so that the value specified by the parameter (X, Y, Z) becomes the minimum value.
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
Truncate the elements of the vector to the maximum value
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = comparison value (real value)
%inst
Truncate each element of the FV value stored in the variable specified by fv so that the value specified by the parameter (X, Y, Z) becomes the maximum value.
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
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = Vector value to be calculated (real value)
%inst
Find and substitute the cross product of the FV value stored in the variable specified by fv and the vector specified by the decimal value (X, Y, Z).
%href
fvseti
fvset
fvinner


%index
fvinner
Vector inner product
%group
Extended screen control command
%prm
fv,x,y,z
fv = variable name to which the FV value is assigned
(x, y, z) = Vector value to be calculated (real value)
%inst
Find the inner product of the FV value stored in the variable specified by fv and the vector specified by the decimal value (X, Y, Z) and assign it to fv.0.
%href
fvseti
fvset
fvouter


%index
fvunit
Vector normalization
%group
Extended screen control command
%prm
fv
fv = variable name to which the FV value is assigned
%inst
Normalizes the vector (FV value) stored in the variable specified by fv.
%href
fvseti
fvset


%index
fsin
Ask for a sign
%group
Extended screen control command
%prm
fval,frot
fval = variable name to which a real value is assigned
frot = rotation angle (radians)
%inst
Assigns the sine value of the angle specified by frot to the variable specified by fval.
The unit of angle is radians (2ƒÎ = 360 degrees).
%href
fcos
fsqr
froti



%index
fcos
Ask for cosine
%group
Extended screen control command
%prm
fval,frot
fval = variable name to which a real value is assigned
frot = rotation angle (radians)
%inst
Assigns the cosine value of the angle specified by frot to the variable specified by fval.
The unit of angle is radians (2ƒÎ = 360 degrees).
%href
fsin
fsqr
froti



%index
fsqr
Find the square root
%group
Extended screen control command
%prm
fval,fprm
fval = variable name to which a real value is assigned
fprm = value used for operation (real number)
%inst
Assigns the square root of the value specified by fprm to the variable specified by fval.
%href
fsin
fcos
froti



%index
str2fv
Convert string to vector
%group
Extended screen control command
%prm
fv,"x,y,z"
fv = variable name to which the FV value is assigned
"x, y, z" = A string containing real numbers separated by ","
%inst
The character string information specified by "x, y, z" is read as X, Y, Z decimal values separated by "," and stored in the variable specified by fv.
If there is a (illegal) character string that cannot be correctly recognized as a numerical value for each item, 0.0 will be assigned including the subsequent items.
%href
fv2str
str2f
f2str
f2i


%index
fv2str
Convert vector to string
%group
Extended screen control command
%prm
fv
fv = variable name to which the FV value is assigned
%inst
Converts the vector (FV value) stored in the variable specified by fv to a character string and returns the result in the system variable refstr.
%href
str2fv
str2f
f2str
f2i


%index
str2f
Convert strings to decimal numbers
%group
Extended screen control command
%prm
fval,"fval"
fval = variable name to which a real value is assigned
"fval" = string containing real numbers
%inst
Reads the character string information specified by "fval" as a decimal value and stores it in the variable specified by fval.
%href
fv2str
str2fv
f2str
f2i


%index
f2str
Convert decimal numbers to strings
%group
Extended screen control command
%prm
sval,fval
sval = variable name to which the string is assigned
fval = real value of conversion source
%inst
Converts the decimal value specified by fval to a string and returns the result in the string type variable specified by val.
%href
fv2str
str2fv
str2f
f2i


%index
delobj
Delete object
%group
Extended screen control command
%prm
ObjID
ObjID: Object ID
%inst
Deletes the specified object.
%href
regobj


%index
setpos
Set pos group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Sets the pos group (display coordinates) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.

%href
setang
setangr
setscale
setdir
setefx
setwork


%index
setang
ang Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Set the ang group (display angle) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.
The unit of angle is radians.
(The order of rotation is X-> Y-> Z. There are setangy and setangz instructions to rotate in other orders.)
There is also a setangr command to set the angle with an integer.

%href
setpos
setangr
setscale
setdir
setefx
setwork


%index
setangr
ang Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Set the ang group (display angle) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.
The unit of the angle is an integer, and the value that goes around from 0 to 255 is used.
There is also a setang command for setting the angle in radians.

%href
setpos
setang
setscale
setdir
setefx
setwork


%index
setscale
scale Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Sets the scale group (magnification) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.

%href
setpos
setang
setangr
setdir
setefx
setwork


%index
setdir
dir Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Sets the dir group (movement vector) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.
The value registered in the movement vector is referenced in the automatic movement mode (OBJ_MOVE) of the object.

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
id: object ID
(x, y, z): Value to set (default = 0)

%inst
Set the parameters of the object.
Set the work group (work value) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.

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
Add pos group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Sets the pos group (display coordinates) to the value specified by (x, y, z).
(x, y, z) can be a real number or an integer value.

%href
addang
addangr
addscale
adddir
addefx
addwork


%index
addang
ang Add group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the ang group (display angle).
(x, y, z) can be a real number or an integer value.
The unit of angle is radians.
There is also a setangr command to set the angle with an integer.

%href
addpos
addangr
addscale
adddir
addefx
addwork


%index
addangr
ang Add group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the ang group (display angle).
(x, y, z) can be a real number or an integer value.
The unit of the angle is an integer, and the value that goes around from 0 to 255 is used.
There is also a setang command for setting the angle in radians.

%href
addpos
addang
addscale
adddir
addefx
addwork


%index
addscale
scale Add group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the scale group (magnification).
(x, y, z) can be a real number or an integer value.

%href
addpos
addang
addangr
adddir
addefx
addwork


%index
adddir
Add dir group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the dir group (movement vector).
(x, y, z) can be a real number or an integer value.

%href
addpos
addang
addangr
addscale
addefx
addwork


%index
addwork
Add work group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to add (default = 0)

%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the work group (work value).
(x, y, z) can be a real number or an integer value.

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
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the pos group (display coordinates) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.

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
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the scale group (magnification) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.

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
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the dir group (movement vector) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.

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
get work group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the work group (work value) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.

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
Get pos group information as an integer
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the pos group (display coordinates) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as an integer variable.

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
Get scale group information as an integer
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the scale group (magnification) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as an integer variable.

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
Get dir group information as an integer
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the dir group (movement vector) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as an integer variable.

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
Get work group information as an integer
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the work group (work value) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as an integer variable.

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
Set moving coordinates to MOC information
%group
Extended screen control command
%prm
id
id: object ID
%inst
Set the MOC group that is the target of the MOC setting command to pos (coordinates)
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
id: object ID
%inst
Set the MOC group that is the target of the MOC setting command to ang (rotation angle)
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
Set scale to MOC information
%group
Extended screen control command
%prm
id
id: object ID
%inst
Set the MOC group that is the target of the MOC setting command to scale.
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
Set the amount of movement in MOC information
%group
Extended screen control command
%prm
id
id: object ID
%inst
Set the MOC group that is the target of the MOC setting command to dir (movement amount)
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
Set object work as MOC information
%group
Extended screen control command
%prm
id
id: object ID
%inst
Set the MOC group that is the target of the MOC setting command to work.
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
x: Value to set
y: Value to set 2
z: Value to set 3

%inst
Set the MOC information.
Offset numbers 0 to 3 parameters are covered.
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
fx: Value to be set (real value)
fy: Value to be set 2 (real value)
fz: Value to be set 3 (real value)

%inst
Set the MOC information.
Offset numbers 0 to 3 parameters are covered.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3
Add MOC information
%group
Extended screen control command
%prm
x,y,z
x: Value to add
y: Value to add 2
z: Value to add 3

%inst
Add x, y, z to the value set in the MOC information.
Offset numbers 0 to 3 parameters are covered.
%href
objset3
objadd3r
objset3r
objsetf3
objaddf3


%index
objaddf3
Add MOC information
%group
Extended screen control command
%prm
fx,fy,fz
fx: Value to be added (real value)
fy: Value to be added 2 (real value)
fz: Value to be added 3 (real value)

%inst
Add fx, fy, fz to the value set in the MOC information.
Offset numbers 0 to 3 parameters are covered.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objadd3r
Add MOC information
%group
Extended screen control command
%prm
ofs,fx,fy,fz
ofs: MOC offset number
fx: Value to add (integer angle value)
fy: Value to be added 2 (integer angle value)
fz: Value to add 3 (integer angle value)
%inst
Add fx, fy, fz to the value set in the MOC information.
However, the parameter obtained by converting the integer value (1 rotation at 256) into radians is added.
Please note that the values will not be normal except for the parameters that specify the angle.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
objset3r
Set MOC information
%group
Extended screen control command
%prm
x,y,z
x: Value to set
y: Value to set 2
z: Value to set 3

%inst
Set the angle information in the MOC information.
Offset numbers 0 to 3 parameters are covered.
Convert an integer value (1 rotation at 256) to radians and write the parameters.
Please note that the values will not be normal except for the parameters that specify the angle.
%href
objset3
objadd3
objset3r
objsetf3
objaddf3


%index
setobjmode
Object mode settings
%group
Extended screen control command
%prm
ObjID,mode,sw
ObjID: Object ID
mode: mode value
sw: Setting switch
%inst
Changes the mode of the specified object.
The mode value is the same as that specified by the regobj instruction.
sw works as follows.
^p
sw = 0: Add the specified mode value
sw = 1: Delete the specified mode value
sw = 2: Set only the specified mode value
^p
%href
regobj
setobjmodel


%index
setcoli
Object collision settings
%group
Extended screen control command
%prm
id,mygroup,enegroup
id: object ID
mygroup: Group value to which you belong
enegroup: Group value for which collisions are detected
%inst
Set collision group information for the object.
Only one collision group value can be selected from 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768.
%href
getcoli
findobj
nextobj


%index
getcoli
Object collision judgment
%group
Extended screen control command
%prm
val,id,distance
val: variable name to which the result is assigned
id: object ID
distance: Range to detect collision (real value)
%inst
Based on the collision information of the specified object, the ID of another object that the object collides with is checked.
distance specifies the range (radius) of collision with a real value.
If a collision is detected, the variable will be assigned the object ID.
If no collisions are detected, -1 is substituted.
^
In HGIMG4, when a negative value is specified for distance, collision detection is performed based on the value obtained by multiplying the collision range (bounding box) of the 3D model by distance.
For example, if you specify -1.5, collision detection will be performed with the collision range expanded 1.5 times. It is also possible to use the gppcontact instruction to create more accurate collision information.
%href
setcoli
findobj
nextobj
gppcontact


%index
getobjcoli
Get a group of objects
%group
Extended screen control command
%prm
var,id,group
var: variable name to which the result is assigned
id (0): Object ID
group (0): Group ID
%inst
Gets the group value (collision group, etc.) to which the specified object belongs and assigns it to the variable specified by var.
The group values are as follows.
^p
Group ID content
---------------------------------------
  0 Collision group (set by setcoli)
  1 Collision target group (set by setcoli)
  2 Rendering group (set by setobjrender)
  3 Writing group (set by setobjrender)
^p

%href
setcoli
getcoli
setobjrender


%index
setobjrender
Object rendering group settings
%group
Extended screen control command
%prm
id,rendergroup,lightgroup
id (0): Object ID
rendergroup (1): Rendering group value
lightgroup (1): Lighting group value
%inst
Set the rendering group and lighting group information for the object.
The rendering group value is a value for setting ON / OFF of the display when rendering from the camera. If it is the same as the rendering group value of the camera, the display is valid.
Normally, group 1 is set for both objects and cameras. It can be used when you want to disable the display only from a specific camera.
Lighting group values enable / disable for specific lights. If it is different from the lighting group value of the light, the light will be disabled.

Each group value can be specified by combining any bit from 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768.

%href
setcoli
getobjcoli


%index
findobj
Object search
%group
Extended screen control command
%prm
exmode,group
exmode (0): Mode to exclude search
group (0): Search target collision group value
%inst
Enumerates valid objects.
If you specify a collision group value, only objects that belong to a particular collision group are searched.
If the collision group value is 0, all objects will be searched.
You can run findobj first and then search for that object with the nextobj instruction.
Also, the mode specified by exmode (same as the mode value specified by regobj) is excluded from the search.
%href
setcoli
nextobj


%index
nextobj
Next object search
%group
Extended screen control command
%prm
val
val: variable name to which the result is assigned
%inst
Finds an object based on the conditions specified by the findobj instruction.
When searched, the variable is assigned the object ID.
When there are no more search targets, -1 is substituted.
%href
setcoli
findobj


%index
setborder
Object scope setting
%group
Extended screen control command
%prm
fx,fy,fz,option
(fx, fy, fz): Border area setting value (real value)
option (0): Configuration option (0-2)
%inst
Set the border area (object scope).
The content to be set in (fx, fy, fz) changes depending on the option parameter.
If the option parameter is omitted or 0,
The (fx, fy, fz) size cube centered on (0,0,0) is the border area.
If the option parameter is 1, set the coordinates of (fx, fy, fz) as the border area on the smaller number side.
If the option parameter is 2, set the coordinates of (fx, fy, fz) as the border area on the larger number side.

%href
regobj
setobjmode


%index
selmoc
Set MOC information
%group
Extended screen control command
%prm
id, mocofs
id: object ID
mocofs: MOC group specification
%inst
Specifies the MOC group that is the target of the MOC setting command.
id is the object ID.
Normally, use the selpos, selang, selscale, and seldir instructions.
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
Extended screen control command
%prm
fv
fv = variable name to which the FV value is assigned
%inst
Assign the value set in MOC to the variable fv.
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
Extended screen control command
%prm
fv
fv = variable name to which the FV value is assigned
%inst
Set the contents of the variable fv to MOC.
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
Extended screen control command
%prm
fv
fv = variable name to which the FV value is assigned
%inst
Adds the contents of the variable fv to the MOC.
%href
objgetfv
fvset
fvadd
fvsub
fvmul
fvdiv


%index
objexist
Check if the object ID is valid
%group
Extended screen control command
%prm
p1
p1 (0): Object ID
%inst
Checks if the object ID specified in p1 is valid.
If the object ID is valid (registered), 0 is assigned to the system variable stat.
If the object ID is invalid (unregistered), -1 is assigned to the system variable stat.

%href
regobj
delobj



%index
event_wait
Add wait time event
%group
Extended screen control command
%prm
id,p1
id: event ID
p1 (0): Wait time (frame)
%inst
The wait time event is added to the event ID specified by id.
The wait event holds the next event for the number of frames specified in p1.

%href
newevent
setevent


%index
event_jump
Added jump event
%group
Extended screen control command
%prm
id,p1,p2
id: event ID
p1 (0): Event number of jump destination
p2 (0): Probability of ignoring jump (%)
%inst
A jump event is added to the event ID specified by id.
The jump event tells you to continue execution from the specified event number.
It corresponds to the goto command in the event list.
The event number specified by p1 is counted as 0, 1, 2 ... in the order in which they were added to the event.
With p2, you can set the probability of ignoring jumps (%).
If it is 0 or omitted, it always jumps (unconditionally).
In other cases, jump with a probability of 1 to 100% based on random numbers.
If you do not jump, proceed to the next event.

%href
newevent
setevent


%index
event_prmset
Added parameter setting event
%group
Extended screen control command
%prm
id,p1,p2
id: event ID
p1 (0): Parameter ID (PRMSET_ *)
p2 (0): Value to be set
%inst
The parameter setting event is added to the event ID specified by id.
The parameter setting event sets the value of p2 to the parameter ID specified by p1.
(The contents set up to that point will be deleted)
You can use the following names for the parameter ID:
^p
Parameter ID Contents
---------------------------------------
PRMSET_MODE operation mode
PRMSET_FLAG Existence flag
PRMSET_SHADE Shading mode
PRMSET_TIMER timer
PRMSET_MYGROUP Collision group value
PRMSET_COLGROUP Target group value
^p

%href
event_prmon
event_prmoff
newevent
setevent


%index
event_prmon
Added parameter bit setting event
%group
Extended screen control command
%prm
id,p1,p2
id: event ID
p1 (0): Parameter ID (PRMSET_ *)
p2 (0): Bit to be set
%inst
The parameter bit setting event is added to the event ID specified by id.
The parameter bit setting event sets the bit of p2 to the parameter ID specified by p1.
(The contents set up to that point are retained, and only the bits with the new value are valid.)
See the event_prmset instruction for more information on parameter IDs.

%href
event_prmset
event_prmoff
newevent
setevent


%index
event_prmoff
Added parameter bit erase event
%group
Extended screen control command
%prm
id,p1,p2
id: event ID
p1 (0): Parameter ID (PRMSET_ *)
p2 (0): Bits to be erased
%inst
The parameter bit erase event is added to the event ID specified by id.
The parameter bit erase event starts from the parameter ID specified in p1.
Erases only the bits in p2.
See the event_prmset instruction for more information on parameter IDs.

%href
event_prmset
event_prmon
newevent
setevent


%index
event_setpos
Added pos group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
ang Added group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
ang Added group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
Added scale group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
Added dir group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
Added work group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
The group setting event is added to the event ID specified by id.
Group settings events set the parameters that an object has.
If you specify (x1, y1, z1) and (x2, y2, z2), the values within each range will be created with random numbers.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value will be set as it is.

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
Added pos group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1,sw
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
sw (1): Interpolation options
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, an absolute spline is set.
When 16 is added to the sw value, the change is set using the work group value of the object instead of the value set by (x1, y1, z1).

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
ang Added group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1,sw
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
sw (0): Interpolation options
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.
When 16 is added to the sw value, the change is set using the work group value of the object instead of the value set by (x1, y1, z1).
(The angle is specified in the same way as the setang instruction, and the rotation order is X-> Y-> Z. There are event_angy and event_angz instructions to rotate in other orders.)


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
ang Added group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.
When 16 is added to the sw value, the change is set using the work group value of the object instead of the value set by (x1, y1, z1).

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
scale Added group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1,sw
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
sw (0): Interpolation options
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.
When 16 is added to the sw value, the change is set using the work group value of the object instead of the value set by (x1, y1, z1).

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
Added dir group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1,sw
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
sw (0): Interpolation options
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.
When 16 is added to the sw value, the change is set using the work group value of the object instead of the value set by (x1, y1, z1).

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
Added work group change event
%group
Extended screen control command
%prm
id,frame,x1,y1,z1,sw
id: event ID
frame: Number of frames until change
(x1, y1, z1): Value to be set
sw (0): Interpolation options
%inst
The group change event is added to the event ID specified by id.
The group change event sets the time-dependent changes in the parameters that the object has.
The value becomes (x1, y1, z1) when the number of frames specified by frame has elapsed.
The following values can be specified for the sw interpolation option.
^p
sw = 0: Linear interpolation (absolute value)
sw = 1: Spline interpolation (absolute value)
sw = 2: Linear interpolation (relative value)
sw = 3: Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.

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
Added pos group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Added ang group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Added ang group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Added scale group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Added dir group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Added work group addition event
%group
Extended screen control command
%prm
id,x,y,z
id: event ID
(x, y, z): Value to be added
%inst
The group addition event is added to the event ID specified by id.
The group addition event adds the value of (x, y, z) to the parameters that the object has.

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
Set an event on an object
%group
Extended screen control command
%prm
p1,p2,p3
p1 (0): Object ID
p2 (0): Event ID
p3 (-1): Event slot ID
%inst
Applies the event of p2 to the object specified by p1.
It is necessary to prepare an event list in which a fixed flow process (event) is registered in advance.
^
Up to 4 events set by the setevent command can be applied to each object at the same time.
You can specify the event slot ID (0 to 3) for setting the event in p3.
If p3 is omitted or -1 is specified, the event slot IDs that are vacant in order from 0 are used.
To delete the event set in the object, specify the event slot ID in p3 and set p2 to a negative value.
^
If the event setting is successful, the event slot ID set in the system variable stat is assigned.
If the event setting fails, the system variable stat is assigned -1.


%href
newevent


%index
delevent
Delete event list
%group
Extended screen control command
%prm
p1
p1: Event ID
%inst
Deletes the event list specified in p1.

%href
newevent


%index
newevent
Create an event list
%group
Extended screen control command
%prm
p1
p1: Variable name to which the event ID is assigned
%inst
Gets the new event ID and assigns it to the variable specified in p1.
^
When creating a new event, it is necessary to acquire the event ID with the newevent instruction.
Next, various actions can be registered by the event list addition command starting with "event_".
Once the event ID is acquired, the scene is reset (hgreset instruction) or
Or it is retained until the event list is deleted by the delevent instruction.
^
The resulting event can be applied to the object at any time with the setevent command.


%href
delevent
setevent


%index
getang
ang get group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the ang group (display angle) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.

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
ang get group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get

%inst
Gets the parameters of the object.
The contents of the ang group (display angle) are assigned to the variables specified by (x, y, z).
(x, y, z) is set as an integer variable.
The unit of the angle is an integer, and the value that goes around from 0 to 255 is used.

%href
getpos
getang
getscale
getdir
getefx
getwork


%index
event_delobj
Added object delete event
%group
Extended screen control command
%prm
id
id: event ID
%inst
The object deletion event is added to the event ID specified by id.
An object delete event is an instruction that deletes the object that is currently executing the event.
%href
event_regobj
newevent
setevent


