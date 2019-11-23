#include <any>
#include <string>
#include <tuple>
#include <gtest/gtest.h>
#include "parser.h"

using namespace autumn;
using namespace autumn::ast;

namespace {

// 工具函数
void test_literal(const std::any& expect, const Expression* exp) {
    ASSERT_TRUE(exp != nullptr);
    if (expect.type() == typeid(int)) {
        auto int_literal = exp->cast<IntegerLiteral>();
        ASSERT_TRUE(int_literal != nullptr);
        EXPECT_EQ(std::any_cast<int>(expect), int_literal->value());
    } else if (expect.type() == typeid(bool)) {
        auto bool_literal = exp->cast<BooleanLiteral>();
        ASSERT_TRUE(bool_literal != nullptr);
        EXPECT_EQ(std::any_cast<bool>(expect), bool_literal->value());
    } else if (expect.type() == typeid(std::string)) {
        auto ident = exp->cast<Identifier>();
        ASSERT_TRUE(ident != nullptr);
        EXPECT_EQ(std::any_cast<std::string>(expect), ident->token_literal());
        EXPECT_EQ(std::any_cast<std::string>(expect), ident->value());
    } else if (expect.type() == typeid(const char*)) {
        auto ident = exp->cast<Identifier>();
        ASSERT_TRUE(ident != nullptr);
        EXPECT_STREQ(std::any_cast<const char*>(expect), ident->token_literal().c_str());
        EXPECT_STREQ(std::any_cast<const char*>(expect), ident->value().c_str());
    } else {
        ASSERT_TRUE(false);
    }
}

void test_infix_expression(
        const std::any& left_expect,
        const std::string& op_expect,
        const std::any& right_expect,
        const Expression* exp) {
    ASSERT_TRUE(exp != nullptr);
    auto infix_exp = exp->cast<InfixExpression>();
    ASSERT_TRUE(infix_exp != nullptr);
    test_literal(left_expect, infix_exp->left());
    test_literal(right_expect, infix_exp->right());
}

TEST(Parser, TestLetStatment) {
    std::string input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
        let foo = true;
        let bar = false;
    )";

    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    ASSERT_EQ(5u, statments.size());

    std::vector<std::tuple<std::string, std::any>> tests = {
        std::make_tuple("x", 5),
        std::make_tuple("y", 10),
        std::make_tuple("foobar", 838383),
        std::make_tuple("foo", true),
        std::make_tuple("bar", false),
    };

    for (size_t i = 0; i < tests.size(); ++i) {
        auto& test = tests[i];
        auto& stmt = statments[i];
        ASSERT_STREQ("let", stmt->token_literal().c_str());
        auto let_stmt = stmt->cast<LetStatment>();
        ASSERT_TRUE(let_stmt != nullptr);
        EXPECT_EQ(std::get<0>(test), let_stmt->identifier()->token_literal());
        EXPECT_EQ(std::get<0>(test), let_stmt->identifier()->value());

        auto exp = let_stmt->expression();
        test_literal(std::get<1>(test), exp);
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
        return true;
        return false;
    )";

    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    auto& errors = parser.errors();

    EXPECT_TRUE(errors.empty());
    ASSERT_EQ(5u, statments.size());

    std::vector<std::any> tests = {
        5,
        10,
        938383,
        true,
        false,
    };

    ASSERT_EQ(5u, statments.size());

    for (size_t i = 0; i < tests.size(); ++i) {
        auto& expect = tests[i];
        auto& stmt = statments[i];
        EXPECT_STREQ("return", stmt->token_literal().c_str());
        auto return_stmt = stmt->cast<ReturnStatment>();
        EXPECT_TRUE(return_stmt != nullptr);
        test_literal(expect, return_stmt->expression());
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
    test_literal(std::string("foobar"), ident);
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

TEST(Parser, TestStringLiteralExpression) {
    // 类似于这各只有一个标志符的，也是表达式
    std::string input = R"("hello world")";
    Parser parser;

    auto program = parser.parse(input);
    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());

    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto str_literal = exp->cast<StringLiteral>();
    ASSERT_TRUE(str_literal != nullptr);
    EXPECT_STREQ("hello world", str_literal->value().c_str());
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

        test_infix_expression(left_expect, op, right_expect, exp);
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
        {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
        {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
        {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
        {"a * [1, 2, 3, 4][b * c] * d", "((a * ([1, 2, 3, 4][(b * c)])) * d)"},
        {"add(a * b[2], b[1], 2 * [1, 2][1])", "add((a * (b[2])), (b[1]), (2 * ([1, 2][1])))"},
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

TEST(Parser, TestIfExpression) {
    std::string input = "if (x < y) { x }";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }

    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto if_exp = exp->cast<IfExpression>();
    ASSERT_TRUE(if_exp != nullptr);
    auto condition_exp = if_exp->condition();
    test_infix_expression("x", "<", "y", condition_exp);

    auto consequence = if_exp->consequence();
    ASSERT_TRUE(consequence != nullptr);

    auto& consequence_statments = consequence->statments();
    ASSERT_EQ(1u, consequence_statments.size());
    stmt = consequence_statments[0]->cast<ExpressionStatment>();
    test_literal(std::string("x"), stmt->expression());

    auto alternative = if_exp->alternative();
    ASSERT_TRUE(alternative == nullptr);
}

TEST(Parser, TestIfElseExpression) {
    std::string input = "if (x < y) { x } else { y }";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }

    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto if_exp = exp->cast<IfExpression>();
    ASSERT_TRUE(if_exp != nullptr);
    auto condition_exp = if_exp->condition();
    test_infix_expression("x", "<", "y", condition_exp);

    auto consequence = if_exp->consequence();
    ASSERT_TRUE(consequence != nullptr);

    auto& consequence_statments = consequence->statments();
    ASSERT_EQ(1u, consequence_statments.size());
    stmt = consequence_statments[0]->cast<ExpressionStatment>();
    test_literal(std::string("x"), stmt->expression());

    auto alternative = if_exp->alternative();
    ASSERT_TRUE(alternative != nullptr);
    auto& alternative_statments = alternative->statments();
    ASSERT_EQ(1u, alternative_statments.size());
    stmt = alternative_statments[0]->cast<ExpressionStatment>();
    test_literal(std::string("y"), stmt->expression());
}

TEST(Parser, TestFunctionLiteralParsing) {
    std::string input = "fn(x, y) { x + y; }";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }

    ASSERT_TRUE(program != nullptr);
    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);

    auto fn_literal = exp->cast<FunctionLiteral>();
    ASSERT_TRUE(fn_literal != nullptr);
    auto& params = fn_literal->parameters();
    ASSERT_EQ(2u, params.size());
    test_literal("x", params[0].get());
    test_literal("y", params[1].get());

    auto body = fn_literal->body();
    auto& body_stats = body->statments();
    ASSERT_EQ(1u, body_stats.size());
    stmt = body_stats[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    exp = stmt->expression();
    test_infix_expression("x", "+", "y", exp);
}

TEST(Parser, TestFunctionParameterParsing) {
    std::vector<std::tuple<std::string, std::vector<std::string>>> tests = {
        {"fn() {}", {}},
        {"fn(x) {}", {"x"}},
        {"fn(x, y, z) {}", {"x", "y", "z"}},
    };

    Parser parser;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect_params = std::get<1>(test);

        auto program = parser.parse(input);
        for (auto& error : parser.errors()) {
            std::cout << error << std::endl;
        }
        ASSERT_TRUE(program != nullptr);

        auto& statments = program->statments();
        ASSERT_EQ(1u, statments.size());
        auto stmt = statments[0]->cast<ExpressionStatment>();
        ASSERT_TRUE(stmt != nullptr);
        auto exp = stmt->expression();
        ASSERT_TRUE(exp != nullptr);

        auto fn_literal = exp->cast<FunctionLiteral>();
        ASSERT_TRUE(fn_literal != nullptr);
        auto& params = fn_literal->parameters();
        ASSERT_EQ(expect_params.size(), params.size());

        for (size_t i = 0; i < expect_params.size(); ++i) {
            auto& expect_param = expect_params[i];
            test_literal(expect_param, params[i].get());
        }
    }
}

TEST(Parser, TestCallExpressionParsing) {
    std::string input = "add(1, 2 * 3, 4 + 5)";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto call_exp = exp->cast<CallExpression>();
    ASSERT_TRUE(call_exp != nullptr);
    auto fn_exp = call_exp->function();
    ASSERT_TRUE(fn_exp != nullptr);
    test_literal("add", fn_exp);

    auto& args = call_exp->arguments();
    ASSERT_EQ(3u, args.size());
    test_literal(1, args[0].get());
    test_infix_expression(2, "*", 3, args[1].get());
    test_infix_expression(4, "*", 5, args[2].get());
}

TEST(Parser, TestParsingArrayLiteral) {
    std::string input = "[1, 2 * 2, 3 + 3]";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto array_literal = exp->cast<ArrayLiteral>();
    ASSERT_TRUE(array_literal != nullptr);

    auto& elems = array_literal->elements();

    ASSERT_EQ(3u, elems.size());
    test_literal(1, elems[0].get());
    test_infix_expression(2, "*", 2, elems[1].get());
    test_infix_expression(3, "+", 3, elems[2].get());
}

TEST(Parser, TestIndexExpression) {
    std::string input = "arr[1 + 1]";

    Parser parser;
    auto program = parser.parse(input);
    for (auto& error : parser.errors()) {
        std::cout << error << std::endl;
    }
    ASSERT_TRUE(program != nullptr);

    auto& statments = program->statments();
    ASSERT_EQ(1u, statments.size());
    auto stmt = statments[0]->cast<ExpressionStatment>();
    ASSERT_TRUE(stmt != nullptr);
    auto exp = stmt->expression();
    ASSERT_TRUE(exp != nullptr);
    auto idx_exp = exp->cast<IndexExpression>();
    ASSERT_TRUE(idx_exp != nullptr);

    auto index = idx_exp->index();
    auto left = idx_exp->left();

    test_infix_expression(1, "+", 1, index);
    test_literal("arr", left);
}

}
