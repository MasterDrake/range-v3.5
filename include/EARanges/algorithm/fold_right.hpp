/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2021-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

#ifndef EARANGES_ALGORITHM_FOLD_RIGHT_HPP
#define EARANGES_ALGORITHM_FOLD_RIGHT_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/algorithm/fold_left.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/optional.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    namespace detail
    {
        template<typename F>
        struct flipped
        {
            F f;

            template(class T, class U)(requires invocable<F &, U, T>) //
                invoke_result_t<F &, U, T>
                operator()(T &&, U &&);
        };
    } // namespace detail

    // clang-format off
    /// \concept indirectly_binary_right_foldable
    /// \brief The \c indirectly_binary_right_foldable concept
    template<class F, class T, class I>
    CPP_concept indirectly_binary_right_foldable =
        indirectly_binary_left_foldable<detail::flipped<F>, T, I>;
    // clang-format on

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(fold_right)

        template(typename I, typename S, typename T, typename Op)(
            requires sentinel_for<S, I> AND bidirectional_iterator<I> AND
                indirectly_binary_right_foldable<Op, T, I>) //
            constexpr auto
            EARANGES_FUNC(fold_right)(I first, S last, T init, Op op)
        {
            using U = eastl::decay_t<invoke_result_t<Op &, iter_reference_t<I>, T>>;

            if(first == last)
            {
                return U(eastl::move(init));
            }

            I tail = next(first, last);
            U accum = invoke(op, *--tail, eastl::move(init));
            while(first != tail)
            {
                accum = invoke(op, *--tail, eastl::move(accum));
            }
            return accum;
        }

        template(typename Rng, typename T, typename Op)(
            requires bidirectional_range<Rng> AND
                indirectly_binary_right_foldable<Op, T, iterator_t<Rng>>) //
            constexpr auto
            EARANGES_FUNC(fold_right)(Rng && rng, T init, Op op)
        {
            return (*this)(begin(rng), end(rng), eastl::move(init), eastl::move(op));
        }

    EARANGES_FUNC_END(fold_right)

    EARANGES_FUNC_BEGIN(fold_right_last)

        template(typename I, typename S, typename Op)(
            requires sentinel_for<S, I> AND bidirectional_iterator<I> AND
                indirectly_binary_right_foldable<Op, iter_value_t<I>, I>
                    AND constructible_from<iter_value_t<I>, iter_reference_t<I>>) //
            constexpr auto
            EARANGES_FUNC(fold_right_last)(I first, S last, Op op)
        {
            using U = invoke_result_t<fold_right_fn, I, S, iter_value_t<I>, Op>;
            if(first == last)
            {
                return optional<U>();
            }

            I tail = prev(next(first, eastl::move(last)));
            return optional<U>(
                in_place,
                fold_right_fn{}(
                    eastl::move(first), tail, iter_value_t<I>(*tail), eastl::move(op)));
        }

        template(typename R, typename Op)(
            requires bidirectional_range<R> AND
                indirectly_binary_right_foldable<Op, range_value_t<R>, iterator_t<R>>
                    AND constructible_from<range_value_t<R>, range_reference_t<R>>) //
            constexpr auto
            EARANGES_FUNC(fold_right_last)(R && rng, Op op)
        {
            return (*this)(begin(rng), end(rng), eastl::move(op));
        }

    EARANGES_FUNC_END(fold_right_last)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif