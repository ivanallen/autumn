#include "parser.h"

namespace autumn {

Program::Program(const std::string& input) : _lexer(input) {
    next_token();
    parse();
}

const std::vector<std::unique_ptr<Statment>>& Program::statments() const {
    return _statments;
}

bool Program::parse() {
    while (_current_token.type != Token::END) {
        auto stmt = parse_statment();
        if (stmt != nullptr) {
            _statments.push_back(std::move(stmt));
        }
        next_token();
    }

    return true;
}

void Program::next_token() {
    _current_token = _peek_token;
    _peek_token = _lexer.next_token();
}

bool Program::expect_peek(Token::Type type) {
    if (_peek_token.type == type) {
        next_token();
        return true;
    }

    return false;
}

std::unique_ptr<Statment> Program::parse_statment() {
    switch (_current_token.type) {
    case Token::LET:
        return parse_let_statment();
        break;
    }

    return nullptr;
}

std::unique_ptr<Statment> Program::parse_let_statment() {
    std::unique_ptr<LetStatment> stmt(new LetStatment(_current_token));
    if (!expect_peek(Token::IDENT)) {
        return nullptr;
    }
    stmt->set_identifier(new Identifier(_current_token, _current_token.literal));
    return std::unique_ptr<Statment>(std::move(stmt));
}

};
