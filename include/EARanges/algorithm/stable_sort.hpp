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
//  Copyright (c) 1994
//  Hewlett-Packard Company
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Hewlett-Packard Company makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//
//  Copyright (c) 1996
//  Silicon Graphics Computer Systems, Inc.
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Silicon Graphics makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//

#ifndef EARANGES_ALGORITHM_STABLE_SORT_HPP
#define EARANGES_ALGORITHM_STABLE_SORT_HPP

#include <EASTL/unique_ptr.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/iterator.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/inplace_merge.hpp>
#include <EARanges/algorithm/merge.hpp>
#include <EARanges/algorithm/min.hpp>
#include <EARanges/algorithm/sort.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/move_iterators.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/memory.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I, typename C, typename P>
        void inplace_stable_sort(I first, I last, C & pred, P & proj)
        {
            if(last - first < 15)
                return detail::insertion_sort(first, last, pred, proj), void();
            I middle = first + (last - first) / 2;
            detail::inplace_stable_sort(first, middle, pred, proj);
            detail::inplace_stable_sort(middle, last, pred, proj);
            detail::inplace_merge_no_buffer(first,
                                            middle,
                                            last,
                                            middle - first,
                                            last - middle,
                                            eastl::ref(pred),
                                            eastl::ref(proj));
        }

        template<typename I1, typename I2, typename D, typename C, typename P>
        void merge_sort_loop(I1 first, I1 last, I2 result, D step_size, C & pred,
                             P & proj)
        {
            D two_step = 2 * step_size;
            while(last - first >= two_step)
            {
                result = merge(make_move_iterator(first),
                               make_move_iterator(first + step_size),
                               make_move_iterator(first + step_size),
                               make_move_iterator(first + two_step),
                               result,
                               eastl::ref(pred),
                               eastl::ref(proj),
                               eastl::ref(proj))
                             .out;
                first += two_step;
            }
            step_size = ranges::min(D(last - first), step_size);
            merge(make_move_iterator(first),
                  make_move_iterator(first + step_size),
                  make_move_iterator(first + step_size),
                  make_move_iterator(last),
                  result,
                  eastl::ref(pred),
                  eastl::ref(proj),
                  eastl::ref(proj));
        }

        constexpr int merge_sort_chunk_size()
        {
            return 7;
        }

        template<typename I, typename D, typename C, typename P>
        void chunk_insertion_sort(I first, I last, D chunk_size, C & pred, P & proj)
        {
            while(last - first >= chunk_size)
            {
                detail::insertion_sort(first, first + chunk_size, pred, proj);
                first += chunk_size;
            }
            detail::insertion_sort(first, last, pred, proj);
        }

        // buffer points to raw memory, we create objects, and then restore the buffer to
        // raw memory by destroying the objects on return.
        template<typename I, typename V, typename C, typename P>
        void merge_sort_with_buffer(I first, I last, V * buffer, C & pred, P & proj)
        {
            iter_difference_t<I> len = last - first, step_size = detail::merge_sort_chunk_size();
            detail::chunk_insertion_sort(first, last, step_size, pred, proj);
            if(step_size >= len)
                return;
            // The first call to merge_sort_loop moves into raw storage. Construct
            // on-demand and keep track of how many objects we need to destroy.
            V * buffer_end = buffer + static_cast<std::ptrdiff_t>(len);
            auto tmpbuf = make_raw_buffer(buffer);
            detail::merge_sort_loop(first, last, tmpbuf.begin(), step_size, pred, proj);
            step_size *= 2;
        loop:
            detail::merge_sort_loop(
                buffer, buffer_end, first, (std::ptrdiff_t)step_size, pred, proj);
            step_size *= 2;
            if(step_size >= len)
                return;
            detail::merge_sort_loop(first, last, buffer, step_size, pred, proj);
            step_size *= 2;
            goto loop;
        }

        // buffer points to raw memory
        template<typename I, typename V, typename C, typename P>
        void stable_sort_adaptive(I first, I last, V * buffer, std::ptrdiff_t buffer_size, C & pred, P & proj)
        {
            iter_difference_t<I> len = (last - first + 1) / 2;
            I middle = first + len;
            if(len > buffer_size)
            {
                detail::stable_sort_adaptive(first, middle, buffer, buffer_size, pred, proj);
                detail::stable_sort_adaptive(middle, last, buffer, buffer_size, pred, proj);
            }
            else
            {
                detail::merge_sort_with_buffer(first, middle, buffer, pred, proj);
                detail::merge_sort_with_buffer(middle, last, buffer, pred, proj);
            }
            detail::merge_adaptive(first,
                                   middle,
                                   last,
                                   middle - first,
                                   last - middle,
                                   buffer,
                                   buffer_size,
                                   eastl::ref(pred),
                                   eastl::ref(proj));
        }
    } // namespace detail
    /// \endcond

    EARANGES_FUNC_BEGIN(stable_sort)

        /// \brief function template \c stable_sort
        template(typename I, typename S, typename C = less, typename P = identity)(requires sortable<I, C, P> AND random_access_iterator<I> AND sentinel_for<S, I>)
        I EARANGES_FUNC(stable_sort)(I first, S end_, C pred = C{}, P proj = P{})
        {
            I last = ranges::next(first, end_);
            using D = iter_difference_t<I>;
            using V = iter_value_t<I>;
            D len = last - first;
            auto buf = len > 256 ? detail::get_temporary_buffer<V>(len) : detail::value_init{};
            eastl::unique_ptr<V, detail::return_temporary_buffer> h{buf.first};
            if(buf.first == nullptr)
                detail::inplace_stable_sort(first, last, pred, proj);
            else
                detail::stable_sort_adaptive(first, last, buf.first, buf.second, pred, proj);
            return last;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires sortable<iterator_t<Rng>, C, P> AND random_access_range<Rng>)
        borrowed_iterator_t<Rng> //
        EARANGES_FUNC(stable_sort)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(stable_sort)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif