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
#ifndef RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP
#define RANGES_V3_ALGORITHM_PARTIAL_SORT_HPP

#include "EASTL/functional.h"

#include "../range_fwd.hpp"

#include "../algorithm/heap_algorithm.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
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
    RANGES_FUNC_BEGIN(partial_sort)

        /// \brief function template \c partial_sort
        template(typename I, typename S, typename C = less, typename P = identity)(requires sortable<I, C, P> AND random_access_iterator<I> AND sentinel_for<S, I>)
        constexpr I RANGES_FUNC(partial_sort)(I first, I middle, S last, C pred = C{}, P proj = P{}) //
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
        constexpr borrowed_iterator_t<Rng> RANGES_FUNC(partial_sort)(Rng && rng, iterator_t<Rng> middle, C pred = C{}, P proj = P{}) //
        {
            return (*this)(begin(rng),
                           eastl::move(middle),
                           end(rng),
                           eastl::move(pred),
                           eastl::move(proj));
        }

    RANGES_FUNC_END(partial_sort)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif