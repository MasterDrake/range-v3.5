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

#ifndef RANGES_V3_ACTION_TRANSFORM_HPP
#define RANGES_V3_ACTION_TRANSFORM_HPP

#include "../range_fwd.hpp"

#include "../action/action.hpp"
#include "../algorithm/transform.hpp"
#include "../functional/bind_back.hpp"
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
        struct transform_fn
        {
            template(typename F, typename P = identity)(requires (!range<F>))
            constexpr auto operator()(F fun, P proj = P{}) const
            {
                return make_action_closure(bind_back(transform_fn{}, eastl::move(fun), eastl::move(proj)));
            }

            template(typename Rng, typename F, typename P = identity)(requires input_range<Rng> AND copy_constructible<F> AND indirectly_writable<iterator_t<Rng>, indirect_result_t<F &, projected<iterator_t<Rng>, P>>>)
            Rng operator()(Rng && rng, F fun, P proj = P{}) const
            {
                ranges::transform(rng, begin(rng), eastl::move(fun), eastl::move(proj));
                return static_cast<Rng &&>(rng);
            }
        };

        /// \relates actions::transform_fn
        RANGES_INLINE_VARIABLE(transform_fn, transform)
    } // namespace actions
    /// @}
} // namespace ranges

#include "../detail/epilogue.hpp"

#endif