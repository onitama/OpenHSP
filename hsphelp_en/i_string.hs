;
;	HSP help manager HELP source file
;	(Lines starting with ";" are treated as comments)
;

%type
Built-in Command
%ver
3.6
%note
Standard command for ver3.6
%date
2020/01/07
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli
Let

%index
getstr
Read string from buffer
%group
String Operation Commands
%prm
p1,p2,p3,p4,p5
p1=variable : Variable name to read the content into
p2=variable : Variable name to which the buffer is assigned
p3=0〜    : Buffer index (in bytes)
p4=0〜255 : ASCII code of the delimiter character
p5=0〜(1024) : Maximum number of characters to read

%inst
Reads the content located at an arbitrary location in the memory buffer as a string into a variable.
The string is read until code 00 or a line feed code is encountered. The line feed code is not included in the string.
^
You can also specify a delimiter character code to retrieve strings delimited by arbitrary characters.
^
The number of bytes read by this command is automatically assigned to the system variable strsize. strsize is used to obtain the amount of movement to the next index.
^
For example, if the variable b buffer contains the data 'A' 'B' 'C' ',' 'D' 'E' 'F' 00 (end code),
^p
getstr a,b,0,','
^p
Then, the content of variable a will be the part 'A' 'B' 'C' read as the string "ABC", and strsize will be 4.
^
The p5 parameter allows you to specify the maximum number of characters (bytes) to read.
If the p5 parameter is omitted, up to 1024 characters (bytes) are read.
(The buffer for the variable specified by p1 is automatically allocated, so you don't need to set the buffer with the sdim command, etc.)
^
This command is designed to efficiently extract text string data containing multiple lines, or data separated by specific characters.
There are also memory notepad command sets available for handling multi-line text. The strmid command is available for extracting a specific number of characters from a string.

%href
strmid

%index
noteadd
Add or change specified line
%group
String Operation Commands
%prm
p1,p2,p3
p1=string  : String or variable name to add or change
p2=0〜(-1) : Index to add to
p3=0〜1(0) : Overwrite mode specification (0=add・1=overwrite)

%inst
Adds or changes the content of the memory notepad.
^
Specify the string to add or change in p1. Specify the target index in p2.
If the p2 parameter is omitted or -1 is specified, the last line will be targeted.
Specify whether to add or overwrite with the p3 parameter. If omitted or 0 is specified, it will be in add mode, and the index after the target index will be shifted one by one.
If p3 is 1, it will be in overwrite mode, the content of the target index will be deleted, and replaced with the specified string.
^
If noteadd tries to add content beyond the capacity previously allocated to the variable buffer by the sdim command, it automatically increases the allocated capacity and stores the content.
In other words, even if it is a memory buffer that can only store up to 64 characters with "sdim a,64", the memory size is automatically adjusted according to the size to be added, so it can be stored safely.
^
To use memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
notesel

%sample
	sdim a,10000
	notesel a
	noteadd "newidx1"
	noteadd "newidx3"
	noteadd "newidx2",1
	mes a
	stop

%index
notedel
Delete line
%group
String Operation Commands
%prm
p1
p1=0〜 : Index to delete

%inst
Deletes the specified index in the memory notepad.
The content of the index specified by p1 is deleted, and the subsequent indexes are shifted one by one.
^
To use memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
notesel

%index
noteget
Read specified line
%group
String Operation Commands
%prm
p1,p2
p1=variable : Variable name to read into
p2=0〜(0) : Index to read

%inst
Assigns the content of the index specified by p2 in the memory notepad to the variable specified by p1. You can read the content of any line in the memory notepad.
^
Note that the index starts from 0.
If you try to add content beyond the capacity previously allocated to the variable buffer by the sdim command, the variable specified by p1 will automatically increase the allocated capacity and store the content. Also, the variable of p1 is forcibly changed to a string type.
^
To use memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
notesel

%sample
	a="idx0\nidx1\nidx2"
	notesel a
	noteget b,1
	mes b
	stop

%index
noteinfo
Get memory notepad information
%group
String Operation Functions
%prm
(p1)
p1(0) : Information acquisition mode

%inst
The noteinfo function retrieves information about the currently targeted memory notepad.
Specify the information acquisition mode with p1. Macros corresponding to each mode are also defined.
^p
   Mode : Macro    Content
 -----------------------------------------------------------
      0   : notemax   Total number of lines
      1   : notesize  Total number of characters (bytes)
^p
The notemax macro is used when you want to perform repetitive processing on all lines of a multi-line string.
The total number of lines is the number of elements in the memory notepad.
If there are lines "APPLE", "ORANGE", and "GRAPE", it will be 3.
In this case, the index will be from 0 to 2.
^
To use memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
notesel
noteget
notedel
notemax
notesize

%sample
	nmax=0
	notesel a
	noteload "aaa.txt"
	idx=0
	repeat notemax
		noteget b,idx
		print "index"+idx+"="+b
		idx++
	loop
	stop

%index
notesel
Specify target buffer
%group
String Operation Commands
%prm
p1
p1=variable : Variable name to which the buffer is assigned

%inst
Sets the variable specified by p1 as the buffer for the memory notepad command.
^
The variable specified by p1 is forcibly changed to a string type.
To use other memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
noteunsel
noteget
noteadd
notedel
noteinfo
notemax
notesize
notefind

%sample
notesel a
noteadd "test strings"
mes a
stop

%index
noteunsel
Restore target buffer
%group
String Operation Commands
%prm

%inst
The noteunsel command returns to the buffer setting that was set before the buffer was set as the target with the notesel command.
The noteunsel command does not necessarily have to be included, but by including it after finishing using the buffer set by the notesel command, it prevents multiple buffer specifications by notesel.

%href
notesel

%index
strmid
Extract part of string
%group
String Operation Functions
%prm
(p1,p2,p3)
p1=variable name : Variable name in which the string to be extracted is stored
p2=-1〜   : Index to start extracting
p3=0〜    : Number of characters to extract
%inst
Returns the characters extracted from the string type variable specified by p1 according to the conditions specified by p2 and p3.
Specify the index to start extracting with p2. This is because the first character of the string is 0, and then it increases in order as 1, 2, 3... Please note that it does not start from 1.
Specify the number of characters to extract with p3. If you specify more than the number of characters actually stored, the actual number of characters will be extracted.
Also, if you specify -1 for p2, it will extract the number of characters specified by p3 from the right of the string.
%sample
	b="ABCDEF"
	a=strmid(b,-1,3)   ; Extract 3 characters from the right
	a=strmid(b,1,3)    ; Extract 3 characters from the 2nd character from the left
%href
getstr

%index
instr
Search string
%group
String Operation Functions
%prm
(p1,p2,"string")
p1=variable name : String type variable name in which the string to be searched is stored
p2=0〜(0) : Index to start searching
"string"  : String to search
%inst
Checks whether the string specified by "string" is in the string type variable specified by p1 and returns the index.
^
If the specified string is found, the index value is returned. This is because the first character of the string is 0, and then it increases in order as 1, 2, 3... (similar to the index specified by the strmid command).
Please note that it does not start from 1.
(If p2 is specified, the index will be based on p2 (0).)
(If p2 is a negative value, -1 is always returned.)
If the specified string is not found, -1 is returned.
%href
strmid
strrep
notefind

%index
notesave
Save target buffer
%group
String Operation Commands
%prm
"filename"
"filename" : Write file name
%inst
Saves the contents of the memory notepad command buffer to the specified file as a text file.
Note that you must always set the target buffer with the notesel command first.
The notesave command saves the string length included in the specified buffer.

%href
notesel
noteload
noteget
noteadd
notedel
noteinfo
%port-
Let

%index
noteload
Load target buffer
%group
String Operation Commands
%prm
"filename",p1
"filename" : Read file name
p1(-1)     : Upper limit of read size
%inst
Loads the specified file into the memory notepad command buffer.
Normally, it reads a text file and uses it as a target to be read out by the memory notepad command.
Since the memory notepad command buffer automatically adjusts the allocation size according to the file size, there is no need to specify the buffer size in advance.
You can specify the maximum size of the file to be read with p1.
If you omit the specification or set it to a negative value, it will read any size.
It is also possible to read files other than text files.
Note that you must always set the target buffer with the notesel command first.
%href
notesel
notesave
noteget
noteadd
notedel
noteinfo
notemax
notesize
notefind


%index
getpath
Get a portion of a path
%group
String Manipulation Functions
%prm
(p1,p2)
p1=String : Original string to extract from
p2=0~ : Information type specification
%inst
Returns a string that is the result of converting the file path string specified by p1 into the information of the type specified by p2.
^p
Example:
	a="c:\\disk\\test.bmp"
	b = getpath(a,8+1)
	mes b
	↓(Result)
	"test" will be the result
^p
Details of the type specification are as follows:
^p
   Type : Content
 -----------------------------------------------------------
     0    : String copy (no operation)
     1    : Filename excluding extension
     2    : Extension only (.???)
     8    : Remove directory information
    16    : Convert string to lowercase
    32    : Directory information only
^p
Multiple specifications can be made by summing the type values.
If type 16 is specified, all alphabetic characters are converted to lowercase.
%href
getstr
instr





%index
strf
Convert formatted string
%group
String Manipulation Functions
%prm
("format",p1...)
"format" : Format specification string
p1 : Format specification parameter
%inst
Returns a string in which an integer or real value has been converted to a string in an appropriate format.
Specify a format specification string like the following in "format".
^p
Example:
	a=123
	mes strf("Decimal[%d]",a)
	mes strf("Hexadecimal[%x]",a)
	mes strf("Decimal with digit specification[%05d]",a)
	mes strf("Hexadecimal with digit specification[%05x]",a)
	a=sqrt(2)
	mes strf("Decimal real number[%f]",a)
	mes strf("Decimal real number with digit specification[%3.10f]",a)
^p
For the part that indicates the parameter specified after p1, make the specification following "%".
"%d" displays an integer value, "%x" displays a hexadecimal integer value, "%c" displays a character code, and "%f" displays a real value.
If you specify multiple parameters in the format, describe the parameters consecutively, separated by "," by that number.
To display a normal "%" symbol, specify "%%".
^p
Example:
	a=1:b=2.0:c="ABC"
	mes strf("[%03d] [%f] [%s]",a,b,c)
^p
The format specification string is almost the same as the sprintf format supported by the C runtime library.
^p
	%[width][.precision][I64]type

	width : Minimum number of characters to output
	precision : Maximum number of characters to output
	            (Number of digits after the decimal point if type is f)
	I64   : Prefix indicating a 64bit value
	type  : Type of parameter to be input
^p
The above format is used, and the description in [] can be omitted.
The characters that can be specified by type are as follows:
^p
	Character           Content
	---------------------------------------------
	  c     1-byte character code
	  d	Signed decimal integer
	  i	Signed decimal integer
	  u	Unsigned decimal integer
	  o	Unsigned octal integer
	  x	Unsigned hexadecimal integer (lowercase)
	  X	Unsigned hexadecimal integer (uppercase)
	  e	Signed real value in [sign]dd[d] format
	  E	Signed real value in [sign]dd[d] format (uppercase)
	  f	Signed real value in dddd.dddd format
		(The number of digits in the integer part is determined by the absolute value of the number,
		 and the number of digits in the decimal part is determined by the required precision.)
	  g	The shorter of the formats f or e used to output a signed value that
		can represent the specified value and precision
	  G	Same as format G, but uses uppercase letters
	  p	Outputs the address pointed to by the hexadecimal argument
	  s	String
^p
Note that only strings up to 1023 characters are recognized as format specification strings.

%href
mes
print




%index
cnvwtos
Convert unicode to normal string
%group
String Manipulation Functions
%prm
(p1)
p1=Variable : Variable name from which to read content

%inst
Reads the data saved in the variable as unicode (UTF-16) and returns it as a normal string.
It can be used when handling unicode data or for data conversion with external DLLs and COM objects.

%href
cnvstow
cnvstoa
cnvatos
str
%port-
Let




%index
cnvstow
Convert normal string to unicode
%group
String Manipulation Commands
%prm
p1,"string"
p1=Variable : Variable name to write the result to
"string" : String to convert

%inst
Converts the data specified by "string" into a unicode (UTF-16) string and saves it in the variable buffer.
The variable specified by p1 must have a buffer size sufficient for conversion as a string type in advance.
It can be used when handling unicode data or for data conversion to external DLLs and COM objects.

%href
cnvwtos
cnvstoa
cnvatos
str
%port-
Let


%index
strtrim
Remove only the specified character
%group
String Manipulation Functions
%prm
(p1,p2,p3)
p1=Variable : Variable with the original string assigned
p2=0~3(0) : Specification of the position to remove
p3=0~65535(32) : Character code
%inst
Removes only the specified characters from the string stored in the variable specified by p1.
You can specify the position to remove with p2.
If p2 is omitted, it will only be removed if the specified characters are at both ends of the string.
You can specify the character code to remove with p3.
If p3 is omitted, it will be a half-width space (32).
^p
Example:
	a=" ABC DEF "
	b = strtrim(a,0,' ')
	mes b
^p
The details of the values indicating the removal position specified by p2 are as follows:
^p
   Type : Content
 -----------------------------------------------------------
     0    : Remove the specified characters at both ends (default)
     1    : Remove the specified characters at the left end
     2    : Remove the specified characters at the right end
     3    : Remove all specified characters in the string
^p
You can specify a 2-byte code indicating a full-width character as the character code specified by p3.
^p
Example:
	s="　Hello　This is a full-width character　"
	zenspace="　"				; Full-width space
	code = wpeek(zenspace,0)		; Get the code for the full-width space
	mes strtrim(s,3,code)
^p
%href
strmid
instr



%index
split
Assign elements separated from a string
%group
String Manipulation Commands
%prm
p1,"string",p2...
p1=Variable : Variable with the original string assigned
"string" : String for delimiters
p2=Variable : Variable to which the divided element is assigned

%inst
Assigns elements separated by the specified string to variables.
For example, you can extract the elements "12", "34", and "56" from a string separated by "," such as "12,34,56" and assign them to separate variables.
Specify the variable name with the original string assigned in p1. (The variable must be a string type)
Specify the string for delimiters in "string".
Specify the variable names to which the divided elements will be assigned in p2 and later.
You can specify as many variables to assign as you want, separated by ",".
The divided elements are assigned to the variables in order from the first variable specified.
^p
Example:
	buf="12,34,56"
	split buf, ",", a, b, c
	mes a
	mes b
	mes c
^p
If there are fewer original elements than the number of specified variables, an empty string ("") is assigned to the remaining variables.
If there are more divided elements than the number of specified variables, they are assigned to the array of the specified variables.
^p
Example:
	buf="12,34,56,78"
	split buf, ",", results
	repeat stat
		mes "Array("+cnt+")="+results(cnt)
	loop
^p
After execution, the system variable stat is assigned the number of divisions that were possible.

%href
getstr
csvnote
%port-
Let



%index
strrep
Replace string
%group
String Manipulation Commands
%prm
p1,"Search string","Replacement string"
p1=Variable name : String type variable name that stores the string to be searched
"Search string" : String to search for
"Replacement string" : String to replace with
%inst
Replaces all occurrences of the string specified by "Search string" with the "Replacement string" in the entire contents of the string type variable specified by p1.
^
After execution, the system variable stat is assigned the number of times the string was replaced.

%href
instr



%index
notefind
Memory notepad search
%group
String Manipulation Functions
%prm
("string",p1)
"string" : String to search for
p1(0) : Search mode

%inst
Searches the currently targeted memory notepad for a line containing a specific string.
Specify the search mode in p1. Macros corresponding to each mode are defined.
^p
   Mode : Macro Content
 -----------------------------------------------------------
      0 : notefind_match Search for a line that completely matches "string"
      1 : notefind_first Search for a line that starts with "string"
      2 : notefind_instr Search for a line containing "string"
^p
The return value is the line (index) that matches the specified string from all lines of the memory notepad.
If there is no line matching the search, -1 is returned.
^
In order to use the notefind function, you must first set the target buffer with the notesel command.

%href
notesel
noteget
notedel
notemax
notesize



%index
cnvatos
Convert ANSI string to normal string
%note
Include hsp3utf.as.
%group
String Manipulation Functions
%prm
(p1)
p1=Variable : Variable name from which to read content

%inst
Reads the data saved in the variable as an ANSI (ShiftJIS) string and returns it as a normal string.
This command only works on runtimes (hsp3utf) that treat UTF-8 as the standard string code. Please note that it will be an error in standard HSP3.


%href
cnvstow
cnvwtos
cnvstoa
str
%port-
Let



%index
cnvstoa
Convert normal string to ANSI string
%note
Include hsp3utf.as.
%group
String Manipulation Commands
%prm
p1,"string"
p1=Variable : Variable name to write the result to
"string" : String to convert

%inst
Converts the data specified by "string" into an ANSI (ShiftJIS) string and saves it in the variable buffer.
The variable specified by p1 must have a buffer size sufficient for conversion as a string type in advance.
This command only works on runtimes (hsp3utf) that treat UTF-8 as the standard string code. Please note that it will be an error in standard HSP3.

%href
cnvstow
cnvwtos
cnvatos
str
%port-
Let




%index
strexchange
Replace strings in the script
%group
String Manipulation Commands
%prm
"filename",p1
"filename" : Filename where the replacement string (strmap) is recorded
p1(0) : Replacement option

%inst
Replaces all string data included in the script collectively.
It is necessary to prepare a file (strmap) in which the replacement string and hash code are recorded in advance.
(strmap can be generated by the hspcmp command or the hspcmp plugin command. For details, refer to the sample/new36/strex_test1.hsp sample.)
The strings in the script are replaced based on the file (strmap) specified by "filename".
Specify the replacement option with p1.
If p1 is 0, only strings with matching hash codes are converted.
If p1 is 1, replacement will not be performed if all hash codes in the replacement data (strmap) do not match.
After strexchange is executed, the system variable stat is assigned the result.
If the system variable stat is 0, it indicates normal termination; otherwise, it indicates that there was an error.
%port-
Let

(Please fill in the part after "Let" with the actual content you want to be translated.)
