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
#ifndef EARANGES_ALGORITHM_SWAP_RANGES_HPP
#define EARANGES_ALGORITHM_SWAP_RANGES_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-algorithms
        /// @{
        template<typename I1, typename I2>
        using swap_ranges_result = detail::in1_in2_result<I1, I2>;

        EARANGES_FUNC_BEGIN(swap_ranges)

            /// \brief function template \c swap_ranges
            template(typename I1, typename S1, typename I2)(
                requires input_iterator<I1> AND sentinel_for<S1, I1> AND input_iterator<
                    I2>
                    AND indirectly_swappable<I1, I2>) constexpr swap_ranges_result<I1,
                                                                                   I2> //
            EARANGES_FUNC(swap_ranges)(I1 begin1, S1 end1, I2 begin2)                  //
            {
                for(; begin1 != end1; ++begin1, ++begin2)
                    ranges::iter_swap(begin1, begin2);
                return {begin1, begin2};
            }

            /// \overload
            template(typename I1, typename S1, typename I2, typename S2)(
                requires input_iterator<I1> AND sentinel_for<S1, I1> AND input_iterator<
                    I2>
                    AND sentinel_for<S2, I2>
                        AND indirectly_swappable<I1, I2>) constexpr swap_ranges_result<I1,
                                                                                       I2>
            EARANGES_FUNC(swap_ranges)(I1 begin1,
                                       S1 end1,
                                       I2 begin2,
                                       S2 end2) //
            {
                for(; begin1 != end1 && begin2 != end2; ++begin1, ++begin2)
                    ranges::iter_swap(begin1, begin2);
                return {begin1, begin2};
            }

            template(typename Rng1, typename I2_)(
                requires input_range<Rng1> AND input_iterator<uncvref_t<I2_>> AND
                    indirectly_swappable<
                        iterator_t<Rng1>,
                        uncvref_t<I2_>>) constexpr swap_ranges_result<iterator_t<Rng1>,
                                                                      uncvref_t<I2_>> //
            EARANGES_FUNC(swap_ranges)(Rng1 && rng1, I2_ && begin2)                   //
            {
                return (*this)(begin(rng1), end(rng1), (I2_ &&)begin2);
            }

            template(typename Rng1, typename Rng2)(
                requires input_range<Rng1> AND input_range<Rng2> AND indirectly_swappable<
                    iterator_t<Rng1>,
                    iterator_t<
                        Rng2>>) constexpr swap_ranges_result<borrowed_iterator_t<Rng1>,
                                                             borrowed_iterator_t<Rng2>> //
            EARANGES_FUNC(swap_ranges)(Rng1 && rng1, Rng2 && rng2)                      //
            {
                return (*this)(begin(rng1), end(rng1), begin(rng2), end(rng2));
            }

        EARANGES_FUNC_END(swap_ranges)
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif