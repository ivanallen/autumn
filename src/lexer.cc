#include "lexer.h"
#include <algorithm>
namespace autumn {

Lexer::Lexer(const std::string& input) :
    _input(input) {
    read_char();
}

Token Lexer::next_token() {
    skip_whitespace();
    Token token;

    switch (_ch) {
    case '=':
        if (peek_char() == '=') {
            read_char();
            token = Token{Token::EQ, "=="};
        } else {
            token = Token{Token::ASSIGN, "="};
        }
        break;
    case '+':
        token = Token{Token::PLUS, "+"};
        break;
    case '-':
        token = Token{Token::MINUS, "-"};
        break;
    case '*':
        token = Token{Token::ASTERISK, "*"};
        break;
    case '/':
        token = Token{Token::SLASH, "/"};
        break;
    case '!':
        if (peek_char() == '=') {
            read_char();
            token = Token{Token::NEQ, "!="};
        } else {
            token = Token{Token::BANG, "!"};
        }
        break;
    case '<':
        if (peek_char() == '=') {
            read_char();
            token = Token{Token::LTE, "<="};
        } else {
            token = Token{Token::LT, "<"};
        }
        break;
    case '>':
        if (peek_char() == '=') {
            read_char();
            token = Token{Token::GTE, ">="};
        } else {
            token = Token{Token::GT, ">"};
        }
        break;
    case '(':
        token = Token{Token::LPAREN, "("};
        break;
    case ')':
        token = Token{Token::RPAREN, ")"};
        break;
    case '{':
        token = Token{Token::LBRACE, "{"};
        break;
    case '}':
        token = Token{Token::RBRACE, "}"};
        break;
    case ',':
        token = Token{Token::COMMA, ","};
        break;
    case ';':
        token = Token{Token::SEMICOLON, ";"};
        break;
    case 0:
        token = Token{Token::END, ""};
        break;
    default:
        if (is_letter(_ch)) {
            auto ident = read_identifier();
            return Token{Token::lookup(ident), ident};
        } else if (is_digital(_ch)) {
            auto num = read_number();
            return Token{Token::INT, num};
        } else {
            return Token{Token::ILLEGAL, ""};
        }
    }

    read_char();
    return token;
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

char Lexer::peek_char() const {
    if (_read_pos >= _input.length()) {
        return 0;
    } else {
        return _input[_read_pos];
    }
}

bool Lexer::is_letter(char c) const {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool Lexer::is_digital(char c) const {
    return '0' <= c && c <= '9';
}

void Lexer::skip_whitespace() {
    static char whitespace[] = {' ', '\n', '\r', '\t'};
    while (std::find(std::begin(whitespace),
            std::end(whitespace),
            _ch) != std::end(whitespace)) {
        read_char();
    }
}

std::string Lexer::read_identifier() {
    int pos = _pos;
    while (is_letter(_ch)) {
        read_char();
    }
    return _input.substr(pos, _pos - pos);
}

std::string Lexer::read_number() {
    int pos = _pos;
    while (is_digital(_ch)) {
        read_char();
    }
    return _input.substr(pos, _pos - pos);
}

} // namespace autumn

