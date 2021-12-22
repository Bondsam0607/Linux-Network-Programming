#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <libgen.h>

#define LEN 32768

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "usage: %s filename\n", basename(argv[0]));
    return EXIT_FAILURE;
  }

  int file = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
  assert(file > 0);

  int fd_std[2];
  int ret = pipe(fd_std);
  assert(ret != -1);

  int fd_file[2];
  ret = pipe(fd_file);
  assert(ret != -1);

  // from stdin to fd_file[1]
  ret = splice(0, NULL, fd_file[1], NULL, LEN, SPLICE_F_MORE);
  assert(ret != -1);

  // from fd_file[1] to fd_std[1]
  ret = tee(fd_file[0], fd_std[1], LEN, SPLICE_F_NONBLOCK);
  assert(ret != -1);

  // from fd_file[0] to file
  ret = splice(fd_file[0], NULL, file, NULL, LEN, SPLICE_F_MORE);
  assert(ret != -1);

  // from fd_std[0] to stdout
  ret = splice(fd_std[0], NULL, 1, NULL, LEN, SPLICE_F_MORE);
  assert(ret != -1);

  close(fd_file[0]);
  close(fd_file[1]);
  close(fd_std[0]);
  close(fd_std[1]);
  close(file);
  return EXIT_SUCCESS;
}

