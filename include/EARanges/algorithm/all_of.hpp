/// \file
// Range v3 library
//
//  Copyright Andrew Sutton 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_ALL_OF_HPP
#define EARANGES_ALGORITHM_ALL_OF_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(all_of)

        /// \brief function template \c all_of
        template(typename I, typename S, typename F, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirect_unary_predicate<F, projected<I, P>>)
        constexpr bool EARANGES_FUNC(all_of)(I first, S last, F pred, P proj = P{}) //
        {
            return eastl::all_of(first, last, eastl::move(pred), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirect_unary_predicate<F, projected<iterator_t<Rng>, P>>)
        constexpr bool EARANGES_FUNC(all_of)(Rng && rng, F pred, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(all_of)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif