#include <gtest/gtest.h>
#include "format.h"

using namespace autumn;

namespace {

struct Foo {
    Foo(int a, const std::string& b) : a(a), b(b) {}

    int a;
    std::string b;
};

std::ostream& operator<<(std::ostream& out, const Foo& foo) {
    return out << '{' << foo.a << ',' << foo.b << '}';
}

TEST(Format, TestFormat) {
    EXPECT_EQ("my name is autumn", format("my name is {}", "autumn"));
    EXPECT_EQ("3 + 4 = 7", format("{} + {} = {}", 3, 4, 3 + 4));
    EXPECT_EQ("3 + 4 = 7", format("{:a} + {:b} = {:c}", 3, 4, 3 + 4));
    EXPECT_EQ("3 4", format("{} {}", 3, 4, 3 + 4));
    EXPECT_EQ("{1,2,3}", format("{{},{},{}}", 1, 2, 3));
    EXPECT_EQ("this is foo:{10,autumn}", format("this is foo:{}", Foo(10, "autumn")));
}

}
