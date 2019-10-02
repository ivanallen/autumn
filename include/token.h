#include <string>

namespace autumn {

struct Token {
    enum Type {
        UNDEFINED = 0,
        ASSIGN,
        PLUS,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        COMMA,
        SEMICOLON,
    };

    Type type;
    std::string literal;

    bool operator==(const Token& rhs) const;
};

} // namespace autumn
