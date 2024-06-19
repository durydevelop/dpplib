#include <gtest/gtest.h>
#include <dpplib/DString.h>

using namespace DTools;

TEST(DStringTest, StringToNumber) {
    ASSERT_EQ(-1,DString::ToNumber<int>("-1"));
}
