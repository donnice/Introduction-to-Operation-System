# Project README file

This is **YOUR** Readme file.

## Project Description
We will manually review your file looking for:

Warm up 1: echo client/server
using the similar code provided by example to create, connect and send/recv message via Socket.
1. client: socket() -> set struct sockaddr_in -> connect() -> send() -> read() 
2. server: socket() -> bind() -> listen() -> accept() -> read() -> write()

Warm up 2: transfer file
File is transferred via chunk by chunk.
The code is still similar to warm up 1. For client side, using "fwrite" to save binary file by chunks. for server, using "fread" to read chunks from disk file.

Part 1: Getfile Client and Server
gfclient: Create and fill in a struct gfcrequest_t. A socket will be used to to connect with server, receiving response from server, checking header format, which will return ERROR if there is something wrong. Finally, receive chunks and save them by callback functions.

gfserver: Create and set gfcontext_t and gfserver_t struct, check recv request from client, set status. If works, send header firstly, then send data by chunks via callback function.


Part 2: Multireading Client and Server
gfclient_download.c: file operations should set lock to make sure the correctess and avoid race condition.

handler.c: file operations and aquire request from queue both should set lock to avoid race condition.


## Known Bugs/Issues/Limitations

The most several problem for my current code is concentrated on the memory leak in both gfclient and gfclient_download. Maybe it is because I am not familiar with C programming, which lead to the leak. But I think I have free all the mallocated variables, which shouldn't not lead to the problem...

## References

1. The Linux Programming Interface, Michael Kerrisk
2. Modern Operating Systems, Andrew S. Tanenbaum
