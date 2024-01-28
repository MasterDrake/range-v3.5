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
#ifndef EARANGES_ALGORITHM_SEARCH_N_HPP
#define EARANGES_ALGORITHM_SEARCH_N_HPP

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
            template<typename I, typename S, typename D, typename V, typename C,
                     typename P>
            constexpr subrange<I> search_n_sized_impl(I const begin_, S last, D const d_,
                                                      D cnt, V const & val, C & pred,
                                                      P & proj)
            {
                D d = d_; // always the distance from first to end
                auto first = uncounted(begin_);
                while(true)
                {
                    // Find first element in sequence 1 that matches val, with a mininum
                    // of loop checks
                    while(true)
                    {
                        if(d < cnt) // return the last if we've run out of room
                        {
                            auto e = ranges::next(
                                recounted(begin_, eastl::move(first), d_ - d),
                                eastl::move(last));
                            return {e, e};
                        }
                        if(invoke(pred, invoke(proj, *first), val))
                            break;
                        ++first;
                        --d;
                    }
                    // *first matches val, now match elements after here
                    auto m = first;
                    D c = 0;
                    while(true)
                    {
                        if(++c == cnt) // If pattern exhausted, first is the answer (works
                                       // for 1 element pattern)
                            return {recounted(begin_, eastl::move(first), d_ - d),
                                    recounted(begin_, eastl::move(++m), d_ - d)};
                        ++m; // No need to check, we know we have room to match
                             // successfully
                        if(!invoke(
                               pred,
                               invoke(proj, *m),
                               val)) // if there is a mismatch, restart with a new begin
                        {
                            first = next(eastl::move(m));
                            d -= (c + 1);
                            break;
                        } // else there is a match, check next elements
                    }
                }
            }

            template<typename I, typename S, typename D, typename V, typename C,
                     typename P>
            constexpr subrange<I> search_n_impl(I first, S last, D cnt, V const & val,
                                                C & pred, P & proj)
            {
                while(true)
                {
                    // Find first element in sequence 1 that matches val, with a mininum
                    // of loop checks
                    while(true)
                    {
                        if(first == last) // return last if no element matches val
                            return {first, first};
                        if(invoke(pred, invoke(proj, *first), val))
                            break;
                        ++first;
                    }
                    // *first matches val, now match elements after here
                    I m = first;
                    D c = 0;
                    while(true)
                    {
                        if(++c == cnt) // If pattern exhausted, first is the answer (works
                                       // for 1 element pattern)
                            return {first, ++m};
                        if(++m ==
                           last) // Otherwise if source exhausted, pattern not found
                            return {m, m};
                        if(!invoke(
                               pred,
                               invoke(proj, *m),
                               val)) // if there is a mismatch, restart with a new begin
                        {
                            first = next(eastl::move(m));
                            break;
                        } // else there is a match, check next elements
                    }
                }
            }
        } // namespace detail
        /// \endcond

        EARANGES_FUNC_BEGIN(search_n)

            /// \brief function template \c search_n
            template(typename I,
                     typename S,
                     typename V,
                     typename C = equal_to,
                     typename P = identity)(
                requires forward_iterator<I> AND sentinel_for<S, I> AND
                    indirectly_comparable<I, V const *, C, P>) constexpr subrange<I>
            EARANGES_FUNC(search_n)(I first,
                                    S last,
                                    iter_difference_t<I>
                                        cnt,
                                    V const & val,
                                    C pred = C{},
                                    P proj = P{}) //
            {
                if(cnt <= 0)
                    return {first, first};
                if(EARANGES_CONSTEXPR_IF(sized_sentinel_for<S, I>))
                    return detail::search_n_sized_impl(eastl::move(first),
                                                       eastl::move(last),
                                                       distance(first, last),
                                                       cnt,
                                                       val,
                                                       pred,
                                                       proj);
                else
                    return detail::search_n_impl(
                        eastl::move(first), eastl::move(last), cnt, val, pred, proj);
            }

            /// \overload
            template(
                typename Rng, typename V, typename C = equal_to, typename P = identity)(
                requires forward_range<Rng> AND
                    indirectly_comparable<iterator_t<Rng>,
                                          V const *,
                                          C,
                                          P>) constexpr borrowed_subrange_t<Rng>
            EARANGES_FUNC(search_n)(Rng && rng,
                                    iter_difference_t<iterator_t<Rng>>
                                        cnt,
                                    V const & val,
                                    C pred = C{},
                                    P proj = P{}) //
            {
                if(cnt <= 0)
                    return subrange<iterator_t<Rng>>{begin(rng), begin(rng)};
                if(EARANGES_CONSTEXPR_IF(sized_range<Rng>))
                    return detail::search_n_sized_impl(
                        begin(rng), end(rng), distance(rng), cnt, val, pred, proj);
                else
                    return detail::search_n_impl(
                        begin(rng), end(rng), cnt, val, pred, proj);
            }

        EARANGES_FUNC_END(search_n)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif