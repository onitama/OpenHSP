;
;	HSP help manager HELP source file
;	(Lines starting with ";" are treated as comments)
;
%ver
3.7
%date
2022/06/01
%group
Input/Output Control Commands

%type
User Defined Command

%note
Include layer_fade.as.

%author
onitama
%dll
layer_fade
%url
https://hsp.tv/
%port
Win
%portinfo
Works on HSP3 standard runtime/HSP3Dish/HGIMG4.


%index
layer_fade
Initializes the fade-in/out module
%inst
Initializes the fade-in/out module (layer_fade).
After this, you can use the fadein/fadeout commands for fade-in/out.
The fade-in/out module operates as one of the layer objects.
(Please note that it will not work correctly in scripts that do not use the stop or redraw commands.)
Also, when the screen is initialized using the cls command, it is necessary to initialize it again.
The fade-in/out module works on HSP3 standard runtime and HSP3Dish/HGIMG4.
However, please note that in HSP3 standard runtime, fade will not be applied to placed objects such as buttons due to specifications.
%sample
#include "layer_fade.as"	; Use the fade module

	screen 0,640,480
	layer_fade		; Initialize the fade module
	fadecolor $000000	; Set the fade color
	fadespeed 8		; Set the fade speed
	fadein			; Fade in
*main
	redraw 0
	rgbcolor $ffffff
	boxf
	color 255,0,0
	font msgothic,60,1
	pos 100,180:mes "Fading...",mesopt_outline
	redraw 1
	await 1000/30

	stick key
	if key&32 : fadeout	; Fade out with the [Enter] key

	goto *main
%href
fadein
fadeout
fadespeed
fadecolor



%index
fadein
Fades in the screen
%inst
Fades in the screen using the fade-in/out module.
It is essential to initialize using the layer_fade command beforehand.
%href
layer_fade


%index
fadeout
Fades out the screen
%inst
Fades out the screen using the fade-in/out module.
It is essential to initialize using the layer_fade command beforehand.
%href
layer_fade


%index
fadespeed
Sets the speed of fade-in/out
%prm
p1
p1(0) : Fade speed (1-128)
%inst
Sets the fade speed of the screen using the fade-in/out module.
The value set as the fade speed will be the amount of change per frame.
Specify a value around 1 to 128. The smaller the value, the slower the fade. The larger the value, the sooner the fade will complete.
It is essential to initialize using the layer_fade command beforehand.
%href
layer_fade


%index
fadecolor
Sets the color for fade-in/out
%prm
color
color(0) : Fade color code (RGB)
%inst
Sets the fade color of the screen using the fade-in/out module.
The color code is in the same RGB format as the rgbcolor command.
It is essential to initialize using the layer_fade command beforehand.
%href
layer_fade
