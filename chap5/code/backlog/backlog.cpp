#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <libgen.h> // for basename

static bool stop = false;

static void handle_term(int sig) {
  stop = true;
}

int main(int argc, char * argv[]) {
  signal(SIGTERM, handle_term);
  // error situation
  if (argc <= 3) {
    printf("usage: %s ip_address port_number backlog\n", basename(argv[0]));
    return 1;
  }
  const char * ip = argv[1]; // ip_address, decimal
  int port = atoi(argv[2]);
  int backlog = atoi(argv[3]);
  // ipv4, TCP, default protocol
  int sock = socket(PF_INET, SOCK_STREAM, 0); 
  assert(sock >= 0); // file descriptor distributed

  struct sockaddr_in address; // IPv4
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  // translation decimal to net endian
  inet_pton(AF_INET, ip, &address.sin_addr); 
  address.sin_port = htons(port); 

  int ret = bind(sock, (struct sockaddr*) &address, sizeof(address));
  assert(ret != -1);

  ret = listen(sock, backlog);
  assert(ret != -1);

  while (!stop) {
    sleep(1);
  }

  //close socket
  close(sock);
  return EXIT_SUCCESS;
}







