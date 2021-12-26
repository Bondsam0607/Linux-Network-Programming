/* The program is to realize a non-blocking connect
 * If connect does not succeed immediately(EINPROGRESS), use select to listen on the writable event of the sockfd.
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <libgen.h>

#define BUFFER_SIZE 1024

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

int unblock_connect(const char* ip, int port, int time) {
  // return the sockfd when success, return -1 when failure
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);
  // set non blocking 
  int fdopt = setnonblocking(sockfd);
  int ret = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
  if (ret == 0) { // success
    printf("connect with the server immediately\n");
    // recover opt
    fcntl(sockfd, F_SETFL, fdopt);
    return sockfd;
  }
  else if (errno != EINPROGRESS) {
    // error
    printf("unblock connect not supported\n");
    return -1;
  }
  else {
    // EINPROGRESS
    fd_set readfds;
    fd_set writefds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    // lirten on writeable event
    FD_SET(sockfd, &writefds);

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    ret = select(sockfd+1, NULL, &writefds, NULL, &timeout);
    if (ret <= 0) {
      // wrong select
      printf("connection timeout\n");
      close(sockfd);
      return -1;
    }

    if (!FD_ISSET(sockfd, &writefds)) {
      // not set
      printf("no events on sockfd found\n");
      close(sockfd);
      return -1;
    }

    // got writeable event
    int error = 0;
    socklen_t length = sizeof(error);
    // clear the error on sockfd
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &length) < 0) {
      printf("get socket option failed\n");
      close(sockfd);
      return -1;
    }

    // if error != 0, error
    if (error != 0) {
      printf("connection failed after select with the error: %d\n", error);
      close(sockfd);
      return -1;
    }

    printf("connection ready after select with the socket: %d\n", sockfd);
    fcntl(sockfd, F_SETFL, fdopt);
    return sockfd;
  }
}


int main(int argc, char *argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s ip_address port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char * ip = argv[1];
  int port = atoi(argv[2]);

  int sockfd = unblock_connect(ip, port, 10);
  if (sockfd < 0) {
    return EXIT_FAILURE;
  }
  close(sockfd);
  return EXIT_FAILURE;
}










