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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef EARANGES_ALGORITHM_PARTITION_HPP
#define EARANGES_ALGORITHM_PARTITION_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

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
#include <EARanges/utility/swap.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I, typename S, typename C, typename P>
        constexpr I partition_impl(I first, S last, C pred, P proj, eastl::forward_iterator_tag)
        {
            while(true)
            {
                if(first == last)
                    return first;
                if(!invoke(pred, invoke(proj, *first)))
                    break;
                ++first;
            }
            for(I p = first; ++p != last;)
            {
                if(invoke(pred, invoke(proj, *p)))
                {
                    ranges::iter_swap(first, p);
                    ++first;
                }
            }
            return first;
        }

        template<typename I, typename S, typename C, typename P>
        constexpr I partition_impl(I first, S end_, C pred, P proj, eastl::bidirectional_iterator_tag)
        {
            I last = ranges::next(first, end_);
            while(true)
            {
                while(true)
                {
                    if(first == last)
                        return first;
                    if(!invoke(pred, invoke(proj, *first)))
                        break;
                    ++first;
                }
                do
                {
                    if(first == --last)
                        return first;
                } while(!invoke(pred, invoke(proj, *last)));
                ranges::iter_swap(first, last);
                ++first;
            }
        }
    } // namespace detail
    /// \endcond

    EARANGES_FUNC_BEGIN(partition)

        /// \brief function template \c partition
        template(typename I, typename S, typename C, typename P = identity)(requires permutable<I> AND sentinel_for<S, I> AND indirect_unary_predicate<C, projected<I, P>>)
        constexpr I EARANGES_FUNC(partition)(I first, S last, C pred, P proj = P{})
        {
            return detail::partition_impl(eastl::move(first),
                                          eastl::move(last),
                                          eastl::move(pred),
                                          eastl::move(proj),
                                          iterator_tag_of<I>());
        }

        /// \overload
        template(typename Rng, typename C, typename P = identity)(requires forward_range<Rng> AND permutable<iterator_t<Rng>> AND indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(partition)(Rng && rng, C pred, P proj = P{})
        {
            return detail::partition_impl(begin(rng),
                                          end(rng),
                                          eastl::move(pred),
                                          eastl::move(proj),
                                          iterator_tag_of<iterator_t<Rng>>());
        }

    EARANGES_FUNC_END(partition)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif