%dll
mod_picfont

%ver
3.3

%date
2009/08/01

%author
onitama

%url
http://hsp.tv/

%note
Be sure to include mod_picfont.as.

%type
User-defined instruction

%group
Extended screen control instruction

%port
Win
Let

%index
picfont
Prepare for image-based font display
%prm
id, cx, cy, mode, offset
id : Buffer ID containing the font image
cx,cy  : X and Y size of one font character
mode   : Display mode (gmode mode value)
offset : Offset for display position

%inst
Prepares for image-based font display.
Specifies the buffer ID where the font image exists with id.
The image must contain half-width alphanumeric characters of equal width in 16 characters x 8 rows (in character code order) beforehand.
This image is compatible with the one used by the HGIMG3 setfont command.
Specify the character size per character with cx and cy.
Specify the copy mode (mode value specified by gmode) for display with mode.
(For example, if you specify 2, the background black color will be displayed transparently)
Specify a value to correct the horizontal position when displaying one character in offset.
The larger this value, the wider the character spacing, and the smaller the value, the narrower the spacing.
If you specify 0, the character feed will be the same as the font size.
The font image initialized by the picfont command can be displayed by the picfprt command.

%href
picfprt


%index
picfprt
Image-based font display
%prm
"message"
"message" : Message to display

%inst
Displays the font image initialized by the picfont command.
Specify the string to display in "message". Only half-width alphanumeric characters can be displayed.
The position of the displayed character is the current position specified by the pos command.
picfprt recognizes line feed codes.
Also, after displaying the message, the current position moves to the line-fed position.

%href
picfont
