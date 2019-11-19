#pragma once

#include "object.h"
#include "parser.h"

namespace autumn {
 
class Evaluator {
public:
    Evaluator();
    const std::shared_ptr<object::Object> eval(const std::string& input);

private:
    std::shared_ptr<object::Object> eval(const ast::Node* node) const;
    std::shared_ptr<object::Object> eval_statments(const std::vector<std::unique_ptr<ast::Statment>>& statments) const;
    std::shared_ptr<object::Object> eval_prefix_expression(const std::string& op, const object::Object* object) const;
    std::shared_ptr<object::Object> eval_bang_operator_expression(const object::Object* right) const;
    std::shared_ptr<object::Object> eval_minus_prefix_operator_expression(const object::Object* right) const;

private:
    Parser _parser;
private:
    std::shared_ptr<object::Object> _null;
    std::shared_ptr<object::Object> _true;
    std::shared_ptr<object::Object> _false;
};

} // namespace autumn
