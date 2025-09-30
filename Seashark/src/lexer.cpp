#include "lexer.h"
#include <cctype>

Lexer::Lexer(const std::string &src) : src(src), pos(0), line(1) {}

char Lexer::peek() {
    return pos < src.size() ? src[pos] : '\0';
}
char Lexer::advance() {
    char c = peek();
    if (c == '\n') ++line;
    if (pos < src.size()) pos++;
    return c;
}
void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

Token Lexer::makeIdent() {
    std::string s;
    int startLine = line;
    while (isalnum(peek()) || peek() == '_') s += advance();
    if (s == "print") return {TokenType::PRINT, s, startLine};
    if (s == "let") return {TokenType::LET, s, startLine};
    if (s == "func") return {TokenType::FUNC, s, startLine};
    if (s == "if") return {TokenType::IF, s, startLine};
    if (s == "else") return {TokenType::ELSE, s, startLine};
    if (s == "while") return {TokenType::WHILE, s, startLine};
    if (s == "return") return {TokenType::RETURN, s, startLine};
    return {TokenType::IDENT, s, startLine};
}

Token Lexer::makeNumber() {
    std::string s;
    int startLine = line;
    while (isdigit(peek())) s += advance();
    // simple integer numbers only for now
    return {TokenType::NUMBER, s, startLine};
}

Token Lexer::makeString() {
    int startLine = line;
    advance(); // skip opening "
    std::string s;
    while (peek() != '"' && peek() != '\0') {
        s += advance();
    }
    if (peek() == '"') advance(); // closing "
    return {TokenType::STRING, s, startLine};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < src.size()) {
        skipWhitespace();
        char c = peek();
        if (c == '\0') break;
        if (isalpha(c) || c == '_') {
            tokens.push_back(makeIdent());
            continue;
        }
        if (isdigit(c)) {
            tokens.push_back(makeNumber());
            continue;
        }
        if (c == '"') {
            tokens.push_back(makeString());
            continue;
        }
        // single-char tokens
        int currentLine = line;
        switch (advance()) {
            case '(': tokens.push_back({TokenType::LPAREN, "(", currentLine}); break;
            case ')': tokens.push_back({TokenType::RPAREN, ")", currentLine}); break;
            case '{': tokens.push_back({TokenType::LBRACE, "{", currentLine}); break;
            case '}': tokens.push_back({TokenType::RBRACE, "}", currentLine}); break;
            case ',': tokens.push_back({TokenType::COMMA, ",", currentLine}); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", currentLine}); break;
            case '+': tokens.push_back({TokenType::PLUS, "+", currentLine}); break;
            case '-': tokens.push_back({TokenType::MINUS, "-", currentLine}); break;
            case '*': tokens.push_back({TokenType::STAR, "*", currentLine}); break;
            case '/': tokens.push_back({TokenType::SLASH, "/", currentLine}); break;
            case '=': tokens.push_back({TokenType::EQUAL, "=", currentLine}); break;
            default:
                tokens.push_back({TokenType::UNKNOWN, std::string(1, src[pos-1]), currentLine});
                break;
        }
    }
    tokens.push_back({TokenType::END, "", line});
    return tokens;
}
