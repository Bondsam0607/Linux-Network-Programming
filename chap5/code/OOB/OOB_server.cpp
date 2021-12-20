/* This program is to set a server and listen on a certain port
 * And test how recv func will act when receving OOB data
 */

#include <sys/socket.h> // sock, bind, listen, accept, send, recv
#include <netinet/in.h> // hton, ntoh
#include <arpa/inet.h> // decimal dot to net endian

#include <stdio.h>
#include <unistd.h> //close 
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <libgen.h>

#define BUF_SIZE 1024

int main(int argc, char * argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage %s ip_address, port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  // collect user input
  const char * ip = argv[1];
  int port = atoi(argv[2]);

  // tranlation into binary
  struct sockaddr_in address;
  bzero(&address, sizeof(address));

  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &(address.sin_addr));
  address.sin_port = htons(port);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, 5);
  assert(ret != -1);

  // get client from listening queue
  struct sockaddr_in client;
  socklen_t client_addrlength = sizeof(client);
  int connfd = accept(sock, (struct sockaddr*)&client, &client_addrlength);

  if (connfd < 0) {
    fprintf(stderr, "errno is:%d\n", errno);
    return EXIT_FAILURE;
  }
  else {
    char buffer[BUF_SIZE];
    
    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE-1, 0);
    printf("got %d bytes of normal data '%s'\n", ret, buffer);
   
     
    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE-1, MSG_OOB);
    printf("got %d bytes of OOB data '%s'\n", ret, buffer);
    

    memset(buffer, '\0', BUF_SIZE);
    ret = recv(connfd, buffer, BUF_SIZE-1, 0);
    printf("got %d bytes of normal data '%s'\n", ret, buffer);

    close(connfd);
  }
  close(sock);
  return EXIT_SUCCESS;
}

    



