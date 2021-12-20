The program is to see the effect of `backlog` on the connection.

We firstly set up a server and let it listen on some port, with backlog.

The program will create a listening queue. 

We set the backlog as 5, but have 6 connection established.

The server can allow a bit more connections than backlog.

Other connections will be in state SYN_REVD.
