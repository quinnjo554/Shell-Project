#include <cstddef>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_ARGS 10 // Maximum number of arguments

int main() {
  int argc = 0;
  char *argv[MAX_ARGS];
  char *cmd = NULL;
  char *delim = " \n";
  size_t n = 120;

  while (true) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("BALLS:) %s$ ", cwd);
    } else {
      perror("getcwd() error");
      return 1;
    }
    if (getline(&cmd, &n, stdin) == -1) {
      perror("getline failed");
      return -1;
    }

    cmd[strcspn(cmd, "\n")] = 0;

    if (strcmp(cmd, "exit") == 0) {
      break; // Exit the loop
    }

    char *token = strtok(cmd, delim);
    argc = 0;

    while (token && argc < MAX_ARGS) {
      argv[argc++] = token;
      token = strtok(NULL, delim);
    }

    argv[argc] = NULL; // execvp expects a NULL at the end of the array

    if (strcmp(argv[0], "cd") == 0) {
      if (chdir(argv[1]) < 0) {
        perror("cd failed");
      }
      continue;
    }

    pid_t pid;

    pid = fork();

    if (pid == -1) {
      perror("fork failed");
      return -1;
    }

    if (pid == 0) {
      if (execvp(argv[0], argv) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
      }
    } else {
      if (wait(NULL) == -1) {
        perror("wait failed");
      }
    }
  }

  free(cmd);
  return 0;
}
