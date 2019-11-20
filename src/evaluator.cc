#include "evaluator.h"

namespace autumn {

Evaluator::Evaluator() :
    _null(new object::Null()),
    _true(new object::Boolean(true)),
    _false(new object::Boolean(false)) {
}
 
const std::shared_ptr<object::Object> Evaluator::eval(const std::string& input) {
    auto program = _parser.parse(input);
    return eval(program.get());
}

std::shared_ptr<object::Object> Evaluator::eval(const ast::Node* node) const {
    if (node == nullptr) {
        return _null;
    }

    if (typeid(*node) == typeid(ast::Program)) {
        auto n = node->cast<ast::Program>();
        return eval_statments(n->statments());
    } else if (typeid(*node) == typeid(ast::ExpressionStatment)) {
        auto n = node->cast<ast::ExpressionStatment>();
        return eval(n->expression());
    } else if (typeid(*node) == typeid(ast::IntegerLiteral)) {
        auto n = node->cast<ast::IntegerLiteral>();
        return std::shared_ptr<object::Integer>(
                new object::Integer(n->value()));
    } else if (typeid(*node) == typeid(ast::BooleanLiteral)) {
        auto n = node->cast<ast::BooleanLiteral>();
        return n->value() ? _true : _false;
    } else if (typeid(*node) == typeid(ast::PrefixExpression)) {
        auto n = node->cast<ast::PrefixExpression>();
        auto right = eval(n->right());
        return eval_prefix_expression(n->op(), right.get());
    }

    return _null;
}

std::shared_ptr<object::Object> Evaluator::eval_statments(
        const std::vector<std::unique_ptr<ast::Statment>>& statments) const {
    std::shared_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get());
    }
    return result;
}

std::shared_ptr<object::Object> Evaluator::eval_prefix_expression(const std::string& op, const object::Object* right) const {
    if (op == "!") {
        return eval_bang_operator_expression(right);
    } else if (op == "-") {
        return eval_minus_prefix_operator_expression(right);
    }

    return _null;
}

std::shared_ptr<object::Object> Evaluator::eval_bang_operator_expression(const object::Object* right) const {
    if (right == _null.get()) {
        return _true;
    } else if (right == _true.get()) {
        return _false;
    } else if (right == _false.get()) {
        return _true;
    }
    return _false;
}

std::shared_ptr<object::Object> Evaluator::eval_minus_prefix_operator_expression(const object::Object* right) const {
    if (typeid(*right) != typeid(object::Integer)) {
        return _null;
    }

    auto result = right->cast<object::Integer>();
    return std::make_shared<object::Integer>(-result->value());
}

} // namespace autumn
