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
#ifndef RANGES_V3_ALGORITHM_MISMATCH_HPP
#define RANGES_V3_ALGORITHM_MISMATCH_HPP

#include "EASTL/utility.h"

#include "../meta/meta.hpp"

#include "../range_fwd.hpp"

#include "../algorithm/result_types.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/dangling.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"
//TODO: remove the uneeded function overload in mismatch
namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I1, typename I2>
    using mismatch_result = detail::in1_in2_result<I1, I2>;

    RANGES_FUNC_BEGIN(mismatch)

        /// \brief function template \c mismatch
        template(typename I1,
                     typename S1,
                     typename I2,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)(
            requires input_iterator<I1> AND sentinel_for<S1, I1> AND
                input_iterator<I2> AND
                indirect_relation<C, projected<I1, P1>, projected<I2, P2>>)
        RANGES_DEPRECATED(
            "Use the variant of ranges::mismatch that takes an upper bound for "
            "both sequences")
        mismatch_result<I1, I2> RANGES_FUNC(mismatch)(I1 begin1,
                                                      S1 end1,
                                                      I2 begin2,
                                                      C pred = C{},
                                                      P1 proj1 = P1{},
                                                      P2 proj2 = P2{}) //
        {
            for(; begin1 != end1; ++begin1, ++begin2)
                if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    break;
            return {begin1, begin2};
        }

        /// \overload
        template(typename I1,
                     typename S1,
                     typename I2,
                     typename S2,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)(
            requires input_iterator<I1> AND sentinel_for<S1, I1> AND
                input_iterator<I2> AND sentinel_for<S2, I2> AND
                indirect_relation<C, projected<I1, P1>, projected<I2, P2>>)
        constexpr mismatch_result<I1, I2> RANGES_FUNC(mismatch)(I1 begin1,
                                                                S1 end1,
                                                                I2 begin2,
                                                                S2 end2,
                                                                C pred = C{},
                                                                P1 proj1 = P1{},
                                                                P2 proj2 = P2{}) //
        {
            for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                if(!invoke(pred, invoke(proj1, *begin1), invoke(proj2, *begin2)))
                    break;
            return {begin1, begin2};
        }

        /// \overload
        template(typename Rng1,
                     typename I2Ref,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)( //s
            requires input_range<Rng1> AND input_iterator<uncvref_t<I2Ref>> AND
                indirect_relation<C,
                                  projected<iterator_t<Rng1>, P1>,
                                  projected<uncvref_t<I2Ref>, P2>>)
        RANGES_DEPRECATED(
            "Use the variant of ranges::mismatch that takes an upper bound for "
            "both sequences")
        mismatch_result<borrowed_iterator_t<Rng1>, uncvref_t<I2Ref>>
        RANGES_FUNC(mismatch)(Rng1 && rng1,
                              I2Ref && begin2,
                              C pred = C{}, // see below [*]
                              P1 proj1 = P1{},
                              P2 proj2 = P2{}) //
        {
            RANGES_DIAGNOSTIC_PUSH
            RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
            return (*this)(begin(rng1),
                           end(rng1),
                           static_cast<uncvref_t<I2Ref> &&>(begin2),
                           eastl::move(pred),
                           eastl::move(proj1),
                           eastl::move(proj2));
            RANGES_DIAGNOSTIC_POP
        }

        /// \overload
        template(typename Rng1,
                     typename Rng2,
                     typename C = equal_to,
                     typename P1 = identity,
                     typename P2 = identity)(
            requires input_range<Rng1> AND input_range<Rng2> AND
                indirect_relation<C,
                                  projected<iterator_t<Rng1>, P1>,
                                  projected<iterator_t<Rng2>, P2>>)
        constexpr mismatch_result<borrowed_iterator_t<Rng1>, borrowed_iterator_t<Rng2>> //
        RANGES_FUNC(mismatch)(Rng1 && rng1,
                              Rng2 && rng2,
                              C pred = C{},
                              P1 proj1 = P1{},
                              P2 proj2 = P2{}) //
        {
            return (*this)(begin(rng1),
                           end(rng1),
                           begin(rng2),
                           end(rng2),
                           eastl::move(pred),
                           eastl::move(proj1),
                           eastl::move(proj2));
        }

    RANGES_FUNC_END(mismatch)

    // [*] In this case, the 'begin2' iterator is taken by universal reference. Why? So
    // that we can properly distinguish this case:
    //   int x[] = {1,2,3,4};
    //   int y[] = {1,2,3,4};
    //   mismatch(x, y);
    // Had 'begin2' been taken by value as is customary, this call could match as either
    // two ranges, or a range and an iterator, where the iterator is the array, decayed
    // to a pointer. Yuk!

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif