;
;	HSP help manager HELP source file
;	(Lines starting with ";" are treated as comments)
;
%type
Extended command
%ver
3.7
%note
Include hgimg4.as or hgimg4dx.as.
%date
2024/07/04
%author
onitama
%dll
hgimg4
%url
https://hsp.tv/make/hgimg4.html
%port
Win
%portinfo
Works on systems that support HGIMG4 such as Windows/Linux/Android/iOS.
For Windows, OpenGL 3.1 or later/DirectX 9 is required.

%index
gpreset
HGIMG4 Initialization
%group
Extended Screen Control Commands
%prm
option
option(0) : Initialization option
%inst
Initializes HGIMG4.
Use this when reconstructing the scene.
The content of initialization changes depending on the content set in option.
^p
	Option value  Content
	---------------------------------------------------------------
	    0         Destroys all objects and returns to the initial state
	    1         Destroys only objects existing in the scene
^p
%href


%index
gpdraw
Draw all objects in the scene
%group
Extended Screen Control Commands
%prm
option,debug
option($ffff) : Drawing options
debug(0)      : Debug options
%inst
Draws all objects in the scene.
By specifying the option parameter, you can draw only limited items.
^p
        Macro name                      Content
    -------------------------------------------------------------------------------
	GPDRAW_OPT_OBJUPDATE          Automatic object movement processing/animation processing
	GPDRAW_OPT_DRAWSCENE          3D scene drawing processing
	GPDRAW_OPT_DRAWSCENE_LATE     3D scene drawing processing (OBJ_LATE)
	GPDRAW_OPT_DRAW2D             2D sprite drawing processing
	GPDRAW_OPT_DRAW2D_LATE        2D sprite drawing processing (OBJ_LATE)
^p
If the option parameter is omitted, all items are selected.
Normally, there is no problem even if you do not specify the option parameter.
Also, if you only use 2D direct drawing commands and do not need drawing by objects, you do not need to write the gpdraw command.
Items with the mode flag value of OBJ_LATE added (including those with a translucent óØ value set) are drawn after opaque objects, so the items are divided.
Multiple items can be specified by separating them with "|".
^p
	Example:
	gpdraw GPDRAW_OPT_DRAW2D|GPDRAW_OPT_DRAW2D_LATE
^p
In the above example, only 2D sprite drawing processing and 2D sprite drawing processing (OBJ_LATE) are executed.
By using this, it becomes possible to display with other drawing commands such as the gcopy command between 3D scene drawing and 2D sprite drawing.
You can add drawing for debugging by specifying the debug parameter.
Normally, you do not need to specify the debug parameter.
^p
        Macro name                      Content
    -------------------------------------------------------------------------------
	GPDRAW_OPT_DEBUG_PHY          Debug display of physical objects
^p
GPDRAW_OPT_DEBUG_PHY enables debug display of 3D objects with physical settings.

%href


%index
gpusescene
Scene switching
%group
Extended Screen Control Commands
%prm
id
id(0) : Scene ID to select
%inst
(This is a reserved keyword for future versions. The function is not yet implemented.)
%href


%index
gpsetprm
Set object core parameters
%group
Extended Screen Control Commands
%prm
objid,prmid,value
objid(0) : Object ID
prmid(0) : Core parameter ID
value(0) : Value to set (integer value)
%inst
Overwrites the core parameter held for each object with the value specified by value.
If you want to turn ON/OFF bit by bit instead of overwriting the value, use the gpsetprmon/gpsetprmoff commands.

Core parameters are 32-bit integer values and manage various information.
The following items can be specified as core parameter IDs.
^p
	Macro name             | Content
	--------------------------------------------------------------
	PRMSET_FLAG            Object registration flag (*)
	PRMSET_MODE            Mode flag value
	PRMSET_ID              Object ID (*)
	PRMSET_ALPHA           Transparency (óØ value)
	PRMSET_TIMER           Timer value
	PRMSET_MYGROUP         Own collision group
	PRMSET_COLGROUP        Collision detection collision group
	PRMSET_SHAPE           Shape ID (*)
	PRMSET_USEGPMAT        Material ID
	PRMSET_USEGPPHY        Physical setting ID (*)
	PRMSET_COLILOG         Collision log ID (*)
	PRMSET_FADE            Fade parameters
	PRMSET_SPRID           Source buffer ID (sprite only)
	PRMSET_SPRCELID        Source cell ID (sprite only)
	PRMSET_SPRGMODE        Copy mode (sprite only)

	(*) items are read-only
^p
For example, you can control fade-in/out by changing the PRMSET_FADE parameter.
^p
	Example:
	gpsetprm id, PRMSET_FADE, -4	; Fade out
^p
With the mode flag value set by PRMSET_MODE, you can set the behavior of the object for each bit.
The setobjmode command is also available to set/clear only specific mode flag values.

%href
gpgetprm
gpsetprmon
gpsetprmoff
setobjmode


%index
gpgetprm
Get object core parameters
%group
Extended Screen Control Commands
%prm
var,objid,prmid
var      : Variable name to which the value is assigned
objid(0) : Object ID
prmid(0) : Core parameter ID
%inst
Gets the value of the core parameter held for each object.
Core parameters are 32-bit integer values and manage various information.
The following items can be specified as core parameter IDs.
^p
	Macro name             | Content
	--------------------------------------------------------------
	PRMSET_FLAG            Object registration flag (*)
	PRMSET_MODE            Mode flag value
	PRMSET_ID              Object ID (*)
	PRMSET_ALPHA           Transparency (óØ value)
	PRMSET_TIMER           Timer value
	PRMSET_MYGROUP         Own collision group
	PRMSET_COLGROUP        Collision detection collision group
	PRMSET_SHAPE           Shape ID (*)
	PRMSET_USEGPMAT        Material ID
	PRMSET_COLILOG         Collision log ID (*)
	PRMSET_FADE            Fade parameters
	PRMSET_SPRID           Source buffer ID (sprite only)
	PRMSET_SPRCELID        Source cell ID (sprite only)
	PRMSET_SPRGMODE        Copy mode (sprite only)

	(*) items are read-only
^p
%href
gpsetprm



%index
gppostefx
Post effect generation
%group
Extended Screen Control Commands
%prm
var
var     : Variable name to which the effect ID is assigned
%inst
(This is a reserved keyword for future versions. The function is not yet implemented.)
%href


%index
gpuselight
Register light object
%group
Extended Screen Control Commands
%prm
id,index
id(0) : ID of the light object to register
index=0Å`9(0) : Index of the light object to register
%inst
Registers the specified light object to the current light.
Be sure to specify a light object that has been initialized as a light.
Light objects can be initialized with the gplight command.
Up to 10 directional lights, point lights, and spotlights can be registered for each current light.
In the initial state, only one directional light can be set for the current light. If you want to use multiple lights such as point lights and spotlights, you need to decide the number of current lights in advance with the gpresetlight command.
The index value is used when registering multiple lights in the current light. The first light is 0, the second light is 1, and so on. Up to 10 lights can be set for each type.
By registering with the current light, models and materials created after that will be affected by the current light.
If you want to change the settings of the light already set for the object, use the setobjlight command.

%href
gpresetlight
gplight
gpusecamera
setobjlight


%index
gpusecamera
Switch camera object
%group
Extended Screen Control Commands
%prm
id
id(0) : ID of the camera object to select
%inst
Selects the specified node object as a valid camera object.
Be sure to specify a node object that has been initialized as a camera.
From then on, the scene will be drawn with the switched camera as the viewpoint.
%href
gpuselight


%index
gpmatprm
Set material parameters
%group
Extended Screen Control Commands
%prm
id,"name",x,y,z
id(0)  : Material ID/Object ID
"name" : Parameter name
x(0.0) : X setting value (real number)
y(0.0) : Y setting value (real number)
z(0.0) : Z setting value (real number)
%inst
Sets the shader parameters of the generated material.
If id is an object ID, the material set for the object is set directly.
If id is a material ID, the setting held by the material is changed.
^p
	Example:
	gpmatprm objid,"u_diffuseColor",1,0,0.5 ; Set material parameters
^p
In the above example, a vector value of ( 1.0, 0.0, 0.5 ) is set for the parameter named u_diffuseColor.
(The parameter set with the name u_diffuseColor is passed to the shader.)
The gpmatprm1, gpmatprm2, gpmatprm4, and gpmatprm16 commands are available depending on the number of items to be set in the shader parameter.
Use gpmatprm4 to set four items (x, y, z, w).
Also, gpmatprmt, gpmatprmp commands to pass texture (Sampler) parameters to the shader, and gpmatprmf command to set texture are available.
%href
gpmat
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmt
gpmatprmtf
gpmatprmp


%index
gpmatstate
Set material state
%group
Extended Screen Control Commands
%prm
objid,"name","value"
id(0)   : Material ID/Object ID
"name"  : Setting item name (string)
"value" : Setting value (string)
%inst
Changes the display state setting of the generated material.
Sets the content specified by value for the item specified by name as a string.
Note that item names and settings are all done in strings.
Note that item names and settings are all done in strings. (Item names and setting values are case-insensitive.)
^p
	name             value
    --------------------------------------------------------------------
	"blend"          Enable/disable blending
	                         (Specify true or false)
	"blendSrc"       Select blending source (see below)
	"blendDst"       Select blending destination (see below)
	"cullFace"       Enable/disable hidden surface removal (culling)
	                         (Specify true or false)
	"cullFaceSide"   Specify the face for hidden surface removal (culling)
	"depthTest"      true or false
	                         (Specify true or false)
	"depthWrite"     Enable/disable Z-buffer writing
	                         (Specify true or false)
	"depthFunc"      Select Z-value comparison method (see below)

	(*) Strings that can be set for blendSrc and blendDst

	"ZERO"                     Immediate value (0)
	"ONE"                      Immediate value (1)
	"SRC_COLOR"                Source color
	"ONE_MINUS_SRC_COLOR"      Source color (inverted)
	"DST_COLOR"                Destination color
	"ONE_MINUS_DST_COLOR"      Destination color (inverted)
	"SRC_ALPHA"                Source alpha
	"ONE_MINUS_SRC_ALPHA"      Source alpha (inverted)
	"DST_ALPHA"                Destination alpha
	"ONE_MINUS_DST_ALPHA"      Destination alpha (inverted)
	"CONSTANT_ALPHA"           Fixed alpha value
	"ONE_MINUS_CONSTANT_ALPHA" Fixed alpha value (inverted)
	"SRC_ALPHA_SATURATE"       Inverted source alpha value

	(*) Strings that can be set for cullFaceSide

	BACK             Cull back faces
	FRONT            Cull front faces
	FRONT_AND_BACK   Cull both faces

	(*) Strings that can be set for depthFunc

	NEVER            Always reject
	LESS             Allow only when the value is smaller
	EQUAL            Allow only when the value is the same
	LEQUAL           Allow only when the value is the same or smaller
	GREATER          Allow only when the value is larger
	NOTEQUAL         Allow only when the value is not the same
	GEQUAL           Allow only when the value is the same or larger
	ALWAYS           Always allow
^p
%href


%index
gpviewport
Viewport settings
%group
Extended screen control commands
%prm
x,y,sx,sy
x(0)  : X-coordinate of the viewport origin
y(0)  : Y-coordinate of the viewport origin
sx    : X-size of the viewport
sy    : Y-size of the viewport
%inst
Sets the area (viewport) where HGIMG4 performs drawing.
Drawing is performed on the area specified by (sx, sy) with the screen coordinates (x, y) as the origin. (The coordinates specified by X, Y specify the number of pixels of the entire screen with (0, 0) as the lower left).
If sx and sy are omitted, the current display X and Y sizes are specified as defaults.
Viewport settings
%href
viewcalc
gpcnvaxis


%index
setobjname
Set the node name of an object
%group
Extended screen control commands
%prm
objid,"name"
objid(0) : Object ID
"name"   : Name to be set (string)
%inst
Sets the specified name for the node object.
All node objects can have an arbitrarily specified name.
%href
getobjname


%index
getobjname
Get the node name of an object
%group
Extended screen control commands
%prm
var,objid
var      : Variable name where the result is assigned
objid(0) : Object ID
%inst
Gets the specified name for the node object.
The name is assigned to the variable specified by var as a string type.
All node objects have an arbitrarily specified name set.
%href
setobjname


%index
gpcolormat
Create color material
%group
Extended screen control commands
%prm
var,color,opt
var      : Variable name to which the generated material ID is assigned
color(0) : Material color (24bit RGB value)
opt(0)   : Material option value
%inst
Creates a color material with the specified color.
The color parameter specifies a 24bit RGB value (a value represented by 0xRRGGBB). If omitted, white (0xffffff) is set.
By creating materials independently, detailed texture settings become possible.

^p
	Example:
	gpcolormat mat_id,0xff00ff,GPOBJ_MATOPT_NOLIGHT
	gpbox id_model, 1, , mat_id
^p
In the example above, a cube node object is generated with a material whose RGB color is 0xff00ff (purple) and which does not perform lighting.
The material settings can be changed using the opt parameter.
These settings can also be set separately with the gpmatstate command.
^p
        Macro name                Content
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    Do not perform lighting
	GPOBJ_MATOPT_NOMIPMAP   Do not generate MIPMAP
	GPOBJ_MATOPT_NOCULL     Disable culling
	GPOBJ_MATOPT_NOZTEST    Disable Z-test
	GPOBJ_MATOPT_NOZWRITE   Disable Z-buffer writing
	GPOBJ_MATOPT_BLENDADD   Set the blend mode to addition
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) occurs.
If generated correctly, the material ID (integer value) is assigned to the variable specified by var.

%href
gptexmat
gpusermat


%index
gptexmat
Create texture material
%group
Extended screen control commands
%prm
var,"file",opt
var     : Variable name to which the generated material ID is assigned
"file"  : Texture file name to be read
opt(0)  : Material option value
%inst
Creates a texture (image) material.
By creating materials independently, detailed texture settings become possible.
^p
	Example:
	gptexmat id_texmat, "res/qbox.png"	; Create texture material
	gpbox id_model, 1, , id_texmat		; Add box node
^p
In the example above, a cube node object is generated with a material that has qbox.png in the res folder as a texture.
The material settings can be changed using the opt parameter.
These settings can also be set separately with the gpmatstate command.
^p
        Macro name                Content
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    Do not perform lighting
	GPOBJ_MATOPT_NOMIPMAP   Do not generate MIPMAP
	GPOBJ_MATOPT_NOCULL     Disable culling
	GPOBJ_MATOPT_NOZTEST    Disable Z-test
	GPOBJ_MATOPT_NOZWRITE   Disable Z-buffer writing
	GPOBJ_MATOPT_BLENDADD   Set the blend mode to addition
	GPOBJ_MATOPT_SPECULAR   Apply specular (u_specularExponent) during lighting calculation
	GPOBJ_MATOPT_MIRROR     Display as an inverted image
	GPOBJ_MATOPT_CUBEMAP    Set as a cube map
	GPOBJ_MATOPT_NODISCARD  Disable pixel discarding by óØ channel
	GPOBJ_MATOPT_UVOFFSET	Allow to specify UV offset value (u_textureOffset)
	GPOBJ_MATOPT_UVREPEAT	Allow to specify UV repeat value (u_textureRepeat)
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) occurs.
If generated correctly, the material ID (integer value) is assigned to the variable specified by var.

%href
gpcolormat
gpusermat
gpscrmat


%index
gpscrmat
Create off-screen texture material
%group
Extended screen control commands
%prm
var,id,opt
var     : Variable name to which the generated material ID is assigned
id      : Off-screen buffer ID to be referenced
opt(0)  : Material option value
%inst
Creates a material that references an off-screen texture buffer.
By creating materials independently, it becomes possible to reuse an image rendered off-screen as a texture.
^p
	Example:
	buffer id_render,512,512,screen_offscreen
	gpscrmat id_texmat, id_render, GPOBJ_MATOPT_NOLIGHT|GPOBJ_MATOPT_NOMIPMAP	; Create texture material
	gpbox id_model, 1, , id_texmat		; Add box node
^p
In the example above, a cube node object is generated that references the off-screen texture buffer created by the buffer command.
The material settings can be changed using the opt parameter.
These settings can also be set separately with the gpmatstate command.
^p
        Macro name                Content
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    Do not perform lighting
	GPOBJ_MATOPT_NOMIPMAP   Do not generate MIPMAP
	GPOBJ_MATOPT_NOCULL     Disable culling
	GPOBJ_MATOPT_NOZTEST    Disable Z-test
	GPOBJ_MATOPT_NOZWRITE   Disable Z-buffer writing
	GPOBJ_MATOPT_BLENDADD   Set the blend mode to addition
	GPOBJ_MATOPT_SPECULAR   Apply specular (u_specularExponent) during lighting calculation
	GPOBJ_MATOPT_MIRROR     Display as an inverted image
	GPOBJ_MATOPT_NODISCARD  Disable pixel discarding by óØ channel
^p
If the material cannot be generated normally, error 3 (parameter value is abnormal) occurs.
If generated correctly, the material ID (integer value) is assigned to the variable specified by var.

%href
gpcolormat
gpusermat


%index
gpusermat
Create custom material
%group
Extended screen control commands
%prm
var,"vsh","fsh","defs",color,opt
var       : Variable name to which the generated material ID is assigned
"vsh"     : Vertex shader file name
"fsh"     : Fragment shader file name
"defs"    : Additional label definitions
color(-1) : Material color (24bit RGB value)
opt(0)    : Material option value
%inst
Creates a custom material.
Custom materials perform their own drawing by specifying vertex shaders and fragment shaders.
Specify the shader file name written in OpenGL's shader language (GLSL) for the "vsh" and "fsh" parameters.
In the "defs" parameter, describe the label definitions to be added when compiling the shader.
You can set the default color with the color parameter. If omitted, white (0xffffff) is set.
If the "vsh", "fsh", and "defs" parameters are omitted, the content set by gpusershader is set.
The material settings can be changed using the opt parameter.
These settings can also be set separately with the gpmatstate command.
^p
        Macro name                Content
    -------------------------------------------------------------------
	GPOBJ_MATOPT_NOLIGHT    Do not perform lighting
	GPOBJ_MATOPT_NOMIPMAP   Do not generate MIPMAP
	GPOBJ_MATOPT_NOCULL     Disable culling
	GPOBJ_MATOPT_NOZTEST    Disable Z-test
	GPOBJ_MATOPT_NOZWRITE   Disable Z-buffer writing
	GPOBJ_MATOPT_BLENDADD   Set the blend mode to addition
	GPOBJ_MATOPT_SPECULAR   Apply specular (u_specularExponent) during lighting calculation
	GPOBJ_MATOPT_MIRROR     Display as an inverted image
	GPOBJ_MATOPT_CUBEMAP    Set as a cube map
	GPOBJ_MATOPT_NODISCARD  Disable pixel discarding by óØ channel
^p

If the material cannot be generated successfully, error 3 (parameter value is abnormal) will occur.
If generated correctly, the material ID (integer value) will be assigned to the variable specified by var.

Custom materials allow you to control all rendering using arbitrary shaders.
This is an advanced feature for users with shader knowledge and is not usually required.
When a custom material is generated, you can set the values to be passed to the shader parameters using the gpmatprm instruction.

Creates a new object with the same settings as an existing object.
The ID of the duplicated object is assigned to the variable specified by var.
It has the same shape and parameter settings (excluding physics settings) but allows you to set new coordinates, angles, etc.
This reduces the effort of performing settings and loading multiple times when generating multiple identical objects and also leads to speed increases.

	Example:
	gpload id_model,"res/duck"	; Load model
	gpclone i, id_model		; Clone the same

In the example above, the 3D model node (id_model) from the res/duck.gpb file is cloned, and the new object ID is assigned to the variable i.
If cloning or ID assignment fails, -1 is assigned to the variable specified by var.
You can apply an event to the cloned object by specifying the event ID in the eventid parameter. If eventid is a negative value or omitted, no event is set.
*Please note that in the current version, only displayable objects present in the scene can be cloned.

Creates an empty node object (null node).
The ID of the created object is assigned to the variable specified by var.
Null nodes are used when creating nodes that do not need to have models, such as cameras or lights.
The created node can be freely manipulated in 3D space, just like other nodes.

Generates a 3D model data (.gpb file) as a node.
The ID of the generated object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
Specify the folder and file name in "file". You do not need to specify the extension such as ".gpb".
If you want to load only a node with a specific name in the file, you can specify it with "name".
If the "name" is omitted, the entire hierarchical structure contained in the file is loaded and generated as a node.
Material information is read from the ".material" file. Be sure to prepare it as a set with the ".gpb" file.
If the "defs" parameter is specified, the specified label definitions are added during all shader compilations.

To set the physics settings for a node, use the gppbind instruction.
The contact determination of 3D model data uses a sphere that encloses the entire model (bounding sphere).

The drawing of the generated object (node) is performed collectively by the gpdraw instruction.

	Example:
	gpload id_model,"res/duck"		; Load model

In the example above, the duck.gpb file in the res folder is loaded.
At that time, the duck.material file, which contains material information, is referenced.
Furthermore, if textures are required, image files in the same folder are also loaded. If an error occurs during file loading, a negative value is assigned as the object ID.
To investigate more detailed loading errors or warnings, you can collect logs using the gpgetlog instruction.

The file (.gpb format) to be loaded with the gpload instruction is a data format that is standardly used in gameplay3D.
HGIMG4 provides a GPB converter (gpbconv.exe) to generate the .gpb format.
For details, please refer to the HGIMG4 manual.

Generates a node as 3D model data of a vertically standing plate (rectangle).
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
The X and Y sizes of the model can be specified with the sizex and sizey parameters.
Specify the color of the model (material color) with a 24-bit RGB value (value shown by 0xRRGGBB) with the color parameter.
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
If you are using a material created by the user, such as with the gpcolormat instruction, please specify it.
If the matobj parameter is omitted, the standard material is used.

The drawing of the generated object (node) is performed collectively by the gpdraw instruction.

To set the physics settings for a node, use the gppbind instruction.
The contact determination of the node uses the same shape as the original model.

Generates a node as 3D model data of a single floor (rectangle).
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
The X and Y sizes of the model can be specified with the sizex and sizey parameters.
Specify the color of the model (material color) with a 24-bit RGB value (value shown by 0xRRGGBB) with the color parameter.
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
If you are using a material created by the user, such as with the gpcolormat instruction, please specify it.
If the matobj parameter is omitted, the standard material is used.

The drawing of the generated object (node) is performed collectively by the gpdraw instruction.

To set the physics settings for a node, use the gppbind instruction.
(By default, the floor model has fixed physics properties in the same location.)
The contact determination of the node uses the same shape as the original model.

Generates a cube of the specified size as 3D model data as a node.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated in 3D space.
The X and Y sizes of the model can be specified with the sizex and sizey parameters.
Specify the color of the model (material color) with a 24-bit RGB value (value shown by 0xRRGGBB) with the color parameter.
If the color parameter is omitted, white (0xffffff) is used.
You can specify the material ID with the matobj parameter.
If you are using a material created by the user, such as with the gpcolormat instruction, please specify it.
If the matobj parameter is omitted, the standard material is used.

The drawing of the generated object (node) is performed collectively by the gpdraw instruction.

To set the physics settings for a node, use the gppbind instruction.
The contact determination of the node uses the same shape as the original model.

Generates a node for displaying 2D sprites.
The ID of the created object is assigned to the variable specified by var.
The generated node can be freely manipulated within the 2D screen.
Specify the buffer ID where the display source image is loaded with the bufid parameter, and the cell ID to display with the celid parameter.
Also, specify the copy mode similar to the one specified in the gcopy instruction with the gmode parameter.

	 gmode value  |  Content
	---------------------------------------------------
	    0,1      Alpha channel disabled
	    2        Alpha channel enabled
	    5        Color addition / Alpha channel enabled

The drawing of the generated object (node) is performed collectively by the gpdraw instruction.

	; Drawing for each sprite
	gmode [gmode setting value]
	pos [Node X coordinate],[Node Y coordinate]
	celput [Buffer ID], [Cell ID], [Node X scale], [Node Y scale], [Node Z rotation]
So, you can think of a 2D sprite node as having a feature that pre-registers settings for performing the above processing in a batch.
Also, it is possible to easily perform contact detection between 2D sprite nodes using the `getcoli` command.

%href
gpdraw

%index
gplight
Set up a light node
%group
Extended screen control commands
%prm
id,opt,range,inner,outer
id(0)     : Object ID
opt(0)    : Light generation option
range(1)  : Influence range parameter
inner(0.5): Inner attenuation parameter
outer(1)  : Outer attenuation parameter
%inst
Adds the functionality of a light to an existing node.
Specify the node's object ID with the id parameter.
You can specify the light generation option value with the opt parameter.
^p
	  opt value               |  Content
	--------------------------------------------------------------
	  GPOBJ_LGTOPT_NORMAL    Parallel light (directional light)
	  GPOBJ_LGTOPT_POINT     Point light
	  GPOBJ_LGTOPT_SPOT      Spotlight
^p
The range parameter sets the influence range for point lights and spotlights.
The inner and outer parameters set the inner and outer parameters by which the spotlight is attenuated.
^p
	Example:
	gpnull id_alight			; Create a null node
	gplight id_alight, GPOBJ_LGTOPT_NORMAL	; Register as light
	gpuselight id_alight			; Set to default light
^p
The `gplight` command only sets the light functionality to the object.
The set light is reflected in subsequently created models and materials by registering it as the current light with the `gpuselight` command.

%href
gpresetlight
gpuselight
gpnull

%index
gpcamera
Set up a camera node
%group
Extended screen control commands
%prm
id,fov,aspect,near,far,sw
id(0)      : Object ID
fov(45)    : Field of view (FOV) parameter
aspect(1.5): Aspect ratio
near(0.5)  : Near clipping Z value
far(768)   : Far clipping Z value
sw(0)      : Camera type value (0,1)
%inst
Adds the functionality of a camera to an existing node.
Specify the node's object ID with the id parameter.
Set the field of view (FOV) with the fov parameter. If 45 is specified, the field of view is 45 degrees.
Specify the aspect ratio with the aspect parameter.
The near and far parameters can be used to specify the near and far Z coordinates (Z coordinates used for clipping).
^p
	Example:
	gpnull id_camera			; Create a null node
	gpcamera id_camera, 45, 1.5, 0.5, 768	; Set as camera
	gpusecamera id_camera			; Select the camera to use
	setpos id_camera, 0,20,20		; Set the camera position
^p
By specifying 1 for the camera type value (sw), it is possible to set up a camera that performs orthographic projection. In that case, the fov value is reflected as the zoom value (1.0 is standard).

Cameras placed in the scene can be switched with the `gpusecamera` command.

%href
gpusecamera
gpnull

%index
gplookat
Rotate a node towards the specified coordinates
%group
Extended screen control commands
%prm
objid,x,y,z
objid(0)  : Object ID
x(0)      : Target X coordinate
y(0)      : Target Y coordinate
z(0)      : Target Z coordinate
%inst
Rotates a 3D node toward the specified coordinates.
When applied to a camera, sets the angle to look at the specified coordinates.
%href

%index
gppbind
Set standard physics properties for a node
%group
Extended screen control commands
%prm
objid,mass,friction,option
objid(0)      : Object ID
mass(1)       : Weight
friction(0.5) : Friction coefficient
option(0)     : Setting option
%inst
Performs basic physics settings for node objects.
The mass parameter is the weight of the object. The friction parameter is the friction coefficient. If the values are omitted, the default values are used.
Setting the weight (mass) to 0 sets it as a static rigid body (it collides, but does not move).
Otherwise, it falls to a place with a floor according to the laws of physics.
^
The `gppbind` command only sets basic physical properties. To set detailed physical parameters, use the `gppset` command.
^
*When setting physical behavior, be sure to create the floor (ground) portion.
By setting standard physical properties to the floor generated by the `gpfloor` command, a fixed floor surface can be generated.
If no floor exists, the object will continue to fall forever.
^
After setting the physics properties, the node object will operate independently, and coordinate changes such as `setpos` will be invalid.
To apply force to an object, use the `gppapply` command.
^
Options can be added at the time of setting using the option parameter. You can specify the following macros.
^p
	  opt value               |  Content
	--------------------------------------------------------------

	  GPPBIND_NOSCALE        Do not reflect the node's scale in the collision
	  GPPBIND_MESH           Reflect the node's model as a collision
^p
When a physical setting is made on a node object, information (collision) for performing contact detection is created.

Box nodes, floor nodes, and board nodes handle their respective shapes as collisions. If GPPBIND_NOSCALE is specified in option, the original shape without the scale reflected becomes the collision.
3D model nodes, by default, create a sphere that covers the entire model as a collision. However, if GPPBIND_MESH is specified in option, the shape of the model itself is treated as a collision. However, if a complex model shape is used as a collision, the load on contact detection increases.
Also, when treating the shape of the model as a collision, it is necessary to consider the recessed parts. Recessed parts may not be judged correctly.
The collision is information that is independent of the shape of the model. Note that the scale value of the node is not reflected after the collision is created.

%href
gppapply
gppset

%index
getwork2
Get node work value 2
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variable to get
%inst
Gets the work2 parameter of the object.
(x,y,z) are set as real number variables.
By adding "i" to the end of the command, the value can be obtained as an integer value.
%href
getwork2i

%index
getwork2i
Get node work value 2 (integer value)
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variable to get
%inst
Gets the work2 parameter of the object.
(x,y,z) are set as integer variables.
By using the `getwork2` command, the value can be obtained as a real value.
%href
getwork2

%index
getcolor
Get object color value
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variable to get
%inst
Gets the color parameter of the object.
(x,y,z) are set as real number variables.
By adding "i" to the end of the command, the value can be obtained as an integer value.
%href
getcolori

%index
getcolori
Get object color value (integer value)
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Variable to get
%inst
Gets the color parameter of the object.
(x,y,z) are set as integer variables.
By using the `getcolor` command, the value can be obtained as a real value.
%href
getcolor

%index
setcolor
Set object color value
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)
%inst
Sets the parameters of the object.
Sets the values specified by (x,y,z) in the color group (color value).
Real or integer values can be specified for (x,y,z).
%href
getcolor
addcolor

%index
addcolor
Add object color value
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)
%inst
Adds parameters to the object.
Adds the values specified by (x,y,z) to the color group (color value).
Real or integer values can be specified for (x,y,z).
%href
getcolor
setcolor

%index
selquat
Set object rotation information to MOC information
%group
Extended screen control commands
%prm
id
id(0) : Object ID
%inst
Sets the MOC group targeted by the MOC setting command to quat (quaternion rotation information).
id is the object ID.
%href

%index
selwork2
Set object work2 to MOC information
%group
Extended screen control commands
%prm
id
id(0) : Object ID
%inst
Sets the MOC group targeted by the MOC setting command to work2 (work2).
id is the object ID.
%href

%index
setwork2
Set work2 group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)
%inst
Sets the parameters of the object.
Sets the values specified by (x,y,z) in the work2 group (work2 value).
Real or integer values can be specified for (x,y,z).
%href
setwork

%index
addwork2
Add work2 group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)
%inst
Sets the parameters of the object.
Adds the values specified by (x,y,z) to the work2 group (work2 value).
Real or integer values can be specified for (x,y,z).
%href
addwork

%index
gpcnvaxis
Convert 3D coordinates
%group
Extended screen control commands
%prm
var_x,var_y,var_z,x,y,z,mode
var_x   : Variable to be assigned the X value
var_y   : Variable to be assigned the Y value
var_z   : Variable to be assigned the Z value
x(0.0)  : X value of the conversion source
y(0.0)  : Y value of the conversion source
z(0.0)  : Z value of the conversion source
mode(0) : Conversion mode
%inst
Converts the 3D coordinates of (x,y,z) according to the determined mode.
The result is assigned to the variables specified by var_x, var_y, and var_z as real numbers. (Variable types are automatically set)
The conversion contents by mode value are as follows.
^p
Mode  Content
-----------------------------------------------
0       Drawn 2D coordinate (X, Y) position + Z-buffer value
1       Same as 0, but (X, Y) coordinates are normalized
2       (X, Y, Z) coordinates after view (camera perspective) transformation
3       (X, Y, Z) coordinates after world (camera-independent) transformation
^p
Modes 0 and 1 convert to X, Y coordinates and Z-buffer values when performing 2D projection onto the screen.
In the case of mode 1, the (X, Y) coordinates are normalized to a value between 0 and 1.0. Note that the (X, Y) coordinates calculated in mode 0 are not the coordinates of the entire screen, but are converted to the coordinate system set by gpviewport.
Mode 2 converts to X, Y, and Z coordinate values after performing a view transformation that takes the camera position into account.
Mode 3 converts to X, Y, and Z coordinate values after performing a world transformation that does not take the camera position into account.
%href
gpviewport

%index
gppset
Sets the physical parameters of a node
%group
Extended Screen Control Commands
%prm
objid,prmid,x,y,z
objid(0)  : Object ID
prmid(0)  : Parameter ID
x(0.0)    : Setting parameter X value
y(0.0)    : Setting parameter Y value
z(0.0)    : Setting parameter Z value
%inst
Sets detailed physical parameters for the node specified by objid.
The prmid parameter specifies the item to set, and the values specified in X, Y, and Z are applied as values corresponding to each item.
^p
        Macro Name                          Content
    ----------------------------------------------------------------------------
	GPPSET_ENABLE                     X=ON/OFF of physical behavior (0=disabled)
	GPPSET_FRICTION                   X=Coefficient of friction, Y=Elasticity setting
	GPPSET_DAMPING                    X=Linear repulsion (0~1.0), Y=Angular repulsion (0~1.0)
	GPPSET_KINEMATIC                  X=ON/OFF of Kinematic setting (0=disabled)
	GPPSET_ANISOTROPIC_FRICTION       X,Y,Z=Anisotropic friction
	GPPSET_GRAVITY                    X,Y,Z=Gravity
	GPPSET_LINEAR_FACTOR              X,Y,Z=Settings for movement along a vector
	GPPSET_ANGULAR_FACTOR             X,Y,Z=Settings for rotation along a vector
	GPPSET_ANGULAR_VELOCITY           X,Y,Z=Angular velocity
	GPPSET_LINEAR_VELOCITY            X,Y,Z=Linear velocity
	GPPSET_MASS_CENTER                X,Y,Z=Center coordinate offset
^p
Be sure to set basic physical properties with the gppbind command before making detailed settings.
An error will occur if the setting value is incorrect.
%href
gppbind

%index
gpobjpool
Object ID generation settings
%group
Extended Screen Control Commands
%prm
start,num
start(0) : Allocation start number
num(-1)  : Maximum number of allocations
%inst
Changes the object ID allocation method.
Object IDs are assigned unused ID numbers within a predetermined maximum number.
The maximum number of objects is 1024 by default and can be extended by the sysreq command.
^p
	Example:
	setreq SYSREQ_MAXOBJ,4096	; Expand the maximum number of objects to 4096
^p
The gpobjpool command allows you to arbitrarily change the ID number allocation range.
The number specified by the start parameter is set as the allocation start number.
The number specified by the num parameter is set as the maximum number that can be allocated.
If the num parameter is a negative value or is omitted, the maximum range that can be taken from the allocation start number is set.
^p
	Example:
	gpobjpool 100,50	; Change the object ID generation settings
^p
In the above example, only ID numbers 100 to 150 are changed to the allocation target.
In subsequent object creation commands (such as gpbox or gpnull), only object IDs within the specified range are assigned.
This can be used to limit the number of objects generated to a specific number within the entire object range, or to control the display order to some extent.
The gpobjpool command cannot extend the pre-set maximum number of objects.
Be sure to set the value within the range of the maximum number.
%href
delobj

%index
gppapply
Applies a physical force to a node
%group
Extended Screen Control Commands
%prm
objid,action,x,y,z
objid(0)  : Object ID
action(0) : Type of force
x(0.0)    : Setting parameter X value
y(0.0)    : Setting parameter Y value
z(0.0)    : Setting parameter Z value
%inst
Applies a physical force with a vector specified by (x, y, z) to the node.
The types that can be set with the action parameter are as follows:
^p
        Macro Name                     Content
    ----------------------------------------------------------------------------
	GPPAPPLY_FORCE               Adds a force to move
	GPPAPPLY_IMPULSE             Applies an instantaneous impact
	GPPAPPLY_TORQUE              Applies a torque (twisting) force
	GPPAPPLY_TORQUE_IMPULSE      Applies torque + impact
	GPPAPPLY_FORCE_POS           Adds a force to move (with coordinate specification)
	GPPAPPLY_IMPULSE_POS         Applies an instantaneous impact (with coordinate specification)
^p
If GPPAPPLY_FORCE_POS or GPPAPPLY_IMPULSE_POS is specified, the WORK value of the model specified by the object ID (set by the setwork command, etc.) is used as the coordinate value to apply the force.
Be sure to set basic physical properties with the gppbind command before applying force with this command.
An error will occur if the setting value is incorrect.
%href
gppbind

%index
gpmatprm1
Material parameter setting (1)
%group
Extended Screen Control Commands
%prm
id,"name",value
id(0)      : Material ID/Object ID
"name"     : Parameter name
value(0.0) : X setting value (real number)
%inst
Sets the shader parameters of the generated material.
This command has basically the same functionality as the gpmatprm command.
The gpmatprm1 command specifies a real number parameter for only one item.
%href
gpmat
gpmatprm
gpmatprm2
gpmatprm4
gpmatprm16

%index
gpmatprm2
Material parameter setting (2)
%group
Extended Screen Control Commands
%prm
id,"name",value,value2
id(0)      : Material ID/Object ID
"name"     : Parameter name
value(0.0) : X setting value (real number)
value2(0.0) : Y setting value (real number)
%inst
Sets the shader parameters of the generated material.
This command has basically the same functionality as the gpmatprm command.
The gpmatprm2 command specifies a real number parameter for two items.
%href
gpmat
gpmatprm
gpmatprm1
gpmatprm4
gpmatprm16

%index
gpmatprm4
Material parameter setting (4)
%group
Extended Screen Control Commands
%prm
id,"name",x,y,z,w
id(0)  : Material ID/Object ID
"name" : Parameter name
x(0.0) : X setting value (real number)
y(0.0) : Y setting value (real number)
z(0.0) : Z setting value (real number)
w(0.0) : W setting value (real number)
%inst
Sets the shader parameters of the generated material.
This command has basically the same functionality as the gpmatprm command.
The gpmatprm4 command specifies a real number parameter for 4 items (x, y, z, w).
%href
gpmat
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm16

%index
setalpha
Object transparency (óØ value) setting
%group
Extended Screen Control Commands
%prm
objid,value
objid(0)   : Object ID
value(255) : Value to set (integer value) (0-255)
%inst
Sets the transparency (óØ value) of an object.
Sets the value specified by the value parameter to the object of the objid parameter.
The value specified by the value parameter is an integer value (óØ value) from 0 to 255 indicating transparency.
0 is completely transparent, and 255 is opaque.
For objects with an óØ value other than 255 (semi-transparent), the drawing order is changed to the front.
(Same state as when the OBJ_LATE mode flag value is specified)
%href
gpsetprm

%index
gpgetlog
Get HGIMG4 error log
%group
Extended Screen Control Commands
%prm
var
var   : Variable to which the error log is assigned
%inst
Gets the HGIMG4 error log and assigns it to a variable. The variable is initialized as a string type.
The error log records details of errors that occurred during the execution of commands such as gpreset and gpload, and can be used to investigate the cause.

%href
gpload
gpreset

%index
gpaddanim
Add animation clip
%group
Extended Screen Control Commands
%prm
objid,"name",start,end,option
objid(0)  : Object ID
"name"    : Animation clip name
start(0)  : Start frame (milliseconds)
end(-1)   : End frame (milliseconds)
option(0) : Additional options
%inst
Adds a new animation clip based on the animation data of the loaded 3D model.
Animation clips are used to manage a portion of frames extracted from the animation data of a 3D model with a name.
It is necessary to load model data including animation data from a gpb file in advance using the gpload command.
Specify the name of the animation clip with the "name" parameter. You cannot specify a name that has already been added.
Specify the frame range of the animation clip with the start and end parameters. These are time specifications in milliseconds.
If the end parameter is omitted or a negative value is specified, the last frame of the entire animation is applied.

If the process is completed successfully, 0 is assigned to the system variable stat. If an error occurs, a negative value is assigned to the system variable stat.

%href
gpgetanim
gpsetanim
gpact

%index
gpact
Play/stop animation clip
%group
Extended Screen Control Commands
%prm
objid,"name",option
objid(0)  : Object ID
"name"(""): Animation clip name
option(1) : Playback options
%inst
Controls the playback/stop of the animation clip with the specified name.
Specify the object ID of the model to which the animation is set, and select the animation clip with "name".
Animation clips can be set in advance with the gpaddanim command. If the specification of "name" is omitted, or "" (empty string) is specified, the default animation clip is selected.
The playback is controlled with the value of option.
^p
        Macro Name              Value          Content
    ----------------------------------------------------------------------------
	GPACT_STOP            0           Stop
	GPACT_PLAY            1           Start
	GPACT_PAUSE           2           Pause
^p
Animation playback is controlled for each animation clip.
Multiple animation clips can be played simultaneously for one model.
gpgetanim and gpsetanim are provided as commands to get and set the state of animation clip playback.
If the process is completed successfully, 0 is assigned to the system variable stat. If an error occurs, a negative value is assigned to the system variable stat.
%href
gpaddanim
gpgetanim
gpsetanim

%index
gpgetanim
Get animation clip settings
%group
Extended screen control commands
%prm
var,objid,index,prmid
var   : Variable to store the information
objid(0) : Object ID
index(0) : Animation clip index (0~)
prmid(0) : Parameter ID
%inst
Gets the settings of the specified animation clip and assigns it to the variable specified by var.
Specify the object ID to get the settings from with objid, and the animation clip index with index.
The animation clip index is a number assigned sequentially to the animation clips held by the object. It is an integer value starting from 0, such as 0,1,2,3... If a non-existent index is specified, the system variable is assigned -1 (error).
Specify what kind of information to get with prmid. The values that can be specified with prmid are as follows:
^p
        Macro Name                   Value          Content
    ----------------------------------------------------------------------------
	GPANIM_OPT_START_FRAME     0           Start frame (in milliseconds)
	GPANIM_OPT_END_FRAME       1           End frame (in milliseconds)
	GPANIM_OPT_DURATION        2           Playback length (in milliseconds)
	GPANIM_OPT_ELAPSED         3           Elapsed time (in milliseconds)
	GPANIM_OPT_BLEND           4           Blend factor (in %)
	GPANIM_OPT_PLAYING         5           Playing flag (0=stopped/1=playing)
	GPANIM_OPT_SPEED           6           Playback speed (in %)
	GPANIM_OPT_NAME            16          Animation clip name
^p
The variable to be obtained is initialized with an appropriate type. In the case of GPANIM_OPT_NAME, it will be a string type.
If the process completes normally, the system variable stat is assigned 0. If an error occurs, the system variable stat is assigned a negative value.
%href
gpaddanim
gpsetanim

%index
gpsetanim
Update animation clip settings
%group
Extended screen control commands
%prm
objid,index,prmid,value
objid(0) : Object ID
index(0) : Animation clip index (0~)
prmid(0) : Parameter ID
value(0) : Value to be set (integer value)
%inst
Updates the settings of the specified animation clip with the new value specified by value.
Specify the object ID to get the settings from with objid, and the animation clip index with index.
The animation clip index is a number assigned sequentially to the animation clips held by the object. It is an integer value starting from 0, such as 0,1,2,3... If a non-existent index is specified, the system variable is assigned -1 (error).
Specify what kind of information to set with prmid. The values that can be specified with prmid are as follows:
^p
        Macro Name                   Value          Content
    ----------------------------------------------------------------------------
	GPANIM_OPT_DURATION        2           Playback length (in milliseconds)
	GPANIM_OPT_BLEND           4           Blend factor (in %)
	GPANIM_OPT_SPEED           6           Playback speed (in %)
^p
If the process completes normally, the system variable stat is assigned 0. If an error occurs, the system variable stat is assigned a negative value.
%href
gpaddanim
gpgetanim

%index
gpmatprm16
Material parameter settings (matrix)
%group
Extended screen control commands
%prm
id,"name",var
id(0)      : Material ID/Object ID
"name"     : Parameter name
var        : Array variable name with the value assigned (real number type)
count(1)   : Number of matrices to set
%inst
Sets the shader parameters of the generated material.
gpmatprm16 command has basically the same function as the gpmatprm command, but gpmatprm16 sets the numerical values stored in the real number type array variable specified by var as a 4x4 matrix.
16 real numbers from var(0) to var(15) become a matrix. If count is specified, values are taken from the array variable by that number.
When setting the number of matrices to 2 or more, be sure to specify the material ID.
Also, please note that in the current version, there is a limit of only one type of two or more matrices per material. If the number of matrices is 1, you can set the parameters without restrictions.
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4

%index
gpmatprmt
Material parameter settings (texture file)
%group
Extended screen control commands
%prm
id,"name","filename"
id(0)      : Material ID/Object ID
"name"     : Parameter name
"filename" : Image file name
opt(0)     : Material option value
%inst
Sets the shader parameters of the generated material.
The gpmatprmt command sets the file specified by "filename" as a texture image to the shader parameter.
Use this when changing a material to which a texture has already been assigned.
This command can be used when passing a sampler2D type parameter to the shader. This makes it possible to handle multi-textures with your own shader.
If the parameter name is omitted or an empty string (""), the standard shader parameter name "u_diffuseTexture" of HGIMG4 is used.
If GPOBJ_MATOPT_NOMIPMAP is specified for the opt parameter, MIPMAP will not be generated.
Also, if GPOBJ_MATOPT_CUBEMAP is specified for the opt parameter, the texture is treated as a cube map.
A cube map is a special format that combines images from 6 faces and can be used for environment maps and skyboxes.
(For cube map images, use a single .PNG format file with 6 images of +X,-X,+Y,-Y,+Z,-Z broadcasting vertically connected.)
If you want to set parameters using a material ID that has already been loaded, use the gpmatprmp command.
Also, the gpmatprmf command is available for setting textures.
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmp
gpmatprmf

%index
gpmatprmp
Material parameter settings (material ID)
%group
Extended screen control commands
%prm
id,"name",matobj
id(0)      : Material ID/Object ID
"name"     : Parameter name
matobj(0)  : Material ID
%inst
Sets the shader parameters of the generated material.
The gpmatprmp command sets the texture that the material ID specified by matobj has as a shader parameter.
This command can be used when passing a sampler2D type parameter to the shader. This makes it possible to handle multi-textures with your own shader.
If the parameter name is omitted or an empty string (""), the standard shader parameter name "u_diffuseTexture" of HGIMG4 is used.
Instead of the material ID, you can specify the screen buffer ID registered as an off-screen. By specifying it in the form of "GPOBJ_ID_SRCFLAG+screen buffer ID", you can pass the screen buffer as a texture.

To set parameters using a texture file, use the gpmatprmt command.
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmt
gpmatprmf

%index
gpusershader
User shader specification
%group
Extended screen control commands
%prm
"vsh","fsh","defs"
"vsh"     : Vertex shader file name
"fsh"     : Fragment shader file name
"defs"    : Additional label definitions
%inst
Specifies a shader for users to set up on their own.
You can write and use vertex shaders, fragment shaders, and additional label definitions.
The specified shader is referenced as the default value of gpusermat when specifying a custom shader for the screen buffer with the buffer command.
%href
buffer
gpusermat

%index
gpgetmat
Get Material ID
%group
Extended screen control commands
%prm
var,id,opt
var    : Variable to store the Material ID
id(0)  : Referenced ID
opt(0) : Get option
%inst
Gets the custom material ID used by the specified object and screen buffer.
Specify the get option with the opt parameter. The values that can be specified with opt are as follows:
^p
    Macro Name                   Value     Content
    ----------------------------------------------------------------------------
	GPGETMAT_OPT_OBJMAT        0      Custom material of the object
	GPGETMAT_OPT_SCRMAT        1      Custom material of the screen buffer
^p
If GPGETMAT_OPT_OBJMAT is specified, you can specify the object ID in the id parameter to get the custom material ID of the object.
If GPGETMAT_OPT_SCRMAT is specified, you can specify the screen buffer ID (window ID) in the id parameter to get the custom material ID of each screen buffer.

%href
buffer
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16

%index
gpmatprmf
Material texture detail settings
%group
Extended screen control commands
%prm
id,"name","value","prmname"
id(0)       : Material ID/Object ID
"name"("")  : Setting name
"value"("") : Value to set
"prmname"("") : Parameter name
%inst
Changes the texture settings set in the material.
You can change the filter mode and texture wrap mode by specifying the material ID or object ID for which the texture has already been set.
Specify the material ID or object ID to perform texture settings in the id parameter. If the material ID or object ID is incorrect, or if you specify a material that does not have a texture set, an error will occur.
Set the value specified by "value" to the setting name specified by "name". These specifications are done with strings.
The following strings are recognized for the settings. (Uppercase and lowercase are treated the same)
^p
    Setting Name           Value          Content
    ----------------------------------------------------------------------------
    filter           nearest     Set filter mode to NEAREST
    filter           linear      Set filter mode to LINEAR
    warp             repeat      Set wrap mode to REPEAT
    warp             clamp       Set wrap mode to CLAMP
^p
filter: This setting configures the texture scaling filter. "nearest" uses the closest pixel, while "linear" applies linear interpolation.
wrap: This setting configures how texture UV values outside the 0-1 range are handled. "repeat" repeats the texture, while "clamp" clips it.
^
"prmname" can be used to specify the shader parameter name. If the parameter name is omitted or set to an empty string (""), the standard HGIMG4 shader parameter name "u_diffuseTexture" will be used.
If the settings are applied successfully, the system variable will be set to 0. If any problems occur and the settings are not applied, a negative value will be assigned.
Use the gpmatstate command to set material state information.
%href
gpmatprm
gpmatprm1
gpmatprm2
gpmatprm4
gpmatprm16
gpmatprmp
gpmatprmf
gpmatstate


%index
setquat
Set quaternion group information
%group
Extended screen control commands
%prm
id,x,y,z,w
id      : Object ID
(x,y,z,w) : Value to set (default=0)

%inst
Sets the rotation parameters of an object.
Sets the value specified by (x,y,z,w) to the quaternion group.
(x,y,z,w) performs the setting using quaternions. HGIMG4 internally stores all rotation information as quaternions.

%href
getquat


%index
getquat
Get quaternion group information
%group
Extended screen control commands
%prm
id,x,y,z,w
id      : Object ID
(x,y,z,w) : Variable to get

%inst
Gets the rotation parameters of an object.
The contents of the quaternion group are assigned to the variables specified by (x,y,z,w).
(x,y,z,w) are set as real number variables.
HGIMG4 internally stores all rotation information as quaternions.
Use the getang or getangr command to convert to angle information.
However, note that rotation information may not be fully reproduced when converting to angle information.

%href
getang
getangr
setquat



%index
event_suicide
Add object destruction event
%group
Extended screen control commands
%prm
p1
p1    : Event ID
%inst
*This command is scheduled to be deprecated in the future. Please replace it with event_delobj.
Adds an object destruction event to the event ID specified by id.
This command can be used in the same way as event_delobj.
%href
event_delobj


%index
gpsetprmon
Set object core parameter (bit addition)
%group
Extended screen control commands
%prm
objid,prmid,value
objid(0) : Object ID
prmid(0) : Core parameter ID
value(0) : Value to set (integer value)
%inst
Sets only the bits set in value to ON for the core parameter value held by each object.
(The value of value is ORed with the original core parameter value)
This can be used when adding new bits to parameters that need to be set in bit units, such as mode flag values.

Core parameters are 32-bit integer values that manage various information.
The following items can be specified as core parameter IDs:
^p
	Macro name             | Contents
	--------------------------------------------------------------
	PRMSET_FLAG            Object registration flag (*)
	PRMSET_MODE            Mode flag value
	PRMSET_ID              Object ID (*)
	PRMSET_ALPHA           Transparency (alpha value)
	PRMSET_TIMER           Timer value
	PRMSET_MYGROUP         Object's collision group
	PRMSET_COLGROUP        Collision group to detect collisions with
	PRMSET_SHAPE           Shape ID (*)
	PRMSET_USEGPMAT        Material ID
	PRMSET_USEGPPHY        Physics settings ID (*)
	PRMSET_COLILOG         Collision log ID (*)
	PRMSET_FADE            Fade parameters
	PRMSET_SPRID           Source buffer ID (sprites only)
	PRMSET_SPRCELID        Source cell ID (sprites only)
	PRMSET_SPRGMODE        Copy mode (sprites only)

	(*) Items are read-only
^p
%href
gpgetprm
gpsetprm
gpsetprmoff



%index
gpsetprmoff
Set object core parameter (bit deletion)
%group
Extended screen control commands
%prm
objid,prmid,value
objid(0) : Object ID
prmid(0) : Core parameter ID
value(0) : Value to set (integer value)
%inst
Sets only the bits set in value to OFF for the core parameter value held by each object.
(The bits specified by value are deleted from the original core parameter value)
This can be used to delete specific bits from parameters that need to be set in bit units, such as mode flag values.

Core parameters are 32-bit integer values that manage various information.
The following items can be specified as core parameter IDs:
^p
	Macro name             | Contents
	--------------------------------------------------------------
	PRMSET_FLAG            Object registration flag (*)
	PRMSET_MODE            Mode flag value
	PRMSET_ID              Object ID (*)
	PRMSET_ALPHA           Transparency (alpha value)
	PRMSET_TIMER           Timer value
	PRMSET_MYGROUP         Object's collision group
	PRMSET_COLGROUP        Collision group to detect collisions with
	PRMSET_SHAPE           Shape ID (*)
	PRMSET_USEGPMAT        Material ID
	PRMSET_USEGPPHY        Physics settings ID (*)
	PRMSET_COLILOG         Collision log ID (*)
	PRMSET_FADE            Fade parameters
	PRMSET_SPRID           Source buffer ID (sprites only)
	PRMSET_SPRCELID        Source cell ID (sprites only)
	PRMSET_SPRGMODE        Copy mode (sprites only)

	(*) Items are read-only
^p
%href
gpgetprm
gpsetprm
gpsetprmon



%index
setangy
Set ang group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)

%inst
Has the same function as the setang command, but the rotation order is Y->X->Z.

%href
setang
setangz
setangr


%index
setangz
Set ang group information
%group
Extended screen control commands
%prm
id,x,y,z
id      : Object ID
(x,y,z) : Value to set (default=0)

%inst
Has the same function as the setang command, but the rotation order is Z->Y->X.

%href
setang
setangy
setangr


%index
event_angy
Add ang group change event
%group
Extended screen control commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Value to set
sw(0)      : Interpolation option

%inst
Has the same function as the event_ang command, but the rotation order is Y->X->Z.

%href
event_ang
event_angz
event_angr


%index
event_angz
Add ang group change event
%group
Extended screen control commands
%prm
id,frame,x1,y1,z1,sw
id         : Event ID
frame      : Number of frames until change
(x1,y1,z1) : Value to set
sw(0)      : Interpolation option

%inst
Has the same function as the event_ang command, but the rotation order is Z->Y->X.

%href
event_ang
event_angy
event_angr


%index
event_setangy
Add ang group setting event
%group
Extended screen control commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to set (lower limit)
(x2,y2,z2) : Value to set (upper limit)
%inst
Has the same function as the event_setang command, but the rotation order is Y->X->Z.

%href
event_setang
event_setangz
event_setangr


%index
event_setangz
Add ang group setting event
%group
Extended screen control commands
%prm
id,x1,y1,z1,x2,y2,z2
id         : Event ID
(x1,y1,z1) : Value to set (lower limit)
(x2,y2,z2) : Value to set (upper limit)
%inst
Has the same function as the event_setang command, but the rotation order is Z->Y->X.

%href
event_setang
event_setangy
event_setangr


%index
gpresetlight
Initialize current light
%group
Extended screen control commands
%prm
p1,p2,p3
p1=1Å`9(1) : Number of directional lights
p2=0Å`9(0) : Number of point lights
p3=0Å`9(0) : Number of spotlights
%inst
Initializes the current light along with the type of light to use.
By setting the current light, you can determine the details of the light received by models and materials generated thereafter.
In the initial state of the scene, only one directional light can be set. If you want to use multiple lights, point lights, or spotlights, you need to determine the number of current lights in advance using the gpresetlight command.
The p1, p2, and p3 parameters specify the maximum number of lights to use for each light type. You must use at least one directional light.
Specify the other point lights and spotlights as needed. Avoid specifying unnecessary lights as much as possible because they can increase the GPU's computational load.
After the current light is initialized, it can be registered using the gpuselight command.

%href
gpuselight
gplight


%index
setobjlight
Set current light to object
%group
Extended screen control commands
%prm
id
id      : Object ID
%inst
Sets the current light settings for the object specified by id.
The object specified by id must be a 3D model that performs lighting calculations.
The light settings previously set for the object are discarded and the current light settings are overwritten.
However, note that the current light settings (number of directional lights, point lights, and spotlights) at the time the 3D model was generated are not changed. (Be sure to set the same number of lights included in the current light.)
%href
gpuselight
gpresetlight


%index
gppcontact
Create object physical collision information
%group
Extended screen control commands
%prm
var,objid
var      : Variable to assign result to
objid(0) : Object ID
%inst
Creates accurate collision information for a 3D object with physics settings.
Creates all collision information for the 3D object specified by objid when it collides with other objects.
The 3D object specified by objid must be physically set using the gppbind command.
Also, the collision group of the objects to be detected for collision must be properly set using the setcoli command.
The variable specified by var is assigned the number of collision information created as an integer value.
A negative value is assigned if an error occurs.
Multiple collision information may be created. This is because it may collide with multiple objects at the same time. If 0 is assigned to the variable, it indicates that there are no colliding objects.
The actual collision information can be obtained using the gppinfo command. Collision information includes the object ID of the collision target, the collision coordinates, and the collision strength.
Collision information is created for each object ID and is retained until a new gppcontact command is executed.
The gppcontact command is a command to obtain the details of collisions that occurred during physical behavior. If you only need to know about simple collisions, you can use the getcoli command as an alternative.
Also, note that collision information is not created in a completely motionless state (a state where they are simply touching), as it is based on the information that they collided (penetrated other objects) when moving due to physical behavior.
%href
gppbind
gppinfo
getcoli
setcoli
gppraytest
gppsweeptest


%index
gppinfo
Get physical collision information of an object
%group
Extended screen control commands
%prm
fv,var,objid,index
fv       : Variable name to which the FV value is assigned
var      : Variable name to which the collided object ID value is assigned
objid(0) : Object ID
index(0) : Index value
%inst
Gets the physical collision information of an object created by the gppcontact command. The gppcontact command must be executed first.
Assigns the physical collision information of the 3D object specified by objid to the variable specified by var and fv. The following contents are stored in the variables.
^p
    Variable                 | Assigned content
    --------------------------------------------------------------
	var                    Object ID of the collision
    fv(0)                  X coordinate of the collision
    fv(1)                  Y coordinate of the collision
    fv(2)                  Z coordinate of the collision
    fv(3)                  Strength of the collision (not implemented)
^p
If there is more than one collision information detected by the gppcontact command, it is identified by the index value starting from 0.
If there are three collision information, the index values 0 to 2 can be specified.
If the process completes normally, 0 is assigned to the system variable stat. If an error occurs, a negative value is assigned to the system variable stat.
^
Please note that the collision strength is not assigned when gppcontact is running in the current version.

%href
gppcontact


%index
gppraytest
Get physical collision information on a vector
%group
Extended screen control commands
%prm
var,objid,distance,group
var      : Variable name to which the detected object ID value is assigned
objid(0)      : Object ID
distance(100) : Length of the vector
group(0)      : Target collision group (0=all)
%inst
Acquires information about an object that collides with a specified line (vector) among 3D objects that have been physically configured.
Starting from the object with the 3D node specified by objid as the base point, the object ID of the first 3D object that collides within the distance specified by distance from the direction in which the object is facing is investigated.
The object specified by objid does not need to be physically configured. It can be a camera or null node, etc.
The group parameter allows you to specify the collision group to be targeted. Please note that the collision settings set for the object specified by objid are not referenced.
If the group parameter is omitted or 0 is specified, all physically configured objects are targeted.
^
The result is assigned to the variable specified by var. If any error occurs, a negative value is assigned.
If there is an object that collides with the line segment, its object ID is assigned. If there is no object that collides, 0 is assigned.
If there is a collision, more detailed information is stored in the node work values (work, work2) of the object specified by objid.
The node work values can be obtained using the getwork and getwork2 commands. The contents set for each are as follows.
^p
	Node work value       | Setting content
	--------------------------------------------------------------
	work                   X, Y, Z coordinates where the collision occurred
	work2                  Normal vector X, Y, Z values of the colliding surface
^p
Collision with plate nodes (generated by gpplate) may not be correctly determined, so use thick objects (such as boxes) when performing collision tests.
If you want to test collision with a 3D model instead of a simple line (vector), you can investigate it using the gppsweeptest command.
%href
gppcontact
getcoli
getwork
getwork2
gppsweeptest


%index
gppsweeptest
Get physical collision information when moving a model in a specified vector
%group
Extended screen control commands
%prm
var,objid,x,y,z,group
var     : Variable name to which the detected object ID value is assigned
objid(0): Object ID
x(0)    : X coordinate (real number)
y(0)    : Y coordinate (real number)
z(0)    : Z coordinate (real number)
group(0): Collision group application switch (0=none)
%inst
Starting from the current position of the object with the 3D node specified by the objid parameter as the base point, the object ID of the first 3D object that collides when moved to the coordinates specified by (X,Y,Z) is investigated.
Collision detection is tested based on the physical information set for the object specified by the objid parameter (it is not actually moved).
The group parameter sets whether to consider the collision group to be targeted. If the group parameter is omitted or a value other than 0 is set, the collision group specified by the setcoli command is considered. If 0 is specified, all physically configured objects are targeted.
^
The result is assigned to the variable specified by var. If any error occurs, a negative value is assigned.
If there is an object that collides with the line segment, its object ID is assigned. If there is no object that collides, 0 is assigned.
If there is a collision, more detailed information is stored in the node work values (work, work2) of the object specified by objid.
The node work values can be obtained using the getwork and getwork2 commands. The contents set for each are as follows.
^p
	Node work value       | Setting content
	--------------------------------------------------------------
	work                   X, Y, Z coordinates where the collision occurred
	work2                  Normal vector X, Y, Z values of the colliding surface
^p
Collision with plate nodes (generated by gpplate) may not be correctly determined, so use thick objects (such as boxes) when performing collision tests.
If you want to know whether it collides with a simple line (vector) instead of a model, you can investigate it using the gppraytest command.
%href
gppcontact
setcoli
getcoli
getwork
getwork2
gppraytest


%index
event_fade
Add a fade setting event
%group
Extended screen control commands
%prm
id,fade
id         : Event ID
fade(0)    : Value to be set
%inst
Adds an event to set the fade value of an object.
By specifying a negative value for the fade, the alpha value can be faded out towards 0, and by specifying a positive value, the alpha value can be faded in towards 255.
This is equivalent to setting the PRMSET_FADE parameter with the event_prmset command.
%href
event_prmset


%index
gpmesh
Generate a 3D mesh node
%group
Extended screen control commands
%prm
var,color,matobj
var     : Variable name to which the generated object ID is assigned
color(-1)  : Material color (24bit RGB value)
matobj(-1) : Material ID
%inst
Generates a free-form shape defined as a 3D model as a node object.
It is necessary to construct model information called a custom 3D mesh in advance using commands such as gpmeshclear, gpmeshadd, and gpmeshpolygon.
Unlike simple models such as gpbox and gpplate, you can freely define complex shapes, but it is a function for advanced users because it is necessary to define each vertex coordinate and normal vector.
Normally, please use 3D model data prepared as a .gpb file.
^
Custom 3D meshes can be created in the following steps.
^p
	1. Initialize the custom 3D mesh with the gpmeshclear command
	2. Register the necessary vertex information with the gpmeshadd command
	3. Configure triangular or quadrilateral faces by combining vertex information with the gpmeshpolygon command
	4. Repeat the above registration only for the necessary faces
^p
The gpmesh command generates a model using all registered faces based on the created custom 3D mesh information.
Vertex information can include X, Y, Z coordinates, normal vectors, texture UV coordinates, and more. By setting these appropriately, you can define free shapes.
Custom 3D meshes can be redefined any number of times to register different model shapes.
^
The ID of the generated object is assigned to the variable specified by var.
The generated object can be manipulated in the same way as a 3D model loaded by the gpload command.
The color parameter specifies the color of the entire model (material color) with a 24-bit RGB value (a value represented by 0xRRGGBB).
If the color parameter is omitted, white (0xffffff) is used.
The matobj parameter allows you to specify a material ID.
Specify this if you are using a material generated by the user with the gpcolormat command, etc.
If the matobj parameter is omitted, the standard material is used.

%href
gpmeshclear
gpmeshpolygon
gpmeshadd


%index
gpmeshclear
Initialize custom 3D mesh
%group
Extended screen control commands
%inst
Initializes the registration information of the custom 3D mesh.
The custom 3D mesh is a mechanism for constructing data to define free 3D shapes.
The actual data registration uses the gpmeshadd and gpmeshpolygon commands.
The constructed data can be registered as a node object with the gpmesh command.
%href
gpmesh
gpmeshpolygon
gpmeshadd


%index
gpmeshadd
Add vertex information to custom 3D mesh
%group
Extended screen control commands
%prm
var,x,y,z,nx,ny,nz,u,v
var     : Variable name to which the generated mesh vertex ID is assigned
x(0)    : X coordinate (real number)
y(0)    : Y coordinate (real number)
z(0)    : Z coordinate (real number)
nx(0)   : Normal vector X (real number)
ny(0)   : Normal vector Y (real number)
nz(0)   : Normal vector Z (real number)
u(0)    : Texture vertex coordinate X (real number)
v(0)    : Texture vertex coordinate Y (real number)
%inst
Adds and registers vertex information to the custom 3D mesh.
Vertex information registers X, Y, Z coordinates, normal vectors, and texture UV coordinates as a single set.
The registered mesh vertex ID is assigned to the variable specified by var. The mesh vertex ID is used when configuring faces (triangles, quadrilaterals).
If it is vertex information that has already been registered, the mesh vertex ID registered in the past is assigned, and duplicate registration is not performed. Up to 32767 vertex information can be registered.
^
(X,Y,Z) coordinates indicate a vertex in 3D space.
The normal vector (X,Y,Z) indicates the normalized direction (vector) the vertex is facing. The normal vector is necessary for performing lighting calculations.
Texture vertex coordinates (X(u), Y(v)) specify the coordinates referenced when applying a texture. These are normalized values from the origin (0.0, 0.0) to the ends (1.0, 1.0). They are required when applying a texture as a material.
^
When defining a custom 3D mesh, you must first clear the registration information with the gpmeshclear command.
To actually register faces (triangles, quadrilaterals) by combining vertex information, use the gpmeshpolygon command.
The constructed data can be registered as a node object with the gpmesh command.
^
Add face information to a custom 3D mesh.
It is necessary to register the required vertex information in advance using the gpmeshadd command.
Combine mesh vertex IDs indicating the index of vertex information to construct a face (triangle, quadrilateral).
For a triangle, specify the p1, p2, p3 parameters; for a quadrilateral, specify the p1, p2, p3, p4 parameters with mesh vertex IDs. The order in which the vertices are specified should be such that the p1, p2, p3 triangle is arranged clockwise. (For a quadrilateral, add the remaining one vertex last.)
(The coordinates to specify are in a coordinate system where (-1, -1, 0) is the upper left and (1, 1, 0) is the lower right.
It is not the OpenGL coordinate system, but rather the model coordinates are considered based on the size of the coordinate values.
Also, be aware that the top and bottom of the texture (UV) coordinates 0-1 are reversed.)
^
The constructed data can be registered as a node object with the gpmesh command.
^
Get 3D model hierarchy information.
The hierarchy of the 3D model loaded by the gpload command is obtained and the result is assigned to the variable specified by var.
Specify the individual node by designating the hierarchy that the 3D model specified by objid has by its name.
(The hierarchy name (node name) is given in advance when creating the 3D model. To obtain all hierarchies, use the gptree_get command of the mod_gputil module.)
You can select the result to be assigned by the option value. The values that can be specified for the option value are as follows.
^p
	Option Value          | Content Obtained
	--------------------------------------------------------------
	GPNODEINFO_NODE         Object ID indicating the corresponding node
	GPNODEINFO_MODEL        Object ID indicating the corresponding model node
	GPNODEINFO_MATNUM       The number of materials the node holds
	GPNODEINFO_MATERIAL     Node's material ID
	GPNODEINFO_NAME         Corresponding node name (*)
	GPNODEINFO_CHILD        Node name of the child that the hierarchy has (*)
	GPNODEINFO_SIBLING      Node name of the adjacent node in the same hierarchy (*)
	GPNODEINFO_SKINROOT     Node name that the skin mesh has (*)
	GPNODEINFO_MATNAME      Material name set for the node (*)

    (*) The result string is assigned
^p
If GPNODEINFO_NODE is specified, a special object ID value indicating the hierarchy with the specified name is assigned.
This object ID can be used to obtain the coordinates and rotation information of each hierarchy, such as getpos, getang, getquat. (This object ID can only be used immediately after obtaining it with the gpnodeinfo command. If you obtain another hierarchy with the gpnodeinfo command, the previously obtained object ID will be invalid.)
If GPNODEINFO_MODEL is specified, an object ID is obtained only for the hierarchy that has the model to be drawn.
The GPNODEINFO_CHILD, GPNODEINFO_SIBLING, and GPNODEINFO_SKINROOT options obtain the connection to other hierarchies. This allows you to examine the hierarchy of the nodes.
If the process ends normally, 0 is assigned to the system variable stat. If an error occurs, a negative value is assigned to the system variable stat.
^
If GPNODEINFO_MATNUM is specified, the number of materials held by the node is obtained. -1 is returned if a non-mesh model node is specified.
If GPNODEINFO_MATNAME is specified, the material name set for the node is obtained. An empty string ("") is returned if a non-mesh model node or a 3D node not loaded with gpload is specified.
If multiple materials are set for one node, add the index value such as GPNODEINFO_MATNAME+1, GPNODEINFO_MATNAME+2. You can check whether multiple materials are set with the GPNODEINFO_MATNUM option.
^
If GPNODEINFO_MATERIAL is specified, a material ID for referencing the material set for the node is generated. -1 is returned if a non-mesh model node or a 3D node not loaded with gpload is specified.
The obtained material ID can be additionally changed with gpmatprm series and gpmatstate series commands.
If multiple materials are set for one node, add the index value such as GPNODEINFO_MATERIAL+1, GPNODEINFO_MATERIAL+2. You can check whether multiple materials are set with the GPNODEINFO_MATNUM option.
The material ID obtained by GPNODEINFO_MATERIAL is a special ID generated to refer to the information in the node. If you perform a process such as acquiring it every frame, a new material ID will be acquired for each frame, so acquire the material ID only once after the model is loaded and use it.
Even if the obtained material ID is destroyed by the gpdelobj command, the original material information is maintained, so you can discard unnecessary material IDs.
^
Add a work2 group change event.
Adds a group change event to the event ID specified by id.
A group change event sets a time-dependent change of the parameters that an object has.
The value will be (x1, y1, z1) when the number of frames specified by frame has elapsed.
The interpolation options for sw can specify the following values.
^p
	sw = 0 : Linear interpolation (absolute value)
	sw = 1 : Spline interpolation (absolute value)
	sw = 2 : Linear interpolation (relative value)
	sw = 3 : Spline interpolation (relative value)
^p
If sw is omitted, the absolute value linear is set.

^
Add a work2 group setting event.
Adds a group setting event to the event ID specified by id.
The group setting event sets the parameters of the object.
If (x1, y1, z1) and (x2, y2, z2) are specified, values within each range are created randomly.
If (x2, y2, z2) is omitted and only (x1, y1, z1) is specified, the value is set as is.

^
Add a work2 group addition event.
Adds a group addition event to the event ID specified by id.
The group addition event adds the values of (x, y, z) to the parameters of the object.

^
Add a parameter addition event.
Adds a parameter addition event to the event ID specified by id.
The parameter setting event adds the value of p2 to the parameter ID specified by p1. (Negative values can also be set)
The following names can be used for the parameter ID.
^p
Parameter ID   Content
---------------------------------------
		PRMSET_FLAG            Object registration flag (*)
		PRMSET_MODE            Mode flag value
		PRMSET_ID              Object ID (*) (HGIMG4 only)
		PRMSET_ALPHA           Transparency (óØ value) (HGIMG4 only)
		PRMSET_SHADE           Shading mode (HGIMG3 only)
		PRMSET_TIMER           Timer value
		PRMSET_MYGROUP         Own collision group
		PRMSET_COLGROUP        Collision detection collision group
		PRMSET_SHAPE           Shape ID (*) (HGIMG4 only)
		PRMSET_USEGPMAT        Material ID (HGIMG4 only)
		PRMSET_USEGPPHY        Physics setting ID (*) (HGIMG4 only)
		PRMSET_COLILOG         Collision log ID (*) (HGIMG4 only)
		PRMSET_FADE            Fade parameter (HGIMG4 only)
		PRMSET_SPRID           Source buffer ID (sprite only) (HGIMG4 only)
		PRMSET_SPRCELID        Source cell ID (sprite only) (HGIMG4 only)
		PRMSET_SPRGMODE        Copy mode (sprite only) (HGIMG4 only)

		(*) Items are read-only
^p
The p3 and p4 parameters allow you to specify the minimum and maximum values set after addition.
It is possible to perform addition so that it stays only within a specific range.
^
