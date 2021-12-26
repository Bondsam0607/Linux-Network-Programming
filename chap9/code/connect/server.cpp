#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

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

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  int ret = bind(sockfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(sockfd, 5);
  assert(ret != -1);

  sleep(20);
  close(sockfd);
  return EXIT_SUCCESS;
}

