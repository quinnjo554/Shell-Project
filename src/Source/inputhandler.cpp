#include "inputhandler.h"
#include "cppcommand.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <dirent.h>
#include <filesystem>
#define PRINT_BLUE(cwd) printf("\033[0;34m%s\033[0m", cwd)
#define PRINT_RED(cwd) printf("\033[0;31m%s\033[0m", cwd)
#define PRINT_GREEN(cwd) printf("\033[0;32m%s\033[0m", cwd)
#define PRINT_PURPLE(cwd) printf("\033[0;35m%s\033[0m", cwd)
#define PRINT_ORANGE(cwd) printf("\033[0;33m%s\033[0m", cwd)
#define MAX_ARGS 10         // Maximum number of arguments
#define MAX_LINE_LENGTH 250 // Maximum size of cmd
InputHandler ::InputHandler()
    : cmd(new char[MAX_LINE_LENGTH]), n(0), delim((char *)" \n") {
  initCommandHandler();
}

InputHandler::~InputHandler() {
  if (cmd != NULL) {
    delete[] cmd;
  }
}

void InputHandler::initCommandHandler() {
  commandHandler["cd"] = [this](char **argv, int &argc) -> char ** {
    return handleCDCommand(argv);
  };
  commandHandler["ls"] = [this](char **argv, int &argc) -> char ** {
    return handleLSCommand(argv, argc);
  };
  commandHandler["exit"] = [this](char **argv, int &argc) -> char ** {
    exit(-1);
    return nullptr;
  };
}

char **InputHandler::parseInput(char **argv, int &argc) {

  argc = 0;
  char cwd[1024]; // could cause issues

  printPrompt(cwd, sizeof(cwd));

  if (createArgvFromTokens(argv, argc) == nullptr) {
    return nullptr;
  }
  if (commandHandler.count(argv[0]) > 0) {
    argv = commandHandler[argv[0]](argv, argc);
    if (argv == nullptr) {
      return nullptr;
    }
  }
  if ((argv = CppCommand::execute(argv, argc)) == nullptr ||
      (argv = autoComplete(argv)) == nullptr) {
    return nullptr;
  }
  if (argv != nullptr) {
    argv[argc] = NULL; // c is wierd and last arg needs to be null terminated
  }

  return argv;
}

// CRAZY IDEA: make a compiler that takes
// /src/->src/headers/interfaces/:headers->(F)balls.h  compile that and make a
// folder structure based on that steps for making a proj command c++, rust,
// python
//  1.) check if argv is proj
//  2.) make a function that creats a directory with the name of the project
//  3.) make a function that creates boiler plate folders and makefile (c++)
//  4.) make a funciton that creates folders for rust (need more info on how
//  that works) 5.)

char **InputHandler::createArgvFromTokens(char **argv, int &argc) {

  if (getline(&cmd, &n, stdin) == -1) {
    perror("getline failed");
    exit(-1);
  }
  // make sure to exit if the line is empty
  if (strcmp(cmd, "\n") == 0) {
    return nullptr;
  }

  cmd[strcspn(cmd, "\n")] = '\0'; // Add null terminator

  char *token = strtok(cmd, delim);

  while (token && argc < MAX_ARGS) {
    argv[argc++] = token;
    token = strtok(NULL, delim);
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
      chdir(getenv("HOME"));
    }
    return nullptr; // no further processing
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
    if (argc < MAX_ARGS - 1) {
      argv[argc++] = (char *)"--color=auto";
    }
  }
  return argv;
}

char **InputHandler::autoComplete(char **argv) {

  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    std::string filename = entry.path().filename().string();
    if (argv[1] && filename.find(argv[1]) == 0) {
      strcpy(argv[1], filename.c_str()); // Copy the filename
      return argv;
    }
  }
  return argv;
}
