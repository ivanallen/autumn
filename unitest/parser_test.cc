#include <string>
#include <tuple>
#include <gtest/gtest.h>

// for test
#define private public

#include "parser.h"

using namespace autumn;

namespace {

TEST(Parser, TestLetStatment) {
    std::string input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
    )";

    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    ASSERT_EQ(3u, statments.size());

    std::vector<std::tuple<std::string, std::string>> tests = {
        std::make_tuple("x", ""),
        std::make_tuple("y", ""),
        std::make_tuple("foobar", ""),
    };

    for (size_t i = 0; i < tests.size(); ++i) {
        auto& test = tests[i];
        auto& stmt = statments[i];
        ASSERT_STREQ("let", stmt->token_literal().c_str());
        auto let_stmt = stmt->cast<LetStatment>();
        ASSERT_TRUE(let_stmt != nullptr);
        EXPECT_EQ(std::get<0>(test), let_stmt->identifier()->token_literal());
        EXPECT_EQ(std::get<0>(test), let_stmt->identifier()->value());
    }
}

TEST(Parser, TestErrorLetStatment) {
    std::string input = R"(
        let x 5;
        let = 10;
        let 838383;
    )";

    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    auto& errors = parser.errors();

    for (auto& error : errors) {
        std::cout << error << std::endl;
    }
}

TEST(Parser, TestReturnStatment) {
    std::string input = R"(
        return 5;
        return 10;
        return 938383;
    )";

    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    auto& errors = parser.errors();

    EXPECT_TRUE(errors.empty());
    ASSERT_EQ(3u, statments.size());


    for (auto& stmt : statments) {
        EXPECT_STREQ("return", stmt->token_literal().c_str());
        auto return_stmt = stmt->cast<ReturnStatment>();
        EXPECT_TRUE(return_stmt != nullptr);
    }
}

TEST(Parser, TestString) {
    Program p;
    auto stmt = new LetStatment({Token::LET, "let"});
    stmt->_identifier.reset(new Identifier({Token::IDENT, "my_var"}, "my_var"));
    stmt->_expression.reset(new Identifier({Token::IDENT, "another_var"}, "another_var"));
    p._statments.emplace_back(stmt);
    EXPECT_STREQ("let my_var = another_var;", p.to_string().c_str());
}

TEST(Parser, TestIdentifierExpression) {
    // 类似于这各只有一个标志符的，也是表达式
    std::string input = "foobar;";
    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());

    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto ident = exp->cast<Identifier>();
    ASSERT_TRUE(ident != nullptr);
    EXPECT_STREQ("foobar", ident->token_literal().c_str());
    EXPECT_STREQ("foobar", ident->value().c_str());
}

TEST(Parser, TestIntegerLiteralExpression) {
    // 类似于这各只有一个标志符的，也是表达式
    std::string input = "123;";
    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());

    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto int_literal = exp->cast<IntegerLiteral>();
    ASSERT_TRUE(int_literal != nullptr);
    EXPECT_STREQ("123", int_literal->token_literal().c_str());
    EXPECT_EQ(123, int_literal->value());
}

TEST(Parser, TestParsingPrefixExpression) {
    std::vector<std::tuple<std::string, std::string, int>> tests = {
        {"!5", "!", 5},
        {"-15", "-", 15},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);

        auto program = parser.parse(input);
        auto& errors = parser.errors();
        for (auto& error : errors) {
            std::cout << error << std::endl;
        }
        ASSERT_TRUE(program != nullptr);
        auto& statments = program->statments();
        ASSERT_EQ(1u, statments.size());
        auto stmt = statments[0]->cast<ExpressionStatment>();
        ASSERT_TRUE(stmt != nullptr);
        auto exp = stmt->expression();
        ASSERT_TRUE(exp != nullptr);
        auto prefix_exp = exp->cast<PrefixExpression>();
        ASSERT_TRUE(prefix_exp != nullptr);
    }
}

}
