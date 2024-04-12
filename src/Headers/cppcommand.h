class CppCommand {
private:
  char **argv;
  int argc;

public:
  CppCommand(char **argv, int argc);
  void generateMakeFile();
  void generateDirectory();
  // make your own argv here so you can free it without fucking up the other
  // commands
};
