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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef EARANGES_ALGORITHM_PARTITION_POINT_HPP
#define EARANGES_ALGORITHM_PARTITION_POINT_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/aux_/partition_point_n.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
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

    EARANGES_FUNC_BEGIN(partition_point)

        /// \brief function template \c partition_point
        template(typename I, typename S, typename C, typename P = identity)(
            requires forward_iterator<I> AND sentinel_for<S, I> AND
            indirect_unary_predicate<C, projected<I, P>>)
        constexpr I EARANGES_FUNC(partition_point)(I first, S last, C pred, P proj = P{})
        {
            if(EARANGES_CONSTEXPR_IF(sized_sentinel_for<S, I>))
            {
                auto len = distance(first, eastl::move(last));
                return aux::partition_point_n(eastl::move(first), len, eastl::move(pred), eastl::move(proj));
            }

            // Probe exponentially for either last-of-range or an iterator
            // that is past the partition point (i.e., does not satisfy pred).
            auto len = iter_difference_t<I>{1};
            while(true)
            {
                auto mid = first;
                auto d = advance(mid, len, last);
                if(mid == last || !invoke(pred, invoke(proj, *mid)))
                {
                    len -= d;
                    return aux::partition_point_n(eastl::move(first), len, ranges::ref(pred), ranges::ref(proj));
                }
                first = eastl::move(mid);
                len *= 2;
            }
        }

        /// \overload
        template(typename Rng, typename C, typename P = identity)(requires forward_range<Rng> AND indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(partition_point)(Rng && rng, C pred, P proj = P{}) //
        {
            if(EARANGES_CONSTEXPR_IF(sized_range<Rng>))
            {
                auto len = distance(rng);
                return aux::partition_point_n(begin(rng), len, eastl::move(pred), eastl::move(proj));
            }
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(partition_point)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif