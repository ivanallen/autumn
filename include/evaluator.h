#pragma once

#include "environment.h"
#include "format.h"
#include "object.h"
#include "parser.h"

namespace autumn {
 
class Evaluator {
public:
    Evaluator();
    // 使用 shared_ptr 的原因是有些对象是可以共享复用的
    // 比如 true/false/null
    std::shared_ptr<const object::Object> eval(const std::string& input);

    void reset_env();
private:
    bool is_error(const object::Object* obj) const;
    std::shared_ptr<object::Object> eval(const ast::Node* node, std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_program(const std::vector<std::unique_ptr<ast::Statment>>& statments, std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_statments(const std::vector<std::unique_ptr<ast::Statment>>& statments, std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_prefix_expression(
            const std::string& op,
            const object::Object* object,
            std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_infix_expression(
            const std::string& op,
            const object::Object* left,
            const object::Object* right,
            std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_integer_infix_expression(
            const std::string& op,
            const object::Object* left,
            const object::Object* right,
            std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_string_infix_expression(
            const std::string& op,
            const object::Object* left,
            const object::Object* right,
            std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_bang_operator_expression(
            const object::Object* right) const;
    std::shared_ptr<object::Object> eval_minus_prefix_operator_expression(const object::Object* right) const;
    std::shared_ptr<object::Object> native_bool_to_boolean_object(bool input) const;
    std::shared_ptr<object::Object> eval_if_expression(
            const ast::IfExpression* exp,
            std::shared_ptr<object::Environment>& env) const;
    std::shared_ptr<object::Object> eval_identifier(
            const ast::Identifier* identifier,
            std::shared_ptr<object::Environment>& env) const;
    std::vector<std::shared_ptr<object::Object>> eval_expressions(
            const std::vector<std::unique_ptr<ast::Expression>>& exps,
            std::shared_ptr<object::Environment>& env) const;

    std::shared_ptr<object::Object> apply_function(
            const object::Object* fn,
            std::vector<std::shared_ptr<object::Object>>& args) const;
    std::shared_ptr<object::Environment> extend_function_env(
            const object::Function* fn,
            std::vector<std::shared_ptr<object::Object>>& args) const;
private:
    bool is_truthy(const object::Object* obj) const;

    template <typename... Args>
    std::shared_ptr<object::Error> new_error(std::string_view fmt, Args&&... args) const {
        return std::make_shared<object::Error>(format(fmt, std::forward<Args>(args)...));
    }

    std::shared_ptr<object::Error> new_error(std::string_view message) const {
        return std::make_shared<object::Error>(std::string(message));
    }
private:
    Parser _parser;
    std::shared_ptr<object::Object> _null;
    std::shared_ptr<object::Object> _true;
    std::shared_ptr<object::Object> _false;
    mutable std::shared_ptr<object::Environment> _env;
};

} // namespace autumn
