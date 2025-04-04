%ver
3.3

%date
2011/05/10

%author
onitama

%url
http://hsp.tv/

%type
Macro

%group
Standard Defined Macros

%port
Win
Cli
Let

%index
_debug
Debug Mode

%inst
This macro is defined only in debug mode.
It can be used with #ifdef, #ifndef commands to branch the process.

%sample
#ifdef _debug
	mes "This is debug mode."
#else
	mes "This is not debug mode."
#endif
	stop
%href
hspstat

%index
__hsp30__
Version 3.0 or Later

%inst
Defined when using HSP3.0 or later and "Use HSP Extended Macro" is checked.

%sample
#ifdef __hsp30__
	mes {"HSP3.0 or later, and \"Use HSP Extended Macro\" is
	checked."}
#else
	mes {"Older than HSP3.0, or \"Use HSP Extended Macro\" is
	not checked."}
#endif
	stop

%index
__file__
Filename being parsed at use

%inst
Replaced with the filename being parsed at the time of use.
If the file is not saved, it is replaced with "???".

%sample
	mes "The filename being parsed at the time of use is " + __file__
	stop

%href
__line__

%index
__line__
Line number being parsed at use

%inst
Replaced with the line number being parsed at the time of use. The line number is an integer value starting from 1.

%sample
	mes "The line number being parsed at the time of use is " + __line__
	stop

%href
__file__

%index
__date__
Date at use

%inst
Replaced with the date at the time of use (compilation). The date is expressed in YYYY/MM/DD format.

It cannot be used unless "Use HSP Extended Macro" is checked.

%sample
	mes "The date at the time of use is " + __date__
	stop

%href
__time__

%index
__time__
Time at use

%inst
Replaced with the time at the time of use (compilation). The time is expressed in HH:MM:SS format.

It cannot be used unless "Use HSP Extended Macro" is checked.

%sample
	mes "The time at the time of use is " + __time__
	stop

%href
__date__

%index
__hspver__
HSP Version Number

%inst
Replaced with the HSP version number. The upper 8 bits and lower 8 bits indicate the major version and minor version, respectively.

It cannot be used unless "Use HSP Extended Macro" is checked.

%sample
	mes "The HSP version number is " + strf("%4x", __hspver__)
	stop

%href
hspver
