#include "parser.h"

namespace autumn {

namespace {

// 操作符优先级表
const std::unordered_map<Token::Type, Parser::Precedence> PRECEDENCES = {
    {Token::EQ, Parser::Precedence::EQUALS},
    {Token::NEQ, Parser::Precedence::EQUALS},
    {Token::LT, Parser::Precedence::LESSGREATER},
    {Token::GT, Parser::Precedence::LESSGREATER},
    {Token::PLUS, Parser::Precedence::SUM},
    {Token::MINUS, Parser::Precedence::SUM},
    {Token::SLASH, Parser::Precedence::PRODUCT},
    {Token::ASTERISK, Parser::Precedence::PRODUCT},
};

}

Parser::Parser() {
    using namespace std::placeholders;
    // 注册前缀解析函数
    _prefix_parse_funcs[Token::IDENT] = std::bind(&Parser::parse_identifier, this);
    _prefix_parse_funcs[Token::INT] = std::bind(&Parser::parse_integer_literal, this);
    _prefix_parse_funcs[Token::BANG] = std::bind(&Parser::parse_prefix_expression, this);
    _prefix_parse_funcs[Token::MINUS] = std::bind(&Parser::parse_prefix_expression, this);

    // 注册中缀解析函数
    _infix_parse_funcs[Token::PLUS] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::MINUS] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::ASTERISK] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::SLASH] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::EQ] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::NEQ] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::LT] = std::bind(&Parser::parse_infix_expression, this, _1);
    _infix_parse_funcs[Token::GT] = std::bind(&Parser::parse_infix_expression, this, _1);
}

const std::vector<std::string>& Parser::errors() const {
    return _errors;
}

std::unique_ptr<ast::Program> Parser::parse(const std::string& input) {
    Lexer lexer(input);
    _lexer = &lexer;
    _errors.clear();

    next_token();
    next_token();

    return parse();
}

std::unique_ptr<ast::Program> Parser::parse() {
    std::unique_ptr<ast::Program> program(new ast::Program);

    while (!current_token_is(Token::END)) {
        auto stmt = parse_statment();
        if (stmt != nullptr) {
            program->append(stmt.release());
        }
        next_token();
    }

    return program;
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

Parser::Precedence Parser::current_precedence() const {
    auto it = PRECEDENCES.find(_current_token.type);
    if (it != PRECEDENCES.end()) {
        return it->second;
    }
    return Precedence::LOWEST;
}

Parser::Precedence Parser::peek_precedence() const {
    auto it = PRECEDENCES.find(_peek_token.type);
    if (it != PRECEDENCES.end()) {
        return it->second;
    }
    return Precedence::LOWEST;
}

std::unique_ptr<ast::Statment> Parser::parse_statment() {
    // debug
    // std::cout << "parse:" << _current_token << std::endl;
    switch (_current_token.type) {
    case Token::LET:
        return parse_let_statment();
    case Token::RETURN:
        return parse_return_statment();
    default:
        return parse_expression_statment();
    }

    return nullptr;
}

std::unique_ptr<ast::Statment> Parser::parse_let_statment() {
    std::unique_ptr<ast::LetStatment> stmt(new ast::LetStatment(_current_token));

    // 查看下一个 token，并取出
    if (!expect_peek(Token::IDENT)) {
        return nullptr;
    }

    stmt->set_identifier(new ast::Identifier(_current_token, _current_token.literal));

    // 标识符的下一个 token 必须是 = 号
    if (!expect_peek(Token::ASSIGN)) {
        return nullptr;
    }

    // 表达式解析部分，暂时跳过
    while (!current_token_is(Token::SEMICOLON)) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<ast::Statment> Parser::parse_return_statment() {
    std::unique_ptr<ast::ReturnStatment> stmt(new ast::ReturnStatment(_current_token));

    // 表达式解析部分，暂时跳过
    while (!current_token_is(Token::SEMICOLON)) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<ast::Statment> Parser::parse_expression_statment() {
    std::unique_ptr<ast::ExpressionStatment> stmt(new ast::ExpressionStatment(_current_token));

    auto expression = parse_expression(Precedence::LOWEST);
    stmt->set_expression(expression.release());

    if (peek_token_is(Token::SEMICOLON)) {
        next_token();
    }
    return stmt;
}

std::unique_ptr<ast::Expression> Parser::parse_expression(Precedence precedence) {
    auto prefix = _prefix_parse_funcs.find(_current_token.type);
    if (prefix == _prefix_parse_funcs.end()) {
        _errors.push_back("no prefix parse function found for `" + _current_token.literal + "`");
        return nullptr;
    }

    auto left = prefix->second();

    while (!peek_token_is(Token::SEMICOLON) && precedence < peek_precedence()) {
        auto infix = _infix_parse_funcs.find(_peek_token.type); 
        if (infix == _infix_parse_funcs.end()) {
            return left;
        }

        next_token();
        auto exp = infix->second(left.release());
        left.swap(exp);
    }

    return left;
}

std::unique_ptr<ast::Expression> Parser::parse_identifier() {
    return std::unique_ptr<ast::Expression>(new ast::Identifier(
        _current_token,
        _current_token.literal
    ));
}

std::unique_ptr<ast::Expression> Parser::parse_integer_literal() {
    return std::unique_ptr<ast::Expression>(new ast::IntegerLiteral(_current_token));
}

std::unique_ptr<ast::Expression> Parser::parse_prefix_expression() {
    std::unique_ptr<ast::PrefixExpression> prefix_expression(
            new ast::PrefixExpression(_current_token));
    next_token();
    auto right = parse_expression(Precedence::PREFIX);
    prefix_expression->set_right(right.release());
    return prefix_expression;
}

std::unique_ptr<ast::Expression> Parser::parse_infix_expression(ast::Expression* left) {
    std::unique_ptr<ast::InfixExpression> infix_expression(
            new ast::InfixExpression(_current_token));


    auto precedence = current_precedence();
    next_token();
    auto right = parse_expression(precedence);

    infix_expression->set_left(left);
    infix_expression->set_right(right.release());

    return infix_expression;
}

} // namespace autumn
