;
;	HSP help manager用 HELPソースファイル
;	(先頭が「;」の行はコメントとして処理されます)
;

%type
Built-in command
%ver
3.6
%note
ver3.6 standard command
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
Multimedia control command
%prm
"strings"
"strings" : MCI command string

%inst
Sends a command string to MCI (Multimedia Control Interface). You can perform MIDI playback, movie playback, etc. that are compatible with MCI.
^
Please read the Windows MCI manual for details on MCI commands. Only simple usage is introduced here.
^p
	mci "play filename"
^p
plays the file "filename". For example, "play aaaa.mid" plays the MIDI file "aaaa.mid". If the extension is avi, it plays a video; if it is wav, it plays PCM; otherwise, it plays a device registered in MCI.
^p
	mci "open filename alias abc"
^p
opens the file "filename" and aliases it to the name "abc" from now on. After this,
^p
	mci "play abc"    ; Play the device
	mci "stop abc"    ; Stop device playback
	mci "close abc"   ; Close the device
^p
can be specified.
The result of sending a command to mci is reflected in the system variable stat.
If stat is -1, it indicates that an error occurred when interpreting the mci command. Also, when a command to read the status is sent with mci, the result is reflected in stat.
MCI commands only work on the Windows platform. Please note that it cannot be used in the HSP3Dish environment.

%portinfo
In HSPLet, only the following commands are supported:

open file alias name 
play name 
seek name to position 
status name position 
stop name 
close name 

WAV/AIFF/AU/MIDI/MP3 are supported. The included jl1.0.jar is required for MP3 playback.


%index
mmplay
Media playback
%group
Multimedia control command
%prm
p1
p1=0〜(0) : Media buffer ID to play

%inst
Plays media loaded by the mmload command.
By specifying the media buffer ID in p1, you can select which of the multiple media loaded by the mmload command to play.
^
The mmplay command usually starts media playback and HSP proceeds to the next command.
However, if the loading mode 2 is specified in the mmload command, HSP will not proceed to the next command until the media playback is finished.
^
If media is already playing, executing the mmplay command further terminates the previous media playback and starts playing the newly specified media.
However, if the devices are different, such as playing PCM while playing MIDI, playback will not be stopped.
^
AVI (video) playback displays with the current position of the operated window as the upper left corner when the mmplay command is executed.
However, if +16 (play in the entire window) is specified for the mode of the mmload command, it will be played over the entire display surface of the window.
In this case, it will be scaled to the size of the window regardless of the screen size and aspect ratio, so please use it after considering the screen size and aspect ratio of the video in advance.

%href
mmload
mmstop




%index
mmload
Media file loading
%group
Multimedia control command
%prm
"filename",p1,p2
p1=0〜(0)  : Media buffer ID to be assigned
p2=0〜2(0) : Mode to be assigned

%inst
Registers a media data file.
The following formats can be read:
^p
WAV format  : Extension WAV         : Windows standard PCM audio data.
AVI format  : Extension AVI         : Windows standard video data
MID format  : Extension MID         : Standard MIDI file data.(*)
MP3 format  : Extension MP3         : MP3 format audio data
ASF format  : Extension ASF,WMV,WMA : Windows Media format audio data(*)
MPEG format : Extension MPG         : MPEG format video data(*)
Audio CD : Filename "CD : Track number" :
               (The audio track portion of the CD is targeted)(*)
(*) formats only work on the Windows platform
^p
For example, to specify track 3 of an audio CD,
^p
	mmload "CD:3",1
^p
The p1 is the media buffer ID to be assigned. This is an integer value of 0 or more that is required when playing.
To handle multiple media files, you need to assign them to separate buffer numbers.
The mmload command reads the contents into memory for audio (WAV) files of 2MB or less and makes them playable in real time. In other cases, only the file name is registered, and the actual file is read when the media is played (when mmplay is executed).
^
p2 sets the mode. The following values can be specified.
^p
Mode 0   : The specified file is played normally
Mode 1   : The specified file is played in an infinite loop
Mode 2   : Wait until the specified file finishes playing
Mode 3   : (CD only) Play from the specified track onwards
Mode+16 : (AVI only) Play in the entire target window
^p
If mode 2 is specified, when the specified file is played, HSP will not execute the next command until the playback is finished.
The mmload command only stores the data information in memory, and the performance does not start immediately.
^
Mode +16 is a mode that can only be used for AVI (video) files.
By specifying a value obtained by adding 16 to modes 0 to 2, the video is played to the full size of the window being displayed.
The video file is determined by the extension.
^
MP3 format and ASF format can only be played when the OS supports it.
(Windows Media Player 5.2 or later must be installed in early Windows 95 and Windows 98.)
^
The actual playback is performed by the mmplay command.

%href
mmplay
mmstop

%portinfo
In HSPLet, WAV/AIFF/AU/MIDI/MP3 are supported. The included jl1.0.jar is required for MP3 playback.


%index
mmstop
Stop media playback
%group
Multimedia control command
%prm
p1
p1=0〜(-1) : Media buffer ID
%inst
Stops media playback by the mmplay command.
By specifying the media buffer ID in p1, you can stop the playback of the specified media.
If p1 is omitted or a negative value is specified, all media playback is stopped.


%href
mmplay
mmload


%index
mmvol
Setting the volume
%group
Extended multimedia control command
%prm
p1,p2
p1=0〜(0) : Media buffer ID
p2(0)     : Volume value (-1000〜0)
%inst
Sets the volume of the media played by the mmplay command.
The volume value is 0 for the maximum volume and -1000 for the silent state.
This parameter is set to 0 (maximum) by default.
*Only available in the HSP3Dish environment
*Cannot be set for MIDI music files
*The volume value is closer to silence as the minus value increases, similar to the dmmvol command.
However, note that the dmmvol command is a decibel value from -10000 to 0, while the mmvol command is a value that linearly changes (auditory volume) from -1000 to 0.

%href
mmplay


%index
mmpan
Setting the panning
%group
Extended multimedia control command
%prm
p1,p2
p1=0〜(0) : Media buffer ID
p2(0)     : Panning value (-1000〜1000)
%inst
Sets the panning (stereo left/right balance) of the media played by the mmplay command.
-1000 is the leftmost localization, 1000 is the rightmost localization value. (0 is the center localization)
This parameter is set to 0 (center) by default.
*Only available in the HSP3Dish environment
*Setting is not possible for files other than WAV files
*It is not reflected when playing mp3 files on Windows
*Note that the value resolution is different from the dmmpan command

%href
mmplay


%index
mmstat
Get media status
%group
Extended multimedia control command
%prm
p1,p2,p3
p1 : Variable where the status is acquired
p2(0) : Media buffer ID
p3(0) : Acquisition mode
%inst
Acquires the status of the media played by the mmplay command and substitutes it into the p1 variable.
You can specify the acquisition mode with p3.
The values of the acquisition mode are as follows.
^p
	Mode value  Content
	------------------------------------------------------
	    0     Setting flag value
	    1     Volume value
	    2     Panning value
	    3     Playback rate (0 = original)
	    16    Playback flag (0 = stopped / 1 = playing)

*Playback flag is only available in the HSP3Dish environment
^p

%href
mmplay
