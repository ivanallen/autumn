#include <gtest/gtest.h>
#include "format.h"

using namespace autumn;

namespace {

TEST(Format, TestFormat) {
    EXPECT_EQ("my name is autumn", format("my name is {}", "autumn"));
    EXPECT_EQ("3 + 4 = 7", format("{} + {} = {}", 3, 4, 3 + 4));

    EXPECT_EQ("error 3", format("error {}", 3, 4, 3 + 4));
}

}
