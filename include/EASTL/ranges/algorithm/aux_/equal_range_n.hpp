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
#ifndef RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP
#define RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP

//#include <EASTL/functional.h>
#include "EASTL/functional.h"

#include "../../range_fwd.hpp"

#include "../../algorithm/aux_/lower_bound_n.hpp"
#include "../../algorithm/aux_/upper_bound_n.hpp"
#include "../../functional/comparisons.hpp"
#include "../../functional/identity.hpp"
#include "../../functional/invoke.hpp"
#include "../../functional/reference_wrapper.hpp"
#include "../../iterator/operations.hpp"
#include "../../range/access.hpp"
#include "../../range/concepts.hpp"
#include "../../range/traits.hpp"
#include "../../utility/static_const.hpp"
#include "../../view/subrange.hpp"

#include "../../detail/prologue.hpp"

namespace ranges
{
    namespace aux
    {
        struct equal_range_n_fn
        {
            template(typename I, typename V, typename R = less, typename P = identity)(
                requires forward_iterator<I> AND indirect_strict_weak_order<R, V const *, projected<I, P>>)
            constexpr subrange<I> operator()(I first,
                                             iter_difference_t<I> dist,
                                             V const & val,
                                             R pred = R{},
                                             P proj = P{}) const
            {
                if(0 < dist)
                {
                    do
                    {
                        auto half = dist / 2;
                        auto middle = ranges::next(first, half);
                        auto && v = *middle;
                        auto && pv = invoke(proj, (decltype(v) &&)v);
                        if(invoke(pred, pv, val))
                        {
                            first = std::move(++middle);
                            dist -= half + 1;
                        }
                        else if(invoke(pred, val, pv))
                        {
                            dist = half;
                        }
                        else
                        {
                            return {lower_bound_n(std::move(first),
                                                  half,
                                                  val,
                                                  ranges::ref(pred),
                                                  ranges::ref(proj)),
                                    upper_bound_n(ranges::next(middle),
                                                  dist - (half + 1),
                                                  val,
                                                  ranges::ref(pred),
                                                  ranges::ref(proj))};
                        }
                    } while(0 != dist);
                }
                return {first, first};
            }
        };

        RANGES_INLINE_VARIABLE(equal_range_n_fn, equal_range_n)
    } // namespace aux
} // namespace ranges

#include "../../detail/epilogue.hpp"

#endif