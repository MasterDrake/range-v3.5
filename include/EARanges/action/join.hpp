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

#ifndef EARANGES_ACTION_JOIN_HPP
#define EARANGES_ACTION_JOIN_HPP

#include <EASTL/vector.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/concepts.hpp>
#include <EARanges/action/push_back.hpp>
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
        template<typename Rng>
        using join_action_value_t_ =
            meta::if_c<(bool)ranges::container<range_value_t<Rng>>, //
                       range_value_t<Rng>,                          //
                       eastl::vector<range_value_t<range_value_t<Rng>>>>;

        struct join_fn
        {
            template(typename Rng)(requires input_range<Rng> AND input_range<range_value_t<Rng>> AND semiregular<join_action_value_t_<Rng>>)
            join_action_value_t_<Rng> operator()(Rng && rng) const
            {
                join_action_value_t_<Rng> ret;
                auto last = ranges::end(rng);
                for(auto it = begin(rng); it != last; ++it)
                    push_back(ret, *it);
                return ret;
            }
        };

        /// \relates actions::join_fn
        /// \sa action_closure
        EARANGES_INLINE_VARIABLE(action_closure<join_fn>, join)
    } // namespace actions
    /// @}
} // namespace ranges

#include <EARanges/detail/epilogue.hpp>

#endif