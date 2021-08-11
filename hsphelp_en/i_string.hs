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
Read a string from the buffer
%group
String manipulation command
%prm
p1,p2,p3,p4,p5
p1 = variable: variable name to read the contents
p2 = variable: variable name to which the buffer is allocated
p3 = 0 ~: Buffer index (in Byte)
p4 = 0 to 255: ASCII code of delimiter character
p5 = 0 to (1024): Maximum number of characters to read

%inst
Reads the contents of any location in the buffer in memory as a string into a variable.
The character string is read until there is a code 00 or a line feed code. The line feed code is not included in the string.
^
You can also specify a delimiter character code and retrieve a character string delimited by any character.
^
The number of bytes read by this instruction is automatically assigned to a system variable called strsize. strsize is used to get the amount of movement to the next index.
^
For example, if the buffer of variable b contains the data'A',' B',' C',',' D',' E',' F'00 (exit code).
^p
getstr a,b,0,','
^p
When you execute, the contents of the variable a will be read out from the'A',' B', and'C', and will be the string "ABC", and the strsize will be 4.
^
The maximum number of characters (bytes) to be read can be specified by the p5 parameter.
If the p5 parameter is omitted, up to 1024 characters (byte) will be read.
(Since the buffer of the variable specified by p1 is automatically allocated, it is not necessary to set the buffer with the sdim instruction etc.)
^
This command is for efficiently cutting out text string data containing multiple lines and data separated by specific characters.
There is another set of memory notepad instructions for working with multi-line text. In addition, a strmid instruction is provided to extract a specific number of characters from a string.

%href
strmid




%index
noteadd
Addition / change of specified line
%group
String manipulation command
%prm
p1,p2,p3
p1 = Character string: Character string or variable name to be added / changed
p2 = 0 to (-1): Index to be added
p3 = 0 to 1 (0): Overwrite mode specification (0 = add / 1 = overwrite)

%inst
Add / change the contents of the memory notepad.
^
Specify the character string to be added / changed to p1. In p2, specify the target index.
If you omit the p2 parameter or specify -1, the last line is targeted.
The p3 parameter specifies whether to add or overwrite. If omitted or specified as 0, the mode is added and the target index and subsequent indexes are shifted one by one.
If p3 is 1, the overwrite mode is set and the contents of the target index are erased and replaced with the specified character string.
^
noteadd automatically increases the reserved capacity and stores it when trying to add contents to the variable buffer beyond the capacity reserved in advance by the sdim instruction or the like.
In other words, even if the memory buffer can only store up to 64 characters with "sdim a, 64", the memory size can be automatically adjusted according to the size to be added and stored safely.
^
In order to use the memory notepad instructions (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel instruction.

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
Delete row
%group
String manipulation command
%prm
p1
p1 = 0 ~: Index to delete

%inst
Deletes the specified index on the memory notepad.
The contents of the index specified by p1 are deleted, and subsequent indexes are shifted one by one.
^
In order to use the memory notepad instructions (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel instruction.

%href
notesel




%index
noteget
Read the specified line
%group
String manipulation command
%prm
p1,p2
p1 = variable: variable name of the read destination
p2 = 0 to (0): Index to read

%inst
Assigns the contents of the index specified by p2 in the memory notepad to the variable specified by p1. You can read the contents on any line in the memory notepad.
^
Note that the index starts at 0.
In the variable specified by p1, if you try to add the contents to the variable buffer beyond the capacity reserved in advance by the sdim instruction etc., the reserved capacity is automatically increased and stored. Also, the variable of p1 is forcibly changed to the string type.
^
In order to use the memory notepad instructions (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel instruction.

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
Memory notepad information acquisition
%group
String manipulation function
%prm
(p1)
p1 (0): Information acquisition mode

%inst
The noteinfo function gets information about the currently targeted memory notepad.
Specify the information acquisition mode with p1. In addition, macros corresponding to each mode are defined.
^p
   Mode: Macro content
 -----------------------------------------------------------
      0: notemax Total number of lines
      1: notesize Total number of characters (bytes)
^p
Use the notemax macro when you want to iterate over all lines with a multi-line string.
The total number of lines is the number of elements in the memory notepad.
If there are lines such as "APPLE", "ORANGE", and "GRAPE", it will be 3.
In this case, the index will be 0-2.
^
In order to use the memory notepad instructions (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel instruction.

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
Target buffer specification
%group
String manipulation command
%prm
p1
p1 = variable: variable name to which the buffer is allocated

%inst
Sets the variable specified in p1 to the memory notepad instruction buffer.
^
The variable specified in p1 is forcibly changed to the string type.
In order to use other memory notepad instructions (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel instruction.

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
Return of target buffer
%group
String manipulation command
%prm

%inst
The noteunsel instruction returns to the buffer setting that was set before the target buffer was set by the notesel instruction.
It is not always necessary to insert the noteunsel instruction, but by inserting it after using the buffer set by the notesel instruction, it is possible to prevent multiple buffer specifications by notesel.

%href
notesel





%index
strmid
Extract a part of the character string
%group
String manipulation function
%prm
(p1,p2,p3)
p1 = variable name: variable name that stores the character string to be retrieved
p2 = -1 ~: Index at the beginning of retrieval
p3 = 0 ~: Number of characters to retrieve
%inst
Returns the character string type variables specified in p1 from which characters have been extracted under the conditions specified in p2 and p3.
Specifies the index to start fetching with p2. In this case, the first character at the beginning of the character string is set to 0, and the number increases in order of 1, 2, 3 ... Please note that it does not start from 1.
Specify the number of characters to retrieve with p3. If you specify more than the number of characters actually stored, up to the actual number of characters will be retrieved.
Also, if -1 is specified for p2, the number of characters specified by p3 is extracted from the right side of the character string.
%sample
	b="ABCDEF"
a = strmid (b, -1,3); Extract 3 characters from the right
a = strmid (b, 1,3); Extract 3 characters from the 2nd character from the left
%href
getstr


%index
instr
Search for a character string
%group
String manipulation function
%prm
(p1,p2,"string")
p1 = variable name: Character string type variable name that stores the character string to be searched
p2 = 0 to (0): Index to start searching
"string": The string to search for
%inst
Checks if the string specified by "string" exists in the string type variable specified by p1 and returns the index.
^
If the specified string is found, the index value is returned. This starts with 0 as the first character of the character string and increases in order of 1, 2, 3 ... (similar to the index specified by the strmid instruction).
Please note that it does not start from 1.
(If p2 is specified, the index will start from p2 (0).)
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
String manipulation command
%prm
"filename"
"filename": write file name
%inst
Memory Saves the contents of the notepad instruction buffer as a text file to the specified file.
Note that you must first set the target buffer with the notesel instruction.
The notesave instruction saves the length of the character string contained in the specified buffer.

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
Read target buffer
%group
String manipulation command
%prm
"filename",p1
"filename": Read file name
p1 (-1): Upper limit of read size
%inst
Reads the specified file into the memory notepad instruction buffer.
Normally, the text file is read and the target is read by the memory notepad instruction.
The buffer for memory notepad instructions automatically adjusts the reserved size according to the file size, so there is no need to specify the buffer size in advance.
You can specify the maximum size of the file to read with p1.
If the specification is omitted or a negative value is specified, any size will be read.
It is also possible to read files other than text files.
Note that you must first set the target buffer with the notesel instruction.
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
Get part of the path
%group
String manipulation function
%prm
(p1,p2)
p1 = character string: Original character string to be extracted
p2 = 0 ~: Specify information type
%inst
The character string indicating the file path specified in p1 is converted into the type of information specified in p2 and returned as a character string.
^p
example :
	a="c:\\disk\\test.bmp"
	b = getpath(a,8+1)
	mes b
↓ (Result)
Become "test"
^p
The details of the type specification are as follows.
^p
   Type: Content
 -----------------------------------------------------------
     0: Copy of character string (no operation)
     1: File name excluding extension
     2: Extension only (.???)
     8: Remove directory information
    16: Convert string to lowercase
    32: Directory information only
^p
Multiple type values can be specified by totaling.
If type 16 is specified, all alphabetic strings are converted to lowercase.
%href
getstr
instr





%index
strf
Convert formatted string
%group
String manipulation function
%prm
("format",p1...)
"format": Format string
p1: Formatting parameters
%inst
Returns a string that is an integer or real number converted to a string in the proper format.
In "format", specify the following format specification string.
^p
example :
	a=123
mes strf ("decimal [% d]", a)
mes strf ("hexadecimal [% x]", a)
mes strf ("with decimal digit number specified [% 05d]", a)
mes strf ("with specified number of hexadecimal digits [% 05x]", a)
	a=sqrt(2)
mes strf ("decimal real number [% f]", a)
mes strf ("decimal real number digits specified [% 3.10f]", a)
^p
In the part indicating the parameter specified after p1, specify after "%".
"% D" can display an integer value, "% x" can display a hexadecimal integer value, "% c" can display a character code, and "% f" can display a real value.
If multiple parameters are specified in the format, separate the parameters with "," and describe the parameters in succession.
If you want to display the normal "%" symbol, specify "%%".
^p
example :
	a=1:b=2.0:c="ABC"
	mes strf("[%03d] [%f] [%s]",a,b,c)
^p
The format string is similar to the sprintf format supported by the C runtime library.
^p
	%[width][.precision][I64]type

width: Minimum number of characters to output
precision: Maximum number of characters to output
(If type is f, the number of digits after the decimal point)
I64: Prefix indicating 64bit value
type: Type of input parameter
^p
The format is as follows, and the description in [] can be omitted.
The characters that can be specified by type are as follows.
^p
Character content
	---------------------------------------------
c 1-byte character code
d Signed decimal integer
i Signed decimal integer
u unsigned decimal integer
o unsigned octal integer
x unsigned hexadecimal integer (lowercase)
X unsigned hexadecimal integer (uppercase)
Signed real number in e [sign] dd [d] format
Signed real number (uppercase) in E [sign] dd [d] format
f dddd.dddd signed real number
(The number of digits in the integer part is determined by the absolute value of that number,
The number of decimal places is determined by the required precision. )
g Of the signed values output in format f or e
The shorter format that can represent the specified value and precision
G Same as format G, using uppercase letters
p Prints the address pointed to by the hexadecimal argument
s string
^p
Please note that the format specification string recognizes only strings up to 1023 characters.

%href
mes
print




%index
cnvwtos
Convert unicode to regular string
%group
String manipulation function
%prm
(p1)
p1 = variable: the name of the variable from which the contents are read

%inst
Reads the data stored in the variable as unicode (UTF-16) and returns the converted character string.
It can be used when handling unicode data, data conversion with external DLLs, COM objects, etc.

%href
cnvstow
cnvstoa
cnvatos
str
%port-
Let




%index
cnvstow
Convert normal strings to unicode
%group
String manipulation command
%prm
p1,"string"
p1 = variable: variable name to write the result
"string": conversion source string

%inst
Converts the data specified by "character string" to a unicode (UTF-16) character string and saves it in the variable buffer.
The variable specified by p1 must have a sufficient buffer size for conversion as a character string type in advance.
It can be used when handling unicode data, data conversion to external DLLs, COM objects, etc.

%href
cnvwtos
cnvstoa
cnvatos
str
%port-
Let


%index
strtrim
Remove only specified characters
%group
String manipulation function
%prm
(p1,p2,p3)
p1 = variable: variable to which the original string is assigned
p2 = 0 to 3 (0): Specify the position to remove
p3 = 0 to 65535 (32): Character code
%inst
Removes only the specified character from the character string stored in the variable specified by p1.
You can specify the position to remove with p2.
If p2 is omitted, it will be removed only if both ends of the string are specified characters.
You can specify the character code to be removed with p3.
If p3 is omitted, it will be a half-width space (32).
^p
example :
	a=" ABC DEF "
	b = strtrim(a,0,' ')
	mes b
^p
The details of the value indicating the removal position specified by p2 are as follows.
^p
   Type: Content
 -----------------------------------------------------------
     0: Remove the specified characters at both ends (default)
     1: Remove the specified character at the left end
     2: Remove the specified character at the right end
     3: Remove all specified characters in the string
^p
For the character code specified in p3, you can specify a 2-byte code that indicates double-byte characters.
^p
example :
s = "Hello, this is full-width characters."
zenspace = ""; Full-width space
code = wpeek (zenspace, 0); Get the code for double-byte space
	mes strtrim(s,3,code)
^p
%href
strmid
instr



%index
split
Substitute the element divided from the string
%group
String manipulation command
%prm
p1,"string",p2...
p1 = variable: variable to which the original string is assigned
"string": Delimiter string
p2 = variable: variable to which the split element is assigned

%inst
Assigns the element divided by the specified character string to the variable.
For example, you can extract the elements of "12", "34", and "56" from a character string separated by "," such as "12,34,56" and assign them to different variables.
In p1, specify the variable name to which the original string was assigned. (Variable must be of type string)
Specify the character string to separate to "string".
After p2, specify the variable name to which the divided element is assigned.
You can specify any number of variables to be assigned, separated by ",".
The divided elements are assigned in order from the first specified variable.
^p
example :
	buf="12,34,56"
	split buf, ",", a, b, c
	mes a
	mes b
	mes c
^p
If the number of original elements is less than the number of variables specified, the remaining variables are assigned an empty string ("").
If there are more divided elements than the specified number of variables, they will be assigned to the specified array of variables.
^p
example :
	buf="12,34,56,78"
	split buf, ",", results
	repeat stat
mes "array (" + cnt + ") =" + results (cnt)
	loop
^p
After execution, the number that can be divided is assigned to the system variable stat.

%href
getstr
csvnote
%port-
Let



%index
strrep
Replace strings
%group
String manipulation command
%prm
p1, "Search string", "Replacement string"
p1 = variable name: Character string type variable name that stores the character string to be searched
"Search string": Search string
"Replacement string": The replacement string
%inst
In all the contents of the string type variable specified by p1
Replaces the character string specified in "Search character string" with "Replacement character string".
^
After execution, the system variable stat is assigned the number of times the string has been replaced.

%href
instr



%index
notefind
Memory notepad search
%group
String manipulation function
%prm
("string",p1)
"string": The string to search for
p1 (0): Search mode

%inst
Searches the currently targeted memory notepad for lines that contain a particular string.
Specify the search mode with p1. Macros corresponding to each mode are defined.
^p
   Mode: Macro content
 -----------------------------------------------------------
      0: notefind_match Find the line that exactly matches "string"
      1: notefind_first Search for lines starting with "string"
      2: search for lines containing notefind_instr "string"
^p
The return value is the line (index) that matches the specified string from all lines in the memory notepad.
If there are no matching rows in the search, -1 is returned.
^
In order to use the notefind function, you must first set the target buffer with the notesel instruction.

%href
notesel
noteget
notedel
notemax
notesize



%index
cnvatos
Convert ANSI string to regular string
%note
Include hsp3utf.as.
%group
String manipulation function
%prm
(p1)
p1 = variable: the name of the variable from which the contents are read

%inst
Reads the data stored in the variable as an ANSI (Shift JIS) character string and returns the converted character string.
This instruction only works on the runtime (hsp3utf), which treats UTF-8 as standard string code. Please note that an error will occur with standard HSP3.


%href
cnvstow
cnvwtos
cnvstoa
str
%port-
Let



%index
cnvstoa
Convert regular strings to ANSI strings
%note
Include hsp3utf.as.
%group
String manipulation command
%prm
p1,"string"
p1 = variable: variable name to write the result
"string": conversion source string

%inst
Converts the data specified by "character string" to an ANSI (Shift JIS) character string and saves it in the variable buffer.
The variable specified by p1 must have a sufficient buffer size for conversion as a character string type in advance.
This instruction only works on the runtime (hsp3utf), which treats UTF-8 as standard string code. Please note that an error will occur with standard HSP3.

%href
cnvstow
cnvwtos
cnvatos
str
%port-
Let




%index
strexchange
Replace strings in scripts
%group
String manipulation command
%prm
"filename",p1
"filename": File name where the replacement string (strmap) is recorded
p1 (0): Replacement option

%inst
Replaces all string data contained in the script at once.
It is necessary to prepare a file (strmap) in which the replacement character string and hash code are recorded in advance.
(Strmap can be generated by the hspcmp command or the hspcmp plugin command. See sample / new36 / strex_test1.hsp sample for details)
The string in the script will be replaced based on the file (strmap) specified by "filename".
Specify the replacement option with p1.
If p1 is 0, only the string of the corresponding hash code is converted.
If p1 is 1, the replacement will not be performed if all the hash codes in the replacement data (strmap) do not match.
After executing strexchange, the result is assigned to the system variable stat.
If the system variable stat is 0, it indicates normal completion, otherwise it indicates that there was an error.

%port-
Let




