#pragma once

#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"

namespace autumn {

class Tracer {
public:
    Tracer() {
        const char* env = getenv("DEBUG_AUTUMN");
        if (env != nullptr && strcmp("1", env) == 0) {
            _debug_env = true;
        }
    }

    std::function<void()> trace(const std::string& message, const std::string& token_literal) {
        ++_level;
        print(format("{:dark}BEGIN: {:message}: {:yellow}{:token}{:off}",
                    color::dark::dark,
                    message,
                    color::dark::yellow,
                    token_literal,
                    color::off));
        return std::bind(&Tracer::untrace, this, message, token_literal);
    }

    void untrace(const std::string& message, const std::string& token_literal) {
        print(format("{:dark}END: {:message}: {:yellow}{:token}{:off}",
                    color::dark::dark,
                    message,
                    color::dark::yellow,
                    token_literal,
                    color::off));
        --_level;
    }

    void reset() {
        _level = 0;
    }
private:
    void print(const std::string& message) {
        if (_debug_env) {
            if (_level > 1) {
                std::string ident(4*(_level - 1), ' ');
                std::cout << ident << ' ';
            }
            std::cout << message << std::endl;
        }
    }
private:
    int _level = 0;
    bool _debug_env = false;
};

} // namespace autumn
