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

TEST(Evaluator, TestEvalIntegerExpression) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
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

}
