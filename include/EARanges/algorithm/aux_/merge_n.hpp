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
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
#ifndef EARANGES_ALGORITHM_AUX_MERGE_N_HPP
#define EARANGES_ALGORITHM_AUX_MERGE_N_HPP

#include <EASTL/tuple.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/copy_n.hpp>
#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace aux
    {
        template<typename I0, typename I1, typename O>
        using merge_n_result = detail::in1_in2_out_result<I0, I1, O>;

        struct merge_n_fn
        {
            template(typename I0, typename I1, typename O, typename C = less, typename P0 = identity, typename P1 = identity)(requires mergeable<I0, I1, O, C, P0, P1>)
            merge_n_result<I0, I1, O> operator()(I0 begin0,
                                                 iter_difference_t<I0> n0,
                                                 I1 begin1,
                                                 iter_difference_t<I1> n1,
                                                 O out,
                                                 C r = C{},
                                                 P0 p0 = P0{},
                                                 P1 p1 = P1{}) const
            {
                using T = merge_n_result<I0, I1, O>;
                auto n0orig = n0;
                auto n1orig = n1;
                auto b0 = uncounted(begin0);
                auto b1 = uncounted(begin1);
                while(true)
                {
                    if(0 == n0)
                    {
                        auto res = copy_n(b1, n1, out);
                        begin0 = recounted(begin0, b0, n0orig);
                        begin1 = recounted(begin1, res.in, n1orig);
                        return T{begin0, begin1, res.out};
                    }
                    if(0 == n1)
                    {
                        auto res = copy_n(b0, n0, out);
                        begin0 = recounted(begin0, res.in, n0orig);
                        begin1 = recounted(begin1, b1, n1orig);
                        return T{begin0, begin1, res.out};
                    }
                    if(invoke(r, invoke(p1, *b1), invoke(p0, *b0)))
                    {
                        *out = *b1;
                        ++b1;
                        ++out;
                        --n1;
                    }
                    else
                    {
                        *out = *b0;
                        ++b0;
                        ++out;
                        --n0;
                    }
                }
            }
        };

        EARANGES_INLINE_VARIABLE(merge_n_fn, merge_n)
    } // namespace aux
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif