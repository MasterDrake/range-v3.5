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

#ifndef EARANGES_VIEW_UNIQUE_HPP
#define EARANGES_VIEW_UNIQUE_HPP

#include <EASTL/utility.h>

#include <EARanges/meta/meta.hpp>

#include <EARanges/range_fwd.hpp>

#include <EARanges/functional/bind_back.hpp>
#include <EARanges/functional/not_fn.hpp>
#include <EARanges/utility/static_const.hpp>
#include <EARanges/view/adjacent_filter.hpp>
#include <EARanges/view/all.hpp>
#include <EARanges/view/view.hpp>

#include <EARanges/detail/prologue.hpp>

namespace eastl
{
    namespace ranges
    {
        /// \addtogroup group-views
        /// @{
        namespace views
        {
            struct unique_base_fn
            {
                template(typename Rng, typename C = equal_to)(
                    requires viewable_range<Rng> AND forward_range<Rng> AND
                        indirect_relation<
                            C, iterator_t<
                                   Rng>>) constexpr adjacent_filter_view<all_t<Rng>,
                                                                         logical_negate<
                                                                             C>> //
                operator()(Rng && rng, C pred = {}) const
                {
                    return {all(static_cast<Rng &&>(rng)), not_fn(pred)};
                }
            };

            struct unique_fn : unique_base_fn
            {
                using unique_base_fn::operator();

                template(typename C)(requires(!range<C>)) constexpr auto operator()(
                    C && pred) const
                {
                    return make_view_closure(
                        bind_back(unique_base_fn{}, static_cast<C &&>(pred)));
                }
            };

            /// \relates unique_fn
            /// \ingroup group-views
            EARANGES_INLINE_VARIABLE(view_closure<unique_fn>, unique)
        } // namespace views
        /// @}
    } // namespace ranges
} // namespace eastl

#include <EARanges/detail/epilogue.hpp>

#endif