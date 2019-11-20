#include <any>
#include <string>
#include <tuple>
#include <gtest/gtest.h>

// for test
#define private public

#include "evaluator.h"

using namespace autumn;
using namespace autumn::object;

namespace {

void test_integer_object(const Object* object, int expect) {
    auto result = object->cast<Integer>();
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(expect, result->value());
}

void test_boolean_object(const Object* object, bool expect) {
    auto result = object->cast<Boolean>();
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(expect, result->value());
}

void test_null_object(const Object* object) {
    EXPECT_EQ(typeid(*object), typeid(Null));
}

TEST(Evaluator, TestEvalIntegerExpression) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        test_integer_object(object.get(), expect);
    }
}

TEST(Evaluator, TestEvalBooleanExpression) {
    std::vector<std::tuple<std::string, bool>> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        test_boolean_object(object.get(), expect);
    }
}

TEST(Evaluator, TestBangOperator) {
    std::vector<std::tuple<std::string, bool>> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        test_boolean_object(object.get(), expect);
    }
}

TEST(Evaluator, TestIfElseExpression) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {"if (true) { 10 }", 10},
        {"if (false) { 10 }", nullptr},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 }", nullptr},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        if (expect.type() == typeid(int)) {
            test_integer_object(object.get(), std::any_cast<int>(expect));
        } else {
            test_null_object(object.get());
        }
    }
}

TEST(Evaluator, TestReturnStatment) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        { R"(
                if (10 > 1) {
                    if (10 > 1) { return 10; }
                    return 1;
                }
            )", 10},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        test_integer_object(object.get(), expect);
    }
}

}
