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

#ifndef RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP
#define RANGES_V3_ALGORITHM_MINMAX_ELEMENT_HPP

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/dangling.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I>
    using minmax_element_result = detail::min_max_result<I, I>;

    RANGES_FUNC_BEGIN(minmax_element)

        /// \brief function template \c minmax_element
        template(typename I, typename S, typename C = less, typename P = identity)(requires forward_iterator<I> AND sentinel_for<S, I> AND indirect_strict_weak_order<C, projected<I, P>>)
        constexpr minmax_element_result<I> //
        RANGES_FUNC(minmax_element)(I first, S last, C pred = C{}, P proj = P{}) //
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
                    else if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                        result.max = tmp;
                    break;
                }
                else
                {
                    if(invoke(pred, invoke(proj, *first), invoke(proj, *tmp)))
                    {
                        if(invoke(pred, invoke(proj, *first), invoke(proj, *result.min)))
                            result.min = first;
                        if(!invoke(pred, invoke(proj, *tmp), invoke(proj, *result.max)))
                            result.max = tmp;
                    }
                    else
                    {
                        if(invoke(pred, invoke(proj, *tmp), invoke(proj, *result.min)))
                            result.min = tmp;
                        if(!invoke(pred, invoke(proj, *first), invoke(proj, *result.max)))
                            result.max = first;
                    }
                }
            }
            return result;
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires forward_range<Rng> AND indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        constexpr minmax_element_result<borrowed_iterator_t<Rng>> //
        RANGES_FUNC(minmax_element)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    RANGES_FUNC_END(minmax_element)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif