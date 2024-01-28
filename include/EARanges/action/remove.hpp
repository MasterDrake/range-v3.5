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
#ifndef EARANGES_ACTION_REMOVE_HPP
#define EARANGES_ACTION_REMOVE_HPP

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/algorithm/remove.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/range/concepts.hpp>
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
            struct remove_fn
            {
                template(typename V, typename P)(requires(!range<V>)) constexpr auto
                operator()(V && value, P proj) const
                {
                    return make_action_closure(bind_back(
                        remove_fn{}, static_cast<V &&>(value), eastl::move(proj)));
                }

                template<typename V>
                constexpr auto operator()(V && value) const
                {
                    return make_action_closure(
                        bind_back(remove_fn{}, static_cast<V &&>(value), identity{}));
                }

                template(typename Rng, typename V, typename P = identity)(
                    requires forward_range<Rng> AND permutable<iterator_t<Rng>> AND
                        erasable_range<Rng, iterator_t<Rng>, sentinel_t<Rng>>
                            AND indirect_relation<equal_to, projected<iterator_t<Rng>, P>,
                                                  V const *>) Rng
                operator()(Rng && rng, V const & value, P proj = {}) const
                {
                    auto it = ranges::remove(rng, value, eastl::move(proj));
                    ranges::erase(rng, it, ranges::end(rng));
                    return static_cast<Rng &&>(rng);
                }
            };

            /// \relates actions::remove_fn
            EARANGES_INLINE_VARIABLE(remove_fn, remove)
        } // namespace actions
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif