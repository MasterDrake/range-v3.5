/// \file
// Range v3 library
//
// Copyright Eric Niebler 2004
// Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
// Implementation based on the code in libc++
//   http://http://libcxx.llvm.org/

#ifndef EARANGES_NUMERIC_ADJACENT_DIFFERENCE_HPP
#define EARANGES_NUMERIC_ADJACENT_DIFFERENCE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/arithmetic.hpp>
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
    /// \addtogroup group-numerics
    /// @{
    // clang-format off
    /// \concept differenceable_
    /// \brief The \c differenceable_ concept
    template(typename I, typename O, typename BOp, typename P)(
    concept (differenceable_)(I, O, BOp, P),
        invocable<P&, iter_value_t<I>> AND
        copy_constructible<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> AND
        movable<uncvref_t<invoke_result_t<P&, iter_value_t<I>>>> AND
        output_iterator<O, invoke_result_t<P&, iter_value_t<I>>> AND
        invocable<
            BOp&,
            invoke_result_t<
                P&,
                iter_value_t<I>>,
                invoke_result_t<P&, iter_value_t<I>>> AND
        output_iterator<
            O,
            invoke_result_t<
                BOp&,
                invoke_result_t<P&, iter_value_t<I>>,
                invoke_result_t<P&, iter_value_t<I>>>>);

    /// \concept differenceable
    /// \brief The \c differenceable concept
    template<typename I, typename O, typename BOp = minus, typename P = identity>
    CPP_concept differenceable =
        input_iterator<I> &&
        CPP_concept_ref(ranges::differenceable_, I, O, BOp, P);
    // clang-format on

    template<typename I, typename O>
    using adjacent_difference_result = detail::in_out_result<I, O>;

    struct adjacent_difference_fn
    {
        template(typename I, typename S, typename O, typename S2, typename BOp = minus,
                 typename P = identity)(
            requires sentinel_for<S, I> AND sentinel_for<S2, O> AND
                differenceable<I, O, BOp, P>)
        adjacent_difference_result<I, O> operator()(I first,
                                                    S last,
                                                    O result,
                                                    S2 end_result,
                                                    BOp bop = BOp{},
                                                    P proj = P{}) const
        {
            // BUGBUG think about the use of coerce here.
            using V = iter_value_t<I>;
            using X = invoke_result_t<P &, V>;
            coerce<V> v;
            coerce<X> x;

            if(first != last && result != end_result)
            {
                auto t1(x(invoke(proj, v(*first))));
                *result = t1;
                for(++first, ++result; first != last && result != end_result;
                    ++first, ++result)
                {
                    auto t2(x(invoke(proj, v(*first))));
                    *result = invoke(bop, t2, t1);
                    t1 = eastl::move(t2);
                }
            }
            return {first, result};
        }

        template(typename I, typename S, typename O, typename BOp = minus,
                 typename P = identity)(
            requires sentinel_for<S, I> AND differenceable<I, O, BOp, P>)
        adjacent_difference_result<I, O> //
        operator()(I first, S last, O result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(eastl::move(first),
                           eastl::move(last),
                           eastl::move(result),
                           unreachable,
                           eastl::move(bop),
                           eastl::move(proj));
        }

        template(typename Rng, typename ORef, typename BOp = minus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = uncvref_t<ORef>)(
            requires range<Rng> AND differenceable<I, O, BOp, P>)
        adjacent_difference_result<borrowed_iterator_t<Rng>, O> //
        operator()(Rng && rng, ORef && result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(begin(rng),
                           end(rng),
                           static_cast<ORef &&>(result),
                           eastl::move(bop),
                           eastl::move(proj));
        }

        template(typename Rng, typename ORng, typename BOp = minus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = iterator_t<ORng>)(
            requires range<Rng> AND range<ORng> AND differenceable<I, O, BOp, P>)
        adjacent_difference_result<borrowed_iterator_t<Rng>, borrowed_iterator_t<ORng>>
        operator()(Rng && rng, ORng && result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(begin(rng),
                           end(rng),
                           begin(result),
                           end(result),
                           eastl::move(bop),
                           eastl::move(proj));
        }
    };

    EARANGES_INLINE_VARIABLE(adjacent_difference_fn, adjacent_difference)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif