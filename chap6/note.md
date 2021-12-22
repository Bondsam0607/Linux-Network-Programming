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


## 6.4 sendfile Function

Used to copy data directly between two file descriptors, so there'll be no copy between user space and kernel space, zero copy.

```
#include <sys/sendfile.h>
ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);
```

data flows from `in_fd` to `out_fd`, `offset` is the start point of sending, `count` sending byte count.

`in_fd` must be a real file instead of socket or pipe

## 6.5 mmap Function and munmap Function

- `mmap` is used to alloc a piece of memory space
- `munmap` is to free the memory space

we can treat the memory space as shared memory between processes or map files into it.

```
#include <sys/mman.h>
void* mmap(void* start, size_t length, int prot, int flags, int fd, off_t offset); // prot: protections
int munmap(void* start, size_t length);
```

prot:
- PROT_READ
- PROT_WRITE
- PROT_EXEC
- PROT_NONE: connot be accessed

flags:
- MAP_SHARED
- MAP_PRIVATE
- MAP_ANONYMOUS: not from fd mapping
- MAP_FIXED: must start from `start`, start must be 4096*N
- MAP_HUGETLB

## 6.6 splice Function

Used to move data between two file descriptors, zero copy

```
#include <fcntl.h>
ssize_t splice(int fd_in, loff_t* off_in, int fd_out, loff_t* off_out, size_t len, unsigned int flags);
```

data from `fd_in` to `fd_out`, one of `fd_in` and `fd_out` must be pipe

## 6.7 tee Function

`tee` function copy data between two pipe file descriptor, zero copy.

```
#include <fcntl.h>
ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags);
```

## 6.8 fcntl Function

fcntl: File Control

```
#include <fcntl.h>
int fcntl(int fd, int cmd, ...);
```

The third argument is optional, arguments of cmd

`fcntl` is usually used to set a fd non-blocking.

```
int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL); // get old fd state mark
  int new_option = old_option | O_NONBLOCK; // set non-blocking
  fcntl(fd, F_SETFL, new_option);
  return old_option; // return old option for later recovery
}
```











