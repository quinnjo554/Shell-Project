#include "inputhandler.h"
#include <cstddef>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#define PRINT_BLUE(cwd) printf("\033[0;34m%s\033[0m", cwd)
#define PRINT_RED(cwd) printf("\033[0;31m%s\033[0m", cwd)
#define PRINT_GREEN(cwd) printf("\033[0;32m%s\033[0m", cwd)
#define PRINT_PURPLE(cwd) printf("\033[0;35m%s\033[0m", cwd)
#define PRINT_ORANGE(cwd) printf("\033[0;33m%s\033[0m", cwd)
#define MAX_ARGS 10         // Maximum number of arguments
#define MAX_LINE_LENGTH 250 // Maximum size of cmd

InputHandler::~InputHandler() {
  free(cmd); // Assuming cmd is allocated using malloc in the constructor
}

char **InputHandler::parseInput(int &argc) {
  argc = 0;
  char **argv = new char *[MAX_ARGS]; // Set size for max arguments
  char cwd[1024];
  size_t n = 0; // Declare and initialize n for getline
  char *cmd = (char *)malloc(MAX_LINE_LENGTH); // Allocate memory for cmd
  printPrompt(cwd, sizeof(cwd));

  if (getline(&cmd, &n, stdin) == -1) {
    perror("getline failed");
    exit(-1);
  }

  cmd[strcspn(cmd, "\n")] = '\0'; // Add null terminator

  char *token = strtok(cmd, delim);

  while (token && argc < MAX_ARGS) {
    argv[argc++] = token;
    token = strtok(NULL, delim);

    if (strcmp(argv[0], "exit") == 0) {
      exit(-1);
    }
  }
  autoComplete(argv);
  argv = handleLSCommand(argv, argc);
  // spotify command here.
  //  this returns a null pointer potensially so has to be called last
  //  put in a function together and have it ruten null if neither is called or
  //  something

  argv = handleCDCommand(argv); // needs to be last function

  if (argv != nullptr) {
    argv[argc] = NULL; // c is wierd and last arg needs to be null terminated
  }

  return argv;
}

char **InputHandler::handleCDCommand(char **argv) {

  if (argv[0] && strcmp(argv[0], "cd") == 0) {
    if (argv[1]) {
      if (chdir(argv[1]) != 0) {
        perror("chdir failed");
      }
    } else {
      chdir(getenv("HOME")); // Change to home directory if no argument
    }
    return nullptr; // No further processing
  }
  return argv; // need to run execvp
}

void InputHandler::printPrompt(char *cwd, size_t size) {
  if (getcwd(cwd, size) != NULL) {
    PRINT_ORANGE("DEV-SHELL");
    printf(":");
    PRINT_PURPLE(cwd);
    printf("$ ");
  } else {
    perror("getcwd() error");
    exit(1);
  }
}

char **InputHandler::handleLSCommand(char **argv, int &argc) {
  if (argv[0] && strcmp(argv[0], "ls") == 0) {
    // Add "--color=auto" as the second argument (index 1)

    setenv(
        "LS_COLORS",
        "di=1;35:ln=36:so=35:pi=30;44:ex=1;33:bd=46;34:cd=43;34:su=41;30:sg=" // set LS_COLORS env so the colors are cool
        "46;30:tw=42;30:ow=43;30",
        1);
    argv[argc++] = (char *)"--color=auto"; // Cast for compatibility
  }

  return argv;
}

char **InputHandler::autoComplete(char **argv) {

  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    std::string filename = entry.path().filename().string();
    if (argv[1] && filename.find(argv[1]) == 0) {
      argv[1] = strdup(
          filename.c_str()); // duplicate the string to avoid dangling pointer
      return argv;
    }
  }
  return argv;
}
size_t InputHandler::WriteCallback(void *contents, size_t size, size_t nmemb,
                                   std::string *userp) {
  userp->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string spotifyAPIRequest(const std::string &url,
                              const std::string &token) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();

  if (curl) {
    struct curl_slist *headers = NULL;
    headers =
        curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return readBuffer;
}
