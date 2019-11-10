#include "parser.h"

namespace autumn {

const std::vector<std::string>& Parser::errors() const {
    return _errors;
}
std::unique_ptr<Program> Parser::parse(const std::string& input) {
    Lexer lexer(input);
    _lexer = &lexer;
    _current_token = Token{Token::ILLEGAL, ""};
    _peek_token = Token{Token::ILLEGAL, ""};

    return parse();
}

std::unique_ptr<Program> Parser::parse() {
    std::unique_ptr<Program> program(new Program);

    next_token();
    while (!current_token_is(Token::END)) {
        auto stmt = parse_statment();
        if (stmt != nullptr) {
            program->append(stmt.release());
        }
        next_token();
    }

    return std::unique_ptr<Program>(std::move(program));
}

void Parser::next_token() {
    _current_token = _peek_token;
    _peek_token = _lexer->next_token();
}

void Parser::peek_error(Token::Type type) {
    std::string error = "expected next token to be "
        + Token::to_string(type) 
        + ", got "
        + _peek_token.to_string()
        + " instead. literal `"
        + _peek_token.literal
        + "`";
    _errors.push_back(std::move(error));
}

bool Parser::expect_peek(Token::Type type) {
    if (peek_token_is(type)) {
        next_token();
        return true;
    }

    peek_error(type);
    return false;
}

bool Parser::current_token_is(Token::Type type) const {
    return _current_token.type == type;
}

bool Parser::peek_token_is(Token::Type type) const {
    return _peek_token.type == type;
}

std::unique_ptr<Statment> Parser::parse_statment() {
    switch (_current_token.type) {
    case Token::LET:
        return parse_let_statment();
    case Token::RETURN:
        return parse_return_statment();
    }

    return nullptr;
}

std::unique_ptr<Statment> Parser::parse_let_statment() {
    std::unique_ptr<LetStatment> stmt(new LetStatment(_current_token));

    // 查看下一个 token，并取出
    if (!expect_peek(Token::IDENT)) {
        return nullptr;
    }

    stmt->set_identifier(new Identifier(_current_token, _current_token.literal));

    // 标识符的下一个 token 必须是 = 号
    if (!expect_peek(Token::ASSIGN)) {
        return nullptr;
    }

    // 表达式解析部分，暂时跳过
    while (!current_token_is(Token::SEMICOLON)) {
        next_token();
    }

    return std::unique_ptr<Statment>(std::move(stmt));
}

std::unique_ptr<Statment> Parser::parse_return_statment() {
    std::unique_ptr<ReturnStatment> stmt(new ReturnStatment(_current_token));

    // 表达式解析部分，暂时跳过
    while (!current_token_is(Token::SEMICOLON)) {
        next_token();
    }

    return std::unique_ptr<Statment>(std::move(stmt));
}

};
