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
    EXPECT_EQ(3u, errors.size());

    for (auto& error : errors) {
        std::cout << error << std::endl;
    }

    ASSERT_TRUE(statments.empty());
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

}
