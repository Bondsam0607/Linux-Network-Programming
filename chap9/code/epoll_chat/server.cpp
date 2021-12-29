#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <libgen.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535

struct client_data {
  sockaddr_in address;
  char* write_buf;
  char read_buf[BUFFER_SIZE];
};

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

void addfd(int epollfd, int fd, bool ifClosed) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  if (ifClosed) {
    event.events |= EPOLLRDHUP;
  }
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

void addout(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLOUT | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
  setnonblocking(fd);
}

void addin(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
  setnonblocking(fd);
}

void deletefd(int epollfd, int fd) {
  epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}


int main(int argc, char* argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }
  const char* ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);
  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(listenfd, 5);
  assert(ret != -1);
  
  // set kernel event table
  int epollfd = epoll_create(USER_LIMIT+1);
  assert(epollfd > 0);

  epoll_event events[USER_LIMIT+1];
  addfd(epollfd, listenfd, false);

  client_data* users = new client_data[FD_LIMIT];
  int user_count = 0;

  while (1) {
    ret = epoll_wait(epollfd, events, USER_LIMIT+1, -1);
    assert(ret >= 0);

    for (int i=0; i<ret; ++i) {
      int sockfd = events[i].data.fd;
      if ((sockfd == listenfd) && (events[i].events & EPOLLIN)) {
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        int connfd = accept(listenfd, (struct sockaddr*)&client, &client_addrlength);
        assert(connfd >= 0);

        user_count++;
        users[connfd].address = client;
        addfd(epollfd, connfd, true);
        printf("a new user, now have %d users\n", user_count);
      }
      else if (events[i].events & EPOLLRDHUP) {
        // close connection
        users[sockfd] = users[events[user_count].data.fd];
        close(sockfd);
        i--;
        user_count--;
        printf("a client left\n");
      }
      else if (events[i].events & EPOLLIN) {
        // data come from the client
        memset(users[sockfd].read_buf, '\0', BUFFER_SIZE);
        ret = recv(sockfd, users[sockfd].read_buf, BUFFER_SIZE-1, 0);
        if (ret < 0) {
          if (errno != EAGAIN) {
            close(sockfd);
            users[sockfd] = users[events[user_count].data.fd];
            i--;
            user_count--;
          }
        }
        else if (ret == 0) {}
        else {
          printf("Still good\n");
          for (int j=1; j<=user_count; ++j) {
            if (events[j].data.fd == listenfd) continue;
            addout(epollfd, sockfd);
            users[events[j].data.fd].write_buf = users[sockfd].read_buf;
          }
        }
      }
      else if(events[i].events & EPOLLOUT) {
        // read to write
        int connfd = events[i].data.fd;
        if (!users[connfd].write_buf) {
          continue;
        }
        ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
        users[connfd].write_buf = NULL;

        addin(epollfd, connfd);
      }
    }
  }
  delete[] users;
  close(listenfd);
  return EXIT_FAILURE;
}













