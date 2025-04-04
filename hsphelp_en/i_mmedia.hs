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
Send a command to MCI
%group
Multimedia control commands
%prm
"strings"
"strings" : MCI command string

%inst
Sends a command string to MCI (Multimedia Control Interface). It can be used to play MIDI, movies, etc. that are compatible with MCI.
^
Please read the Windows MCI documentation for details on MCI commands. Only a simple usage example is introduced here.
^p
	mci "play filename"
^p
will play the file "filename". For example, "play aaaa.mid" will play the MIDI file "aaaa.mid". If the extension is avi, it will play a movie, if it is wav, it will play PCM, and it can play other devices registered in MCI.
^p
	mci "open filename alias abc"
^p
opens the file "filename" and allows you to use the name "abc" as an alias after that. After this, you can specify the following:
^p
	mci "play abc"    ; Play the device
	mci "stop abc"    ; Stop playing the device
	mci "close abc"   ; Close the device
^p
The result of sending a command to mci is reflected in the system variable stat.
If stat is -1, it indicates that an error occurred when interpreting the mci command. Also, when sending a command to read the status with mci, the result is reflected in stat.
MCI commands only work on the Windows platform. Please note that it cannot be used in the HSP3Dish environment.

%portinfo
In HSPLet, only the following commands are supported:

open file alias name 
play name 
seek name to position 
status name position 
stop name 
close name 

WAV/AIFF/AU/MIDI/MP3 are supported.  The included jl1.0.jar is required for MP3 playback.


%index
mmplay
Media playback
%group
Multimedia control commands
%prm
p1
p1=0Å`(0) : Media buffer ID to play

%inst
Plays the media loaded by the mmload command.
By specifying the media buffer ID in p1, you can select which of the multiple media loaded by the mmload command to play.
^
The mmplay command normally starts playing the media and HSP proceeds to the next command.
However, if load mode 2 is specified in the mmload command, HSP will not proceed to the next command until the media playback is finished.
^
If media is already playing and you execute the mmplay command again, the previous media playback will be stopped before starting the newly specified media playback.
However, if you are playing a MIDI file and then play a PCM file, the playback will not be stopped.
^
AVI (movie) playback is displayed with the current position of the target window as the upper left corner when the mmplay command is executed.
However, if +16 (play on the entire window) is specified in the mmload command mode, it will be played across the entire display surface of the window.
In this case, it will be scaled to the size of the window regardless of the screen size or aspect ratio, so please consider the screen size and aspect ratio of the video in advance before using it.

%href
mmload
mmstop




%index
mmload
Media file loading
%group
Multimedia control commands
%prm
"filename",p1,p2
p1=0Å`(0)  : Media buffer ID to allocate
p2=0Å`2(0) : Allocation mode

%inst
Registers media data files.
The following formats can be loaded:
^p
WAV format : Extension WAV : Windows standard PCM audio data.
AVI format : Extension AVI : Windows standard video data
MID format : Extension MID : Standard MIDI file data.(*)
MP3 format : Extension MP3 : MP3 format audio data
ASF format : Extension ASF,WMV,WMA : Windows Media format audio data(*)
MPEG format: Extension MPG : MPEG format video data(*)
Audio CD : File name "CD : Track number" :
               (The audio track part of the CD is targeted)(*)
(*) The formats only work on the Windows platform.
^p
For example, to specify track 3 of an audio CD, use:
^p
	mmload "CD:3",1
^p
p1 is the media buffer ID to be assigned. This is an integer value of 0 or more that is required when playing.
To handle multiple media files, you need to assign them to different buffer numbers.
If the specified file is an audio (WAV) file of 2MB or less, the mmload command loads the content into memory and makes it possible to play it in real time. Otherwise, only the file name is registered, and the actual file is read when the media is played (when mmplay is executed).
^
p2 sets the mode. The following values can be specified:
^p
Mode 0   : The specified file is played normally
Mode 1   : The specified file is played in an infinite loop
Mode 2   : The specified file waits until playback is complete
Mode 3   : (CD only) Plays from the specified track onwards
Mode +16 : (AVI only) Play across the entire target window
^p
If mode 2 is specified, HSP will not execute the next command until the specified file has been played.
The mmload command only stores the data information in memory and does not start playing immediately.
^
Mode +16 is a mode that can only be used with AVI (video) files.
By specifying a value of 0 to 2 plus 16, the video will be played to fill the size of the display window.
Video files are determined by their extension.
^
MP3 and ASF formats can only be played if the OS supports them.
(On initial Windows 95 and Windows 98, Windows Media Player 5.2 or later must be installed.)
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
Multimedia control commands
%prm
p1
p1=0Å`(-1) : Media buffer ID
%inst
Stops media playback by the mmplay command.
By specifying the media buffer ID in p1, you can stop the playback of the specified media.
If p1 is omitted or a negative value is specified, all media playback will be stopped.


%href
mmplay
mmload


%index
mmvol
Volume settings
%group
Extended multimedia control commands
%prm
p1,p2
p1=0Å`(0) : Media buffer ID
p2(0)     : Volume value (-1000Å`0)
%inst
Sets the volume of media played by the mmplay command.
The volume value is 0 for maximum volume and -1000 for silent.
This parameter is set to 0 (maximum) by default.
* Can only be used in the HSP3Dish environment
* Cannot be set for MIDI music files
* The volume value is closer to silence as the negative value is larger, similar to the dmmvol command.
However, note that the dmmvol command is a decibel value from -10000 to 0, whereas the mmvol command is a linear change from -1000 to 0
(perceived loudness).

%href
mmplay


%index
mmpan
Panning settings
%group
Extended multimedia control commands
%prm
p1,p2
p1=0Å`(0) : Media buffer ID
p2(0)     : Panning value (-1000Å`1000)
%inst
Sets the panning (stereo left/right balance) of media played by the mmplay command.
-1000 is the leftmost localization, 1000 is the rightmost localization. (0 is the center localization)
This parameter is set to 0 (center) by default.
* Can only be used in the HSP3Dish environment
* Cannot be set for files other than WAV files
* Is not reflected during mp3 file music playback on Windows
* Note that the resolution of the value is different from the dmmpan command

%href
mmplay


%index
mmstat
Get media status
%group
Extended multimedia control commands
%prm
p1,p2,p3
p1 : Variable to get the status
p2(0) : Media buffer ID
p3(0) : Get mode
%inst
Gets the status of the media being played by the mmplay command and assigns it to the p1 variable.
You can specify the mode to get with p3.
The values of the acquisition mode are as follows:
^p
	Mode value  Content
	------------------------------------------------------
	    0     Setting flag value
	    1     Volume value
	    2     Panning value
	    3     Playback rate (0 = original)
	    16    Playing flag (0 = stopped / 1 = playing)

* The playing flag can only be used in the HSP3Dish environment
^p

%href
mmplay
