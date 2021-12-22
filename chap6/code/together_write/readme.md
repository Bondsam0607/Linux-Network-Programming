This program is to test the write together feature

Step:
1. set up the server
2. create header_buf, file_buf, file_stat, valid
3. if the user is granted read
4. alloc a piece of memory to store the file
5. if valid, put header info to headed_buf, put file to file_buf, put it into iovec, use writev to write together
6. if no valid, put 500 into header, directly send

