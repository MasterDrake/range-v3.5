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
#ifndef RANGES_V3_FUNCTIONAL_COMPOSE_HPP
#define RANGES_V3_FUNCTIONAL_COMPOSE_HPP

#include "EASTL/type_traits.h"
#include "EASTL/utility.h"

//#include <EASTL/type_traits.h>
//#include <EASTL/utility.h>

#include "../concepts/concepts.hpp"

#include "../detail/config.hpp"
#include "../functional/invoke.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    template<typename Second, typename First>
    struct composed
    {
    private:
        RANGES_NO_UNIQUE_ADDRESS
        First first_;
        RANGES_NO_UNIQUE_ADDRESS
        Second second_;

        // clang-format off
        template<typename A, typename B, typename... Ts>
        static constexpr auto //
        CPP_auto_fun(do_)(A &&a, B &&b, eastl::false_type, Ts &&... ts)
        (
            return invoke((B &&) b, invoke((A &&) a, (Ts &&) ts...))
        )
        template<typename A, typename B, typename... Ts>
        static constexpr auto CPP_auto_fun(do_)(A &&a, B &&b, eastl::true_type, Ts &&... ts)
        (
            return (invoke((A &&) a, (Ts &&) ts...), invoke((B &&) b))
        )
    public:
        composed() = default;
        // clang-format on
        constexpr composed(Second second, First first)
          : first_(eastl::move(first))
          , second_(eastl::move(second))
        {}
        // clang-format off
        template<typename... Ts>
        constexpr auto CPP_auto_fun(operator())(Ts &&... ts)(mutable &)
        (
            return composed::do_(first_,
                                 second_,
                                 eastl::is_void<invoke_result_t<First &, Ts...>>{},
                                 (Ts &&) ts...)
        )
        template<typename... Ts>
        constexpr auto CPP_auto_fun(operator())(Ts &&... ts)(const &)
        (
            return composed::do_((First const &)first_,
                                 (Second const &)second_,
                                 eastl::is_void<invoke_result_t<First const &, Ts...>>{},
                                 (Ts &&) ts...)
        )
        template<typename... Ts>
        constexpr auto CPP_auto_fun(operator())(Ts &&... ts)(mutable &&)
        (
            return composed::do_((First &&)first_,
                                 (Second &&)second_,
                                 eastl::is_void<invoke_result_t<First &&, Ts...>>{},
                                 (Ts &&) ts...)
        )
        // clang-format on
    };

    struct compose_fn
    {
        template<typename Second, typename First>
        constexpr composed<Second, First> operator()(Second second, First first) const
        {
            return {eastl::move(second), eastl::move(first)};
        }
    };

    /// \ingroup group-functional
    /// \sa `compose_fn`
    RANGES_INLINE_VARIABLE(compose_fn, compose)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif