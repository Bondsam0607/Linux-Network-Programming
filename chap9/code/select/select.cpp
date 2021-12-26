/* this program is to use select to parse OOB data and normal data at the same time
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>

#include <libgen.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage %s: ip_address port_number\n", basename(argv[0]));
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
  assert(ret != -1);

  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int conn = accept(sock, (struct sockaddr*)&client, &client_addrlength);
  if (conn < 0) {
    fprintf(stderr, "errno is %d\n", errno);
    close(sock);
    return EXIT_FAILURE;
  }

  char buffer[1024];
  fd_set read_fds;
  fd_set exception_fds;
  // initialization
  FD_ZERO(&read_fds);
  FD_ZERO(&exception_fds);
  while (true) {
    FD_SET(conn, &read_fds);
    FD_SET(conn, &exception_fds);

    ret = select(conn+1, &read_fds, NULL, &exception_fds, NULL);
    if (ret < 0) {
      fprintf(stderr, "selection failure\n");
      return EXIT_FAILURE;
    }

    if (FD_ISSET(conn, &read_fds)) { // if readable
      memset(buffer, '\0', sizeof(buffer));
      ret = recv(conn, buffer, sizeof(buffer)-1, 0);
      if (ret <= 0) {
        break;
      }
      printf("get %d bytes of normal data: '%s'\n", ret, buffer);
    }
    if (FD_ISSET(conn, &exception_fds)) {
      memset(buffer, '\0', sizeof(buffer));
      ret = recv(conn, buffer, sizeof(buffer)-1, MSG_OOB);
      if (ret <= 0) {
        break;
      }
      printf("get %d bytes of OOB data: '%s'\n", ret, buffer);
    }
  }
  close(conn);
  close(sock);
  return EXIT_SUCCESS;
}



