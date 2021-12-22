#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <fcntl.h>

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

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock > 0);
  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(sock, 5);

  struct sockaddr_in client;
  socklen_t client_addrlength; 
  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
  if (connfd == -1) {
    fprintf(stderr, "errno is %d\n", errno);
    return EXIT_FAILURE;
  }
  int fd[2];
  ret = pipe(fd);
  assert(ret != -1);
  ret = splice(connfd, NULL, fd[1], NULL, 32768, SPLICE_F_MORE);
  assert(ret != -1);
  ret = splice(fd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE);
  assert(ret != -1);
  
  close(connfd);
  close(sock);
  return EXIT_SUCCESS;
}

