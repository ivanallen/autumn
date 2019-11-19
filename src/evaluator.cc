#include "evaluator.h"

namespace autumn {

Evaluator::Evaluator() {
}
 
std::unique_ptr<object::Object> Evaluator::eval(const std::string& input) {
    auto program = _parser.parse(input);
    return eval(program.get());
}

std::unique_ptr<object::Object> Evaluator::eval(const ast::Node* node) const {
    if (node == nullptr) {
        return std::unique_ptr<object::Object>(new object::Null());
    }

    if (typeid(*node) == typeid(ast::Program)) {
        auto n = node->cast<ast::Program>();
        return eval_statments(n->statments());
    } else if (typeid(*node) == typeid(ast::ExpressionStatment)) {
        auto n = node->cast<ast::ExpressionStatment>();
        return eval(n->expression());
    } else if (typeid(*node) == typeid(ast::IntegerLiteral)) {
        auto n = node->cast<ast::IntegerLiteral>();
        return std::unique_ptr<object::Integer>(new object::Integer(n->value()));
    }

    return std::unique_ptr<object::Object>(new object::Null());
}

std::unique_ptr<object::Object> Evaluator::eval_statments(
        const std::vector<std::unique_ptr<ast::Statment>>& statments) const {
    std::unique_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get());
    }
    return result;
}

} // namespace autumn
