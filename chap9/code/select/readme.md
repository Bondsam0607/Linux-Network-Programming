This program is to use select to listen on different kinds of file descriptors at the same time.

1. use read_fds, exception_fds(fd_set) to record event
2. use `select` to write events on read_fds and exception_fds
3. check if the connection socket matches readable fd or exception fd
4. output the received data
