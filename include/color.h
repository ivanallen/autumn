#pragma once
#include <ostream>
#include <string_view>

namespace autumn {
namespace color {

class Color {
public:
    Color(const std::string_view& c);
    friend std::string operator+(const Color& c, const std::string& rhs);
    friend std::string operator+(const std::string& rhs, const Color& c);
    friend std::ostream& operator<<(std::ostream&, const Color&);

private:
    const std::string_view _color;
    bool _color_env = true;
};

// Reset
extern const Color off; // Text Reset

// Regular Colors
extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White

// Light
namespace light {
extern const Color light;       // light 
extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White
}

// Light 
namespace dark {

extern const Color dark;        // Dark
extern const Color black;       // Black
extern const Color red;         // Red
extern const Color green;       // Green
extern const Color yellow;      // Yellow
extern const Color blue;        // Blue
extern const Color purple;      // Purple
extern const Color cyan;        // Cyan
extern const Color white;       // White

} // namespace dark

// Underline
namespace underline {

extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White

}

// Background
namespace background {
extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White
}

// High Intensity
namespace intensity {

extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White

}

// Bold High IntensIty
namespace bold {
namespace intensity {

extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White

}
}

// High Intensity bAckgrounds
namespace background {
namespace intensity {

extern const Color black; // Black
extern const Color red; // Red
extern const Color green; // Green
extern const Color yellow; // Yellow
extern const Color blue; // Blue
extern const Color purple; // Purple
extern const Color cyan; // Cyan
extern const Color white; // White

}
}

} // namespace color
} // namespace autumn
