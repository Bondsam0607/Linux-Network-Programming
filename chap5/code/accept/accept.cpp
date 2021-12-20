#include <sys/socket.h> //struct sockaddr, socket, bind, listen, accept
#include <netinet/in.h> //htonl, ntohl, htons, ntohs
#include <arpa/inet.h> //translation between decimal dot and net binary

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libgen.h> // for basename

int main(int argc, char * argv[]) {
  if (argc <= 2) {
    printf("usage: %s ip_address, port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }
  const char * ip = argv[1];
  int port = atoi(argv[2]);
  struct sockaddr_in address; //ipv4
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET; //ipv4
  inet_pton(AF_INET, ip, &address.sin_addr); //decimal dot to net endian
  address.sin_port = htons(port); // port num to net endian
  
  // create socket, ipv4 TCP
  int sock = socket(PF_INET, SOCK_STREAM, 0); // return sockfd
  assert(sock >= 0);

  // return 0 when success
  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  
  // return 0 when success
  ret = listen(sock, 5);
  assert(ret != -1);

  sleep(20);

  // accept client connection, put the client info into "client"
  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);

  if (connfd < 0) {
    printf("error is: %d\n", errno);
  }
  else {
    // print client IP address and port
    char remote[INET_ADDRSTRLEN];
    printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port));
    close(connfd);
  }
  close(sock);
  return EXIT_SUCCESS;
}


  



