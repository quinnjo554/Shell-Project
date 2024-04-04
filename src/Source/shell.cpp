#include "shell.h"

void Shell::run() {
  while (true) { // ass prob

    int argc = 0;
    char **argv = inputHandler.parseInput(argc);
    if (argv[0]) {
      executeCmd(argv);
    }
    delete[] argv;
  }
  free(cmd);
}

void Shell::executeCmd(char **argv) {

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork failed");
    exit(-1);
  }

  if (pid == 0) {

    if (execvp(argv[0], argv) == -1) { // argv ['cd', '../', nullptr]
      fprintf(stderr, "Error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

  } else {
    if (wait(NULL) == -1) {
      perror("wait failed");
    }
  }
}
