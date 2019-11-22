#include "object.h"


namespace autumn {
namespace object {

std::unordered_map<int, std::string> Type::_type_to_name = {
    {INTEGER_OBJECT, "INTEGER"},
    {BOOLEAN_OBJECT, "BOOLEAN"},
    {STRING_OBJECT, "STRING"},
    {ERROR_OBJECT, "ERROR"},
    {NULL_OBJECT, "NULL"},
    {RETURN_OBJECT, "RETURN"},
    {FUNCTION_OBJECT, "FUNCTION"},
    {BUILTIN_OBJECT, "BUILTIN"},
};

std::ostream& operator<<(std::ostream& out, const Type& type) {
    auto it = type._type_to_name.find(type._type);
    if (it != type._type_to_name.end()) {
        return out << it->second;
    } else {
        return out << '{' << it->first << '}';
    }
}

} // object
} // autumn
