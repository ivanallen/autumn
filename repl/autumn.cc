#include <functional>
#include <iostream>
#include <map>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

#include <readline/readline.h>
#include <readline/history.h>

const std::string PROMPT = "> ";

void lexer_repl(const std::string& line);
void parser_repl(const std::string& line);
void eval_repl(const std::string& line);
void do_nothing(const std::string& line);

autumn::Evaluator evaluator;

const std::map<std::string, std::function<void(const std::string&)>> REPLS = {
    {"lexer", lexer_repl},    
    {"parser", parser_repl},    
    {"eval", eval_repl},
};

int main(int argc, char* argv[]) {
    std::function<void(const std::string&)> repl(do_nothing);
    if (argc > 1) {
        auto it = REPLS.find(argv[1]);
        if (it != REPLS.end()) {
            repl = it->second;
        }
    }
    std::string line;
    while (true) {
        char* ln = readline(PROMPT.c_str());
        if (ln == nullptr) return 0;

        line = ln;
        free(ln);

        if (!line.empty()) {
            if (line == "q" || line == "quit") return 0;
            repl(line);
            add_history(line.c_str());
        }
    }

    return 0;
}

void lexer_repl(const std::string& line) {
    autumn::Lexer lexer(line);

    for (auto token = lexer.next_token();
            token.type != autumn::Token::END;
            token = lexer.next_token()) {
        std::cout << token << std::endl;
    }
}

void parser_repl(const std::string& line) {
    autumn::Parser parser;
    auto program = parser.parse(line);
    if (!parser.errors().empty()) {
        for (auto& error : parser.errors()) {
            std::cerr << "\x1b[1;31merror: \x1b[0m"
                << error << std::endl;
        }
        return;
    }
    std::cout << program->to_string() << std::endl;
}

void eval_repl(const std::string& line) {
    auto obj = evaluator.eval(line);

    if (typeid(*obj) == typeid(autumn::object::String)) {
        std::cout << "\x1b[32m";
    } else {
        std::cout << "\x1b[1;33m";
    }
    std::cout << obj->inspect();
    std::cout << "\x1b[0m" << std::endl;
}

void do_nothing(const std::string& line) {
    std::cout << "do_nothing:" << line << std::endl;
}
