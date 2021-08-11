;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Built-in instructions
%ver
3.5
%note
ver3.5 standard instruction
%date
2017/09/13
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli


%index
#define
Register a new macro
%group
Preprocessor instructions
%prm
Macro name Macro definition
%inst
Registers with the preprocessor so that the keyword specified by the macro name can be replaced with the specified definition.
The #define macro is used only when an individual wants to write a script easily and customize it, and is not suitable for beginners. The features described here are also infrequently used and should only be referred to when needed.
The #define macro basically registers a replacement string.
^p
example :
	#define hyouji mes
	hyouji "AAAAA..."
↓ (after deployment)
	mes "AAAAA..."
^p
By inserting "global" immediately after the #define instruction, you can create a macro that can be used permanently in all modules.
^p
example :
	#module
	#define global test 0x1234
	#global
a = test; 0x1234 is assigned to a
^p
Normally, if you define #define in a module, the same name will not be recognized in other modules and global areas.
By entering the global specification, you can replace the name defined everywhere after that with a macro.
^
In addition to simple replacement macros, expansion with arguments is possible.
The argument is specified by the argument name of% 1,% 2,% 3 ... enclosed in parentheses after the macro name.
vinegar.
The argument must be specified as "% numerical value", and the numerical value should be described in order from 1. Note that you cannot specify it by symbol name like the C or C ++ preprocessor.
^p
example :
	#define hyouji(%1) mes "prm="+%1
	hyouji "AAAAA..."
↓ (after deployment)
	mes "prm="+"AAAAA..."
^p
You can also set the initial (default) value for the argument.
^p
example :
	#define hyouji(%1="PRM=",%2=123) mes %1+%2
	hyouji "AAA",a
	hyouji "BBB"
	hyouji ,b
↓ (after deployment)
	mes "AAA"+a
	mes "BBB"+123
	mes "PRM="+b
^p
The initial (default) value is a value that is automatically completed if omitted when using a macro. If the initial value is omitted, it will not be completed.
When specifying macro arguments, the side specified by #define is enclosed in parentheses, but when actually using it, specify it without parentheses.
^p
	#define hyouji(%1) mes "prm="+%1
	hyouji("AAAAA...")
^p
Please note that a description like this will result in an error.
However, the following parenthesized description is possible by using the ctype option.
^p
	#define ctype kansu(%1) %1*5+1
	a=kansu(5)
^p
This option is effective when you want to use a macro other than the instruction part such as a calculation formula. At first glance, it behaves like a function in C language, but please note that the range of application is narrow because it is actually replaced by a macro.
This description method is different from the original HSP grammar, so we do not recommend using it for anything other than customization purposes where you want to write in your own style.
^
It is possible to set keywords that perform special actions when expanding macros.
This special keyword is mainly for sharing parameters between different macros and for realizing nested structures by stacking.
^p
	#define start(%1) %tstart %s1 mes "START"
	#define owari %tstart mes %o
^p
The "% s1" and "% o" specified here are special expansion macros.
A sample using this will be expanded as follows.
^p
	start "OK" -> mes "START"
	owari -> mes "OK"
^p
In this way, it is possible to share data between different macros.
The special expansion macro has the following types and functions.
^p
   Macro: Function
 ----------------------------------------------------------------
     % t: Set the tag name
     % n: Generate a unique label name
     % i: Generate a unique label name and put it on the stack
     % o: Extract the string on the stack
     % p: Extract the character string on the stack (maintain the stack)
     % s: Stack argument parameters
     % c: Start a new line
^p
The special expansion macro is expressed by "%" followed by one alphabetic character + parameter.
Please put a space after the special expansion macro to distinguish it from the following keywords. Up to the part including spaces such as "% tabc aaa" is judged as a special expansion macro.
The special expansion macro has a general stack (First In Last Out).
This stack can be shared by macros with the same tag name.
Specify the tag name using up to 16 single-byte alphabetic characters after "% t", such as "% t tag name". In the previous example, "start" specified as "% tstart" corresponds to the tag name.
"% S" is a special expansion macro for stacking argument parameters.
If "% s1" is specified, the parameter of "% 1" is pushed on the stack.
Use "% o" to get the string on the stack.
"% O" takes out the character string on the stack and expands it. Since it is a stack, the last one stacked is taken out first.
If you specify "% o0", the stack is ejected, but the string is not expanded (stack eject only).
"% P" retrieves only the contents without returning the stack. "% P0" expands the contents of the next stacked stack. "% P1" takes out another deeper stack. After that, it is possible to specify from "% p0" to "% p9".
An example of label generation is shown below.
^p
	#define start %tstart *%i
	#define owari %tstart await 20:stick a:if a=0  :  goto
*%o
^p
A sample using this will be expanded as follows.
^p
	start -> *_start_0000
	owari -> await 20:stick a:if a=0 : goto *_start_0000
^p
"% I" creates a unique label name that does not overlap with others and stacks it on the stack. If "% i0" is specified, the label name will be pushed one step on the stack, but it will not be expanded.
Also, "% n" only generates and expands a unique label name, not on the stack.
In the above example, a new label is created by label name generation to realize a loop structure. Using this method, you can build a loop structure in which label names do not overlap even if they are nested.
Also, macros (tag names) that did not get all the stacks in one source script file will report an error at compile time.
Be sure to configure the macro instructions so that the entire stack is ejected and finished.
^
The standard definition macros while ~ wend, do ~ until, and for ~ next added in HSP ver2.6 are created by special expansion macros.
^
In special situations, it is possible to expand with a line break by "% c".
The line is split and expanded at the "% c" part. It can be used mainly for defining macros that are expanded to multiple preprocess statements.
However, not all preprocessors currently support macro expansion. If you use it too much, it may be difficult to see, so please understand it carefully before using it.
^p
example :
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
External DLL call instruction registration
%group
Preprocessor instructions
%prm
New name "function name" type name 1,…

%inst
Register a new instruction to call the external DLL.
You must first specify the external DLL name with the #uselib instruction.
Write the new name, function name, and type separated by a space.
By putting "global" just before the new name, you can create an instruction that can be used permanently in all modules.
^
You can write the full export name of the DLL by enclosing the function name in double quotes, such as "function name".
If it is not enclosed in double quotes, it will be converted to the name "_function name @ 16" based on the VC ++ export convention.
^
Describe the details of the argument in the type name.
As with the #deffunc instruction, specify the argument types separated by ",".
There is no limit to the number of arguments or the order of the types.
The character strings that can be used as the argument type are as follows.
^p
     Type: Contents
 ------------------------------------------------------------------
    int: Integer value (32bit)
    var: Variable data pointer (32bit)
    str: String pointer (32bit)
    wstr: unicode string pointer (32bit)
    sptr: Pointer Integer value or string pointer (32bit)
    wptr: Pointer Integer value or unicode string pointer (32bit)
    double: real number (64bit)
    float: real number (32bit)
    pval: Pointer of PVal structure (32bit)
    comobj: COMOBJ type variable data pointer (32bit)
(*) bmscr: Pointer of BMSCR structure (32bit)
(*) prefstr: Pointer of system variable refstr (32bit)
(*) pexinfo: Pointer of EXINFO structure (32bit)
(*) nullptr: null pointer (32bit)
^p
Items marked with (*) indicate parameters that are automatically passed without having to be specified as arguments.
The following is an example of executing by specifying four arguments.
^p
example :
	#uselib "test.dll"
	#func test "_func@16" var,int,int,int
test a, 1,2,3; func (& a, 1,2,3) in test.dll is called
^p
If you specify a numerical value for the type, arguments compatible with the DLL type specification of ver2.5 or later are automatically set. However, there are some incompatible specifications. In that case, it will be reported as an error at compile time.
^
It is necessary to pay attention to compatibility with ver2.5 in the following points.
^p
-The BMSCR structure can only refer to the fields from flag to color.
-The PVal structure is compatible with the ver2.5 PVAL2 structure.
-The flag field (type type value) and len field (array information) of the PVal structure cannot be rewritten on the DLL side.
-The string type array variable cannot be accessed from the PVal structure.
^p
In other respects, the same information as HSP ver2.5 will be passed.
^
You can register it as an end call function by putting "onexit" before the function name.
^p
example :
	#func test onexit "_func@16" str,int,int
^p
In the above example, "_func @ 16" is automatically called when the application is closed.


%href
#uselib
#cfunc
%port+
Let
%portinfo
At HSPLet, the method with the same name as the function will be called. (Refer to the HSPLet manual for details.)


%index
#cfunc
External DLL call function registration
%group
Preprocessor instructions
%prm
New name "function name" type name 1,…

%inst
Register a new function to call the external DLL.
You must first specify the external DLL name with the #uselib instruction.
Write the new name, function name, and type separated by a space.
By putting "global" just before the new name, you can create an instruction that can be used permanently in all modules.
^
You can write the full export name of the DLL by enclosing the function name in double quotes, such as "function name".
If it is not enclosed in double quotes, it will be converted to the name "_function name @ 16" based on the VC ++ export convention.
^
Describe the details of the argument in the type name.
You can use the same argument parameters as the #func instruction.
See the #func reference for more information.
The new name registered by the #cfunc instruction can be described in the expression as a function.
^p
example :
	#uselib "test.dll"
	#cfunc test "_func@16" var,int,int,int
res = test (a,1,2,3); func (& a,1,2,3) in test.dll is called
^p
As the return value of the registered function, the integer value (32bit int) acquired as a result of the external call is returned as it is.
For HSP2.5 compatible calls, the value returned in the system variable stat is the return value of the function.


%href
#uselib
#func
%port+
Let
%portinfo
At HSPLet, the method with the same name as the function will be called. (Refer to the HSPLet manual for details.)






%index
#include
Combine different files
%group
Preprocessor instructions
%prm
"filename"
"filename": Filename to combine

%inst
Specify the files to include.
The file specified here will be combined with the original file and compiled.

%href
#addition
%port+
Let



%index
#addition
Combine separate files 2
%group
Preprocessor instructions
%prm
"filename"
"filename": Filename to combine

%inst
Specify the files to include.
The file specified here will be combined with the original file and compiled.
Same behavior as the #include instruction, but #addition continues without stopping as an error, even if the file does not exist.
It is used when combining additional definition files.

%href
#include
%port+
Let



%index
#uselib
Specifying an external DLL
%group
Preprocessor instructions
%prm
"filename"
"filename": External DLL filename

%inst
Specify the file name of the external DLL to be called from HSP.
The DLL file name must be written completely, including the extension.
If the file name is omitted, the DLL name will be specified from the script at runtime for linking.

%href
#func
%sample
	#uselib "user32.dll"
	#func MessageBoxA "MessageBoxA" int,sptr,sptr,int
	MessageBoxA hwnd,"test message","title",0
%port+
Let
%portinfo
At HSPLet, the class with the same name as the specified DLL is loaded.
The class name loaded will be the last one without .dll or .hpi.
For example, using winmm.dll loads the winmm class.
When you created your own extension library class, put the JAR file in the lib folder.
The compiler will automatically recognize it and check it at compile time.



%index
#global
Module end
%group
Preprocessor instructions
%prm
%inst
Ends the module section and returns the rest to the normal program area.
See the #module instruction for more information on modules.
%href
#module
#deffunc
%port+
Let




%index
#module
Module start
%group
Preprocessor instructions
%prm
Module name Variable name 1,…
Module name: New module name
Variable name: Module variable name to be registered

%inst
Allocate the section after #module as a module to another space.
Variables and labels inside the module are independent of those outside the module.
^
"Module name" is a name that can be given when dividing multiple modules by name, and those with the same module name share a variable name or label name.
Variable names and label names are treated as completely different among those with different module names.
^
If you omit the "module name", the name "m number" is automatically assigned.
^
Modules must always start with "#module" and end with "#global". By specifying the module section in this way, it is possible to make the space independent of others.
The module name can be up to 18 characters in length and cannot be duplicated with other keywords such as variable names.
Also, be sure to specify a string that does not contain spaces or symbols. (The character types that can be used in the module name are alphabets from a to z, numbers from 0 to 9, and the "_" symbol. They are equivalent to character strings that can be used as variables.)
^
The module variable name registers the local variable associated with the module. The registered module variable name can be used in the processing routine defined by #modfunc or #modcfunc.
^
For more information on module variables, see the programming manual (hspprog.htm).

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
Register a new instruction
%group
Preprocessor instructions
%prm
p1 p2 p3,…
p1: Name of the assigned instruction
p2 p3 ~: Parameter type name / alias name

%inst
Register a new instruction by the user.
Specify the name of the new instruction in p1 and the call parameter type after p2.
The name defined by the #deffunc instruction can be used as an instruction in the script.
^
The new instruction will be executed after the line specified by #deffunc.
Execution is performed as a subroutine jump like the gosub instruction, and the return instruction returns to the original execution position.
^p
example :
	#deffunc test int a
mes "parameter =" + a
	return
^p
The new instruction added will be able to receive parameters.
You can specify the parameter type and alias name for each parameter. The parameter types you specify include:
^p
   int: integer value
   var: variable (no array)
   array: Variable (with array)
   str: string
   double: real number
   label: label
   local: local variable
^p
The alias name indicates the contents of the passed parameter and can be used in much the same way as a variable.
Care must be taken when using var and array properly, and the local type, which indicates a local variable, has a special purpose.
For more information, see the module entry in the programming manual (hspprog.htm).

As a special use, you can register it as a cleanup instruction by writing "onexit" instead of the parameter type. The cleanup instruction is automatically called at the end of HSP script execution.
^p
example:
#deffunc name onexit
^p
It can be used to clean up the system and release memory when the function is expanded by a module.
^
The name of the new instruction is usually shared in all module spaces and global spaces.
However, if the name is specified after the local specification, it will be treated as module-specific.
^p
example :
	#module user
	#deffunc local test int a
mes "parameter =" + a
	return
	#global
	test@user 5
^p
This can be used, for example, to define instructions that are used only within the module.
The name registered by specifying local must be called in the format of "name @ module name".


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
For HSPLet, only str / int / double / var / array argument types are supported.
You cannot specify local etc.


%index
#defcfunc
Register a new function
%group
Preprocessor instructions
%prm
p1 p2 p3,…
p1: The name of the function to register
p2 p3 ~: Parameter type name / alias name

%inst
Register a new function by the user.
Specify the name of the new function in p1 and the call parameter type after p2.
The name defined by the #defcfunc instruction can be used as a function in the script.
^
The new function will be executed after the line specified by #defcfunc. Execution is performed as a subroutine jump like the gosub instruction, and the return instruction returns to the original execution position.
At that time, it is necessary to specify the parameter of the return value in the return instruction.
^p
example :
	#defcfunc half int a
	return a/2
^p
The new function added will be able to receive parameters.
You can specify the parameter type and alias name for each parameter. The parameter types you specify include:
^p
   int: integer value
   var: variable (no array)
   array: Variable (with array)
   str: string
   double: real number
   local: local variable
^p
The alias name indicates the contents of the passed parameter and can be used in much the same way as a variable.
Care must be taken when using var and array properly, and the local type, which indicates a local variable, has a special purpose.
For more information, see the module entry in the programming manual (hspprog.htm).
^
The name of the new function is usually shared in all module spaces and global spaces.
However, if the name is specified after the local specification, it will be treated as module-specific.
^p
example :
	#module user
	#defcfunc local test int a
	return a+5
	#global
	mes test@user(5)
^p
This can be used, for example, to define a function that is only used within the module.
The name registered by specifying local must be called in the format of "name @ module name".

%href
return
#deffunc
%port+
Let
%portinfo
For HSPLet, only str / int / double / var / array argument types are supported.
You cannot specify local etc.



%index
#pack
PACKFILE Additional file specification
%group
Preprocessor instructions
%prm
"filename"
"filename": Files to be added to PACKFILE
%inst
In the executable file automatic creation (ctrl + F9), specify the file to be added to packfile. The specified file is packed together as a resource when the executable file is created. #pack packs in the normal format. If you want to encrypt and pack, please use #epack.
If you try to add a duplicate file, it will be ignored.
"Start.ax" does not need to be specified as an additional file because it is automatically added when the executable file is automatically created.
^p
example :
	#pack "a.txt"
^p
In the above example, we will pack a file called "a.txt" with the executable file.
As before, you can also create an executable file by selecting the file to be packed from "Edit pack file".
Please note that if you perform "Automatic executable file creation", the packfile will be created automatically, and the packfile information saved up to that point will be overwritten.

%href
#epack
#packopt


%index
#epack
PACKFILE Encrypted file specification
%group
Preprocessor instructions
%prm
"filename"
"filename": Files to be added to PACKFILE
%inst
In the executable file automatic creation (ctrl + F9), specify the file to be added to packfile. The specified file is packed together as a resource when the executable file is created. #epack encrypts and packs the specified file.
If you don't need to encrypt, use #pack.
If you try to add a duplicate file, it will be ignored.
"Start.ax" does not need to be specified as an additional file because it is automatically added when the executable file is automatically created.
^p
example :
	#epack "a.bmp"
^p
In the above example, the file "a.bmp" is encrypted and packed with the executable file.
As before, you can also create an executable file by selecting the file to be packed from "Edit pack file".
Please note that if you perform "Automatic executable file creation", the packfile will be created automatically, and the packfile information saved up to that point will be overwritten.

%href
#pack
#packopt


%index
#packopt
Automatic creation option specification
%group
Preprocessor instructions
%prm
p1 p2
p1: Keyword
p2: Settings
%inst
Specifies the operation for automatic execution file creation.
Enter the parameter (specify like "strings" in the case of a character string) by inserting a space or TAB after the keyword name.
The keywords that can be specified with #packopt are as follows.
^p
  Keywords | Contents | Initial value
 ------------------------------------------------------
  name | Executable file name | "hsptmp"
  runtime | Runtime to use | "hsprt"
  type | Executable file type | 0
             | (0 = EXE file) |
             | (1 = full screen EXE) |
             | (2 = screen saver) |
  xsize | Initial window X size | 640
  ysize | initial window Y size | 480
  hide | Hide initial window SW | 0
  orgpath | Initial directory maintenance SW | 0
  icon | Icon file settings | None
  version | Version resource settings | None
  manifest | Manifest settings | None
  lang | language code setting | none
  upx | UPX compression settings | None
 ------------------------------------------------------
^p
The following example creates a screensaver called "test.scr" using a runtime called "hsp2c.hrt".
^p
example :
	#packopt type 2
	#packopt name "test"
	#packopt runtime "hsp2c.hrt"
^p
When "Automatic executable file is created", an executable file is generated based on the information of #pack, #epack, #packopt described in the script being edited.
At that time, "start.ax" is added encrypted by default.
The runtime file (with the extension hrt) specified by "#packopt runtime" runtime file name "" is used in the same directory as hspcmp.dll or in the runtime directory.
The keywords icon, version, manifest, lang, upx are set using the iconins tool after the executable file is generated.
For the icon file, you need to specify a .ico format file.
^p
example :
// Specify the icon file to embed
	#packopt icon "test.ico"
// Specify the file that describes the version information to be embedded
	#packopt version "test.txt"
// Set "1" when compressing using UPX
	#packopt upx "1"
// Specify language Default is Japanese (1041) Described in decimal
	#packopt lang "1041"
^p
If you want to use UPX compression, you need to download upx.exe (Win32 console version) in advance and place it in the same folder as the iconins tool.


%href
#pack
#epack
#cmpopt


%index
#const
Macro name constant definition
%group
Preprocessor instructions
%prm
Macro name constant expression
%inst
Replaces with the specified macro name and sets the numerical value.
Similar to #define, but #const replaces the result of pre-calculation when replacing a constant (numerical value).
^p
example :
	#const KAZU 100+50
	a=KAZU
↓ (after deployment)
	a=150
^p
If the value to be used in the source is decided in advance, it is effective for speeding up the source. It is also possible to include macros that have already been defined, so
^p
example :
	#const ALL 50
	#const KAZU 100*ALL
	a=KAZU
↓ (after deployment)
	a=5000
^p
Can be used like.
Integers and real numbers can be used in the calculation formula.
You can use the same operator and number description styles as you would for a regular expression. You can also specify the ranking in parentheses.
^
By inserting "global" immediately after the #const instruction, you can create a macro that can be used permanently in all modules.
^p
example :
	#module
	#const global test 1234
	#global
a = test; 1234 is assigned to a
^p
Normally, if you define #const within a module, other modules and global areas will not recognize the same name.
By entering the global specification, you can replace the name defined everywhere after that with a macro.
^
By inserting "double" immediately after the #const instruction, the defined number is forcibly recognized as a real number.
If you do not specify this, an integer and a real number are automatically determined depending on whether there is a value after the decimal point.

%href
#define
%port+
Let


%index
#undef
Cancel macro name
%group
Preprocessor instructions
%prm
Macro name
%inst
Cancels the macro name that has already been registered.
Even if it is specified for a macro name that has not been registered, no error will occur and it will be ignored.
%href
#define
%port+
Let


%index
#if
Compile control from numerical values
%group
Preprocessor instructions
%prm
Numerical formula
%inst
Specifies compile ON / OFF.
If the specified number is 0, #if turns off the subsequent compilation output and ignores the compilation result (it is not executed as a program).
If the value is other than 0, the output will be ON.
This compile control targets the period until #endif appears.
#if, #ifdef, or #ifndef must have #endif in pairs.
^p
example :
	#if 0
mes "ABC"; This part is ignored
a = 111; This part is ignored
mes "DEF"; This part is ignored
	#endif
^p
You can also use an expression to specify #if, so
^p
example :
	#define VER 5
	#if VER<3
mes "ABC"; This part is ignored
a = 111; This part is ignored
mes "DEF"; This part is ignored
	#endif
^p
You can also use it like this. The description of the calculation formula and the precautions for the operator are the same as for the #const instruction.
You can also nest #if to #endif blocks.
^p
example :
	#ifdef SW
		#ifdef SW2
mes "AAA"; if SW and SW2 are defined
		#else
mes "BBB"; If SW is defined
		#endif
	#endif
^p
Basically, it can be used like a C or C ++ preprocessor.
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
Preprocessor instructions
%prm
Macro name
%inst
Specifies compile ON / OFF.
If the specified macro is not defined, #ifdef turns off the subsequent compilation output and ignores the compilation result. If defined, the output will be ON. This compile control targets the period until #endif appears.
For more information on compile control, see the #if instruction reference.
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
Preprocessor instructions
%prm
Macro name
%inst
Specifies compile ON / OFF.
If the specified macro is defined, #ifndef turns off the subsequent compilation output and ignores the compilation result. If it is not defined, the output will be ON. This compile control targets the period until #endif appears.
For more information on compile control, see the #if instruction reference.
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
Preprocessor instructions
%inst
Invert ON / OFF within the compile control section such as #if, #ifdef, #ifndef.
For more information on compilation control, see the #if instruction reference.
%href
#if
#ifdef
#ifndef
#endif
%port+
Let


%index
#endif
Compile control block end
%group
Preprocessor instructions
%inst
Terminates compile control sections such as #if, #ifdef, and #ifndef.
For more information on compilation control, see the #if instruction reference.
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
Preprocessor instructions
%prm
p1 p2 p3,…
p1: Name of the assigned instruction
p2 p3 ~: Parameter type name / alias name

%inst
Register a new instruction to process the module variable.
Specify the name of the new instruction in p1 and the call parameter type after p2.
After the position where the instruction is defined, the specified name can be used as the instruction word.
You can also treat the system variable thismod as your own module variable within the #modfunc routine.
^
The parameters of the #modfunc instruction have the same format as the #deffunc instruction.
The difference from the #deffunc instruction is that you need to specify a modular variable when calling. For more information on module variables, see the module entry in the programming manual (hspprog.htm).

%href
#modcfunc
#deffunc
#modinit
#modterm
thismod



%index
#modcfunc
Assign a new function
%group
Preprocessor instructions
%prm
p1 p2 p3,…
p1: Name of the assigned instruction
p2 p3 ~: Parameter type name / alias name

%inst
Register a new function to handle module variables.
Specify the name of the new instruction in p1 and the call parameter type after p2.
After the position where the instruction is defined, the specified name can be used as the instruction word.
You can also treat the system variable thismod as your own module variable within the #modcfunc routine.
^
The parameters of the #modcfunc instruction have the same format as the #defcfunc instruction.
The difference from the #defcfunc instruction is that you need to specify a modular variable when calling. For more information on module variables, see the module entry in the programming manual (hspprog.htm).

%href
#modfunc
#deffunc
#modinit
#modterm
thismod



%index
#modinit
Registration of module initialization process
%group
Preprocessor instructions
%prm
p1 p2,…
p1 p2 ~: Parameter type name / alias name

%inst
Register the process (constructor) for initializing the module variable.
You can optionally specify the call parameter type and alias name.
The interval defined by #modinit will be called automatically when the newmod instruction is executed.
In addition, the optional parameters specified by the newmod instruction can be obtained on the constructor side.
For more information on module variables, see the module entry in the programming manual (hspprog.htm).
^
The parameter type and alias name of the #modinit instruction have the same format as the #deffunc instruction.

%href
#deffunc
#modfunc
#modterm
newmod



%index
#modterm
Registration of module release process
%group
Preprocessor instructions
%prm

%inst
Register the process (destructor) for destroying the module variable.
You can optionally specify the call parameter type and alias name.
The interval defined by #modterm is automatically called when the delmod instruction is executed or the module variable is destroyed.
For more information on module variables, see the module entry in the programming manual (hspprog.htm).

%href
#deffunc
#modfunc
#modinit
delmod



%index
#regcmd
Registration of extension plugin
%group
Preprocessor instructions
%prm
"Initialization function name", "DLL file name", variable type extension number
"Initialization function name": Export name of the plugin initialization function
"DLL file name": DLL name of the plugin initialization function
Number of variable type extensions (0): Number of variable types extended by the plugin

%inst
Register the HSP extension plug-in.
The initialization function name must specify exactly the name exported from the DLL. When exporting from VC ++, "_" is added at the beginning and "@ 4" is added at the end, so describe the name including it. (For DLLs created with compilers other than VC ++, the export name rules are different. For details, refer to the documentation for each environment.)
For example, if you want to register a function called "hsp3cmdinit" in "hpi3sample.dll",
^p
example :
	#regcmd "_hsp3cmdinit@4","hpi3sample.dll"
^p
It looks like.
When extending a variable type with a plug-in, it is necessary to specify "Variable type extension number".
To add one type of variable type, specify 1 for the number of variable type extensions. If you do not want to extend the variable type, omit the number of variable type extensions or specify 0. Note that if the variable type extension is not specified correctly, the type registration will be invalid.
^
If you specify an integer for the first parameter, the type value to be executed as an internal command is assigned to the keywords defined in the subsequent #cmd.
The type value is a unique value used inside HSP3. Normally, you do not need to use this feature. Also, if you specify "*" instead of an integer, an empty type value is automatically assigned.


%href
#cmd
#uselib
#func


%index
#cmd
Registration of extended keywords
%group
Preprocessor instructions
%prm
New keyword sub ID
New keyword: Keyword to be added
Sub ID: Sub ID value given by the keyword

%inst
Register keywords for HSP extension plugins.
It is necessary to register the plug-in initialization function in advance by using the #regcmd command.
^p
example :
	#cmd newcmd $000
	#cmd newcmd2 $001
	#cmd newcmd3 $002
^p
In the above example, the keyword "newcmd" is registered as sub-ID0, the keyword "newcmd2" is registered as sub-ID1, and the keyword "newcmd3" is registered as sub-ID2.


%href
#regcmd
#uselib
#func



%index
#usecom
Specifying an external COM interface
%group
Preprocessor instructions
%prm
Interface name "interface IID" "class IID"
Interface name: Keywords that identify the interface
"Interface IID": IID string indicating the COM interface
"Class IID": IID string indicating the class of COM

%inst
Define the external component (COM).
Assign class IID and interface IID to the specified interface name to make it available.
IID can be specified with a string ({~}) similar to the registry.
Also, "class IID" can be omitted.
^p
example :
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
External COM call instruction registration
%group
Preprocessor instructions
%prm
New name Index type name 1,…
New name: Keyword name recognized as an instruction
Index: method index value
Type name: Specify the argument types separated by,

%inst
Register a new instruction to call the external component (COM).
#comfunc registers to call the interface method specified by the #usecom instruction as an instruction.
After that, you can call the component by combining the instruction specified by the new name and the variable of COM object type.
^
Write the new name, index, and type separated by a space.
By putting "global" just before the new name, you can create an instruction that can be used permanently in all modules.
^
Describe the details of the argument in the type name.
As with the #deffunc instruction, specify the argument types separated by ",".
There is no limit to the number of arguments or the order of the types.
The character strings that can be used as the argument type are as follows.
^p
   Type: Contents
 -------------------------------------------------------------
   int: Integer value (32bit)
   var: Variable data pointer (32bit)
   str: String pointer (32bit)
   wstr: unicode string pointer (32bit)
   double: real number (64bit)
   float: real number (32bit)
   pval: Pointer of PVal structure (32bit)
   bmscr: (*) Pointer of BMSCR structure (32bit)
   hwnd: (*) Handle of currently selected window (HWND)
   hdc: (*) Device context of the currently selected window (HDC)
   hinst: (*) Running HSP instance handle
^p
Items marked with (*) indicate parameters that are automatically passed without having to be specified as arguments.
^p
example :
; Class ID of shell link object
	#define CLSID_ShellLink "{00021401-0000-0000-C000-000000000046}"
Interface ID of the IShellLink interface
	#define IID_IShellLinkA "{000214EE-0000-0000-C000-000000000046}"

	#usecom IShellLinkA IID_IShellLinkA CLSID_ShellLink
	#comfunc IShellLink_SetPath 20 str

	newcom slink, IShellLinkA
	IShellLink_SetPath slink, "c:\\hsp261\\hsp2.exe"
^p
In the above example, IShellLink_SetPath of the IShellLinkA interface is called with a variable called slink.
Note that the instruction registered with #comfunc will always have the same first argument as a COM object type variable with the same interface.

%href
#usecom
newcom
delcom
querycom



%index
#enum
List macro name constants
%group
Preprocessor instructions
%prm
Macro name = p1
Macro name: Macro name to which a constant is assigned
p1: Allotted constant
%inst
Assigns consecutive values to the specified macro name.
Similar to the #const instruction, you can define a macro name that indicates a constant.
^p
example :
#enum KAZU_A = 0; KAZU_A becomes 0
#enum KAZU_B; KAZU_B becomes 1
#enum KAZU_C; KAZU_C becomes 2
	a=KAZU_B
↓ (after deployment)
	a=1
^p
The constant is initialized by writing "= (equal)" and a numerical value (or expression) after the macro name. After that, the number will increase by 1 each time it is defined with the #enum instruction.
Use the #enum instruction when you want to define consecutive values by macro.
The #const command eliminates the need to specify a number one by one, making it easy to add or delete later.

%href
#const
%port+
Let




%index
#runtime
Runtime file settings
%group
Preprocessor instructions
%prm
"Runtime name"
"Runtime name": Runtime name to be set

%inst
Sets the runtime file name used to run the script.
(For the runtime file name, specify the name part of the file without the extension.)
It is used to set the runtime when executing from the script editor or when creating an executable file.
If multiple #runtime instructions are set, the last set content will be valid.

%href
#packopt
#cmpopt
%port+
Let
%portinfo
When using HSPLet, specify "hsplet3" as the runtime name.


%index
#cmpopt
Compile time settings
%group
Preprocessor instructions
%prm
Option name parameter
Option name: Option type
Parameter: Parameter to set

%inst
Specifies the behavior when compiling the script.
Parameter with option name, followed by space or TAB
(In the case of a character string, specify it as "" strings "").
The keywords that can be specified with #cmpopt are as follows.
^p
  Options | Contents | Initial value
 ------------------------------------------------------
  ppout | Preprocessor file output | 0
             | (0 = none / 1 = output) |
  optcode | Unnecessary code optimization | 1
             | (0 = none / 1 = optimize) |
  optinfo | Optimization information log output | 0
             | (0 = none / 1 = output) |
  varname | except when debugging | 0
             | Output of variable name information |
             | (0 = none / 1 = yes) |
  varinit | Checking Uninitialized Variables | 0
             | (0 = warning / 1 = error) |
  optprm | Parameter code optimization | 1
             | (0 = none / 1 = optimize) |
  skipjpspc | Ignore double-byte spaces | 1
             | (0 = error / 1 = ignore) |
  utf8 | Output string in UTF-8 format | 0
             | (0 = invalid / 1 = valid) |
 ------------------------------------------------------
^p
The following example outputs the preprocessor result to a file.
^p
example :
	#cmpopt ppout 1
^p
Basically, write the #cmpopt instruction at the beginning of the script.
If multiple #cmpopt instructions are written, the last setting will take effect for each option. (Same as #packopt option)
Also, it is not possible to write an option that applies only to a specific range.

%href
#packopt
#runtime
%port+
Let



%index
#bootopt
Runtime settings
%group
Preprocessor instructions
%prm
Option name parameter
Option name: Option type
Parameter: Parameter to set

%inst
Make detailed operation settings for the script runtime.
Enter the value of the setting switch 1 or 0 by inserting a space or TAB after the option name.
#bootopt can be written anywhere in the script.
If there are multiple specifications, the last one set will be the overall setting.

The keywords that can be specified with #bootopt are as follows.
^p
  Options | Contents | Initial value
 -----------------------------------------------------------
  notimer | Use high precision timer | Automatic setting
             | (0 = use / 1 = not use) |
  utf8 | Use UTF-8 format strings | Automatic setting
             | (0 = use / 1 = not use) |
  hsp64 | Using 64-bit runtime | Automatic configuration
             | (0 = use / 1 = not use) |
 -----------------------------------------------------------
^p
The following example suppresses the use of the precision timer.
^p
example :
	#bootopt notimer 1
^p

%href
#cmpopt
#runtime



%index
#aht
Describe AHT file header
%group
Preprocessor instructions
%prm
Setting name p1
Setting name: The name of the setting item to which the constant is assigned
p1: Allotted constant
%inst
Adds AHT file information to the source script.
You can assign a string or number to the specified setting name.
With the #aht command, the source script with the AHT file header added can be referenced as an AHT file from the template manager, etc.
Can be used as a setting name
^p
  Setting name | Contents
 ------------------------------------------------------------
  class | Specifies the class name of the AHT file
  name | Specifies the name of the AHT file
  author | Specifies the author name of the AHT file
  ver | Specifies the AHT file version
  exp | Specifies a description for the AHT file
  icon | Specifies an icon file specific to the AHT file
  iconid | AHT Specifies a file-specific icon ID
 ------------------------------------------------------------
^p
For more information on AHT files, please refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#ahtmes
%port+
Let


%index
#ahtmes
Output of AHT message
%group
Preprocessor instructions
%prm
p1
p1: Output string expression
%inst
Outputs a message to the outside during AHT parsing.
It is mainly used to write the source code added on the editor by "Easy input".
Like the mes instruction, the #ahtmes instruction can output strings and macros by connecting them with the "+" operator.
^p
example :
#define Variable to assign a ;; str
#const Random number range 100 ;; help = "occurs from 0 to specified range -1"
#ahtmes "" + Variable to be assigned + "= rnd ("+ Range of random numbers + ") \\ t \\ t; Variable" + Variable to be assigned + "is assigned a random number."
^p
Note that unlike the normal mes instruction, it only connects macros defined on the preprocessor.
For more information on AHT files, please refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#aht
%port+
Let





