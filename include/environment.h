#pragma once

#include <memory>
#include <string>
#include <map>

namespace autumn {
namespace object {

class Object;
class Environment {
public:
    std::shared_ptr<Object> get(const std::string& name) {
        auto it = _store.find(name);
        if (it == _store.end()) {
            return nullptr;
        }

        return it->second;
    }

    std::shared_ptr<Object> set(const std::string& name,
            std::shared_ptr<Object>& val) {
        _store[name] = val;
        return val;
    }
private:
    std::map<std::string, std::shared_ptr<Object>> _store;
};

} // namespace object
} // namespace autumn
