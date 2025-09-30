#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string &src) : src(src), pos(0) {}

char Lexer::peek() {
    return pos < src.size() ? src[pos] : '\0';
}

char Lexer::advance() {
    return pos < src.size() ? src[pos++] : '\0';
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < src.size()) {
        skipWhitespace();
        char c = peek();

        if (isalpha(c)) {
            std::string ident;
            while (isalnum(peek())) ident += advance();
            if (ident == "print")
                tokens.push_back({TokenType::PRINT, ident});
            else
                tokens.push_back({TokenType::IDENT, ident});
        }
        else if (isdigit(c)) {
            std::string num;
            while (isdigit(peek())) num += advance();
            tokens.push_back({TokenType::NUMBER, num});
        }
        else if (c == '"') {
            advance();
            std::string str;
            while (peek() != '"' && peek() != '\0') str += advance();
            advance();
            tokens.push_back({TokenType::STRING, str});
        }
        else {
            switch (advance()) {
                case '(': tokens.push_back({TokenType::LPAREN, "("}); break;
                case ')': tokens.push_back({TokenType::RPAREN, ")"}); break;
                case '=': tokens.push_back({TokenType::EQUAL, "="}); break;
                case '+': tokens.push_back({TokenType::PLUS, "+"}); break;
                case '-': tokens.push_back({TokenType::MINUS, "-"}); break;
                case '*': tokens.push_back({TokenType::STAR, "*"}); break;
                case '/': tokens.push_back({TokenType::SLASH, "/"}); break;
                default: tokens.push_back({TokenType::UNKNOWN, std::string(1, c)}); break;
            }
        }
    }

    tokens.push_back({TokenType::END, ""});
    return tokens;
}
