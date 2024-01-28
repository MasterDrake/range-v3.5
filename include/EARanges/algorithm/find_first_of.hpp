/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_FIND_FIRST_OF_HPP
#define EARANGES_ALGORITHM_FIND_FIRST_OF_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

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

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(find_first_of)
            // Rationale: return I0 instead of pair<I0,I1> because find_first_of need
            // not actually compute the end of [I1,S0); therefore, it is not necessarily
            // losing information. E.g., if begin0 == end0, we can return begin0
            // immediately. If we returned pair<I0,I1>, we would need to do an O(N) scan
            // to find the end position.

            /// \brief function template \c find_first_of
            template(typename I0,
                     typename S0,
                     typename I1,
                     typename S1,
                     typename R = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
                requires input_iterator<I0> AND sentinel_for<S0, I0> AND
                    forward_iterator<I1>
                        AND sentinel_for<S1, I1>
                            AND indirect_relation<R,
                                                  projected<I0, P0>,
                                                  projected<I1, P1>>) constexpr I0
            EARANGES_FUNC(find_first_of)(I0 begin0,
                                         S0 end0,
                                         I1 begin1,
                                         S1 end1,
                                         R pred = R{},
                                         P0 proj0 = P0{},
                                         P1 proj1 = P1{}) //
            {
                for(; begin0 != end0; ++begin0)
                    for(auto tmp = begin1; tmp != end1; ++tmp)
                        if(invoke(pred, invoke(proj0, *begin0), invoke(proj1, *tmp)))
                            return begin0;
                return begin0;
            }

            /// \overload
            template(typename Rng0,
                     typename Rng1,
                     typename R = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
                requires input_range<Rng0> AND forward_range<Rng1> AND indirect_relation<
                    R,
                    projected<iterator_t<Rng0>, P0>,
                    projected<iterator_t<Rng1>, P1>>) constexpr borrowed_iterator_t<Rng0>
            EARANGES_FUNC(find_first_of)(Rng0 && rng0,
                                         Rng1 && rng1,
                                         R pred = R{},
                                         P0 proj0 = P0{},
                                         P1 proj1 = P1{}) //
            {
                return (*this)(begin(rng0),
                               end(rng0),
                               begin(rng1),
                               end(rng1),
                               eastl::move(pred),
                               eastl::move(proj0),
                               eastl::move(proj1));
            }

        EARANGES_FUNC_END(find_first_of)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif