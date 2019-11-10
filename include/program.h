#pragma once

#include <memory>
#include <string>
#include <vector>

namespace autumn {

// 抽象节点
class Node {
public:
    virtual std::string token_literal() const = 0;

    template<typename T>
    const T* cast() const {
        return dynamic_cast<const T*>(this);
    };
};

// 语句
class Statment : public Node {
public:
    Statment(const Token& token) :
        _token(token) {
    }

    std::string token_literal() const override {
        return _token.literal;
    }

protected:
    Token _token;
};

// 表达式
class Expression : public Node {
public:
    Expression(const Token& token) :
        _token(token) {
    }

    std::string token_literal() const override {
        return _token.literal;
    }

protected:
    Token _token;
};

// 标识符
class Identifier : public Expression {
public:
    Identifier(const Token& token, const std::string& value) :
        Expression(token), _value(value) {
    }

    const std::string& value() const {
        return _value;
    }

private:
    std::string _value;
};

class LetStatment : public Statment {
public:
    friend class Parser;
    using Statment::Statment;

    std::string token_literal() const override {
        return _token.literal;
    }

    const Identifier* identifier() const {
        return _identifier.get();
    }

    const Expression* expression() const {
        return _expression.get();
    }

private:
    void set_identifier(Identifier* identifier) {
        _identifier.reset(identifier);
    }

    void set_expression(Identifier* expression) {
        _expression.reset(expression);
    }

private:
    std::unique_ptr<Identifier> _identifier;
    std::unique_ptr<Expression> _expression;
};

class ReturnStatment : public Statment {
public:
    friend class Parser;
    using Statment::Statment;

    const Expression* expression() const {
        return _expression.get();
    }

private:
    void set_expression(Identifier* expression) {
        _expression.reset(expression);
    }
private:
    std::unique_ptr<Expression> _expression;
};

class Program {
public:
    friend class Parser;
    const std::vector<std::unique_ptr<Statment>>& statments() const {
        return _statments;
    }

private:
    void append(Statment* statment) {
        _statments.emplace_back(statment);
    }
private:
    std::vector<std::unique_ptr<Statment>> _statments;
};

} // namespace autumn
