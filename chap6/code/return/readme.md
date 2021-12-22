This program realized a returnback server: it will return the message you send.

It defines a pipe, move the connfd into the write port of the pipe, and move the read port to connfd to realize the return function.

`splice` function is not supported in macos
