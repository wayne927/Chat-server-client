This is a very basic server/client suite that allows multiple clients to connect to the server and chat IRC style. I remember writing something like this for one of my undergraduate systems courses, but now I'm doing it just for fun and as an exercise.

The code uses Unix pthreads and sockets to handle communications between one server and many clients. The server code listens for incoming connections, and creates a separate thread for each incoming client. When a client sends a message to the server, the server broadcasts that message to all clients including the source. The client's main thread takes keyboard input from the user, and another thread which receives messages from the server and prints them on screen.

Note: This program is only meant to be my own programming exercise. It is not meant to be actually used and is definitely not fool proof!

