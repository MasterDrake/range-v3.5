/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_FIND_IF_NOT_HPP
#define EARANGES_ALGORITHM_FIND_IF_NOT_HPP

#include <EASTL/utility.h>

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
    EARANGES_FUNC_BEGIN(find_if_not)
        /// \brief template function \c find_if_not
        ///
        /// range-based version of the \c find_if_not std algorithm
        ///
        /// \pre `Rng` is a model of the `range` concept
        /// \pre `I` is a model of the `input_iterator` concept
        /// \pre `S` is a model of the `sentinel_for<I>` concept
        /// \pre `P` is a model of the `invocable<V>` concept, where `V` is the
        ///      value type of I.
        /// \pre `F` models `predicate<X>`, where `X` is the result type
        ///      of `invocable<P, V>`
        template(typename I, typename S, typename F, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirect_unary_predicate<F, projected<I, P>>)
        constexpr I EARANGES_FUNC(find_if_not)(I first, S last, F pred, P proj = P{})
        {
            for(; first != last; ++first)
                if(!invoke(pred, invoke(proj, *first)))
                    break;
            return first;
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirect_unary_predicate<F, projected<iterator_t<Rng>, P>>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(find_if_not)(Rng && rng, F pred, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(find_if_not)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif