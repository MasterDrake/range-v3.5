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
#ifndef RANGES_V3_ALGORITHM_LOWER_BOUND_HPP
#define RANGES_V3_ALGORITHM_LOWER_BOUND_HPP

#include "../range_fwd.hpp"

#include "../algorithm/aux_/lower_bound_n.hpp"
#include "../algorithm/partition_point.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../iterator/operations.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/dangling.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(lower_bound)

        /// \brief function template \c lower_bound
        template(typename I,
                 typename S,
                 typename V,
                 typename C = less,
                 typename P = identity)(
            requires forward_iterator<I> AND sentinel_for<S, I> AND
                indirect_strict_weak_order<C, V const *, projected<I, P>>)
        constexpr I RANGES_FUNC(lower_bound)(I first, 
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
        RANGES_FUNC(lower_bound)(Rng && rng, V const & val, C pred = C{}, P proj = P{})
        {
            return partition_point(rng, detail::make_lower_bound_predicate(pred, val), eastl::move(proj));
        }

    RANGES_FUNC_END(lower_bound)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif