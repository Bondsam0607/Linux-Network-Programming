# Detailed IP protocol

Two parts:
- IP header info
- IP Routing and Forwarding

## 2.1 Characteristics of IP service

Provide Stateless, Connectionless, Unreliable service for upper level protocol.

**Stateless**
- IP communication will not syncronize data state info, may cause 乱序、重复
- No relationship between IP datagrams.
- Simple, Efficient

Stateless is common: UDP, HTTP

**Connectionless**
- Two ends of communication will not hold each other's info
- Each datagram should have the other end's IP address

**Unreliable**
- __Best Effort__ IP protocol will not ensure the IP datagram will reach the receiver.
- When IP module of sender finds failure, it will tell the upper protocol without re-sending.


## 2.2 IPv4 Header Structure

### 2.2.1 Header Structure

- Version(4bit) : 4
- Header Length(4bit) : how many words(32bit) does the header has, header at most 60 bytes
- Total Length(16bit) : IP datagram at most <img src="https://render.githubusercontent.com/render/math?math=2^{16}-1"> bytes, usually not as large as <img src="https://render.githubusercontent.com/render/math?math=2^{16}-1"> bytes because of MTU
- Identification(16bit) : all the fragments of a single datagram will have the same identification number
- DF(1bit) : Don't fragment, if the datagram is larger than MTU, drop it and return an error message
- MF(1bit) : set as 1 until the last fragment
- TTL(8bit) : minus 1 in each hop, drop when 0, prevent entering routing loop
- Protocol(8bit) : distinguish between upper layer protocols, i.e. 1: ICMP, 6: TCP, 17: UDP
- Header Checksum(16bit) : check if broken during the transmission

## 2.3 IP Fragmenting

3 parts for fragmenting and rearranging:
- Identification
- Flags
- Offset

Only the first fragment of IP datagram has the header info of upper layer datagram

## 2.4 IP Routing

### 2.4.1 Working Process

Steps after getting an IP datagram:
1. CRC check
2. if the datagram is for the host
3. if not allow routing, drop the datagram
4. routing by looking up routing table

### 2.4.2 Routing Mechanism

1. IP地址完全匹配，则使用该路由项
2. 具有相同网路ID的网络IP地址，使用该路由项
3. 默认路由项

### 2.4.3 Updating Routing table

```
sudo route add-host 192.168.1.109 dev eth0
sudo route del-net 192,168.1.0 netmask 255.255.255.0
sudo route del default
sudo route add default gw 192,168.0.109 dev eth0
```

- BGP
- RIP
- OSPF

## 2.5 IP Forwarding

1. Check TTL, if 0, drop
2. if necessary, send ICMP to source server
3. TTL -= 1
4. parse IP header
5. if necessary, do IP fragmenting

## 2.6 Redirection

ICMP redirection segment:
- source IP address
- IP address that should be used

