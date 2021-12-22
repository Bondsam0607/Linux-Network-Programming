This program is to use `sendfile` function to directly send data in kernel space.

In together write, we need to copy the data in `file_buf`, then pu the data into iov, then use `writev` to write the data into connfd, this involves a copy.
