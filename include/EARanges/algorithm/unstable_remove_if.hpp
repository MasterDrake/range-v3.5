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
#ifndef EARANGES_ALGORITHM_UNSTABLE_REMOVE_IF_HPP
#define EARANGES_ALGORITHM_UNSTABLE_REMOVE_IF_HPP

#include <EASTL/functional.h>
#include <EASTL/utility.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/find_if.hpp>
#include <EARanges/algorithm/find_if_not.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/reverse_iterator.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/move.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{

        /// unstable_remove have O(1) complexity for each element remove, unlike remove
        /// O(n) [for worst case]. Each erased element overwritten (moved in) with last
        /// one. unstable_remove_if does not preserve relative element order.
        EARANGES_FUNC_BEGIN(unstable_remove_if)

            /// \brief function template \c unstable_remove_if
            template(typename I, typename C, typename P = identity)(
                requires bidirectional_iterator<I> AND permutable<I> AND
                    indirect_unary_predicate<C, projected<I, P>>) constexpr I
            EARANGES_FUNC(unstable_remove_if)(I first, I last, C pred, P proj = {})
            {
                while(true)
                {
                    first = find_if(
                        eastl::move(first), last, ranges::ref(pred), ranges::ref(proj));
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
                    permutable<iterator_t<Rng>>
                        AND indirect_unary_predicate<
                            C,
                            projected<iterator_t<Rng>,
                                      P>>) constexpr borrowed_iterator_t<Rng>   //
            EARANGES_FUNC(unstable_remove_if)(Rng && rng, C pred, P proj = P{}) //
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(unstable_remove_if)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ALGORITHM_UNSTABLE_REMOVE_IF_HPP