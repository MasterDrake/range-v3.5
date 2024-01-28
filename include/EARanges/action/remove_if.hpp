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

#ifndef EARANGES_ACTION_REMOVE_IF_HPP
#define EARANGES_ACTION_REMOVE_IF_HPP

#include <EASTL/utility.h>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/algorithm/remove_if.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/identity.hpp>
#include <EARanges/range/traits.hpp>
#include <EARanges/utility/static_const.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        // TODO Look at all the special cases handled by erase_if in Library Fundamentals
        // 2 - Eric Niebler

        /// \addtogroup group-actions
        /// @{
        namespace actions
        {
            struct remove_if_fn
            {
                template(typename C,
                         typename P = identity)(requires(!range<C>)) constexpr auto
                operator()(C pred, P proj = P{}) const
                {
                    return make_action_closure(
                        bind_back(remove_if_fn{}, eastl::move(pred), eastl::move(proj)));
                }

                template(typename Rng, typename C, typename P = identity)(
                    requires forward_range<Rng> AND
                        erasable_range<Rng &, iterator_t<Rng>, iterator_t<Rng>>
                            AND permutable<iterator_t<Rng>>
                                AND indirect_unary_predicate<
                                    C, projected<iterator_t<Rng>, P>>) Rng
                operator()(Rng && rng, C pred, P proj = P{}) const
                {
                    auto it =
                        ranges::remove_if(rng, eastl::move(pred), eastl::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                    return static_cast<Rng &&>(rng);
                }
            };

            /// \relates actions::remove_if_fn
            EARANGES_INLINE_VARIABLE(remove_if_fn, remove_if)
        } // namespace actions
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif
