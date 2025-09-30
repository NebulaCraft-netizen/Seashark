#pragma once
#include <string>
#include <vector>

enum class TokenType {
    PRINT, IDENT, NUMBER, STRING,
    LET, FUNC,
    LPAREN, RPAREN, LBRACE, RBRACE, COMMA,
    EQUAL, PLUS, MINUS, STAR, SLASH,
    IF, ELSE, WHILE, RETURN,
    SEMICOLON, END, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};
class Lexer {
public:
    Lexer(const std::string &src);
    std::vector<Token> tokenize();
private:
    std::string src;
    size_t pos;
    int line;
    char peek();
    char advance();
    void skipWhitespace();
    Token makeIdent();
    Token makeNumber();
    Token makeString();
};
