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
#ifndef EARANGES_ALGORITHM_TRANSFORM_HPP
#define EARANGES_ALGORITHM_TRANSFORM_HPP

#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/iterator/unreachable_sentinel.hpp>
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
    template<typename I, typename O>
    using unary_transform_result = detail::in_out_result<I, O>;

    template<typename I1, typename I2, typename O>
    using binary_transform_result = detail::in1_in2_out_result<I1, I2, O>;

    EARANGES_FUNC_BEGIN(transform)

        // Single-range variant
        /// \brief function template \c transform
        template(typename I, typename S, typename O, typename F, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
            weakly_incrementable<O> AND copy_constructible<F> AND
            indirectly_writable<O, indirect_result_t<F &, projected<I, P>>>)
        constexpr unary_transform_result<I, O> //
        EARANGES_FUNC(transform)(I first, S last, O out, F fun, P proj = P{}) //
        {
            return eastl::transform(first, last, out, eastl::move(fun), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename O, typename F, typename P = identity)(
            requires input_range<Rng> AND weakly_incrementable<O> AND
            copy_constructible<F> AND
            indirectly_writable<O, indirect_result_t<F &, projected<iterator_t<Rng>, P>>>)
        constexpr unary_transform_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(transform)(Rng && rng, O out, F fun, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out), eastl::move(fun), eastl::move(proj));
        }

        // Double-range variant, 4-iterator version
        /// \overload
        template(typename I0,
                 typename S0,
                 typename I1,
                 typename S1,
                 typename O,
                 typename F,
                 typename P0 = identity,
                 typename P1 = identity)(
            requires input_iterator<I0> AND sentinel_for<S0, I0> AND
                input_iterator<I1> AND sentinel_for<S1, I1> AND
                weakly_incrementable<O> AND copy_constructible<F> AND
                indirectly_writable<O, indirect_result_t<F &, projected<I0, P0>, projected<I1, P1>>>)
        constexpr binary_transform_result<I0, I1, O> //
        EARANGES_FUNC(transform)(I0 begin0,
                               S0 end0,
                               I1 begin1,
                               S1 end1,
                               O out,
                               F fun,
                               P0 proj0 = P0{},
                               P1 proj1 = P1{}) //
        {
            return eastl::transform(begin0, end0, begin1, end1, out, eastl::move(fun), eastl::move(proj0), eastl::move(proj1));
        }

        /// \overload
        template(typename Rng0,
                 typename Rng1,
                 typename O,
                 typename F,
                 typename P0 = identity,
                 typename P1 = identity)(
            requires input_range<Rng0> AND input_range<Rng1> AND
                weakly_incrementable<O> AND copy_constructible<F> AND
                indirectly_writable<
                    O,
                    indirect_result_t<F &,
                                      projected<iterator_t<Rng0>, P0>,
                                      projected<iterator_t<Rng1>, P1>>>)
        constexpr binary_transform_result<borrowed_iterator_t<Rng0>,
                                          borrowed_iterator_t<Rng1>,
                                          O> //
        EARANGES_FUNC(transform)(Rng0 && rng0, Rng1 && rng1, O out, F fun, P0 proj0 = P0{}, P1 proj1 = P1{}) //
        {
            return (*this)(begin(rng0),
                           end(rng0),
                           begin(rng1),
                           end(rng1),
                           eastl::move(out),
                           eastl::move(fun),
                           eastl::move(proj0),
                           eastl::move(proj1));
        }

    EARANGES_FUNC_END(transform)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif