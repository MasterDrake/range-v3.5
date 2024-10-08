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

#ifndef EARANGES_ALGORITHM_MERGE_HPP
#define EARANGES_ALGORITHM_MERGE_HPP

#include "EASTL/tuple.h"

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/copy.hpp>
#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/dangling.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-algorithms
    /// @{
    template<typename I0, typename I1, typename O>
    using merge_result = detail::in1_in2_out_result<I0, I1, O>;

    EARANGES_FUNC_BEGIN(merge)

        /// \brief function template \c merge
        template(typename I0,
                 typename S0,
                 typename I1,
                 typename S1,
                 typename O,
                 typename C = less,
                 typename P0 = identity,
                 typename P1 = identity)(requires sentinel_for<S0, I0> AND sentinel_for<S1, I1> AND mergeable<I0, I1, O, C, P0, P1>)
        constexpr merge_result<I0, I1, O> EARANGES_FUNC(merge)(I0 begin0,
                                                             S0 end0,
                                                             I1 begin1,
                                                             S1 end1,
                                                             O out,
                                                             C pred = C{},
                                                             P0 proj0 = P0{},
                                                             P1 proj1 = P1{}) //
        {
            for(; begin0 != end0 && begin1 != end1; ++out)
            {
                if(invoke(pred, invoke(proj1, *begin1), invoke(proj0, *begin0)))
                {
                    *out = *begin1;
                    ++begin1;
                }
                else
                {
                    *out = *begin0;
                    ++begin0;
                }
            }
            auto t0 = ranges::copy(begin0, end0, out);
            auto t1 = ranges::copy(begin1, end1, t0.out);
            return {t0.in, t1.in, t1.out};
        }

        /// \overload
        template(typename Rng0,
                 typename Rng1,
                 typename O,
                 typename C = less,
                 typename P0 = identity,
                 typename P1 = identity)(requires range<Rng0> AND range<Rng1> AND mergeable<iterator_t<Rng0>, iterator_t<Rng1>, O, C, P0, P1>)
        constexpr merge_result<borrowed_iterator_t<Rng0>, borrowed_iterator_t<Rng1>, O>
        EARANGES_FUNC(merge)(Rng0 && rng0,
                           Rng1 && rng1,
                           O out,
                           C pred = C{},
                           P0 proj0 = P0{},
                           P1 proj1 = P1{})
        {
            return (*this)(begin(rng0),
                           end(rng0),
                           begin(rng1),
                           end(rng1),
                           eastl::move(out),
                           eastl::move(pred),
                           eastl::move(proj0),
                           eastl::move(proj1));
        }

    EARANGES_FUNC_END(merge)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif