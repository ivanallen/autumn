#pragma once

#include <iostream>
#include <map>
#include <string>

namespace autumn {

struct Token {
    enum Type {
        ILLEGAL = 0,
        ASSIGN,
        PLUS,
        MINUS,
        ASTERISK,
        SLASH,
        BANG,
        LT,
        LTE,
        GT,
        GTE,
        EQ,
        NEQ,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
        COMMA,
        SEMICOLON,
        LET,
        IDENT,
        FUNCTION,
        INT,
        TRUE,
        FALSE,
        IF,
        ELSE,
        RETURN,
        STRING,
        END,
    };

    static std::map<std::string, Token::Type> keywords;
    static Type lookup(const std::string& token);

    static const std::string& to_string(Token::Type type);

    const std::string& to_string() const;

    Type type;
    std::string literal;

    bool operator==(const Token& rhs) const;
    friend std::ostream& operator<<(std::ostream& out, const Token& token);
};

std::ostream& operator<<(std::ostream& out, const Token& token);

} // namespace autumn
