#include <cstddef>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
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

public:
  InputHandler() : cmd(NULL), n(120), delim((char *)" \n") {}
  ~InputHandler();
  char **parseInput(int &argc);
  char **handleCDCommand(char **argv); // dont like this but it tells the execvp
  char **handleLSCommand(char **argv, int &argc);
  void printPrompt(char *cwd, size_t size);
  char **autoComplete(char **argv);
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              std::string *userp);
  std::string spotifyAPIPlaySongRequest(const std::string &url,
                                        const std::string &token);
  void printSpotifyRequest();
};
