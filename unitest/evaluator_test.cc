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

TEST(Evaluator, TestErrorHandling) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {"5 + true;", "type mismatch: `INTEGER + BOOLEAN`"},
        {"5 + true; 5;", "type mismatch: `INTEGER + BOOLEAN`"},
        {"-true", "unknown operator: `-BOOLEAN`"},
        {"true + false;", "unknown operator: `BOOLEAN + BOOLEAN`"},
        {"5; true + false; 5", "unknown operator: `BOOLEAN + BOOLEAN`"},
        {"if (10 > 1) { true + false; }", "unknown operator: `BOOLEAN + BOOLEAN`"},
        {R"(
                if (10 > 1) {
                    if (10 > 1) { return true + false; }
                    return 1;
                }
            )", "unknown operator: `BOOLEAN + BOOLEAN`"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);
        auto error_object = object->cast<Error>();
        ASSERT_TRUE(error_object != nullptr);
        EXPECT_EQ(expect, error_object->message());
    }
}

TEST(Evaluator, TestLetStatment) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"let a = 5; a", 5},
        {"let a = 5 * 5; a", 25},
        {"let a = 50; let b = a; b", 50},
        {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        test_integer_object(object.get(), expect);
    }
}

TEST(Evaluator, TestFunctionObject) {
    std::string input = "fn(x) { x + 2; }";
    Evaluator evaluator;
    auto object = evaluator.eval(input);
    auto fn_obj = object->cast<Function>();
    ASSERT_TRUE(fn_obj != nullptr);
    auto& params = fn_obj->parameters();
    EXPECT_EQ(1u, params.size());
    EXPECT_STREQ("x", params[0]->to_string().c_str());
    ASSERT_TRUE(fn_obj->body() != nullptr);
    EXPECT_STREQ("(x + 2)", fn_obj->body()->to_string().c_str());
}

TEST(Evaluator, TestFunctionApplication) {
    std::vector<std::tuple<std::string, int>> tests = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto expect = std::get<1>(test);

        evaluator.reset_env();
        auto object = evaluator.eval(input);

        test_integer_object(object.get(), expect);
    }
}

TEST(Evaluator, TestClosures) {
    std::string input = R"(
        let foo = fn(x) {
            return fn(y) {
                return x + y;
            };
        };

        let addtwo = foo(2);
        addtwo(10);
    )";

    Evaluator evaluator;

    auto object = evaluator.eval(input);

    if (typeid(object) == typeid(object::Error)) {
        std::cout << object->inspect() << std::endl;
        ASSERT_TRUE(false);
    }

    test_integer_object(object.get(), 12);
}

TEST(Evaluator, TestStringExpression) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {R"("hello world")", "hello world"},
        {R"("hello autumn")", "hello autumn"},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        if (object->type() == object::Type::ERROR_OBJECT) {
            std::cout << object->inspect() << std::endl;
            ASSERT_TRUE(false);
        }

        test_string_object(object.get(), expect);
    }
}

TEST(Evaluator, TestStringConcatenation) {
    std::string input = R"("hello" + " " + "world")";
    Evaluator evaluator;
    auto object = evaluator.eval(input);
    test_string_object(object.get(), "hello world");
}

TEST(Evaluator, TestBuiltinFunctions) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {R"(len(""))", 0},
        {R"(len("hello world"))", 11},
        {R"(len(1))", "argument to `len` not supported, got INTEGER"},
        {R"(len("one", "two"))", "wrong number of arguments. expected 1, got 2"},
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

TEST(Evaluator, TestArrayLiteral) {
    std::string input = "[1, 2 + 2, 3 * 3]";
    Evaluator evaluator;
    auto object = evaluator.eval(input);

    ASSERT_TRUE(object != nullptr);

    ASSERT_EQ(object->type(), object::Type::ARRAY_OBJECT);
    auto array_obj = object->cast<object::Array>();
    ASSERT_EQ(3u, array_obj->elements().size());

    EXPECT_STREQ("[1, 4, 9]", array_obj->inspect().c_str());

    test_integer_object(array_obj->elements()[0].get(), 1);
    test_integer_object(array_obj->elements()[1].get(), 4);
    test_integer_object(array_obj->elements()[2].get(), 9);
}

TEST(Evaluator, TestIndexExpression) {
    std::vector<std::tuple<std::string, std::any>> tests = {
        {"[1, 2, 3][0]", 1},
        {"[1, 2, 3][1]", 2},
        {"[1, 2, 3][2]", 3},
        {"let i = 0; [1][i];", 1},
        {"[1, 2, 3][1 + 1];", 3},
        {"let myArray = [1, 2, 3]; myArray[2];", 3},
        {"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", 6},
        {"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", 2},
        {"[1, 2, 3][3]", nullptr},
        {"[1, 2, 3][-1]", 3},
    };

    Evaluator evaluator;

    for (auto& test : tests) {
        auto& input = std::get<0>(test);
        auto& expect = std::get<1>(test);

        auto object = evaluator.eval(input);

        if (expect.type() == typeid(int)) {
            test_integer_object(object.get(), std::any_cast<int>(expect));
        } else if (expect.type() == typeid(std::nullptr_t)) {
            test_null_object(object.get());
        } else {
            std::cout << object->inspect() << std::endl;
        }
    }
}

}
