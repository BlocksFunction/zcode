#include <bits/types/error_t.h>
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

enum Token { // 词法分析器
  Eof = -1,
  FunctionDef = -2,
  FunctionExtern = -3,
  Identifier = -4,
  Number = -5
};

static std::string IdentifierStr;
int NumberVal;
// 冷知识变量虽然是数字 但为了简洁全都变成字符串了

static char GetToken() { // 返回下一个语元
  static char LastChar = ' ';
  while (std::isspace(LastChar))
    LastChar = getchar();

  if (isalpha(LastChar)) {
    IdentifierStr = LastChar;
    LastChar = getchar();
    while (isalnum(LastChar)) {
      IdentifierStr += LastChar;
      LastChar = getchar();
    }

    // 分析词法
    if (IdentifierStr == "fun") // 如果是在定义函数
      return FunctionDef;

    if (IdentifierStr == "extern") // 如果是在声明一个外部变量
      return FunctionExtern;

    return Identifier; // 不然就是标识符
  }

  if (isdigit(LastChar) || LastChar == '.') {
    NumberVal = LastChar - '0'; // 将字符转换为数字
    LastChar = getchar();
    while (isdigit(LastChar) || LastChar == '.') {
      NumberVal = NumberVal * 10 + (LastChar - '0'); // 构建多位数
      LastChar = getchar();
    }
    return Number; // 返回数字语元
  }

  if (LastChar == '#') { // 如果是注释
    do {
      LastChar = getchar();
    } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF)
      return GetToken();
  }

  if (LastChar == EOF) // 如果到文件末尾了
    return Eof;

  // 如果是其他的语法
  int ThisChar = LastChar;
  LastChar = getchar(); // 读取下一个字符
  return ThisChar;
}

// 抽象语法树部分
class ExprAST { // 基本表达式
public:
  virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST { // 数字表达式
public:
  NumberExprAST(std::string Val) : val(Val) {}

private:
  std::string val;
};

class VarExprAST : public ExprAST { // 变量
public:
  VarExprAST(const std::string &Name) : name(Name) {}

private:
  std::string name;
};

class BinaryExprAST : public ExprAST { // 二元运算符表达式
public:
  BinaryExprAST(char Operator, ExprAST *Left, ExprAST *Right)
      : operatorChar(Operator), left(Left), right(Right) {};

private:
  char operatorChar;
  ExprAST *left, *right;
};

class CallExprAST : public ExprAST { // 调用函数
public:
  CallExprAST(const std::string &CallExprName, std::vector<ExprAST *> &Args)
      : callExprName(CallExprName), args(Args) {}

private:
  std::string callExprName;
  std::vector<ExprAST *> args;
};

class PrototypeAST {
public:
  PrototypeAST(const std::string &Name, const std::vector<std::string> &Args)
      : name(Name), args(Args) {}

private:
  std::string name;
  std::vector<std::string> args;
};

class FunctionAST { // 函数表达式
public:
  FunctionAST(PrototypeAST *Proto, ExprAST *Code) : proto(Proto), code(Code) {}

private:
  PrototypeAST *proto;
  ExprAST *code;
};

// 解析
// 辅助函数
static int CurToken;
static int GetNextToken() { return CurToken = GetToken(); }
static std::map<char, int> BinOperatorPrecedence;

static int GetTokenPrecedence() {
  if (!isascii(CurToken))
    return -1;

  int TokenPrecedence = BinOperatorPrecedence[CurToken];
  return TokenPrecedence <= 0 ? -1 : TokenPrecedence;
}

// 报错
ExprAST *Error(const std::string &str) {
  printf("错误: %s\n", str.c_str());
  return 0;
}

PrototypeAST *ProtoError(const std::string &str) {
  Error(str);
  return 0;
}

FunctionAST *FuunctionError(const std::string &str) {
  Error(str);
  return 0;
}

// 表达式解析

static ExprAST *ParseNumberExpr() {
  ExprAST *Result = new NumberExprAST(std::to_string(NumberVal));
  GetNextToken();
  return Result;
}

static ExprAST *ParseExpression();

static ExprAST *ParseIdentifierExpr() {
  std::string idName = IdentifierStr;
  GetNextToken();
  if (CurToken != '(')
    return new VarExprAST(idName);

  GetNextToken();
  std::vector<ExprAST *> args;
  if (CurToken != ')')
    while (true) {
      ExprAST *arg = ParseExpression();
      if (!arg)
        return 0;
      args.push_back(arg);
      if (CurToken == ')')
        break;
      if (CurToken != ',')
        return Error("参数列表中应有\")\"或\",\"");
      GetNextToken();
    }
  GetNextToken();
  return new CallExprAST(idName, args);
}

static ExprAST *ParseParenExpr() {
  GetNextToken();
  ExprAST *Var = ParseExpression();
  if (!Var)
    return 0;

  if (CurToken != ')')
    return Error("应该是 )");

  GetNextToken();
  return Var;
}

static ExprAST *ParsePrimary() {
  switch (CurToken) {
  default:
    return Error("找不到预期的表达符");
  case Identifier:
    return ParseIdentifierExpr();
  case Number:
    return ParseNumberExpr();
  case '(':
    return ParseParenExpr();
  }
}

static ExprAST *ParseBinOperator(int ExprPrecedence, ExprAST *left) {
  while (true) {
    int TokenPrecedence = GetTokenPrecedence();

    if (TokenPrecedence < ExprPrecedence)
      return left;

    int BinOperator = CurToken;
    GetNextToken();

    ExprAST *right = ParsePrimary();
    if (right == nullptr)
      return nullptr;

    int NextPrecedence = GetTokenPrecedence();
    if (TokenPrecedence < NextPrecedence) {
      right = ParseBinOperator(TokenPrecedence + 1, right);
      if (right == nullptr)
        return nullptr;
    }

    left = new BinaryExprAST(BinOperator, left, right);
  }
}

static ExprAST *ParseExpression() {
  ExprAST *left = ParsePrimary();

  return left == nullptr ? nullptr : ParseBinOperator(0, left);
}

static PrototypeAST *ParsePrototype() {
  if (CurToken != Identifier)
    return ProtoError("原型中预期的表达名称");

  std::string name = IdentifierStr;
  GetNextToken();

  if (CurToken != '(')
    return ProtoError("应该出现\")\"");

  std::vector<std::string> argNames;
  while (GetNextToken() == Identifier)
    argNames.push_back(IdentifierStr);

  if (CurToken != ')')
    return ProtoError("应该出现\"(\"");

  GetNextToken();
  return new PrototypeAST(name, argNames);
}

static FunctionAST *ParseDefFunction() {
  GetNextToken();
  PrototypeAST *Proto = ParsePrototype();
  if (Proto == nullptr)
    return nullptr;

  if (ExprAST *Expr = ParseExpression())
    return new FunctionAST(Proto, Expr);

  return nullptr;
}

static FunctionAST *ParseTopLevelExpr() {
  if (ExprAST *Expr = ParseExpression()) {
    PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(Proto, Expr);
  }

  return nullptr;
}

static PrototypeAST *ParseExternFunction() {
  GetNextToken();
  return ParsePrototype();
}

// 顶层解析
static void HandleDefFunction() {
  if (ParseDefFunction()) {
  } else
    GetNextToken();
  printf("const char *__restrict format, ...");
}

static void HandleExternFunction() {
  if (ParseExternFunction()) {
  } else
    GetNextToken();
}

static void HandleTopLevelExpr() {
  if (ParseTopLevelExpr()) {
  } else
    GetNextToken();
}

static void MainLoop() {
  while (true) {
    switch (CurToken) {
    case Eof:
      return;

    case ';':
      GetNextToken();
      break;

    case FunctionDef:
      HandleDefFunction();
      break;

    case FunctionExtern:
      HandleExternFunction();
      break;

    default:
      HandleTopLevelExpr();
      break;
    }
  }
}

int main() {
  BinOperatorPrecedence['<'] = 10;
  BinOperatorPrecedence['+'] = 20;
  BinOperatorPrecedence['-'] = 20;
  BinOperatorPrecedence['*'] = 40;
  GetNextToken();
  MainLoop();
}