#include <cstddef>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
class InputHandler {
  private:
    char* cmd;
    size_t n;
   const char* delim;
  public:
    InputHandler():cmd(NULL),n(120),delim(" \n"){}
    ~InputHandler();
    char** parseInput(int& argc);
   char** handleCDCommand(char **argv); //dont like this but it tells the execvp command that it shoundt run; 
    void printPrompt(char* cwd, size_t size);
};
