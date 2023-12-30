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

#ifndef EARANGES_ACTION_SORT_HPP
#define EARANGES_ACTION_SORT_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/algorithm/sort.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/comparisons.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct sort_fn
        {
            template(typename C, typename P = identity)(requires (!range<C>))
            constexpr auto operator()(C pred, P proj = {}) const
            {
                return make_action_closure(bind_back(sort_fn{}, eastl::move(pred), eastl::move(proj)));
            }

            template(typename Rng, typename C = less, typename P = identity)(requires forward_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
            Rng operator()(Rng && rng, C pred = {}, P proj = {}) const
            {
                ranges::sort(rng, eastl::move(pred), eastl::move(proj));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::sort_fn
        /// \sa action_closure
        EARANGES_INLINE_VARIABLE(action_closure<sort_fn>, sort)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif