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
#ifndef EARANGES_FUNCTIONAL_COMPARISONS_HPP
#define EARANGES_FUNCTIONAL_COMPARISONS_HPP

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-functional
        /// @{
        struct equal_to
        {
            template(typename T, typename U)(requires equality_comparable_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return (T &&)t == (U &&)u;
            }
            using is_transparent = void;
        };

        struct not_equal_to
        {
            template(typename T,
                     typename U)(requires equality_comparable_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return !equal_to{}((T &&)t, (U &&)u);
            }
            using is_transparent = void;
        };

        struct less
        {
            template(typename T,
                     typename U)(requires totally_ordered_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return (T &&)t < (U &&)u;
            }
            using is_transparent = void;
        };

        struct less_equal
        {
            template(typename T,
                     typename U)(requires totally_ordered_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return !less{}((U &&)u, (T &&)t);
            }
            using is_transparent = void;
        };

        struct greater_equal
        {
            template(typename T,
                     typename U)(requires totally_ordered_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return !less{}((T &&)t, (U &&)u);
            }
            using is_transparent = void;
        };

        struct greater
        {
            template(typename T,
                     typename U)(requires totally_ordered_with<T, U>) constexpr bool
            operator()(T && t, U && u) const
            {
                return less{}((U &&)u, (T &&)t);
            }
            using is_transparent = void;
        };
        /*TODO: I don't know if we need it or where is it, especially the header or the
        three_way_comparable_with concept that i guess is in compare.hpp
        #if __cplusplus > 201703L && __has_include(<compare>) && \
            defined(__cpp_concepts) && defined(__cpp_impl_three_way_comparison)
            struct compare_three_way
            {
                template(typename T, typename U)( requires three_way_comparable_with<T,
        U>) constexpr auto operator()(T && t, U && u) const -> decltype((T &&) t <=> (U
        &&) u)
                {
                    return (T &&) t <=> (U &&) u;
                }

                using is_transparent = void;
            };
        #endif // __cplusplus
        */
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
