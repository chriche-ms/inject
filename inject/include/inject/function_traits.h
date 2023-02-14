#pragma once

#include <tuple>

namespace inject
{
    template<typename R, typename... Args>
    struct function_traits_base
    {
        using type_return = R;
        using type_args = std::tuple<Args...>;

        template<std::size_t Idx>
        using type_arg = std::tuple_element_t<Idx, type_args>;
    };

    // Callable object
    template<typename Fn>
    struct function_traits : function_traits<decltype(&Fn::operator())>
    {
    };

    // Specialization - Function
    template<typename R, typename... Args>
    struct function_traits<R(Args...)> : function_traits_base<R, Args...>
    {
    };

    // Specialization - Pointer to function
    template<typename R, typename... Args>
    struct function_traits<R(*)(Args...)> : function_traits_base<R, Args...>
    {
    };

    // Specialization - Pointer to member function
    template<typename T, typename R, typename... Args>
    struct function_traits<R(T::*)(Args...)> : function_traits_base<R, Args...>
    {
    };

    // Specialization - Pointer to const member function
    template<typename T, typename R, typename... Args>
    struct function_traits<R(T::*)(Args...) const> : function_traits_base<R, Args...>
    {
    };
}
