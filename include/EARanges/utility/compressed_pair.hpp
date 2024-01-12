/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_UTILITY_COMPRESSED_PAIR_HPP
#define EARANGES_UTILITY_COMPRESSED_PAIR_HPP

#include <EASTL/type_traits.h>
#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/utility/box.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    template<typename First, typename Second>
    struct EARANGES_EMPTY_BASES compressed_pair
      : box<First, meta::size_t<0>>
      , box<Second, meta::size_t<1>>
    {
        using first_type = First;
        using second_type = Second;

        compressed_pair() = default;

        template(typename U, typename V)(
            requires constructible_from<First, U> AND constructible_from<Second, V>)
        constexpr compressed_pair(U && u, V && v) //
            noexcept(noexcept(First((U &&) u)) && noexcept(Second((V &&) v)))
          : box<First, meta::size_t<0>>{(U &&) u}
          , box<Second, meta::size_t<1>>{(V &&) v}
        {}

        constexpr First & first() &
        {
            return this->box<First, meta::size_t<0>>::get();
        }
        constexpr First const & first() const &
        {
            return this->box<First, meta::size_t<0>>::get();
        }
        constexpr First && first() &&
        {
            return static_cast<First &&>(this->box<First, meta::size_t<0>>::get());
        }

        constexpr Second & second() &
        {
            return this->box<Second, meta::size_t<1>>::get();
        }
        constexpr Second const & second() const &
        {
            return this->box<Second, meta::size_t<1>>::get();
        }
        constexpr Second && second() &&
        {
            return static_cast<Second &&>(this->box<Second, meta::size_t<1>>::get());
        }

        template(typename F, typename S)(requires convertible_to<First const &, F> AND convertible_to<Second const &, S>)
        constexpr operator eastl::pair<F, S>() const
        {
            return eastl::pair<F, S>{first(), second()};
        }
    };

    struct make_compressed_pair_fn
    {
        // clang-format off
        template<typename First, typename Second>
        constexpr auto CPP_auto_fun(operator())(First &&f, Second &&s) (const)
        (
            return compressed_pair<bind_element_t<First>, bind_element_t<Second>>{static_cast<First &&>(f), static_cast<Second &&>(s)}
        )
        // clang-format on
    };

    /// \ingroup group-utility
    /// \sa `make_compressed_pair_fn`
    EARANGES_INLINE_VARIABLE(make_compressed_pair_fn, make_compressed_pair)
} // namespace ranges

EARANGES_DIAGNOSTIC_PUSH
EARANGES_DIAGNOSTIC_IGNORE_MISMATCHED_TAGS
namespace eastl
{
    template<typename First, typename Second>
    struct tuple_size<::ranges::compressed_pair<First, Second>> : integral_constant<size_t, 2>
    {};

    template<typename First, typename Second>
    struct tuple_element<0, ::ranges::compressed_pair<First, Second>>
    {
        using type = First;
    };

    template<typename First, typename Second>
    struct tuple_element<1, ::ranges::compressed_pair<First, Second>>
    {
        using type = Second;
    };
} // namespace eastl
EARANGES_DIAGNOSTIC_POP

#include <EARanges/detail/epilogue.hpp>

#endif