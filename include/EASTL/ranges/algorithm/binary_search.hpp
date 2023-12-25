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
#ifndef RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP
#define RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP

//#include <EASTL/functional.h>
//#include <EASTL/utility.h>
#include "EASTL/functional.h"
#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../algorithm/lower_bound.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(binary_search)
        /// \brief function template \c binary_search
        ///
        /// range-based version of the \c binary_search std algorithm
        ///
        /// \pre `Rng` is a model of the `range` concept
        template(typename I,
                 typename S,
                 typename V,
                 typename C = less,
                 typename P = identity)(requires forward_iterator<I> AND sentinel_for<S, I> AND  indirect_strict_weak_order<C, V const *, projected<I, P>>)
        constexpr bool RANGES_FUNC(binary_search)(I first, S last, V const & val, C pred = C{}, P proj = P{})
        {
            first =lower_bound(eastl::move(first), last, val, ranges::ref(pred), ranges::ref(proj));
            return first != last && !invoke(pred, val, invoke(proj, *first));
        }

        /// \overload
        template(typename Rng, typename V, typename C = less, typename P = identity)(requires forward_range<Rng> AND indirect_strict_weak_order<C, V const *, projected<iterator_t<Rng>, P>>)
        constexpr bool RANGES_FUNC(binary_search)(Rng && rng, V const & val, C pred = C{}, P proj = P{}) //
        {
            static_assert(!is_infinite<Rng>::value, "Trying to binary search an infinite range");
            return (*this)(begin(rng), end(rng), val, eastl::move(pred), eastl::move(proj));
        }
    RANGES_FUNC_END(binary_search)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif