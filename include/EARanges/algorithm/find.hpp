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
#ifndef EARANGES_ALGORITHM_FIND_HPP
#define EARANGES_ALGORITHM_FIND_HPP

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
    EARANGES_FUNC_BEGIN(find)
        /// \brief template function \c find
        ///
        /// range-based version of the \c find std algorithm
        ///
        /// \pre `Rng` is a model of the `range` concept
        /// \pre `I` is a model of the `input_iterator` concept
        /// \pre `S` is a model of the `sentinel_for<I>` concept
        /// \pre `P` is a model of the `invocable<iter_common_reference_t<I>>` concept
        /// \pre The ResultType of `P` is equality_comparable with V
        template(typename I, typename S, typename V, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirect_relation<equal_to, projected<I, P>, V const *>)
        constexpr I EARANGES_FUNC(find)(I first, S last, V const & val, P proj = P{})
        {
            for(; first != last; ++first)
                if(invoke(proj, *first) == val)
                    break;
            return first;
        }

        /// \overload
        template(typename Rng, typename V, typename P = identity)(requires input_range<Rng> AND indirect_relation<equal_to, projected<iterator_t<Rng>, P>, V const *>)
        constexpr borrowed_iterator_t<Rng> //
        EARANGES_FUNC(find)(Rng && rng, V const & val, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), val, eastl::move(proj));
        }

    EARANGES_FUNC_END(find)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif