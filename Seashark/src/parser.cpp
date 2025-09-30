#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}

Token Parser::peek() {
    return tokens[pos];
}
Token Parser::advance() {
    if (pos < tokens.size()) pos++;
    return tokens[pos-1];
}
bool Parser::match(TokenType t) {
    if (check(t)) { advance(); return true; }
    return false;
}
bool Parser::check(TokenType t) {
    return peek().type == t;
}
Token Parser::consume(TokenType t, const std::string &msg) {
    if (check(t)) return advance();
    throw std::runtime_error("Parse error: " + msg + " at line " + std::to_string(peek().line));
}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> out;
    while (!check(TokenType::END)) {
        out.push_back(declaration());
    }
    return out;
}

StmtPtr Parser::declaration() {
    if (match(TokenType::LET)) return varDeclaration();
    if (match(TokenType::FUNC)) return funcDeclaration();
    return statement();
}

StmtPtr Parser::varDeclaration() {
    Token name = consume(TokenType::IDENT, "Expected variable name");
    consume(TokenType::EQUAL, "Expected '=' after variable name");
    ExprPtr init = expression();
    if (match(TokenType::SEMICOLON)) {}
    return std::make_shared<VarDeclStmt>(name.value, init);
}

StmtPtr Parser::funcDeclaration() {
    Token name = consume(TokenType::IDENT, "Expected function name");
    consume(TokenType::LPAREN, "Expected '(' after function name");
    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        do {
            Token p = consume(TokenType::IDENT, "Expected parameter name");
            params.push_back(p.value);
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RPAREN, "Expected ')'");
    consume(TokenType::LBRACE, "Expected '{' before function body");
    std::vector<StmtPtr> body = block();
    auto f = std::make_shared<FuncDeclStmt>();
    f->name = name.value;
    f->params = params;
    f->body = body;
    return f;
}

StmtPtr Parser::statement() {
    if (match(TokenType::PRINT)) return printStatement();
    if (match(TokenType::IF)) return ifStatement();
    if (match(TokenType::WHILE)) return whileStatement();
    if (match(TokenType::RETURN)) return returnStatement();
    if (match(TokenType::LBRACE)) {
        auto b = block();
        // return block as multiple statements wrapped in a single ExprStmt with nullptr expr? We'll return first as placeholder
        // instead we'll create a container: Not needed here; parser returns sequence from parse()
        // but for simplicity we return the first stmt if present
        if (!b.empty()) return b.front();
        return nullptr;
    }
    return expressionStatement();
}

StmtPtr Parser::printStatement() {
    consume(TokenType::LPAREN, "Expected '(' after print");
    ExprPtr expr = expression();
    consume(TokenType::RPAREN, "Expected ')'");
    if (match(TokenType::SEMICOLON)) {}
    return std::make_shared<PrintStmt>(expr);
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LPAREN, "Expected '(' after if");
    ExprPtr cond = expression();
    consume(TokenType::RPAREN, "Expected ')'");
    consume(TokenType::LBRACE, "Expected '{' after if condition");
    std::vector<StmtPtr> thenBranch = block();
    std::vector<StmtPtr> elseBranch;
    if (match(TokenType::ELSE)) {
        consume(TokenType::LBRACE, "Expected '{' after else");
        elseBranch = block();
    }
    auto node = std::make_shared<IfStmt>();
    node->cond = cond;
    node->thenBranch = thenBranch;
    node->elseBranch = elseBranch;
    return node;
}

StmtPtr Parser::whileStatement() {
    consume(TokenType::LPAREN, "Expected '(' after while");
    ExprPtr cond = expression();
    consume(TokenType::RParen, "Expected ')'");
    consume(TokenType::LBRACE, "Expected '{' after while");
    std::vector<StmtPtr> body = block();
    auto node = std::make_shared<WhileStmt>();
    node->cond = cond;
    node->body = body;
    return node;
}

StmtPtr Parser::returnStatement() {
    ExprPtr val = expression();
    if (match(TokenType::SEMICOLON)) {}
    return std::make_shared<ReturnStmt>(val);
}

std::vector<StmtPtr> Parser::block() {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
        statements.push_back(declaration());
    }
    consume(TokenType::RBRACE, "Expected '}' after block");
    return statements;
}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    if (match(TokenType::SEMICOLON)) {}
    // assignment check: if expr is VarExpr and next token is '=' -> treat as assignment
    // But we handle assignments via grammar: simple heuristic: if it's IDENT then EQUAL
    // For simplicity, if expression starts with VarExpr and next token was =, parser should have created assignment earlier.
    return std::make_shared<ExprStmt>(expr);
}

ExprPtr Parser::expression() {
    return equality();
}

ExprPtr Parser::equality() {
    ExprPtr expr = addition();
    // equality omitted for brevity; can add == != later
    return expr;
}

ExprPtr Parser::addition() {
    ExprPtr expr = multiplication();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token op = advance();
        ExprPtr right = multiplication();
        expr = std::make_shared<BinaryExpr>(op.value[0], expr, right);
    }
    return expr;
}

ExprPtr Parser::multiplication() {
    ExprPtr expr = unary();
    while (check(TokenType::STAR) || check(TokenType::SLASH)) {
        Token op = advance();
        ExprPtr right = unary();
        expr = std::make_shared<BinaryExpr>(op.value[0], expr, right);
    }
    return expr;
}

ExprPtr Parser::unary() {
    if (check(TokenType::MINUS)) {
        Token op = advance();
        ExprPtr right = unary();
        return std::make_shared<BinaryExpr>('-', std::make_shared<NumberExpr>(0), right);
    }
    return primary();
}

ExprPtr Parser::primary() {
    if (match(TokenType::NUMBER)) {
        long v = std::stol(tokens[pos-1].value);
        return std::make_shared<NumberExpr>(v);
    }
    if (match(TokenType::STRING)) {
        return std::make_shared<StringExpr>(tokens[pos-1].value);
    }
    if (match(TokenType::IDENT)) {
        std::string name = tokens[pos-1].value;
        // function call?
        if (check(TokenType::LPAREN)) {
            advance(); // consume (
            auto call = std::make_shared<CallExpr>(name);
            if (!check(TokenType::RPAREN)) {
                do {
                    call->args.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expected ')' after arguments");
            return call;
        }
        return std::make_shared<VarExpr>(name);
    }
    if (match(TokenType::LPAREN)) {
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    }
    throw std::runtime_error("Unexpected token in primary at line " + std::to_string(peek().line));
}
