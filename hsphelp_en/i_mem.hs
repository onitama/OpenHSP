Special memory assignment to a variable
%group
Special assignment instructions
%prm
Variable name, Source, Type ID
Variable name : Variable to assign memory to
Source : Variable or address to use as source
Type ID : Variable type to assign (omit: current type)

%inst
Assigns special memory to a variable.
The variable name will be associated with memory that is specified in "Source".
^
This is used for dynamically manipulating variables, and for data sharing with external DLLs. It is also a more advanced version of the "dup" instruction.
^p
(Example)
	mref a,b		; Make "a" refer to variable "b"
	mref a,p1,10		; Assign memory address "p1" as a 10-element integer array to "a"
^p
When accessing external DLL functions, memory pointers can be received as a variable.
In that case, memory access using the mref instruction is required, as follows:
^p
	; (Example)
	; "test.dll" sample DLL declaration
	;
	; int GetBuffer(int** buffer, int* size);
	;  buffer  : Address to the buffer (*Receive address in C language)
	;  size    : Size of the buffer (*Receive size in C language)
	;
	#uselib "test.dll"
	#func GetBuffer "GetBuffer" int, var
		
	size = 0
	GetBuffer varptr, varptr(size)
	mref buffer, var, 1
	mes "Buffer Size:"+size
	mes "Buffer Contents:"+buffer(0)
^p
The vartype function returns type IDs.
^
When type ID is omitted, the current variable type is maintained.
When a variable is assigned to Source, the variable type will be the same as the Source's type.
^
When a variable name is assigned to Source, changes to Source will also affect the contents of the assigned variable.
The assigned variable will point to the internal buffer of Source, so changing Source (such as string buffer expansion or array resizing) will break the connection.

%href
dup
dupptr
vartype
varptr
dllfunc
dllproc
%port+
Let
p1,p2
p1=Variable name : Variable name to be assigned
p2=0Å`(0) : Resource ID (Memory content to be assigned)

%inst
Assigns the memory content specified by p2 to the variable specified by p1.
^p
     Value  : Corresponding resource
 --------------------------------------------------
   0Å` 7 : Local parameter #1Å`8(Numeric)
    64   : System variable stat
    65   : System variable refstr
    66   : In-window image data (VRAM)
    67   : Current window information (BMSCR structure)
    68   : HSP system information (HSPCTX structure)
    69   : Palette information
    96Å` : Window ID 0Å` information (BMSCR structure)
^p
The system variables with resource IDs 64 and 65, for example, "mref a,64", will make the variable a equivalent to the system variable stat, allowing you to assign values to it. This allows you to reflect the calculation results within a user-defined command to the system variable and return it to the caller.
^
The in-window image data (VRAM data) of resource 66 becomes an array variable containing the displayed image.
This allows direct access to image data using poke and peek commands.
^
Similarly, resource 67 and later allow direct access to HSP's internal data, but it is usually not necessary to use them. They are prepared for very limited purposes, such as preparing parameters to be passed to DLLs, and most people will not use them.
^
Local parameters are used to obtain the parameter content of newly added commands in user-defined commands (#deffunc).
It is possible to obtain them according to the parameter type (numeric, variable, string).
Local parameter acquisition is provided for compatibility with parameter acquisition methods up to HSP2.x (some resource types are not compatible).
For HSP3.0 and later, it is recommended to use the alias function of user-defined commands (#deffunc).
%href
dup
dupptr
#deffunc
%port+
Let
%portinfo
Only stat/refstr can be used when using HSPLet.
It cannot be used for function arguments, so please use the 3.0 format for receiving values.

%index
newmod
Create a module type variable
%group
Memory management commands
%prm
p1,p2,p3...
p1    : Variable name
p2    : Module name
p3... : Initialization parameters

%inst
Adds an element to the variable specified by p1 as a module type.
If the variable p1 is not a module type, it is initialized as a module type.
If it is already a module type, new elements are added as an array variable.
You can specify a module name that has already been registered in p2, and specify initialization parameters in p3 and later.
Module variables provide a new data storage method that allows you to manage multiple variables and data together.
^p
Example:
	#module a x,y,z
^p
In the above example, the module "a" has three variables: x, y, and z.
A variable with the module type "a" contains all the variables x, y, and z, and can be handled by module processing commands (#modfunc).
^p
Example:
	newmod v,a
^p
In the above example, the variable v for the module "a" is initialized.
The variable v now contains the entire content of the module variables x, y, and z of the module "a".
If an initialization command (#modinit) is prepared for each module, the parameters p3 and later are passed to the initialization command.
^p
Example:
	#module a x,y,z
	#modinit int p1,int p2,int p3
	x=p1:y=p2:z=p3
	return
	#global
	newmod v,a,1,2,3
^p
Elements of module-type variables can be manipulated with the newmod and delmod commands, and complex data can be processed simply with commands such as foreach.

%href
#modfunc
#modinit
#modterm
delmod
foreach

%index
delmod
Delete an element of a module type variable
%group
Memory management commands
%prm
p1
p1 : Variable name

%inst
Deletes the element of the module-type variable specified by p1.
p1 must be a variable that has already been set as a module type.
^p
Example:
	delmod v.1
^p
The delmod command automatically calls the release routine (destructor) if it is defined by the #modterm command.
For details on module-type variables, please refer to the help for the newmod command and the programming manual (hspprog.htm).

%href
#modterm
newmod

%index
memexpand
Reallocate memory block
%group
Memory management commands
%prm
p1,p2
p1 = Variable    : Target variable
p2 = 0Å`(64) : Reallocation size (in 1-byte units)

%inst
Reallocates the memory area that the variable specified by p1 has.
Memory area allocation during assignment, etc. is done automatically by the system, but this is used to explicitly change the size.
Even if reallocation is performed, the previous contents are retained.
Specify the reallocation size in p2. If the value of p2 is less than 64, it is automatically adjusted to 64. If you specify a value smaller than the already allocated size, nothing is done.
The variable specified by p1 must be a type that can dynamically change the amount of memory allocated, such as a string type (str).
An error will occur if the reallocation cannot be performed for the specified type.

%href
memcpy
memset
alloc

%index
ldim
Create a label type array variable
%group
Memory management commands
%prm
p1,p2...
p1=Variable : Variable name to assign the array to
p2=0Å`  : Maximum number of elements

%inst
Creates a label-type array variable.
The parameters specify the maximum number of elements, just like the dim command.
^p
Example :
	ldim a,100 ; Variable a secures a label type array from a(0) to a(99)
^p
It is possible to secure multi-dimensional arrays in the same way as the dim command.
Multi-dimensional arrays can be secured up to 4 dimensions.
The ldim command can be defined/redefined anywhere in the script.
Also, when an array variable is created, all contents are cleared to an "undefined" state.
^
Å¶This command is defined as a macro in hspdef.as.

%href
dim
sdim
ddim
dimtype

%index
newlab
Initialize label type variable
%group
Memory management commands
%prm
p1,p2
p1=Variable    : Variable name to initialize
p2=Reference source  : Label or option to be referenced

%inst
Initializes a label-type variable with the specified label assigned.
Specify the variable name to be initialized in p1.
Specify the reference source of the label to be stored in the variable in p2.
If you specify a label in p2, the location indicated by the label is used as the reference source.
In this case, the operation is the same as when "variable=*label name" is written.
If you specify a numerical value in p2, the following operation is performed.
^
     Value  : Label to be referenced
 --------------------------------------------------
      0    Refers to the program location to be executed next
      1    Refers to the program location to be executed after skipping the next statement
^
If p2 is 1, it is assumed that a return command exists after the newlab command.
The "location next to the newlab command and return command" will be referenced.
The newlab command is for initializing label-type variables with special values.
If you simply want to save a label, you can use a normal assignment statement.

%href
ldim
