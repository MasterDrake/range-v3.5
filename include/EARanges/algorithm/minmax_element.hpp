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
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef EARANGES_ALGORITHM_MINMAX_ELEMENT_HPP
#define EARANGES_ALGORITHM_MINMAX_ELEMENT_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
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
        template<typename I>
        using minmax_element_result = detail::min_max_result<I, I>;

        EARANGES_FUNC_BEGIN(minmax_element)

            /// \brief function template \c minmax_element
            template(typename I, typename S, typename C = less, typename P = identity)(
                requires forward_iterator<I> AND sentinel_for<S, I> AND
                    indirect_strict_weak_order<
                        C,
                        projected<I, P>>) constexpr minmax_element_result<I>           //
            EARANGES_FUNC(minmax_element)(I first, S last, C pred = C{}, P proj = P{}) //
            {
                minmax_element_result<I> result{first, first};
                if(first == last || ++first == last)
                    return result;
                if(invoke(pred, invoke(proj, *first), invoke(proj, *result.min)))
                    result.min = first;
                else
                    result.max = first;
                while(++first != last)
                {
                    I tmp = first;
                    if(++first == last)
                    {
                        if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                            result.min = tmp;
                        else if(!invoke(
                                    pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                            result.max = tmp;
                        break;
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj, *first), invoke(proj, *tmp)))
                        {
                            if(invoke(
                                   pred, invoke(proj, *first), invoke(proj, *result.min)))
                                result.min = first;
                            if(!invoke(
                                   pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                                result.max = tmp;
                        }
                        else
                        {
                            if(invoke(
                                   pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                                result.min = tmp;
                            if(!invoke(
                                   pred, invoke(proj, *first), invoke(proj, *result.max)))
                                result.max = first;
                        }
                    }
                }
                return result;
            }

            /// \overload
            template(typename Rng, typename C = less, typename P = identity)(
                requires forward_range<Rng> AND indirect_strict_weak_order<
                    C,
                    projected<
                        iterator_t<Rng>,
                        P>>) constexpr minmax_element_result<borrowed_iterator_t<Rng>> //
            EARANGES_FUNC(minmax_element)(Rng && rng, C pred = C{}, P proj = P{})      //
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(minmax_element)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif