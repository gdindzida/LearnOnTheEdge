#include "calculator/calculator.h"
#include <gtest/gtest.h>

TEST(Calculator, AddsPositiveNumbers)
{
    EXPECT_EQ(calculator::add(2, 3), 5);
}

TEST(Calculator, HandlesNegativeNumbersAndZero)
{
    EXPECT_EQ(calculator::add(-2, 3), 1);
    EXPECT_EQ(calculator::add(0, 0), 0);
}
