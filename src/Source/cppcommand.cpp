#include "cppcommand.h"
#include "shell.h"
#include <cstddef>
#include <cstring>
#include <fstream>
#include <ostream>
#include <stdio.h>
#include <string>
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
      std::string argument = std::string(this->argv[2]);
      std::string quote = "\"";
      if (argument.find(quote) != 0) {
        return CommandType::NAMED_PROJECT;
      }
    }
  }
  return CommandType::INVALID;
}

char **CppCommand::execute(char **argv, int &argc) {

  CppCommand cmd(argv, argc);
  CommandType cmdValidation = cmd.validateCmd();
  Shell shell;
  // args containing make
  char **makeArgv = new char *[2];
  // args containing ./project
  char **exeArgv = new char *[2];

  char *exe = (char *)"./";
  char *file = argv[2];
  char buff[100]; // could cause issues
  snprintf(buff, sizeof(buff), "%s%s", exe, file);

  makeArgv[0] = (char *)"make";
  makeArgv[1] = nullptr;

  exeArgv[0] = buff;
  exeArgv[1] = nullptr;

  if (cmdValidation != CommandType::NAMED_PROJECT) {
    return argv;
  }

  bool made = cmd.generateDirectory(argv[2]);
  if (made) {

    cmd.generateMakeFile();
    cmd.generateDirectory((char *)"src");

    // make a main file with the proj name
    cmd.generateMainFile();
    chdir("..");
    cmd.generateDirectory((char *)"include");
    chdir("..");
    cmd.generateDirectory((char *)"test");
    chdir("..");
    shell.executeCmd(makeArgv);
    chdir("bin");

    shell.executeCmd(exeArgv);
    chdir("..");
    chdir("..");

    delete[] makeArgv;
    delete[] exeArgv;
  }
  return nullptr;
}

void CppCommand::generateMainFile() {
  std::string filename = std::string(argv[2]) + ".cpp";
  touch(filename.c_str());

  std::ofstream file;
  file.open(filename);
  if (!file) {
    std::cerr << "Unable to open file example.txt";
    return;
  }

  file << "#include <iostream>\n\n"
       << "int main(){\n"
       << "  std::cout << \"Project " << argv[2] << " has been made.\\n\";\n"
       << "  return 0;\n"
       << "}\n";

  file.close();
}

void CppCommand::generateMakeFile() {
  char *make = (char *)"Makefile";
  if (touch(make) != 0) {
    return;
  }

  std::ofstream makefile;
  makefile.open(make);
  if (!makefile) {
    std::cerr << "Unable to open file example.txt";
    return;
  }
  makefile
      << "EXE = " << argv[2] << "\nCC = gcc\n"
      << "CXX = g++\n"
      << "LD = g++\n"
      << "CFLAGS = \n"
      << "CXXFLAGS = \n"
      << "CPPFLAGS = -Wall\n"
      << "DEPFLAGS = -MMD -MP\n"
      << "LDFLAGS = \n"
      << "LDLIBS = \n"
      << "BIN = bin\n"
      << "OBJ = obj\n"
      << "SRC = src\n"
      << "SOURCES := $(wildcard $(SRC)/*.c $(SRC)/*.cc $(SRC)/*.cpp "
         "$(SRC)/*.cxx)\n"
      << "OBJECTS := \\\n"
      << "\t$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c)) \\\n"
      << "\t$(patsubst $(SRC)/%.cc, $(OBJ)/%.o, $(wildcard $(SRC)/*.cc)) \\\n"
      << "\t$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(wildcard $(SRC)/*.cpp)) "
         "\\\n"
      << "\t$(patsubst $(SRC)/%.cxx, $(OBJ)/%.o, $(wildcard $(SRC)/*.cxx))\n"
      << "DEPENDS := $(OBJECTS:.o=.d)\n"
      << "COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@\n"
      << "COMPILE.cxx = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@\n"
      << "LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@\n"
      << ".DEFAULT_GOAL = all\n"
      << ".PHONY: all\n"
      << "all: $(BIN)/$(EXE)\n"
      << "$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)\n"
      << "\t$(LINK.o)\n"
      << "$(SRC):\n"
      << "\tmkdir -p $(SRC)\n"
      << "$(OBJ):\n"
      << "\tmkdir -p $(OBJ)\n"
      << "$(BIN):\n"
      << "\tmkdir -p $(BIN)\n"
      << "$(OBJ)/%.o: $(SRC)/%.c\n"
      << "\t$(COMPILE.c) $<\n"
      << "$(OBJ)/%.o: $(SRC)/%.cc\n"
      << "\t$(COMPILE.cxx) $<\n"
      << "$(OBJ)/%.o: $(SRC)/%.cpp\n"
      << "\t$(COMPILE.cxx) $<\n"
      << "$(OBJ)/%.o: $(SRC)/%.cxx\n"
      << "\t$(COMPILE.cxx) $<\n"
      << ".PHONY: remake\n"
      << "remake: clean $(BIN)/$(EXE)\n"
      << ".PHONY: run\n"
      << "run: $(BIN)/$(EXE)\n"
      << "\t./$(BIN)/$(EXE)\n"
      << ".PHONY: clean\n"
      << "clean:\n"
      << "\t$(RM) $(OBJECTS)\n"
      << "\t$(RM) $(DEPENDS)\n"
      << "\t$(RM) $(BIN)/$(EXE)\n"
      << ".PHONY: reset\n"
      << "reset:\n"
      << "\t$(RM) -r $(OBJ)\n"
      << "\t$(RM) -r $(BIN)\n"
      << "-include $(DEPENDS)\n";

  makefile.close();
}

bool CppCommand::touch(const char *filename) {
  std::ofstream file;
  file.open(filename);
  if (!file) {
    std::cerr << "Unable to open file example.txt";
    return 1;
  }
  file.close();
  return 0; // Success
}

bool CppCommand::generateDirectory(char *dir) {
  // proper command should look like // cproj -m "Name"
  struct stat st = {0};
  if (!argv || !dir) {
    return false;
  }
  if (stat(dir, &st) != -1) {
    return false;
  }
  mkdir(dir, 0755); // owner has permissions
  chdir(dir);
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
