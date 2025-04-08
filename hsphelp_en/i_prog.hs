;
; HELP source file for HSP help manager
; (Lines starting with ";" are treated as comments)
;

%type
Built-in command
%ver
3.7
%note
ver3.7 standard command
%date
2024/06/04
%author
onitama
%url
https://hsp.tv/
%port
Win
Cli
Let

%index
await
Wait for a specific time
%group
Program control command
%prm
p1
p1=0〜(0) : Wait time (in ms)

%inst
Interrupts program execution for a specific duration.
^
Similar to the wait command, but the await command specifies the waiting time from the last wait time. This prevents time from passing too quickly due to differences in drawing speed, etc.  It is used to keep the speed of real-time updated screens constant.
^
The p1 parameter is in milliseconds (ms). Specifying 1000 will specify a wait time of 1 second.
^
The await command allows for more precise and finer waits than the wait command, but consumes more CPU tasks than the wait command.
If it is not a process such as rewriting images in real time, it is better to use the wait command to reduce the burden on the CPU (Windows).
It is recommended to use the wait command for accessories that are always placed on the desktop, and the await command for game applications.
^
In HSP, it is recommended to put wait or await commands in places where long time loops may occur to give other Windows tasks processing time. If you execute a program that loops for a long time within a command without wait or await, it will be difficult to drag the window or it will take time to switch to other tasks.
^
The p1 parameter is in milliseconds (ms). Specifying 1000 will specify a wait time of 1 second.
The wait time specified as a parameter does not guarantee strict execution in 1ms units.  Please note that errors may occur if CPU tasks or interrupt processing occurs.
In particular, when proceeding with precise time like a stopwatch or clock, be sure to acquire absolute time with the gettime command and process it.

%href
wait

%index
exec
Execute a Windows file
%group
Program control command
%prm
"filename",p1,"command"
"filename" : Target filename
p1=0〜(0)  : File execution mode
"command"  : Context menu operation name

%inst
exec executes the application with the filename specified by "filename".
If the extension is EXE, the extension can be omitted.
You can select the execution mode with p1. If p1 is omitted, it will be 0.
^p
 Mode 0 : HSP normal execution
 Mode 2 : Execute in minimized mode
 Mode 16 : Execute the associated application
 Mode 32 : Print the specified file
^p
If "command" is specified, it will be the context menu operation name for the file or folder name specified by "filename".
The context menu operation name is a string that specifies the action registered in the registry, and the following are prepared as standard.
^p
  Operation name  |  Description
 --------------------------------------------------
  edit       | Open the editor to edit.
  explore    | Select a folder and start Explorer.
  open       | Open the file with the associated application.
  print      | Print the file with the associated application.
  properties | Displays file or folder properties.
^p
If "command" is specified, file execution modes 16 and 32 are disabled. In addition, a value indicating the execution result is returned to the system variable stat.
^
Normal execution activates the newly launched application window and moves the input focus to it, but in multi-tasking, HSP continues to execute without terminating.
^p
Example:
	exec "calc" ; Start Windows calculator
^p
You can also write parameters after the file name.
^p
Example:
	exec "notepad hsp.txt" ; Open hsp.txt with notepad
^p
Mode 2 executes the specified application in a minimized state.
You can execute DOS applications in the background without blocking the HSP screen.
^
Mode 16 specifies a data file instead of an executable file, and executes the application associated with that extension.
^p
Example:
	exec "hsp.txt",16 ; Open hsp.txt with the associated application
^p
In the above example, "hsp.txt" is opened with the associated application.
(Usually notepad.exe)
Also, in mode 16, you can specify a URL on the Internet to use it as an Internet shortcut.
^p
Example:
	;Open internet page
	exec "http://www.onionsoft.net/hsp/",16
^p
In mode 32, the application associated with the specified file is used to print to the printer.
For example,
^p
Example:
	exec "hsp.txt",32 ; Print hsp.txt with printer
^p
will automatically print "hsp.txt" with the application associated with "txt". All printing options are those specified in the associated application.
^
Multiple modes can be specified at the same time.
For example, if you specify mode 2 and mode 16 at the same time, the value will be 18 by adding each value. In this case, "Execute the associated application in minimized mode".
^
* Mode 1 (task execution wait), which was supported until HSP2.61, is not supported in HSP3. Please monitor the launched process or use the pipeexec command for console applications.

%href
pipeexec
%portinfo
When HSPLet, it is used for the purpose of displaying an HTML page.
Since the meaning of the third argument is different from the original HSP, it is not recommended to use this command directly.
Instead, it is recommended to use the showPage fileName, target command defined in letutil.hsp.

%index
end
Program termination
%group
Program control command
%prm
p1
p1(0) : Exit code
%inst
Terminates the HSP program.
The end command can be anywhere in the script.
When the end command is executed, the application terminates.
^
The application exit code can be specified by the parameter p1. If p1 is omitted, 0 will be the application exit code.
The application exit code is used to notify other processes or batch files. Normally, there is no problem if it is omitted (0).

%href
stop

%index
gosub
Subroutine jump to specified label
%group
Program control command
%prm
*label
*label : Label name

%inst
Performs a subroutine jump.
The gosub command jumps to the location specified by *label.
After that, if there is a return command, it will return to the next line of the gosub command and continue execution.
gosub to return can be nested.
(The maximum number of nesting varies depending on the amount of stack memory HSP has)

%href
goto
return

%index
goto
Jump to specified label
%group
Program control command
%prm
*label
*label : Label name

%inst
Jumps unconditionally to the specified label.
From then on, execution will continue from the location specified by the label name.

%href
gosub

%index
if
Execute the instruction on that line if the condition is met
%group
Program control command
%prm
p1
p1 : Conditional expression

%inst
If the conditional expression of p1 is met, the subsequent instructions are executed. If it is not satisfied, the next line is moved to.
Note that unlike BASIC, you must put :(colon) between the next instruction on that line.
^p
Example:
	a=10
	if a=10 : mes "a is 10."
^p
In the above example, "a=10" is the conditional expression. The conditional expression mainly includes:
^p
	a=b  : a and b are equal
	a!b  : a and b are not equal
	a<b  : a is less than b
	a>b  : a is greater than b
	a<=b : a is less than or equal to b
	a>=b : a is greater than or equal to b
^p
is used. After if + conditional expression, separate with: (colon), followed by the part to be executed if the condition is met.
(Just like in C and Java, you can write "= ""!" as "==" "!=") If you want to change the program flow depending on the conditions,
^p
Example:
	a=10
	if a>10 : goto *over10
	mes "a is less than or equal to 10."
	stop
*over10
	mes "a is greater than 10."
	stop
^p
As above, you can jump to another label using the goto command.
You can also use the else command to write the process when the condition is not met on the same line.
^p
Example:
	a=10
	mes "a is,"
	if a=10 : mes "10." : else : mes "It is not 10."
	stop
^p
In this case, the part up to the else command is executed when the condition is met, and the else command and subsequent instructions are executed when the condition is not met.
The following lines will be executed as usual regardless of the condition.
^
To make more complex conditional judgments, you can also use logical expressions.
^p
   a&b  : a and b are both true (and)
   a|b  : Either a or b is true (or)
^p
This allows you to describe multiple conditions at once.
^p
Example:
	a=10:b=20
	if (a=10)|(b=10) : mes "Either a or b is 10."
^p
In the above example, the conditional expressions "a=10" and "b=10" are connected with |(or), and if either one is true, the result will be true.
Logical operations can be described not only with symbols such as "|""&" but also with strings such as "or""and".
^p
Example:
	if (a=10)or(b=10) : mes "Either a or b is 10."
^p
The result will be the same even if you write as above.
^
If you want to describe the script to be executed when the condition of the if command is true over multiple lines,
^p
Example:
	a=10
	if a>5 {
		mes "TRUE"
		mes "(MULTILINE IF)"
	}
^p
Specify the part starting with "{" and ending with "}".
(In the above example, TAB is inserted at the beginning of the line in a C language style to make it easier to see, but it is not particularly necessary. Please write it in a way that is easy for the user to see.)
%href
else

You can also specify multiple lines with the else command.
^p
Example:
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
However, be careful because multiple lines are not possible unless you write "{" after else.

%index
loop
Return to the beginning of the loop
%group
Program Control Commands
%inst
Specifies the end of the loop specified by the repeat/foreach command.
See the repeat command section for details.

%href
repeat
foreach

%index
onexit
Jump on exit
%group
Program Control Commands
%prm
goto/gosub *label
*label : Label name

%inst
Specifies the location to automatically jump to when the close box (the close button at the top right of the window) is pressed.
^
After onexit, specify the goto or gosub keyword followed by a label.
goto performs a simple program jump. gosub performs a subroutine jump.
If the goto/gosub keyword is omitted, it will behave the same as the goto keyword.
^p
Example :
	onexit gosub *exit_routine  ; Call on exit
^p
Normally, pressing the close box would force a termination at any time, but after this command is executed, pressing the close box will jump to the label specified by the onexit command and continue execution from there.
^
This can be used when you want to display a confirmation message when exiting, or when you want to save data after exiting.
^
Please be careful because once this command is executed, the script cannot be stopped except by the end command.
^
If multiple windows are created, an interrupt will occur with the close button in any of the windows. At this time, the window ID that received the termination notification is assigned to the system variable wparam.
^
Immediately after jumping with onexit, the cause of termination is saved as a value in the system variable iparam.
If iparam is 0, the program is terminated at the user's will.
If iparam is 1, the termination is due to Windows shutdown (restart or power OFF). For termination processing during Windows shutdown, if you generate idle time (waiting time) for the system with await, wait, stop commands, the shutdown process will be aborted (it will not be shut down).
If you exit with the end command without using the await, wait, stop commands, the shutdown process will continue as is.
^
It is also possible to temporarily turn ON/OFF the exit jump.
^p
	onexit 0
^p
Temporarily stops the interrupt.
^p
	onexit 1
^p
You can resume the interrupt that was temporarily stopped.

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
Program Control Commands
%prm
p1
p1 : System variable assignment value
%inst
Ends a subroutine called by the gosub command or a user-defined command. The return command returns the stack to the calling source.
By specifying p1, you can reflect the value from the subroutine to the system variable.
If you specify a number for p1, it will be assigned to the system variable stat.
If you specify a string for p1, it will be assigned to the system variable refstr.
If you specify a real number for p1, it will be assigned to the system variable refdval.
If the specification of p1 is omitted, the system variable will not be assigned.

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
Program Control Commands
%prm
"filename","cmdline"
"filename" : HSP object file name to execute
"cmdline"  : String assigned to the system variable dir_cmdline
%inst
Executes an HSP program written in another file.
The file you specify must be an HSP object file (with the extension AX).
Object files can be created with the "Create Object File" option in the script editor.
^p
Example:
    run "MENU2.AX","-s"
^p
When this is executed, the previous program disappears and the program in the file "MENU2.AX" is executed from the beginning.
The screen state, variables, and memory buffer contents are all initialized. Also, the execution starts with the string "-s" assigned to the system variable dir_cmdline.
^
If the "cmdline" parameter is omitted, the contents of the system variable dir_cmdline will be
empty. Also, the system variable dir_cmdline can store up to 1024 characters.
^
The run command only works with object files targeting the same runtime. Also, operation with the HSP3Dish runtime is currently not supported.
%href

%index
stop
Interrupt program
%group
Program Control Commands
%inst
Temporarily suspends program execution.
Used when waiting for a button to be clicked, etc.

%href
end

%index
wait
Interrupt execution for a certain time
%group
Program Control Commands
%prm
p1
p1(100) : Waiting time (in 10ms units)

%inst
Interrupts program execution for a certain period of time.
^
Specify the length of the waiting time in p1. The unit is 10 milliseconds. (1 millisecond = 1/1000 seconds)
The waiting time is the same on any model regardless of CPU speed.
;If p1 is specified as 0, or if it is omitted, it waits for the same amount of time as the previous time.
^
The await command allows for more accurate and finer-grained waits than the wait command, but it consumes more CPU tasks than the wait command.
Unless you are doing real-time image rewriting or other processing, it is better to use the wait command to reduce the load on the CPU (Windows).
It is recommended to use the wait command for accessories that are always placed on the desktop, and the await command for game applications, etc.
^
In HSP, it is recommended to put a wait or await command in places where a long time loop may occur to give processing time to other Windows tasks.
If you execute a program that loops for a long time in a command that does not have a wait or await command, it will be difficult to drag the window, or it will take time to move to other tasks.
^
Please note that the waiting time specified as a parameter does not guarantee strict execution. Errors may occur when CPU tasks or interrupt processing occur.
In particular, if you want to proceed with precise time such as a stopwatch or clock, you should acquire the absolute time with the gettime command and process it.

%href
await

%index
repeat
Indicates the beginning of the loop
%group
Program Control Commands
%prm
p1,p2
p1=1〜(-1) : Number of loops
p2=0〜(0)  : Start value of system variable cnt

%inst
Repeatedly executes between repeat and loop.
The repeat command indicates the start point of the repetition.
The loop command returns to the last passed repeat command. In other words,
^p
	repeat 5
		print "Hello"
	loop
^p
A program like this displays "Hello" 5 times.
You can specify the number of repetitions with the parameter p1 of the repeat command. If the number of times is omitted or a negative value is specified, it becomes an infinite loop.
If the number of repetitions is 0, the repetition part is not executed and the program jumps to the corresponding loop command.
^
You can also describe a nested structure in which the part including repeat〜loop is further looped. However, if you exit without going through the loop command normally, the nested structure will be corrupted, so avoid creating a structure that forces you to exit the loop.
^
You can refer to the system variable cnt to check the number of loops or use a counter. cnt usually starts from 0 and increases by 1 each time it loops with the loop command.
However, you can also change the number from which the counter starts with p2 of the repeat command. For example, if you specify repeat 3,1, the value of the variable cnt will change in the order of 1, 2, and 3.
^
Use the break command to force the loop to exit inside the loop. The continue command is also available to move to the next loop.

%href
loop
break
continue
foreach

%index
break
Exit the loop
%group
Program Control Commands
%inst
Forces the exit from the loop between repeat〜loop.
^p
	repeat 5
		if cnt=2 : break
		mes "Repetition["+cnt+"]"
	loop
^p
In the above example, the break command is executed according to the judgment of the if command when the system variable becomes 2, that is, when it becomes the third loop.
When the break command is executed, even if the number of repetitions is still left, the program forcibly exits from the repetition and continues execution from the command next to the loop command.
The break command and subsequent commands (mes command in the above example) are not executed.
Using this command, you can create a script like the following.
^p
	repeat
		getkey a,1
		if a>0 : break
		await 10
	loop
^p
The above script is a loop that waits until the left mouse button is pressed.
If you omit the number of times specified for the repeat command, it becomes an infinite loop, so it repeats the same place until the button state becomes 1. When the button is pressed, the break command is executed and the program exits the loop.

%href
repeat
loop
continue
foreach

%index
continue
Redo the loop
%group
Program Control Commands
%prm
p1
p1 = 0〜 : Repeat counter change value

%inst
Redo the loop between repeat〜loop.
When the continue command is executed, the program returns to the repeat command and executes the next iteration.
^p
	repeat 5
		if cnt=2 : continue
		mes "cnt="+cnt
	loop
^p
In the above example, the continue command is executed when the system variable cnt becomes 2.
When the above script is executed,
^p
	cnt=0
	cnt=1
	cnt=3
	cnt=4
^p
You can see that the mes command is not executed only when the system variable cnt is 2.
It may be difficult to understand at first, but the continue command can be said to have the same function as the loop command, but not at the location of the loop command.
The repetition counter increases by 1 as with the loop command, even if the continue command is executed.
If the continue command is executed in the last repetition, the repeat〜loop ends, that is, the program continues execution from the command next to the loop command.
%href
repeat
foreach
loop
break


%index
onkey
Specify key interrupt execution
%group
Program Control Instructions
%prm
goto/gosub *label
*label : Label name

%inst
Specifies the location to automatically jump to when a key is pressed.
^
After onkey, specify the goto or gosub keyword, followed by a label. goto performs a simple program jump. gosub performs a subroutine jump.
^
If the goto/gosub keyword is omitted, it behaves the same as the goto keyword.
^
When a label is specified with the onkey command, the program jumps to the label specified by *label every time there is a key input while the HSP window is active from that point on.
^
The interrupt jump accepts and jumps to the interrupt when it is stopped by the stop, wait, and await commands.
After the jump by interrupt, the system variables iparam, wparam, and lparam are set.
^p
   Interrupt factor | iparam     | wparam | lparam
 ---------------------------------------------------------
    onkey       | Character code | wParam | lParam
^p
The system variable iparam is assigned a parameter for each interrupt factor.
Also, wparam and lparam store the parameters passed as Windows messages as they are.
It is also possible to temporarily turn ON/OFF the event interrupt execution.
^p
	onkey 0
^p
Temporarily stops key interrupts.
^p
	onkey 1
^p
You can resume key interrupts that have been temporarily stopped.


%href
onclick
onexit
onerror


%index
onclick
Specify click interrupt execution
%group
Program Control Instructions
%prm
goto/gosub *label
*label : Label name

%inst
Specifies the location to automatically jump to when a mouse button is pressed.
^
After onclick, specify the goto or gosub keyword, followed by a label. goto performs a simple program jump. gosub performs a subroutine jump.
^
If the goto/gosub keyword is omitted, it behaves the same as the goto keyword.
^
When a label is specified with the onclick command, the program jumps to the label specified by *label every time there is a mouse click on the HSP window from that point on.
^
The interrupt jump accepts and jumps to the interrupt when it is stopped by the stop, wait, and await commands.
After the jump by interrupt, the system variables iparam, wparam, and lparam are set.
^p
   Interrupt factor | iparam         | wparam | lparam
 -------------------------------------------------------
   onclick      | Mouse button ID | wParam | lParam
^p
The system variable iparam is assigned a parameter for each interrupt factor.
Also, wparam and lparam store the parameters passed as Windows messages as they are.
It is also possible to temporarily turn ON/OFF the event interrupt execution.
^p
	onclick 0
^p
Temporarily stops the interrupt.
^p
	onclick 1
^p
You can resume the interrupt that has been temporarily stopped.


%href
onkey
onexit
onerror


%index
onerror
Jump when an error occurs
%group
Program Control Instructions
%prm
goto/gosub *label
*label : Label name
%inst
Specifies the location to automatically jump to when an error occurs internally in HSP due to a script.
^
After onerror, specify the goto or gosub keyword, followed by a label. goto performs a simple program jump. gosub performs a subroutine jump.
^
If the goto/gosub keyword is omitted, it behaves the same as the goto keyword.
^
Normally, an error message dialog from the system is displayed when an error occurs, but instead, the program is set to jump to the specified label. After the jump, the following system variables are assigned information.
^p
	wparam : Error number
	lparam : Error occurrence line number
	iparam : 0 (none)
^p
Even if the processing after an error is specified by the onerror command, please end the application with the end command as much as possible without resuming the execution of the application after the necessary processing is completed.
The onerror command does not recover from errors.
Depending on the cause of the error, the HSP system itself may become unstable or a failure may occur. The onerror command can be used to display a unique error message on the application side when an error occurs during executable file creation, or to display debugging information only when a specific error occurs.
^
It is also possible to temporarily turn ON/OFF the termination jump.
^p
	onerror 0
^p
Temporarily stops the interrupt.
^p
	onerror 1
^p
You can resume the interrupt that has been temporarily stopped.


%href
onkey
onclick
onexit


%index
exgoto
Conditional jump to the specified label
%group
Program Control Instructions
%prm
var,p1,p2,*label
var    : Variable used for comparison
p1     : Comparison flag
p2     : Comparison value
*label : Label name
%inst
Conditionally jumps to the specified label based on the value assigned to the variable specified by var and the comparison value of p2.
The comparison method is as follows: if the comparison flag of p1 is 0 or more (positive value), jump when (var's value >= p2) is true, and if p1 is -1 or less (negative value), jump when (var's value <= p2) is true.
The variable specified by var must be an integer type. If it is initialized with a different type, an error will occur at runtime.
This command is prepared to speed up some macro processing such as for.
You can use the exgoto command alone, but we recommend performing normal conditional branching with the if command from the viewpoint of script visibility.
%href
goto
if


%index
on
Branching by number
%group
Program Control Instructions
%prm
p1 goto/gosub Label0,Label1…
p1=0〜(0) : Value for branching
Label0〜 : Label name of branch destination
%inst
The on command selects a branch destination according to the value specified by p1 as 0, 1, 2...
You can write in the format "on number goto" or "on number gosub", followed by one or more labels to branch to, separated by commas.
If the specified number is 0, label 0 is the branch target, if the number is 1, label 1 is the branch target, and so on. You can describe any number of labels corresponding to 2, 3, 4... onwards.
"on number goto" is a simple branch like the goto command, and "on number gosub" is a subroutine jump like the gosub command.
If the number is a negative value or a corresponding label is not specified, the branch will not be performed and the program will proceed to the next line.
Please note that unlike N88-BASIC, the first label corresponds to the number 0. (In N88-BASIC, the number 1 corresponds to the first label.)
%href
goto
gosub
%sample
	a=1
	on a goto *a0,*a1,*a2
	mes "Other":stop
*a0
	mes "A=0":stop
*a1
	mes "A=1":stop
*a2
	mes "A=2":stop


%index
while
Start while loop
%group
Program Control Macro
%prm
p1
p1=Conditional expression(1) : Condition to repeat
%inst
Repeats from while to wend only while the condition following while is met.
If the condition is not met, while to wend is not executed.
If the condition following while is omitted, the loop is performed infinitely.
^p
	a=0
	while a<5
	a=a+1:mes "A="+a
	wend     ; Repeats while below while only while a is less than or equal to 5
^p
Also, resumption and escape are possible with the _continue and _break macros.
The while〜wend control is realized using the preprocessor's macro function.
Please use this if you want to describe something similar to C or Java.
For beginners, we recommend the repeat〜loop command or the repetition description using the if command.
%href
_continue
_break
wend


%index
wend
End of while loop
%group
Program Control Macro
%inst
Specifies the end of the repetition section specified by while.
See the while macro section for details.
%href
while


%index
until
End of do loop
%group
Program Control Macro
%prm
p1
p1=Conditional expression(1) : Condition to repeat
%inst
Repeats the part from do to until until the condition following until is met.
Even if the condition is met, the do〜until section is executed at least once.
If the condition following until is omitted, the loop is not performed.
^p
	a=0
	do
	a=a+1:mes "A="+a
	until a>5    ; Repeats while below do until a is greater than 5
^p
Also, resumption and escape are possible with the _continue and _break macros.
The do〜until control is realized using the preprocessor's macro function.
Please use this if you want to describe something similar to C or Java.
For beginners, we recommend the repeat〜loop command or the repetition description combining the goto command and the if command.
%href
_continue
_break
do


%index
do
Start do loop
%group
Program Control Macro
%inst
Specifies the beginning of the repetition section to until.
The repetition condition is described by the until macro.
See the until macro section for details.
%href
until


%index
for
Start loop for specified number of times
%group
Program Control Macro
%prm
p1,p2,p3,p4
p1    : Variable name
p2(0) : Initial value
p3(0) : Final value
p4(1) : Increment
%inst
Specifying a variable name, initial value, final value, increment, etc. as parameters repeats between for and next the specified number of times.
Values in parentheses are values when omitted. The variable name cannot be omitted.
The specified variable is used as a counter, starting from the initial value, and adding the increment each time it is repeated. When the final value is reached, the loop is exited (the final value is not included in the loop).
If the final value condition is met from the beginning, the loop is not executed.
Also, resumption and escape are possible with the _continue and _break macros.
%index
next
Specifies the end of a repetition interval defined by for.
%group
Program Control Macro
%inst
Specifies the end of a repetition interval defined by the for macro.
See the for macro section for details.
%href
for


%index
switch
Starts a comparison block
%group
Program Control Macro
%prm
p1
p1 : Comparison source
%inst
switch~case~swend is a construct that allows you to write multiple conditional judgments and processes together in a block.
The comparison source parameter specified by p1 can be a variable or an expression. After switch,
^
case comparison value
^
can be placed to describe the content to be processed when the "comparison source" is the same as the "comparison value".
Multiple cases can be written in a block to specify the process for each comparison value. If the case comparison is correct, the subsequent instructions are executed until a swbreak is present. Also, instead of case,
^
default
^
can be placed, and the subsequent instructions will be executed if none of the comparison values specified by case apply.
^
When the switch block is finished, you must always write
^
swend
^
at the end.
The following is an example of a script using the switch macro.
^p
	a=0
	switch a               ; a is the comparison target
	case 0                 ; if a is 0
		mes "A=0"
		swbreak            ; end case0 condition execution
	case 1                 ; if a is 1
		mes "A=1"
	default                ; if a is other than 0
		mes "A!=0"
		swbreak
	swend
^p
In this example, the conditional branching is performed based on whether the contents of the variable a are 0, 1, or other.
After "case 0", everything up to "swbreak" is executed, but in the case of "case 1", "swbreak" does not exist, so "mes "A!=0"", which is executed after "default", is also executed, so be careful.
%href
case
default
swbreak
swend


%index
swend
Ends a comparison block
%group
Program Control Macro
%inst
Specifies the end of a comparison block defined by switch.
See the switch macro section for details.
%href
switch
case
default
swbreak


%index
default
Specifies a default comparison
%group
Program Control Macro
%inst
Specifies the process to be executed when the condition does not match any of the previous cases in the switch~swend comparison block.
See the switch macro section for details.
%href
switch
case
swbreak
swend


%index
case
Specifies a comparison value
%group
Program Control Macro
%prm
p1
p1 : Comparison value
%inst
Specifies the process to be executed when the condition with the comparison source is specified and is correct in the switch~swend comparison block.
See the switch macro section for details.
%href
switch
default
swbreak
swend


%index
swbreak
Specifies an exit from comparison execution
%group
Program Control Macro
%inst
Exits from the switch~swend comparison block.
See the switch macro section for details.
%href
switch
default
case
swend


%index
_continue
Restarts a macro loop
%group
Program Control Macro
%inst
Restarts the loop between the repetition macros defined by the standard macro,
while~wend, do~until, for~next.
The current and subsequent blocks within the loop are not executed, and the loop resumes from the next iteration.
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
Exits a macro loop
%group
Program Control Macro
%inst
Exits the loop between the repetition macros defined by the standard macro,
while~wend, do~until, for~next.
The current and subsequent blocks within the loop are not executed, and the loop resumes from the instruction after the loop.
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
Repeats for the number of elements in a variable
%group
Program Control Command
%prm
p1
p1=variable name : Variable to be looped over

%inst
Repeats as many times as there are elements stored in the array variable.
foreach is written at the start of the loop, like the repeat command, and the loop command is written at the end of the loop.
The difference from the repeat command is that the number of elements in the specified array variable is the number of repetitions, and in the case of module-type variables, the loop contents are not executed for elements deleted by the delmod command.
^p
Example :
	dim a,10
	foreach a
	mes "#"+cnt+":"+a.cnt
	loop
^p
By specifying the system variable cnt as an array element in the loop, it is possible to write a process that repeats all elements in a concise manner.
Also, the break and continue commands function in the same way as when using the repeat command.

%href
repeat
loop
break
continue



%index
oncmd
Specifies Windows message interrupt execution
%group
Program Control Command
%prm
goto/gosub *label,p1
*label : Label name
p1     : Message ID

%inst
Specifies the location to automatically jump to when the specified window message is sent.
^
After oncmd, continue with the goto or gosub keyword and then write the label.
In the case of goto, it is a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto/gosub keyword is omitted, it will operate the same as the goto keyword.
^
When a label is specified with the oncmd command, the program jumps to the label specified by *label when the message of p1 is sent to the HSP window thereafter.
The oncmd command only applies to the currently active window.
If you are creating multiple windows, you can register using the oncmd command for each window.
^
If a window message that has already been registered is specified, the previous registration will be invalidated.
Also, interrupt registration is not synchronized with window initialization.
Once an interrupt is registered, it will be valid until the application is terminated.
^
The interrupt jump accepts the interrupt and jumps when the program is stopped by the stop command or the wait and await commands.
Also, after performing a jump due to an interrupt, the system variables iparam, wparam, and lparam are set.
The window ID where the interrupt occurred can be obtained using the ginfo function.
^p
   Interrupt factor | iparam       | wparam | lparam
 ---------------------------------------------------------------------
   oncmd        | Message ID | wParam | lParam
^p
The transmitted message ID is substituted into the system variable iparam.
Also, wparam and lparam contain the parameters passed as Windows messages as they are.
If a subroutine jump is generated by the gosub keyword in the interrupt setting, it is possible to set a return value using the return command.
With "return numerical value", the value returned by the window procedure is specified.
If no parameters are specified for the return command, the process is passed to the default window procedure.
^
It is also possible to temporarily turn ON/OFF event interrupt execution.
^p
	oncmd 0, Message ID
^p
temporarily stops the interrupt for the specified message ID.
^p
	oncmd 1, Message ID
^p
restarts the temporarily stopped interrupt.
It is also possible to turn ON/OFF event interrupt execution collectively.
^p
	oncmd 0
^p
stops all Windows message interrupts.
^p
	oncmd 1
^p
restarts all Windows message interrupts.


%href
onkey
onclick
onexit
onerror


%index
else
Executes the command on that line if the condition is not met
%group
Program Control Command
%inst
Used in combination with the if command.
See the explanation of the if command for details.

%sample
if a == 0 {
	mes "a is 0."
} else {
	mes "a is not 0."
}

%href
if
