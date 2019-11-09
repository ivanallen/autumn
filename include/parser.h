#pragma once

#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

namespace autumn {

// 抽象节点
class Node {
public:
    virtual std::string token_literal() = 0;

    template<typename T>
    const T* cast() const {
        return dynamic_cast<const T*>(this);
    };

    template<typename T>
    const T& cast() {
        return dynamic_cast<T&>(*this);
    };
};

// 语句
class Statment : public Node {};

// 表达式
class Expression : public Node {};

// 标识符
class Identifier : public Expression {
public:
    Identifier(const Token& token, const std::string& value) :
        _token(token), _value(value) {
    }

    std::string token_literal() override {
        return _token.literal;
    }
private:
    Token _token;
    std::string _value;
};

class LetStatment : public Statment {
public:
    LetStatment(const Token& token) :
        _token(token) {
    }

    std::string token_literal() override {
        return _token.literal;
    }

    void set_identifier(Identifier* identifier) {
        _identifier.reset(identifier);
    }

    void set_expression(Identifier* expression) {
        _expression.reset(expression);
    }
private:
    Token _token;
    std::unique_ptr<Identifier> _identifier;
    std::unique_ptr<Expression> _expression;
};

class Program {
public:
    Program(const std::string& input);

    const std::vector<std::unique_ptr<Statment>>& statments() const;
private:
    void next_token();
    bool expect_peek(Token::Type type);
    bool parse();
    std::unique_ptr<Statment> parse_statment();
    std::unique_ptr<Statment> parse_let_statment();
private:
    std::vector<std::unique_ptr<Statment>> _statments;
    Lexer _lexer;
    Token _current_token{Token::ILLEGAL, ""};
    Token _peek_token{Token::ILLEGAL, ""};
};

} // namespace autumn
