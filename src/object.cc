#include "object.h"


namespace autumn {
namespace object {

std::unordered_map<int, std::string> Type::_type_to_name = {
    {INTEGER, "INTEGER"},
    {BOOLEAN, "BOOLEAN"},
    {NULL_OBJECT, "NULL_OBJECT"},
    {RETURN_OBJECT, "RETURN_OBJECT"},
};

std::ostream& operator<<(std::ostream& out, const Type& type) {
    auto it = type._type_to_name.find(type._type);
    if (it != type._type_to_name.end()) {
        return out << it->second;
    } else {
        return out;
    }
}

} // object
} // autumn
