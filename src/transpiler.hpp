#include <iostream>
#include <fstream>
#include "parser.hpp"

std::string readFile(std::string filename) {
  std::ifstream ReadFile(filename);
  std::string text = "";
  std::string line;

  while (std::getline(ReadFile, line)) {
    text += line + "\n";
  }

  if (text[text.length()-1] == EOF) text = text.substr(0, text.length() - 1);

  ReadFile.close();

  return text;
}

class Spacing {
  public:
  int spaces;
  int level;

  std::string str;

  Spacing(int spaces, int level): spaces(spaces), level(level) {
    str = "";
    setup();
  };

  Spacing(int spaces): spaces(spaces), level(0) {
    str = "";
    setup();
  };

  Spacing(): spaces(2), level(0) {
    str = "";
    setup();
  }

  void setup() {
    std::string space = "";
    for (int i = 0; i < spaces; i++) {
      space += " ";
    }

    for (int i = 0; i < level; i++) {
      str += space;
    }
  }

  std::string getString() {
    return str;
  }

  int getLevel() {
    return level;
  }
};

class Transpiler {
  Expression* ast;

  public:
  std::string fullcode;

  Transpiler(Expression* ast): ast(ast) {
    fullcode = "";
  }

  void defineLibs(std::vector<std::string> files) {
    for (std::string file : files) {
      std::string fileData = readFile(file);

      fullcode += fileData + "\n\n";
    }
  }

  std::string vectorMap(std::vector<Expression*> vector, Spacing spacing, std::string joiner) {
    std::string value = "";
    int size = vector.size();

    for (int i = 0; i < size; i++) {
      Expression* item = vector[i];
      value += codeGen(item, spacing);

      if (i < size - 1) {
        value += joiner;
      }
    }

    return value;
  }

  std::string createScope(Expression* exp, Spacing spacing) {
    std::string code = "";

    Spacing newSpacing = Spacing(2, spacing.getLevel() + 1);


    if (exp->value.getString() == "main") {
      code += "int main(int argc, char** argv) {\n" + spacing.getString();
      code += vectorMap(exp->block, spacing, ";\n" + spacing.getString());
      code += ";\n};";
    } else {
      code += newSpacing.getString() + vectorMap(exp->block, newSpacing, ";\n" + newSpacing.getString());
      code += ";\n" + spacing.getString();
    }

    return code;
  }

  std::string createType(Expression* exp, Spacing spacing) {
    if (exp->type == ExprTypes::String) {
      return "\"" + exp->value.getString() + "\"";
    }
  }

  std::string createFuncCall(Expression* exp, Spacing spacing) {
    std::string name = exp->value.getString();
    std::vector<Expression*> args = exp->args;

    std::string code = name;

    code += "(" + vectorMap(args, spacing, ", ") + ")";
    return code;
  }

  std::string codeGen(Expression* exp, Spacing spacing) {
    switch (exp->type) {
      case ExprTypes::Scope:
        return createScope(exp, spacing);
      case ExprTypes::String:
        return createType(exp, spacing);
      case ExprTypes::FunctionCall:
        return createFuncCall(exp, spacing);

      default:
        return "";
    }
  }

  std::string transpile() {
    return fullcode += codeGen(ast, Spacing(2, 1));
  }
};