/// \file
// Range v3 library
//
//  Copyright Johel Guerrero 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_CONTAINS_HPP
#define EARANGES_ALGORITHM_CONTAINS_HPP

#include <EASTL/utility.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/algorithm/find.hpp>
#include <EARanges/detail/config.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(contains)

            /// \brief function template \c contains
            template(typename I, typename S, typename T, typename P = identity)(
                requires input_iterator<I> AND sentinel_for<S, I> AND
                    indirect_relation<equal_to,
                                      projected<I, P>,
                                      const T *>) constexpr bool
            EARANGES_FUNC(contains)(I first, S last, const T & val, P proj = {})
            {
                return find(eastl::move(first), last, val, eastl::move(proj)) != last;
            }

            /// \overload
            template(typename Rng, typename T, typename P = identity)(
                requires input_range<Rng> AND
                    indirect_relation<equal_to,
                                      projected<iterator_t<Rng>, P>,
                                      const T *>) constexpr bool
            EARANGES_FUNC(contains)(Rng && rng, const T & val, P proj = {})
            {
                return (*this)(begin(rng), end(rng), val, eastl::move(proj));
            }

        EARANGES_FUNC_END(contains)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ALGORITHM_CONTAINS_HPP