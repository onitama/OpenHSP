;
;	HSP help manageróp HELPÉ\Å[ÉXÉtÉ@ÉCÉã
;	(êÊì™Ç™Åu;ÅvÇÃçsÇÕÉRÉÅÉìÉgÇ∆ÇµÇƒèàóùÇ≥ÇÍÇ‹Ç∑)
;

%type
Built-in Command
%ver
3.7
%note
ver3.7 Standard Command

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
Copy a File
%group
File Operation Commands
%prm
"filename1","filename2"
"filename1" : Source File Name
"filename2" : Destination File Name

%inst
Copies the file "filename1" to a file named "filename2".

%href
bload
bsave




%index
chdir
Change Directory
%group
File Operation Commands
%prm
"dirname"
"dirname" : Destination Directory Name

%inst
Moves to the directory with the name specified by "dirname".
If the specified directory does not exist, or the format is incorrect, error 12 ("File not found or invalid name") will occur.
The current directory can be referenced with the system variable dir_cur.
%href
dir_cur
%port+
Let



%index
delete
Delete a File
%group
File Operation Commands
%prm
"filename"
"filename" : File Name to Delete

%inst
Deletes the file specified by "filename".
If the specified file does not exist, or the format is incorrect, error 12 ("File not found or invalid name") will occur.
Be sure to check for the existence of the file using the exist command before executing the delete command.



%index
dirlist
Get Directory List
%group
File Operation Commands
%prm
p1,"filemask",p2
p1=Variable : String variable to store the directory list
"filemask" : File mask for the list
p2=0Å`(0)   : Directory acquisition mode

%inst
Creates a list of files in the current directory and assigns it to a variable.
"filemask" specifies the file mask to use for creating the list.
The file mask is a parameter specified in a wildcard format, such as "*.*".
For example, the file mask "*.as" creates a list of all files with the as extension. "*.*" will create a list of all files in the current directory.
The directory list is assigned to the string variable specified by p1 as a string separated by line feeds (\n) for each file. This data is in a format that can be handled by the memory notepad command.
^
When the dirlist command is executed, the system variable stat is assigned the number of files for which the list was created.
If there are no files, the system variable stat is assigned 0.
By specifying the p2 mode, you can select the type of file to acquire. The details of the mode values are as follows.
If the mode is omitted, it will be 0.
^p
  Mode :  Content Acquired
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
mes a            ; Display the contents
stop
%href
dirlist2




%index
exist
Get File Size
%group
File Operation Commands
%prm
"filename"
"filename" : Name of the file to check the size of

%inst
Checks whether the file specified by "filename" exists and gets its file size. When the exist command is executed, the system variable strsize reflects the result.
^
If the file exists, its file size is assigned to strsize.
If the file does not exist, -1 is assigned to strsize.
%port+
Let




%index
mkdir
Create Directory
%group
File Operation Commands
%prm
"dirname"
"dirname" : Directory Name to Create

%inst
Creates a directory with the name specified by "dirname".
Directories can only be created one level deep.
If an error occurs during creation, error 12 ("File not found or invalid name") will occur.
Be sure to check for the existence of the folder with the dirlist command before executing the mkdir command.




%index
bload
Load File into Buffer
%group
File Operation Commands
%prm
"filename",p1,p2,p3
"filename" : File Name to Load
p1=Variable : Variable Name
p2=64Å`(-1): Size to Load (in Bytes)
p3=0Å`(-1) : File Offset

%inst
Loads the contents of a file into a memory buffer.
Normally used with memory buffers secured with the sdim command, but can also be used with variables of other types or array variables.
If the buffer size p2 is omitted or a negative value is specified, the variable buffer size is automatically set.
^
Also, after the bload command is executed, the system variable strsize reflects the size of the loaded data.
^
Specifying a file offset value shifts the location of the operation by that value from the beginning of the file. For example, if you specify an offset value of 100 with the bload command, data is normally read into memory from the beginning of the file, but data is read into memory from the location 100 bytes past the beginning (skipping 100 bytes).
^
This makes it possible to read only a part of a large file, or to process it in segments.
^
It is recommended to use the dedicated noteload command to read text files.
^
Note that when reading a file encrypted with the #epack command using the bload command, you cannot specify the file offset value.

%href
noteload
bcopy
bsave
%port+
Let




%index
bsave
Save Buffer to File
%group
File Operation Commands
%prm
"filename",p1,p2,p3
"filename" : File Name to Save
p1=Variable : Variable Name
p2=0Å`(-1) : Size to Save (in Bytes)
p3=0Å`(-1) : File Offset
%inst
Writes the contents of a memory buffer to a file.
Normally used with memory buffers secured with the sdim command, but can also be used with variables of other types or array variables.
If the buffer size p2 is omitted or a negative value is specified, the variable buffer size is automatically set.
^
Specifying a file offset value shifts the location of the operation by that value from the beginning of the file.
By specifying an offset with the bsave command, you can save from a location past an arbitrary size from the beginning of the file.
(In this case, the file contents before that offset will not change.
Also, an error will occur if the file does not exist when the offset is specified.)
^
This makes it possible to update only a part of a large file, or to process it in segments.
^
It is recommended to use the dedicated notesave command to save text files.

%href
notesave
bcopy
bload


%index
memfile
Memory Stream Setting
%group
File Operation Commands
%prm
p1,p2,p3
p1=Variable : Variable Name to be Streamed
p2=0Å`(0)   : Buffer Offset
p3=0Å`(MAX) : Buffer Size
%inst
Sets a variable that has a memory space to be treated as a substitute for a file (memory stream function).
The variable specified by p1 will be the read source of the file specified by "MEM:filename" thereafter.
p2 specifies the offset (start position when reading). If 0 is specified or omitted, it will be read from the beginning of the buffer.
p3 specifies the size to be read. If 0 is specified or omitted, the entire variable buffer will be targeted.
It is possible to read image files by specifying the file image loaded in memory as "picload "MEM:a.jpg"".
Note that when mmload command or something that judges the extension as a file type, it is necessary to specify a dummy file name (such as "MEM:a.wav").
^
The memory stream function is intended to be used only in special situations such as preparing a unique format archive or encryption format and expanding the contents as an image.
Note that the memory stream function is in principle not applied to file names passed to external DLLs or extension plugins.
(Excluding extension plugins that support the memory stream function of HSP3)

%href
bload
picload
%port+
Let



%index
chdpm
DPM File Setting
%group
File Operation Commands
%prm
"dpmname",p1,p2
"dpmname" : DPM File Name
p1(-1)    : Encryption Key Specification
p2(-1)    : Slot Number (0Å`15)

%inst
Sets the .dpm file (data file packed with multiple files) to be read.
When the chdpm command is executed, the file specified by "dpmname" is treated as data packed in DPM format from then on.
If an error occurs while initializing the DPM file, error 12 ("File not found or invalid name") will occur.
^
The slot number is the number to which the DPM file is read. You can specify a value from 0 to 15. If you specify a negative value, a new slot number is automatically assigned.
This makes it possible to refer to and target multiple DPM format files simultaneously.
Files embedded in the exe file have slot number 0.
If you specify an empty string ("") for the DPM file name, the specified slot number is discarded from the read target.
^
By performing different encryption for each DPM file, it is possible to provide more robust file protection.
For details, refer to the programming manual "File Packing and Encryption".
%href
#pack
#epack
