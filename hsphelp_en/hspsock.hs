;
;HELP source file for HSP help manager
;(Lines beginning with ";" are treated as comments)
;

%type
Extension instructions
%ver
3.4
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
Initialize and connect socket
%group
Communication control command
%prm
p1,"url",p2
p1 = 0 to 31 (0): Socket ID number
"url": URL specification string
p2 = 0 to (0): Port number
%inst
Initializes the socket so that it can be used.
When performing TCP / IP communication, it must be initialized first and connected to the other server.
^
Specify the socket ID number with p1.
^
Specify the URL of the connection destination as a character string with "url". This can be either a domain name such as "www.yahoo.co.jp" or one that directly specifies an IP address such as "202.132.50.7". Do not enter scheme names such as "http: //" or "ftp: //".
^
In p2, specify the port number to connect to. This depends on what kind of server you are connecting to. If you create your own port, make sure you use 1024 or later.
^
The result of executing the sockopen instruction is assigned to the system variable stat.
If the result is an error, it will be a non-zero value. If an error occurs during initialization, communication will not be possible.
^
The details of the value of the system variable stat are as follows.
^p
  stat: Error details
 ----------------------------------------------------
   1: TCP / IP is not installed on Windows
   2: Socket initialization failed
   3: Failed to get the server name of the URL
   4: Could not connect to the server
^p
%href
sockmake
sockclose




%index
sockclose
Disconnect socket
%group
Communication control command
%prm
p1
p1 = 0 to 31 (0): Socket ID number
%inst
Disconnect the connected socket.
Be sure to use the sockclose instruction to disconnect the initialized socket.
%href
sockopen
sockmake




%index
sockput
Send data
%group
Communication control command
%prm
"message",p1
"message": Message to send
p1 = 0 to 31 (0): Socket ID number
%inst
Sends data to the socket. The character string specified in "message" or the contents of the character string type variable are sent as they are.
^
If an error occurs during transmission, the system variable stat is assigned a non-zero value.
%href
sockputc
sockputb




%index
sockputc
Send 1 byte of data
%group
Communication control command
%prm
p1,p2
p1 = 0 to 255 (0): Message to be sent (1 byte)
p2 = 0 to 31 (0): Socket ID number
%inst
Sends data to the socket. Sends 1 byte of the numerical value specified by p1.
^
If an error occurs during transmission, the system variable stat is assigned a non-zero value.
%href
sockput
sockputb




%index
sockputb
Send buffer data
%group
Communication control command
%prm
p1,p2,p3,p4
p1 = variable: variable name where the transmission contents are stored
p2 = 0 to (0): Buffer offset (in bytes)
p3 = 0 to (64): Transmission size (byte unit)
p4 = 0 to 31 (0): Socket ID number
%inst
Sends the contents of the variable buffer specified in p1 to the socket as binary data.
The variable specified by p1 can be either a string type or a numeric type. The contents of the memory allocated by the variable are sent directly.
^
You can specify the variable buffer offset with p2. For example, if p2 is 100, the content from the 100th byte from the beginning will be sent. If p2 is omitted, it will be 0.
^
Specify the transmission size with p3. The send size must be within the size previously reserved in the variable buffer. If p3 is omitted or 0, 64 (byte) is specified.
^
After executing the sockputb instruction, the size (byte) actually sent is assigned to the system variable stat. If the content of the system variable stat is 0, it means that the system variable stat has not been sent at all due to a communication error or timeout.
(The contents of the variable buffer can be manipulated with the peek command, poke command, etc.)
%href
sockput
sockputc



%index
sockcheck
Examine the arrival of data
%group
Communication control command
%prm
p1
p1 = 0 to 31 (0): Socket ID number
%inst
Checks whether the data that can be acquired by the sockget type instruction has arrived at the specified socket.
If there is data that can be received, the system variable stat is assigned 0. If no data has arrived, the system variable stat will be 1.
^
If a communication error occurs, the system variable stat will be a value of 2 or more. If an error occurs, communication will not be possible.
^
The details of the value of the system variable stat are as follows.
^p
  stat: content
 ---------------------------------------------------
   0: Received data has arrived
   1: Received data has not arrived (timeout)
   2: An error occurred during communication
^p
sockcheck only checks the arrival of data. To get the actual data, it must be received by instructions such as sockget, sockgetc, sockgetb.
%href
sockget
sockgetc
sockgetb



%index
sockget
Receive data
%group
Communication control command
%prm
p1,p2,p3
p1 = variable: variable name where the received contents are stored
p2 = 0 to (64): Receive size (byte unit)
p3 = 0 to 31 (0): Socket ID number
%inst
Receives data from the socket and assigns the contents to the string type variable specified by p1. Specify the maximum reception size with p2.
^
If an error occurs during reception, the system variable stat is assigned a non-zero value.
%href
sockgetc
sockgetb




%index
sockgetc
Receive 1 byte of data
%group
Communication control command
%prm
p1,p2
p1 = variable: variable name where the received contents are stored
p2 = 0 to 31 (0): Socket ID number
%inst
Receives 1 byte of data from the socket and assigns the contents to the numeric variable specified by p1.
^
If an error occurs during reception, the system variable stat is assigned a non-zero value.
%href
sockget
sockgetb




%index
sockgetb
Receive data in buffer
%group
Communication control command
%prm
p1,p2,p3,p4
p1 = variable: variable name where the received contents are stored
p2 = 0 to (0): Buffer offset (in bytes)
p3 = 0 to (64): Receive size (byte unit)
p4 = 0 to 31 (0): Socket ID number
%inst
Receives data from the socket and stores the contents as binary data in the variable buffer specified by p1.
The variable specified by p1 can be either a string type or a numeric type. The received contents are read directly into the memory allocated by the variable.
^
With p2, you can specify the offset of the buffer to be stored.
For example, if p2 is 100, the received data will be stored from the 100th byte from the beginning. If p2 is omitted, it will be 0.
^
Specify the maximum reception size with p3.
When specifying the receive size, the memory that can store the specified size in the variable buffer must be secured in advance.
If p3 is omitted or 0, 64 (byte) is specified.
^
After executing the sockgetb instruction, the actually received size (byte) is assigned to the system variable stat. If the content of the system variable stat is 0, it means that the system variable stat has not been received at all due to a communication error or timeout.
(The contents of the variable buffer can be manipulated with the peek command, poke command, etc.)
%href
sockget
sockgetc




%index
sockmake
Initialize socket as server
%group
Communication control command
%prm
p1,p2
p1 = 0 to 31 (0): Socket ID number
p2 = 0 to (0): Port number
%inst
Initializes the socket so that it can be used as a server.
Specify the socket ID number with p1.
Specify the port number on p2. Socket numbers that are already reserved on the system cannot be used. If you define your own protocol, it's a good idea to use numbers after 1024.
^
When the sockmake instruction is executed, it is ready as a server.
After this, in order to actually connect to the client, it is necessary to wait until the client connection is completed with the sockwait instruction.
^
If the result is an error, it will be a non-zero value. If an error occurs during initialization, communication will not be possible.

The details of the value of the system variable stat are as follows.
^p
  stat: Error details
 ----------------------------------------------------
   1: TCP / IP is not installed on Windows
   2: Socket initialization failed
   3: Socket binding failed
^p
%href
sockwait
sockopen
sockclose




%index
sockwait
Wait for a client to arrive
%group
Communication control command
%prm
p1,p2
p1 = 0 to 31 (0): Socket ID number
p2 = 0 to 31 (0): Alternate socket ID number
%inst
The socket prepared as a server by the sockmake instruction waits for incoming clients.
The sockwait instruction must always be executed after the sockmake instruction completes successfully.
^
The sockwait instruction returns a non-zero value to the system variable stat if there is no incoming client or if an error occurs.
Only when 0 is assigned to the system variable stat will the connection with the client be successful, and communication such as sockget and sockput instructions will be possible.
^
When waiting for an incoming call as a server, initialize the socket as a server with the sockmake instruction, and then loop and wait until the system variable stat becomes 0 with the sockwait instruction. In that case, it is necessary to put a wait or await instruction in the loop to make a gap.
Otherwise, the task will not go to other Windows applications.
^
If p2 is 0 or omitted, it is possible to communicate with the client through the socket ID number specified in p1.
If you specify a value other than 0 for p2, you can communicate with the incoming client through the alternate socket ID specified in p2.
In this case, with the socket ID number specified in p1, it is possible to continue server operation with the sockmake instruction. If you want to accept multiple client incoming calls on the same port, be sure to use an alternate socket ID number.
^
If the result is an error, it will be a non-zero value. If an error occurs, communication will not be possible.

The details of the value of the system variable stat are as follows.
^p
  stat: Error details
 -----------------------------------------------------
   1: No connection request from the client
   2: The server is not ready with the sockmake instruction
   3: Socket is not in the receiving state
   4: Failed to put Socket in receive state
   5: Failed to connect to the client
^p
%href
sockmake




%index
ipget
Get the IP address of the host
%group
Communication control command
%prm
%inst
Get the IP address of the host and assign it to the system variable refstr.
^
If an error occurs while getting the IP address, 1 is assigned to the system variable stat. If the IP address can be obtained normally, the value of the system variable stat will be 0.




