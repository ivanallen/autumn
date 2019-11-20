#include "evaluator.h"

namespace autumn {

Evaluator::Evaluator() :
    _null(new object::Null()),
    _true(new object::Boolean(true)),
    _false(new object::Boolean(false)) {
}

const std::vector<std::string>& Evaluator::errors() const {
    return _parser.errors();
}
 
std::shared_ptr<const object::Object> Evaluator::eval(const std::string& input) {
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
    } else if (typeid(*node) == typeid(ast::BlockStatment)) {
        auto n = node->cast<ast::BlockStatment>();
        return eval_statments(n->statments());
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
    } else if (typeid(*node) == typeid(ast::InfixExpression)) {
        auto n = node->cast<ast::InfixExpression>();
        auto left = eval(n->left());
        auto right = eval(n->right());
        return eval_infix_expression(n->op(), left.get(), right.get());
    } else if (typeid(*node) == typeid(ast::IfExpression)) {
        return eval_if_expression(node->cast<ast::IfExpression>());
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

std::shared_ptr<object::Object> Evaluator::eval_prefix_expression(
        const std::string& op,
        const object::Object* right) const {
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


std::shared_ptr<object::Object> Evaluator::native_bool_to_boolean_object(bool input) const {
    return input ? _true : _false;
};

std::shared_ptr<object::Object> Evaluator::eval_integer_infix_expression(
        const std::string& op,
        const object::Object* left,
        const object::Object* right) const {
    auto left_val = left->cast<object::Integer>();
    auto right_val = right->cast<object::Integer>();

    if (op == "+") {
        return std::make_shared<object::Integer>(left_val->value() + right_val->value());
    } else if (op == "-") {
        return std::make_shared<object::Integer>(left_val->value() - right_val->value());
    } else if (op == "*") {
        return std::make_shared<object::Integer>(left_val->value() * right_val->value());
    } else if (op == "/") {
        return std::make_shared<object::Integer>(left_val->value() / right_val->value());
    } else if (op == "<") {
        return native_bool_to_boolean_object(left_val->value() < right_val->value());
    } else if (op == ">") {
        return native_bool_to_boolean_object(left_val->value() > right_val->value());
    } else if (op == "==") {
        return native_bool_to_boolean_object(left_val->value() == right_val->value());
    } else if (op == "!=") {
        return native_bool_to_boolean_object(left_val->value() != right_val->value());
    }

    return _null;
}

std::shared_ptr<object::Object> Evaluator::eval_infix_expression(
        const std::string& op,
        const object::Object* left,
        const object::Object* right) const {
    // 如果你想对其它类型做单独处理，改这个位置即可
    // 原作者在其书中调侃：十年后，当 Monkey 语言出名后，可能会有人在 stackoverflow 上提问：
    // 为什么在 Monkey 语言中(当前我们的项目叫 Autum)，整型值的比较比其它类型要慢呢？
    // 此时你可以回复：balabala...，来自：M78 星云，Allen
    if (typeid(*left) == typeid(object::Integer)
            && typeid(*right) == typeid(object::Integer)) {
        return eval_integer_infix_expression(op, left, right);
    } else if (op == "==") {
        // 非整数类型，直接比较对象指针
        // 如果是 Boolean 类型，因为全局都是用的同一份，所以指针相同
        return native_bool_to_boolean_object(left == right);
    } else if (op == "!=") {
        return native_bool_to_boolean_object(left != right);
    }
    return _null;
}

bool Evaluator::is_truthy(const object::Object* obj) const {
    if (obj == _null.get()) {
        return false;
    } else if (obj == _true.get()) {
        return true;
    } else if (obj == _false.get()) {
        return false;
    }
    return true;
}

std::shared_ptr<object::Object> Evaluator::eval_if_expression(
            const ast::IfExpression* exp) const {
    if (exp->condition() == nullptr) {
        return _null;
    }
    auto condition = eval(exp->condition());

    if (is_truthy(condition.get()) && exp->consequence() != nullptr) {
        return eval(exp->consequence());
    } else if (exp->alternative() != nullptr) {
        return eval(exp->alternative());
    }

    return _null;
}

} // namespace autumn
