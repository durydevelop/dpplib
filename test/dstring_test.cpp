#include <gtest/gtest.h>
#include <libdpp/DString.h>

TEST(DStringTest, StringToNumber) {
    ASSERT_EQ(-1,DTools::DString::ToNumber<int>("-1"));
}
