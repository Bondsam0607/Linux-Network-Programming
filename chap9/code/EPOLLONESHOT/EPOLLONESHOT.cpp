#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

struct fds {
  int epollfd;
  int sockfd;
};

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

void addfd(int epollfd, int fd, bool oneshot) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET;
  if (oneshot) {
    event.events |= EPOLLONESHOT;
  }
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
  setnonblocking(fd);
}

void reset_oneshot(int epollfd, int fd) {
  epoll_event event;
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

void* worker(void* arg) { // working thread
  int sockfd = ((fds*)arg)->sockfd;
  int epollfd = ((fds*)arg)->epollfd;
  printf("start new thread to receive data on fd:%d\n", sockfd);
  char buf[BUFFER_SIZE];
  memest(buf, '\0', BUFFER_SIZE);
  // read until EAGAIN
  while (1) {
    int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
    if (ret == 0) {
      close(sockfd);
      printf("foreign closing connection\n");
      break;
    }
    else if (ret < 0) { // ERROR
      if (errno == EAGAIN) {
         // nothing to read
         reset_oneshot(epollfd, sockfd);
         printf("read after\n");
         break;
      }
    }
    else {
      // got data
      printf("get content: '%s'\n", buf);
      sleep(5);
    }
  }
  printf("end thread: received data on fd:%d\n", sockfd);
}

int main(int argc, char *argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char * ip = argv[1];
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

  epoll_event events[MAX_EVENT_NUMBER];
  int epollfd = epoll_create(5); // create kernel event table
  assert(epollfd >= 0);

  // listening socket cannot register EPOLLONESHOT event
  addfd(epollfd, listenfd, false);

  while (1) {
    int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
    if (ret < 0) {
      printf("epoll failure\n");
      break;
    }
    for(int i=0; i<ret; ++i) {
      int sockfd = events[i].data.fd;
      // get ready socket
      if (sockfd == listenfd) {
        struct sockaddr_in client_address;
        socklen_t client_addrlength = sizeof(client_address);
        int connfd = accept(listenfd, (struct sockaddr*)&client_address, &client_addrlength);
        // add EPOLLONESHOT to every connection fd
        addfd(epollfd, connfd, true);
      }
      else if (events[i].events & EPOLLIN) {
        pthread_t thread;
        fds fds_for_new_worker;
        fds_for_new_worker.epollfd = epollfd;
        fds_for_new_worker.sockfd = sockfd;
        // new working thread
        pthread_create(&thread, NULL, worker, (void*)&fd_for_new_worker);
      }
      else {
        printf("something else happened\n");
      }
    }
  }
  close(listenfd);
  return EXIT_SUCCESS;
}







