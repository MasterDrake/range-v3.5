// Range v3 library
//
//  Copyright 2019 Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
#ifndef RANGES_V3_ALGORITHM_STARTS_WITH_HPP
#define RANGES_V3_ALGORITHM_STARTS_WITH_HPP

#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../algorithm/equal.hpp"
#include "../algorithm/mismatch.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/operations.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{

    // template<typename I1, typename I2>
    // struct starts_with_result : detail::in1_in2_result<I1, I2>
    // {
    //     bool result;
    // };

    RANGES_FUNC_BEGIN(starts_with)

        /// \brief function template \c starts_with
        template(typename I1,
                 typename S1,
                 typename I2,
                 typename S2,
                 typename Comp = equal_to,
                 typename Proj1 = identity,
                 typename Proj2 = identity)(
            requires input_iterator<I1> AND sentinel_for<S1, I1> AND input_iterator<I2> AND sentinel_for<S2, I2> AND indirectly_comparable<I1, I2, Comp, Proj1, Proj2>)
        constexpr bool RANGES_FUNC(starts_with)(I1 first1,
                                                S1 last1,
                                                I2 first2,
                                                S2 last2,
                                                Comp comp = {},
                                                Proj1 proj1 = {},
                                                Proj2 proj2 = {}) //
        {
            return mismatch(eastl::move(first1),
                            eastl::move(last1),
                            eastl::move(first2),
                            last2,
                            eastl::move(comp),
                            eastl::move(proj1),
                            eastl::move(proj2))
                       .in2 == last2;
        }

        /// \overload
        template(typename R1,
                 typename R2,
                 typename Comp = equal_to,
                 typename Proj1 = identity,
                 typename Proj2 = identity)(requires input_range<R1> AND input_range<R2> AND indirectly_comparable<iterator_t<R1>, iterator_t<R2>, Comp, Proj1, Proj2>)
        constexpr bool RANGES_FUNC(starts_with)(R1 && r1, R2 && r2, Comp comp = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) //
        {
            return (*this)( //
                begin(r1),
                end(r1),
                begin(r2),
                end(r2),
                eastl::move(comp),
                eastl::move(proj1),
                eastl::move(proj2));
        }

    RANGES_FUNC_END(starts_with)
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_ALGORITHM_STARTS_WITH_HPP