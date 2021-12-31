# I/O Multiplexing

Situation where using multiplexing:
- parsing multiple sockets at the same time
- parsing user input and connection at the same time
- TCP servers listening on listening socket and connection socket at the same time
- servers parsing TCP request and UDP request at the same time
- Listening on multiple ports

I/O multiplexing is blocking if we don't use multi-processing or multi-threading.


## 9.1 Select syscall

During a certain range of time, listen readable, writeable, except events of certain fds.

### 9.1.1 select API

```
#include <sys/select.h>
// return file descriptor count when success, -1 when failure
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
```

```
#include <sys/select.h>
FD_ZERO(fd_set* fdset); // clear all the bits of fdset
FD_SET(int fd, fd_set* fdset);
FD_CLR(int fd, fd_set* fdset);
int FD_ISSET(int fd, fd_set* fdset);
```

nfds: set as fd count + 1

fd_set includes an int array, each bit of each element of the array marks a file descriptor

### 9.1.2 File Descriptor Ready Condition

**Readable**
- socket kernel recv buffer byte count >= SO_RCVLOWAT
- the other side of the socket closed, return 0
- listening socket has new connection request
- there's un-handled errors on the socket

**Writable**
- socket kernel send buffer available byte count >= SO_SNDLOWAT
- socket write is closed
- non-blocking connect succeeds or fails
- un-handled error

**Except**
- OOB data

## 9.2 poll syscall

in given time, polling the file descriptors to test if there are any fds ready

```
#include <poll.h>
int poll(struct pollfd* fds, nfds_t nfds, int timeout);
```

```
struct pollfd {
  int fd;
  short event; // registered events
  short revents; // real events, modified by the kernel
};
```

1. event tells `poll` what kind of events to listen on fd
2. revents is modified by the kernel to tell the application

## 9.3 epoll syscall

### 9.3.1 kernel event table

Very different from select and poll:
- `epoll` uses a group of functions, `select` and `poll` use a single function
- `epoll` puts all the file descriptors into a kernel event table, `select` and `poll` takes the fd as argument every time, but `epoll` needs an extra file descriptor to mark the event table

```
#include <sys/epoll.h>
int epoll_create(int size); // create kernel event table
```

Function to manipulate the kernel event table:
```
#include <sys/epoll.h>
// return 0 when success
// epfd: the kernel event table
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
```

op: operator
- EPOLL_CTL_ADD, register events to the table
- EPOLL_CTL_MOD, modify registered events of the fd
- EPOLL_CTL_DEL, delete registered events of the fd

event: 
```
struct epoll_event {
  __unit32_t events; // epoll event
  epoll_data_t data; // user data
};
```

```
typedef union epoll_data {
  void * ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;
```

### 9.3.2 epoll_wait function

wait for a group of fds during a certain time range
```
#include <sys/epoll.h>
// return ready fd count when success
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout);
```

1. when `epoll_wait` detects events, copy all the ready events from kernel event table to `event`
2. so all the events in `event` are all ready

```
// poll
int ret = poll(fds, MAX_EVENT_NUMBER, -1);
for (int i=0; i<MAX_EVENT_NUMBER; ++i) {
  if (fds[i].revents & POLLIN) {
    int sockfd = fds[i].fd;
    ...
  }
}
// epoll
int ret = epoll_wait(epfd, events, MAX_EVENT_NUMBER, -1); // timeout == -1, blocking
for (int i=0; i<ret; ++i) {
  // no need to check if the events are ready
  int sockfd = events[i].data.fd;
}
```


### 9.3.3 LT and ET

- LT: default
  - when `epoll_wait` detects events happening, the application can handle the event later
  - when the application calls `epoll_wait` next time, `epoll_wait` will again inform the event
- ET: high performance, register an EPOLLET event to a fd in the epfd, activate ET mode
  - when `epoll_wait` detects the events, the application must handle the event immediately
  - decreate calling `epoll_wait` times

### 9.3.4 EPOLLONESHOT event

we want each socket handled by only one thread.

To the fds which are registered EPOLLONESHOT, the OS at most will activate only one readable, writable or exception event only once.

When the socket is handled over by a thread, the thread needs to reset the EPOLLONESHOT event of the socket.


## 9/4 Comparison between three I/O multiplexing functions

### Similiarity

- Can listen on multiple fd at the same time
- Wait for `timeout` until there are events happening on one or more fds
- return the count of ready fd, 0 for nothing happened

### select

use three `fd_set` to pass interested fds to listen:
- can only listen on three kinds of events
- have to reset fd_set before next calling `select`

The application needs to iterate through all the fds to find the ready fds: O(N)

### poll

use `pollfd` to bind event with fd, the kernel will write `revents` to tell the application:
- no need to reset `pollfd`

The application needs to iterate through all the fds to find the ready fds: O(N)

### epoll

- preserve a event table in the kernel, use `epoll_ctl` to modify the event
- `epoll_wait` directly get user-registered event from the table instead of reading from user space
- `events` only return ready event: O(1)

- `select` and `poll` use polling to scan the fd set to get the ready fd: O(N)
- `epoll` let the kernel to put the ready event to the ready queue: O(1)

`epoll` is suitable for the situation where there are connections but few activities





