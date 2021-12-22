#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/sendfile.h>
#include <libgen.h>

int main(int argc, char * argv[]) {
  if (argc <= 3) {
    fprintf(stderr, "usage: %s ip_address port_number filename\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char * ip = argv[1];
  int port = atoi(argv[2]);
  const char * filename = argv[3];

  int filefd = open(filename, O_RDONLY);
  assert(filefd > 0);
  struct stat stat_buf;
  fstat(filefd, &stat_buf);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
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
  socklen_t  client_addrlength = sizeof(client);

  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);
  assert(connfd != -1);

  sendfile(connfd, filefd, NULL, stat_buf.st_size);
  close(connfd);
  close(sock);
  return EXIT_SUCCESS;
}


