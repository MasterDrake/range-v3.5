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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef EARANGES_ALGORITHM_SEARCH_HPP
#define EARANGES_ALGORITHM_SEARCH_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/primitives.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/subrange.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{

        /// \cond
        namespace detail
        {
            template<typename I1, typename S1, typename D1, typename I2, typename S2,
                     typename D2, typename C, typename P1, typename P2>
            constexpr subrange<I1> search_sized_impl(I1 const begin1_, S1 end1,
                                                     D1 const d1_, I2 begin2, S2 end2,
                                                     D2 d2, C & pred, P1 & proj1,
                                                     P2 & proj2)
            {
                D1 d1 = d1_;
                auto begin1 = uncounted(begin1_);
                while(true)
                {
                    // Find first element in sequence 1 that matches *begin2, with a
                    // mininum of loop checks
                    while(true)
                    {
                        if(d1 < d2) // return the last if we've run out of room
                        {
                            auto e = ranges::next(
                                recounted(begin1_, eastl::move(begin1), d1_ - d1),
                                eastl::move(end1));
                            return {e, e};
                        }
                        if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                            break;
                        ++begin1;
                        --d1;
                    }
                    // *begin1 matches *begin2, now match elements after here
                    auto m1 = begin1;
                    I2 m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2) // If pattern exhausted, begin1 is the answer
                                         // (works for 1 element pattern)
                        {
                            return {recounted(begin1_, eastl::move(begin1), d1_ - d1),
                                    recounted(begin1_, eastl::move(++m1), d1_ - d1)};
                        }
                        ++m1; // No need to check, we know we have room to match
                              // successfully
                        if(!invoke(pred,
                                   invoke(proj1, *m1),
                                   invoke(proj2,
                                          *m2))) // if there is a mismatch, restart with a
                                                 // new begin1
                        {
                            ++begin1;
                            --d1;
                            break;
                        } // else there is a match, check next elements
                    }
                }
            }

            template<typename I1, typename S1, typename I2, typename S2, typename C,
                     typename P1, typename P2>
            constexpr subrange<I1> search_impl(I1 begin1, S1 end1, I2 begin2, S2 end2,
                                               C & pred, P1 & proj1, P2 & proj2)
            {
                while(true)
                {
                    // Find first element in sequence 1 that matches *begin2, with a
                    // mininum of loop checks
                    while(true)
                    {
                        if(begin1 == end1) // return end1 if no element matches *begin2
                            return {begin1, begin1};
                        if(invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                            break;
                        ++begin1;
                    }
                    // *begin1 matches *begin2, now match elements after here
                    I1 m1 = begin1;
                    I2 m2 = begin2;
                    while(true)
                    {
                        if(++m2 == end2) // If pattern exhausted, begin1 is the answer
                                         // (works for 1 element pattern)
                            return {begin1, ++m1};
                        if(++m1 ==
                           end1) // Otherwise if source exhausted, pattern not found
                            return {m1, m1};
                        if(!invoke(pred,
                                   invoke(proj1, *m1),
                                   invoke(proj2,
                                          *m2))) // if there is a mismatch, restart with a
                                                 // new begin1
                        {
                            ++begin1;
                            break;
                        } // else there is a match, check next elements
                    }
                }
            }
        } // namespace detail
        /// \endcond

        EARANGES_FUNC_BEGIN(search)

            /// \brief function template \c search
            template(typename I1,
                     typename S1,
                     typename I2,
                     typename S2,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)(
                requires forward_iterator<I1> AND sentinel_for<S1, I1> AND
                    forward_iterator<I2>
                        AND sentinel_for<S2, I2>
                            AND indirectly_comparable<I1,
                                                      I2,
                                                      C,
                                                      P1,
                                                      P2>) constexpr subrange<I1>
            EARANGES_FUNC(search)(I1 begin1,
                                  S1 end1,
                                  I2 begin2,
                                  S2 end2,
                                  C pred = C{},
                                  P1 proj1 = P1{},
                                  P2 proj2 = P2{}) //
            {
                if(begin2 == end2)
                    return {begin1, begin1};
                if(EARANGES_CONSTEXPR_IF(sized_sentinel_for<S1, I1> &&
                                         sized_sentinel_for<S2, I2>))
                    return detail::search_sized_impl(eastl::move(begin1),
                                                     eastl::move(end1),
                                                     distance(begin1, end1),
                                                     eastl::move(begin2),
                                                     eastl::move(end2),
                                                     distance(begin2, end2),
                                                     pred,
                                                     proj1,
                                                     proj2);
                else
                    return detail::search_impl(eastl::move(begin1),
                                               eastl::move(end1),
                                               eastl::move(begin2),
                                               eastl::move(end2),
                                               pred,
                                               proj1,
                                               proj2);
            }

            /// \overload
            template(typename Rng1,
                     typename Rng2,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)(
                requires forward_range<Rng1> AND forward_range<Rng2> AND
                    indirectly_comparable<iterator_t<Rng1>,
                                          iterator_t<Rng2>,
                                          C,
                                          P1,
                                          P2>) constexpr borrowed_subrange_t<Rng1>
            EARANGES_FUNC(search)(Rng1 && rng1,
                                  Rng2 && rng2,
                                  C pred = C{},
                                  P1 proj1 = P1{},
                                  P2 proj2 = P2{}) //
            {
                if(empty(rng2))
                    return subrange<iterator_t<Rng1>>{begin(rng1), begin(rng1)};
                if(EARANGES_CONSTEXPR_IF(sized_range<Rng1> && sized_range<Rng2>))
                    return detail::search_sized_impl(begin(rng1),
                                                     end(rng1),
                                                     distance(rng1),
                                                     begin(rng2),
                                                     end(rng2),
                                                     distance(rng2),
                                                     pred,
                                                     proj1,
                                                     proj2);
                else
                    return detail::search_impl(begin(rng1),
                                               end(rng1),
                                               begin(rng2),
                                               end(rng2),
                                               pred,
                                               proj1,
                                               proj2);
            }

        EARANGES_FUNC_END(search)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif