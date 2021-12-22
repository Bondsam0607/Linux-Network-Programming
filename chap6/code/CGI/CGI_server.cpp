#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <libgen.h>

#define BUF_SIZE 512

int main(int argc, char * argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s ip_address, port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char * ip = argv[1];
  int port = atoi(argv[2]);
  struct sockaddr_in address;

  bzero(&address, sizeof(address));
  // initialization
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);
  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(sock, 5);
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);

  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
  if (connfd < 0) {
    fprintf(stderr, "errno is %d\n", errno);
    return EXIT_FAILURE;
  }
  else {
    close(STDOUT_FILENO); // 0: stdin, 1:stdout, 2: stderr
    dup(connfd); // the duplicated connfd will take fd 1 -- stdout, so the stdout will go to the client
    printf("abcd\n");
    close(connfd);
  }
  close(sock);
  return EXIT_SUCCESS;
}


