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

#ifndef EARANGES_ALGORITHM_FOLD_LEFT_HPP
#define EARANGES_ALGORITHM_FOLD_LEFT_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>
#include <EARanges/utility/optional.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    // clang-format off
    /// \concept indirectly_binary_left_foldable_impl
    /// \brief The \c indirectly_binary_left_foldable_impl concept
    template<class F, class T, class I, class U>
    CPP_concept indirectly_binary_left_foldable_impl =
        movable<T> &&
        movable<U> &&
        convertible_to<T, U> &&
        invocable<F&, U, iter_reference_t<I>> &&
        assignable_from<U&, invoke_result_t<F&, U, iter_reference_t<I>>>;

    /// \concept indirectly_binary_left_foldable
    /// \brief The \c indirectly_binary_left_foldable concept
    template<class F, class T, class I>
    CPP_concept indirectly_binary_left_foldable =
        copy_constructible<F> &&
        indirectly_readable<I> &&
        invocable<F&, T, iter_reference_t<I>> &&
        convertible_to<invoke_result_t<F&, T, iter_reference_t<I>>,
          eastl::decay_t<invoke_result_t<F&, T, iter_reference_t<I>>>> &&
        indirectly_binary_left_foldable_impl<F, T, I, eastl::decay_t<invoke_result_t<F&, T, iter_reference_t<I>>>>;
    // clang-format on

    /// \addtogroup group-algorithms
    /// @{
    EARANGES_FUNC_BEGIN(fold_left)

        template(typename I, typename S, typename T, typename Op)(requires sentinel_for<S, I> AND input_iterator<I> AND indirectly_binary_left_foldable<Op, T, I>) //
            constexpr auto
            EARANGES_FUNC(fold_left)(I first, S last, T init, Op op)
        {
            using U = eastl::decay_t<invoke_result_t<Op &, T, iter_reference_t<I>>>;

            if(first == last)
            {
                return U(eastl::move(init));
            }

            U accum = invoke(op, eastl::move(init), *first);
            for(++first; first != last; ++first)
            {
                accum = invoke(op, eastl::move(accum), *first);
            }
            return accum;
        }

        template(typename Rng, typename T, typename Op)(requires input_range<Rng> AND indirectly_binary_left_foldable<Op, T, iterator_t<Rng>>) //
            constexpr auto
            EARANGES_FUNC(fold_left)(Rng && rng, T init, Op op)
        {
            return (*this)(begin(rng), end(rng), eastl::move(init), eastl::move(op));
        }

    EARANGES_FUNC_END(fold_left)

    EARANGES_FUNC_BEGIN(fold_left_first)

        template(typename I, typename S, typename Op)(requires sentinel_for<S, I> AND input_iterator<I> AND indirectly_binary_left_foldable<Op, iter_value_t<I>, I> AND constructible_from<iter_value_t<I>, iter_reference_t<I>>) //
            constexpr auto
            EARANGES_FUNC(fold_left_first)(I first, S last, Op op)
        {
            using U = invoke_result_t<fold_left_fn, I, S, iter_value_t<I>, Op>;
            if(first == last)
            {
                return optional<U>();
            }

            iter_value_t<I> init = *first;
            ++first;
            return optional<U>(
                in_place,
                fold_left_fn{}(eastl::move(first), eastl::move(last), eastl::move(init), op));
        }

        template(typename R, typename Op)(requires input_range<R> AND indirectly_binary_left_foldable<Op, range_value_t<R>, iterator_t<R>> AND constructible_from<range_value_t<R>, range_reference_t<R>>) //
            constexpr auto
            EARANGES_FUNC(fold_left_first)(R && rng, Op op)
        {
            return (*this)(begin(rng), end(rng), eastl::move(op));
        }

    EARANGES_FUNC_END(fold_left_first)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif