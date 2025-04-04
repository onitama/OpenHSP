;
; HSP help manager HELP source file
; (Lines starting with ";" are treated as comments)
;

%type
Extended command
%ver
3.6
%note
Include hspsock.as.
%date
2014/07/04
%author
onitama
%dll
hspsock
%url
http://hsp.tv/
%port
Win


%index
sockopen
Initializes a socket and connects to it
%group
Communication control command
%prm
p1,"url",p2
p1=0〜31(0) : Socket ID number
"url"       : URL specification string
p2=0〜(0)   : Port number
%inst
Initializes a socket to make it usable.
When performing TCP/IP communication, it must always be initialized first and connected to the other server.
^
Specify the socket ID number with p1.
^
Specify the URL of the connection destination as a string in "url". This can be either a domain name such as "www.yahoo.co.jp" or an IP address such as "202.132.50.7". Do not include scheme names such as "http://" or "ftp://".
^
Specify the port number to connect to in p2. This depends on the type of server to connect to. When creating an original port, use 1024 or higher.
^
The result of executing the sockopen command is assigned to the system variable stat.
If the result is an error, it will be a value other than 0. If an error occurs during initialization, communication cannot be performed.
^
The details of the value of the system variable stat are as follows.
^p
  stat : Error details
 ----------------------------------------------------
   1   : TCP/IP is not installed on Windows
   2   : Socket initialization failed
   3   : Failed to get server name from URL
   4   : Failed to connect to the server
^p
%href
sockmake
sockclose




%index
sockclose
Disconnects the socket
%group
Communication control command
%prm
p1
p1=0〜31(0) : Socket ID number
%inst
Disconnects the connected socket.
Be sure to disconnect the initialized socket with the sockclose command.
%href
sockopen
sockmake




%index
sockput
Sends data
%group
Communication control command
%prm
"message",p1
"message"   : Message to send
p1=0〜31(0) : Socket ID number
%inst
Sends data to the socket. The string specified by "message" or the contents of the string type variable is sent as is.
^
If an error occurs during transmission, a value other than 0 is assigned to the system variable stat.
%href
sockputc
sockputb




%index
sockputc
Sends data 1 byte at a time
%group
Communication control command
%prm
p1,p2
p1=0〜255(0) : Message to send (1 byte)
p2=0〜31(0)  : Socket ID number
%inst
Sends data to the socket. Sends the number specified by p1 as 1 byte.
^
If an error occurs during transmission, a value other than 0 is assigned to the system variable stat.
%href
sockput
sockputb




%index
sockputb
Sends buffer data
%group
Communication control command
%prm
p1,p2,p3,p4
p1=Variable     : Variable name containing the transmission contents
p2=0〜(0)   : Buffer offset (in bytes)
p3=0〜(64)  : Transmission size (in bytes)
p4=0〜31(0) : Socket ID number
%inst
Sends the contents of the variable buffer specified by p1 as binary data to the socket.
The variable specified by p1 can be either a string type or a numeric type. The contents of the memory secured by the variable are sent directly.
^
You can specify the offset of the variable buffer with p2. For example, if p2 is 100, the content from the 100th byte from the beginning will be sent. If p2 is omitted, it becomes 0.
^
Specify the transmission size with p3. The transmission size must be within the size that has been secured in advance for the variable buffer. If p3 is omitted or 0, 64 (bytes) is specified.
^
After the execution of the sockputb command, the actually transmitted size (in bytes) is assigned to the system variable stat. If the contents of the system variable stat is 0, it indicates that no data has been transmitted due to a communication error or timeout.
(The contents of the variable buffer can be manipulated with the peek command, poke command, etc.)
%href
sockput
sockputc



%index
sockcheck
Checks for data arrival
%group
Communication control command
%prm
p1
p1=0〜31(0) : Socket ID number
%inst
Checks whether data that can be acquired with the sockget series command has arrived at the specified socket.
If there is receivable data, 0 is assigned to the system variable stat. If data has not arrived, the system variable stat becomes 1.
^
If a communication error or the like occurs, the system variable stat will be a value of 2 or more. If an error occurs, communication cannot be performed.
^
The details of the value of the system variable stat are as follows.
^p
  stat : Content
 ---------------------------------------------------
   0   : Received data has arrived
   1   : Received data has not arrived (timeout)
   2   : An error occurred during communication
^p
sockcheck only checks for data arrival. To actually acquire the data, it is necessary to receive it with commands such as sockget, sockgetc, sockgetb.
%href
sockget
sockgetc
sockgetb



%index
sockget
Receives data
%group
Communication control command
%prm
p1,p2,p3
p1=Variable     : Variable name where the received content is stored
p2=0〜(64)  : Reception size (in bytes)
p3=0〜31(0) : Socket ID number
%inst
Receives data from the socket and assigns the contents to the string type variable specified by p1. Specify the maximum reception size with p2.
^
If an error occurs during reception, a value other than 0 is assigned to the system variable stat.
%href
sockgetc
sockgetb




%index
sockgetc
Receives data 1 byte at a time
%group
Communication control command
%prm
p1,p2
p1=Variable     : Variable name where the received content is stored
p2=0〜31(0) : Socket ID number
%inst
Receives data 1 byte from the socket and assigns the contents to the numeric type variable specified by p1.
^
If an error occurs during reception, a value other than 0 is assigned to the system variable stat.
%href
sockget
sockgetb




%index
sockgetb
Receives data into the buffer
%group
Communication control command
%prm
p1,p2,p3,p4
p1=Variable : Variable name where the received content is stored
p2=0〜(0)   : Buffer offset (in bytes)
p3=0〜(64)  : Reception size (in bytes)
p4=0〜31(0) : Socket ID number
%inst
Receives data from the socket and stores the contents as binary data in the variable buffer specified by p1.
The variable specified by p1 can be either a string type or a numeric type. The received content is read directly into the memory secured by the variable.
^
You can specify the offset of the buffer where the data is stored with p2.
For example, if p2 is 100, the received data will be stored from the 100th byte from the beginning. If p2 is omitted, it becomes 0.
^
Specify the maximum reception size with p3.
When specifying the reception size, the variable buffer must have enough memory secured to store the specified size.
If p3 is omitted or 0, 64 (bytes) is specified.
^
After the execution of the sockgetb command, the actually received size (in bytes) is assigned to the system variable stat. If the contents of the system variable stat is 0, it indicates that no data has been received due to a communication error or timeout.
(The contents of the variable buffer can be manipulated with the peek command, poke command, etc.)
%href
sockget
sockgetc




%index
sockmake
Initializes the socket as a server
%group
Communication control command
%prm
p1,p2
p1=0〜31(0) : Socket ID number
p2=0〜(0)   : Port number
%inst
Initializes the socket to make it usable as a server.
Specify the socket ID number with p1.
Specify the port number with p2. You cannot use socket numbers that are already reserved by the system. When defining your own protocol, it is best to use a number after 1024.
^
When the sockmake command is executed, preparation as a server is completed.
After this, in order to actually connect with the client, it is necessary to wait for the client's connection to complete with the sockwait command.
^
If the result is an error, it will be a value other than 0. If an error occurs during initialization, communication cannot be performed.

The details of the value of the system variable stat are as follows.
^p
  stat : Error details
 ----------------------------------------------------
   1   : TCP/IP is not installed on Windows
   2   : Socket initialization failed
   3   : Failed to bind Socket
^p
%href
sockwait
sockopen
sockclose




%index
sockwait
Waits for client connection
%group
Communication control command
%prm
p1,p2
p1=0〜31(0) : Socket ID number
p2=0〜31(0) : Alternate socket ID number
%inst
Waits for client connection on the socket prepared as a server by the sockmake command.
The sockwait command must always be executed after the sockmake command has completed normally.
^
The sockwait command returns a value other than 0 to the system variable stat if there is no client connection or if an error occurs.
Only when 0 is assigned to the system variable stat, the connection with the client is successful, and it becomes possible to perform communication such as the sockget command and the sockput command.
^
When waiting for connection as a server, initialize the socket as a server with the sockmake command, and then loop and wait until the system variable stat becomes 0 with the sockwait command. At that time, it is necessary to put a wait or await command in the loop to leave a space.
Otherwise, tasks will not be distributed to other Windows applications.
^
If p2 is 0 or omitted, it becomes possible to communicate with the client through the socket ID number specified by p1.
If a value other than 0 is specified in p2, communication with the connected client can be performed through the alternate socket ID specified in p2.
In this case, the socket ID number specified in p1 can continue to perform server operation with the sockmake command. If you want to accept multiple client connections on the same port, use an alternate socket ID number.
^
If the result is an error, it will be a value other than 0. If an error occurs, communication cannot be performed.

The details of the value of the system variable stat are as follows.
^p
  stat : Error details
 -----------------------------------------------------
1 : No connection requests have been received from the client.
2 : Server preparation is not complete due to the sockmake command.
3 : The socket is not in a receiving state.
4 : Failed to put the socket into a receiving state.
5 : Failed to connect to the client.
