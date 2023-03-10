#pragma once

#include "factory.h"

#include <atomic>
#include <memory>
#include <mutex> // std::call_once

namespace inject
{
    class container
    {
    public:
        template<typename T, typename Fn>
        void register_type(Fn&& fn)
        {
            return m_factory.register_type<T>(std::forward<Fn>(fn));
        }

        template<typename T, typename Fn>
        void register_cached(Fn&& fn)
        {
            auto fn_cache = [this, fn = std::forward<Fn>(fn), cache = std::make_shared<cache<T>>()]() mutable
            {
                return cache->get_value([&]() { return m_factory.resolve(fn); }); // The lambda isn't stored by the call to get_value so a default capture mode of '&' is fine
            };

            return register_type<T>(std::move(fn_cache));
        }

        template<typename T, typename Fn>
        void register_shared(Fn&& fn)
        {
            return register_cached<std::shared_ptr<T>>(std::forward<Fn>(fn));
        }

        template<typename T>
        bool is_registered() const
        {
            return m_factory.is_registered<T>();
        }

        template<typename T>
        bool is_registered_shared() const
        {
            return is_registered<std::shared_ptr<T>>();
        }

        template<typename T>
        T resolve() const
        {
            return m_factory.resolve<T>();
        }

        template<typename Fn>
        auto resolve(Fn&& fn) const
        {
            return m_factory.resolve(std::forward<Fn>(fn));
        }

        template<typename T>
        std::shared_ptr<T> resolve_shared() const
        {
            return resolve<std::shared_ptr<T>>();
        }

        factory& get_factory() noexcept
        {
            return m_factory;
        }

        const factory& get_factory() const noexcept
        {
            return m_factory;
        }

    private:
        // General case - T must be default constructible
        template<typename T>
        struct cache
        {
            template<typename Fn>
            T get_value(Fn&& fn)
            {
                std::call_once(m_flag, [&]()
                {
                    m_value = fn();
                });

                return m_value;
            }

            T m_value;
            std::once_flag m_flag;
        };

        // Specialization - std::shared_ptr<T>
        template<typename T>
        struct cache<std::shared_ptr<T>>
        {
            template<typename Fn>
            std::shared_ptr<T> get_value(Fn&& fn)
            {
                std::shared_ptr<T> value = std::atomic_load(&m_value);

                // Return the cached value if it exists otherwise create
                // the shared instance and assign it to the cached value
                if (!value)
                {
                    std::shared_ptr<T> value_desired = fn(); // It's possible, although unlikely, that the factory function is called by multiple threads during resolution

                    if (std::atomic_compare_exchange_strong(&m_value, &value, value_desired)) // The expected shared_ptr 'value' is empty
                    {
                        value = std::move(value_desired); // Successfully updated m_value so ensure the new value is returned
                    }
                }

                return value;
            }

            std::shared_ptr<T> m_value; // Replace with std::atomic<std::shared_ptr<T>> once using C++20
        };

        factory m_factory;
    };
}
