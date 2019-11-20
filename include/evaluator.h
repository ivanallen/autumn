#pragma once

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

private:
    bool is_error(const object::Object* obj) const;
    std::shared_ptr<object::Object> eval(const ast::Node* node) const;
    std::shared_ptr<object::Object> eval_program(const std::vector<std::unique_ptr<ast::Statment>>& statments) const;
    std::shared_ptr<object::Object> eval_statments(const std::vector<std::unique_ptr<ast::Statment>>& statments) const;
    std::shared_ptr<object::Object> eval_prefix_expression(
            const std::string& op,
            const object::Object* object) const;
    std::shared_ptr<object::Object> eval_infix_expression(
            const std::string& op,
            const object::Object* left,
            const object::Object* right) const;
    std::shared_ptr<object::Object> eval_integer_infix_expression(
            const std::string& op,
            const object::Object* left,
            const object::Object* right) const;
    std::shared_ptr<object::Object> eval_bang_operator_expression(const object::Object* right) const;
    std::shared_ptr<object::Object> eval_minus_prefix_operator_expression(const object::Object* right) const;
    std::shared_ptr<object::Object> native_bool_to_boolean_object(bool input) const;
    std::shared_ptr<object::Object> eval_if_expression(
            const ast::IfExpression* exp) const;

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
};

} // namespace autumn
