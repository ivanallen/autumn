#include <any>
#include <string>
#include <tuple>
#include <gtest/gtest.h>
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

void test_string_object(const Object* object, const std::string& expect) {
    auto result = object->cast<String>();
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(expect, result->value());
}

void test_null_object(const Object* object) {
    EXPECT_EQ(typeid(*object), typeid(Null));
}

void test_error_object(const Object* object, const std::string& expect) {
    auto result = object->cast<Error>();
    ASSERT_TRUE(result != nullptr);
    EXPECT_EQ(expect, result->message());
}


TEST(Builtin, TestLen) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {R"(len(""))", 0},
        {R"(len("hello world"))", 11},
        {R"(len(1))", "argument to `len` not supported, got INTEGER"},
        {R"(len("one", "two"))", "wrong number of arguments. expected 1, got 2"},
        {R"(len([1, 2, 3]))", 3},
        {R"(len([]))", 0},
        {R"(let a = [1, 2, 3, 4]; len(a))", 4},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        if (expect.type() == typeid(int)) {
            test_integer_object(object.get(), std::any_cast<int>(expect));
        } else {
            test_error_object(object.get(), std::any_cast<const char*>(expect));
        }
    }
}

TEST(Builtin, TestFirst) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {"let a = [1, 2, 3, 4]; first(a)", 1},
        {R"(let a = ["hello world", 2, 3, 4]; first(a))", "hello world"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        if (expect.type() == typeid(int)) {
            test_integer_object(object.get(), std::any_cast<int>(expect));
        } else if (expect.type() == typeid(const char*)) {
            test_string_object(object.get(), std::any_cast<const char*>(expect));
        } else {
            test_error_object(object.get(), std::any_cast<const char*>(expect));
        }
    }
}

TEST(Builtin, TestLast) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {"let a = [1, 2, 3, 4]; last(a)", 4},
        {R"(let a = [1, 2, 3, "hello autumn"]; last(a))", "hello autumn"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        if (expect.type() == typeid(int)) {
            test_integer_object(object.get(), std::any_cast<int>(expect));
        } else if (expect.type() == typeid(const char*)) {
            test_string_object(object.get(), std::any_cast<const char*>(expect));
        } else {
            test_error_object(object.get(), std::any_cast<const char*>(expect));
        }
    }
}

TEST(Builtin, TestRest) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {"let a = [1, 2, 3, 4]; rest(a)", "[2, 3, 4]"},
        {"let a = [1]; rest(a)", "[]"},
        {"let a = []; rest(a)", "null"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        EXPECT_EQ(expect, object->inspect());
    }
}

TEST(Builtin, TestPush) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {"let a = [1, 2, 3]; push(a, 4)", "[1, 2, 3, 4]"},
        {"let a = [1]; push(a, 2)", "[1, 2]"},
        {"let a = []; push(a, 1)", "[1]"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        EXPECT_EQ(expect, object->inspect());
    }
}

}
