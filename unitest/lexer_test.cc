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
    };

    Lexer lexer(input);

    for (auto& expect_token: expect_tokens) {
        auto token = lexer.next_token();
        EXPECT_EQ(expect_token, token);
    }
}
