#include "inputhandler.h"
#include <cstddef>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class Shell {
private:
  int argc;
  const char *delim;
  size_t n;
  InputHandler inputHandler;

public:
  Shell() : argc(0), delim(" \n"), n(120) {}
  void executeCmd(char **argv);
  void run();
};
