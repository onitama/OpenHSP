;
;	HSP help manageróp HELPÉ\Å[ÉXÉtÉ@ÉCÉã
;	(êÊì™Ç™Åu;ÅvÇÃçsÇÕÉRÉÅÉìÉgÇ∆ÇµÇƒèàóùÇ≥ÇÍÇ‹Ç∑)
;

%type
Built-in command
%ver
3.6
%note
ver3.6 standard command
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
String manipulation commands
%prm
p1,p2,p3,p4,p5
p1=Variable : Variable name to read the content into
p2=Variable : Variable name to which the buffer is assigned
p3=0Å` : Buffer index (in bytes)
p4=0Å`255 : ASCII code of the delimiter character
p5=0Å`(1024) : Maximum number of characters to read

%inst
Reads the content from an arbitrary location in the memory buffer as a string into a variable.
The string is read until the code 00 or a newline code is encountered. The newline code is not included in the string.
^
It is also possible to specify a delimiter character code to extract a string separated by an arbitrary character.
^
The number of bytes read by this command is automatically assigned to the system variable strsize. strsize is used to obtain the amount of movement to the next index.
^
For example, if the variable b's buffer contains the data 'A' 'B' 'C' ',' 'D' 'E' 'F' 00 (end code),
^p
getstr a,b,0,','
^p
When executed, the variable a's content will be the part 'A' 'B' 'C' read as the string "ABC", and strsize will be 4.
^
The p5 parameter allows you to specify the maximum number of characters (bytes) to read.
If the p5 parameter is omitted, up to 1024 characters (bytes) will be read.
(The buffer for the variable specified by p1 is automatically allocated, so there is no need to set up the buffer with the sdim command, etc.)
^
This command is designed for efficiently cutting out text string data containing multiple lines or data separated by specific characters.
To handle text containing multiple lines, there are also memory notepad command sets. In addition, the strmid command is available to extract a specific number of characters from a string.

%href
strmid




%index
noteadd
Add/change specified line
%group
String manipulation commands
%prm
p1,p2,p3
p1=String : String or variable name to add or change
p2=0Å`(-1) : Index to add to
p3=0Å`1(0) : Overwrite mode specification (0=add, 1=overwrite)

%inst
Adds or changes the contents of the memory notepad.
^
Specify the string to add or change in p1. Specify the target index in p2.
If the p2 parameter is omitted or -1 is specified, the last line is targeted.
Specify whether to add or overwrite with the p3 parameter. If omitted or 0 is specified, it is in add mode, and the index after the target index is shifted by one.
If p3 is 1, it is in overwrite mode, and the contents of the target index are deleted and replaced with the specified string.
^
noteadd automatically increases the allocated capacity and stores it when trying to add content beyond the capacity pre-allocated in the variable buffer with the sdim command.
In other words, even in a memory buffer that can only store up to 64 characters with "sdim a,64", the memory size is automatically adjusted and safely stored according to the size to be added.
^
In order to use the memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

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
String manipulation commands
%prm
p1
p1=0Å` : Index to delete

%inst
Deletes the specified index of the memory notepad.
The contents of the index specified in p1 are deleted, and the subsequent indexes are shifted by one.
^
In order to use the memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

%href
notesel




%index
noteget
Read specified line
%group
String manipulation commands
%prm
p1,p2
p1=Variable : Variable name to read to
p2=0Å`(0) : Index to read

%inst
Assigns the contents of the index specified in p2 in the memory notepad to the variable specified in p1. You can read the contents of any line in the memory notepad.
^
Note that the index starts from 0.
If you try to add content beyond the capacity pre-allocated in the variable buffer with the sdim command, the variable specified in p1 automatically increases the allocated capacity and stores it. Also, the p1 variable is forcibly changed to a string type.
^
In order to use the memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

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
String manipulation functions
%prm
(p1)
p1(0) : Information acquisition mode

%inst
The noteinfo function obtains information about the currently targeted memory notepad.
Specify the information acquisition mode in p1. Also, macros corresponding to each mode are defined.
^p
   Mode : Macro    Content
 -----------------------------------------------------------
      0   : notemax   Total number of lines
      1   : notesize  Total number of characters (bytes)
^p
The notemax macro is used when you want to perform repetitive processing on all lines in a multi-line string.
The total number of lines is the number of elements in the memory notepad.
If there are lines "APPLE", "ORANGE", and "GRAPE", the number will be 3.
In this case, the indexes will be from 0 to 2.
^
In order to use the memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

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
String manipulation commands
%prm
p1
p1=Variable : Variable name to which the buffer is allocated

%inst
Sets the variable specified in p1 as the buffer for the memory notepad command.
^
The variable specified in p1 is forcibly changed to a string type.
In order to use other memory notepad commands (noteget, noteadd, notedel, noteinfo), you must first set the target buffer with the notesel command.

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
String manipulation commands
%prm

%inst
The noteunsel command returns the buffer setting to the setting before the target buffer was set with the notesel command.
The noteunsel command is not always required, but by inserting it after the buffer set by the notesel command is finished being used, it prevents multiple buffer specifications by notesel from occurring.

%href
notesel





%index
strmid
Extract part of a string
%group
String manipulation functions
%prm
(p1,p2,p3)
p1=Variable name : Variable name storing the original string to extract from
p2=-1Å` : Index to start extracting from
p3=0Å` : Number of characters to extract
%inst
Returns the characters extracted from the string variable specified in p1 according to the conditions specified in p2 and p3.
p2 specifies the index to start extracting from. This is the index where the first character of the string is 0, and it increases sequentially as 1, 2, 3... Note that it does not start from 1.
p3 specifies the number of characters to extract. If you specify more than the actual number of characters stored, the actual number of characters will be extracted.
Also, if you specify -1 for p2, only the number of characters specified by p3 will be extracted from the right side of the string.
%sample
	b="ABCDEF"
	a=strmid(b,-1,3)   ; Extract 3 characters from the right
	a=strmid(b,1,3)    ; Extract 3 characters from the second character from the left
%href
getstr


%index
instr
Search for a string
%group
String manipulation functions
%prm
(p1,p2,"string")
p1=Variable name : String variable name storing the string to be searched
p2=0Å`(0) : Index to start searching from
"string"  : String to search for
%inst
Checks whether the string specified by "string" exists in the string variable specified by p1 and returns the index.
^
If the specified string is found, the index value is returned. This is the index where the first character of the string is 0, and it increases sequentially as 1, 2, 3... (similar to the index specified in the strmid command).
Note that it does not start from 1.
(If p2 is specified, the index is based on p2 as the starting point (0).)
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
String manipulation commands
%prm
"filename"
"filename" : File name to write to
%inst
Saves the contents of the memory notepad command buffer as a text file to the specified file.
Note that you must first set the target buffer with the notesel command.
The notesave command saves according to the length of the string included in the specified buffer.

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
String manipulation commands
%prm
"filename",p1
"filename" : File name to load
p1(-1)     : Upper limit of loading size
%inst
Loads the specified file into the memory notepad command buffer.
Normally, a text file is loaded and targeted for reading with the memory notepad command.
The memory notepad command buffer automatically adjusts the allocated size according to the file size, so there is no need to specify the buffer size in advance.
You can specify the maximum size of the file to load in p1.
If the specification is omitted or a negative value is specified, any size will be loaded.
It is also possible to load files other than text files.
Note that you must first set the target buffer with the notesel command.
%href
notesel
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
Get a part of a path
%group
String manipulation functions
%prm
(p1,p2)
p1=string : The original string from which to extract
p2=0~    : Information type specification
%inst
Returns a string representing the file path specified by p1, converted to the type of information specified by p2.
^p
Example:
	a="c:\\disk\\test.bmp"
	b = getpath(a,8+1)
	mes b
	Å´(Result)
	"test"
^p
Details of type specification are as follows:
^p
   Type : Content
 -----------------------------------------------------------
     0    : Copy of string (no operation)
     1    : File name without extension
     2    : Extension only (.???)
     8    : Remove directory information
    16    : Convert string to lowercase
    32    : Directory information only
^p
Multiple specifications can be made by summing the type values.
If type 16 is specified, all English strings will be converted to lowercase.
%href
getstr
instr

%index
strf
Convert formatted string
%group
String manipulation functions
%prm
("format",p1...)
"format" : Format specification string
p1       : Format specification parameter
%inst
Returns a string with an integer or real value converted to the appropriate format.
Specify a format specification string such as the following in "format".
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
For the part indicating the parameter specified after p1, make a specification following "%".
"%d" can display an integer value, "%x" can display a hexadecimal integer value, "%c" can display a character code, and "%f" can display a real value.
If multiple parameters are specified in the format, describe the parameters consecutively, separated by "," by the number of parameters.
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
	            (number of digits after the decimal point if type is f)
	I64   : Prefix indicating a 64-bit value
	type  : Type of parameter to be input
^p
The above format is used, and the description in [] can be omitted.
The characters that can be specified in type are as follows:
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
	  g	The shorter of the signed values output in format f or e that
		can express the specified value and precision
	  G	Same as format G, using uppercase letters
	  p	Outputs the address pointed to by the hexadecimal argument
	  s	String
^p
Please note that the format specification string will only recognize strings up to 1023 characters.

%href
mes
print

%index
cnvwtos
Convert unicode to normal string
%group
String manipulation functions
%prm
(p1)
p1=variable : Variable name from which to read the contents

%inst
Reads the data stored in the variable as unicode (UTF-16) and returns it as a normal string.
It can be used when handling unicode data or for data conversion with external DLLs or COM objects.

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
String manipulation command
%prm
p1,"string"
p1=variable  : Variable name to write the result
"string" : String to convert

%inst
Converts the data specified by "string" into a unicode (UTF-16) string and saves it in the variable buffer.
The variable specified by p1 must have a sufficient buffer size secured in advance as a string type for conversion.
It can be used when handling unicode data or for data conversion with external DLLs or COM objects.

%href
cnvwtos
cnvstoa
cnvatos
str
%port-
Let

%index
strtrim
Remove only the specified characters
%group
String manipulation functions
%prm
(p1,p2,p3)
p1=variable : Variable with the original string assigned
p2=0~3(0) : Specification of the position to remove
p3=0~65535(32) : Character code
%inst
Removes only the specified characters from the string stored in the variable specified by p1.
You can specify the position to remove with p2.
If p2 is omitted, it will only be removed if the specified character is at both ends of the string.
You can specify the character code to remove with p3.
If p3 is omitted, it will be a half-width space (32).
^p
Example:
	a=" ABC DEF "
	b = strtrim(a,0,' ')
	mes b
^p
Details of the values indicating the removal position specified by p2 are as follows:
^p
   Type : Content
 -----------------------------------------------------------
     0    : Remove the specified characters at both ends (default)
     1    : Remove the specified character on the left end
     2    : Remove the specified character on the right end
     3    : Remove all specified characters in the string
^p
For the character code specified by p3, you can specify a 2-byte code indicating a full-width character.
^p
Example:
	s="Å@HelloÅ@This is a full-width characterÅ@"
	zenspace="Å@"				; Full-width space
	code = wpeek(zenspace,0)		; Get the code for the full-width space
	mes strtrim(s,3,code)
^p
%href
strmid
instr

%index
split
Assign split elements from a string
%group
String manipulation command
%prm
p1,"string",p2...
p1=variable  : Variable with the original string assigned
"string" : Delimiter string
p2=variable  : Variable to which the split elements are assigned

%inst
Assigns elements split by a specified string to variables.
For example, from a string separated by "," like "12,34,56", you can extract the elements "12", "34", and "56" and assign them to separate variables.
Specify the variable name to which the original string is assigned with p1. (The variable must be a string type)
Specify the string to delimit in "string".
Specify the variable names to which the split elements are assigned after p2.
The variables to be assigned can be specified as many as you like, separated by ",".
The split elements are assigned in order from the first specified variable.
^p
Example:
	buf="12,34,56"
	split buf, ",", a, b, c
	mes a
	mes b
	mes c
^p
If there are fewer original elements than the number of specified variables, an empty string ("") is assigned to the remaining variables.
If there are more split elements than the number of specified variables, they are assigned to the array of the specified variables.
^p
Example:
	buf="12,34,56,78"
	split buf, ",", results
	repeat stat
		mes "Array("+cnt+")="+results(cnt)
	loop
^p
After execution, the number of splits is assigned to the system variable stat.

%href
getstr
csvnote
%port-
Let

%index
strrep
Replace a string
%group
String manipulation command
%prm
p1,"Search string","Replacement string"
p1=Variable name : String type variable name where the string to be searched is stored
"Search string" : String to search for
"Replacement string" : String to replace with
%inst
Replaces the string specified by "Search string" with the "Replacement string" in all contents of the string type variable specified by p1.
^
After execution, the number of times the string was replaced is assigned to the system variable stat.

%href
instr

%index
notefind
Memory notepad search
%group
String manipulation functions
%prm
("string",p1)
"string" : String to search for
p1(0)    : Search mode

%inst
Searches the current target memory notepad for lines containing a specific string.
Specify the search mode with p1. Macros corresponding to each mode are defined.
^p
   Mode : Macro            Content
 -----------------------------------------------------------
      0   : notefind_match    Search for lines that exactly match "string"
      1   : notefind_first    Search for lines starting with "string"
      2   : notefind_instr    Search for lines containing "string"
^p
The return value is the line (index) that matches the specified string from all lines of the memory notepad.
If there are no lines matching the search, -1 is returned.
^
To use the notefind function, you must first set the target buffer with the notesel command.

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
String manipulation functions
%prm
(p1)
p1=variable : Variable name from which to read the contents

%inst
Reads the data stored in the variable as an ANSI (ShiftJIS) string and returns it as a normal string.
This command only works on runtimes (hsp3utf) that treat UTF-8 as the standard string code. Please note that it will be an error with standard HSP3.

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
String manipulation command
%prm
p1,"string"
p1=variable  : Variable name to write the result
"string" : String to convert

%inst
Converts the data specified by "string" into an ANSI (ShiftJIS) string and saves it in the variable buffer.
The variable specified by p1 must have a sufficient buffer size secured in advance as a string type for conversion.
This command only works on runtimes (hsp3utf) that treat UTF-8 as the standard string code. Please note that it will be an error with standard HSP3.

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
String manipulation command
%prm
"filename",p1
"filename" : Filename where the replacement string (strmap) is recorded
p1(0)      : Replacement option

%inst
Replaces all string data included in the script in a batch.
It is necessary to prepare a file (strmap) in which the replacement string and hash code are recorded in advance.
(strmap can be generated by the hspcmp command or the hspcmp plugin command. For details, refer to the sample/new36/strex_test1.hsp sample)
The strings in the script are replaced based on the file (strmap) specified by "filename".
Specify the replacement option with p1.
If p1 is 0, only the string with the corresponding hash code is converted.
If p1 is 1, the replacement will not be executed unless all hash codes in the replacement data (strmap) match.
The result is assigned to the system variable stat after strexchange is executed.
If the system variable stat is 0, it indicates normal termination; otherwise, it indicates that there was an error.
%port-
Let
