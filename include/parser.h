#pragma once

#include <memory>
#include <string>
#include <vector>

#include "lexer.h"
#include "program.h"

namespace autumn {

class Parser {
public:
    std::unique_ptr<Program> parse(const std::string& input);
private:
    void next_token();
    bool expect_peek(Token::Type type);
    bool current_token_is(Token::Type type) const;
    bool peek_token_is(Token::Type type) const;
    std::unique_ptr<Program> parse();
    std::unique_ptr<Statment> parse_statment();
    std::unique_ptr<Statment> parse_let_statment();
private:
    Lexer* _lexer = nullptr;
    Token _current_token{Token::ILLEGAL, ""};
    Token _peek_token{Token::ILLEGAL, ""};
};

} // namespace autumn
