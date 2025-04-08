;
; HSP help manager HELP source file
; (Lines starting with ; are treated as comments)
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
Convert to integer
%group
Basic input/output functions
%prm
(p1)
p1 : Value or variable to convert

%inst
Returns the integer value of p1.
If the value is a real number, the decimal part is truncated.
If the value is a string, it becomes the number if it's a numeric string; otherwise, it becomes 0.

%href
str
double



%index
rnd
Generate random number
%group
Basic input/output functions
%prm
(p1)
p1=1〜32768 : Range of random numbers

%inst
Generates an integer random number in the range from 0 to (p1-1).
The value of p1 is not included in the range. For example,
^p
Example:
    a=rnd(10)
^p
This assigns a random number from 0 to 9 to variable a.
The rnd function generates random numbers in a fixed pattern after the program starts.
If you want to make the random number pattern non-constant, use the randomize command.

%href
randomize



%index
strlen
Get string length
%group
Basic input/output functions
%prm
(p1)
p1 : String or string-type variable name whose length you want to check

%inst
Returns the length (memory size) of the string specified by p1 or the string held by the string-type variable.
If the string contains double-byte characters (Japanese), even one character is calculated as two characters.
(However, in the case of HSP64, UTF-8 runtime, the length of the characters may be different)
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
Returns the number of array elements (1-dimensional) possessed by the variable specified by p1.
If the number of array elements is 5, then p1(0) to p1(4) exist.

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
Returns the number of array elements (2-dimensional) possessed by the variable specified by p1.
If the number of array elements is 5, then p1(0,0) to p1(?,4) exist.
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
Returns the number of array elements (3-dimensional) possessed by the variable specified by p1.
If the number of array elements is 5, then p1(0,0,0) to p1(?,?,4) exist. If the dimension of the array does not exist, 0 is returned.

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
Returns the number of array elements (4-dimensional) possessed by the variable specified by p1.
If the number of array elements is 5, then p1(0,0,0,0) to p1(?,?,?,4) exist. If the dimension of the array does not exist, 0 is returned.

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
p1 : Variable to check the type, or string

%inst
Checks and returns the type of value stored in the variable specified by p1.
The value returned is an integer value indicating the type. The values of the types are as follows.
^p
 1 : Label type
 2 : String type
 3 : Real number type
 4 : Integer type
 5 : Module type
 6 : COM object type
^p
If the type is extended by a plugin, etc., values other than these will be returned.
Also, if a string is specified for p1, it is treated as a type name indicating the type.
The type name must exactly match the registered one, including case.
^p
"int"    : Integer type
"str"    : String type
"double" : Real number type
"struct" : Module type
^p
The strings that can be used as standard type names are as above. However, if the type is extended by plugins, etc., the type names that can be specified are also added.


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
p1 : Variable to check the pointer, or command

%inst
Returns the memory address where the data stored in the variable specified by p1 is located.
If you specify an external extended command (an instruction for calling a DLL defined with #func) in p1, it returns the address of the external function that is actually executed.
This function is used in special cases, such as when you want to pass a pointer to an external DLL, and is not normally something you need to remember.
The pointer obtained by varptr may change due to array expansion or content updates, so use it immediately before referring to the value.

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
Returns the buffer size (memory size secured) of the data stored in the variable specified by p1.
The size obtained by varsize may change due to content updates, so use it immediately before referring to the value.

%href
varptr
vartype
libptr
%port-
Let




%index
gettime
Get time/date
%group
Basic input/output functions
%prm
(p1)
p1=0〜7(0) : Type to acquire

%inst
Returns date/time information of the type specified by p1.
The types to get are as follows:

^p
    0 : Year
    1 : Month
    2 : Day of week (DayOfWeek)
    3 : Day
    4 : Hour
    5 : Minute
    6 : Second
    7 : Milliseconds
^p
For example,

^p
Example:
    a=gettime(4)   ; What time is it?
^p
assigns the current hour to variable a.

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
Returns the value specified by p1 as a string.

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
p1=0〜5 : Type to acquire

%inst
Returns the directory name of the type specified by p1.
The types to get are as follows:
^p
    0 : Current directory (dir_cur)
    1 : Directory where HSP executable file is located (dir_exe)
    2 : Windows directory (dir_win)
    3 : Windows system directory (dir_sys)
    4 : Command line string (dir_cmdline)
    5 : HSPTV directory (dir_tv)
^p
p1 cannot be omitted.
Also, if a value of 0x10000 or higher is specified for p1, the special folder is acquired by treating bits 0 to 15 as CSIDL values.
This allows you to acquire most of the folders managed by the system, such as Desktop (0x10000) and My Documents (0x10005).
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
Convert to real number
%group
Basic input/output functions
%prm
(p1)
p1 : Value or variable to convert

%inst
Returns the value specified by p1 as a real number.
If the value is a string, it becomes the number if it's a numeric string; otherwise, it becomes 0.

%href
int
str



%index
sin
Returns sine value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the sine value of p1 as a real number.
The unit specified by p1 is radians (2π is 360 degrees).
%href
cos
tan
atan


%index
cos
Returns cosine value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the cosine value of p1 as a real number.
The unit specified by p1 is radians (2π is 360 degrees).
%href
sin
tan
atan


%index
tan
Returns tangent value
%group
Basic input/output functions
%prm
(p1)
p1=(0.0) : Angle value (radians)

%inst
Returns the tangent value of p1 as a real number.
The unit specified by p1 is radians (2π is 360 degrees).
%href
sin
cos
atan


%index
atan
Returns arctangent value
%group
Basic input/output functions
%prm
(p1,p2)
p1      : Y value
p2(1.0) : X value

%inst
Using p1 as Y and p2 as the X value, the angle of Y/X (arctangent value) is returned in real radians (2π is 360 degrees).
If p2 is omitted, 1.0 is used.

%href
sin
cos
tan


%index
sqrt
Returns root value
%group
Basic input/output functions
%prm
(p1)
p1=0.0〜(0.0) : Value to find the root

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
p1=0〜 : Type to acquire

%inst
Returns the system information value of the type specified by p1.
The types that can be acquired are as follows:
^p
  0 : String OS name and version number
  1 : String Logged-in user name
  2 : String Computer name on the network
  3 : Numeric Language used by HSP (0=English/1=Japanese)
 16 : Numeric Type of CPU used (code)
 17 : Numeric Number of CPUs used
 33 : Numeric Amount of physical memory usage (unit %)
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
p1=Variable : Variable name of the source to read content from
p2=0〜  : Index of buffer (in bytes)

%inst
Returns the content of 1 byte (8 bits) at an arbitrary location on the data memory saved in the variable as a numerical value.
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
p1=Variable : Variable name of the source to read content from
p2=0〜  : Index of buffer (in bytes)

%inst
Returns the content of 2 bytes (16 bits) at an arbitrary location on the data memory saved in the variable as a numerical value.
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
p1=Variable : Variable name of the source to read content from
p2=0〜  : Index of buffer (in bytes)

%inst
Returns the content of 4 bytes (32 bits) at an arbitrary location on the data memory saved in the variable as a numerical value.
The return value of the function is an integer value from 0 to $ffffffff.

%href
peek
wpeek
%href
peek
wpeek



%index
callfunc
Calling external functions
%group
Basic Input/Output Functions
%prm
(p1,p2,p3)
p1 : Array variable storing parameters
p2 : Function address
p3 : Number of parameters

%inst
Calls the address specified by p2 as a native function.
Uses the values stored in the numeric array variable specified by p1 as arguments for the call. You can specify the number of parameters with p3.
^p
Example :
	a.0=1
	a.1=2
	a.2=3
	res = callfunc( a, proc, 3 )
^p
In the above example, the function at the address indicated by proc is called with the arguments (1,2,3).
The return value of the called function becomes the return value of callfunc as is.
This function is used in special situations where you prepare and call a function address yourself.
Normally, you do not need to use this. Also, if the external function call fails, freezing or unexpected results may occur, so use it with sufficient caution.

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
p1 : Real value to convert to absolute value

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
p1 : Integer value to convert to absolute value

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
p1=0.0〜(0.0) : Value to find the logarithm of

%inst
Returns the logarithm (log) value of p1 as a real number.
If 0 is specified for p1, infinity (INF) is returned.
%href
expf



%index
expf
Returns the exponential value
%group
Basic Input/Output Functions
%prm
(p1)
p1=0.0〜(0.0) : Value to find the exponential value of

%inst
Returns the exponential (exp) value of p1 as a real number.
If it overflows, infinity (INF) is returned, and if it underflows, 0 is returned.
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
Returns the value specified in p1 converted to an integer that falls within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
Therefore, the value returned by the limit function is always within the range of p2 to p3.
If the p2 parameter is omitted, the minimum value is not limited.
If the p3 parameter is omitted, the maximum value is not limited.
If you want to find the range of real numbers, use the limitf function.

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
Returns the value specified in p1 converted to a real number that falls within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
Therefore, the value returned by the limitf function is always within the range of p2 to p3.
If the p2 parameter is omitted, the minimum value is not limited.
If the p3 parameter is omitted, the maximum value is not limited.
If you want to find the range of integers, use the limit function.

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
varuse is only valid when there are situations where there is no actual entity, such as module type or COM object type variables.
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
Get the information address of an external call command
%group
Basic Input/Output Functions
%prm
(p1)
p1 : Command to check the address

%inst
Obtains the information address of the command specified in p1 and returns it as an integer value.
By specifying an external DLL call command or function as a parameter, you can obtain the address of the STRUCTDAT structure where information about the command is stored.
The STRUCTDAT structure is defined as follows in HSPSDK.
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
The address of the STRUCTDAT structure is also obtained when a COM call command, user-defined command, or user-defined function is specified in p1.
The libptr function assists in accessing internal data used by HSP, and should be used with sufficient knowledge of the contents of the information handled here.
Normally, it is not necessary to use or remember this function in the normal range of use.
^
By referring to the STRUCTDAT structure, it is possible to obtain information such as the address of the external call DLL and the DLL handle.

%href
varptr
%sample
	#uselib "user32.dll"
	#func MessageBoxA "MessageBoxA" int,sptr,sptr,int

	ladr=libptr( MessageBoxA )
	dupptr lptr,ladr,28	; Get the STRUCTDAT structure
	lib_id=wpeek(lptr,0)
	mes "LIB#"+lib_id
	mref hspctx,68
	linf_adr=lpeek( hspctx, 832 )
	dupptr linf,linf_adr + lib_id*16,16	; Get the LIBDAT structure
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
Check COM event content
%group
COM Object Manipulation Functions
%prm
(p1)
p1      : Variable name

%inst
Retrieves the event's dispatch ID (DISPID) within the event subroutine of the variable (COM object type) specified by p1.
The variable specified by p1 must be initialized by the comevent command.
Also, the acquisition must be done within the event subroutine.

%href
comevent
comevarg
%port-
Let


%index
powf
Calculate exponentiation (power)
%group
Basic Input/Output Functions
%prm
(p1, p2)
p1 : Base (0 or more)
p2 : Exponent

%inst
Calculates the value of p1 raised to the power of p2. The result is given as a real number.
p1 must be positive. If it is negative, it will not be an error, but a non-number (-1.#IND00) will be returned.
p2 can be positive or negative. It is also possible to specify a real number.

%sample
	repeat 5, -2
		mes "10 to the power of " + cnt + " is " + powf(10, cnt) + "."
	loop
	stop
%port-
Let



%index
getease
Get the easing value as an integer
%group
Basic Input/Output Functions
%prm
(p1,p2)
p1(0) : Elapsed time value (integer value)
p2(4096) : Maximum elapsed time value (integer value)
%inst
Acquires the calculation result value of the easing function, which interpolates numerical values in a determined range using arbitrary calculation formulas, as an integer.
When using the easing function, it is necessary to first set the minimum value, maximum value, and calculation formula to be output by the setease command.
The elapsed time value specified by p1 is an integer value starting from 0. By specifying up to the maximum elapsed time value (p2 parameter), the calculation result of the easing function is returned.
If the maximum value (p2 parameter) is omitted, 4096 is used.
Normally, if the elapsed time value is a negative value, it is regarded as 0. Also, even if the elapsed time value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added to the calculation formula type setting by the setease command, the interpolation is looped (repeated) even including values outside the range.)

%href
setease
geteasef
%port-
Let



%index
geteasef
Get the easing value as a real number
%group
Basic Input/Output Functions
%prm
(p1,p2)
p1(0) : Elapsed time value (real value)
p2(1.0) : Maximum elapsed time value (real value)
%inst
Acquires the calculation result value of the easing function, which interpolates numerical values in a determined range using arbitrary calculation formulas, as a real number.
When using the easing function, it is necessary to first set the minimum value, maximum value, and calculation formula to be output by the setease command.
The elapsed time value specified by p1 is a real number starting from 0. By specifying up to the maximum elapsed time value (p2 parameter), the calculation result of the easing function is returned.
If the maximum value (p2 parameter) is omitted, 1.0 is used.
Normally, if the elapsed time value is a negative value, it is regarded as 0. Also, even if the elapsed time value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added to the calculation formula type setting by the setease command, the interpolation is looped (repeated) even including values outside the range.)
The geteasef command obtains more detailed easing function calculation results. When acquiring values passed as integers such as normal coordinate values, it is faster to use the getease function.

%href
setease
getease
%port-
Let
