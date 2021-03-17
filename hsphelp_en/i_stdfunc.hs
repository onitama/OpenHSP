;
;HELP source file for HSP help manager
;(Lines beginning with; are treated as comments)
;

%type
Built-in functions
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
Basic input / output functions
%prm
(p1)
p1: Source value or variable

%inst
Returns an integer version of the value specified by p1.
If the value is real, it will be truncated after the decimal point.
If the value is a string, it will be that number if it is a numeric string, otherwise it will be 0.

%href
str
double



%index
rnd
Generate random numbers
%group
Basic input / output functions
%prm
(p1)
p1 = 1 to 32768: Random number range

%inst
Generates an integer random value in the range 0 to (p1-1).
The value of p1 is not included in the range. For example
^p
Example:
    a=rnd(10)
^p
Assigns a random number from 0 to 9 to the variable a.
The rnd function generates random numbers in a certain pattern after the program is started.
If you want the random number pattern to be non-constant, use the randomize instruction.

%href
randomize



%index
strlen
Check the length of the string
%group
Basic input / output functions
%prm
(p1)
p1: Character string or string type variable name for which you want to check the length of the character string

%inst
Returns the length of the string specified by p1 or the string held by the string type variable.
If the character string is Japanese full-width, even one character will be calculated for two characters.
%href




%index
length
Returns the number of one-dimensional elements in an array
%group
Basic input / output functions
%prm
(p1)
p1: Variables that examine arrays

%inst
Returns the number of array elements (one dimension) that the variable specified by p1 has.
If the number of array elements is 5, then p1 (0) to p1 (4) exist.

%href
length2
length3
length4



%index
length2
Returns the number of 2D elements in an array
%group
Basic input / output functions
%prm
(p1)
p1: Variables that examine arrays

%inst
Returns the number of array elements (2D) that the variable specified by p1 has.
If the number of array elements is 5, then p1 (0,0) to p1 (?, 4) exist.
If the dimensions of the array do not exist, 0 is returned.

%href
length
length3
length4



%index
length3
Returns the number of 3D elements in an array
%group
Basic input / output functions
%prm
(p1)
p1: Variables that examine arrays

%inst
Returns the number of array elements (3D) that the variable specified by p1 has.
If the number of array elements is 5, then p1 (0,0,0) to p1 (?,?, 4) exist. If the dimensions of the array do not exist, 0 is returned.

%href
length
length2
length4



%index
length4
Returns the number of 4D elements in an array
%group
Basic input / output functions
%prm
(p1)
p1: Variables that examine arrays

%inst
Returns the number of array elements (4 dimensions) that the variable specified by p1 has.
If the number of array elements is 5, then p1 (0,0,0,0) to p1 (?,?,?, 4) exist. If the dimensions of the array do not exist, 0 is returned.

%href
length
length2
length3



%index
vartype
Returns the type of a variable
%group
Basic input / output functions
%prm
(p1)
p1: Variable to check type, or string

%inst
Checks and returns the type of the value stored in the variable specified by p1.
The value returned will be an integer value indicating the type. The type values are:
^p
 1: Label type
 2: String type
 3: Real type
 4: Integer type
 5: Modular
 6: COM object type
^p
If the type is extended by a plug-in etc., other values will be returned.
If a character string is specified for p1, it will be treated as a type name indicating the type.
The type name must match the registered one exactly, including case.
^p
"int": Integer type
"str": String type
"double": real type
"struct": Modular
^p
As a standard type name, you can use it, but the strings are as above. However, in addition to this, if the type is extended by a plug-in etc., the type name that can be specified will also be added.


%href
varptr
varsize



%index
varptr
Returns a pointer to variable data
%group
Basic input / output functions
%prm
(p1)
p1: Variable or instruction to check the pointer

%inst
Returns the memory address where the data stored in the variable specified by p1 is stored.
If an external extension instruction (instruction for calling a DLL defined by #func) is specified in p1, the address of the external function that is actually executed is returned.
This function is used in special cases, such as when you want to pass a pointer to an external DLL, and you usually don't need to remember it.
The pointer obtained by varptr may change due to array expansion or content update, so use it immediately before referencing the value.

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
Basic input / output functions
%prm
(p1)
p1: Variable to check the buffer size

%inst
Returns the buffer size (allocated memory size) of the data stored in the variable specified by p1.
The size obtained by varsize may change due to content update etc., so please use it just before referring to the value.

%href
varptr
vartype
libptr
%port-
Let




%index
gettime
Get time / date
%group
Basic input / output functions
%prm
(p1)
p1 = 0 to 7 (0): Type to get

%inst
Returns the date / time information of the type specified in p1.
The types to get are as follows.

^p
    0: Year
    1: Month
    2: Day Of Week
    3: Day
    4: Hour
    5: Minute
    6: Second
    7: Milliseconds
^p
For example

^p
Example:
    a = gettime (4); What time is it now?
^p
Assigns the current time to the variable a.

%href




%index
str
Convert to string
%group
Basic input / output functions
%prm
(p1)
p1: Source value or variable

%inst
Returns a string of the values specified by p1.

%href
int
double



%index
dirinfo
Get directory information
%group
Basic input / output functions
%prm
(p1)
p1 = 0 to 5: Type to get

%inst
Returns the directory name of the type specified in p1.
The types to get are as follows.
^p
    0: Current (current) directory (dir_cur)
    1: Directory containing HSP executable files (dir_exe)
    2: Windows directory (dir_win)
    3: Windows system directory (dir_sys)
    4: Command line string (dir_cmdline)
    5: HSPTV directory (dir_tv)
^p
p1 cannot be omitted.
If a value of 0x10000 or more is specified for p1, bits 0 to 15 are used as the CSIDL value to acquire the special folder.
This allows you to retrieve most of the system-managed folders such as Desktop (0x10000) and My Documents (0x10005).
Normally, use it through the "dir _ *" macro defined in hspdef.as.

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
Basic input / output functions
%prm
(p1)
p1: Source value or variable

%inst
Returns the real number of the value specified by p1.
If the value is a string, it will be that number if it is a numeric string, otherwise it will be 0.

%href
int
str



%index
sin
Returns sine value
%group
Basic input / output functions
%prm
(p1)
p1 = (0.0): Angle value (radians)

%inst
Returns the sine value of p1 as a real number.
The unit specified by p1 is radian (2π is 360 degrees).
%href
cos
tan
atan


%index
cos
Returns a cosine value
%group
Basic input / output functions
%prm
(p1)
p1 = (0.0): Angle value (radians)

%inst
Returns the cosine value of p1 as a real number.
The unit specified by p1 is radian (2π is 360 degrees).
%href
sin
tan
atan


%index
tan
Returns a tangent value
%group
Basic input / output functions
%prm
(p1)
p1 = (0.0): Angle value (radians)

%inst
Returns the tangent value of p1 as a real number.
The unit specified by p1 is radian (2π is 360 degrees).
%href
sin
cos
atan


%index
atan
Returns the arctangent value
%group
Basic input / output functions
%prm
(p1,p2)
p1: Y value
p2 (1.0): X value

%inst
Returns the Y / X angle (arc tangent value) in real radians (2π is 360 degrees), where p1 is Y and p2 is the value of X.
If p2 is omitted, 1.0 is used. Also, if p2 is 0, 0.5π (90 degrees) is returned.

%href
sin
cos
tan


%index
sqrt
Returns the root value
%group
Basic input / output functions
%prm
(p1)
p1 = 0.0 ~ (0.0): Value to find the route

%inst
Returns the root (square root) value of p1 as a real number.
You cannot specify a negative value for p1.
%href



%index
sysinfo
Get system information
%group
Basic input / output functions
%prm
(p1)
p1 = 0 ~: Type to get

%inst
Returns the system information value of the type specified in p1.
The types that can be obtained are as follows.
^p
  0: String OS name and version number
  1: String Username logged in
  2: String Computer name on the network
  3: Numerical language used by HSP (0 = English / 1 = Japanese)
 16: Numerical value CPU type used (code)
 17: Numerical number Number of CPUs used
 33: Numerical physical memory size usage (unit:%)
 34: Numerical total physical memory size
 35: Numerical free physical memory size
 36: Total size of numeric swap file
 37: Numerical free size of swap file
 38: Total memory size including numeric virtual memory
 39: Free memory size including numeric virtual memory
^p

%href



%index
peek
Read 1 byte from the buffer
%group
Memory management function
%prm
(p1,p2)
p1 = variable: the name of the variable from which the contents are read
p2 = 0 ~: Buffer index (in Byte)

%inst
Returns the contents of 1 byte (8 bits) in any location on the data memory stored in the variable as a numerical value.
The return value of the function will be an integer value from 0 to 255.

%href
wpeek
lpeek



%index
wpeek
Read 2 bytes from the buffer
%group
Memory management function
%prm
(p1,p2)
p1 = variable: the name of the variable from which the contents are read
p2 = 0 ~: Buffer index (in Byte)

%inst
Returns the contents of 2 bytes (16 bits) anywhere on the data memory stored in the variable as a numerical value.
The return value of the function will be an integer value from 0 to 65535.

%href
peek
lpeek



%index
lpeek
Read 4 bytes from the buffer
%group
Memory management function
%prm
(p1,p2)
p1 = variable: the name of the variable from which the contents are read
p2 = 0 ~: Buffer index (in Byte)

%inst
Returns the contents of 4 bytes (32 bits) stored in a variable anywhere on the data memory as a numerical value.
The return value of the function will be an integer value from 0 to $ ffffffff.

%href
peek
wpeek



%index
callfunc
Calling an external function
%group
Basic input / output functions
%prm
(p1,p2,p3)
p1: Array variable containing parameters
p2: Function address
p3: Number of parameters

%inst
Calls the address specified in p2 as a native function.
Use the value stored in the numeric array variable specified by p1 as the argument of the call. You can specify the number of parameters with p3.
^p
Example:
	a.0=1
	a.1=2
	a.2=3
	res = callfunc( a, proc, 3 )
^p
In the above example, the function at the address indicated by proc is called with the argument (1,2,3).
The return value of the called function becomes the return value of callfunc as it is.
This function is used in special situations where you prepare and call the function address yourself.
Normally you do not need to use it. Also, if the external function call fails, it may freeze or cause unexpected results, so be careful when using it.

%href
#uselib
#func



%index
absf
Returns the absolute value of a real number
%group
Basic input / output functions
%prm
(p1)
p1: Real value to convert to absolute value

%inst
Returns the absolute value of p1 as a real number.
If you want the absolute value of an integer, use the abs function.
%href
abs

%index
abs
Returns the absolute value of an integer
%group
Basic input / output functions
%prm
(p1)
p1: Integer value to convert to absolute value

%inst
Returns the absolute value of p1 as an integer.
If you want the absolute value of a real number, use the absf function.
%href
absf


%index
logf
Returns logarithm
%group
Basic input / output functions
%prm
(p1)
p1 = 0.0 ~ (0.0): Value to find the logarithm

%inst
Returns the logarithmic (log) value of p1 as a real number.
If 0 is specified for p1, infinity (INF) is returned.
%href
expf



%index
expf
Returns the index
%group
Basic input / output functions
%prm
(p1)
p1 = 0.0 ~ (0.0): Value for exponent

%inst
Returns the exponential (exp) value of p1 as a real number.
Returns infinity (INF) if it overflows, 0 if it underflows.
%href
logf



%index
limit
Returns an integer within a certain range
%group
Basic input / output functions
%prm
(p1,p2,p3)
p1: Target value
p2: Minimum value
p3: Maximum value

%inst
Returns the value specified in p1 converted to an integer that falls within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
As a result, the value returned by the limit function is always in the range of p2 to p3.
If the p2 parameter is omitted, the minimum value is not limited.
If the p3 parameter is omitted, the maximum value is not limited.
Use the limitf function to find the range of real numbers.

%href
limitf



%index
limitf
Returns a real number within a certain range
%group
Basic input / output functions
%prm
(p1,p2,p3)
p1: Target value
p2: Minimum value
p3: Maximum value

%inst
Returns the value specified in p1 converted to a real number that fits within the range of p2 to p3.
If the value of p1 is less than p2, the value of p2 is returned, and if the value of p1 is greater than p3, the value of p3 is returned.
As a result, the value returned by the limitf function is always in the range p2 to p3.
If the p2 parameter is omitted, the minimum value is not limited.
If the p3 parameter is omitted, the maximum value is not limited.
Use the limit function to find a range of integers.

%href
limit



%index
varuse
Returns variable usage
%group
Basic input / output functions
%prm
(p1)
p1: Variable to check usage

%inst
Returns the usage status of the variable specified in p1.
varuse is only useful in situations where there is no reality, such as modular or COM object type variables.
Returns 1 if the specified variable is valid, 0 otherwise.
If it is a modular variable, you can check whether it is unused (0), initialized (1), or a clone of another variable (2).
If it is a COM object type, you can check if you have a valid COM object.

%href
newmod
delmod
%port-
Let




%index
libptr
Get the information address of the external call command
%group
Basic input / output functions
%prm
(p1)
p1: Command to look up the address

%inst
Gets the information address of the command specified in p1 and returns it as an integer value.
By specifying an external DLL call command or function as a parameter, you can get the address of the STRUCTDAT structure that stores information about the command.
The STRUCTDAT structure is defined in the HSPSDK as follows:
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
When a COM call command, user-defined command, or user-defined function is specified in p1, the address of the STRUCTDAT structure is obtained in the same way.
The libptr function assists in accessing the internal data used by HSP, so please be sure to have sufficient knowledge about the contents of the information handled here.
In normal use, you don't need to use or remember this function.
^
By referring to the STRUCTDAT structure, it is possible to obtain information such as the address of the externally called DLL and the DLL handle.

%href
varptr
%sample
	#uselib "user32.dll"
	#func MessageBoxA "MessageBoxA" int,sptr,sptr,int

	ladr=libptr( MessageBoxA )
dupptr lptr, ladr, 28; Get STRUCTDAT structure
	lib_id=wpeek(lptr,0)
	mes "LIB#"+lib_id
	mref hspctx,68
	linf_adr=lpeek( hspctx, 832 )
dupptr linf, linf_adr + lib_id * 16,16; Get LIBDAT structure
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
Check the contents of the COM event
%group
COM object manipulation function
%prm
(p1)
p1: Variable name

%inst
Gets the event dispatch ID (DISPID) in the event subroutine of the variable (COM object type) specified in p1.
The variable specified by p1 must be initialized by the come event instruction.
Also, acquisition must always be done in the event subroutine.

%href
comevent
comevarg
%port-
Let


%index
powf
Find power
%group
Basic input / output functions
%prm
(p1, p2)
p1: Bottom (0 or more)
p2: index

%inst
Find the value of p1 raised to the power of p2. The result is given as a real number.
p1 must always be positive. If it is negative, no error will occur, but a non-number (-1. # IND00) will be returned.
p2 can be either positive or negative. You can also specify a real number.

%sample
	repeat 5, -2
mes "10" + cnt + "power is" + powf (10, cnt) + "."
	loop
	stop
%port-
Let



%index
getease
Get the easing value as an integer
%group
Basic input / output functions
%prm
(p1,p2)
p1 (0): Time elapsed value (integer value)
p2 (4096): Maximum time elapsed value (integer value)
%inst
Acquires the calculation result value of the easing function that interpolates the numerical value in the determined range with an arbitrary calculation formula as an integer.
Before using the easing function, it is necessary to set the minimum value, maximum value, and calculation formula output by the setease instruction.
The time elapsed value specified by p1 is an integer value starting from 0, and by specifying up to the maximum time elapsed value (p2 parameter), the calculation result of the easing function is returned.
If you omit the maximum value (p2 parameter), 4096 is used.
Normally, if the elapsed time value is negative, it is considered to be 0. Also, if the elapsed time value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added in the type setting of the calculation formula by the setease instruction, the interpolation loop (repetition) is performed including the values outside the range.)

%href
setease
geteasef
%port-
Let



%index
geteasef
Get the easing value as a real number
%group
Basic input / output functions
%prm
(p1,p2)
p1 (0): Time elapsed value (real value)
p2 (1.0): Maximum time elapsed value (real value)
%inst
Acquires the calculation result value of the easing function that interpolates the numerical value in the determined range with an arbitrary calculation formula as a real number.
Before using the easing function, it is necessary to set the minimum value, maximum value, and calculation formula output by the setease instruction.
The time elapsed value specified by p1 is a real value starting from 0, and by specifying up to the maximum time elapsed value (p2 parameter), the calculation result of the easing function is returned.
If you omit the maximum value (p2 parameter), 1.0 is used.
Normally, if the elapsed time value is negative, it is considered to be 0. Also, if the elapsed time value exceeds the maximum value (p2 parameter), it is treated as the maximum value.
(However, if ease_loop (interpolation loop) is added in the type setting of the calculation formula by the setease instruction, the interpolation loop (repetition) is performed including the values outside the range.)
The geteasef instruction gets the calculation result of the finer easing function. If you want to get a value that is passed as an integer, such as a normal coordinate value, it is faster to use the getease function.

%href
setease
getease
%port-
Let



