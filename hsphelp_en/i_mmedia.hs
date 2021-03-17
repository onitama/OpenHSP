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
2019/08/01
%author
onitama
%url
http://hsp.tv/
%port
Win
Let



%index
mci
Send command to MCI
%group
Multimedia control instructions
%prm
"strings"
"strings": MCI command strings

%inst
Sends a command string to the MCI (Multimedia Control Interface). MIDI playback and movie playback compatible with MCI can be performed.
^
For more information on MCI commands, read the Windows MCI documentation. Here, I will introduce only the simple usage.
^p
	mci "play filename"
^p
Plays the "filename" file. For example, "play aaaa.mid" will play a MIDI file called "aaaa.mid". If the extension is avi, you can play videos, if it is wav, you can play PCM, and other devices registered in MCI.
^p
	mci "open filename alias abc"
^p
Now you can open the file with "filename" and rename it "abc" from now on. After this
^p
mci "play abc"; device play
mci "stop abc"; Stop device playback
mci "close abc"; device close
^p
You will be able to specify such as.
The result of sending an instruction to mci is reflected in the system variable stat.
If stat is -1, it indicates that there is an error when interpreting the mci instruction. Also, if you send a command to read the status with mci, the result will be reflected in stat.
Commands to MCI only work on Windows platforms. Please note that it cannot be used in the HSP3 Dish environment.

%portinfo
At HSPLet, only the following instructions are supported.

open file alias name 
play name 
seek name to position 
status name position 
stop name 
close name 

It supports WAV / AIFF / AU / MIDI / MP3. The included jl1.0.jar is required to play MP3s.


%index
mmplay
Sound playback
%group
Multimedia control instructions
%prm
p1
p1 = 0 to (0): Media buffer ID to play

%inst
Plays the media loaded by the mmload command.
By specifying the media buffer ID with p1, you can select which of the multiple media loaded by the mmload command to play.
^
The mmplay instruction normally starts playing the sound and the HSP advances to the next instruction.
However, if load mode 2 is specified in the mmload command, the next command will not proceed until the sound playback is completed.
^
If you execute the mmplay command while the sound is already playing, the previous sound playback will end and then the newly specified sound will start playing.
However, for different devices, such as playing PCM while playing MIDI, playback will not stop.
^
For AVI (video) playback, the current position of the window that is the operation destination when the mmplay command is executed is displayed as the upper left.
However, if the mode of the mmload instruction is set to +16 (play in the entire window), it will play to fill the display surface of the window.
In this case, it will be scaled to the window size regardless of the screen size and aspect ratio, so please consider the screen size and aspect ratio of the video in advance before using it.

%href
mmload
mmstop




%index
mmload
Read sound file
%group
Multimedia control instructions
%prm
"filename",p1,p2
p1 = 0 to (0): Media buffer ID to allocate
p2 = 0 to 2 (0): Assign mode

%inst
Register the media data file.
The formats that can be read are as follows.
^p
WAV format: Extension WAV: Windows standard PCM audio data.
AVI format: Extension AVI: Windows standard video data
MID format: Extension MID: Standard MIDI file data. (*)
MP3 format: Extension MP3: MP3 format audio data
ASF format: Extensions ASF, WMV, WMA: Windows Media format Audio data (*)
MPEG format: Extension MPG: MPEG format video data (*)
Audio CD: File name "CD: Track number":
               (The audio track part of the CD is the target) (*)
The (*) format only works on Windows platforms
^p
For example, if you want to specify track 3 of an audio CD,
^p
	mmload "CD:3",1
^p
To do.
p1 is the media buffer ID to allocate. This is an integer value greater than or equal to 0 required for playback.
To handle multiple media files, you need to assign them to different buffer numbers.
If the specified file is an audio (WAV) file of 2MB or less, the mmload command reads the contents into memory and makes it playable in real time. In other cases, only the file name is registered, and the actual reading of the file will be performed during media playback (when mmplay is executed).
^
p2 sets the mode. You can specify the following values:
^p
Mode 0: The specified file will play normally
Mode 1: The specified file is played in an infinite loop.
Mode 2: The specified file waits until the end of playback
Mode 3: (CD only) Plays after the specified track
Mode +16: (AVI only) Play in the entire target window
^p
If mode 2 is specified, when the specified file is played, HSP will not execute the next instruction until the playback is completed.
The mmload instruction only stores the data information in memory, and the performance does not start immediately.
^
Mode +16 is a mode that can only be used with AVI (video) files.
By specifying a value obtained by adding 16 to modes 0 and 2, the video will be played to the full size of the window to be displayed.
The video file is judged by the extension.
^
MP3 format and ASF format can be played only when the OS supports them.
(In early Windows 95 and Windows 98, Windows Media Player 5.2 or later must be installed.)
^
The actual playback is done by the mmplay command.

%href
mmplay
mmstop
%portinfo
Supports WAV / AIFF / AU / MIDI / MP3 when HSPLet. The included jl1.0.jar is required to play MP3s.



%index
mmstop
Sound stop
%group
Multimedia control instructions
%prm
p1
p1 = 0 to (-1): Media buffer ID
%inst
Stops media playback by the mmplay command.
You can stop the playback of the specified sound by specifying the media buffer ID with p1.
If p1 is omitted or a negative value is specified, all sound playback will be stopped.


%href
mmplay
mmload


%index
mmvol
Volume setting
%group
Extended multimedia control instructions
%prm
p1,p2
p1 = 0 to (0): Media buffer ID
p2 (0): Volume value (-1000 to 0)
%inst
Sets the volume of the media played by the mmplay command.
The volume value is 0 for maximum volume and -1000 for silence.
This parameter is set to 0 (maximum) by default.
* Available only in HSP3 Dish environment
* As with the dmmvol command, the larger the negative value, the closer the volume value becomes to silence.
However, the dmmvol instruction has a decibel value from -10000 to 0, while the mmvol instruction has a linear change from -1000 to 0.
Note that it is a value that does (audible volume).

%href
mmplay


%index
mmpan
Panning settings
%group
Extended multimedia control instructions
%prm
p1,p2
p1 = 0 to (0): Media buffer ID
p2 (0): Panning value (-100 to 100: 00)
%inst
Sets the panning (stereo left / right balance) of the media played by the mmplay command.
-1000 is the leftmost localization value and 1000 is the rightmost localization value. (0 is the central localization)
This parameter is set to 0 (center) by default.
* Available only in HSP3 Dish environment
* Not reflected when playing music with mp3 files on Windows
* Please note that the resolution of the value is different from that of the dmmpan instruction.

%href
mmplay


%index
mmstat
Get media status
%group
Extended multimedia control instructions
%prm
p1,p2,p3
p1: Variable for which state is acquired
p2 (0): Media buffer ID
p3 (0): Acquisition mode
%inst
Gets the state of the media played by the mmplay instruction and assigns it to a variable in p1.
You can specify the mode to acquire with p3.
The values of the acquisition mode are as follows.
^p
Mode value Contents
	------------------------------------------------------
0 Setting flag value
1 Volume value
2 Panning value
3 Playback rate (0 = original)
16 Playing flag (0 = Stopped / 1 = Playing)

* The playing flag can only be used in the HSP3 Dish environment.
^p

%href
mmplay


