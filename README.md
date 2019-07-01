# SocketsC
Socket Client-Server using C

The server is based on a constant message exchange.

1 - Run the server:
    Start a terminal in the server folder, then enter the command: "./server [port number i.e. 3000]"

2 - Run the client:
    Start a new terminal in the client folder, then enter the command:
    "./client [server ip i.e. localhost] [port number i.e. 3000]"

When connected, both client and server will receive connection messages.
Enter your message where it says "Tip your message" and wait until the other connection point
reply to the sent message. The server works with a message for each point at a time.

Create a file:
    Create the file by specifying the extension (i.e. arqv.txt) that you want to send to the server, and copy it to the client folder;
    
    
In the client code (client.c) change the define ARQVSEND by entering the name of the file you want to send;
    Use the client-side /write command to upload the file;
    After the file is copied to the server, the client will automatically send a hash (checksum) of the uploaded file;
    The server, in turn, will create a hash of the received file, then will compare the two hashes to verify that the file has not been corrupted in the submission.
    At the end, chacksum hashes will be displayed on the server and an error or success message will be displayed.

To end the chat, use the /close command.
