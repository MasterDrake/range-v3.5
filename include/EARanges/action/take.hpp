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

#ifndef EARANGES_ACTION_TAKE_HPP
#define EARANGES_ACTION_TAKE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/operations.hpp>
#include <EARanges/iterator/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace ranges
{
    /// \addtogroup group-actions
    /// @{
    namespace actions
    {
        struct take_fn
        {
            template(typename Int)(requires detail::integer_like_<Int>)
            constexpr auto operator()(Int n) const
            {
                return make_action_closure(bind_back(take_fn{}, n));
            }

            template(typename Rng)( requires forward_range<Rng> AND erasable_range<Rng &, iterator_t<Rng>, sentinel_t<Rng>>)
            Rng operator()(Rng && rng, range_difference_t<Rng> n) const
            {
                EARANGES_EXPECT(n >= 0);
                ranges::actions::erase(rng, ranges::next(begin(rng), n, end(rng)), end(rng));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::take_fn
        EARANGES_INLINE_VARIABLE(take_fn, take)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif