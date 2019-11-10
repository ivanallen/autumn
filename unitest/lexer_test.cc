#include <string>
#include <gtest/gtest.h>

#include "lexer.h"

using namespace autumn;

TEST(Lexer, TestSymbol) {
    std::string input = "+=(){},;";
    Token expect_tokens[] = {
        {Token::PLUS, "+"},
        {Token::ASSIGN, "="},
        {Token::LPAREN, "("},
        {Token::RPAREN, ")"},
        {Token::LBRACE, "{"},
        {Token::RBRACE, "}"},
        {Token::COMMA, ","},
        {Token::SEMICOLON, ";"},
        {Token::END, ""},
    };

    Lexer lexer(input);

    for (auto& expect_token: expect_tokens) {
        auto token = lexer.next_token();
        EXPECT_EQ(expect_token.literal, token.literal);
        EXPECT_EQ(expect_token.type, token.type);
    }
}

TEST(Lexer, TestIdentity) {
    std::string input = R"(
        let five = 5;
        let ten = 10;
        let add = fn(x, y) {
            return x + y;
        };
        let result = add(five, ten);
    )";

    Token expect_tokens[] = {
        {Token::LET, "let"},
        {Token::IDENT, "five"},
        {Token::ASSIGN, "="},
        {Token::INT, "5"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "ten"},
        {Token::ASSIGN, "="},
        {Token::INT, "10"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "add"},
        {Token::ASSIGN, "="},
        {Token::FUNCTION, "fn"},
        {Token::LPAREN, "("},
        {Token::IDENT, "x"},
        {Token::COMMA, ","},
        {Token::IDENT, "y"},
        {Token::RPAREN, ")"},
        {Token::LBRACE, "{"},
        {Token::RETURN, "return"},
        {Token::IDENT, "x"},
        {Token::PLUS, "+"},
        {Token::IDENT, "y"},
        {Token::SEMICOLON, ";"},
        {Token::RBRACE, "}"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "result"},
        {Token::ASSIGN, "="},
        {Token::IDENT, "add"},
        {Token::LPAREN, "("},
        {Token::IDENT, "five"},
        {Token::COMMA, ","},
        {Token::IDENT, "ten"},
        {Token::RPAREN, ")"},
        {Token::SEMICOLON, ";"},
        {Token::END, ""},
    };

    Lexer lexer(input);

    for (auto& expect_token: expect_tokens) {
        auto token = lexer.next_token();
        EXPECT_EQ(expect_token.literal, token.literal);
        EXPECT_EQ(expect_token.type, token.type);
    }
}

TEST(Lexer, TestNontrivial) {
    std::string input = R"(
        let five = 5;
        let ten = 10;
        let add = fn(x, y) {
            return x + y;
        };
        let result = add(five, ten);
        !-/*5;
        5 < 10 > 5;
        if (5 < 10) {
            return true;
        } else {
            return false;
        }
        10 == 10;
        10 != 9;
        3 <= 6 >= 3;
    )";

    Token expect_tokens[] = {
        {Token::LET, "let"},
        {Token::IDENT, "five"},
        {Token::ASSIGN, "="},
        {Token::INT, "5"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "ten"},
        {Token::ASSIGN, "="},
        {Token::INT, "10"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "add"},
        {Token::ASSIGN, "="},
        {Token::FUNCTION, "fn"},
        {Token::LPAREN, "("},
        {Token::IDENT, "x"},
        {Token::COMMA, ","},
        {Token::IDENT, "y"},
        {Token::RPAREN, ")"},
        {Token::LBRACE, "{"},
        {Token::RETURN, "return"},
        {Token::IDENT, "x"},
        {Token::PLUS, "+"},
        {Token::IDENT, "y"},
        {Token::SEMICOLON, ";"},
        {Token::RBRACE, "}"},
        {Token::SEMICOLON, ";"},
        {Token::LET, "let"},
        {Token::IDENT, "result"},
        {Token::ASSIGN, "="},
        {Token::IDENT, "add"},
        {Token::LPAREN, "("},
        {Token::IDENT, "five"},
        {Token::COMMA, ","},
        {Token::IDENT, "ten"},
        {Token::RPAREN, ")"},
        {Token::SEMICOLON, ";"},
        {Token::BANG, "!"},
        {Token::MINUS, "-"},
        {Token::SLASH, "/"},
        {Token::ASTERISK, "*"},
        {Token::INT, "5"},
        {Token::SEMICOLON, ";"},
        {Token::INT, "5"},
        {Token::LT, "<"},
        {Token::INT, "10"},
        {Token::GT, ">"},
        {Token::INT, "5"},
        {Token::SEMICOLON, ";"},
        {Token::IF, "if"},
        {Token::LPAREN, "("},
        {Token::INT, "5"},
        {Token::LT, "<"},
        {Token::INT, "10"},
        {Token::RPAREN, ")"},
        {Token::LBRACE, "{"},
        {Token::RETURN, "return"},
        {Token::TRUE, "true"},
        {Token::SEMICOLON, ";"},
        {Token::RBRACE, "}"},
        {Token::ELSE, "else"},
        {Token::LBRACE, "{"},
        {Token::RETURN, "return"},
        {Token::FALSE, "false"},
        {Token::SEMICOLON, ";"},
        {Token::RBRACE, "}"},
        {Token::INT, "10"},
        {Token::EQ, "=="},
        {Token::INT, "10"},
        {Token::SEMICOLON, ";"},
        {Token::INT, "10"},
        {Token::NEQ, "!="},
        {Token::INT, "9"},
        {Token::SEMICOLON, ";"},
        {Token::INT, "3"},
        {Token::LTE, "<="},
        {Token::INT, "6"},
        {Token::GTE, ">="},
        {Token::INT, "3"},
        {Token::SEMICOLON, ";"},
        {Token::END, ""},
    };

    Lexer lexer(input);

    for (auto& expect_token: expect_tokens) {
        auto token = lexer.next_token();
        EXPECT_EQ(expect_token.literal, token.literal);
        EXPECT_EQ(expect_token.type, token.type);
    }
}
