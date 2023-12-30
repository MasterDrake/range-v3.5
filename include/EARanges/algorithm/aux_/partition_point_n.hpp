/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ALGORITHM_AUX_PARTITION_POINT_N_HPP
#define EARANGES_ALGORITHM_AUX_PARTITION_POINT_N_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace aux
    {
        struct partition_point_n_fn
        {
            template(typename I, typename C, typename P = identity)(requires forward_iterator<I> AND indirect_unary_predicate<C, projected<I, P>>)
            constexpr I operator()(I first,
                                   iter_difference_t<I> d,
                                   C pred,
                                   P proj = P{}) const //
            {
                if(0 < d)
                {
                    do
                    {
                        auto half = d / 2;
                        auto middle = next(uncounted(first), half);
                        if(invoke(pred, invoke(proj, *middle)))
                        {
                            first = recounted(first, std::move(++middle), half + 1);
                            d -= half + 1;
                        }
                        else
                            d = half;
                    } while(0 != d);
                }
                return first;
            }
        };

        EARANGES_INLINE_VARIABLE(partition_point_n_fn, partition_point_n)
    } // namespace aux
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif