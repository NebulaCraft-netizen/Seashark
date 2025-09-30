#include "interpreter.h"
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter() {
    pushEnv();
    // add built-in functions
    functions["print"] = Function{ {"value"}, {} };
}

void Interpreter::pushEnv() {
    envStack.emplace_back();
}
void Interpreter::popEnv() {
    envStack.pop_back();
}
void Interpreter::defineVar(const std::string &name, Value val) {
    envStack.back()[name] = val;
}
void Interpreter::assignVar(const std::string &name, Value val) {
    for (auto it = envStack.rbegin(); it != envStack.rend(); ++it) {
        if (it->count(name)) { (*it)[name] = val; return; }
    }
    // if not found, define in current scope
    defineVar(name, val);
}
Value Interpreter::getVar(const std::string &name) {
    for (auto it = envStack.rbegin(); it != envStack.rend(); ++it) {
        if (it->count(name)) return (*it)[name];
    }
    return {}; // monostate = null
}

Value Interpreter::evalExpr(ExprPtr e) {
    if (auto n = std::dynamic_pointer_cast<NumberExpr>(e)) {
        return (Value) n->value;
    }
    if (auto s = std::dynamic_pointer_cast<StringExpr>(e)) {
        return (Value) s->value;
    }
    if (auto v = std::dynamic_pointer_cast<VarExpr>(e)) {
        return getVar(v->name);
    }
    if (auto b = std::dynamic_pointer_cast<BinaryExpr>(e)) {
        Value left = evalExpr(b->left);
        Value right = evalExpr(b->right);
        long l = std::get<long>(left);
        long r = std::get<long>(right);
        switch (b->op) {
            case '+': return (Value)(l + r);
            case '-': return (Value)(l - r);
            case '*': return (Value)(l * r);
            case '/': return (Value)(l / r);
        }
    }
    if (auto c = std::dynamic_pointer_cast<CallExpr>(e)) {
        return callFunction(c->callee, c->args);
    }
    return {};
}

void Interpreter::execStmt(StmtPtr s) {
    if (!s) return;
    if (auto ps = std::dynamic_pointer_cast<PrintStmt>(s)) {
        Value v = evalExpr(ps->expr);
        if (std::holds_alternative<long>(v)) std::cout << std::get<long>(v) << std::endl;
        else if (std::holds_alternative<std::string>(v)) std::cout << std::get<std::string>(v) << std::endl;
        else std::cout << "null\n";
        return;
    }
    if (auto vs = std::dynamic_pointer_cast<VarDeclStmt>(s)) {
        Value v = evalExpr(vs->init);
        defineVar(vs->name, v);
        return;
    }
    if (auto es = std::dynamic_pointer_cast<ExprStmt>(s)) {
        evalExpr(es->expr);
        return;
    }
    if (auto fd = std::dynamic_pointer_cast<FuncDeclStmt>(s)) {
        Function f;
        f.params = fd->params;
        f.body = fd->body;
        functions[fd->name] = f;
        return;
    }
    if (auto ifs = std::dynamic_pointer_cast<IfStmt>(s)) {
        Value cond = evalExpr(ifs->cond);
        bool truth = false;
        if (std::holds_alternative<long>(cond)) truth = (std::get<long>(cond) != 0);
        if (truth) {
            for (auto &st : ifs->thenBranch) execStmt(st);
        } else {
            for (auto &st : ifs->elseBranch) execStmt(st);
        }
        return;
    }
    if (auto ws = std::dynamic_pointer_cast<WhileStmt>(s)) {
        while (true) {
            Value cond = evalExpr(ws->cond);
            bool truth = false;
            if (std::holds_alternative<long>(cond)) truth = (std::get<long>(cond) != 0);
            if (!truth) break;
            for (auto &st : ws->body) execStmt(st);
        }
        return;
    }
    if (auto rs = std::dynamic_pointer_cast<ReturnStmt>(s)) {
        // return handling omitted (would need call stack / exceptions)
        return;
    }
}

Value Interpreter::callFunction(const std::string &name, const std::vector<ExprPtr> &args) {
    // built-in: print
    if (name == "print") {
        for (auto &a : args) {
            Value v = evalExpr(a);
            if (std::holds_alternative<long>(v)) std::cout << std::get<long>(v);
            else if (std::holds_alternative<std::string>(v)) std::cout << std::get<std::string>(v);
            else std::cout << "null";
        }
        std::cout << std::endl;
        return {};
    }
    // user-defined
    if (!functions.count(name)) throw std::runtime_error("Undefined function: " + name);
    Function &f = functions[name];
    if (f.params.size() != args.size()) throw std::runtime_error("Argument count mismatch on " + name);
    pushEnv();
    for (size_t i=0;i<args.size();++i) {
        Value v = evalExpr(args[i]);
        defineVar(f.params[i], v);
    }
    for (auto &s : f.body) execStmt(s);
    popEnv();
    return {};
}

void Interpreter::run(const std::vector<StmtPtr> &stmts) {
    for (auto &s : stmts) {
        execStmt(s);
    }
}
