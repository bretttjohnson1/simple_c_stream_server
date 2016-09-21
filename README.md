A simple C stream server

client usage

./client <server_adress>

server usage

./server <filename>

The stream server transfers a file byte by byte to the client. The client then writes the file to stdout.

To stream an mp3 file for example

./server thing.mp3

./client 0.0.0.0 | mpg123 -

To print text

./server test.txt

./client 0.0.0.0


*note 0.0.0.0 is the ip of the server
