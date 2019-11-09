#include <string>
#include <gtest/gtest.h>

#include "parser.h"

using namespace autumn;

TEST(Parser, TestLetStatment) {
    std::string input = R"(
let x = 5;
let y = 10;
let foobar = 838383;
)";

    Program program(input);

    auto& statments = program.statments();
    ASSERT_EQ(3u, statments.size());

    for (auto& statment : statments) {
        ASSERT_STREQ("let", statment->token_literal().c_str());
        auto& let_stmt = statment->cast<LetStatment>();
    }
}
