;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Built-in instructions
%ver
3.4
%note
ver3.4 standard instruction

%date
2009/08/01
%author
onitama
%url
http://hsp.tv/
%port
Win
Cli


%index
bcopy
Copy file
%group
File operation command
%prm
"filename1","filename2"
"filename1": Copy source file name
"filename2": Copy destination file name

%inst
Copy the file "filename1" to the filename "filename2".

%href
bload
bsave




%index
chdir
Directory move
%group
File operation command
%prm
"dirname"
"dirname": destination directory name

%inst
Change to the directory with the name specified by "dirname".
If the specified directory does not exist or is malformed, error 12 ("File not found or invalid name") will occur.
The current directory can be referenced with the system variable dir_cur.
%href
dir_cur
%port+
Let



%index
delete
File deletion
%group
File operation command
%prm
"filename"
"filename": Filename to delete

%inst
Deletes the file specified by "filename".
If the specified file does not exist or is malformed, error 12 ("File not found or invalid name") will occur.
Before executing the delete instruction, use the exist instruction to check for the existence of the file.



%index
dirlist
Get directory list
%group
File operation command
%prm
p1,"filemask",p2
p1 = variable: A string type variable that stores a list of directories
"filemask": Filemask for list
p2 = 0 to (0): Directory acquisition mode

%inst
Create a list of files in the current directory and assign them to variables.
In "filemask", specify the filemask for creating the list.
File masks are parameters that you specify in the form of wildcards, such as "*. *".
For example, the file mask "* .as" creates a list of all files with the extension as. To create a list of all files in the current directory, use "*. *".
The directory list is assigned to the character string type variable specified by p1 as a character string separated by a line feed (\\ n) for each file. This data is in a format that can be handled by memory notepad instructions.
^
When the dirlist instruction is executed, the system variable stat is assigned the number of files listed.
If there are no files, 0 is assigned to the system variable stat.
You can select the type of file to retrieve by specifying the mode of p2. The details of the mode value are as follows.
0 if the mode is omitted.
^p
  Mode: What is acquired
 ---------------------------------------------------------------------
     0: All files
     1: All files except directories
     2: All files except hidden and system attributes
     3: All files except directories, hidden attributes, and system attributes
     5: Directory only
     6: Hidden attribute / system attribute file only
     7: Directory and hidden / system attribute files only
^p

%sample
sdim a,64
dirlist a, "*. *"; Get current directory
mes a; Display of contents
stop
%href
dirlist2




%index
exist
Get file size
%group
File operation command
%prm
"filename"
"filename": The name of the file to look up for size

%inst
Checks if the file specified by "filename" exists and gets the file size. When the exist instruction is executed, the result is reflected in the system variable strsize.
^
If the file exists, its file size is assigned to strsize.
If the file does not exist, -1 will be assigned to strsize.
%port+
Let




%index
mkdir
Directory creation
%group
File operation command
%prm
"dirname"
"dirname": Name of the directory to create

%inst
Creates a directory with the name specified by "dirname".
A directory can only be created up to one level ahead.
If an error occurs during creation, you will get error 12 ("File not found or invalid name").
Be sure to use the dirlist instruction to check for the existence of folders before executing the mkdir instruction.




%index
bload
Load the file into the buffer
%group
File operation command
%prm
"filename",p1,p2,p3
"filename": Filename to load
p1 = variable: variable name
p2 = 64 ~ (-1): Loaded size (in bytes)
p3 = 0 to (-1): File offset

%inst
Reads the contents of the file into the memory buffer.
Normally, it is used for the memory buffer allocated by the sdim instruction, but it can also be used for variables with other types and array variables.
If you omit the p2 buffer size or specify a negative value, the variable buffer size is set automatically.
^
Also, after executing the bload instruction, the size of the read data is reflected in the system variable strsize.
^
If you specify an offset value for the file, the operation will be performed from the location shifted from the beginning of the file by that value. For example, if you specify an offset value of 100 with the bload instruction, data is normally read into memory from the beginning of the file, but it is read into memory from the location 100 bytes past the beginning (skipping 100 bytes).
^
This makes it possible to read only part of a large file, split it up, and so on.
^
When reading a text file, it is recommended to use the dedicated noteload instruction.
^
When reading a file encrypted by the #epack instruction with the bload instruction, note that the offset value of the file cannot be specified.

%href
noteload
bcopy
bsave
%port+
Let




%index
bsave
Save buffer to file
%group
File operation command
%prm
"filename",p1,p2,p3
"filename": Filename to save
p1 = variable: variable name
p2 = 0 to (-1): Size to save (in Byte)
p3 = 0 to (-1): File offset
%inst
Writes the contents of the memory buffer to a file.
Normally, it is used for the memory buffer allocated by the sdim instruction, but it can also be used for variables with other types and array variables.
If you omit the p2 buffer size or specify a negative value, the variable buffer size is set automatically.
^
If you specify an offset value for the file, the operation will be performed from the location shifted from the beginning of the file by that value.
If you specify an offset with the bsave instruction, you can save from a location that exceeds an arbitrary size from the beginning of the file.
(In this case, the contents of the file before that offset will not change.
Also, when offset is specified, an error will occur if the file does not exist. )
^
This makes it possible to update only part of a large file, split it up, and so on.
^
It is recommended to use the dedicated notesave instruction when saving a text file.

%href
notesave
bcopy
bload


%index
memfile
Memory stream settings
%group
File operation command
%prm
p1,p2,p3
p1 = variable: variable name to be streamed
p2 = 0 to (0): Buffer offset
p3 = 0 to (MAX): Buffer size
%inst
Set a variable with a memory space to be treated as a substitute for a file. (Memory stream function)
The variable specified by p1 will be the read source of the file specified by "MEM: file name" after that.
In p2, specify the offset (start position when reading). If 0 is specified or omitted, it is read from the beginning of the buffer.
In p3, specify the size to be read. If 0 is specified or omitted, the entire buffer of the variable is covered.
It is possible to read the image file by specifying the file image loaded in the memory as "picload" MEM: a.jpg "".
Note that it is necessary to specify a dummy file name (such as "MEM: a.wav") when the extension is judged as the file type such as the mmload command.
^
The memory stream function is used when preparing an archive or encrypted format in a unique format, reading the contents, and expanding it as an image.
It is intended to be used only in special situations.
Please note that the memory stream function does not apply to external DLLs and file names passed to extension plug-ins in principle.
(Excluding extension plug-ins that support the memory stream function of HSP3)

%href
bload
picload
%port+
Let



%index
chdpm
DPM file settings
%group
File operation command
%prm
"dpmname",p1
"dpmname": DPM file name
p1 (-1): Encryption key specification

%inst
Set the DPM file to be read.
When the chdpm instruction is executed, the file specified by "dpmname" will be treated as packed data in DPM format.
If an error occurs while initializing the DPM file, you will get error 12 ("File not found or invalid name").
^
The chdpm instruction is effective when switching between multiple DPM format files.
It is also possible to provide stronger file protection by performing different encryption for each DPM file.
Other than this command, it can be called as part of the file name, such as "picload" DPM: data.dpm: test.jpg "". (In this case, the encryption key cannot be specified.)
^
The DPM file with the encryption key must be generated directly from the script. For more information, see the sample script "mkpack.hsp".



