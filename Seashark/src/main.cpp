#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string readFile(const std::string &path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: seashark <file.ss>\n";
        return 1;
    }
    try {
        std::string src = readFile(argv[1]);
        Lexer lex(src);
        auto tokens = lex.tokenize();
        Parser parser(tokens);
        auto stmts = parser.parse();
        Interpreter interp;
        interp.run(stmts);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }
    return 0;
}

