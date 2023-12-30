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
#ifndef EARANGES_ALGORITHM_PARTIAL_SORT_HPP
#define EARANGES_ALGORITHM_PARTIAL_SORT_HPP

#include <EASTL/functional.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/heap_algorithm.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
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
    EARANGES_FUNC_BEGIN(partial_sort)

        /// \brief function template \c partial_sort
        template(typename I, typename S, typename C = less, typename P = identity)(requires sortable<I, C, P> AND random_access_iterator<I> AND sentinel_for<S, I>)
        constexpr I EARANGES_FUNC(partial_sort)(I first, I middle, S last, C pred = C{}, P proj = P{}) //
        {
            make_heap(first, middle, ranges::ref(pred), ranges::ref(proj));
            auto const len = middle - first;
            I i = middle;
            for(; i != last; ++i)
            {
                if(invoke(pred, invoke(proj, *i), invoke(proj, *first)))
                {
                    iter_swap(i, first);
                    detail::sift_down_n(first, len, first, ranges::ref(pred), ranges::ref(proj));
                }
            }
            sort_heap(first, middle, ranges::ref(pred), ranges::ref(proj));
            return i;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires sortable<iterator_t<Rng>, C, P> AND random_access_range<Rng>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(partial_sort)(Rng && rng, iterator_t<Rng> middle, C pred = C{}, P proj = P{}) //
        {
            return (*this)(begin(rng),
                           eastl::move(middle),
                           end(rng),
                           eastl::move(pred),
                           eastl::move(proj));
        }

    EARANGES_FUNC_END(partial_sort)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif