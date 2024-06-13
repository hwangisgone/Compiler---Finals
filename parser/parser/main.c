#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "parser.h"

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("Syntax: %s <filepath>\n", argv[0]);
    return -1;
  }

  if (compile(argv[1]) == IO_ERROR) {
    printf("Can't read input file: %s\n", argv[1]);
    return -1;
  }

  return 0;
}
