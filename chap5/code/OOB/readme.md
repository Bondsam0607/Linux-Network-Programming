The program is to test the OOB data.

We use the pi as the client and the mac as the server.

We let the pi send a 3byte message, and a 3byte OOB message, and a 3byte message.

result:
```
got 5 bytes of normal data '123ab'
got 1 bytes of OOB data 'c'
got 3 bytes of normal data '123'
```

As we can see: 
- the two normal data is splited by the OOB data
- only 'c' is treated as OOB data
- the URG pointer points at the next byte of the OOB data, URG number is 3, pointer points at 3+4(SYN)

**Other situations**
- If there's no OOB receiver, the server will ignore the OOB data, but we still need two recv to get the data
```
got 5 bytes of normal data '123ab'
got 3 bytes of normal data '123'
```
- If there's no OOB sender, the server will receive twice with all the data in the first buffer, the second will get nothing
```
got 6 bytes of normal data '123123'
got 0 bytes of normal data ''
```
