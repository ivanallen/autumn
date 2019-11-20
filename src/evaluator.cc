#include "evaluator.h"

namespace autumn {

Evaluator::Evaluator() :
    _null(new object::Null()),
    _true(new object::Boolean(true)),
    _false(new object::Boolean(false)),
    _env(new object::Environment()) {
}
 
std::shared_ptr<const object::Object> Evaluator::eval(const std::string& input) {
    auto program = _parser.parse(input);
    return eval(program.get());
}

bool Evaluator::is_error(const object::Object* obj) const {
    return typeid(*obj) == typeid(object::Error);
}

void Evaluator::reset_env() {
    _env.reset(new object::Environment());
}

std::shared_ptr<object::Object> Evaluator::eval(const ast::Node* node) const {
    if (node == nullptr) {
        std::string message;
        for (size_t i = 0; i < _parser.errors().size(); ++i) {
            auto& error = _parser.errors()[i];
            if (i != 0) {
                message.append(1, '\n');
            }
            message.append(error);
        }
        return new_error("abort: {}", message);
    }

    if (typeid(*node) == typeid(ast::Program)) {
        auto n = node->cast<ast::Program>();
        return eval_program(n->statments());
    } else if (typeid(*node) == typeid(ast::ExpressionStatment)) {
        auto n = node->cast<ast::ExpressionStatment>();
        return eval(n->expression());
    } else if (typeid(*node) == typeid(ast::BlockStatment)) {
        auto n = node->cast<ast::BlockStatment>();
        return eval_statments(n->statments());
    } else if (typeid(*node) == typeid(ast::ReturnStatment)) {
        auto n = node->cast<ast::ReturnStatment>();
        auto return_val = eval(n->expression());
        if (is_error(return_val.get())) {
            return return_val;
        }
        return std::make_shared<object::ReturnValue>(return_val);
    } else if (typeid(*node) == typeid(ast::LetStatment)) {
        auto n = node->cast<ast::LetStatment>();
        auto val = eval(n->expression());
        if (is_error(val.get())) {
            return val;
        }

        _env->set(n->identifier()->value(), val);
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
        if (is_error(right.get())) {
            return right;
        }
        return eval_prefix_expression(n->op(), right.get());
    } else if (typeid(*node) == typeid(ast::InfixExpression)) {
        auto n = node->cast<ast::InfixExpression>();
        auto left = eval(n->left());
        if (is_error(left.get())) {
            return left;
        }
        auto right = eval(n->right());
        if (is_error(right.get())) {
            return right;
        }
        return eval_infix_expression(n->op(), left.get(), right.get());
    } else if (typeid(*node) == typeid(ast::IfExpression)) {
        return eval_if_expression(node->cast<ast::IfExpression>());
    } else if (typeid(*node) == typeid(ast::Identifier)) {
        return eval_identifier(node->cast<ast::Identifier>());
    }

    return _null;
}

std::shared_ptr<object::Object> Evaluator::eval_program(
        const std::vector<std::unique_ptr<ast::Statment>>& statments) const {
    std::shared_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get());
        if (typeid(*result) == typeid(object::ReturnValue)) {
            return result->cast<object::ReturnValue>()->value();
        } else if (typeid(*result) == typeid(object::Error)) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<object::Object> Evaluator::eval_identifier(const ast::Identifier* identifier) const {
    auto val = _env->get(identifier->value());
    if (val == nullptr) {
        return new_error("identifier not found: {}", identifier->value());
    }
    return val;
}

std::shared_ptr<object::Object> Evaluator::eval_statments(
        const std::vector<std::unique_ptr<ast::Statment>>& statments) const {
    std::shared_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get());
        if (typeid(*result) == typeid(object::ReturnValue)
                || typeid(*result) == typeid(object::Error)) {
            return result;
        }
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

    return new_error("unknown operator: {}{}", op, right->type());
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
        return new_error("unknown operator: -{}", right->type());
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

    return new_error("unknown operator: {} {} {}", left->type(), op, right->type());
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
    } else if (typeid(*left) != typeid(*right)) {
        return new_error("type mismatch: {} {} {}", left->type(), op, right->type());
    } else if (op == "==") {
        // 非整数类型，直接比较对象指针
        // 如果是 Boolean 类型，因为全局都是用的同一份，所以指针相同
        return native_bool_to_boolean_object(left == right);
    } else if (op == "!=") {
        return native_bool_to_boolean_object(left != right);
    }
    return new_error("unknown operator: {} {} {}", left->type(), op, right->type());
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
    if (is_error(condition.get())) {
        return condition;
    }

    if (is_truthy(condition.get()) && exp->consequence() != nullptr) {
        return eval(exp->consequence());
    } else if (exp->alternative() != nullptr) {
        return eval(exp->alternative());
    }

    return _null;
}

} // namespace autumn
