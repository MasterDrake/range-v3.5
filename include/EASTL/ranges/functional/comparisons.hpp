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
#ifndef RANGES_V3_FUNCTIONAL_COMPARISONS_HPP
#define RANGES_V3_FUNCTIONAL_COMPARISONS_HPP

#include "../concepts/concepts.hpp"

#include "../range_fwd.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-functional
    /// @{
    struct equal_to
    {
        template(typename T, typename U)(requires equality_comparable_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return (T &&) t == (U &&) u;
        }
        using is_transparent = void;
    };

    struct not_equal_to
    {
        template(typename T, typename U)(requires equality_comparable_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return !equal_to{}((T &&) t, (U &&) u);
        }
        using is_transparent = void;
    };

    struct less
    {
        template(typename T, typename U)(requires totally_ordered_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return (T &&) t < (U &&) u;
        }
        using is_transparent = void;
    };

    struct less_equal
    {
        template(typename T, typename U)(requires totally_ordered_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return !less{}((U &&) u, (T &&) t);
        }
        using is_transparent = void;
    };

    struct greater_equal
    {
        template(typename T, typename U)(requires totally_ordered_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return !less{}((T &&) t, (U &&) u);
        }
        using is_transparent = void;
    };

    struct greater
    {
        template(typename T, typename U)(requires totally_ordered_with<T, U>)
        constexpr bool operator()(T && t, U && u) const
        {
            return less{}((U &&) u, (T &&) t);
        }
        using is_transparent = void;
    };

#if __cplusplus > 201703L && __has_include(<compare>) && \
    defined(__cpp_concepts) && defined(__cpp_impl_three_way_comparison)
    struct compare_three_way
    {
        template(typename T, typename U)( requires three_way_comparable_with<T, U>)
        constexpr auto operator()(T && t, U && u) const -> decltype((T &&) t <=> (U &&) u)
        {
            return (T &&) t <=> (U &&) u;
        }

        using is_transparent = void;
    };
#endif // __cplusplus

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif