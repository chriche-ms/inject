#include "inject/factory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(factory, register_type_succeeds)
{
    // Arrange
    inject::factory factory;

    // Action
    factory.register_type<int>([]() { return 1; });

    // Assert
    ASSERT_TRUE(factory.is_registered<int>());
}
