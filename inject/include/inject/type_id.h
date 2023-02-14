#pragma once

#include <functional> // std::hash

namespace inject
{
    // An alternative to std::type_index that doesn't require RTTI to be enabled
    class type_id
    {
    public:
        template<typename T>
        static type_id get() noexcept
        {
            return type_id(generate<T>());
        }

        const std::size_t id;

    private:
        explicit type_id(std::size_t id) noexcept : id(id)
        {
        }

        template<typename T>
        static std::size_t generate() noexcept
        {
            static const std::size_t id = generate_next();
            return id;
        }

        static std::size_t generate_next() noexcept
        {
            static std::size_t id_next = {};
            return id_next++;
        }
    };

    inline bool operator<(type_id lhs, type_id rhs) noexcept
    {
        return lhs.id < rhs.id;
    }

    inline bool operator>(type_id lhs, type_id rhs) noexcept
    {
        return rhs < lhs;
    }

    inline bool operator<=(type_id lhs, type_id rhs) noexcept
    {
        return !(lhs > rhs);
    }

    inline bool operator>=(type_id lhs, type_id rhs) noexcept
    {
        return !(lhs < rhs);
    }

    inline bool operator==(type_id lhs, type_id rhs) noexcept
    {
        return lhs.id == rhs.id;
    }

    inline bool operator!=(type_id lhs, type_id rhs) noexcept
    {
        return !(lhs == rhs);
    }
}
