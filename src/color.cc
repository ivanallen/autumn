#include "color.h"

namespace autumn {
namespace color {

Color::Color(const std::string_view& c) :
        _color(c) {
    const char* env(getenv("AUTUMN_COLOR_OFF"));
    if (env != nullptr) {
        _color_env = false;
    }
}


std::string operator+(const Color& c, const std::string& rhs) {
    if (c._color_env) {
        return std::string(c._color) + rhs;
    }
    return rhs;
}

std::string operator+(const std::string& lhs, const Color& c) {
    if (c._color_env) {
        return lhs + std::string(c._color);
    }
    return lhs;
}

std::ostream& operator<<(std::ostream& out, const Color& c) {
    if (c._color_env) {
        return out << c._color;
    }
    return out;
}

// Reset
const Color off("\x1b[0m");       // Text Reset

// Regular Colors
const Color black("\x1b[0;30m");        // Black
const Color red("\x1b[0;31m");          // Red
const Color green("\x1b[0;32m");        // Green
const Color yellow("\x1b[0;33m");       // Yellow
const Color blue("\x1b[0;34m");         // Blue
const Color purple("\x1b[0;35m");       // Purple
const Color cyan("\x1b[0;36m");         // Cyan
const Color white("\x1b[0;37m");        // White

// Light 
namespace light {

const Color light("\x1b[1m");       // Black
const Color black("\x1b[1;30m");       // Black
const Color red("\x1b[1;31m");         // Red
const Color green("\x1b[1;32m");       // Green
const Color yellow("\x1b[1;33m");      // Yellow
const Color blue("\x1b[1;34m");        // Blue
const Color purple("\x1b[1;35m");      // Purple
const Color cyan("\x1b[1;36m");        // Cyan
const Color white("\x1b[1;37m");       // White

} // namespace bold

// Light 
namespace dark {

const Color dark("\x1b[2m");       // Black
const Color black("\x1b[2;30m");       // Black
const Color red("\x1b[2;31m");         // Red
const Color green("\x1b[2;32m");       // Green
const Color yellow("\x1b[2;33m");      // Yellow
const Color blue("\x1b[2;34m");        // Blue
const Color purple("\x1b[2;35m");      // Purple
const Color cyan("\x1b[2;36m");        // Cyan
const Color white("\x1b[2;37m");       // White

} // namespace dark

// Underline
namespace underline{

const Color black("\x1b[4;30m");       // Black
const Color red("\x1b[4;31m");         // Red
const Color green("\x1b[4;32m");       // Green
const Color yellow("\x1b[4;33m");      // Yellow
const Color blue("\x1b[4;34m");        // Blue
const Color purple("\x1b[4;35m");      // Purple
const Color cyan("\x1b[4;36m");        // Cyan
const Color white("\x1b[4;37m");       // White

} // namespace underline

// Background
namespace background {

const Color black("\x1b[40m");       // Black
const Color red("\x1b[41m");         // Red
const Color green("\x1b[42m");       // Green
const Color yellow("\x1b[43m");      // Yellow
const Color blue("\x1b[44m");        // Blue
const Color purple("\x1b[45m");      // Purple
const Color cyan("\x1b[46m");        // Cyan
const Color white("\x1b[47m");       // White

}

// High Intensity
namespace intensity {

const Color black("\x1b[0;90m");       // Black
const Color red("\x1b[0;91m");         // Red
const Color green("\x1b[0;92m");       // Green
const Color yellow("\x1b[0;93m");      // Yellow
const Color blue("\x1b[0;94m");        // Blue
const Color purple("\x1b[0;95m");      // Purple
const Color cyan("\x1b[0;96m");        // Cyan
const Color white("\x1b[0;97m");       // White

} // namespace intentsiY

// Bold High Intensity
namespace bold {
namespace intensity {

const Color black("\x1b[1;90m");      // Black
const Color red("\x1b[1;91m");        // Red
const Color green("\x1b[1;92m");      // Green
const Color yellow("\x1b[1;93m");     // Yellow
const Color blue("\x1b[1;94m");       // Blue
const Color purple("\x1b[1;95m");     // Purple
const Color cyan("\x1b[1;96m");       // Cyan
const Color white("\x1b[1;97m");      // White

} // namespace intensity
} // namespace bold

// High Intensity backgrounds
namespace background {
namespace intensity {

const Color black("\x1b[0;100m");   // Black
const Color red("\x1b[0;101m");     // Red
const Color green("\x1b[0;102m");   // Green
const Color yellow("\x1b[0;103m");  // Yellow
const Color blue("\x1b[0;104m");    // Blue
const Color purple("\x1b[0;105m");  // Purple
const Color cyan("\x1b[0;106m");    // Cyan
const Color white("\x1b[0;107m");   // White

} // namespace intensity
} // namespace background

} // namespace color
} // namespace autumn
