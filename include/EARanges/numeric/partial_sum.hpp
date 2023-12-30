/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_NUMERIC_PARTIAL_SUM_HPP
#define EARANGES_NUMERIC_PARTIAL_SUM_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/arithmetic.hpp>
#include <EARanges/functional/compose.hpp>
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

    /// \cond
    namespace detail
    {
        // Only needed for type-checking purposes:
        struct as_lvalue_fn
        {
            template<typename T>
            constexpr T & operator()(T && t) const noexcept
            {
                return t;
            }
        };
        template<typename I>
        using as_value_type_t = composed<as_lvalue_fn, coerce<iter_value_t<I>>>;
    } // namespace detail
      /// \endcond

    // axiom: BOp is associative over values of I.
    // clang-format off
    /// \concept indirect_semigroup_
    /// \brief The \c indirect_semigroup_ concept
    template(typename I, typename BOp)(
    concept (indirect_semigroup_)(I, BOp),
        copyable<iter_value_t<I>> AND
        indirectly_regular_binary_invocable_<
            composed<coerce<iter_value_t<I>>, BOp>,
            iter_value_t<I>*, I>
    );
    /// \concept indirect_semigroup
    /// \brief The \c indirect_semigroup concept
    template<typename I, typename BOp>
    CPP_concept indirect_semigroup =
        indirectly_readable<I> &&
        CPP_concept_ref(ranges::indirect_semigroup_, I, BOp);

    /// \concept partial_sum_constraints_
    /// \brief The \c partial_sum_constraints_ concept
    template(typename I, typename O, typename BOp, typename P)(
    concept (partial_sum_constraints_)(I, O, BOp, P),
        indirect_semigroup<
            projected<projected<I, detail::as_value_type_t<I>>, P>,
            BOp> AND
        output_iterator<
            O,
            iter_value_t<
                projected<projected<I, detail::as_value_type_t<I>>, P>> const &>
    );
    /// \concept partial_sum_constraints
    /// \brief The \c partial_sum_constraints concept
    template<typename I, typename O, typename BOp = plus, typename P = identity>
    CPP_concept partial_sum_constraints =
        input_iterator<I> &&
        CPP_concept_ref(ranges::partial_sum_constraints_, I, O, BOp, P);
    // clang-format on

    template<typename I, typename O>
    using partial_sum_result = detail::in_out_result<I, O>;

    struct partial_sum_fn
    {
        template(typename I, typename S1, typename O, typename S2, typename BOp = plus,
                 typename P = identity)(
            requires sentinel_for<S1, I> AND sentinel_for<S2, O> AND
                partial_sum_constraints<I, O, BOp, P>)
        partial_sum_result<I, O> operator()(I first,
                                            S1 last,
                                            O result,
                                            S2 end_result,
                                            BOp bop = BOp{},
                                            P proj = P{}) const
        {
            using X = projected<projected<I, detail::as_value_type_t<I>>, P>;
            coerce<iter_value_t<I>> val_i;
            coerce<iter_value_t<X>> val_x;
            if(first != last && result != end_result)
            {
                auto && cur1 = val_i(*first);
                iter_value_t<X> t(invoke(proj, cur1));
                *result = t;
                for(++first, ++result; first != last && result != end_result;
                    ++first, ++result)
                {
                    auto && cur2 = val_i(*first);
                    t = val_x(invoke(bop, t, invoke(proj, cur2)));
                    *result = t;
                }
            }
            return {first, result};
        }

        template(typename I, typename S, typename O, typename BOp = plus,
                 typename P = identity)(
            requires sentinel_for<S, I> AND partial_sum_constraints<I, O, BOp, P>)
        partial_sum_result<I, O> //
        operator()(I first, S last, O result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(eastl::move(first),
                           eastl::move(last),
                           eastl::move(result),
                           unreachable,
                           eastl::move(bop),
                           eastl::move(proj));
        }

        template(typename Rng, typename ORef, typename BOp = plus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = uncvref_t<ORef>)(
            requires range<Rng> AND partial_sum_constraints<I, O, BOp, P>)
        partial_sum_result<borrowed_iterator_t<Rng>, O> //
        operator()(Rng && rng, ORef && result, BOp bop = BOp{}, P proj = P{}) const
        {
            return (*this)(begin(rng),
                           end(rng),
                           static_cast<ORef &&>(result),
                           eastl::move(bop),
                           eastl::move(proj));
        }

        template(typename Rng, typename ORng, typename BOp = plus, typename P = identity,
                 typename I = iterator_t<Rng>, typename O = iterator_t<ORng>)(
            requires range<Rng> AND range<ORng> AND partial_sum_constraints<I, O, BOp, P>)
        partial_sum_result<borrowed_iterator_t<Rng>, borrowed_iterator_t<ORng>> //
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

    EARANGES_INLINE_VARIABLE(partial_sum_fn, partial_sum)
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif