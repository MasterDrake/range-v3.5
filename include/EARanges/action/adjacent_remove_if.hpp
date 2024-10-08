/// \file
// Range v3 library
//
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef EARANGES_ACTION_ADJACENT_REMOVE_IF_HPP
#define EARANGES_ACTION_ADJACENT_REMOVE_IF_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/algorithm/adjacent_remove_if.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct adjacent_remove_if_fn
        {
            template(typename Pred, typename Proj = identity)(requires (!range<Pred>))
            constexpr auto operator()(Pred pred, Proj proj = {}) const
            {
                return make_action_closure(bind_back(adjacent_remove_if_fn{}, eastl::move(pred), eastl::move(proj)));
            }

            template(typename Rng, typename Pred, typename Proj = identity)(
                requires forward_range<Rng> AND
                    erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>> AND
                    indirect_relation<Pred, projected<iterator_t<Rng>, Proj>> AND
                    permutable<iterator_t<Rng>>)
            Rng operator()(Rng && rng, Pred pred, Proj proj = {}) const
            {
                auto i = adjacent_remove_if(rng, eastl::move(pred), eastl::move(proj));
                erase(rng, eastl::move(i), end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::adjacent_remove_if_fn
        EARANGES_INLINE_VARIABLE(adjacent_remove_if_fn, adjacent_remove_if)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif // EARANGES_ACTION_ADJACENT_REMOVE_IF_HPP