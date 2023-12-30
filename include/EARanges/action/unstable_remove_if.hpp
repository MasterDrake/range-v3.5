/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ACTION_UNSTABLE_REMOVE_IF_HPP
#define EARANGES_ACTION_UNSTABLE_REMOVE_IF_HPP

#include <EASTL/utility.h>

#include <EARanges/concepts/concepts.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/algorithm/unstable_remove_if.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/access.hpp>
#include <EARanges/range/concepts.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct unstable_remove_if_fn
        {
            template(typename C, typename P = identity)(requires (!range<C>))
            constexpr auto operator()(C pred, P proj = P{}) const
            {
                return make_action_closure(bind_back(unstable_remove_if_fn{}, eastl::move(pred), eastl::move(proj)));
            }

            template(typename Rng, typename C, typename P = identity)(
                requires bidirectional_range<Rng> AND common_range<Rng> AND
                    permutable<iterator_t<Rng>> AND
                    indirect_unary_predicate<C, projected<iterator_t<Rng>, P>> AND
                    erasable_range<Rng, iterator_t<Rng>, iterator_t<Rng>>)
            Rng operator()(Rng && rng, C pred, P proj = P{}) const
            {
                auto it = ranges::unstable_remove_if(ranges::begin(rng),
                                                     ranges::end(rng),
                                                     eastl::move(pred),
                                                     eastl::move(proj));
                ranges::erase(rng, it, ranges::end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \sa `actions::unstable_remove_if_fn`
        EARANGES_INLINE_VARIABLE(unstable_remove_if_fn, unstable_remove_if)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ACTION_UNSTABLE_REMOVE_IF_HPP