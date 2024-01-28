/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/
#ifndef EARANGES_ALGORITHM_IS_SORTED_UNTIL_HPP
#define EARANGES_ALGORITHM_IS_SORTED_UNTIL_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(is_sorted_until)
            /// \brief template function \c is_sorted_until
            ///
            /// range-based version of the \c is_sorted_until eastl algorithm
            ///
            /// Works on forward_ranges
            ///
            /// \pre `Rng` is a model of the `forward_range` concept
            /// \pre `I` is a model of the `forward_iterator` concept
            /// \pre `S` and `I` model the `sentinel_for<S, I>` concept
            /// \pre `R` and `projected<I, P>` model the `indirect_strict_weak_order<R,
            /// projected<I, P>>` concept
            ///
            template(typename I, typename S, typename R = less, typename P = identity)(
                requires forward_iterator<I> AND sentinel_for<S, I> AND
                    indirect_strict_weak_order<R, projected<I, P>>) constexpr I
            EARANGES_FUNC(is_sorted_until)(I first, S last, R pred = R{}, P proj = P{})
            {
                auto i = first;
                if(first != last)
                {
                    while(++i != last)
                    {
                        if(invoke(pred, invoke(proj, *i), invoke(proj, *first)))
                            return i;
                        first = i;
                    }
                }
                return i;
            }

            /// \overload
            template(typename Rng, typename R = less, typename P = identity)(
                requires forward_range<Rng> AND indirect_strict_weak_order<
                    R,
                    projected<iterator_t<Rng>, P>>) constexpr borrowed_iterator_t<Rng> //
            EARANGES_FUNC(is_sorted_until)(Rng && rng, R pred = R{}, P proj = P{})
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(is_sorted_until)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif