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

#ifndef EARANGES_ACTION_TRANSFORM_HPP
#define EARANGES_ACTION_TRANSFORM_HPP

#include <EARanges/range_fwd.hpp>

#include <EARanges/action/action.hpp>
#include <EARanges/algorithm/transform.hpp>
#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/identity.hpp>
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
            struct transform_fn
            {
                template(typename F,
                         typename P = identity)(requires(!range<F>)) constexpr auto
                operator()(F fun, P proj = P{}) const
                {
                    return make_action_closure(
                        bind_back(transform_fn{}, eastl::move(fun), eastl::move(proj)));
                }

                template(typename Rng, typename F, typename P = identity)(
                    requires input_range<Rng> AND copy_constructible<F> AND
                        indirectly_writable<
                            iterator_t<Rng>,
                            indirect_result_t<F &, projected<iterator_t<Rng>, P>>>) Rng
                operator()(Rng && rng, F fun, P proj = P{}) const
                {
                    ranges::transform(
                        rng, begin(rng), eastl::move(fun), eastl::move(proj));
                    return static_cast<Rng &&>(rng);
                }
            };

            /// \relates actions::transform_fn
            EARANGES_INLINE_VARIABLE(transform_fn, transform)
        } // namespace actions
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif