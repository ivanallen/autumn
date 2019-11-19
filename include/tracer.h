#pragma once

#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>

namespace autumn {

class Tracer {
public:
    Tracer() {
        const char* env = getenv("DEBUG_AUTUMN");
        if (env != nullptr && strcmp("1", env) == 0) {
            _debug_env = true;
        }
    }

    std::function<void()> trace(const std::string& message) {
        ++_level;
        print("BEGIN " + message);
        return std::bind(&Tracer::untrace, this, message);
    }

    void untrace(const std::string& message) {
        print("END " + message);
        --_level;
    }

    void reset() {
        _level = 0;
    }
private:
    void print(const std::string& message) {
        if (_debug_env) {
            std::cout << "\x1b[2m";
            if (_level > 1) {
                std::string ident(4*(_level - 1), ' ');
                std::cout << ident << ' ';
            }
            std::cout << message << "\x1b[0m" << std::endl;
        }
    }
private:
    int _level = 0;
    bool _debug_env = false;
};

} // namespace autumn
