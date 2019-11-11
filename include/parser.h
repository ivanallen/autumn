#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "lexer.h"
#include "program.h"

namespace autumn {

class Parser {
public:
    enum Operand {
        UNKNOWN = 0,
        LOWEST,
        EQUALS, // ==
        LESSGREATER, // < or >
        SUM, // +
        PRODUCT, // *
        PREFIX, // -x or !x
        CALL, // fn(x)
    };
public:
    Parser();
    std::unique_ptr<Program> parse(const std::string& input);
    const std::vector<std::string>& errors() const;
private:
    void next_token();
    bool expect_peek(Token::Type type);
    bool current_token_is(Token::Type type) const;
    bool peek_token_is(Token::Type type) const;
    void peek_error(Token::Type type);

    std::unique_ptr<Program> parse();
    std::unique_ptr<Statment> parse_statment();
    std::unique_ptr<Statment> parse_let_statment();
    std::unique_ptr<Statment> parse_return_statment();
    std::unique_ptr<Statment> parse_expression_statment();

    std::unique_ptr<Expression> parse_expression(Operand op);
private:
    // 注册函数
    std::unique_ptr<Expression> parse_identifier();
private:
    using PrefixParseFunc = std::function<std::unique_ptr<Expression>()>;
    using InfixParseFunc = std::function<std::unique_ptr<Expression>(const Expression* expression)>;

    Lexer* _lexer = nullptr;
    Token _current_token{Token::ILLEGAL, ""};
    Token _peek_token{Token::ILLEGAL, ""};
    std::vector<std::string> _errors;

    // 用于解析前缀操作符
    std::map<Token::Type, PrefixParseFunc> _prefix_parse_funcs;
    // 用于解析后缀操作符
    std::map<Token::Type, InfixParseFunc> _infix_parse_funcs;
};

} // namespace autumn
