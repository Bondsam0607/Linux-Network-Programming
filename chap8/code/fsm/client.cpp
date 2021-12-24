#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

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
  int conn = connect(sock, (struct sockaddr*)&address, sizeof(address));

  if (conn < 0) {
	  fprintf(stderr, "errno is %d", errno);
	  return EXIT_FAILURE;
  }
  else {
	  const char* data = "GET\thttp://www.baidu.com/index.html\tHTTP/1.0\r\n\r\nHost:www.baidu.com\r\n";
	  send(sock, data, strlen(data), 0);
  }


  close(sock);
  return EXIT_SUCCESS;
}


