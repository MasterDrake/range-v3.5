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

#ifndef RANGES_V3_ACTION_STABLE_SORT_HPP
#define RANGES_V3_ACTION_STABLE_SORT_HPP

#include "../range_fwd.hpp"

#include "../action/action.hpp"
#include "../algorithm/stable_sort.hpp"
#include "../functional/bind_back.hpp"
#include "../functional/comparisons.hpp"
#include "../functional/identity.hpp"
#include "../iterator/concepts.hpp"
#include "../iterator/traits.hpp"
#include "../utility/static_const.hpp"

#include "../detail/prologue.hpp"

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct stable_sort_fn
        {
            template(typename C, typename P = identity)( requires (!range<C>))
            constexpr auto operator()(C pred, P proj = P{}) const
            {
                return make_action_closure(bind_back(stable_sort_fn{}, eastl::move(pred), eastl::move(proj)));
            }

            template(typename Rng, typename C = less, typename P = identity)(requires forward_range<Rng> AND sortable<iterator_t<Rng>, C, P>)
            Rng operator()(Rng && rng, C pred = C{}, P proj = P{}) const
            {
                ranges::stable_sort(rng, eastl::move(pred), eastl::move(proj));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::stable_sort_fn
        /// \sa action_closure
        RANGES_INLINE_VARIABLE(action_closure<stable_sort_fn>, stable_sort)
    } // namespace actions
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif
