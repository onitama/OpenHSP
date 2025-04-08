;
;	HELP source file for HSP help manager
;	(Lines beginning with ";" are treated as comments)
;

%type
Built-in command
%ver
3.7
%note
ver3.7 standard command
%date
2023/08/22
%author
onitama
%url
https://hsp.tv/
%port
Win
Cli


%index
#define
Register a new macro
%group
Preprocessor command
%prm
Macro name Macro definition
%inst
Registers a keyword specified by the macro name with the preprocessor so that it can be replaced with the specified definition.
#define macros are for personal use to customize scripts and are not intended for beginners. The functions described here are not frequently used, so please refer to them only when necessary.
#define macros basically register replacement strings.
^p
Example :
	#define hyouji mes
	hyouji "AAAAA..."
		↓(after expansion)
	mes "AAAAA..."
^p
By putting "global" immediately after the #define command, you can create a macro that can be used permanently in all modules.
^p
Example :
	#module
	#define global test 0x1234
	#global
	a=test   ; 0x1234 is assigned to a
^p
Normally, if #define is defined within a module, the same name will not be recognized in other modules or in the global area.
By including the global specification, you can replace defined names with macros in all subsequent locations.
^
In addition to simple replacement macros, argument expansion is possible.
Arguments are specified by argument names %1, %2, %3... enclosed in parentheses after the macro name.
Arguments must always be specified with "%number", and the numbers must be written in order from 1. Note that you cannot specify symbolic names as in C or C++ preprocessors.
^p
Example :
	#define hyouji(%1) mes "prm="+%1
	hyouji "AAAAA..."
		↓(after expansion)
	mes "prm="+"AAAAA..."
^p
It is also possible to set an initial (default) value for the argument.
^p
Example :
	#define hyouji(%1="PRM=",%2=123) mes %1+%2
	hyouji "AAA",a
	hyouji "BBB"
	hyouji ,b
		↓(after expansion)
	mes "AAA"+a
	mes "BBB"+123
	mes "PRM="+b
^p
The initial (default) value is a value that is automatically complemented when the macro is omitted. If the initial value is omitted, it will not be complemented.
When specifying macro arguments, enclose them in parentheses when specifying them with #define, but specify them without parentheses when actually using them.
^p
	#define hyouji(%1) mes "prm="+%1
	hyouji("AAAAA...")
^p
Note that the above description will result in an error.
However, it is possible to use the following parenthesized notation by using the ctype option.
^p
	#define ctype kansu(%1) %1*5+1
	a=kansu(5)
^p
This option is effective when you want to use a macro for something other than a command part, such as a calculation formula. At first glance, it behaves like a function in C language, etc., but please note that the application range is narrow because it is actually just replacing with a macro.
Since this description method is different from the original HSP syntax, we do not recommend using it for purposes other than customizing your own style.
^
It is possible to set keywords that perform special actions when expanding macros.
These special keywords are mainly for sharing parameters between different macros or for implementing nested structures using stacks.
^p
	#define start(%1) %tstart %s1 mes "START"
	#define owari %tstart mes %o
^p
The special expansion macros specified here are "%s1" and "%o".
A sample using this will be expanded as follows.
^p
	start "OK" → mes "START"
	owari → mes "OK"
^p
In this way, it becomes possible to share data between different macros.
The following are the types and functions of special expansion macros.
^p
   Macro : Function
 ----------------------------------------------------------------
     %t   : Set tag name
     %n   : Generate a unique label name
     %i   : Generate a unique label name and push it onto the stack
     %o   : Retrieve a string pushed onto the stack
     %p   : Retrieve a string pushed onto the stack (maintain stack)
     %s   : Push an argument parameter onto the stack
     %c   : Perform a line feed
^p
Special expansion macros are expressed by "%" followed by a single alphabetic character + a parameter.
In order to identify it from subsequent keywords, put a single space after the special expansion macro. Everything up to the part including the space, such as "%tabc aaa", is determined to be a special expansion macro.
Special expansion macros have a general stack (First In Last Out).
This stack can be shared by macros with the same tag name.
The tag name is specified by following "%t" with a single-byte alphabetic character of 16 characters or less, such as "%ttag name". In the previous example, "start" specified as "%tstart" is the tag name.
"%s" is a special expansion macro for pushing argument parameters onto the stack.
If "%s1" is specified, the parameter of "%1" is pushed onto the stack.
To retrieve a string pushed onto the stack, use "%o".
"%o" retrieves and expands the string pushed onto the stack. Because it is a stack, the last thing pushed is retrieved first.
If "%o0" is specified, the stack is retrieved, but the string is not expanded (stack retrieval only).
"%p" retrieves only the contents without returning the stack. "%p0" expands the contents of the stack to be retrieved next. "%p1" retrieves another deeper stack. You can specify "%p0" to "%p9" onwards.
The following is an example of label generation.
^p
	#define start %tstart *%i
	#define owari %tstart await 20:stick a:if a=0  :  goto
*%o
^p
A sample using this will be expanded as follows.
^p
	start → *_start_0000
	owari → await 20:stick a:if a=0 : goto *_start_0000
^p
"%i" generates a unique label name that does not overlap with others and pushes it onto the stack. If "%i0" is specified, the label name is pushed onto the stack, but it is not expanded.
Also, "%n" generates and expands a unique label name, but does not push it onto the stack.
In the above example, a new label is created by label name generation to realize a loop structure. By using this method, it is possible to construct a loop structure in which label names do not overlap even if they are nested.
In addition, macros (tag names) whose stacks have not been completely retrieved in one source script file will report an error during compilation.
Be sure to create a macro command configuration in which all stacks are retrieved and finished.
^
The standard definition macros while〜wend, do〜until, and for〜next added in HSP ver2.6 are created by special expansion macros.
^
In special situations, it is possible to expand with line feeds inserted by "%c".
The line is divided and expanded at the "%c" part. It is mainly used for defining macros that expand into multiple preprocessor statements.
However, not all preprocessors currently support macro expansion. Overusing it can make it more difficult to see, so please use it after fully understanding it.
^p
Example :
	#define def(%1,%2) #deffunc %1@ %c mes@hsp %2@
	def test,a
	return
	def test2,a
	return
^p
%href
#const
#ifdef
#ifndef
%port+
Let


%index
#func
External DLL call command registration
%group
Preprocessor command
%prm
New name "Function name" Type name 1,…

%inst
Registers a new command for calling an external DLL.
The external DLL name must be specified in advance with the #uselib command.
Write the new name, function name, and type separated by spaces.
By putting "global" immediately before the new name, you can create a command that can be used permanently in all modules.
^
The function name can be enclosed in double quotes, such as "function name", to describe the complete export name of the DLL.
If it is not enclosed in double quotes, it will be converted to a name based on the VC++ export convention, such as "_function name@16".
^
The type name describes the details of the argument.
Similar to the #deffunc command, specify the argument type separated by ",".
There are no restrictions on the number of arguments or the order of the types.
The following strings can be used as argument types.
^p
     Type     :  Contents
 ------------------------------------------------------------------
    int     :  Integer value (32bit)
    var     :  Variable data pointer (32bit)
    str     :  String pointer (32bit)
    wstr    :  Unicode string pointer (32bit)
    sptr    :  Pointer integer value or string pointer (32bit)
    wptr    :  Pointer integer value or unicode string pointer (32bit)
    double  :  Real value (64bit)
    float   :  Real value (32bit)
    pval    :  PVal structure pointer (32bit)
    comobj  :  COMOBJ type variable data pointer (32bit)
(*) bmscr   :  BMSCR structure pointer (32bit)
(*) prefstr :  System variable refstr pointer (32bit)
(*) pexinfo :  EXINFO structure pointer (32bit)
(*) nullptr :  Null pointer (32bit)
^p
Items with (*) are parameters that are automatically passed without the need to specify them as arguments.
The following is an example of executing with four arguments specified.
^p
Example :
	#uselib "test.dll"
	#func test "_func@16" var,int,int,int
	test a,1,2,3    ; test.dll's func(&a,1,2,3) is called
^p
If a numerical value is specified for the type, arguments compatible with the DLL type specification of ver2.5 and later will be automatically set. However, there are some incompatible specified values. In that case, an error will be reported at compile time.
^
The following points require attention to compatibility with ver2.5.
^p
・The BMSCR structure can only refer to the fields from flag to color.
・The PVal structure is compatible with the PVAL2 structure of ver2.5.
・The flag field (type type value) and len field (array information) of the PVal structure cannot be rewritten on the DLL side.
・You cannot access string type array variables from the PVal structure.
^p
In other respects, information equivalent to HSP ver2.5 is passed.
^
By putting "onexit" before the function name, it can be registered as an exit call function.
^
Example:
	#func test onexit "_func@16" str,int,int
^p
In the above example, "_func@16" is automatically called when the application exits.

%href
#uselib
#cfunc
%port+
Let
%portinfo
In HSPLet, a method with the same name as the function will be called. (See the HSPLet manual for details.)

%index
#cfunc
Register external DLL call function
%group
Preprocessor instruction
%prm
New name "Function name" Type name1,...

%inst
Registers a new function for calling an external DLL.
You must specify the external DLL name with the #uselib instruction beforehand.
Write the new name, function name, and type, separated by spaces.
By placing "global" before the new name, you can create an instruction that can be permanently used in all modules.
^
The function name can be enclosed in double quotes, like "Function name", to describe the complete export name of the DLL.
If not enclosed in double quotes, it will be converted to a name based on the VC++ export convention, such as "_FunctionName@16".
^
Specify the details of the arguments in the type name.
The argument parameters are the same as those used in the #func instruction.
See the #func reference for more information.
The new name registered by the #cfunc instruction can be written in an expression as a function.
^p
Example:
	#uselib "test.dll"
	#cfunc test "_func@16" var,int,int,int
	res=test(a,1,2,3)  ; func(&a,1,2,3) in test.dll is called
^p
The integer value (32bit int) acquired as a result of the external call is returned as the return value of the registered function.
In HSP2.5 compatible calls, the value returned to the system variable stat is used as the return value of the function.

%href
#uselib
#func
%port+
Let
%portinfo
In HSPLet, a method with the same name as the function will be called. (See the HSPLet manual for details.)

%index
#include
Combine additional script files
%group
Preprocessor instruction
%prm
"filename"
"filename" : Script file name to combine

%inst
Specifies the file to include.
The file specified here is combined with the original source script and compiled.
Used to specify additional script files such as runtime specifications and modules.
The #use instruction is also available to simplify the specification of the script file to be combined.
%href
#addition
#use
%port+
Let

%index
#addition
Combine if additional script file exists
%group
Preprocessor instruction
%prm
"filename"
"filename" : Script file name to combine

%inst
Specifies the file to include.
The file specified here is combined with the original source script and compiled.
The behavior is the same as the #include instruction, but #addition continues without stopping as an error even if the file does not exist.
Used to combine additional definition files such as runtime specifications and modules.
%href
#include
#use
%port+
Let

%index
#use
Simplify the specification of additional script files
%group
Preprocessor instruction
%prm
name,...
name : Additional script name (excluding extension)

%inst
Simplifies the specification of additional script files.
Like the #include instruction, it is used to specify additional script files such as runtime specifications and modules, but the #use instruction allows you to specify only the name excluding the extension, such as "hsp3dish", instead of a string like ""hsp3dish.as"".
A script file with the extension ".as" appended to the name specified by the name parameter is combined.
If a file with the extension ".as" does not exist, a file with the extension ".hsp" is searched.
If a file with either extension does not exist, an error occurs.
The name of the name parameter must be written in the same rules as variables.
In other words, "#use hsp3dish" and "#include "hsp3dish.as"" have exactly the same behavior.
Also, the #use instruction allows you to write multiple names in succession by separating them with the "," symbol.
^p
Example:
	#use strict,hgimg4,layer_fade
^p
In the above example, the files "strict.as", "hgimg4.as", and "layer_fade.as" will be combined.
%href
#addition
#use
%port+
Let

%index
#uselib
Specify external DLL
%group
Preprocessor instruction
%prm
"filename"
"filename" : External DLL file name

%inst
Specifies the file name of the external DLL to be called from HSP.
The DLL file name must be written completely, including the extension.
If the file name is omitted, the DLL name will be specified from the script at runtime to perform the link.

%href
#func
%sample
	#uselib "user32.dll"
	#func MessageBoxA "MessageBoxA" int,sptr,sptr,int
	MessageBoxA hwnd,"test message","title",0
%port+
Let
%portinfo
In HSPLet, a class with the same name as the specified DLL is loaded.
The loaded class name is the same as the DLL name, without the ".dll" or ".hpi" extension.
For example, using winmm.dll will load the winmm class.
If you have created your own extension library class, place the JAR file in the lib folder.
The compiler will automatically recognize it and perform a check during compilation.

%index
#global
End of module
%group
Preprocessor instruction
%prm
%inst
Ends the module section and returns to the normal program area.
See the #module instruction for more details about modules.
%href
#module
#deffunc
%port+
Let

%index
#module
Start of module
%group
Preprocessor instruction
%prm
Module name Variable name1,...
Module name : New module name
Variable name : Module variable name to register

%inst
Allocates the section after #module to a separate space as a module.
Variables and labels within the module are independent from those outside the module.
^
"Module name" is a name that can be used to distinguish multiple modules by name. Modules with the same module name share variable names and label names.
Variable names and label names are treated as completely different between modules with different module names.
^
If "Module name" is omitted, the name "mNumerical value" is automatically assigned.
^
The module must always be instructed to start with "#module" and must be ended with "#global". By specifying the module section in this way, the inside can be made into an independent space from others.
The module name cannot exceed 18 characters in length and cannot overlap with other keywords such as variable names.
Also, be sure to specify a string that does not contain spaces or symbols. (The character types that can be used for the module name are the alphabet from a to z, numbers from 0 to 9, and the "_" symbol. It is equivalent to a string that can be used as a variable.)
^
The module variable name is for registering local variables associated with the module. The registered module variable name can be used in the processing routines defined by #modfunc or #modcfunc.
^
See the programming manual (hspprog.htm) for more details on module variables.

%href
#global
#deffunc
#modfunc
#modcfunc
#modinit
newmod
delmod
%port+
Let

%index
#deffunc
Register new command
%group
Preprocessor instruction
%prm
p1 p2 p3,…
p1 : Name assigned to the command
p2 p3〜 : Parameter type name・Alias name

%inst
Registers a new command by the user.
Specify the name of the new command in p1 and the calling parameter type in p2 and later.
It becomes possible to use the name defined by the #deffunc instruction as a command in the script.
^
The content to be executed for the new command is the line after the line specified by #deffunc.
Execution is performed as a subroutine jump, similar to the gosub command, and returns to the original execution position with the return command.
^p
Example:
	#deffunc test int a
	mes "Parameter="+a
	return
^p
The added new command can now receive parameters.
Each parameter can have a parameter type and alias name specified. The parameter types that can be specified are as follows.
^p
   int : Integer value
   var : Variable (no array)
   array : Variable (with array)
   str : String
   double : Real number value
   label : Label
   local : Local variable
^p
The alias name indicates the contents of the passed parameter and can be used in almost the same way as a variable.
Care must be taken when using var and array separately, and the local type indicating a local variable has a special purpose.
See the module section of the programming manual (hspprog.htm) for more details.

As a special use, you can register it as a cleanup command by writing "onexit" instead of the parameter type. The cleanup command is automatically called at the end of HSP script execution.
^p
Example:
	#deffunc Name onexit
^p
It can be used to perform post-processing such as when the function is extended by a module, and to release the system and memory.
^
The name of the new command is normally shared in all module spaces and global space.
However, if a name is specified after the local designation, it is treated as unique to the module.
^p
Example:
	#module user
	#deffunc local test int a
	mes "Parameter="+a
	return
	#global
	test@user 5
^p
This can be used to define commands that are used only within the module.
The name registered by the local designation must always be called in the format "Name@ModuleName".

%href
#global
#module
#modfunc
#modcfunc
#modinit
#modterm
mref
%port+
Let
%portinfo
In HSPLet, only the argument types str/int/double/var/array are supported.
local etc. cannot be specified.

%index
#defcfunc
Register a new function
%group
Preprocessor instruction
%prm
p1 p2 p3,…
p1 : Name of the function to register
p2 p3〜 : Parameter type name・Alias name
%inst
Registers a new user-defined function.
Specifies the name of the new function in p1, and the calling parameter types in p2 and subsequent parameters.
The name defined with the #defcfunc command can be used as a function within the script.
^
The content to be executed for the new function is from the line specified by #defcfunc onwards. Execution is performed as a subroutine jump, similar to the gosub command, and the return command returns to the original execution position.
At that time, you must specify a return value parameter for the return command.
^p
Example:
	#defcfunc half int a
	return a/2
^p
The added new function can now receive parameters.
For each parameter, it is possible to specify a parameter type and an alias name. The following parameter types can be specified:
^p
   int     : Integer value
   var     : Variable (without array)
   array   : Variable (with array)
   str     : String
   double  : Real number
   local   : Local variable
^p
The alias name indicates the contents of the passed parameter and can be used in almost the same way as a variable.
Care must be taken when distinguishing between var and array, and the local type indicating a local variable is for special purposes.
For more information, see the module item in the programming manual (hspprog.htm).
^
The name of the new function is usually shared by all module spaces and global spaces.
However, if a name is specified after a local designation, it is treated as specific to the module.
^p
Example:
	#module user
	#defcfunc local test int a
	return a+5
	#global
	mes test@user(5)
^p
This can be used to define functions that are only used within the module.
Names registered with local specification must always be called in the format "name@modulename".

%href
return
#deffunc
%port+
Let
%portinfo
When using HSPLet, argument types are only supported for str/int/double/var/array.
local etc. cannot be specified.

%index
#pack
Specifies the file to be packed (3.6 specification)
%group
Preprocessor directive
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the file to be packed when creating an executable file automatically (ctrl+F9).
The specified file is registered as an embedded file as a single file without considering the folder structure.
This command remains to specify the files to be packed with the specification up to HSP3.6.
For HSP3.7 and later, please use the #packdir command.
%href
#packdir

%index
#epack
Specifies the encrypted file to be packed (3.6 specification)
%group
Preprocessor directive
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the file to be packed when creating an executable file automatically (ctrl+F9).
The specified file is registered as an encrypted embedded file as a single file without considering the folder structure.
This command remains to specify the files to be packed with the specification up to HSP3.6.
For HSP3.7 and later, please use the #epackdir command.
%href
#epackdir

%index
#packdir
Specifies the file to be packed
%group
Preprocessor directive
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the file to be packed when creating an executable file automatically (ctrl+F9). The specified file is embedded as a resource when creating the executable file. #packdir packs in the normal format. If you want to pack with encryption, please use #epackdir.
If you try to add a duplicate file, it will be ignored.
"start.ax" is automatically added when creating an executable file, so there is no need to specifically specify it as an additional file.
^p
Example:
	#packdir "a.txt"
^p
In the example above, the file "a.txt" is packed together with the executable file.
Wildcard characters using the "*" symbol can be specified for the file name.
If you specify "data/*", all files under the data folder will be packed.
Also, by specifying "*.png", it is possible to pack all files with the extension ".png".
%href
#epackdir
#packopt

%index
#epackdir
Specifies the encrypted packed file
%group
Preprocessor directive
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the file to be packed when creating an executable file automatically (ctrl+F9). The specified file is embedded as a resource when creating the executable file. #epackdir encrypts and packs the specified file.
If you do not need to perform encryption, please use #packdir.
If you try to add a duplicate file, it will be ignored.
"start.ax" is automatically added when creating an executable file, so there is no need to specifically specify it as an additional file.
^p
Example:
	#epackdir "a.bmp"
^p
In the example above, the file "a.bmp" is encrypted and packed together with the executable file.
Wildcard characters using the "*" symbol can be specified for the file name.
If you specify "data/*", all files under the data folder will be packed.
Also, by specifying "*.png", it is possible to pack all files with the extension ".png".

%href
#packdir
#packopt

%index
#packopt
Automatic creation option specification
%group
Preprocessor directive
%prm
p1 p2
p1 : Keyword
p2 : Setting content
%inst
Specifies the operation of automatic executable file creation.
Write the keyword name, followed by a space or TAB, and then the parameters (specify as ""strings"" for strings).
The keywords that can be specified with #packopt are as follows:
^p
  Keyword  |        Content         | Initial value
 ------------------------------------------------------
  name       | Executable file name   | "hsptmp"
  runtime    | Runtime to use      | "hsprt"
  type       | Executable file type | 0
             | (0=EXE file)         |
             | (1=Full screen EXE)    |
             | (2=Screen saver)       |
  xsize      | Initial window X size  | 640
  ysize      | Initial window Y size  | 480
  hide       | Initial window hide SW | 0
  orgpath    | Initial directory keep SW | 0
  icon       | Icon file setting    | None
  version    | Version resource setting | None
  manifest   | Manifest setting     | None
  lang       | Language code setting  | None
  upx        | UPX compression setting | None
 ------------------------------------------------------
^p
In the following example, a screen saver named "test.scr" is created using a runtime named "hsp2c.hrt".
^p
Example:
	#packopt type 2
	#packopt name "test"
	#packopt runtime "hsp2c.hrt"
^p
When you perform "Automatic Executable File Creation", an executable file is generated based on the information of #packdir, #epackdir, #packopt written in the script being edited.
At that time, "start.ax" is added by default in encrypted form.
The runtime file (with the extension hrt) specified by "#packopt runtime "runtime file name"" is used from the one placed in the same directory as hspcmp.dll or in the runtime directory.
The icon, version, manifest, lang, and upx keywords are set using the iconins tool after generating the executable file.
The icon file must be specified in the .ico format.
^p
Example:
	// Specify the icon file to embed
	#packopt icon "test.ico"
	// Specify the file describing the version information to embed
	#packopt version "test.txt"
	// Set "1" to use UPX compression
	#packopt upx "1"
	// Specify the language Default is Japanese (1041) Described in decimal
	#packopt lang "1041"
^p
To use UPX compression, you must download upx.exe (Win32 console version) in advance and place it in the same folder as the iconins tool.

%href
#packdir
#epackdir
#cmpopt

%index
#const
Constant definition of macro name
%group
Preprocessor directive
%prm
Macro name Constant expression
%inst
Sets a replacement value for the specified macro name.
Similar to #define, but #const performs the calculation in advance and replaces the result when replacing constants (numerical values).
^p
Example:
	#const KAZU 100+50
	a=KAZU
		↓(After expansion)
	a=150
^p
It is effective for speeding up the source code if the value to be used in the source is determined in advance. Since it is also possible to include already defined macros,
^p
Example:
	#const ALL 50
	#const KAZU 100*ALL
	a=KAZU
		↓(After expansion)
	a=5000
^p
It can be used like this.
The calculation formula can use integers and real numbers.
The description style of operators and numerical values is the same as that of ordinary expressions. It is also possible to specify the order by parentheses.
^
By putting "global" immediately after the #const command, you can create a macro that can be used permanently in all modules.
^p
Example:
	#module
	#const global test 1234
	#global
	a=test   ; 1234 is assigned to a
^p
Normally, if you define #const in a module, the same name is not recognized in other modules and global areas.
By inserting a global specification, you can replace the name you defined in all subsequent locations with a macro.
^
By putting "double" immediately after the #const command, the defined numerical value is forcibly recognized as a real number.
By putting "int" immediately after the #const command, the defined numerical value is forcibly recognized as an integer.
If this specification is not made, integers and real numbers are automatically determined depending on whether there is a value after the decimal point.
^p
Example:
	#const double dnum 1
	#const dval dnum*2
	mes""+dnum	; 1.0 is displayed
	mes""+dval	; 2 is displayed
^p
Please note that the handling of real numbers and integers differs from the calculation rules of normal HSP.

%href
#define
%port+
Let

%index
#undef
Cancel macro name
%group
Preprocessor directive
%prm
Macro name
%inst
Cancels a macro name that has already been registered.
Specifying an unregistered macro name does not cause an error and is ignored.
%href
#define
%port+
Let


%index
#if
Compile control from numeric value
%group
Preprocessor directive
%prm
Numeric expression
%inst
Specifies compilation ON/OFF.
If the specified numeric value is 0, #if turns OFF the subsequent compilation output and ignores the compilation result (it is not executed as a program).
If the numeric value is other than 0, the output is turned ON.
This compilation control applies to the section until #endif appears.
One of #if, #ifdef, or #ifndef must have a pair with #endif.
^p
Example:
	#if 0
	mes "ABC"       ; This part is ignored
	a=111           ; This part is ignored
	mes "DEF"       ; This part is ignored
	#endif
^p
Since an expression can also be used for the #if specification,
^p
Example:
	#define VER 5
	#if VER<3
	mes "ABC"       ; This part is ignored
	a=111           ; This part is ignored
	mes "DEF"       ; This part is ignored
	#endif
^p
It can also be used like this. Calculation expression description and operator notes are the same as for the #const instruction.
It is also possible to nest #if-#endif blocks.
^p
Example:
	#ifdef SW
		#ifdef SW2
		mes "AAA"       ; When SW and SW2 are defined
		#else
		mes "BBB"       ; When SW is defined
		#endif
	#endif
^p
Basically, it can be used in a way similar to C or C++ preprocessors.
The preprocessor is automatically applied during normal compilation.
%href
#else
#endif
#ifdef
#ifndef
#define
#const
%port+
Let


%index
#ifdef
Compile control from macro definition
%group
Preprocessor directive
%prm
Macro name
%inst
Specifies compilation ON/OFF.
#ifdef turns OFF the subsequent compilation output and ignores the compilation result if the specified macro is not defined. If it is defined, the output is turned ON. This compilation control applies to the section until #endif appears.
For details on compilation control, see the reference for the #if instruction.
%href
#if
#ifndef
#else
#endif
%port+
Let


%index
#ifndef
Compile control from macro definition 2
%group
Preprocessor directive
%prm
Macro name
%inst
Specifies compilation ON/OFF.
#ifndef turns OFF the subsequent compilation output and ignores the compilation result if the specified macro is defined. If it is not defined, the output is turned ON. This compilation control applies to the section until #endif appears.
For details on compilation control, see the reference for the #if instruction.
%href
#if
#ifdef
#else
#endif
%port+
Let


%index
#else
Invert compile control
%group
Preprocessor directive
%inst
Inverts ON/OFF within a compilation control section such as #if, #ifdef, #ifndef, etc.
For details on compilation control, see the reference for the #if instruction.
%href
#if
#ifdef
#ifndef
#endif
%port+
Let


%index
#endif
End of compile control block
%group
Preprocessor directive
%inst
Ends a compilation control section such as #if, #ifdef, #ifndef, etc.
For details on compilation control, see the reference for the #if instruction.
%href
#if
#ifdef
#ifndef
#else
%port+
Let



%index
#modfunc
Assign new instruction
%group
Preprocessor directive
%prm
p1 p2 p3,…
p1      : Name of the assigned instruction
p2 p3〜 : Parameter type name/alias name

%inst
Registers a new instruction for processing module variables.
Specifies the name of the new instruction in p1, and the calling parameter type in p2 and subsequent parameters.
After the instruction is defined, the specified name can be used as an instruction word.
Also, within the #modfunc routine, the system variable thismod can be treated as its own module variable.
^
The parameters of the #modfunc instruction are in the same format as the #deffunc instruction.
The difference from the #deffunc instruction is that it is necessary to specify a module type variable when calling. For details on module variables, refer to the module item in the programming manual (hspprog.htm).

%href
#modcfunc
#deffunc
#modinit
#modterm
thismod



%index
#modcfunc
Assign new function
%group
Preprocessor directive
%prm
p1 p2 p3,…
p1      : Name of the assigned instruction
p2 p3〜 : Parameter type name/alias name

%inst
Registers a new function for processing module variables.
Specifies the name of the new instruction in p1, and the calling parameter type in p2 and subsequent parameters.
After the instruction is defined, the specified name can be used as an instruction word.
Also, within the #modcfunc routine, the system variable thismod can be treated as its own module variable.
^
The parameters of the #modcfunc instruction are in the same format as the #defcfunc instruction.
The difference from the #defcfunc instruction is that it is necessary to specify a module type variable when calling. For details on module variables, refer to the module item in the programming manual (hspprog.htm).

%href
#modfunc
#deffunc
#modinit
#modterm
thismod



%index
#modinit
Module initialization process registration
%group
Preprocessor directive
%prm
p1 p2,…
p1 p2〜 : Parameter type name/alias name

%inst
Registers a process (constructor) for initializing module variables.
You can specify the calling parameter type and alias name as options.
The section defined by #modinit is automatically called when the newmod instruction is executed.
It is also possible to acquire the optional parameters specified by the newmod instruction on the constructor side.
For details on module variables, refer to the module item in the programming manual (hspprog.htm).
^
The parameter type and alias name of the #modinit instruction are in the same format as the #deffunc instruction.

%href
#deffunc
#modfunc
#modterm
newmod



%index
#modterm
Module release process registration
%group
Preprocessor directive
%prm

%inst
Registers a process (destructor) for discarding module variables.
You can specify the calling parameter type and alias name as options.
The section defined by #modterm is automatically called when the delmod instruction is executed or when the module variable is discarded.
For details on module variables, refer to the module item in the programming manual (hspprog.htm).

%href
#deffunc
#modfunc
#modinit
delmod



%index
#regcmd
Register extension plugin
%group
Preprocessor directive
%prm
"Initialization function name","DLL file name",Number of extended variable types
"Initialization function name"  : Export name of the plugin initialization function
"DLL file name" : DLL name of the plugin initialization function
Number of extended variable types(0) : The number of variable types extended by the plugin

%inst
Registers the HSP extension plugin.
The initialization function name must accurately specify the name exported from the DLL. When exporting from VC++, "_" is added to the beginning and "@4" is added to the end, so describe the name including them. (For DLLs created with compilers other than VC++, the export name rules are different. For details, refer to the documentation for each environment.)
For example, if you want to register the "hsp3cmdinit" function of "hpi3sample.dll",
^p
Example:
	#regcmd "_hsp3cmdinit@4","hpi3sample.dll"
^p
It will be like this.
If you want to extend the variable type with a plugin, you need to specify "Number of extended variable types".
If you want to add one variable type, specify 1 for the number of extended variable types. If you do not want to extend the variable type, omit the number of extended variable types or specify 0. Note that if the number of extended variable types is not specified correctly, the type registration will be invalid.
^
If an integer is specified as the first parameter, the type value executed as an internal command is assigned to the keyword defined by #cmd thereafter.
The type value is a unique value used internally by HSP3. Normally, you do not need to use this function. Also, if "*" is specified instead of an integer, an empty type value will be automatically assigned.


%href
#cmd
#uselib
#func


%index
#cmd
Register extension keyword
%group
Preprocessor directive
%prm
New keyword Sub ID
New keyword : Keyword to be added
Sub ID         : Sub ID value given to the keyword

%inst
Registers keywords for the HSP extension plugin.
It is necessary to register the plugin initialization function in advance using the #regcmd instruction.
^p
Example:
	#cmd newcmd $000
	#cmd newcmd2 $001
	#cmd newcmd3 $002
^p
In the example above, the keyword "newcmd" is registered as sub ID 0, the keyword "newcmd2" is registered as sub ID 1, and the keyword "newcmd3" is registered as sub ID 2.


%href
#regcmd
#uselib
#func



%index
#usecom
Specify external COM interface
%group
Preprocessor directive
%prm
Interface name "Interface IID" "Class IID"
Interface name    : Keyword to identify the interface
"Interface IID" : IID string indicating the COM interface
"Class IID"           : IID string indicating the COM class

%inst
Defines an external component (COM).
Assigns the class IID and interface IID to the specified interface name and makes it usable.
The IID can be specified as a string similar to the registry ({〜}).
Also, "Class IID" can be omitted.
^p
Example:
	#define CLSID_ShellLink "{00021401-0000-0000-C000-000000000046}"
	#define IID_IShellLinkA "{000214EE-0000-0000-C000-000000000046}"
	#usecom IShellLinkA IID_IShellLinkA CLSID_ShellLink
^p
By putting "global" immediately before the interface name, you can create an interface that can be used permanently in all modules.


%href
#comfunc
newcom
delcom
querycom



%index
#comfunc
Register external COM call instruction
%group
Preprocessor directive
%prm
New name Index Type name 1,…
New name     : Keyword recognized as an instruction
Index : Method index value
Type name     : Specify the argument type separated by commas
%inst
Registers a new instruction for calling external components (COM).
#comfunc registers a method of the interface specified by the #usecom instruction as an instruction for calling.
After that, you can call the component by combining the instruction specified with the new name and the variable of the COM object type.
^
Write the new name, index, and type separated by spaces.
By putting "global" immediately before the new name, you can create an instruction that can be used permanently in all modules.
^
Specify the details of the arguments in the type name.
Specify the argument types separated by "," in the same way as the #deffunc instruction.
There are no restrictions on the number of arguments or the order of types.
The following strings can be used as argument types:
^p
   Type      :  Content
 -------------------------------------------------------------
   int     :  Integer value (32bit)
   var     :  Variable data pointer (32bit)
   str     :  String pointer (32bit)
   wstr    :  Unicode string pointer (32bit)
   double  :  Real value (64bit)
   float   :  Real value (32bit)
   pval    :  PVal structure pointer (32bit)
   bmscr   :  (*)BMSCR structure pointer (32bit)
   hwnd    :  (*)Handle of the currently selected window (HWND)
   hdc     :  (*)Device context of the currently selected window (HDC)
   hinst   :  (*)Running HSP instance handle
^p
Items marked with (*) indicate parameters that are automatically passed without the need to specify them as arguments.
^p
Example:
	; Class ID of the shell link object
	#define CLSID_ShellLink "{00021401-0000-0000-C000-000000000046}"
	; Interface ID of the IShellLink interface
	#define IID_IShellLinkA "{000214EE-0000-0000-C000-000000000046}"

	#usecom IShellLinkA IID_IShellLinkA CLSID_ShellLink
	#comfunc IShellLink_SetPath 20 str

	newcom slink, IShellLinkA
	IShellLink_SetPath slink, "c:\\hsp261\\hsp2.exe"
^p
In the example above, IShellLink_SetPath of the IShellLinkA interface is called together with the variable slink.
Note that the instruction registered with #comfunc always has a COM object type variable with the same interface as the first argument.

%href
#usecom
newcom
delcom
querycom



%index
#enum
Enumerate macro name constants
%group
Preprocessor instruction
%prm
Macro name = p1
Macro name : Macro name to assign a constant
p1       : Constant to be assigned
%inst
Assigns consecutive values to the specified macro name.
You can define a macro name indicating a constant, similar to the #const instruction.
^p
Example:
	#enum KAZU_A = 0	; KAZU_A will be 0
	#enum KAZU_B		; KAZU_B will be 1
	#enum KAZU_C		; KAZU_C will be 2
	a=KAZU_B
		↓(After expansion)
	a=1
^p
By writing "=(equal)" and a number (or expression) after the macro name, the constant is initialized. After that, the number increases by 1 each time it is defined with the #enum instruction.
The #enum instruction is used when you want to define consecutive values by macros.
By using the #const instruction, it is not necessary to specify numbers one by one, and it is easy to add or delete later.

%href
#const
%port+
Let




%index
#runtime
Runtime file settings
%group
Preprocessor instruction
%prm
"Runtime name"
"Runtime name" : Runtime name to set

%inst
Sets the runtime filename used to execute the script.
(The runtime filename specifies the name part of the file, excluding the extension.)
Use this to set the runtime when executing from the script editor or when creating an executable file.
If multiple #runtime instructions are set, the last setting will be valid.

%href
#packopt
#cmpopt
%port+
Let
%portinfo
When using HSPLet, specify "hsplet3" as the runtime name.


%index
#cmpopt
Compilation settings
%group
Preprocessor instruction
%prm
Option name  Parameter
Option name : Type of option
Parameter : Parameter to set

%inst
Specifies the behavior during script compilation.
Write the option name followed by a space or TAB and then the parameter
(If it is a string, specify it like "strings").
The keywords that can be specified with #cmpopt are as follows:
^p
  Option |      Content            | Initial value
 ------------------------------------------------------
  ppout      | Preprocessor file output | 0
             | (0=None/1=Output)        |
  optcode    | Optimization of         | 1
             | unnecessary code        |
             | (0=None/1=Optimize)      |
  optinfo    | Optimization information  | 0
             | log output               |
             | (0=None/1=Output)        |
  varname    | Output of variable name   | 0
             | information other than     |
             | during debugging           |
             | (0=None/1=Present)        |
  varinit    | Uninitialized variable    | 0
             | check                     |
             | (0=Warning/1=Error)        |
  optprm     | Optimization of          | 1
             | parameter code            |
             | (0=None/1=Optimize)      |
  skipjpspc  | Ignore full-width spaces  | 1
             | (0=Error/1=Ignore)       |
  utf8       | Output strings in         | 0
             | UTF-8 format             |
             | (0=Disabled/1=Enabled)   |
 ------------------------------------------------------
^p
In the following example, the preprocessor result is output to a file.
^p
Example:
	#cmpopt ppout 1
^p
The #cmpopt instruction should basically be written at the beginning of the script.
If multiple #cmpopt instructions are written, the last setting will be valid for each option (similar to the #packopt option).
Also, you cannot write to apply the option only to a specific range.

%href
#packopt
#runtime
%port+
Let



%index
#bootopt
Runtime startup settings
%group
Preprocessor instruction
%prm
Option name  Parameter
Option name : Type of option
Parameter : Parameter to set

%inst
Performs detailed behavior settings for the script runtime.
Write the setting switch, a numeric value of 1 or 0, after the option name, with a space or TAB in between.
#bootopt can be written at any position in the script.
If multiple specifications are made, the last setting will be the overall setting.

The keywords that can be specified with #bootopt are as follows:
^p
  Option |      Content                | Initial value
 -----------------------------------------------------------
  notimer    | Use of high-precision timer | Automatic setting
             | (0=Use/1=Do not use)      |
  utf8       | Use of UTF-8 format         | Automatic setting
             | strings                     |
             | (0=Use/1=Do not use)      |
  hsp64      | Use of 64-bit runtime     | Automatic setting
             | (0=Use/1=Do not use)      |
 -----------------------------------------------------------
^p
In the following example, the use of a high-precision timer is suppressed.
^p
Example:
	#bootopt notimer 1
^p

%href
#cmpopt
#runtime



%index
#aht
Describes AHT file header
%group
Preprocessor instruction
%prm
Setting name p1
Setting name : Name of setting item to assign a constant to
p1     : Constant to be assigned
%inst
Adds AHT file information to the source script.
You can assign a string or number to the specified setting name.
Source scripts with AHT file headers added by the #aht instruction can be referenced as AHT files from template managers, etc.
Settings names that can be used
^p
  Setting name     |      Content
 ------------------------------------------------------------
  class      | Specifies the class name of the AHT file
  name       | Specifies the name of the AHT file
  author     | Specifies the author name of the AHT file
  ver        | Specifies the version of the AHT file
  exp        | Specifies the description of the AHT file
  icon       | Specifies the icon file unique to the AHT file
  iconid     | Specifies the icon ID unique to the AHT file
 ------------------------------------------------------------
^p
For details on AHT files, refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#ahtmes
%port+
Let


%index
#ahtmes
Output of AHT message
%group
Preprocessor instruction
%prm
p1
p1     : String expression to be output
%inst
Outputs a message to the outside during AHT parsing.
It is mainly used to describe the source code that is added on the editor with "Easy Input".
The #ahtmes instruction can connect strings and macros with the "+" operator and output them in the same way as the mes instruction.
^p
Example:
	#define Variable to assign	a	;;str
	#const Range of random numbers 100		;;help="Generated from 0 to specified range-1"
	#ahtmes	"	"+Variable to assign+" = rnd( "+Range of random numbers+" )\t\t; Assigns a random number to the variable "+Variable to assign+"."
^p
Please note that, unlike the normal mes instruction, it only connects macros defined on the preprocessor.
For details on AHT files, refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#aht
%port+
Let



%index
#var
Declare the variables to use
%group
Preprocessor instruction
%prm
var,...
var : Variable name to declare
%inst
Declares the variables to be used in the script.
By declaring the variables to be used in advance, it is possible to prevent them from being detected as uninitialized variables and to operate safely.
With the #var instruction, you can write multiple variables consecutively by separating them with the "," symbol.
^p
Example:
	#var a,b,c
^p
The example above declares that three variables, a, b, and c, will be used in the script.
Also, #varint, #varstr, #vardouble, and #varlabel are also available for declaring variables with fixed types.
%href
#varint
#varstr
#vardouble
#varlabel



%index
#varint
Declare a variable with its type fixed to integer (int)
%group
Preprocessor instruction
%prm
var,...
var : Variable name to declare
%inst
Declares a variable used in the script and fixes it as an integer (int) type.
If you try to change the type of a variable with a fixed type to a different type during execution, an error will occur. This prevents the type of the variable from being changed unintentionally.
Type fixing is only detected during debug execution. Please note that it is invalid in scripts converted to executable files or in platform-converted environments (iOS, android).
Also, by declaring the variables to be used in advance, it is possible to prevent them from being detected as uninitialized variables and to operate safely.
Use the #var instruction to declare variables without fixing their types.
As with the #var instruction, you can write multiple variables consecutively by separating them with the "," symbol.
%href
%index
#varstr
Declares a variable with its type fixed to string (str)
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as string (str) type.
If you try to change the type of a fixed-type variable to a different type during execution, an error will occur. This prevents the variable's type from changing unintentionally.
Type fixing is only detected during debug execution. Please note that it is disabled in scripts converted to executable files and platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, it is possible to prevent them from being detected as uninitialized variables and to operate safely.
To declare variables without fixing the type, use the #var command.
As with the #var command, you can write multiple variables consecutively by separating them with the "," symbol.
%href
#var
#varint
#vardouble
#varlabel



%index
#vardouble
Declares a variable with its type fixed to real number (double)
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as real number (double) type.
If you try to change the type of a fixed-type variable to a different type during execution, an error will occur. This prevents the variable's type from changing unintentionally.
Type fixing is only detected during debug execution. Please note that it is disabled in scripts converted to executable files and platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, it is possible to prevent them from being detected as uninitialized variables and to operate safely.
To declare variables without fixing the type, use the #var command.
As with the #var command, you can write multiple variables consecutively by separating them with the "," symbol.
%href
#var
#varint
#varstr
#varlabel



%index
#varlabel
Declares a variable with its type fixed to label
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as label type.
If you try to change the type of a fixed-type variable to a different type during execution, an error will occur. This prevents the variable's type from changing unintentionally.
Type fixing is only detected during debug execution. Please note that it is disabled in scripts converted to executable files and platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, it is possible to prevent them from being detected as uninitialized variables and to operate safely.
To declare variables without fixing the type, use the #var command.
As with the #var command, you can write multiple variables consecutively by separating them with the "," symbol.
%href
#var
#varint
#varstr
#vardouble
