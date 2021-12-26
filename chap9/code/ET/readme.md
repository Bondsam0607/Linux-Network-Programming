This program is to test the differenct two tigger ways of epoll: LT and ET.

LT is similar to `select`, when detect new events, keep telling the working thread, working thread can handle or let the event wait.

ET will let the working thread handle the event immediately so it will reduce the `epoll_wait` calling count.

