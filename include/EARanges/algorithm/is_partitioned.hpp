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
//===-------------------------- algorithm ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef EARANGES_ALGORITHM_IS_PARTITIONED_HPP
#define EARANGES_ALGORITHM_IS_PARTITIONED_HPP

#include <EARanges/meta/meta.hpp>

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
        EARANGES_FUNC_BEGIN(is_partitioned)

            /// \brief function template \c is_partitioned
            template(typename I, typename S, typename C, typename P = identity)(
                requires input_iterator<I> AND sentinel_for<S, I> AND
                    indirect_unary_predicate<C, projected<I, P>>) constexpr bool
            EARANGES_FUNC(is_partitioned)(I first, S last, C pred, P proj = P{}) //
            {
                for(; first != last; ++first)
                    if(!invoke(pred, invoke(proj, *first)))
                        break;
                for(; first != last; ++first)
                    if(invoke(pred, invoke(proj, *first)))
                        return false;
                return true;
            }

            /// \overload
            template(typename Rng, typename C, typename P = identity)(
                requires input_range<Rng> AND indirect_unary_predicate<
                    C,
                    projected<iterator_t<Rng>, P>>) constexpr bool
            EARANGES_FUNC(is_partitioned)(Rng && rng, C pred, P proj = P{}) //
            {
                return (*this)(
                    begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
            }

        EARANGES_FUNC_END(is_partitioned)

        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif