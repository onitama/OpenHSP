%ver
3.3

%date
2011/05/10

%author
onitama

%url
http://hsp.tv/

%type
macro

%group
Standard definition macro

%port
Win
Cli
Let

%index
_debug
In debug mode

%inst
This macro is defined only in debug mode.
It is possible to distinguish and branch with #ifdef, #ifndef instructions, etc.

%sample
#ifdef _debug
mes "Debug mode."
#else
mes "Not in debug mode."
#endif
	stop
%href
hspstat

%index
__hsp30__
When using ver3.0 or later

%inst
It is defined when "Use HSP extension macro" is checked in HSP3.0 or later.

%sample
#ifdef __hsp30__
mes {"HSP3.0 or later, to" use HSP extension macro "
It is checked. "}
#else
mes {"older than HSP3.0 or to" use HSP extension macro "
It is not checked. "}
#endif
	stop

%index
__file__
File name analyzed at the time of use

%inst
It will be replaced with the filename being parsed at the time of use.
If the file is not saved, it will be replaced with "???".

%sample
mes "The file name being parsed at the time of use is" + __file__
	stop

%href
__line__

%index
__line__
Line number analyzed at the time of use

%inst
Replaces the line number being parsed at the time of use. Line numbers are integer values starting at 1.

%sample
mes "The line number being parsed at the time of use is" + __line__
	stop

%href
__file__

%index
__date__
Date of use

%inst
Replaces the date at the time of use (when compiled). Dates are represented in YYYY / MM / DD format.

It cannot be used unless "Use HSP extension macro" is checked.

%sample
mes "The date at the time of use is" + __date__
	stop

%href
__time__

%index
__time__
Time of use

%inst
It will be replaced with the time when it was used (when it was compiled). The time is expressed in HH: MM: SS format.

It cannot be used unless "Use HSP extension macro" is checked.

%sample
mes "Time at the time of use" + __time__
	stop

%href
__date__

%index
__hspver__
HSP version number

%inst
It will be replaced with the HSP version number. The upper 8 bits and lower 8 bits indicate the major version and minor version, respectively.

It cannot be used unless "Use HSP extension macro" is checked.

%sample
mes "HSP version number is" + strf ("% 4x", __hspver__)
	stop

%href
hspver
