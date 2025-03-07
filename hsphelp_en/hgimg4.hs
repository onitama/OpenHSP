;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;
%type
Extension instructions
%ver
3.6
%note
Include hgimg4.as or hgimg4dx.as.
%date
2021/01/06
%author
onitama
%dll
hgimg4
%url
http://www.onionsoft.net/
%port
Win
%portinfo
Works on Windows + OpenGL 3.1+ / DirectX9 systems.



%index
gpreset
Initialization of HGIM G4
%group
Extended screen control command
%prm
option
option (0): Initialization option
%inst
Initialize HGIM G4.
Use this when rebuilding the scene.
The initialization content changes depending on the content set in option.
^p
Option value Contents
	---------------------------------------------------------------
0 Destroy all objects and return to the initial state
1 Discard only the objects that exist in the scene
^p
%href


%index
gpdraw
Draw all objects in the scene
%group
Extended screen control command
%prm
option
option (0): Drawing option
%inst
Draws all the objects in the scene.
By specifying the option parameter, you can draw only a limited number of items.
^p
        Macro name Contents
    --------------------------------------------------------------------
Automatic movement process of GPDRAW_OPT_OBJUPDATE object
GPDRAW_OPT_DRAWSCENE 3D scene drawing process
GPDRAW_OPT_DRAWSCENE_LATE 3D scene drawing process (OBJ_LATE)
GPDRAW_OPT_DRAW2D 2D sprite drawing process
GPDRAW_OPT_DRAW2D_LATE 2D sprite drawing process (OBJ_LATE)
^p
If you omit the option parameter, all items are selected.
Normally, you don't have to specify the option parameter.
Also, if you use only 2D direct drawing instructions and do not need to draw with an object, you do not need to write a gpdraw instruction.
Items with the OBJ_LATE mode flag value added (including those with a semi-transparent 留 value) are drawn after the opaque object, so the items are separated.
You can also specify multiple items separated by "|".
^p
Example:
	gpdraw GPDRAW_OPT_DRAW2D|GPDRAW_OPT_DRAW2D_LATE
^p
In the above example, only 2D sprite drawing process and 2D sprite drawing process (OBJ_LATE) are executed.
If you use this, you can use another drawing command such as gcopy command between 3D scene drawing and 2D sprite drawing.
It becomes possible to display.

%href


%index
gpusescene
Switching scenes
%group
Extended screen control command
%prm
id
id (0): Scene ID to select
%inst
(This is a reserved keyword to support in future versions. The feature is not yet implemented.)
%href


%index
gpsetprm
Object core parameter settings
%group
Extended screen control command
%prm
objid,prmid,value
objid (0): Object ID
prmid (0): core parameter ID
value (0): Value to be set (integer value)
%inst
Overrides the value specified by value in the core parameter held for each object.
If you want to turn ON / OFF bit by bit instead of overwriting the value, use the gpsetprmon / gpsetprmoff instruction.

The core parameter is a 32-bit integer value that manages various information.
The items that can be specified as the core parameter ID are as follows.
^p
Macro name | Contents
	--------------------------------------------------------------
PRMSET_FLAG Object registration flag (*)
PRMSET_MODE mode flag value
PRMSET_ID Object ID (*)
PRMSET_ALPHA Transparency (留 value)
PRMSET_TIMER Timer value
PRMSET_MYGROUP own collision group
PRMSET_COLGROUP Collision group for collision detection
PRMSET_SHAPE Shape ID (*)
PRMSET_USEGPMAT Material ID
PRMSET_COLILOG Collision log ID (*)
PRMSET_FADE fade parameter
PRMSET_SPRID Source buffer ID (sprites only)
PRMSET_SPRCELID Source cell ID (sprites only)
PRMSET_SPRGMODE Copy mode (sprites only)

Items in (*) are read only
^p
%href
gpgetprm
gpsetprmon
gpsetprmoff


%index
gpgetprm
Get core parameters of an object
%group
Extended screen control command
%prm
var,objid,prmid
var: variable name to which the value is assigned
objid (0): Object ID
prmid (0): core parameter ID
%inst
Gets the values for the core parameters held for each object.
The core parameter is a 32-bit integer value that manages various information.
The items that can be specified as the core parameter ID are as follows.
^p
Macro name | Contents
	--------------------------------------------------------------
PRMSET_FLAG Object registration flag (*)
PRMSET_MODE mode flag value
PRMSET_ID Object ID (*)
PRMSET_ALPHA Transparency (α value)
PRMSET_TIMER Timer value
PRMSET_MYGROUP own collision group
PRMSET_COLGROUP Collision group for collision detection
PRMSET_SHAPE Shape ID (*)
PRMSET_USEGPMAT Material ID
PRMSET_COLILOG Collision log ID (*)
PRMSET_FADE fade parameter
PRMSET_SPRID Source buffer ID (sprites only)
PRMSET_SPRCELID Source cell ID (sprites only)
PRMSET_SPRGMODE Copy mode (sprites only)

Items in (*) are read only
^p
%href
gpsetprm



%index
gppostefx
Post effect generation
%group
Extended screen control command
%prm
var
var: Variable name to which the effect ID is assigned
%inst
(This is a reserved keyword to support in future versions. The feature is not yet implemented.)
%href


%index
gpuselight
Registration of light object
%group
Extended screen control command
%prm
id,index
id (0): ID of the light object to be registered
index = 0 to 9 (0): Index of the light object to be registered
%inst
Registers the specified light object with the current light.
You must always specify a light object that is initialized as a light.
Light objects can be initialized with the gplight instruction.
Up to 10 directional lights, point lights, and spot lights can be registered in the current light.
In the initial state, only one directional light can be set as the current light. When using multiple lights such as point lights and spot lights, it is necessary to determine the number of current lights in advance by using the gpresetlight command.
The index value is used to register multiple lights with the current light. It is possible to set up to 10 lights for each type in the form of 0 for the first light, 1 for the second light, and so on.
By registering with the current light, the model materials generated after that will be affected by the current light.
If you want to change the light settings that are already set on the object, use the setobjlight command.

%href
gpresetlight
gplight
gpusecamera
setobjlight


%index
gpusecamera
Switching camera objects
%group
Extended screen control command
%prm
id
id (0): ID of the camera object to select
%inst
Selects the specified node object as a valid camera object.
You must always specify a node object that is initialized as a camera.
After that, the scene is drawn with the switched camera as the viewpoint.
%href
gpuselight


%index
gpmatprm
Material parameter settings
%group
Extended screen control command
%prm
id,"name",x,y,z
id (0): Material ID / Object ID
"name": Parameter name
x (0.0): X setting value (real value)
y (0.0): Y setting value (real value)
z (0.0): Z setting value (real value)
%inst
Sets the shader parameters for the generated material.
^p
Example:
gpmatprm objid, "u_diffuseColor", 1,0,0.5; Material parameter settings
^p
In the above example, a parameter named u_diffuseColor is set to a vector value of (1.0, 0.0, 0.5).
The gpmatprm1 and gpmatprm4 instructions are provided according to the number of items to be set in the shader parameter.
Use gpmatprm to set the three items (x, y, z).
%href
gpmat
gpmatprm1
gpmatprm4


%index
gpmatstate
Material state setting
%group
Extended screen control command
%prm
objid,"name","value"
id (0): Material ID / Object ID
"name": Setting item name (character string)
"value": Setting value (character string)
%inst
Change the display state setting of the generated material.
Set the content specified by value for the item specified by name as a character string.
Please note that all item names and settings are character strings.
Please note that all item names and settings are character strings. (Item names and setting values are not case sensitive.)
^p
	name             value
    --------------------------------------------------------------------
"blend" Enable / disable blend
(Specify true or false)
"blendSrc" Select blend source (see below)
"blendDst" Select a blend destination (see below)
"cullFace" Enable / disable hidden surface removal (culling)
(Specify true or false)
"cullFaceSide" Face designation for hidden surface removal (culling)
"depthTest" true or false
(Specify true or false)
"depthWrite" Enable / disable Z-buffer write
(Specify true or false)
"depthFunc" Select Z-value comparison method (see below)

(*) Character string that can be set with blendSrc and blendDst

"ZERO" immediate value (0)
"ONE" immediate value (1)
"SRC_COLOR" Writing source color
"ONE_MINUS_SRC_COLOR" Original color (inverted value)
"DST_COLOR" Write destination color
"ONE_MINUS_DST_COLOR" Write destination color (inverted value)
"SRC_ALPHA" write source α
"ONE_MINUS_SRC_ALPHA" Write source α (inverted value)
"DST_ALPHA" write destination α
"ONE_MINUS_DST_ALPHA" Write destination α (inverted value)
"CONSTANT_ALPHA" α fixed value
"ONE_MINUS_CONSTANT_ALPHA" α fixed value (inverted value)
"SRC_ALPHA_SATURATE" Write source α inversion value

(*) Character string that can be set with cullFaceSide

BACK back side culling
FRONT surface culling
FRONT_AND_BACK Culling both sides

(*) Character string that can be set with depthFunc

NEVER always refuse
LESS Allowed only when the value is small
EQUAL Only the same value is allowed
LEQUAL Allowed only when the value is the same or smaller
GREATER Allowed only when the value is large
NOTEQUAL Only non-identical values are allowed
GEQUAL Allowed only when the value is the same or larger
ALWAYS always allow
^p
%href


%index
gpviewport
Viewport settings
%group
Extended screen control command
%prm
x,y,sx,sy
x (0): Viewport top left X coordinate
y (0): Viewport top left Y coordinate
sx: Viewport X size
sy: Viewport Y size
%inst
Set the area (viewport) where HGIM G4 draws.
Drawing is performed on the area of the size specified by (sx, sy) from (x, y) of the screen coordinates.
If sx, sy is omitted, the current display X, Y size is specified as the default.
%href


%index
setobjname
Set the node name of the object
%group
Extended screen control command
%prm
objid,"name"
objid (0): ID of the object
"name": Name to be set (character string)
%inst
Sets the name specified for the node object.
All node objects can be set to any specified name.
%href
getobjname


%index
getobjname
Get the node name of an object
%group
Extended screen control command
%prm
var,objid
var: variable name to which the result is assigned
objid (0): ID of the object
%inst
Gets the name specified for the node object.
The name is assigned as a string type to the variable specified by var.
All node objects have arbitrarily specified names.
%href
setobjname


%index
gpcolormat
Color material generation
%group
Extended screen control command
%prm
var,color,opt
var: Variable name to which the generated material ID is assigned
color (0): Material color (24bit RGB value)
opt (0): Material option value
%inst
Generates a color material with the specified color.
The color parameter specifies a 24-bit RGB value (value indicated by 0xRRGGBB). If omitted, white (0xffffff) is set.
By generating your own material, you can set detailed textures.

^p
Example:
	gpcolormat mat_id,0xff00ff,GPOBJ_MATOPT_NOLIGHT
	gpbox id_model, 1, , mat_id
^p
The above example will generate a cubic node object with an RGB color of 0xff00ff (purple) and a non-lighting material.
The opt parameter allows you to change the material settings.
These settings can also be set separately with the gpmatstate instruction.
^p
        Macro name Contents
    -------------------------------------------------------------------
GPOBJ_MATOPT_NOLIGHT Do not write
GPOBJ_MATOPT_NOMIPMAP Do not generate MIPMAP
GPOBJ_MATOPT_NOCULL Disable culling
GPOBJ_MATOPT_NOZTEST Disable Z test
GPOBJ_MATOPT_NOZWRITE Disables Z-buffer writing
GPOBJ_MATOPT_BLENDADD Set blend mode to add
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) will occur.
If generated correctly, the material ID (integer value) will be assigned to the variable specified by var.

%href
gptexmat
gpusermat


%index
gptexmat
Generate texture material
%group
Extended screen control command
%prm
var,"file",opt
var: Variable name to which the generated material ID is assigned
"file": Texture file name to be read
opt (0): Material option value
%inst
Generates a texture (image) material.
By generating your own material, you can set detailed textures.
^p
Example:
gptexmat id_texmat, "res / qbox.png"; Texture material creation
gpbox id_model, 1,, id_texmat; Add box node
^p
In the above example, a cubic node object with a material with qbox.png as a texture in the res folder will be created.
The opt parameter allows you to change the material settings.
These settings can also be set separately with the gpmatstate instruction.
^p
        Macro name Contents
    -------------------------------------------------------------------
GPOBJ_MATOPT_NOLIGHT Do not write
GPOBJ_MATOPT_NOMIPMAP Do not generate MIPMAP
GPOBJ_MATOPT_NOCULL Disable culling
GPOBJ_MATOPT_NOZTEST Disable Z test
GPOBJ_MATOPT_NOZWRITE Disables Z-buffer writing
GPOBJ_MATOPT_BLENDADD Set blend mode to add
GPOBJ_MATOPT_SPECULAR Apply specular when calculating light source
GPOBJ_MATOPT_MIRROR Display as inverted image
GPOBJ_MATOPT_CUBEMAP Set as a cubemap
GPOBJ_MATOPT_NODISCARD Disable pixel discard by alpha channel
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) will occur.
If generated correctly, the material ID (integer value) will be assigned to the variable specified by var.

%href
gpcolormat
gpusermat
gpscrmat


%index
gpscrmat
Generating offscreen texture material
%group
Extended screen control command
%prm
var,id,opt
var: Variable name to which the generated material ID is assigned
id: Referenced offscreen buffer ID
opt (0): Material option value
%inst
Generates a material that references the offscreen texture buffer.
By generating your own material, you can reuse the offscreen rendered image as a texture.
^p
Example:
	buffer id_render,512,512,screen_offscreen
gpscrmat id_texmat, id_render, GPOBJ_MATOPT_NOLIGHT | GPOBJ_MATOPT_NOMIPMAP; Texture material creation
gpbox id_model, 1,, id_texmat; Add box node
^p
The above example creates a cubic node object that references the offscreen texture buffer created by the buffer instruction.
The opt parameter allows you to change the material settings.
These settings can also be set separately with the gpmatstate instruction.
^p
        Macro name Contents
    -------------------------------------------------------------------
GPOBJ_MATOPT_NOLIGHT Do not write
GPOBJ_MATOPT_NOMIPMAP Do not generate MIPMAP
GPOBJ_MATOPT_NOCULL Disable culling
GPOBJ_MATOPT_NOZTEST Disable Z test
GPOBJ_MATOPT_NOZWRITE Disables Z-buffer writing
GPOBJ_MATOPT_BLENDADD Set blend mode to add
GPOBJ_MATOPT_SPECULAR Apply specular when calculating light source
GPOBJ_MATOPT_MIRROR Display as inverted image
GPOBJ_MATOPT_NODISCARD Disable pixel discard by alpha channel
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) will occur.
If generated correctly, the material ID (integer value) will be assigned to the variable specified by var.

%href
gpcolormat
gpusermat


%index
gpusermat
Generating custom materials
%group
Extended screen control command
%prm
var,"vsh","fsh","defs",color,opt
var: Variable name to which the generated material ID is assigned
"vsh": Vertex shader filename
"fsh": Fragment shader filename
"defs": additional label definitions
color (-1): Material color (24bit RGB value)
opt (0): Material option value
%inst
Generate a custom material.
Custom materials perform their own drawing with the vertex shader and fragment shader specified.
Specify the shader file name written in OpenGL shader language (GLSL) in the "vsh" and "fsh" parameters.
The "defs" parameter describes the label definition that is added when the shader is compiled.
You can set the default color with the color parameter. If omitted, white (0xffffff) is set.
If the "vsh", "fsh", and "defs" parameters are omitted, the contents set by gpusershader will be set.
The opt parameter allows you to change the material settings.
These settings can also be set separately with the gpmatstate instruction.
^p
        Macro name Contents
    -------------------------------------------------------------------
GPOBJ_MATOPT_NOLIGHT Do not write
GPOBJ_MATOPT_NOMIPMAP Do not generate MIPMAP
GPOBJ_MATOPT_NOCULL Disable culling
GPOBJ_MATOPT_NOZTEST Disable Z test
GPOBJ_MATOPT_NOZWRITE Disables Z-buffer writing
GPOBJ_MATOPT_BLENDADD Set blend mode to add
GPOBJ_MATOPT_SPECULAR Apply specular when calculating light source
GPOBJ_MATOPT_MIRROR Display as inverted image
GPOBJ_MATOPT_CUBEMAP Set as a cubemap
GPOBJ_MATOPT_NODISCARD Disable pixel discard by alpha channel
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) will occur.
If generated correctly, the material ID (integer value) will be assigned to the variable specified by var.
^
Custom materials allow you to control everything in your drawing with any shader.
This is an extension for advanced users who are familiar with shaders and should not normally be used.
If you generated a custom material, you can set the values to pass to the shader parameters with the gpmatprm instruction.

%href
gpmatprm
gpcolormat
gptexmat
gpusershader
gpscrmat


%index
gpclone
Duplicate node
%group
Extended screen control command
%prm
var,objid,eventid
var: Variable name to which the ID of the duplicated object is assigned
objid (0): Object ID of the duplication source
eventid (-1): Event ID
%inst
Creates a new object with the same settings as an existing object.
The ID of the duplicated object is assigned to the variable specified by var.
It has the same shape and parameter settings (excluding physical settings), but you can set new coordinates and angles.
When generating the same object multiple times, it saves the trouble of setting and reading many times and leads to speedup.
^p
Example:
gpload id_model, "res / duck"; model loading
gpclone i, id_model; clone the same
^p
In the above example, we clone the 3D model node (id_model) from the res / duck.gpb file and assign the new object ID to the variable i.
If the clone or ID assignment fails, -1 is assigned to the variable specified by var.
You can apply the event to the cloned object by specifying the event ID in the eventid parameter. If eventid is negative or omitted, no event will be set.
* Please note that in the current version, only viewable objects that exist in the scene can be cloned.
%href
gpdraw


%index
gpnull
Generate null node
%group
Extended screen control command
%prm
var
var: Variable name to which the generated object ID is assigned
%inst
Creates an empty node object (null node).
The ID of the duplicated object is assigned to the variable specified by var.
Null nodes are used, for example, to create nodes that do not need to have a model, such as cameras and lights.
The generated node can be freely manipulated in 3D space like any other node.
%href
gpcamera
gplight


%index
gpload
Generate 3D model node
%group
Extended screen control command
%prm
var,"file","name"
var: Variable name to which the generated object ID is assigned
"file": gpb file name to read
"name": Node name to be read
"defs": additional label definitions
%inst
Generate filed 3D model data (.gpb file) as a node.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
Specify the folder and file name with "file". You do not need to specify an extension such as ".gpb".
If you want to read only the node with a specific name in the file, you can specify it with "name".
If "name" is omitted, all the hierarchical structures contained in the file will be generated as read nodes.
Information about the material is read from the ".material" file. Be sure to prepare it as a set with the ".gpb" file.
If the "defs" parameter is specified, all shader compile-time specified label definitions will be added.
^
Use the gppbind instruction to make physical settings for a node.
For the contact judgment of 3D model data, a sphere that wraps the whole (bounding sphere) is used.
^
The created objects (nodes) are drawn together by the gpdraw instruction.
^p
Example:
gpload id_model, "res / duck"; model loading
^p
In the above example, we will load the duck.gpb file in the res folder.
At that time, the duck.material file that contains the material information is referenced.
In addition, if textures are needed, the image files in the same folder will also be loaded. If an error occurs when reading the file, a negative value will be assigned as the object ID.
If you want to investigate more detailed read errors and warnings, you can collect logs with the gpgetlog instruction.

^
The file (.gpb format) for loading with the gpload instruction is the standard data format used in gameplay 3D.
HGIMG4 provides a GPB converter (gpbconv.exe) to generate .gpb format.
See the HGIM G4 manual for more information.
%href
gpgetlog
gpdraw
gppbind


%index
gpplate
Generate board node
%group
Extended screen control command
%prm
var,sizex,sizey,color,matobj
var: Variable name to which the generated object ID is assigned
sizex (1): Generated X size
sizey (1): Y size generated
color (-1): Material color (24bit RGB value)
matobj (-1): Material ID
%inst
A node is generated using a single plate (quadrangle) standing vertically as 3D model data.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
You can specify the X, Y size of the model with the sizex, sizey parameters.
Specify the model color (material color) with the color parameter by the 24-bit RGB value (value indicated by 0xRRGGBB).
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
Specify when using a user-generated material, such as with the gpcolormat command.
If the matobj parameter is omitted, standard materials are used.
^
The created objects (nodes) are drawn together by the gpdraw instruction.
^
Use the gppbind instruction to make physical settings for a node.
The same shape as the original model is used for the contact judgment of the node.
%href
gpdraw
gppbind


%index
gpfloor
Generate floor node
%group
Extended screen control command
%prm
var,sizex,sizey,color,matobj
var: Variable name to which the generated object ID is assigned
sizex (1): Generated X size
sizey (1): Y size generated
color (-1): Material color (24bit RGB value)
matobj (-1): Material ID
%inst
Generate a node using one floor (quadrangle) as 3D model data.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
You can specify the X, Y size of the model with the sizex, sizey parameters.
Specify the model color (material color) with the color parameter by the 24-bit RGB value (value indicated by 0xRRGGBB).
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
Specify when using a user-generated material, such as with the gpcolormat command.
If the matobj parameter is omitted, standard materials are used.
^
The created objects (nodes) are drawn together by the gpdraw instruction.
^
Use the gppbind instruction to make physical settings for a node.
(Floor model is standard and has fixed physical characteristics in the same place)
The same shape as the original model is used for the contact judgment of the node.
%href
gpdraw
gppbind


%index
gpbox
Generate box node
%group
Extended screen control command
%prm
var,size,color,matobj
var: Variable name to which the generated object ID is assigned
size (1): generated size
color (-1): Material color (24bit RGB value)
matobj (-1): Material ID
%inst
Generates a node using a cube of the specified size as 3D model data.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
You can specify the X, Y size of the model with the sizex, sizey parameters.
Specify the model color (material color) with the color parameter by the 24-bit RGB value (value indicated by 0xRRGGBB).
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
Specify when using a user-generated material, such as with the gpcolormat command.
If the matobj parameter is omitted, standard materials are used.
^
The created objects (nodes) are drawn together by the gpdraw instruction.
^
Use the gppbind instruction to make physical settings for a node.
The same shape as the original model is used for the contact judgment of the node.
%href
gpdraw
gppbind


%index
gpspr
2D sprite node generation
%group
Extended screen control command
%prm
var,bufid,celid,gmode
var: Variable name to which the generated object ID is assigned
bufid (0): Buffer ID
celid (0): cell ID
gmode (3): copy mode
%inst
Generates a node to display 2D sprites.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in the 2D screen.
Use the bufid parameter to specify the buffer ID in which the display source image is loaded, and the celid parameter to specify the cell ID to display.
Also, specify a copy mode similar to that specified by the gcopy instruction with the gmode parameter.
^p
gmode value | content
	---------------------------------------------------
0,1 Alpha channel disabled
2 Alpha channel enabled
5 color addition / alpha channel enabled
^p
The created objects (nodes) are drawn together by the gpdraw instruction.
^p
Draw per sprite
gmode [gmode setting value]
pos [node X coordinates], [node Y coordinates]
celput [buffer ID], [cell ID], [node X scale], [node Y scale], [node Z rotation]
^p
In other words, the 2D sprite node can be thought of as a function that collectively performs the above processing with the settings registered in advance.
In addition, it is possible to easily determine the contact between 2D sprite nodes using the getcoli command.
%href
gpdraw


%index
gplight
Set light node
%group
Extended screen control command
%prm
id,opt,range,inner,outer
id (0): ID of the object
opt (0): Light generation option
range (1): Range of influence parameter
inner (0.5): Inner attenuation parameter
outer (1): Outer damping parameter
%inst
Adds the function as a light to the generated node.
The id parameter specifies the object ID of the node.
You can specify the light generation option value with the opt parameter.
^p
opt value | content
	--------------------------------------------------------------
GPOBJ_LGTOPT_NORMAL Parallel light source (directional light)
GPOBJ_LGTOPT_POINT Point light source (point light)
GPOBJ_LGTOPT_SPOT Spotlight
^p
The range parameter sets the range of influence of point lights and spot lights.
In addition, the inner and outer parameters set the inner and outer parameters at which the spotlight is attenuated.
^p
Example:
gpnull id_alight; generate a null node
gplight id_alight, GPOBJ_LGTOPT_NORMAL; Register as a light
gpuselight id_alight; Set to default light
^p
The gplight instruction only sets the object to function as a light.
The set light is reflected in the model material generated after that by registering it as the current light with the gpuselight command.

%href
gpresetlight
gpuselight
gpnull

%index
gpcamera
Set camera node
%group
Extended screen control command
%prm
id,fov,aspect,near,far,sw
id (0): ID of the object
fov (45): Field of view (FOV) parameter
aspect (1.5): aspect ratio
near (0.5): Near clip Z value
far (768): Fur clip Z value
sw (0): Camera type value (0,1)
%inst
Add the function as a camera to the generated node.
The id parameter specifies the object ID of the node.
Set the field of view (FOV) with the fov parameter. If 45 is specified, the field of view will be 45 degrees.
Specify the aspect ratio (aspect ratio) with the aspect parameter.
You can also specify near and far Z coordinates (Z coordinates used for clipping) with the near and far parameters.
^p
Example:
gpnull id_camera; generate a null node
gpcamera id_camera, 45, 1.5, 0.5, 768; Set as camera
gpusecamera id_camera; Select the camera to use
setpos id_camera, 0,20,20; Set the camera position
^p
By specifying 1 for the camera type value (sw), it is possible to set the camera that performs parallel projection (Orthographic). In that case, the fov value is reflected as the zoom value (1.0 is standard).

The cameras placed in the scene can be switched by the gpuse camera command.

%href
gpusecamera
gpnull


%index
gplookat
Rotate the node towards the specified coordinates
%group
Extended screen control command
%prm
objid,x,y,z
objid (0): Object ID
x (0): Target X coordinate
y (0): Target Y coordinate
z (0): Target Z coordinate
%inst
Rotates the 3D node towards the specified coordinates.
When applied to a camera, it is set to the angle at which the specified coordinates are gazed.
%href


%index
gppbind
Set standard physical characteristics for nodes
%group
Extended screen control command
%prm
objid,mass,friction,option
objid (0): Object ID
mass (1): Weight
friction (0.5): friction coefficient
option (0): Configuration option
%inst
Make basic physical settings for the node object.
The mass parameter is the weight of the object. The friction parameter is the coefficient of friction. If you omit the value, the default value is used.
By setting the mass to 0, it is set as a static rigid body (collision, but does not move).
Otherwise, it will fall to the floor according to the laws of physics.
^
The gppbind instruction sets only basic physical characteristics. To set detailed physical parameters, use the gppset instruction.
^
* When setting the physical behavior, be sure to create the part that will be the floor (ground).
A fixed floor surface can be created by setting standard physical characteristics for the floor generated by the gpfloor instruction.
If the floor did not exist, the object would continue to fall forever.
^
After that, the node object with physical settings will operate independently, and coordinate changes such as setpos will be invalid.
If you want to apply force to an object, use the gppapply instruction.
^
The option at the time of setting is added by the option parameter. You can specify the following macros.
^p
opt value | content
	--------------------------------------------------------------

GPPBIND_NOSCALE Do not reflect node scale in collision
Reflect the node model as a GPPBIND_MESH collision
^p
When you make physical settings for a node object, information (collision) for making contact judgments is created.

Box nodes, floor nodes, and board nodes treat their shapes as collisions. If GPPBIND_NOSCALE is specified for option, the original shape that does not reflect the scale will be a collision.
By default, 3D model nodes create collisions with spheres that cover the entire model. However, if GPPBIND_MESH is specified for option, the model shape itself is treated as a collision. However, if the complex model shape is made into a collision, the load on the contact judgment will increase.
Collisions are information that is independent of the shape of the model. Please note that the scale value of the node will not be reflected after the collision is created.


%href
gppapply
gppset


%index
getwork2
Get nodework value 2
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get
%inst
Gets the work2 parameter of the object.
(x, y, z) is set as a real variable.
You can get the value as an integer value by adding "i" to the end of the instruction.
%href
getwork2i


%index
getwork2i
Get nodework value 2 (integer value)
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Variables to get
%inst
Gets the work2 parameter of the object.
(x, y, z) is set as an integer variable.
You can get the value as a real number by using the getwork2 instruction.
%href
getwork2


%index
selquat
Set object rotation information as MOC information
%group
Extended screen control command
%prm
id
id (0): Object ID
%inst
Set the MOC group that is the target of the MOC setting command to quat (quaternion rotation information).
id is the object ID.
%href


%index
selwork2
Set Object Work 2 as MOC information
%group
Extended screen control command
%prm
id
id (0): Object ID
%inst
Set the MOC group that is the target of the MOC setting command to work2.
id is the object ID.
%href


%index
setwork2
work2 Set group information
%group
Extended screen contro�0����