/* This program is to set a client and connect to the server
 * And test how send func will act when sending OOB data
 */

#include <sys/socket.h> // sock, bind, listen, accept, recv, send
#include <netinet/in.h> // hton, ntoh
#include <arpa/inet.h> // decimal dot to net endian

#include <assert.h>
#include <stdio.h>
#include <unistd.h> // close
#include <string.h>
#include <stdlib.h>

#include <libgen.h>

int main(int argc, char * argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage %s ip_address, port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);
  struct sockaddr_in server_address; // ipv4 addr structure
  bzero(&server_address, sizeof(server_address));

  server_address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &server_address.sin_addr);
  server_address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  assert(sock >= 0);

  int conn = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));

  if (conn < 0) { // fail
    fprintf(stderr, "connection failed\n");
    return EXIT_FAILURE;
  }
  else {
    const char* oob_data = "abc";
    const char* normal_data = "123";
    send(sock, normal_data, strlen(normal_data), 0);
    send(sock, oob_data, strlen(oob_data), MSG_OOB);
    send(sock, normal_data, strlen(normal_data), 0);
  }
  close(sock);
  return EXIT_SUCCESS;
}
  

