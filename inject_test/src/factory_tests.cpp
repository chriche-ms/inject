#include "inject/factory.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <sstream>

TEST(factory, register_type_succeeds)
{
    // Arrange
    inject::factory factory;

    // Action
    factory.register_type<int>([]() { return 1; });

    // Assert
    ASSERT_TRUE(factory.is_registered<int>());
}

TEST(factory, register_type_interface_succeeds)
{
    struct itype { virtual ~itype() = default; };
    struct type : itype {};

    // Arrange
    inject::factory factory;

    // Action
    factory.register_type<std::unique_ptr<itype>>([]() { return std::make_unique<type>(); });

    // Assert
    ASSERT_TRUE(factory.is_registered<std::unique_ptr<itype>>());
}

TEST(factory, register_type_duplicate_throws)
{
    // Arrange
    inject::factory factory;

    auto fn = []()
    {
        return 1;
    };

    // Action
    factory.register_type<int>(fn);

    // Assert
    ASSERT_THROW(factory.register_type<int>(fn), inject::factory_exception);
}

TEST(factory, resolve_succeeds)
{
    // Arrange
    inject::factory factory;

    auto fn = []()
    {
        return 1;
    };

    factory.register_type<int>(fn);

    // Action
    auto result = factory.resolve<int>();

    // Assert
    constexpr bool is_expected_type = std::is_same_v<decltype(result), int>;

    ASSERT_TRUE(is_expected_type);
    ASSERT_EQ(1, result);
}

TEST(factory, resolve_repeat_succeeds)
{
    // Arrange
    inject::factory factory;

    auto fn = [count = std::make_shared<int>()]() mutable
    {
        return std::make_unique<int>(++*count);
    };

    factory.register_type<std::unique_ptr<int>>(fn);

    // Action
    auto result1 = factory.resolve<std::unique_ptr<int>>();
    auto result2 = factory.resolve<std::unique_ptr<int>>();

    // Assert
    const bool is_same = result1.get() == result2.get();

    ASSERT_FALSE(is_same);
    ASSERT_EQ(1, *result1);
    ASSERT_EQ(2, *result2);
}

TEST(factory, resolve_not_registered)
{
    // Arrange
    inject::factory factory;

    struct type
    {
    };

    // Action
    ASSERT_THROW(factory.resolve<type>(), inject::factory_exception);
}

TEST(factory, resolve_args_succeeds)
{
    // Arrange
    inject::factory factory;

    factory.register_type<std::string>([](char ch, float f)
        {
            std::stringstream ss;
            ss << "Char: " << ch << ", ";
            ss << "Float: " << f;
            return ss.str();
        });

    factory.register_type<char>([]()
        {
            return 'a';
        });

    factory.register_type<float>([]()
        {
            return 3.142f;
        });

    // Action
    auto result = factory.resolve<std::string>();

    // Assert
    ASSERT_EQ("Char: a, Float: 3.142", result);
}

TEST(factory, resolve_nested_succeeds)
{
    // Arrange
    inject::factory factory;

    struct type_a
    {
        int value;
    };

    struct type_b
    {
        int value;
    };

    struct type_c
    {
        int value;
    };

    factory.register_type<type_a>([](type_b b) -> type_a
        {
            return { b.value + 1 };
        });

    factory.register_type<type_b>([](type_c c) -> type_b
        {
            return { c.value + 1 };
        });

    factory.register_type<type_c>([]() -> type_c
        {
            return { 1 };
        });

    // Action
    auto result = factory.resolve<type_a>();

    // Assert
    ASSERT_EQ(3, result.value);
}
