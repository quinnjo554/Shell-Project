#include <cstddef>
#include <curl/curl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sched.h>
#include <sstream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
class InputHandler {
private:
  char *cmd;
  size_t n;
  const char *delim;
  std::map<std::string, std::function<char **(char **argv, int &argc)>>
      commandHandler;

  void initCommandHandler();

public:
  InputHandler();
  ~InputHandler();
  char **parseInput(char **argv, int &argc);
  char **handleCDCommand(char **argv); // dont like this but it tells the execvp
  char **handleLSCommand(char **argv, int &argc);
  void printPrompt(char *cwd, size_t size);
  char **autoComplete(char **argv);
  char **createArgvFromTokens(char **argv, int &argc);
};
