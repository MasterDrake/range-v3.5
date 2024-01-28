/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Gonzalo Brito Gadeschi 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef EARANGES_ACTION_REVERSE_HPP
#define EARANGES_ACTION_REVERSE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/algorithm/reverse.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-actions
        /// @{
        namespace actions
        {
            /// Reversed the source range in-place.
            struct reverse_fn
            {
                template(typename Rng)(requires bidirectional_range<Rng> AND permutable<iterator_t<Rng>>) Rng
                operator()(Rng && rng) const
                {
                    ranges::reverse(rng);
                    return static_cast<Rng &&>(rng);
                }
            };

            /// \relates actions::reverse_fn
            /// \sa action_closure
            EARANGES_INLINE_VARIABLE(action_closure<reverse_fn>, reverse)
        } // namespace actions
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif