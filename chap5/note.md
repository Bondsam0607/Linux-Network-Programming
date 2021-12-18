# Linux Network Programming Fundamental API

Three Parts:
- socket address API: socket means a pair of IP address and port
- socket fundamental API: in sys/socket.h, creating socket, naming socket, listening socket...
- network info API: netdb.h, Translation between hostname and IP address, service name and port number

## 5.1 Socket Address API

### 5.1.1 Endian

- Little Endian: 23-31bit in the high address, 0-7bit in the low address (commonly used)
- Big Endian: 23-31bit in the low address, 0-7bit in the high address

Strategy:
The sender translates the data into big endian, the receiver translates the data into its endian.

So big endian is also called network endian, little endian is called host endian.

```
#include <netinet/in.h>
unsigned long int htonl(unsigned long int hostlong); host to network, unsigned long for IP address
unsigned long int ntohl(unsigned long int netlong); network to host, unsigned long for IP address
unsigned short int htons(unsigned short int hostshort); host to network, unsigned short for port number 
unsigned short int ntohs(unsigned short int netshort); network to host, unsigned short for port number
```

### 5.1.2 General Socket Address

use `sockaddr` to represent socket address

```
#include <bits/socket.h>
struct sockaddr {
  sa_family_t sa_family;
  char sa_data[14];
};
```

- `sa_family_t`: socket address family type: AF_UNIX, AF_INET, AF_INET6
- `sa_data`: storing socket address value, not big enough

**new socket addr struct**
```
#include <bits/socket.h>
struct sockaddr_storage {
  sa_family_t sa_family;
  unsigned long int __ss_align; // for alignment
  char __ss_padding[128-sizeof(__ss_align)];
};
```

### 5.1.3 Special Socket Address

**1. UNIX Local Domain**
```
#include <sys/un.h>
struct sockaddr_un {
  sa_family_t sin_family; // AF_UNIX
  char sun_path[108]; // path name
};
```

**2. INET4**
```
struct sockaddr_in {
  sa_family_t sin_family; // AF_INET
  u_int16_t sin_port; // port number
  struct in_addr sin_addr; // IPv4 address struct
};
struct in_addr {
  u_int32_t s_addr;//IPv4 address, 4bytes, net endian
};
```

**3. INET6**
```
struct sockaddr_in6 {
  sa_family_t sin6_family; // AF_INET6
  u_int16_t sin6_port; // port number;
  u_int32_t sin6_flowinfo; // flow info
  struct in6_addr sin6_addr;
  u_int32_t sin6_scope_id;
};
struct in6_addr {
  unsigned char sa_addr[16]; // 16bytes, net endian
};
```

All the special socket address should be forcely translated into general socket address.

### 5.1.4 IP Addresss Translation Function

```
// Translation between net endian binary IP address and decimal IP address
#include <arpa/inet.h>
in_addr_t inet_addr(const char* strptr); // decimal to net endian binary, NADDR_NONE when failure
int inet_aton(const char* cp, struct in_addr* inp); // same as above, save the res into inp
char * inet_ntoa(struct in_addr in); // 
```



