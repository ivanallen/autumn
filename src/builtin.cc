#include "builtin.h"
#include "format.h"

namespace autumn {
namespace builtin {

std::map<std::string, object::BuiltinFunction> BUILTINS = {
    {"len", len},
};

std::shared_ptr<object::Object> len(const std::vector<std::shared_ptr<object::Object>>& args) {
    if (args.size() != 1) {
        return std::make_shared<object::Error>(format("wrong number of arguments. expected 1, got {}", args.size()));
    }

    auto& arg = args[0];

    if (typeid(*arg) == typeid(object::String)) {
        auto string_obj = arg->cast<object::String>();
        return std::make_shared<object::Integer>(string_obj->value().length());
    }
    return std::make_shared<object::Error>(format("argument to `len` not supported, got {}", arg->type()));
}

} // namespace builtin
} // namespace autumn
