#include "cppcommand.h"
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
CppCommand::~CppCommand() {
  for (int i = 0; i < argc; ++i) {
    delete[] argv[i]; // Free each string
  }
  delete[] argv; // Free the array
}

CppCommand::CppCommand(char **argv, int argc) : argc(argc) {

  this->argv = new char *[argc + 1]; // Allocate memory for the copy
  for (int i = 0; i < argc; ++i) {
    this->argv[i] =
        new char[strlen(argv[i]) + 1]; // Allocate memory for each string
    strcpy(this->argv[i], argv[i]);    // Copy each string
  }
  this->argv[argc] = NULL; // Null-terminate the array
}

CommandType CppCommand::validateCmd() {
  if (strcmp(this->argv[0], "cproj") == 0) {
    if (argc == 1) {
      return CommandType::INVALID;
    } else if (argc == 3 && strcmp(this->argv[1], "-m") == 0) {
      return CommandType::NAMED_PROJECT;
    }
  }
  return CommandType::INVALID;
}

void CppCommand::execute() {
  bool made = generateDirectory();
  if (made) {
    generateMakeFile();
  }
}
void CppCommand::generateMakeFile() {
  char *make = (char *)"Makefile";
  touch(make);
}

bool CppCommand::touch(const char *filename) {
  std::ofstream file;
  file.open(filename);
  if (!file) {
    std::cerr << "Unable to open file example.txt";
    return false;
  }
  file.close();
  return true; // Success
}

bool CppCommand::generateDirectory() {
  // proper command should look like // cproj -m "Name"
  CommandType cmdValidation = validateCmd();
  std::string cmdType = CommandTypeToString(cmdValidation);
  if (cmdType != "NAMED_PROJECT") {
    return false;
  }
  if (!argv || !argv[2]) {
    return false;
  }
  struct stat st = {0};
  if (stat(argv[2], &st) != -1) {
    return false;
  }
  mkdir(argv[2], 0755); // owner has permissions
  chdir(argv[2]);
  return true;
}

std::string CppCommand::CommandTypeToString(CommandType commandType) {
  switch (commandType) {
  case CommandType::DEFAULT:
    return "DEFAULT";
  case CommandType::NAMED_PROJECT:
    return "NAMED_PROJECT";
  case CommandType::INVALID:
    return "INVALID";
  default:
    return "UNKNOWN";
  }
}
