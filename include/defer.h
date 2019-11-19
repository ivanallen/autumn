#pragma once

#include <functional>

namespace autumn {

class Defer {
public:
    Defer(const std::function<void()>& fn) : _fn(fn) {}

    ~Defer() {
        try {
            if (_fn) {
                _fn();
            }
        } catch (...) {
            // do_nothing
        }
    }
private:
    std::function<void()> _fn;
};

} // namespace autumn
