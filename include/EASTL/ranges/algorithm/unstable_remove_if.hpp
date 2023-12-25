/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//  Copyright Casey Carter 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP

#include "EASTL/functional.h"
#include "EASTL/utility.h"

#include "../concepts/concepts.hpp"

#include "../range_fwd.hpp"

#include "../algorithm/find_if.hpp"
#include "../algorithm/find_if_not.hpp"
#include "../functional/identity.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../iterator/reverse_iterator.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../utility/move.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// unstable_remove have O(1) complexity for each element remove, unlike remove O(n)
    /// [for worst case]. Each erased element overwritten (moved in) with last one.
    /// unstable_remove_if does not preserve relative element order.
    RANGES_FUNC_BEGIN(unstable_remove_if)

        /// \brief function template \c unstable_remove_if
        template(typename I, typename C, typename P = identity)(
            requires bidirectional_iterator<I> AND permutable<I> AND
            indirect_unary_predicate<C, projected<I, P>>)
        constexpr I RANGES_FUNC(unstable_remove_if)(I first, I last, C pred, P proj = {})
        {
            while(true)
            {
                first = find_if(eastl::move(first), last, ranges::ref(pred), ranges::ref(proj));
                if(first == last)
                    return first;

                last = next(find_if_not(make_reverse_iterator(eastl::move(last)),
                                        make_reverse_iterator(next(first)),
                                        ranges::ref(pred),
                                        ranges::ref(proj)))
                           .base();
                if(first == last)
                    return first;

                *first = iter_move(last);

                // discussion here: https://github.com/ericniebler/range-v3/issues/988
                ++first;
            }
        }

        /// \overload
        template(typename Rng, typename C, typename P = identity)(
            requires bidirectional_range<Rng> AND common_range<Rng> AND
            permutable<iterator_t<Rng>> AND
            indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        RANGES_FUNC(unstable_remove_if)(Rng && rng, C pred, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    RANGES_FUNC_END(unstable_remove_if)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_ALGORITHM_UNSTABLE_REMOVE_IF_HPP