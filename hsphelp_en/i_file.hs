;
;	HELP source file for HSP help manager
;	(Lines starting with ";" are treated as comments)
;

%type
Built-in command
%ver
3.7
%note
ver3.7 standard command

%date
2022/04/07
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
File Operation Commands
%prm
"filename1","filename2"
"filename1" : Source filename
"filename2" : Destination filename

%inst
Copies the file "filename1" to a file named "filename2".

%href
bload
bsave




%index
chdir
Change directory
%group
File Operation Commands
%prm
"dirname"
"dirname" : Destination directory name

%inst
Moves to the directory specified by "dirname".
If the specified directory does not exist or the format is incorrect, error 12 ("File not found or invalid name") will occur.
The current directory can be referenced by the system variable dir_cur.
%href
dir_cur
%port+
Let



%index
delete
Delete file
%group
File Operation Commands
%prm
"filename"
"filename" : Filename to delete

%inst
Deletes the file specified by "filename".
If the specified file does not exist or the format is incorrect, error 12 ("File not found or invalid name") will occur.
Be sure to check the existence of the file with the exist command before executing the delete command.



%index
dirlist
Get directory list
%group
File Operation Commands
%prm
p1,"filemask",p2
p1=Variable    : String variable to store the directory list
"filemask" : File mask for the list
p2=0〜(0)  : Directory acquisition mode

%inst
Creates a list of files in the current directory and assigns it to a variable.
"filemask" specifies the file mask for creating the list.
The file mask is a parameter specified in a wildcard format such as "*.*".
For example, the file mask "*.as" creates a list of all files with the as extension. To create a list of all files in the current directory, use "*.*".
The directory list is assigned to the string variable specified by p1 as a string separated by line breaks (\n) for each file. This data is in a format that can be handled by the memnote command.
^
After the dirlist command is executed, the system variable stat is assigned the number of files for which a list was created.
If there are no files, the system variable stat is assigned 0.
By specifying the p2 mode, you can select the type of file to be acquired. The details of the mode values are as follows.
If the mode is omitted, it will be 0.
^p
  Mode :  Content to be acquired
 ---------------------------------------------------------------------
     0   :  All files
     1   :  All files except directories
     2   :  All files except hidden and system attributes
     3   :  All files except directories, hidden and system attributes
     5   :  Directories only
     6   :  Hidden and system attribute files only
     7   :  Directories and hidden/system attribute files only
^p

%sample
sdim a,64
dirlist a,"*.*"  ; Get the current directory
mes a            ; Display content
stop
%href
dirlist2




%index
exist
Get file size
%group
File Operation Commands
%prm
"filename"
"filename" : Name of the file to check the size of

%inst
Checks whether the file specified by "filename" exists and gets its file size. After the exist command is executed, the result is reflected in the system variable strsize.
^
If the file exists, its file size is assigned to strsize.
If the file does not exist, -1 is assigned to strsize.
%port+
Let




%index
mkdir
Create directory
%group
File Operation Commands
%prm
"dirname"
"dirname" : Name of the directory to create

%inst
Creates a directory with the name specified by "dirname".
Directories can only be created one level deep.
If an error occurs during creation, error 12 ("File not found or invalid name") will occur.
Be sure to check the existence of the folder with the dirlist command before executing the mkdir command.




%index
bload
Load file into buffer
%group
File Operation Commands
%prm
"filename",p1,p2,p3
"filename" : Filename to load
p1=Variable    : Variable name
p2=64〜(-1): Size to load (in bytes)
p3=0〜(-1) : File offset

%inst
Loads the contents of a file into a memory buffer.
Normally, it is used for memory buffers allocated with the sdim command, but it can also be used for variables with other types or array variables.
If you omit the buffer size of p2 or specify a negative value, the variable buffer size will be set automatically.
^
Also, after the bload command is executed, the size of the loaded data is reflected in the system variable strsize.
^
If you specify a file offset value, the operation will be performed from a location shifted from the beginning of the file by that value. For example, if you specify an offset value of 100 in the bload command, data will normally be read into memory from the beginning of the file, but it will be read into memory from a location 100 bytes past the beginning (skipping 100 bytes).
^
This makes it possible to read only a portion of a large file, or to process it in segments.
^
It is recommended to use the dedicated noteload command when reading text files.
^
Please note that if you read a file encrypted with the #epack command with the bload command, you cannot specify the file offset value.

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
File Operation Commands
%prm
"filename",p1,p2,p3
"filename" : Filename to save
p1=Variable    : Variable name
p2=0〜(-1) : Size to save (in bytes)
p3=0〜(-1) : File offset
%inst
Writes the contents of a memory buffer to a file.
Normally, it is used for memory buffers allocated with the sdim command, but it can also be used for variables with other types or array variables.
If you omit the buffer size of p2 or specify a negative value, the variable buffer size will be set automatically.
^
If you specify a file offset value, the operation will be performed from a location shifted from the beginning of the file by that value.
If you specify an offset with the bsave command, you can save from a location past an arbitrary size from the beginning of the file.
(In this case, the file contents before that offset will not change.
Also, an error will occur if the file does not exist when the offset is specified.)
^
This makes it possible to update only a portion of a large file, or to process it in segments.
^
It is recommended to use the dedicated notesave command when saving text files.

%href
notesave
bcopy
bload


%index
memfile
Memory stream settings
%group
File Operation Commands
%prm
p1,p2,p3
p1=Variable     : Variable name to be streamed
p2=0〜(0)   : Buffer offset
p3=0〜(MAX) : Buffer size
%inst
Sets a variable that has a memory space to be treated as a substitute for a file. (Memory stream function)
The variable specified by p1 will be the read source of the file specified by "MEM:filename" from now on.
p2 specifies the offset (starting position when reading). If 0 is specified or omitted, it will be read from the beginning of the buffer.
p3 specifies the size to be read. If 0 is specified or omitted, the entire variable buffer will be targeted.
It is possible to load image files by specifying the file image loaded in memory as "picload "MEM:a.jpg"".
Please note that if the extension is used as a file type to determine, such as with the mmload command, it is necessary to specify a dummy file name (such as "MEM:a.wav").
^
The memory stream function is intended to be used only in special situations, such as preparing a proprietary archive or encryption format and expanding the contents as an image by reading them.
Please note that the memory stream function is in principle not applied to file names passed to external DLLs or extension plugins.
(Excluding extension plugins that support the HSP3 memory stream function)

%href
bload
picload
%port+
Let



%index
chdpm
DPM file settings
%group
File Operation Commands
%prm
"dpmname",p1,p2
"dpmname" : DPM filename
p1(-1)    : Encryption key specification
p2(-1)    : Slot number (0〜15)

%inst
Sets the .dpm file (data file in which multiple files are packed) to be read.
After the chdpm command is executed, the file specified by "dpmname" is treated as data packed in DPM format.
If an error occurs while initializing the DPM file, error 12 ("File not found or invalid name") will occur.
^
The slot number is the number of the location to which the DPM file is loaded. You can specify a value from 0 to 15. If you specify a negative value, a new slot number will be automatically assigned.
This makes it possible to simultaneously refer to and load multiple DPM format files.
Files embedded in the exe file have slot number 0.
If you specify an empty string ("") for the DPM file name, the specified slot number will be discarded from the reading target.
^
It is also possible to perform more powerful file protection by performing different encryption for each DPM file.
For details, please refer to the Programming Manual "File Packing and Encryption".
%href
#pack
#epack
