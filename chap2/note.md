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
- Total Length(16bit) : IP datagram at most $2^16-1$ bytes, usually not as large as $2^16-1$ bytes because of MTU
- Identification(16bit) : all the fragments of a single datagram will have the same identification number
- DF(1bit) : Don't fragment, if the datagram is larger than MTU, drop it and return an error message
- MF(1bit) : set as 1 until the last fragment
- TTL(8bit) : minus 1 in each hop, drop when 0, prevent entering routing loop
- Protocol(8bit) : distinguish between upper layer protocols, i.e. 1: ICMP, 6: TCP, 17: UDP
- Header Checksum(16bit) : check if broken during the transmission

## 2.3 IP fragmenting


