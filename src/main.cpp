#include <iostream>
#include "transpiler.hpp"

void run(std::string dir, std::string filename) {
  std::string input = readFile(filename);
  char abspath[_MAX_PATH];

  #ifdef realpath
    realpath(filename, abspath);
  #else
    std::string fullpath = _fullpath(abspath, filename.c_str(), _MAX_PATH);
    fullpath = fullpath.substr(0, fullpath.find_last_of("\\/"));
  #endif

  Lexer lexer = Lexer(input);

  std::vector<Token> tokens = lexer.tokenize();

  // for (auto token : tokens) {
  //   token.debugPrint();
  // }

  Parser parser = Parser(tokens);
  Expression* ast = parser.parse();
  
  Transpiler transpiler = Transpiler(ast);

  transpiler.defineLibs({ dir + "builtIns/io.cpp" });
  
  std::ofstream File(fullpath + "/output.cpp");

  File << transpiler.transpile();

  File.close();
}

int main(int argc, char** argv) {

  std::string directory = argv[0];
  directory.erase(directory.find_last_of('\\') + 1);

  if (argc < 2) throw std::exception("Not Enough Arguments...");

  if (std::string(argv[1]) == "run")
    run(directory, argv[2]);

  return 0;
}