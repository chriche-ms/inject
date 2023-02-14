#pragma once

#include <stdexcept>

namespace inject
{
    class factory_exception : public std::runtime_error
    {
    public:
        explicit factory_exception(const char* message) : std::runtime_error(message)
        {
        }
    };
}
