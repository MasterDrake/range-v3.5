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
#ifndef RANGES_V3_ALGORITHM_EQUAL_HPP
#define RANGES_V3_ALGORITHM_EQUAL_HPP

#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

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

    RANGES_FUNC_BEGIN(equal)

        /// \brief function template \c equal
        template(typename I0,
                     typename S0,
                     typename I1,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
            requires input_iterator<I0> AND sentinel_for<S0, I0> AND
                input_iterator<I1> AND indirectly_comparable<I0, I1, C, P0, P1>)
        RANGES_DEPRECATED(
            "Use the variant of ranges::equal that takes an upper bound for "
            "both sequences")
        constexpr bool RANGES_FUNC(equal)(I0 begin0,
                                          S0 end0,
                                          I1 begin1,
                                          C pred = C{},
                                          P0 proj0 = P0{},
                                          P1 proj1 = P1{}) //
        {
            for(; begin0 != end0; ++begin0, ++begin1)
                if(!invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                    return false;
            return true;
        }

        /// \overload
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
        constexpr bool RANGES_FUNC(equal)(I0 begin0,
                                          S0 end0,
                                          I1 begin1,
                                          S1 end1,
                                          C pred = C{},
                                          P0 proj0 = P0{},
                                          P1 proj1 = P1{}) //
        {
            if(RANGES_CONSTEXPR_IF(sized_sentinel_for<S0, I0> &&
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
                     typename I1Ref,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(
            requires input_range<Rng0> AND input_iterator<uncvref_t<I1Ref>> AND
                indirectly_comparable<iterator_t<Rng0>, uncvref_t<I1Ref>, C, P0, P1>)
        RANGES_DEPRECATED(
            "Use the variant of ranges::equal that takes an upper bound for "
            "both sequences")
        constexpr bool RANGES_FUNC(equal)(Rng0 && rng0,
                                          I1Ref && begin1,
                                          C pred = C{},
                                          P0 proj0 = P0{},
                                          P1 proj1 = P1{}) //
        {
            RANGES_DIAGNOSTIC_PUSH
            RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
            return (*this)(begin(rng0),
                           end(rng0),
                           (I1Ref &&) begin1,
                           eastl::move(pred),
                           eastl::move(proj0),
                           eastl::move(proj1));
            RANGES_DIAGNOSTIC_POP
        }

        /// \overload
        template(typename Rng0,
                     typename Rng1,
                     typename C = equal_to,
                     typename P0 = identity,
                     typename P1 = identity)(requires input_range<Rng0> AND input_range<Rng1> AND indirectly_comparable<iterator_t<Rng0>, iterator_t<Rng1>, C, P0, P1>)
        constexpr bool RANGES_FUNC(equal)(Rng0 && rng0, Rng1 && rng1, C pred = C{}, P0 proj0 = P0{}, P1 proj1 = P1{}) //
        {
            if(RANGES_CONSTEXPR_IF(sized_range<Rng0> && sized_range<Rng1>))
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

    RANGES_FUNC_END(equal)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif