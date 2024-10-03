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
#ifndef EARANGES_ALGORITHM_MAX_ELEMENT_HPP
#define EARANGES_ALGORITHM_MAX_ELEMENT_HPP

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
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(max_element)

        /// \brief function template \c max_element
        template(typename I, typename S, typename C = less, typename P = identity)(requires forward_iterator<I> AND sentinel_for<S, I> AND indirect_strict_weak_order<C, projected<I, P>>)
        constexpr I EARANGES_FUNC(max_element)(I first, S last, C pred = C{}, P proj = P{})
        {
            return eastl::max_element(first, last, eastl::move(pred), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename C = less, typename P = identity)(requires forward_range<Rng> AND indirect_strict_weak_order<C, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(max_element)(Rng && rng, C pred = C{}, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(max_element)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif