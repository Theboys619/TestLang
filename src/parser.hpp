#include <iostream>
#include <map>
#include "lexer.hpp"

std::map<std::string, int> PRECEDENCE = {
  { "=", 1 },

  { "&&", 4 },
  { "||", 5 },

  { "<", 7 }, { ">", 7 }, { "<=", 7 }, { ">=", 7 }, { "==", 7 }, { "!=", 7 },

  { "+", 10 }, { "-", 10 },
  { "*", 20 }, { "/", 20 }, { "%", 20 },
};

class SyntaxError : public std::exception {
  std::string message;
  public:

  const char* what() const noexcept {
    return message.c_str();
  };

  SyntaxError(std::string message): message(message) {};
};

enum class ExprTypes {
  None,
  Integer,
  String,
  Identifier,
  Assign,
  Binary,
  Scope,
  FunctionCall,
  Function
};

class Expression {
  public:
  ExprTypes type;
  Token value;

  Expression* left;
  Token op;
  Expression* right;

  // Scopes and Functinos

  std::vector<Expression*> block;
  std::vector<Expression*> args;
  Expression* scope;

  // Identifiers
  Expression* dotOp;

  Expression(Token value): type(ExprTypes::None), value(value) {};
  Expression(ExprTypes type, Token value): type(type), value(value) {};
};

class Parser {
  std::vector<Token> tokens;
  int pos = 0;

  Token curTok;

  Expression* ast;

  public:

  Parser(std::vector<Token> tokens): tokens(tokens) {};

  Token advance(int amt = 1) {
    pos += amt;
    return curTok = tokens[pos];
  }

  Token peek(int amt = 1) {
    return tokens[pos + amt];
  }

  bool isType(std::string type, std::string value, Token peeked) {
    return peeked.type == type && peeked.getString() == value;
  }
  bool isType(std::string type, Token peeked) {
    return peeked.type == type;
  }
  bool isType(std::string type, std::string value) {
    return curTok.type == type && curTok.getString() == value;
  }
  bool isType(std::string type) {
    return curTok.type == type;
  }

  bool isIgnore(Token tok) {
    return tok.type == "Delimiter" && tok.getString() == ";";
  }

  bool isEOF() {
    if (curTok.isNull()) return true;

    return curTok.type == "EOF";
  }

  void skipOverVal(std::string val, Token tok) {
    if (tok.getString() != val) throw SyntaxError("Unexpected Token with value '" + val + "'");

    advance();
  }

  void skipOver(std::string type, Token tok) {
    if (tok.type != type) throw SyntaxError("Unexpected Token '" + type + "'");

    advance();
  }

  void skipOver(std::string type, std::string val, Token tok) {
    if (tok.type != type) throw SyntaxError("Unexpected Token '" + type + "'");
    if (tok.getString() != val) throw SyntaxError("Unexpected Token with value '" + val + "'");

    advance();
  }

  std::vector<Expression*> pDelimiters(std::string start, std::string end, std::string separator) {
    std::vector<Expression*> values;
    bool isFirst = true;

    skipOverVal(start, curTok);
    if (isIgnore(curTok)) advance();

    while (!isEOF()) {
      if (isType("Delimiter", end)) {
        break;
      } else if (isFirst) {
        isFirst = false;
      } else {
        skipOverVal(separator, curTok);
      }

      Expression* val = pExpression();
      values.push_back(val);
    }
    skipOverVal(end, curTok);

    return values;
  }

  Expression* isCall(Expression* expression) {
    return isType("Delimiter", "(", peek()) ? pCall(expression) : expression;
  }

  Expression* pCall(Expression* expr) {
    std::string varName = expr->value.getString();
    advance();

    Expression* funcCall = new Expression(ExprTypes::FunctionCall, expr->value);
    funcCall->args = pDelimiters("(", ")", ",");

    return funcCall;
  }

  Expression* pBinary(Expression* left, int prec) {
    Token op = curTok;

    if (isType("BinOperator") || isType("Operator")) {
      std::string opval = op.getString();
      int newPrec = PRECEDENCE[opval];

      if (newPrec > prec) {
        advance();

        ExprTypes type = opval == "=" ? ExprTypes::Assign : ExprTypes::Binary;
        Expression* expr = new Expression(type, op);
        expr->left = left;
        expr->op = op;
        expr->right = pBinary(pAll(), newPrec);

        return pBinary(expr, prec);
      }
    }

    return left;
  }

  Expression* pIdentifier(Expression* oldTok) {
    oldTok->type = ExprTypes::Identifier;

    if (!isType("Delimiter", "(", peek()))
      advance();

    if (isType("Delimiter", ".")) {
      advance();
      oldTok->dotOp = pExpression();
    }


    return oldTok;
  }

  Expression* $pAll() {
    if (isType("Delimiter", "(")) {
      skipOver("Delimiter", "(", curTok); 
      Expression* expr = pExpression();
      skipOver("Delimiter", ")", curTok);

      return expr;
    }

    Expression* oldTok = new Expression(curTok);

    if (isType("String", curTok)) {
      oldTok->type = ExprTypes::String;

      advance();

      return oldTok;
    }

    if (isType("Identifier", curTok)) {
      return pIdentifier(oldTok);
    }

    if (isIgnore(curTok))
      return oldTok;    
  }

  Expression* pAll() {
    return isCall($pAll());
  }

  Expression* pExpression() {
    return isCall(pBinary(pAll(), 0));
  }

  Expression* parse() {
    ast = new Expression(ExprTypes::Scope, Token("main", true));
    curTok = tokens[0];

    while (!curTok.isNull() && !isEOF()) {
      Expression* expr = pExpression();
      ast->block.push_back(expr);

      if (!isEOF() && isIgnore(curTok)) {
        skipOver("Delimiter", curTok);
      }
    }

    return ast;
  }
};