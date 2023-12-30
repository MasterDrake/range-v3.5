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
#ifndef EARANGES_ALGORITHM_REMOVE_HPP
#define EARANGES_ALGORITHM_REMOVE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/find.hpp>
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
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(remove)

        /// \brief function template \c remove
        template(typename I, typename S, typename T, typename P = identity)(requires permutable<I> AND sentinel_for<S, I> AND indirect_relation<equal_to, projected<I, P>, T const *>)
        constexpr I EARANGES_FUNC(remove)(I first, S last, T const & val, P proj = P{})
        {
            first = find(eastl::move(first), last, val, ranges::ref(proj));
            if(first != last)
            {
                for(I i = next(first); i != last; ++i)
                {
                    if(!(invoke(proj, *i) == val))
                    {
                        *first = iter_move(i);
                        ++first;
                    }
                }
            }
            return first;
        }

        /// \overload
        template(typename Rng, typename T, typename P = identity)(requires forward_range<Rng> AND permutable<iterator_t<Rng>> AND indirect_relation<equal_to, projected<iterator_t<Rng>, P>, T const *>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(remove)(Rng && rng, T const & val, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), val, eastl::move(proj));
        }

    EARANGES_FUNC_END(remove)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif