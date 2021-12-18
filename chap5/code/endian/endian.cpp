#include <stdio.h>
#include <stdlib.h>

void
byteorder() {
  union {
    short value;
    char union_bytes[sizeof(short)];
  }test;

  test.value = 0x0102; // 32bit
  if ((test.union_bytes[0] == 1) && (test.union_bytes[1] == 2)) {
    printf("big endian\n");
  } else if ((test.union_bytes[0] == 2) && (test.union_bytes[1] == 1)) {
    printf("little endian\n");
  } else {
    printf("unkown\n");
  }
}

int main() {
  byteorder();
  return EXIT_SUCCESS;
}
