;
;	HSP help manager HELP source file
;	(Lines starting with ";" are treated as comments)
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
Registers the keyword specified by macro name with the preprocessor so that it can be replaced with the specified definition.
The #define macro is primarily for personal customization to make scripting easier and is not intended for beginners. The functions described here are not used frequently, so refer to them only when necessary.
The #define macro basically registers a replacement string.
^p
Example:
	#define hyouji mes
	hyouji "AAAAA..."
		↓(After expansion)
	mes "AAAAA..."
^p
By placing "global" immediately after the #define command, you can create a macro that can be used permanently in all modules.
^p
Example:
	#module
	#define global test 0x1234
	#global
	a=test   ; 0x1234 is assigned to a
^p
Normally, if #define is defined within a module, the same name is not recognized in other modules or in the global area.
By adding a global specification, you can replace the defined name with a macro in all subsequent locations.
^
In addition to simple replacement macros, argument-based expansion is possible.
Arguments are specified using argument names %1, %2, %3, etc., enclosed in parentheses after the macro name.
Arguments must be specified as "%number," and the numbers must be written in order from 1. Note that you cannot specify symbol names as in the C or C++ preprocessor.
^p
Example:
	#define hyouji(%1) mes "prm="+%1
	hyouji "AAAAA..."
		↓(After expansion)
	mes "prm="+"AAAAA..."
^p
In addition, you can set an initial (default) value for arguments.
^p
Example:
	#define hyouji(%1="PRM=",%2=123) mes %1+%2
	hyouji "AAA",a
	hyouji "BBB"
	hyouji ,b
		↓(After expansion)
	mes "AAA"+a
	mes "BBB"+123
	mes "PRM="+b
^p
The initial (default) value is a value that is automatically completed if it is omitted when the macro is used. If the initial value is omitted, it will not be completed.
In the macro argument specification, the parentheses are enclosed on the #define side, but do not specify parentheses when actually using it.
^p
	#define hyouji(%1) mes "prm="+%1
	hyouji("AAAAA...")
^p
Note that the above description will result in an error.
However, you can use the ctype option to enable the following parenthesized description.
^p
	#define ctype kansu(%1) %1*5+1
	a=kansu(5)
^p
This option is useful when you want to use a macro in a place other than the command part, such as a calculation formula. At first glance, it behaves like a function in C language, etc., but since it is actually replaced with a macro, the range of applications is narrow, so be careful.
Since this description method is different from the original HSP syntax, it is not recommended to use it for anything other than customization to describe it in your own style.
^
It is possible to set keywords that perform special operations when expanding a macro.
This special keyword is mainly for sharing parameters between different macros and for realizing a nested structure using a stack.
^p
	#define start(%1) %tstart %s1 mes "START"
	#define owari %tstart mes %o
^p
The special expansion macros specified here are "%s1" and "%o".
A sample using this is expanded as follows.
^p
	start "OK" → mes "START"
	owari → mes "OK"
^p
In this way, it is possible to share data between different macros.
The following are the types and functions of special expansion macros.
^p
   Macro : Function
 ----------------------------------------------------------------
     %t   : Set tag name
     %n   : Generate a unique label name
     %i   : Generate a unique label name and push it onto the stack
     %o   : Take out the string stacked on the stack
     %p   : Take out the string stacked on the stack (maintain the stack)
     %s   : Push argument parameters onto the stack
     %c   : Perform a line feed
^p
Special expansion macros are expressed by "%" followed by one English character + parameter.
To distinguish it from subsequent keywords, put a half-width space after the special expansion macro. The part including the space such as "%tabc aaa" is judged as a special expansion macro.
Special expansion macros have a general stack (First In Last Out).
This stack can be shared by macros with the same tag name.
The tag name is specified by "%t tag name" with "%t" followed by a half-width alphanumeric character within 16 characters. In the previous example, "start" specified as "%tstart" is the tag name.
"%s" is a special expansion macro for pushing argument parameters onto the stack.
If "%s1" is specified, "%1" parameter is pushed onto the stack.
To retrieve a string stacked on the stack, use "%o".
"%o" takes out and expands the string stacked on the stack. Since it is a stack, the last thing pushed in is the first thing taken out.
If "%o0" is specified, the stack is taken out, but the string is not expanded (stack retrieval only).
"%p" retrieves only the contents without returning the stack. "%p0" expands the contents of the stack to be taken out next. "%p1" retrieves a stack that is one level deeper. You can specify "%p0" to "%p9" thereafter.
The following shows an example of label generation.
^p
	#define start %tstart *%i
	#define owari %tstart await 20:stick a:if a=0  :  goto
*%o
^p
A sample using this is expanded as follows.
^p
	start → *_start_0000
	owari → await 20:stick a:if a=0 : goto *_start_0000
^p
"%i" generates a unique label name that does not overlap with others and pushes it onto the stack. If "%i0" is specified, the label name is pushed onto the stack, but it is not expanded.
Also, "%n" generates and expands a unique label name, but does not push it onto the stack.
In the above example, a new label is created by label name generation to realize a loop structure. By using this method, it is possible to construct a loop structure in which label names do not overlap even if they are nested.
Also, macros (tag names) in which all stacks have not been taken out in one source script file will report an error at compile time.
Be sure to create a macro command configuration that ends with all stacks taken out.
^
The standard definition macros while~wend, do~until, and for~next added in HSP ver2.6 are created by special expansion macros.
^
In special cases, it is possible to expand with a line feed inserted by "%c".
The line is split and expanded at the "%c" part. It is mainly used for defining macros that are expanded into multiple preprocessor statements.
However, not all preprocessors currently support macro expansion. If you use it too much, it may become difficult to see, so please use it after fully understanding it.
^p
Example:
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
You must specify the external DLL name in advance with the #uselib command.
Write the new name, function name, and type separated by spaces.
By placing "global" immediately before the new name, you can create a command that can be used permanently in all modules.
^
The function name can be written with the complete export name of the DLL by enclosing it in double quotes, such as "function name".
If it is not enclosed in double quotes, it will be converted to a name based on the VC++ export convention "_function name@16".
^
Specify the details of the arguments in the type name.
As with the #deffunc command, specify the argument type separated by ",".
There are no restrictions on the number of arguments or the order of types.
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
Items with (*) are parameters that are automatically passed without having to be specified as arguments.
The following is an example of executing with four arguments specified.
^p
Example:
	#uselib "test.dll"
	#func test "_func@16" var,int,int,int
	test a,1,2,3    ; test.dll's func(&a,1,2,3) is called
^p
If you specify a number for the type, an argument compatible with the DLL type specification after ver2.5 will be automatically set. However, there are some incompatible specified values. In that case, an error will be reported at compile time.
^
The following points require attention in terms of compatibility with ver2.5.
^p
・The BMSCR structure can only refer to the fields from flag to color.
・The PVal structure is compatible with the PVAL2 structure of ver2.5.
・The DLL cannot rewrite the flag field (type type value) or the len field (array information) of the PVal structure.
・You cannot access a string type array variable from the PVal structure.
^p
In other respects, the same information as HSP ver2.5 is passed.
^
By putting "onexit" before the function name, it can be registered as a termination call function.
^p

Example:
	#func test onexit "_func@16" str,int,int
^p
In the example above, "_func@16" is automatically called when the application exits.

This section is for HSPLet; a method with the same name as the function will be called (see the HSPLet manual for details).

A new function to call an external DLL is registered.
The external DLL name must be specified in advance with the #uselib command.
Write the new name, function name, and type separated by spaces.
By placing "global" immediately before the new name, you can create a command that can be permanently used in all modules.
^
The function name can be enclosed in double quotes, like ""function name"", to describe the complete export name of the DLL.
If not enclosed in double quotes, it will be converted to a name based on the VC++ export convention, such as "_function name@16".
^
The type name describes the details of the arguments.
The argument parameters are the same as those used in the #func command.
See the #func reference for details.
The new name registered with the #cfunc command can be written as a function in an expression.
^p
Example:
	#uselib "test.dll"
	#cfunc test "_func@16" var,int,int,int
	res=test(a,1,2,3)  ; test.dll's func(&a,1,2,3) is called
^p
The integer value (32bit int) obtained as a result of the external call is returned as the return value of the registered function.
In HSP2.5 compatible calls, the value returned to the system variable stat is used as the return value of the function.

This section is for HSPLet; a method with the same name as the function will be called (see the HSPLet manual for details).

Specifies the file to be included.
The file specified here is combined with the original source script and compiled.
This is used to specify additional script files such as runtime specification and modules.
The #use command is also available to easily specify script files to be combined.

Specifies the file to be included.
The file specified here is combined with the original source script and compiled.
The behavior is the same as the #include command, but #addition continues without stopping as an error even if the file does not exist.
This is used to combine additional definition files such as runtime specifications and modules.

Easily specify additional script files.
This is used to specify additional script files such as runtime specification and modules in the same way as the #include command, but the #use command can specify only the name excluding the extension, such as "hsp3dish", instead of a string like ""hsp3dish.as"".
A script file with the extension ".as" appended to the name specified by the name parameter is combined.
If a file with the extension ".as" does not exist, a file with the extension ".hsp" is searched for.
If no file with either extension exists, an error will occur.
The name of the name parameter must be a character written with the same rules as variables.
In other words, "#use hsp3dish" and "#include "hsp3dish.as"" behave exactly the same.
Also, the #use command can describe multiple names in succession by separating them with the "," symbol.
^p
Example:
	#use strict,hgimg4,layer_fade
^p
In the above example, the files "strict.as", "hgimg4.as", and "layer_fade.as" will be combined.

Specifies the filename of the external DLL to be called from HSP.
The DLL filename must be written completely, including the extension.
If the filename is omitted, the DLL name will be specified from the script at runtime to perform linking.

This section is for HSPLet; a class with the same name as the specified DLL is loaded.
The name of the class to be loaded is the name with .dll or .hpi removed from the end.
For example, using winmm.dll will load the winmm class.
When you create your own extension library class, place the JAR file in the lib folder.
The compiler will automatically recognize it and allow for checks during compilation.

Ends the module section and returns to the normal program area.
See the #module command for details on modules.

Allocates the section after #module to a separate space as a module.
Variables and labels within a module are independent from those outside the module.
^
"Module name" is a name that can be given to distinguish multiple modules by name, and modules with the same module name share variable names and label names.
Variable names and label names are treated as completely different between modules with different module names.
^
If the "module name" is omitted, the name "m number" is automatically assigned.
^
The module must be started with "#module" and ended with "#global". By specifying the module section in this way, you can make the inside an independent space.
The module name must be 18 characters or less in length and cannot duplicate other keywords such as variable names.
Also, make sure to specify a string that does not contain spaces or symbols. (The character types that can be used in the module name are alphabets from a to z, numbers from 0 to 9, and the "_" symbol. It is the same as the string that can be used as a variable.)
^
The module variable name is for registering local variables associated with the module. The registered module variable name can be used within the processing routine defined by #modfunc or #modcfunc.
^
See the programming manual (hspprog.htm) for details on module variables.

Registers a new command created by the user.
Specify the name of the new command in p1, and the call parameter type in p2 and later.
It becomes possible to use the name defined by the #deffunc command as a command in the script.
^
The content to be executed for the new command is the line after the line specified by #deffunc.
Execution is performed as a subroutine jump like the gosub command, and returns to the original execution position with the return command.
^p
Example:
	#deffunc test int a
	mes "Parameter="+a
	return
^p
The added new command can now receive parameters.
For each parameter, it is possible to specify the parameter type and alias name. The following parameter types can be specified.
^p
   int     :  Integer value
   var     :  Variable (no array)
   array   :  Variable (with array)
   str     :  String
   double  :  Real number
   label   :  Label
   local   :  Local variable
^p
The alias name indicates the content of the passed parameter and can be used almost like a variable.
Care must be taken when distinguishing between var and array, and the local type indicating a local variable is for special purposes.
For details, see the module section of the programming manual (hspprog.htm).

As a special use, by describing "onexit" instead of the parameter type, it can be registered as a cleanup command. The cleanup command is automatically called when the HSP script execution ends.
^p
Example:
	#deffunc name onexit
^p
It can be used to perform post-processing, such as releasing systems and memory, when functions are extended by modules.
^
The name of the new command is usually shared by all module spaces and global spaces.
However, if a name is specified after the local specification, it is treated as unique to the module.
^p
Example:
	#module user
	#deffunc local test int a
	mes "Parameter="+a
	return
	#global
	test@user 5
^p
This can be used to define commands that are only used within the module.
The name registered by local specification must be called in the format "name@module name".

During HSPLet, argument types str/int/double/var/array are only supported.
local etc. cannot be specified.

Registers a new function.
Specify the name of the function to be registered in p1.
Specify the parameter type name and alias name in p2 and later.
%inst
Registers a new user-defined function.
p1 specifies the name of the new function, and p2 onwards specify the calling parameter types.
The name defined by the #defcfunc instruction can be used as a function within the script.
^
The content to be executed for the new function starts from the line specified by #defcfunc. Execution is performed as a subroutine jump, similar to the gosub instruction, and returns to the original execution position with the return instruction.
At that time, you must specify the return value parameter in the return instruction.
^p
Example:
	#defcfunc half int a
	return a/2
^p
The added new function can now accept parameters.
For each parameter, you can specify a parameter type and an alias name. The following parameter types can be specified:
^p
   int     :  Integer value
   var     :  Variable (without array)
   array   :  Variable (with array)
   str     :  String
   double  :  Real number value
   local   :  Local variable
^p
The alias name indicates the content of the passed parameter and can be used almost like a variable.
Note that var and array must be used carefully, and the local type indicating local variables has a special purpose.
For details, see the module item in the programming manual (hspprog.htm).
^
The name of the new function is usually shared in all module spaces and global spaces.
However, if a name is specified after a local designation, it is treated as module-specific.
^p
Example:
	#module user
	#defcfunc local test int a
	return a+5
	#global
	mes test@user(5)
^p
This can be used when defining a function that is only used within a module.
A name registered with the local designation must always be called in the format "name@module name".

%href
return
#deffunc
%port+
Let
%portinfo
When using HSPLet, the argument types are only supported as str/int/double/var/array.
local, etc., cannot be specified.

%index
#pack
Specification of files to be packed (3.6 specification)
%group
Preprocessor instruction
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the files to be packed in the automatic executable file creation (ctrl+F9).
The specified files are registered as embedded files as single files without considering the folder structure.
This instruction remains to specify files to be packed with the specifications up to HSP3.6.
For HSP3.7 and later, use the #packdir instruction.
%href
#packdir

%index
#epack
Specification of encrypted files to be packed (3.6 specification)
%group
Preprocessor instruction
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the files to be packed in the automatic executable file creation (ctrl+F9).
The specified files are registered as encrypted embedded files as single files without considering the folder structure.
This instruction remains to specify files to be packed with the specifications up to HSP3.6.
For HSP3.7 and later, use the #epackdir instruction.
%href
#epackdir

%index
#packdir
Specification of files to be packed
%group
Preprocessor instruction
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the files to be packed in the automatic executable file creation (ctrl+F9). The specified files are embedded as resources when creating the executable file. #packdir packs in the normal format. Use #epackdir if you want to pack with encryption.
If you try to add duplicate files, it will be ignored.
"start.ax" is automatically added when automatically creating an executable file, so it is not necessary to specify it as an additional file.
^p
Example:
	#packdir "a.txt"
^p
In the above example, the file "a.txt" is packed together with the executable file.
Wildcard specification using the "*" symbol is possible for file names.
If you specify "data/*", all files under the data folder will be packed.
Also, by specifying "*.png", you can pack all files with the extension ".png".
%href
#epackdir
#packopt

%index
#epackdir
Specification of files to be encrypted and packed
%group
Preprocessor instruction
%prm
"filename"
"filename" : File to be packed
%inst
Specifies the files to be packed in the automatic executable file creation (ctrl+F9). The specified files are embedded as resources when creating the executable file. #epackdir encrypts and packs the specified files.
Use #packdir if you don't need to encrypt.
If you try to add duplicate files, it will be ignored.
"start.ax" is automatically added when automatically creating an executable file, so it is not necessary to specify it as an additional file.
^p
Example:
	#epackdir "a.bmp"
^p
In the above example, the file "a.bmp" is encrypted and packed together with the executable file.
Wildcard specification using the "*" symbol is possible for file names.
If you specify "data/*", all files under the data folder will be packed.
Also, by specifying "*.png", you can pack all files with the extension ".png".

%href
#packdir
#packopt

%index
#packopt
Automatic creation option specification
%group
Preprocessor instruction
%prm
p1 p2
p1 : Keyword
p2 : Setting content
%inst
Specifies the operation of automatic executable file creation.
Write the keyword name, followed by a space or TAB, and then the parameters (specify strings like ""strings"").
The following keywords can be specified with #packopt.
^p
  Keyword |      Content        | Initial value
 ------------------------------------------------------
  name       | Executable file name         | "hsptmp"
  runtime    | Runtime to use     | "hsprt"
  type       | Executable file type   | 0
             | (0=EXE file)        |
             | (1=Full-screen EXE)  |
             | (2=Screen saver) |
  xsize      | Initial window X size  | 640
  ysize      | Initial window Y size  | 480
  hide       | Initial window hidden SW | 0
  orgpath    | Initial directory maintenance SW | 0
  icon       | Icon file setting   | None
  version    | Version resource setting | None
  manifest   | Manifest setting       | None
  lang       | Language code setting         | None
  upx        | UPX compression setting            | None
 ------------------------------------------------------
^p
In the following example, a screen saver called "test.scr" is created using a runtime called "hsp2c.hrt".
^p
Example:
	#packopt type 2
	#packopt name "test"
	#packopt runtime "hsp2c.hrt"
^p
When you perform "Automatic Executable File Creation", an executable file is generated based on the information of #packdir, #epackdir, and #packopt described in the script being edited.
At that time, "start.ax" is added by default as an encrypted one.
The runtime file (with the extension hrt) specified by "#packopt runtime "runtime file name"" is used from the same directory as hspcmp.dll or from the runtime directory.
The icon, version, manifest, lang, and upx keywords are set using the iconins tool after the executable file is generated.
The icon file must be specified in .ico format.
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
To use UPX compression, you need to download upx.exe (Win32 console version) in advance and place it in the same folder as the iconins tool.

%href
#packdir
#epackdir
#cmpopt

%index
#const
Constant definition of macro name
%group
Preprocessor instruction
%prm
Macro name Constant expression
%inst
Sets the replacement numerical value for the specified macro name.
This is similar to #define, but #const replaces the result of pre-calculation when replacing a constant (numerical value).
^p
Example:
	#const KAZU 100+50
	a=KAZU
		↓(after expansion)
	a=150
^p
It is effective for speeding up the source code if the values used in the source code are determined in advance. Since it is also possible to include already defined macros,
^p
Example:
	#const ALL 50
	#const KAZU 100*ALL
	a=KAZU
		↓(after expansion)
	a=5000
^p
It can be used as.
The calculation formula can use integers and real numbers.
The description style of operators and numerical values is the same as that of normal expressions. It is also possible to specify the order using parentheses.
^
By inserting "global" immediately after the #const instruction, you can create a macro that can be used permanently in all modules.
^p
Example:
	#module
	#const global test 1234
	#global
	a=test   ; 1234 is assigned to a
^p
Normally, if #const is defined in a module, the same name is not recognized in other modules or in the global area.
By inserting a global specification, you can replace the defined name with a macro in all subsequent locations.
^
By inserting "double" immediately after the #const instruction, the defined numerical value is forcibly recognized as a real number.
By inserting "int" immediately after the #const instruction, the defined numerical value is forcibly recognized as an integer.
If this specification is not performed, integers and real numbers are automatically determined depending on whether there is a value after the decimal point.
^p
Example:
	#const double dnum 1
	#const dval dnum*2
	mes""+dnum	; 1.0 is displayed
	mes""+dval	; 2 is displayed
^p
Please note that the handling of real numbers and integers differs from the normal calculation rules of HSP.

%href
#define
%port+
Let

%index
#undef
Cancellation of macro name
%group
Preprocessor instruction
%prm
Macro name
%inst
Cancels a macro name that has already been registered.
Specifying a macro name that is not registered does not cause an error and is ignored.
%href
#define
%port+
Let

%index
#if
Compilation Control from Numerical Value
%group
Preprocessor Directive
%prm
Numerical Expression
%inst
Specifies whether compilation is ON or OFF.
If the specified numerical value is 0, #if turns OFF subsequent compilation output and ignores the compilation result (it will not be executed as a program).
If the numerical value is other than 0, the output is ON.
This compilation control applies to the section up to the #endif.
One of #if, #ifdef, or #ifndef must have a paired #endif.
^p
Example :
	#if 0
	mes "ABC"       ; This part is ignored
	a=111           ; This part is ignored
	mes "DEF"       ; This part is ignored
	#endif
^p
Since expressions can be used in the #if specification,
^p
Example :
	#define VER 5
	#if VER<3
	mes "ABC"       ; This part is ignored
	a=111           ; This part is ignored
	mes "DEF"       ; This part is ignored
	#endif
^p
It can also be used like this. Notes on writing and operators in calculations are the same as for the #const directive.
Also, it is possible to nest #if〜#endif blocks.
^p
Example :
	#ifdef SW
		#ifdef SW2
		mes "AAA"       ; If SW and SW2 are defined
		#else
		mes "BBB"       ; If SW is defined
		#endif
	#endif
^p
Basically, it can be used in a similar way to the C and C++ preprocessors.
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
Compilation Control from Macro Definition
%group
Preprocessor Directive
%prm
Macro Name
%inst
Specifies whether compilation is ON or OFF.
#ifdef turns OFF subsequent compilation output and ignores the compilation result if the specified macro is not defined. If it is defined, the output is ON. This compilation control applies to the section up to the #endif.
See the reference for the #if directive for details on compilation control.
%href
#if
#ifndef
#else
#endif
%port+
Let

%index
#ifndef
Compilation Control from Macro Definition 2
%group
Preprocessor Directive
%prm
Macro Name
%inst
Specifies whether compilation is ON or OFF.
#ifndef turns OFF subsequent compilation output and ignores the compilation result if the specified macro is defined. If it is not defined, the output is ON. This compilation control applies to the section up to the #endif.
See the reference for the #if directive for details on compilation control.
%href
#if
#ifdef
#else
#endif
%port+
Let

%index
#else
Invert Compilation Control
%group
Preprocessor Directive
%inst
Inverts ON/OFF within a compilation control section such as #if, #ifdef, #ifndef.
See the reference for the #if directive for details on compilation control.
%href
#if
#ifdef
#ifndef
#endif
%port+
Let

%index
#endif
End Compilation Control Block
%group
Preprocessor Directive
%inst
Ends a compilation control section such as #if, #ifdef, #ifndef.
See the reference for the #if directive for details on compilation control.
%href
#if
#ifdef
#ifndef
#else
%port+
Let

%index
#modfunc
Assign a New Command
%group
Preprocessor Directive
%prm
p1 p2 p3,…
p1      : Name of the command to be assigned
p2 p3〜 : Parameter type name/alias name

%inst
Registers a new command for processing module variables.
Specify the name of the new command in p1, and the calling parameter types in p2 and subsequent parameters.
After the position where the command is defined, the specified name can be used as a command word.
Also, within the #modfunc routine, the system variable thismod can be treated as its own module variable.
^
The parameters of the #modfunc command are in the same format as the #deffunc command.
The difference from the #deffunc command is that it is necessary to specify a module-type variable when calling it. For details on module variables, see the module item in the programming manual (hspprog.htm).

%href
#modcfunc
#deffunc
#modinit
#modterm
thismod

%index
#modcfunc
Assign a New Function
%group
Preprocessor Directive
%prm
p1 p2 p3,…
p1      : Name of the command to be assigned
p2 p3〜 : Parameter type name/alias name

%inst
Registers a new function for processing module variables.
Specify the name of the new command in p1, and the calling parameter types in p2 and subsequent parameters.
After the position where the command is defined, the specified name can be used as a command word.
Also, within the #modcfunc routine, the system variable thismod can be treated as its own module variable.
^
The parameters of the #modcfunc command are in the same format as the #defcfunc command.
The difference from the #defcfunc command is that it is necessary to specify a module-type variable when calling it. For details on module variables, see the module item in the programming manual (hspprog.htm).

%href
#modfunc
#deffunc
#modinit
#modterm
thismod

%index
#modinit
Register Module Initialization Processing
%group
Preprocessor Directive
%prm
p1 p2,…
p1 p2〜 : Parameter type name/alias name

%inst
Registers processing (constructor) to initialize module variables.
You can specify the calling parameter type and alias name as options.
The section defined by #modinit is automatically called when the newmod command is executed.
Also, it is possible to acquire the optional parameters specified by the newmod command on the constructor side.
For details on module variables, see the module item in the programming manual (hspprog.htm).
^
The parameter type and alias name of the #modinit command are in the same format as the #deffunc command.

%href
#deffunc
#modfunc
#modterm
newmod

%index
#modterm
Register Module Release Processing
%group
Preprocessor Directive
%prm

%inst
Registers processing (destructor) to discard module variables.
You can specify the calling parameter type and alias name as options.
The section defined by #modterm is automatically called when the delmod command is executed or when the module variable is discarded.
For details on module variables, see the module item in the programming manual (hspprog.htm).

%href
#deffunc
#modfunc
#modinit
delmod

%index
#regcmd
Register Extension Plug-in
%group
Preprocessor Directive
%prm
"Initialization Function Name","DLL File Name",Number of Variable Type Extensions
"Initialization Function Name"  : Export name of the plug-in initialization function
"DLL File Name" : DLL name of the plug-in initialization function
Number of Variable Type Extensions (0) : Number of variable types extended by the plug-in

%inst
Registers an HSP extension plug-in.
The initialization function name must be specified exactly as the name exported from the DLL. If it is exported from VC++, "_" is added to the beginning and "@4" is added to the end, so write the name including them. (For DLLs created with compilers other than VC++, the export name rules are different. For details, see the documentation for each environment.)
For example, to register the function "hsp3cmdinit" in "hpi3sample.dll",
^p
Example :
	#regcmd "_hsp3cmdinit@4","hpi3sample.dll"
^p
It becomes like this.
When extending the variable type with a plug-in, it is necessary to specify the "Number of variable type extensions".
If you want to add one type of variable, specify 1 for the number of variable type extensions. If you do not want to extend the variable type, omit the number of variable type extensions or specify 0. Note that type registration will be invalid unless the number of variable type extensions is specified correctly.
^
If an integer is specified as the first parameter, the type value to be executed as an internal command is assigned to the keyword defined by #cmd thereafter.
The type value is a unique value used internally by HSP3. Normally, it is not necessary to use this function. If "*" is specified instead of an integer, an empty type value is automatically assigned.

%href
#cmd
#uselib
#func

%index
#cmd
Register Extension Keyword
%group
Preprocessor Directive
%prm
New Keyword Sub ID
New Keyword : Keyword to be added
Sub ID         : Sub ID value given to the keyword

%inst
Registers a keyword for the HSP extension plug-in.
It is necessary to register the plug-in initialization function in advance with the #regcmd directive.
^p
Example :
	#cmd newcmd $000
	#cmd newcmd2 $001
	#cmd newcmd3 $002
^p
In the above example, the keyword "newcmd" is registered as sub ID 0, the keyword "newcmd2" as sub ID 1, and the keyword "newcmd3" as sub ID 2.

%href
#regcmd
#uselib
#func

%index
#usecom
Specify External COM Interface
%group
Preprocessor Directive
%prm
Interface Name "Interface IID" "Class IID"
Interface Name    : Keyword to identify the interface
"Interface IID" : IID string indicating the COM interface
"Class IID"           : IID string indicating the COM class

%inst
Defines an external component (COM).
Assigns the class IID and interface IID to the specified interface name to make it usable.
The IID can be specified as a string ({〜}) similar to the registry.
Also, "Class IID" can be omitted.
^p
Example :
	#define CLSID_ShellLink "{00021401-0000-0000-C000-000000000046}"
	#define IID_IShellLinkA "{000214EE-0000-0000-C000-000000000046}"
	#usecom IShellLinkA IID_IShellLinkA CLSID_ShellLink
^p
By inserting "global" immediately before the interface name, you can create an interface that can be used permanently in all modules.

%href
#comfunc
newcom
delcom
querycom

%index
#comfunc
Register External COM Call Command
%group
Preprocessor Directive
%prm
New Name Index Type Name1,…
New Name     : Keyword recognized as a command
Index : Method index value
Type Name     : Specifies the argument type separated by commas
%inst
Registers a new instruction for calling external components (COM).
#comfunc registers a method of the interface specified by the #usecom instruction as an instruction.
After that, you can call the component by combining the instruction specified with the new name and the variable of the COM object type.
^
Write the new name, index, and type, separated by spaces.
By placing "global" directly before the new name, you can create an instruction that can be used permanently in all modules.
^
Describe the details of the arguments in the type name.
As with the #deffunc instruction, specify the argument types separated by ",".
There are no restrictions on the number of arguments or the order of the types.
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
   hinst   :  (*)HSP instance handle being executed
^p
Items marked with (*) indicate parameters that are automatically passed without the need to be specified as arguments.
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
In the above example, IShellLink_SetPath of the IShellLinkA interface is called together with a variable named slink.
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
Macro name : Macro name to which a constant is assigned
p1       : Constant to be assigned
%inst
Assigns consecutive values to the specified macro names.
Similar to the #const instruction, you can define macro names that represent constants.
^p
Example:
	#enum KAZU_A = 0	; KAZU_A becomes 0
	#enum KAZU_B		; KAZU_B becomes 1
	#enum KAZU_C		; KAZU_C becomes 2
	a=KAZU_B
		↓(After expansion)
	a=1
^p
By writing "=(equals)" and a numerical value (or expression) after the macro name, the constant is initialized. After that, the numerical value increases by 1 each time it is defined with the #enum instruction.
The #enum instruction is used when you want to define consecutive values using macros.
With the #const instruction, there is no need to specify the numerical value each time, and it is easy to add or delete later.

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
Sets the runtime file name used to execute the script.
(The runtime file name specifies the name part of the file, excluding the extension.)
It is used when setting the runtime when executing from the script editor or when creating an executable file.
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
Compilation settings
%group
Preprocessor instruction
%prm
Option name  Parameter
Option name : Type of option
Parameter : Parameter to set

%inst
Specifies the behavior during script compilation.
Write the option name, followed by a space or TAB, and the parameter (specify strings like "strings").
The following keywords can be specified with #cmpopt:
^p
  Option |      Content            | Initial Value
 ------------------------------------------------------
  ppout      | Preprocessor file output | 0
             | (0=None/1=Output)        |
  optcode    | Optimize unnecessary code | 1
             | (0=None/1=Optimize)      |
  optinfo    | Output optimization info log | 0
             | (0=None/1=Output)        |
  varname    | Output variable name       | 0
             | information except during  |
             | debugging (0=None/1=Yes)   |
  varinit    | Check uninitialized       | 0
             | variables (0=Warning/1=Error)|
  optprm     | Optimize parameter code   | 1
             | (0=None/1=Optimize)      |
  skipjpspc  | Ignore full-width spaces  | 1
             | (0=Error/1=Ignore)       |
  utf8       | Output strings in UTF-8   | 0
             | format (0=Invalid/1=Valid) |
 ------------------------------------------------------
^p
In the following example, the preprocessor result is output to a file.
^p
Example:
	#cmpopt ppout 1
^p
The #cmpopt instruction should basically be written at the beginning of the script.
If multiple #cmpopt instructions are written, the last setting will be valid for each option (#packopt option is the same).
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
Write the setting switch, a numerical value of 1 or 0, after the option name, followed by a space or TAB.
#bootopt can be written at any position in the script.
If multiple specifications are made, the last setting will be the overall setting.

The following keywords can be specified with #bootopt:
^p
  Option |      Content                | Initial Value
 -----------------------------------------------------------
  notimer    | Use high-precision timer | Automatic setting
             | (0=Use/1=Do not use)      |
  utf8       | Use UTF-8 format strings | Automatic setting
             | (0=Use/1=Do not use)      |
  hsp64      | Use 64-bit runtime       | Automatic setting
             | (0=Use/1=Do not use)      |
 -----------------------------------------------------------
^p
In the following example, the use of the high-precision timer is suppressed.
^p
Example:
	#bootopt notimer 1
^p

%href
#cmpopt
#runtime

%index
#aht
Describe AHT file header
%group
Preprocessor instruction
%prm
Setting name p1
Setting name : Name of the setting item to which a constant is assigned
p1     : Constant to be assigned
%inst
Adds AHT file information to the source script.
You can assign a string or numerical value to the specified setting name.
The source script to which the AHT file header is added by the #aht instruction can be referenced as an AHT file from the template manager, etc.
Can be used as a setting name
^p
  Setting name     |      Content
 ------------------------------------------------------------
  class      | Specifies the class name of the AHT file
  name       | Specifies the name of the AHT file
  author     | Specifies the author name of the AHT file
  ver        | Specifies the version of the AHT file
  exp        | Specifies the explanation of the AHT file
  icon       | Specifies the icon file unique to the AHT file
  iconid     | Specifies the icon ID unique to the AHT file
 ------------------------------------------------------------
^p
For details on AHT files, please refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#ahtmes
%port+
Let

%index
#ahtmes
Output AHT message
%group
Preprocessor instruction
%prm
p1
p1     : String expression to be output
%inst
Outputs a message to the outside during AHT parsing.
Mainly used to describe the source code added on the editor with "Easy Input".
The #ahtmes instruction can connect and output strings and macros with the "+" operator, similar to the mes instruction.
^p
Example:
	#define Variable to assign	a	;;str
	#const Range of random numbers 100		;;help="Occurs from 0 to specified range -1"
	#ahtmes	"	"+Variable to assign+" = rnd( "+Range of random numbers+" )\t\t; Assign a random number to variable "+Variable to assign+"."
^p
Note that, unlike the normal mes instruction, this is only for connecting macros defined on the preprocessor.
For details on AHT files, please refer to the document "Additional HSP Template & Tools" (aht.txt).

%href
#aht
%port+
Let

%index
#var
Declare variables to use
%group
Preprocessor instruction
%prm
var,...
var : Variable name to declare
%inst
Declares the variables to be used in the script.
By declaring the variables to be used in advance, it becomes possible to prevent them from being detected as uninitialized variables and to operate safely.
With the #var instruction, multiple variables can be written continuously by separating them with the "," symbol.
^p
Example:
	#var a,b,c
^p
In the above example, it is declared that three variables a, b, and c are used in the script.
In addition, #varint, #varstr, #vardouble, and #varlabel are also available for declaring variables with fixed types.
%href
#varint
#varstr
#vardouble
#varlabel

%index
#varint
Declares a variable with a fixed integer (int) type
%group
Preprocessor instruction
%prm
var,...
var : Variable name to declare
%inst
Declares a variable to be used in the script and fixes it as an integer (int) type.
If you try to change the type of a fixed-type variable to a different type during execution, an error will occur. This prevents the type of the variable from being changed unintentionally.
Type fixing is only detected during debug execution. Please note that it is invalid in scripts that have been made into executable files or in platform-converted environments (iOS, android).
Also, by declaring the variables to be used in advance, it becomes possible to prevent them from being detected as uninitialized variables and to operate safely.
Use the #var instruction to declare variables without fixing the type.
Similar to the #var instruction, multiple variables can be written continuously by separating them with the "," symbol.
%href
#var
#varstr
#vardouble
#varlabel

%index
#varstr
Declares a variable with its type fixed to String (str).
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as String (str) type.
If you attempt to change the type of a fixed-type variable during execution, an error will occur. This prevents the variable from unintentionally changing its type.
Type fixation is only detected during debug execution.  Note that it is disabled in scripts converted into executable files or platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, you can prevent them from being detected as uninitialized variables and operate safely.
To declare a variable without fixing its type, use the #var directive.
Similar to the #var directive, multiple declarations can be written consecutively by separating them with the "," symbol.
%href
#var
#varint
#vardouble
#varlabel

%index
#vardouble
Declares a variable with its type fixed to Real number (double).
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as Real number (double) type.
If you attempt to change the type of a fixed-type variable during execution, an error will occur. This prevents the variable from unintentionally changing its type.
Type fixation is only detected during debug execution.  Note that it is disabled in scripts converted into executable files or platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, you can prevent them from being detected as uninitialized variables and operate safely.
To declare a variable without fixing its type, use the #var directive.
Similar to the #var directive, multiple declarations can be written consecutively by separating them with the "," symbol.
%href
#var
#varint
#varstr
#varlabel

%index
#varlabel
Declares a variable with its type fixed to Label (label).
%group
Preprocessor Directive
%prm
var,...
var : Name of the variable to declare
%inst
Declares variables used in the script and fixes them as Label (label) type.
If you attempt to change the type of a fixed-type variable during execution, an error will occur. This prevents the variable from unintentionally changing its type.
Type fixation is only detected during debug execution.  Note that it is disabled in scripts converted into executable files or platform-converted environments (iOS, Android).
Also, by declaring the variables to be used in advance, you can prevent them from being detected as uninitialized variables and operate safely.
To declare a variable without fixing its type, use the #var directive.
Similar to the #var directive, multiple declarations can be written consecutively by separating them with the "," symbol.
%href
#var
#varint
#varstr
#vardouble
