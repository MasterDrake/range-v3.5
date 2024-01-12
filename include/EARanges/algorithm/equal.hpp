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
#ifndef EARANGES_ALGORITHM_EQUAL_HPP
#define EARANGES_ALGORITHM_EQUAL_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

//TODO: Check deprecated in equal
namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    /// \cond
    namespace detail
    {
        template<typename I0, typename S0, typename I1, typename S1, typename C, typename P0, typename P1>
        constexpr bool equal_nocheck(I0 begin0, S0 end0, I1 begin1, S1 end1, C pred, P0 proj0, P1 proj1)
        {
            for(; begin0 != end0 && begin1 != end1; ++begin0, ++begin1)
                if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                    return false;
            return begin0 == end0 && begin1 == end1;
        }
    } // namespace detail
    /// \endcond

    EARANGES_FUNC_BEGIN(equal)

        /// \brief function template \c equal
        template(typename I0,
                 typename S0,
                 typename I1,
                 typename S1,
                 typename C = equal_to,
                 typename P0 = identity,
                 typename P1 = identity)(
            requires input_iterator<I0> AND sentinel_for<S0, I0> AND
                input_iterator<I1> AND sentinel_for<S1, I1> AND
                indirectly_comparable<I0, I1, C, P0, P1>)
        constexpr bool EARANGES_FUNC(equal)(I0 begin0,
                                          S0 end0,
                                          I1 begin1,
                                          S1 end1,
                                          C pred = C{},
                                          P0 proj0 = P0{},
                                          P1 proj1 = P1{}) //
        {
            if(EARANGES_CONSTEXPR_IF(sized_sentinel_for<S0, I0> &&
                                   sized_sentinel_for<S1, I1>))
                if(distance(begin0, end0) != distance(begin1, end1))
                    return false;
            return detail::equal_nocheck(eastl::move(begin0),
                                         eastl::move(end0),
                                         eastl::move(begin1),
                                         eastl::move(end1),
                                         eastl::move(pred),
                                         eastl::move(proj0),
                                         eastl::move(proj1));
        }

        /// \overload
        template(typename Rng0,
                     typename Rng1,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(requires input_range<Rng0> AND input_range<Rng1> AND indirectly_comparable<iterator_t<Rng0>, iterator_t<Rng1>, C, P0, P1>)
        constexpr bool EARANGES_FUNC(equal)(Rng0 && rng0, Rng1 && rng1, C pred = C{}, P0 proj0 = P0{}, P1 proj1 = P1{}) //
        {
            if(EARANGES_CONSTEXPR_IF(sized_range<Rng0> && sized_range<Rng1>))
                if(distance(rng0) != distance(rng1))
                    return false;
            return detail::equal_nocheck(begin(rng0),
                                         end(rng0),
                                         begin(rng1),
                                         end(rng1),
                                         eastl::move(pred),
                                         eastl::move(proj0),
                                         eastl::move(proj1));
        }

    EARANGES_FUNC_END(equal)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif