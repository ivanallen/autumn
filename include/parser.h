#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "lexer.h"
#include "program.h"
#include "tracer.h"

namespace autumn {

class Parser {
public:
    enum Precedence {
        UNKNOWN = 0,
        LOWEST,
        EQUALS, // ==
        LESSGREATER, // < or >
        SUM, // +
        PRODUCT, // *
        PREFIX, // -x or !x
        CALL, // fn(x)
        INDEX, // array[index]
    };
public:
    Parser();
    std::unique_ptr<ast::Program> parse(const std::string& input);
    const std::vector<std::string>& errors() const;
private:
    void next_token();
    bool expect_peek(Token::Type type);
    bool current_token_is(Token::Type type) const;
    bool peek_token_is(Token::Type type) const;
    void peek_error(Token::Type type);
    Parser::Precedence current_precedence() const;
    Parser::Precedence peek_precedence() const;

    std::unique_ptr<ast::Program> parse();
    std::unique_ptr<ast::Statment> parse_statment();
    std::unique_ptr<ast::Statment> parse_let_statment();
    std::unique_ptr<ast::Statment> parse_return_statment();
    std::unique_ptr<ast::Statment> parse_expression_statment();
    std::unique_ptr<ast::BlockStatment> parse_block_statment();
    std::vector<std::shared_ptr<ast::Identifier>> parse_function_parameters();
    std::vector<std::unique_ptr<ast::Expression>> parse_expression_list(Token::Type end);

    std::unique_ptr<ast::Expression> parse_expression(Precedence precedence);
private:
    // 注册函数
    std::unique_ptr<ast::Expression> parse_identifier();
    std::unique_ptr<ast::Expression> parse_integer_literal();
    std::unique_ptr<ast::Expression> parse_string_literal();
    std::unique_ptr<ast::Expression> parse_boolean_literal();
    std::unique_ptr<ast::Expression> parse_function_literal();
    std::unique_ptr<ast::Expression> parse_array_literal();
    std::unique_ptr<ast::Expression> parse_hash_literal();
    std::unique_ptr<ast::Expression> parse_group_expression();
    std::unique_ptr<ast::Expression> parse_prefix_expression();
    std::unique_ptr<ast::Expression> parse_if_expression();
    std::unique_ptr<ast::Expression> parse_infix_expression(ast::Expression* left);
    std::unique_ptr<ast::Expression> parse_call_expression(ast::Expression* left);
    std::unique_ptr<ast::Expression> parse_index_expression(ast::Expression* left);
private:
    using PrefixParseFunc = std::function<std::unique_ptr<ast::Expression>()>;
    using InfixParseFunc = std::function<std::unique_ptr<ast::Expression>(ast::Expression* expression)>;

    Lexer* _lexer = nullptr;
    Token _current_token{Token::ILLEGAL, ""};
    Token _peek_token{Token::ILLEGAL, ""};
    std::vector<std::string> _errors;

    // 用于解析前缀操作符
    std::map<Token::Type, PrefixParseFunc> _prefix_parse_funcs;
    // 用于解析后缀操作符
    std::map<Token::Type, InfixParseFunc> _infix_parse_funcs;

    Tracer _tracer;
};

} // namespace autumn
