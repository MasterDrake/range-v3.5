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
#ifndef RANGES_V3_ALGORITHM_ANY_OF_HPP
#define RANGES_V3_ALGORITHM_ANY_OF_HPP

//#include <EASTL/utility.h>
#include "EASTL/utility.h"

#include "../range_fwd.hpp"

#include "../functional/identity.hpp"
#include "../functional/invoke.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"
#include "../range/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    RANGES_FUNC_BEGIN(any_of)

        /// \brief function template \c any_of
        template(typename I, typename S, typename F, typename P = identity)(requires input_iterator<I> AND sentinel_for<S, I> AND indirect_unary_predicate<F, projected<I, P>>)
        constexpr bool RANGES_FUNC(any_of)(I first, S last, F pred, P proj = P{}) //
        {
            for(; first != last; ++first)
                if(invoke(pred, invoke(proj, *first)))
                    return true;
            return false;
        }

        /// \overload
        template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND indirect_unary_predicate<F, projected<iterator_t<Rng>, P>>)
        constexpr bool RANGES_FUNC(any_of)(Rng && rng, F pred, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(pred), eastl::move(proj));
        }

    RANGES_FUNC_END(any_of)

    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif