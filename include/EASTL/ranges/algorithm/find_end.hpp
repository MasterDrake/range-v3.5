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
#ifndef RANGES_V3_ALGORITHM_FIND_END_HPP
#define RANGES_V3_ALGORITHM_FIND_END_HPP

//#include <EASTL/utility.h>
#include "EASTL/utility.h"

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"

#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"
#include "../view/subrange.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \cond
    namespace detail
    {
        template(typename I, typename S)(requires input_iterator<I> AND sentinel_for<S, I>)
        constexpr I next_to_if(I i, S s, eastl::true_type)
        {
            return ranges::next(i, s);
        }

        template(typename I, typename S)(requires input_iterator<I> AND sentinel_for<S, I>)
        constexpr S next_to_if(I, S s, eastl::false_type)
        {
            return s;
        }

        template(bool B, typename I, typename S)(requires input_iterator<I> AND sentinel_for<S, I>)
        constexpr meta::if_c<B, I, S> next_to_if(I i, S s)
        {
            return detail::next_to_if(eastl::move(i), eastl::move(s), meta::bool_<B>{});
        }

        template<typename I1, typename S1, typename I2, typename S2, typename R,typename P>
        constexpr subrange<I1> find_end_impl(I1 begin1, S1 end1, I2 begin2, S2 end2, R pred, P proj, eastl::forward_iterator_tag, eastl::forward_iterator_tag)
        {
            bool found = false;
            I1 res_begin, res_end;
            if(begin2 == end2)
            {
                auto e1 = ranges::next(begin1, end1);
                return {e1, e1};
            }
            while(true)
            {
                while(true)
                {
                    if(begin1 == end1)
                        return {(found ? res_begin : begin1), (found ? res_end : begin1)};
                    if(invoke(pred, invoke(proj, *begin1), *begin2))
                        break;
                    ++begin1;
                }
                auto tmp1 = begin1;
                auto tmp2 = begin2;
                while(true)
                {
                    if(++tmp2 == end2)
                    {
                        res_begin = begin1++;
                        res_end = ++tmp1;
                        found = true;
                        break;
                    }
                    if(++tmp1 == end1)
                        return {(found ? res_begin : tmp1), (found ? res_end : tmp1)};
                    if(!invoke(pred, invoke(proj, *tmp1), *tmp2))
                    {
                        ++begin1;
                        break;
                    }
                }
            }
        }

        template<typename I1, typename I2, typename R, typename P>
        constexpr subrange<I1> find_end_impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj, eastl::bidirectional_iterator_tag, eastl::bidirectional_iterator_tag)
        {
            // modeled after search algorithm (in reverse)
            if(begin2 == end2)
                return {end1, end1}; // Everything matches an empty sequence
            I1 l1 = end1;
            I2 l2 = end2;
            --l2;
            while(true)
            {
                // Find end element in sequence 1 that matches *(end2-1), with a mininum
                // of loop checks
                do
                    // return {end1,end1} if no element matches *begin2
                    if(begin1 == l1)
                        return {end1, end1};
                while(!invoke(pred, invoke(proj, *--l1), *l2));
                // *l1 matches *l2, now match elements before here
                I1 m1 = l1;
                I2 m2 = l2;
                do
                    // If pattern exhausted, {m1,++l1} is the answer
                    // (works for 1 element pattern)
                    if(m2 == begin2)
                        return {m1, ++l1};
                    // Otherwise if source exhausted, pattern not found
                    else if(m1 == begin1)
                        return {end1, end1};
                // if there is a mismatch, restart with a new l1
                // else there is a match, check next elements
                while(invoke(pred, invoke(proj, *--m1), *--m2));
            }
        }

        template<typename I1, typename I2, typename R, typename P>
        constexpr subrange<I1> find_end_impl(I1 begin1, I1 end1, I2 begin2, I2 end2, R pred, P proj, eastl::random_access_iterator_tag, eastl::random_access_iterator_tag)
        {
            // Take advantage of knowing source and pattern lengths.  Stop short when
            // source is smaller than pattern
            auto len2 = end2 - begin2;
            if(len2 == 0)
                return {end1, end1};
            auto len1 = end1 - begin1;
            if(len1 < len2)
                return {end1, end1};
            I1 const start =
                begin1 + (len2 - 1); // End of pattern match can't go before here
            I1 l1 = end1;
            I2 l2 = end2;
            --l2;
            while(true)
            {
                do
                    if(start == l1)
                        return {end1, end1};
                while(!invoke(pred, invoke(proj, *--l1), *l2));
                I1 m1 = l1;
                I2 m2 = l2;
                do
                    if(m2 == begin2)
                        return {m1, ++l1};
                // no need to check range on m1 because s guarantees we have enough source
                while(invoke(pred, invoke(proj, *--m1), *--m2));
            }
        }
    } // namespace detail
    /// \endcond

    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(find_end)

        /// \brief function template \c find_end
        template(typename I1,
                 typename S1,
                 typename I2,
                 typename S2,
                 typename R = equal_to,
                 typename P = identity)(requires forward_iterator<I1> AND sentinel_for<S1, I1> AND forward_iterator<I2> AND sentinel_for<S2, I2> AND indirect_relation<R, projected<I1, P>, I2>)
        constexpr subrange<I1> RANGES_FUNC(find_end)(I1 begin1, S1 end1, I2 begin2, S2 end2, R pred = R{}, P proj = P{}) //
        {
            constexpr bool Bidi = bidirectional_iterator<I1> && bidirectional_iterator<I2>;
            return detail::find_end_impl(begin1,
                                         detail::next_to_if<Bidi>(begin1, end1),
                                         begin2,
                                         detail::next_to_if<Bidi>(begin2, end2),
                                         eastl::move(pred),
                                         eastl::move(proj),
                                         iterator_tag_of<I1>(),
                                         iterator_tag_of<I2>());
        }

        /// \overload
        template(typename Rng1,
                 typename Rng2,
                 typename R = equal_to,
                 typename P = identity)(requires forward_range<Rng1> AND forward_range<Rng2> AND indirect_relation<R, projected<iterator_t<Rng1>, P>, iterator_t<Rng2>>)
        constexpr borrowed_subrange_t<Rng1> RANGES_FUNC(find_end)(Rng1 && rng1, Rng2 && rng2, R pred = R{}, P proj = P{}) //
        {
            return (*this)(begin(rng1),
                           end(rng1),
                           begin(rng2),
                           end(rng2),
                           eastl::move(pred),
                           eastl::move(proj));
        }

    RANGES_FUNC_END(find_end)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif