;
;	HSP help manageróp HELPÉ\Å[ÉXÉtÉ@ÉCÉã
;	(êÊì™Ç™;ÇÃçsÇÕÉRÉÅÉìÉgÇ∆ÇµÇƒèàóùÇ≥ÇÍÇ‹Ç∑)
;

%type
Built-in function
%ver
3.6
%note
ver3.6 standard function
%date
2019/12/19
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli
Let




%index
int
Convert to integer value
%group
Basic input/output functions
%prm
(p1)
p1 : Value or variable to convert

%inst
Returns the integer value of p1.
If the value is a real number, the decimal part is truncated.
If the value is a string, it will be converted to the number if it is a numeric string, otherwise it will be 0.

%href
str
double



%index
rnd
Generate a random number
%group
Basic input/output functions
%prm
(p1)
p1=1Å`32768 : Range of random numbers

%inst
Generates an integer random number in the range of 0 to (p1-1).
The value of p1 is not included in the range. For example,
^p
Example :
    a=rnd(10)
^p
assigns a random number from 0 to 9 to the variable a.
The rnd function generates random numbers in a fixed pattern after the program starts.
If you want to make the random number pattern non-constant, use the randomize command.

%href
randomize



%index
strlen
Get the length of a string
%group
Basic input/output functions
%prm
(p1)
p1 : String or string variable name to get the length of

%inst
Returns the length (memory size) of the string specified by p1 or the string held by the string variable.
If the string contains double-byte characters (Japanese), each character is calculated as two characters.
(However, the length of the character may be different in HSP64 and UTF-8 runtime)
%href




%index
length
Returns the number of 1-dimensional array elements
%group
Basic input/output functions
%prm
(p1)
p1 : Variable to check the array

%inst
Returns the number of array elements (1-dimensional) that the variable specified by p1 has.
If the number of array elements is 5, p1(0) to p1(4) will exist.

%href
length2
length3
length4



%index
length2
Returns the number of 2-dimensional array elements
%group
Basic input/output functions
%prm
(p1)
p1 : Variable to check the array

%inst
Returns the number of array elements (2-dimensional) that the variable specified by p1 has.
If the number of array elements is 5, p1(0,0) to p1(?,4) will exist.
If the dimension of the array does not exist, 0 is returned.

%href
length
length3
length4



%index
length3
Returns the number of 3-dimensional array elements
%group
Basic input/output functions
%prm
(p1)
p1 : Variable to check the array

%inst
Returns the number of array elements (3-dimensional) that the variable specified by p1 has.
If the number of array elements is 5, p1(0,0,0) to p1(?,?,4) will exist. If the dimension of the array does not exist, 0 is returned.

%href
length
length2
length4



%index
length4
Returns the number of 4-dimensional array elements
%group
Basic input/output functions
%prm
(p1)
p1 : Variable to check the array

%inst
Returns the number of array elements (4-dimensional) that the variable specified by p1 has.
If the number of array elements is 5, p1(0,0,0,0) to p1(?,?,?,4) will exist. If the dimension of the array does not exist, 0 is returned.

%href
length
length2
length3



%index
vartype
Returns the type of variable
%group
Basic input/output functions
%prm
(p1)
p1 : Variable or string to check the type

%inst
Checks and returns the type of value stored in the variable specified by p1.
The value returned is an integer value that indicates the type. The type values are as follows:
^p
 1 : Label type
 2 : String type
 3 : Real number type
 4 : Integer type
 5 : Module type
 6 : COM object type
^p
If the type is extended by a plugin, etc., a value other than these will be returned.
Also, if a string is specified for p1, it is treated as a type name indicating the type.
The type name must exactly match the registered one, including case.
^p
"int"    : Integer type
"str"    : String type
"double" : Real number type
"struct" : Module type
^p
The strings that can be used as standard type names are as above. However, if the type is extended by a plugin, etc., the type names that can be specified will also be added.


%href
varptr
varsize



%index
varptr
Returns the pointer of variable data
%group
Basic input/output functions
%prm
(p1)
p1 : Variable or command to check the pointer

%inst
Returns the memory address where the data stored in the variable specified by p1 is located.
If an external extension command (command for calling DLL defined by #func) is specified for p1, the address of the external function that is actually executed is returned.
This function is used in special cases such as when you want to pass a pointer to an external DLL, and you usually don't need to remember it.
The pointer obtained by varptr may change due to array expansion or content updates, so use it just before referencing the value.

%href
varsize
vartype
libptr
%port-
Let


%index
varsize
Returns the buffer size of variable data
%group
Basic input/output functions
%prm
(p1)
p1 : Variable to check the buffer size

%inst
Returns the buffer size (memory size allocated) of the data stored in the variable specified by p1.
The size obtained by varsize may change due to content updates, so use it just before referencing the value.

%href
varptr
vartype
libptr
%port-
Let




%index
gettime
Get the time and date
%group
Basic input/output functions
%prm
(p1)
p1=0Å`7(0) : Type to get

%inst
Returns the date and time information of the type specified by p1.
The types to get are as follows:

^p
    0 : Year
    1 : Month
    2 : DayOfWeek
    3 : Day
    4 : Hour
    5 : Minute
    6 : Second
    7 : Milliseconds
^p
For example,

^p
Example :
    a=gettime(4)   ; What time is it?
^p
assigns the current hour to the variable a.

%href




%index
str
Convert to string
%group
Basic input/output functions
%prm
(p1)
p1 : Value or variable to convert

%inst
Returns the string value of p1.

%href
int
double



%index
dirinfo
Get directory information
%group
Basic input/output functions
%prm
(p1)
p1=0Å`5 : Type to get

%inst
Returns the directory name of the type specified by p1.
The types to get are as follows:
^p
    0 : Current directory (dir_cur)
    1 : Directory where the HSP executable file is located (dir_exe)
    2 : Windows directory (dir_win)
    3 : Windows system directory (dir_sys)
    4 : Command line string (dir_cmdline)
    5 : HSPTV directory (dir_tv)
^p
p1 cannot be omitted.
Also, if a value of 0x10000 or more is specified for p1, the special folder is acquired by using bits 0 to 15 as the CSIDL value.
This allows you to acquire most folders managed by the system, such as Desktop (0x10000) and My Documents (0x10005).
Normally, please use it through the "dir_*" macro defined in hspdef.as.

%href
dir_cur
dir_exe
dir_win
dir_sys
dir_cmdline
dir_tv



%index
double
Convert to real value
%group
Basic input/output functions
%prm
(p1)
p1 : Value or variable to convert

%inst
Returns the real value of p1.
If the value is a string, it will be converted to the number if it is a numeric string, otherwise it will be 0.

%href
int
str



%index
sin
Returns the sine value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the sine value of p1 as a real number.
The unit specified by p1 is radians (2ÉŒ is 360 degrees).
%href
cos
tan
atan


%index
cos
Returns the cosine value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the cosine value of p1 as a real number.
The unit specified by p1 is radians (2ÉŒ is 360 degrees).
%href
sin
tan
atan


%index
tan
Returns the tangent value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the tangent value of p1 as a real number.
The unit specified by p1 is radians (2ÉŒ is 360 degrees).
%href
sin
cos
atan


%index
atan
Returns the arctangent value
%group
Basic input/output functions
%prm
(p1,p2)
p1      : Y value
p2(1.0) : X value

%inst
Using p1 as the Y value and p2 as the X value, the angle of Y/X (arctangent value) is returned in real radian units (2ÉŒ is 360 degrees).
If p2 is omitted, 1.0 is used.

%href
sin
cos
tan


%index
sqrt
Returns the root value
%group
Basic input/output functions
%prm
(p1)
p1=0.0Å`(0.0) : Value to find the root

%inst
Returns the root (square root) value of p1 as a real number.
A negative value cannot be specified for p1.
%href



%index
sysinfo
Get system information
%group
Basic input/output functions
%prm
(p1)
p1=0Å` : Type to get

%inst
Returns the system information value of the type specified by p1.
The types that can be obtained are as follows:
^p
  0 : String OS name and version number
  1 : String Logged in user name
  2 : String Computer name on the network
  3 : Numeric Language used by HSP (0=English/1=Japanese)
 16 : Numeric Type of CPU being used (code)
 17 : Numeric Number of CPUs being used
 33 : Numeric Usage of physical memory size (in %)
 34 : Numeric Total physical memory size
 35 : Numeric Free physical memory size
 36 : Numeric Total size of swap file
 37 : Numeric Free size of swap file
 38 : Numeric Total memory size including virtual memory
 39 : Numeric Free memory size including virtual memory
^p

%href



%index
peek
Read 1 byte from buffer
%group
Memory management functions
%prm
(p1,p2)
p1=Variable : Variable name from which to read the contents
p2=0Å`  : Buffer index (in bytes)

%inst
Returns the contents of 1 byte (8 bits) at an arbitrary location on the data memory stored in the variable as a numerical value.
The return value of the function is an integer value from 0 to 255.

%href
wpeek
lpeek



%index
wpeek
Read 2 bytes from buffer
%group
Memory management functions
%prm
(p1,p2)
p1=Variable : Variable name from which to read the contents
p2=0Å`  : Buffer index (in bytes)

%inst
Returns the contents of 2 bytes (16 bits) at an arbitrary location on the data memory stored in the variable as a numerical value.
The return value of the function is an integer value from 0 to 65535.

%href
peek
lpeek



%index
lpeek
Read 4 bytes from buffer
%group
Memory management functions
%prm
(p1,p2)
p1=Variable : Variable name from which to read the contents
p2=0Å`  : Buffer index (in bytes)

%inst
Returns the contents of 4 bytes (32 bits) at an arbitrary location on the data memory stored in the variable as a numerical value.
The return value of the function is an integer value from 0 to $ffffffff.

%href
peek
wpeek
%href
peek
wpeek

%index
callfunc
Calling External Functions
%group
Basic Input/Output Functions
%prm
(p1,p2,p3)
p1 : Array variable containing parameters
p2 : Function address
p3 : Number of parameters

%inst
Calls the address specified by p2 as a native function.
Uses the values stored in the numerical array variable specified by p1 as arguments for the call. You can specify the number of parameters with p3.
^p
Example:
	a.0=1
	a.1=2
	a.2=3
	res = callfunc( a, proc, 3 )
^p
In the example above, the function at the address indicated by proc is called with the arguments (1,2,3).
The return value of the called function becomes the return value of callfunc.
This function is used in special situations where you prepare a function address yourself and call it.
Normally, it is not necessary to use this. Also, be very careful when using this function, as failure to call an external function may result in freezing or unexpected results.

%href
#uselib
#func

%index
absf
Returns the absolute value of a real number
%group
Basic Input/Output Functions
%prm
(p1)
p1 : Real number to convert to absolute value

%inst
Returns the absolute value of p1 as a real number.
If you need the absolute value of an integer, use the abs function.
%href
abs

%index
abs
Returns the absolute value of an integer
%group
Basic Input/Output Functions
%prm
(p1)
p1 : Integer to convert to absolute value

%inst
Returns the absolute value of p1 as an integer.
If you need the absolute value of a real number, use the absf function.
%href
absf

%index
logf
Returns the logarithm
%group
Basic Input/Output Functions
%prm
(p1)
p1=0.0Å`(0.0) : Value to calculate the logarithm of

%inst
Returns the logarithm (log) value of p1 as a real number.
If you specify 0 for p1, it returns infinity (INF).
%href
expf

%index
expf
Returns the exponential value
%group
Basic Input/Output Functions
%prm
(p1)
p1=0.0Å`(0.0) : Value to calculate the exponential of

%inst
Returns the exponential (exp) value of p1 as a real number.
If it overflows, it returns infinity (INF), and if it underflows, it returns 0.
%href
logf

%index
limit
Returns an integer within a certain range
%group
Basic Input/Output Functions
%prm
(p1,p2,p3)
p1 : Target value
p2 : Minimum value
p3 : Maximum value

%inst
Returns the value specified in p1 converted to an integer within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
As a result, the value returned by the limit function is always within the range of p2 to p3.
If the p2 parameter is omitted, the minimum value is not restricted.
If the p3 parameter is omitted, the maximum value is not restricted.
To find the range of real numbers, use the limitf function.

%href
limitf

%index
limitf
Returns a real number within a certain range
%group
Basic Input/Output Functions
%prm
(p1,p2,p3)
p1 : Target value
p2 : Minimum value
p3 : Maximum value

%inst
Returns the value specified in p1 converted to a real number within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
As a result, the value returned by the limitf function is always within the range of p2 to p3.
If the p2 parameter is omitted, the minimum value is not restricted.
If the p3 parameter is omitted, the maximum value is not restricted.
To find the range of integers, use the limit function.

%href
limit

%index
varuse
Returns the usage status of a variable
%group
Basic Input/Output Functions
%prm
(p1)
p1 : Variable to check usage status

%inst
Returns the usage status of the variable specified in p1.
varuse is only effective in situations where an entity does not exist, such as module-type or COM object-type variables.
Returns 1 if the specified variable is valid, and 0 otherwise.
For module-type variables, you can check whether it is unused (0), initialized (1), or a clone of another variable (2).
For COM object types, you can check whether it holds a valid COM object.

%href
newmod
delmod
%port-
Let

%index
libptr
Gets the information address of an external call command
%group
Basic Input/Output Functions
%prm
(p1)
p1 : Command to check the address

%inst
Acquires the information address of the command specified in p1 and returns it as an integer value.
By specifying an external DLL call command or function as a parameter, you can obtain the address of the STRUCTDAT structure where information about the command is stored.
The STRUCTDAT structure is defined in HSPSDK as follows:
^p
	typedef struct STRUCTDAT {
	short	index;           // base LIBDAT index
	short	subid;           // struct index
	int	prmindex;            // STRUCTPRM index(MINFO)
	int	prmmax;              // number of STRUCTPRM
	int	nameidx;             // name index (DS)
	int	size;                // struct size (stack)
	int	otindex;             // OT index(Module)  / cleanup flag(Dll)
	union {
		void	*proc;       // proc address
		int	funcflag;        // function flags(Module)
	};
	} STRUCTDAT;
^p
Similarly, when a COM call command, a user-defined instruction, or a user-defined function is specified in p1, the address of the STRUCTDAT structure is obtained.
The libptr function assists in accessing the internal data used by HSP, and you should use it with sufficient knowledge of the contents of the information handled here.
Normally, it is not necessary to use or remember this function in the normal usage range.
^
By referring to the STRUCTDAT structure, it is possible to obtain information such as the address of the external call DLL and the DLL handle.

%href
varptr
%sample
	#uselib "user32.dll"
	#func MessageBoxA "MessageBoxA" int,sptr,sptr,int

	ladr=libptr( MessageBoxA )
	dupptr lptr,ladr,28	; Get STRUCTDAT structure
	lib_id=wpeek(lptr,0)
	mes "LIB#"+lib_id
	mref hspctx,68
	linf_adr=lpeek( hspctx, 832 )
	dupptr linf,linf_adr + lib_id*16,16	; Get LIBDAT structure
	dll_flag = linf(0)
	dll_name = linf(1)
	dll_handle = linf(2)
	mes "FLAG("+dll_flag+") NAME_ID#"+dll_name
	mes "HANDLE="+strf("%x",dll_handle)
	stop
%port-
Let

%index
comevdisp
Check COM event contents
%group
COM Object Manipulation Functions
%prm
(p1)
p1      : Variable name

%inst
Within the event subroutine of the variable (COM object type) specified by p1, obtains the event dispatch ID (DISPID).
The variable specified by p1 must be initialized by the comevent command.
Also, acquisition must always be done within the event subroutine.

%href
comevent
comevarg
%port-
Let

%index
powf
Calculates powers (exponentiation)
%group
Basic Input/Output Functions
%prm
(p1, p2)
p1 : Base (0 or more)
p2 : Exponent

%inst
Calculates the value of p1 raised to the power of p2. The result is given as a real number.
p1 must always be positive. If it is negative, it will not result in an error, but a non-number (-1.#IND00) will be returned.
p2 can be either positive or negative. It is also possible to specify a real number.

%sample
	repeat 5, -2
		mes "10 to the power of " + cnt + " is " + powf(10, cnt) + "."
	loop
	stop
%port-
Let

%index
getease
Gets the easing value as an integer
%group
Basic Input/Output Functions
%prm
(p1,p2)
p1(0) : Time elapsed value (integer value)
p2(4096) : Maximum of time elapsed value (integer value)
%inst
Obtains the calculation result value of the easing function, which interpolates numerical values within a determined range using an arbitrary calculation formula, as an integer.
When using an easing function, it is necessary to first set the minimum value, maximum value, and calculation formula to be output by the setease command.
The time elapsed value specified by p1 is an integer value starting from 0, and by specifying up to the maximum of the time elapsed value (p2 parameter), the calculation result of the easing function is returned.
If the maximum value (p2 parameter) is omitted, 4096 is used.
Normally, if the time elapsed value is a negative value, it is regarded as 0. Also, if the time elapsed value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added to the calculation formula type setting by the setease command, interpolation is performed including values outside the range (repeatedly).)

%href
setease
geteasef
%port-
Let

%index
geteasef
Gets the easing value as a real number
%group
Basic Input/Output Functions
%prm
(p1,p2)
p1(0) : Time elapsed value (real number value)
p2(1.0) : Maximum of time elapsed value (real number value)
%inst
Obtains the calculation result value of the easing function, which interpolates numerical values within a determined range using an arbitrary calculation formula, as a real number.
When using an easing function, it is necessary to first set the minimum value, maximum value, and calculation formula to be output by the setease command.
The time elapsed value specified by p1 is a real number value starting from 0, and by specifying up to the maximum of the time elapsed value (p2 parameter), the calculation result of the easing function is returned.
If the maximum value (p2 parameter) is omitted, 1.0 is used.
Normally, if the time elapsed value is a negative value, it is regarded as 0. Also, if the time elapsed value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added to the calculation formula type setting by the setease command, interpolation is performed including values outside the range (repeatedly).)
The geteasef command obtains more detailed easing function calculation results. If you want to get values that are passed as integers, such as normal coordinate values, it is faster to use the getease function.

%href
setease
getease
%port-
Let
