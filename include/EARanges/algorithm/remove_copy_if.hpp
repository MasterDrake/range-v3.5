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
#ifndef EARANGES_ALGORITHM_REMOVE_COPY_IF_HPP
#define EARANGES_ALGORITHM_REMOVE_COPY_IF_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/algorithm/result_types.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/functional/invoke.hpp>
#include <EARanges/iterator/concepts.hpp>
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
    template<typename I, typename O>
    using remove_copy_if_result = detail::in_out_result<I, O>;

    EARANGES_FUNC_BEGIN(remove_copy_if)

        /// \brief function template \c remove_copy_if
        template(typename I, typename S, typename O, typename C, typename P = identity)(
            requires input_iterator<I> AND sentinel_for<S, I> AND
            weakly_incrementable<O> AND indirect_unary_predicate<C, projected<I, P>> AND
            indirectly_copyable<I, O>)
        constexpr remove_copy_if_result<I, O> //
        EARANGES_FUNC(remove_copy_if)(I first, S last, O out, C pred, P proj = P{}) //
        {
            return eastl::remove_copy_if(first, last, out, eastl::move(pred), eastl::move(proj));
        }

        /// \overload
        template(typename Rng, typename O, typename C, typename P = identity)(
            requires input_range<Rng> AND weakly_incrementable<O> AND
            indirect_unary_predicate<C, projected<iterator_t<Rng>, P>> AND
            indirectly_copyable<iterator_t<Rng>, O>)
        constexpr remove_copy_if_result<borrowed_iterator_t<Rng>, O> //
        EARANGES_FUNC(remove_copy_if)(Rng && rng, O out, C pred, P proj = P{}) //
        {
            return (*this)(begin(rng), end(rng), eastl::move(out), eastl::move(pred), eastl::move(proj));
        }

    EARANGES_FUNC_END(remove_copy_if)

    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif