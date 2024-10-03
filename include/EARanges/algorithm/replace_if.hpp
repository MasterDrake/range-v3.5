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
#ifndef EARANGES_ALGORITHM_REPLACE_IF_HPP
#define EARANGES_ALGORITHM_REPLACE_IF_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

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

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(replace_if)

        /// \brief function template \c replace_if
        template(typename I, typename S, typename C, typename T, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
                indirect_unary_predicate<C, projected<I, P>> AND
                indirectly_writable<I, T const &>)
        constexpr I EARANGES_FUNC(replace_if)(
            I first, S last, C pred, T const & new_value, P proj = P{}) //
        {
            return eastl::replace_if(first, last, eastl::move(pred), new_value, eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename C, typename T, typename P = identity)(
            requires input_range<Rng> AND
                indirect_unary_predicate<C, projected<iterator_t<Rng>, P>> AND
                indirectly_writable<iterator_t<Rng>, T const &>)
        constexpr borrowed_iterator_t<Rng> EARANGES_FUNC(replace_if)(Rng && rng, C pred, T const & new_value, P proj = P{}) //
        {
            return (*this)(
                begin(rng), end(rng), eastl::move(pred), new_value, eastl::move(proj));
        }

    EARANGES_FUNC_END(replace_if)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif