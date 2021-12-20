#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // decimal dot <-> net endian

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <libgen.h>

#define BUFFER_SIZE 512

int main(int argc, char * argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s ip_address, port_number, send_buffer_size\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char * ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in server_address;
  bzero(&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &server_address.sin_addr);
  server_address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);
  int sendbuf = atoi(argv[3]);
  int len = sizeof(sendbuf);

  setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, sizeof(sendbuf));
  getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &sendbuf, (socklen_t*)&len);

  printf("The tcp send buffer size after setting is %d\n", sendbuf);

  int conn = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));
  if (conn == -1) {
    printf("Failed to connect\n");
    return EXIT_FAILURE;
  }
  if (conn != -1) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 'a', BUFFER_SIZE);
    send(sock, buffer, BUFFER_SIZE, 0);
  }
  close(sock);
  return EXIT_SUCCESS;
}
