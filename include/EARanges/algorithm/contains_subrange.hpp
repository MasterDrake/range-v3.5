/// \file
// Range v3 library
//
//  Copyright Google LLC, 2021
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_CONTAINS_SUBRANGE_HPP
#define EARANGES_ALGORITHM_CONTAINS_SUBRANGE_HPP

#include <EASTL/utility.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/algorithm/search.hpp>
#include <EARanges/detail/config.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// `contains_subrange` is a search-based algorithm that checks whether or
    /// not a given "needle" range is a subrange of a "haystack" range.
    ///
    /// Example usage:
    /// ```cpp
    /// auto const haystack = eastl::vector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    /// auto const needle = eastl::vector{3, 4, 5};
    /// assert(ranges::contains(haystack, needle));
    /// assert(ranges::contains(haystack, needle, ranges::less));
    ///
    /// auto const not_a_needle = eastl::vector{4, 5, 3};
    /// assert(not ranges::contains(haystack, not_a_needle));
    /// ```
    ///
    /// The interface supports both iterator-sentinel pairs and range objects.
    /// Due to multi-pass iteration, this algorithm requires both ranges to be
    /// forward ranges, and the elements' projections need to be comparable when
    /// using the predicate.
    EARANGES_FUNC_BEGIN(contains_subrange)

        /// \brief function template \c contains
        template(typename I1, typename S1, typename I2, typename S2,
                 typename Pred = equal_to,
                 typename Proj1 = identity, typename Proj2 = identity)(
            /// \pre
            requires forward_iterator<I1> AND sentinel_for<S1, I1> AND
            forward_iterator<I2> AND sentinel_for<S2, I2> AND
            indirectly_comparable<I1, I2, Pred, Proj1, Proj2>)
        constexpr bool EARANGES_FUNC(contains)(I1 first1, S1 last1,
                                             I2 first2, S2 last2,
                                             Pred pred = {},
                                             Proj1 proj1 = {},
                                             Proj2 proj2 = {})
        {
            return first2 == last2 ||
                   ranges::search(first1, last1, first2, last2, pred, proj1, proj2).empty() == false;
        }

        /// \overload
        template(typename Rng1, typename Rng2, typename Pred = equal_to,typename Proj1 = identity, typename Proj2 = identity)(
            /// \pre
            requires forward_range<Rng1> AND forward_range<Rng2> AND
            indirectly_comparable<iterator_t<Rng1>, iterator_t<Rng2>, Pred, Proj1, Proj2>)
        constexpr bool EARANGES_FUNC(contains)(Rng1 && rng1, Rng2 && rng2,
                                             Pred pred = {},
                                             Proj1 proj1 = {},
                                             Proj2 proj2 = {})
        {
            return (*this)(ranges::begin(rng1), ranges::end(rng1),
                           ranges::begin(rng2), ranges::end(rng2),
                           eastl::move(pred), eastl::move(proj1), eastl::move(proj2));
        }

    EARANGES_FUNC_END(contains_subrange)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ALGORITHM_CONTAINS_SUBRANGE_HPP