Variable,p1
Variable : Variable to be assigned
p1 : Address of the memory to be assigned

%inst
Assigns the specified memory address to the variable.
The variable will function as a reference to the memory location.
^
mref is a function for advanced users who want to perform direct memory access.
This is a function for compatibility with older versions of HSP and for low-level variable buffer manipulation.
It should not be used casually as it can lead to errors such as overwriting important system memory.
The target address must be in a readable or writable memory area.
The programmer is responsible for ensuring that the specified address is valid and accessible.
^
For example, if a variable containing data is assigned to mref, the value will be copied into the variable. After that, even if the original variable is updated, the variable assigned to mref will not be updated. mref only copies the value at the time of assignment.
^
mref does not allocate any memory. You must ensure that there is enough memory at the specified address before using mref.
Also, mref does not check the type or size of the memory at the specified address. This means that you can use mref to access memory of any type or size. However, you must be careful to ensure that you are accessing the memory correctly.
^
Due to the nature of the command, even if the assignment is successful, it may not work correctly if the memory is protected by the OS.

%href
dup
dupptr
%port+
Let



%index
memexpand
メモリブロックを拡張
%group
メモリ管理命令
%prm
p1,p2
p1=変数 : 拡張する変数名
p2=バイト数

%inst
変数に割り当てられているメモリブロックを拡張します。
p1で指定された変数に割り当てられているメモリ領域を、p2で指定されたサイズだけ拡張します。
(変数の型によって、拡張の単位が異なります。)
この命令は、文字列バッファや配列要素のサイズを動的に変更したい場合に有効です。
変数が確保している領域を越える指定がされている場合は、バッファオーバーフローのエラーになります。

%href
memcpy
memset
%port+
Let
%index
p1,p2
p1=Variable name : Variable name to be assigned
p2=0〜(0) : Resource ID (Memory content to be assigned)

%inst
Assigns the memory content specified by p2 to the variable specified by p1.
^p
     Value  : Corresponding Resource
 --------------------------------------------------
   0〜 7 : Local Parameter #1〜8(Numeric)
    64   : System Variable stat
    65   : System Variable refstr
    66   : In-Window Image Data (VRAM)
    67   : Current Window Information (BMSCR structure)
    68   : HSP System Information (HSPCTX structure)
    69   : Palette Information
    96〜 : Window ID 0〜 Information (BMSCR structure)
^p
For system variables with resource IDs 64 and 65, for example, if you use "mref a,64", the variable a becomes equivalent to the system variable stat, and you can assign a value to it. This allows you to reflect the calculation results in user-defined commands to the system variables and return them to the caller.
^
The in-window image data (VRAM data) of resource 66 is an array variable containing the displayed image. This allows direct access to image data using poke and peek commands.
^
Similarly, resources 67 and later allow direct access to HSP's internal data, but there is usually no need to use them. They are provided for very limited purposes, such as preparing parameters to pass to DLLs, and most people should not need to use them.
^
Local parameters are for acquiring the parameter contents of commands newly added in user-defined commands (#deffunc).
It is possible to acquire them according to the parameter type (numeric, variable, string).
Local parameter acquisition is provided for compatibility with parameter acquisition methods up to HSP2.x. (Some resource types are not compatible.)
In HSP3.0 and later, it is recommended to use the alias function of user-defined commands (#deffunc).
%href
dup
dupptr
#deffunc
%port+
Let
%portinfo
Only stat/refstr can be used for HSPLet.
It cannot be used as a function argument, so please use the 3.0 format for receiving.

%index
newmod
Create module-type variable
%group
Memory Management Commands
%prm
p1,p2,p3...
p1    : Variable name
p2    : Module name
p3... : Initialization parameters

%inst
Adds elements to the variable specified by p1 as a module type.
If the variable p1 is not a module type, it is initialized as a module type.
If it is already a module type, a new element is added as an array variable.
You can specify a module name that has already been registered in p2, and specify initialization parameters in p3 and later.
Module variables provide a new data storage method that allows you to manage multiple variables and data together.
^p
Example :
	#module a x,y,z
^p
In the example above, the module a has three variables: x, y, and z.
A variable with the module type a contains all the variables x, y, and z, and can be handled by module processing commands (#modfunc).
^p
Example :
	newmod v,a
^p
In the example above, the variable v for the module "a" is initialized.
Now the variable v will contain the entire contents of the module variables x, y, and z that the module "a" has.
If an initialization command (#modinit) is prepared for each module, the parameters in p3 and later are passed to the initialization command.
^p
Example :
	#module a x,y,z
	#modinit int p1,int p2,int p3
	x=p1:y=p2:z=p3
	return
	#global
	newmod v,a,1,2,3
^p
Module-type variables can be operated on with newmod and delmod commands, and complex data can be processed simply with the foreach command.

%href
#modfunc
#modinit
#modterm
delmod
foreach

%index
delmod
Delete element of module-type variable
%group
Memory Management Commands
%prm
p1
p1 : Variable name

%inst
Deletes the element of the module-type variable specified by p1.
p1 must be a variable that has already been set as a module type.
^p
Example :
	delmod v.1
^p
The delmod command automatically calls the release routine (destructor) if it is defined by the #modterm command.
For more information about module-type variables, see the help for the newmod command and the programming manual (hspprog.htm).

%href
#modterm
newmod

%index
memexpand
Reallocate memory block
%group
Memory Management Commands
%prm
p1,p2
p1 = Variable    : Target variable
p2 = 0〜(64) : Reallocation size (in bytes)

%inst
Reallocates the memory area of the variable specified by p1.
The system automatically allocates memory area at the time of assignment, etc., but this command is used to explicitly change the size.
Even if reallocation is performed, the previous contents are retained.
Specify the reallocation size in p2. If the value of p2 is less than 64, it will be automatically adjusted to 64. If you specify a value smaller than the already allocated size, nothing will be done.
The variable specified by p1 must be a type that can dynamically change the amount of memory allocated, such as a string type (str).
If the type cannot be reallocated, an error will occur.

%href
memcpy
memset
alloc

%index
ldim
Create label-type array variable
%group
Memory Management Commands
%prm
p1,p2...
p1=Variable : Variable name to assign array to
p2=0〜  : Maximum number of elements

%inst
Creates a label-type array variable.
The parameters specify the maximum number of elements, similar to the dim command.
^p
Example :
	ldim a,100 ; Variable a secures a label-type array from a(0) to a(99)
^p
It is possible to secure multi-dimensional arrays in the same way as the dim command.
Multi-dimensional arrays can be secured up to 4 dimensions.
The ldim command can be defined and redefined anywhere in the script.
Also, when an array variable is created, all contents are cleared to an "undefined" state.
^
※This command is defined as a macro within hspdef.as.

%href
dim
sdim
ddim
dimtype

%index
newlab
Initialize label-type variable
%group
Memory Management Commands
%prm
p1,p2
p1=Variable    : Variable name to initialize
p2=Reference source  : Label or option to be referenced

%inst
Initializes a label-type variable to which the specified label is assigned.
Specify the variable name to be initialized in p1.
Specify the reference source of the label to be saved in the variable in p2.
If you specify a label in p2, the location indicated by the label is used as the reference source.
In this case, the behavior is the same as when "variable = *label name" is written.
If you specify a number in p2, the following behavior occurs.
^
     Value  : Label to be referenced
 --------------------------------------------------
      0    Refer to the program position to be executed next
      1    Refer to the program position to be executed after skipping the next 1 state
^
If p2 is 1, it is assumed that there is a return command after the newlab command.
"The position next to the newlab command and return command" will be referenced.
The newlab command is for initializing label-type variables with special values.
If you simply want to save a label, you can use a normal assignment statement.

%href
ldim
