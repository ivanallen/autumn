#include <any>
#include <string>
#include <tuple>
#include <gtest/gtest.h>

// for test
#define private public

#include "parser.h"

using namespace autumn;
using namespace autumn::ast;

namespace {

// 工具函数
void test_literal(const std::any& expect, const Expression* exp) {
    if (expect.type() == typeid(int)) {
        ASSERT_TRUE(exp != nullptr);
        auto int_literal = exp->cast<IntegerLiteral>();
        ASSERT_TRUE(int_literal != nullptr);
        EXPECT_EQ(std::any_cast<int>(expect), int_literal->value());
    } else if (expect.type() == typeid(bool)) {
        ASSERT_TRUE(exp != nullptr);
        auto bool_literal = exp->cast<BooleanLiteral>();
        ASSERT_TRUE(bool_literal != nullptr);
        EXPECT_EQ(std::any_cast<bool>(expect), bool_literal->value());
    }
}

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
    std::vector<std::tuple<std::string, std::string, std::any>> tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& op = std::get<1>(test);
        auto& expect = std::get<2>(test);

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
        EXPECT_EQ(op, prefix_exp->op());
        auto right = prefix_exp->right();

        test_literal(expect, right);
    }
}

TEST(Parser, TestParsingInfixExpression) {
    std::vector<std::tuple<std::string, std::any, std::string, std::any>> tests = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
        {"true == true;", true, "==", true},
        {"false == false;", false, "==", false},
        {"true != false;", true, "!=", false},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& left_expect = std::get<1>(test);
        auto& op = std::get<2>(test);
        auto& right_expect = std::get<3>(test);

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
        auto infix_exp = exp->cast<InfixExpression>();
        ASSERT_TRUE(infix_exp != nullptr);
        EXPECT_EQ(op, infix_exp->op());

        auto left_exp = infix_exp->left();
        auto right_exp = infix_exp->right();

        test_literal(left_expect, left_exp);
        test_literal(right_expect, right_exp);
    }
}

TEST(Parser, TestOperatorPrecedenceParsing) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto program = parser.parse(input);
        ASSERT_TRUE(program != nullptr);
        auto& errors = parser.errors();
        for (auto& error : errors) {
            std::cout << error << std::endl;
        }
        EXPECT_EQ(expect, program->to_string());
    }
}

TEST(parser, TestBooleanLiteralExpression) {
    std::vector<std::tuple<std::string, bool>> tests = {
        {"true;", true},
        {"false;", false},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto expect = std::get<1>(test);

        auto program = parser.parse(input);
        ASSERT_TRUE(program != nullptr);
        auto& statments = program->statments();
        ASSERT_EQ(1u, statments.size());
        auto stat = statments[0]->cast<ExpressionStatment>();
        ASSERT_TRUE(stat != nullptr);
        auto exp = stat->expression();

        test_literal(expect, exp);
    }
}

}
