/* the program is to realize a server:
 * 1. listen on 5 users
 * 2. get user input and send it to the other clients
 */

#define _GNU_SOURCE 1

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <poll.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <libgen.h>

#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535
#define USERNAME_SIZE 50

struct client_data {
  char username[USERNAME_SIZE];
  sockaddr_in address;
  char* write_buf;
  char read_buf[BUFFER_SIZE];
};

int setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
  return old_option;
}

int main(int argc, char* argv[]) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s id_address port_number\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);
  int ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);
  ret = listen(listenfd, 5);
  assert(ret != -1);

  // users array
  struct client_data* users = new client_data[FD_LIMIT];

  pollfd fds[USER_LIMIT+1]; // 5 users and a listenfd

  fds[0].fd = listenfd;
  fds[0].events = POLLIN;
  fds[0].revents = 0;

  for (int i=1; i<USER_LIMIT+1; ++i) {
    fds[i].fd = -1;
    fds[i].events = 0;
    fds[i].revents = 0;
  }

  int user_count = 0;

  while (1) {
    ret = poll(fds, user_count+1, -1);
    if (ret < 0) {
      fprintf(stderr, "poll failure\n");
      return EXIT_FAILURE;
    }
    for (int i=0; i<user_count+1; ++i) {
      if (fds[i].fd == listenfd && fds[i].revents & POLLIN) {
        // get new connection
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        int connfd = accept(listenfd, (struct sockaddr*)&client, &client_addrlength);
        if (connfd < 0) {
          printf("errno is %d\n", errno);
          continue;
        }
        // too many connection
        if (user_count >= USER_LIMIT) {
          const char* info = "too many users\n";
          printf("%s", info);
          send(connfd, info, strlen(info), 0);
          close(connfd);
          continue;
        }

        // safely get new connection
        user_count++;
        users[connfd].address = client;

        memset(users[connfd].username, '\0', USERNAME_SIZE);
        ret = recv(connfd, users[connfd].username, USERNAME_SIZE-1, 0);
        assert(ret != -1);

        setnonblocking(connfd);
        fds[user_count].fd = connfd;
        fds[user_count].events = POLLIN | POLLRDHUP;
        fds[user_count].revents = 0;
        printf("a new user comes, now have %d users\n", user_count);
      }
      else if (fds[i].revents & POLLRDHUP) {
        // the client closed
        users[fds[i].fd] = users[fds[user_count].fd];
        close(fds[i].fd);
        fds[i] = fds[user_count];
        i--;
        user_count--;
        printf("a user left\n");
      }
      else if (fds[i].revents & POLLIN) {
        int connfd = fds[i].fd;
        // recv data from connfd
        memset(users[connfd].read_buf, '\0', BUFFER_SIZE);
        ret = recv(connfd, users[connfd].read_buf, BUFFER_SIZE-1, 0);
        if (ret < 0) {
          if (errno != EAGAIN) {
            close(connfd);
            users[fds[i].fd] = users[fds[user_count].fd];
            fds[i] = fds[user_count];
            i--;
            user_count--;
          }
        }
        else if (ret == 0) {}
        else {
          // get data
          for (int j=1; j<=user_count; ++j) {
            if (fds[j].fd == connfd) {
              continue;
            }
            fds[j].events |= ~POLLIN;
            fds[j].events |= POLLOUT;
            // listen on write and write data to the other users
            char tmp[1074];
            memset(tmp, '\0', 1074);
            strcat(tmp, users[connfd].username);
            strcat(tmp, " : ");
            strcat(tmp, users[connfd].read_buf);
            users[fds[j].fd].write_buf = tmp;
          }
        }
      }
      else if (fds[i].revents & POLLOUT) {
        int connfd = fds[i].fd;
        if (!users[connfd].write_buf) {
          continue;
        }
        ret = send(connfd, users[connfd].write_buf, strlen(users[connfd].write_buf), 0);
        users[connfd].write_buf = NULL;
        // continue to listen on readable
        fds[i].events |= ~POLLOUT;
        fds[i].events |= POLLIN;
      }
    }
  }
  delete[] users;
  close(listenfd);
  return EXIT_SUCCESS;
}












