# High-level I/O Functions

Three parts:
- pipe, dup/dup2
- readv/writev, sendfile, mmap/mummap, splice, tee
- fcntl

## 6.1 pipe

create a pipeline for inter-process communication

```
#include <unistd.h>
int pipe(int fd[2]);
```

If success, return 0 and put the fds into the array.

- fd[0]: read port
- fd[1]: write port

The pair of fds are blocking:
- if we call `read` to read the pipe, the pipe will be blocked until there's data
- if we call `write` to write the pipe, the pipe will be blocked until there's vacancy to write

- If fd[1] reference decreases to 0, read f[0] will return EOF
- If fd[0] reference decreases to 0, write f[1] will fail

The data in pipe is byte stream with a maximum of 65536 bytes.

Function to build bi-directional pipe:
```
#include <sys/types.h>
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int fd[2]);
```

domain can only be AF_UNIX, fd[0] and fd[1] can both read and write.

## 6.2 dup Function and dup2 Function

Used in redirection.

```
#include <unistd.h>
int dup(int file_descriptor);
int dup2(int file_descriptor_one, int file_descriptor_two);
```

- `dup` will copy the file_descriptor return the new fd(the smallest fd number)
- `dup2` will return a fd >= file_descriptor_two

A CGI server can accept client connection, call corresponding CGI programs, let the programs print output into stdout, redirect the stdout into the sockfd, return it to the client.

## 6.3 readv Function and writev Function

- `readv` will read data from fd to scattered memory: scatter read
- `writev` will put scattered memory data into fd: together write

```
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec* vector, int count);
ssize_t writev(int fd, cont struct iovec* vector, int count);
```

return byte count when success, -1 when failure










