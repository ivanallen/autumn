#pragma once

#include "object.h"
#include "parser.h"

namespace autumn {
 
class Evaluator {
public:
    Evaluator();
    std::unique_ptr<object::Object> eval(const std::string& input);

private:
    std::unique_ptr<object::Object> eval(const ast::Node* node) const;
    std::unique_ptr<object::Object> eval_statments(const std::vector<std::unique_ptr<ast::Statment>>& statments) const;

private:
    using EvalFunc = std::function<std::unique_ptr<object::Object>(const ast::Node* node)>;
    Parser _parser;
};

} // namespace autumn
