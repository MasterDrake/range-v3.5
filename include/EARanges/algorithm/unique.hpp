/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
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

#ifndef EARANGES_ALGORITHM_UNIQUE_HPP
#define EARANGES_ALGORITHM_UNIQUE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/adjacent_find.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
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
    EARANGES_FUNC_BEGIN(unique)

        /// \brief template function \c unique
        ///
        /// range-based version of the \c unique eastl algorithm
        ///
        /// \pre `Rng` is a model of the `forward_range` concept
        /// \pre `I` is a model of the `forward_iterator` concept
        /// \pre `S` is a model of the `sentinel_for` concept
        /// \pre `C` is a model of the `relation` concept
        ///
        template(typename I, typename S, typename C = equal_to, typename P = identity)(requires sortable<I, C, P> AND sentinel_for<S, I>)
        constexpr I EARANGES_FUNC(unique)(I first, S last, C pred = C{}, P proj = P{})
        {
            return eastl::unique(first, last, eastl::move(pred), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename C = equal_to, typename P = identity)(requires sortable<iterator_t<Rng>, C, P> AND range<Rng>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(unique)(Rng && rng, C pred = C{}, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(unique)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif