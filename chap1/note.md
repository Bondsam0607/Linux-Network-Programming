# TCP/IP Protocol Family

**This chapter will focus on ICMP, ARP and DNS**

## 1.1 TCP/IP Protocol Family Structure and main protocols

TCP/IP is a four-layer protocol system : Data-Link Layer, Network Layer, Transport Layer, Application Layer.

**Application Layer is in the user space, Other layers are in kernel space**

### 1.1.1 Data-Link Layer

ARP & RARP : translation between IP address and MAC address

ARP : IP -> MAC

RARP : MAC -> IP

### 1.1.2 Network Layer

Routing and forwarding

WAN(Wide Area Network) uses multi-layer routers to connect detributed hosts and LANs

IP : find the next hop

ICMP : check internet connection
- 差错报文 : reply network error, unreachable or redirection
- 查询报文 : look up network info

### 1.1.3 Transport Layer

Provide end to end communication for applications from two hosts

- TCP : 可靠的、面向连接、基于流  
  - 可靠 : 超时重传、数据确认
  - 基于流 : 基于流的数据没有边界，逐个字节发送
- UDP : 不可靠、无连接、基于数据报
  - 无连接 : 每次发送数据要明确接收端地址
  - 基于数据报 : 每个UDP数据报都有一个长度，接收端必须以该长度为最小单位进行接收

