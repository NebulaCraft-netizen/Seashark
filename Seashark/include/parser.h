#pragma once
#include "lexer.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

// AST forward declarations
struct ASTNode;
struct Expr;
struct Stmt;

// Base pointer types
using ASTNodePtr = std::shared_ptr<ASTNode>;
using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

struct ASTNode { virtual ~ASTNode() = default; };

struct Expr : ASTNode {};
struct Stmt : ASTNode {};

// Expressions
struct NumberExpr : Expr { long value; NumberExpr(long v):value(v){} };
struct StringExpr : Expr { std::string value; StringExpr(const std::string &v):value(v){} };
struct VarExpr : Expr { std::string name; VarExpr(const std::string &n):name(n){} };
struct BinaryExpr : Expr { char op; ExprPtr left, right; BinaryExpr(char o, ExprPtr l, ExprPtr r):op(o),left(l),right(r){} };
struct CallExpr : Expr { std::string callee; std::vector<ExprPtr> args; CallExpr(const std::string &c):callee(c){} };

// Statements
struct ExprStmt : Stmt { ExprPtr expr; ExprStmt(ExprPtr e):expr(e){} };
struct PrintStmt : Stmt { ExprPtr expr; PrintStmt(ExprPtr e):expr(e){} };
struct VarDeclStmt : Stmt { std::string name; ExprPtr init; VarDeclStmt(const std::string &n, ExprPtr i):name(n),init(i){} };
struct AssignStmt : Stmt { std::string name; ExprPtr value; AssignStmt(const std::string &n, ExprPtr v):name(n),value(v){} };
struct IfStmt : Stmt { ExprPtr cond; std::vector<StmtPtr> thenBranch; std::vector<StmtPtr> elseBranch; };
struct WhileStmt : Stmt { ExprPtr cond; std::vector<StmtPtr> body; };
struct FuncDeclStmt : Stmt { std::string name; std::vector<std::string> params; std::vector<StmtPtr> body; };
struct ReturnStmt : Stmt { ExprPtr value; ReturnStmt(ExprPtr v):value(v){} };

class Parser {
public:
    Parser(const std::vector<Token> &tokens);
    std::vector<StmtPtr> parse();

private:
    const std::vector<Token> &tokens;
    size_t pos;

    Token peek();
    Token advance();
    bool match(TokenType t);
    bool check(TokenType t);
    Token consume(TokenType t, const std::string &msg);

    // parse helpers
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr funcDeclaration();
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr returnStatement();
    std::vector<StmtPtr> block();

    StmtPtr expressionStatement();
    ExprPtr expression();
    ExprPtr equality();
    ExprPtr addition();
    ExprPtr multiplication();
    ExprPtr unary();
    ExprPtr primary();
};

