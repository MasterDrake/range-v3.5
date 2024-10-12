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
#ifndef EARANGES_ALGORITHM_INPLACE_MERGE_HPP
#define EARANGES_ALGORITHM_INPLACE_MERGE_HPP



#include <EASTL/utility.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/functional.h>
#include <EASTL/memory.h>
#include <EASTL/type_traits.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/lower_bound.hpp>
#include <EARanges/algorithm/merge.hpp>
#include <EARanges/algorithm/min.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/algorithm/rotate.hpp>
#include <EARanges/algorithm/upper_bound.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/functional/not_fn.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/move_iterators.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/reverse_iterator.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/memory.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/utility/swap.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        struct merge_adaptive_fn
        {
        private:
            template<typename I, typename C, typename P>
            static void impl(I first, I middle, I last, iter_difference_t<I> len1,
                             iter_difference_t<I> len2, iter_value_t<I> * const buf,
                             C & pred, P & proj)
            {
                auto tmpbuf = make_raw_buffer(buf);
                if(len1 <= len2)
                {
                    auto p = ranges::move(first, middle, tmpbuf.begin()).out;
                    merge(make_move_iterator(buf),
                          make_move_iterator(p.base().base()),
                          make_move_iterator(eastl::move(middle)),
                          make_move_iterator(eastl::move(last)),
                          eastl::move(first),
                          eastl::ref(pred),
                          eastl::ref(proj),
                          eastl::ref(proj));
                }
                else
                {
                    auto p = ranges::move(middle, last, tmpbuf.begin()).out;
                    using RBi = ranges::reverse_iterator<I>;
                    using Rv = ranges::reverse_iterator<iter_value_t<I> *>;
                    merge(make_move_iterator(RBi{eastl::move(middle)}),
                          make_move_iterator(RBi{eastl::move(first)}),
                          make_move_iterator(Rv{p.base().base()}),
                          make_move_iterator(Rv{buf}),
                          RBi{eastl::move(last)},
                          not_fn(eastl::ref(pred)),
                          eastl::ref(proj),
                          eastl::ref(proj));
                }
            }

        public:
            template(typename I, typename C = less, typename P = identity)(
                requires bidirectional_iterator<I> AND sortable<I, C, P>)
            void operator()(I first, I middle, I last, iter_difference_t<I> len1,
                            iter_difference_t<I> len2, iter_value_t<I> * buf,
                            std::ptrdiff_t buf_size, C pred = C{}, P proj = P{}) const
            {
                using D = iter_difference_t<I>;
                while(true)
                {
                    // if middle == last, we're done
                    if(len2 == 0)
                        return;
                    // shrink [first, middle) as much as possible (with no moves),
                    // returning if it shrinks to 0
                    for(; true; ++first, --len1)
                    {
                        if(len1 == 0)
                            return;
                        if(invoke(pred, invoke(proj, *middle), invoke(proj, *first)))
                            break;
                    }
                    if(len1 <= buf_size || len2 <= buf_size)
                    {
                        merge_adaptive_fn::impl(eastl::move(first),
                                                eastl::move(middle),
                                                eastl::move(last),
                                                len1,
                                                len2,
                                                buf,
                                                pred,
                                                proj);
                        return;
                    }
                    // first < middle < end
                    // *first > *middle
                    // partition [first, m1) [m1, middle) [middle, m2) [m2, last) such
                    // that
                    //     all elements in:
                    //         [first, m1)  <= [middle, m2)
                    //         [middle, m2) <  [m1, middle)
                    //         [m1, middle) <= [m2, last)
                    //     and m1 or m2 is in the middle of its range
                    I m1;    // "median" of [first, middle)
                    I m2;    // "median" of [middle, last)
                    D len11; // distance(first, m1)
                    D len21; // distance(middle, m2)
                    // binary search smaller range
                    if(len1 < len2)
                    { // len >= 1, len2 >= 2
                        len21 = len2 / 2;
                        m2 = next(middle, len21);
                        m1 = upper_bound(first,
                                         middle,
                                         invoke(proj, *m2),
                                         eastl::ref(pred),
                                         eastl::ref(proj));
                        len11 = distance(first, m1);
                    }
                    else
                    {
                        if(len1 == 1)
                        { // len1 >= len2 && len2 > 0, therefore len2 == 1
                            // It is known *first > *middle
                            ranges::iter_swap(first, middle);
                            return;
                        }
                        // len1 >= 2, len2 >= 1
                        len11 = len1 / 2;
                        m1 = next(first, len11);
                        m2 = lower_bound(middle,
                                         last,
                                         invoke(proj, *m1),
                                         eastl::ref(pred),
                                         eastl::ref(proj));
                        len21 = distance(middle, m2);
                    }
                    D len12 = len1 - len11; // distance(m1, middle)
                    D len22 = len2 - len21; // distance(m2, last)
                    // [first, m1) [m1, middle) [middle, m2) [m2, last)
                    // swap middle two partitions
                    middle = rotate(m1, eastl::move(middle), m2).begin();
                    // len12 and len21 now have swapped meanings
                    // merge smaller range with recursive call and larger with tail
                    // recursion elimination
                    if(len11 + len21 < len12 + len22)
                    {
                        (*this)(eastl::move(first),
                                eastl::move(m1),
                                middle,
                                len11,
                                len21,
                                buf,
                                buf_size,
                                eastl::ref(pred),
                                eastl::ref(proj));
                        first = eastl::move(middle);
                        middle = eastl::move(m2);
                        len1 = len12;
                        len2 = len22;
                    }
                    else
                    {
                        (*this)(middle,
                                eastl::move(m2),
                                eastl::move(last),
                                len12,
                                len22,
                                buf,
                                buf_size,
                                eastl::ref(pred),
                                eastl::ref(proj));
                        last = eastl::move(middle);
                        middle = eastl::move(m1);
                        len1 = len11;
                        len2 = len21;
                    }
                }
            }
        };

        EARANGES_INLINE_VARIABLE(merge_adaptive_fn, merge_adaptive)

        struct inplace_merge_no_buffer_fn
        {
            template(typename I, typename C = less, typename P = identity)(
                requires bidirectional_iterator<I> AND sortable<I, C, P>)
            void operator()(I first, I middle, I last, iter_difference_t<I> len1,
                            iter_difference_t<I> len2, C pred = C{}, P proj = P{}) const
            {
                merge_adaptive(eastl::move(first),
                               eastl::move(middle),
                               eastl::move(last),
                               len1,
                               len2,
                               static_cast<iter_value_t<I> *>(nullptr),
                               0,
                               eastl::move(pred),
                               eastl::move(proj));
            }
        };

        EARANGES_INLINE_VARIABLE(inplace_merge_no_buffer_fn, inplace_merge_no_buffer)
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(inplace_merge)

        // TODO reimplement to only need forward iterators -Eric Niebler

        /// \brief function template \c inplace_merge
        template(typename I, typename S, typename C = less, typename P = identity)(
            requires bidirectional_iterator<I> AND sortable<I, C, P>)
        I EARANGES_FUNC(inplace_merge)(
            I first, I middle, S last, C pred = C{}, P proj = P{})
        {
            using value_type = iter_value_t<I>;
            auto len1 = distance(first, middle);
            auto len2_and_end = enumerate(middle, last);
            auto buf_size = ranges::min(len1, len2_and_end.first);
            eastl::pair<value_type *, std::ptrdiff_t> buf{nullptr, 0};
            eastl::unique_ptr<value_type, detail::return_temporary_buffer> h;
            if(detail::is_trivially_copy_assignable_v<value_type> && 8 < buf_size)
            {
                buf = detail::get_temporary_buffer<value_type>(buf_size);
                h.reset(buf.first);
            }
            detail::merge_adaptive(eastl::move(first),
                                   eastl::move(middle),
                                   len2_and_end.second,
                                   len1,
                                   len2_and_end.first,
                                   buf.first,
                                   buf.second,
                                   eastl::move(pred),
                                   eastl::move(proj));
            return len2_and_end.second;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires bidirectional_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
        borrowed_iterator_t<Rng> EARANGES_FUNC(inplace_merge)(Rng && rng, iterator_t<Rng> middle, C pred = C{}, P proj = P{})
        {
            return (*this)(begin(rng),
                           eastl::move(middle),
                           end(rng),
                           eastl::move(pred),
                           eastl::move(proj));
        }

    EARANGES_FUNC_END(inplace_merge)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif