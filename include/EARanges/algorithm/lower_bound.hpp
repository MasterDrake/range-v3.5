/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_LOWER_BOUND_HPP
#define EARANGES_ALGORITHM_LOWER_BOUND_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/aux_/lower_bound_n.hpp>
#include <EARanges/algorithm/partition_point.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(lower_bound)

        /// \brief function template \c lower_bound
        template(typename I,
                 typename S,
                 typename V,
                 typename C = less,
                 typename P = identity)(
            requires forward_iterator<I> AND sentinel_for<S, I> AND
                indirect_strict_weak_order<C, V const *, projected<I, P>>)
        constexpr I EARANGES_FUNC(lower_bound)(I first, 
                                             S last, 
                                             V const & val, 
                                             C pred = C{}, 
                                             P proj = P{})
        {
            return partition_point(eastl::move(first),
                                   eastl::move(last),
                                   detail::make_lower_bound_predicate(pred, val),
                                   eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename V, typename C = less, typename P = identity)(
            requires forward_range<Rng> AND
                indirect_strict_weak_order<C, V const *, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(lower_bound)(Rng && rng, V const & val, C pred = C{}, P proj = P{})
        {
            return partition_point(rng, detail::make_lower_bound_predicate(pred, val), eastl::move(proj));
        }

    EARANGES_FUNC_END(lower_bound)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif