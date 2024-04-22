#include <cstring>
#include <iostream>
#include <sys/stat.h>

enum class CommandType { DEFAULT, NAMED_PROJECT, INVALID };

class CppCommand {
private:
  char **argv;
  int argc;

public:
  CppCommand(char **argv, int argc);
  ~CppCommand();
  CommandType validateCmd();
  static char **execute(char **argv, int &argc);
  void generateMainFile();
  void generateMakeFile();
  bool generateDirectory(char *dir);
  bool touch(const char *filename);
  std::string CommandTypeToString(CommandType commandType);
  // make your own argv here so you can free it without fucking up the other
  // commands
};
