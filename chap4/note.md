# Visiting the Web Server on the Internet

We will talk about the case where the client use HTTP protocol to communicate with the server via the proxy server.

## 4.2 Proxy Server

### 4.2.1 Working Principle of Proxy Server

Three kinds of Proxy servers:
- Forward Proxy: the client should set proxy server address, let the proxy server visit the target resource
- Reverse Proxy: set at the server, forward the request to the inner servers, the proxy server looks like a real server
- Transparent Proxy: set at gateway, the proxy is transparent to users, can be used as cache

## 4.4 Visiting DNS Server

Steps:
1. Get DNS server IP: read /etc/resolv.conf
2. Pass the control to UDP module in the kernel
3. UDP module packs the DNS segment into UDP datagram, adds source port and target port
4. UDP module calls IP module
5. IP module packs the UDP datagram into IP datagram, adds source IP and target IP
6. IP module looks up routing table to decide how to send the IP datagram: default
7. IP datagram is sending to the router for forwarding
8. Make an ARP broadcasting to look up the MAC address of the router
9. Ethenet Driver will pack IP datagram into Ethenet Frame and send to the router

The header of Frame will be changing during the forwarding. The header of IP datagram will not change.

## 4.5 Local Name Looking Up

If we use hostname to visit LAN machine, we can get the IP address by looking up local static file: /etc/hosts

If the hostname is not found in the file, it will visit DNS server.

## 4.6 HTTP Communication


