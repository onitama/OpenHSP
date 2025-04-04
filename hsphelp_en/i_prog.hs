;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Built-in instructions
%ver
3.6
%note
ver3.6 standard instruction
%date
2013/12/09
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli
Let



%index
await
Wait for a certain amount of time
%group
Program control instructions
%prm
p1
p1 = 0 to (0): Waiting time (1ms unit)

%inst
Suspends program execution for a certain period of time.
^
Similar to the wait instruction, but the await instruction specifies the wait time since the last wait. This will prevent the time from passing too fast due to differences in drawing speed. It is used to keep the speed of screens that are updated in real time constant.
^
HSP recommends putting wait or await instructions where long loops can occur to pass processing time to other Windows tasks. If you run a program that loops for a long time inside an instruction that does not have a wait or await instruction, it may be difficult to drag the window or it may take a long time to move to another task.

%href
wait




%index
exec
Run Windows files
%group
Program control instructions
%prm
"filename",p1,"command"
"filename": Target file name
p1 = 0 to (0): File execution mode
"command": Context menu operation name

%inst
exec runs the application with the filename specified by "filename".
If the extension is EXE, you can omit the extension.
You can select the run-time mode with p1. If p1 is omitted, it will be 0.
^p
 Mode 0: HSP normal execution
 Mode 2: Run in minimized mode
 Mode 16: Run the associated application
 Mode 32: Print the specified file
^p
If "command" is specified, it will be the context menu operation name for the file or folder name specified in "filename".
The context menu operation name is a character string that specifies the action registered in the registry, and the following are provided as standard.
^p
  Operation name | Explanation
 ---------------------------------------------------------------------
  edit | Open the editor and edit.
  Select the explore | folder to launch Explorer.
  open | Open the file in the associated application.
  print | Print the file with the associated application.
  properties | View the properties of a file or folder.
^p
If "command" is specified, file execution modes 16 and 32 are disabled. In addition, a value indicating the execution result is returned in the system variable stat.
^
In normal execution, the window of the newly launched application becomes active and the input focus shifts, but in multitasking, HSP continues to execute without quitting.
^p
example :
exec "calc"; Start Windows Calculator
^p
You can also write parameters after the file name.
^p
example :
exec "notepad hsp.txt"; open hsp.txt with notepad
^p
Mode 2 runs with the specified application minimized.
When executing a DOS application, it can be executed in the back without blocking the HSP screen.
^
Mode 16 specifies a data file, not an executable file, to run the application associated with that extension.
^p
example :
exec "hsp.txt", 16; open hsp.txt with associated application
^p
In the above example, "hsp.txt" will be opened in the associated application.
(Usually notepad.exe)
In mode 16, it can also be used as an internet shortcut by specifying the internet URL.
^p
example :
Open internet page
	exec "http://www.onionsoft.net/hsp/",16
^p
Mode 32 prints to the printer via the application associated with the specified file.
for example,
^p
example :
exec "hsp.txt", 32; print hsp.txt with printer
^p
Automatically prints "hsp.txt" in the application associated with "txt". All printing options, etc. will be those specified in the associated application.
^
Multiple modes can be specified at the same time.
For example, if you specify mode 2 and mode 16 at the same time, add the respective values to get 18. In this case, it would be "Run associated application in minimized mode".
^
* Mode 1 (waiting for task execution), which was supported up to HSP2.61, is not supported by HSP3. Monitor the started process or use the pipeexec instruction for console applications.

%href
pipeexec
%portinfo
At HSPLet, it is used to display HTML pages.
It is not recommended to use this instruction directly because the meaning of the third argument is different from the original HSP.
We recommend that you use the showPage fileName, target instructions defined in letutil.hsp instead.



%index
end
The end of the program
%group
Program control instructions
%prm
p1
p1 (0): Exit code
%inst
Quit the HSP program.
The end instruction can be anywhere in the script.
When the end instruction is executed, the application will be terminated.
^
You can specify the exit code of the application with the parameter p1. If p1 is omitted, 0 is the application exit code.
The application exit code is used to notify other processes and batch files. Normally, it is okay to omit (0).

%href
stop





%index
gosub
Subroutine jump to the specified label
%group
Program control instructions
%prm
*label
* label: Label name

%inst
Make a subroutine jump.
The gosub instruction jumps to the location specified by * label.
After that, if there is a return instruction, it returns to the next line of the gosub instruction and continues execution.
gosub ~ return can be nested.
(Maximum number of nesting depends on the amount of stack memory of HSP)

%href
goto
return




%index
goto
Jump to the specified label
%group
Program control instructions
%prm
*label
* label: Label name

%inst
Unconditionally jumps to the specified label.
After that, execution continues from the location specified by the label name.

%href
gosub





%index
if
If the condition is met, the instruction on that line is executed.
%group
Program control instructions
%prm
p1
p1: Conditional expression

%inst
If the conditional expression of p1 is satisfied, the subsequent instructions are executed. If not, move to the next line.
Note that unlike BASIC, you must put a: (colon) between the next instruction on that line.
^p
example :
	a=10
if a = 10: mes "a is 10."
^p
In the above example, the "a = 10" part is the conditional expression. Mainly for conditional expressions
^p
a = b: a and b are equal
a! b: a and b are not equal
a <b: a is less than b
a> b: a is greater than b
a <= b: a is less than or equal to b
a> = b: a is greater than or equal to b
^p
Is used. After the if + conditional expression, separate it with a: (colon), and then write the part to be executed when the condition is met.
(Similar to C language and Java, "=" and "!" Can be described as "==" and "! =") If you want to change the flow of the program depending on the conditions,
^p
example :
	a=10
	if a>10 : goto *over10
mes "a is less than or equal to 10."
	stop
*over10
mes "a is greater than 10."
	stop
^p
As shown above, it is also possible to skip to another label with the goto command.
It is also possible to use the else instruction to write the processing when the condition is not met on the same line.
^p
example :
	a=10
mes "a is"
if a = 10: mes "10.": else: mes "not 10."
	stop
^p
In this case, up to a certain part of the else instruction is executed when the condition is satisfied, and after the else instruction, it is executed when the condition is not satisfied.
The following lines will be executed normally regardless of the conditions.
^
You can also use logical expressions to make more complex conditional decisions.
^p
   a & b: both a and b are correct (and)
   a | b: Either a or b is correct (or)
^p
This allows you to write multiple conditions at once.
^p
example :
	a=10:b=20
if (a = 10) | (b = 10): mes "either a or b is 10."
^p
In the above example, the conditional expressions "a = 10" and "b = 10" are connected with | (or) so that if either is correct, the result will be correct.
Logical operations can be described by the strings "or" and "and" in addition to the symbols "|" and "&".
^p
example :
if (a = 10) or (b = 10): mes "either a or b is 10."
^p
If you write it as above, the result will be the same.
^
If you want to write a script that is executed when the conditions of the if instruction are correct over multiple lines,
^p
example :
	a=10
	if a>5 {
		mes "TRUE"
		mes "(MULTILINE IF)"
	}
^p
Specify the part that starts with "{" and ends with "}" like.
(In the above example, TAB is inserted at the beginning of the line in C language style for easy viewing, but it is not necessary. Please write it so that it is easy for the user to see.)
^
You can also specify multiple lines with the else command.
^p
example :
	a=10
	if a>5 {
		mes "TRUE"
		mes "(MULTILINE IF)"
	}
	else {
		mes " FALSE"
		mes " (MULTILINE IF)"
	}
	return
^p
However, please note that if you do not write "{" after else, it will not be multiple lines.



%href
else





%index
loop
Return to the beginning of the loop
%group
Program control instructions
%inst
Specifies the end of the loop specified by the repeat / foreach instruction.
See the repeat instruction section for details.

%href
repeat
foreach




%index
onexit
Jump at the end
%group
Program control instructions
%prm
goto/gosub *label
* label: Label name

%inst
Specifies where to jump automatically when you press the close box (the exit button at the top right of the window).
^
After onexit, specify the goto or gosub keyword before writing the label.
In the case of goto, a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto / gosub keyword is omitted, the operation is the same as the goto keyword.
^p
example :
onexit gosub * exit_routine; call on exit
^p
Normally, pressing the close box will force it to terminate at any time, but after this instruction is executed, pressing the close box will jump to the label specified by the onexit instruction and continue execution from there. Will be.
^
Use this when you want to display a confirmation message when you are finished, or when you want to save the data when you are finished.
^
When this instruction is executed, the execution of the script cannot be stopped except for the end instruction, so use it with caution.
^
If you have created multiple windows, the exit button in one of the windows will cause an interrupt. At this time, the window ID notified of the end is assigned to the system variable wparam.
^
Immediately after jumping with onexit, the termination factor is saved as a value in the system variable iparam.
If iparam is 0, the program is terminated at the user's will.
If iparam is 1, it is terminated by shutting down Windows (restarting or turning off the power). For termination processing when Windows shuts down, if an idle time (waiting time) is generated in the system by await, wait, stop instructions, etc., shut down.
Cancels processing (does not shut down).
If it ends with the end instruction without using the await, wait, and stop instructions, the shutdown process continues as it is.
^
It is also possible to temporarily turn on / off the jump at the end.
^p
	onexit 0
^p
Temporarily stops the interrupt with.
^p
	onexit 1
^p
You can resume the interrupt that was paused with.


%href
onkey
onclick
onerror
%port-
Let



%index
return
Return from subroutine
%group
Program control instructions
%prm
p1
p1: System variable assignment value
%inst
Terminates a subroutine called by a gosub instruction or a user-defined instruction. The return instruction returns the stack and returns to the caller.
By specifying p1, the value can be reflected in the system variable from the subroutine.
If a numerical value is specified for p1, it will be assigned to the system variable stat.
If a character string is specified for p1, it will be assigned to the system variable refstr.
If you specify a real value for p1, it is assigned to the system variable refdval.
If p1 is omitted, system variables will not be assigned.

%href
gosub
#deffunc
#defcfunc
stat
refstr
refdval




%index
run
Transfer control to the specified file
%group
Program control instructions
%prm
"filename","cmdline"
"filename": HSP object file name to execute
"cmdline": A string assigned to the system variable dir_cmdline
%inst
Run the HSP program written in another file.
The file you specify must be an HSP object file (extension AX).
The object file can be created by "Create object file" in the script editor.
^p
example :
    run "MENU2.AX","-s"
^p
When is executed, the existing program disappears and the program in the file "MENU2.AX" is executed from the beginning.
The screen state is maintained, but all variables and memory buffer contents are initialized. Also, execution starts with the string "-s" assigned to the system variable dir_cmdline.
^
If the "cmdline" parameter is omitted, the contents of the system variable dir_cmdline will be
It will be empty. Also, the system variable dir_cmdline stores up to 1024 characters.
%href






%index
stop
Program interruption
%group
Program control instructions
%inst
Suspends program execution.
It is used when waiting for the button to be clicked.

%href
end





%index
wait
Suspend execution for a certain period of time
%group
Program control instructions
%prm
p1
p1 (100): Waiting time (10ms unit)

%inst
Suspends program execution for a certain period of time.
^
Specify the length of the waiting time in p1. The unit is 10 milliseconds. (1 millisecond = 1/1000 second)
All models have the same latency, regardless of CPU speed.
;If 0 is specified for p1 or omitted, wait for the same time as the previous time.
^
The await instruction allows for more precise and finer weights than the wait instruction, but consumes more CPU tasks than the wait instruction.
Unless it is a process such as rewriting an image in real time, using the wait command will reduce the load on the CPU (Windows).
It's a good idea to use the wait instruction for accessories that are always placed on the desktop, and the await instruction for game applications.
^
HSP recommends putting wait or await instructions where long loops can occur to pass processing time to other Windows tasks.
If you run a program that loops for a long time inside an instruction that does not have a wait or await instruction, it may be difficult to drag the window or it may take a long time to move to another task.

%href
await






%index
repeat
Indicates where the loop begins
%group
Program control instructions
%prm
p1,p2
p1 = 1 ~ (-1): Number of loops
p2 = 0 to (0): Start value of system variable cnt

%inst
Executes repeatedly between repeat and loop.
The repeat instruction indicates the starting point of the repeat.
The loop instruction returns to the last repeat instruction passed. in short,
^p
	repeat 5
print "Yaho"
	loop
^p
A program like this will display "Yaho" 5 times.
You can specify the number of repeats with the parameter of the repeat instruction, p1. If you omit the number of times or specify a negative value, an infinite loop will occur.
If the number of iterations is 0, the iteration part is not executed and jumps to the corresponding loop instruction.
^
You can also write a nested structure that further loops the part containing repeat ~ loop. However, if you normally exit through the loop instruction without looping, the nested structure will be strange, so please do not make it a structure that forcibly exits from the loop.
^
You can refer to the system variable cnt when you want to check the number of loops or when you want to use the counter. cnt usually starts at 0 and increments by 1 each time the loop instruction loops.
However, it is also possible to change the value at which the counter starts with p2 of the repeat instruction. For example, if you specify repeat 3,1, the value of the variable cnt changes in the order of 1,2,3.
^
If you want to force the loop out inside the loop, use the break instruction. There is also a continue instruction to move to the next loop.

%href
loop
break
continue
foreach



%index
break
Get out of the loop
%group
Program control instructions
%inst
Forcibly breaks out of the loop between repeat and loop.
^p
	repeat 5
		if cnt=2 : break
mes "Repeat [" + cnt + "]"
	loop
^p
In the above example, when the system variable becomes 2, that is, when it becomes the third loop, the break instruction is executed according to the judgment of the if instruction.
When the break instruction is executed, even if the number of repetitions still remains, it is forced to break out of the repetition and continue execution from the instruction following the loop instruction.
After the break instruction (mes instruction in the above example) is not executed.
Using this instruction, you can create the following script, for example.
^p
	repeat
		getkey a,1
		if a>0 : break
		await 10
	loop
^p
In the script above, it's a loop that waits until you press the left mouse button.
If you omit the number of repeat instructions, an infinite loop will occur, so you can use it to repeat the same process until the button status becomes 1. When the button is pressed, the break instruction is executed to break out of the iteration.

%href
repeat
loop
continue
foreach



%index
continue
Redo the loop
%group
Program control instructions
%prm
p1
p1 = 0 ~: Repeat counter change value

%inst
Repeat The loop between loop is repeated.
When the continue instruction is executed, it returns to the repeat instruction and executes the next iteration.
^p
	repeat 5
		if cnt=2 : continue
		mes "cnt="+cnt
	loop
^p
In the above example, the continue instruction is executed when the system variable cnt becomes 2.
When the above script is executed,
^p
	cnt=0
	cnt=1
	cnt=3
	cnt=4
^p
You can see that the mes instruction is not executed only when the system variable cnt is 2.
It may be confusing at first, but the continue instruction is not the location of the loop instruction, but it works the same as the loop instruction.
The repeat counter is incremented by 1 when the continue instruction is executed, just like the loop instruction.
If the continue instruction is executed at the last iteration, execution will continue from the state where repeat ~ loop is finished, that is, the instruction following the loop instruction.
^
There is another way to specify parameters in the continue instruction.
You can change the contents of the system variable cnt, which indicates a repeat counter, by specifying a numeric or numeric variable after continue.
For example, if "continue 1" is specified, the content of the system variable cnt becomes 1, and the repeat instruction is repeated with that value.
If you omit the parameter and just "continue", the process is the same as the loop instruction, and if you specify a numerical value, the counter value will be changed and the process will be repeated.
So
^p
	repeat 1
		await 10
		getkey a,1
		if a=0 : continue 0
	loop
^p
In such a script, repeat ~ loop is repeated, which should normally be executed only once, but if the left mouse button is not pressed, the counter is returned to 0 and an infinite loop is entered. Now you have to wait until you press the button.

%href
repeat
foreach
loop
break


%index
onkey
Key interrupt execution specification
%group
Program control instructions
%prm
goto/gosub *label
* label: Label name

%inst
Specifies where to jump automatically when you press the keyboard.
^
After onkey, specify the goto or gosub keyword before writing the label. In the case of goto, a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto / gosub keyword is omitted, the operation is the same as the goto keyword.
^
If you specify a label with the onkey instruction, it will jump to the label specified by * label every time there is a key input when the HSP window is active.
^
The interrupt jump accepts an interrupt and jumps when it is stopped by the stop instruction and the wait or await instruction.
In addition, the system variables iparam, wparam, and lparam are set after jumping by interrupt.
^p
   Interrupt factor | iparam | wparam | lparam
 ---------------------------------------------------------
    onkey | Character code | wParam | lParam
^p
Parameters for each interrupt factor are assigned to the system variable iparam.
In addition, wparam and lparam store the parameters passed as Windows messages as they are.
It is also possible to temporarily turn ON / OFF the execution of event interrupts.
^p
	onkey 0
^p
Temporarily stops the key interrupt with.
^p
	onkey 1
^p
You can resume the key interrupt that was paused with.


%href
onclick
onexit
onerror


%index
onclick
Click interrupt execution specification
%group
Program control instructions
%prm
goto/gosub *label
* label: Label name

%inst
Specifies where to jump automatically when the mouse button is pressed.
^
After onclick, specify the goto or gosub keyword before writing the label. In the case of goto, a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto / gosub keyword is omitted, the operation is the same as the goto keyword.
^
If you specify a label with the onclicky command, it will jump to the label specified by * label every time there is a mouse click on the HSP window.
^
The interrupt jump accepts an interrupt and jumps when it is stopped by the stop instruction and the wait or await instruction.
In addition, the system variables iparam, wparam, and lparam are set after jumping by interrupt.
^p
   Interrupt factor | iparam | wparam | lparam
 -------------------------------------------------------
   onclick | Mouse button ID | wParam | lParam
^p
Parameters for each interrupt factor are assigned to the system variable iparam.
In addition, wparam and lparam store the parameters passed as Windows messages as they are.
It is also possible to temporarily turn ON / OFF the execution of event interrupts.
^p
	onclick 0
^p
Temporarily stops the interrupt with.
^p
	onclick 1
^p
You can resume the interrupt that was paused with.


%href
onkey
onexit
onerror


%index
onerror
Jump when an error occurs
%group
Program control instructions
%prm
goto/gosub *label
* label: Label name
%inst
Specifies where to jump automatically when an error occurs inside the HSP due to a script.
^
After onerror, specify the goto or gosub keyword before writing the label. In the case of goto, a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto / gosub keyword is omitted, the operation is the same as the goto keyword.
^
Normally, the system error message dialog is displayed when an error occurs, but instead it is set to jump to the specified label. After the jump, the information is assigned to the following system variables.
^p
wparam: error number
lparam: Error line number
iparam: 0 (none)
^p
Even if you specify the processing after the error by the onerror instruction, when the necessary processing is completed, do not restart the application execution as it is, but end it with the end instruction as much as possible.
The onerror instruction does not recover from an error.
Depending on the cause of the error, the HSP system itself may become unstable or fail. As a scene to use the onerror instruction, if you want to display your own error on the application side when an error occurs when creating an executable file, or if you want to display for debugging only when a specific error occurs, you can think of it. Can be done.
^
It is also possible to temporarily turn on / off the jump at the end.
^p
	onerror 0
^p
Temporarily stops the interrupt with.
^p
	onerror 1
^p
You can resume the interrupt that was paused with.


%href
onkey
onclick
onexit


%index
exgoto
Conditional jump to the specified label
%group
Program control instructions
%prm
var,p1,p2,*label
var: Variables used for comparison
p1: Comparison flag
p2: Comparison value
* label: Label name
%inst
The value assigned to the variable specified by var and the label specified by the comparison value of p2 are jumped conditionally.
The comparison method is to jump when (var value> = p2) is satisfied when the comparison flag of p1 is 0 or more (positive value), and (var value <=) when p1 is -1 or less (negative value). Jump when p2) is established.
The variable specified by var must be of type integer. If it is initialized with a different type, an error will occur at runtime.
This instruction is provided to speed up some macro processing such as for.
Although the exgoto instruction can be used alone, it is recommended to perform normal conditional branching by the if instruction from the viewpoint of script visibility.
%href
goto
if


%index
on
Numerical branch
%group
Program control instructions
%prm
p1 goto / gosub Label 0, Label 1c
p1 = 0 to (0): Value for branching
Label 0 to: Label name of branch destination
%inst
The on instruction selects the branch destination according to the value specified by p1 being 0, 1, 2 ...
You can write one or more labels separated by "," in a format such as "on number goto" or "on number gosub".
If the specified number is 0, label 0 will be the target of branching, and if the number is 1, label 1 will be the target of branching. After that, describe any number of labels corresponding to 2, 3, 4 ... Is possible.
In the case of "on numeric goto", the same simple branch as the goto instruction is performed, and in the case of "on numeric gosub", the same subroutine jump as the gosub instruction is performed.
If the number is negative or no corresponding label is specified, no branching will occur and the next line will proceed.
Please note that unlike the n88 series BASIC, the first label corresponds to the number 0. (In n88 series BASIC, the number 1 is the first label)
%href
goto
gosub
%sample
	a=1
	on a goto *a0,*a1,*a2
mes "Other": stop
*a0
	mes "A=0":stop
*a1
	mes "A=1":stop
*a2
	mes "A=2":stop


%index
while
while iterative start
%group
Program control macro
%prm
p1
p1 = Conditional expression (1): Condition to repeat
%inst
Repeats from while to end only as long as the conditions following while are met.
If the condition is not met, while to wend will not be executed.
If the condition following while is omitted, it will be repeated infinitely.
^p
	a=0
	while a<5
	a=a+1:mes "A="+a
wend; repeat while and below only while a is 5 or less
^p
You can also restart and escape with the _continue and _break macros.
While-wend control is realized by using the macro function of the preprocessor.
Please use it when you want to write something similar to C or Java.
For beginners, we recommend repeating the description with the repeat ~ loop instruction or the if instruction.
%href
_continue
_break
wend


%index
wend
while iteratively ends
%group
Program control macro
%inst
Specifies the end of the repeating interval specified by while.
See the while macro section for details.
%href
while


%index
until
do repeat end
%group
Program control macro
%prm
p1
p1 = Conditional expression (1): Condition to repeat
%inst
Repeat the part from do to until until the conditions following until are met.
Execute in do ~ until at least once even if the conditions are met.
If the condition following until is omitted, the process will not be repeated.
^p
	a=0
	do
	a=a+1:mes "A="+a
until a> 5; repeat do or less until a is 5 or more
^p
You can also restart and escape with the _continue and _break macros.
Do ~ until control is realized by using the macro function of the preprocessor.
Please use it when you want to write something similar to C or Java.
For beginners, we recommend the repeat ~ loop instruction or the repeated description that combines the goto instruction and the if instruction.
%href
_continue
_break
do


%index
do
do repeat start
%group
Program control macro
%inst
Specifies the beginning of the repeating interval until until.
The repeat condition is described by the until macro.
See the until macro section for more information.
%href
until


%index
for
Start repeating specified number of times
%group
Program control macro
%prm
p1,p2,p3,p4
p1: Variable name
p2 (0): Initial value
p3 (0): Closing price
p4 (1): Increment
%inst
If you specify a variable name, initial value, closing price, increment, etc. as parameters, it repeats between for and next a specified number of times.
Values in parentheses are omitted. The variable name cannot be omitted.
The specified variable is used as a counter, starting from the initial value and incrementing each time it is repeated. When the closing price is reached, it exits the iteration (the closing price is not included in the loop).
If the closing price condition is met from the beginning, the iteration will not be executed.
You can also restart and escape with the _continue and _break macros.
^p
	for a,0,5,1
	mes "A="+a
next; a repeats between 0 and 4 (5 times)
^p
In this example, the variable a repeats 0,1,2,3,4 5 times to end the loop.
You can also specify a negative value for the increment.
^p
	for a,5,0,-1
	mes "A="+a
next; a repeats between 5 and 1 (5 times)
^p
In this case, the variable a repeats 5,4,3,2,1 5 times to end the loop.
^
It is necessary to specify an integer for the initial value and closing price specified by the for macro.
If you specify any other type, an error will occur.
Also note that depending on the increment or closing price specified, an infinite loop may occur.
^
For ~ next control is realized by using the macro function of the preprocessor.
Please use it when you want to write something similar to C or Java.
For beginners, we recommend repeating the description with the repeat ~ loop instruction or the if instruction. The for macro internally generates a special program control instruction exgoto after macro expansion.
%href
_continue
_break
next
exgoto


%index
next
Ends repeating the specified number of times
%group
Program control macro
%inst
Specifies the end of the repeating interval specified by for.
See the for macro section for details.
%href
for


%index
switch
Comparison block start
%group
Program control macro
%prm
p1
p1: Comparison source
%inst
switch ~ case ~ swend is a syntax that allows you to write multiple conditional judgments and processes together in a block.
The comparison source parameter specified by p1 can be a variable or an expression. After the switch,
^
case comparison value
^
By putting, you can describe the content to be processed when the "comparison source" is the same as the "comparison value" after this.
Multiple cases can be described in the block, and the processing for each comparison value can be specified. If the case comparison is correct, execute subsequent instructions until swbreak exists. Also, instead of case
^
default
^
If is set, the following will be executed if the condition does not apply to any of the comparison values specified in case.
^
If the switch block is finished
^
swend
^
Must be written at the end.
The following is an example script using the switch macro.
^p
	a=0
switch a; compare a
case 0; if a is 0
		mes "A=0"
swbreak; Condition execution end of case0
case 1; if a is 1
		mes "A=1"
default; if a is non-zero
		mes "A!=0"
		swbreak
	swend
^p
In this example, conditional branching is performed depending on whether the content of variable a is 0, 1 or something else.
After "case 0", up to "swbreak" is executed, but in the case of "case 1", since "swbreak" does not exist, "mes" A! = 0 "" executed after "default" Please note that it will be executed including.
%href
case
default
swbreak
swend


%index
swend
Comparison block end
%group
Program control macro
%inst
Specifies the end of the comparison block specified by the switch.
See the switch macro section for more details.
%href
switch
case
default
swbreak


%index
default
Default comparison specification
%group
Program control macro
%inst
In the comparison block of switch to swend, specify the process to be executed when the conditions do not apply to the previous cases.
See the switch macro section for more details.
%href
switch
case
swbreak
swend


%index
case
Comparison value specification
%group
Program control macro
%prm
p1
p1: Comparison value
%inst
In the comparison block of switch to swend, specify the condition with the comparison source and specify the process to be executed when it is correct.
See the switch macro section for more details.
%href
switch
default
swbreak
swend


%index
swbreak
Comparison execution escape designation
%group
Program control macro
%inst
Escape from the comparison block of switch ~ swend.
See the switch macro section for more details.
%href
switch
default
case
swend


%index
_continue
Redo the macro loop
%group
Program control macro
%inst
Repeating macros defined by standard macros,
Redo the loop between while ~ wed, do ~ until, and for ~ next.
It does not execute in the repeating block after the present, and restarts from the next repeating.
%href
_break
while
wend
do
until
for
next


%index
_break
Escape the macro loop
%group
Program control macro
%inst
Repeating macros defined by standard macros,
Break out of the loop between while ~ wend, do ~ until, and for ~ next.
It does not execute in the repeating block after the present, and resumes from the instruction next to the loop.
%href
_continue
while
wend
do
until
for
next




%index
foreach
Repeat for the number of elements of the variable
%group
Program control instructions
%prm
p1
p1 = variable name: variable to be looped

%inst
Iterate as many times as there are elements stored in the array variable.
Foreach is described at the loop start location like the repeat instruction, and the loop instruction is described at the loop end location.
The difference from the repeat instruction is that the number of elements of the specified array variable is the number of repetitions, and in the case of modular variables, the elements deleted by the delmod instruction do not execute the loop contents.
^p
example :
	dim a,10
	foreach a
	mes "#"+cnt+":"+a.cnt
	loop
^p
By specifying the system variable cnt as an array element in a loop, you can concisely write a process that causes all elements to be executed repeatedly.
Also, the break and continue instructions function in the same way as when using the repeat instruction.

%href
repeat
loop
break
continue



%index
oncmd
Windows message interrupt execution specification
%group
Program control instructions
%prm
goto/gosub *label,p1
* label: Label name
p1: Message ID

%inst
Specifies where to jump automatically when the specified window message is sent.
^
After oncmd, specify the goto or gosub keyword before writing the label.
In the case of goto, a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto / gosub keyword is omitted, the operation is the same as the goto keyword.
^
If you specify a label with the oncmd instruction, it will jump to the label specified by * label when the p1 message is sent to the HSP window after that.
The oncmd instruction applies only to the current operation window.
If multiple windows are created, each window can be registered using the oncmd command.
^
If a window message that has already been registered is specified, the previous registration will be invalidated.
Also, interrupt registration is out of sync with window initialization.
Once registered, interrupts are valid until the application is closed.
^
The interrupt jump accepts an interrupt and jumps when it is stopped by the stop instruction and the wait or await instruction.
In addition, the system variables iparam, wparam, and lparam are set after jumping by interrupt.
The window ID where the interrupt occurred can be obtained by using the ginfo function.
^p
   Interrupt factor | iparam | wparam | lparam
 ---------------------------------------------------------------------
   oncmd | Message ID | wParam | lParam
^p
The sent message ID is assigned to the system variable iparam.
In addition, wparam and lparam store the parameters passed as Windows messages as they are.
If a subroutine jump occurs due to the gosub keyword in the interrupt settings, the return value can be set with the return instruction.
For "return number", specify the value returned by the window procedure.
If no parameter is specified in the return instruction, processing is passed to the default window procedure.
^
It is also possible to temporarily turn ON / OFF the execution of event interrupts.
^p
	oncmd 0
^p
Temporarily stops the interrupt with.
^p
	oncmd 1
^p
You can resume the interrupt that was paused with.


%href
onkey
onclick
onexit
onerror


%index
else
If the condition is not met, execute the instruction on that line
%group
Program control instructions
%inst
Used in combination with the if instruction.
For details, refer to the explanation of the if instruction.

%sample
if a == 0 {
mes "a is 0."
} else {
mes "a is not 0."
}

%href
if

