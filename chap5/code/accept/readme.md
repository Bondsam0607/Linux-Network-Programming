The program is to test how accept work.

We first set up a server and listen on certain port. Once we get a client, we print it out.

After the client sent connection request, we close the client so the client will send a FIN to the server and the connection will be in CLOSE_WAIT state.

But the program will still print the client info.

So it means that the `accept` function will only take a connection from the listening queue without checking its state.
