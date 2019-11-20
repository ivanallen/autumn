#pragma once

#include <sstream>
#include <string_view>

namespace autumn {

template <typename T>
std::string format(std::string_view fmt, T&& t) {
    auto found = fmt.find("{}");
    if (found == std::string::npos) {
        return std::string(fmt);
    }

    auto left = fmt.substr(0, found);
    auto right = fmt.substr(found + 2);
    std::stringstream ss;
    ss << std::forward<T>(t);
    return std::string(left) + ss.str() + std::string(right);
}

template <typename T, typename... Args>
std::string format(std::string_view fmt, T&& t, Args&&... args) {
    auto found = fmt.find("{}");
    if (found == std::string::npos) {
        return std::string(fmt);
    }

    auto left = fmt.substr(0, found);
    auto right = fmt.substr(found + 2);
    std::stringstream ss;
    ss << std::forward<T>(t);
    std::string f = std::string(left) + ss.str() + std::string(right);
    return format(f, std::forward<Args>(args)...);
}

} // namespace autumn
