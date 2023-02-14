#pragma once

#include "factory_exception.h"
#include "function_traits.h"
#include "type_id.h"

#include <any>
#include <functional>
#include <shared_mutex>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace inject
{
    class factory
    {
    public:
        template<typename T, typename Fn>
        void register_type(Fn&& fn)
        {
            using type_to = T;
            using type_from = function_traits<std::remove_reference_t<Fn>>::type_return;

            static_assert(std::is_convertible_v<type_from, type_to>, "inject::factory::register_type: Template parameter Fn must be a callable type returning a type implicitly convertible to template parameter T");

            auto fn_bind = [this, fn = std::forward<Fn>(fn)]() mutable
            {
                return resolve(fn);
            };

            std::unique_lock lock(m_factory_mutex); // Write operation - unique lock must be acquired
            auto result = m_factories.emplace(type_id::get<T>(), std::function<T()>(std::move(fn_bind)));

            if (!result.second)
            {
                throw factory_exception("A factory for the specified type has already been registered");
            }
        }

        template<typename T>
        bool is_registered() const
        {
            std::shared_lock lock(m_factory_mutex); // Read operation - shared lock acquired
            return m_factories.find(type_id::get<T>()) != m_factories.end();
        }

        template<typename T>
        T resolve() const
        {
            auto fn_any = find_factory(type_id::get<T>());
            auto fn = std::any_cast<std::function<T()>>(fn_any);

            return fn();
        }

        template<typename Fn>
        auto resolve(Fn&& fn) const
        {
            using type_args = function_traits<std::remove_reference_t<Fn>>::type_args;
            using type_args_tag = tag<type_args>;

            return std::apply(std::forward<Fn>(fn), resolve_args(type_args_tag()));
        }

    private:
        // Used to avoid needing to construct an instance of std::tuple
        template<typename T>
        struct tag
        {
        };

        template<typename T>
        T resolve_arg() const
        {
            return resolve<T>();
        }

        template<typename... Ts>
        std::tuple<Ts...> resolve_args(tag<std::tuple<Ts...>>) const
        {
            return std::make_tuple(resolve_arg<Ts>()...);
        }

        std::any find_factory(type_id id) const
        {
            std::shared_lock lock(m_factory_mutex); // Read operation - shared lock acquired

            if (auto it = m_factories.find(id); it != m_factories.end())
            {
                return it->second;
            }

            throw factory_exception("No factory has been registered for the specified type");
        }

        mutable std::shared_mutex m_factory_mutex;

        // Store std::any, rather than std::function<std::any(void)>, as std::any requires its contained value to be copy constructible
        std::unordered_map<type_id, std::any> m_factories;
    };
}
