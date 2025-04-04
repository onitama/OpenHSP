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
Include mod_picfont.as.

%type
User-defined instructions

%group
Extended screen control command

%port
Win
Let


%index
picfont
Font display preparation using images
%prm
id, cx, cy, mode, offset
id: Buffer ID with the font image
cx, cy: XY size per font character
mode: Display mode (gmode mode value)
offset: Display position offset

%inst
Prepare for font display using images.
Specify the buffer ID where the font image exists in id.
In the image, it is necessary to store half-width alphanumeric monospaced characters in 16 characters x 8 columns (character code order) in advance.
This image is compatible with the one used in the HGIMG3 setfont instruction.
Specify the character size per character by cx and cy.
By mode, specify the copy mode (mode value specified by gmode) at the time of display.
(For example, if 2 is specified, the black background will be displayed transparently.)
For offset, specify a value that corrects the horizontal position when displaying one character.
The higher this value, the wider the character spacing, and the smaller this value, the narrower the spacing.
If 0 is specified, the character feed will be the same as the font size.
The font image initialized by the picfont command can be displayed by the picfprt command.

%href
picfprt


%index
picfprt
Font display using images
%prm
"message"
"message": Message to display

%inst
The font image initialized by the picfont command is displayed.
Specify the character string to be displayed in "message". Only single-byte alphanumeric characters can be displayed.
The position of the displayed character is the current position specified by the pos instruction.
picfprt recognizes the line feed code.
Also, after the message is displayed, the current position moves to the line break position.

%href
picfont


