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
#ifndef EARANGES_ALGORITHM_COUNT_HPP
#define EARANGES_ALGORITHM_COUNT_HPP

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
    EARANGES_FUNC_BEGIN(count)

        /// \brief function template \c count
        template(typename I, typename S, typename V, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirect_relation<equal_to, projected<I, P>, V const *>)
        constexpr iter_difference_t<I> //
        EARANGES_FUNC(count)(I first, S last, V const & val, P proj = P{})
        {
            iter_difference_t<I> n = 0;
            for(; first != last; ++first)
                if(invoke(proj, *first) == val)
                    ++n;
            return n;
        }

        /// \overload
        template(typename Rng, typename V, typename P = identity)(requires input_range<Rng> AND indirect_relation<equal_to, projected<iterator_t<Rng>, P>, V const *>)
        constexpr iter_difference_t<iterator_t<Rng>> //
        EARANGES_FUNC(count)(Rng && rng, V const & val, P proj = P{})
        {
            return (*this)(begin(rng), end(rng), val, eastl::move(proj));
        }

    EARANGES_FUNC_END(count)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif