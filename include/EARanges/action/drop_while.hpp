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

#ifndef EARANGES_ACTION_DROP_WHILE_HPP
#define EARANGES_ACTION_DROP_WHILE_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/action/erase.hpp>
#include <EARanges/algorithm/find_if_not.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/iterator/concepts.hpp>
#include <EARanges/iterator/traits.hpp>
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
            struct drop_while_fn
            {
                template(typename Fun)(requires(!range<Fun>)) constexpr auto operator()(
                    Fun fun) const
                {
                    return make_action_closure(
                        bind_back(drop_while_fn{}, eastl::move(fun)));
                }

                template(typename Rng, typename Fun)(
                    requires forward_range<Rng> AND
                        indirect_unary_predicate<Fun, iterator_t<Rng>>
                            AND erasable_range<Rng &, iterator_t<Rng>, iterator_t<Rng>>)
                    Rng
                    operator()(Rng && rng, Fun fun) const
                {
                    ranges::actions::erase(
                        rng,
                        begin(rng),
                        find_if_not(begin(rng), end(rng), eastl::move(fun)));
                    return static_cast<Rng &&>(rng);
                }
            };

            /// \relates actions::drop_while_fn
            EARANGES_INLINE_VARIABLE(drop_while_fn, drop_while)
        } // namespace actions
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif