;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;
%ver
3.7
%date
2021/05/31
%group
Input/Output Control Commands

%type
User-Defined Extended Command

%note
Requires hgimg4.as(hgimg4dx.as) and mod_posteffect.as to be included.

%author
onitama
%dll
mod_posteffect
%url
https://hsp.tv/make/hgimg4.html
%port
Win
%portinfo
Runs on systems that support HGIMG4, such as Windows/Linux/android/iOS.


%index
post_reset
Initialize Post Effects
%prm
option
option(0) : Initialization Options
%inst
Initializes post effects. Post effects should be initialized only once during screen initialization.
Post effects using the mod_posteffect module must be executed within the HGIMG4 runtime environment.
The content of the initialization changes depending on the value set in option.
^p
	Option Value  Content
	-------------------------------------------------------------------------
	POST_RESET_OWNDRAW         The user draws the post effect themselves.
	POST_RESET_POSTLAYER       Include the GUI layer in the post effect.
^p
Normally, the initialization option can be omitted without any issues.
When the post effect initialization is complete, the system secures window ID 1 as the drawing window for post effects. (Also, IDs 64 and above are used as effect buffers.)
Usually, the screen drawn in 3D (the screen drawn with the gpdraw command) is the target of post effects, and 2D direct drawing (mes command, etc.) is not. If POST_RESET_POSTLAYER is specified in the option, 2D direct drawing will also be targeted.
When using post effects, update the screen using post_drawstart to post_drawend instead of redraw 0 to redraw 1.
When post_drawend is executed, the post effect is automatically applied to the screen that was drawn up to that point.
If POST_RESET_OWNDRAW is specified in the option, post effects are not drawn when post_drawend is executed. In this case, the user needs to update window ID 0 and draw the post effect themselves.
%href
post_select
post_drawstart
post_drawend


%index
post_select
Specify Post Effect ID
%prm
id
id(0) : Post Effect ID
%inst
Specifies a post effect ID to enable the screen effect.
You must initialize the post effect with the post_reset command beforehand.
The post effect ID can be specified as follows:
^p
	ID               Content                Options
	-------------------------------------------------------------------------
	POSTID_NONE      None
	POSTID_SEPIA     Sepia
	POSTID_GRAY      Grayscale
	POSTID_BLUR      Blur              level(4.0)
	POSTID_BLUR2     Blur (High Quality)      level(6.0)
	POSTID_MOSAIC    Mosaic
	POSTID_CONTRAST  Contrast        level(2.0),bright(-0.2)
	POSTID_SOBEL     Edge Detection
	POSTID_CRT       CRT Monitor          Curve Rate X(0.5), Curve Rate Y(0.4)
	POSTID_OLDFILM   Old Film
	POSTID_GLOW      Glow              rate(64),bright(0.3)
	POSTID_GLOW2     Glow (High Quality/Additive) rate(128),cutoff(0.5),level(10)
	POSTID_GLOW3     Glow (High Quality)      rate(128),cutoff(0.5),level(10)
	POSTID_CUTOFF    Cutoff          ratio(0.8)
^p
After enabling a post effect ID, you can set the option values for each effect using the post_setprm command. (You can specify the parameters in the Options column above. The values in parentheses are the default values.)
%href
post_reset
post_setprm


%index
post_setprm
Specify Post Effect Options
%prm
p1,p2,p3
p1(0.0) : Post Effect Parameter 1 (Real Number)
p2(0.0) : Post Effect Parameter 2 (Real Number)
p3(0)   : Post Effect Parameter 3 (Integer)
%inst
Sets the option values associated with the post effect set by the post_select command.
Option values allow you to adjust the effect of the set post effect, and the available options differ depending on the type of post effect.
%href
post_select


%index
post_drawstart
Start Screen Drawing for Post Effects
%inst
Specifies the timing to start screen drawing when using the post effects module.
You must initialize the post effect with the post_reset command beforehand.
When using post effects, update the screen using post_drawstart to post_drawend instead of redraw 0 to redraw 1.
%href
post_drawend


%index
post_drawend
End Screen Drawing for Post Effects
%inst
Specifies the timing to end screen drawing when using the post effects module.
You must initialize the post effect with the post_reset command beforehand.
When using post effects, update the screen using post_drawstart to post_drawend instead of redraw 0 to redraw 1.
%href
post_drawstart


%index
post_getname
Get Post Effect Name
%prm
var
var : Variable name to which the result is assigned.
%inst
Assigns the name of the currently active post effect to the variable specified by var.
The post effect name is assigned as a string.
%href
post_getmaxid
post_select


%index
post_getmaxid
Get Maximum Post Effect ID
%prm
var
var : Variable name to which the result is assigned.
%inst
Assigns the maximum value that can be used as a post effect ID to the variable specified by var.
The post effect ID is the type of post effect specified by post_select.
%href
post_getname
post_select
