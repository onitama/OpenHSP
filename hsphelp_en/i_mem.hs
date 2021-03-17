;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Built-in instructions
%ver
3.4
%note
ver3.4 standard instruction
%date
2009/08/01
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli




%index
alloc
Allocate a buffer
%group
Memory management instructions
%prm
p1,p2
p1 = variable: variable name to allocate the buffer
p2 = 1 to (64): Buffer size (in Byte)

%inst
Create a buffer in memory and assign it to a variable.
The assigned variable will be of type string.
^
This instruction is provided for compatibility with past HSP instructions.
In general, it is recommended to use the sdim instruction.
^
* This instruction is defined in hspdef.as as a macro.

%href
sdim
memexpand
%port+
Let



%index
dim
Create array variable
%group
Memory management instructions
%prm
p1,p2...
p1 = variable: variable name to which the array is assigned
p2 = 0 ~: Maximum of elements

%inst
Create an array variable with arbitrary elements.
^p
Example:
	dim a,20
^p
In the above example, 20 elements of variable a, that is, "a (0)" to "a (19)" are reserved in advance.
^
It is also possible to create a multidimensional array by increasing the parameters.
^p
Example:
dim a, 10,5: Variable a is a two-dimensional array
a (0,0) = 1: Substitute 1 for element (0,0)
a (1,0) = 2: Substitute 2 for element (1,0)
a (0,1) = 3: Substitute 3 for element (0,1)
^p
In the above example, you can use from a (0,0) to a (9,4).
A multidimensional array can be secured up to 4 dimensions.
^
The dim instruction can be defined and redefined anywhere in the script.
Also, when you create an array variable, all the contents are cleared to 0.

%href
sdim
ddim
ldim
dimtype
%port+
Let




%index
dimtype
Create an array variable of the specified type
%group
Memory management instructions
%prm
p1,p2,p3...
p1 = variable: variable name to which the array is assigned
p2 = type type: variable type
p3 = 0 ~: Maximum of elements

%inst
Create an array variable with arbitrary elements.
It behaves like the dim instruction, but dimtype allows you to specify the type of a variable.
For p2, you must specify a type type value that indicates the variable type.
The type type value can be obtained from the type name string with the vartype function.
^p
Example:
	dimtype a,vartype("double"),20
^p
In the above example, 20 elements of the real number type variable a, that is, "a (0)" to "a (19)" are reserved in advance.
It is possible to secure a multidimensional array in the same way as the dim instruction.
A multidimensional array can be secured up to 4 dimensions.

%href
dim
sdim
ddim
ldim
vartype
%port+
Let



%index
poke
Write 1 byte to the buffer
%group
Memory management instructions
%prm
p1,p2,p3
p1 = variable: variable name to which the buffer is allocated
p2 = 0 ~: Buffer index (in Byte)
p3 (0): Value to be written to the buffer or character string (in Byte)

%inst
Rewrites the contents of 1 byte anywhere on the data memory stored in the variable.
^
Writes the value of p3 to the index location specified by p2 on the buffer of the variable specified by p1. The value is a 1-byte (8-bit) value from 0 to 255.
^
If a string is specified in p3, the string data is expanded in memory and the length of the written string is returned in strsize.


%href
wpoke
lpoke
%port+
Let



%index
wpoke
Write 2 bytes to the buffer
%group
Memory management instructions
%prm
p1,p2,p3
p1 = variable: variable name to which the buffer is allocated
p2 = 0 ~: Buffer index (in Byte)
p3 = 0 to (0): Value to be written to the buffer (16-bit integer value)

%inst
Rewrites the contents of 2 bytes anywhere in the data memory stored in the variable.
^
Writes the value of p3 to the index location specified by p2 on the buffer of the variable specified by p1. The value is a 2-byte (16-bit) value from 0 to 65535.

%href
poke
lpoke
%port+
Let



%index
lpoke
Write 4 bytes to the buffer
%group
Memory management instructions
%prm
p1,p2,p3
p1 = variable: variable name to which the buffer is allocated
p2 = 0 ~: Buffer index (in Byte)
p3 = 0 to (0): Value to be written to the buffer (32-bit integer value)

%inst
Rewrites the contents of 4 bytes anywhere on the data memory stored in the variable.
^
Writes the value of p3 to the index location specified by p2 on the buffer of the variable specified by p1. The value is a 4-byte (32-bit) value from 0 to $ ffffffff.

%href
poke
wpoke
%port+
Let




%index
sdim
Create a string type array variable
%group
Memory management instructions
%prm
p1,p2,p3...
p1 = variable: variable name to which the array is assigned
p2 = 1 ~: Default number of characters
p3 = 0 ~: Maximum of elements

%inst
Create a string type array variable.
The difference from the dim instruction is that the parameter of p2 is "default number of characters in the string", and the maximum number of actual array elements is entered after the parameter of p3.
^p
Example:
sdim a, 5000; Variable a pre-allocates memory for 5000 characters
^p
In the above example, the variable a reserves memory for 5000 characters and is not an array variable.
The default number of characters is that by specifying the memory to be reserved in advance, extra processing will not be required during automatic expansion.
If the default number of characters is small, the string buffer will be reallocated many times each time a long string is assigned, which may reduce efficiency.
^
In the case of a multidimensional array, it is possible to secure up to 4 dimensions separately from the number of characters.
The sdim instruction can be defined and redefined anywhere in the script.
Also, when you create an array variable, all the contents are cleared to 0.

%href
dim
ddim
ldim
dimtype
%port+
Let



%index
ddim
Create a real array variable
%group
Memory management instructions
%prm
p1,p2...
p1 = variable: variable name to which the array is assigned
p2 = 0 ~: Maximum of elements

%inst
Create a real array variable.
The parameter specifies the maximum number of elements, similar to the dim instruction.
^p
Example:
ddim a, 100; Variable a allocates a real type array from a (0) to a (99)
^p
It is possible to secure a multidimensional array in the same way as the dim instruction.
A multidimensional array can be secured up to 4 dimensions.
The ddim instruction can be defined and redefined anywhere in the script.
Also, when you create an array variable, all the contents are cleared to 0.
^
* This instruction is defined in hspdef.as as a macro.

%href
dim
sdim
ldim
dimtype
%port+
Let



%index
memcpy
Copy of memory block
%group
Memory management instructions
%prm
p1,p2,p3,p4,p5
p1: Copy destination variable
p2: Copy source variable
p3: Copy size (1 byte unit)
p4: Copy destination variable memory offset (default = 0)
p5: Copy source variable memory offset (default = 0)

%inst
In the memory area allocated to the variable specified by p1
Copies the contents of memory allocated to the variable specified by p2.
The copy size (1 byte unit) is specified by p3.
High-speed memory copy can be performed when a large area is allocated to a variable.
With p4 and p5, the copy destination and copy source start positions can be adjusted in 1-byte units.
If a specification that exceeds the area reserved by the variable is specified, a buffer overflow error will occur.


%href
memset
memexpand
%port+
Let


%index
memset
Clear memory block
%group
Memory management instructions
%prm
p1,p2,p3,p4
p1 = variable: variable to write to
p2 = 0 to 255 (0): Value to clear (1 byte)
p3 = 0 to (0): Clear size (1 byte unit)
p4 = 0 to (0): Variable memory offset of write destination

%inst
Fill the memory block with a fixed value of 1 byte.
Writes the value specified by p2 to the memory area allocated to the variable specified by p1 by the size of p3. This is useful when you want to write the same value in a large area. With p4, you can adjust the memory start position in 1-byte units.
If a specification that exceeds the area reserved by the variable is specified, a buffer overflow error will occur.

%href
memcpy
memexpand
%port+
Let



%index
dup
Create clone variable
%group
Special assignment instruction
%prm
Variable name 1, variable name 2
Variable name 1: Variable name to clone
Variable name 2: Clone source variable name

%inst
Create a variable that points to the memory of the source variable.
The clone variable will now function as a variable for referencing the memory information of the clone source.
^
If the type of the clone source is changed, the array is expanded, or the string buffer is expanded, the clone operation will not be performed.
Note that the clone is valid only until the source variable is assigned.
The dup instruction remains a feature for maintaining compatibility with previous versions and for low-level variable buffer operations.
There is also a dupptr instruction that creates a clone variable directly from the memory address.
We do not recommend the dup instruction for beginners.
%href
mref
dupptr
%port+
Let



%index
dupptr
Create clone variable from pointer
%group
Special assignment instruction
%prm
Variable name, p1, p2, p3
Variable name: Variable name to clone
p1 = 0 ~: Memory address of clone source
p2 = 0 ~: Memory size of clone source
p3 = 1 to (4): Clone variable type specification

%inst
Creates a variable that points to the specified address pointer.
Clone variables will now act as numeric array variables for referencing information in memory.
^
You can specify the type of the clone variable created by p3.
The value of p3 is the same as the value indicating the type obtained by the vartype function. If p3 is omitted, it will be 4 (integer type).
^
Clone variables cannot detect changes in the location of the memory they are pointing to.
For example, even if you point to the memory address of the data stored in a variable, it cannot be referenced correctly if the variable's type or contents are updated and the memory location changes.
Use it only for temporary memory reference, and handle it with care. The dupptr instruction is provided as a function for performing memory references and low-level variable buffer operations exchanged by external functions such as DLLs.
We do not recommend the dupptr instruction for beginners.

%href
mref
dup



%index
mref
Allocate special memory to variables
%group
Special assignment instruction
%prm
p1,p2
p1 = variable name: variable name to be assigned
p2 = 0 to (0): Resource ID (memory contents to be allocated)

%inst
Allocates the memory contents specified in p2 to the variables specified in p1.
^p
     Value: Corresponding resource
 --------------------------------------------------
   0 to 7: Local parameter # 1 to 8 (numerical value)
    64: System variable stat
    65: System variable refstr
    66: Image data in the window (VRAM)
    67: Current window information (BMSCR structure)
    68: HSP system information (HSPCTX structure)
    69: Palette information
    96 ~: Information of window ID0 ~ (BMSCR structure)
^p
For the system variable of resource ID 64,65, for example, if "mref a, 64" is set, the variable a becomes equivalent to the system variable stat, and a value can be assigned. This makes it possible to reflect the calculation results in user-defined instructions in system variables and return them to the caller.
^
The image data (VRAM data) in the window of resource 66 is an array variable containing the displayed image.
This makes it possible to directly access the image data with the poke, peek commands, etc.
^
Resources 67 and above will also give you direct access to HSP internal data, but you usually don't need to use it. It is provided for very limited purposes, such as preparing parameters for passing to a DLL, and should not be used by most people.
^
The local parameter is for getting the parameter contents of the newly added instruction by the user-defined instruction (#deffunc).
It can be obtained according to the parameter type (number, variable, string).
Local parameter acquisition is provided for compatibility with parameter acquisition methods up to HSP2.x. (Some resource types are not compatible.)
In HSP3.0 or later, it is recommended to use the alias function of the user-defined instruction (#deffunc).
%href
dup
dupptr
#deffunc
%port+
Let
%portinfo
Only stat / refstr can be used during HSPLet.
It cannot be used as a function argument, so please use the 3.0 format.


%index
newmod
Creating modular variables
%group
Memory management instructions
%prm
p1,p2,p3...
p1: Variable name
p2: Module name
p3 ...: Initialization parameters

%inst
Adds an element with the variable specified in p1 as a module type.
If the variable in p1 is not modular, it will be initialized as modular.
If it is already modular, add new elements as array variables.
You can specify the module name already registered in p2, and specify the initialization parameters after p3.
Module variables provide a new way of storing data that allows you to manage multiple variables and data together.
^p
Example:
	#module a x,y,z
^p
In the above example, a module called a has three variables, x, y, and z.
A variable with the module type a contains all the variables x, y, and z, and can be handled by a module processing instruction (#modfunc).
^p
Example:
	newmod v,a
^p
The above example initializes the variable v for module "a".
Now, the variable v contains the entire contents of the module variables x, y, z of the module "a".
If an initialization instruction (#modinit) is prepared for each module, the parameters after p3 are passed to the initialization instruction.
^p
Example:
	#module a x,y,z
	#modinit int p1,int p2,int p3
	x=p1:y=p2:z=p3
	return
	#global
	newmod v,a,1,2,3
^p
Modular variables can be used to manipulate elements with newmod and delmod instructions, and can also be used with foreach instructions to handle complex data in a concise manner.


%href
#modfunc
#modinit
#modterm
delmod
foreach



%index
delmod
Delete elements of modular variables
%group
Memory management instructions
%prm
p1
p1: Variable name

%inst
Deletes the element of the modular variable specified in p1.
p1 must be a variable that is already set as modular.
^p
Example:
	delmod v.1
^p
The delmod instruction calls automatically if a release routine (destructor) is defined by the #modterm instruction.
For more information on modular variables, see the newmod instruction help and programming manual (hspprog.htm).


%href
#modterm
newmod



%index
memexpand
Reallocate memory block
%group
Memory management instructions
%prm
p1,p2
p1 = variable: variable of interest
p2 = 0 to (64): Reserved size (1 byte unit)

%inst
Reallocates the memory area of the variable specified by p1.
The system automatically allocates a memory area such as when assigning, but it is used when explicitly changing the size.
Even if you re-allocate, the previous contents will be retained.
In p2, specify the reallocated size. If the value of p2 is less than 64, it will be automatically adjusted to 64. If you specify a value smaller than the size already reserved, nothing is done.
The variable specified by p1 must be of a type that can dynamically change the memory allocation amount, such as the string type (str).
If the type cannot be reallocated, an error will occur.

%href
memcpy
memset
alloc



%index
ldim
Create a label type array variable
%group
Memory management instructions
%prm
p1,p2...
p1 = variable: variable name to which the array is assigned
p2 = 0 ~: Maximum of elements

%inst
Create a label type array variable.
The parameter specifies the maximum number of elements, similar to the dim instruction.
^p
Example:
ldim a, 100; Variable a allocates a labeled array from a (0) to a (99)
^p
It is possible to secure a multidimensional array in the same way as the dim instruction.
A multidimensional array can be secured up to 4 dimensions.
The ldim instruction can be defined and redefined anywhere in the script.
Also, when you create an array variable, all the contents are cleared to the "undefined" state.
^
* This instruction is defined in hspdef.as as a macro.

%href
dim
sdim
ddim
dimtype




%index
newlab
Initialize label type variable
%group
Memory management instructions
%prm
p1,p2
p1 = variable: variable name to initialize
p2 = Source: Referenced label or option

%inst
Initializes a variable of label type to which the specified label is assigned.
In p1, specify the variable name to be initialized.
In p2, specify the source of the label stored in the variable.
If you specify a label for p2, the reference source is the location indicated by the label.
In this case, the operation is the same as when "variable = * label name" is described.
If you specify a numerical value for p2, the following operations are performed.
^
     Value: Referenced label
 --------------------------------------------------
      0 Refer to the position of the program to be executed next
      1 Refers to the program position executed after skipping the next 1 state
^
If p2 is 1, it is assumed that the return instruction exists after the newlab instruction.
"The position next to the newlab instruction and return instruction" will be referenced.
The newlab instruction is for initializing a label type variable with a special value.
If you want to save a simple label, you can use a normal assignment statement.

%href
ldim




