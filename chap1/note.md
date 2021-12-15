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

### 1.1.4 Application Layer

- ping : application, not protocol
- telnet : protocol
- OSPF : dynamic routing update protocol
- DNS : Domain Name Service

## 1.2 Encapsulation

Each Layer will add its own infomation of the base of upper layer data to realize its function.

### TCP : TCP message segment

TCP message segment is made up of TCP header info and TCP kernel buffer(send buffer or receive buffer)

When a sender-end application writes data to a TCP connection, the TCP module in the kernel will copy the data into TCP send buffer.

### UDP : UDP datagram

When an UDP datagram is successfully sent, the data in the UDP kernel buffer will be dropped.
If it needs to re-send, it needs to again copy the data from user space.

### IP : IP datagram

may include a TCP message segment, an UDP datagram or an ICMP message segment.

### Data Link : frame

Different transmission media will have different frame type : ethernet frame, token ring frame.

### MTU : Max Transmit Unit

Largest data size a frame can hold.
If an IP datagram is longer than MTU, it might need fragmenting.

## 1.3 Demultiplexing

Each layer will parse corresponding layer header info and eventually sent the data to the application.

Frame use "type"(2 bytes) to identify the upper layer protocol type, i.e. 0x800: IP, 0x806: ARP, 0x835: RARP.

IP datagram use "protocol"(2 bytes) to identify if TCP or UDP.

TCP message segment and UDP datagram use "port number"(2 bytes) to identify upper application, i.e. 53: DNS, 80: HTTP.

## 1.5 ARP protocol

IP -> MAC : the host broadcasts an ARP request and request target machine address.

ARP request and answer are all from Ethernet Driver Program, which is different from other ethernet frames.

## 1.6 DNS










