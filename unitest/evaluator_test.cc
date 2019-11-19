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

TEST(Evaluator, TestEvalIntegerExpression) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"5", 5},
        {"10", 10},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        test_integer_object(object.get(), expect);
    }
}

}
