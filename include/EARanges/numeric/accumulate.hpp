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
#ifndef EARANGES_NUMERIC_ACCUMULATE_HPP
#define EARANGES_NUMERIC_ACCUMULATE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/functional/arithmetic.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-numerics
        /// @{
        struct accumulate_fn
        {
            template(typename I, typename S, typename T, typename Op = plus,
                     typename P = identity)(
                requires sentinel_for<S, I> AND input_iterator<I> AND
                    indirectly_binary_invocable_<Op, T *, projected<I, P>>
                        AND assignable_from<
                            T &, indirect_result_t<Op &, T *, projected<I, P>>>) T
            operator()(I first, S last, T init, Op op = Op{}, P proj = P{}) const
            {
                for(; first != last; ++first)
                    init = invoke(op, init, invoke(proj, *first));
                return init;
            }

            template(typename Rng, typename T, typename Op = plus, typename P = identity)(
                requires input_range<Rng> AND indirectly_binary_invocable_<
                    Op, T *, projected<iterator_t<Rng>, P>>
                    AND assignable_from<
                        T &, indirect_result_t<Op &, T *, projected<iterator_t<Rng>, P>>>)
                T
                operator()(Rng && rng, T init, Op op = Op{}, P proj = P{}) const
            {
                return (*this)(begin(rng),
                               end(rng),
                               eastl::move(init),
                               eastl::move(op),
                               eastl::move(proj));
            }
        };

        EARANGES_INLINE_VARIABLE(accumulate_fn, accumulate)
        /// @}
    } // namespace ranges
} // namespace eastl
#include <EARanges/detail/epilogue.hpp>

#endif