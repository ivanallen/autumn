#include "token.h"

namespace autumn {
std::map<std::string, Token::Type> Token::keywords = {
    {"fn", Token::FUNCTION},
    {"let", Token::LET},
    {"true", Token::TRUE},
    {"false", Token::FALSE},
    {"if", Token::IF},
    {"else", Token::ELSE},
    {"return", Token::RETURN},
};

static std::map<Token::Type, std::string> s_token_type = {
    {Token::ILLEGAL, "ILLEGAL"},
    {Token::ASSIGN, "ASSIGN"},
    {Token::PLUS, "PLUS"},
    {Token::MINUS, "MINUS"},
    {Token::ASTERISK, "ASTERISK"},
    {Token::SLASH, "SLASH"},
    {Token::BANG, "BANG"},
    {Token::LT, "LT"},
    {Token::LTE, "LTE"},
    {Token::GT, "GT"},
    {Token::GTE, "GTE"},
    {Token::EQ, "EQ"},
    {Token::NEQ, "NEQ"},
    {Token::LPAREN, "LPAREN"},
    {Token::RPAREN, "RPAREN"},
    {Token::LBRACE, "LBRACE"},
    {Token::RBRACE, "RBRACE"},
    {Token::COMMA, "COMMA"},
    {Token::SEMICOLON, "SEMICOLON"},
    {Token::LET, "LET"},
    {Token::IDENT, "IDENT"},
    {Token::FUNCTION, "FUNCTION"},
    {Token::INT, "INT"},
    {Token::TRUE, "TRUE"},
    {Token::FALSE, "FALSE"},
    {Token::IF, "IF"},
    {Token::ELSE, "ELSE"},
    {Token::RETURN, "RETURN"},
    {Token::STRING, "STRING"},
    {Token::END, "END"},
};

bool Token::operator==(const Token& rhs) const {
    return type == rhs.type && literal == rhs.literal;
}

Token::Type Token::lookup(const std::string& token) {
    auto it = Token::keywords.find(token);
    if (it != Token::keywords.end()) {
        return it->second;
    }

    return Token::IDENT;
}

const std::string& Token::to_string(Token::Type type) {
    return s_token_type[type];
}

const std::string& Token::to_string() const {
    return s_token_type[type];
}

std::ostream& operator<<(std::ostream& out, const Token& token) {
    auto it = s_token_type.find(token.type);
    std::string type = "ERROR";
    if (it != s_token_type.end()) {
        type = it->second;
    }
    out << '{' << type << ',' << token.literal << '}';
    return out;
}

}
