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
#ifndef EARANGES_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP
#define EARANGES_ALGORITHM_LEXICOGRAPHICAL_COMPARE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/comparisons.hpp>
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
    EARANGES_FUNC_BEGIN(lexicographical_compare)

        /// \brief function template \c lexicographical_compare
        template(typename I0,
                 typename S0,
                 typename I1,
                 typename S1,
                 typename C = less,
                 typename P0 = identity,
                 typename P1 = identity)(
            requires input_iterator<I0> AND sentinel_for<S0, I0> AND
                input_iterator<I1> AND sentinel_for<S1, I1> AND
                indirect_strict_weak_order<C, projected<I0, P0>, projected<I1, P1>>)
        constexpr bool EARANGES_FUNC(lexicographical_compare)(I0 begin0,
                                                            S0 end0,
                                                            I1 begin1,
                                                            S1 end1,
                                                            C pred = C{},
                                                            P0 proj0 = P0{},
                                                            P1 proj1 = P1{})
        {
            for(; begin1 != end1; ++begin0, ++begin1)
            {
                if(begin0 == end0 ||
                   invoke(pred, invoke(proj0, *begin0), invoke(proj1, *begin1)))
                    return true;
                if(invoke(pred, invoke(proj1, *begin1), invoke(proj0, *begin0)))
                    return false;
            }
            return false;
        }

        /// \overload
        template(typename Rng0,
                 typename Rng1,
                 typename C = less,
                 typename P0 = identity,
                 typename P1 = identity)(
            requires input_range<Rng0> AND input_range<Rng1> AND
                indirect_strict_weak_order<C,
                                           projected<iterator_t<Rng0>, P0>,
                                           projected<iterator_t<Rng1>, P1>>)
        constexpr bool EARANGES_FUNC(lexicographical_compare)(Rng0 && rng0, 
                                                            Rng1 && rng1, 
                                                            C pred = C{}, 
                                                            P0 proj0 = P0{}, 
                                                            P1 proj1 = P1{}) //
        {
            return (*this)(begin(rng0),
                           end(rng0),
                           begin(rng1),
                           end(rng1),
                           eastl::move(pred),
                           eastl::move(proj0),
                           eastl::move(proj1));
        }

    EARANGES_FUNC_END(lexicographical_compare)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif