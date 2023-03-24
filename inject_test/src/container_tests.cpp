#include "inject/container.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(container, register_shared_succeeds)
{
    // Arrange
    inject::container container;

    // Action
    container.register_shared<int>([]() { return std::make_shared<int>(1); });

    // Assert
    ASSERT_TRUE(container.is_registered<std::shared_ptr<int>>());
    ASSERT_TRUE(container.is_registered_shared<int>());
}

TEST(container, register_shared_duplicate)
{
    // Arrange
    inject::container container;

    auto fn = []()
    {
        return std::make_shared<int>(1);
    };

    // Action
    container.register_shared<int>(fn);

    // Assert
    ASSERT_THROW(container.register_shared<int>(fn), inject::factory_exception);
}

TEST(container, register_shared_interface_succeeds)
{
    struct itype { virtual ~itype() = default; };
    struct type : itype {};

    // Arrange
    inject::container container;

    // Action
    container.register_shared<itype>([]() { return std::make_shared<type>(); });

    // Assert
    ASSERT_TRUE(container.is_registered<std::shared_ptr<itype>>());
    ASSERT_TRUE(container.is_registered_shared<itype>());
}

TEST(container, register_unique_succeeds)
{
    // Arrange
    inject::container container;

    // Action
    container.register_unique<int>([]() { return std::make_unique<int>(1); });

    // Assert
    ASSERT_TRUE(container.is_registered<std::unique_ptr<int>>());
    ASSERT_TRUE(container.is_registered_unique<int>());
}

TEST(container, register_unique_duplicate)
{
    // Arrange
    inject::container container;

    auto fn = []()
    {
        return std::make_unique<int>(1);
    };

    // Action
    container.register_unique<int>(fn);

    // Assert
    ASSERT_THROW(container.register_unique<int>(fn), inject::factory_exception);
}

TEST(container, register_unique_interface_succeeds)
{
    struct itype { virtual ~itype() = default; };
    struct type : itype {};

    // Arrange
    inject::container container;

    // Action
    container.register_unique<itype>([]() { return std::make_unique<type>(); });

    // Assert
    ASSERT_TRUE(container.is_registered<std::unique_ptr<itype>>());
    ASSERT_TRUE(container.is_registered_unique<itype>());
}

TEST(container, resolve_cached_succeeds)
{
    // Arrange
    inject::container container;

    container.register_cached<int>([]()
        {
            return 1;
        });

    // Action
    auto result = container.resolve<int>();

    // Assert
    ASSERT_EQ(1, result);
}

TEST(container, resolve_shared_succeeds)
{
    // Arrange
    inject::container container;

    container.register_shared<int>([]()
        {
            return std::make_shared<int>(1);
        });

    // Action
    auto result = container.resolve_shared<int>();

    // Assert
    constexpr bool is_expected_type = std::is_same_v<decltype(result), std::shared_ptr<int>>;

    ASSERT_TRUE(is_expected_type);
    ASSERT_EQ(1, *result);
}

TEST(container, resolve_unique_succeeds)
{
    // Arrange
    inject::container container;

    container.register_unique<int>([]()
        {
            return std::make_unique<int>(1);
        });

    // Action
    auto result = container.resolve_unique<int>();

    // Assert
    constexpr bool is_expected_type = std::is_same_v<decltype(result), std::unique_ptr<int>>;

    ASSERT_TRUE(is_expected_type);
    ASSERT_EQ(1, *result);
}

TEST(container, resolve_cached_repeat_succeeds)
{
    // Arrange
    inject::container container;

    container.register_cached<int>([count = std::make_shared<int>(0)]() mutable
        {
            return ++(*count);
        });

    // Action
    auto result1 = container.resolve<int>();
    auto result2 = container.resolve<int>();

    // Assert
    ASSERT_EQ(1, result1);
    ASSERT_EQ(1, result2);
}

TEST(container, resolve_shared_repeat_succeeds)
{
    // Arrange
    inject::container container;

    container.register_shared<int>([count = std::make_shared<int>(0)]() mutable
        {
            return std::make_shared<int>(++(*count));
        });

    // Action
    auto result1 = container.resolve_shared<int>();
    auto result2 = container.resolve_shared<int>();

    // Assert
    const bool is_same = result1.get() == result2.get();

    ASSERT_TRUE(is_same);
    ASSERT_EQ(1, *result1);
    ASSERT_EQ(1, *result2);
}

TEST(container, resolve_unique_repeat_succeeds)
{
    // Arrange
    inject::container container;

    container.register_unique<int>([count = std::make_shared<int>(0)]() mutable
        {
            return std::make_unique<int>(++(*count));
        });

    // Action
    auto result1 = container.resolve_unique<int>();
    auto result2 = container.resolve_unique<int>();

    // Assert
    const bool is_same = result1.get() == result2.get();

    ASSERT_FALSE(is_same);
    ASSERT_EQ(1, *result1);
    ASSERT_EQ(2, *result2);
}

TEST(container, resolve_shared_args_succeeds)
{
    // Arrange
    inject::container container;

    container.register_shared<std::string>([](std::shared_ptr<char> ch, std::shared_ptr<float> f)
        {
            std::stringstream ss;
            ss << "Char: " << *ch << ", ";
            ss << "Float: " << *f;
            return std::make_shared<std::string>(ss.str());
        });

    container.register_shared<char>([]()
        {
            return std::make_shared<char>('a');
        });

    container.register_shared<float>([]()
        {
            return std::make_shared<float>(3.142f);
        });

    // Action
    auto result1 = container.resolve_shared<std::string>();
    auto result2 = container.resolve_shared<std::string>();

    // Assert
    const bool is_same = result1.get() == result2.get();

    ASSERT_TRUE(is_same);
    ASSERT_EQ("Char: a, Float: 3.142", *result1);
    ASSERT_EQ("Char: a, Float: 3.142", *result2);
}

TEST(container, resolve_unique_args_succeeds)
{
    // Arrange
    inject::container container;

    container.register_unique<std::string>([](std::unique_ptr<char> ch, std::unique_ptr<float> f)
        {
            std::stringstream ss;
            ss << "Char: " << *ch << ", ";
            ss << "Float: " << *f;
            return std::make_unique<std::string>(ss.str());
        });

    container.register_unique<char>([]()
        {
            return std::make_unique<char>('a');
        });

    container.register_unique<float>([]()
        {
            return std::make_unique<float>(3.142f);
        });

    // Action
    auto result1 = container.resolve_unique<std::string>();
    auto result2 = container.resolve_unique<std::string>();

    // Assert
    const bool is_same = result1.get() == result2.get();

    ASSERT_FALSE(is_same);
    ASSERT_EQ("Char: a, Float: 3.142", *result1);
    ASSERT_EQ("Char: a, Float: 3.142", *result2);
}
