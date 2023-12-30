/// \file
// Range v3 library
//
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_ADJACENT_REMOVE_IF_HPP
#define EARANGES_ALGORITHM_ADJACENT_REMOVE_IF_HPP

//#include <EASTL/functional.h>
//#include <EASTL/utility.h>

#include "EASTL/functional.h"
#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/adjacent_find.hpp>
#include <EARanges/algorithm/move.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/move.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(adjacent_remove_if)
        /// \brief function \c adjacent_remove_if
        ///
        /// range-based version of the \c adjacent_remove_if algorithm
        ///
        /// \pre `Rng` is a model of the `forward_range` concept.
        /// \pre `Pred` is a model of the `BinaryPredicate` concept.
        template(typename I, typename S, typename Pred, typename Proj = identity)(requires permutable<I> AND sentinel_for<S, I> AND indirect_relation<Pred, projected<I, Proj>>)
        constexpr I EARANGES_FUNC(adjacent_remove_if)(I first, S last, Pred pred = {}, Proj proj = {})
        {
            first = adjacent_find(eastl::move(first), last, ranges::ref(pred), ranges::ref(proj));
            if(first == last)
                return first;

            auto i = first;
            for(auto j = ++i; ++j != last; ++i)
            {
                if(!invoke(pred, invoke(proj, *i), invoke(proj, *j)))
                {
                    *first = iter_move(i);
                    ++first;
                }
            }

            *first = iter_move(i);
            ++first;
            return first;
        }

        /// \overload
        template(typename Rng, typename Pred, typename Proj = identity)(requires forward_range<Rng> AND indirect_relation<Pred, projected<iterator_t<Rng>, Proj>> AND permutable<iterator_t<Rng>>)
        constexpr borrowed_iterator_t<Rng>
        EARANGES_FUNC(adjacent_remove_if)(Rng && rng, Pred pred, Proj proj = {}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }
    EARANGES_FUNC_END(adjacent_remove_if)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ALGORITHM_ADJACENT_REMOVE_IF_HPP