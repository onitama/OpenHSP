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
Items with the OBJ_LATE mode flag value added (including those with a semi-transparent —¯ value) are drawn after the opaque object, so the items are separated.
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
PRMSET_ALPHA Transparency (—¯ value)
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
PRMSET_ALPHA Transparency (ƒ¿ value)
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
"SRC_ALPHA" write source ƒ¿
"ONE_MINUS_SRC_ALPHA" Write source ƒ¿ (inverted value)
"DST_ALPHA" write destination ƒ¿
"ONE_MINUS_DST_ALPHA" Write destination ƒ¿ (inverted value)
"CONSTANT_ALPHA" ƒ¿ fixed value
"ONE_MINUS_CONSTANT_ALPHA" ƒ¿ fixed value (inverted value)
"SRC_ALPHA_SATURATE" Write source ƒ¿ inversion value

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
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)
%inst
Set the parameters of the object.
Sets the value specified by (x, y, z) to the work2 group (work2 value).
(x, y, z) can be a real number or an integer value.
%href
setwork


%index
addwork2
work2 Add group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)
%inst
Set the parameters of the object.
Adds the value specified by (x, y, z) to the work2 group (work2 value).
(x, y, z) can be a real number or an integer value.
%href
addwork


%index
gpcnvaxis
Perform 3D coordinate conversion
%group
Extended screen control command
%prm
var_x,var_y,var_z,x,y,z,mode
var_x: Variable to which the X value is assigned
var_y: Variable to which the Y value is assigned
var_z: Variable to which the Z value is assigned
x (0.0): X value of conversion source
y (0.0): Y value of conversion source
z (0.0): Z value of conversion source
mode (0): Conversion mode
%inst
Converts the 3D coordinates of (x, y, z) according to the determined mode.
The result is assigned in real type to the variables specified by var_x, var_y, var_z. (Variable type is set automatically)
The contents of conversion by mode value are as follows.
^p
Mode content
-----------------------------------------------
0 2D coordinate (X, Y) position to be drawn + Z buffer value
Same as 10 but with normalized (X, Y) coordinates
2 View-transformed (X, Y, Z) coordinates
^p
Modes 0 and 1 convert to the X, Y coordinates, and Z-buffer values of a 2D projection on the screen.
In mode 2, the view is converted to the X, Y, Z coordinate values that take the camera position into consideration.
%href


%index
gppset
Set the physical parameters of the node
%group
Extended screen control command
%prm
objid,prmid,x,y,z
objid (0): Object ID
prmid (0): Parameter ID
x (0.0): Setting parameter X value
y (0.0): Setting parameter Y value
z (0.0): Setting parameter Z value
%inst
Sets the detailed physical parameters of the node specified by objid.
Specify the item to be set with the prmid parameter, and the values specified for X, Y, and Z are applied as the values corresponding to each item.
^p
        Macro name Contents
    ----------------------------------------------------------------------------
GPPSET_ENABLE X = Physical behavior ON / OFF (0 = Invalid)
GPPSET_FRICTION X = coefficient of friction, Y = elasticity setting
GPPSET_DAMPING X = linear repulsion (0-1.0), Y = angular repulsion (0-1.0)
GPPSET_KINEMATIC X = Kinematic setting ON / OFF (0 = invalid)
GPPSET_ANISOTROPIC_FRICTION X, Y, Z = anisotropic friction
GPPSET_GRAVITY X, Y, Z = Gravity
GPPSET_LINEAR_FACTOR X, Y, Z = Vector movement settings
GPPSET_ANGULAR_FACTOR X, Y, Z = Rotation settings along vector
GPPSET_ANGULAR_VELOCITY X, Y, Z = Rotating Velocity
GPPSET_LINEAR_VELOCITY X, Y, Z = Linear Velocity
GPPSET_MASS_CENTER X, Y, Z = Center coordinate offset
^p
Be sure to set the basic physical characteristics with the gppbind instruction before making the detailed settings.
If the settings are incorrect, an error will occur.
%href
gppbind


%index
gpobjpool
Object ID generation settings
%group
Extended screen control command
%prm
start,num
start (0): Assignment start number
num (-1): Maximum number of allocations
%inst
Change the method of assigning the object ID.
The object ID is assigned an unused ID number within a predetermined maximum number.
The maximum number of objects is 1024 by default and can be expanded with the sysreq instruction.
^p
Example:
setreq SYSREQ_MAXOBJ, 4096; Expand maximum number of objects to 4096
^p
The gpobjpool instruction can change the ID number allocation range as desired.
Sets the number specified by the start parameter as the allocation start number.
Sets the number specified by the num parameter to the maximum number that can be assigned.
If the num parameter is negative or omitted, the maximum range that can be taken from the allocation start number is set.
^p
Example:
gpobjpool 100,50; Change object ID generation settings
^p
In the above example, only ID numbers 100 to 150 are assigned.
Subsequent object creation instructions (such as gpbox and gpnull) will only assign object IDs within the specified range.
If you want to limit the number of objects that can be generated to a certain number, or
It can be used when you want to control the display order to some extent.
The gpobjpool instruction cannot extend the preset maximum number of objects.
Be sure to set within the maximum number.
%href
delobj


%index
gppapply
Apply physical force to a node
%group
Extended screen control command
%prm
objid,action,x,y,z
objid (0): Object ID
action (0): Force type (type)
x (0.0): Setting parameter X value
y (0.0): Setting parameter Y value
z (0.0): Setting parameter Z value
%inst
Apply the physical force of the vector specified by (x, y, z) to the node.
The types that can be set with the action parameter are as follows.
^p
        Macro name Contents
    ----------------------------------------------------------------------------
GPPAPPLY_FORCE Add the force to move
GPPAPPLY_IMPULSE Gives a momentary impact
GPPAPPLY_TORQUE Gives torque (twisting) force
GPPAPPLY_TORQUE_IMPULSE Torque + impact
^p
Be sure to apply the force after setting the basic physical characteristics with the gppbind instruction.
If the settings are incorrect, an error will occur.
%href
gppbind


%index
gpmatprm1
Material parameter setting (2)
%group
Extended screen control command
%prm
id,"name",value
id (0): Material ID / Object ID
"name": Parameter name
value (0.0): X setting value (real value)
%inst
Sets the shader parameters for the generated material.
It has basically the same function as the gpmatprm instruction.
The gpmatprm1 instruction specifies a real parameter for only one item.
%href
gpmat
gpmatprm
gpmatprm4
gpmatprm16


%index
gpmatprm4
Material parameter setting (3)
%group
Extended screen control command
%prm
id,"name",x,y,z,w
id (0): Material ID / Object ID
"name": Parameter name
x (0.0): X setting value (real value)
y (0.0): Y setting value (real value)
z (0.0): Z setting value (real value)
w (0.0): W setting value (real value)
%inst
Sets the shader parameters for the generated material.
It has basically the same function as the gpmatprm instruction.
The gpmatprm4 instruction specifies real parameters for 4 items (x, y, z, w).
%href
gpmat
gpmatprm
gpmatprm1
gpmatprm16


%index
setalpha
Object transparency (—¯ value) setting
%group
Extended screen control command
%prm
objid,value
objid (0): Object ID
value (255): Value to be set (integer value) (0 to 255)
%inst
Sets the transparency (—¯ value) of the object.
Sets the value specified by the value parameter to the object of the objid parameter.
The value specified by the value parameter is an integer value (—¯ value) from 0 to 255 that indicates transparency.
0 applies to full transparency and 255 applies opacity.
For objects whose —¯ value is other than 255 (semi-transparent), the drawing order is changed to the front.
(Same state as the mode flag value of OBJ_LATE was specified)
%href
gpsetprm


%index
gpgetlog
Get HGIM G4 error log
%group
Extended screen control command
%prm
var
var: Variable to which the error log is assigned
%inst
Get the HGIMG4 error log and assign it to a variable. The variable is initialized as a string type.
The error log records the details of the error that occurred during execution of the gpreset, gpload instruction, etc., and can be used for investigating the cause.

%href
gpload
gpreset


%index
gpaddanim
Add animation clip
%group
Extended screen control command
%prm
objid,"name",start,end,option
objid (0): Object ID
"name": Animation clip name
start (0): Start frame (milliseconds)
end (-1): end frame (milliseconds)
option (0): Additional options
%inst
Add a new animation clip based on the animation data of the loaded 3D model.
Animated clips are managed by naming and managing some frames cut out from the animation data of a 3D model.
It is necessary to read the model data including animation data from the gpb file in advance by the gpload command.
The "name" parameter specifies the name of the animation clip. You cannot specify a name that has already been added.
Specify the frame range of the animation clip with the start and end parameters. These are time specifications in milliseconds.
If you omit the end parameter or specify a negative value, the last frame of the entire animation is applied.

If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, the system variable stat will be assigned a negative value.

%href
gpgetanim
gpsetanim
gpact


%index
gpact
Play / Stop Animation Clip
%group
Extended screen control command
%prm
objid,"name",option
objid (0): Object ID
"name" (""): Animation clip name
option (1): Playback option
%inst
Controls the play / stop of the animation clip with the specified name.
Specify the object ID of the model in which the animation is set, and select the animation clip with "name".
Animation clips can be set in advance with the gpaddanim instruction. Also, omit the specification of "name", or if it is "" (empty string), it will be the default animation clip.
Playback is controlled by the value of option.
^p
        Macro name value Content
    ----------------------------------------------------------------------------
GPACT_STOP 0 Stop
GPACT_PLAY 1 start
GPACT_PAUSE 2 Pause
^p
Animation playback is controlled for each animation clip.
It is possible to play multiple animation clips for one model at the same time.
Ggpetanim and gpsetanim are provided as commands for acquiring and setting the playback status of animation clips.
If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, the system variable stat will be assigned a negative value.

%href
gpaddanim
gpgetanim
gpsetanim


%index
gpgetanim
Get animation clip settings
%group
Extended screen control command
%prm
var,objid,index,prmid
var: Variable to which information is assigned
objid (0): Object ID
index (0): Index of animation clip (0 ~)
prmid (0): Parameter ID
%inst
Gets the settings for the specified animation clip and assigns it to the variable specified by var.
Specify the index of the animation clip in index and the object ID to get the setting by objid.
The animation clip index is a number that is sequentially assigned to the animation clips held by the object. If an index that does not exist is specified with an integer value starting from 0 such as 0,1,2,3 ..., -1 (error) is assigned to the system variable.
Specifies what information to get with prmid. The values that can be specified with prmid are as follows.
^p
        Macro name value Content
    ----------------------------------------------------------------------------
GPANIM_OPT_START_FRAME 0 Start frame (in milliseconds)
GPANIM_OPT_END_FRAME 1 End frame (in milliseconds)
GPANIM_OPT_DURATION 2 Playback length (in milliseconds)
GPANIM_OPT_ELAPSED 3 Elapsed time (in milliseconds)
GPANIM_OPT_BLEND 4 Blend coefficient (in%)
GPANIM_OPT_PLAYING 5 Playing flag (0 = stop / 1 = play)
GPANIM_OPT_SPEED 6 Playback speed (in%)
GPANIM_OPT_NAME 16 Animation clip name
^p
The variables retrieved are initialized with the appropriate type. In the case of GPANIM_OPT_NAME, it will be a character string type.
If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, the system variable stat will be assigned a negative value.
%href
gpaddanim
gpsetanim


%index
gpsetanim
Updated animation clip settings
%group
Extended screen control command
%prm
objid,index,prmid,value
objid (0): Object ID
index (0): Index of animation clip (0 ~)
prmid (0): Parameter ID
value (0): Set value (integer value)
%inst
Updates the specified animation clip settings with the new value specified by value.
Specify the index of the animation clip in index and the object ID to get the setting by objid.
The animation clip index is a number that is sequentially assigned to the animation clips held by the object. If an index that does not exist is specified with an integer value starting from 0 such as 0,1,2,3 ..., -1 (error) is assigned to the system variable.
Specifies what information to set in prmid. The values that can be specified with prmid are as follows.
^p
        Macro name value Content
    ----------------------------------------------------------------------------
GPANIM_OPT_DURATION 2 Playback length (in milliseconds)
GPANIM_OPT_BLEND 4 Blend coefficient (in%)
GPANIM_OPT_SPEED 6 Playback speed (in%)
^p
If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, the system variable stat will be assigned a negative value.
%href
gpaddanim
gpgetanim


%index
gpmatprm16
Material parameter settings (matrix)
%group
Extended screen control command
%prm
id,"name",var
id (0): Material ID / Object ID
"name": Parameter name
var: Array variable name to which the value is assigned (real type)
count (1): Number of matrices to be set
%inst
Sets the shader parameters for the generated material.
It has basically the same function as the gpmatprm instruction, but the gpmatprm16 instruction sets the numerical value stored in the real number type array variable specified by var as a 4x4 matrix matrix.
The 16 real numbers from var (0) to var (15) form a matrix matrix. If count is specified, the number of values will be fetched from the array variable.
%href
gpmatprm
gpmatprm1
gpmatprm4


%index
gpmatprmt
Material parameter settings (texture)
%group
Extended screen control command
%prm
id,"name","filename"
id (0): Material ID / Object ID
"name": Parameter name
"filename": image file name
opt (0): Material option value
%inst
Sets the shader parameters for the generated material.
It has basically the same function as the gpmatprm instruction, but the gpmatprmt instruction sets the file specified by "filename" as a texture image.
It can be used to pass sampler2D type parameters to the shader.
If GPOBJ_MATOPT_NOMIPMAP is specified in the opt parameter, MIPMAP will not be generated.
If GPOBJ_MATOPT_CUBEMAP is specified in the opt parameter, the texture will be treated as a cubemap.
Cubemaps can be used for environment maps, skyboxes, etc. in a special format that summarizes images in six directions.
(For the cubemap image, use one .PNG format in which 6 images of + X, -X, + Y, -Y, + Z, -Z broadcast are connected vertically.)

%href
gpmatprm
gpmatprm1
gpmatprm4



%index
gpusershader
Specifying a user shader
%group
Extended screen control command
%prm
"vsh","fsh","defs"
"vsh": Vertex shader filename
"fsh": Fragment shader filename
"defs": additional label definitions
%inst
Specifies a shader for the user to set independently.
Vertex shaders, fragment shaders and additional label definitions can be written and used.
The specified shader is referenced when specifying a custom shader for the screen buffer by the buffer instruction as the default value of gpusermat.
%href
buffer
gpusermat


%index
gpgetmat
Get Material ID
%group
Extended screen control command
%prm
var,id,opt
var: Variable to which the material ID is assigned
id (0): Referenced ID
opt (0): Get options
%inst
Gets the ID of the specified object and the custom material used in the screen buffer.
Specify the acquisition option with the opt parameter. The values that can be specified with opt are as follows. ^ p
        Macro name value Content
    ----------------------------------------------------------------------------
GPGETMAT_OPT_OBJMAT 0 Custom material of the object
GPGETMAT_OPT_SCRMAT 1 Custom material in the screen buffer
^p
If GPGETMAT_OPT_OBJMAT is specified, the ID of the custom material that the object has can be obtained by specifying the object ID in the id parameter.
If GPGETMAT_OPT_SCRMAT is specified, the ID of the custom material of each screen buffer can be obtained by specifying the screen buffer ID in the id parameter.


%href
buffer
gpmatprm
gpmatprm1
gpmatprm4
gpmatprm16


%index
setquat
Set quat group information
%group
Extended screen control command
%prm
id,x,y,z,w
id: object ID
(x, y, z, w): Value to set (default = 0)

%inst
Sets the rotation parameters of the object.
Sets the quat group (quaternion) to the value specified by (x, y, z, w).
(x, y, z, w) is set by quaternion (quaternion). Inside the HGIMG4, all rotation information is held by the quaternion.

%href
getquat


%index
getquat
Get quat group information
%group
Extended screen control command
%prm
id,x,y,z,w
id: object ID
(x, y, z, w): Variables to get

%inst
Gets the rotation parameters of the object.
The contents of the quat group (quaternion) are assigned to the variables specified by (x, y, z, w).
(x, y, z, w) is set as a real type variable.
Inside the HGIMG4, all rotation information is held by the quaternion.
Use the getang or getangr instructions to convert to angle information.
However, it may not be possible to completely reproduce the rotation information, so be careful when converting it to angle information.

%href
getang
getangr
setquat



%index
event_suicide
Added object destruction event
%group
Extended screen control command
%prm
p1
p1: Event ID
%inst
The object destruction event is added to the event ID specified by id.
This instruction can be used equivalent to event_delobj.
%href
event_delobj


%index
gpsetprmon
Object core parameter setting (bit addition)
%group
Extended screen control command
%prm
objid,prmid,value
objid (0): Object ID
prmid (0): core parameter ID
value (0): Value to be set (integer value)
%inst
Set the value of the core parameter held for each object to ON only for the bit set by value.
(The value of value is combined with the original core parameter value by OR)
This can be used, for example, to add a new bit to a parameter that needs to be set on a bit-by-bit basis, such as the mode flag value.

The core parameter is a 32-bit integer value that manages various information.
The items that can be specified as the core parameter ID are as follows.
^p
Macro name | Contents
	--------------------------------------------------------------
PRMSET_FLAG Object registration flag (*)
PRMSET_MODE mode flag value
PRMSET_ID Object ID (*)
PRMSET_ALPHA Transparency (—¯ value)
PRMSET_TIMER Timer value
PRMSET_MYGROUP own collision group
PRMSET_COLGROUP Collision group for collision detection
PRMSET_SHAPE Shape ID (*)
PRMSET_USEGPMAT Material ID (*)
PRMSET_COLILOG Collision log ID (*)
PRMSET_FADE fade parameter
PRMSET_SPRID Source buffer ID (sprites only)
PRMSET_SPRCELID Source cell ID (sprites only)
PRMSET_SPRGMODE Copy mode (sprites only)

Items in (*) are read only
^p
%href
gpgetprm
gpsetprm
gpsetprmoff



%index
gpsetprmoff
Object core parameter setting (bit deletion)
%group
Extended screen control command
%prm
objid,prmid,value
objid (0): Object ID
prmid (0): core parameter ID
value (0): Value to be set (integer value)
%inst
Set the value of the core parameter held for each object to OFF only for the bit set by value.
(Erases the bit specified by value from the original core parameter value)
This can be used, for example, to remove a specific bit from a parameter that needs to be set bitwise, such as the mode flag value.

The core parameter is a 32-bit integer value that manages various information.
The items that can be specified as the core parameter ID are as follows.
^p
Macro name | Contents
	--------------------------------------------------------------
PRMSET_FLAG Object registration flag (*)
PRMSET_MODE mode flag value
PRMSET_ID Object ID (*)
PRMSET_ALPHA Transparency (—¯ value)
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
gpsetprm
gpsetprmon



%index
setangy
ang Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
It has the same function as the setang command, but the order of rotation is Y-> X-> Z.

%href
setang
setangz
setangr


%index
setangz
ang Set group information
%group
Extended screen control command
%prm
id,x,y,z
id: object ID
(x, y, z): Value to set (default = 0)

%inst
It has the same function as the setang command, but the order of rotation is Z-> Y-> X.

%href
setang
setangy
setangr


%index
event_angy
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
It has the same function as the event_ang instruction, but the order of rotation is Y-> X-> Z.

%href
event_ang
event_angz
event_angr


%index
event_angz
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
It has the same function as the event_ang instruction, but the order of rotation is Z-> Y-> X.

%href
event_ang
event_angy
event_angr


%index
event_setangy
ang Added group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
It has the same function as the event_setang instruction, but the order of rotation is Y-> X-> Z.

%href
event_setang
event_setangz
event_setangr


%index
event_setangz
ang Added group configuration event
%group
Extended screen control command
%prm
id,x1,y1,z1,x2,y2,z2
id: event ID
(x1, y1, z1): Set value (lower limit)
(x2, y2, z2): Set value (upper limit)
%inst
It has the same function as the event_setang instruction, but the order of rotation is Z-> Y-> X.

%href
event_setang
event_setangy
event_setangr


%index
gpresetlight
Initialization of current light
%group
Extended screen control command
%prm
p1,p2,p3
p1 = 1 to 9 (1): Number of directional lights
p2 = 0 to 9 (0): Number of point lights
p3 = 0-9 (0): Number of spotlights
%inst
Initializes the current light along with the type of light to use.
By setting the current light, you can determine the details of the light received by the models and materials generated after that.
When the scene is initialized, only one directional light can be set. When using multiple lights, point lights, spot lights, etc., the number of current lights must be determined in advance by the gpresetlight command.
The p1, p2, and p3 parameters specify the maximum number of lights used by each light type. You must use at least one directional light.
Specify other point lights and spot lights as needed. Avoid specifying unnecessary lights as much as possible, as this will increase the computational load on the GPU.
After the current light is initialized, it can be registered with the gpuselight command.

%href
gpuselight
gplight


%index
setobjlight
Set current light on object
%group
Extended screen control command
%prm
id
id: object ID
%inst
Set the current light for the object specified by id.
The object specified by id must be a 3D model that performs light source calculations.
The light settings previously set on the object will be discarded and the current light settings will be overwritten.
However, please note that the current light settings (number of directional lights, point lights, spotlights) at the time the 3D model is generated will not be changed. (Be sure to set the number of lights included in the current light to be the same.)
%href
gpuselight
gpresetlight


%index
gppcontact
Create physical collision information for objects
%group
Extended screen control command
%prm
var,objid
var: Variable to which the result is assigned
objid (0): Object ID
%inst
Creates accurate collision information for physically configured 3D objects.
Creates all the information when the 3D object specified by objid collides with another object.
The 3D object specified by objid must be physically set by the gppbind instruction.
In addition, it is necessary to properly set the collision group of the object for which collision is detected by the setcoli instruction.
The number of collision information created is assigned as an integer value to the variable specified by var.
If an error occurs, a negative value will be assigned.
Multiple collision information may be created. This is because multiple objects can collide at the same time. If the variable is assigned 0, it indicates that there are no conflicting objects.
The actual collision information can be obtained by the gppinfo instruction. As collision information, you can obtain the object ID of the collision target, the coordinates of the collision, the strength of the collision, and so on.
Collision information is created for each object ID and is retained until a new gppcontact instruction is executed.
The gppcontact instruction is an instruction to obtain the details of the collision that occurred in the process of physical behavior. If you just want to know a simple collision, you can use the getcoli instruction as an alternative.
In addition, since it is based on the information that it collided when it moved by physical behavior (it was sunk into another object), it is collision information in a state where it is completely stationary (a state where it is just in contact). Please note that is not created.
%href
gppbind
gppinfo
getcoli
setcoli
gppraytest


%index
gppinfo
Get physical collision information for objects
%group
Extended screen control command
%prm
var,fv,objid,index
var: Variable name to which the conflicting object ID value is assigned
fv: variable name to which the FV value is assigned
objid (0): Object ID
index (0): Index value
%inst
Gets the physical collision information of the object created by the gppcontact instruction. Be sure to execute the gppcontact instruction first.
Assigns the physical collision information of the 3D object specified by objid to the variables specified by var and fv. The following contents are stored in the variable.
^p
Variables | What to assign
	--------------------------------------------------------------
var Collision object ID
        fv (0) Collision X coordinate
        fv (1) Collision Y coordinate
        fv (2) Collision Z coordinate
        fv (3) Collision strength
^p
If there is multiple collision information detected by the gppcontact instruction, it is identified by the index value starting from 0.
If there are three collision information, the index value can be 0 to 2.
If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, the system variable stat is assigned a negative value.
%href
gppcontact


%index
gppraytest
Get physical collision information on a vector
%group
Extended screen control command
%prm
var,objid,distance
var: Variable name to which the detected object ID value is assigned
objid (0): Object ID
distance (100): vector length
%inst
Acquires the information of the object that collides with the specified line segment in the 3D object with physical settings.
Investigate the object ID of the first 3D object that collides within the distance specified by distance from the direction the object is facing, starting from the object with the 3D node specified by objid.
The object specified by objid does not need to be physically configured. There is no problem even if it is a camera or a null node.
The result is assigned to the variable specified by var. If any error occurs, a negative value will be assigned.
If there is an object that collides with the line segment, that object ID is assigned. If there are no conflicting objects, 0 is assigned.
If there is a conflict, more detailed information will be stored in the node work values (work, work2) of the object specified by objid.
The nodework value can be obtained by the getwork and getwork2 instructions. The contents set for each are as follows.
^p
Nodework value | What is set
	--------------------------------------------------------------
work X, Y, Z coordinates where the collision occurred
work2 Normal vector of collision surface X, Y, Z value
^p
With the gppraytest instruction, all physically configured objects are subject to collision. Collision group settings are not taken into account.
%href
gppcontact
getcoli
getwork
getwork2


%index
event_fade
Added fade setting event
%group
Extended screen control command
%prm
id,fade
id: event ID
fade (0): set value
%inst
Add an event that sets the fade value of the object.
By specifying a negative value for the fade, the —¯ value can be faded out toward 0, and by specifying a positive value, the —¯ value can be faded in toward 255.
Equivalent to setting the PRMSET_FADE parameter with the event_prmset instruction.
%href
event_prmset


%index
gpmesh
Generate 3D mesh node
%group
Extended screen control command
%prm
var,color,matobj
var: Variable name to which the generated object ID is assigned
color (-1): Material color (24bit RGB value)
matobj (-1): Material ID
%inst
A node object that defines a free shape as a 3D model is generated.
In advance, it is necessary to build model information called a custom 3D mesh using instructions such as gpmeshclear, gpmeshadd, and gpmeshpolygon.
It is not a simple model like gpbox or gpplate, but you can freely define complicated shapes, but since you need to define each vertex coordinate and normal vector, it is a function for advanced users. Please think.
Normally, use the 3D model data prepared as a .gpb file.
^
You can create a custom 3D mesh by following the steps below.
^p
1. Initialize the custom 3D mesh with the gpmeshclear instruction
2. Register the required vertex information with the gpmeshadd instruction.
3. Use the gpmeshpolygon command to combine vertex information to form a triangle or quadrangle face.
4. Repeat registration for only the required aspects
^p
The gpmesh instruction generates a model using all the registered faces based on the information of the created custom 3D mesh.
X, Y, Z coordinates, normal vectors, texture UV coordinates, etc. can be registered in the vertex information. By setting these appropriately, you can define a free shape.
Custom 3D meshes can be redefined as many times as you like to register different model shapes.
^
The ID of the created object is assigned to the variable specified by var.
The generated object can be operated in the same way as the 3D model loaded by the gpload instruction.
In the color parameter, specify the color (material color) of the entire model by the 24-bit RGB value (value indicated by 0xRRGGBB).
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
Specify when using a user-generated material, such as with the gpcolormat command.
If the matobj parameter is omitted, standard materials are used.

%href
gpmeshclear
gpmeshpolygon
gpmeshadd


%index
gpmeshclear
Initialize custom 3D mesh
%group
Extended screen control command
%inst
Initialize the registration information of the custom 3D mesh.
Custom 3D meshes are a mechanism for building data to define free 3D shapes.
The actual data registration uses the gpmeshadd and gpmeshpolygon instructions.
The constructed data can be registered as a node object with the gpmesh instruction.
%href
gpmesh
gpmeshpolygon
gpmeshadd


%index
gpmeshadd
Add vertex information to custom 3D mesh
%group
Extended screen control command
%prm
var,x,y,z,nx,ny,nz,u,v
var: Variable name to which the generated mesh vertex ID is assigned
x (0): X coordinate (real number)
y (0): Y coordinate (real number)
z (0): Z coordinate (real number)
nx (0): normal vector X (real number)
ny (0): Normal vector Y (real number)
nz (0): Normal vector Z (real number)
u (0): Texture vertex coordinates X (real number)
v (0): Texture vertex coordinates Y (real number)
%inst
Add vertex information to the custom 3D mesh.
For vertex information, X, Y, Z coordinates, normal vectors, and texture UV coordinates are registered as one set.
The registered mesh vertex ID is assigned to the variable specified by var. The mesh vertex ID is used when constructing faces (triangles, quadrilaterals).
In the case of vertex information that has already been registered, the mesh vertex ID registered in the past will be assigned and duplicate registration will not be performed. Up to 32767 vertex information can be registered.
^
The (X, Y, Z) coordinates indicate the vertices in 3D space.
The normal vector (X, Y, Z) shows the normalized direction (vector) of the vertices. The normal vector is needed when performing the light source calculation.
Texture vertex coordinates (X (u), Y (v)) specify the coordinates that will be referenced when pasting the texture. It is a normalized value from the origin (0.0, 0.0) to both ends (1.0, 1.0). You will need it when you apply a texture as a material.
^
When defining a custom 3D mesh, it is necessary to clear the registration information with the gpmeshclear instruction first.
To actually register faces (triangles, quadrilaterals) by combining vertex information, use the gpmeshpolygon instruction.
The constructed data can be registered as a node object with the gpmesh instruction.
%href
gpmeshclear
gpmeshpolygon
gpmesh


%index
gpmeshpolygon
Add surface information to custom 3D mesh
%group
Extended screen control command
%prm
p1,p2,p3,p4
p1 (0): Mesh vertex ID1
p2 (0): Mesh vertex ID2
p3 (0): Mesh vertex ID3
p4 (-1): Mesh vertex ID4
%inst
Add face (polygon) information to the custom 3D mesh.
It is necessary to register the necessary vertex information in advance by using the gpmeshadd command.
A face (quadrangle, quadrangle) is constructed by combining mesh vertex IDs that indicate the index of vertex information.
Specify the mesh vertex ID in the p1, p2, p3 parameter for a quadrangle and the p1, p2, p3, p4 parameter for a quadrangle. Arrange the vertices so that the triangles p1, p2, and p3 are arranged clockwise. (For a quadrangle, add the remaining 1 vertex at the end)
^
The constructed data can be registered as a node object with the gpmesh instruction.
%href
gpmeshclear
gpmeshadd
gpmesh


