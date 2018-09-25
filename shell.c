#include <stdlib.h>
#include "shell.h"

int main(int argc, char** argv) {
  loop();

  return EXIT_SUCCESS;
}

void loop() {
  char* line;
  char** args;
  int status;

  do {
    printf("> ");
    line = read_line();
    args = split_line();
    status = execute(args);

    free(line);
    free(args);
  } while(status);
}
