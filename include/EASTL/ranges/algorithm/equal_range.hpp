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
#ifndef RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP
#define RANGES_V3_ALGORITHM_EQUAL_RANGE_HPP

#include "../range_fwd.hpp"

#include "../algorithm/aux_/equal_range_n.hpp"
#include "../algorithm/aux_/lower_bound_n.hpp"
#include "../algorithm/upper_bound.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/operations.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"
#include "../view/subrange.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(equal_range)

        /// \brief function template \c equal_range
        template(typename I,
                 typename S,
                 typename V,
                 typename C = less,
                 typename P = identity)(requires forward_iterator<I> AND sentinel_for<S, I> AND indirect_strict_weak_order<C, V const *, projected<I, P>>)
        constexpr subrange<I> RANGES_FUNC(equal_range)(I first, S last, V const & val, C pred = C{}, P proj = P{})
        {
            if(RANGES_CONSTEXPR_IF(sized_sentinel_for<S, I>))
            {
                auto const len = distance(first, last);
                return aux::equal_range_n(eastl::move(first), len, val, eastl::move(pred), eastl::move(proj));
            }

            // Probe exponentially for either end-of-range, an iterator that
            // is past the equal range (i.e., denotes an element greater
            // than val), or is in the equal range (denotes an element equal
            // to val).
            auto dist = iter_difference_t<I>{1};
            while(true)
            {
                auto mid = first;
                auto d = advance(mid, dist, last);
                if(d || mid == last)
                {
                    // at the end of the input range
                    dist -= d;
                    return aux::equal_range_n(eastl::move(first), dist, val, ranges::ref(pred), ranges::ref(proj));
                }
                // if val < *mid, mid is after the target range.
                auto && v = *mid;
                auto && pv = invoke(proj, (decltype(v) &&)v);
                if(invoke(pred, val, pv))
                {
                    return aux::equal_range_n(eastl::move(first), dist, val, ranges::ref(pred), ranges::ref(proj));
                }
                else if(!invoke(pred, pv, val))
                {
                    // *mid == val: the lower bound is <= mid, and the upper bound is >
                    // mid.
                    return {
                        aux::lower_bound_n(
                            eastl::move(first), dist, val, ranges::ref(pred), ranges::ref(proj)),
                        upper_bound(eastl::move(mid),
                                    eastl::move(last),
                                    val,
                                    ranges::ref(pred),
                                    ranges::ref(proj))};
                }
                // *mid < val, mid is before the target range.
                first = eastl::move(mid);
                ++first;
                dist *= 2;
            }
        }

        /// \overload
        template(typename Rng, typename V, typename C = less, typename P = identity)(requires forward_range<Rng> AND indirect_strict_weak_order<C, V const *, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_subrange_t<Rng> //
        RANGES_FUNC(equal_range)(Rng && rng, V const & val, C pred = C{}, P proj = P{}) //
        {
            if(RANGES_CONSTEXPR_IF(sized_range<Rng>))
            {
                auto const len = distance(rng);
                return aux::equal_range_n(begin(rng), len, val, eastl::move(pred), eastl::move(proj));
            }

            return (*this)(begin(rng), end(rng), val, eastl::move(pred), eastl::move(proj));
        }

    RANGES_FUNC_END(equal_range)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif