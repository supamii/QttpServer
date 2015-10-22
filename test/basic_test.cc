#include "native/native.h"
#include "gtest/gtest.h"

TEST(TestGTest, EqConstInt)
{
    EXPECT_EQ(1, 1);
}

TEST(TestGTest, EqConstDouble)
{
    EXPECT_EQ(1.0, 1.0);
}
