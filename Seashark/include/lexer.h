#pragma once
#include <string>
#include <vector>

enum class TokenType {
    PRINT, IDENT, NUMBER, STRING,
    LPAREN, RPAREN, EQUAL, PLUS, MINUS, STAR, SLASH,
    END, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer(const std::string &src);
    std::vector<Token> tokenize();
private:
    std::string src;
    size_t pos;
    char peek();
    char advance();
    void skipWhitespace();
};

