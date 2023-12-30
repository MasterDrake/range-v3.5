/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_FUNCTIONAL_OVERLOAD_HPP
#define EARANGES_FUNCTIONAL_OVERLOAD_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/concepts.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    /// \cond
    namespace detail
    {
        struct _id
        {
            template<typename T>
            using invoke = T;
        };
        struct _ref
        {
            template<typename T>
            using invoke = T &;
        };
        struct _cref
        {
            template<typename T>
            using invoke = T const &;
        };
        template<typename T>
        struct _bind_front
        {
            template<typename... Args>
            using invoke = invoke_result_t<T, Args...>;
        };
    } // namespace detail
    /// \endcond

    template<typename... Ts>
    struct overloaded;

    template<>
    struct overloaded<>
    {
    private:
        template<typename...>
        friend struct overloaded;
        template<typename, typename...>
        using _result_t = void;
    };

    template<typename First, typename... Rest>
    struct overloaded<First, Rest...>
    {
    private:
        template<typename...>
        friend struct overloaded;

        EARANGES_NO_UNIQUE_ADDRESS
        First first_;
        EARANGES_NO_UNIQUE_ADDRESS
        overloaded<Rest...> second_;

        template<typename Qual>
        using _result_first = detail::_bind_front<meta::invoke<Qual, First>>;
        template<typename Qual>
        struct _result_second
        {
            template<typename... Args>
            using invoke = typename overloaded<Rest...>
                ::template _result_t<Qual, Args...>;
        };

        template<typename Qual, typename... Args>
        using _result_t =
            meta::invoke<
                meta::conditional_t<
                    (bool) invocable<meta::invoke<Qual, First>, Args...>,
                    _result_first<Qual>,
                    _result_second<Qual>>,
                Args...>;

    public:
        overloaded() = default;
        constexpr overloaded(First first, Rest... rest)
          : first_(static_cast<First &&>(first))
          , second_{static_cast<Rest &&>(rest)...}
        {}

        template(typename... Args)(
            requires invocable<First, Args...>)
        constexpr _result_t<detail::_id, Args...> operator()(Args &&... args) &&
        {
            return invoke((First &&) first_, (Args &&) args...);
        }
        template(typename... Args)(
            requires (!invocable<First, Args...>) AND
                invocable<overloaded<Rest...>, Args...>)
        constexpr _result_t<detail::_id, Args...> operator()(Args &&... args) &&
        {
            return invoke((overloaded<Rest...> &&) second_, (Args &&) args...);
        }

        template(typename... Args)(
            requires invocable<First &, Args...>)
        constexpr _result_t<detail::_ref, Args...> operator()(Args &&... args) &
        {
            return invoke(first_, (Args &&) args...);
        }
        template(typename... Args)(
            requires (!invocable<First &, Args...>) AND
                invocable<overloaded<Rest...> &, Args...>)
        constexpr _result_t<detail::_ref, Args...> operator()(Args &&... args) &
        {
            return invoke(second_, (Args &&) args...);
        }

        template(typename... Args)(
            requires invocable<First const &, Args...>)
        constexpr _result_t<detail::_cref, Args...> operator()(Args &&... args) const &
        {
            return invoke(first_, (Args &&) args...);
        }
        template(typename... Args)(
            requires (!invocable<First const &, Args...>) AND
                invocable<overloaded<Rest...> const &, Args...>)
        constexpr _result_t<detail::_cref, Args...> operator()(Args &&... args) const &
        {
            return invoke(second_, (Args &&) args...);
        }
    };

    struct overload_fn
    {
        template<typename Fn>
        constexpr Fn operator()(Fn fn) const
        {
            return fn;
        }
        template<typename... Fns>
        constexpr overloaded<Fns...> operator()(Fns... fns) const
        {
            return overloaded<Fns...>{static_cast<Fns &&>(fns)...};
        }
    };

    /// \ingroup group-functional
    /// \sa `overload_fn`
    EARANGES_INLINE_VARIABLE(overload_fn, overload)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif