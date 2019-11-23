#include "evaluator.h"
#include "builtin.h"

namespace autumn {

Evaluator::Evaluator() :
    _null(new object::Null()),
    _true(new object::Boolean(true)),
    _false(new object::Boolean(false)),
    _env(new object::Environment()) {
}
 
std::shared_ptr<const object::Object> Evaluator::eval(const std::string& input) {
    auto program = _parser.parse(input);
    return eval(program.get(), _env);
}

bool Evaluator::is_error(const object::Object* obj) const {
    return typeid(*obj) == typeid(object::Error);
}

void Evaluator::reset_env() {
    _env.reset(new object::Environment());
}

std::shared_ptr<object::Object> Evaluator::eval(
        const ast::Node* node,
        std::shared_ptr<object::Environment>& env) const {
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
        return eval_program(n->statments(), env);

    } else if (typeid(*node) == typeid(ast::ExpressionStatment)) {
        auto n = node->cast<ast::ExpressionStatment>();
        return eval(n->expression(), env);

    } else if (typeid(*node) == typeid(ast::BlockStatment)) {
        auto n = node->cast<ast::BlockStatment>();
        return eval_statments(n->statments(), env);

    } else if (typeid(*node) == typeid(ast::ReturnStatment)) {
        auto n = node->cast<ast::ReturnStatment>();
        auto return_val = eval(n->expression(), env);
        if (is_error(return_val.get())) {
            return return_val;
        }
        return std::make_shared<object::ReturnValue>(return_val);

    } else if (typeid(*node) == typeid(ast::LetStatment)) {
        auto n = node->cast<ast::LetStatment>();
        auto val = eval(n->expression(), env);
        if (is_error(val.get())) {
            return val;
        }

        env->set(n->identifier()->value(), val);

    } else if (typeid(*node) == typeid(ast::IntegerLiteral)) {
        auto n = node->cast<ast::IntegerLiteral>();
        return std::shared_ptr<object::Integer>(
                new object::Integer(n->value()));

    } else if (typeid(*node) == typeid(ast::BooleanLiteral)) {
        auto n = node->cast<ast::BooleanLiteral>();
        return n->value() ? _true : _false;

    } else if (typeid(*node) == typeid(ast::StringLiteral)) {
        auto n = node->cast<ast::StringLiteral>();
        return std::make_shared<object::String>(n->value());

    } else if (typeid(*node) == typeid(ast::ArrayLiteral)) {
        auto n = node->cast<ast::ArrayLiteral>();
        auto elems = eval_expressions(n->elements(), env);
        if (!elems.empty() && is_error(elems[0].get())) {
            return elems[0];
        }
        return std::make_shared<object::Array>(elems);

    } else if (typeid(*node) == typeid(ast::PrefixExpression)) {
        auto n = node->cast<ast::PrefixExpression>();
        auto right = eval(n->right(), env);
        if (is_error(right.get())) {
            return right;
        }
        return eval_prefix_expression(n->op(), right.get(), env);

    } else if (typeid(*node) == typeid(ast::InfixExpression)) {
        auto n = node->cast<ast::InfixExpression>();
        auto left = eval(n->left(), env);
        if (is_error(left.get())) {
            return left;
        }

        auto right = eval(n->right(), env);
        if (is_error(right.get())) {
            return right;
        }

        return eval_infix_expression(n->op(), left.get(), right.get(), env);

    } else if (typeid(*node) == typeid(ast::IfExpression)) {
        return eval_if_expression(node->cast<ast::IfExpression>(), env);

    } else if (typeid(*node) == typeid(ast::Identifier)) {
        return eval_identifier(node->cast<ast::Identifier>(), env);

    } else if (typeid(*node) == typeid(ast::FunctionLiteral)) {
        auto n = node->cast<ast::FunctionLiteral>();
        return std::make_shared<object::Function>(
                n->parameters(), n->body(), env);

    } else if (typeid(*node) == typeid(ast::CallExpression)) {
        auto n = node->cast<ast::CallExpression>();
        auto function = eval(n->function(), env);
        if (is_error(function.get())) {
            return function;
        }

        auto args = eval_expressions(n->arguments(), env);
        if (!args.empty() && is_error(args[0].get())) {
            return args[0];
        }

        return apply_function(function.get(), args);

    } else if (typeid(*node) == typeid(ast::IndexExpression)) {
        auto n = node->cast<ast::IndexExpression>();
        auto array = eval(n->left(), env);
        if (is_error(array.get())) {
            return array;
        }

        auto index = eval(n->index(), env);
        if (is_error(index.get())) {
            return index;
        }

        return eval_index_expression(array.get(), index.get());

    }

    return nullptr;
}

std::shared_ptr<object::Object> Evaluator::eval_index_expression(
        const object::Object* array,
        const object::Object* index) const {
    if (typeid(*array) == typeid(object::Array)
            && typeid(*index) == typeid(object::Integer)) {
        auto a = array->cast<object::Array>();
        auto i = index->cast<object::Integer>();

        auto& elems = a->elements();
        auto idx = i->value();

        if (idx < 0) {
            idx += elems.size();
        }

        if (idx < 0 || idx >= elems.size()) {
            return _null;
        }

        return elems[idx];
    }
    return new_error("index operator not supported: `{}`", array->type());
}

std::shared_ptr<object::Object> Evaluator::apply_function(
        const object::Object* fn,
        std::vector<std::shared_ptr<object::Object>>& args) const {

    std::shared_ptr<object::Object> val = _null;

    if (typeid(*fn) == typeid(object::Function)) {
        auto function = fn->cast<object::Function>();
        auto extended_env = extend_function_env(function, args);
        // 开始执行函数体内的语句
        val = eval(function->body(), extended_env);
    } else if (typeid(*fn) == typeid(object::Builtin)) {
        auto builtin_fn = fn->cast<object::Builtin>();
        val = builtin_fn->run(args);
    }

    if (typeid(*val) == typeid(object::ReturnValue)) {
        return val->cast<object::ReturnValue>()->value();
    }
    return val;
}

std::shared_ptr<object::Environment> Evaluator::extend_function_env(
        const object::Function* fn,
        std::vector<std::shared_ptr<object::Object>>& args) const {
    auto new_env = std::make_shared<object::Environment>(fn->env());
    auto& params = fn->parameters();

    for (size_t i = 0; i < params.size() && i < args.size(); ++i) {
        new_env->set(params[i]->value(), args[i]);
    }
    return new_env;
}

std::vector<std::shared_ptr<object::Object>> Evaluator::eval_expressions(
        const std::vector<std::unique_ptr<ast::Expression>>& exps,
        std::shared_ptr<object::Environment>& env) const {
    std::vector<std::shared_ptr<object::Object>> results;
    for (auto& exp : exps) {
        auto val = eval(exp.get(), env);
        if (is_error(val.get())) {
            return { val };
        }
        results.emplace_back(val);
    }
    return results;
}

std::shared_ptr<object::Object> Evaluator::eval_program(
        const std::vector<std::unique_ptr<ast::Statment>>& statments,
        std::shared_ptr<object::Environment>& env) const {
    std::shared_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get(), env);
        if (result == nullptr) {
            continue;
        }

        if (typeid(*result) == typeid(object::ReturnValue)) {
            return result->cast<object::ReturnValue>()->value();
        } else if (typeid(*result) == typeid(object::Error)) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<object::Object> Evaluator::eval_identifier(
        const ast::Identifier* identifier,
        std::shared_ptr<object::Environment>& env) const {
    auto val = env->get(identifier->value());
    if (val != nullptr) {
        return val;
    }

    auto builtin_fn = builtin::BUILTINS.find(identifier->value());
    if (builtin_fn != builtin::BUILTINS.end()) {
        return std::make_shared<object::Builtin>(builtin_fn->second);
    }

    return new_error("identifier not found: {}", identifier->value());
}

std::shared_ptr<object::Object> Evaluator::eval_statments(
        const std::vector<std::unique_ptr<ast::Statment>>& statments,
        std::shared_ptr<object::Environment>& env) const {
    std::shared_ptr<object::Object> result;

    for (auto& stat : statments) {
        result = eval(stat.get(), env);
        if (typeid(*result) == typeid(object::ReturnValue)
                || typeid(*result) == typeid(object::Error)) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<object::Object> Evaluator::eval_prefix_expression(
        const std::string& op,
        const object::Object* right,
        std::shared_ptr<object::Environment>& env) const {
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
        const object::Object* right,
        std::shared_ptr<object::Environment>& env) const {
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

std::shared_ptr<object::Object> Evaluator::eval_string_infix_expression(
        const std::string& op,
        const object::Object* left,
        const object::Object* right,
        std::shared_ptr<object::Environment>& env) const {
    auto left_val = left->cast<object::String>();
    auto right_val = right->cast<object::String>();

    if (op == "+") {
        return std::make_shared<object::String>(left_val->value() + right_val->value());
    }

    return new_error("unknown operator: {} {} {}", left->type(), op, right->type());
}

std::shared_ptr<object::Object> Evaluator::eval_infix_expression(
        const std::string& op,
        const object::Object* left,
        const object::Object* right,
        std::shared_ptr<object::Environment>& env) const {
    // 如果你想对其它类型做单独处理，改这个位置即可
    // 原作者在其书中调侃：十年后，当 Monkey 语言出名后，可能会有人在 stackoverflow 上提问：
    // 为什么在 Monkey 语言中(当前我们的项目叫 Autum)，整型值的比较比其它类型要慢呢？
    // 此时你可以回复：balabala...，来自：M78 星云，Allen
    if (typeid(*left) == typeid(object::Integer)
            && typeid(*right) == typeid(object::Integer)) {
        return eval_integer_infix_expression(op, left, right, env);
    } else if (typeid(*left) == typeid(object::String)
            && typeid(*right) == typeid(object::String)) {
        return eval_string_infix_expression(op, left, right, env);
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
            const ast::IfExpression* exp,
            std::shared_ptr<object::Environment>& env) const {
    if (exp->condition() == nullptr) {
        return _null;
    }
    auto condition = eval(exp->condition(), env);
    if (is_error(condition.get())) {
        return condition;
    }

    if (is_truthy(condition.get()) && exp->consequence() != nullptr) {
        return eval(exp->consequence(), env);
    } else if (exp->alternative() != nullptr) {
        return eval(exp->alternative(), env);
    }

    return _null;
}

} // namespace autumn
