/// \file
// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_ENDS_WITH_HPP
#define EARANGES_ALGORITHM_ENDS_WITH_HPP

#include <EASTL/utility.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/algorithm/equal.hpp>
#include <EARanges/detail/config.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(ends_with)

            /// \brief function template \c ends_with
            template(typename I0,
                     typename S0,
                     typename I1,
                     typename S1,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
                requires((forward_iterator<I0> && sentinel_for<S0, I0>) ||
                         (input_iterator<I0> && sized_sentinel_for<S0, I0>))
                    AND((forward_iterator<I1> && sentinel_for<S1, I1>) ||
                        (input_iterator<I1> && sized_sentinel_for<S1, I1>))
                        AND indirectly_comparable<I0, I1, C, P0, P1>) constexpr bool
            EARANGES_FUNC(ends_with)(I0 begin0,
                                     S0 end0,
                                     I1 begin1,
                                     S1 end1,
                                     C pred = C{},
                                     P0 proj0 = P0{},
                                     P1 proj1 = P1{}) //
            {
                const auto drop = distance(begin0, end0) - distance(begin1, end1);
                if(drop < 0)
                    return false;
                return equal(next(eastl::move(begin0), drop),
                             eastl::move(end0),
                             eastl::move(begin1),
                             eastl::move(end1),
                             eastl::move(pred),
                             eastl::move(proj0),
                             eastl::move(proj1));
            }

            /// \overload
            template(typename Rng0,
                     typename Rng1,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
                requires(forward_range<Rng0> || (input_range<Rng0> && sized_range<Rng0>))
                    AND(forward_range<Rng1> || (input_range<Rng1> && sized_range<Rng1>))
                        AND indirectly_comparable<iterator_t<Rng0>,
                                                  iterator_t<Rng1>,
                                                  C,
                                                  P0,
                                                  P1>) constexpr bool
            EARANGES_FUNC(ends_with)(Rng0 && rng0,
                                     Rng1 && rng1,
                                     C pred = C{},
                                     P0 proj0 = P0{},
                                     P1 proj1 = P1{}) //
            {
                const auto drop = distance(rng0) - distance(rng1);
                if(drop < 0)
                    return false;
                return equal(next(begin(rng0), drop),
                             end(rng0),
                             begin(rng1),
                             end(rng1),
                             eastl::move(pred),
                             eastl::move(proj0),
                             eastl::move(proj1));
            }

        EARANGES_FUNC_END(ends_with)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif