#include <iostream>
#include "lexer.h"

const std::string PROMPT = ">> ";

int main() {
    std::string line;
    std::cout << PROMPT;
    while (std::getline(std::cin, line)) {
        autumn::Lexer lexer(line);

        for (auto token = lexer.next_token();
                token.type != autumn::Token::END;
                token = lexer.next_token()) {
            std::cout << token << std::endl;
        }

        std::cout << PROMPT;
    }

    return 0;
}
