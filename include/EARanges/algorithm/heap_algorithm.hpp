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
#ifndef EARANGES_ALGORITHM_HEAP_ALGORITHM_HPP
#define EARANGES_ALGORITHM_HEAP_ALGORITHM_HPP

#include <EASTL/functional.h>

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
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct is_heap_until_n_fn
        {
            template(typename I, typename C = less, typename P = identity)(requires random_access_iterator<I> AND indirect_strict_weak_order<C, projected<I, P>>)
            constexpr I operator()(I const begin_, 
                                   iter_difference_t<I> const n_, 
                                   C pred = C{},
                                   P proj = P{}) const
            {
                EARANGES_EXPECT(0 <= n_);
                iter_difference_t<I> p = 0, c = 1;
                I pp = begin_;
                while(c < n_)
                {
                    I cp = begin_ + c;
                    if(invoke(pred, invoke(proj, *pp), invoke(proj, *cp)))
                        return cp;
                    ++c;
                    ++cp;
                    if(c == n_ || invoke(pred, invoke(proj, *pp), invoke(proj, *cp)))
                        return cp;
                    ++p;
                    ++pp;
                    c = 2 * p + 1;
                }
                return begin_ + n_;
            }
        };

        EARANGES_INLINE_VARIABLE(is_heap_until_n_fn, is_heap_until_n)

        struct is_heap_n_fn
        {
            template(typename I, typename C = less, typename P = identity)(requires random_access_iterator<I> AND indirect_strict_weak_order<C, projected<I, P>>)
            constexpr bool operator()(I first, 
                                      iter_difference_t<I> n, 
                                      C pred = C{},
                                      P proj = P{}) const
            {
                return is_heap_until_n(first, n, eastl::move(pred), eastl::move(proj)) ==
                       first + n;
            }
        };

        EARANGES_INLINE_VARIABLE(is_heap_n_fn, is_heap_n)
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(is_heap_until)

        /// \brief function template \c is_heap_until
        template(typename I, typename S, typename C = less, typename P = identity)(
            requires random_access_iterator<I> AND sentinel_for<S, I> AND
            indirect_strict_weak_order<C, projected<I, P>>)
        constexpr I EARANGES_FUNC(is_heap_until)(I first, S last, C pred = C{}, P proj = P{})
        {
            return detail::is_heap_until_n(eastl::move(first),
                                           distance(first, last),
                                           eastl::move(pred),
                                           eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(
            requires random_access_range<Rng> AND
            indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(is_heap_until)(Rng && rng, C pred = C{}, P proj = P{})
        {
            return detail::is_heap_until_n(
                begin(rng), distance(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(is_heap_until)

    EARANGES_FUNC_BEGIN(is_heap)

        /// \brief function template \c is_heap
        template(typename I, typename S, typename C = less, typename P = identity)(
            requires random_access_iterator<I> AND sentinel_for<S, I> AND
            indirect_strict_weak_order<C, projected<I, P>>)
        constexpr bool EARANGES_FUNC(is_heap)(I first, S last, C pred = C{}, P proj = P{}) //
        {
            return detail::is_heap_n(eastl::move(first),
                                     distance(first, last),
                                     eastl::move(pred),
                                     eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(
            requires random_access_range<Rng> AND
            indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        constexpr bool EARANGES_FUNC(is_heap)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            return detail::is_heap_n(
                begin(rng), distance(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(is_heap)

    /// @}

    /// \cond
    namespace detail
    {
        struct sift_up_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            constexpr void operator()(I first, 
                                      iter_difference_t<I> len, 
                                      C pred = C{},
                                      P proj = P{}) const
            {
                if(len > 1)
                {
                    I last = first + len;
                    len = (len - 2) / 2;
                    I i = first + len;
                    if(invoke(pred, invoke(proj, *i), invoke(proj, *--last)))
                    {
                        iter_value_t<I> v = iter_move(last);
                        do
                        {
                            *last = iter_move(i);
                            last = i;
                            if(len == 0)
                                break;
                            len = (len - 1) / 2;
                            i = first + len;
                        } while(invoke(pred, invoke(proj, *i), invoke(proj, v)));
                        *last = eastl::move(v);
                    }
                }
            }
        };

        EARANGES_INLINE_VARIABLE(sift_up_n_fn, sift_up_n)

        struct sift_down_n_fn
        {
            template<typename I, typename C = less, typename P = identity>
            constexpr void operator()(I first, 
                                      iter_difference_t<I> len, 
                                      I start, 
                                      C pred = C{},
                                      P proj = P{}) const
            {
                // left-child of start is at 2 * start + 1
                // right-child of start is at 2 * start + 2
                auto child = start - first;

                if(len < 2 || (len - 2) / 2 < child)
                    return;

                child = 2 * child + 1;
                I child_i = first + child;

                if((child + 1) < len &&
                   invoke(pred, invoke(proj, *child_i), invoke(proj, *(child_i + 1))))
                {
                    // right-child exists and is greater than left-child
                    ++child_i;
                    ++child;
                }

                // check if we are in heap-order
                if(invoke(pred, invoke(proj, *child_i), invoke(proj, *start)))
                    // we are, start is larger than it's largest child
                    return;

                iter_value_t<I> top = iter_move(start);
                do
                {
                    // we are not in heap-order, swap the parent with it's largest child
                    *start = iter_move(child_i);
                    start = child_i;

                    if((len - 2) / 2 < child)
                        break;

                    // recompute the child based off of the updated parent
                    child = 2 * child + 1;
                    child_i = first + child;

                    if((child + 1) < len &&
                       invoke(pred, invoke(proj, *child_i), invoke(proj, *(child_i + 1))))
                    {
                        // right-child exists and is greater than left-child
                        ++child_i;
                        ++child;
                    }

                    // check if we are in heap-order
                } while(!invoke(pred, invoke(proj, *child_i), invoke(proj, top)));
                *start = eastl::move(top);
            }
        };

        EARANGES_INLINE_VARIABLE(sift_down_n_fn, sift_down_n)
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(push_heap)

        /// \brief function template \c push_heap
        template(typename I, typename S, typename C = less, typename P = identity)(
            requires random_access_iterator<I> AND sentinel_for<S, I> AND
            sortable<I, C, P>)
        constexpr I EARANGES_FUNC(push_heap)(I first, S last, C pred = C{}, P proj = P{})
        {
            auto n = distance(first, last);
            detail::sift_up_n(first, n, eastl::move(pred), eastl::move(proj));
            return first + n;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires random_access_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(push_heap)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            iterator_t<Rng> first = ranges::begin(rng);
            auto n = distance(rng);
            detail::sift_up_n(first, n, eastl::move(pred), eastl::move(proj));
            return first + n;
        }

    EARANGES_FUNC_END(push_heap)

    /// @}

    /// \cond
    namespace detail
    {
        struct pop_heap_n_fn
        {
            template(typename I, typename C = less, typename P = identity)(requires random_access_iterator<I> AND sortable<I, C, P>)
            constexpr void operator()(I first, 
                                      iter_difference_t<I> len, 
                                      C pred = C{},
                                      P proj = P{}) const
            {
                if(len > 1)
                {
                    ranges::iter_swap(first, first + (len - 1));
                    detail::sift_down_n(
                        first, len - 1, first, eastl::move(pred), eastl::move(proj));
                }
            }
        };

        EARANGES_INLINE_VARIABLE(pop_heap_n_fn, pop_heap_n)
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(pop_heap)

        /// \brief function template \c pop_heap
        template(typename I, typename S, typename C = less, typename P = identity)(requires random_access_iterator<I> AND sentinel_for<S, I> AND sortable<I, C, P>)
        constexpr I EARANGES_FUNC(pop_heap)(I first, S last, C pred = C{}, P proj = P{})
        {
            auto n = distance(first, last);
            detail::pop_heap_n(first, n, eastl::move(pred), eastl::move(proj));
            return first + n;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires random_access_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(pop_heap)(Rng && rng, C pred = C{}, P proj = P{})
        {
            iterator_t<Rng> first = ranges::begin(rng);
            auto n = distance(rng);
            detail::pop_heap_n(first, n, eastl::move(pred), eastl::move(proj));
            return first + n;
        }

    EARANGES_FUNC_END(pop_heap)

    EARANGES_FUNC_BEGIN(make_heap)

        /// \brief function template \c make_heap
        template(typename I, typename S, typename C = less, typename P = identity)(requires random_access_iterator<I> AND sentinel_for<S, I> AND sortable<I, C, P>)
        constexpr I EARANGES_FUNC(make_heap)(I first, S last, C pred = C{}, P proj = P{})
        {
            iter_difference_t<I> const n = distance(first, last);
            if(n > 1)
                // start from the first parent, there is no need to consider children
                for(auto start = (n - 2) / 2; start >= 0; --start)
                    detail::sift_down_n(
                        first, n, first + start, ranges::ref(pred), ranges::ref(proj));
            return first + n;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires random_access_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(make_heap)(Rng && rng, C pred = C{}, P proj = P{})
        {
            iterator_t<Rng> first = ranges::begin(rng);
            auto const n = distance(rng);
            if(n > 1)
                // start from the first parent, there is no need to consider children
                for(auto start = (n - 2) / 2; start >= 0; --start)
                    detail::sift_down_n(
                        first, n, first + start, ranges::ref(pred), ranges::ref(proj));
            return first + n;
        }

    EARANGES_FUNC_END(make_heap)

    EARANGES_FUNC_BEGIN(sort_heap)

        template(typename I, typename S, typename C = less, typename P = identity)(requires random_access_iterator<I> AND sentinel_for<S, I> AND sortable<I, C, P>)
        constexpr I EARANGES_FUNC(sort_heap)(I first, S last, C pred = C{}, P proj = P{})
        {
            iter_difference_t<I> const n = distance(first, last);
            for(auto i = n; i > 1; --i)
                detail::pop_heap_n(first, i, ranges::ref(pred), ranges::ref(proj));
            return first + n;
        }

        template(typename Rng, typename C = less, typename P = identity)(requires random_access_range<Rng &> AND sortable<iterator_t<Rng>, C, P>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(sort_heap)(Rng && rng, C pred = C{}, P proj = P{})
        {
            iterator_t<Rng> first = ranges::begin(rng);
            auto const n = distance(rng);
            for(auto i = n; i > 1; --i)
                detail::pop_heap_n(first, i, ranges::ref(pred), ranges::ref(proj));
            return first + n;
        }

    EARANGES_FUNC_END(sort_heap)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif