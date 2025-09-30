#pragma once
#include "parser.h"
#include <map>
#include <string>
#include <variant>
#include <functional>

using Value = std::variant<std::monostate, long, std::string>;

struct Function {
    std::vector<std::string> params;
    std::vector<StmtPtr> body;
};

class Interpreter {
public:
    Interpreter();
    void run(const std::vector<StmtPtr> &stmts);

private:
    // environment stack
    std::vector<std::map<std::string, Value>> envStack;
    std::map<std::string, Function> functions;

    void pushEnv();
    void popEnv();
    void defineVar(const std::string &name, Value val);
    void assignVar(const std::string &name, Value val);
    Value getVar(const std::string &name);

    Value evalExpr(ExprPtr e);
    void execStmt(StmtPtr s);
    Value callFunction(const std::string &name, const std::vector<ExprPtr> &args);
};
