#include "shell.h"
#define MAX_ARGS 10
void Shell::run() {
  while (true) { // not good prob

    int argc = 0;
    char **argv = new char *[MAX_ARGS]; // Set size for max arguments
    argv = inputHandler.parseInput(argv, argc);
    if (argv != nullptr) {
      executeCmd(argv);
    }

    delete[] argv;
  }
}

void Shell::executeCmd(char **argv) {
  pid_t pid = fork();

  if (pid == -1) {
    perror("fork failed");
    exit(-1);
  }
  if (pid == 0) {
    if (execvp(argv[0], argv) == -1) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    if (wait(NULL) == -1) {
      perror("wait failed");
    }
  }
}
