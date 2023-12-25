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
#ifndef RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP
#define RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP

#include "EASTL/utility.h"

#include "../concepts/concepts.hpp"

#include "../range_fwd.hpp"

#include "../action/action.hpp"
#include "../action/erase.hpp"
#include "../algorithm/unstable_remove_if.hpp"
#include "../functional/bind_back.hpp"
#include "../functional/identity.hpp"
#include "../iterator/concepts.hpp"
#include "../range/access.hpp"
#include "../range/concepts.hpp"

#include "../detail/prologue.hpp"

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
        RANGES_INLINE_VARIABLE(unstable_remove_if_fn, unstable_remove_if)
    } // namespace actions
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif // RANGES_V3_ACTION_UNSTABLE_REMOVE_IF_HPP