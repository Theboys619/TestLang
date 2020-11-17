#include <iostream>
#include <string>
#include <vector>

class Token {
  void* value;

  public:
  std::string type;

  int index = 1;
  int line = 1;

  Token() {
    type = "Null";
    value = nullptr;
  };

  Token(int val) {
    type = "Int";

    value = new int(val);
  };

  Token(double val) {
    type = "Double";

    value = new double(val);
  };

  Token(std::string val, bool isIdentifier = false) {
    type = isIdentifier ? "Identifier" : "String";

    value = new std::string(val);
  };

  Token(std::string type, std::string val): type(type) {
    value = new std::string(val);
  };

  Token(std::string type, char val): type(type) {
    value = new std::string(1, val);
  };

  Token(std::string type, void* value): type(type), value(value) {};

  std::string getString() {
    return *(std::string*)value;
  }

  int getInt(bool cast = false) {
    if (cast)
      return std::stoi(getString());

    return *(int*)value;
  }

  double getDouble(bool cast = false) {
    if (cast)
      return std::stod(getString());

    return *(double*)value;
  }

  bool isNull() {
    return type == "Null";
  }

  void debugPrint() {
    std::string beginning = "Token<'" + type + "', '";
    std::string ending = "";

    if (type == "String" || type == "Identifier" || type == "Delimiter") {
      ending = (*(std::string*)value + std::string("'> : ")) + std::to_string(index) + std::string(":") + std::to_string(line) + std::string("\n");
      //*(std::string*)value << "'> : " << index << ":" << line << std::endl;
    }

    if (type == "String") {
      std::cout << beginning << ending;
    } else if (type == "Identifier") {
      std::cout << beginning << ending;
    } else if (type == "Delimiter") {
      std::cout << beginning << ending;
    } else if (type == "EOF") {
      std::cout << beginning << "EOF'> : " << index << line << std::endl;
    } else if (type == "Integer" || type == "Double") {
      std::cout << beginning << getString() << "'> : " << index << ":" << line << std::endl;
    }
  }
};

class Lexer {
  public:
  std::string input;
  int length;

  std::vector<Token> tokens;

  int pos = 0;

  int line = 1;
  int index = 1;

  char curChar;

  Lexer(std::string input) : input(input) {
    length = input.length() - 1;
  };

  char advance(int amt = 1) {
    pos += amt;
    index += amt;

    if (pos >= length) return curChar = '\0';

    return curChar = input[pos];
  }

  char peek(int amt = 1) {
    if (pos + amt >= length) return '\0';

    return input[pos + amt];
  }

  bool isWhitespace(char c) {
		return c == ' ' || c == '\r' || c == '\n' || c == '\t';
	}

  bool isAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
  }

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool isNumber(char c) {
    return (
      (c == '-' && isDigit(peek()))
      || isDigit(c)
    );
  }

  bool isQuote(char c) {
    return c == '\"' || c == '\'';
  }

  bool isOperator(char c) {
    return (
      c == '+'
      || c == '-'
      || c == '*'
      || c == '/'
      || c == '%'
    );
  }

  bool isOperatorTwo(char c) {
    return (
      (c == '=' && peek() == '=')
      || (c == '!' && peek() == '=')
      || (c == '&' && peek() == '&')
      || (c == '|' && peek() == '|')
    );
  }

  std::string getOperator(char c) {
    if (isOperator(c)) return std::string(1, c);
    else if (
      (c == '=' && peek() == '=')
      || (c == '!' && peek() == '=')
      || (c == '&' && peek() == '&')
      || (c == '|' && peek() == '|')
    ) return std::string(1, c) + std::string(1, peek());
  }

  std::vector<Token> tokenize() {
    curChar = input[0];

    while (curChar != '\0') {
      int lastPos = pos;

      if (isWhitespace(curChar)) {
        if (curChar == '\n') {
          index = 0;
          ++line;
        }
        
        advance();
      }

      if (curChar == '(' || curChar == ')' || curChar == ';') {
        Token tok = Token("Delimiter", curChar);
        tok.index = index;
        tok.line = line;

        advance();

        tokens.push_back(tok);
      }

      if (isNumber(curChar)) {
        std::string type = "Integer";
        int ind = index;
        int ln = line;

        std::string val = "";

        if (curChar == '-') {
          val += curChar;
          advance();
        }

        while (isNumber(curChar)) {
          val += curChar;
          advance();

          if (curChar == '.') {
            type = "Double";
            val += ".";
            advance();
          }
        }

        Token tok = Token(type, val);
        tok.index = ind;
        tok.line = ln;

        tokens.push_back(tok);
      }

      if (isOperator(curChar)) {
        Token tok = Token("Operator", curChar);
        tok.index = index;
        tok.line = line;

        advance();

        tokens.push_back(tok);
      } else if (isOperatorTwo(curChar)) {
        Token tok = Token("Operator", getOperator(curChar));
        tok.index = index;
        tok.line = line;
        
        advance();

        tokens.push_back(tok);
      }

      if (isQuote(curChar)) {
        int tokindex = index;
        std::string value = "";
        value += advance();

        while (!isQuote(curChar)) {
          if (isQuote(peek())) {
            advance();
            break;
          }
          value += advance();
        }
        advance();

        Token tok = Token(value);
        tok.index = tokindex;
        tok.line = line;

        tokens.push_back(tok);
      }

      if (isAlpha(curChar)) {
        std::string value = "";
        std::string type = "Identifier";

        int tokindex = index;

        while (curChar != '\0' && isAlpha(curChar)) {
          value += curChar;
          advance();
        }

        Token tok = Token(value, true);
        tok.index = tokindex;
        tok.line = line;

        tokens.push_back(tok);
      }

      if (curChar == '\0') break;
      if (lastPos == pos) {
        std::string text = "Unknown character '";
        text += curChar;
        text += '\'';

        std::cout << text << std::endl;
        
        throw std::exception();
      }
    }

    tokens.push_back(Token("EOF", nullptr));

    return tokens;
  };
};