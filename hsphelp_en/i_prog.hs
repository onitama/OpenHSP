;
; HSP help manager用 HELPソースファイル
; (先頭が「;」の行はコメントとして処理されます)
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
Wait for a specified time
%group
Program control commands
%prm
p1
p1=0〜(0) : Wait time (in milliseconds)

%inst
Pauses the execution of the program for a specified time.
^
Similar to the wait command, the await command specifies the waiting time from the last wait. This prevents the time from passing too quickly due to differences in drawing speed, etc. It is used to keep the speed of real-time updated screens constant.
^
The p1 parameter is in milliseconds (ms). If you specify 1000, the wait time will be 1 second.
^
The await command allows for more precise and finer waits than the wait command, but it consumes more CPU tasks than the wait command.
If you are not processing images in real-time, using the wait command will put less strain on the CPU (Windows).
It is best to use the wait command for accessories that are always placed on the desktop, and the await command for game applications, etc.
^
In HSP, it is recommended to insert wait or await commands where long time loops may occur in order to give processing time to other Windows tasks. If you run a program that loops for a long time without wait or await commands, it will be difficult to drag windows or it will take a long time to switch to other tasks.
^
The p1 parameter is in milliseconds (ms). If you specify 1000, the wait time will be 1 second.
The waiting time specified as a parameter does not guarantee strict execution in 1ms units. Please note that errors may occur if CPU tasks or interrupt processing occur.
In particular, if you are progressing with precise time like a stopwatch or clock, be sure to acquire absolute time using the gettime command or the like.

%href
wait

%index
exec
Execute a Windows file
%group
Program control commands
%prm
"filename",p1,"command"
"filename" : Target filename
p1=0〜(0)  : File execution mode
"command"  : Context menu operation name

%inst
exec executes the application with the filename specified by "filename".
If the extension is EXE, the extension can be omitted.
You can choose the execution mode with p1. If p1 is omitted, it defaults to 0.
^p
 Mode 0  : HSP normal execution
 Mode 2  : Execute minimized
 Mode 16 : Execute associated application
 Mode 32 : Print the specified file
^p
If "command" is specified, it will be the context menu operation name for the file or folder name specified by "filename".
The context menu operation name is a string that specifies the action registered in the registry, and the following are provided as standard.
^p
 Operation name | Explanation
---------------------------------------------------------------------
 edit       | Open the editor and edit.
 explore    | Select a folder and launch Explorer.
 open       | Open the file with the associated application.
 print      | Print the file with the associated application.
 properties | Show file or folder properties.
^p
If "command" is specified, file execution modes 16 and 32 are disabled. The value indicating the execution result is returned to the system variable stat.
^
Normal execution activates the newly launched application window and moves the input focus to it, but HSP continues to run without exiting in a multitasking environment.
^p
Example :
 exec "calc"  ; Launch Windows calculator
^p
You can also write parameters after the filename.
^p
Example :
 exec "notepad hsp.txt"  ; Open hsp.txt with Notepad
^p
Mode 2 executes the specified application in a minimized state.
You can run DOS applications in the background without obscuring the HSP screen.
^
Mode 16 specifies a data file instead of an executable file, and executes the application associated with that extension.
^p
Example :
 exec "hsp.txt",16  ; Open hsp.txt with the associated application
^p
In the example above, "hsp.txt" is opened with the associated application.
(Normally, notepad.exe)
In Mode 16, you can also use the Internet URL to use it as an Internet shortcut.
^p
Example :
 ;Open an internet page
 exec "http://www.onionsoft.net/hsp/",16
^p
In mode 32, printing to a printer is done via an application associated with the specified file.
For example,
^p
Example :
 exec "hsp.txt",32  ; Print hsp.txt with the printer
^p
automatically prints "hsp.txt" with the application associated with "txt". All printing options are those specified by the associated application.
^
Multiple modes can be specified simultaneously.
For example, if you specify mode 2 and mode 16 at the same time, the value will be 18 by adding the values of each. In this case, it will be "Execute the associated application in minimized mode".
^
* Mode 1 (task execution wait), which was supported until HSP2.61, is not supported in HSP3. Use the pipeexec command to monitor the launched process or, in the case of a console application.

%href
pipeexec
%portinfo
In HSPLet, this command is used to display HTML pages.
Because the meaning of the third argument is different from the original HSP, it is not recommended to use this command directly.
Instead, it is recommended to use the showPage fileName, target command defined in letutil.hsp.

%index
end
End program
%group
Program control commands
%prm
p1
p1(0) : Exit code
%inst
Ends the HSP program.
The end command can be anywhere in the script.
When the end command is executed, the application exits.
^
You can specify the application exit code with the parameter p1. If p1 is omitted, 0 will be the application exit code.
The application exit code is used to notify other processes or batch files. Normally, it is omitted (0) and there is no problem.

%href
stop

%index
gosub
Subroutine jump to specified label
%group
Program control commands
%prm
*label
*label : Label name

%inst
Makes a subroutine jump.
The gosub command jumps to the location specified by *label.
After that, if there is a return command, it will return to the next line of the gosub command and continue execution.
gosub〜return can be nested.
(The maximum number of nesting depends on the amount of stack memory that HSP has)

%href
goto
return

%index
goto
Jump to specified label
%group
Program control commands
%prm
*label
*label : Label name

%inst
Jumps unconditionally to the specified label.
From then on, execution continues from the location specified by the label name.

%href
gosub

%index
if
Execute the command on that line if the condition is met
%group
Program control commands
%prm
p1
p1 : Conditional expression

%inst
If the condition expression of p1 is met, the subsequent command is executed. If it is not met, the process moves to the next line.
Unlike BASIC, be careful to insert a : (colon) between the if and the next command on that line.
^p
Example :
 a=10
 if a=10 : mes "a is 10."
^p
In the example above, "a=10" is the conditional expression. The conditional expression mainly includes:
^p
 a=b  : a is equal to b
 a!b  : a is not equal to b
 a<b  : a is less than b
 a>b  : a is greater than b
 a<=b : a is less than or equal to b
 a>=b : a is greater than or equal to b
^p
Use it. After if + conditional expression, separate with: (colon) and then write the part that will be executed if the condition is met.
(You can also write "= " "!" like "==" "!=" as in C and Java.) If you want to change the flow of the program depending on the conditions,
^p
Example :
 a=10
 if a>10 : goto *over10
 mes "a is 10 or less."
 stop
*over10
 mes "a is greater than 10."
 stop
^p
As above, you can jump to another label with the goto command.
You can also use the else command to write the process when the condition is not met on the same line.
^p
Example :
 a=10
 mes "a is, "
 if a=10 : mes "10." : else : mes "It is not 10."
 stop
^p
In this case, up to the part with the else command is executed if the condition is met, and the else command and subsequent commands are executed if the condition is not met.
From the next line onwards, the process is executed as usual regardless of the conditions.
^
To make more complex conditional judgments, you can also use logical expressions.
^p
 a&b  : a and b are both true (and)
 a|b  : either a or b is true (or)
^p
This allows you to write multiple conditions at once.
^p
Example :
 a=10:b=20
 if (a=10)|(b=10) : mes "Either a or b is 10."
^p
In the above example, the conditional expressions "a=10" and "b=10" are connected by |(or), and if either is true, a true result is output.
For logical operations, you can write the symbols "| " "&" as well as the strings "or " "and".
^p
Example :
 if (a=10)or(b=10) : mes "Either a or b is 10."
^p
The result will be the same if you write it as above.
^
If you want to describe the script that is executed when the if command condition is true over multiple lines,
^p
Example :
 a=10
 if a>5 {
  mes "TRUE"
  mes "(MULTILINE IF)"
 }
^p
Specify the part from the beginning with "{" and ending with "}".
(In the example above, TAB is inserted at the beginning of the line in C language style to make it easier to see, but it is not particularly necessary. Write it so that it is easy for the user to see.)
%href
else

The else command can also specify multiple lines.
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
Note that you must include "{" after else for it to apply to multiple lines.

%index
loop
Return to the beginning of the loop
%group
Program control commands
%inst
Specifies the end of the loop defined by the repeat/foreach commands.
See the repeat command section for details.

%href
repeat
foreach

%index
onexit
Jump on exit
%group
Program control commands
%prm
goto/gosub *label
*label : Label name

%inst
Specifies a location to automatically jump to when the close box (exit button in the upper right corner of the window) is pressed.
^
Following onexit, specify the goto or gosub keyword, then write a label.
goto performs a simple program jump. gosub performs a subroutine jump.
If the goto/gosub keyword is omitted, it behaves the same as the goto keyword.
^p
Example:
	onexit gosub *exit_routine  ; Exit time call
^p
Normally, pressing the close box will force a program termination in any circumstance. However, after this command is executed, pressing the close box will jump to the label specified in the onexit command, and execution will continue from there.
^
This is used when you want to display a confirmation message when exiting, or when you want to save data when exiting, etc.
^
Once this command is executed, the script cannot be stopped except by the end command, so be careful when using it.
^
When multiple windows are created, an interrupt will occur with the exit button in any of the windows. At this time, the system variable wparam is assigned the window ID of the window that received the termination notification.
^
Immediately after jumping with onexit, the exit factor is stored as a value in the system variable iparam.
If iparam is 0, the program is terminated by the user's intention.
If iparam is 1, the program is terminated due to Windows shutdown (reboot or power OFF). For termination processing during Windows shutdown, if you generate idle time in the system (waiting time) with await, wait, or stop commands, the shutdown processing will be stopped (not shut down).
If you end with the end command without using the await, wait, or stop commands, the shutdown processing will continue as is.
^
It is also possible to temporarily turn ON/OFF the exit time jump.
^p
	onexit 0
^p
Temporarily stops the interrupt.
^p
	onexit 1
^p
Allows to resume the interrupted interruption.

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
Program control commands
%prm
p1
p1 : System variable assignment value
%inst
Ends a subroutine called by the gosub command or a user-defined command. The return command restores the stack and returns to the caller.
By specifying p1, a value can be reflected from the subroutine to the system variable.
If a numeric value is specified for p1, it is assigned to the system variable stat.
If a string is specified for p1, it is assigned to the system variable refstr.
If a real number value is specified for p1, it is assigned to the system variable refdval.
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
Program control commands
%prm
"filename","cmdline"
"filename" : Name of the HSP object file to execute
"cmdline"  : String assigned to the system variable dir_cmdline
%inst
Executes an HSP program written in another file.
The specified file must be an HSP object file (extension AX).
Object files can be created using the "Create Object File" in the script editor.
^p
Example:
    run "MENU2.AX","-s"
^p
When executed, the previous program will be gone and the program in the "MENU2.AX" file will be executed from the beginning.
The state of the screen, variables, and memory buffer contents are all initialized. Also, execution starts with the string "-s" assigned to the system variable dir_cmdline.
^
If the "cmdline" parameter is omitted, the contents of the system variable dir_cmdline will be empty. Also, the system variable dir_cmdline can save up to 1024 characters.
^
The run command only works with object files targeting the same runtime. Also, operation on the HSP3Dish runtime is not currently supported.
%href

%index
stop
Program interruption
%group
Program control commands
%inst
Temporarily suspends program execution.
Use when waiting for a button to be clicked, etc.

%href
end

%index
wait
Interrupt execution for a certain time
%group
Program control commands
%prm
p1
p1(100) : Waiting time (in 10ms units)

%inst
Interrupts program execution for a certain period of time.
^
Specify the length of the wait time in p1. The unit is 10 milliseconds. (1 millisecond = 1/1000 seconds)
Regardless of the CPU speed, the waiting time will be the same on any model.
;If p1 is specified as 0, or omitted, the wait will be for the same amount of time as the previous time.
^
The await command allows for finer-grained waiting with higher precision than the wait command, but it consumes more CPU tasks than the wait command.
Unless you are processing real-time image rewriting, using the wait command puts less burden on the CPU (Windows).
It is a good idea to use the wait command for accessories that are always placed on the desktop, and the await command for game applications, etc.
^
In HSP, it is recommended to include a wait or await command in places where long time loops may occur in order to give processing time to other Windows tasks.
If you execute a program that loops for a long time in commands that do not have wait or await commands, it will be difficult to drag windows, or it will take time to switch to other tasks.
^
Note that the waiting time specified as a parameter does not guarantee strict execution. Errors may occur when CPU tasks or interrupt processing occur.
In particular, if you are proceeding with precise time such as a stopwatch or clock, be sure to acquire the absolute time with the gettime command and process it.

%href
await

%index
repeat
Indicates the beginning of a loop
%group
Program control commands
%prm
p1,p2
p1=1〜(-1) : Number of loops
p2=0〜(0)  : Start value of system variable cnt

%inst
Repeatedly executes the section between the repeat〜loop commands.
The repeat command indicates the starting point of the repetition.
The loop command returns to the repeat command that was last passed. In other words,
^p
	repeat 5
		print "Yahoo!"
	loop
^p
A program like this will display "Yahoo!" 5 times.
The parameter p1 of the repeat command can be used to specify the number of repetitions. If the number of times is omitted, or if a negative value is specified, it becomes an infinite loop.
If the number of repetitions is 0, the repeat portion is not executed and jumps to the corresponding loop command.
^
A nested structure in which the part including repeat〜loop is further looped can also be described. However, if you exit the loop without passing through the loop command normally, the nested structure will be messed up, so avoid structures that force you to exit the loop.
^
The system variable cnt can be referenced to check the number of loops or to use a counter. cnt normally starts from 0 and increases by 1 each time it loops with the loop command.
However, it is also possible to change the number from which the counter starts with p2 of the repeat command. For example, if you specify repeat 3,1, the value of the variable cnt will change in the order of 1, 2, 3.
^
Use the break command to forcibly exit the loop from within the loop. The continue command is also available to move to the next loop.

%href
loop
break
continue
foreach

%index
break
Exit the loop
%group
Program control commands
%inst
Forcibly exits from the loop between the repeat〜loop commands.
^p
	repeat 5
		if cnt=2 : break
		mes "Repeat["+cnt+"]"
	loop
^p
In the above example, the break command is executed by the judgment of the if command when the system variable becomes 2, that is, on the 3rd loop.
When the break command is executed, even if there are still repetitions remaining, it forcibly exits the repetition and continues execution from the command next to the loop command.
The break command and later (mes command in the above example) are not executed.
By using this command, you can create scripts such as the following.
^p
	repeat
		getkey a,1
		if a>0 : break
		await 10
	loop
^p
The above script is a loop that waits until the left mouse button is pressed.
If the number of times specification of the repeat command is omitted, it becomes an infinite loop, so it is used to repeat the same place until the button state becomes 1. When the button is pressed, the break command is executed and exits the repetition.

%href
repeat
loop
continue
foreach

%index
continue
Redo the loop
%group
Program control commands
%prm
p1
p1 = 0〜 : Repetition counter change value

%inst
Redoes the loop between repeat〜loop commands.
When the continue command is executed, it returns to the repeat command and executes the next repetition.
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
The display will be like this, and you can see that the mes command is not executed only when the system variable cnt is 2.
It may be difficult to understand at first, but the continue command can also be said to work the same as the loop command, but not in the same location as the loop command.
The repetition counter increases by 1 in the same way as the loop command even when the continue command is executed.
If the continue command is executed in the last repetition, the repeat〜loop ends, that is, execution continues from the command after the loop command.

%href
repeat
foreach
loop
break

%index
onkey
Specify key interrupt execution
%group
Program control command
%prm
goto/gosub *label
*label : Label name

%inst
Specifies the location to automatically jump to when a key is pressed.
^
After `onkey`, specify the `goto` or `gosub` keyword, followed by a label. `goto` performs a simple program jump. `gosub` performs a subroutine jump.
^
If the `goto/gosub` keyword is omitted, it behaves the same as the `goto` keyword.
^
When a label is specified with the `onkey` command, from then on, whenever a key is pressed while the HSP window is active, the program jumps to the label specified by `*label`.
^
Interrupt jumps are accepted and performed when the program is stopped with the `stop` command or during `wait` and `await` commands.
After an interrupt jump, the system variables `iparam`, `wparam`, and `lparam` are set.
^p
   Interrupt Cause | iparam     | wparam | lparam
 ---------------------------------------------------------
    onkey       | Character code | wParam | lParam
^p
The system variable `iparam` is assigned the parameter for each interrupt cause.
Also, `wparam` and `lparam` contain the parameters passed as Windows messages.
It is also possible to temporarily turn ON/OFF event interrupt execution.
^p
	onkey 0
^p
temporarily stops key interrupts.
^p
	onkey 1
^p
restarts temporarily stopped key interrupts.

%href
onclick
onexit
onerror

%index
onclick
Specify click interrupt execution
%group
Program control command
%prm
goto/gosub *label
*label : Label name

%inst
Specifies the location to automatically jump to when a mouse button is pressed.
^
After `onclick`, specify the `goto` or `gosub` keyword, followed by a label. `goto` performs a simple program jump. `gosub` performs a subroutine jump.
^
If the `goto/gosub` keyword is omitted, it behaves the same as the `goto` keyword.
^
When a label is specified with the `onclick` command, from then on, whenever there is a mouse click on the HSP window, the program jumps to the label specified by `*label`.
^
Interrupt jumps are accepted and performed when the program is stopped with the `stop` command or during `wait` and `await` commands.
After an interrupt jump, the system variables `iparam`, `wparam`, and `lparam` are set.
^p
   Interrupt Cause | iparam         | wparam | lparam
 -------------------------------------------------------
   onclick      | Mouse button ID | wParam | lParam
^p
The system variable `iparam` is assigned the parameter for each interrupt cause.
Also, `wparam` and `lparam` contain the parameters passed as Windows messages.
It is also possible to temporarily turn ON/OFF event interrupt execution.
^p
	onclick 0
^p
temporarily stops interrupts.
^p
	onclick 1
^p
restarts temporarily stopped interrupts.

%href
onkey
onexit
onerror

%index
onerror
Jump when an error occurs
%group
Program control command
%prm
goto/gosub *label
*label : Label name
%inst
Specifies the location to automatically jump to when an error occurs within HSP due to the script.
^
After `onerror`, specify the `goto` or `gosub` keyword, followed by a label. `goto` performs a simple program jump. `gosub` performs a subroutine jump.
^
If the `goto/gosub` keyword is omitted, it behaves the same as the `goto` keyword.
^
Normally, a system error message dialog is displayed when an error occurs, but instead, the program is set to jump to the specified label. After the jump, the following system variables are assigned information:
^p
	wparam : Error number
	lparam : Error line number
	iparam : 0 (none)
^p
Even if processing after an error is specified with the `onerror` command, after the necessary processing is completed, do not resume execution of the application and instead, terminate it with the `end` command.
The `onerror` command is not for recovering from errors.
Depending on the cause of the error, the HSP system itself may become unstable or malfunctions may occur. The `onerror` command can be used when you want the application to display its own error message when an error occurs during executable file creation, or when you want to display debugging information only when a specific error occurs.
^
It is also possible to temporarily turn ON/OFF the end-time jump.
^p
	onerror 0
^p
temporarily stops interrupts.
^p
	onerror 1
^p
restarts temporarily stopped interrupts.

%href
onkey
onclick
onexit

%index
exgoto
Conditional jump to specified label
%group
Program control command
%prm
var,p1,p2,*label
var    : Variable used for comparison
p1     : Comparison flag
p2     : Comparison value
*label : Label name
%inst
Conditionally jumps to the specified label based on the value assigned to the variable specified by `var` and the comparison value of `p2`.
For the comparison method, if the comparison flag of `p1` is 0 or greater (positive value), the program jumps when (var's value >= p2) is true, and if `p1` is -1 or less (negative value), the program jumps when (var's value <= p2) is true.
The variable specified by `var` must be an integer type. If it is initialized with a different type, an error will occur at runtime.
This command is provided to speed up some macro processing, such as `for`.
You can use the `exgoto` command alone, but from the perspective of script visibility, it is recommended to use normal conditional branching with the `if` command.
%href
goto
if

%index
on
Branching by number
%group
Program control command
%prm
p1 goto/gosub Label0,Label1…
p1=0〜(0) : Value for branching
Label0〜 : Branch destination label name
%inst
The `on` command selects a branch destination according to the value specified by `p1` (0, 1, 2...).
In the format "on number goto" or "on number gosub", you can then write one or more branch destination labels separated by ",".
If the specified number is 0, Label0 is the branch target, and if the number is 1, Label1 is the branch target. You can write any number of corresponding labels after 2, 3, 4...
"on number goto" is the same as a simple branch with the `goto` command, and "on number gosub" is the same as a subroutine jump with the `gosub` command.
If the number is a negative value or the corresponding label is not specified, branching does not occur and the program proceeds to the next line.
Please note that unlike N88-BASIC, the first label corresponds to the number 0. (In N88-BASIC, the number 1 is the first label.)
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
Program control macro
%prm
p1
p1=Conditional expression(1) : Condition for looping
%inst
The section from `while` to `wend` is repeated only while the condition following `while` is met.
If the condition is not met, the section from `while` to `wend` is not executed.
If the condition following `while` is omitted, the program loops infinitely.
^p
	a=0
	while a<5
	a=a+1:mes "A="+a
	wend     ; Repeats while A is less than or equal to 5
^p
Also, it is possible to resume and exit using the `_continue` and `_break` macros.
The `while` to `wend` control is implemented using the preprocessor's macro function.
Please use this if you want to write code similar to C or Java.
For beginners, we recommend the `repeat` to `loop` commands, or a loop description using the `if` command.
%href
_continue
_break
wend

%index
wend
End while loop
%group
Program control macro
%inst
Specifies the end of the loop section specified by `while`.
See the `while` macro section for more information.
%href
while

%index
until
End do loop
%group
Program control macro
%prm
p1
p1=Conditional expression(1) : Condition for looping
%inst
The section from `do` to `until` is repeated until the condition following `until` is met.
Even if the condition is met, the `do` to `until` section is executed at least once.
If the condition following `until` is omitted, the program does not loop.
^p
	a=0
	do
	a=a+1:mes "A="+a
	until a>5    ; Repeats do until A is greater than 5
^p
Also, it is possible to resume and exit using the `_continue` and `_break` macros.
The `do` to `until` control is implemented using the preprocessor's macro function.
Please use this if you want to write code similar to C or Java.
For beginners, we recommend the `repeat` to `loop` commands, or a loop description using the `goto` command in combination with the `if` command.
%href
_continue
_break
do

%index
do
Start do loop
%group
Program control macro
%inst
Specifies the beginning of the loop section up to `until`.
The loop condition is described by the `until` macro.
See the `until` macro section for more information.
%href
until

%index
for
Start specified number of loops
%group
Program control macro
%prm
p1,p2,p3,p4
p1    : Variable name
p2(0) : Initial value
p3(0) : End value
p4(1) : Increment
%inst
When a variable name, initial value, end value, increment, etc. are specified as parameters, the section between `for` and `next` is repeated the specified number of times.
Values in parentheses are the default values. The variable name cannot be omitted.
The specified variable is used as a counter, starting from the initial value, and the increment is added each time the loop is repeated. The loop is exited when the end value is reached (the end value is not included in the loop).
If the end value condition is met from the beginning, the loop is not executed.
Also, it is possible to resume and exit using the `_continue` and `_break` macros.
	for a,0,5,1
	mes "A="+a
	next    ; Repeats 5 times while a is between 0 and 4
^p
In this example, the variable a repeats 5 times with the values 0, 1, 2, 3, and 4 before the loop ends.
It is also possible to specify a negative value for the increment.
^p
	for a,5,0,-1
	mes "A="+a
	next    ; Repeats 5 times while a is between 5 and 1
^p
In this case, the variable a repeats 5 times with the values 5, 4, 3, 2, and 1 before the loop ends.
^
The initial value and final value specified in the for macro must be integers.
Specifying other types will result in an error.
Also, be careful as an infinite loop may occur depending on the specified increment or final value.
^
The for~next control is implemented using the preprocessor's macro function.
Please use it when you want to write code similar to C or Java.
We recommend repeat~loop commands or repetitive descriptions using if commands for beginners. The for macro internally generates a special program control command exgoto after macro expansion.
%href
_continue
_break
next
exgoto


%index
next
Ends the specified number of repetitions
%group
Program control macro
%inst
Specifies the end of the repetition interval specified by for.
See the for macro section for details.
%href
for


%index
switch
Start comparison block
%group
Program control macro
%prm
p1
p1 : Comparison source
%inst
switch~case~swend is a construct that allows you to write multiple conditional judgments and processing together in a block.
The comparison source parameter specified by p1 can be a variable or an expression. After switch,
^
case Comparison value
^
By placing this, you can describe the content to be processed if the "comparison source" is the same as the "comparison value" thereafter.
Multiple cases can be described in a block, and processing for each comparison value can be specified. If the case comparison is correct, the subsequent instructions are executed until swbreak exists. Also, instead of case
^
default
^
If you place this, the following will be executed if none of the comparison values specified in case match.
^
When the switch block is finished,
^
swend
^
Must be written at the end without fail.
The following is an example of a script using the switch macro.
^p
	a=0
	switch a               ; a is the comparison target
	case 0                 ; If a is 0
		mes "A=0"
		swbreak            ; End case 0 condition execution
	case 1                 ; If a is 1
		mes "A=1"
	default                ; If a is other than 0
		mes "A!=0"
		swbreak
	swend
^p
In this example, conditional branching is performed based on whether the content of variable a is 0, 1, or something else.
After "case 0", the execution continues until "swbreak", but in the case of "case 1", "swbreak" does not exist, so the "mes "A!=0"" that is executed after "default" is also executed. Please note this.
%href
case
default
swbreak
swend


%index
swend
End comparison block
%group
Program control macro
%inst
Specifies the end of the comparison block specified by switch.
See the switch macro section for details.
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
Within the switch~swend comparison block, specifies the processing that is executed when the condition does not match any of the previous cases.
See the switch macro section for details.
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
p1 : Comparison value
%inst
Within the switch~swend comparison block, specifies the condition with the comparison source and specifies the processing to be executed if the condition is correct.
See the switch macro section for details.
%href
switch
default
swbreak
swend


%index
swbreak
Comparison execution exit specification
%group
Program control macro
%inst
Exits from within the switch~swend comparison block.
See the switch macro section for details.
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
Restarts the loop between the standard macro-defined repetition macros: while~wend, do~until, for~next.
The current and subsequent blocks within the repetition are not executed, and the next repetition is restarted.
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
Exit the macro loop
%group
Program control macro
%inst
Breaks out of the standard macro-defined repetition macros: while~wend, do~until, for~next.
The current and subsequent blocks within the repetition are not executed, and the command after the loop is restarted.
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
Repeats as many times as the number of elements in the variable
%group
Program control command
%prm
p1
p1=Variable name : Variable to be looped

%inst
Repeats as many times as the number of elements stored in the array variable.
foreach is written at the loop start location like the repeat command, and the loop command is written at the loop end location.
The difference from the repeat command is that the number of elements of the specified array variable is the number of repetitions, and in the case of a module type variable, the loop content is not executed for elements deleted by the delmod command.
^p
Example :
	dim a,10
	foreach a
	mes "#"+cnt+":"+a.cnt
	loop
^p
By specifying the system variable cnt as an array element in the loop, you can simply write processing that repeatedly executes all elements.
Also, the break and continue commands function the same way as when using the repeat command.

%href
repeat
loop
break
continue



%index
oncmd
Windows message interrupt execution specification
%group
Program control command
%prm
goto/gosub *label,p1
*label : Label name
p1     : Message ID

%inst
Specifies the location to automatically jump to when the specified window message is sent.
^
After oncmd, specify the goto or gosub keyword followed by a label.
In the case of goto, it is a simple program jump. In the case of gosub, a subroutine jump is performed.
If the goto/gosub keyword is omitted, it will operate the same as the goto keyword.
^
When a label is specified by the oncmd command, from then on, when a message with p1 is sent to the HSP window, it jumps to the label specified by *label.
The oncmd command only targets the current operation window.
If you are creating multiple windows, you can register using the oncmd command for each window.
^
If a window message that has already been registered is specified, the previous registration is invalidated.
Also, interrupt registration is not synchronized with window initialization.
Once an interrupt is registered, it remains valid until the application is terminated.
^
The interrupt jump accepts interrupts and jumps when it is stopped by the stop command and the wait, await commands.
Also, after performing a jump due to an interrupt, the system variables iparam, wparam, and lparam are set.
The window ID in which the interrupt occurred can be obtained using the ginfo function.
^p
   Interrupt factor | iparam       | wparam | lparam
 ---------------------------------------------------------------------
   oncmd        | Message ID | wParam | lParam
^p
The system variable iparam is assigned the ID of the sent message.
Also, wparam and lparam store the parameters passed as Windows messages as is.
When a subroutine jump occurs due to the gosub keyword in the interrupt setting, it is possible to set a return value using the return command.
Specify the value to be returned by the window procedure with "return numeric value".
If no parameters are specified for the return command, the processing is passed to the default window procedure.
^
It is also possible to temporarily turn ON/OFF the event interrupt execution.
^p
	oncmd 0, Message ID
^p
Temporarily stops the interrupt with the specified message ID.
^p
	oncmd 1, Message ID
^p
Restarts the temporarily stopped interrupt.
It is also possible to turn ON/OFF event interrupt execution collectively.
^p
	oncmd 0
^p
Stops Windows message interrupts entirely.
^p
	oncmd 1
^p
Restarts Windows message interrupts entirely.


%href
onkey
onclick
onexit
onerror


%index
else
Execute the command on that line if the condition is not met
%group
Program control command
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
