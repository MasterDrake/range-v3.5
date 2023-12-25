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
#ifndef RANGES_V3_ALGORITHM_REMOVE_IF_HPP
#define RANGES_V3_ALGORITHM_REMOVE_IF_HPP

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"

#include "../algorithm/find_if.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
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
    RANGES_FUNC_BEGIN(remove_if)

        /// \brief function template \c remove_if
        template(typename I, typename S, typename C, typename P = identity)(requires permutable<I> AND sentinel_for<S, I> AND indirect_unary_predicate<C, projected<I, P>>)
        constexpr I RANGES_FUNC(remove_if)(I first, S last, C pred, P proj = P{})
        {
            first = find_if(eastl::move(first), last, ranges::ref(pred), ranges::ref(proj));
            if(first != last)
            {
                for(I i = next(first); i != last; ++i)
                {
                    if(!(invoke(pred, invoke(proj, *i))))
                    {
                        *first = iter_move(i);
                        ++first;
                    }
                }
            }
            return first;
        }

        /// \overload
        template(typename Rng, typename C, typename P = identity)(requires forward_range<Rng> AND permutable<iterator_t<Rng>> AND indirect_unary_predicate<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> RANGES_FUNC(remove_if)(Rng && rng, C pred, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    RANGES_FUNC_END(remove_if)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif