#pragma once

#include <regex>
#include <sstream>
#include <string_view>

namespace autumn {

namespace {

std::tuple<size_t, size_t> search(std::string_view fmt) {
    std::cmatch cm;
    if (std::regex_search(fmt.cbegin(),
                fmt.cend(),
                cm,
                std::regex("\\{\\}|\\{:.*?\\}"),
                std::regex_constants::format_first_only)) {
        return {cm.position(0), cm[0].length()};
    }
    return {std::string::npos, 0};
}

}

template <typename T>
std::ostream& format_to_stream(std::ostream& out, std::string_view fmt, T&& t) {
    auto [found, len] = search(fmt);
    if (found == std::string::npos) {
        return out << fmt;
    }

    auto left = fmt.substr(0, found);
    auto right = fmt.substr(found + len);
    return out << left << std::forward<T>(t) << right;
}

template <typename T, typename... Args>
std::ostream& format_to_stream(std::ostream& out, std::string_view fmt, T&& t, Args&&... args) {
    auto [found, len] = search(fmt);
    if (found == std::string::npos) {
        return out << fmt;
    }

    auto left = fmt.substr(0, found);
    auto right = fmt.substr(found + len);

    out << left << std::forward<T>(t);

    return format_to_stream(out, right, std::forward<Args>(args)...);
}

template <typename... Args>
std::string format(std::string_view fmt, Args&&... args) {
    std::stringstream ss;
    format_to_stream(ss, fmt, std::forward<Args>(args)...);
    return ss.str();
}

} // namespace autumn
