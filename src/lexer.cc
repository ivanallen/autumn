#include "lexer.h"
namespace autumn {

Lexer::Lexer(const std::string& input) :
    _input(input) {
}

Token Lexer::next_token() {
    read_char();
    switch (_ch) {
    case '=':
        return Token{Token::ASSIGN, "="};
    case '+':
        return Token{Token::PLUS, "+"};
    case '(':
        return Token{Token::LPAREN, "("};
    case ')':
        return Token{Token::RPAREN, ")"};
    case '{':
        return Token{Token::LBRACE, "{"};
    case '}':
        return Token{Token::RBRACE, "}"};
    case ',':
        return Token{Token::COMMA, ","};
    case ';':
        return Token{Token::SEMICOLON, ";"};
    }

    return Token();
}

void Lexer::read_char() {
    if (_read_pos >= _input.length()) {
        _ch = 0;
    } else {
        _ch = _input[_read_pos];
    }
    _pos = _read_pos;
    ++_read_pos;
}

} // namespace autumn

