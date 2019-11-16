#pragma once

#include <memory>
#include <string>
#include <vector>

namespace autumn {

class Parser;

namespace ast {


// 抽象节点
class Node {
public:
    virtual std::string token_literal() const = 0;
    virtual std::string to_string() const = 0;

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

    std::string to_string() const override {
        return _value;
    }

    const std::string& value() const {
        return _value;
    }

private:
    std::string _value;
};

class IntegerLiteral : public Expression {
public:
    IntegerLiteral (const Token& token) :
            Expression(token) {
        _value = std::stoi(token.literal);
    }

    std::string to_string() const override {
        return token_literal();
    }

    int value() const {
        return _value;
    }

private:
    int _value;
};

class PrefixExpression : public Expression {
public:
    friend class autumn::Parser;
    PrefixExpression(const Token& token) :
            Expression(token), _operator(token.literal) {
    }

    std::string to_string() const override {
        if (_right == nullptr) {
            return "()";
        }
        return "(" + _operator + _right->to_string() + ")";
    }

    const std::string& op() const {
        return _operator;
    }

    const Expression* right() const {
        return _right.get();
    }
private:
    void set_right(Expression* expression) {
        _right.reset(expression);
    }
private:
    std::string _operator;
    std::unique_ptr<Expression> _right;
};

class InfixExpression : public Expression {
public:
    friend class autumn::Parser;
    InfixExpression(const Token& token) :
            Expression(token), _operator(token.literal) {
    }

    std::string to_string() const override {
        if (_left == nullptr || _right == nullptr) {
            return "()";
        }
        return "(" + _left->to_string() + _operator + _right->to_string() + ")";
    }

    const std::string& op() const {
        return _operator;
    }

    const Expression* left() const {
        return _left.get();
    }

    const Expression* right() const {
        return _right.get();
    }
private:
    void set_left(Expression* expression) {
        _left.reset(expression);
    }

    void set_right(Expression* expression) {
        _right.reset(expression);
    }
private:
    std::string _operator;
    std::unique_ptr<Expression> _left;
    std::unique_ptr<Expression> _right;
};

class LetStatment : public Statment {
public:
    friend class autumn::Parser;
    using Statment::Statment;

    std::string token_literal() const override {
        return _token.literal;
    }

    std::string to_string() const override {
        std::string ret = token_literal()
            + ' ' + identifier()->to_string()
            + " = ";
        if (_expression != nullptr) {
            ret += _expression->to_string();
        }
        ret += ';';
        return ret;
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
    friend class autumn::Parser;
    using Statment::Statment;

    const Expression* expression() const {
        return _expression.get();
    }

    std::string to_string() const override {
        std::string ret = token_literal() + ' ';
        if (_expression != nullptr) {
            ret += _expression->to_string();
        }
        ret += ';';
        return ret;
    }

private:
    void set_expression(Expression* expression) {
        _expression.reset(expression);
    }
private:
    std::unique_ptr<Expression> _expression;
};

class ExpressionStatment : public Statment {
public:
    friend class autumn::Parser;
    using Statment::Statment;
    const Expression* expression() const {
        return _expression.get();
    }

    std::string to_string() const override {
        std::string ret;
        if (_expression != nullptr) {
            ret = _expression->to_string();
        }
        return ret;
    }
private:
    void set_expression(Expression* expression) {
        _expression.reset(expression);
    }
private:
    std::unique_ptr<Expression> _expression;
};

class Program {
public:
    friend class autumn::Parser;
    const std::vector<std::unique_ptr<Statment>>& statments() const {
        return _statments;
    }

    std::string to_string() const {
        std::string ret;
        for (auto& stmt : _statments) {
            ret += stmt->to_string();
        }
        return ret;
    }

private:
    void append(Statment* statment) {
        _statments.emplace_back(statment);
    }
private:
    std::vector<std::unique_ptr<Statment>> _statments;
};

} // namespace ast
} // namespace autumn
