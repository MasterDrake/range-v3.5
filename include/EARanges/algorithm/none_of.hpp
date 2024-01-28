/// \file
// Range v3 library
//
//  Copyright Andrew Sutton 2014
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_NONE_OF_HPP
#define EARANGES_ALGORITHM_NONE_OF_HPP

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

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        EARANGES_FUNC_BEGIN(none_of)

            /// \brief function template \c none_of
            template(typename I, typename S, typename F, typename P = identity)(
                requires input_iterator<I> AND sentinel_for<S, I> AND
                    indirect_unary_predicate<F, projected<I, P>>) constexpr bool
            EARANGES_FUNC(none_of)(I first, S last, F pred, P proj = P{}) //
            {
                for(; first != last; ++first)
                    if(invoke(pred, invoke(proj, *first)))
                        return false;
                return true;
            }

            /// \overload
            template(typename Rng, typename F, typename P = identity)(
                requires input_range<Rng> AND indirect_unary_predicate<
                    F,
                    projected<iterator_t<Rng>, P>>) constexpr bool
            EARANGES_FUNC(none_of)(Rng && rng, F pred, P proj = P{}) //
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(none_of)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif