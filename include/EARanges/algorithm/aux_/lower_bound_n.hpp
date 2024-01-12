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
#ifndef EARANGES_ALGORITHM_AUX_LOWER_BOUND_N_HPP
#define EARANGES_ALGORITHM_AUX_LOWER_BOUND_N_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/aux_/partition_point_n.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \cond
    namespace detail
    {
        // [&](auto&& i){ return invoke(pred, i, val); }
        template<typename Pred, typename Val>
        struct lower_bound_predicate
        {
            Pred & pred_;
            Val & val_;

            template<typename T>
            constexpr bool operator()(T && t) const
            {
                return invoke(pred_, static_cast<T &&>(t), val_);
            }
        };

        template<typename Pred, typename Val>
        constexpr lower_bound_predicate<Pred, Val> make_lower_bound_predicate(Pred & pred, Val & val)
        {
            return {pred, val};
        }
    } // namespace detail
    /// \endcond

    namespace aux
    {
        struct lower_bound_n_fn
        {
            template(typename I, typename V, typename C = less, typename P = identity)(requires forward_iterator<I> AND indirect_strict_weak_order<C, V const *, projected<I, P>>)
            constexpr I operator()(I first,
                                   iter_difference_t<I> d,
                                   V const & val,
                                   C pred = C{},
                                   P proj = P{}) const
            {
                return partition_point_n(eastl::move(first),
                                         d,
                                         detail::make_lower_bound_predicate(pred, val),
                                         eastl::move(proj));
            }
        };

        EARANGES_INLINE_VARIABLE(lower_bound_n_fn, lower_bound_n)
    } // namespace aux
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif